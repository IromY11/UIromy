#ifndef _ITF_TEXTURE_H_
#define _ITF_TEXTURE_H_

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/resource.h"
#endif //_ITF_RESOURCE_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef ITF_CORE_SWAPBYTE_H_
#include "core/SwapByte.h"
#endif // ITF_CORE_SWAPBYTE_H_


namespace ITF
{
    class UVAtlas;
	class RenderTarget;

	enum GFX_TEXTURE_FLAGS
	{
		GFX_TEXFLAG_LINK = 0X1,
		GFX_TEXFLAG_BIND_AS_TARGET = 0x2,
	};

    enum GFX_TEXADRESSMODE
    {
        GFX_TEXADRESSMODE_WRAP = 0,
        GFX_TEXADRESSMODE_MIRROR,
        GFX_TEXADRESSMODE_CLAMP,
        GFX_TEXADRESSMODE_BORDER,
        GFX_TEXADRESSMODE_COUNT
    };

    enum GFX_TEXREMAP
    {
        GFX_TEXREMAP_FROM_A = 0,
        GFX_TEXREMAP_FROM_R,
        GFX_TEXREMAP_FROM_G,
        GFX_TEXREMAP_FROM_B,
        GFX_TEXREMAP_ZERO,
        GFX_TEXREMAP_ONE,
        GFX_TEXREMAP_COUNT,

        ENUM_FORCE_SIZE_32(GFX_TEXREMAP)
    };
    #define GFX_TEXREMAP_MODE(a,r,g,b)  ((a<<24)|(r<<16)|(g<<8)|(b<<0))
    #define GFX_TEXREMAP_DEFAULT()      GFX_TEXREMAP_MODE(GFX_TEXREMAP_FROM_A,GFX_TEXREMAP_FROM_R,GFX_TEXREMAP_FROM_G,GFX_TEXREMAP_FROM_B)
    #define GFX_TEXREMAP_GETA(m)        ((m>>24)&0xFF)
    #define GFX_TEXREMAP_GETR(m)        ((m>>16)&0xFF)
    #define GFX_TEXREMAP_GETG(m)        ((m>>8)&0xFF)
    #define GFX_TEXREMAP_GETB(m)        ((m>>0)&0xFF)
    #define GFX_TEXREMAP_SETA(m,a)      m=(m&0x00FFFFFF)|(a<<24)
    #define GFX_TEXREMAP_SETR(m,r)      m=(m&0xFF00FFFF)|(r<<16)
    #define GFX_TEXREMAP_SETG(m,g)      m=(m&0xFFFF00FF)|(g<<8)
    #define GFX_TEXREMAP_SETB(m,b)      m=(m&0xFFFFFF00)|(b<<0)

	enum TexturePlatformFlags
	{
		TEXTURE_PLATFORM_FLAG_NONE = 0,
		TEXTURE_PLATFORM_FLAG_NEEDMIPMAP = 1,
        TEXTURE_PLATFORM_FLAG_ALLOW_AUTODEGRAD_SD = 2
    };

    // Header for all platform in textures. Used for tools and to get additionnal information.
    // keep it as a pod (the constructor may not be called)
    // Some informations in this header can be duplicated in the platform-specific data.
    // In this case, platform specific data are the one to be trusted.
    // Can be kept forward compatible if new members are always added at the end (thus tools do not need to be updated).
    struct TextureCookedHeader
    {
        u32 version;
        char signature[4]; // 4CC "TEX\0"
        static char SIGNATURE[4];
        u32 rawDataStartOffset; // the offset from the header position to start of the raw data (ie. rawDataPtr = (u8 *)&header.version + rawDataStartOffset)
        u32 rawDataSize; // platform specific texture raw data size (usually rawDataSize = sizeOfFile - rawDataStartOffset)
        u16 width;  // should be at least 1. This is the original image size. Can differ if the TFI choose to start at a sub mipmap.
        u16 height; // 1 for 1D texture
        u16 depth; // 1 for 2D texture
        ux getNbPixel() const { ITF_ASSERT(width!=0 && height!= 0 && depth!= 0); return ux(width) * ux(height) * ux(depth); }
        u8 bpp; // bpp of original texture : RGB -> 24, RGBA -> 32
        enum Type
        {
            TYPE_GENERIC    = 0,                // nothing special
            TYPE_BACK_LIGHT = 1,                // a texture mainly used for back lighting
            TYPE_BACK_LIGHT_EMMISSIVE = 2,      // a texture mainly used for back lighting with an emissive part
            // cube map, light map, whatever..
        };
        u8 type; // enum Type
		u8 platformFlags; // texture platform flag (TexturePlatformFlags)
        u32 memorySize; // amount of memory the texture will take once loaded in GPU accessible RAM
        u32 uncompressedSize; // uncompressed size (0 for no compression format)
        u32 nbOpaquePixels; // ie. pixels with alpha = 1
        u32 nbHolePixels;   // ie. pixels with alpha = 0
        u32 crcTextureConfig; // crc computed from the texture config for the present platform
        u8  wrapModeX; // one of GFX_TEXADRESSMODE
        u8  wrapModeY;
        
