#include "precompiled_GFXAdapter_Directx9.h"

#ifndef ITF_RENDERINGBUFFER_DX9_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_RenderingBuffer_Directx9.h"
#endif //ITF_RENDERINGBUFFER_DX9_H_

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_

namespace ITF
{
	PlatformTexture::PlatformTexture()
	{
		m_DXTexture = nullptr;
		m_flags = 0;
		m_binding = 0;
	}
	PlatformTexture::~PlatformTexture()
	{
		if ( m_binding )
			static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->unbindAllSampler(this);
        if(m_DXTexture)
        {
            if(m_flags & GFX_TEXFLAG_LINK)
                static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->DXReleaseLinkTexture(&m_DXTexture);
            else
                static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->DXReleaseTexture(&m_DXTexture);
        }
	}

    GFX_RenderingBuffer_DX9::GFX_RenderingBuffer_DX9()
        : m_width(0)
        , m_height(0)
        , m_pixFmt(D3DFMT_UNKNOWN)
        , m_flags(0)
#ifdef ITF_X360
        , m_EdramOffset(0)
#endif
    {
		m_DXSurface = nullptr;
		m_DXLockableSurface = nullptr;
    }

    GFX_RenderingBuffer_DX9::~GFX_RenderingBuffer_DX9()
    {
        if(m_DXLockableSurface)
            D3D_NM_RELEASE(m_DXLockableSurface);
        if(m_DXSurface)
            D3D_NM_RELEASE(m_DXSurface);
    }

    void GFX_RenderingBuffer_DX9::reset()
    {
        deleteBuffer();
        m_width = 0;
        m_height = 0;
        m_pixFmt = D3DFMT_UNKNOWN;
        m_flags = 0;
        m_DXSurface = NULL;
        m_DXTexture = NULL;
    #ifdef ITF_X360
        m_EdramOffset = 0;
    #endif
    }

