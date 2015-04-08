#include "precompiled_GFXAdapter_OpenGLES2.h"

#ifndef _ITF_OPENGLES2_DRAWPRIM_H_
#include "adapters/GFXAdapter_OpenGLES2/GFXAdapter_OpenGLES2.h"
#endif //_ITF_OPENGLES2_DRAWPRIM_H_

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_DDSFORMAT_H_
#include "engine/display/ddsformat.h"
#endif //_ITF_DDSFORMAT_H_

#ifndef _ITF_RASTERSMANAGER_H_
#include "engine/rasters/RastersManager.h"
#endif //_ITF_RASTERSMANAGER_H_

#ifdef ITF_TEXTURE_MEMORY_TRACKING
#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_
#endif


#undef ITF_GLES2_GENERATEMIPMAPS


namespace ITF
{
	//GFXAdapter_OpenGLES2::SamplerManager GFXAdapter_OpenGLES2::m_SamplerManager;

	enum
	{
		//! Use DXT1 compression.
		kDxt1 = ( 1 << 0 ), 
		//! Use DXT3 compression.
		kDxt3 = ( 1 << 1 ), 
		//! Use DXT5 compression.
		kDxt5 = ( 1 << 2 ), 
		//! Use a very slow but very high quality colour compressor.
		kColourIterativeClusterFit = ( 1 << 8 ),	
		//! Use a slow but high quality colour compressor (the default).
		kColourClusterFit = ( 1 << 3 ),	
		//! Use a fast but low quality colour compressor.
		kColourRangeFit	= ( 1 << 4 ),
		//! Use a perceptual metric for colour error (the default).
		kColourMetricPerceptual = ( 1 << 5 ),
		//! Use a uniform metric for colour error.
		kColourMetricUniform = ( 1 << 6 ),
		//! Weight the colour by alpha during cluster fit (disabled by default).
		kWeightColourByAlpha = ( 1 << 7 )
	};

	static i32 Unpack565( u8 const* _packed, u8* _colour )
	{
		// build the packed value
		i32 value = ( int )_packed[0] | ( ( int )_packed[1] << 8 );
	
		// get the components in the stored range
		u8 red = ( u8 )( ( value >> 11 ) & 0x1f );
		u8 green = ( u8 )( ( value >> 5 ) & 0x3f );
		u8 blue = ( u8 )( value & 0x1f );

		// scale up to 8 bits
#ifdef swapRB
		_colour[0] = ( red << 3 ) | ( red >> 2 );
		_colour[2] = ( blue << 3 ) | ( blue >> 2 );
#else
		_colour[2] = ( red << 3 ) | ( red >> 2 );
		_colour[0] = ( blue << 3 ) | ( blue >> 2 );
#endif
		_colour[1] = ( green << 2 ) | ( green >> 4 );
		_colour[3] = 255;

		// return the value
		return value;
	}

	void DecompressColour( u8* _rgba, void const* _block, bool _isDxt1 )
	{
		// get the block bytes
		u8 const* bytes = reinterpret_cast< u8 const* >( _block );
	
		// unpack the endpoints
		u8 codes[16];
		i32 a = Unpack565( bytes, codes );
		i32 b = Unpack565( bytes + 2, codes + 4 );
	
		// generate the midpoints
		for( i32 i = 0; i < 3; ++i )
		{
			i32 c = codes[i];
			i32 d = codes[4 + i];

			if( _isDxt1 && a <= b )
			{
				codes[8 + i] = ( u8 )( ( c + d )/2 );
				codes[12 + i] = 0;
			}
			else
			{
				codes[8 + i] = ( u8 )( ( 2*c + d )/3 );
				codes[12 + i] = ( u8 )( ( c + 2*d )/3 );
			}
		}
	
		// fill in alpha for the intermediate values
		codes[8 + 3] = 255;
		codes[12 + 3] = ( _isDxt1 && a <= b ) ? 0 : 255;
	
		// unpack the indices
		u8 indices[16];
		for( i32 i = 0; i < 4; ++i )
		{
			u8* ind = indices + 4*i;
			u8 packed = bytes[4 + i];
		
			ind[0] = packed & 0x3;
			ind[1] = ( packed >> 2 ) & 0x3;
			ind[2] = ( packed >> 4 ) & 0x3;
			ind[3] = ( packed >> 6 ) & 0x3;
		}

		// store out the colours
		for( i32 i = 0; i < 16; ++i )
		{
			u8 offset = 4 * indices[i];
			for( i32 j = 0; j < 4; ++j )
				_rgba[4*i + j] = codes[offset + j];
		}
	}

	void DecompressAlphaDxt3( u8* _rgba, void const* _block )
	{
		u8 const* bytes = reinterpret_cast< u8 const* >( _block );
	
		// unpack the alpha values pairwise
		for( i32 i = 0; i < 8; ++i )
		{
			// quantise down to 4 bits
			u8 quant = bytes[i];
		
			// unpack the values
			u8 lo = quant & 0x0f;
			u8 hi = quant & 0xf0;

			// convert back up to bytes
			_rgba[8*i + 3] = lo | ( lo << 4 );
			_rgba[8*i + 7] = hi | ( hi >> 4 );
		}
	}

