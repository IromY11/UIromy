#include "precompiled_engine.h"

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_PREFETCH_FCT_H_
#include "engine/boot/PrefetchFct.h"
#endif //_ITF_PREFETCH_FCT_H_


namespace ITF
{
    char TextureCookedHeader::SIGNATURE[4] = { 'T', 'E', 'X', '\0' };

    void TextureCookedHeader::init()
    {
        version = Versioning::textureVer();
        memcpy(signature, SIGNATURE, sizeof(signature));
        rawDataStartOffset = sizeof(TextureCookedHeader);
        rawDataSize = 0;
        width = 0;
        height = 0;
        depth = 0;
        bpp = 0;
        type = TYPE_GENERIC;
		platformFlags = TEXTURE_PLATFORM_FLAG_ALLOW_AUTODEGRAD_SD;
        memorySize = 0;
        uncompressedSize = 0;
        nbOpaquePixels = 0;
        nbHolePixels = 0;
        crcTextureConfig = 0;
        wrapModeX = GFX_TEXADRESSMODE_WRAP;
        wrapModeY = GFX_TEXADRESSMODE_WRAP;
        remap = GFX_TEXREMAP_DEFAULT();
    }

//////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(Texture)

    bool PrefetchTexture( const Path& path, PrefetchFactory::Params& params )
    {
        //return ResourceManager::PrefetchResource( path, params, Resource::ResourceType_Texture );
        return bfalse;
    }

    Texture::Texture( const Path &path ) 
    : Resource(path)
    , m_adapterimplementationData(0)
    , m_datasizeX(0)
    , m_datasizeY(0)
    , m_texelScaling(1.f, 1.f)
    , m_pixFormat(PF_RGBA)
    , m_mipMapLevels(0)
    , m_sizeX(1)
    , m_sizeY(1)
#ifdef MODE_HRESTEXTURE_SUPPORTED
    , m_HResVersion(bfalse)
#endif //MODE_HRESTEXTURE_SUPPORTED
    , m_uvAtlas(0)
    , m_ratio(1.0f)
    , m_fileContent(NULL)
    , m_fileContentSize(0)
    , m_wrapModeX(GFX_TEXADRESSMODE_WRAP)
    , m_wrapModeY(GFX_TEXADRESSMODE_WRAP)
    , m_remap(GFX_TEXREMAP_DEFAULT())
    , m_opaqueRatio(0.5f)
    , m_holeRatio(0.25f)
	, m_platformFlags(0)
    {
#ifndef ITF_FINAL
        m_dbgNfo[0]=0;
#endif // ITF_FINAL
    }

    Texture::~Texture()
    {
#ifdef ITF_RESOURCE_REPLACE_AT_COOKING
        setReplacementResource(bfalse);
#endif
		delete(m_adapterimplementationData);
    }

    void    Texture::postCreate()
    {
         createAtlasIfExist();
    };

    void Texture::createAtlasIfExist()
    {
        m_uvAtlas = UVATLASMANAGER->getAtlas(getPath());
    }

    void Texture::flushPhysicalData()
    {
        GFX_ADAPTER->cleanupTexture(this);
        SF_DEL_ARRAY(m_fileContent);
    }

    bbool Texture::tryLoadFile()
    {
        ITF_ASSERT_MSG(!isPhysicallyLoaded(), "duplicate resource loading can lead to crashes");
        if (!isPhysicallyLoaded())
        {
            bbool res = GFX_ADAPTER->loadTexture(this, m_cookedPath);
            ITF_WARNING_CATEGORY(Graph,NULL, res, "Could not load texture : %s", getPath().toString8().cStr());
            return res;
        }

        return btrue;
    }


    bbool Texture::reloadFromHost(const Path& _filename)
    {
        m_cookedPath = _filename;
        bbool res = GFX_ADAPTER->loadTexture(this,m_cookedPath);
        ITF_WARNING_CATEGORY(Graph,NULL, res, "Could not load texture : %s", getPath().toString8().cStr());
        return res;
    }