    // Buffer allocation
    // _shiftSurf (X360 only) : reserve EDRAM zone next this buffer
    // _pSrcBaseTexture (X360 only) : for alias buffer, the buffer that contains the aliased buffer
    // _pLinkedTexture (X360 only) : for alias buffer, if not at baseTexture offset 0, alias texture are define just after _pLinkedTexture
    void GFX_RenderingBuffer_DX9::createBuffer(u32 _width, u32 _height, D3DFORMAT _format, u32 _flags, u32 _shiftSurfSize, GFX_RenderingBuffer_DX9 *_pSrcBaseTexture, GFX_RenderingBuffer_DX9 *_pLinkedTexture)
    {
        if (m_DXTexture || m_DXSurface)
            return;

        m_width = _width;
        m_height = _height;
        m_pixFmt = _format;
        m_flags = _flags;
        if (_shiftSurfSize != 0)
            m_flags |= e_linkedRT;
        bbool isDepthBuffer = (m_pixFmt == D3DFMT_D24S8 || m_pixFmt == D3DFMT_D24FS8);
        if(isDepthBuffer)
        {
        #ifdef _USE_ZBUFFER_FLOAT
            m_pixFmt = D3DFMT_D24FS8;
        #else
            m_pixFmt = D3DFMT_D24S8;
        #endif
        }

        HRESULT hr;
#ifdef ITF_WINDOWS
        if(!isDepthBuffer)
        {
            if(!(m_flags & e_noTextureBuffer) || !(m_flags & e_noSurfaceBuffer) )
            {
                hr = static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->DXCreateTexture(m_width, m_height, 1, D3DUSAGE_RENDERTARGET, m_pixFmt, D3DPOOL_DEFAULT, (LPDIRECT3DTEXTURE9*) &m_DXTexture);
                ITF_ASSERT_CRASH(hr == S_OK, "CreateTexture Failed : %d", (u32)hr);
            }

            if(!(m_flags & e_noSurfaceBuffer))
            {
                hr = static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->DXGetSurfaceLevel(m_DXTexture, 0, &m_DXSurface);
                ITF_ASSERT_CRASH(hr == S_OK, "CreateRenderTarget Failed : %d", (u32)hr);
            }
        }
        else
        {
            hr = static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->DXCreateDepthStencilSurface(m_width, m_height,
                m_pixFmt,
                D3DMULTISAMPLE_NONE,
                0,
                FALSE,
                &m_DXSurface                                                    
                );
        }
#else
        if(!(m_flags & e_noTextureBuffer))
        {
            IDirect3DTexture9* srcBaseTexture = (_pSrcBaseTexture != NULL)?_pSrcBaseTexture->getTexture():NULL;
            if(srcBaseTexture == NULL)
            {
                hr = static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->DXCreateTexture( m_width, m_height, 1, 0,
                    m_pixFmt, D3DPOOL_DEFAULT, &m_DXTexture);
            }
            else
            {
                IDirect3DTexture9* linkedTexture = (_pLinkedTexture != NULL)?_pLinkedTexture->getTexture():NULL;
                hr = static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->DXCreateLinkTexture( _pSrcBaseTexture->getTexture(), linkedTexture, m_width, m_height, 1, 0,
                    m_pixFmt, D3DPOOL_DEFAULT, &m_DXTexture);
            }

            ITF_ASSERT_CRASH(hr == S_OK, "CreateTexture Failed : %d", (u32)hr);
        }

        if(!(m_flags & e_noSurfaceBuffer))
        {
            D3DSURFACE_PARAMETERS SurfaceParameters;
            ITF_MemSet( &SurfaceParameters, 0, sizeof( D3DSURFACE_PARAMETERS ) );

            if(m_flags & e_linkedRT)
            {
                const u32 TileTargetSizeBytes = _shiftSurfSize;
                m_EdramOffset = TileTargetSizeBytes / GPU_EDRAM_TILE_SIZE;
                SurfaceParameters.Base = m_EdramOffset;
            }
            if(isDepthBuffer)
            {
                SurfaceParameters.HierarchicalZBase = 0;
            #ifdef _USE_ZBUFFER_FLOAT
                SurfaceParameters.HiZFunc = D3DHIZFUNC_GREATER_EQUAL;
            #else
                SurfaceParameters.HiZFunc = D3DHIZFUNC_DEFAULT;
            #endif
            }

            hr = static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->DXCreateRenderTarget( m_width, m_height, m_pixFmt,
                D3DMULTISAMPLE_NONE, 0, FALSE, &m_DXSurface,
                &SurfaceParameters );

            ITF_ASSERT_CRASH(hr == S_OK, "CreateRenderTarget Failed : %d", (u32)hr);
        }
#endif
    }

    void GFX_RenderingBuffer_DX9::deleteBuffer()
    {
        if(m_DXTexture)
        {
            if(m_flags & e_linkedRT)
                static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->DXReleaseLinkTexture(&m_DXTexture);
            else
                static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->DXReleaseTexture(&m_DXTexture);
        }
        if(m_DXLockableSurface)
            D3D_NM_RELEASE(m_DXLockableSurface);
        if(m_DXSurface)
            D3D_NM_RELEASE(m_DXSurface);
		m_DXTexture = NULL;
		m_DXSurface = NULL;
		m_DXLockableSurface = NULL;
    }

    void GFX_RenderingBuffer_DX9::setSurface(IDirect3DSurface9* _surf)
    {
        m_DXSurface = _surf;
        D3DSURFACE_DESC desc;
        m_DXSurface->GetDesc(&desc);
        m_width = desc.Width;
        m_height = desc.Height;
        m_pixFmt = desc.Format;
#ifdef USE_D3D_MEMORY_TRACKER
        const bbool isManaged = btrue;
        static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->getD3DMemTracker().addRessource((uPtr)m_DXSurface, D3DMemTracker::MEMTRACE_SURFACE, isManaged);
#endif
    }

}
