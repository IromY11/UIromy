#ifndef ITF_RENDERINGBUFFER_DX9_H_
#define ITF_RENDERINGBUFFER_DX9_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#include <d3dx9.h>

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

namespace ITF
{
	class PlatformTexture : public BasePlatformTexture
	{
	public:
		PlatformTexture();
		~PlatformTexture();
		IDirect3DTexture9 *m_DXTexture;
		u32	m_flags;
		u32 m_binding;
	};
	
	inline PlatformTexture *getPlatformTexture( Texture *_texture )
	{
		PlatformTexture *platformTex = (PlatformTexture *)_texture->m_adapterimplementationData;
		if ( !platformTex )
		{
			platformTex = newAlloc(mId_GfxAdapter, PlatformTexture);
			_texture->m_adapterimplementationData = platformTex;
		}
		return platformTex;
	}
	inline void removePlatformTexture( Texture *_texture )
	{
		PlatformTexture *platformTex = getPlatformTexture(_texture);
		delete(platformTex);
		_texture->m_adapterimplementationData = nullptr;
	}

    class GFX_RenderingBuffer_DX9 : public PlatformTexture
    {
		friend class GFXAdapter_Directx9;
    public: 

       enum GFX_RenderingBuffer_DX9_Flag
        {
            e_noSurfaceBuffer   = 1,
            e_noTextureBuffer   = 2,
            e_linkedRT          = 4,
        };


        GFX_RenderingBuffer_DX9();
        ~GFX_RenderingBuffer_DX9();

        u32						getWidth()      { return m_width; }
        u32						getHeight()     { return m_height; }
        u32						getFlags()      { return m_flags; }
        D3DFORMAT				getFormat()     { return m_pixFmt; }
        IDirect3DTexture9*		getTexture()    { return m_DXTexture; }
        IDirect3DSurface9*		getSurface()    { return m_DXSurface; }
        IDirect3DSurface9*		getLockableSurface()    { return m_DXLockableSurface; }

        void					setFlags(u32 _flags) { m_flags = _flags;}
        void					setSurface(IDirect3DSurface9* _surf);

        void					reset();
        void					createBuffer(u32 _width, u32 height, D3DFORMAT _format, u32 _flags, u32 _shiftSurfSize = 0, GFX_RenderingBuffer_DX9 *_pSrcBaseTexture = NULL, GFX_RenderingBuffer_DX9 *_pLinkedTexture = NULL);
        void					deleteBuffer();

    private:

        u32						m_width;
        u32						m_height;
        D3DFORMAT				m_pixFmt;
        u32						m_flags;

		LPDIRECT3DSURFACE9		m_DXSurface;
		LPDIRECT3DSURFACE9      m_DXLockableSurface;

    // only for Xbox360
    #ifdef ITF_X360
    public:
        u32                 getEdramOffset()    { return m_EdramOffset;}
    private:
        u32                 m_EdramOffset;
    #endif
    };

}

#endif // ITF_RENDERINGBUFFER_DX9_H_