        u32 remap;

        void init();

        ITF_BEGIN_SWAP_MEMBER_LIST
            ITF_SWAP_MEMBER(version);
            ITF_SWAP_MEMBER(rawDataStartOffset);
            ITF_SWAP_MEMBER(rawDataSize);
            ITF_SWAP_MEMBER(width);
            ITF_SWAP_MEMBER(height);
            ITF_SWAP_MEMBER(depth);
            ITF_SWAP_MEMBER(bpp);
            ITF_SWAP_MEMBER(type);
			ITF_SWAP_MEMBER(platformFlags);
            ITF_SWAP_MEMBER(memorySize);
            ITF_SWAP_MEMBER(uncompressedSize);
            ITF_SWAP_MEMBER(nbOpaquePixels);
            ITF_SWAP_MEMBER(nbHolePixels);
            ITF_SWAP_MEMBER(crcTextureConfig);
            ITF_SWAP_MEMBER(remap);
        ITF_END_SWAP_MEMBER_LIST
    };

	class BasePlatformTexture
	{
	public:
		virtual ~BasePlatformTexture() {}
	};

    class Texture : public Resource
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Texture,Resource,3846959819);
    public:
        enum PixFormat
        {
            //! Each pixel is 3 bytes. RGB in that order.
            PF_RGB,
            //! Each pixel is 4 bytes. RGBA in that order.
            PF_RGBA,

            PF_BC1,
            PF_BC2,
            PF_BC3,
            PF_DXT1 = PF_BC1,
            PF_DXT3 = PF_BC2,
            PF_DXT5 = PF_BC3,
            // BC4 and BC5 unused
            PF_BC6H,
            PF_BC7,

            PF_RGB_NATIVE_CTR,
            PF_RGBA_NATIVE_CTR,
            PF_ETC1_CTR,
            PF_ETC1_A4_CTR,
            
            PF_CMPA, // WII : DXT1 for RGB + DXT1 for Alpha
            PF_L8,
            PF_LA8,
            PF_A8,

			PF_RGBA_FLT16,
			PF_RGBA_FLT32,
			PF_RG_FLT16,
			PF_RG_FLT32,
			PF_R_FLT16,
			PF_R_FLT32,
			PF_PVRTBPP4,

			PF_D24S8,
			PF_D24FS8,

            ENUM_FORCE_SIZE_32(0)
        };

        BasePlatformTexture *m_adapterimplementationData;

        u32         m_datasizeX, m_datasizeY;
        Vec2d       m_texelScaling;
        PixFormat   m_pixFormat;
        u32         m_mipMapLevels;


        ResourceType getType() const { return ResourceType_Texture; }
        
        ITF_INLINE const UVAtlas* getUVAtlas() const { return m_uvAtlas; }
        void    createAtlasIfExist();
        void    setUvAtlas(const UVAtlas *_uvAtlas) {m_uvAtlas = _uvAtlas;}

        ITF_INLINE f32 getRatioXY() const { return m_ratio;}
        ITF_INLINE void setSize(u32 _sizeX, u32 _sizeY) 
        {
            m_sizeX = _sizeX;
            m_sizeY = _sizeY;
            if (_sizeY!=0)
                m_ratio = (f32) _sizeX / (f32)_sizeY;
            if (m_ratio==0.f)
                m_ratio = 1.f;
        }

        ITF_INLINE GFX_TEXADRESSMODE getUVAddressModeX() const { return GFX_TEXADRESSMODE(m_wrapModeX);}
        ITF_INLINE GFX_TEXADRESSMODE getUVAddressModeY() const { return GFX_TEXADRESSMODE(m_wrapModeY);}

		ITF_INLINE u8 getPlatformFlags() const {return m_platformFlags;}
        ITF_INLINE u32 getRemap() const {return m_remap;}

        ITF_INLINE u32 getSizeX() const {return m_sizeX;}
        ITF_INLINE u32 getSizeY() const {return m_sizeY;}
        ITF_INLINE PixFormat getPixFormat() const {return m_pixFormat;}

        ITF_INLINE f32 getHoleRatio() const { return m_holeRatio; } // ratio of pixels with alpha = 0
        ITF_INLINE f32 getOpaqueRatio() const { return m_opaqueRatio; } //  // ratio of pixels with alpha = 1

        virtual void    postCreate();
#ifdef ITF_SUPPORT_COOKING
        virtual bbool mustBeCooked() const { return btrue; }
        virtual void setUseDefault(Resource* _pDefaultResource);
        virtual void cancelDefault();
#endif //ITF_SUPPORT_COOKING


#ifdef  MODE_HRESTEXTURE_SUPPORTED
        bbool hasHResVersion() const { return m_HResVersion; }
#endif //MODE_HRESTEXTURE_SUPPORTED

        void setGenericParams(const TextureCookedHeader & _genericHeader);

#ifdef ITF_RESOURCE_REPLACE_AT_COOKING
        virtual void setReplacementResource(bbool _value);
