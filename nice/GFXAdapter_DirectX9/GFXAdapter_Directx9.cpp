#include "precompiled_GFXAdapter_Directx9.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef __MINILZO_H
#include "core/lzo/minilzo.h"
#endif //__MINILZO_H
 
#ifndef _ITF_VIDEOCAPTURE_H_
#include "engine/display/videoCapture.h"
#endif //_ITF_VIDEOCAPTURE_H_

#ifndef _ITF_SLOTALLOCATORMANAGER_H_
#include "core/memory/slotallocatorManager.h"
#endif //_ITF_SLOTALLOCATORMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifdef ITF_WINDOWS
#include "../../../extern/CaptureJpeg/Source/INTERFCE.h"
#include <shlobj.h>
#endif

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifdef ITF_WINDOWS
#ifndef _ITF_PROCESSSPAWNER_H_
#include "core/system/ProcessSpawner.h"
#endif //_ITF_PROCESSSPAWNER_H_
#endif // ITF_WINDOWS

#ifndef _ITF_STACKWALKER_H_
#include "core/StackWalker.h"
#endif //_ITF_STACKWALKER_H_

///////////////////////////////////////////////////////////////////////////////
///////////////////////       implementation of various shapes drawing
///////////////////////////////////////////////////////////////////////////////

#include <math.h>
#ifdef WIN32
    #include <windows.h>        // otherwise GL.h and GLU.h won't compile on PC
#endif // WIN32

#ifdef ITF_X360
#include <fxl.h>
#include <xgraphics.h>
#endif

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_
#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_
#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_CODECIMAGEADAPTER_H_
#include "engine/AdaptersInterfaces/CodecImageAdapter.h"
#endif //_ITF_CODECIMAGEADAPTER_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#include <d3dx9.h>
#include <algorithm>

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#ifndef _ITF_DDSFORMAT_H_
#include "engine/display/ddsformat.h"
#endif //_ITF_DDSFORMAT_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_RASTERSMANAGER_H_
#include "engine/rasters/RastersManager.h"
#endif //_ITF_RASTERSMANAGER_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifdef ITF_X360
#define USE_TEXTURE_TILE
#endif //ITF_X360

#ifdef CHECK_SHADERS_VALIDITY
#include "engine/display/ShaderParameterDBDesc.h"
#endif // CHECK_SHADERS_VALIDITY

#ifndef _ITF_VIDEOADAPTER_H_
#include "engine/AdaptersInterfaces/VideoAdapter.h"
#endif //_ITF_VIDEOADAPTER_H_

#include "engine/video/videoHandle.h"

// no guard -> should be included once only !
#include "engine/AdaptersInterfaces/GFXAdapter_Common_Impl.h"

#ifndef _ITF_SYSTEMADAPTER_X360_H_
#include "adapters/SystemAdapter_x360/SystemAdapter_x360.h"
#endif //_ITF_SYSTEMADAPTER_X360_H_

#ifndef ITF_CORE_UNICODE_TOOLS_H_
#include "core/UnicodeTools.h"
#endif // ITF_CORE_UNICODE_TOOLS_H_

#ifndef _ITF_OUTPUTDEBUGGER_H_
#include "core/OutputDebugger.h"
#endif //_ITF_OUTPUTDEBUGGER_H_

#include "engine/bloombergInterface/ITFBloombergInterface.h"

namespace ITF
{
    // explicit instantiation
    template class GFXAdapter_Common<GFXAdapter_Directx9>;