	void DecompressAlphaDxt5( u8* _rgba, void const* _block )
	{
		// get the two alpha values
		u8 const* bytes = reinterpret_cast< u8 const* >( _block );
		i32 alpha0 = bytes[0];
		i32 alpha1 = bytes[1];
	
		// compare the values to build the codebook
		u8 codes[8];
		codes[0] = ( u8 )alpha0;
		codes[1] = ( u8 )alpha1;
		if( alpha0 <= alpha1 )
		{
			// use 5-alpha codebook
			for( i32 i = 1; i < 5; ++i )
				codes[1 + i] = ( u8 )( ( ( 5 - i )*alpha0 + i*alpha1 )/5 );
			codes[6] = 0;
			codes[7] = 255;
		}
		else
		{
			// use 7-alpha codebook
			for( i32 i = 1; i < 7; ++i )
				codes[1 + i] = ( u8 )( ( ( 7 - i )*alpha0 + i*alpha1 )/7 );
		}
	
		// decode the indices
		u8 indices[16];
		u8 const* src = bytes + 2;
		u8* dest = indices;
		for( i32 i = 0; i < 2; ++i )
		{
			// grab 3 bytes
			i32 value = 0;
			for( i32 j = 0; j < 3; ++j )
			{
				i32 byte = *src++;
				value |= ( byte << 8*j );
			}
		
			// unpack 8 3-bit values from it
			for( i32 j = 0; j < 8; ++j )
			{
				int index = ( value >> 3*j ) & 0x7;
				*dest++ = ( u8 )index;
			}
		}
	
		// write out the indexed codebook values
		for( i32 i = 0; i < 16; ++i )
			_rgba[4*i + 3] = codes[indices[i]];
	}

	static i32 FixFlags( i32 _flags )
	{
		// grab the flag bits
		int method = _flags & ( kDxt1 | kDxt3 | kDxt5 );
		int fit = _flags & ( kColourIterativeClusterFit | kColourClusterFit | kColourRangeFit );
		int metric = _flags & ( kColourMetricPerceptual | kColourMetricUniform );
		int extra = _flags & kWeightColourByAlpha;
	
		// set defaults
		if( method != kDxt3 && method != kDxt5 )
			method = kDxt1;
		if( fit != kColourRangeFit )
			fit = kColourClusterFit;
		if( metric != kColourMetricUniform )
			metric = kColourMetricPerceptual;
		
		// done
		return method | fit | metric | extra;
	}

	void Decompress( u8* _rgba, void const* _block, i32 _flags )
	{
		// fix any bad flags
		_flags = FixFlags( _flags );

		// get the block locations
		void const* colourBlock = _block;
		void const* alphaBock = _block;
		if( ( _flags & ( kDxt3 | kDxt5 ) ) != 0 )
			colourBlock = reinterpret_cast< u8 const* >( _block ) + 8;

		// decompress colour
		DecompressColour( _rgba, colourBlock, ( _flags & kDxt1 ) != 0 );

		// decompress alpha separately if necessary
		if( ( _flags & kDxt3 ) != 0 )
			DecompressAlphaDxt3( _rgba, alphaBock );
		else if( ( _flags & kDxt5 ) != 0 )
			DecompressAlphaDxt5( _rgba, alphaBock );
	}

	void decompressDXTImage( u8* _rgba, i32 _width, i32 _height, void const* _blocks, i32 _flags )
	{
		// fix any bad flags
		_flags = FixFlags( _flags );

		// initialise the block input
		u8 const* sourceBlock = reinterpret_cast< u8 const* >( _blocks );
		i32 bytesPerBlock = ( ( _flags & kDxt1 ) != 0 ) ? 8 : 16;

		// loop over blocks
		for( i32 y = 0; y < _height; y += 4 )
		{
			for( i32 x = 0; x < _width; x += 4 )
			{
				// decompress the block
				u8 targetRgba[4*16];
				Decompress( targetRgba, sourceBlock, _flags );
			
				// write the decompressed pixels to the correct image locations
				u8 const* sourcePixel = targetRgba;
				for( i32 py = 0; py < 4; ++py )
				{
					for( i32 px = 0; px < 4; ++px )
					{
						// get the target location
						i32 sx = x + px;
						i32 sy = y + py;
						if( sx < _width && sy < _height )
						{
							u8* targetPixel = _rgba + 4*( _width*sy + sx );
						
							// copy the rgba value
							for( i32 i = 0; i < 4; ++i )
								*targetPixel++ = *sourcePixel++;
						}
						else
						{
							// skip this pixel as its outside the image
							sourcePixel += 4;
						}
					}
				}
			
				// advance
				sourceBlock += bytesPerBlock;
			}
		}
	}

