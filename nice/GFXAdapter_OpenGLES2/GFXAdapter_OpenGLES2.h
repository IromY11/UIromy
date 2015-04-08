#ifndef _ITF_OPENGLES2_DRAWPRIM_H_
#define _ITF_OPENGLES2_DRAWPRIM_H_


#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif // _ITF_GFX_ADAPTER_H_

#if defined ITF_WINDOWS || defined ITF_IOS || defined ITF_ANDROID

#if defined(ITF_IOS) || defined(ITF_ANDROID)
//#define GLES2_NO_Z_BUFFER
#endif

//#define GL_CHECK(exp) {exp; GLint err = glGetError(); ITF_ASSERT_MSG( ( err == GL_NO_ERROR ), "Error : %08X", err ); }
#define GL_CHECK(exp) exp;

#define GL_GLEXT_PROTOTYPES

#if defined ITF_IOS
    #include <CoreVideo/CVOpenGLESTextureCache.h> // will include OpenGL and define CVEaglContext
    #include <CoreVideo/CVPixelBuffer.h>

    // official iOS includes
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>

#elif defined ITF_ANDROID
	#include <GLES/gl.h>
	#include <GLES/glext.h>
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#ifdef ITF_GLES2_POWER_VR
		#include "GLES2/PVR/Include/EGL/egl.h"
		#include "GLES2/PVR/Include/EGL/eglext.h"
		#include "GLES2/PVR/Include/EGL/eglplatform.h"
	#else
		#include "EGL/egl.h"
		#include "EGL/eglext.h"
		#include "EGL/eglplatform.h"
	#endif


    #ifdef ITF_GLES2_POWER_VR
        #include <GLES2/PVR/Include/GLES2/gl2.h>
        #include <GLES2/PVR/Include/GLES2/gl2ext.h>
        //	#include <GLES2/PVR/Include/GLES2/gl2platform.h>
    #else
        #include "GLES2/gl2.h"
        #include "GLES2/gl2ext.h"
    #endif


#endif


#ifndef _ITF_GFXADAPTER_SHADERMANAGER_H_
#include "engine/AdaptersInterfaces/GFXAdapter_ShaderManager.h"
#endif //_ITF_GFXADAPTER_SHADERMANAGER_H_

#ifndef ITF_GFX_ADAPTER_COMMON_H_
#include "engine/AdaptersInterfaces/GFXAdapter_Common.h"
#endif // ITF_GFX_ADAPTER_COMMON_H_

#ifndef ITF_DISABLE_WARNING
#define CHECK_SHADERS_VALIDITY
#endif // ITF_DISABLE_WARNING

#ifndef _ITF_STACKWALKER_H_
#include "core/StackWalker.h"
#endif //_ITF_STACKWALKER_H_

#ifndef _ITF_SYNCHRONIZE_H_
#include "core/System/Synchronize.h"
#endif

#ifndef _GFXADAPTER_BITMAPFONT_OPENGLES2_H_
#include "adapters/GFXAdapter_OpenGLES2/GFXAdapter_BitmapFont_OpenGLES2.h"
#endif // _GFXADAPTER_BITMAPFONT_OPENGLES2_H_

namespace ITF
{
	class ITF_Mesh;
	struct ITF_MeshElement;

//	#define _HARDCODED_SHADER

	#define GFX_ADAPTER_GLES2		(static_cast< GFXAdapter_OpenGLES2* > (GFX_ADAPTER))
	#define MaxSampler 16

	#define GLES2_DirtyTexMaskPS 0xffff
	#define GLES2_DirtyConstVS 0x10000
	#define GLES2_DirtyConstPS 0x20000
	#define GLES2_DirtyAttribVS 0x40000
	struct ITF_IndexBufferStruct
    { 
        ITF_IndexBufferStruct( u32 _indexBuf, u16 *_lockedBuff ) : mp_IndexBuffer(_indexBuf), mp_LockedBuffer(_lockedBuff) {}
        u32                     mp_IndexBuffer;    /// Data adress.
		u16						*mp_LockedBuffer;
    };
    
    struct ITF_VertexBufferStruct
    {
        ITF_VertexBufferStruct( u32 _vertexBuf, char *_lockedBuff ) : mp_VertexBuffer(_vertexBuf), mp_LockedBuffer(_lockedBuff) {}
        u32                     mp_VertexBuffer;    /// Data adress.
		char*					mp_LockedBuffer;
    };

    // Needed to rebuild the wind component at the right moment on opengl iOS
    class WindComponent;
    struct WindComponentUpdate
    {
        WindComponentUpdate( WindComponent *_wind, u32 _value ) { m_windComponent = _wind; m_value = _value; }
        WindComponent *m_windComponent;
        u32             m_value;
        
    } ;

	class GFXAdapter_OpenGLES2 : public GFXAdapter_Common<GFXAdapter_OpenGLES2>
	{
	public :
		class PlatformTexture : public BasePlatformTexture
		{
		public:
			PlatformTexture()
			{
				hwdData = 0;
				m_lockedTex = 0;
			}
			GLuint hwdData;
			void*	m_lockedTex;
		};

		class RenderTarget_GLES2 : public PlatformTexture
		{
		public: 
			RenderTarget_GLES2(RenderTargetInfo &_info);
			~RenderTarget_GLES2();

		private:
			RenderTargetInfo		m_info;
		};

		typedef GFXAdapter_Common<GFXAdapter_OpenGLES2> Super;

		enum
		{
			supportWireFrame = 0
		};


		struct BlendControl
		{
			GLenum		colorSrcBlend;
			GLenum		colorDstBlend;
			GLenum		colorCombine;
			GLboolean	separateAlphaBlend;
			GLenum		alphaSrcBlend;
			GLenum		alphaDstBlend;
			GLenum		alphaCombine;
			bool operator == (const BlendControl& _rhs) const
			{
				if(colorSrcBlend != _rhs.colorSrcBlend)
					return bfalse;
				if(colorDstBlend != _rhs.colorDstBlend)
					return bfalse;
				if(separateAlphaBlend != _rhs.separateAlphaBlend)
					return bfalse;
				if(alphaSrcBlend != _rhs.alphaSrcBlend)
					return bfalse;
				if(alphaDstBlend != _rhs.alphaDstBlend)
					return bfalse;
				if(alphaCombine != _rhs.alphaCombine)
					return bfalse;
				//if(target != _rhs.target)
				//	return bfalse;
				if(colorCombine != _rhs.colorCombine)
					return bfalse;
				return btrue;
			}
		};

		struct DepthStencilControl
		{
			GLboolean depthEnable;
			GLboolean depthWriteEnable;
			GLenum depthFunc;
			GLboolean stencilTestEnable;

			GLenum frontStencilFunc;
			GLenum frontStencilZPass;
			GLenum frontStencilZFail;
			GLenum frontStencilFail;
			u8 ref;
			u8 preMask;
			u8 writeMask;

			bool operator == (const DepthStencilControl& _rhs) const
			{
				if(depthEnable != _rhs.depthEnable)
					return bfalse;
				if(depthWriteEnable != _rhs.depthWriteEnable)
					return bfalse;
				if(depthFunc != _rhs.depthFunc)
					return bfalse;
				if(stencilTestEnable != _rhs.stencilTestEnable)
					return bfalse;
				if(frontStencilFunc != _rhs.frontStencilFunc)
					return bfalse;
				if(frontStencilZPass != _rhs.frontStencilZPass)
					return bfalse;
				if(frontStencilZFail != _rhs.frontStencilZFail)
					return bfalse;
				if(frontStencilFail != _rhs.frontStencilFail)
					return bfalse;
				if(ref != _rhs.ref)
					return bfalse;
				if(preMask != _rhs.preMask)
					return bfalse;
				if(writeMask != _rhs.writeMask)
					return bfalse;

				return btrue;
			}
		};

		struct AlphaTest
		{
			GLboolean alphaTestEnable;
			f32 ref;

			bool operator == (const AlphaTest& _rhs) const
			{
				if(alphaTestEnable != _rhs.alphaTestEnable)
					return bfalse;
				if(ref != _rhs.ref)
					return bfalse;
				return btrue;
			}
		 };

		struct ColorMask
		{
		};

		struct RenderStates
		{
			public:
			const BlendControl&         getBlendControl()         const {return m_BlendControl;}
			const DepthStencilControl&  getDepthStencilControl()  const {return m_DepthStencilControl;}
			const AlphaTest&            getAlphaTest()            const {return m_AlphaTest;}
			private:
			BlendControl m_BlendControl;
			DepthStencilControl m_DepthStencilControl;
			AlphaTest m_AlphaTest;

			public:
			void flush()
			{
				//SetBlendControl(m_BlendControl, btrue);
				SetDepthStencilControl(m_DepthStencilControl, btrue);
				SetAlphaTest(m_AlphaTest, btrue);
			}