    DECLARE_RASTER(doScreenCapture, RasterGroup_Misc, Color::violet());
    DECLARE_RASTER(present, RasterGroup_Misc, Color::green());

#ifndef GFX_COMMON_POSTEFFECT
    const u32   GFXAdapter_Directx9::LIGHTS_BFR_RATIO     = 3;
    const u32   GFXAdapter_Directx9::QRT_LIGHTS_BFR_RATIO = LIGHTS_BFR_RATIO * 2;
    const u32   GFXAdapter_Directx9::REFLECTION_BFR_RATIO = 2;
    const u32   GFXAdapter_Directx9::REFRACTION_BFR_RATIO = 2;
    const u32   GFXAdapter_Directx9::FLUID_BLUR_BFR_RATIO = 2;
    const u32   GFXAdapter_Directx9::FLUID_GLOW_BFR_RATIO = 8;
#endif

#ifdef ITF_X360
    const u32   GFXAdapter_Directx9::c_shaderMemoryPoolSize = 3*1024*1024;
#endif

bbool GFXAdapter_Directx9::createDevice(bbool _fullscreen, void* _hwnd)
{
    bbool res = btrue;

    m_hwnd = (HWND)_hwnd;
    m_fullscreen = _fullscreen;

#ifdef ITF_WINDOWS
    res = createDXDeviceWin32();
#elif defined(ITF_X360)
    res = createDXDeviceX360();
#endif
    if(res)
    {
        /// Core Shaders.    
        loadCoreShaders();

        if (!createVertexDeclaration())
            return bfalse;

        slotAllocatorManager::enablePhysicalAllocation();
    }

    return res;
}

//----------------------------------------------------------------------------//

#ifdef _DEBUG
void printMatrix(const D3DXMATRIX& _matrix,char* name)
{
    LOG("%s:",name);
    for (int i=0;i<4;++i)
        LOG("( %0.08f %0.08f %0.08f %0.08f )",_matrix(i,0),_matrix(i,1),_matrix(i,2),_matrix(i,3));
}
#endif //_DEBUG

#if defined(ITF_USE_REMOTEVIEW) && !defined(ITF_FINAL)
u32 WINAPI DRC_Thread(void * user)
{
    static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->DRCThread();
    return 0;
}
#endif

#ifdef ITF_SUPPORT_DBGPRIM_TEXT

#ifdef ITF_WINDOWS

void preloadFont( ID3DXFont* _pFont)
{
    _pFont->PreloadCharacters('0', '9');
    _pFont->PreloadCharacters('A', 'Z');
    _pFont->PreloadCharacters('a', 'z');
    _pFont->PreloadGlyphs('0', '9');
    _pFont->PreloadGlyphs('A', 'Z');
    _pFont->PreloadGlyphs('a', 'z');
}
#else
void preloadFont( Fontdebug&)//do nothing right now
{}
#endif //ITF_WINDOWS

void GFXAdapter_Directx9::createDebugFonts(u32 _screenwidth, u32 _screenHeight)
{
#ifdef ITF_X360
    /// Create a font.
    Path fontPath("enginedata/console/x360/Arial_16.xpr");
    m_FontDebug.Create( fontPath,  m_pd3dDevice );
#else
    D3D_NM_RELEASE( mp_FontDebug );
    D3D_NM_RELEASE( mp_FontDebug_Monospace );
    D3D_NM_RELEASE( mp_FontDebug_Big );

    D3DXCreateFontA( m_pd3dDevice, getDebugFontSize(), 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                              "Arial", &mp_FontDebug );

    ITF_ASSERT(mp_FontDebug);
    preloadFont(mp_FontDebug);
    
    D3DXCreateFontA( m_pd3dDevice, getDebugFontSize(), 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        "Courier", &mp_FontDebug_Monospace );

    ITF_ASSERT(mp_FontDebug_Monospace);
    preloadFont(mp_FontDebug_Monospace);
    
    D3DXCreateFontA( m_pd3dDevice, getBigDebugFontSize(), 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                              "Arial", &mp_FontDebug_Big );

    ITF_ASSERT(mp_FontDebug_Big);
    preloadFont(mp_FontDebug_Big);
#endif
}
#endif

//----------------------------------------------------------------------------//

GFXAdapter_Directx9::GFXAdapter_Directx9()
{
    m_pD3D = 0;
    m_pd3dDevice = 0;
    m_useD3D9Ex = bfalse;

    isRender = 0;
    m_depthStencilFormat = D3DFMT_D16;

    m_BackBuffer = nullptr;
    m_backBufferNoTile = nullptr;

	m_currentDepthBuffer = NULL;
	for ( ux i = 0; i < 4; i++ )
	{
		m_currentRenderTarget[i] = NULL;
	}
    for(u32 i=0; i<NB_TEX_BFR; i++)
    {
        m_AdditionnalRenderingBuffer[i] = nullptr;
    }
    m_MediumZBuffer = nullptr;

    for(u32 i=0; i<NB_TEX_EXT_BFR; i++)
    {
        m_AdditionnalExternalRenderingBuffer[0][i] = nullptr;
        m_AdditionnalExternalRenderingBuffer[1][i] = nullptr;
    }


    m_DebugRenderingBuffer = nullptr;

    m_debugBufferOffscreenSurf[0] = nullptr;
    m_debugBufferOffscreenSurf[1] = nullptr;

    m_asyncScreenshotBuffer = nullptr;
    m_asyncScreenshotOffscreenSurf = nullptr;
   
    mp_VDcl_PCT = 0;
    mp_VDcl_PC2T = 0;
    mp_VDcl_PCTBIBW = 0;
    mp_VDcl_PNCTBIBW = 0;
    mp_VDcl_PC = 0;
    mp_VDcl_PT = 0;
    mp_VDcl_PCBT = 0;
    mp_VDcl_PCB2T = 0;
    mp_VDcl_PNCT = 0;
    mp_VDcl_PNC3T = 0;
    mp_VDcl_QI_2TC = 0;
    mp_VDcl_T = 0;
    mp_VDcl_I = 0;

    m_textureBinded.reserve(MAX_SAMPLER);
    
    for (u32 index = 0; index < MAX_SAMPLER; ++index)
    {
        m_textureBinded.push_back((PlatformTexture*)(0));
        m_textureBindedFiltering.push_back(btrue);
    }


#if defined( ITF_SUPPORT_DBGPRIM_TEXT) && defined(ITF_WINDOWS)
    mp_FontDebug = 0;
    mp_FontDebug_Big = 0;
    mp_FontDebug_Monospace = 0;
    m_spriteInterface = 0;
#endif

#ifdef ITF_WINDOWS
    m_previousDisableRendering2d = bfalse;
#ifdef ALLOW_DIRECTXEX_USE
    m_displayModeEx = NULL;
#endif
    m_displayMode = NULL;
#endif

    m_currentRenderingSurface = 0;

#ifdef ITF_USE_REMOTEVIEW
    for(u32 i=0; i< e_engineView_Count;i++)
    {
        m_BackBufferScreen[i] = nullptr;
        m_DepthStencilScreen[i] = nullptr;
    }
    m_currentEngineView = e_engineView_MainScreen;
    m_displayingEngineView = e_engineView_MainScreen;
    m_drcThread = NULL;
#else
    m_DepthStencil = nullptr;
#endif

    m_pPickingOffScreenBufferSurf[0] = NULL;
    m_pPickingOffScreenBufferSurf[1] = NULL;
    m_RendereringBufferPicking = nullptr;

    m_PickingBufferSwap = 0;
    m_debugBufferOffscreenSwap = 0;

    setUseDepthBuffer(btrue);

#ifdef ITF_X360
    Synchronize::createCriticalSection(&m_csPresent);
    m_askToReleaseOwnerShip = bfalse;
    Synchronize::createEvent(&m_ReleaseOwnerShipEvent,"releaseOwnership");
    Synchronize::createEvent(&m_AcquireOwnerShipEvent,"acquireOwnership");
#endif //ITF_X360

    m_overDrawRamptexture = NULL;

    mf_Gamma = 1.0f;
#ifdef ITF_X360
	mf_Bright = 0.07f;
#else
	mf_Bright = 0.0f;
#endif
	mf_CurrentBright = 0.0f;
	mf_CurrentGamma = 1.0f;

#ifndef ITF_FINAL
    m_dbgTxtAfterFX.clear();
#endif

    m_preUHDScreenWidth = 0;
    m_preUHDScreenHeight = 0;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::destroy()
{
	for ( ux i = 0; i < nbrBufOD; ++i )
		m_overDrawBuffers[i].deleteBuffer();

    DXReleaseTexture(&m_overDrawRamptexture);

#if defined( ITF_SUPPORT_DBGPRIM_TEXT) && defined(ITF_WINDOWS)
    D3D_NM_RELEASE( mp_FontDebug );
    D3D_NM_RELEASE( mp_FontDebug_Big );
    D3D_NM_RELEASE( mp_FontDebug_Monospace );
#endif // ITF_SUPPORT_DBGPRIM_TEXT && ITF_WINDOWS

    /// clean up vertex declaration.
#ifdef ITF_SUPPORT_RESET_DEVICE
    m_VertexBufferManager.destroyAllVertexBuffer();
    m_VertexBufferManager.destroyAllIndexBuffer();
#endif //ITF_SUPPORT_RESET_DEVICE

    /// clean up vertex declaration.
    releaseVertexDeclaration();

    /// cleanup direct 3d systems.
    SF_RELEASE(m_pD3D);
    SF_RELEASE(m_pd3dDevice);

#ifdef ITF_X360
    Synchronize::destroyCriticalSection(&m_csPresent);
    Synchronize::destroyEvent(&m_ReleaseOwnerShipEvent);
    Synchronize::destroyEvent(&m_AcquireOwnerShipEvent);
#else
    SF_DEL_ARRAY(m_displayMode);
#ifdef ALLOW_DIRECTXEX_USE
    SF_DEL_ARRAY(m_displayModeEx);
#endif
#endif //ITF_X360

#ifdef ITF_USE_REMOTEVIEW
    
    //wait the state
    if (m_drcThread)
    {
        m_quitDRCThread  = btrue;
        Synchronize::waitEvent(&m_RemoteExitEvent);
        Synchronize::destroyEvent(&m_RemoteExitEvent);
    }
    
    for (u32 index = 0;index<maxRingSurface;index++)
        D3D_NM_RELEASE(m_RingSurface[index]);

    Synchronize::destroyCriticalSection(&m_csDRC);
#endif //ITF_USE_REMOTEVIEW
#ifdef USE_D3D_MEMORY_TRACKER
    m_D3DMemTracker.dump(bfalse);
#endif
    GFXAdapter::destroy();
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_Directx9::createVertexDeclaration()
{
    HRESULT hr;

    // Create vertex declaration for scene
    if( FAILED( hr = DXCreateVertexDeclaration( VElem_PCT, &mp_VDcl_PCT ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = DXCreateVertexDeclaration( VElem_PC2T, &mp_VDcl_PC2T ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = DXCreateVertexDeclaration( VElem_PCTBIBW, &mp_VDcl_PCTBIBW ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = DXCreateVertexDeclaration( VElem_PNCTBIBW, &mp_VDcl_PNCTBIBW ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = DXCreateVertexDeclaration( VElem_PC, &mp_VDcl_PC ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = DXCreateVertexDeclaration( VElem_PT, &mp_VDcl_PT ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = DXCreateVertexDeclaration( VElem_PCBT, &mp_VDcl_PCBT ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = DXCreateVertexDeclaration( VElem_PCB2T, &mp_VDcl_PCB2T ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = DXCreateVertexDeclaration( VElem_PNCT, &mp_VDcl_PNCT ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = DXCreateVertexDeclaration( VElem_PNC3T, &mp_VDcl_PNC3T ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = DXCreateVertexDeclaration( VElem_QI_2TC, &mp_VDcl_QI_2TC ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = DXCreateVertexDeclaration( VElem_T, &mp_VDcl_T ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = DXCreateVertexDeclaration( VElem_I, &mp_VDcl_I ) ) )
    {
        return bfalse;
    }

    return btrue;
}

void GFXAdapter_Directx9::releaseVertexDeclaration()
{
    D3D_NM_RELEASE( mp_VDcl_PC );
    D3D_NM_RELEASE( mp_VDcl_PCT );
    D3D_NM_RELEASE( mp_VDcl_PC2T );
    D3D_NM_RELEASE( mp_VDcl_PCTBIBW );
    D3D_NM_RELEASE( mp_VDcl_PNCTBIBW );
    D3D_NM_RELEASE( mp_VDcl_PT );
    D3D_NM_RELEASE( mp_VDcl_PCBT );
    D3D_NM_RELEASE( mp_VDcl_PCB2T );
    D3D_NM_RELEASE( mp_VDcl_PNCT );
    D3D_NM_RELEASE( mp_VDcl_PNC3T );
    D3D_NM_RELEASE( mp_VDcl_QI_2TC );
    D3D_NM_RELEASE( mp_VDcl_T );
    D3D_NM_RELEASE( mp_VDcl_I );
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::destroyResources()
{
    GFXAdapter_Common<GFXAdapter_Directx9>::destroyResources();
    preD3DReset();
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::updateCameraFrustumPlanes(Camera* _cam)
{
    extractFrustumPlanes(*(const Matrix44*)&m_worldViewProj.getWorldViewProj(), _cam);
}

#ifndef GFX_COMMON_POSTEFFECT
// just a helper
RenderTarget* GFXAdapter_Directx9::createTarget(GFXAdapter_Directx9 *_gfxAdapter, u32 _width, u32 height, Texture::PixFormat _pixformat, u32 _flags, u32 _shiftSurfSize, RenderTarget *_pSrcBaseTexture, RenderTarget *_pLinkedTexture )
{
	RenderTargetInfo rtInfo(_width, height, 1, _pixformat);
	rtInfo.m_EDRAMOffset = _shiftSurfSize;
	rtInfo.m_srcBaseRT = _pSrcBaseTexture;
	rtInfo.m_linkedRT = _pLinkedTexture;
	return _gfxAdapter->createRenderTarget(rtInfo);
}

void GFXAdapter_Directx9::createAdditionalRenderTarget()
{
	m_AdditionnalRenderingBuffer[TEX_BFR_FULL_1] = createTarget(this, getScreenWidth(), getScreenHeight(), Texture::PF_RGBA);
#ifdef ITF_WINDOWS
	m_AdditionnalRenderingBuffer[TEX_BFR_FULL_2] = createTarget(this, getScreenWidth(), getScreenHeight(), Texture::PF_RGBA);
#endif
	m_AdditionnalRenderingBuffer[TEX_BFR_QRT_1] = createTarget(this, getScreenWidth()/2, getScreenHeight()/2, Texture::PF_RGBA);

#ifdef ITF_WINDOWS
    u32 afterBackEDRAMOffset = 0;
    u32 afterLightEDRAMOffset = 0;
#else
    //EDRAM offset just after tiled backbuffer and tiled Zbuffer
    u32 afterBackEDRAMOffset = getTargetDX(m_BackBuffer)->getSurface()->Size + getCurrentDepthStencilSurf()->Size;
    //EDRAM offset just after tiled backbuffer and tiled Zbuffer and the quarter buffer
    u32 afterLightEDRAMOffset = afterBackEDRAMOffset + (XGSurfaceSize(getScreenWidth()/LIGHTS_BFR_RATIO, getScreenHeight()/LIGHTS_BFR_RATIO, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE) * GPU_EDRAM_TILE_SIZE);
#endif

    //linked buffer
	m_AdditionnalRenderingBuffer[TEX_BFR_FRONT_LIGHT] = createTarget( this, getScreenWidth()/LIGHTS_BFR_RATIO, getScreenHeight()/LIGHTS_BFR_RATIO, Texture::PF_RGBA, 0       , afterBackEDRAMOffset, m_AdditionnalRenderingBuffer[TEX_BFR_FULL_1], NULL);
    m_AdditionnalRenderingBuffer[TEX_BFR_BACK_LIGHT] = createTarget( this, getScreenWidth()/LIGHTS_BFR_RATIO, getScreenHeight()/LIGHTS_BFR_RATIO, Texture::PF_RGBA, 0        , afterBackEDRAMOffset, m_AdditionnalRenderingBuffer[TEX_BFR_FULL_1], m_AdditionnalRenderingBuffer[TEX_BFR_FRONT_LIGHT]);
    m_AdditionnalRenderingBuffer[TEX_BFR_FLUID_BLUR] = createTarget( this, getScreenWidth()/FLUID_BLUR_BFR_RATIO, getScreenHeight()/FLUID_BLUR_BFR_RATIO, Texture::PF_RGBA, 0, 0                     , m_AdditionnalRenderingBuffer[TEX_BFR_FULL_1], m_AdditionnalRenderingBuffer[TEX_BFR_BACK_LIGHT]);
    m_AdditionnalRenderingBuffer[TEX_BFR_REFLECTION] = createTarget( this, getScreenWidth()/REFLECTION_BFR_RATIO, getScreenHeight()/REFLECTION_BFR_RATIO, Texture::PF_RGBA, 0, 0                     , m_AdditionnalRenderingBuffer[TEX_BFR_FULL_1], m_AdditionnalRenderingBuffer[TEX_BFR_FLUID_BLUR]);

    m_AdditionnalRenderingBuffer[TEX_BFR_QRT_FRONT_LIGHT] = createTarget(this, getScreenWidth()/QRT_LIGHTS_BFR_RATIO, getScreenHeight()/QRT_LIGHTS_BFR_RATIO, Texture::PF_RGBA, 0, afterLightEDRAMOffset, m_AdditionnalRenderingBuffer[TEX_BFR_QRT_1], NULL);
    m_AdditionnalRenderingBuffer[TEX_BFR_QRT_BACK_LIGHT] = createTarget( this, getScreenWidth()/QRT_LIGHTS_BFR_RATIO, getScreenHeight()/QRT_LIGHTS_BFR_RATIO, Texture::PF_RGBA, 0, afterLightEDRAMOffset, m_AdditionnalRenderingBuffer[TEX_BFR_QRT_1], m_AdditionnalRenderingBuffer[TEX_BFR_QRT_FRONT_LIGHT]);
    m_AdditionnalRenderingBuffer[TEX_BFR_QRT_TMP_LIGHT] = createTarget( this, getScreenWidth()/QRT_LIGHTS_BFR_RATIO, getScreenHeight()/QRT_LIGHTS_BFR_RATIO, Texture::PF_RGBA, 0, afterLightEDRAMOffset, m_AdditionnalRenderingBuffer[TEX_BFR_QRT_1], m_AdditionnalRenderingBuffer[TEX_BFR_QRT_BACK_LIGHT]);
    m_AdditionnalRenderingBuffer[TEX_BFR_FLUID_GLOW] = createTarget( this, getScreenWidth()/FLUID_GLOW_BFR_RATIO, getScreenHeight()/FLUID_GLOW_BFR_RATIO, Texture::PF_RGBA, 0, NULL, m_AdditionnalRenderingBuffer[TEX_BFR_QRT_1], m_AdditionnalRenderingBuffer[TEX_BFR_QRT_TMP_LIGHT]);

    // External Buffer, need surface for All Views
    m_AdditionnalExternalRenderingBuffer[0][TEX_EXT_BFR_SCENE_COPY] = createTarget(this, getScreenWidth(), getScreenHeight(), Texture::PF_RGBA, 0);
    m_AdditionnalExternalRenderingBuffer[0][TEX_EXT_BFR_IMPOSTOR_COPY] = createTarget(this, getScreenWidth(), getScreenHeight(), Texture::PF_RGBA, 0);
#ifdef ITF_USE_REMOTEVIEW
    m_AdditionnalExternalRenderingBuffer[1][TEX_EXT_BFR_SCENE_COPY] = createTarget(this, getScreenWidth(), getScreenHeight(), Texture::PF_RGBA, 0);
    m_AdditionnalExternalRenderingBuffer[1][TEX_EXT_BFR_IMPOSTOR_COPY] = createTarget(this, getScreenWidth(), getScreenHeight(), Texture::PF_RGBA, 0);
#endif

#ifdef ITF_GFX_USE_DEBUG_RENDER_TARGET
    m_DebugRenderingBuffer = createTarget(this, getScreenWidth(), getScreenHeight(), Texture::PF_RGBA, 0);
#endif

#ifndef ITF_WINDOWS
    afterBackEDRAMOffset = getTargetDX(m_AdditionnalRenderingBuffer[TEX_BFR_REFLECTION])->getSurface()->Size;
#endif
    m_MediumZBuffer = createTarget( this, getReflectionPassWidth(), getReflectionPassHeight(), Texture::PF_D24S8, 0, afterBackEDRAMOffset);
}

void GFXAdapter_Directx9::releaseAdditionalRenderTarget()
{
    for(u32 i=0; i<NB_TEX_BFR; i++)
    {
		if ( m_AdditionnalRenderingBuffer[i] )
		{
			m_AdditionnalRenderingBuffer[i]->release();
			m_AdditionnalRenderingBuffer[i] = NULL;
		}
    }

    for(u32 i=0; i<NB_TEX_EXT_BFR; i++)
    {
		if ( m_AdditionnalExternalRenderingBuffer[0][i] )
		{
			m_AdditionnalExternalRenderingBuffer[0][i]->release();
			m_AdditionnalExternalRenderingBuffer[0][i] = NULL;
		}
		if ( m_AdditionnalExternalRenderingBuffer[1][i] )
		{
			m_AdditionnalExternalRenderingBuffer[1][i]->release();
			m_AdditionnalExternalRenderingBuffer[1][i] = NULL;
		}
    }

	if ( m_DebugRenderingBuffer )
	{
		m_DebugRenderingBuffer->release();
		m_DebugRenderingBuffer = NULL;
	}
	if ( m_MediumZBuffer )
	{
		m_MediumZBuffer->release();
		m_MediumZBuffer = NULL;
	}
}

#endif

void GFXAdapter_Directx9::init()
{
    Super::preInitDriver();

#ifndef GFX_COMMON_POSTEFFECT
    createAdditionalRenderTarget();
#endif

    m_currentAFXSceneTexture = NULL;
    m_currentAFXBlurTexture = NULL;
    m_AFXTimer = SYSTEM_ADAPTER->getTime();

    if (getScreenWidth() == 0 || getScreenHeight() == 0)
    {
        LOG("GFXAdapter init error : resolution = 0, call setResolution(width,height) before init");
    }

    GFX_RECT Viewport;
    Viewport.left = 0;
    Viewport.top = 0;
    Viewport.right = getScreenWidth();
    Viewport.bottom = getScreenHeight();
    setupViewport(&Viewport);
    
    setFillMode(GFX_FILL_SOLID);

    // Turn off culling
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
#ifdef ITF_WINDOWS
    m_pd3dDevice->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
#endif
    
    setAlphaTest(bfalse);
    setAlphaRef();
    m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

    if(isRender)
    {
        HRESULT hr;
        hr = m_pd3dDevice->BeginScene();
    }
    isRender = 0;

    setShaderGroup(m_defaultShaderGroup);
  
#ifndef _USE_ZBUFFER_TILING
    getBackBuffer();
#endif

    resetSceneResolvedFlag();

#ifdef VBMANAGER_USE_DYNAMICRING_VB
    m_VertexBufferManager.createDynamicRingVB();
#endif

    setGlobalColor(Color::white());

    createOverDrawRamp( 50, 100, 150);
    SetTextureBind(0, NULL, true);
    SetTextureBind(1, NULL, true);
    SetTextureBind(2, NULL, true);
    SetTextureBind(3, NULL, true);
    SetTextureBind(4, NULL, true);
    SetTextureBind(5, NULL, true);
    SetTextureBind(6, NULL, true);
    SetTextureBind(7, NULL, true);

#ifdef ITF_USE_REMOTEVIEW
    if (!m_drcThread)
    {
        Synchronize::createCriticalSection(&m_csDRC);

        m_quitDRCThread = bfalse;
        m_currentIndexRing = 0;
        for (u32 index = 0;index<maxRingSurface;index++)
            m_RingSurface[index] = NULL;

        m_newBufferSet = bfalse;
    }
   
#endif

#ifdef ITF_SUPPORT_GPU_METRICS
    m_Metrics.init(m_pd3dDevice);
#endif

#ifdef ITF_WINDOWS
	BLOOMBERGINTERFACE->SetDirect3dDevice(m_pd3dDevice);    
#elif ITF_X360
    D3DTexture* tex;
    IDirect3DDevice9 *device = static_cast<IDirect3DDevice9 *>(GFX_ADAPTER_DX9->getRenderingDevice());
    device->GetFrontBuffer(&tex);
    BLOOMBERGINTERFACE->SetFrontBuffer(tex);
#endif

    Super::postInitDriver();
}

//----------------------------------------------------------------------------//

#ifdef ITF_X360

static inline Texture::PixFormat D3DToGFXPixFormat(D3DFORMAT _format)
{
    switch (_format)
    {
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
        return Texture::PF_RGBA;
    case D3DFMT_DXT1:
    case D3DFMT_LIN_DXT1:
        return Texture::PF_DXT1;
    case D3DFMT_DXT3:
    case D3DFMT_LIN_DXT3:
        return Texture::PF_DXT3;
    case D3DFMT_DXT5:
    case D3DFMT_LIN_DXT5:
        return Texture::PF_DXT5;
    case D3DFMT_L8:
        return Texture::PF_L8;
    case D3DFMT_A8:
        return Texture::PF_A8;
    case D3DFMT_A8L8:
        return Texture::PF_LA8;
    default:
        return Texture::PF_RGBA;
    }
}

#else
static inline Texture::PixFormat D3DToGFXPixFormat(D3DFORMAT _format)
{
    switch (_format)
    {
    case D3DFMT_R8G8B8:
        return Texture::PF_RGB;
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
        return Texture::PF_RGBA;
    case D3DFMT_DXT1:
        return Texture::PF_DXT1;
    case D3DFMT_DXT2:
    case D3DFMT_DXT3:
        return Texture::PF_DXT3;
    case D3DFMT_DXT4:
    case D3DFMT_DXT5:
        return Texture::PF_DXT5;
    case D3DFMT_L8:
    case D3DFMT_P8:
        return Texture::PF_L8;
    case D3DFMT_A8:
        return Texture::PF_A8;
    case D3DFMT_A8P8:
    case D3DFMT_A8L8:
        return Texture::PF_LA8;
	case D3DFMT_D24S8:
		return Texture::PF_D24S8;
	case D3DFMT_D24FS8:
		return Texture::PF_D24FS8;
    default:
        return Texture::PF_RGBA;
    }
}
#endif

static inline D3DFORMAT GFXPixFormatToD3D(Texture::PixFormat _pixformat)
{
    switch (_pixformat)
    {
	default:
    case Texture::PF_RGB:
        ITF_ASSERT(0);
        return D3DFMT_UNKNOWN;

    case Texture::PF_DXT1:
#ifdef ITF_WINDOWS
        return D3DFMT_DXT1;
#else
        return D3DFMT_LIN_DXT1;
#endif //ITF_WINDOWS
    case Texture::PF_DXT3:
#ifdef ITF_WINDOWS
        return D3DFMT_DXT3;
#else
        return D3DFMT_LIN_DXT3;
#endif //ITF_WINDOWS
    case Texture::PF_DXT5:
#ifdef ITF_WINDOWS
        return D3DFMT_DXT5;
#else
        return D3DFMT_LIN_DXT5;
#endif //ITF_WINDOWS

    case Texture::PF_RGBA:
#ifdef ITF_WINDOWS
        return D3DFMT_A8B8G8R8;
#else
        return D3DFMT_LIN_A8R8G8B8;
#endif //ITF_WINDOWS
    case Texture::PF_L8:
#ifdef ITF_WINDOWS
        return D3DFMT_L8;
#else
        return D3DFMT_LIN_L8;
#endif //ITF_WINDOWS
    case Texture::PF_A8:
#ifdef ITF_WINDOWS
        return D3DFMT_A8;
#else
        return D3DFMT_LIN_A8;
#endif //ITF_WINDOWS
    case Texture::PF_RGBA_FLT16:
#ifdef ITF_WINDOWS
        return D3DFMT_A16B16G16R16F;
#else
        return D3DFMT_LIN_A16B16G16R16F;
#endif //ITF_WINDOWS
    case Texture::PF_RGBA_FLT32:
#ifdef ITF_WINDOWS
        return D3DFMT_A32B32G32R32F;
#else
        return D3DFMT_LIN_A32B32G32R32F;
#endif //ITF_WINDOWS
    case Texture::PF_RG_FLT16:
#ifdef ITF_WINDOWS
        return D3DFMT_A16B16G16R16F;
#else
        return D3DFMT_LIN_A16B16G16R16F;
#endif //ITF_WINDOWS
    case Texture::PF_RG_FLT32:
#ifdef ITF_WINDOWS
        return D3DFMT_G32R32F;
#else
        return D3DFMT_LIN_G32R32F;
#endif //ITF_WINDOWS
    case Texture::PF_R_FLT16:
#ifdef ITF_WINDOWS
        return D3DFMT_R16F;
#else
        return D3DFMT_LIN_R16F;
#endif //ITF_WINDOWS
    case Texture::PF_R_FLT32:
#ifdef ITF_WINDOWS
        return D3DFMT_R32F;
#else
        return D3DFMT_LIN_R32F;
#endif //ITF_WINDOWS
	case Texture::PF_D24S8:
		return D3DFMT_D24S8;
	case Texture::PF_D24FS8:
		return D3DFMT_D24FS8;
    }
}

static inline D3DFORMAT GFXPixFormatToD3DNoLin(Texture::PixFormat _pixformat)
{
    switch (_pixformat)
    {
	default:
    case Texture::PF_RGB:
        ITF_ASSERT(0);
        return D3DFMT_UNKNOWN;

    case Texture::PF_DXT1:
        return D3DFMT_DXT1;
    case Texture::PF_DXT3:
        return D3DFMT_DXT3;
    case Texture::PF_DXT5:
        return D3DFMT_DXT5;
    case Texture::PF_RGBA:
        return D3DFMT_A8B8G8R8;
    case Texture::PF_L8:
        return D3DFMT_L8;
    case Texture::PF_A8:
        return D3DFMT_A8;
    case Texture::PF_RGBA_FLT16:
        return D3DFMT_A16B16G16R16F;
    case Texture::PF_RGBA_FLT32:
        return D3DFMT_A32B32G32R32F;
    case Texture::PF_RG_FLT16:
        return D3DFMT_A16B16G16R16F;
    case Texture::PF_RG_FLT32:
        return D3DFMT_G32R32F;
    case Texture::PF_R_FLT16:
        return D3DFMT_R16F;
    case Texture::PF_R_FLT32:
        return D3DFMT_R32F;
	case Texture::PF_D24S8:
		return D3DFMT_D24S8;
	case Texture::PF_D24FS8:
		return D3DFMT_D24FS8;
    }
}


//----------------------------------------------------------------------------//

#ifdef ITF_X360
static u32 getSwizzle(const u32 _mode)
{
    switch(_mode)
    {
    case GFX_TEXREMAP_FROM_A: return GPUSWIZZLE_W;
    case GFX_TEXREMAP_FROM_R: return GPUSWIZZLE_X;
    case GFX_TEXREMAP_FROM_G: return GPUSWIZZLE_Y;
    case GFX_TEXREMAP_FROM_B: return GPUSWIZZLE_Z;
    case GFX_TEXREMAP_ZERO:   return GPUSWIZZLE_0;
    case GFX_TEXREMAP_ONE:    return GPUSWIZZLE_1;
    }
    return GPUSWIZZLE_KEEP;
}
static D3DFORMAT computeSwizzleFormat(const D3DFORMAT _format, const u32 _mode)
{
    if (_mode == GFX_TEXREMAP_DEFAULT())
        return _format;

    // Reset current swizzle
    u32 u32Format = (u32)_format & ~D3DFORMAT_SWIZZLE_MASK;

    // Add wanted swizzle
    u32Format |= (getSwizzle(GFX_TEXREMAP_GETA(_mode)) << D3DFORMAT_SWIZZLEW_SHIFT) |
                 (getSwizzle(GFX_TEXREMAP_GETR(_mode)) << D3DFORMAT_SWIZZLEX_SHIFT) |
                 (getSwizzle(GFX_TEXREMAP_GETG(_mode)) << D3DFORMAT_SWIZZLEY_SHIFT) |
                 (getSwizzle(GFX_TEXREMAP_GETB(_mode)) << D3DFORMAT_SWIZZLEZ_SHIFT);

    // Cast result
    return (D3DFORMAT)u32Format;
}
#endif // ITF_X360

bbool GFXAdapter_Directx9::loadTextureFromPlatformRawData(Texture* _texture, const u8 * _rawBuf)
{ 
#ifdef USE_TEXTURE_TILE
	PlatformTexture *platformTex = getPlatformTexture(_texture);

    cleanupTexture(_texture);

    D3DTexture* texture = (D3DTexture*)_rawBuf;
    // Get description of the texture's mip0
    D3DSURFACE_DESC desc;
    texture->GetLevelDesc( 0, &desc );

    u32 extraDescFlags = 0;//D3DUSAGE_CPU_CACHED_MEMORY;

    u32 baseSize;
    u32 mipSize;
    if(texture->GetType() == D3DRTYPE_TEXTURE)
    {
        D3DTexture* _texadd = newAlloc(mId_Textures, D3DTexture);

        _texture->m_mipMapLevels = texture->GetLevelCount();

        XGSetTextureHeaderEx
            (
            desc.Width,
            desc.Height, 
            _texture->m_mipMapLevels, 
            desc.Usage | extraDescFlags,
#ifdef ITF_X360
            // Compute remapping
            computeSwizzleFormat(desc.Format, _texture->getRemap()),
#else //ITF_X360
            desc.Format,
#endif //ITF_X360
            0, 
            0, 
            0, 
            XGHEADER_CONTIGUOUS_MIP_OFFSET,
            0,
            ( D3DTexture* )_texadd, 
            &baseSize, 
            &mipSize
            );

        //_texadd->Common |= MipDataContinuousInMemory;

        // Compute the required memory for the texture data
        u32 textureDataSize = baseSize + mipSize;
 
        u8* textureData;

        //bbool flushCache = ( (desc.Usage | extraDescFlags) & D3DUSAGE_CPU_CACHED_MEMORY ) != 0;

        textureData = (u8*)slotAllocatorManager::allocatePhysicalMemory( textureDataSize, MEM_ALIGNMENT_4K,  PAGE_READWRITE|PAGE_WRITECOMBINE ,MemoryId::mId_Textures);

        ITF_MemcpyWriteCombined(textureData, _rawBuf + sizeof(D3DTexture), /*sizeof(u8) **/ textureDataSize);

        XGOffsetBaseTextureAddress( _texadd, textureData, textureData );

        Size size( desc.Width, desc.Height);
        setTextureSize(_texture, size);

        platformTex->m_DXTexture = _texadd;

        Size d_dataSize(size);
        updateTextureSize(_texture);
        Size TexSize(_texture->getSizeX(), _texture->getSizeY());
        updateCachedScaleValues(_texture);

        _texture->m_datasizeX = (u32)d_dataSize.d_width;
        _texture->m_datasizeY = (u32)d_dataSize.d_height;
        _texture->m_pixFormat = D3DToGFXPixFormat(desc.Format);
    }
#else

    DDS_HEADER header;
    ITF_Memcpy(&header, _rawBuf, sizeof(header));

    if (header.dwHeight >= 8192 )
    {
        ITF_FATAL_ERROR("Texture size too big %d ==> %s ,quit the game and fix ", header.dwHeight, _texture->getPath().toString8());
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
        default:
            PixFormat = Texture::PF_RGBA;
            break;
    }

    loadFromMemory(_rawBuf, size, _texture, PixFormat, btrue, header.dwMipMapCount);

#endif

    //RESOURCE_MANAGER->addResourceAsLoaded(_texture);
    return btrue;
}

bbool GFXAdapter_Directx9::loadTexture(Texture* _texture, const Path & _path)
{   
#ifdef MODE_HRESTEXTURE_SUPPORTED
    if (_texture->hasHResVersion())
    {
        Path hdFileNameCooked = FILESERVER->getCookedHResVersionName(_texture->getPath());
        File* fileTexture = FILEMANAGER->openFile(hdFileNameCooked, ITF_FILE_ATTR_READ);
        if (!fileTexture)
        {
            LOG("GFXAdapter_Directx9::loadTexture >> Missing HRes texture %s", hdFileNameCooked.toString8().cStr());
            return bfalse;
        }

        ux sizeFile = ux(fileTexture->getLength());
        u8 * rawBuf = newAlloc(mId_Temporary,u8[sizeFile]);
        ITF_ASSERT_MSG(rawBuf, "LOAD Texture Failed out of memory %s", hdFileNameCooked.toString8());

        return loadTextureFromPlatformRawData(_texture, rawBuf);
    }
    else
#endif //MODE_HRESTEXTURE_SUPPORTED
    return Super::loadTexture(_texture, _path);
}

void GFXAdapter_Directx9::resolveRenderTargetForPass(GFX_ZLIST_PASS_TYPE _passType)
{
    if((_passType == GFX_ZLIST_PASS_ZPREPASS) || _passType == GFX_ZLIST_PASS_3D)
    {
        return;
    }
    else if (m_currentRenderingSurface == getCurrentBackBufferSurf())
    {
        resolve(m_backBufferNoTile);
    }
    else
    {
        for(u32 i=0; i<NB_TEX_BFR; i++)
        {
			GFX_RenderingBuffer_DX9 *targetDX9 = getBufferTextureDX(i);
            if (targetDX9 && (m_currentRenderingSurface == targetDX9->m_DXSurface))
            {
                resolve(getBufferTexture(i));
            }
        }
    }
}

void GFXAdapter_Directx9::copyDebugRenderTarget(f32 _alpha, const Vec2d & _pixelOffset)
{
	if ( !m_DebugRenderingBuffer )
		return;
	GFX_RenderingBuffer_DX9 *debugRTDX = (GFX_RenderingBuffer_DX9 *)m_DebugRenderingBuffer->m_adapterimplementationData;
    if (!debugRTDX->m_DXSurface)
        return;

    // right now only used for debugging
#ifdef ITF_GFX_USE_DEBUG_RENDER_TARGET
#ifdef ITF_WINDOWS
    if (_alpha<=0.f)
    {
        RECT rectSrc;
        rectSrc.left   = 0;
        rectSrc.right  = getScreenWidth();
        rectSrc.top    = 0;
        rectSrc.bottom = getScreenHeight();
        RECT rectDest;
        rectDest.left   = 0 + u32(_pixelOffset.x());
        rectDest.right  = getScreenWidth();
        rectDest.top    = 0 + u32(_pixelOffset.y());
        rectDest.bottom = getScreenHeight();
        ITF_VERIFY(D3D_OK == m_pd3dDevice->StretchRect( debugRTDX->m_DXSurface, &rectSrc,
                                                        m_currentRenderingSurface, &rectDest,
                                                        D3DTEXF_LINEAR));
    }
    else
#endif
    {
        init2DRender();

        RenderPassContext rdrCtx;
        PrimitiveContext primCtx = PrimitiveContext(&rdrCtx);
        DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

        //setInternalRenderTarget(m_currentRenderingSurface, 0);
        drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ALPHA)
            .setDepthTest(bfalse)
            .setDepthWrite(bfalse);

        setShaderGroup(m_defaultShaderGroup);
        setGfxMatDefault(drawCallCtx);
        bindTexture(0, m_DebugRenderingBuffer, bfalse, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);
        drawScreenQuad(drawCallCtx, 0 + _pixelOffset.x(), 0 - _pixelOffset.y(), (f32)debugRTDX->getWidth(), (f32)debugRTDX->getHeight(), 0, Color(_alpha, 1.f, 1.f, 1.f).getAsU32(), bfalse);
        //restore camera    
        View::getCurrentView()->apply();
        end2DRender();

    }

#endif
}

void GFXAdapter_Directx9::copySurfaceToDebugRenderTarget(RenderTarget *_sourceRT)
{
	GFX_RenderingBuffer_DX9 *debugRTDX = (GFX_RenderingBuffer_DX9 *)m_DebugRenderingBuffer->m_adapterimplementationData;
	GFX_RenderingBuffer_DX9 *sourceRTDX = (GFX_RenderingBuffer_DX9 *)_sourceRT->m_adapterimplementationData;
    RECT rectDest;
    rectDest.left   = 0;
    rectDest.right  = getScreenWidth();
    rectDest.top    = 0;
    rectDest.bottom = getScreenHeight();

#ifdef ITF_WINDOWS
    RECT rectSrc;
    D3DSURFACE_DESC desc;
    sourceRTDX->getSurface()->GetDesc(&desc);
    rectSrc.left   = 0;
    rectSrc.right  = desc.Width;
    rectSrc.top    = 0;
    rectSrc.bottom = desc.Height;

    ITF_VERIFY(D3D_OK == m_pd3dDevice->StretchRect( sourceRTDX->getSurface(), &rectSrc,
                                                    debugRTDX->getSurface(), &rectDest,
                                                    D3DTEXF_LINEAR));
#else
    if(sourceRTDX != NULL)
    {
        init2DRender();

        setInternalRenderTarget(debugRTDX->getSurface(), 0);

        RenderPassContext rdrCtx;
        PrimitiveContext primCtx = PrimitiveContext(&rdrCtx);
        DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

        drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
            .setDepthTest(bfalse)
            .setDepthWrite(bfalse);

        SetTextureBind(0, sourceRTDX, bfalse);
        D3DSURFACE_DESC desc;
        sourceRTDX->getSurface()->GetDesc(&desc);
        drawScreenQuad(drawCallCtx, 0,0, (f32)desc.Width, (f32)desc.Height, 0, COLOR_WHITE, bfalse);
        resolve(m_DebugRenderingBuffer);
        //restore camera
        View::getCurrentView()->apply();
        end2DRender();

    }
#endif
}

void GFXAdapter_Directx9::copyPickingToDebugRenderTarget()
{
    if (!m_RendereringBufferPicking) return;
    copySurfaceToDebugRenderTarget(m_RendereringBufferPicking);
}

void GFXAdapter_Directx9::copyPassToDebugRenderTarget(GFX_ZLIST_PASS_TYPE _passType)
{
	if ( !m_DebugRenderingBuffer )
		return;
	GFX_RenderingBuffer_DX9 *debugRTDX = (GFX_RenderingBuffer_DX9 *)m_DebugRenderingBuffer->m_adapterimplementationData;
    if (!debugRTDX->getSurface())
        return;

    // right now only used for debugging
#ifdef ITF_GFX_USE_DEBUG_RENDER_TARGET

    RenderTarget *sourceRT = NULL;
    if (_passType == GFX_ZLIST_PASS_FRONT_LIGHT)
    {
        if(m_lightManager->getFrontLightBufferType()==LIGHT_BFR_FULL)
        {
            sourceRT = getBufferTexture(TEX_BFR_FRONT_LIGHT);
        }
        else
        {
            sourceRT = getBufferTexture(TEX_BFR_QRT_FRONT_LIGHT);
        }
    }
    else if (_passType == GFX_ZLIST_PASS_BACK_LIGHT)
    {
        if(m_lightManager->getBackLightBufferType()==LIGHT_BFR_FULL)
        {
            sourceRT = getBufferTexture(TEX_BFR_BACK_LIGHT);
        }
        else
        {
            sourceRT = getBufferTexture(TEX_BFR_QRT_BACK_LIGHT);
        }
    }
    else if (_passType == GFX_ZLIST_PASS_REFLECTION)
    {
        sourceRT = getBufferTexture(TEX_BFR_REFLECTION);
    }
    else if (_passType == GFX_ZLIST_PASS_FLUID)
    {
        sourceRT = getBufferTexture(TEX_BFR_FLUID_GLOW);
    }
    else if (_passType == GFX_ZLIST_PASS_REGULAR)
    {
        sourceRT = getCurrentBackBuffer();
    }
    else
    {
        ITF_ASSERT(0);
        return;
    }

    copySurfaceToDebugRenderTarget(sourceRT);
#else
    // TODO on X360
    ITF_ASSERT(0);
#endif
}

u32* GFXAdapter_Directx9::lockDebugRenderTarget(u32 & _width, u32 & _height, u32 & _pixelPitch)
{
#ifndef ITF_WINDOWS
    return Super::lockDebugRenderTarget(_width, _height, _pixelPitch);
#else
	GFX_RenderingBuffer_DX9 *debugRTDX = (GFX_RenderingBuffer_DX9 *)m_DebugRenderingBuffer->m_adapterimplementationData;
    _width = m_DebugRenderingBuffer->getSizeX();
    _height = m_DebugRenderingBuffer->getSizeY();
    _pixelPitch = 0;


    m_debugBufferOffscreenSwap ^= 1;

    LPDIRECT3DSURFACE9 & pOffscreenSurf = m_debugBufferOffscreenSurf[m_debugBufferOffscreenSwap];

    if (!pOffscreenSurf)
    {
        HRESULT hr = DXCreateOffscreenPlainSurface(_width, _height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pOffscreenSurf, NULL);
        if (hr!=S_OK)
            messageD3DError(hr);
    }
    
    HRESULT hr = m_pd3dDevice->GetRenderTargetData(debugRTDX->getSurface(), pOffscreenSurf);
    if (hr!=S_OK)
        messageD3DError(hr);

    D3DLOCKED_RECT lockedrect;
    if (S_OK == pOffscreenSurf->LockRect(&lockedrect, NULL, D3DLOCK_READONLY))
    {
        _pixelPitch = u32(lockedrect.Pitch) / sizeof(u32);
        return reinterpret_cast<u32 *>(lockedrect.pBits);
    }

    return nullptr;
#endif
}

void GFXAdapter_Directx9::unlockDebugRenderTarget()
{
#ifndef ITF_WINDOWS
    return Super::unlockDebugRenderTarget();
#else
    LPDIRECT3DSURFACE9 & pOffscreenSurf = m_debugBufferOffscreenSurf[m_debugBufferOffscreenSwap];
    ITF_ASSERT_CRASH(pOffscreenSurf != nullptr, "lockDebugRenderTarget/unlockDebugRenderTarget pairing issue");
    pOffscreenSurf->UnlockRect();
#endif
}


void GFXAdapter_Directx9::copyScene()
{
    u32 engineView = e_engineView_MainScreen;
#ifdef ITF_USE_REMOTEVIEW
    if(View::getCurrentView()->isRemoteView())
        engineView = e_engineView_RemoteScreen;
#endif

#ifdef ITF_WINDOWS
    LPDIRECT3DSURFACE9 surfSrc = getExternalBufferTextureDX(engineView, TEX_EXT_BFR_SCENE_COPY)->getSurface();
    RECT rectDest;
    rectDest.left   = 0;
    rectDest.right  = getScreenWidth();
    rectDest.top    = 0;
    rectDest.bottom = getScreenHeight();

    ITF_VERIFY(D3D_OK == m_pd3dDevice->StretchRect( getCurrentBackBufferSurf(), &rectDest,
        surfSrc, NULL,
        D3DTEXF_LINEAR));

    //Force alpha value to 1
    init2DRender();

    RenderPassContext rdrCtx;
    PrimitiveContext primCtx = PrimitiveContext(&rdrCtx);
    DrawCallContext drawCallCtx = DrawCallContext(&primCtx);
    setInternalRenderTarget(getExternalBufferTextureDX(engineView, TEX_EXT_BFR_SCENE_COPY)->getSurface(), NULL);

    setShaderGroup(m_defaultShaderGroup);
    setGfxMatDefault(drawCallCtx);

    drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
        .setAlphaWrite(btrue)
        .setColorWrite(bfalse)
        .setSeparateAlphaBlend(GFX_ALPHAMASK_ADDALPHA)
        .setDepthTest(bfalse);

    u32 W = getScreenWidth();
    u32 H = getScreenHeight();
    drawScreenQuad(drawCallCtx, 0.f, 0.f, (f32)W, (f32)H, 0.f, COLOR_WHITE, btrue);

    end2DRender();

#else
    resolve( getExternalBufferTexture(engineView, TEX_EXT_BFR_SCENE_COPY) );
#endif
}

void GFXAdapter_Directx9::drawScreenTransition(f32 _fadeVal)
{

#ifndef ITF_X360

    const screenTransition& transition = m_screenTransitionManager.getCurrentTransition();
    f32 fade = _fadeVal;

    init2DRender();

    u32 screenwidth = getScreenWidth();
    u32 screenHeight = getScreenHeight();

    u32 engineViewID = e_engineView_MainScreen;
#ifdef ITF_USE_REMOTEVIEW
    if(View::getCurrentView()->isRemoteView())
        engineViewID = e_engineView_RemoteScreen;

    engineView view = (engineView)getCurrentEngineView();
    if(view != e_engineView_MainScreen)
    {
        screenwidth = getRemoteScreenWidth();
        screenHeight = getRemoteScreenHeight();
    }
#endif

    RenderPassContext rdrCtx;
    rdrCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
        .setAlphaTest(bfalse)
        .setDepthTest(bfalse);
    SetDrawCallState(rdrCtx.getStateCache(), 0xFFFFFFFF, btrue);
    PrimitiveContext primCtx = PrimitiveContext(&rdrCtx);
    DrawCallContext drawCallCtx = DrawCallContext(&primCtx);
    setShaderGroup(m_defaultShaderGroup);
    setGfxMatDefault(drawCallCtx);
    //setGFXPrimitiveParameters(getCommonParam(), _rdrPassCtxt);
    setGlobalColor(Color::white());

    SetTextureBind(0, getExternalBufferTextureDX(engineViewID, TEX_EXT_BFR_SCENE_COPY));

    if(transition.type == GFX_ST_TYPE_FADE)
    {
        fade *= 2.0f;
        Color fadeColor = transition.color;
        if(fade > 1.0f)
        {
            //Fade out : draw new scene
            fade = 2.0f - fade;
            fadeColor = Interpolate(Color(0.0f, 1.0f, 1.0f, 1.0f), fadeColor, fade);
            drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ALPHA)
                .setAlphaTest(bfalse)
                .setDepthTest(bfalse);
            drawScreenQuad(drawCallCtx, 0, 0, (f32)screenwidth, (f32)screenHeight, 0, fadeColor.getAsU32(), btrue);
        }
        else
        {
            //Fade in : draw old copy scene
            fadeColor = Interpolate(Color::white(), fadeColor, fade);
            drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
                .setAlphaTest(bfalse)
                .setDepthTest(bfalse);
            drawScreenQuad(drawCallCtx, 0, 0, (f32)screenwidth, (f32)screenHeight, 0, fadeColor.getAsU32(), bfalse);
        }
    }
    else if(transition.type == GFX_ST_TYPE_SLIDE_LEFT || transition.type == GFX_ST_TYPE_SLIDE_RIGHT)
    {
        f32 slide = fade * screenwidth;
        if(transition.type == GFX_ST_TYPE_SLIDE_LEFT)
            slide *= -1;
        drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY);
        drawScreenQuad(drawCallCtx, slide, 0, (f32)screenwidth, (f32)screenHeight, 0, 0xFFFFFFFF, bfalse);
    }
    else if(transition.type == GFX_ST_TYPE_SLIDE_UP || transition.type == GFX_ST_TYPE_SLIDE_DOWN)
    {
        f32 slide = fade * screenHeight;
        if(transition.type == GFX_ST_TYPE_SLIDE_DOWN)
            slide *= -1;
        drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY);
        drawScreenQuad(drawCallCtx, 0, slide, (f32)screenwidth, (f32)screenHeight, 0, 0xFFFFFFFF, bfalse);
    }
    else if(transition.type == GFX_ST_TYPE_SCALE_LEFT || transition.type == GFX_ST_TYPE_SCALE_RIGHT)
    {
        f32 left = 0.0f;
        f32 slide = fade * screenwidth;
        f32 right = (f32)screenwidth - slide;
        if(transition.type == GFX_ST_TYPE_SCALE_RIGHT)
        {
            left = slide;
        }
        drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY);
        drawScreenQuad(drawCallCtx, left, 0, (f32)right, (f32)screenHeight, 0, 0xFFFFFFFF, bfalse);
    }
    else if(transition.type == GFX_ST_TYPE_ZOOM_OUT)
    {
        Color fadeAlpha = Color::white();
        fadeAlpha.m_a = (1.0f - fade);
        f32 scaleX = fade * screenwidth;
        f32 scaleY = fade * screenHeight;
        drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ALPHA);
        drawScreenQuad(drawCallCtx, scaleX/ 2.0f, scaleY/ 2.0f, (f32)screenwidth - scaleX, (f32)screenHeight - scaleY, 0, fadeAlpha.getAsU32(), bfalse);
    }
    else if(transition.type == GFX_ST_TYPE_ZOOM_IN)
    {
        const f32 zoomFactor = 1.0f;
        Color fadeAlpha = Color::white();
        fadeAlpha.m_a = (1.0f - fade);
        f32 scaleX = fade * screenwidth * zoomFactor;
        f32 scaleY = fade * screenHeight * zoomFactor;
        drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ALPHA);
        drawScreenQuad(drawCallCtx, -scaleX/2.0f, -scaleY/2.0f, (f32)screenwidth + scaleX, (f32)screenHeight + scaleY, 0, fadeAlpha.getAsU32(), bfalse);
    }

    end2DRender();

#endif
}

void GFXAdapter_Directx9::setRenderTargetForPass(GFX_ZLIST_PASS_TYPE _passType, bbool _restoreContext)
{
    m_curPassType = _passType;

    switch (_passType)
    {
    case GFX_ZLIST_PASS_FRONT_LIGHT:
    case GFX_ZLIST_PASS_BACK_LIGHT:
        {
            // be sure the render target buffers are no more set as texture
            SetTextureBind(6, NULL);
            SetTextureBind(7, NULL);

			GFX_RenderingBuffer_DX9 *rt = (_passType==GFX_ZLIST_PASS_FRONT_LIGHT)?getFrontLightTextureDX(m_lightManager->getFrontLightBufferType()):getBackLightTextureDX(m_lightManager->getBackLightBufferType());
            PlatformSurface* surf = rt->getSurface();
            setInternalRenderTarget(surf, NULL);

          #ifdef ITF_X360
            forceRenderToAllTile(btrue);
          #endif
        }
        break;
    case GFX_ZLIST_PASS_REGULAR:
    case GFX_ZLIST_PASS_ZPREPASS:
        {
            setBackBuffer(btrue, _restoreContext);
            m_currentRenderingSurface = getCurrentBackBufferSurf();

            GFX_RECT rcViewport;
            const GFX_Viewport &viewport = View::getCurrentView()->getViewport();
            rcViewport.left = viewport.m_x;
            rcViewport.right = viewport.m_x + viewport.m_width;
            rcViewport.top =  viewport.m_y;
            rcViewport.bottom = viewport.m_y + viewport.m_height;

            setupViewport(&rcViewport);

          #ifdef ITF_X360
            if(_passType==GFX_ZLIST_PASS_REGULAR)
                m_pd3dDevice->SetShaderGPRAllocation(0, 16, 112);
            else
                m_pd3dDevice->SetShaderGPRAllocation(0, 64, 64);

            forceRenderToAllTile(bfalse);
          #endif
        }
        break;

    case GFX_ZLIST_PASS_SCREENSHOT_2D:
        {
			GFX_RenderingBuffer_DX9 *rt = (GFX_RenderingBuffer_DX9 *)m_asyncScreenshotBuffer->m_adapterimplementationData;
            setInternalRenderTarget(rt->getSurface(), nullptr);
            m_currentRenderingSurface = rt->getSurface();
        }
        break;

    case GFX_ZLIST_PASS_REFRACTION:
        {
            PlatformSurface* surfDest = getBufferTextureDX(TEX_BFR_REFRACTION)->getSurface();

            setInternalRenderTarget(surfDest, NULL);
            m_currentRenderingSurface = surfDest;
        }
        break;

    case GFX_ZLIST_PASS_REFLECTION_PREPASS:
        {
            PlatformSurface* surfDest = getBufferTextureDX(TEX_BFR_REFLECTION)->getSurface();
            PlatformSurface* surfZDest = getAdditionnalZBufferDX()->getSurface();

            setInternalRenderTarget(surfDest, surfZDest);
            m_currentRenderingSurface = surfDest;
       }
        break;

    case GFX_ZLIST_PASS_REFLECTION:
        {
            PlatformSurface* surfDest = getBufferTextureDX(TEX_BFR_REFLECTION)->getSurface();
            PlatformSurface* surfZDest = getAdditionnalZBufferDX()->getSurface();

            setInternalRenderTarget(surfDest, surfZDest);
            m_currentRenderingSurface = surfDest;
        }
        break;

    case GFX_ZLIST_PASS_FLUID:
        {
            PlatformSurface* surfDest = getBufferTextureDX(TEX_BFR_FLUID_GLOW)->getSurface();

            setInternalRenderTarget(surfDest, NULL);
            m_currentRenderingSurface = surfDest;
        }
        break;
    }

#ifdef ITF_PICKING_SUPPORT
    // override the render target in picking mode
    if (m_bPickingMode)
    {
        setRenderTarget_Picking();
        if (m_RendereringBufferPicking)
		{
			GFX_RenderingBuffer_DX9 *pickingRTDX = (GFX_RenderingBuffer_DX9 *)m_RendereringBufferPicking->m_adapterimplementationData;
            m_currentRenderingSurface = pickingRTDX->getSurface();
		}
    }
#endif
}

//----------------------------------------------------------------------------//
ITF_INLINE void GFXAdapter_Directx9::clear(u32 _buffer, u32 _Col)
{
    u32 Buf = 0;

    if(_buffer & GFX_CLEAR_COLOR)
    {
        Buf |= D3DCLEAR_TARGET;
    }

    if(_buffer & GFX_CLEAR_ZBUFFER)
    {
        Buf |= D3DCLEAR_ZBUFFER;
    }

    if(_buffer & GFX_CLEAR_STENCIL)
    { 
        Buf |= D3DCLEAR_STENCIL;
    }

    if (getOverDrawRender())
    {
        _Col = 0;
    }
    m_pd3dDevice->Clear( 0, NULL, Buf,D3DCOLOR(_Col), internalDepth(1.0f), 0 );
}

void GFXAdapter_Directx9::clear(u32 _buffer, f32 _r, f32 _g, f32 _b, f32 _a)
{
    GFXAdapter_Directx9::clear(_buffer, (u32)D3DCOLOR_ARGB((u32)(_a*255.f), (u32)(_r*255.f),(u32)(_g*255.f),(u32)(_b*255.f)));
}
//----------------------------------------------------------------------------//

f32 GFXAdapter_Directx9::internalDepth(f32 _z)
{
#ifdef _USE_ZBUFFER_FLOAT
    //invert Z with floating Zbuffer for better rejection
    return 1.0f - _z;
#else
    return _z;
#endif
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setAlphaBlend(GFX_BLENDMODE _Blend, GFX_ALPHAMASK _alphaMask)
{
    if ( getOverDrawRender() )
        _Blend = GFX_BLEND_ADD;

	if ( ( _Blend == GFX_BLEND_COPY ) && ( _alphaMask == GFX_ALPHAMASK_NONE ) )
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	else
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

    switch(_Blend)
    {
        case GFX_BLEND_COPY:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        default:
        case GFX_BLEND_UNKNOWN:
            ITF_ASSERT_MSG(bfalse, "Incorrect blend mode");
            //missing break intentionally
        case GFX_BLEND_ALPHA:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ALPHAPREMULT:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ALPHADEST:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVDESTALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ALPHADESTPREMULT:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ADD:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ADDSMOOTH:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ADDALPHA:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_SUBALPHA:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
        break;

        case GFX_BLEND_SUB:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
        break;

        case GFX_BLEND_MUL:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ALPHAMUL:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_IALPHAMUL:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_IALPHA:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_IALPHAPREMULT:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_IALPHADEST:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVDESTALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_IALPHADESTPREMULT:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVDESTALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;
    
        case GFX_BLEND_MUL2X:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ALPHATOCOLOR:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_IALPHATOCOLOR:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVDESTALPHA );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_SETTOCOLOR:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        // screen is sometimes a less color saturating additif effect
        case GFX_BLEND_SCREEN:
            m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
            m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
            m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;
    }

    switch(_alphaMask)
    {
    case GFX_ALPHAMASK_NONE :
    default :
        m_pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ONE );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD );
        break;

    case GFX_ALPHAMASK_ADDALPHA :
        m_pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_ONE );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ONE );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD );
        break;

    case GFX_ALPHAMASK_SUBALPHA :
        m_pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD );
        break;

    case GFX_ALPHAMASK_COPY :
        m_pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_ONE );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD );
        break;
	}
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setCullMode(GFX_CULLMODE _Cull)
{
    switch(_Cull)
    {
        case GFX_CULL_NONE:     m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE );break;
        case GFX_CULL_CCW:      m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );break;
        case GFX_CULL_CW:       m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );break;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setFillMode(GFX_FILLMODE _fillmode)
{
    switch(_fillmode)
    {
        case GFX_FILL_SOLID:        m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );break;
        case GFX_FILL_WIREFRAME:    m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );break;
        case GFX_FILL_POINT:        m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT );break;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::depthMask(u32 _depthMask)
{
    if (_depthMask)
        m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    else
        m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::depthFunc(u32 _Func)
{
#ifdef _USE_ZBUFFER_FLOAT
    //invert Z with floating Zbuffer for better rejection. so we nedd to invert Zfunc too
    switch(_Func)
    {
    case GFX_CMP_NEVER:          m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_NEVER); break;
    case GFX_CMP_LESS:           m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER); break;
    case GFX_CMP_EQUAL:          m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL); break;
    case GFX_CMP_LESSEQUAL:      m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL); break;
    case GFX_CMP_GREATER:        m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS); break;
    case GFX_CMP_NOTEQUAL:       m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_NOTEQUAL); break;
    case GFX_CMP_GREATEREQUAL:   m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL); break;
    case GFX_CMP_ALWAYS:         m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS); break;
    }