    void Texture::setGenericParams(const TextureCookedHeader & _genericHeader)
    {
        ITF_ASSERT_CRASH(_genericHeader.version == Versioning::textureVer(), "Textures are not up to date");
        ux nbPixel = _genericHeader.getNbPixel();
        if (nbPixel)
        {
            m_opaqueRatio =  f32(_genericHeader.nbOpaquePixels) / f32(nbPixel);
            m_holeRatio =  f32(_genericHeader.nbHolePixels) / f32(nbPixel);
        }
        m_wrapModeX		= _genericHeader.wrapModeX;
        m_wrapModeY		= _genericHeader.wrapModeY;
        m_remap			= _genericHeader.remap;
		m_platformFlags = _genericHeader.platformFlags;
    }

#ifdef ITF_RESOURCE_REPLACE_AT_COOKING
void Texture::setReplacementResource(bbool _value)
{
    if(_value)
    {
        if(m_replacementResource == NULL)
        {
            if(Resource * defaultResource = RESOURCE_MANAGER->getDefaultResource(getType()).getResource())
            {
                // If the texture has an UV atlas we need to build a custom texture with the same UV atlas
                if(m_uvAtlas && defaultResource->isPhysicallyLoaded())
                {
                    Texture *newTexture =  static_cast<Texture *>(RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture));
                    // Initialize the texture, we reload the file, it could be better...
                    GFX_ADAPTER->loadTexture(newTexture, defaultResource->getCookedPath()); 
                    newTexture->setUvAtlas(getUVAtlas());
                    m_replacementResource = newTexture;
                }
                else        
                {
                    m_replacementResource = defaultResource;
                }
            }
        }
    }
    else
    {        
        if(m_replacementResource)
        {
            Texture *replacementTexture =  static_cast<Texture *>(m_replacementResource);
            if(replacementTexture->getUVAtlas())
            {                
                RESOURCE_MANAGER->releaseEmptyResource(m_replacementResource);
            }
            m_replacementResource = NULL;
        }
    }
}
#endif

#ifdef ITF_SUPPORT_COOKING

    void Texture::cancelDefault()              
    {
        /*
        hijackPhysicalLoaded(bfalse); // pretend unload() was called without changing physical users
        tryLoadFile();
        setAsShared(bfalse);
        */
    }

    void Texture::setUseDefault(Resource* _pDefaultResource)
    {
        /*
        setPhysicalLoaded(btrue);
        
        if (_pDefaultResource)
        {
            setAsShared(btrue);
            Texture* defaultTexture = static_cast<Texture*>(_pDefaultResource);

            m_adapterimplementationData = defaultTexture->m_adapterimplementationData;
            m_sizeX                     = defaultTexture->m_sizeX;
            m_sizeY                     = defaultTexture->m_sizeY;
            m_datasizeX                 = defaultTexture->m_datasizeX;
            m_datasizeY                 = defaultTexture->m_datasizeY;
        }
        */
    }

#endif //ITF_SUPPORT_COOKING

#define STRING_PIXFORMAT(__pix) case __pix:_pixFormat=#__pix;break;

#ifndef ITF_FINAL
    void Texture::getStringPixFormat(String8& _pixFormat) const
    {
        switch (m_pixFormat)
        {
            STRING_PIXFORMAT(PF_RGB)
            STRING_PIXFORMAT(PF_RGBA)
            STRING_PIXFORMAT(PF_DXT1) 
            STRING_PIXFORMAT(PF_DXT3)
            STRING_PIXFORMAT(PF_DXT5)
            STRING_PIXFORMAT(PF_RGB_NATIVE_CTR)
            STRING_PIXFORMAT(PF_RGBA_NATIVE_CTR)
            STRING_PIXFORMAT(PF_ETC1_CTR)
            STRING_PIXFORMAT(PF_ETC1_A4_CTR)
            STRING_PIXFORMAT(PF_CMPA)
            STRING_PIXFORMAT(PF_L8)
            STRING_PIXFORMAT(PF_LA8)
            STRING_PIXFORMAT(PF_A8)
			STRING_PIXFORMAT(PF_PVRTBPP4)
        default:
            _pixFormat= "unknow";
            break;
        };
    }

    void Texture::getCustomInfo( String8& _info ) const
    {   
        String8 textureFormat;
        getStringPixFormat(textureFormat);
        _info.setTextFormat("%dx%d %s %s", m_sizeX ,m_sizeY, m_dbgNfo,textureFormat.cStr());
    }
#endif //ITF_FINAL

RenderTarget::RenderTarget(RenderTargetInfo& _texnfo) : Texture(Path("RenderTarget"))
{
    m_path = Path();

    // set the resource as"loaded" and refcounted once
	newClient();
    loadResource();
    setState(Resource::State_Idle);
    m_isLoaded = btrue;

	m_flags = _texnfo.m_flags;

	m_sizeX = _texnfo.m_width;
	m_sizeY = _texnfo.m_heigth;
	m_pixFormat = _texnfo.m_format;
	m_mipMapLevels = _texnfo.m_mipLevel;
}