			void SetBlendControl( const BlendControl& _val, bbool _bflush = bfalse)
			{
				if(!_bflush)
					if(_val == m_BlendControl)
						return;

				m_BlendControl = _val;
				glEnable(GL_BLEND);
				glBlendEquation(_val.colorCombine);
				if (_val.separateAlphaBlend)
					glBlendFuncSeparate(_val.colorSrcBlend, _val.colorDstBlend, _val.alphaSrcBlend, _val.alphaDstBlend);
				else
					glBlendFunc(_val.colorSrcBlend, _val.colorDstBlend);
			}

			void SetDepthStencilControl( const DepthStencilControl& _val, bbool _bflush = bfalse)
			{
				if(!_bflush)
					if(_val == m_DepthStencilControl)
						return;
				m_DepthStencilControl = _val;
					
				if (_val.depthWriteEnable)
				{
#ifndef GLES2_NO_Z_BUFFER
					glDepthMask(GL_TRUE);
#endif
				}
				else
				{
					glDepthMask(GL_FALSE);
				}

#ifndef GLES2_NO_Z_BUFFER
				if (_val.depthEnable)
				{
					glEnable(GL_DEPTH_TEST);
				}
				else
#endif
				{
					glDisable(GL_DEPTH_TEST);
				}

				glDepthFunc(_val.depthFunc);

				if (_val.stencilTestEnable)
				{
					glEnable(GL_STENCIL_TEST);

					glStencilFunc(_val.frontStencilFunc, _val.ref, _val.preMask);
					glStencilOp (_val.frontStencilFail, _val.frontStencilZFail, _val.frontStencilZPass);
					glStencilMask(_val.writeMask);
				}
				else
				{
					glDisable(GL_STENCIL_TEST);
				}
			}

			void SetAlphaTest(const AlphaTest& _val, bbool _bflush = bfalse)
			{
				if(!_bflush)
					if(_val == m_AlphaTest)
						return;
				m_AlphaTest = _val;
			}
		};

		typedef GLuint PlatformSurface;
	
		class DepthBuffer
		{
		public :
			DepthBuffer()
			{
				m_surfWidth = 0;
				m_surfHeight = 0;
				m_depthBuffer = 0;
			}


			u32              getWidth()           const { return m_surfWidth; }
			u32              getHeight()          const { return m_surfHeight; }
			void*            getPointer()         const { return 0;/*m_depthBuffer.surface.imagePtr;*/ }
			u32              getSize()            const { return 0;/*m_depthBuffer.surface.imageSize;*/ }
			u32              getAlignment()       const { return 0;/*m_depthBuffer.surface.alignment;*/ }
			//DEAC GX2SurfaceFormat getFormat()          const { return 0;/*m_depthBuffer.surface.format;*/ }

			bbool            isHiZEnable()        const { return 0;/*DEAC m_depthBuffer.hiZPtr != NULL; */}

			//GLuint			*getDepthBuffer()           { return &m_depthBuffer; }
			GLuint			getDepthBuffer()	        { return m_depthBuffer; }
			PlatformTexture *getTextureSamp()           { return &m_textureSamp; }

			u32				m_surfWidth;
			u32				m_surfHeight;

			void Invalidate()
			{
			}

            void Release()
            {
				// ? glBindRenderbuffer(GL_RENDERBUFFER, 0);
                glDeleteRenderbuffers(1, &m_depthBuffer);
                
            }
            
			void Init(u32 _width, u32 _height/*, GX2SurfaceFormat _format*/)
			{
				m_surfWidth = _width;
				m_surfHeight = _height;
#if 0
				GLuint texture;
				glGenTextures(1, (GLuint*)&texture);
				glBindTexture(GL_TEXTURE_2D, texture);

				u32 mipIndex = 0;
				GLint internalFmt = GL_RGBA;
				GLenum format = GL_RGBA;
				GLenum type = GL_UNSIGNED_BYTE;
				u32 size = getWidth() * getHeight() * 4; // 4 = RGBA
				
				void* image_data = (void*)malloc(size);
				memset(image_data, 0, size);

				glTexImage2D(GL_TEXTURE_2D, mipIndex, internalFmt, getWidth(),  getHeight(), 0, format, type, (GLvoid*) image_data);
				
				free(image_data);
#endif
				glGenRenderbuffers(1, &m_depthBuffer);
				glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);

/*				GLuint framebufferWidth = getWidth();
				GLuint framebufferHeight = getHeight();
				glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth);
				glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);*/

//				glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, getWidth(), getHeight());

				// attach renderbuffer.
				//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
				//DEAC m_textureSampled = texture;


				//DEAC GX2InitDepthBuffer(&m_depthBuffer, _width, _height, _format, _aa);
#ifndef ITF_FINAL
/*
				GX2InitTexture(m_textureSamp.getTexture(), getWidth(), getHeight(), 1, 0, GX2_SURFACE_FORMAT_TCD_R32_FLOAT, GX2_SURFACE_DIM_2D);
				m_textureSamp.getTexture()->surface.use = GX2_SURFACE_USE_TEXTURE;
				m_textureSamp.getTexture()->compSel = GX2_COMP_SEL_XXXX;
				GX2InitTextureRegs(m_textureSamp.getTexture());
				m_textureSamp.InitSampler(GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP, GX2_TEX_XY_FILTER_POINT);
*/
#endif // ITF_FINAL
			}

			void InitPtr(void *_ptr)
			{
				//DEAC GX2InitDepthBufferPtr(&m_depthBuffer, _ptr);
#ifndef ITF_FINAL
				//DEAC GX2InitTexturePtrs(m_textureSamp.getTexture(), getPointer(), 0);
#endif // ITF_FINAL
			}

			void InitHiZPtr(void *_ptr)
			{
				//DEAC GX2InitDepthBufferHiZPtr(&m_depthBuffer, _ptr);
			}

#ifndef ITF_FINAL
			void Expand()
			{
				//DEAC GX2ConvertDepthBufferToTextureSurface(&m_depthBuffer, &m_textureSamp.getTexture()->surface, 0, 0);
				Invalidate();
			}
#endif // ITF_FINAL

			void Copy(DepthBuffer *_src)
			{
				m_depthBuffer     = _src->m_depthBuffer;
				m_textureSamp     = _src->m_textureSamp;
			}

			void Resize(u32 _width, u32 _height, bool _allocated = false)
			{
				Init(_width, _height/*, getFormat()*/);
			}

    private:
			GLuint			m_depthBuffer;
			PlatformTexture	m_textureSamp;
		};

		class TextureBuffer
		{
			public :
			TextureBuffer()            
				: m_allocatedSize(0)
				, m_allocatedHeight(0)
				, m_allocatedWidth(0)
				, m_textureId(0)
			{
#ifdef TEX_BFR_USE_SUB_LIST
				m_mainTextureBuffer = this;
				m_next = NULL;
#endif // TEX_BFR_USE_SUB_LIST

				//ITF_MemSet(&m_frameBuffer, 0, sizeof(m_frameBuffer));
				//DEAC m_textureSampled.InitSampler(GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP, GX2_TEX_XY_FILTER_POINT);
			
				m_surfWidth = 0;
				m_surfHeight = 0;
			}

			~TextureBuffer()
			{
#ifdef TEX_BFR_USE_SUB_LIST
				setMain(NULL);
#endif // TEX_BFR_USE_SUB_LIST
			}

			u32              getWidth()           const { return m_surfWidth; }
			u32              getHeight()          const { return m_surfHeight; }
			u32              getAllocatedWidth()  const { return m_allocatedWidth; }
			u32              getAllocatedHeight() const { return m_allocatedHeight; }
			void*            getPointer()         const { return 0;/*DEAC m_frameBuffer.surface.imagePtr;*/ }
			u32              getAllocatedSize()   const { return m_allocatedSize; }
			u32              getSize()            const { return 0;/*DEAC m_frameBuffer.surface.imageSize;*/ };
			//DEAC GX2SurfaceFormat getFormat()          const { return m_frameBuffer.surface.format; }

			u32				m_surfWidth;
			u32				m_surfHeight;
			GLuint			getColorBuffer()           { return m_textureId; }
			//DEAC GLuint*				getTexture()               { return m_textureSampled.getTexture(); }
			PlatformTexture*	getTextureSamp()           { return &m_textureSampled; }

#ifdef TEX_BFR_USE_SUB_LIST
			void*            getBasePointer()     const;
			TextureBuffer   *getMain()                  { return m_mainTextureBuffer; }
			void             setMain(TextureBuffer *_m);
			bool             isSub()              const { return m_mainTextureBuffer != this; }
#endif // TEX_BFR_USE_SUB_LIST

