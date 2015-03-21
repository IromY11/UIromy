#include "precompiled_GFXAdapter_Directx9.h"

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_

#ifdef ITF_X360
#ifdef ITF_X360
#include <fxl.h>
#include <xgraphics.h>
#endif

#include <math.h>
#include <d3dx9.h>
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

#ifndef ITF_CORE_UNICODE_TOOLS_H_
#include "core/UnicodeTools.h"
#endif // ITF_CORE_UNICODE_TOOLS_H_

#ifndef _ITF_SYSTEMADAPTER_X360_H_
#include "adapters/SystemAdapter_x360/SystemAdapter_x360.h"
#endif //_ITF_SYSTEMADAPTER_X360_H_

#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_

#endif //X360

namespace ITF
{

void GFXAdapter_Directx9::BuildPresentParamsX360()
{
#ifdef ITF_X360

    ZeroMemory(&m_ppars, sizeof(m_ppars));

    if (getScreenWidth()  == 1920)
        setUseDepthBuffer(btrue);

    XVIDEO_MODE videoMode; 
    XMemSet( &videoMode, 0, sizeof(XVIDEO_MODE) ); 
    XGetVideoMode( &videoMode );

    m_ppars.BackBufferWidth = getScreenWidth();
    m_ppars.BackBufferHeight = getScreenHeight();
    m_ppars.BackBufferFormat = D3DFMT_A8R8G8B8;
    m_ppars.BackBufferCount = 0;
    m_ppars.MultiSampleType = D3DMULTISAMPLE_NONE;
    m_ppars.MultiSampleQuality = 0;
    m_ppars.SwapEffect = D3DSWAPEFFECT_COPY;
    m_ppars.hDeviceWindow = NULL;
    m_ppars.Windowed = FALSE;
    m_ppars.EnableAutoDepthStencil = bfalse;//isUseDepthBuffer();
#ifdef _USE_ZBUFFER_FLOAT
    m_ppars.AutoDepthStencilFormat = D3DFMT_D24FS8;
#else
    m_ppars.AutoDepthStencilFormat = D3DFMT_D24S8;
#endif  

    m_ppars.Flags = 0;
    m_ppars.FullScreen_RefreshRateInHz = 0;
    m_ppars.PresentationInterval = m_waitVBL ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

    m_monitorRefreshRate = videoMode.RefreshRate;

#ifdef _USE_ZBUFFER_TILING    
    m_ppars.BackBufferFormat = ( D3DFORMAT )( D3DFMT_A8R8G8B8 );
    m_ppars.FrontBufferFormat = ( D3DFORMAT )( D3DFMT_LE_X8R8G8B8 );

    m_ppars.DisableAutoBackBuffer = TRUE;
    m_ppars.DisableAutoFrontBuffer = TRUE;
#endif

#endif //ITF_X360
}


bbool GFXAdapter_Directx9::createDXDeviceX360()
{
#ifdef ITF_X360

    // Create the D3D object, which is needed to create the D3DDevice.
    if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return 0;

    const bbool forceFullHDResolution = bfalse;
    // Video Mode
    XVIDEO_MODE VideoMode; 
    XMemSet( &VideoMode, 0, sizeof(XVIDEO_MODE) ); 
    XGetVideoMode( &VideoMode );

    u32 width   = VideoMode.dwDisplayWidth;
    u32 height  = VideoMode.dwDisplayHeight;
    if (!VideoMode.fIsWideScreen)
    {
        // compute the useful height to stay with a 16/9 display aspect ratio
        height = u32(float(VideoMode.dwDisplayHeight) * (9.f/16.f) * (4.f/3.f));
    }

    // We choose between two different resolution in order to avoid
    // too much reduction (to keep text readable)
    if (width >= 1280 || height >= 720 || forceFullHDResolution)
    {
        width   = 1920;
        height  = 1080;
    }
    else
    {
        width   = 1280;
        height  = 720;
    }
    setResolution(width, height);

    //PredicatedTiling
    u32 BackBufferWidth = getScreenWidth();
    u32 BackBufferHeight = getScreenHeight();

    m_TilingParam.usePredicatedTiling = bfalse;
    //bbool isFullHD = (BackBufferWidth == 1920 && BackBufferHeight == 1080);
    if(isUseDepthBuffer() /*&& isFullHD*/)
    {
        m_TilingParam.usePredicatedTiling = btrue;
    }

    if(m_TilingParam.usePredicatedTiling)
    {
        //Compute Tiling Size for RenderTarget
        m_TilingParam.backBufferFullWidth = getScreenWidth();
        m_TilingParam.backBufferFullHeight = getScreenHeight();

        const u32 numTile = 2;

        u32 TileWidth = BackBufferWidth;
        u32 TileHeight = BackBufferHeight / numTile;

        //For D3DMULTISAMPLE_NONE
        TileWidth = XGNextMultiple( TileWidth, GPU_EDRAM_TILE_WIDTH_1X );
        TileHeight = XGNextMultiple( TileHeight, GPU_EDRAM_TILE_HEIGHT_1X );

        // Expand tile surface dimensions to texture tile size, if it isn't already
        TileWidth = XGNextMultiple( TileWidth, GPU_TEXTURE_TILE_DIMENSION );
        TileHeight = XGNextMultiple( TileHeight, GPU_TEXTURE_TILE_DIMENSION );

        BackBufferWidth = TileWidth;
        BackBufferHeight = TileHeight;

        m_TilingParam.tileCount = numTile;
        m_TilingParam.TilingFlags = 0;
        m_TilingParam.constantOffset = PS_Attrib_tillingVposOffset; //must be a multiple of 4


        m_TilingParam.surfaceRect[0].x1 = 0;
        m_TilingParam.surfaceRect[0].y1 = 0;
        m_TilingParam.surfaceRect[0].x2 = TileWidth;
        m_TilingParam.surfaceRect[0].y2 = TileHeight;
        m_TilingParam.surfaceRect[1].x1 = 0;
        m_TilingParam.surfaceRect[1].y1 = TileHeight;
        m_TilingParam.surfaceRect[1].x2 = TileWidth;
        m_TilingParam.surfaceRect[1].y2 = m_TilingParam.backBufferFullHeight;
    }
    m_isPredicatedTilingPass = bfalse;

    //Create Device
    BuildPresentParamsX360();

    DWORD bevahiorFlags = 0;
//#ifdef _USE_ZBUFFER_TILING
    bevahiorFlags = D3DCREATE_BUFFER_2_FRAMES;
//#endif  

    if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd,
                                      bevahiorFlags,&m_ppars, &m_pd3dDevice ) ) )
    {
        ITF_ASSERT_MSG(0, "FAILED( m_pD3D->CreateDevice...");
        return bfalse;
    }

    D3DCAPS9 caps;
    m_pd3dDevice->GetDeviceCaps(&caps);
    if (!(caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST))
    {
        ITF_FATAL_ERROR("Direct3D9Renderer: Hardware does not support D3DPRASTERCAPS_SCISSORTEST.  Unable to proceed." );
    }

    m_maxTextureSize = std::min(caps.MaxTextureHeight, caps.MaxTextureWidth);
    m_supportNonSquareTex = !(caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY);
    m_supportNPOTTex = !(caps.TextureCaps & D3DPTEXTURECAPS_POW2) ||
                       (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL);


    //BackBuffer RenderTarget

    // Use custom EDRAM allocation to create the backBuffer.
    // The color rendertarget is placed at address 0 in EDRAM.
    D3DSURFACE_PARAMETERS SurfaceParams;
    memset( &SurfaceParams, 0, sizeof( D3DSURFACE_PARAMETERS ) );
    SurfaceParams.Base = 0;
    SurfaceParams.HierarchicalZBase = 0;

    static MEMORYSTATUS stat,stat2;
    GlobalMemoryStatus( &stat );

    m_BackBuffer = createTarget(this, BackBufferWidth, BackBufferHeight, Texture::PF_RGBA, GFX_RenderingBuffer_DX9::e_noTextureBuffer);
    
    GlobalMemoryStatus( &stat2 );
    
    //Z Buffer
    if(isUseDepthBuffer() && m_TilingParam.usePredicatedTiling)
    {
        // The Z rendertarget is placed just after Color buffer in EDRAM.
        m_DepthStencil = createTarget(this, BackBufferWidth, BackBufferHeight, Texture::PF_D24S8, GFX_RenderingBuffer_DX9::e_noTextureBuffer,
			                          getTargetDX(m_BackBuffer)->getSurface()->Size);
    }

