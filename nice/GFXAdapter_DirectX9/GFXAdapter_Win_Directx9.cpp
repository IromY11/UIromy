#include "precompiled_GFXAdapter_Directx9.h"

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_

#ifdef ITF_WINDOWS

#include <math.h>
#include <d3dx9.h>
#include <D3D9Types.h>
#include <algorithm>


#ifndef _ITF_SLOTALLOCATORMANAGER_H_
#include "core/memory/slotallocatorManager.h"
#endif //_ITF_SLOTALLOCATORMANAGER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_VIDEOCAPTURE_H_
#include "engine/display/videoCapture.h"
#endif //_ITF_VIDEOCAPTURE_H_

#ifndef _ITF_VIDEOADAPTER_H_
#include "engine/AdaptersInterfaces/VideoAdapter.h"
#endif //_ITF_VIDEOADAPTER_H_

#include "engine/video/videoHandle.h"

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#endif //WIN32

#ifndef ITF_CORE_UNICODE_TOOLS_H_
#include "core/UnicodeTools.h"
#endif // ITF_CORE_UNICODE_TOOLS_H_

namespace ITF
{

#define SF_RELEASE_NULL_CHECK(p) if (p) { u32 refLeft = (p)->Release(); ITF_ASSERT_CRASH(refLeft==0, "DX9 reference not null at release"); (p) = 0; }

//----------------------------------------------------------------------------//

#ifdef ALLOW_DIRECTXEX_USE
HRESULT CheckD3D9Ex( void )
{
    HRESULT hr = E_FAIL;
    HMODULE libHandle = NULL;

    // Manually load the d3d9.dll library.
    libHandle = LoadLibrary(L"d3d9.dll");

    if(libHandle != NULL)
    {
        // Define a function pointer to the Direct3DCreate9Ex function.
        typedef HRESULT (WINAPI *LPDIRECT3DCREATE9EX)( UINT, void **);

        // Obtain the address of the Direct3DCreate9Ex function. 
        LPDIRECT3DCREATE9EX Direct3DCreate9ExPtr = NULL;

        Direct3DCreate9ExPtr = (LPDIRECT3DCREATE9EX)GetProcAddress( libHandle, "Direct3DCreate9Ex" );

        if ( Direct3DCreate9ExPtr != NULL)
        {
            // Direct3DCreate9Ex is supported.
            hr = S_OK;
        }
        else
        {
            // Direct3DCreate9Ex is not supported on this
            // operating system.
            hr = ERROR_NOT_SUPPORTED;
        }

        // Free the library.
        FreeLibrary( libHandle );

    }

    return hr;
}
#endif

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::BuildPresentParamsWin32()
{
    //POPO CHECK
#ifdef ITF_WINDOWS
    ZeroMemory( &m_ppars, sizeof( m_ppars ) );

    m_ppars.Windowed                = !m_fullscreen;
    m_ppars.BackBufferFormat        = m_fullscreen ? D3DFMT_X8R8G8B8 : D3DFMT_A8R8G8B8;

#ifdef ITF_USE_REMOTEVIEW
    m_ppars.EnableAutoDepthStencil  = FALSE; // used by remote screen surfaces
#else
    m_ppars.EnableAutoDepthStencil  = TRUE;
#endif
    m_ppars.AutoDepthStencilFormat  = D3DFMT_D24S8;
    m_ppars.MultiSampleType         = D3DMULTISAMPLE_NONE;

    m_ppars.Flags                   = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER ;
    m_ppars.hDeviceWindow           = m_hwnd;

    /// synchro ??.
    if (m_waitVBL)
    {
        m_ppars.PresentationInterval   = D3DPRESENT_INTERVAL_ONE;
    }
    else
    {
        m_ppars.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    }

    if (m_fullscreen)
    {
        //scan the res
        bbool resolutionSupported = bfalse;
        m_displayModeIndex = 0;
        for( u32 mode = 0; mode < m_displayModeCount; mode++ )
        {
            if(m_useD3D9Ex)
            {
                if (m_displayModeEx[mode].Height == m_windowHeight && m_displayModeEx[mode].Width == m_windowWidth && (m_displayModeEx[mode].RefreshRate == 60 || m_displayModeEx[mode].RefreshRate == 59))
                {
                    resolutionSupported = btrue;
                    m_displayModeIndex = mode;
                    break;
                }
            }
            else
            {
                if (m_displayMode[mode].Height == m_windowHeight && m_displayMode[mode].Width == m_windowWidth && (m_displayModeEx[mode].RefreshRate == 60 || m_displayModeEx[mode].RefreshRate == 59))
                {
                    resolutionSupported = btrue;
                    m_displayModeIndex = mode;
                    break;
                }
            }
        }

        if (resolutionSupported == bfalse)
        {
            D3DFORMAT format = (m_useD3D9Ex)?m_displayModeEx[m_displayModeCount-1].Format:m_displayMode[m_displayModeCount-1].Format;
            if (m_displayModeCount && format!=0)
            {  
                if(m_useD3D9Ex)
                {
                    m_windowWidth   = m_displayModeEx[m_displayModeCount-1].Width;
                    m_windowHeight  = m_displayModeEx[m_displayModeCount-1].Height;
                }
                else
                {
                    m_windowWidth   = m_displayMode[m_displayModeCount-1].Width;
                    m_windowHeight  = m_displayMode[m_displayModeCount-1].Height;
                }
                m_displayModeIndex = m_displayModeCount-1;
            }
            else
            {
                SYSTEM_ADAPTER->messageBox("Error","Fullscreen mode not supported",MB_OK,ITF_IDYES);
            }
        }

        m_ppars.BackBufferWidth               =   getWindowWidth();
        m_ppars.BackBufferHeight              =   getWindowHeight();
        m_ppars.BackBufferCount               =   2;
        m_ppars.FullScreen_RefreshRateInHz    =   (m_useD3D9Ex)?m_displayModeEx[m_displayModeIndex].RefreshRate:m_displayMode[m_displayModeIndex].RefreshRate;
        m_ppars.SwapEffect = D3DSWAPEFFECT_FLIP;//D3DSWAPEFFECT_DISCARD;
    }
    else
    {
        /// multiple monitor.
        m_ppars.BackBufferWidth               =   getWindowWidth();
        m_ppars.BackBufferHeight              =   getWindowHeight();
        m_ppars.FullScreen_RefreshRateInHz    =   0;
        m_ppars.BackBufferCount               =   1;
        m_ppars.SwapEffect                    = D3DSWAPEFFECT_COPY;
    }

    m_screenWidth = getWindowWidth();
    m_screenHeight = getWindowHeight();

#endif //ITF_WINDOWS
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_Directx9::createDXDeviceWin32()
{
#ifdef ITF_WINDOWS

    // Create the D3D object, which is needed to create the D3DDevice.
#ifdef ALLOW_DIRECTXEX_USE
    HRESULT HRCheckD3DVersion = CheckD3D9Ex();
    m_useD3D9Ex = (HRCheckD3DVersion == S_OK);
    IDirect3D9Ex * pD3DEx = NULL;
    if(m_useD3D9Ex)
    {
        HRESULT hr;
        if(FAILED(hr = Direct3DCreate9Ex( D3D_SDK_VERSION, &pD3DEx )))
        {
            m_pd3dDevice = NULL;
            return bfalse;
        }
        m_pD3D = pD3DEx;

        D3DDISPLAYMODEFILTER displayModeFilter;
        displayModeFilter.Size = sizeof(D3DDISPLAYMODEFILTER);
        displayModeFilter.Format = D3DFMT_X8R8G8B8;
        displayModeFilter.ScanLineOrdering = D3DSCANLINEORDERING_UNKNOWN;
        
        m_displayModeCount = pD3DEx->GetAdapterModeCountEx(D3DADAPTER_DEFAULT, &displayModeFilter);
        m_displayModeEx = newAlloc(mId_GfxAdapter,D3DDISPLAYMODEEX[m_displayModeCount]);
        for( u32 mode = 0; mode < m_displayModeCount; mode++ )
        {
            m_displayModeEx[mode].Size = sizeof(D3DDISPLAYMODEEX);
            pD3DEx->EnumAdapterModesEx(D3DADAPTER_DEFAULT,&displayModeFilter, mode, &m_displayModeEx[mode]);
        }
    }
    else
#endif
    {
        if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
            return 0;
        m_displayModeCount = m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT,D3DFMT_A8R8G8B8);
        m_displayMode = newAlloc(mId_GfxAdapter,D3DDISPLAYMODE[m_displayModeCount]);
        for( u32 mode = 0; mode < m_displayModeCount; mode++ )
            m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT,D3DFMT_A8R8G8B8,mode,&m_displayMode[mode]);
    }