			void Invalidate()
			{
			}


			void Resize(u32 _width, u32 _height, bool _allocated = false)
			{
				Init(_width, _height/*, DEAC getFormat()*/ );
				if(_allocated)
				{
					m_allocatedWidth  = _width;
					m_allocatedHeight = _height;
				}
			}

			/* DEAC void ChangeFormat(GX2SurfaceFormat _fmt)
			{
				m_colorBuffer.surface.format = _fmt;
				Init(getWidth(), getHeight(), getFormat(), getAAMode());
			}*/

			void InitFrom(GLuint* _ptrTex)
			{
				ITF_ASSERT_CRASH(_ptrTex != 0, "NULL texture pointer");
				//DEAC Init(_ptrTex->surface.width, _ptrTex->surface.height, _ptrTex->surface.format, _ptrTex->surface.aa);
				//DEAC setPtr(_ptrTex->surface.imagePtr);

				//DEAC ITF_ASSERT_CRASH(getSize() == _ptrTex->surface.imageSize, "Invalid texture size");
				//DEAC ITF_ASSERT_CRASH(getTexture()->surface.imageSize == _ptrTex->surface.imageSize, "Invalid texture size");
			}


			void Init(u32 _width, u32 _height,/*GX2SurfaceFormat _format,*/ bbool _ftv = bfalse);

			void InitPtr(void* _baseptr, u32 _allocatedWidth, u32 _allocatedHeight, u32 _allocatedSize, u32 _pointerOffset = 0)
			{
				//DEAC setPtr(_baseptr + _pointerOffset);
			
				m_allocatedWidth  = _allocatedWidth;
				m_allocatedHeight = _allocatedHeight;
				m_allocatedSize   = _allocatedSize;
			}

			void Copy(TextureBuffer *_src, u32 _pointerOffset = 0)
			{
				m_textureId       = _src->m_textureId;
				m_textureSampled  = _src->m_textureSampled;
				m_allocatedSize   = _src->m_allocatedSize;
				m_allocatedWidth  = _src->m_allocatedWidth;
				m_allocatedHeight = _src->m_allocatedHeight;
			}

#ifdef TEX_BFR_USE_SUB_LIST
			void changePointer(void *_ptr);
#endif // TEX_BFR_USE_SUB_LIST

			void release()
			{
/*				if (m_frameBuffer)
				{
					glDeleteFramebuffers(1, &m_frameBuffer);
				}*/

				if (m_textureSampled.hwdData)
				{
					glDeleteTextures(1, &m_textureId);					
				}			
			}

			void clear(const Color& color = Color::zero())
			{
				ITF_ASSERT(getColorBuffer());
			}

		private:

			GLuint			m_textureId;
			PlatformTexture	m_textureSampled;
			u32             m_allocatedSize;
			u32             m_allocatedWidth;
			u32             m_allocatedHeight;
#ifdef TEX_BFR_USE_SUB_LIST
			TextureBuffer*  m_mainTextureBuffer; // link to main when subset of buffer. If not a subset then m_mainTextureBuffer = this.
			TextureBuffer*  m_next;
#endif // TEX_BFR_USE_SUB_LIST
		};

#ifdef _HARDCODED_SHADER
		#define MAX_SHADER 10
		GLuint gProgram[MAX_SHADER];
		GLuint gvPositionHandle[MAX_SHADER];
		GLuint gvColorHandle[MAX_SHADER];
		GLuint gvUVHandle[MAX_SHADER];
#endif

		#define ITF_DECLARE_VS_ATTRIB_REG(attribType, attribName, attribFirstReg) \
		VS_Attrib_##attribName = attribFirstReg,

		#define ITF_DECLARE_VS_ATTRIB_ARRAY_REG(attribType, attribName, arraySize, attribFirstReg) \
			VS_Attrib_##attribName = attribFirstReg,

		#define ITF_DECLARE_VS_ATTRIB_STRUCT_REG(structType, attribName, structSize, attribFirstReg) \
			VS_Attrib_##attribName = attribFirstReg,

		typedef enum _VertexShaderAttrib
		{
			#include "../bin/Shaders_dev/Unified/ParameterListVS_Macros.h"
		} VertexShaderAttrib;

		// ignore PS reg on PS3
		#define ITF_DECLARE_PS_ATTRIB_REG(attribType, attribName, attribFirstReg) \
			PS_Attrib_##attribName = attribFirstReg,

		#define ITF_DECLARE_PS_ATTRIB_ARRAY_REG(attribType, attribName, arraySize, attribFirstReg) \
			PS_Attrib_##attribName = attribFirstReg,

		#define ITF_DECLARE_PS_ATTRIB_STRUCT_REG(structType, attribName, structSize, attribFirstReg) \
			PS_Attrib_##attribName = attribFirstReg,

		enum PixelShaderAttrib
		{
			#include "../bin/Shaders_dev/Unified/ParameterListPS_Macros.h"
		};

#if !defined(ITF_FINAL)
			virtual void                project                 (   const Vec3d& /*_in3d*/, Vec2d & /*_out2d*/) {/*ITF_ASSERT_MSG(0,__FUNCTION__ " not implemented");*/}
#endif // !defined(ITF_FINAL)
			class textureGLES2
			{
				public:				
				textureGLES2()
				{
					mp_dataRaw = 0;
					mp_texture = 0;
					mp_dataSize = 0;
				}
				
				u8* mp_dataRaw;
				u32 mp_dataSize;
				Texture* mp_texture;

				void FlushDeferred();

				//GFX_TEXADRESSMODE   m_clampX;
				//GFX_TEXADRESSMODE   m_clampY;
				//GX2TexXYFilterType  m_filter;
				//GX2Sampler*         m_sampler;
			};
			
			bbool						loadTexture             (   Texture* _tex, const Path& _path);
			u32							loadTextureFromPlatformRawData(Texture* _texture, u8 * _rawBuf);
		    void						loadFromMemory (   const void * _buffer, const Size & _texture_size, Texture* _texture, Texture::PixFormat _pixel_format, bbool bCooked, u32 _dwMipMapCount);
			void						FlushAllPendingCreateUnlock      ();

			ITF_VECTOR <textureGLES2>					m_pendingCreateUnlockTex2D;
			ITF_VECTOR <BasePlatformTexture*>           m_pendingTextureDelete;

			ITF_VECTOR <ITF_VertexBuffer*>	m_pendingCreateUnlockVerterBuffer;
			ITF_VECTOR <ITF_IndexBuffer*>	m_pendingCreateUnlockIndexBuffer;

			ITF_VECTOR <ITF_VertexBuffer*>	m_pendingCreateVerterBuffer;
			ITF_VECTOR <ITF_IndexBuffer*>	m_pendingCreateIndexBuffer;

			ITF_VECTOR <ITF_IndexBufferStruct>	m_pendingReleaseIndexBuffer;
            ITF_VECTOR <ITF_VertexBufferStruct>	m_pendingReleaseVertexBuffer;
        

			ITF_THREAD_CRITICAL_SECTION		m_csUnlockTexturePend;
			ITF_THREAD_CRITICAL_SECTION		m_csDeleteTexturePend;
			ITF_THREAD_CRITICAL_SECTION		m_csVBPend;
			ITF_THREAD_CRITICAL_SECTION		m_csIBPend;
            
			ITF_INLINE void				AddPendingCreateUnlockTex2D(textureGLES2& _tex2D)
			{
                csAutoLock cs(m_csUnlockTexturePend);
				m_pendingCreateUnlockTex2D.push_back(_tex2D);
			}

            ITF_INLINE void				AddPendingTextureDelete(BasePlatformTexture* _tex)
            {
                csAutoLock cs(m_csDeleteTexturePend);
                m_pendingTextureDelete.push_back(_tex);
            }
        
#ifdef ITF_SUPPORT_EDITOR
			static void waitEndOfProcessing() {}
#else
			static void waitEndOfProcessing();
#endif
			private:
            ITF_INLINE bool	isAdapterStillProcessing()
			{
				int count = 0;
                {
                    csAutoLock csVB(m_csVBPend);

					count += m_pendingCreateUnlockVerterBuffer.size();
					count += m_pendingCreateVerterBuffer.size();
                    count += m_pendingReleaseVertexBuffer.size();				}

				{
					csAutoLock csIB(m_csIBPend);
                    
                    count += m_pendingCreateUnlockIndexBuffer.size();
                    count += m_pendingCreateIndexBuffer.size();
                    count += m_pendingCreateIndexBuffer.size();
				}   

                if( count == 0 )
                    return false;
                    
                return true;
			}

			public:
                // this one is used to remove from the pending VBO creation a deleted vertex buffer 
                        void removePendingVertexBufferCreation(ITF_VertexBuffer* _vertxb);