	static inline i32 getDXTFlag(const Texture::PixFormat _pixFmt)
	{
		switch (_pixFmt)
		{
		case Texture::PF_DXT1:
			return kDxt1;
			break;
		case Texture::PF_DXT3:
			return kDxt3;
			break;
		case Texture::PF_DXT5:
			return kDxt5;
			break;
		default:
			ITF_ASSERT(0);
			return 0;
			break;
		}
	}

	static inline void getPixelFormat(const Texture::PixFormat _pixFmt, GLint & _internalFmt, GLenum & _format, GLenum & _type)
	{
		switch(_pixFmt)
		{
		case Texture::PF_RGBA:
#ifdef ITF_IOS
				_internalFmt = GL_RGBA;
				_format = GL_BGRA_EXT;
                
				_type = GL_UNSIGNED_BYTE; // GL_UNSIGNED_SHORT_4_4_4_4;
#else
			// OpenGL PC : 
				_internalFmt =  GL_BGRA_EXT;
				_format =  GL_BGRA_EXT;
				_type = GL_UNSIGNED_BYTE;
#endif
				break;

		case Texture::PF_A8:
		case Texture::PF_L8:
				_internalFmt = GL_ALPHA;
				_format = GL_ALPHA;
				_type = GL_UNSIGNED_BYTE;
				break;
#ifndef ITF_IOS

		case Texture::PF_DXT1:
				_internalFmt = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
				_type = GL_UNSIGNED_BYTE;
				break;

		case Texture::PF_DXT3:
				_internalFmt = GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE;
				_format = GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE; 
				_type = GL_UNSIGNED_BYTE;
				break;

		case Texture::PF_DXT5:
				//_internalFmt = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				//_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				_internalFmt = GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE;
				_format = GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE; 
				_type = GL_UNSIGNED_BYTE;
				break;
#endif
                
		case Texture::PF_PVRTBPP4:
			_internalFmt = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
			_format = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
			_type = GL_UNSIGNED_BYTE;
			break;

			default:
				ITF_ASSERT(!"getPixelFormat() : unsupported format");
				_internalFmt = GL_NONE;
				_format = GL_NONE;
				_type = GL_UNSIGNED_BYTE;
				break;
		}
	}

	static inline Texture::PixFormat GLES2ToGFXPixFormat(GLenum _format)
	{
		switch (_format)
		{
		case GL_RGBA:
			return Texture::PF_RGBA;
		case GL_LUMINANCE_ALPHA:
			return Texture::PF_LA8;
		default:
			return Texture::PF_RGBA;
		}
	}

	// ------------------------------------------------------------------------------
	
	#define RoundTo(x, y)  (((x) + (y-1)) & ~ (y-1))
	void ComputeTextureMipLevelSizeAndPitch	(Texture::PixFormat _pixel_format,
														 u32         _Width,
														 u32         _Height,
														 u32         _MipLevel,
														 u32*       _Size,
														 u32*       _Pitch)
	{
		u32 size = 0;
		u32 pitch = 0;

		const u32 width = Max(_Width >> _MipLevel, u32(1));
		const u32 height = Max(_Height >> _MipLevel, u32(1));

		size = width * height;

		// store pitch
		pitch = width;

		switch (_pixel_format)
		{

		// 32 bits formats
		case Texture::PF_RGBA :
			size     *= 4;
			pitch    *= 4;
			break;

		// 16 bits formats
		case Texture::PF_LA8:
			size     *= 2;
			pitch    *= 2;
			break;

		case Texture::PF_A8 :
		case Texture::PF_L8 :
			// Nothing to do.
			break;

		case Texture::PF_DXT3 :
		case Texture::PF_DXT5 :
			// align width and height to 4x4 pixels, and calculate pitch
			size     = (RoundTo(width, 4) * RoundTo(height, 4));
			pitch    =  RoundTo(width, 4) * 4; // one row is 4 pixels high, that's why it is multiplied by 4
			break;

		// 4 bits compressed formats
		case Texture::PF_DXT1 :
			// all is divided by two because it is 4 bits per pixel format
			size     = ((RoundTo(width, 4) * RoundTo(height, 4)) >> 1);
			pitch    = ((RoundTo(width, 4) * 4) >> 1); // one row is 4 pixels high, that's why it is multiplied by 4
			break;

		case Texture::PF_PVRTBPP4 :
			size     = (RoundTo(width, 8) * RoundTo(height, 8)) * 4 / 8;
			pitch    = (RoundTo(width, 8) * 8 ) * 4 / 8;
			break;

		default:
			ITF_ASSERT (!"Pixel format not supported or not implemented on GLES2");
			break;
		}

		// store
		*_Pitch = pitch;
		*_Size = size;
	}