#ifdef _USE_ZBUFFER_TILING
    DXCreateTexture( getScreenWidth(),
        getScreenHeight(),
        1, 0,
        ( D3DFORMAT )( D3DFMT_LE_X8R8G8B8 ),
        D3DPOOL_DEFAULT,
        &m_pFrontBufferTexture[0]);

    DXCreateTexture( getScreenWidth(),
        getScreenHeight(),
        1, 0,
        ( D3DFORMAT )( D3DFMT_LE_X8R8G8B8 ),
        D3DPOOL_DEFAULT,
        &m_pFrontBufferTexture[1]);
#endif

    m_currFrontBuffer = 0;
    m_backBufferNoTile = createTarget(this, getScreenWidth(), getScreenHeight(), Texture::PF_RGBA, 0);

    // Set the full screen rendertarget.
#ifdef _USE_ZBUFFER_TILING
	RenderContext rtContext(m_BackBuffer, m_DepthStencil);
#else
	RenderContext rtContext(m_BackBuffer);
#endif
	setRenderContext(rtContext);

    //Alloc shader Pool
    m_shaderMemoryPoolBuffer = (u8*)slotAllocatorManager::allocatePhysicalMemory( c_shaderMemoryPoolSize, D3DSHADER_ALIGNMENT,  PAGE_READWRITE|PAGE_WRITECOMBINE, MemoryId::mId_GfxAdapter );
    m_shaderMemoryPoolPtr = m_shaderMemoryPoolBuffer;
    