			ITF_INLINE void				AddPendingCreateUnlockVertexBuffer(ITF_VertexBuffer* _vertxb)
			{
				csAutoLock cs(m_csVBPend);
				m_pendingCreateUnlockVerterBuffer.push_back(_vertxb);
			}
			ITF_INLINE void				AddPendingCreateUnlockIndexBuffer(ITF_IndexBuffer* _indxb)
			{
				csAutoLock cs(m_csIBPend);
				m_pendingCreateUnlockIndexBuffer.push_back(_indxb);
			}
			ITF_INLINE void				AddPendingCreateVertexBuffer(ITF_VertexBuffer* _vertxb)
			{
				csAutoLock cs(m_csVBPend);
				m_pendingCreateVerterBuffer.push_back(_vertxb);
			}
			ITF_INLINE void				AddPendingCreateIndexBuffer(ITF_IndexBuffer* _indxb)
			{
				csAutoLock cs(m_csIBPend);
				m_pendingCreateIndexBuffer.push_back(_indxb);
			}
  			ITF_INLINE void				AddPendingReleaseIndexBuffer(ITF_IndexBuffer* _indxb)
			{
#ifdef ITF_GLES2
				csAutoLock cs(m_csIBPend);
				m_pendingReleaseIndexBuffer.push_back( ITF_IndexBufferStruct(_indxb->mp_IndexBuffer, _indxb->mp_LockedBuffer) );
#endif
			}
  			ITF_INLINE void				AddPendingReleaseVertexBuffer(ITF_VertexBuffer* _vertxb)
			{
#ifdef ITF_GLES2
				csAutoLock cs(m_csVBPend);
                ITF_VertexBufferStruct VB(_vertxb->mp_VertexBuffer, _vertxb->mp_LockedBuffer);
				m_pendingReleaseVertexBuffer.push_back(VB);
#endif
			}
            
			ITF_INLINE GLenum GFX_2_GLES2_AddressModeConvert(GFX_TEXADRESSMODE _texAdressMode)
			{
				switch (_texAdressMode)
				{
				case GFX_TEXADRESSMODE_WRAP:
					return GL_REPEAT;
				case GFX_TEXADRESSMODE_MIRROR:
					return GL_MIRRORED_REPEAT;
				case GFX_TEXADRESSMODE_CLAMP:
					return GL_CLAMP_TO_EDGE;
				case GFX_TEXADRESSMODE_BORDER:
				default:
					ITF_ASSERT_MSG(0, "Unknown wrap mode (%d)", _texAdressMode);
					break;
				}
				return GL_REPEAT;
			}

			ITF_INLINE GLenum getCmpFunc(GFX_CMPFUNC _cmp)
			{
				switch ( _cmp )
				{
					case GFX_CMP_NEVER:			return GL_NEVER;
					case GFX_CMP_LESS:			return GL_LESS;
					case GFX_CMP_EQUAL:			return GL_EQUAL;
					case GFX_CMP_LESSEQUAL:		return GL_LEQUAL;
					case GFX_CMP_GREATER:		return GL_GREATER;
					case GFX_CMP_NOTEQUAL:		return GL_NOTEQUAL;
					case GFX_CMP_GREATEREQUAL:	return GL_GEQUAL;
					default:
					case GFX_CMP_ALWAYS:		return GL_ALWAYS;
				}
			}

			ITF_INLINE GLenum getStencilOp(GFX_STENCILFUNC _func)
			{
				switch ( _func )
				{
					default:
					case GFX_STENCIL_KEEP:			return GL_KEEP;
					case GFX_STENCIL_ZERO:			return GL_ZERO;
					case GFX_STENCIL_REPLACE:		return GL_REPLACE;
					case GFX_STENCIL_INCR_WRAP:		return GL_INCR_WRAP;
					case GFX_STENCIL_DECR_WRAP:		return GL_DECR_WRAP;
					case GFX_STENCIL_INVERT:		return GL_INVERT;
					case GFX_STENCIL_INCR_SAT:      return GL_INCR;
					case GFX_STENCIL_DECR_SAT:      return GL_DECR;
				}
			}

                                        GFXAdapter_OpenGLES2();
        virtual                         ~GFXAdapter_OpenGLES2   (   ); // always declare virtual destructor for adapters
		virtual void                    init                    (   );
       
		virtual void                    destroy();
				void                    close();
		virtual void                    destroyResources();

		virtual void                    setAlphaBlend           (   GFX_BLENDMODE _blend, GFX_ALPHAMASK _alphaMask  );
		virtual void                    setCullMode             (   GFX_CULLMODE /*_cull*/    );
		virtual void                    setFillMode             (   GFX_FILLMODE /*_fillmode*/  );

		virtual void                    depthSet                (   u32 _depthTest, u32 _depthMask, u32 _depthFunc, f32 _biasOffset, f32 _biasSlope );
		virtual void                    depthSet                (   u32 _depthTest, u32 _depthMask, f32 _biasOffset, f32 _biasSlope );
		virtual void                    depthSet                (   u32 _depthTest, u32 _depthMask );
		virtual void                    depthMask               (   u32 /*_depthMask*/    );
		virtual void                    depthFunc               (   u32 /*_func*/    );
		virtual void                    depthTest               (   u32 /*_test*/    );
		void                            depthStencil            (   bbool stencilTestEnable, GFX_CMPFUNC stencilFunc,
																	GFX_STENCILFUNC stencilZPass, GFX_STENCILFUNC stencilZFail, GFX_STENCILFUNC stencilFail,
																	u8 ref, u8 preMask=0xff, u8 writeMask=0xff);
		virtual void                    colorMask               (   GFX_COLOR_MASK_FLAGS _colorMask = GFX_COL_MASK_ALL);

		virtual void                    setScissorRect          (   GFX_RECT* /*_clipRect*/    );
				void                    SetTextureBind          (   u32 _Sampler, BasePlatformTexture * _Bind, bbool _linearFiltering = btrue);
		virtual void                    lockTexture             (   Texture* /*_tex*/, LOCKED_TEXTURE* /*_lockTex*/, u32 /*_flag*/ = 0 );
		virtual void                    unlockTexture           (   Texture* /*_tex*/   );

		virtual void                    cleanupTexture          (   Texture* /*_texture*/   );
                void                    cleanupTextureInternal  (   BasePlatformTexture* _texture );
				void                    initDefault2DRender     (   RenderPassContext& _rdrCtx );

		// Matrix.
		virtual void                    setupViewport           (   GFX_RECT* /*_Viewport*/    );
				void                    setupViewport           (   i32 x, i32 y, i32 w, i32 h );
#if !defined(ITF_FINAL)
		virtual void                    unprojectPoint          (    const Vec2d& /*_in*/, float /*_viewDistance*/, const Matrix44* /*_MatrixModel*/, const Matrix44* /*_MatrixProj*/, const int* /*_View*/, Vec2d& /*_out*/) {/*ITF_ASSERT_MSG(0,__FUNCTION__ " not implemented");*/}
		virtual void                    setLookAtMatrix         (    Matrix44* /*_MatrixOut*/, double /*_midx*/, double /*_midy*/, float /*_viewDistance*/, float /*_aspect*/) {/*ITF_ASSERT_MSG(0,__FUNCTION__ " not implemented");*/}
#endif // !defined(ITF_FINAL)
		ITF_INLINE Matrix44             convertM44ToSpecificPlatformGPU(const Matrix44 &_in)
		{
			Matrix44 trans;
			trans.transpose(_in);
			return trans;
		}

		virtual void                    getViewport             (    GFX_Viewport & _vp);
				void                    prepareGfxMatReflection (   const GFX_MATERIAL& _gfxMat );
				void                    prepareGfxMatFluid          (   const GFX_MATERIAL& _gfxMat );

				f32                     getCentroidOffset       () const { return 0.f; }

		/// Texture.
		virtual void                    generateTexture         (   Texture* /*_texture*/  )  {}
		virtual void                    createTexture           (   Texture* /*_texture*/, u32 /*_sizeX*/, u32 /*_sizeY*/, u32 /*_mipLevel*/, Texture::PixFormat /*_pixformat*/, u32 /*_pool*/ = 1, bbool /*_dynamic*/ = bfalse );
		virtual unsigned char*          grabTexture             (   Texture* /*_texture*/   ) {/*ITF_ASSERT_MSG(0,__FUNCTION__ " not implemented");*/return NULL;}
		virtual u32                     getMaxTextureSize       (   ) {/*ITF_ASSERT_MSG(0,__FUNCTION__ " not implemented");*/return 0;}
		virtual void                    saveToMemory            (    void* /*_buffer*/, Texture* /*_Tex*/ ) {/*ITF_ASSERT_MSG(0,__FUNCTION__ " not implemented");*/}