	// ------------------------------------------------------------------------------
	inline u32 GetLog2(u32 nValue)  // the pow2 "floor"
	{
		// Test this optimized version if you need it
		// return 1<<(powerOfTwo(nValue))
		ITF_ASSERT(nValue > 0); // Log(0) is not defined

		u32 r = 0;
		u32 shift;

		shift = ( ( nValue & 0xFFFF0000 ) != 0 ) << 4; nValue >>= shift; r |= shift;
		shift = ( ( nValue & 0xFF00     ) != 0 ) << 3; nValue >>= shift; r |= shift;
		shift = ( ( nValue & 0xF0       ) != 0 ) << 2; nValue >>= shift; r |= shift;
		shift = ( ( nValue & 0xC        ) != 0 ) << 1; nValue >>= shift; r |= shift;
		shift = ( ( nValue & 0x2        ) != 0 ) << 0; nValue >>= shift; r |= shift;

		return  r;
	}

	void GFXAdapter_OpenGLES2::loadFromMemory(const void* _buffer, const Size& _texture_size, Texture* _texture, Texture::PixFormat _pixel_format, bbool bCooked, u32 _dwMipMapCount)
	{
        PRF_M_SCOPE(GLES2_LoadFromMemory);
        
		cleanupTexture(_texture);

		if (bCooked)
		{
			GLuint texture;
			u32 mipCount = _dwMipMapCount > 0 ? _dwMipMapCount : 1;

            GLint internalFmt;
			GLenum format;
			GLenum type;

			bbool useDXTsoftDecompression = bfalse;
			
			if (_pixel_format == Texture::PF_DXT1 || _pixel_format == Texture::PF_DXT3 || _pixel_format == Texture::PF_DXT5)
			{
				useDXTsoftDecompression = useDXTHardDecompression ? bfalse : btrue;
				
				if (useDXTsoftDecompression == btrue)
					getPixelFormat(Texture::PF_RGBA, internalFmt, format, type);
				else
					getPixelFormat(_pixel_format, internalFmt, format, type);
			}
			else
				getPixelFormat(_pixel_format, internalFmt, format, type);
			            
			GL_CHECK( glGenTextures(1, (GLuint*)&texture) );
            GL_CHECK( glBindTexture(GL_TEXTURE_2D, texture) );

#ifdef ITF_GLES2_GENERATEMIPMAPS
            if( _texture->getPlatformFlags() == TEXTURE_PLATFORM_FLAG_NEEDMIPMAP ) // mips were not saved in this case, generate them
            {
                GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR) );
            }
            else
            {
                GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) );
            }
#else
            GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (mipCount > 1) ? GL_LINEAR_MIPMAP_LINEAR: GL_LINEAR) );
#endif
			GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
			GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ) );
			GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ) );
			
			PlatformTexture* newTex = newAlloc(mId_Textures, PlatformTexture);
			newTex->hwdData = texture;
			_texture->m_adapterimplementationData = newTex;
#ifdef ITF_ANDROID			
			LogI("GLGEN : glGenTextures %d , %s, %d %d", texture, _texture->getCookedPath().toString8().cStr(), _texture_size.d_width,  _texture_size.d_height);
#endif
			u8* currentBuffer = ((u8*)_buffer + sizeof(ITF::DDS_HEADER));

			u32 maxMipCount = GetLog2(Max(_texture_size.d_width,_texture_size.d_height)) + 1;
			ITF_ASSERT( ( mipCount == 1 ) || ( mipCount == maxMipCount ) );


#ifdef ITF_GLES2_GENERATEMIPMAPS
	        u32 mipIndex = 0;
#else
            u32 skippedMipCount=0;
			for (u32 mipIndex = 0; mipIndex < mipCount; ++mipIndex)