    BuildPresentParamsWin32();

    DWORD bevahiorFlags = D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED;

    HRESULT hr;
    u32 nbTry = 0;
    do 
    {
#ifdef ALLOW_DIRECTXEX_USE
        if(m_useD3D9Ex)
        {
            IDirect3DDevice9Ex * pDevice = NULL;
            hr = pD3DEx->CreateDeviceEx( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd, bevahiorFlags,&m_ppars, m_fullscreen?&m_displayModeEx[m_displayModeIndex]:NULL, &pDevice );
            m_pd3dDevice = pDevice;
        }
        else
#endif
        {
            hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd, bevahiorFlags,&m_ppars, &m_pd3dDevice );
        }
        if(FAILED(hr))
            Sleep(1000);
    }
    while (FAILED(hr) && (nbTry++ < 10));

    if( FAILED( hr ) )
    {
        String8 msg;
        msg.setTextFormat("Create DirectX device Failed 10 times!\n please copy this error code to jira : %d", hr);
        SYSTEM_ADAPTER->messageBox( "Create DirectX device Failed", msg, ITF_MSG_ICONERROR | ITF_MSG_OK, ITF_IDOK );
        return bfalse;
    }

    D3DCAPS9 caps;
    m_pd3dDevice->GetDeviceCaps(&caps);

    if (!(caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST))
    {
        SYSTEM_ADAPTER->messageBox( "Create DirectX device Failed", "Direct3D9Renderer: Hardware does not support D3DPRASTERCAPS_SCISSORTEST.  Unable to proceed.", ITF_MSG_ICONERROR | ITF_MSG_OK, ITF_IDOK );
        return bfalse;
    }

    m_maxTextureSize = std::min(caps.MaxTextureHeight, caps.MaxTextureWidth);

    m_supportNonSquareTex = !(caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY);

    m_supportNPOTTex = !(caps.TextureCaps & D3DPTEXTURECAPS_POW2) ||
                       (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL);