		virtual void                    setSamplerState         (   u32 /*_samp*/, GFX_SAMPLERSTATETYPE /*_type*/, i32 /*_value*/ );
		virtual void                    setTextureAdressingMode (   u32 _samp, GFX_TEXADRESSMODE _texAdressModeU, GFX_TEXADRESSMODE _texAdressModeV);

		// Sampler state global cache.
		GLenum	m_samplerMinFilterPS[MaxSampler];
		GLenum	m_samplerMaxFilterPS[MaxSampler];
		GLenum	m_samplerWrapUPS[MaxSampler];
		GLenum	m_samplerWrapVPS[MaxSampler];
		PlatformTexture*	m_texturePS[MaxSampler];

		virtual void                    clear                   (   u32 /*_buffer*/, float /*_r*/=0.f, float /*_g*/=0.f, float /*_b*/=0.f, float /*_a*/=0.f );

		void							drawDebug();


		virtual void                    present                 (   );

		virtual void                    beginDisplay            (f32 _dt);
		virtual void                    endDisplay              (   );
		virtual void                    beginViewportRendering  ( View &_view  );
		virtual void                    endViewportRendering    ( View &_view  );
		virtual void					beginSceneRendering		( View &_view  );
		virtual void					endSceneRendering		( View &_view  );

				void					setBackBuffer			(bbool _setZbuffer = btrue, bbool _restoreBackBuffer = bfalse);

				void                    updateRenderBuffers     (   );
		virtual bbool                   waitPreviousGPUframeEnd ( u32 _depth );
				void                    syncGPU                 ( u64 _timeStamp );
		virtual void                    syncGPU                 (   );

				void                    BeginSimpleFrame();
				void                    EndSimpleFrame();
				void                    RenderSimpleFrame();

		virtual bbool                   setScreenProtection(bbool _enable);
				void                    removeForceScreenProtection();
				void                    forceSetScreenProtection();

    
		virtual bbool                   isDeviceLost            (   )   { return bfalse; }
		virtual bbool                   resetDevice             (   );

#ifdef ITF_USE_REMOTEVIEW
		virtual ITF_INLINE u32          getRemoteScreenWidth    (   ) const { return 856; }
		virtual ITF_INLINE u32          getRemoteScreenHeight   (   ) const { return 480; }
#else // ITF_USE_REMOTEVIEW
		virtual ITF_INLINE u32          getRemoteScreenWidth    (   ) const { return 0; }
		virtual ITF_INLINE u32          getRemoteScreenHeight   (   ) const { return 0; }
#endif
		virtual  u32                getLightPassWidth     (   ) const { return getScreenWidth() / LIGHTS_BFR_RATIO;}
		virtual  u32                getLightPassHeight     (   ) const { return getScreenHeight() / LIGHTS_BFR_RATIO;}
		virtual  u32                getQrtLightPassWidth     (   ) const { return getScreenWidth() / QRT_LIGHTS_BFR_RATIO;}
		virtual  u32                getQrtLightPassHeight     (   ) const { return getScreenHeight() / QRT_LIGHTS_BFR_RATIO;}
		virtual  u32                getReflectionPassWidth     (   ) const { return getScreenWidth() / REFLECTION_BFR_RATIO;}
		virtual  u32                getReflectionPassHeight     (   ) const { return getScreenHeight() / REFLECTION_BFR_RATIO;}
		virtual  u32                getRefractionPassWidth     (   ) const {return getScreenWidth() / REFRACTION_BFR_RATIO;}
		virtual  u32                getRefractionPassHeight     (   ) const { return getScreenHeight() / REFRACTION_BFR_RATIO;}
		virtual  u32                getFluidPassWidth     (   ) const {return getScreenWidth() / FLUID_GLOW_BFR_RATIO;}
		virtual  u32                getFluidPassHeight     (   ) const { return getScreenHeight() / FLUID_GLOW_BFR_RATIO;}

		// Vertex declaration.
		virtual void				setVertexFormat                    ( u32 _vformat );

		virtual String8				getRenderName(   )				{ return "OpenGLES2"; }

		bbool						createDevice( bbool _fullscreen, void* _hwnd);
		void*						getRenderingDevice()    { return NULL; }
		void						displayDebugInfoGLES2(DrawCallContext &_drawCallCtx);

		// ScreenShot.
		void						startScreenshotMode() override;
		void						endScreenshotMode() override;
		u32		                    m_preUHDScreenWidth, m_preUHDScreenHeight;
		bbool						m_previousDisableRendering2d;
		u8*							m_pScreenShotBuffer;
		u32		                    m_ScreenShotWidth, m_ScreenShotHeight;

		Texture*					m_screenShotAsTexture;
		bbool						m_screenShotReady;
        bbool                       m_screenShotValid;
		Texture*					getScreenShotTexture() { return m_screenShotAsTexture; }
		bbool						isScreenShotTextureAvailable() { return m_screenShotReady; }
		bbool						isScreenShotTextureValid() { return m_screenShotValid; }
		void 						invalidateScreenShotTexture() { m_screenShotValid = bfalse; }

		// benchMark.
		void						doBenchMark();
		u32							nbBenchMarkPass;
		f64							startTimeBenchMark;
		f64							curTimeBenchMark;
		bbool						benchEnable;
		bbool						isBenchOver() { return nbBenchMarkPass > 10 ? btrue : bfalse; }
		bbool						isBenchEnable() { return benchEnable; }
		f64							getBenchTime() { return curTimeBenchMark;}
		void						setBenchEnable(bbool _enable) { benchEnable = _enable; }

#ifdef ITF_SUPPORT_DBGPRIM_TEXT
		GFX_BitmapFont*				GFX_gpDebugTextManager;
#endif

#ifdef ITF_WINDOWS
		bbool						createGLES2DeviceWin32(   );

        // opengl.
        EGLDisplay					m_EGLDisplay;
        EGLSurface					m_EGLWindow;
        EGLContext					m_EGLContext;
        EGLConfig					m_EGLConfig;
        EGLNativeDisplayType		m_NDT;
        EGLNativePixmapType			m_NPT;
        EGLNativeWindowType			m_NWT;
        EGLint						m_MajorVersion, m_MinorVersion;
        EGLint						m_iConfig;
            
#else
        GLint						m_MajorVersion, m_MinorVersion;
        GLint						m_iConfig;
#endif


		void*						m_hwnd;

            
#ifndef ITF_FINAL
		ITF_INLINE f64              getVBLSyncTime() const      { return mf64_VBLSyncTime; }
		ITF_INLINE void             setVBLSyncTime(f64 _value)  { mf64_VBLSyncTime = _value; }
		ITF_INLINE f32              getfPsNoVsync() const       { return (f32)m_fPsNoVsync; }
		ITF_INLINE void             setfPsNoVsync(f32 _fps)     { m_fPsNoVsync = _fps; }
	private:
		f64             mf64_VBLSyncTime;
		f64             m_fPsNoVsync;
#endif // ITF_FINAL

	public:

		//Movie    
		virtual void                    drawMovie               ( PrimitiveContext &_primCtx, Matrix44* _matrix,f32 _alpha,ITF_VertexBuffer* _pVertexBuffer,Texture** _pTextureArray,u32 _countTexture) {/*ITF_ASSERT_MSG(0,__FUNCTION__ " not implemented");*/}

		/// VertexBuffer.
		virtual void                    createVertexBuffer      (   ITF_VertexBuffer* /*_vertexBuffer*/ );
		virtual void                    releaseVertexBuffer     (   ITF_VertexBuffer* /*_vertexBuffer*/ );
		virtual void                    LockVertexBuffer        (   ITF_VertexBuffer* /*_vertexBuffer*/, void** /*_data*/, u32 /*_offset*/, u32 /*_size*/, u32 /*_flag*/ );
		virtual void                    UnlockVertexBuffer      (   ITF_VertexBuffer* /*_vertexBuffer*/ );
		virtual void                    UnlockVertexBuffer      (   ITF_VertexBuffer* _vb, u32 _offset, u32 _size);

		/// IndexBuffer.
		virtual void                    createIndexBuffer       (   ITF_IndexBuffer* /*_indexBuffer*/ );
		virtual void                    releaseIndexBuffer      (   ITF_IndexBuffer* /*_indexBuffer*/ );
		virtual void                    LockIndexBuffer         (   ITF_IndexBuffer* /*_indexBuffer*/, void** /*_data*/ );
		virtual void                    UnlockIndexBuffer       (   ITF_IndexBuffer* /*_indexBuffer*/ );
   