#endif
	        {
				u32 size, pitch;
				
				ComputeTextureMipLevelSizeAndPitch(_pixel_format, _texture_size.d_width, _texture_size.d_height, mipIndex, &size, &pitch);
                
                DeviceInfo::DeviceSpeed deviceSpeed = SYSTEM_ADAPTER->getDeviceInfo().getDeviceSpeed();
                if( _texture->getPlatformFlags() & TEXTURE_PLATFORM_FLAG_ALLOW_AUTODEGRAD_SD )
                {
                    if( deviceSpeed == DeviceInfo::Device_SpeedLow || deviceSpeed == DeviceInfo::Device_SpeedMedium )
                    {
                        // On a low or medium device, forget the first level of mipmaps
                        if( mipCount > 1 )
                        {
                            if( mipIndex ==  0 )
                            {
                                // just skip the first level
                                skippedMipCount = 1;
                                currentBuffer += size;
                                continue; // and continue
                            }
                        }
                    }
                }

				if (!useDXTsoftDecompression && (_pixel_format == Texture::PF_DXT1 || _pixel_format == Texture::PF_DXT3 || _pixel_format == Texture::PF_DXT5 || _pixel_format == Texture::PF_PVRTBPP4))
				{
#ifdef ITF_IOS
                    ITF_ASSERT_MSG(0, "The texture %s is in DXT! Not supported on iOS", _texture->getCookedPath().toString8().cStr());
#endif
					GL_CHECK( glCompressedTexImage2D(
							GL_TEXTURE_2D,
							mipIndex-skippedMipCount,
							internalFmt,
							Max(_texture_size.d_width>>mipIndex, (u32)1),
							Max(_texture_size.d_height>>mipIndex, (u32)1),
							0,
							size,
							currentBuffer
							) );
				}
				else
				{
					if (useDXTsoftDecompression)
					{
						int sizex =	Max(_texture_size.d_width>>mipIndex, u32(1));
						int sizey = Max(_texture_size.d_height>>mipIndex, u32(1));

						u32 computesize = sizex * sizey * 4;
						u8* buffer = (u8*)malloc( computesize );
						decompressDXTImage((u8*)buffer, sizex, sizey, (void*)currentBuffer, getDXTFlag(_pixel_format));

						PRF_M_SCOPE(GLES2_glTexImage2D);
						GL_CHECK( glTexImage2D(
							GL_TEXTURE_2D,
							mipIndex-skippedMipCount,
							internalFmt,
							sizex,
							sizey,
							0,
							format,
							type,
							buffer
							) );

						free(buffer);
					}
					else
					{
						PRF_M_SCOPE(GLES2_glTexImage2D);
						GL_CHECK( glTexImage2D(
							GL_TEXTURE_2D,
							mipIndex-skippedMipCount,
							internalFmt,
							Max(_texture_size.d_width>>mipIndex, u32(1)),
							Max(_texture_size.d_height>>mipIndex, u32(1)),
							0,
							format,
							type,
							currentBuffer
							) );
					}
				}

#ifdef ITF_GLES2_GENERATEMIPMAPS
				// Warning : won't work on PC... 
				if( _texture->getPlatformFlags() == TEXTURE_PLATFORM_FLAG_NEEDMIPMAP ) // mips were not saved in this case, generate them
                {
                    // nicest is way too slow
                    PRF_M_SCOPE(GLES2_GenerateMipmaps);
                    
                    glHint( GL_GENERATE_MIPMAP_HINT, GL_FASTEST );
					glGenerateMipmap( GL_TEXTURE_2D );
                    
                    _texture->m_mipMapLevels = (int)f32_Ceil( GetLog2(Max(_texture_size.d_width,_texture_size.d_height)))+1;
                }
#endif

                // get ready for next mip level
                // Alignment to do
                currentBuffer += size;
            }
		}
		else
		{
            // RGBA
            if (_pixel_format == Texture::PF_RGBA)
            {
                GFX_ADAPTER->createTexture(_texture, _texture_size.d_width, _texture_size.d_height, 1, _pixel_format, 2, btrue);
                
				LOCKED_TEXTURE lockRect;
				u32 lock_flags = 0;

				GFX_ADAPTER->lockTexture(_texture, &lockRect, lock_flags);

				u32* pDst = (u32 *)lockRect.mp_Bits;
				const u8* pSrc = static_cast<const u8*>(_buffer);

                u32 fullSize = 0;
                
				for ( ux y = 0; y < _texture->m_datasizeY; ++y )
				{
					for ( ux x = 0; x < _texture->m_datasizeX; ++x )
					{
						const char c = pSrc[x];
						pDst[x] = c | (c<<8) | (c<<16) | (c<<24);
					}
					pDst += lockRect.m_pitch/4;
					pSrc += _texture->m_datasizeX;
                    
                    fullSize += lockRect.m_pitch/4;
				}
                
				GFX_ADAPTER->unlockTexture(_texture);
			}
		}
	}

	// ------------------------------------------------------------------------------

	u32 GFXAdapter_OpenGLES2::loadTextureFromPlatformRawData(Texture* _texture, u8 * _rawBuf)
	{
		DDS_HEADER header;
		ITF_Memcpy(&header, _rawBuf, sizeof(header));
        
        PRF_M_SCOPE(GLES2_loadTextureFromPlatformRawData);
        
        
		if (header.dwHeight >= 8192 )
		{
			ITF_FATAL_ERROR("Texture size too big %d ==> %s ,quit the game and fix ", header.dwHeight, _texture->getPath().toString8().cStr());
		}

        Size size( header.dwWidth, header.dwHeight);
		setTextureSize(_texture, size);

		ITF::Texture::PixFormat PixFormat;

		switch (header.format.dwFourCC)
		{
			case MAKEFOURCC('D', 'X', 'T', '1'):
				PixFormat = Texture::PF_DXT1;
				break;
			case MAKEFOURCC('D', 'X', 'T', '2'):
				PixFormat = Texture::PF_DXT3;
				break;
			case MAKEFOURCC('D', 'X', 'T', '3'):
				PixFormat = Texture::PF_DXT3;
				break;
			case MAKEFOURCC('D', 'X', 'T', '4'):
				PixFormat = Texture::PF_DXT5;
				break;
			case MAKEFOURCC('D', 'X', 'T', '5'):
				PixFormat = Texture::PF_DXT5;
				break;
			case MAKEFOURCC('P', 'V', 'R', '4'):
				PixFormat = Texture::PF_PVRTBPP4;
				break;
			default:
				PixFormat = Texture::PF_RGBA;
				break;
		}

		if(!_texture)
			return 0;

		if (PixFormat == Texture::PF_PVRTBPP4)
			header.dwMipMapCount = GetLog2(Max(header.dwWidth,header.dwHeight)) + 1;

		// Fill Texture info
		_texture->m_datasizeX = _texture->getSizeX();
		_texture->m_datasizeY = _texture->getSizeY();
		_texture->m_mipMapLevels = header.dwMipMapCount;
		_texture->m_pixFormat = PixFormat;//GLES2ToGFXPixFormat(GL_RGBA);
		updateCachedScaleValues(_texture);

#if 1 // Decompress DXT right after loading (inside loading thread then)
		if ((PixFormat == Texture::PF_DXT1 || PixFormat == Texture::PF_DXT3 || PixFormat == Texture::PF_DXT5) && useDXTHardDecompression == bfalse)
		{
			u32 mipNumber = header.dwMipMapCount > 0 ? header.dwMipMapCount : 1;
/*#ifdef ITF_GLES2_GENERATEMIPMAPS
			mipNumber = 0;
			u32 computesize = _texture->m_datasizeX * _texture->m_datasizeY * 4;
#else*/
			u32 computesize = 0;
			u32 sizex = _texture->m_datasizeX;
			u32 sizey = _texture->m_datasizeY;

			for (u32 mipIndex = 0; mipIndex < mipNumber; mipIndex++)
			{

				u32 sx = Max(sizex>>mipIndex, (u32)1);
				u32 sy = Max(sizey>>mipIndex, (u32)1);

				computesize += sx * sy * 4;
			}
//#endif
			u8* buffer = (u8 *)Memory::alignedMallocCategory(computesize + sizeof(DDS_HEADER), 8192, MemoryId::mId_Textures);
			u8* currentBuffer = buffer + sizeof(DDS_HEADER);
			u8* prawbuffer = _rawBuf + sizeof(DDS_HEADER);

			sizex = _texture->m_datasizeX;
			sizey = _texture->m_datasizeY;
			int flag = getDXTFlag(PixFormat);

			for (u32 mipIndex = 0; mipIndex < mipNumber; ++mipIndex)
			{
				u32 sizec, pitch;
				ComputeTextureMipLevelSizeAndPitch(PixFormat, _texture->m_datasizeX, _texture->m_datasizeY, mipIndex, &sizec, &pitch);

				u32 sx = Max(sizex >> mipIndex, (u32)1);
				u32 sy = Max(sizey >> mipIndex, (u32)1);

				decompressDXTImage((u8*)currentBuffer, sx, sy, (void*)prawbuffer, flag);

				currentBuffer += sx * sy * 4;
				prawbuffer += sizec;
			}

			Memory::alignedFree(_rawBuf);
			_rawBuf = buffer;

			_texture->m_pixFormat = Texture::PF_RGBA;
		}
#endif
		
		// Defered if not main thread.
        
        // Always differed
// 		if ( 0 ) // Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID )
// 		{
// 			loadFromMemory(_rawBuf, size, _texture, PixFormat, btrue, header.dwMipMapCount);
// 		}
// 		else
		{
			textureGLES2 texture;
			texture.mp_dataRaw = _rawBuf;
			texture.mp_texture = _texture;
            static int iT=0;
            
            //LOG( "Adding Pending texture count %u", iT++ );
            
			AddPendingCreateUnlockTex2D(texture);
			return 2;
		}

// #ifndef ITF_FINAL
// 		//sprintf(_texture->m_dbgNfo,"more info cafe");
// #endif
// 		return 1;
	}

	// ------------------------------------------------------------------------------
	bbool GFXAdapter_OpenGLES2::loadTexture(Texture* _texture, const Path& _path)
	{
        PRF_M_SCOPE(GLES2_loadTexture);

		const Path & filenameCooked = _texture->getCookedPath();
		File* fileTexture = FILEMANAGER->openFile(filenameCooked, ITF_FILE_ATTR_READ);

		if (!fileTexture)
		{
			LOG("GFXAdapter_GLES2::loadTexture >> Missing texture %s", _path.toString8().cStr());
			return bfalse;
		}

		ux sizeRaw = ux(fileTexture->getLength());
		u32 bytesRead = 0;
		bbool success = bfalse;

		TextureCookedHeader texCookedHeader;
		ITF_VERIFY(fileTexture->read(&texCookedHeader, sizeof(TextureCookedHeader), &bytesRead));
		ITF_SwapByte(texCookedHeader);

		if (memcmp(texCookedHeader.signature, TextureCookedHeader::SIGNATURE, sizeof(TextureCookedHeader::SIGNATURE)) == 0)
		{
			_texture->setGenericParams(texCookedHeader);

			sizeRaw -= texCookedHeader.rawDataStartOffset;
			
			u8 * rawBuf = (u8 *)Memory::alignedMallocCategory(sizeRaw, 8192, MemoryId::mId_Textures);
			ITF_ASSERT_MSG(rawBuf, "LOAD Texture Failed out of memory %s", _path.toString8().cStr());
			if (rawBuf)
			{
				fileTexture->seek(texCookedHeader.rawDataStartOffset, FILE_BEGIN);
				ITF_VERIFY(fileTexture->read(rawBuf, sizeRaw, &bytesRead));

				u32 result = loadTextureFromPlatformRawData(_texture, rawBuf);

#ifndef ITF_FINAL
				_texture->setMemoryUsage(texCookedHeader.memorySize);
#endif //ITF_FINAL

				if(!result)
				{
					LOG("GFXAdapter_GLES2::loadTexture >> loading failed %s", _path.toString8().cStr());
					Memory::alignedFree(rawBuf);
				}
				else if (1 == result) // Loading Ok if main Thread
				{
					Memory::alignedFree(rawBuf);
					success = btrue;
				}
				else if (2 == result) // load in defered mode.
				{
					success = btrue;
				}
#ifdef ITF_TEXTURE_MEMORY_TRACKING
				if(result == 1 || result == 2)
				{
					// A Hack to keep track of textures allocations
					// Please note that this is not so accurate, REMOVE_ALLOCATION_SIZE is called before the texture is completely freed
					ADD_ALLOCATION_SIZE(texCookedHeader.memorySize, MemoryId::mId_Textures);

				}
#endif
			}
		}

		FILEMANAGER->closeFile(fileTexture);
		return success;
	}

	// ------------------------------------------------------------------------------


	// ------------------------------------------------------------------------------
	void GFXAdapter_OpenGLES2::cleanupTextureInternal( BasePlatformTexture * _texture )
	{
		if (!_texture)
            return;
        
		// Free the texture.
			PlatformTexture* tex = (PlatformTexture*)_texture;
			GLuint p = tex->hwdData;
            
            
            
			glDeleteTextures(1, &p);
            ///LOG( "glDeleteTexture %u", p );
            
            
          //  CVPixelBufferRelease(tex->m_pixelBuffer);
            
            /*
            glBindBuffer(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
            glActiveTexture(GL_TEXTURE_2D);
            
            glFlush();
            glFinish();
            */
            SF_DEL(tex);
		
	}
	void GFXAdapter_OpenGLES2::cleanupTexture( Texture* _texture )
	{
		if (!_texture)
            return;


        if( _texture->m_adapterimplementationData )
		{
#ifdef ITF_TEXTURE_MEMORY_TRACKING
			// Normally this should be called when the texture is freed
			REMOVE_ALLOCATION_SIZE(_texture->getMemoryUsage(), MemoryId::mId_Textures);
#endif

            AddPendingTextureDelete( _texture->m_adapterimplementationData );
		}

		_texture->m_adapterimplementationData = 0;
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::setSamplerState(u32 _samp, GFX_SAMPLERSTATETYPE _type, i32 _value)
	{
		ITF_ASSERT(_samp < MaxSampler);

		switch(_type)
		{
		case GFXSAMP_ADDRESSU:
			m_samplerWrapUPS[_samp] = (_value == GFX_TEXADRESSMODE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
			break;
		case GFXSAMP_ADDRESSV:
			m_samplerWrapVPS[_samp] = (_value == GFX_TEXADRESSMODE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
			break;
		case GFXSAMP_MINFILTER:
			m_samplerMinFilterPS[_samp] = (_value != 0) ? GL_LINEAR : GL_NEAREST;
			break;
		case GFXSAMP_MAGFILTER:
			m_samplerMaxFilterPS[_samp] = (_value != 0) ? GL_LINEAR : GL_NEAREST;
			break;
		default:
			ITF_ASSERT_MSG(0, "Unknown wrap type (%d)", _type);
			break;
		}
		m_dirtyFlags |= (1 << _samp);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::setTextureAdressingMode(u32 _samp, GFX_TEXADRESSMODE _texAdressModeU, GFX_TEXADRESSMODE _texAdressModeV)
	{
		m_samplerWrapUPS[_samp] = GFX_2_GLES2_AddressModeConvert(_texAdressModeU);
		m_samplerWrapVPS[_samp] = GFX_2_GLES2_AddressModeConvert(_texAdressModeV);

		m_dirtyFlags |= (1 << _samp);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::SetTextureBind(u32 _Sampler, BasePlatformTexture *_Bind, bbool _linearFiltering)
	{
		PlatformTexture *platformTex = static_cast<PlatformTexture *>(_Bind);
		ITF_ASSERT(_Sampler < MaxSampler);
		m_texturePS[_Sampler] = platformTex;
		if ( _linearFiltering )
		{
			m_samplerMinFilterPS[_Sampler] = GL_LINEAR;
			m_samplerMaxFilterPS[_Sampler] = GL_LINEAR;
		}
		else
		{
			m_samplerMinFilterPS[_Sampler] = GL_NEAREST;
			m_samplerMaxFilterPS[_Sampler] = GL_NEAREST;
		}
		m_dirtyFlags |= (1 << _Sampler);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::flushSetTextures()
	{
		for(u32 i=0 ; i < MaxSampler ; ++i)
		{
			if(m_texturePS[i])
			{
				GL_activeTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, m_texturePS[i]->hwdData);
			}
		}
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::invalidateSamplers()
	{
		for(u32 i=0; i<MaxSampler ; ++i)
		{
			m_texturePS[i] = 0;
		}
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::createTexture( Texture* _texture, u32 _sizeX, u32 _sizeY, u32 _mipLevel, Texture::PixFormat _pixformat, u32 _pool, bbool _dynamic )
	{
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		u32 mipIndex = 0;
		GLint internalFmt = GL_RGBA;
		GLenum format = GL_RGBA;
		GLenum type = GFX_ADAPTER_OPENGLES2->m_direct2BackBuffer ? GL_UNSIGNED_SHORT_4_4_4_4 : GL_UNSIGNED_BYTE;

		u32 size = _sizeX * _sizeY * ( GFX_ADAPTER_OPENGLES2->m_direct2BackBuffer ? 2 : 4); // 4 = RGBA, 2 = 4444
		u8* data = newAlloc(mId_Textures, u8[size]);


		glTexImage2D(GL_TEXTURE_2D, mipIndex, internalFmt, _sizeX, _sizeY, 0, format, type, (GLvoid*) data);
		delete[] data;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		PlatformTexture* newtex = (PlatformTexture*)newAlloc( mId_Textures, PlatformTexture());
		newtex->hwdData = texture;
		_texture->m_adapterimplementationData = newtex;

		// Fill Texture info.
		Size TexSize(_sizeX, _sizeY);
		setTextureSize(_texture, TexSize);
		_texture->m_datasizeX = (u32)TexSize.d_width;
		_texture->m_datasizeY = (u32)TexSize.d_height;
		_texture->m_mipMapLevels = mipIndex;
		_texture->m_pixFormat = GLES2ToGFXPixFormat(internalFmt);
		updateCachedScaleValues(_texture);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::lockTexture( Texture* _tex, LOCKED_TEXTURE* _lockTex, u32 _flag )
	{
		if ( !_tex || !_tex->m_adapterimplementationData ) return;

		u32 size = 0, pitch = 0;
		GLint internalFmt;
		GLenum format;
		GLenum type;
		
		getPixelFormat(_tex->m_pixFormat, internalFmt, format, type);

		ComputeTextureMipLevelSizeAndPitch(
			_tex->m_pixFormat,
			_tex->m_datasizeX,
			_tex->m_datasizeY,
			_tex->m_mipMapLevels,
			&size,
			&pitch
			);

		_lockTex->mp_Bits = (u8 *)Memory::alignedMallocCategory(size * sizeof(u8), 16, MemoryId::mId_Textures);
		_lockTex->m_pitch = pitch;

		PlatformTexture* tex = (PlatformTexture*)_tex->m_adapterimplementationData;
		tex->m_lockedTex = _lockTex->mp_Bits;
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::unlockTexture( Texture* _tex )
	{
		// WARNING: call from main thread !!.

		PlatformTexture* tex = (PlatformTexture*)_tex->m_adapterimplementationData;
		const GLuint textureId = tex->hwdData;
// 		const u32 u32_MipCount = _tex->m_mipMapLevels;
		const ux sx = _tex->m_datasizeX;
		const ux sy = _tex->m_datasizeY;
 
		u32 size = 0, pitch = 0;
		GLint internalFmt;
		GLenum format;
		GLenum type;
		
		getPixelFormat(_tex->m_pixFormat, internalFmt, format, type);
		
		ComputeTextureMipLevelSizeAndPitch(
			_tex->m_pixFormat,
			_tex->m_datasizeX,
			_tex->m_datasizeY,
			_tex->m_mipMapLevels,
			&size,
			&pitch
			);

		glBindTexture(GL_TEXTURE_2D, textureId);
		
		for (ux i = 0; i < 1/*u32_MipCount*/; ++i)
		{
			if (tex->m_lockedTex/*m_lockedMipInfo[i].m_pLockedData*/)
			{
				/*if (PixFmt == GRD_E_PixFmt_DXT1 || PixFmt == GRD_E_PixFmt_DXT5)
				{
					//glCompressedTexImage2D(GL_TEXTURE_2D, i, internalFmt, sx>>i, sy>>i, 0, size, (GLuint*)m_lockedMipInfo[i].m_pLockedData));
				}
				else*/
				{
					glTexImage2D(GL_TEXTURE_2D, i, internalFmt, sx>>i, sy>>i, 0, format, type, (GLvoid*)tex->m_lockedTex);
				}

				Memory::alignedFree( tex->m_lockedTex );
				tex->m_lockedTex = NULL;
			}
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT ); // GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); // GL_CLAMP_TO_EDGE);
	}

	// ------------------------------------------------------------------------------

}// namespace ITF