#endif

    protected:
        f32         m_opaqueRatio; // ratio of pixels with alpha = 1. Use wisely as mipmaps gets different ratios
        f32         m_holeRatio;   // ratio of pixels with alpha = 0. Use wisely as mipmaps gets different ratios
        f32         m_ratio;
        const UVAtlas*    m_uvAtlas;
        u8*         m_fileContent;
        u32         m_fileContentSize;
        u8          m_wrapModeX;
        u8          m_wrapModeY;
		u8			m_platformFlags; // (TEXTURE_PLATFORM_FLAG)
        u32         m_remap;

        u32         m_sizeX, m_sizeY;

#ifdef MODE_HRESTEXTURE_SUPPORTED
        bbool      m_HResVersion;
#endif //MODE_HRESTEXTURE_SUPPORTED

        friend class ResourceLoader; // Don't add more friends here. You MUST use RESOURCE_MANAGER to instantiate a resource

        Texture(const Path &path);
        virtual ~Texture();

        virtual void flushPhysicalData();
        bbool   tryLoadFile();
        bbool   reloadFromHost(const Path& _filename);


#ifndef ITF_FINAL
        virtual void getCustomInfo( String8& _info ) const;
        void getStringPixFormat(String8& _pixFormat) const;
        public:
        char m_dbgNfo[8];
#endif //!ITF_FINAL
    };

    struct RenderTargetInfo
    {
		RenderTargetInfo() {};
        RenderTargetInfo(u32 _sizeX, u32 _sizeY, u32 _mipLevel, Texture::PixFormat _pixformat)
        {
            m_width = _sizeX;
            m_heigth = _sizeY;
			m_mipLevel = _mipLevel;
            m_format = _pixformat;
            m_flags = 0;
			m_EDRAMOffset = 0;
			m_srcBaseRT = NULL;
			m_linkedRT = NULL;
        }
        u32 m_width;
        u32 m_heigth;
		u32 m_mipLevel;
        Texture::PixFormat m_format;
        u32 m_flags;
		u32	m_EDRAMOffset;
		RenderTarget *m_srcBaseRT;
		RenderTarget *m_linkedRT;
    };
    /*
        Render target.
    */
    class RenderTarget : public Texture
    {
    public:
        RenderTarget(RenderTargetInfo& _texnfo);
		void release();
	protected:
        virtual ~RenderTarget();

    protected:
        u32 m_flags;
    };

    /*
        Dynamic texture.
    */
    class TextureDyn : public Texture
    {
        TextureDyn();
        virtual ~TextureDyn();

    public:
        struct TextureInfo
        {
            void init()
            {
                m_width = 0;
                m_heigth = 0;
                m_format = PF_RGBA;
                m_CpuAccess = btrue;
            }
            u32 m_width;
            u32 m_heigth;
            PixFormat m_format;
            bbool m_CpuAccess;
        };
        static TextureDyn * createLoadedResource(TextureInfo& _texnfo);

        void swap();
        u8 *Lock(i32* _ppitch=0);
        void Unlock();

#if defined (ITF_WINDOWS) || defined (ITF_X360)
        void OnResetDevice();
        void OnLostDevice();
#endif
        void destroyBuffers();

    protected:
        void createBuffers(TextureInfo& _texnfo);
        virtual void flushPhysicalData();

        BasePlatformTexture *m_adapterimplementationDataBuff[GFX_BUFFERED_FRAME_COUNT];
#if defined (ITF_WINDOWS) || defined (ITF_X360) || defined (ITF_DURANGO)
        void *m_surfaceLevelBuff[GFX_BUFFERED_FRAME_COUNT];
    public :
        void *m_surfaceLevel;
        TextureInfo m_texnfo;
#endif
    };

    inline void TextureDyn::swap()
    {
        #if (GFX_BUFFERED_FRAME_COUNT == 2) && !(defined(ITF_WINDOWS)|| defined(ITF_X360) || defined (ITF_DURANGO))
        if(m_adapterimplementationDataBuff[0])
            m_adapterimplementationData = m_adapterimplementationData == m_adapterimplementationDataBuff[0] ? m_adapterimplementationDataBuff[1] : m_adapterimplementationDataBuff[0];
        #else
        if(m_adapterimplementationDataBuff[0])
        {
            for(i32 i = 0; i < GFX_BUFFERED_FRAME_COUNT; ++i)
            {
                if(m_adapterimplementationDataBuff[i] == m_adapterimplementationData)
                {
                    m_adapterimplementationData = m_adapterimplementationDataBuff[(i+1)%GFX_BUFFERED_FRAME_COUNT];
                    #if defined (ITF_WINDOWS) || defined (ITF_X360) || defined (ITF_DURANGO)
                    m_surfaceLevel = m_surfaceLevelBuff[(i+1)%GFX_BUFFERED_FRAME_COUNT];
                    #endif
                    return;
                }
            }
            ITF_ASSERT_CRASH(0, "TextureDyn::swap()");
        }
        #endif
    }
} // namespace ITF

#endif // _ITF_TEXTURE_H_