 		void							createVertexBufferGL    (   ITF_VertexBuffer* _vertexBuffer );
		void							createIndexBufferGL     (   ITF_IndexBuffer* _vertexBuffer );
        void                            unlockVertexBufferGL    (   ITF_VertexBuffer*  );
        void                            unlockIndexBufferGL     (   ITF_IndexBuffer*  );
        void                            releaseIndexBufferGL    (   ITF_IndexBufferStruct &_indexBuffer );
        void                            releaseVertexBufferGL   (   ITF_VertexBufferStruct &_vertexBuffer );
            
		/// Vertex/Index Buffer Draw
				bool                    beginDrawCall(DrawCallContext &_drawCallCtx);

				void                    SetDrawVertexBufferCommand(PRIMITIVETYPE _type, u32 _vertexStart, u32 _vertexNumber);
				void                    SetDrawIndexedVertexBufferCommand(PRIMITIVETYPE _type, u32 _indexNumber, u32 _startVertex = 0);
				// DrawPrimitive has been overloaded from the GFXAdapter Common, so SetDrawPrimitiveCommand should not be called
				// But it is requested to be declared in order to compile.
				void                    SetDrawPrimitiveCommand(PRIMITIVETYPE _type, const void * _p_Vertex, u32 _NumberVertex) { /*ITF_ASSERT(0);*/ } // should not be called
				void                    DrawPrimitive(DrawCallContext &_drawCallCtx, PRIMITIVETYPE _type, const void * _p_Vertex, u32 _NumberVertex);
				void                    SetDrawInstancedVertexBufferCommand(PRIMITIVETYPE _type, u32 _indexNumber, u32 _startVertex = 0);

	public:

#ifndef ITF_CONSOLE_FINAL
		virtual void                    reloadShaders();
#endif

		// New texture Target.   
		virtual RenderTarget*           createRenderTarget      (   RenderTargetInfo &_info );
		virtual void					setRenderContext		(   RenderContext &_context );
		virtual void					prepareLockableRTAccess (	RenderTarget* _target);
		virtual void					lockRenderTarget        (	RenderTarget* _target, LOCKED_TEXTURE * _lockTex);
		virtual void					unlockRenderTarget      (	RenderTarget* _target);

		void                DrawAFXPostProcess(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess);
		void                PrepareBlurTarget(RenderPassContext & _rdrCtxt, f32 _pixelBlurSize, u32 _quality);
		void                AFXpostProcessPass(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess);
		void                AFXPostProcessKaleiPass(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess);
		void                fillHexagoneVertexBuffer(VertexPCT* _data, u32 _startIndex, Vec2d _position, f32 _hexagoneSize, const Vec2d& _uv1, const Vec2d& _uv2, const Vec2d& _uv3 );
		void                blurFluidRenderBuffer(RenderPassContext & _rdrCtxt);
		void                blurFrontLightRenderBuffer(RenderPassContext & _rdrCtxt, u32 _quality, u32 _size);
		void                blurBackLightRenderBuffer(RenderPassContext & _rdrCtxt, u32 _quality, u32 _size);
		void                restoreBackLightRenderBuffer(RenderPassContext & _rdrCtxt, eLightTexBuffer _currentLightBufferType);

		void                BlurRenderBuffer(RenderPassContext & _rdrCtxt, u32 _pixelBlurSize, u32 _quality, u32 _src, u32 _dst, u32 _tmp);

		virtual void        setGammaRamp(Vec3d gamma,f32 brigthnessValue,f32 contrastValue) {/*ITF_ASSERT_MSG(0,__FUNCTION__ " not implemented");*/}

		// afterfx options:
		virtual void        setRenderTargetForPass(GFX_ZLIST_PASS_TYPE _passType, bbool _restoreContext = bfalse);
		virtual void        resolveRenderTargetForPass(GFX_ZLIST_PASS_TYPE _passType);

#ifndef ITF_FINAL
		virtual void        copyDebugRenderTarget(f32 _alpha = 0.f, const Vec2d & _pixelOffset = Vec2d::Zero);
		virtual void        copyPassToDebugRenderTarget(GFX_ZLIST_PASS_TYPE _passType);

		virtual void        startRasterForPass(GFX_ZLIST_PASS_TYPE _passType);
		virtual void        endRasterForPass(GFX_ZLIST_PASS_TYPE _passType);
#endif // ITF_FINAL

		void                beginGPUEvent(const char* _name, GPU_MARKER _marker = GFX_GPUMARKER_Default);
		void                endGPUEvent(GPU_MARKER _marker = GFX_GPUMARKER_Default);

		virtual void        copyScene();
		void				mainBufferToBackBuffer();
		virtual void        drawScreenTransition(f32 _fadeVal);

		virtual void        doScreenCapture() {};
    
		virtual void        setBlendSeparateAlpha() {}

		virtual void        showOverDrawRecord(RenderPassContext &_rdrCtx) {/*ITF_ASSERT_MSG(0,__FUNCTION__ " not implemented");*/}

		virtual void        setAlphaRef(int /*_ref*/ =1);
		virtual void        setAlphaTest(bbool _enabled);

		bbool				isForceWriteAlpha()	{ return m_usingForceWriteAplha; }
		void				seForceWriteAplha(bbool _usingForceWriteAplha)	{ m_usingForceWriteAplha = _usingForceWriteAplha; }

		void				setDXTHardwareDecompression(bbool _hardEnable)	{ useDXTHardDecompression = _hardEnable; }

		// Win32 only
		//void			    startScreenCapture(const Path *_directory) {}
		//void			    stopScreenCapture() {}

		virtual void*       compilePixelShader(const char* _fxFileName, const char* _functionName) {return NULL;}
		virtual void*       compileVertexShader(const char* _fxFileName, const char* _functionName) {return NULL;}

		virtual bbool       isSupportingZBuffer() const { return btrue; }

    public:
        void GL_enableScissorTest();
        void GL_disableScissorTest();
        void GL_activeTexture( u32 _text );
        
    protected:
        int m_GL_scissorTestEnabled;
        u32 m_GL_currentActiveTexture;
        u32 m_GL_VP_X;
        u32 m_GL_VP_Y;
        u32 m_GL_VP_W;
        u32 m_GL_VP_H;
	private:

		#define MAX_AFTER_FX_FULL_SCREEN    1
		#define MAX_AFTER_FX_QUART          3

		enum eTexBfr
		{
			// MEM1
			TEX_BFR_MAIN,

			TEX_BFR_FIRST_FS,
			TEX_BFR_LAST_FS     = TEX_BFR_FIRST_FS + MAX_AFTER_FX_FULL_SCREEN - 1,

			TEX_BFR_FIRST_QS,
			TEX_BFR_LAST_QS     = TEX_BFR_FIRST_QS + MAX_AFTER_FX_QUART - 1,

			TEX_BFR_FRONT_LIGHT,
			TEX_BFR_BACK_LIGHT,
			TEX_BFR_QRT_FRONT_LIGHT,
			TEX_BFR_QRT_BACK_LIGHT,
			TEX_BFR_QRT_TMP_LIGHT,

			TEX_BFR_FLUID_BLUR,
			TEX_BFR_FLUID_2,

			// MEM2
			TEX_BFR_SCENE_COPY,
#ifndef ITF_FINAL
			TEX_BFR_DBG_COPY,
#endif // ITF_FINAL

			//
			NB_TEX_BFR
		};
		static const u32 TEX_BFR_REFRACTION = TEX_BFR_LAST_QS;
		static const u32 TEX_BFR_REFLECTION = TEX_BFR_LAST_QS;

		enum eTexZBfr
		{
			// MEM1
			TEX_ZBFR_MAIN,
			TEX_ZBFR_REFLECTION,

			NB_TEX_ZBFR
		};

	public :
		
		PlatformTexture * getRefractionBuffer()     { ITF_ASSERT(m_currentTextureBuffer[TEX_BFR_REFRACTION]); return m_currentTextureBuffer[TEX_BFR_REFRACTION]->getTextureSamp(); }
		PlatformTexture * getReflectionBuffer()     { ITF_ASSERT(m_currentTextureBuffer[TEX_BFR_REFLECTION]); return m_currentTextureBuffer[TEX_BFR_REFLECTION]->getTextureSamp(); }
		PlatformTexture * getFluidTmpBuffer()       { ITF_ASSERT(m_currentTextureBuffer[TEX_BFR_FLUID_2]); return m_currentTextureBuffer[TEX_BFR_FLUID_2]->getTextureSamp(); }
		PlatformTexture * getFluidBlurBuffer()      { ITF_ASSERT(m_currentTextureBuffer[TEX_BFR_FLUID_BLUR]); return m_currentTextureBuffer[TEX_BFR_FLUID_BLUR]->getTextureSamp(); }
		PlatformTexture * getSceneCopyBuffer()      { ITF_ASSERT(m_currentTextureBuffer[TEX_BFR_SCENE_COPY]); return m_currentTextureBuffer[TEX_BFR_SCENE_COPY]->getTextureSamp(); }