void RenderTarget::release()
{
	//RESOURCE_MANAGER->unloadResource(this);
	//RESOURCE_MANAGER->releaseResource(this);

    RESOURCE_MANAGER->releaseEmptyResource(this);
}

RenderTarget::~RenderTarget()
{
//	unloadResource(0.0f);
//	removeClient();
}

TextureDyn::TextureDyn()
    :Texture(Path("TextureDyn"))
{
    ITF_MemSet(m_adapterimplementationDataBuff,0,sizeof(m_adapterimplementationDataBuff));
#if defined (ITF_WINDOWS) || defined (ITF_X360)
    ITF_MemSet(m_surfaceLevelBuff,0,sizeof(m_surfaceLevelBuff));
    m_surfaceLevel = 0;
    m_texnfo.init();
#endif

    m_path = Path();
}

TextureDyn::~TextureDyn()
{
    destroyBuffers();
    Texture::flushPhysicalData();
}

void TextureDyn::flushPhysicalData()
{
}

TextureDyn * TextureDyn::createLoadedResource(TextureInfo& _texnfo)
{
    TextureDyn * texDyn = newAlloc(mId_Textures, TextureDyn());
    // set the resource as"loaded" and refcounted once
    texDyn->newClient();
    texDyn->loadResource();
    texDyn->setState(Resource::State_Idle);
    texDyn->m_isLoaded = btrue;
    texDyn->createBuffers(_texnfo);
    return texDyn;
}

void TextureDyn::createBuffers(TextureInfo& _texnfo)
{
#if defined (ITF_WINDOWS) || defined (ITF_X360)
    m_texnfo = _texnfo;
#endif
    if(_texnfo.m_CpuAccess)
    {
        for(u32 i = 0; i < GFX_BUFFERED_FRAME_COUNT; ++i)
        {
            GFX_ADAPTER->createTexture( this, _texnfo.m_width, _texnfo.m_heigth, 1, _texnfo.m_format, 1/*default value*/, _texnfo.m_CpuAccess );
            m_adapterimplementationDataBuff[i] = m_adapterimplementationData;
            #if defined (ITF_WINDOWS) || defined (ITF_X360)
            GFX_ADAPTER->getTextureDynSurfaceLevel(this);
            m_surfaceLevelBuff[i] = m_surfaceLevel;
            #endif
        }
    }
    else
    {
        GFX_ADAPTER->createTexture( this, _texnfo.m_width, _texnfo.m_heigth, 1, _texnfo.m_format, 3, _texnfo.m_CpuAccess );
        m_adapterimplementationDataBuff[0] = NULL;
        #if defined (ITF_WINDOWS) || defined (ITF_X360)
        GFX_ADAPTER->getTextureDynSurfaceLevel(this);
        #endif
    }
}

void TextureDyn::destroyBuffers()
{
    if(m_adapterimplementationDataBuff[0])
    {
        for(u32 i = 0; i < GFX_BUFFERED_FRAME_COUNT; ++i)
        {
            #if defined (ITF_WINDOWS) || defined (ITF_X360)
            m_surfaceLevel = m_surfaceLevelBuff[i];
            #endif
            m_adapterimplementationData = m_adapterimplementationDataBuff[i];
            GFX_ADAPTER->cleanupTexture(this);
            m_adapterimplementationDataBuff[i] = 0;
            #if defined (ITF_WINDOWS) || defined (ITF_X360)
            GFX_ADAPTER->releaseTextureDynSurfaceLevel(this);
            m_surfaceLevelBuff[i] = 0;
            #endif
        }
    }
    else
    {
        GFX_ADAPTER->cleanupTexture(this);
        #if defined (ITF_WINDOWS) || defined (ITF_X360)
        GFX_ADAPTER->releaseTextureDynSurfaceLevel(this);
        #endif
    }
}

u8* TextureDyn::Lock(i32* _ppitch)
{
    LOCKED_TEXTURE lt;
    GFX_ADAPTER->lockTexture( this, &lt );
    if(_ppitch)
        *_ppitch = lt.m_pitch;
    return (u8*)lt.mp_Bits;
}

void TextureDyn::Unlock()
{
    GFX_ADAPTER->unlockTexture( this );
}

#if defined (ITF_WINDOWS) || defined (ITF_X360)
void TextureDyn::OnResetDevice()
{
    createBuffers(m_texnfo);
}

void TextureDyn::OnLostDevice()
{
    destroyBuffers();
}
#endif
} // namespace ITF