#else
    switch(_Func)
    {
    case GFX_CMP_NEVER:          m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_NEVER); break;
    case GFX_CMP_LESS:           m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS); break;
    case GFX_CMP_EQUAL:          m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL); break;
    case GFX_CMP_LESSEQUAL:      m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL); break;
    case GFX_CMP_GREATER:        m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER); break;
    case GFX_CMP_NOTEQUAL:       m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_NOTEQUAL); break;
    case GFX_CMP_GREATEREQUAL:   m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL); break;
    case GFX_CMP_ALWAYS:         m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS); break;
    }
#endif
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::depthTest(u32 _Test)
{
    if (_Test)
        m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    else
        m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
}
//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::depthBias(f32 _offset, f32 slope)
{
#ifdef _USE_ZBUFFER_FLOAT
    //invert Z with floating Zbuffer for better rejection.
    _offset *= -1;
#endif
    m_pd3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS , *(u32*)&slope);
    m_pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS , *(u32*)&_offset);
}

//----------------------------------------------------------------------------//
void GFXAdapter_Directx9::depthStencil(bbool stencilTestEnable, GFX_CMPFUNC stencilFunc,
                                       GFX_STENCILFUNC stencilZPass, GFX_STENCILFUNC stencilZFail, GFX_STENCILFUNC stencilFail,
                                       u8 ref, u8 preMask, u8 writeMask)
{
    m_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE , stencilTestEnable?TRUE:FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, (D3DCMPFUNC)stencilFunc);
    m_pd3dDevice->SetRenderState(D3DRS_STENCILPASS, (D3DSTENCILOP)stencilZPass);
    m_pd3dDevice->SetRenderState(D3DRS_STENCILZFAIL, (D3DSTENCILOP)stencilZFail);
    m_pd3dDevice->SetRenderState(D3DRS_STENCILFAIL, (D3DSTENCILOP)stencilFail);
    m_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, preMask);
    m_pd3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, writeMask);
    m_pd3dDevice->SetRenderState(D3DRS_STENCILREF, ref);
}
 
//----------------------------------------------------------------------------//
void GFXAdapter_Directx9::colorMask(GFX_COLOR_MASK_FLAGS _colorMask)
{
    u32 mask = 0;
    if ( _colorMask & GFX_COL_MASK_RED )
        mask |= D3DCOLORWRITEENABLE_RED;
    if ( _colorMask & GFX_COL_MASK_GREEN )
        mask |= D3DCOLORWRITEENABLE_GREEN;
    if ( _colorMask & GFX_COL_MASK_BLUE )
        mask |= D3DCOLORWRITEENABLE_BLUE;
    if ( _colorMask & GFX_COL_MASK_ALPHA )
        mask |= D3DCOLORWRITEENABLE_ALPHA;

    m_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, mask);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setScissorRect(GFX_RECT* _clipRect)
{
    if (_clipRect)
    {
        // setup clip region
        RECT clip;
        clip.left   = static_cast<LONG>(_clipRect->left);
        clip.top    = static_cast<LONG>(_clipRect->top);
        clip.right  = static_cast<LONG>(_clipRect->right);
        clip.bottom = static_cast<LONG>(_clipRect->bottom);
#if defined(ASSERT_ENABLED)
        GFX_Viewport vp;
        getViewport(vp);
        ITF_ASSERT(vp.m_x<=clip.left);
        ITF_ASSERT(vp.m_x+vp.m_width>=clip.right);
        ITF_ASSERT(vp.m_y<=clip.top);
        ITF_ASSERT(vp.m_y+vp.m_height>=clip.bottom);
#endif

        m_pd3dDevice->SetScissorRect(&clip);

        m_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
    }
    else
    {
        m_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setClipPlane(bbool _enable, f32 _ax, f32 _by, f32 _cz, f32 _dw)
{
    if(_enable)
    {
        m_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0x1);

        //Transform plane because the plane equations are assumed to be in the clipping space (the same space as output vertices). 
        Matrix44 invView = getWorldViewProjMatrix()->getViewProjInv();
        Matrix44 viewProjIT;
        viewProjIT.transpose(invView);

        D3DXPLANE plane = D3DXPLANE(_ax, _by, _cz, _dw);
        D3DXPLANE planeOut;
        D3DXPlaneTransform(&planeOut, &plane, reinterpret_cast<const D3DXMATRIX *>(&viewProjIT)); 

        m_pd3dDevice->SetClipPlane(0, (float*)&planeOut);
    }
    else
    {
        m_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0x0);
    }

}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::SetTextureBind(ux _Sampler, BasePlatformTexture *_Bind, bbool _linearFiltering)
{
    ITF_ASSERT_CRASH(_Sampler<MAX_SAMPLER, "Incorrect sampler index");
	PlatformTexture *platformTex = static_cast<PlatformTexture *>(_Bind);

    // cache texture change
    if (m_textureBinded[_Sampler] == _Bind && m_textureBindedFiltering[_Sampler] == _linearFiltering)
        return;

    int filtering = D3DTEXF_LINEAR;
    if (!_linearFiltering)
        filtering = D3DTEXF_POINT;
    m_pd3dDevice->SetSamplerState(_Sampler, D3DSAMP_MINFILTER, filtering);
    m_pd3dDevice->SetSamplerState(_Sampler, D3DSAMP_MAGFILTER, filtering);
    m_pd3dDevice->SetSamplerState(_Sampler, D3DSAMP_MIPFILTER, filtering);

#ifdef ITF_X360
    m_pd3dDevice->SetSamplerState(_Sampler, D3DSAMP_TRILINEARTHRESHOLD,D3DTRILINEAR_THREEEIGHTHS);
#endif
	// Unbind previous texture.
	u32 bit = 1UL << _Sampler;
	if ( ( _Sampler == 4 ) && platformTex )
	{
		platformTex->m_binding |= bit;
	}
	PlatformTexture *oldPlatformTex = m_textureBinded[_Sampler];
	if ( oldPlatformTex )
		oldPlatformTex->m_binding &= ~bit;

	// Bind new texture.
	if ( platformTex )
	{
		ITF_ASSERT( (platformTex->m_flags & GFX_TEXFLAG_BIND_AS_TARGET) == 0 );
		m_pd3dDevice->SetTexture( _Sampler, platformTex->m_DXTexture );
		platformTex->m_binding |= bit;
	}
	else
		m_pd3dDevice->SetTexture( _Sampler, nullptr );
    m_textureBinded[_Sampler] = platformTex;
    m_textureBindedFiltering[_Sampler] = _linearFiltering;
}