		BasePlatformTexture * getBackLightTexture(eLightTexBuffer _currentLightBufferType) const;
		BasePlatformTexture * getFrontLightTexture(eLightTexBuffer _currentLightBufferType) const;

		TextureBuffer * getFrontLightSurface(eLightTexBuffer _currentLightBufferType);
		TextureBuffer * getBackLightSurface(eLightTexBuffer _currentLightBufferType);

		void    SetVertexShaderConstantF(u32 location, const f32 *floats, u32 count);
		void    SetPixelShaderConstantF (u32 location, const f32 *floats, u32 count);

		GFX_Vector4	m_vertexShaderConstantGlobalCache[256];
		GFX_Vector4	m_pixelShaderConstantGlobalCache[256];

		// Platform specific.
		static GLenum PrimitiveToGLES2(PRIMITIVETYPE _v);
		static u32              VertexFormat_TO_StructSize(u32 _vformat);

		void    LockRendering();
		void    UnlockRendering();

		void    DisplayMessageBox(const String8& _title, const String8& _msg, u32 _type);

#ifndef ITF_FINAL
		void    RequestDisplayMessageBox(String8 *_title, String8 *_msg, u32 _type, volatile u32 *_res);
#endif // ITF_FINAL

#ifdef _ENABLE_DEBUGFONT_
		void    BeginDisplayMessage(f32 &_fontW, f32 &_fontH);
		f32     DisplayMessage(const char *_msg, f32 xpos, f32 ypos, f32 hScale);
		void    EndDisplayMessage();

		static bbool   ForceNoLockRendering;
#endif // _ENABLE_DEBUGFONT_

	public:

		// Platform specific.
		static const u32    LIGHTS_BFR_RATIO;
		static const u32    QRT_LIGHTS_BFR_RATIO;
		static const u32    REFLECTION_BFR_RATIO;
		static const u32    REFRACTION_BFR_RATIO;
		static const u32    FLUID_BLUR_BFR_RATIO;
		static const u32    FLUID_GLOW_BFR_RATIO;

		static u32          GX_CORE_ID;

		class RasterizeControl
		{
            public:
                RasterizeControl() { m_isDirty = true; m_frontFaceMode=GL_CW; m_cullMode=GL_NONE;}
            
                void setDefault()
                {
                    setFrontFaceMode(GL_CW);
                    setCullMode(GL_NONE);
                }
            
                void setFrontFaceMode( GLenum _frontFaceMode )
                {
                    if( m_frontFaceMode !=_frontFaceMode )
                    {
                        m_frontFaceMode = _frontFaceMode;
                        m_isDirty = true;
                    }
                }
        
                void setCullMode( GLenum _cullMode )
                {
                    if( m_cullMode !=_cullMode )
                    {
                        m_cullMode = _cullMode;
                        m_isDirty = true;
                    }
                }
            
                GLenum getFromFaceMode() { return m_frontFaceMode; }
                GLenum getCullMode() { return m_cullMode; }
            
                void sendRasterizeControl()
                {
                    if( m_isDirty )
                    {
                        glFrontFace(m_frontFaceMode);
                        if (!m_cullMode)
                            glDisable(GL_CULL_FACE);
                        else
                        {
                            glEnable(GL_CULL_FACE);
                            glCullFace(m_cullMode);
                        }
                        m_isDirty = false;
                    }
                }
            
            private:
                GLenum		m_frontFaceMode;
                GLenum		m_cullMode;
                bool        m_isDirty;
		};

		void CopySurface(RenderPassContext & _rdrCtxt, TextureBuffer* _dstTex, TextureBuffer* _srcTex, GFX_BLENDMODE _blend, bbool _invalidateDst, f32 _scale = 1.0f);
		void PrepareRenderTarget(TextureBuffer *_dstTex , DepthBuffer *_dstDepth);
		void RestoreRenderTarget();

		virtual void setIndexBuffer(ITF_IndexBuffer* _indexBuffer);
		virtual void setVertexBuffer(ITF_VertexBuffer* _vertexBuffer) { setVertexBuffer(_vertexBuffer, 0); }
	
	public:

		TextureBuffer*	getRenderTextureBuffer()  { return m_RenderTextureBuffer; }
		DepthBuffer*	getRenderDepthBuffer()    { return m_RenderDepthBuffer; }

	private:
		ITF_THREAD_CRITICAL_SECTION m_csRendering;

#ifdef ITF_SUPPORT_DIM
		ITF_THREAD_CRITICAL_SECTION m_csDimming;
		u32                         m_cptDimming;
		bbool                       m_ForceDimming;
#endif // ITF_SUPPORT_DIM

#ifdef VSYNC_USE_CB
		static volatile u32         mu32_SwapCount;
		static volatile u32         mu32_FlipCount;
		static ITF_THREAD_EVENT     me_FlipEvent;
		static void                 GX2_Flip_CB(GX2CallbackEvent event, void *userDataPtr);
#endif // VSYNC_USE_CB

		enum eOperation
		{
			GLES2_MEM_INVALIDATE_OP,
#ifndef ITF_FINAL
			GLES2_DISPLAY_MESSAGE_BOX,
#endif // ITF_FINAL
			GLES2_OP_COUNT
		};

		struct GLES2Operation
		{
			GLES2Operation(eOperation _op, volatile u32 *_res, void* _ptr1, void* _ptr2, u32 _p1, u32 _p2)
				: op(_op), res(_res), ptr1(_ptr1), ptr2(_ptr2), p1(_p1), p2(_p2) {}
			GLES2Operation(const GLES2Operation& _op)
				: op(_op.op), res(_op.res), ptr1(_op.ptr1), ptr2(_op.ptr2), p1(_op.p1), p2(_op.p2) {}

			eOperation      op;
			volatile u32    *res;

			void*           ptr1;
			void*           ptr2;

			u32             p1;
			u32             p2;
		};

		ITF_THREAD_CRITICAL_SECTION m_csGXOperation;

		void setVertexBuffer(ITF_VertexBuffer* _vertexBuffer, u32 _statIdx);
		void drawMeshElement(const ITF_Mesh& _mesh, ITF_VertexBuffer* _vertexBuffer, const ITF_MeshElement& _elem);

#ifdef _DISPLAY_DEBUGINFO_
		// Debug Infos
		void drawDebugInfos(RenderPassContext& _rdrCtx);
		#ifdef ITF_SUPPORT_DBGPRIM_ANY
		void clearDebugInfos();
		#endif // ITF_SUPPORT_DBGPRIM_ANY
#endif // _DISPLAY_DEBUGINFO_

		void            SetDefaultSettings();

	public:
		void            setRenderBuffers(TextureBuffer* _colorBuffer, DepthBuffer *_depthBuffer);
		RenderStates    m_RenderStates;


        void            setForeground();
        void            setBackground();

	private:
        void            createScreenShotBuffer();
        void            destroyScreenShotBuffer();

		void            createRenderBuffers();
		void            allocRenderBuffer(u32 _texID, u32 _mainW, u32 _mainH, bbool _ftv);
		void            linkRenderBuffer(u32 _dstID, u32 _srcID, u32 _mainW, u32 _mainH, u32 _remoteW, u32 _remoteH,/* GX2SurfaceFormat _format,*/ bbool _offseted = bfalse);
		void            linkZBuffer(u32 _mainW, u32 _mainH, u32 _remoteW, u32 _remoteH);

		void            releaseRenderBuffers(bbool _mem1only = bfalse);
 
		void            createDynamicRingVB();
		void            releaseDynamicRingVB();

		u32				m_curBufferSizeX;
		u32				m_curBufferSizeY;
		TextureBuffer*	m_RenderTextureBuffer;
		DepthBuffer*	m_RenderDepthBuffer;

		TextureBuffer*	m_currentTextureBuffer[NB_TEX_BFR];
		DepthBuffer*	m_currentDepthBuffer[NB_TEX_ZBFR];

		TextureBuffer   m_mainTextureBuffer[NB_TEX_BFR];
		DepthBuffer     m_mainDepthBuffer;
		DepthBuffer     m_QSDepthBuffer;

		TextureBuffer*	m_aftexFXTextureBlur;
		TextureBuffer*	m_copyCurrentTextureBuffer;

        bool			m_initCalled;
        bbool			m_usingForceWriteAplha;
		bbool			useDXTHardDecompression;
		u32				m_curProg;

        GFX_COLOR_MASK_FLAGS m_currentColorMask;
        
		GFX_Viewport    m_CurrentViewport; // Real viewport set
		GFX_Viewport    m_RegularViewport; // Viewport of Regular Pass