#endif //ITF_X360
    return btrue;
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_Directx9::isDeviceLost()
{
    return bfalse;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::preD3DReset()
{
#ifdef _USE_ZBUFFER_TILING
    DXReleaseTexture(&m_pFrontBufferTexture[0]);
    DXReleaseTexture(&m_pFrontBufferTexture[1]);
#endif

    mp_shaderManager.destroyAll();
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::postD3DReset()
{
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::displayDebugInfoX360(DrawCallContext &_drawCallCtx)
{
#if defined(ITF_X360) && defined(ITF_SUPPORT_DBGPRIM_TEXT)

    /// Debug info.
    /// Show fps.
    GFX_ADAPTER->beginGPUEvent("Debug");

    m_FontDebug.SetScaleFactors( 1.f, 1.f );

    RECT rct;
    rct.left=80;
    rct.right=780;
    rct.top=40;
    rct.bottom=rct.top+20;

    wchar_t utf16ConvertBuffer[512];

    if (getfPs()>0.0f && getDisplayDebugStats())
    {
        wchar_t cfps[128];
        const u32 MB = (1024*1024);
        
        MEMORYSTATUS stat;
        // Get the memory status.
        GlobalMemoryStatus( &stat );
        f32 fps = getfPs();
        swprintf_s( cfps, 128, L"%6.2f fps Mem free:%d Mb %6.2f ms", fps,stat.dwAvailPhys / MB ,fps>=0.01f ? 1000.0f/fps : 0.0f);
        m_FontDebug.DrawText( (f32) (m_windowWidth- rct.left-500), (f32)rct.top+20, COLOR_WHITE, cfps); 

#ifndef ITF_DISABLE_DEBUGINFO
        f32 gpuMs = m_GPURasters[m_fenceFrameParity][GFX_GPUMARKER_Scene] + 0.6f;
        const String8& titleIp  = ((SystemAdapter_X360*)SystemAdapter::getptr())->getTitleIP();
        String8 cCpu;
        cCpu.setTextFormat("CPU %6.2f ms", m_CPURaster);
        String8 cGpuIp;
        cGpuIp.setTextFormat("GPU %6.2f ms   IP:%s", gpuMs,titleIp.cStr());
        u32 cpuPerfColor = (m_CPURaster<16.5f)?((m_CPURaster<15.5f)?COLOR_WHITE:ITFCOLOR_TO_U32(255, 255, 255, 0)):COLOR_RED;
        u32 gpuPerfColor = (gpuMs<16.5f)?((gpuMs<15.5f)?COLOR_WHITE:ITFCOLOR_TO_U32(255, 255, 255, 0)):COLOR_RED;

        buildUTF16FromUTF8(cCpu.cStr(), (u16*)utf16ConvertBuffer, ITF_ARRAY_SIZE(utf16ConvertBuffer));
        m_FontDebug.DrawText( (f32) (m_windowWidth- rct.left-500), (f32)rct.top+40, cpuPerfColor, utf16ConvertBuffer); 
        buildUTF16FromUTF8(cGpuIp.cStr(), (u16*)utf16ConvertBuffer, ITF_ARRAY_SIZE(utf16ConvertBuffer));
        m_FontDebug.DrawText( (f32) (m_windowWidth- rct.left-350), (f32)rct.top+40, gpuPerfColor, utf16ConvertBuffer); 
        buildUTF16FromUTF8(m_engineDataVersion.cStr(), (u16*)utf16ConvertBuffer, ITF_ARRAY_SIZE(utf16ConvertBuffer));
        m_FontDebug.DrawText( (f32) (m_windowWidth- rct.left-500), (f32)rct.top+60, COLOR_WHITE, utf16ConvertBuffer); 
        buildUTF16FromUTF8(m_extraInfo.cStr(), (u16*)utf16ConvertBuffer, ITF_ARRAY_SIZE(utf16ConvertBuffer));
        m_FontDebug.DrawText( (f32) (m_windowWidth- rct.left-500), (f32)rct.top+80, COLOR_WHITE, utf16ConvertBuffer);

#endif // ITF_DISABLE_DEBUGINFO
#ifdef ITF_CATEGORY_MEMORY_ENABLE
        u32 textColor;
        // Raster for texture memory
        const u32 cReserveTextureMemoryMaxSizeMb = 160;
        u32 textureMemorySize = u32( memoryStatsManager::get().getCategoryCurrentAllocated(MemoryId::mId_Textures)/MB );
        textColor = (textureMemorySize<cReserveTextureMemoryMaxSizeMb)?COLOR_WHITE:COLOR_RED;
        String8 textureMemory;
        textureMemory.setTextFormat("Texture used: %d Mb / Max Peak : %d Mb",
            textureMemorySize,
            memoryStatsManager::get().getCategoryCurrentAllocatedPeak(MemoryId::mId_Textures)/MB);
        buildUTF16FromUTF8(textureMemory.cStr(), (u16*)utf16ConvertBuffer, ITF_ARRAY_SIZE(utf16ConvertBuffer));
        m_FontDebug.DrawText( (f32) (m_windowWidth- rct.left-500), (f32)rct.top+100, textColor, utf16ConvertBuffer); 

        //raster for sound memory
        const u32 cReserveSoundMemoryMaxSizeMb = 40;
        u32 soundMemorySize = u32( memoryStatsManager::get().getCategoryCurrentAllocated(MemoryId::mId_AudioEngine)/MB );
        textColor = (soundMemorySize<cReserveSoundMemoryMaxSizeMb)?COLOR_WHITE:COLOR_RED;
        String8 soundMemory;
        soundMemory.setTextFormat("Sound used: %d Mb / Max Peak : %d Mb",
            soundMemorySize,
            memoryStatsManager::get().getCategoryCurrentAllocatedPeak(MemoryId::mId_AudioEngine)/MB);
        buildUTF16FromUTF8(soundMemory.cStr(), (u16*)utf16ConvertBuffer, ITF_ARRAY_SIZE(utf16ConvertBuffer));
        m_FontDebug.DrawText( (f32) (m_windowWidth- rct.left-500), (f32)rct.top+120, textColor, utf16ConvertBuffer); 

#endif //ITF_CATEGORY_MEMORY_ENABLE
                            
#if !defined(ITF_FINAL)
        if(!m_dbgTxtAfterFX.isEmpty())
        {
            buildUTF16FromUTF8(m_dbgTxtAfterFX.cStr(), (u16*)utf16ConvertBuffer, ITF_ARRAY_SIZE(utf16ConvertBuffer));
            m_FontDebug.DrawText( (f32) (m_windowWidth- rct.left-500), (f32)rct.top+140, COLOR_WHITE, utf16ConvertBuffer);
            m_dbgTxtAfterFX.clear();
        }
#endif // ITF_FINAL

    }

    rct.top=getScreenHeight()-30;
    rct.bottom=rct.top+20;

    ITF_VECTOR<DBGText>::iterator it = m_DBGTexts.begin();
    ITF_VECTOR<DBGText>::iterator end = m_DBGTexts.end();
    for (; it != end; ++it)
    {
        u32 color = D3DCOLOR_ARGB(255, (u32)(it->m_r*255.f), (u32)(it->m_g*255.f), (u32)(it->m_b*255.f));

        if ((*it).m_x < 0.0f && (*it).m_y < 0.0f)
        {
            rct.top -= 20;
            rct.bottom = rct.top+20;
            buildUTF16FromUTF8((*it).m_text.cStr(), (u16*)utf16ConvertBuffer, ITF_ARRAY_SIZE(utf16ConvertBuffer));
            m_FontDebug.DrawText( (f32)rct.left, (f32)rct.top, color, utf16ConvertBuffer);
        }
        else
        {
            RECT localRect;
            localRect.left=(i32)((*it).m_x);
            localRect.right=getScreenWidth();
            localRect.top=(i32)((*it).m_y);
            localRect.bottom=rct.top+20;
            buildUTF16FromUTF8((*it).m_text.cStr(), (u16*)utf16ConvertBuffer, ITF_ARRAY_SIZE(utf16ConvertBuffer));
            m_FontDebug.DrawText( (f32)localRect.left, (f32)localRect.top, color, utf16ConvertBuffer);
        }
    }
    m_DBGTexts.clear(); 

    GFX_ADAPTER->endGPUEvent();
#endif
}

#ifdef ITF_X360
void GFXAdapter_Directx9::beginTiling(Color &_clearColor)
{

    //Prepare predicatedTiling
    D3DVECTOR4 vClearColor = { _clearColor.getRed(), _clearColor.getGreen(), _clearColor.getBlue(), _clearColor.getAlpha() };
    if(m_TilingParam.usePredicatedTiling)
    {
        m_isPredicatedTilingPass = btrue;

        setBackBuffer();
        m_pd3dDevice->BeginTiling(
            m_TilingParam.TilingFlags,
            m_TilingParam.tileCount,
            m_TilingParam.surfaceRect,
            &vClearColor, internalDepth(1.0f), 0L );

        //Prepare VPOS offset for predicated tiling
        m_pd3dDevice->GpuOwnPixelShaderConstantF( m_TilingParam.constantOffset, 4 );
        for( DWORD i = 0; i <  m_TilingParam.tileCount; ++i )
        {
            m_pd3dDevice->SetPredication( D3DPRED_TILE( i ) );
            // Request an allocation of pixel shader constants from the command buffer.
            D3DVECTOR4* pConstantData = NULL;
            HRESULT hr = m_pd3dDevice->GpuBeginPixelShaderConstantF4( m_TilingParam.constantOffset,
                &pConstantData, 4 );
            if( SUCCEEDED( hr ) )
            {
                ZeroMemory( pConstantData, 4 * sizeof( D3DVECTOR4 ) );

                // Fill in the first constant with the tiling offset for this tile.
                pConstantData[0] = XMVectorSet( ( FLOAT )m_TilingParam.surfaceRect[i].x1, ( FLOAT )m_TilingParam.surfaceRect[i].y1, 0, 0 );

                m_pd3dDevice->GpuEndPixelShaderConstantF4();
            }
        }
        // Restore automatic predication.
        m_pd3dDevice->SetPredication( 0 );
    }
}

void GFXAdapter_Directx9::endTiling(LPDIRECT3DTEXTURE9 _ptex)
{
    if(m_TilingParam.usePredicatedTiling)
    {
        m_isPredicatedTilingPass = bfalse;

        LPDIRECT3DTEXTURE9 currResolveTexture = _ptex;
        if(!currResolveTexture)
        {
            currResolveTexture = getTargetDX(m_backBufferNoTile)->getTexture();
        }

        // Release ownership of the screenspace offset shader constants.
        m_pd3dDevice->GpuDisownAll();

        D3DVECTOR4 ClearColor = { 0, 0, 0, 0 };
        m_pd3dDevice->EndTiling( D3DRESOLVE_RENDERTARGET0 |
            D3DRESOLVE_ALLFRAGMENTS |
            D3DRESOLVE_CLEARRENDERTARGET |
            D3DRESOLVE_CLEARDEPTHSTENCIL,
            NULL, currResolveTexture,
            &ClearColor, 1.0f, 0L, NULL );
    }
}

// if true, D3D methods are executed for all the tile.
void GFXAdapter_Directx9::forceRenderToAllTile(bbool _force)
{
    if(_force)
    {
        m_pd3dDevice->SetPredication(D3DPRED_ALL_RENDER);
    }
    else
    {
        m_pd3dDevice->SetPredication(0);
    }
}

//--------------------------------------------------------------------------------------
// Draw a full texture into a render target
//--------------------------------------------------------------------------------------
void GFXAdapter_Directx9::copyTextureToEDRAM(RenderTarget* _pSrcTex, RenderTarget* surfDst, bbool _blurTarget, f32 _blurSize)
{
    //GFX_ZLIST_PASS_TYPE oldPassType = getCurPassType();
    m_curPassType = GFX_ZLIST_PASS_REGULAR;

    init2DRender();

    RenderPassContext rdrCtx;
    PrimitiveContext primCtx = PrimitiveContext(&rdrCtx);
    DrawCallContext drawCallCtx = DrawCallContext(&primCtx);
	GFX_RenderingBuffer_DX9* srcBuf = getTargetDX(_pSrcTex);

    if(_blurTarget)
    {
        setShaderGroup(mc_shader_AfterFx, mc_entry_afterFx_VS_blur, mc_entry_afterFx_PS_bigBlur);

        /// shader const.
        u32 W = srcBuf->getWidth();
        u32 H = srcBuf->getHeight();

        const f32 ratiopixelX = 1.f/(f32)W;
        const f32 ratiopixelY = 1.f/(f32)H;
        f32 displaceX = (_blurSize / 2.0f ) * ratiopixelX;
        f32 displaceY = (_blurSize / 2.0f ) * ratiopixelY;
        GFX_Vector4 vconst1(displaceX, displaceY, displaceX, displaceY);
        SetVertexShaderConstantF(VS_Attrib_afx, (f32*)&vconst1, 1);
    }
    else
    {
        setShaderGroup(m_defaultShaderGroup);
    }

    setGfxMatDefault(drawCallCtx);

    drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
        .setDepthTest(bfalse)
        .setDepthWrite(bfalse);

    setGlobalColor(Color::white());

	RenderContext rtContext(surfDst);
	setRenderContext(rtContext);

    bindTexture(0, _pSrcTex, _blurTarget);
    setTextureAdressingMode(0, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);
    drawScreenQuad(drawCallCtx, 0,0, (f32)getScreenWidth(), (f32)getScreenHeight(), 0, COLOR_WHITE, bfalse);
    end2DRender();
}

// downSample the EDRAM surface _scr to the quarter size texture _QrtBuffer
void GFXAdapter_Directx9::downSample4x4(RenderPassContext & _rdrCtxt, RenderTarget* _Src, RenderTarget* _QrtBuffer )
{
    // we assume that _pSrc surface is already set as renderTarget in EDRAM.

	GFX_RenderingBuffer_DX9* srcBuf = getTargetDX(_Src);
	GFX_RenderingBuffer_DX9* qrtBuf = getTargetDX(_QrtBuffer);

	//Alias renderTarget as 4xMSAA
    u32 widthMSAA = srcBuf->getWidth() / 2;
    u32 heightMSAA = srcBuf->getHeight() / 2;

    ITF_ASSERT_MSG( (widthMSAA == qrtBuf->getWidth()) && (heightMSAA == qrtBuf->getHeight()), "the intermediate RenderingBuffer size is incorrect");

    D3DMULTISAMPLE_TYPE eMSAAType = D3DMULTISAMPLE_4_SAMPLES;
    D3DSURFACE_PARAMETERS surfaceParams;
    ITF_MemSet( &surfaceParams, 0, sizeof( D3DSURFACE_PARAMETERS ) );

    surfaceParams.Base = srcBuf->getEdramOffset();
    surfaceParams.HierarchicalZBase = 0xFFFFFFFF;
    surfaceParams.HiZFunc = D3DHIZFUNC_DEFAULT;

    IDirect3DSurface9 oRTAA4x;
    XGSetSurfaceHeader(widthMSAA, heightMSAA,
        srcBuf->getFormat(),
        eMSAAType,
        &surfaceParams,
        &oRTAA4x,
        NULL);

    setInternalRenderTarget(&oRTAA4x, NULL);

    // Resolve to quarter buffer
    resolve(_QrtBuffer);
}


void GFXAdapter_Directx9::setGammaRamp(Vec3d gamma,f32 brigthnessValue,f32 contrastValue)
{
    D3DGAMMARAMP scaledRamp;
    buildGammaRamp( scaledRamp, gamma, brigthnessValue, contrastValue );

    m_pd3dDevice->SetGammaRamp(0, 0, &scaledRamp);

}


void GFXAdapter_Directx9::buildGammaRamp( D3DGAMMARAMP& scaledRamp, Vec3d gamma, f32 brigthnessValue, f32 contrastValue )
{
    gamma.x() = 1.0f / gamma.x();
    gamma.y() = 1.0f / gamma.y();
    gamma.z() = 1.0f / gamma.z();

    brigthnessValue = Clamp(brigthnessValue-1.0f,-1.0f,1.0f);
    contrastValue = Clamp(contrastValue-1.0f,-1.0f,1.0f);

    brigthnessValue = brigthnessValue * ( 1.0f / 6.0f );
    contrastValue = 0.5f * contrastValue + 1.0f;

    for(u32 rampIndex = 0; rampIndex < 256; ++rampIndex)
    {
        f32 redFct   = Clamp(powf(static_cast<f32>(rampIndex) / 255.f, gamma.getX()) + brigthnessValue,0.0f,1.0f);
        f32 greenFct = Clamp(powf(static_cast<f32>(rampIndex) / 255.f, gamma.getY()) + brigthnessValue,0.0f,1.0f);
        f32 blueFct  = Clamp(powf(static_cast<f32>(rampIndex) / 255.f, gamma.getZ()) + brigthnessValue,0.0f,1.0f);

        redFct   = Clamp(redFct  *contrastValue,0.0f,1.0f);
        greenFct = Clamp(greenFct*contrastValue,0.0f,1.0f);
        blueFct  = Clamp(redFct  *contrastValue,0.0f,1.0f);

        scaledRamp.red  [rampIndex] = static_cast<u16>( redFct   * 65535.f );
        scaledRamp.green[rampIndex] = static_cast<u16>( greenFct * 65535.f );
        scaledRamp.blue [rampIndex] = static_cast<u16>( blueFct  * 65535.f );
    }
}



//--------------------------------------------------------------------------------
// D3D function Redefinition 
//--------------------------------------------------------------------------------



HRESULT GFXAdapter_Directx9::DXCreateTexture(u32 _Width, u32 _Height, u32 _level, u32 _usage, D3DFORMAT _d3dFormat, D3DPOOL _Pool, IDirect3DTexture9 **_ppTexture)
{
    IDirect3DTexture9* pTexture = new IDirect3DTexture9;
    u32 baseSize, mipSize;
    XGSetTextureHeader
        (
        _Width, _Height, _level, 
        _usage, _d3dFormat, _Pool, 
        0, XGHEADER_CONTIGUOUS_MIP_OFFSET, 
        0, pTexture, 
        &baseSize, &mipSize
        );
    u32 memSize = baseSize + mipSize;

    u8 *mp_physicalAlloc = (u8*)slotAllocatorManager::allocatePhysicalMemory( memSize, GPU_TEXTURE_ALIGNMENT,  PAGE_READWRITE|PAGE_WRITECOMBINE, MemoryId::mId_TextureEngine );
    if ( mp_physicalAlloc == 0 )
        return S_FALSE;

    void * baseAddress = mp_physicalAlloc;
    void * mipAddress = mp_physicalAlloc;   // because XGHEADER_CONTIGUOUS_MIP_OFFSET is set and baseOffset == 0
    XGOffsetBaseTextureAddress(pTexture, baseAddress, mipAddress);
    *_ppTexture = pTexture;
    return S_OK;
}

void GFXAdapter_Directx9::DXReleaseTexture(IDirect3DTexture9 **_pTexture)
{
    IDirect3DBaseTexture9* basetexture = static_cast< IDirect3DBaseTexture9* >( *_pTexture );
#ifdef ITF_DEBUG
    ITF_ASSERT(!(basetexture->IsSet(m_pd3dDevice) || basetexture->IsBusy()));
#endif
    void* textureData = (void*)(basetexture->Format.BaseAddress << 12);
    slotAllocatorManager::deallocatePhysicalMemory( textureData );
    delete basetexture;
}

HRESULT GFXAdapter_Directx9::DXCreateLinkTexture(IDirect3DTexture9 *_pSrcBaseTexture, IDirect3DTexture9 *_pLinkedTexture, u32 _Width, u32 _Height, u32 _level, u32 _usage, D3DFORMAT _d3dFormat, D3DPOOL _Pool, IDirect3DTexture9 **_ppTexture)
{
    IDirect3DTexture9* pTexture = new IDirect3DTexture9;
    HRESULT hr = S_OK;
    u32 baseSize, mipSize;
    XGSetTextureHeader
        (
        _Width, _Height, _level, 
        _usage, _d3dFormat, _Pool, 
        0, XGHEADER_CONTIGUOUS_MIP_OFFSET, 
        0, pTexture, 
        &baseSize, &mipSize
        );
    //u32 memSize = baseSize + mipSize;

    //get info
    u32 srcbaseData, srcbaseSize;
    XGGetTextureLayout(_pSrcBaseTexture, &srcbaseData, &srcbaseSize, NULL, 0, 4096, NULL, NULL, NULL, NULL, 4096);

    u32 memoryOffset = 0;
    u32 linkedBaseData, linkedBaseSize;
    if(_pLinkedTexture != NULL)
    {
        XGGetTextureLayout(_pLinkedTexture, &linkedBaseData, &linkedBaseSize, NULL, 0, 4096, NULL, NULL, NULL, NULL, 4096);
        memoryOffset = linkedBaseData - srcbaseData + linkedBaseSize;
        ITF_ASSERT(memoryOffset < srcbaseSize);
    }

    IDirect3DBaseTexture9* basetexture = static_cast< IDirect3DBaseTexture9* >( _pSrcBaseTexture );
    void* textureData = (void*)((basetexture->Format.BaseAddress << 12) + memoryOffset);
    void * baseAddress = textureData;
    void * mipAddress = textureData;   // because XGHEADER_CONTIGUOUS_MIP_OFFSET is set and baseOffset == 0
    XGOffsetBaseTextureAddress(pTexture, baseAddress, mipAddress);
    *_ppTexture = pTexture;
    return hr;
}

void GFXAdapter_Directx9::DXReleaseLinkTexture(IDirect3DTexture9 **_pTexture)
{
    IDirect3DBaseTexture9* basetexture = static_cast< IDirect3DBaseTexture9* >( *_pTexture );
    delete basetexture;
}

HRESULT GFXAdapter_Directx9::DXGetSurfaceLevel(IDirect3DTexture9* _pTexture, u32 _level, IDirect3DSurface9 **_ppSurfaceLevel)
{
    HRESULT hr = _pTexture->GetSurfaceLevel(_level, _ppSurfaceLevel);
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreateRenderTarget(u32 _Width, u32 _Height, D3DFORMAT _d3dFormat, D3DMULTISAMPLE_TYPE _MultiSample, u32 _MultisampleQuality, bbool _Lockable, IDirect3DSurface9 **_ppSurface, CONST D3DSURFACE_PARAMETERS *pParameters)
{
    HRESULT hr = m_pd3dDevice->CreateRenderTarget(_Width, _Height, _d3dFormat, _MultiSample, _MultisampleQuality, _Lockable, _ppSurface, pParameters);
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreateDepthStencilSurface(u32 _Width, u32 _Height, D3DFORMAT _d3dFormat, D3DMULTISAMPLE_TYPE _MultiSample, u32 _MultisampleQuality, bbool _Discard, IDirect3DSurface9 **_ppSurface, CONST D3DSURFACE_PARAMETERS *pParameters)
{
    HRESULT hr = m_pd3dDevice->CreateDepthStencilSurface(_Width, _Height, _d3dFormat, _MultiSample, _MultisampleQuality, _Discard, _ppSurface, pParameters);
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreateVertexDeclaration(const D3DVERTEXELEMENT9 *_pVertexElements, IDirect3DVertexDeclaration9 **_ppDecl)
{
    HRESULT hr = m_pd3dDevice->CreateVertexDeclaration(_pVertexElements, _ppDecl);
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreateVertexShader(const DWORD *_pFunction, IDirect3DVertexShader9 **_ppShader)
{
    HRESULT hr = S_OK;
    XGMICROCODESHADERPARTS parts;
    XGGetMicrocodeShaderParts( _pFunction, &parts );

    *_ppShader = (IDirect3DVertexShader9*) new char[parts.cbCachedPartSize];
    XGSetVertexShaderHeader( *_ppShader, parts.cbCachedPartSize, &parts );

    u32 sizeAlign = (parts.cbPhysicalPartSize + (D3DSHADER_ALIGNMENT-1)) & ~(D3DSHADER_ALIGNMENT-1);
    u8 *p_physicalAlloc = m_shaderMemoryPoolPtr;
    if( p_physicalAlloc - m_shaderMemoryPoolBuffer + sizeAlign < c_shaderMemoryPoolSize )
    {
        memcpy( p_physicalAlloc, parts.pPhysicalPart, parts.cbPhysicalPartSize );
        XGRegisterVertexShader( *_ppShader, p_physicalAlloc );
        m_shaderMemoryPoolPtr += sizeAlign;
    }
    else
    {
        hr = S_FALSE;
    }
    ITF_ASSERT_CRASH(p_physicalAlloc - m_shaderMemoryPoolBuffer + sizeAlign < c_shaderMemoryPoolSize, "Not enough memory in the shader Pool");
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreatePixelShader(const DWORD *_pFunction, IDirect3DPixelShader9 **_ppShader)
{
    HRESULT hr = S_OK;
    XGMICROCODESHADERPARTS parts;
    XGGetMicrocodeShaderParts( _pFunction, &parts );

    *_ppShader = (IDirect3DPixelShader9*) new char[parts.cbCachedPartSize];
    XGSetPixelShaderHeader( *_ppShader, parts.cbCachedPartSize, &parts );

    u32 sizeAlign = (parts.cbPhysicalPartSize + (D3DSHADER_ALIGNMENT-1)) & ~(D3DSHADER_ALIGNMENT-1);
    u8 *p_physicalAlloc = m_shaderMemoryPoolPtr;
    if( p_physicalAlloc - m_shaderMemoryPoolBuffer + sizeAlign < c_shaderMemoryPoolSize )
    {
        memcpy( p_physicalAlloc, parts.pPhysicalPart, parts.cbPhysicalPartSize );
        XGRegisterPixelShader( *_ppShader, p_physicalAlloc );
        m_shaderMemoryPoolPtr += sizeAlign;
    }
    else
    {
        hr = S_FALSE;
    }
    ITF_ASSERT_CRASH(p_physicalAlloc - m_shaderMemoryPoolBuffer + sizeAlign < c_shaderMemoryPoolSize, "Not enough memory in the shader Pool");
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreateVertexBuffer(u32 _Lenght, u32 _usage, u32 _FVF, D3DPOOL _pool, IDirect3DVertexBuffer9 **_ppVertexBuffer, HANDLE *_pSharedHandle)
{
    HRESULT hr = m_pd3dDevice->CreateVertexBuffer(_Lenght, _usage, _FVF, _pool, _ppVertexBuffer, _pSharedHandle);
    return hr;
}

HRESULT GFXAdapter_Directx9::DXCreateIndexBuffer(u32 _Lenght, u32 _usage, D3DFORMAT Format, D3DPOOL _pool, IDirect3DIndexBuffer9 **_ppIndexBuffer, HANDLE *_pSharedHandle)
{
    HRESULT hr = m_pd3dDevice->CreateIndexBuffer(_Lenght, _usage, Format, _pool, _ppIndexBuffer, _pSharedHandle);
    return hr;
}

#if defined(ITF_X360) && defined(GFX_COMMON_POSTEFFECT)
	void GFXAdapter_Directx9::getBackBufferAfterOffset(u32 &_afterBackEDRAMOffset, u32 &_afterLightEDRAMOffset)
	{
		//EDRAM offset just after tiled backbuffer and tiled Zbuffer
		_afterBackEDRAMOffset = getTargetDX(m_BackBuffer)->getSurface()->Size + getCurrentDepthStencilSurf()->Size;
		//EDRAM offset just after tiled backbuffer and tiled Zbuffer and the quarter buffer
		_afterLightEDRAMOffset = _afterBackEDRAMOffset + (XGSurfaceSize(getScreenWidth()/LIGHTS_BFR_RATIO, getScreenHeight()/LIGHTS_BFR_RATIO, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE) * GPU_EDRAM_TILE_SIZE);
	}
    void GFXAdapter_Directx9::setShaderGPRAllocation(u32 VertexShaderCount, u32 PixelShaderCount)
	{
		m_pd3dDevice->SetShaderGPRAllocation(0, VertexShaderCount, PixelShaderCount);
	}

	u32 GFXAdapter_Directx9::getBufferSizeOffset(RenderTarget *_target)
	{
		return getTargetDX(_target)->getSurface()->Size;
	}

	// Draw blur Scene in Additionnal RenderTarget 0
/*	void GFXAdapter_Directx9::PrepareBlurTarget(RenderPassContext & _rdrCtxt, u32 _pixelBlurSize, u32 _quality)
	{
		u32 currentDst = TEX_BFR_QRT_AFX_1;
		if(_quality > 0 && _pixelBlurSize > 1.0f)
		{
			u32 currentSrc = TEX_BFR_FULL_1;
			if(_pixelBlurSize > 2.0f)
			{
				u32 pass = 1;
				bbool lastPass = (pass >= _quality);
				copyTextureToEDRAM(m_currentAFXBlurTexture, m_AdditionnalRenderingBuffer[currentSrc]);
				blur4x4(_rdrCtxt, m_AdditionnalRenderingBuffer[currentSrc], m_AdditionnalRenderingBuffer[currentDst], m_AdditionnalRenderingBuffer[TEX_BFR_QRT_1], (f32)_pixelBlurSize, lastPass );
				while (lastPass == bfalse)
				{
					_pixelBlurSize /= 2;
					lastPass = (pass >= _quality) || (_pixelBlurSize <= 2);
					currentSrc = (currentSrc == TEX_BFR_QRT_AFX_1)?TEX_BFR_QRT_AFX_2:TEX_BFR_QRT_AFX_1;
					currentDst = (currentDst == TEX_BFR_QRT_AFX_1)?TEX_BFR_QRT_AFX_2:TEX_BFR_QRT_AFX_1;
					blur4x4(_rdrCtxt, m_AdditionnalRenderingBuffer[currentSrc], m_AdditionnalRenderingBuffer[currentDst], m_AdditionnalRenderingBuffer[TEX_BFR_QRT_FRONT_LIGHT], (f32)_pixelBlurSize, lastPass );

					pass++;
				}
			}
			else
			{
				copyTextureToEDRAM(m_currentAFXBlurTexture, m_AdditionnalRenderingBuffer[currentDst], true, _pixelBlurSize);
				resolve( m_AdditionnalRenderingBuffer[currentDst]);
			}
		}
		m_currentAFXBlurTexture = getBufferTexture(currentDst);
	}*/

	void GFXAdapter_Directx9::blur4x4(RenderPassContext & _rdrCtxt, RenderTarget* _Src, RenderTarget* _Dst, RenderTarget* _QrtIntermediateBuffer, f32 _pixelSize, bbool _resolveDst )
	{
		//------------------------------------------------------------------------
		// see XDK Postprocess Sample
		// Steps:
		//  a) Resolve, aliasing render target as 4xMSAA to get a free (but incorrect)
		//      2x2 downsample
		//  b) Downsample 2x2 using a copy shader (magically compensates for the
		//      incorrectness of previous step)
		//------------------------------------------------------------------------

		// we assume that _pSrc surface is already set as renderTarget in EDRAM.

		LPDIRECT3DSURFACE9 previousSurf = m_currentRenderingSurface;

		GFX_RenderingBuffer_DX9* srcBuf = getTargetDX(_Src);
		GFX_RenderingBuffer_DX9* qrtBuf = getTargetDX(_QrtIntermediateBuffer);

		//Alias renderTarget as 4xMSAA
		u32 widthMSAA = srcBuf->getWidth() / 2;
		u32 heightMSAA = srcBuf->getHeight() / 2;

		ITF_ASSERT_MSG( (widthMSAA == qrtBuf->getWidth()) && (heightMSAA == qrtBuf->getHeight()), "the intermediate RenderingBuffer size is incorrect");

		D3DMULTISAMPLE_TYPE eMSAAType = D3DMULTISAMPLE_4_SAMPLES;
		D3DSURFACE_PARAMETERS surfaceParams;
		ITF_MemSet( &surfaceParams, 0, sizeof( D3DSURFACE_PARAMETERS ) );

		surfaceParams.Base = srcBuf->getEdramOffset();
		surfaceParams.HierarchicalZBase = 0xFFFFFFFF;
		surfaceParams.HiZFunc = D3DHIZFUNC_DEFAULT;

		IDirect3DSurface9 oRTAA4x;
		XGSetSurfaceHeader(widthMSAA, heightMSAA,
			srcBuf->getFormat(),
			eMSAAType,
			&surfaceParams,
			&oRTAA4x,
			NULL);

		setInternalRenderTarget(&oRTAA4x, NULL);

		// Resolve to quarter buffer
		resolve(_QrtIntermediateBuffer);

		copyTextureToEDRAM(_QrtIntermediateBuffer, _Dst, btrue, _pixelSize);

		if(_resolveDst)
		{
			resolve( _Dst);
		}

		setInternalRenderTarget(previousSurf, NULL);
	}

#endif

#endif

} // namespace ITF