void GFXAdapter_Directx9::unbindAllSampler(PlatformTexture *_texture)
{
	u32 binding = _texture->m_binding;
	i32 i = 0;
	while ( binding )
	{
		if ( binding & 1 )
		{
			ITF_ASSERT(m_textureBinded[i] == _texture);
			m_pd3dDevice->SetTexture( i, NULL );
			m_textureBinded[i] = NULL;
		}
		binding >>= 1;
		i++;
	}
	_texture->m_binding = 0;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::fillTexture(   Texture* _tex, u8 * _buffer, u32 _pitch, u32 _height )
{
    if ( !_tex ) return;
	PlatformTexture *platformTex = getPlatformTexture(_tex);
    if ( !platformTex->m_DXTexture ) return;

    D3DLOCKED_RECT     rect;
    LPDIRECT3DTEXTURE9 d3dTex = platformTex->m_DXTexture;

    d3dTex->LockRect(0, &rect, 0, 0);
    // copy data from buffer into texture
    for (u32 i = 0; i < _height; ++i)
    {
        ITF_MEMCOPY((BYTE *)rect.pBits + (i*rect.Pitch), _buffer + (i*_pitch), _pitch);
    }
    d3dTex->UnlockRect(0);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::lockTexture( Texture* _tex, LOCKED_TEXTURE* _lockTex, u32 _flag )
{
    if ( !_tex ) return;
	PlatformTexture *platformTex = getPlatformTexture(_tex);
    if ( !platformTex->m_DXTexture ) return;

    D3DLOCKED_RECT lockTextureRect;
    IDirect3DTexture9*  pDXTexture = platformTex->m_DXTexture;
    HRESULT hr = pDXTexture->LockRect( 0, &lockTextureRect, NULL, _flag );
    ITF_ASSERT(hr == D3D_OK);

    _lockTex->mp_Bits = (DWORD*)(lockTextureRect.pBits);
    _lockTex->m_pitch = lockTextureRect.Pitch;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::unlockTexture( Texture* _tex)
{
	PlatformTexture *platformTex = getPlatformTexture(_tex);
    IDirect3DTexture9*  pDXTexture = platformTex->m_DXTexture;
    HRESULT hr = pDXTexture->UnlockRect( 0 );
    ITF_ASSERT(hr == D3D_OK);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setupViewport(GFX_RECT* _Viewport)
{
    D3DVIEWPORT9 vp;

#ifdef ITF_PC
    vp.X = static_cast<DWORD>(_Viewport->left) + getViewPortOffsetX();
    vp.Y = static_cast<DWORD>(_Viewport->top) + getViewPortOffsetY();
#else //ITF_PC
    vp.X = static_cast<DWORD>(_Viewport->left);
    vp.Y = static_cast<DWORD>(_Viewport->top);
#endif //ITF_PC
    vp.Width = static_cast<DWORD>(_Viewport->right - _Viewport->left);
    vp.Height = static_cast<DWORD>(_Viewport->bottom - _Viewport->top);
    vp.MinZ = internalDepth(0.0f);
    vp.MaxZ = internalDepth(1.0f);
    m_pd3dDevice->SetViewport(&vp);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::unprojectPoint(const Vec2d& _in, float _viewDistance, const Matrix44* _MatrixModel, const Matrix44* _MatrixProj, const int* _View, Vec2d& _out)
{
    D3DVIEWPORT9 vp;
    vp.X = static_cast<DWORD>(_View[0]);
    vp.Y = static_cast<DWORD>(_View[1]);
    vp.Width = static_cast<DWORD>(_View[2]);
    vp.Height = static_cast<DWORD>(_View[3]);
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;

    D3DXVECTOR3 in_vec;
    in_vec.z = 0.0f;

    // project points to create a plane orientated with GeometryBuffer's data
    D3DXVECTOR3 p1;
    D3DXVECTOR3 p2;
    D3DXVECTOR3 p3;
    in_vec.x = 0;
    in_vec.y = 0;
    D3DXVec3Project(&p1, &in_vec, &vp, (D3DXMATRIX*)_MatrixProj, 0, (D3DXMATRIX*)_MatrixModel); 

    in_vec.x = 1;
    in_vec.y = 0;
    D3DXVec3Project(&p2, &in_vec, &vp, (D3DXMATRIX*)_MatrixProj, 0, (D3DXMATRIX*)_MatrixModel); 

    in_vec.x = 0;
    in_vec.y = 1;
    D3DXVec3Project(&p3, &in_vec, &vp, (D3DXMATRIX*)_MatrixProj, 0, (D3DXMATRIX*)_MatrixModel); 

    // create plane from projected points
    D3DXPLANE surface_plane;
    D3DXPlaneFromPoints(&surface_plane, &p1, &p2, &p3);

    // unproject ends of ray
    in_vec.x = vp.Width * 0.5f;
    in_vec.y = vp.Height * 0.5f;
    in_vec.z = -_viewDistance;
    D3DXVECTOR3 t1;
    D3DXVec3Unproject(&t1, &in_vec, &vp, (D3DXMATRIX*)_MatrixProj, 0, (D3DXMATRIX*)_MatrixModel); 

    in_vec.x = _in.x();
    in_vec.y = _in.y();
    in_vec.z = 0.0f;
    D3DXVECTOR3 t2;
    D3DXVec3Unproject(&t2, &in_vec, &vp, (D3DXMATRIX*)_MatrixProj, 0, (D3DXMATRIX*)_MatrixModel); 

    // get intersection of ray and plane
    D3DXVECTOR3 intersect;
    D3DXPlaneIntersectLine(&intersect, &surface_plane, &t1, &t2);

    _out.x() = intersect.x;
    _out.y() = intersect.y;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setLookAtMatrix(Matrix44* _MatrixOut, double _midx, double _midy, float _viewDistance, float _aspect)
{
    const float fov = 0.523598776f;

    f32 midx =(f32)_midx; 
    f32 midy =(f32)_midy; 
    f32 viewDistance = (f32)  _viewDistance;
    f32 aspect = (f32) _aspect;
    
    D3DXVECTOR3 eye(midx, midy, -viewDistance);
    D3DXVECTOR3 at(midx, midy, 1);
    D3DXVECTOR3 up(0, -1, 0);

    D3DXMATRIX tmp;
    D3DXMatrixMultiply((D3DXMATRIX*)_MatrixOut,
        D3DXMatrixLookAtRH((D3DXMATRIX*)_MatrixOut, &eye, &at, &up),
        D3DXMatrixPerspectiveFovRH(&tmp, fov, aspect,
                                   viewDistance * 0.5f,
                                   viewDistance * 2.0f));
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::getViewport(GFX_Viewport & _vp)
{
    D3DVIEWPORT9 vp;
    m_pd3dDevice->GetViewport(&vp);

    _vp.m_x = vp.X;
    _vp.m_y =  vp.Y;
    _vp.m_width = vp.Width;
    _vp.m_height = vp.Height;
    _vp.m_minZ = internalDepth(vp.MinZ);
    _vp.m_maxZ = internalDepth(vp.MaxZ);
}

//----------------------------------------------------------------------------//

Matrix44 GFXAdapter_Directx9::convertM44ToSpecificPlatformGPU(const Matrix44 &_in)
{
    Matrix44 trans;
    D3DXMatrixTranspose((D3DXMATRIX*)&trans, (D3DXMATRIX*)&_in);
    return trans;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setInternalRenderTarget (LPDIRECT3DSURFACE9 _RT, LPDIRECT3DSURFACE9 _ZRT)
{
	// Unbind previous target.
	for ( ux i = 0; i < 2; i++ )
	{
		if ( m_currentRenderTarget[i] )
		{
			m_currentRenderTarget[i]->m_flags &= ~GFX_TEXFLAG_BIND_AS_TARGET;
			m_currentRenderTarget[i] = NULL;
		}
	}
	if ( m_currentDepthBuffer )
	{
		m_currentDepthBuffer->m_flags &= ~GFX_TEXFLAG_BIND_AS_TARGET;
		m_currentDepthBuffer = NULL;
	}

	m_pd3dDevice->SetRenderTarget( 0, _RT);
    m_pd3dDevice->SetRenderTarget( 1, NULL );
    m_pd3dDevice->SetDepthStencilSurface(_ZRT);

    m_currentRenderingSurface = _RT;

    //in Xbox360, need to setViewport after a setRenderTarget
    D3DSURFACE_DESC desc;
    _RT->GetDesc(&desc);
    GFX_RECT rcViewport;
    rcViewport.left = 0;
    rcViewport.right = desc.Width;
    rcViewport.top =  0;
    rcViewport.bottom = desc.Height;
    setupViewport(&rcViewport);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::createTexture( Texture* _texture, u32 _sizeX, u32 _sizeY, u32 _mipLevel, Texture::PixFormat _pixformat, u32 _pool, bbool _dynamic )
{
	PlatformTexture *platformTex = getPlatformTexture(_texture);

	LPDIRECT3DTEXTURE9 d3dTex = platformTex->m_DXTexture;
    
    D3DFORMAT pformat = D3DFMT_A8B8G8R8;
    D3DPOOL pool = m_useD3D9Ex?D3DPOOL_DEFAULT:D3DPOOL_MANAGED;

    if (_pool == 0)
        pool = D3DPOOL_DEFAULT;
    else if (_pool == 2)
        pool = D3DPOOL_SYSTEMMEM;

	pformat = GFXPixFormatToD3D(_pixformat);

#ifdef ITF_X360
    // Compute remapping
    pformat = computeSwizzleFormat(pformat, _texture->getRemap());
#endif //ITF_X360

    u32 usage = 0;
#ifdef ITF_WINDOWS
    if (_dynamic)
        usage = D3DUSAGE_DYNAMIC;
    if(_pool == 3)
    {
        usage = D3DUSAGE_RENDERTARGET;
        pool = D3DPOOL_DEFAULT;
    }
#endif

    m_mxDeviceLost.lock();
    HRESULT hr;
    hr = DXCreateTexture( _sizeX, _sizeY, _mipLevel, usage, pformat, pool, &d3dTex);

    if (FAILED(hr))
    {
        LOG("Direct3D9Texture::createTexture failed."); 
        return;
    }
    m_mxDeviceLost.unlock();

    _texture->setSize(_sizeX, _sizeY);
    _texture->m_pixFormat = _pixformat;
    _texture->m_mipMapLevels  = _mipLevel;
    platformTex->m_DXTexture = d3dTex;
}

RenderTarget* GFXAdapter_Directx9::createRenderTarget( RenderTargetInfo &_info )
{
	RenderTarget *rt = newAlloc(mId_Textures, RenderTarget(_info));
	GFX_RenderingBuffer_DX9 *platformRT = newAlloc(mId_Textures, GFX_RenderingBuffer_DX9);

	D3DFORMAT formatDX = GFXPixFormatToD3DNoLin(_info.m_format);

	GFX_RenderingBuffer_DX9 *srcBaseRT = (GFX_RenderingBuffer_DX9 *)(_info.m_srcBaseRT ? _info.m_srcBaseRT->m_adapterimplementationData : NULL);
	GFX_RenderingBuffer_DX9 *linkedRT = (GFX_RenderingBuffer_DX9 *)(_info.m_linkedRT ? _info.m_linkedRT->m_adapterimplementationData : NULL);
	platformRT->createBuffer(_info.m_width, _info.m_heigth, formatDX, 0, _info.m_EDRAMOffset, srcBaseRT, linkedRT);

	rt->m_adapterimplementationData = platformRT;

	return rt;
}

// For back buffer.
RenderTarget* GFXAdapter_Directx9::createRenderTarget( IDirect3DSurface9 *_surf )
{
	D3DSURFACE_DESC surfDesc;
	_surf->GetDesc(&surfDesc);
	Texture::PixFormat pixformat = D3DToGFXPixFormat(surfDesc.Format);
	RenderTargetInfo info(surfDesc.Width, surfDesc.Height, 1, pixformat);

	RenderTarget *rt = newAlloc(mId_Textures, RenderTarget(info));
	GFX_RenderingBuffer_DX9 *platformRT = newAlloc(mId_Textures, GFX_RenderingBuffer_DX9);
	platformRT->m_DXSurface = _surf;
	platformRT->m_DXTexture = NULL;
	platformRT->m_flags = 0;
	platformRT->m_width = surfDesc.Width;
	platformRT->m_height = surfDesc.Height;
	platformRT->m_pixFmt = surfDesc.Format;

	rt->m_adapterimplementationData = platformRT;

	return rt;
}


void GFXAdapter_Directx9::setRenderContext( RenderContext &_context )
{
	for (ux i = 0; i < _context.m_colorBufferCount; ++i)
	{
		// Unbind previous target.
		if ( m_currentRenderTarget[i] )
			m_currentRenderTarget[i]->m_flags &= ~GFX_TEXFLAG_BIND_AS_TARGET;

		if ( _context.m_colorBuffers[i] )
		{
			// Bind new target.
			GFX_RenderingBuffer_DX9 *colorBufferDX9 = static_cast<GFX_RenderingBuffer_DX9 *>(_context.m_colorBuffers[i]->m_adapterimplementationData);
			if ( colorBufferDX9->m_binding )
				unbindAllSampler(colorBufferDX9);
			if ( i == 0 )
				m_currentRenderingSurface = colorBufferDX9->getSurface();

			m_currentRenderTarget[i] = colorBufferDX9;
			m_pd3dDevice->SetRenderTarget( i, colorBufferDX9->getSurface() );
			colorBufferDX9->m_flags |= GFX_TEXFLAG_BIND_AS_TARGET;
		}
		else
		{
			m_currentRenderTarget[i] = NULL;
			m_pd3dDevice->SetRenderTarget( i, NULL);
		}
	}

	// Unbind previous target.
	if ( m_currentDepthBuffer )
		m_currentDepthBuffer->m_flags &= ~GFX_TEXFLAG_BIND_AS_TARGET;

	if ( _context.m_depthBuffer )
	{
		GFX_RenderingBuffer_DX9 *depthBufferDX9 = static_cast<GFX_RenderingBuffer_DX9 *>(_context.m_depthBuffer->m_adapterimplementationData);
		m_pd3dDevice->SetDepthStencilSurface(depthBufferDX9->getSurface());
		m_currentDepthBuffer = depthBufferDX9;
		depthBufferDX9->m_flags |= GFX_TEXFLAG_BIND_AS_TARGET;
	}
	else
	{
		m_pd3dDevice->SetDepthStencilSurface(NULL);
		m_currentDepthBuffer = NULL;
	}

	if ( _context.m_useViewport )
	{
		D3DVIEWPORT9 vp;
		vp.X = static_cast<DWORD>(_context.m_viewport.m_x);
		vp.Y = static_cast<DWORD>(_context.m_viewport.m_y);
		vp.Width = static_cast<DWORD>(_context.m_viewport.m_width);
		vp.Height = static_cast<DWORD>(_context.m_viewport.m_height);
		vp.MinZ = _context.m_viewport.m_minZ;
		vp.MaxZ = _context.m_viewport.m_maxZ;
		m_pd3dDevice->SetViewport(&vp);
	}
#ifdef ITF_X360
	else if ( _context.m_colorBuffers[0] )
	{
		D3DVIEWPORT9 vp;
		vp.X = 0;
		vp.Y = 0;
		vp.Width = _context.m_colorBuffers[0]->getSizeX();
		vp.Height = _context.m_colorBuffers[0]->getSizeY();
		vp.MinZ = 0.0f;
		vp.MaxZ = 1.0f;
		m_pd3dDevice->SetViewport(&vp);
	}
#endif
}

void GFXAdapter_Directx9::prepareLockableRTAccess( RenderTarget* _target)
{
#ifdef ITF_WINDOWS
	GFX_RenderingBuffer_DX9 *colorBufferDX9 = static_cast<GFX_RenderingBuffer_DX9 *>(_target->m_adapterimplementationData);

	if ( !colorBufferDX9->m_DXLockableSurface )
	{
		// Create lockable surface if needed.
		m_mxDeviceLost.lock();
		HRESULT hr;
		D3DFORMAT pixFormat = colorBufferDX9->getFormat();
		if ( pixFormat == D3DFMT_A8B8G8R8 )
			pixFormat = D3DFMT_A8R8G8B8;
		hr = DXCreateOffscreenPlainSurface(_target->getSizeX(), _target->getSizeY(), pixFormat, D3DPOOL_SYSTEMMEM, &colorBufferDX9->m_DXLockableSurface, NULL);
		if (hr!=S_OK)
			messageD3DError(hr);
		m_mxDeviceLost.unlock();
	}

    HRESULT  hr = m_pd3dDevice->GetRenderTargetData( colorBufferDX9->m_DXSurface,colorBufferDX9->m_DXLockableSurface);
    ITF_ASSERT(hr == S_OK);
#endif
}

void GFXAdapter_Directx9::lockRenderTarget(	RenderTarget* _target, LOCKED_TEXTURE *_lockTex)
{
#ifdef ITF_WINDOWS
	GFX_RenderingBuffer_DX9 *colorBufferDX9 = static_cast<GFX_RenderingBuffer_DX9 *>(_target->m_adapterimplementationData);
    IDirect3DSurface9* surfBuffer = colorBufferDX9->m_DXLockableSurface;
    D3DLOCKED_RECT lockedrect;
    HRESULT  hr = surfBuffer->LockRect(&lockedrect,NULL,D3DLOCK_READONLY|D3DLOCK_NOSYSLOCK);
    ITF_ASSERT_MSG(D3D_OK == hr, "DX9::LockRect failed");
    _lockTex->m_pitch = lockedrect.Pitch;
    _lockTex->mp_Bits = lockedrect.pBits;
#endif
}

void GFXAdapter_Directx9::unlockRenderTarget(RenderTarget* _target)
{
#ifdef ITF_WINDOWS
	GFX_RenderingBuffer_DX9 *colorBufferDX9 = static_cast<GFX_RenderingBuffer_DX9 *>(_target->m_adapterimplementationData);
    IDirect3DSurface9* surfBuffer = colorBufferDX9->m_DXLockableSurface;
    surfBuffer->UnlockRect();
#endif
}

void GFXAdapter_Directx9::cleanupTexture(Texture* _texture)
{
    if (!_texture)
		return;

    PlatformTexture *platformTex = (PlatformTexture *)_texture->m_adapterimplementationData;
    if (platformTex && platformTex->m_DXTexture)
    {
#ifdef USE_TEXTURE_TILE
                IDirect3DBaseTexture9* basetexture = static_cast< IDirect3DBaseTexture9* >( platformTex->m_DXTexture );
                void* textureData = (void*)(basetexture->Format.BaseAddress << 12);
        
                slotAllocatorManager::deallocatePhysicalMemory( textureData );
                //basetexture->Release();
                delete basetexture;
#else
        LPDIRECT3DTEXTURE9 tex = platformTex->m_DXTexture;
        DXReleaseTexture(&tex);
#endif           

        platformTex->m_DXTexture = nullptr;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::cleanupBaseTexture(Texture* _texture)
{
	PlatformTexture *platformTex = getPlatformTexture(_texture);
    if (platformTex && platformTex->m_DXTexture)
    {
        LPDIRECT3DTEXTURE9 tex = (LPDIRECT3DTEXTURE9)platformTex->m_DXTexture;
        DXReleaseTexture(&tex);
        platformTex->m_DXTexture = nullptr;
    }
}

//----------------------------------------------------------------------------//

unsigned char* GFXAdapter_Directx9::grabTexture(Texture* _texture)
{
    return 0;
}


//----------------------------------------------------------------------------//

u32 GFXAdapter_Directx9::getMaxTextureSize()
{
    return m_maxTextureSize;
}

//----------------------------------------------------------------------------//

Size GFXAdapter_Directx9::getAdjustedSize(const Size& sz)
{
    Size s(sz);

    if (!m_supportNPOTTex)
    {
        s.d_width  = static_cast<u32> (getSizeNextPOT((f32)sz.d_width));
        s.d_height = static_cast<u32> (getSizeNextPOT((f32)sz.d_height));
    }
    if (!m_supportNonSquareTex)
        s.d_width = s.d_height = Max(s.d_width, s.d_height);
    return s;
}

//----------------------------------------------------------------------------//

#define USE_MIPMAP

#ifndef ITF_FINAL
ITF_INLINE bbool isPowerOfTwo( u32 n )
{
    return (n & (n-1)) == 0;
}
#endif //ITF_FINAL

u32 getPixFmtBPP(u32 _pixFmt)
{
    switch(_pixFmt)
    {
        case D3DFMT_DXT1:				return 4;
        case D3DFMT_DXT3:				return 8;
        case D3DFMT_DXT5:				return 8;

        default:						return 0;
    }
}

u32 getBufferSize(u32 _width, u32 _height, u32 _pixelFormat, bbool _bTile32 = bfalse)
{
    u32 BPP = getPixFmtBPP(_pixelFormat);

    switch(_pixelFormat)
    {
        case D3DFMT_DXT1:
        case D3DFMT_DXT3:
        case D3DFMT_DXT5:
            {
                u8	limit = _bTile32 ? 8 : 4;
                u8	minTiles = _bTile32 ? 2 : 1;

                _width = (_width >= limit) ? _width >> 2 : minTiles;
                _height = (_height >= limit) ? _height >> 2 : minTiles;
            }

            return(_width * _height) * BPP * 2;
            break;
    }

    return 0;
}

void GFXAdapter_Directx9::loadFromMemory(const void* _buffer, const Size& _texture_size, Texture* _texture, Texture::PixFormat _pixel_format, bbool bCooked, u32 _dwMipMapCount)
{
    m_mxDeviceLost.lock();

    cleanupTexture(_texture);

	PlatformTexture *platformTex = getPlatformTexture(_texture);

    D3DFORMAT pixfmt = D3DFMT_UNKNOWN;
    u32 byte = 4;
    switch (_pixel_format)
    {
    case Texture::PF_RGB:
        ITF_ASSERT(0);
        break;

    case Texture::PF_DXT1:
         byte = 8;
        pixfmt = D3DFMT_DXT1;
        break;
    case Texture::PF_DXT3:
        pixfmt = D3DFMT_DXT3;
            byte = 16;
        break;
    case Texture::PF_DXT5:
        pixfmt = D3DFMT_DXT5;
            byte = 16;
        break;

    case Texture::PF_RGBA:
#ifdef ITF_WINDOWS
        pixfmt = D3DFMT_A8B8G8R8;
#else
        pixfmt = D3DFMT_LIN_A8R8G8B8;
#endif //ITF_WINDOWS
        break;
    default:
        LOG("Direct3D9Texture::loadFromMemory failed: Invalid PixelFormat value specified."); 
    }

    u32 levelMipmap = 1;

#ifdef USE_MIPMAP
        levelMipmap = 0;
#endif 

    Size tex_sz(getAdjustedSize(_texture_size));

    LPDIRECT3DTEXTURE9 d3dTex = platformTex->m_DXTexture;

    HRESULT hr = S_OK;

    if (bCooked)//textureDDS_pixel_format >= Texture::PF_DXT1 && _pixel_format <=Texture::PF_DXT5)
    {
        //Linear format x360
        D3DFORMAT formatCreate = pixfmt;
        
        unsigned long* currentBuffer = (unsigned long*)((u8*)_buffer+sizeof(ITF::DDS_HEADER));

#ifdef ITF_X360
        switch (pixfmt)
        {
        case D3DFMT_DXT1:
            formatCreate = D3DFMT_LIN_DXT1;
            break;
        case D3DFMT_DXT2:
            formatCreate = D3DFMT_LIN_DXT2;
            break;
    
        case D3DFMT_DXT4:
            formatCreate = D3DFMT_LIN_DXT4;
            break;
        }
        // Compute remapping
        formatCreate = computeSwizzleFormat(formatCreate, _texture->getRemap());
#endif //ITF_X360

        hr = D3DERR_NOTAVAILABLE;
        while(hr != D3D_OK)
        {
            u32 usage = 0;
            #ifdef ITF_WINDOWS
                usage = m_useD3D9Ex?D3DUSAGE_DYNAMIC:0;
            #endif
            hr = DXCreateTexture(
                static_cast<u32>(tex_sz.d_width),
                static_cast<u32>(tex_sz.d_height),
                _dwMipMapCount > 0 ? _dwMipMapCount : 1, usage, formatCreate, m_useD3D9Ex?D3DPOOL_DEFAULT:D3DPOOL_MANAGED, &d3dTex);
            if(hr == D3DERR_NOTAVAILABLE)
            {
                Sleep(500);
            }
        }

        u32 levelCount = d3dTex->GetLevelCount();

        for( u32 mipLevel = 0; mipLevel != levelCount; ++mipLevel )
        {
            IDirect3DSurface9* mip_surface;
            hr = DXGetSurfaceLevel(d3dTex, mipLevel, &mip_surface);
            ITF_ASSERT(hr == S_OK);

            D3DSURFACE_DESC pDesc;
            mip_surface->GetDesc(&pDesc);

            u32 width = pDesc.Width;
            u32 height = pDesc.Height;

            ITF_ASSERT(isPowerOfTwo(width));
            ITF_ASSERT(isPowerOfTwo(height));


            D3DLOCKED_RECT lr;
            hr = mip_surface->LockRect(&lr, 0,0);
            ITF_ASSERT(hr == S_OK);

            unsigned long* dst = static_cast<unsigned long*>(lr.pBits);

            u32 count = (Max((u32) 1, (u32)(width / 4)) * Max((u32)1, (u32) (height / 4)) *byte)/sizeof(unsigned long); //8(DXT1) or 16(DXT2-5)

            unsigned long* src = static_cast<unsigned long*>(currentBuffer);
            //produce here

            switch ( _pixel_format)
            {
            case Texture::PF_RGBA:
                {
					if ( u32(lr.Pitch) == width*sizeof(unsigned long) )
					{
                    ITF_MemcpyWriteCombined(dst,src,(width*height)*(sizeof(unsigned long)));
					}
					else
					{
                        u8* locked = (u8*)lr.pBits;
						u8* srcLine = (u8*)src;
						ux lineSize = width*sizeof(unsigned long);
						ux pitch = lr.Pitch;
                        for(u32 i = 0; i < height; i ++)
                        {
                            ITF_MemcpyWriteCombined(locked, srcLine, lineSize);
                            srcLine += lineSize;
                            locked += pitch;
                        }
					}
                    currentBuffer+= (width*height);
                }
                break;

            default:
                {
                   
#ifdef ITF_WINDOWS
                    ITF_MemcpyWriteCombined(dst,src,count*sizeof(unsigned long));
#else
                    ITF_ASSERT(count!=0);

                    u32 imageSize   = getBufferSize(width, height, pixfmt);
                    u32 lineSize    = getBufferSize(width,1,pixfmt);

                    if (lineSize == (u32) lr.Pitch)
                    {
                        ITF_MemcpyWriteCombined(dst, src, imageSize);
                    }
                    else if (lineSize <(u32)  lr.Pitch)
                    {
                        u8* locked = (u8*)lr.pBits;
                        for(u32 i = 0; i < imageSize; i += lineSize)
                        {
                            u8* p = ((u8*)src) + i;
                            ITF_MemcpyWriteCombined(locked, p, lineSize);
                            locked = ((u8 *)locked) + lr.Pitch;
                        }
                    }
                    else
                    {
                        ITF_ASSERT(0);
                    }

#endif //ITF_WINDOWS
                    currentBuffer+= count;
                }
                break;
            }

            mip_surface->UnlockRect();
            D3D_NM_RELEASE(mip_surface);
        }

        switch (hr)
        {
            case S_OK:
                hr=hr;
                break;
            default:
                ITF_ASSERT(0);//Wrong DDS Format;
                break;
        }

        platformTex->m_DXTexture = d3dTex;

        Size d_dataSize(_texture_size);
        updateTextureSize(_texture);
        Size TexSize(_texture->getSizeX(), _texture->getSizeY());
        updateCachedScaleValues(_texture);

        _texture->m_datasizeX = (u32)d_dataSize.d_width;
        _texture->m_datasizeY = (u32)d_dataSize.d_height;
        _texture->m_pixFormat = _pixel_format;
        
   }
   else
   {
       u32 usage = 0;
    #ifdef ITF_WINDOWS
       usage = m_useD3D9Ex?D3DUSAGE_DYNAMIC:0;
    #endif
       DXCreateTexture( static_cast<u32>(tex_sz.d_width),
           static_cast<u32>(tex_sz.d_height),
           levelMipmap, usage, pixfmt, m_useD3D9Ex?D3DPOOL_DEFAULT:D3DPOOL_MANAGED, &d3dTex);

        if (FAILED(hr))
        {
            LOG("Direct3D9Texture::loadFromMemory failed: Direct3D9 texture creation failed."); 
            m_mxDeviceLost.unlock();
            return;
        }
        platformTex->m_DXTexture = d3dTex;

        Size d_dataSize(_texture_size);
        updateTextureSize(_texture);
        Size TexSize(_texture->getSizeX(), _texture->getSizeY());
        updateCachedScaleValues(_texture);

        _texture->m_datasizeX = (u32)d_dataSize.d_width;
        _texture->m_datasizeY = (u32)d_dataSize.d_height;

        // lock the D3D texture

        D3DLOCKED_RECT rect;

        {
            hr = d3dTex->LockRect(0, &rect, 0, 0);

            if (FAILED(hr))
            {
                LOG("Direct3D9Texture::loadFromMemory failed: IDirect3DTexture9::LockRect failed."); 
            }
            else
            {
            // copy data from buffer into texture
            unsigned long* dst = static_cast<unsigned long*>(rect.pBits);
            const unsigned long* src = static_cast<const unsigned long*>(_buffer);

            // RGBA
            if (_pixel_format == Texture::PF_RGBA)
            {
                for (u32 i = 0; i < _texture_size.d_height; ++i)
                {
                    for (u32 j = 0; j < _texture_size.d_width; ++j)
                    {
                        // we dont need endian safety on microsoft
                        u8 r = static_cast<u8>(src[j] & 0xFF);
                        u8 g = static_cast<u8>((src[j] >> 8) & 0xFF);
                        u8 b = static_cast<u8>((src[j] >> 16)  & 0xFF);
                        u8 a = static_cast<u8>((src[j] >> 24) & 0xFF);

                        dst[j] = D3DCOLOR_ARGB(a, r, g, b);
                    }

                    dst += rect.Pitch / sizeof(unsigned long);
                    src += static_cast<unsigned long>(_texture_size.d_width);
                }
            }
            // RGB
            else
            {
                for (u32 i = 0; i < _texture_size.d_height; ++i)
                {
                    for (u32 j = 0; j < _texture_size.d_width; ++j)
                    {
                        u8 r = reinterpret_cast<const u8*>(src)[j * 3];
                        u8 g = reinterpret_cast<const u8*>(src)[j * 3 + 1];
                        u8 b = reinterpret_cast<const u8*>(src)[j * 3 + 2];
                        u8 a = 0xFF;

                        dst[j] = D3DCOLOR_ARGB(a, r, g, b);
                    }

                    dst += rect.Pitch / sizeof(unsigned long);
                    src = reinterpret_cast<const unsigned long*>
                          (reinterpret_cast<const u8*>(src) +
                           static_cast<int>(_texture_size.d_width) * 3);
                }
            }
            d3dTex->UnlockRect(0);
        }
        }

        if (levelMipmap != 1)
        {
            hr = D3DXFilterTexture(d3dTex,NULL,D3DX_DEFAULT , D3DX_FILTER_BOX | D3DX_FILTER_DITHER  );
            ITF_ASSERT(hr == S_OK);
        }
    }
    m_mxDeviceLost.unlock();

}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::updateTextureSize(Texture* _Tex)
{
    D3DSURFACE_DESC surfDesc;
	PlatformTexture *platformTex = getPlatformTexture( _Tex );

    // obtain details of the size of the texture
    if (platformTex->m_DXTexture && SUCCEEDED(platformTex->m_DXTexture->GetLevelDesc(0, &surfDesc)))
    {
        _Tex->setSize(static_cast<u32>(surfDesc.Width),static_cast<u32>(surfDesc.Height));
    }
    // use the original size if query failed.
    // NB: This should probably be an exception.
    else
    {
        _Tex->setSize(static_cast<u32>( _Tex->m_datasizeX),static_cast<u32>(_Tex->m_datasizeY));
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::saveToMemory(void* _buffer, Texture* _Tex)
{
    LOG("Direct3D9Texture::saveToMemory - Unimplemented!"); 
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setSamplerState( u32 _samp, GFX_SAMPLERSTATETYPE _type, i32 _value )
{
    i32 value = 0;
    i32 type = 0;

    switch (_type)
    {
    case GFXSAMP_ADDRESSU:
        type = D3DSAMP_ADDRESSU;
        break;
    case GFXSAMP_ADDRESSV:
        type = D3DSAMP_ADDRESSV;
        break;
    case GFXSAMP_MAGFILTER:
        type = D3DSAMP_MAGFILTER;
        break;
    case GFXSAMP_MINFILTER:
        type = D3DSAMP_MINFILTER;
        break;
    case GFXSAMP_MIPFILTER:
        type = D3DSAMP_MIPFILTER;
        break;
    default:
        // not supported !!.
        ITF_ASSERT(0);
        break;
    }

    switch (_value)
    {
    default:
    case GFX_TEXADRESSMODE_WRAP:
        value = D3DTADDRESS_WRAP;
        break;
    case GFX_TEXADRESSMODE_MIRROR:
        value = D3DTADDRESS_MIRROR;
        break;
    case GFX_TEXADRESSMODE_CLAMP:
        value = D3DTADDRESS_CLAMP;
        break;
    case GFX_TEXADRESSMODE_BORDER:
        value = D3DTADDRESS_BORDER;
        break;
    }

    m_pd3dDevice->SetSamplerState((DWORD)_samp, (D3DSAMPLERSTATETYPE)type, (DWORD)value);
}

template <D3DSAMPLERSTATETYPE samplerAddressModeState>
void GFXAdapter_Directx9::setTextureAdressingMode( u32 _samp, GFX_TEXADRESSMODE _texAdressMode)
{
    DWORD addrModeDXValue = D3DTADDRESS_WRAP;
    switch (_texAdressMode)
    {
    default:
    case GFX_TEXADRESSMODE_WRAP:
        addrModeDXValue = D3DTADDRESS_WRAP;
        break;
    case GFX_TEXADRESSMODE_MIRROR:
        addrModeDXValue = D3DTADDRESS_MIRROR;
        break;
    case GFX_TEXADRESSMODE_CLAMP:
        addrModeDXValue = D3DTADDRESS_CLAMP;
        break;
    case GFX_TEXADRESSMODE_BORDER:
        addrModeDXValue = D3DTADDRESS_BORDER;
        break;
    }

    m_pd3dDevice->SetSamplerState(_samp, samplerAddressModeState, addrModeDXValue);
}


void GFXAdapter_Directx9::setTextureAdressingMode(u32 _samp, GFX_TEXADRESSMODE _texAdressModeU, GFX_TEXADRESSMODE _texAdressModeV)
{
    setTextureAdressingMode<D3DSAMP_ADDRESSU>(_samp, _texAdressModeU);
    setTextureAdressingMode<D3DSAMP_ADDRESSV>(_samp, _texAdressModeV);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::createRenderTarget(LPDIRECT3DTEXTURE9* _ptex, LPDIRECT3DSURFACE9* _psurf, u32 _width, u32 height, D3DFORMAT _format, LPDIRECT3DSURFACE9 _shiftSurf, IDirect3DTexture9 *_pSrcBaseTexture, IDirect3DTexture9 *_pLinkedTexture)
{
    HRESULT hr;
    ITF_ASSERT_CRASH(_ptex,"pText createRenderTarget is null");
    m_mxDeviceLost.lock();
    *_ptex = nullptr;

#ifdef ITF_WINDOWS
    hr = DXCreateTexture(_width, height, 1, D3DUSAGE_RENDERTARGET, _format, D3DPOOL_DEFAULT, _ptex );
    ITF_ASSERT_CRASH(*_ptex,"*pText createRenderTarget is null");

    DXGetSurfaceLevel((*_ptex), 0, _psurf);

    /// clear rendertarget.
    LPDIRECT3DSURFACE9 oldSurface;
    m_pd3dDevice->GetRenderTarget(0, &oldSurface);
    LPDIRECT3DSURFACE9 oldDepthSurf = NULL;
    m_pd3dDevice->GetDepthStencilSurface(&oldDepthSurf);

    setInternalRenderTarget(*_psurf, NULL);
    clear(GFX_CLEAR_COLOR, (u32)0x00000000);
    setInternalRenderTarget(oldSurface, oldDepthSurf);
    oldSurface->Release();
    if (oldDepthSurf)
        oldDepthSurf->Release();

#else
    if(_pSrcBaseTexture == NULL)
    {
        hr = DXCreateTexture( _width, height, 1, 0,
            _format,
            D3DPOOL_DEFAULT, _ptex);
    }
    else
    {
        hr = DXCreateLinkTexture( _pSrcBaseTexture, _pLinkedTexture, _width, height, 1, 0,
            _format,
            D3DPOOL_DEFAULT, _ptex);
    }

    ITF_ASSERT_CRASH(*_ptex,"*pText createRenderTarget is null");

    D3DSURFACE_PARAMETERS SurfaceParameters;
    ITF_MemSet( &SurfaceParameters, 0, sizeof( D3DSURFACE_PARAMETERS ) );

    if (_shiftSurf != NULL)
    {
        const u32 TileTargetSizeBytes = _shiftSurf->Size;
        SurfaceParameters.Base = TileTargetSizeBytes / GPU_EDRAM_TILE_SIZE;
        SurfaceParameters.HierarchicalZBase = 0;
     }

    hr = DXCreateRenderTarget( _width, height, _format,
                                  D3DMULTISAMPLE_NONE, 0, FALSE, _psurf,
                                  &SurfaceParameters );
#endif
    m_mxDeviceLost.unlock();

}

void* GFXAdapter_Directx9::enableTextureDyn(TextureDyn* _texture)
{
    ITF_ASSERT(_texture);
    _texture->swap();
    LPDIRECT3DSURFACE9 oldsurf = getCurrentBackBufferSurf();
    setInternalRenderTarget((LPDIRECT3DSURFACE9) _texture->m_surfaceLevel, getCurrentDepthStencilSurf());
    return oldsurf;
}

void GFXAdapter_Directx9::disableTextureDyn(TextureDyn* _texture, void* _oldSurf)
{
    setInternalRenderTarget((LPDIRECT3DSURFACE9)_oldSurf, getCurrentDepthStencilSurf());
}

void GFXAdapter_Directx9::getTextureDynSurfaceLevel(TextureDyn* _texture )
{
    LPDIRECT3DSURFACE9 surf;
	PlatformTexture *platformTex = getPlatformTexture( _texture );
    DXGetSurfaceLevel(platformTex->m_DXTexture, 0, &surf);
    _texture->m_surfaceLevel = surf;
}

void GFXAdapter_Directx9::releaseTextureDynSurfaceLevel(TextureDyn* _texture)
{
    LPDIRECT3DSURFACE9 surf = (LPDIRECT3DSURFACE9)_texture->m_surfaceLevel;
    D3D_NM_RELEASE(surf);
    _texture->m_surfaceLevel = NULL;
}

//----------------------------------------------------------------------------//

#ifdef ITF_X360
void  GFXAdapter_Directx9::acquireDeviceOwnerShip()
{

    if (Synchronize::getCurrentThreadId() != ThreadSettings::m_settings[eThreadId_mainThread].m_threadID && m_pd3dDevice)
    {
        //ask the main thread to release the owner ship
        {
            ITF_MemoryBarrier();
            {
                csAutoLock cs(m_csPresent);
                m_askToReleaseOwnerShip = btrue;
            }
            //wait until is done;
            Synchronize::waitEvent(&m_ReleaseOwnerShipEvent);
    
            Synchronize::resetEvent(&m_ReleaseOwnerShipEvent);
            m_pd3dDevice->AcquireThreadOwnership();
        }
    }

}

void GFXAdapter_Directx9::releaseDeviceOwnerShip()
{
    if (Synchronize::getCurrentThreadId() != ThreadSettings::m_settings[eThreadId_mainThread].m_threadID && m_pd3dDevice)
    {
        Synchronize::setEvent(&m_AcquireOwnerShipEvent);
        m_pd3dDevice->ReleaseThreadOwnership();
    }
 
    
}

void GFXAdapter_Directx9::minimal_present()
{
    if (m_pd3dDevice)
    {
        clear( GFX_CLEAR_COLOR, 0, 0, 0, 0); 
        m_pd3dDevice->Present(0, 0, 0, 0);
    }
}
#endif //ITF_X360



#ifdef ITF_USE_REMOTEVIEW
void GFXAdapter_Directx9::computeEngineViewRect(RECT& _mainOrigin, RECT& _mainTop, RECT& _remoteOrigin, RECT& _remoteBottom)
{
    const u32 subScreenWidth = getWindowWidth();
    const u32 subScreenHeight = (u32)getWindowHeight()/2;

    //MAIN
    const u32 mainWidht = m_BackBufferScreen[e_engineView_MainScreen]->getSizeX();
    const u32 mainHeight = m_BackBufferScreen[e_engineView_MainScreen]->getSizeY();

    _mainOrigin.left     = 0;
    _mainOrigin.right    = mainWidht;
    _mainOrigin.top      = 0;
    _mainOrigin.bottom   = mainHeight;

    const f32 topSizeRation = f32_Min( ((f32)subScreenHeight / (f32)mainHeight), ((f32)subScreenWidth / (f32)mainWidht));
    const u32 topScreenNewWidth = (u32) (mainWidht * topSizeRation);
    const u32 topScreenNewHeight = (u32) (mainHeight * topSizeRation);

    const u32 topScreenLeft = (u32) (subScreenWidth - topScreenNewWidth) / 2;
    const u32 topScreenRight = topScreenLeft + topScreenNewWidth;

    _mainTop.left    = topScreenLeft;
    _mainTop.right   = topScreenRight;
    _mainTop.top     = 0;
    _mainTop.bottom  = topScreenNewHeight;

    // REMOTE
    const u32 remoteWidht = m_BackBufferScreen[e_engineView_RemoteScreen]->getSizeX();
    const u32 remoteHeight = m_BackBufferScreen[e_engineView_RemoteScreen]->getSizeY();

    _remoteOrigin.left     = 0;
    _remoteOrigin.right    = remoteWidht;
    _remoteOrigin.top      = 0;
    _remoteOrigin.bottom   = remoteHeight;

    const f32 bottomSizeRation = f32_Min( ((f32)subScreenHeight / (f32)remoteHeight), ((f32)subScreenWidth / (f32)remoteWidht));
    const u32 bottomScreenNewWidth = (u32)(remoteWidht * bottomSizeRation);
    const u32 bottomScreenNewHeight = (u32)(remoteHeight * bottomSizeRation);

    const u32 bottomScreenLeft = (subScreenWidth - bottomScreenNewWidth) / 2;
    const u32 bottomScreenRight = bottomScreenLeft + bottomScreenNewWidth;

    _remoteBottom.left     = bottomScreenLeft;
    _remoteBottom.right    = bottomScreenRight;
    _remoteBottom.top      = subScreenHeight;
    _remoteBottom.bottom   = subScreenHeight + bottomScreenNewHeight;
}

Vec2d& GFXAdapter_Directx9::transformScreenToRemoteViewCordinate(Vec2d& _pos)
{
    if( getCurrentDisplayingView() == e_engineView_Both)
    {
        const u32 screenWidth = getWindowWidth();
        const u32 screenHeight = getWindowHeight();

        RECT mainOrigin, remoteOrigin, mainTop, remoteBottom;
        ((GFXAdapter_Directx9*) GFX_ADAPTER)->computeEngineViewRect(mainOrigin, mainTop, remoteOrigin, remoteBottom);

        if(_pos.x()>=mainTop.left && _pos.x()<=mainTop.right && _pos.y()>=mainTop.top && _pos.y()<=mainTop.bottom )
        {
            _pos.x() = (_pos.x() - mainTop.left) * f32(screenWidth) / (mainTop.right - mainTop.left);
            _pos.y() = (_pos.y() - mainTop.top) * f32(screenHeight) / (mainTop.bottom - mainTop.top);
        }

        else if(_pos.x()>=remoteBottom.left && _pos.x()<=remoteBottom.right && _pos.y()>=remoteBottom.top && _pos.y()<=remoteBottom.bottom )
        {
            _pos.x() = (_pos.x() - remoteBottom.left) * f32(screenWidth) / (remoteBottom.right - remoteBottom.left);
            _pos.y() = (_pos.y() - remoteBottom.top) * f32(screenHeight) / (remoteBottom.bottom - remoteBottom.top);
        }
    }

    return _pos;
}
#endif

//copy current screenBuffer to backBuffer
void GFXAdapter_Directx9::applyRemoteScreenToFinalBackBuffer( RenderPassContext &_rdrCtx)
{
    setInternalRenderTarget(getTargetDX(m_BackBuffer)->getSurface(), NULL);
    clear(GFX_CLEAR_COLOR, (u32)0);

#if defined(ITF_WINDOWS) && defined(ITF_USE_REMOTEVIEW)
    if( getCurrentDisplayingView() == e_engineView_Both)
    {
        PrimitiveContext primCtx = PrimitiveContext(&_rdrCtx);
        DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

        drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
            .setDepthTest(bfalse)
            .setDepthWrite(bfalse);

        setShaderGroup(m_defaultShaderGroup);
        setGfxMatDefault(drawCallCtx);

        HRESULT hr_stretch;
        const u32 subScreenHeight = (u32)getWindowHeight()/2;
        
        RECT mainOrigin, remoteOrigin, mainTop, remoteBottom;
        computeEngineViewRect(mainOrigin, mainTop, remoteOrigin, remoteBottom);

        hr_stretch = m_pd3dDevice->StretchRect(getTargetDX(m_BackBufferScreen[e_engineView_MainScreen])->getSurface(),
            &mainOrigin, getTargetDX(m_BackBuffer)->getSurface(), &mainTop, D3DTEXF_POINT);    

        hr_stretch = m_pd3dDevice->StretchRect(getTargetDX(m_BackBufferScreen[e_engineView_RemoteScreen])->getSurface(),
            &remoteOrigin, getTargetDX(m_BackBuffer)->getSurface(), &remoteBottom, D3DTEXF_POINT);  

        //Draw Border
        VertexPC v[4];
        u32 color = COLOR_RED;
        f32 lsize = 1.f;

        setVertexFormat(VertexFormat_PC);
        v[0].m_color = color;
        v[1].m_color = color;
        v[2].m_color = color;
        v[3].m_color = color;

        v[0].m_pos.x() = (f32)0;
        v[0].m_pos.y() = (f32)subScreenHeight;
        v[1].m_pos.x() = (f32)0;
        v[1].m_pos.y() = (f32)subScreenHeight + lsize;
        v[2].m_pos.x() = (f32)getWindowWidth();
        v[2].m_pos.y() = (f32)subScreenHeight;
        v[3].m_pos.x() = (f32)getWindowWidth();
        v[3].m_pos.y() = (f32)subScreenHeight+ lsize;
        DrawPrimitive(drawCallCtx, GFX_TRIANGLE_STRIP, (void*)v, 4);
    }
    else
    {
        depthMask(0);
        depthTest(0);

        const u32 srcWidht = m_BackBufferScreen[getCurrentDisplayingView()]->getSizeX();
        const u32 srcHeight = m_BackBufferScreen[getCurrentDisplayingView()]->getSizeY();

        RECT rectOrigin;
        rectOrigin.left = 0;
        rectOrigin.right = srcWidht;
        rectOrigin.top = 0;
        rectOrigin.bottom = srcHeight;

        const u32 dstWidht = m_BackBuffer->getSizeX();
        const u32 dstHeight = m_BackBuffer->getSizeY();

        RECT rectDest;
        rectDest.left = 0;
        rectDest.right = dstWidht;
        rectDest.top = 0;
        rectDest.bottom = dstHeight;

        D3DTEXTUREFILTERTYPE filterType = D3DTEXF_POINT;
        if(srcWidht != dstWidht ||srcHeight != dstHeight)
            filterType = D3DTEXF_LINEAR;

        HRESULT hr_stretch;
        hr_stretch = m_pd3dDevice->StretchRect(getTargetDX(m_BackBufferScreen[getCurrentDisplayingView()])->getSurface(),
            &rectOrigin, getTargetDX(m_BackBuffer)->getSurface(), &rectDest, D3DTEXF_POINT);    
    }
#endif
}

// These functions have been taken directly form the XBOX SDK Gamma sample to
// undo the implicit system conversion from sRGB to TV-gamma

FLOAT GammaFuncsRGB( FLOAT f )
{
    if( f <= 0.0031308f )
    {
        return f * 12.92f;
    }
    else
    {
        return powf( f, 1.0f / 2.4f ) * 1.055f - 0.055f;
    }
}

FLOAT DegammaFuncTV( FLOAT f )
{
    if( f <= 0.0812f )
    {
        return f / 4.5f;
    }
    else
    {
        return powf( ( f + 0.099f ) / 1.099f, 1.0f / 0.45f );
    }
}

FLOAT DegammaFuncsRGB( FLOAT f )
{
    if( f <= 0.04045f )
    {
        return f / 12.92f;
    }
    else
    {
        return powf( ( f + 0.055f ) / 1.055f, 2.4f );
    }
}

typedef FLOAT DegammaFunc( FLOAT );

typedef FLOAT GammaFunc( FLOAT );

template<typename t_type, UINT t_N>
FLOAT ConvertNBitsToFloat( t_type i ) { return ( (FLOAT) i ) / ( ( 1 << t_N ) - 1 ); }

template<typename t_type, UINT t_N, DegammaFunc Degamma> 
FLOAT ConvertNBitGammaToFloat( t_type i ) { return Degamma( ConvertNBitsToFloat<t_type, t_N>( i ) ); }

/*template<typename t_type>
t_type Min( t_type a, t_type b ) { return a < b ? a : b; }

template<typename t_type>
t_type Max( t_type a, t_type b ) { return a > b ? a : b; }*/

template<typename t_type>
t_type Saturate( t_type a ) { return Min( 1.0f, Max( 0.0f, a ) ); }

template<typename t_type, UINT t_N>
t_type ConvertFloatToNBits( FLOAT f ) { return (t_type) ( ( ( 1 << t_N ) - 1 ) * Saturate( f ) + 0.5f ); }

template<typename t_type, UINT t_N, GammaFunc Gamma> 
t_type ConvertFloatToNBitGamma( FLOAT f ) { return ConvertFloatToNBits<t_type, t_N>( Gamma( f ) ); }

WORD ConvertFloatTo10BitGammasRGB( FLOAT f )        { return ConvertFloatToNBitGamma<WORD, 10, GammaFuncsRGB>( f ); }

WORD ConvertFloatTo10BitGammasRGBOverride( FLOAT f ){ return ConvertFloatTo10BitGammasRGB( DegammaFuncTV ( GammaFuncsRGB( f ) ) ); }

FLOAT Convert8BitGammasRGBToFloat( BYTE i )         { return ConvertNBitGammaToFloat<BYTE,  8, DegammaFuncsRGB>( i ); }

WORD Convert8BitGammasRGBTo10BitGammasRGBOverride( BYTE i ) { return ConvertFloatTo10BitGammasRGBOverride( Convert8BitGammasRGBToFloat( i ) ); }

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void GFXAdapter_Directx9::ComputeGammaRamp(float _fGamma, float _bright)
{
    const float fInvGamma   = 1.0f / _fGamma;
    const float fOneOver255 = 1.0f / 255.0f;

	if ( _fGamma < 0.0f )
	{
		for (ux i = 0; i < 256; i++)
		{
			mtt_GammaRamp.red[i]   = Convert8BitGammasRGBTo10BitGammasRGBOverride(BYTE(i)) << 6;
			mtt_GammaRamp.green[i]   = Convert8BitGammasRGBTo10BitGammasRGBOverride(BYTE(i)) << 6;
			mtt_GammaRamp.blue[i]   = Convert8BitGammasRGBTo10BitGammasRGBOverride(BYTE(i)) << 6;
		}
	}
	else
	{
		for (ux i = 0; i < 256; i++)
		{
			float f = (float)i * fOneOver255;
			f = powf(f, fInvGamma);
			// fpmode=fast can produce a gamma > 1, which is invald; so we need a min.
			f = f * (1.0f-_bright) + _bright;
			f = std::min(f * 65535.0f, 65535.0f);
			const u16 uw_Value = (u16)f;

			mtt_GammaRamp.red[i]   = uw_Value;
			mtt_GammaRamp.green[i] = uw_Value;
			mtt_GammaRamp.blue[i]  = uw_Value;
		}
	}
}


void GFXAdapter_Directx9::ApplyGamma()
{
    if ( (mf_CurrentGamma != mf_Gamma) || (mf_CurrentBright != mf_Bright) )
    {
        ComputeGammaRamp(mf_Gamma, mf_Bright);
#ifdef ITF_WINDOWS
        m_pd3dDevice->SetGammaRamp(0, D3DSGR_CALIBRATE, (const D3DGAMMARAMP *)&mtt_GammaRamp);
#else
        m_pd3dDevice->SetGammaRamp(0, D3DSGR_IMMEDIATE, (const D3DGAMMARAMP *)&mtt_GammaRamp);
#endif
        mf_CurrentGamma = mf_Gamma;
		mf_CurrentBright = mf_Bright;
    }
}

#ifndef ITF_FINAL
#ifdef ITF_X360

__int64 beginGpuMarkers[2][GFX_GPUMARKER_COUNT];
__int64 endGpuMarkers[2][GFX_GPUMARKER_COUNT];

void BeginGPUEventCallback(DWORD _markerIndex)
{
    ux bufferIndex = (_markerIndex>>16) & 1;
    ux markerIndex = _markerIndex & 65535;
    beginGpuMarkers[bufferIndex][markerIndex] = __mftb();
}

void EndGPUEventCallback(DWORD _markerIndex)
{
    ux bufferIndex = (_markerIndex>>16) & 1;
    ux markerIndex = _markerIndex & 65535;
    endGpuMarkers[bufferIndex][markerIndex] = __mftb() - beginGpuMarkers[bufferIndex][markerIndex];
}

#endif
#endif

void GFXAdapter_Directx9::beginGPUEvent(const char* _name, GPU_MARKER _marker)
{
#ifndef ITF_FINAL
#ifdef ITF_X360
    PIXBeginNamedEvent(0xFFFFFFFF, _name);
    if(_marker != GFX_GPUMARKER_Default)
    {
        m_pd3dDevice->InsertCallback(D3DCALLBACK_IMMEDIATE, BeginGPUEventCallback, (u32)_marker | (m_fenceFrameParity<<16));
    }
#endif
#endif
}

void GFXAdapter_Directx9::endGPUEvent( GPU_MARKER _marker)
{
#ifndef ITF_FINAL
#ifdef ITF_X360
    PIXEndNamedEvent();
    if(_marker != GFX_GPUMARKER_Default)
    {
        m_pd3dDevice->InsertCallback(D3DCALLBACK_IMMEDIATE, EndGPUEventCallback, (u32)_marker | (m_fenceFrameParity<<16));
    }
#endif
#endif
}

i32 sortByRank(const ITF::Box2D& _a, const ITF::Box2D& _b)
{
    return _a.m_rank - _b.m_rank;
}

void   GFXAdapter_Directx9::drawDebug()
{
    RenderPassContext passCtx;
    passCtx.getStateCache().setAlphaBlend(GFX_BLEND_ALPHA)
        .setDepthTest(bfalse);
    SetDrawCallState(passCtx.getStateCache(), 0xFFffFFff, btrue);
    PrimitiveContext primitiveCxt = PrimitiveContext(&passCtx);
    primitiveCxt.setPrimitiveParam((GFXPrimitiveParam*)&m_defaultPrimitiveParam);
    DrawCallContext drawCallCtx = DrawCallContext(&primitiveCxt);

    setDefaultGFXPrimitiveParam();
    setVertexShader( NULL );

    init2DRender();

#ifdef ITF_WINDOWS
    m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE); 
#endif // 

    /// set first the shader you want to use.
    setShaderGroup(m_defaultShaderGroup);
    setGfxMatDefault(drawCallCtx);

    /// set Vertex format
    setVertexFormat(VertexFormat_PC);

#ifdef ITF_SUPPORT_DBGPRIM_BOX
    m_DBGBox2DList.quickSort(sortByRank);
    for (u32 it = 0; it < m_DBGBox2DList.size(); ++it)
    {
        const Box2D& rect = m_DBGBox2DList[it];

        DrawPrimitive(drawCallCtx, GFX_TRIANGLE_STRIP, (void*)rect.v, 4);
    }
    m_DBGBox2DList.clear();
#endif // ITF_SUPPORT_DBGPRIM_BOX

#ifdef ITF_SUPPORT_DBGPRIM_MISC
    for (u32 it = 0; it < m_DBGTriangle2DList.size(); ++it)
    {
        const Triangle2D& tri = m_DBGTriangle2DList[it];

        DrawPrimitive(drawCallCtx, GFX_TRIANGLES, (void*)tri.v, 3);
    }
    m_DBGTriangle2DList.clear();

    for (u32 it = 0; it < m_DBGCircle2DList.size(); ++it)
    {
        const Circle2D& circle = m_DBGCircle2DList[it];

        DrawPrimitive(drawCallCtx, GFX_LINE_STRIP, (void*)circle.m_vertexes, circle.m_vertexCount);
    }
    m_DBGCircle2DList.clear();
#endif // ITF_SUPPORT_DBGPRIM_MISC

    end2DRender();
    
#ifndef ITF_FINAL
    if (isShowDebugInfo())
        getVertexBufferManager().drawDebugVBInfo();
#endif // ITF_FINAL

#ifndef ITF_RETAIL
#ifdef ITF_X360
        displayDebugInfoX360(drawCallCtx);
#else
        displayDebugInfoWin32(drawCallCtx);
#endif
#endif //ITF_RETAIL


}

void GFXAdapter_Directx9::present()
{
    if(isDeviceLost())
        return;

    TIMERASTER_SCOPE(present);
    m_pd3dDevice->BeginScene();
    setVertexShader( NULL );
    setShaderGroup(m_defaultShaderGroup);
    setDefaultGFXPrimitiveParam();

    setAlphaBlend(GFX_BLEND_ALPHA, GFX_ALPHAMASK_NONE);

    /// set the wvp matrix.
    setOrthoView(0.f, (f32) getWindowWidth(), 0.f, (f32) getWindowHeight());

    RenderPassContext rdrCtx;

#ifdef ITF_USE_REMOTEVIEW
    applyRemoteScreenToFinalBackBuffer(rdrCtx);
    setInternalRenderTarget(getTargetDX(m_BackBuffer)->getSurface(), NULL);
#endif

    if (getDebugDrawMode())
        showDebugDraw(rdrCtx);

    /// raster.
#ifdef ITF_X360
    //if (!isShowRasters()) toggleRaster();
#endif

    M_RASTER_DRAW();


    m_pd3dDevice->EndScene();
    resetSceneResolvedFlag();

#ifdef VBMANAGER_USE_DYNAMICRING_VB
    m_VertexBufferManager.DynamicRingVBendDraw();
#endif

#ifdef ITF_X360
    m_pd3dDevice->SetShaderGPRAllocation(0, 0, 0);
#endif

#ifndef _USE_ZBUFFER_TILING
#ifdef ITF_X360
    //raster CPU
    m_CPURasterStop = __mftb();
    f32 cpuRaster = (f32)((f32)((u32)((m_CPURasterStop & 0xFFFFFFFF) - (m_CPURasterStart & 0xFFFFFFFF))) * 1000.f / (f32)(static_cast<SystemAdapter_X360*>(SYSTEM_ADAPTER))->GetTickPerSec());
    m_CPURaster = (m_CPURaster + cpuRaster) * 0.5f;
#endif

    /// Present frame buffer
    HRESULT hr;
    hr = m_pd3dDevice->Present(0, 0, 0, 0);

#ifdef ITF_X360
    m_CPURasterStart = __mftb();
#endif

#ifdef ITF_USE_REMOTEVIEW

    if(GfxAdapterCallbackProxy::m_drcEmulatorPluginIsConnected && GfxAdapterCallbackProxy::m_drcEmulatorPluginIsConnected())
    {
#ifndef ITF_FINAL
        if (!m_drcThread)
        {
            m_quitDRCThread= bfalse;

            Synchronize::createEvent(&m_RemoteExitEvent,"RemoteExitEvent");

            m_drcThread = THREADMANAGER->createThread(DRC_Thread,(void*) NULL, ThreadSettings::m_settings[eThreadId_TRCManager]);
        }
#endif

        bbool capture = bfalse;
        {
            csAutoLock cs (m_csDRC);
            capture = btrue;
        }

        if (capture)
        {
            IDirect3DSurface9* BackBuffer = getBackBufferSurf(e_engineView_RemoteScreen);
              
            if (m_RingSurface[0] == NULL)
            {
                D3DSURFACE_DESC bbDesc;
                BackBuffer->GetDesc(&bbDesc);

                for (u32 index = 0;index<maxRingSurface;index++)
                {
                    DXCreateOffscreenPlainSurface(bbDesc.Width, bbDesc.Height, bbDesc.Format, D3DPOOL_SYSTEMMEM, &m_RingSurface[index], 0);
                }
            }

            m_currentIndexRing = (m_currentIndexRing+1)%maxRingSurface;
            m_newBufferSet = btrue;

            m_pd3dDevice->GetRenderTargetData(BackBuffer, m_RingSurface[m_currentIndexRing]);
        }
    }
    else
    {
        csAutoLock cs (m_csDRC);
        m_quitDRCThread = btrue;
    }

#endif //ITF_USE_REMOTEVIEW

#else

    m_pd3dDevice->SynchronizeToPresentationInterval();
    D3DVECTOR4 ClearColor = { 0, 0, 0, 0 };
    // Resolve the rendered scene back to the front buffer.
    m_pd3dDevice->Resolve( D3DRESOLVE_RENDERTARGET0,// | D3DRESOLVE_CLEARRENDERTARGET,
        NULL,
        m_pFrontBufferTexture[m_currFrontBuffer],
        NULL,
        0, 0,
        &ClearColor, 1.0f, 0, NULL );

    setBackBuffer();
       
    m_CPURasterStop = __mftb();
    m_CPURaster = (f32)((f32)((u32)((m_CPURasterStop & 0xFFFFFFFF) - (m_CPURasterStart & 0xFFFFFFFF))) * 1000.f / (f32)(static_cast<SystemAdapter_X360*>(SYSTEM_ADAPTER))->GetTickPerSec());

    m_pd3dDevice->Swap(m_pFrontBufferTexture[m_currFrontBuffer], NULL);
	BLOOMBERGINTERFACE->SetFrontBuffer(m_pFrontBufferTexture[m_currFrontBuffer]);
    m_currFrontBuffer = (m_currFrontBuffer+1)%2;

    m_CPURasterStart = __mftb();
#endif

    //Need to unbind D3D at each frame because if we bind a new ressource whereas the previous ressource was release (and not unbind), D3D change value of the fence on the release ressource.
    for (u32 index = 0;index < m_textureBinded.size();++index)
    {
        //m_textureBinded[index] = (PlatformTexture*)(0);
        //m_textureBindedFiltering[index] = btrue;
        SetTextureBind(index, NULL);
    }

    //vertex Buffer / index buffer
    for (u32 index = 0;index < 5;++index)
    {
        m_pd3dDevice->SetStreamSource(0, NULL, 0, 0);
    }

    m_pd3dDevice->SetIndices(NULL);
    
#ifdef ITF_X360
    ApplyGamma();

    MemoryBarrier();
    bbool valRelease = bfalse;//TODO interlock
    csAutoLock cs(m_csPresent);     
    {
        valRelease = m_askToReleaseOwnerShip;
    }

    if (valRelease)
    {
        m_pd3dDevice->ReleaseThreadOwnership();
        Synchronize::setEvent(&m_ReleaseOwnerShipEvent);
        Synchronize::waitEvent(&m_AcquireOwnerShipEvent);
        Synchronize::resetEvent(&m_AcquireOwnerShipEvent);
        {   
            csAutoLock cs(m_csPresent);  
            m_askToReleaseOwnerShip = bfalse;
        }

        m_pd3dDevice->AcquireThreadOwnership();
    }
        
#endif  //ITF_X360

    //Raster GPU
#ifndef ITF_FINAL
#ifdef ITF_X360
    for(u32 i=0; i<GFX_GPUMARKER_COUNT; i++)
    {
        //__int64 start = beginGpuMarkers[m_fenceFrameParity^1][i];
        //__int64 end   = endGpuMarkers[m_fenceFrameParity^1][i];
        //__int64 ticks = (__int64)(end - start);
        f32 gpuTime = (f32)((f32)((u32)endGpuMarkers[m_fenceFrameParity^1][i]) * 1000.f / (f32)(static_cast<SystemAdapter_X360*>(SYSTEM_ADAPTER))->GetTickPerSec());
        m_GPURasters[m_fenceFrameParity^1][i] = gpuTime;
    }
    m_fenceFrameParity = m_fenceFrameParity^1;
#endif
#endif

#ifdef ITF_SUPPORT_GPU_METRICS
    m_Metrics.endDraw();
#endif
}
#if defined(ITF_USE_REMOTEVIEW) && !defined(ITF_FINAL)
void GFXAdapter_Directx9::DRCThread()
{
    while (!m_quitDRCThread)
    {
        IDirect3DSurface9* workingSurface = NULL;
        bbool isConnected = bfalse;
        csAutoLock cs(m_csDRC);
        {
            isConnected = GfxAdapterCallbackProxy::m_drcEmulatorPluginIsConnected && GfxAdapterCallbackProxy::m_drcEmulatorPluginIsConnected();
        }

        if (isConnected && m_newBufferSet)
        {
            
            u32 currrentSurface = (m_currentIndexRing+2)%maxRingSurface;
            m_newBufferSet = bfalse;

            workingSurface = m_RingSurface[currrentSurface];

            if (workingSurface)
            {
                const u32 bufferCount = 10;

                u32 globalSize = 0;
              
                {

                    D3DLOCKED_RECT lockedrect;
                    D3DSURFACE_DESC descSurface;

                    workingSurface->GetDesc(&descSurface);

                    static HANDLE   hMapFile[bufferCount];
                    static LPCTSTR  pBuf[bufferCount];
                    static bool ringBufferInit = bfalse;

                    if (S_OK == workingSurface->LockRect(&lockedrect,NULL,D3DLOCK_READONLY|D3DLOCK_DONOTWAIT|D3DLOCK_NOSYSLOCK))
                    {
                        u32 pitch    = lockedrect.Pitch;
                        unsigned long* pBits = static_cast<unsigned long*>(lockedrect.pBits);
                        u32 height  = descSurface.Height;


                        globalSize = height*(pitch / sizeof(unsigned long))*4; 

                        if (!ringBufferInit)
                        {
                            
                            ringBufferInit = btrue;
                            for (u32 index =0;index<bufferCount;index++)
                            {

                                String8 name;
                                name.setTextFormat("buffer%d",index);
                                hMapFile[index] = CreateFileMappingA(
                                    INVALID_HANDLE_VALUE,    // use paging file
                                    NULL,                    // default security
                                    PAGE_READWRITE,          // read/write access
                                    0,                       // maximum object size (high-order DWORD)
                                    globalSize,                // maximum object size (low-order DWORD)
                                    name.cStr());                 // name of mapping object

                                pBuf[index] = (LPTSTR) MapViewOfFile( hMapFile[index],   // handle to map object
                                    FILE_MAP_ALL_ACCESS, // read/write permission
                                    0,
                                    0,
                                    globalSize);
                            }
                    }

                  

                    static u32 currentBufferID = 0;
                    currentBufferID = (currentBufferID+1)%bufferCount;
                    ITF_MEMCOPY((PVOID)pBuf[currentBufferID], (PVOID)pBits, globalSize);


                    workingSurface->UnlockRect();


                    String8 name;
                    name.setTextFormat("buffer%d",currentBufferID);

                    if(GfxAdapterCallbackProxy::m_drcEmulatorPluginSendRenderData)
                    {
                        GfxAdapterCallbackProxy::m_drcEmulatorPluginSendRenderData(NULL,globalSize,name);
                    }
                  }
                }
                                                       
              
            }

            sleep(5);
        }

    }

    Synchronize::setEvent(&m_RemoteExitEvent);

}



#endif //ITF_USE_REMOTEVIEW && !ITF_FINAL

DWORD GFXAdapter_Directx9::fontAlignmentToDirectXFormat(FONT_ALIGNMENT _alignment)
{
#ifdef ITF_WINDOWS
    switch (_alignment)
    {
        case FONT_ALIGN_LEFT: return DT_LEFT;
        case FONT_ALIGN_CENTER : return DT_CENTER;
        case FONT_ALIGN_RIGHT : return DT_RIGHT;
        case FONT_ALIGN_CENTERHV : return DT_CENTER | DT_VCENTER;
        default : ITF_ASSERT_MSG(bfalse, "Unknown font alignment"); return DT_LEFT;
    }
#else
    return 0;
#endif //ITF_WINDOWS
}

//---------------------------------------------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_DBGPRIM_TEXT
bbool GFXAdapter_Directx9::drawDBGTextSize( const String8 &_text, f32 &_width, f32 &_height, bbool _useBigFont, bbool _useFixedFont )
{
#if defined(ITF_WINDOWS)
    ID3DXFont *font = mp_FontDebug;
    if ( _useFixedFont)
        font = mp_FontDebug_Monospace;
    else if (_useBigFont)
        font = mp_FontDebug_Big;

    RECT R;
    font->DrawTextA(m_spriteInterface, _text.cStr(), -1 ,&R, DT_CALCRECT, 0);
    _width = (f32) (R.right - R.left);
    _height = (f32) (R.bottom - R.top);
    return btrue;
#else
    return bfalse;
#endif
}
#endif


#ifdef ITF_SUPPORT_VIDEOCAPTURE

void   GFXAdapter_Directx9::startScreenCapture(const Path *_directory)
{
    if (m_videoCapture)
    {
        IDirect3DSurface9*  BackBuffer = NULL;
        m_pd3dDevice->GetRenderTarget(0, &BackBuffer);

        D3DSURFACE_DESC     desc;
        HRESULT             hr;
        hr = BackBuffer->GetDesc(&desc);
        ITF_ASSERT(hr == S_OK);

        m_videoCapture->start(desc.Width,desc.Height, _directory);
    }
}


class VideoCaptureBuffer_Directx9 : public VideoCaptureBuffer
{
public:

    void release();
    void lock();
    void unlock();

};

void GFXAdapter_Directx9::doScreenCapture()
{
    if(isDeviceLost())
        return;

   TIMERASTER_SCOPE(doScreenCapture);
   if (m_videoCapture && m_videoCapture->isEnable())
       {
           IDirect3DSurface9* BackBuffer = getCurrentBackBufferSurf();
           u32 windowsWidth = getWindowWidth();
           u32 windowsHeight = getWindowHeight();

           bbool hasCaptureBuffer = m_videoCapture->hasCaptureBuffer();
           bbool resetMode        = (hasCaptureBuffer && !m_videoCapture->getCaptureBuffer()->getOffscreenBuffer());
           if (!hasCaptureBuffer || resetMode)
           {
               LPDIRECT3DSURFACE9 offscreen  = NULL;
               LPDIRECT3DSURFACE9 stretchscreen = NULL;

               u32 newWindowsWidth = windowsWidth;
               u32 newWindowsHeight = windowsHeight;
               LPDIRECT3DTEXTURE9 texture = NULL;

               m_mxDeviceLost.lock();
               if (m_videoCapture->useLowResolution())
               {
                   newWindowsHeight = (u32)((f32)((windowsHeight*(f32)m_videoCapture->getLowResCaptureWidth())/windowsWidth));
                   newWindowsWidth = m_videoCapture->getLowResCaptureWidth();

                   if (newWindowsHeight&1)
                        newWindowsHeight = newWindowsHeight+1;

                   HRESULT hr = DXCreateRenderTarget(newWindowsWidth, newWindowsHeight, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 0, FALSE, &stretchscreen, NULL);
                   if (hr!=S_OK)
                       messageD3DError(hr);

               }
                  
               HRESULT hr = DXCreateOffscreenPlainSurface(newWindowsWidth, newWindowsHeight, D3DFMT_A8R8G8B8,  D3DPOOL_SYSTEMMEM , &offscreen, NULL);
                   ITF_ASSERT(hr == S_OK);

               VideoCaptureBuffer_Directx9* videoCaptureBuffer = new VideoCaptureBuffer_Directx9();
               m_videoCapture->assignCaptureOffscreenBuffer(videoCaptureBuffer);
               videoCaptureBuffer->setOffscreenBuffer(newWindowsWidth,newWindowsHeight,offscreen,stretchscreen,texture);
               m_mxDeviceLost.unlock();
           }

           ITF_ASSERT(m_videoCapture->hasCaptureBuffer());
           VideoCaptureBuffer* pVideoCaptureBuffer =  m_videoCapture->getCaptureBuffer();

           LPDIRECT3DSURFACE9 offscreen = (LPDIRECT3DSURFACE9) m_videoCapture->getCaptureBuffer()->getOffscreenBuffer();
         
           HRESULT hr;
           if (m_videoCapture->useLowResolution())
           {
               LPDIRECT3DSURFACE9 stretchscreen = (LPDIRECT3DSURFACE9) m_videoCapture->getCaptureBuffer()->getStretchScreenBuffer();
               ITF_ASSERT(stretchscreen);

               hr= m_pd3dDevice->StretchRect(BackBuffer, NULL, stretchscreen, NULL, D3DTEXF_LINEAR);
               ITF_ASSERT(hr == S_OK);

               hr = m_pd3dDevice->GetRenderTargetData(stretchscreen, offscreen);
               ITF_ASSERT(hr == S_OK);
           }
           else
           {
               hr = m_pd3dDevice->GetRenderTargetData(BackBuffer, offscreen);
           }

           if (hr==S_OK) //if for any reasons ,we failed ( screensaver..) ,we skip this frame
           {
               pVideoCaptureBuffer->lock();

               m_videoCapture->resolveBufferScreen();

               pVideoCaptureBuffer->unlock(); 
           }
       }
 }

#endif //ITF_SUPPORT_VIDEOCAPTURE


#ifdef ITF_PICKING_SUPPORT

void  GFXAdapter_Directx9::setRenderTarget_BackBuffer()
{
    setBackBuffer();

    GFX_RECT rcViewport;
    rcViewport.left = 0;
    rcViewport.right = getScreenWidth();
    rcViewport.top =  0;
    rcViewport.bottom = getScreenHeight();

    setupViewport(&rcViewport);
}

#endif //ITF_PICKING_SUPPORT

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setAlphaTest(bbool _enabled)
{
    m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, (_enabled?TRUE:FALSE));
}


void GFXAdapter_Directx9::setAlphaRef(int _ref)
{
    m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, _ref);
}


void GFXAdapter_Directx9::startScreenshotMode()
{
#if defined (ITF_WINDOWS) && !defined(ITF_FINAL)
    m_preUHDScreenWidth = getWindowWidth();
    m_preUHDScreenHeight = getWindowHeight();
    m_previousDisableRendering2d = isDisableRender2d();

    /// compute UHD backbuffer size, taking current aspect ration into account
    u32 UHDWidth = 4096;
    u32 UHDHeight = 4096;
 
    const char * request = getStrSceenshotResquest();
    if (request == strScreenshotUHD() || request == strScreenshotUHDNo2d())
    {
        if (m_preUHDScreenWidth>m_preUHDScreenHeight)
            UHDHeight = (UHDWidth*m_preUHDScreenHeight)/m_preUHDScreenWidth;
        else
            UHDWidth = (UHDHeight*m_preUHDScreenWidth)/m_preUHDScreenHeight;

        if (request == strScreenshotUHDNo2d())
            setDisableRender2d(btrue);
    }
    else
    {
        UHDWidth = m_preUHDScreenWidth;
        UHDHeight = m_preUHDScreenHeight;
    }

    setResolution(UHDWidth, UHDHeight);

    setSeparateAlpha(btrue);
#endif //defined (ITF_WINDOWS) && !defined(ITF_FINAL)
}

void GFXAdapter_Directx9::endScreenshotMode()
{
#if defined (ITF_WINDOWS) && !defined(ITF_FINAL)
    //get and save surface
    D3DSURFACE_DESC pDesc;
    getCurrentBackBufferSurf()->GetDesc(&pDesc);

    m_mxDeviceLost.lock();
    IDirect3DSurface9 *pSystemMemSurface=NULL;
    HRESULT hr = DXCreateOffscreenPlainSurface(pDesc.Width, pDesc.Height, pDesc.Format, D3DPOOL_SYSTEMMEM, &pSystemMemSurface, NULL);
    if (SUCCEEDED(hr))
    {
        hr = m_pd3dDevice->GetRenderTargetData(getCurrentBackBufferSurf(), pSystemMemSurface);
        {
            if (SUCCEEDED(hr))
            {
                const char * request = getStrSceenshotResquest();
                if (request == strScreenshotNormal() || request == strScreenshotUHD() || request == strScreenshotUHDNo2d())
                {
                    char myDocumentPath[MAX_PATH];
                    strcpy(myDocumentPath, "c:\\");
                    PIDLIST_ABSOLUTE idList=NULL;
                    SHGetFolderLocation(NULL, CSIDL_MYPICTURES, NULL, 0, &idList);
                    ITF_ASSERT(idList!=NULL);
                    SHGetPathFromIDListA(idList, myDocumentPath);

                    int screenshotIndex=1;

                    Path path(myDocumentPath);
                    path.setDirectory();
                    path.setFlag(Path::FlagNonData,btrue);
                    
                    for(;;) 
                    {
                        if (request == strScreenshotNormal())
                        {
                            String8 basename;
                            basename.setTextFormat("UBI screenshot %03d.jpg", screenshotIndex);
                            path.changeBasename(basename);
                            if (FILEMANAGER->fileExists(path))
                            {
                                screenshotIndex++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        else if (request == strScreenshotUHD() || request == strScreenshotUHDNo2d())
                        {
                            String8 basename;
                            basename.setTextFormat("UBI Art framework - screenshot %03d.png", screenshotIndex);
                            path.changeBasename(basename);
                            if (FILEMANAGER->fileExists(path))// || FILEMANAGER->fileExists(path2))
                            {
                                screenshotIndex++;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }

                    String8 translated = FILESERVER->platformString8FromPath(path);
                    D3DXSaveSurfaceToFileA(translated.cStr(), request == strScreenshotNormal() ? D3DXIFF_JPG : D3DXIFF_PNG, pSystemMemSurface, NULL, NULL);

                    if (DEBUGINFO)
                    {
                        String8 pathStr;
                        path.toString8(pathStr);
                        String8 info("Saved ScreenShot to: ");
                        info += pathStr;
                        DEBUGINFO->addDebugInfoDisplay(info, 5.0f);
                    }

                    if(GfxAdapterCallbackProxy::m_pluginLauncherOnSnapShot)
                        GfxAdapterCallbackProxy::m_pluginLauncherOnSnapShot( path );

					if(GfxAdapterCallbackProxy::m_pluginCrashDumpSenderOnSnapShot)
						GfxAdapterCallbackProxy::m_pluginCrashDumpSenderOnSnapShot( path );
                }              
                else if (request == strScreenshotProfileFPS())
                {
                    if(GfxAdapterCallbackProxy::m_engineMonitorPluginSendFile)
                    {
                
                        LPD3DXBUFFER pBuffer;
                        if (!FAILED(D3DXSaveSurfaceToFileInMemory(&pBuffer , D3DXIFF_JPG, pSystemMemSurface, NULL, NULL)))
                        {
                            void* ptr = (pBuffer )->GetBufferPointer();
                            UINT len = (pBuffer )->GetBufferSize();
                        
                            char szFullFileName[256];
                            sprintf(szFullFileName, "%s.jpg", request);

                            GfxAdapterCallbackProxy::m_engineMonitorPluginSendFile(szFullFileName, (char*) ptr, len, false, true);
                        }        
                        pBuffer ->Release();
                    }
                }
                else if (request == strScreenshotMapTiler())
                {
                    // TESTING purpose, we set the images from x 0 -> 63 and y 0 -> 63
                    static i32 xIndex = 0;
                    static i32 yIndex = 0;


                    Path    path = m_screenshotMapTilerPath;
                    String8 fileName;

                    fileName.setTextFormat("RO2_%i_%i.png", xIndex, yIndex);
                    path.changeBasename(fileName);

                    String8 translated = FILESERVER->platformString8FromPath(path);
                    D3DXSaveSurfaceToFileA(translated.cStr(), D3DXIFF_PNG, pSystemMemSurface, NULL, NULL);
                    LOG("Saved tile to %s",translated.cStr());
                    xIndex++;
                    if (xIndex >= static_cast<i32>(m_screenshotMapTilerSize))
                    {
                        xIndex = 0;
                        yIndex++;
                    }
                }
            }
        }
        D3D_NM_RELEASE(pSystemMemSurface);
    }
    m_mxDeviceLost.unlock();

    // Restore previous state
    setResolution(m_preUHDScreenWidth, m_preUHDScreenHeight);
    setSeparateAlpha(bfalse);
    setDisableRender2d(m_previousDisableRendering2d);
#endif //defined (ITF_WINDOWS) && !defined(ITF_FINAL)
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::beginDisplay(f32 _dt)
{
    beginGPUEvent("Scene", GFX_GPUMARKER_Scene);

    GFXAdapter::beginDisplay(_dt);

    if (isRender) return;

    HRESULT hr;
    hr = m_pd3dDevice->BeginScene();
    isRender = 1;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::endDisplay()
{
    GFXAdapter::endDisplay();

    m_pd3dDevice->SetIndices(nullptr);
    m_pd3dDevice->SetStreamSource( 0, nullptr, 0, 0);
    m_pd3dDevice->SetStreamSource( 1, nullptr, 0, 0);

    if (!isRender) return;
    
    HRESULT hr;  
    hr = m_pd3dDevice->EndScene();

    isRender = 0;

    endGPUEvent(GFX_GPUMARKER_Scene);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::beginViewportRendering(View &_view)
{
    setBackBuffer();
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::endViewportRendering(View &_view)
{
    if (_view.isMainView() 
#ifdef ITF_PICKING_SUPPORT
        && !m_bPickingMode
#endif //ITF_PICKING_SUPPORT
        )

    {
    #ifndef ITF_CONSOLE_FINAL
        // safe Frame.
        if (safeFrameDrawRequested())
        {
            drawSafeFrame();
        }
    #endif // ITF_CONSOLE_FINAL

    
        drawDebug();
    }

    GFXAdapter::endViewportRendering(_view);
}
//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::beginSceneRendering(View &_view)
{
    Color clearColor;

#ifdef ITF_PC
	clearColor = Color::zero();	
	clearColor.setAlpha(1);
#else //ITF_PC
#ifndef ITF_FINAL
	if (getOverDrawMode()>=1)
		clearColor = Color::zero();
	else 
#endif //!ITF_FINAL
		clearColor = m_clearColor.getAsU32();
	clearColor.setAlpha(0);
#endif //ITF_PC

#ifdef ITF_X360
    //Prepare predicatedTiling
    beginTiling(clearColor);
#endif //ITF_X360

    setBackBuffer();
    u32 buffers = GFX_CLEAR_ZBUFFER | GFX_CLEAR_STENCIL;
    if(_view.getViewType() != View::viewType_ViewportUI)
        buffers |= GFX_CLEAR_COLOR;
    clear(buffers, (u32)clearColor.getAsU32());

    updateShaderScreenDimension(getScreenWidth(), getScreenHeight());
}


//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::endSceneRendering(View &_view)
{
#ifdef ITF_X360
    endTiling();
    //check if need restore
    bbool needRestore = btrue;
    u32 zListViewID = _view.getZlistID();
    if(zListViewID != View::ZlistID_invalid)
    {
        GFX_Zlist<AFXPostProcess> & afterFxZList = getZListManager().getZlistAt<GFX_ZLIST_AFTERFX>(zListViewID);
        u32 nFx = afterFxZList.getNumberEntry();
        needRestore = (nFx==0);
    }
    if(_view.getFreezeState() == View::DPFreeze_COPY_BACKBUFFER)
    {
        needRestore = btrue;
    }

    if(m_TilingParam.usePredicatedTiling)
    {
        setBackBuffer(bfalse, needRestore);
    }
    else if(!needRestore)
    {
        // AFX Post process need backBuffer Texture
        resolve(m_backBufferNoTile);
    }
#endif
}
//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::getBackBuffer()
{
    m_mxDeviceLost.lock();
    if (!m_BackBuffer)
    {
        IDirect3DSurface9 *backBufferSurf;
        m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBufferSurf);
		m_BackBuffer = createRenderTarget(backBufferSurf);
    }
#ifndef ITF_USE_REMOTEVIEW
    if (!m_DepthStencil)
    {
        LPDIRECT3DSURFACE9 depthSurface;
        m_pd3dDevice->GetDepthStencilSurface(&depthSurface);
        m_DepthStencil = createRenderTarget(depthSurface);
    }
#endif
    m_mxDeviceLost.unlock();
}

RenderTarget* GFXAdapter_Directx9::getCurrentBackBuffer()
{
#ifdef ITF_USE_REMOTEVIEW
    return m_BackBufferScreen[m_currentEngineView];
#else
	#ifdef ITF_X360
	if(!m_isPredicatedTilingPass)
		return m_backBufferNoTile;
	#endif
    return m_BackBuffer;
#endif
}

LPDIRECT3DSURFACE9 GFXAdapter_Directx9::getCurrentBackBufferSurf()
{
	RenderTarget *rt = getCurrentBackBuffer();
	GFX_RenderingBuffer_DX9 *rtdx = (GFX_RenderingBuffer_DX9 *)rt->m_adapterimplementationData;
	return rtdx->getSurface();
}

LPDIRECT3DSURFACE9 GFXAdapter_Directx9::getCurrentDepthStencilSurf()
{
#ifdef ITF_USE_REMOTEVIEW
    return getTargetDX(m_DepthStencilScreen[m_currentEngineView])->getSurface();
#else
    return getTargetDX(m_DepthStencil)->getSurface();
#endif
}

BasePlatformTexture* GFXAdapter_Directx9::getFrontLightTexture(eLightTexBuffer _currentLightBufferType) const
{
    if(_currentLightBufferType == LIGHT_BFR_FULL)
        return getBufferTextureDX(TEX_BFR_FRONT_LIGHT);
    else if(_currentLightBufferType == LIGHT_BFR_QRT_MOD_0)
        return getBufferTextureDX(TEX_BFR_QRT_FRONT_LIGHT);
    else
        return getBufferTextureDX(TEX_BFR_QRT_TMP_LIGHT);
}

BasePlatformTexture* GFXAdapter_Directx9::getBackLightTexture(eLightTexBuffer _currentLightBufferType) const
{
    if(_currentLightBufferType == LIGHT_BFR_FULL)
        return getBufferTextureDX(TEX_BFR_BACK_LIGHT);
    else if(_currentLightBufferType == LIGHT_BFR_QRT_MOD_0)
        return getBufferTextureDX(TEX_BFR_QRT_BACK_LIGHT);
    else
        return getBufferTextureDX(TEX_BFR_QRT_TMP_LIGHT);
}

GFX_RenderingBuffer_DX9* GFXAdapter_Directx9::getFrontLightTextureDX(eLightTexBuffer _currentLightBufferType)
{
    if(_currentLightBufferType == LIGHT_BFR_FULL)
        return getBufferTextureDX(TEX_BFR_FRONT_LIGHT);
    else if(_currentLightBufferType == LIGHT_BFR_QRT_MOD_0)
        return getBufferTextureDX(TEX_BFR_QRT_FRONT_LIGHT);
    else
        return getBufferTextureDX(TEX_BFR_QRT_TMP_LIGHT);
}

GFX_RenderingBuffer_DX9* GFXAdapter_Directx9::getBackLightTextureDX(eLightTexBuffer _currentLightBufferType)
{
    if(_currentLightBufferType == LIGHT_BFR_FULL)
        return getBufferTextureDX(TEX_BFR_BACK_LIGHT);
    else if(_currentLightBufferType == LIGHT_BFR_QRT_MOD_0)
        return getBufferTextureDX(TEX_BFR_QRT_BACK_LIGHT);
    else
        return getBufferTextureDX(TEX_BFR_QRT_TMP_LIGHT);
}

u32   GFXAdapter_Directx9::getRemoteScreenWidth    (   ) const 
{
    return 854;
}

u32  GFXAdapter_Directx9::getRemoteScreenHeight   (   ) const 
{
    return 480;
}

void GFXAdapter_Directx9::copyTexture( RenderPassContext & _rdrCtxt, RenderTarget *_source, RenderTarget *_destination)
{
	GFX_RenderingBuffer_DX9* destBuf = getTargetDX(_destination);
#ifdef ITF_WINDOWS
	GFX_RenderingBuffer_DX9* srcBuf = getTargetDX(_source);
    LPDIRECT3DSURFACE9 srcSurf = srcBuf->getSurface();
    LPDIRECT3DSURFACE9 dstSurf = destBuf->getSurface();
    HRESULT hr = m_pd3dDevice->StretchRect( srcSurf, NULL, dstSurf, NULL, D3DTEXF_LINEAR);
    ITF_VERIFY(D3D_OK == hr);
#else
    PrimitiveContext primCtx = PrimitiveContext(&_rdrCtxt);
    DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

    GFX_ZLIST_PASS_TYPE oldPassType = getCurPassType();
    m_curPassType = GFX_ZLIST_PASS_REGULAR;

    u32 W, H;

    W = destBuf->getWidth();
    H = destBuf->getHeight();

    getWorldViewProjMatrix()->push();
    setOrthoView(0.f, (f32)W, 0.f, (f32)H);

    setShaderGroup(m_defaultShaderGroup);
    setGfxMatDefault(drawCallCtx);

    drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
        .setDepthTest(bfalse);

	RenderContext rtContext(_destination);
	setRenderContext(rtContext);

    bindTexture(0, _source);
    setTextureAdressingMode(0, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);

    clear(GFX_CLEAR_COLOR, (u32)0x00000000);
    drawScreenQuad(drawCallCtx, 0.f, 0.f, (f32)W, (f32)H, 0.0f, COLOR_WHITE);

    getWorldViewProjMatrix()->pop();
    m_curPassType = oldPassType;
    SetTextureBind(0, NULL);
#endif
}

#ifdef ITF_USE_REMOTEVIEW

LPDIRECT3DSURFACE9 GFXAdapter_Directx9::getBackBufferSurf(u32 _viewID)
{
    if(_viewID >= e_engineView_Count)
    {
        ITF_ASSERT_CRASH(0,"Illegal view");
    }

    return getTargetDX(m_BackBufferScreen[_viewID])->getSurface();
}
#endif

void GFXAdapter_Directx9::setBackBuffer(bbool _setZbuffer, bbool _restoreBackBuffer)
{
	// Unbind previous target.
	for ( ux i = 0; i < 2; i++ )
	{
		if ( m_currentRenderTarget[i] )
		{
			m_currentRenderTarget[i]->m_flags &= ~GFX_TEXFLAG_BIND_AS_TARGET;
			m_currentRenderTarget[i] = NULL;
		}
	}
	if ( m_currentDepthBuffer )
	{
		m_currentDepthBuffer->m_flags &= ~GFX_TEXFLAG_BIND_AS_TARGET;
		m_currentDepthBuffer = NULL;
	}

    m_pd3dDevice->SetRenderTarget( 0, getCurrentBackBufferSurf() );
    m_pd3dDevice->SetRenderTarget( 1, NULL );
    if(_setZbuffer)
    {
        m_pd3dDevice->SetDepthStencilSurface(getCurrentDepthStencilSurf());
    }
    else
    {
        m_pd3dDevice->SetDepthStencilSurface(NULL);
    }
    m_currentRenderingSurface = getCurrentBackBufferSurf();

#ifdef ITF_X360
    if(_restoreBackBuffer)
    {
        init2DRender();
		GFX_ZLIST_PASS_TYPE oldPassType = m_curPassType;
		m_curPassType = GFX_ZLIST_PASS_2D;

		RenderPassContext rdrCtx;
        PrimitiveContext primCtx = PrimitiveContext(&rdrCtx);
        DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

		setDefaultGFXPrimitiveParam();
        setShaderGroup(m_defaultShaderGroup);
        setGfxMatDefault(drawCallCtx);
        
        drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
            .setDepthTest(bfalse)
            .setDepthWrite(bfalse);

        setGlobalColor(Color::white());
        bindTexture(0, m_backBufferNoTile, bfalse);
        drawScreenQuad(drawCallCtx, 0,0, (f32)getScreenWidth(), (f32)getScreenHeight(), 0, COLOR_WHITE, bfalse);
        end2DRender();

		m_curPassType = oldPassType;
    }
#endif
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::BuildPresentParams()
{
#ifdef ITF_WINDOWS
    BuildPresentParamsWin32();
#else
    BuildPresentParamsX360();
#endif //ITF_WINDOWS
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_Directx9::resetDevice()
{
    if (!m_pd3dDevice) return bfalse;

    preD3DReset();

	BuildPresentParams();

    m_mxDeviceLost.lock();

    HRESULT hr;
    bbool isLooping = btrue;
    while (isLooping)
    {
        hr = m_pd3dDevice->Reset(&m_ppars);
        if (hr == D3DERR_DEVICELOST)
        {
            Sleep(500);
        }
#ifdef ITF_WINDOWS
        else  if (hr == D3DERR_DEVICEREMOVED || hr == D3DERR_DRIVERINTERNALERROR)
        {
            SF_RELEASE(m_pd3dDevice);
            SF_RELEASE(m_pD3D);
            createDXDeviceWin32();
        }
#endif
        else  if (!SUCCEEDED(hr))
        {
            messageD3DError(hr);
        }
        else //(SUCCEEDED(hr))
        {
            isLooping = bfalse;
        }        
    }

    postD3DReset();

    init();

    m_mxDeviceLost.unlock();

    return false;
}

///----------------------------------------------------------------------------//
/// Shaders.
///----------------------------------------------------------------------------//

#ifdef CHECK_SHADERS_VALIDITY
void checkShaderValidity(ShaderParameterDBDesc *_DBDesc, void* _shaderBin, bbool _isPixel)
{
    LPD3DXCONSTANTTABLE pConstantTable;
    D3DXCONSTANT_DESC pConstantDesc[32];
    UINT pConstantNum = 32;
    UINT pSizeOfData;
    BYTE* pData;

    if(_isPixel)
    {
        IDirect3DPixelShader9  *pPShaderResult = (IDirect3DPixelShader9*)_shaderBin;
        pPShaderResult->GetFunction(NULL,&pSizeOfData);
        pData = newAlloc(mId_GfxAdapter, BYTE[pSizeOfData]) ;
        pPShaderResult->GetFunction(pData,&pSizeOfData);
    }
    else
    {
        IDirect3DVertexShader9  *pPShaderResult = (IDirect3DVertexShader9*)_shaderBin;
        pPShaderResult->GetFunction(NULL,&pSizeOfData);
        pData = newAlloc(mId_GfxAdapter, BYTE[pSizeOfData]) ;
        pPShaderResult->GetFunction(pData,&pSizeOfData);
    }
    
#ifdef ITF_WINDOWS
    D3DXGetShaderConstantTableEx(reinterpret_cast<DWORD*>(pData), D3DXCONSTTABLE_LARGEADDRESSAWARE, &pConstantTable);
#else // !ITF_WINDOWS
    D3DXGetShaderConstantTable(reinterpret_cast<DWORD*>(pData), &pConstantTable);
#endif

    D3DXCONSTANTTABLE_DESC pDesc;
    pConstantTable->GetDesc(&pDesc);
    for(UINT StartRegister = 0; StartRegister < pDesc.Constants; StartRegister++)
    {
        D3DXHANDLE Handle = pConstantTable->GetConstant(NULL,StartRegister);
        if(Handle != NULL)
        {
            pConstantTable->GetConstantDesc(Handle,pConstantDesc,&pConstantNum);
            const ShaderParameterDBDesc::const_iterator &it = _DBDesc->find(pConstantDesc->Name);
            bbool isConstant = ((pConstantDesc->Type == D3DXPT_FLOAT) || (pConstantDesc->Type == D3DXPT_INT));
            float nbElem = pConstantDesc->Rows * pConstantDesc->Columns * pConstantDesc->Elements / 4.0f;
            u32 nbReg = (u32)ceil(nbElem);
            if( isConstant
                && ((it == _DBDesc->end())
                ||  (it->firstReg != pConstantDesc->RegisterIndex)
                ||  (it->nbReg != nbReg))
                )
            {
                ITF_WARNING(NULL, 0, "Uniform array '%s' mismatch.", pConstantDesc->Name);
            }
        }
    }
    delete [] pData;
    pConstantTable->Release();
}
#endif

HRESULT LoadFile( const Path& fileName, u8** ppFileData, DWORD* pdwFileSize )
{
    File* shaderFile = FILEMANAGER->openFile(fileName,ITF_FILE_ATTR_READ);

    if (shaderFile)
    {
        u32 lenght = 0;
        u8 * pBuffer = NULL;
    
        lenght = (u32) shaderFile->getLength();
        pBuffer = newAlloc(mId_Temporary,u8[lenght]);

        shaderFile->read(pBuffer,lenght);
        FILEMANAGER->closeFile(shaderFile);

        if( pdwFileSize )
            *pdwFileSize = lenght;
        *ppFileData = pBuffer;

        return S_OK;
    }

    return S_FALSE;
}

VOID UnloadFile( u8* pFileData )
{
    ITF_ASSERT( pFileData != NULL );
    SF_DEL_ARRAY( pFileData );
}

void GFXAdapter_Directx9::releaseShader( ITF_shader* _shaderGroup )
{
    ShaderMapIterator posVs = _shaderGroup->mp_hashVs.begin();
    while (posVs != _shaderGroup->mp_hashVs.end())
    {
		microcodeStruct &ms = posVs.getval();

		if(ms.linkedKey == 0xFFFFFFFFFFFFFFFF)
        {
            IDirect3DVertexShader9* vs = (IDirect3DVertexShader9*)ms.microcode; 
            if (vs)
                D3D_NM_RELEASE(vs);
        }
        ++posVs;
    }

    ShaderMapIterator posPs = _shaderGroup->mp_hashPs.begin();
    while (posPs != _shaderGroup->mp_hashPs.end())
    {
		microcodeStruct ms = posPs.getval();

        if(ms.linkedKey == 0xFFFFFFFFFFFFFFFF)
        {
            IDirect3DPixelShader9* ps = (IDirect3DPixelShader9*)ms.microcode;
            if (ps)
                D3D_NM_RELEASE(ps);
        }
        ++posPs;
    }
}

void* GFXAdapter_Directx9::loadBinPShader( u8* _binShader, ux _binSize)
{
    IDirect3DPixelShader9  *pPShaderResult = 0;
    HRESULT hr = DXCreatePixelShader( (DWORD*) _binShader, &pPShaderResult );
    if ( FAILED( hr ) )
    {
        return( 0 );
    }
#ifdef CHECK_SHADERS_VALIDITY
    else
    {
        checkShaderValidity(m_psDBDesc, (void*)pPShaderResult, btrue);
    }
#endif // CHECK_SHADERS_VALIDITY

    return pPShaderResult;
}

void* GFXAdapter_Directx9::loadBinVShader(u8* _binShader, ux _binSize)
{
    IDirect3DVertexShader9  *pVShaderResult = 0;
    HRESULT hr = DXCreateVertexShader( (DWORD*) _binShader, &pVShaderResult );
    if ( FAILED( hr ) )
    {
        return( 0 );
    }
#ifdef CHECK_SHADERS_VALIDITY
    else
    {
        checkShaderValidity(m_vsDBDesc, (void*)pVShaderResult, bfalse);
    }
#endif // CHECK_SHADERS_VALIDITY

    return pVShaderResult;
}

bbool GFXAdapter_Directx9::loadShader( ITF_shader* _shaderGroup )
{
    Path shaderFxPath;
    String8 sPath = mp_shaderManager.getShaderPath() + String8("unified/");
    shaderFxPath.fromString8(sPath);
    shaderFxPath.changeBasename(_shaderGroup->getName());
    shaderFxPath.changeExtension("fx");
#ifndef LOAD_SHADER_FROM_DATA
	shaderFxPath.setFlag(Path::FlagNonData, btrue);
#endif

    _shaderGroup->generateShaders(shaderFxPath);

    return btrue;
}


#ifndef ITF_CONSOLE_FINAL
void GFXAdapter_Directx9::reloadShaders()
{
    LOG("[SHADER] Reloading shaders ...");

    releaseCoreShaders();

    loadCoreShaders();

    LOG("[SHADER] Shaders reloaded.");
}
#endif // ITF_CONSOLE_FINAL

///----------------------------------------------------------------------------//
/// Vertex Format.
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setVertexFormat( u32 _vformat )
{
    m_drawCallStates.vertexFormat = _vformat;
    
    switch(_vformat)
    {
        default:
        case VertexFormat_PCT:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PCT ) ;
            m_drawCallStates.vertexSize = sizeof(VertexPCT);
        break;
        case VertexFormat_PC2T:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PC2T ) ;
            m_drawCallStates.vertexSize = sizeof(VertexPC2T);
            break;
        case VertexFormat_PCTBIBW:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PCTBIBW ) ;
            m_drawCallStates.vertexSize = sizeof(VertexPCTBIBW);
        break;
        case VertexFormat_PNCTBIBW:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PNCTBIBW ) ;
            m_drawCallStates.vertexSize = sizeof(VertexPNCTBIBW);
        break;
        case VertexFormat_PC:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PC ) ;
            m_drawCallStates.vertexSize = sizeof(VertexPC);
        break;
        case VertexFormat_PT:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PT ) ;
            m_drawCallStates.vertexSize = sizeof(VertexPT);
            break;
        case VertexFormat_PTa:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PT ) ;
            m_drawCallStates.vertexSize = sizeof(VertexPT);
            break;
        case VertexFormat_PCBT:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PCBT ) ;
            m_drawCallStates.vertexSize = sizeof(VertexPCBT);
            break;
        case VertexFormat_PCB2T:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PCB2T ) ;
            m_drawCallStates.vertexSize = sizeof(VertexPCB2T);
            break;
        case VertexFormat_PNCT:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PNCT ) ;
            m_drawCallStates.vertexSize = sizeof(VertexPNCT);
            break;
        case VertexFormat_PNC3T:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PNC3T ) ;
            m_drawCallStates.vertexSize = sizeof(VertexPNC3T);
            break;
       case VertexFormat_P:
            m_drawCallStates.vertexSize = sizeof(VertexP);
        break;
       case VertexFormat_QuadInstance_2TC:
           m_pd3dDevice->SetVertexDeclaration( mp_VDcl_QI_2TC ) ;
#ifdef GFX_NO_INSTANCING
           m_drawCallStates.vertexSize = sizeof(VertexQI_2TC)/4;
#else
           m_drawCallStates.vertexSize = sizeof(VertexQI_2TC);
#endif
           break;
       case VertexFormat_T:
           m_pd3dDevice->SetVertexDeclaration( mp_VDcl_T ) ;
           m_drawCallStates.vertexSize = sizeof(VertexT);
           break;
       case VertexFormat_I:
           m_pd3dDevice->SetVertexDeclaration( mp_VDcl_I ) ;
           m_drawCallStates.vertexSize = sizeof(VertexI);
           break;

    }
}

//----------------------------------------------------------------------------//
// VertexBuffer.
//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::createVertexBuffer( ITF_VertexBuffer* _vertexBuffer )
{
    HRESULT hr;
    D3DPOOL memPool = m_useD3D9Ex?D3DPOOL_DEFAULT:D3DPOOL_MANAGED;
    u32 usage = m_useD3D9Ex?D3DUSAGE_WRITEONLY:0;

#ifdef ITF_WINDOWS
    if (_vertexBuffer->bIsDynamic())
    {
        usage |= D3DUSAGE_DYNAMIC;
        memPool = D3DPOOL_DEFAULT;
        usage |= D3DUSAGE_WRITEONLY;
    }
#endif

    m_mxDeviceLost.lock();

        hr = DXCreateVertexBuffer(_vertexBuffer->m_size,
                                              usage, 0,
                                              memPool, (LPDIRECT3DVERTEXBUFFER9*)&_vertexBuffer->mp_VertexBuffer, NULL );

        if (hr!=S_OK)
        {
            messageD3DError(hr);
            _vertexBuffer->mp_VertexBuffer = NULL;
        }

    m_mxDeviceLost.unlock();
}

void GFXAdapter_Directx9::releaseVertexBuffer( ITF_VertexBuffer* _vertexBuffer )
{
    if (!_vertexBuffer->m_size) return;

    if (_vertexBuffer->mp_VertexBuffer)
    {
         LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer = (LPDIRECT3DVERTEXBUFFER9)_vertexBuffer->mp_VertexBuffer;
         D3D_NM_RELEASE(d3dVertexBuffer);
        _vertexBuffer->mp_VertexBuffer = 0;
    }
}

void GFXAdapter_Directx9::setVertexBuffer( ITF_VertexBuffer* _vertexBuffer )
{
    //You must in the main thread to set the texture as available...
    
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    m_drawCallStates.vertexBuffer = _vertexBuffer;
    m_drawCallStates.nbVertices = _vertexBuffer->m_nVertex;
    m_drawCallStates.useInstancing = bfalse;

    if(!_vertexBuffer->mp_VertexBuffer) return;
    
    setVertexFormat(_vertexBuffer->m_vertexFormat);

#ifdef ITF_WINDOWS
#ifndef GFX_NO_INSTANCING
    if(_vertexBuffer->m_vertexFormat & VF_QUAD_INSTANCE)
    {
        // Set up the geometry data stream
        ITF_VertexBuffer* instancingQuadVB = needSpriteInstancingVertexBuffer();
        m_drawCallStates.nbVertices = 4;

        m_pd3dDevice->SetStreamSourceFreq(0, (D3DSTREAMSOURCE_INDEXEDDATA | _vertexBuffer->m_nbInstance));
        m_pd3dDevice->SetStreamSource(0, ((IDirect3DVertexBuffer9 *)instancingQuadVB->mp_VertexBuffer ), 0, instancingQuadVB->m_structVertexSize );

        // Set up the instance data stream
        m_pd3dDevice->SetStreamSourceFreq(1, UINT((D3DSTREAMSOURCE_INSTANCEDATA | 1)));
        m_pd3dDevice->SetStreamSource(1, ((IDirect3DVertexBuffer9 *)_vertexBuffer->mp_VertexBuffer), 0, _vertexBuffer->m_structVertexSize );
    }
    else
#endif
    {
        m_pd3dDevice->SetStreamSourceFreq(0, 1);
        m_pd3dDevice->SetStreamSourceFreq(1, 1);
        m_pd3dDevice->SetStreamSource( 0, ((IDirect3DVertexBuffer9 *)_vertexBuffer->mp_VertexBuffer), _vertexBuffer->m_offset, _vertexBuffer->m_structVertexSize );
    }
#else
#ifndef GFX_NO_INSTANCING
    if(_vertexBuffer->m_vertexFormat & VF_QUAD_INSTANCE)
    {
        ITF_VertexBuffer* instancingQuadVB = needSpriteInstancingVertexBuffer();
        ITF_VertexBuffer* instancingIndexVB = needSpriteInstancingIndexAsVertexBuffer();

        m_drawCallStates.nbVertices = _vertexBuffer->m_nbInstance;
        m_drawCallStates.useInstancing = btrue;

        m_pd3dDevice->SetStreamSource(0, ((IDirect3DVertexBuffer9 *)instancingQuadVB->mp_VertexBuffer ), 0, instancingQuadVB->m_structVertexSize );

        // Set up the instance data stream
        m_pd3dDevice->SetStreamSource(1, ((IDirect3DVertexBuffer9 *)_vertexBuffer->mp_VertexBuffer), 0, _vertexBuffer->m_structVertexSize );

        //index
        m_pd3dDevice->SetStreamSource(2, ((IDirect3DVertexBuffer9 *)instancingIndexVB->mp_VertexBuffer ), 0, instancingIndexVB->m_structVertexSize );
    }
    else
#endif
    {
        m_pd3dDevice->SetStreamSource( 0, ((IDirect3DVertexBuffer9 *)_vertexBuffer->mp_VertexBuffer), _vertexBuffer->m_offset, _vertexBuffer->m_structVertexSize );
    }
#endif //ITF_WINDOWS

}

void GFXAdapter_Directx9::LockVertexBuffer( ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag )
{
    HRESULT hr = S_OK;

    m_mxDeviceLost.lock();

    u32 flag = 0;

#ifdef ITF_WINDOWS
    /// only dynamic can nooverwrite
    if (_vertexBuffer->bIsDynamic())
    {
        flag |= D3DLOCK_NOOVERWRITE;
    }
#endif

    if (_vertexBuffer->mp_VertexBuffer)
        hr = ((LPDIRECT3DVERTEXBUFFER9)_vertexBuffer->mp_VertexBuffer)->Lock( _offset, _size, _data, flag);
   
     if (hr!=S_OK)
        messageD3DError(hr);

     m_mxDeviceLost.unlock();
}

void GFXAdapter_Directx9::UnlockVertexBuffer( ITF_VertexBuffer* _vertexBuffer )
{
    if (_vertexBuffer->mp_VertexBuffer)
        ((LPDIRECT3DVERTEXBUFFER9)_vertexBuffer->mp_VertexBuffer)->Unlock();
}


///----------------------------------------------------------------------------//
/// IndexBuffer.
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::createIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{
    HRESULT hr;
    D3DPOOL memPool = m_useD3D9Ex?D3DPOOL_DEFAULT:D3DPOOL_MANAGED;
    u32 usage =  m_useD3D9Ex?D3DUSAGE_WRITEONLY:0;
#ifdef ITF_WINDOWS
    if (_indexBuffer->bIsDynamic())
    {
        usage |= D3DUSAGE_DYNAMIC;
        memPool = D3DPOOL_DEFAULT;
        usage |= D3DUSAGE_WRITEONLY;
    }
#endif
    m_mxDeviceLost.lock();

        hr = DXCreateIndexBuffer( _indexBuffer->m_nIndices * sizeof(u16),
                                                      usage, D3DFMT_INDEX16,
                                                      memPool, (LPDIRECT3DINDEXBUFFER9*)&_indexBuffer->mp_IndexBuffer, NULL );
        if (FAILED(hr))
            messageD3DError(hr);

    m_mxDeviceLost.unlock();
}

void GFXAdapter_Directx9::releaseIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{
    if (_indexBuffer->mp_IndexBuffer)
    {
        LPDIRECT3DINDEXBUFFER9 d3dIndexBuffer = (LPDIRECT3DINDEXBUFFER9)_indexBuffer->mp_IndexBuffer;
        D3D_NM_RELEASE(d3dIndexBuffer);
        _indexBuffer->mp_IndexBuffer = 0;
    }
}

void GFXAdapter_Directx9::setIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{
    m_drawCallStates.indexBuffer = _indexBuffer;

    if(!_indexBuffer->mp_IndexBuffer) return;
    
    HRESULT hr;
    hr = m_pd3dDevice->SetIndices((IDirect3DIndexBuffer9 *)_indexBuffer->mp_IndexBuffer );
    if (FAILED(hr))
        messageD3DError(hr);
}

void GFXAdapter_Directx9::LockIndexBuffer( ITF_IndexBuffer* _indexBuffer, void** _data )
{
    HRESULT hr;
    hr = ((LPDIRECT3DINDEXBUFFER9)_indexBuffer->mp_IndexBuffer)->Lock( 0, _indexBuffer->m_nIndices * sizeof(u16), _data, 0);
    if (FAILED(hr))
        messageD3DError(hr);
}

void GFXAdapter_Directx9::UnlockIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{
    ((LPDIRECT3DINDEXBUFFER9)_indexBuffer->mp_IndexBuffer)->Unlock();
}

///----------------------------------------------------------------------------//
//Movie
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::drawMovie( PrimitiveContext &_primCtx, Matrix44* _matrix,f32 _alpha,ITF_VertexBuffer* _pVertexBuffer,Texture** _pTextureArray,u32 _countTexture)
{
    init2DRender();

    DrawCallContext drawCallCtx = DrawCallContext(&_primCtx);

    setShaderGroup(mc_shader_Movie, mc_entry_movies_VS, mc_entry_movies_PS, 0, (_countTexture==4)?mc_define_movies_use_alpha_texture:0);
    setGfxMatDefault(drawCallCtx);

    for (u32 index = 0; index < _countTexture; index++)
	{
		PlatformTexture *platformTex = getPlatformTexture( _pTextureArray[index] );
        SetTextureBind(index, platformTex);
	}

    for( u32 i = 0 ; i < _countTexture ; i++ )
    {
        m_pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    }

    GFX_Vector4 vconsts(_alpha, _alpha, _alpha, _countTexture==4 ? 1.f : 0.f);
    SetPixelShaderConstantF( PS_Attrib_movie, (f32*)&vconsts, 1);

#ifdef ITF_X360
    m_pd3dDevice->SetRenderState( D3DRS_VIEWPORTENABLE, TRUE );
#endif //ITF_X360

    u32 W = getScreenWidth();
    u32 H = getScreenHeight();
    drawScreenQuad(drawCallCtx, 0.f, 0.f, (f32)W, (f32)H, 0.f, COLOR_WHITE);

    end2DRender();
}

void GFXAdapter_Directx9::copyCurrentColorBuffer(u32 _rt)
{
#ifdef ITF_WINDOWS
    depthMask(0);
    depthTest(0);
 
    HRESULT hr;
    RECT rect;
    rect.left=0;
    rect.right=getScreenWidth();
    rect.top=0;
    rect.bottom=getScreenHeight();

    hr = m_pd3dDevice->StretchRect(m_currentRenderingSurface,
        &rect, getBufferTextureDX(TEX_BFR_FULL_1 + _rt)->getSurface(), &rect, D3DTEXF_POINT); 

#elif defined (ITF_X360)
    ITF_ASSERT_MSG(_rt == 0, "No second full rendering Buffer in X360");
    resolve( getBufferTexture(TEX_BFR_FULL_1 + _rt) );
#endif
}

void GFXAdapter_Directx9::resolve(RenderTarget* _target)
{
	GFX_RenderingBuffer_DX9 *rtDX = (GFX_RenderingBuffer_DX9 *) _target->m_adapterimplementationData;

	IDirect3DTexture9* ptex = rtDX->getTexture();
    if (!ptex)
		return;

#ifdef ITF_X360
    HRESULT hr;
    hr = m_pd3dDevice->Resolve( D3DRESOLVE_RENDERTARGET0 , NULL, ptex,
         NULL, 0, 0, NULL, 1.0f, 0L, NULL );
    ITF_ASSERT(hr == S_OK);
#endif
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::project(const Vec3d& _in3d, Vec2d & _out2d)
{
    D3DVIEWPORT9 vp;
    m_pd3dDevice->GetViewport(&vp);
    m_worldViewProj.setWorld(Matrix44::identity());
    D3DXVECTOR3 in, out;

    in.x = _in3d.x();
    in.y = _in3d.y();
    in.z = _in3d.z();
    D3DXVec3Project(&out, &in, &vp, (D3DXMATRIX*)&m_worldViewProj.getProj(), (D3DXMATRIX*)&m_worldViewProj.getView(), (D3DXMATRIX*)&m_worldViewProj.getWorld());
    _out2d.x() = out.x;
    _out2d.y() = out.y;    
    ITF_ASSERT(out.z>=0);
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setBlendSeparateAlpha()
{
    if (m_useSeparateAlpha)
    {
        m_pd3dDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
        m_pd3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
    }
    else
        m_pd3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
}


void GFXAdapter_Directx9::prepareGfxMatReflection( const GFX_MATERIAL& _gfxMat )
{
    /// try normal texture.
    if (isUseReflection() && getBufferTexture(TEX_BFR_REFLECTION) != NULL)
    {
        SetTextureBind(1, getBufferTextureDX(TEX_BFR_REFLECTION));
    }

    f32 reflectorFactor = _gfxMat.getTemplateMatParams().getfParamsAt(0) * _gfxMat.getMatParams().getfParamsAt(0);
    m_drawCallStates.reflectionParam.set(reflectorFactor, 0.0f, 0.0f, 0.0f);
}

void GFXAdapter_Directx9::prepareGfxMatFluid( const GFX_MATERIAL& _gfxMat )
{
    setShaderGroup(m_defaultShaderGroup);

    bbool useGlow = ( _gfxMat.getMatParams().getiParamsAt(0) != 0);

    /// try normal texture.
    if (getBufferTexture(TEX_BFR_FLUID_BLUR) != NULL)
    {
        SetTextureBind(1, getBufferTextureDX(TEX_BFR_FLUID_BLUR));
        setTextureAdressingMode(1, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);
    }

    if (useGlow && getBufferTexture(TEX_BFR_FLUID_GLOW) != NULL)
    {
        SetTextureBind(2, getBufferTextureDX(TEX_BFR_FLUID_GLOW));
        setTextureAdressingMode(2, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);
    }


	PS_Fluid fluidConsts;

	fluidConsts.fluidParam.set(_gfxMat.getMatParams().getfParamsAt(0), _gfxMat.getMatParams().getfParamsAt(1), _gfxMat.getMatParams().getfParamsAt(2), _gfxMat.getMatParams().getfParamsAt(3));
	float ramp1 = 1.0f / (fluidConsts.fluidParam.m_x - fluidConsts.fluidParam.m_y);
	float ramp2 = fluidConsts.fluidParam.m_y / (fluidConsts.fluidParam.m_x - fluidConsts.fluidParam.m_y);
	float flowSizeInv = (1.0f / fluidConsts.fluidParam.m_z) * 2.0f;
	float ramp1MinusRamp2 = ramp1 - ramp2;
	fluidConsts.fluidParam2.set(ramp1, ramp2, flowSizeInv, ramp1MinusRamp2);

	fluidConsts.fluidColor1 = _gfxMat.getMatParams().getvParamsAt(0);
	fluidConsts.fluidColor2 = _gfxMat.getMatParams().getvParamsAt(1);
	fluidConsts.fluidColor3 = _gfxMat.getMatParams().getvParamsAt(2);
	fluidConsts.fluidColor4 = _gfxMat.getMatParams().getvParamsAt(3);
	fluidConsts.fluidColor5 = _gfxMat.getMatParams().getvParamsAt(4);
	SetPixelShaderConstantF( PS_Attrib_fluid, (f32*)&fluidConsts, sizeof(fluidConsts)/16 );

    m_fluidSimuInfo.m_useEmboss = (_gfxMat.getMatParams().getiParamsAt(1) != 0);
}

///----------------------------------------------------------------------------//
/// Overdraw.
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::createOverDrawRamp( u32 _greenPos, u32 _yellowPos, u32 _redPos)
{
    if (m_overDrawRamptexture)
        DXReleaseTexture(&m_overDrawRamptexture);
    
    /// internal OverDraw Texture.
    m_mxDeviceLost.lock();

    u32 usage = 0;
#ifdef ITF_WINDOWS
    usage = m_useD3D9Ex?D3DUSAGE_DYNAMIC:0;
#endif

    HRESULT hr;
    hr = DXCreateTexture( 256, 1, 0, usage, D3DFMT_A8R8G8B8, m_useD3D9Ex?D3DPOOL_DEFAULT:D3DPOOL_MANAGED, &m_overDrawRamptexture);

    D3DLOCKED_RECT lockedRect;
    m_overDrawRamptexture->LockRect( 0, &lockedRect, NULL, 0 );
    DWORD* pData=(DWORD*)(lockedRect.pBits);
    Color ramp;
    u32 marker1 = _greenPos;
    u32 marker2 = _yellowPos;
    u32 marker3 = _redPos;

    for (u32 i = 0; i < 256; i++)
    {
        if (i == 0)
            ramp = Color::black();
        else
        {
            if (i < marker1) // blue/green.
            {
                u32 dist = marker1 - 1;
                f32 lerp = ((f32)(marker1 - i)/(f32)dist);
                ramp = Color::Interpolate(Color::green(),Color::blue(), lerp);
            }
            else if (i < marker2) // green/Yellow.
            {
                u32 dist = marker2 - marker1;
                f32 lerp = ((f32)(marker2 - i)/(f32)dist);
                ramp = Color::Interpolate(Color::yellow(), Color::green(), lerp);
            }
            else if (i < marker3) // Yellow/Red.
            {
                u32 dist = marker3 - marker2;
                f32 lerp = ((f32)(marker3 - i)/(f32)dist);
                ramp = Color::Interpolate(Color::red(), Color::yellow(), lerp);
            }
            else
                ramp = Color::red();
        }
        
        ramp.m_a = 255.f;
        pData[i] = ramp.getAsU32();
    }
    m_overDrawRamptexture->UnlockRect(0);
    m_mxDeviceLost.unlock();

}

void GFXAdapter_Directx9::showOverDrawRecord(RenderPassContext &_rdrCtx)
{
	OverDraw_ComputeDepth(_rdrCtx);

    if (getOverDrawMode() == 3)
        OverDraw_ColorRemap(_rdrCtx);
}

void GFXAdapter_Directx9::OverDraw_ComputeDepth( RenderPassContext &_rdrCtx )
{
#if defined(ITF_X360)
    #pragma message("TODO : OverDraw_ComputeDepth is not implemented on X360. ")
#else
    u32 screenW = getWindowWidth();
    u32 screenH = getWindowHeight();
	m_curPassType = GFX_ZLIST_PASS_2D;

    setBackBuffer(btrue, btrue);
    m_currentRenderingSurface = getCurrentBackBufferSurf();
    copyCurrentColorBuffer(0);

	// Allocate buffers.
	for ( ux i = 0; i < nbrBufOD; i++ )
	{
		if ( !m_overDrawBuffers[i].getTexture() )
		{
			u32 width = ITF::Max( u32(screenW >> (i+1)), u32(8) );
			u32 height = ITF::Max( u32(screenH >> (i+1)), u32(8) );
			m_overDrawBuffers[i].createBuffer( width, height, D3DFMT_A8R8G8B8, 0);
		}
	}

    static LPDIRECT3DSURFACE9 pOffscreenSurf = NULL;
	u32 reduceWidth = m_overDrawBuffers[nbrBufOD-1].getWidth();
	u32 reduceHeight = m_overDrawBuffers[nbrBufOD-1].getHeight();

    if (!pOffscreenSurf)
    {
        HRESULT hr = DXCreateOffscreenPlainSurface(reduceWidth, reduceHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pOffscreenSurf, NULL);
        if (hr!=S_OK)
            messageD3DError(hr);
    }
    
    PrimitiveContext primCtx = PrimitiveContext(&_rdrCtx);
    DrawCallContext drawCallCtx = DrawCallContext(&primCtx);
    Matrix44 World;
    World.setIdentity();
    setObjectMatrix(World);

	// Reduce screen.
	for ( ux i = 0; i < nbrBufOD; i++ )
	{
        setInternalRenderTarget(m_overDrawBuffers[i].getSurface(), NULL);

		setShaderGroup(mc_shader_AfterFx);
		setGfxMatDefault(drawCallCtx);
		setVertexShaderVariants(mc_entry_afterFx_VS_PCT);
		setPixelShaderVariants(mc_entry_afterFx_PS_copyOverDraw);

		f32 W = (f32)m_overDrawBuffers[i].getWidth();
		f32 H = (f32)m_overDrawBuffers[i].getHeight();

		GFX_Vector4 pconsts(W, H, 0.0f, 0.0f);
		SetPixelShaderConstantF( PS_Attrib_Overdraw, (f32*)&pconsts, 1);

		drawCallCtx.getStateCache().setDepthTest(bfalse)
			.setDepthWrite(bfalse)
			.setAlphaBlend(GFX_BLEND_COPY);

		if ( i == 0 )
			SetTextureBind(0, getBufferTextureDX(TEX_BFR_FULL_1), btrue);
		else
			SetTextureBind(0, &m_overDrawBuffers[i-1], btrue);

		setOrthoView(0.f, W, 0.f, H);

		VertexPCT quad[4];
		f32 centroid = 0.5f;
		quad[0].setData( Vec3d( 0.f - centroid, 0.f + centroid, 0.f ), Vec2d( 0.f, 1.0f ), COLOR_WHITE);
		quad[1].setData( Vec3d( 0.f - centroid, 0.f + H + centroid, 0.f ), Vec2d( 0.f, 0.f ), COLOR_WHITE);
		quad[2].setData( Vec3d( 0.f + W - centroid, 0.f + centroid, 0.f ), Vec2d( 1.0f, 1.0f ), COLOR_WHITE);
		quad[3].setData( Vec3d( 0.f + W - centroid, 0.f + H + centroid, 0.f ), Vec2d( 1.0f, 0.0f ), COLOR_WHITE);
    
		setVertexFormat(VertexFormat_PCT);
		DrawPrimitive(drawCallCtx, GFX_TRIANGLE_STRIP, (void*)quad, 4);
	}

    HRESULT hr = m_pd3dDevice->GetRenderTargetData(m_overDrawBuffers[nbrBufOD-1].getSurface(), pOffscreenSurf);
    if (hr!=S_OK)
        messageD3DError(hr);

	f32 depthSum = 0;
    D3DLOCKED_RECT lockedrect;
    if (S_OK == pOffscreenSurf->LockRect(&lockedrect, NULL, D3DLOCK_READONLY))
    {
		u32 pitch = lockedrect.Pitch / 4;
        u32 *pixelsPtr = reinterpret_cast<u32 *>(lockedrect.pBits);
		for ( ux y = 0; y < reduceHeight; y++ )
		{
			for ( ux x = 0; x < reduceWidth; x++ )
			{
				ux colInt = *(pixelsPtr + y * pitch + x);
				Color colFlt(colInt);
				depthSum += colFlt.getGreen();
			}
		}
		pOffscreenSurf->UnlockRect();
		depthSum *= 1.0f / (reduceWidth * reduceHeight);
		depthSum *= 255.0f / 16.0f;
    }
	m_overDrawDepth = depthSum;

	m_curPassType = GFX_ZLIST_PASS_REGULAR;
    setShaderGroup(m_defaultShaderGroup);

	setBackBuffer(btrue, btrue);
    m_currentRenderingSurface = getCurrentBackBufferSurf();
	setOrthoView(0.f, f32(screenW), 0.f, f32(screenH));
#endif
}

void GFXAdapter_Directx9::OverDraw_ColorRemap(RenderPassContext &_rdrCtx)
{
    PrimitiveContext primCtx = PrimitiveContext(&_rdrCtx);
    DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

    Matrix44 World;
    World.setIdentity();
    setObjectMatrix(World);

    setShaderGroup(mc_shader_AfterFx);
    setGfxMatDefault(drawCallCtx);
    setVertexShaderVariants(mc_entry_afterFx_VS_PCT);
    setPixelShaderVariants(mc_entry_afterFx_PS_colorOverDraw);

	PlatformTexture overDrawTexWrapper;
	overDrawTexWrapper.m_DXTexture = m_overDrawRamptexture;
    SetTextureBind(3, &overDrawTexWrapper, bfalse);
	overDrawTexWrapper.m_DXTexture = nullptr;

    f32 W = (f32)getWindowWidth();
    f32 H = (f32)getWindowHeight();

    drawCallCtx.getStateCache().setDepthTest(bfalse)
        .setDepthWrite(bfalse)
        .setAlphaBlend(GFX_BLEND_COPY);

    SetTextureBind(0, getBufferTextureDX(TEX_BFR_FULL_1), bfalse);

    VertexPCT quad[4];
    f32 centroid = 0.5f;
    quad[0].setData( Vec3d( 0.f - centroid, 0.f + centroid, 0.f ), Vec2d( 0.f, 1.0f ), COLOR_WHITE);
    quad[1].setData( Vec3d( 0.f - centroid, 0.f + H + centroid, 0.f ), Vec2d( 0.f, 0.f ), COLOR_WHITE);
    quad[2].setData( Vec3d( 0.f + W - centroid, 0.f + centroid, 0.f ), Vec2d( 1.0f, 1.0f ), COLOR_WHITE);
    quad[3].setData( Vec3d( 0.f + W - centroid, 0.f + H + centroid, 0.f ), Vec2d( 1.0f, 0.0f ), COLOR_WHITE);
    
    setVertexFormat(VertexFormat_PCT);
    DrawPrimitive(drawCallCtx, GFX_TRIANGLE_STRIP, (void*)quad, 4);

    setShaderGroup(m_defaultShaderGroup);
}

void GFXAdapter_Directx9::showDebugDraw(RenderPassContext &_rdrCtx)
{
    PrimitiveContext primCtx = PrimitiveContext(&_rdrCtx);
    DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

    Matrix44 World;
    World.setIdentity();
    setObjectMatrix(World);

    m_currentRenderingSurface = getTargetDX(m_BackBuffer)->getSurface();
    copyCurrentColorBuffer(0);

    setGfxMatDefault(drawCallCtx);

    shaderDefineKey psDefine = 0;
    if (getDebugDrawMode() == 1)
    {
        psDefine |= mc_define_afterFx_Saturation;
    }
    if (getDebugDrawMode() == 2)
    {
        psDefine |= mc_define_afterFx_EdgeDetection;
    }
    if (getDebugDrawMode() == 3)
    {
        psDefine |= mc_define_afterFx_Saturation | mc_define_afterFx_EdgeDetection;
    }

	PS_DebugPass debugConsts;
	debugConsts.viewportDimensions = m_viewportDimensions;
    SetPixelShaderConstantF( PS_Attrib_debugPass, (f32*)&debugConsts, sizeof(debugConsts)/16 );

    setShaderGroup( mc_shader_AfterFx, mc_entry_afterFx_VS_PCT, mc_entry_afterFx_PS_DebugPass, 0, psDefine);

    f32 W = (f32)getWindowWidth();
    f32 H = (f32)getWindowHeight();

    drawCallCtx.getStateCache().setDepthTest(bfalse)
        .setDepthWrite(bfalse)
        .setAlphaBlend(GFX_BLEND_COPY);

    SetTextureBind(0, getBufferTextureDX(TEX_BFR_FULL_1), bfalse);

    VertexPCT quad[4];
    f32 centroid = 0.5f;
    quad[0].setData( Vec3d( 0.f - centroid, 0.f + centroid, 0.f ), Vec2d( 0.f, 1.0f ), COLOR_WHITE);
    quad[1].setData( Vec3d( 0.f - centroid, 0.f + H + centroid, 0.f ), Vec2d( 0.f, 0.f ), COLOR_WHITE);
    quad[2].setData( Vec3d( 0.f + W - centroid, 0.f + centroid, 0.f ), Vec2d( 1.0f, 1.0f ), COLOR_WHITE);
    quad[3].setData( Vec3d( 0.f + W - centroid, 0.f + H + centroid, 0.f ), Vec2d( 1.0f, 0.0f ), COLOR_WHITE);

    setVertexFormat(VertexFormat_PCT);
    DrawPrimitive(drawCallCtx, GFX_TRIANGLE_STRIP, (void*)quad, 4);

    setShaderGroup(m_defaultShaderGroup);
}

#ifdef ITF_SUPPORT_VIDEOCAPTURE
void VideoCaptureBuffer_Directx9::release()
{
    if (m_captureOffscreenSurface)
    {
        LPDIRECT3DSURFACE9 surf = (LPDIRECT3DSURFACE9)m_captureOffscreenSurface;
        D3D_NM_RELEASE(surf);
        m_captureOffscreenSurface = NULL;
    }
    
    if (m_captureStretchSurface)
    {
        LPDIRECT3DSURFACE9 surf = (LPDIRECT3DSURFACE9)m_captureStretchSurface;
        D3D_NM_RELEASE(surf);
        m_captureStretchSurface = NULL;
    }

}


void VideoCaptureBuffer_Directx9::lock()
{
    ITF_ASSERT(m_dataPtr == NULL);
    D3DLOCKED_RECT lockedrect;
    LPDIRECT3DSURFACE9 surface = (LPDIRECT3DSURFACE9) m_captureOffscreenSurface;
    ITF_VERIFY(S_OK == surface->LockRect(&lockedrect,NULL,D3DLOCK_READONLY));
    
    m_dataPtr = lockedrect.pBits;
    m_pitch = lockedrect.Pitch;
}

void VideoCaptureBuffer_Directx9::unlock()
{
    ITF_ASSERT(m_dataPtr != NULL);

    LPDIRECT3DSURFACE9 surface = (LPDIRECT3DSURFACE9) m_captureOffscreenSurface;
    surface->UnlockRect();

    
    m_dataPtr = NULL;
}

#endif //ITF_SUPPORT_VIDEOCAPTURE

void GFXAdapter_Directx9::computeD3DMacro(vector<D3DXMACRO> & _d3dmacro, const char* _define[], u32 _numDef, bbool _isPixel)
{
    _d3dmacro.resize(_numDef + 2); // added 1 for profile, and 1 for ending mark
    for(u32 i=0; i<_numDef; i++)
    {
        _d3dmacro[i].Name = _define[i];
        _d3dmacro[i].Definition = "1";
    }

    _d3dmacro[_numDef].Name = _isPixel?"PIXEL_PROFILE":"VERTEX_PROFILE";
    _d3dmacro[_numDef].Definition = "1";

    _d3dmacro[_numDef+1].Name = 0;
    _d3dmacro[_numDef+1].Definition = 0;
}


void        GFXAdapter_Directx9::invokeExternalShaderCompiler(const char* _dataRootPath)
{
#ifdef ITF_WINDOWS

    ITF_WARNING_CATEGORY(Engine, 0, 0, "Compiled shaders not up to date, being compiled at startup");
    LOG("Compiling shaders, please wait...");

    String8 executable = "ShaderCompiler.exe";
    String8 arguments = _dataRootPath;
    arguments += " platform_PC";
#if defined(ROBOT_COMPILATION)
    arguments += " -usrDir";
#endif

    ProcessSpawner processSpawner;
    processSpawner.create(Path(executable, Path::FlagNonData), arguments, "", bfalse, btrue);
#endif
}

void* GFXAdapter_Directx9::compilePixelShader(const char* _fxFileName, const char* _functionName, const char* _define[], u32 _numDef)
{
    ID3DXBuffer* buffer = 0;
    ID3DXBuffer* compileError = 0;
    IDirect3DPixelShader9  * pShaderResult = 0;
    HRESULT   hr;

    vector<D3DXMACRO> dxMacros;
    ITF_ASSERT(_numDef <= 32);
    computeD3DMacro(dxMacros, _define, _numDef, btrue);

    hr = D3DXCompileShaderFromFile( (LPCWSTR)UTF8ToUTF16(_fxFileName).get(), &dxMacros.front(), 0, (LPCSTR)_functionName,  "ps_3_0", 0, &buffer, &compileError, NULL );
    
    if ( SUCCEEDED( hr ) )
    {
        hr = DXCreatePixelShader( (DWORD*) buffer->GetBufferPointer(), &pShaderResult );
        buffer->Release();

        if ( FAILED( hr ) )
        {
            return( 0 );
        }
    }
    else
    {
        if (compileError)
        {
            const char * errorMsg = reinterpret_cast<const char *>(compileError->GetBufferPointer());
            String8 defines;
            for(ux i = 0; i < _numDef; ++i)
            {
                defines += _define[i];
                defines += " ";
            }
            LOG("Shader compilation error on %s with defines %s:\n", _functionName, defines.cStr());
            LOG("%s\n", errorMsg);
            compileError->Release();
        }
    }

    return pShaderResult;
}

void* GFXAdapter_Directx9::compileVertexShader(const char* _fxFileName, const char* _functionName, const char* _define[], u32 _numDef)
{
    ID3DXBuffer* buffer = 0;
    ID3DXBuffer* compileError = 0;
    IDirect3DVertexShader9  * pShaderResult = 0;
    HRESULT hr;
    
    vector<D3DXMACRO> dxMacros;
    ITF_ASSERT(_numDef <= 32);
    computeD3DMacro(dxMacros, _define, _numDef, bfalse);

    hr = D3DXCompileShaderFromFile( (LPCWSTR)UTF8ToUTF16(_fxFileName).get(), &dxMacros.front(), 0, (LPCSTR)_functionName,  "vs_3_0", 0, &buffer, &compileError, NULL );
    
    if ( SUCCEEDED( hr ) )
    {
        hr = DXCreateVertexShader( (DWORD*) buffer->GetBufferPointer(), &pShaderResult );
        buffer->Release();

        if ( FAILED( hr ) )
        {
            return( 0 );
        }
    }
    else
    {
        if (compileError)
        {
            const char * errorMsg = reinterpret_cast<const char *>(compileError->GetBufferPointer());
            String8 defines;
            for(ux i = 0; i < _numDef; ++i)
            {
                defines += _define[i];
                defines += " ";
            }
            LOG("Shader compilation error on %s with defines %s:\n", _functionName, defines.cStr());
            LOG("%s\n", errorMsg);
            compileError->Release();
        }
    }

    return pShaderResult;
}

void GFXAdapter_Directx9::setVertexShader( void * _shaderBin )
{
    m_pd3dDevice->SetVertexShader( (IDirect3DVertexShader9*)_shaderBin );
}

void GFXAdapter_Directx9::setPixelShader( void * _shaderBin )
{
    m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)_shaderBin );
}


void    GFXAdapter_Directx9::SetVertexShaderConstantF(u32 location, const f32 *floats, u32 count)
{
    m_pd3dDevice->SetVertexShaderConstantF( location, floats, count);
}

void    GFXAdapter_Directx9::SetPixelShaderConstantF(u32 location, const f32 *floats, u32 count)
{
    m_pd3dDevice->SetPixelShaderConstantF( location, floats, count);
}

//--------------------------------------------------------------------------------
// ScreenShot 
//--------------------------------------------------------------------------------


void GFXAdapter_Directx9::copyRenderTargetToScreenshot(RenderPassContext & _rdrCtx, ux _width, ux _height)
{
#ifdef ITF_WINDOWS
    u32 EDRAMOffset = 0;
#else
    u32 EDRAMOffset = getTargetDX(m_backBufferNoTile)->getSurface()->Size;
#endif

    m_asyncScreenshotBuffer = createTarget(this, _width, _height, Texture::PF_RGBA, 0, EDRAMOffset);

#ifdef ITF_WINDOWS

    ITF_ASSERT(!m_asyncScreenshotOffscreenSurf);
    HRESULT hr = DXCreateOffscreenPlainSurface(_width, _height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &m_asyncScreenshotOffscreenSurf, NULL);
    if (hr!=S_OK)
        messageD3DError(hr);

    D3DSURFACE_DESC desc;
    m_currentRenderingSurface->GetDesc(&desc);

    RECT rectSrc;
    rectSrc.left   = 0;
    rectSrc.right  = desc.Width;
    rectSrc.top    = 0;
    rectSrc.bottom = desc.Height;

    RECT rectDest;
    rectDest.left   = 0;
    rectDest.right  = _width;
    rectDest.top    = 0;
    rectDest.bottom = _height;
    ITF_VERIFY(D3D_OK == m_pd3dDevice->StretchRect( m_currentRenderingSurface, &rectSrc,
        getTargetDX(m_asyncScreenshotBuffer)->getSurface(), &rectDest,
        D3DTEXF_LINEAR));
#else // X360 - not tested
    //ITF_ASSERT(0);
    // probably need a resolve here
    PrimitiveContext primCtx = PrimitiveContext(&_rdrCtx);
    DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

    drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ALPHA)
        .setDepthTest(bfalse)
        .setDepthWrite(bfalse);

    setShaderGroup(m_defaultShaderGroup);
    setGfxMatDefault(drawCallCtx);
    bindTexture(0, m_backBufferNoTile, btrue);
    drawScreenQuad(drawCallCtx, 0, 0, (f32)getTargetDX(m_asyncScreenshotBuffer)->getWidth(), (f32)getTargetDX(m_asyncScreenshotBuffer)->getHeight(), 0, Color(1.f, 1.f, 1.f, 1.f).getAsU32(), bfalse);
    bindTexture(0, nullptr, bfalse);
#endif
}

void GFXAdapter_Directx9::releaseScreenshotSurface()
{
	m_asyncScreenshotBuffer->release();
	m_asyncScreenshotBuffer = NULL;
    D3D_NM_RELEASE(m_asyncScreenshotOffscreenSurf);
}

void GFXAdapter_Directx9::copyScreenshotSurfaceToRGBABuffer(u32 * _buffer, ux _width, ux _height)
{
    ITF_ASSERT(_width == m_asyncScreenshotBuffer->getSizeX());
    ITF_ASSERT(_height == m_asyncScreenshotBuffer->getSizeY());

#ifdef ITF_WINDOWS

    HRESULT hr = m_pd3dDevice->GetRenderTargetData(getTargetDX(m_asyncScreenshotBuffer)->getSurface(), m_asyncScreenshotOffscreenSurf);
    if (hr!=S_OK)
        messageD3DError(hr);

    D3DLOCKED_RECT lockedrect;
    LPDIRECT3DSURFACE9 surfToLock = m_asyncScreenshotOffscreenSurf;
    HRESULT hres = surfToLock->LockRect(&lockedrect, NULL, D3DLOCK_READONLY);
    ITF_ASSERT_CRASH(hres ==  S_OK, "Unexpected failed lock");
    if (hres ==  S_OK)
    {
        u32 pixelPitch = u32(lockedrect.Pitch) / sizeof(u32);
        u32 * src = reinterpret_cast<u32 *>(lockedrect.pBits);
        u32 * dst = _buffer;

        for(ux y=0; y<_height; ++y)
        {
            ITF_Memcpy(dst, src, _width*sizeof(u32));
            src += pixelPitch;
            dst += _width;
        }
    }
    surfToLock->UnlockRect();

#endif
}


void GFXAdapter_Directx9::messageD3DError(HRESULT _hr)
{
    switch(_hr)
    {
        case D3DERR_INVALIDCALL:
        #ifdef USE_D3D_MEMORY_TRACKER
            m_D3DMemTracker.dump(btrue);
        #endif
            ITF_ASSERT_CRASH(0, "messageD3DError D3DERR_INVALIDCALL");
        break;
        case D3DERR_INVALIDDEVICE:
            ITF_ASSERT_CRASH(0, "messageD3DError D3DERR_INVALIDDEVICE");
        break;
        case D3DERR_OUTOFVIDEOMEMORY:
            ITF_ASSERT_CRASH(0, "messageD3DError D3DERR_OUTOFVIDEOMEMORY");
        break;
        case D3DERR_NOTAVAILABLE:
            ITF_ASSERT_CRASH(0, "messageD3DError D3DERR_NOTAVAILABLE");
        break;
        case D3DERR_DEVICENOTRESET:
            ITF_ASSERT_CRASH(0, "messageD3DError D3DERR_DEVICENOTRESET");
        break;
        case D3DERR_DEVICELOST:
            //this error could happen,the engine needs to handle it
        break;
        case D3DERR_MOREDATA:
            ITF_ASSERT_CRASH(0, "messageD3DError D3DERR_MOREDATA");
        break;
        case D3DERR_NOTFOUND:
            ITF_ASSERT_CRASH(0, "messageD3DError D3DERR_NOTFOUND");
        break;
        default:
            ITF_ASSERT_CRASH(0, "messageD3DError unknow error");
            break;
    }
}


#ifdef ITF_SUPPORT_RESET_DEVICE
void GFXAdapter_Directx9::VertexBufferManagerOnLostDevice()
{
    if(!m_useD3D9Ex)
        m_VertexBufferManager.UnallocAllDynamicVB();
#ifdef VBMANAGER_USE_DYNAMICRING_VB
    m_VertexBufferManager.deleteDynamicRingVB();
#endif
}
void GFXAdapter_Directx9::VertexBufferManagerOnResetDevice()
{
    if(!m_useD3D9Ex)
        m_VertexBufferManager.ReallocAllDynamicVB();
}
void GFXAdapter_Directx9::VertexBufferOnResetDevice(ITF_VertexBuffer* _vertexBuffer )
{
    createVertexBuffer(_vertexBuffer);
}
void GFXAdapter_Directx9::VertexBufferOnLostDevice(ITF_VertexBuffer* _vertexBuffer )
{
    releaseVertexBuffer(_vertexBuffer);
}

void GFXAdapter_Directx9::IndexBufferManagerOnLostDevice()
{
    m_VertexBufferManager.UnallocAllDynamicIB();
}
void GFXAdapter_Directx9::IndexBufferManagerOnResetDevice()
{
    m_VertexBufferManager.ReallocAllDynamicIB();
}
void GFXAdapter_Directx9::IndexBufferOnResetDevice(ITF_IndexBuffer* _indexBuffer )
{
    releaseIndexBuffer(_indexBuffer);
}
void GFXAdapter_Directx9::IndexBufferOnLostDevice(ITF_IndexBuffer* _indexBuffer )
{
    createIndexBuffer(_indexBuffer);
}
#endif //ITF_SUPPORT_RESET_DEVICE

//--------------------------------------------------------------------------------
// D3D mem tracker 
//--------------------------------------------------------------------------------


#ifdef USE_D3D_MEMORY_TRACKER
D3DMemTracker::D3DMemTracker()
:m_globalNumAlloc(0), m_currentNumAlloc(0)
{
#if defined(USE_D3D_MEMORY_CALLSTACK_TRACKER_FOR_NOTMANAGE) || defined(USE_D3D_MEMORY_CALLSTACK_TRACKER_FOR_NOTMANAGE)
    StackWalker::initialize();
#endif
}

D3DMemTracker::~D3DMemTracker()
{

}

void D3DMemTracker::addRessource(uPtr _ptr, memTraceType _type, bbool _managed )
{
    m_mxChangeArray.lock();
    memTrace trace = memTrace(_ptr, _type, m_globalNumAlloc);
    if(!_managed)
    {
#ifdef USE_D3D_MEMORY_CALLSTACK_TRACKER_FOR_NOTMANAGE
        trace.stackDepth = StackWalker::retrieveStackFast(0, trace.stack ,MAX_STACK_DEPTH);
#endif
        m_traceNotManagedArray.push_back(trace);
    }
    else
    {
#ifdef USE_D3D_MEMORY_CALLSTACK_TRACKER_FOR_MANAGE
        trace.stackDepth = m_stackWalker.retrieveStackFast(0, trace.stack ,MAX_STACK_DEPTH);
#endif
        m_traceArray.push_back(trace);
    }
    m_globalNumAlloc++;
    m_currentNumAlloc++;
    m_mxChangeArray.unlock();
}

void D3DMemTracker::releaseRessource(uPtr _ptr)
{
    m_mxChangeArray.lock();
    memTrace trace;
    trace.ptr = _ptr;
    i32 id = m_traceArray.find(trace);
    if(id >= 0)
    {
        m_traceArray.eraseNoOrder(id);
    }
    else
    {
        i32 id2 = m_traceNotManagedArray.find(trace);
        if(id2 >= 0)
        {
            m_traceNotManagedArray.eraseNoOrder(id2);
        }
    }
    m_mxChangeArray.unlock();
    m_currentNumAlloc--;
}

void D3DMemTracker::dump(bbool _onlyManagedRessource)
{
    OutputDebugger dbg;
    u32 nbRessourceManaged = m_traceArray.size();
    u32 nbRessourceNotManaged = m_traceNotManagedArray.size();
    if(_onlyManagedRessource)
        nbRessourceManaged = 0;
    dbg << "D3D leaks detector : " << nbRessourceManaged+nbRessourceNotManaged << "  missing release ressource" << "\n";

    m_mxChangeArray.lock();
    for(u32 i = 0; i < nbRessourceNotManaged; i++)
    {
        dbg << "    Ressource No Managed ptr: " << m_traceNotManagedArray[i].ptr << "  type: " << m_traceNotManagedArray[i].type << "  allocNum: " << m_traceNotManagedArray[i].allocNum << "\n";
#ifdef USE_D3D_MEMORY_CALLSTACK_TRACKER_FOR_NOTMANAGE
        String8 callstack_custom;
        ProcessAddressDescriptor desc;

        for(u32 j = 0; j < m_traceNotManagedArray[i].stackDepth; ++j)
        {
            u64 addr = m_traceNotManagedArray[i].stack[j];
            if (addr)
            {
                StackWalker::fillDescriptor(addr,desc);
                if (desc.m_line > 0)
                {
                    if(j != 0)
                        callstack_custom += "(...)|";
                    callstack_custom += desc.m_symbolName;
                    callstack_custom += "\n";
                }
            }
        }
        dbg << callstack_custom.cStr() << "\n";
#endif
    }

    if(!_onlyManagedRessource)
    {
        for(u32 i = 0; i < nbRessourceManaged; i++)
        {
            dbg << "    Ressource Managed ptr: " << m_traceArray[i].ptr << "  type :" << m_traceArray[i].type << "  allocNum: " << m_traceArray[i].allocNum << "\n";
        }
    }
    m_mxChangeArray.unlock();
}
#endif

#ifdef ITF_SUPPORT_GPU_METRICS

GFXAdapter_metrics_DX9::GFXAdapter_metrics_DX9():
    m_Enabled(bfalse),
    m_tagArrayIdx(0),
    m_pD3DDevice(NULL)
{
}

GFXAdapter_metrics_DX9::~GFXAdapter_metrics_DX9()
{
    for(u32 i = 0; i< c_metricsTagMax; i++)
    {
        delete m_TagArray[i];
        m_TagArray[i] = NULL;
    }
    m_TagArray.clear();

    m_pD3DDevice->EnablePerfCounters(FALSE);
}

void GFXAdapter_metrics_DX9::init(IDirect3DDevice9 * _pD3DDevice)
{
    m_pD3DDevice = _pD3DDevice;
    for(u32 i = 0; i< c_metricsTagMax; i++)
    {
        PerfCounterManager* pPerfCounter = newAlloc(mId_GfxAdapter, PerfCounterManager(m_pD3DDevice));
        m_TagArray.push_back(pPerfCounter);
    }
}

void GFXAdapter_metrics_DX9::activateGpuProfile(bbool _on)
{
    if(!m_Enabled)
    {
        m_tagArrayIdx = 0;
        m_Enabled = btrue;
    }
    m_pD3DDevice->EnablePerfCounters(TRUE);
}

void GFXAdapter_metrics_DX9::endDraw()
{
    if(m_Enabled)
    {
        m_tagArrayIdx = 0;
    }
}

void GFXAdapter_metrics_DX9::getNewMetricsTag(u32 *_tag)
{
    *_tag = m_tagArrayIdx;
    
    if(m_tagArrayIdx<c_metricsTagMax)
    {
        m_tagArrayIdx++;
    }
    else
    {
        *_tag = U32_INVALID;
    }
}

void GFXAdapter_metrics_DX9::startTag(u32 _tag)
{
    if(m_Enabled && m_tagArrayIdx<c_metricsTagMax && m_tagArrayIdx!=U32_INVALID)
        m_TagArray[_tag]->start();
}

void GFXAdapter_metrics_DX9::endTag(u32 _tag)
{
    if(m_Enabled && m_tagArrayIdx<c_metricsTagMax && m_tagArrayIdx!=U32_INVALID)
        m_TagArray[_tag]->stop();
}

u64  GFXAdapter_metrics_DX9::getMetricU64(u32 _metric, u32 _tag) const
{
    return (u64)(getMetricF32(_metric, _tag) * 1000);
}

f32  GFXAdapter_metrics_DX9::getMetricF32(u32 _metric, u32 _tag) const
{
    if(m_Enabled && m_tagArrayIdx<c_metricsTagMax && m_tagArrayIdx!=U32_INVALID)
    {
        return m_TagArray[_tag]->getGPUFrameTime();
    }
    else
    {
        return 0.0f;
    }
}

#endif //ITF_SUPPORT_GPU_METRICS

} // namespace ITF