#ifdef ITF_USE_REMOTEVIEW
    createRemoteRenderTargetSurfaces();
#endif

 #endif //ITF_WINDOWS

    
    DXCreateSprite(&m_spriteInterface);

    return btrue;
}

#ifdef ITF_USE_REMOTEVIEW
void GFXAdapter_Directx9::createRemoteRenderTargetSurfaces()
{
    m_BackBufferScreen[e_engineView_MainScreen] = createTarget( this, getWindowWidth(), getWindowHeight(), Texture::PF_RGBA, 0);
    m_BackBufferScreen[e_engineView_RemoteScreen] = createTarget( this, getRemoteScreenWidth(), getRemoteScreenHeight(), Texture::PF_RGBA, 0);
    m_DepthStencilScreen[e_engineView_MainScreen] = createTarget( this, getWindowWidth(), getWindowHeight(), Texture::PF_D24S8, GFX_RenderingBuffer_DX9::e_noTextureBuffer);
    m_DepthStencilScreen[e_engineView_RemoteScreen] = createTarget( this, getRemoteScreenWidth(), getRemoteScreenHeight(), Texture::PF_D24S8, GFX_RenderingBuffer_DX9::e_noTextureBuffer);
}

void GFXAdapter_Directx9::releaseRemoteRenderTargetSurfaces()
{
    for(u32 i=0; i< e_engineView_Count;i++)
    {
        m_BackBufferScreen[i]->release();
		m_BackBufferScreen[i] = NULL;
        m_DepthStencilScreen[i]->release();
		m_DepthStencilScreen[i] = NULL;
    }

    for (u32 index = 0;index<maxRingSurface;index++)
    {
        D3D_NM_RELEASE(m_RingSurface[index]);
    }

}

#endif

//----------------------------------------------------------------------------//