		ITF_VertexBuffer*   m_CurrentVertexBuffer;

		DepthStencilControl m_StencilControl;
		void SendDepthParams()
		{
			m_RenderStates.SetDepthStencilControl(m_StencilControl);
		}

		AlphaTest m_alphaTest;

		ITF_HASHMAP<u64, GLuint>	m_frameBuffersIds;
		ITF_INLINE ITF_HASHMAP<u64, GLuint>& GetFrameBufferHashTable()
		{ 
			return m_frameBuffersIds;
		}
		void clearFrameBufferHashTable();

		RasterizeControl   m_rasterizeControl;
    


		void updateCameraFrustumPlanes(Camera *_camera);

		//blendMode State.
		struct BlendState
		{
			BlendState()
			{
				op = GL_FUNC_ADD;
				src = GL_SRC_ALPHA;
				dst = GL_ONE_MINUS_SRC_ALPHA;
				enable = bfalse;
			}

			GLenum op;
			GLenum src;
			GLenum dst;
			bbool enable;
		};

        bbool       m_direct2BackBuffer; // true on low end devices
        
		BlendState m_blendColorState;
		BlendState m_blendAlphaState;

		void    flushSetTextures();
	public:
		void    invalidateSamplers();

	private:
#ifdef _ENABLE_DEBUGFONT_
		void    initFontDebug();
		void    endFontDebug();
#endif // _ENABLE_DEBUGFONT_

		// Shaders stuff
	public:

		struct GLES2VertexShader
		{
			GLuint vs;
		};
		struct GLES2PixelShader
		{
			GLuint ps;
		};

		enum GLES2_Uniform_Register_Binding_ProgramType
		{
			GLES2_Uniform_Register_Binding_ProgramType_Pixel = 0,
			GLES2_Uniform_Register_Binding_ProgramType_Vertex = 1,

			GLES2_Uniform_Register_Binding_ProgramType_Count
		};

		struct GLES2_Uniform_Register_Binding
		{
			GLuint  loc;       // GLES2 handle
			u8      reg;       // DirectX register index used for binding
			u8      type;      // GRD_GLES2_Uniform_Register_Binding_ProgramType   
		};

		struct GLES2_Uniform_Register_Binding_Float4 : GLES2_Uniform_Register_Binding
		{
			u8      num;       // num of vector for vector types...
            GFX_Vector4 refData;
		};

		struct GLES2_Uniform_Register_Binding_Sampler : GLES2_Uniform_Register_Binding
		{

		};

		typedef u32 GLES2ProgramKey;
		class GLES2Program
		{
		public:
			GLES2Program(GLES2VertexShader *_vertexShader, GLES2PixelShader *_pixelShader);
			void parseAttrib();
			void parseUniform();
			void flushStates(GFXAdapter_OpenGLES2 *_adapter);
			void resetConstantCache();
            
			GLES2VertexShader *m_vertexShader;
			GLES2PixelShader *m_pixelShader;

			// OGL program index.
			GLuint m_programIndex;

			// Attrib indexes.
			GLuint m_positionHandle;
			GLuint m_colorHandle;
			GLuint m_blendHandle;
			GLuint m_UV0Handle;
			GLuint m_UV1Handle;
			GLuint m_UV2Handle;
			GLuint m_UV3Handle;

			// Constants
			ITF_VECTOR<GLES2_Uniform_Register_Binding_Float4> m_constantBindingTableVS;
			ITF_VECTOR<GLES2_Uniform_Register_Binding_Float4> m_constantBindingTablePS;
			// Samplers
			u16 m_usedPSTextureMask;
			ITF_VECTOR<GLES2_Uniform_Register_Binding_Sampler> m_samplerBindingTable;
		};
		ITF_INLINE GLES2ProgramKey getProgramKey(GLES2VertexShader *_vertexShader, GLES2PixelShader *_pixelShader)
		{
			return _vertexShader->vs | (_pixelShader->ps << 16);
		}
		GLES2Program *getProgramFromShaders(GLES2VertexShader *_vertexShader, GLES2PixelShader *_pixelShader);
		void                    releasePrograms();
        void                    resetShaderConstantCache();
        
		virtual bbool           loadShader( ITF_shader *_shaderGroup );
		virtual void            releaseShader( ITF_shader *_shaderGroup );

		virtual void*           loadBinVShader(u8* _binShader, ux _binSize);
		virtual void*           loadBinPShader(u8* _binShader, ux _binSize);
#if defined ITF_IOS || defined ITF_ANDROID
		virtual Path            getShaderCompiledPath( ) {return Path(mp_shaderManager.getShaderPath() + String8("compiled/ios/"));}
#else
		virtual Path            getShaderCompiledPath( ) {return Path(mp_shaderManager.getShaderPath() + String8("compiled/ogl2es/"));}
#endif
		virtual void            setVertexShader(void *_shaderBin);
		virtual void            setPixelShader(void *_shaderBin);

		virtual void            setShaderRepartition( f32 _psPercentage);
	
	private:

		// Shader attributes
		enum shAttrIdx
		{
			 shAI_vPos = 0
			,shAI_vNrm
			,shAI_fColor
			,shAI_vBlendW
			,shAI_vChnl
			,shAI_vTexCoord0
			,shAI_uv2
			,shAI_uv3
			,shAI_uv4
			,shAI_uv5
			,shAI_vTgt

			,shAI_Max
		};
 
		struct shAttr
		{
			GLint size;
			GLenum type;
			GLboolean normalized;
			GLsizei stride;
			const GLvoid* ptr;

			inline void set(GLint _size, GLenum _type, GLboolean _normalized, GLsizei _stride, const GLvoid* _ptr)
			{
				size = _size;
				type = _type;
				normalized = _normalized;
				stride = _stride;
				ptr = _ptr;
			}
		};

		shAttr shAttrArray[shAI_Max];
		GLuint m_vertexBufferId;

	private:
		bbool   loadCoreShaders();

		bbool   loadVShaderFromMemory(GLES2VertexShader **_ppDst, const void *_pSrc, ux _binSize);
		bbool   loadPShaderFromMemory(GLES2PixelShader  **_ppDst, const void *_pSrc, ux _binSize);

		void    freeShaderVS(GLES2VertexShader *_pVS);
		void    freeShaderPS(GLES2PixelShader  *_pPS);

#if 0 //DEAC
	#ifdef CHECK_SHADERS_VALIDITY
		void    CheckUniformVar(const ShaderParameterDBDesc *_dbDesc, const GX2UniformVar *_var);
		bbool   CheckAttribVar(const GX2AttribVar *_var);
		void    CheckSamplerVar(const GX2SamplerVar *_var);
	#endif // CHECK_SHADERS_VALIDITY
#endif

#ifdef CHECK_RUNTIME_SHADER
		void            CheckRuntimeShader();
#endif // CHECK_RUNTIME_SHADER

		GLES2VertexShader *m_CurrentVertexShader;
		GLES2PixelShader  *m_CurrentPixelShader;

		ITF_HASHMAP<GLES2ProgramKey, GLES2Program*> m_programHash;
		GLES2Program	*m_lastProgram;
		u32	m_dirtyFlags;
    
		const GLES2VertexShader *m_cachedVertexShader;
		const GLES2PixelShader  *m_cachedPixelShader;

		void cleanSetShaderCache()
		{
			m_cachedVertexShader = nullptr;
			m_cachedPixelShader = nullptr;
		}

		f64  m_AFXTimer;
	};

	ITF_INLINE void GFXAdapter_OpenGLES2::SetVertexShaderConstantF(u32 location, const f32* floats, u32 count)
	{
		while ( count-- )
		{
			m_vertexShaderConstantGlobalCache[location++] = *(GFX_Vector4*)floats;
			floats += 4;
		}
		m_dirtyFlags |= GLES2_DirtyConstVS;
	}

	ITF_INLINE void GFXAdapter_OpenGLES2::SetPixelShaderConstantF(u32 location, const f32* floats, u32 count)
	{
		while ( count-- )
		{
			m_pixelShaderConstantGlobalCache[location++] = *(GFX_Vector4*)floats;
			floats += 4;
		}
		m_dirtyFlags |= GLES2_DirtyConstPS;
	}

	/*ITF_INLINE void GFXAdapter_OpenGLES2::SetProgramConstantF(u32 location, const f32* floats, u32 count)
	{
		ITF_ASSERT(count);
		glUniform1fv(location, count, floats);
	}*/

    #define GFX_ADAPTER_OPENGLES2         ((GFXAdapter_OpenGLES2*)GFXAdapter_OpenGLES2::getptr())

} // namespace ITF

//#endif // ITF_OPENGLES2

#endif // ITF_WINDOWS || ITF_IOS

#endif // _ITF_OPENGLES2_DRAWPRIM_H_