bbool GFXAdapter_Directx9::isDeviceLost()
{
#ifdef ITF_WINDOWS
    if (!m_pd3dDevice) return btrue;

    HRESULT hr;

    bbool isLooping = btrue;
    while (isLooping)
    {
        hr = m_pd3dDevice->TestCooperativeLevel();

        if (hr == D3DERR_DEVICELOST)
        {
            Sleep(500);
            return btrue;
        }
        else if(hr == D3DERR_DEVICENOTRESET)
        {
            if (resetDevice())
                isLooping = bfalse;
        }
        else if(hr == S_OK)
        {
            isLooping = bfalse;
        }

        if(isLooping)
            Sleep(50);
    }
  
#endif //ITF_WINDOWS
    return bfalse;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::preD3DReset()
{
#ifdef ITF_WINDOWS

    for(u32 i=0; i < 16; i++)
    {
        SetTextureBind(i, NULL, true);
    }

    /// RT.
    if (m_BackBuffer)
    {
        m_BackBuffer->release();
        m_BackBuffer = NULL;
    }

#ifdef ITF_USE_REMOTEVIEW
    releaseRemoteRenderTargetSurfaces();
#else
    if (m_DepthStencil)
    {
        m_DepthStencil->release();
        m_DepthStencil = NULL;
    }
#endif

    releaseAdditionalRenderTarget();

	if ( m_RendereringBufferPicking )
	{
		m_RendereringBufferPicking->release();
		m_RendereringBufferPicking = NULL;
	}

    D3D_NM_RELEASE(m_pPickingOffScreenBufferSurf[0]);
    D3D_NM_RELEASE(m_pPickingOffScreenBufferSurf[1]);

    D3D_NM_RELEASE(m_debugBufferOffscreenSurf[0]);
    D3D_NM_RELEASE(m_debugBufferOffscreenSurf[1]);

    D3D_NM_RELEASE(m_spriteInterface);

#ifdef ITF_SUPPORT_VIDEOCAPTURE
    VideoCapture* videoCapture = VideoCapture::getInstance();
    if ( videoCapture && videoCapture->getCaptureBuffer())
        videoCapture->getCaptureBuffer()->release();
#endif //ITF_SUPPORT_VIDEOCAPTURE

    /// Vertex Buffers.
    VertexBufferManagerOnLostDevice();
    /// Index Buffer
    IndexBufferManagerOnLostDevice();

    if (VIDEO_ADAPTER)
    {
        u32 nhandle = VIDEO_ADAPTER->getNumberHandle();
        for (u32 i =0; i< nhandle ; i++)
        {
            videoHandle* ph = VIDEO_ADAPTER->getHandleAt(i);
            ph->OnLostDevice();
        }
    }

#ifdef ITF_SUPPORT_DBGPRIM_TEXT
    /// font debug.
    if( mp_FontDebug )
        mp_FontDebug->OnLostDevice();

    if( mp_FontDebug_Monospace )
        mp_FontDebug_Monospace->OnLostDevice();

    if( mp_FontDebug_Big )
        mp_FontDebug_Big->OnLostDevice();
#endif

    m_currentRenderingSurface = NULL;
    m_currentAFXSceneTexture = NULL;
#endif

}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::postD3DReset()
{
#ifdef ITF_WINDOWS
    /// backBuffer and mainTarget.
    getBackBuffer();

    DXCreateSprite(&m_spriteInterface);

#ifdef ITF_USE_REMOTEVIEW
    createRemoteRenderTargetSurfaces();
#endif

//    createAdditionalRenderTarget();

    /// Vertex Buffers.
    VertexBufferManagerOnResetDevice();

    /// Index Buffer.
    IndexBufferManagerOnResetDevice();

    /// Video
    if (VIDEO_ADAPTER)
    {
        u32 nhandle = VIDEO_ADAPTER->getNumberHandle();
        for (u32 i =0; i< nhandle ; i++)
        {
            videoHandle* ph = VIDEO_ADAPTER->getHandleAt(i);
            ph->OnResetDevice();
        }
    }

#ifdef ITF_SUPPORT_DBGPRIM_TEXT 
    HRESULT hr;
    /// font debug.
    if( mp_FontDebug )
        hr = mp_FontDebug->OnResetDevice();

    if( mp_FontDebug_Monospace )
        hr = mp_FontDebug_Monospace->OnResetDevice();

    if( mp_FontDebug_Big )
        hr = mp_FontDebug_Big->OnResetDevice();
#endif

#endif
}

//----------------------------------------------------------------------------//
#ifdef ITF_SUPPORT_DBGPRIM_TEXT
u32 GFXAdapter_Directx9::displayDebugText(const DBGText &_text)
{
#ifdef ITF_WINDOWS
    RECT localRect;

    if(_text.m_x > getWindowWidth())
        return 0;

    if(_text.m_y > getWindowHeight())
        return 0;
    
    localRect.left = (i32)(_text.m_x);
    localRect.right = (i32)(_text.m_x + _text.m_w);
    localRect.top= (i32)(_text.m_y);
    localRect.bottom= (i32)(_text.m_y + _text.m_h );

    if(localRect.right < 0)
        return 0;

    if(localRect.bottom < 0)
        return 0;

    const char* text = _text.m_text.cStr();
    DWORD alignment = fontAlignmentToDirectXFormat(_text.m_alignment);

    // Compute width & height
    RECT realRect = localRect;
    if (_text.m_useMonospaceFont)
        mp_FontDebug_Monospace->DrawTextA(m_spriteInterface, text, -1 ,&realRect, DT_CALCRECT | alignment, 0);
    else if (_text.m_useBigFont)
        mp_FontDebug_Big->DrawTextA(m_spriteInterface, text, -1 ,&realRect, DT_CALCRECT | alignment, 0);
    else
        mp_FontDebug->DrawTextA(m_spriteInterface, text, -1 ,&realRect, DT_CALCRECT | alignment, 0);

    if(_text.m_backgroundColor.getAlpha() > 0)
    {
        RECT backRect =  realRect;

        backRect.left -= 2;
        backRect.right += 2;

        u32 backColor =  ITFCOLOR_TO_ARGB(_text.m_backgroundColor.getAsU32());
        drawDBG2dBox(Vec2d(f32(backRect.left), f32(backRect.top)), f32(backRect.right - backRect.left), f32(backRect.bottom - backRect.top), backColor, backColor, backColor, backColor);
    }

    const u32 color = D3DCOLOR_ARGB(255, (u32)(_text.m_r*255.f), (u32)(_text.m_g*255.f), (u32)(_text.m_b*255.f));
    if (_text.m_useMonospaceFont)
        mp_FontDebug_Monospace->DrawTextA(m_spriteInterface, text, -1 ,&localRect, alignment, color);
    else if (_text.m_useBigFont)
        mp_FontDebug_Big->DrawTextA(m_spriteInterface, text, -1 ,&localRect, alignment, color);
    else
        mp_FontDebug->DrawTextA(m_spriteInterface, text, -1 ,&localRect, alignment, color);

    return realRect.bottom - realRect.top;
#endif
}
#endif // ITF_SUPPORT_DBGPRIM_TEXT

void GFXAdapter_Directx9::displayDebugInfoWin32(DrawCallContext &_drawCallCtx)
{
#ifdef ITF_WINDOWS

    init2DRender();

#ifdef ITF_SUPPORT_DBGPRIM_MISC
    u32 height = View::getCurrentView()->getViewport().m_height;
    for (u32 it = 0; it < m_DBGTextureQuads.size();)
    {
        DBGTextureQuad& t = m_DBGTextureQuads[it];
        for (int i = 0; i < 4; i++)
        {
            t.mVtx[i].m_pos.y() = height - t.mVtx[i].m_pos.y();
            t.mVtx[i].m_pos.z() = 0.f;
        }
        drawTriangle(_drawCallCtx, t.mVtx[0], t.mVtx[1], t.mVtx[2], t.t);
        drawTriangle(_drawCallCtx, t.mVtx[1], t.mVtx[2], t.mVtx[3], t.t);
        t.duration -= LOGICDT;
        if (t.duration <= 0.0f)
            m_DBGTextureQuads.eraseKeepOrder(it);
        else
            it++;
    }
#endif // ITF_SUPPORT_DBGPRIM_MISC

    /// Debug info.
    /// Show fps.
#if defined(ITF_SUPPORT_DBGPRIM_TEXT) && !defined(ITF_PREVIEW_EVENT)

    RECT rct;
    rct.left=10;
    rct.right=780;
    rct.top=10;
    rct.bottom=rct.top+20;

    m_spriteInterface->Begin(D3DXSPRITE_ALPHABLEND|D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);

#if !defined(ITF_DISABLE_DEBUGINFO) && !defined(ITF_FINAL)
    if (isShowDebugInfo() && !m_overDrawMode)
    {
        char fps[1024];
        
        u32 len = sprintf_s
            ( 
                fps,1024,
                "%0.02f fps (%0.02fms average loop dt, %0.02fms logical dt, %llu frame(s) dropped) %s", 
                getfPs(), 
                SINGLETONS.m_averageLoopDT * 1000,
                LOGICDT * 1000,
                SINGLETONS.getLostFrameCount(), 
                m_extraInfo.cStr()
            );
        mp_FontDebug->DrawTextA(m_spriteInterface, fps, -1 ,&rct, 0, COLOR_WHITE);
        u32 color = 0x80000000;
        GFX_ADAPTER->drawDBG2dBox( Vec2d(0.1f , (f32) rct.top), f32(len) * 6, 20, color, color, color, color);
        rct.top = rct.bottom;
        rct.bottom = rct.top + 20;
    }
#endif // ITF_DISABLE_DEBUGINFO

    if ( m_overDrawMode )
    {
        wchar_t ovderDrawStr[1024];
        swprintf_s( ovderDrawStr,1024, L"OverDraw : %0.02f", m_overDrawDepth );
        mp_FontDebug->DrawText(m_spriteInterface, ovderDrawStr, -1 ,&rct, 0, COLOR_BLACK);
        rct.left += 1;
        rct.top += 1;
        rct.right += 1;
        rct.bottom += 1;
        mp_FontDebug->DrawText(m_spriteInterface, ovderDrawStr, -1 ,&rct, 0, COLOR_RED);
        rct.left -= 1;
        rct.top -= 1;
        rct.right -= 1;
        rct.bottom -= 1;
        rct.top = rct.bottom;
        rct.bottom = rct.top + 20;
    }

    if (m_renderOccluderMap)
    {
        char ovderDrawStr[1024];
        sprintf_s( ovderDrawStr,1024, "DrawOccluder mode : %d", m_renderOccluderMap );
        mp_FontDebug->DrawTextA(m_spriteInterface, ovderDrawStr, -1 ,&rct, 0, COLOR_BLACK);
        rct.left += 1;
        rct.top += 1;
        rct.right += 1;
        rct.bottom += 1;
        mp_FontDebug->DrawTextA(m_spriteInterface, ovderDrawStr, -1 ,&rct, 0, COLOR_RED);
        rct.left -= 1;
        rct.top -= 1;
        rct.right -= 1;
        rct.bottom -= 1;
        rct.top = rct.bottom;
        rct.bottom = rct.top + 20;
    }

    for (ITF_VECTOR<DBGText>::iterator it = m_DBGTexts.begin(); it != m_DBGTexts.end(); ++it)
    {
        const DBGText& dbgText = *it;
        displayDebugText(dbgText);
    }
    m_DBGTexts.clear();

	u32 currentY = getWindowHeight() - 30;
    for (ITF_VECTOR<DBGText>::iterator it = m_DBGLeftStackedTexts.begin(); it != m_DBGLeftStackedTexts.end(); ++it)
    {
        DBGText& dbgText = *it;
        dbgText.m_y = (f32) currentY;
        dbgText.m_backgroundColor = Color(0.6f, 0.4f, 0.4f, 0.4f);
        currentY -= displayDebugText(dbgText);
    }
    m_DBGLeftStackedTexts.clear();

    currentY = getWindowHeight() - 30;
    for (ITF_VECTOR<DBGText>::iterator it = m_DBGRightStackedTexts.begin(); it != m_DBGRightStackedTexts.end(); ++it)
    {
        DBGText& dbgText = *it;
        dbgText.m_y = (f32) currentY;
        dbgText.m_backgroundColor = Color(0.6f, 0.4f, 0.4f, 0.4f);
        currentY -= displayDebugText(dbgText);
    }
    m_DBGRightStackedTexts.clear();

    m_spriteInterface->End();
#endif // ITF_SUPPORT_DBGPRIM_TEXT

    end2DRender();

#endif // ITF_WINDOWS
}




//--------------------------------------------------------------------------------
// D3D function Redefinition 
//--------------------------------------------------------------------------------




HRESULT GFXAdapter_Directx9::DXCreateTexture(u32 _Width, u32 _Height, u32 _level, u32 _usage, D3DFORMAT _d3dFormat, D3DPOOL _Pool, IDirect3DTexture9 **_ppTexture)
{
    HRESULT hr = D3DXCreateTexture(m_pd3dDevice, _Width, _Height, _level, _usage, _d3dFormat, _Pool, _ppTexture);
#ifdef USE_D3D_MEMORY_TRACKER
    const bbool isManaged = (_Pool != D3DPOOL_DEFAULT);
    m_D3DMemTracker.addRessource((uPtr)*_ppTexture, D3DMemTracker::MEMTRACE_TEXTURE, isManaged);
#endif
    return hr;
}

void GFXAdapter_Directx9::DXReleaseTexture(IDirect3DTexture9 **_pTexture)
{
    D3D_NM_RELEASE(*_pTexture);
}

HRESULT GFXAdapter_Directx9::DXCreateLinkTexture(IDirect3DTexture9 *_pSrcBaseTexture, IDirect3DTexture9 *_pLinkedTexture, u32 _Width, u32 _Height, u32 _level, u32 _usage, D3DFORMAT _d3dFormat, D3DPOOL _Pool, IDirect3DTexture9 **_ppTexture)
{
    HRESULT hr = D3DXCreateTexture(m_pd3dDevice, _Width, _Height, _level, _usage, _d3dFormat, _Pool, _ppTexture);
#ifdef USE_D3D_MEMORY_TRACKER
    const bbool isManaged = (_Pool != D3DPOOL_DEFAULT);
    m_D3DMemTracker.addRessource((uPtr)*_ppTexture, D3DMemTracker::MEMTRACE_TEXTURE, isManaged);
#endif
    return hr;
}

void GFXAdapter_Directx9::DXReleaseLinkTexture(IDirect3DTexture9 **_pTexture)
{
    D3D_NM_RELEASE(*_pTexture);
}

HRESULT GFXAdapter_Directx9::DXGetSurfaceLevel(IDirect3DTexture9* _pTexture, u32 _level, IDirect3DSurface9 **_ppSurfaceLevel)
{
    HRESULT hr = _pTexture->GetSurfaceLevel(_level, _ppSurfaceLevel);
#ifdef USE_D3D_MEMORY_TRACKER
    const bbool isManaged = bfalse;
    m_D3DMemTracker.addRessource((uPtr)*_ppSurfaceLevel, D3DMemTracker::MEMTRACE_SURFACE, isManaged);
#endif
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreateRenderTarget(u32 _Width, u32 _Height, D3DFORMAT _d3dFormat, D3DMULTISAMPLE_TYPE _MultiSample, u32 _MultisampleQuality, bbool _Lockable, IDirect3DSurface9 **_ppSurface, HANDLE *_pSharedHandle)
{
    HRESULT hr = m_pd3dDevice->CreateRenderTarget(_Width, _Height, _d3dFormat, _MultiSample, _MultisampleQuality, _Lockable, _ppSurface, _pSharedHandle);
#ifdef USE_D3D_MEMORY_TRACKER
    const bbool isManaged = bfalse;
    m_D3DMemTracker.addRessource((uPtr)*_ppSurface, D3DMemTracker::MEMTRACE_SURFACE, isManaged);
#endif
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreateDepthStencilSurface(u32 _Width, u32 _Height, D3DFORMAT _d3dFormat, D3DMULTISAMPLE_TYPE _MultiSample, u32 _MultisampleQuality, bbool _Discard, IDirect3DSurface9 **_ppSurface, HANDLE *_pSharedHandle)
{
    HRESULT hr = m_pd3dDevice->CreateDepthStencilSurface(_Width, _Height, _d3dFormat, _MultiSample, _MultisampleQuality, _Discard, _ppSurface, _pSharedHandle);
#ifdef USE_D3D_MEMORY_TRACKER
    const bbool isManaged = bfalse;
    m_D3DMemTracker.addRessource((uPtr)*_ppSurface, D3DMemTracker::MEMTRACE_SURFACE, isManaged);
#endif
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreateOffscreenPlainSurface(u32 _Width, u32 _Height, D3DFORMAT _d3dFormat, D3DPOOL _Pool, IDirect3DSurface9 **_ppSurface, HANDLE *_pSharedHandle)
{
    HRESULT hr = m_pd3dDevice->CreateOffscreenPlainSurface(_Width, _Height, _d3dFormat, _Pool, _ppSurface, _pSharedHandle);
#ifdef USE_D3D_MEMORY_TRACKER
    const bbool isManaged = (_Pool != D3DPOOL_DEFAULT);
    m_D3DMemTracker.addRessource((uPtr)*_ppSurface, D3DMemTracker::MEMTRACE_SURFACE, isManaged);
#endif
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreateVertexDeclaration(const D3DVERTEXELEMENT9 *_pVertexElements, IDirect3DVertexDeclaration9 **_ppDecl)
{
    HRESULT hr = m_pd3dDevice->CreateVertexDeclaration(_pVertexElements, _ppDecl);
#ifdef USE_D3D_MEMORY_TRACKER
    const bbool isManaged = btrue;
    m_D3DMemTracker.addRessource((uPtr)*_ppDecl, D3DMemTracker::MEMTRACE_VERTEXDECLARATION, isManaged);
#endif
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreateVertexShader(const DWORD *_pFunction, IDirect3DVertexShader9 **_ppShader)
{
    HRESULT hr = m_pd3dDevice->CreateVertexShader(_pFunction, _ppShader);
#ifdef USE_D3D_MEMORY_TRACKER
    const bbool isManaged = btrue;
    m_D3DMemTracker.addRessource((uPtr)*_ppShader, D3DMemTracker::MEMTRACE_SHADER, isManaged);
#endif
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreatePixelShader(const DWORD *_pFunction, IDirect3DPixelShader9 **_ppShader)
{
    HRESULT hr = m_pd3dDevice->CreatePixelShader(_pFunction, _ppShader);
#ifdef USE_D3D_MEMORY_TRACKER
    const bbool isManaged = btrue;
    m_D3DMemTracker.addRessource((uPtr)*_ppShader, D3DMemTracker::MEMTRACE_SHADER, isManaged);
#endif
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreateVertexBuffer(u32 _Lenght, u32 _usage, u32 _FVF, D3DPOOL _pool, IDirect3DVertexBuffer9 **_ppVertexBuffer, HANDLE *_pSharedHandle)
{
    HRESULT hr = m_pd3dDevice->CreateVertexBuffer(_Lenght, _usage, _FVF, _pool, _ppVertexBuffer, _pSharedHandle);
#ifdef USE_D3D_MEMORY_TRACKER
    const bbool isManaged = (_pool != D3DPOOL_DEFAULT);
    m_D3DMemTracker.addRessource((uPtr)*_ppVertexBuffer, D3DMemTracker::MEMTRACE_VERTEXBUFFER, isManaged);
#endif
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreateIndexBuffer(u32 _Lenght, u32 _usage, D3DFORMAT Format, D3DPOOL _pool, IDirect3DIndexBuffer9 **_ppIndexBuffer, HANDLE *_pSharedHandle)
{
    HRESULT hr = m_pd3dDevice->CreateIndexBuffer(_Lenght, _usage, Format, _pool, _ppIndexBuffer, _pSharedHandle);
#ifdef USE_D3D_MEMORY_TRACKER
    const bbool isManaged = (_pool != D3DPOOL_DEFAULT);
    m_D3DMemTracker.addRessource((uPtr)*_ppIndexBuffer, D3DMemTracker::MEMTRACE_INDEXBUFFER, isManaged);
#endif
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreateSprite(LPD3DXSPRITE * _spriteInterface)
{
    HRESULT hr = D3DXCreateSprite(m_pd3dDevice, _spriteInterface);
#ifdef USE_D3D_MEMORY_TRACKER
    const bbool isManaged = bfalse;
    m_D3DMemTracker.addRessource((uPtr)*_spriteInterface, D3DMemTracker::MEMTRACE_UNKNOW, isManaged);
#endif
    return hr;
}

#ifdef ITF_WINDOWS
void GFXAdapter_Directx9::getAvailableResolutions(vector<pair<u32, u32>>& _solutions)
{
#ifdef ALLOW_DIRECTXEX_USE
	if(m_useD3D9Ex)
	{
		for(u32 mode = 0; mode < m_displayModeCount; ++mode)
		{
			if (m_displayModeEx[mode].RefreshRate == 59 || m_displayModeEx[mode].RefreshRate == 60)
			{
				_solutions.push_back(make_pair(m_displayModeEx[mode].Width, m_displayModeEx[mode].Height));
			}
		}
	}
	else
#endif //ALLOW_DIRECTXEX_USE
	{
		for(u32 mode = 0; mode < m_displayModeCount; ++mode)
		{
			if (m_displayMode[mode].RefreshRate == 59 || m_displayMode[mode].RefreshRate == 60)
			{
				_solutions.push_back(make_pair(m_displayMode[mode].Width, m_displayMode[mode].Height));
			}
		}
	}

}
#endif //ITF_WINDOWS

} // namespace ITF
