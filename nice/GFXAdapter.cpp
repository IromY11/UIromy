#include "precompiled_engine.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_VIDEOCAPTURE_H_
#include "engine/display/videoCapture.h"
#endif //_ITF_VIDEOCAPTURE_H_

#ifndef _ITF_PROFILERMANAGER_H_
#include "engine/profiler/ProfilerManager.h"
#endif //_ITF_PROFILERMANAGER_H_

#ifndef ITF_MASKRESOLVEPRIMITIVE_H_
#include "engine/display/MaskResolvePrimitive.h"
#endif //ITF_MASKRESOLVEPRIMITIVE_H_

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

#ifndef _ITF_SEEDER_H_
#include "core/Seeder.h"
#endif //_ITF_SEEDER_H_

#ifndef _ITF_LIGHTMANAGER_H_
#include "engine/display/lighting/LightManager.h"
#endif // _ITF_LIGHTMANAGER_H_

#ifndef _ITF_RENDERPARAMMANAGER_H_
#include "engine/display/RenderParamManager.h"
#endif //_ITF_RENDERPARAMMANAGER_H_

#ifndef _ITF_GRIDFLUID_H_
#include "engine/display/GridFluid/GridFluid.h"
#endif // _ITF_GRIDFLUID_H_

#include "engine/AdaptersInterfaces/GFXAdapter_AsyncScreenshot.h"

#ifndef ITF_ENGINE_MESH_H_
# include "engine/display/Primitives/Mesh.h"
#endif //ITF_ENGINE_MESH_H_

#ifndef _ITF_FONTTEXT_H_
# include "engine/display/FontText.h"
#endif //_ITF_FONTTEXT_H_

#if defined(ITF_IOS) || defined(ITF_ANDROID)
    const float gAlphaClearColor = 0.0f; // IOS & ANDROID clear color has a alpha of 0 !
#else
    const float gAlphaClearColor = 1.0f; // but the clear alpha is 1 on other platforms
#endif

namespace ITF
{
    GfxAdapterCallbackProxy::voidPath GfxAdapterCallbackProxy::m_pluginLauncherOnSnapShot = NULL;
	GfxAdapterCallbackProxy::voidPath GfxAdapterCallbackProxy::m_pluginCrashDumpSenderOnSnapShot = NULL;
    GfxAdapterCallbackProxy::boolVoid GfxAdapterCallbackProxy::m_drcEmulatorPluginIsConnected = NULL;
    GfxAdapterCallbackProxy::engineMonitorPluginSendFile GfxAdapterCallbackProxy::m_engineMonitorPluginSendFile = NULL;
    GfxAdapterCallbackProxy::drcEmulatorPluginSendRenderData GfxAdapterCallbackProxy::m_drcEmulatorPluginSendRenderData = NULL;

    BEGIN_SERIALIZATION(VertexPCT)
        SERIALIZE_MEMBER("pos", m_pos);
        SERIALIZE_U32_COLOR("color", m_color);
        SERIALIZE_MEMBER("uv", m_uv);        
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(VertexPCBT)
        SERIALIZE_MEMBER("pos", m_pos);
        SERIALIZE_U32_COLOR("color", m_color);
        SERIALIZE_MEMBER("blendIndices", m_blendindices);
        SERIALIZE_MEMBER("uv", m_uv);        
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(VertexPNC3T)
        SERIALIZE_MEMBER("pos", m_pos);
        SERIALIZE_U32_COLOR("color", m_color);
        SERIALIZE_MEMBER("uv1", m_uv);
        SERIALIZE_OBJECT("uv2", m_uv2);
        SERIALIZE_OBJECT("uv3", m_uv3);
        SERIALIZE_MEMBER("uv4", m_uv4);           
    END_SERIALIZATION()

    // use this scope class to debug drawstate states
    class debugDrawStateScope
    {
    public:
        debugDrawStateScope(GFXAdapter * _gfxa, const ConstObjectRef & _objRef = ConstObjectRef())
            : m_gfxa(_gfxa)
#if defined(ITF_BASE_OBJECT_HAS_DEBUG_DRAW_STATES)
            , m_savedWireFrameMode(WIRE_FRAME_NONE)
#endif
        {
            m_gfxa->debugBeginDrawZListNode();
#if defined(ITF_BASE_OBJECT_HAS_DEBUG_DRAW_STATES)
            m_baseObj = _objRef.getObject();
            if (m_baseObj)
            {
                m_gfxa->setShowDebugColorObject(m_baseObj->getDisplayDebug());

                if(m_gfxa->isShowDebugColor() || m_gfxa->isShowDebugColorObject())
                {
                    m_gfxa->setDebugColor( m_baseObj->getDebugColor());
                    m_gfxa->setDebugBorder( m_baseObj->getDebugBorder());
                }

                m_savedWireFrameMode = m_gfxa->getWireFrameMode();
                WireFrameMode objWireFrameMode = m_baseObj->getDisplayWireFrameDebug();
                if (objWireFrameMode != WIRE_FRAME_NONE)
                {
                    m_gfxa->setWireFrameMode(objWireFrameMode);
                }
            }
#endif
        }

        ~debugDrawStateScope()
        {
            m_gfxa->debugEndDrawZListNode();
#if defined(ITF_BASE_OBJECT_HAS_DEBUG_DRAW_STATES)
            if (m_baseObj)
            {
                m_gfxa->setShowDebugColorObject(bfalse);
                m_gfxa->setWireFrameMode(m_savedWireFrameMode);
            }
#endif
        }

    private:
        GFXAdapter * m_gfxa;
#if defined(ITF_BASE_OBJECT_HAS_DEBUG_DRAW_STATES)
        const BaseObject * m_baseObj;
        WireFrameMode m_savedWireFrameMode;
#endif
    };

const UVSubdiv  GFXAdapter::patchGridBufferTypes[] = {
    { 1, 1 },
    { 2, 2 },
    { 4, 4 },
    { 8, 2 },
    { 2, 8 },
    { MESH_DEFAULT_HDIV, MESH_DEFAULT_VDIV },
    { MESH_DEFAULT_VDIV, MESH_DEFAULT_HDIV },
    { MESH_DEFAULT_VDIV, 1 },
    { 1, MESH_DEFAULT_VDIV },
    { MESH_DEFAULT_VDIV, 2 },
    { 2, MESH_DEFAULT_VDIV },
};
const u32       GFXAdapter::patchGridBufferTypesNb = sizeof(GFXAdapter::patchGridBufferTypes)/sizeof(UVSubdiv);

RenderContext::RenderContext(RenderTarget *_colorBuffer, RenderTarget *_depthBuffer)
{
	m_depthBuffer = _depthBuffer;
	memset(m_colorBuffers, 0, sizeof(m_colorBuffers));
	memset(&m_viewport, 0, sizeof(m_viewport));
	m_useViewport = false;

	if ( _colorBuffer )
	{
		m_colorBuffers[0] = _colorBuffer;
		m_colorBufferCount = 1;
	}
	else
		m_colorBufferCount = 0;
}
void RenderContext::setViewport(u32 _x, u32 _y, u32 _width, u32 _height, f32 _minZ, f32 _maxZ)
{
	m_viewport.m_x = _x;
	m_viewport.m_y = _y;
	m_viewport.m_width = _width;
	m_viewport.m_height = _height;
	m_viewport.m_minZ = _minZ;
	m_viewport.m_maxZ = _maxZ;

	m_useViewport = true;
}


void GFXAdapter::preInitDriver()
{
    createGenericTextures();
#ifdef GFX_COMMON_POSTEFFECT
	createAdditionalRenderTarget();
#endif

#ifdef ITF_SUPPORT_EDITOR
    createMaskBuffer(); // always
#endif
}

void GFXAdapter::createMaskBuffer()
{
    if( m_maskBuffer == NULL )
        m_maskBuffer = createTarget(this, getScreenWidth(), getScreenHeight(), Texture::PF_RGBA);
}

void GFXAdapter::destroyMaskBuffer()
{
    // destroyResources handles the editor case
#ifndef ITF_SUPPORT_EDITOR
    if ( m_maskBuffer )
    {
        m_maskBuffer->release();
        m_maskBuffer = NULL;
    }
#endif
}


void GFXAdapter::postInitDriver()
{
    createPatchGridInternalBuffers();
}

String8 GFXAdapter::getAllowedPatchGridBufferTypesStr()
{
    char str[patchGridBufferTypesNb*10];
    char *strPtr = str;
    for (u32 i=0; i<patchGridBufferTypesNb; i++)
    {
        if (i != 0 )
        {
            (*strPtr++) = ',';
        }
        sprintf(strPtr, "%ix%i", patchGridBufferTypes[i][0], patchGridBufferTypes[i][1]);
        strPtr += strlen(strPtr);
    }

    return String8(str);
}

// the algorithm can be changed per platform
void GFXAdapter::ZPrepassRenderPassAlgo::prepareDrawCallState()
{
    m_drawCallStateCache.reset();
    m_drawCallStateCache
        .setAlphaTest(btrue)
        .setAlphaRef(255)
        .setDepthTest(btrue)
        .setDepthWrite(btrue)
        .setDepthBias(0.0001f)
        .setDepthFunc(GFX_CMP_LESS)
        .setColorWrite(bfalse)
        .setAlphaWrite(bfalse);
}

void GFXAdapter::RegularRenderPassAlgo::begin(class RenderPassContext & _rdrCtxt, bbool _restoreContext)
{
	RenderPassAlgo::begin(_rdrCtxt, _restoreContext);
}

void GFXAdapter::RegularRenderPassAlgo::prepareDrawCallState()
{
    m_drawCallStateCache.reset();
    m_drawCallStateCache.setAlphaTest(bfalse)
        .setAlphaRef(1)
        .setDepthTest(btrue)
        .setDepthWrite(bfalse)
        .setDepthBias(0.0f)
        .setDepthFunc(GFX_CMP_LESSEQUAL)
        .setColorWrite(btrue)
        .setAlphaWrite(bfalse);
}

void GFXAdapter::RegularRenderPassAlgo::unpause(RenderPassContext & _rdrCtxt, bbool _restoreContext)
{
    RenderPassAlgo::unpause(_rdrCtxt, _restoreContext);
    GFXAdapter * gfx = GFX_ADAPTER;
    gfx->SetDrawCallState(_rdrCtxt.getStateCache(), 0xFFffFFff, btrue);
}

void GFXAdapter::RegularRenderPassAlgo::pause(RenderPassContext & _rdrCtxt, bbool _resolveTarget)
{
    RenderPassAlgo::pause(_rdrCtxt, _resolveTarget);
}

void GFXAdapter::ThreeDRenderPassAlgo::prepareDrawCallState()
{
    m_drawCallStateCache.reset();
    m_drawCallStateCache.setAlphaTest(bfalse)
        .setAlphaRef(1)
        .setDepthTest(btrue)
        .setDepthWrite(btrue)
        .setDepthBias(0.0f)
        .setDepthFunc(GFX_CMP_LESSEQUAL)
        .setColorWrite(btrue)
        .setAlphaWrite(bfalse);
}

void GFXAdapter::LightingRenderPassAlgo::prepareDrawCallState()
{
    m_drawCallStateCache.reset();
    m_drawCallStateCache.setDepthTest(bfalse)
        .setDepthWrite(bfalse);
}

void GFXAdapter::RefractionRenderPassAlgo::prepareDrawCallState()
{
    m_drawCallStateCache.reset();
    m_drawCallStateCache.setDepthTest(bfalse)
        .setDepthWrite(bfalse);
}

void GFXAdapter::ReflectionRenderPassAlgo::prepareDrawCallState()
{
    m_drawCallStateCache.reset();
    m_drawCallStateCache.setDepthTest(bfalse)
        .setDepthWrite(bfalse)
        .setStencilTest(btrue)
        .setStencilFunc(GFX_CMP_EQUAL)
        .setStencilZPass(GFX_STENCIL_KEEP)
        .setStencilZFail(GFX_STENCIL_KEEP)
        .setStencilFail(GFX_STENCIL_KEEP)
        .setStencilRef(STENCIL_MASK_REFLECTION)
        .setStencilPreMask(STENCIL_MASK_REFLECTION)
        .setStencilWriteMask(STENCIL_MASK_REFLECTION);
}

void GFXAdapter::ReflectionRenderPassAlgo::unpause(RenderPassContext & _rdrCtxt, bbool _restoreContext)
{
    RenderPassAlgo::unpause(_rdrCtxt, _restoreContext);
    GFXAdapter * gfx = GFX_ADAPTER;
    gfx->SetDrawCallState(_rdrCtxt.getStateCache(), 0xFFffFFff, btrue);
}

void GFXAdapter::ReflectionPrepassRenderPassAlgo::prepareDrawCallState()
{
    m_drawCallStateCache.reset();
    m_drawCallStateCache.setAlphaTest(btrue)
        .setAlphaRef(255)
        .setColorWrite(bfalse)
        .setAlphaWrite(bfalse)
        .setDepthTest(bfalse)
        .setDepthWrite(bfalse)
        .setStencilTest(btrue)
        .setStencilFunc(GFX_CMP_ALWAYS)
        .setStencilZPass(GFX_STENCIL_REPLACE)
        .setStencilZFail(GFX_STENCIL_KEEP)
        .setStencilFail(GFX_STENCIL_KEEP)
        .setStencilRef(STENCIL_MASK_REFLECTION)
        .setStencilPreMask(STENCIL_MASK_REFLECTION)
        .setStencilWriteMask(STENCIL_MASK_REFLECTION);
}

void GFXAdapter::FluidRenderPassAlgo::prepareDrawCallState()
{
    m_drawCallStateCache.reset();
    m_drawCallStateCache.setDepthTest(bfalse)
        .setDepthWrite(bfalse);
}

void GFXAdapter::FluidRenderPassAlgo::begin(RenderPassContext & _rdrCtxt, bbool _restoreContext)
{
    GFXAdapter * gfx = GFX_ADAPTER;
	bbool oldAlphaWrite = _rdrCtxt.getStateCache().getAlphaWrite();
    _rdrCtxt.getStateCache().setAlphaWrite(btrue);
    gfx->SetDrawCallState(_rdrCtxt.getStateCache(), 0xFFffFFff, btrue);
    RenderPassAlgo::begin(_rdrCtxt, _restoreContext);
    _rdrCtxt.getStateCache().setAlphaWrite(oldAlphaWrite);
    _rdrCtxt.getStateCache().setDepthTest(bfalse)
        .setDepthWrite(bfalse);
        //.setSeparateAlphaBlend(GFX_ALPHAMASK_ADDALPHA);
    gfx->SetDrawCallState(_rdrCtxt.getStateCache(), 0xFFffFFff, btrue);
    gfx->m_fluidSimuInfo.m_useGlow = bfalse;
}

void GFXAdapter::FluidRenderPassAlgo::end(RenderPassContext & _rdrCtxt, bbool _resolveTarget)
{
    RenderPassAlgo::end(_rdrCtxt, _resolveTarget);

#ifndef ITF_FINAL
    GFX_ADAPTER->startRasterForPass(_rdrCtxt.getPass());
#endif // ITF_FINAL

    GFXAdapter * gfx = GFX_ADAPTER;
    gfx->init2DRender();
    gfx->blurFluidRenderBuffer(_rdrCtxt);
    gfx->end2DRender();

#ifndef ITF_FINAL
    GFX_ADAPTER->endRasterForPass(_rdrCtxt.getPass());
#endif // ITF_FINAL
}

void GFXAdapter::MaskRenderPassAlgo::prepareDrawCallState()
{
    m_drawCallStateCache.reset();
    m_drawCallStateCache.setDepthTest(bfalse)
        .setDepthWrite(bfalse);
}

void GFXAdapter::MaskRenderPassAlgo::begin(RenderPassContext & _rdrCtxt, bbool _restoreContext)
{
    GFXAdapter * gfx = GFX_ADAPTER;

    _rdrCtxt.setPassFilterFlag( PassFilterFlag<GFX_ZLIST_PASS_REGULAR>() | PassFilterFlag<GFX_ZLIST_PASS_MASK>());

#ifndef ITF_FINAL
    gfx->startRasterForPass(_rdrCtxt.getPass());
#endif // ITF_FINAL

	RenderContext rtContext(gfx->m_maskBuffer);
	gfx->setRenderContext(rtContext);
	gfx->m_maskBufferFilled = true;

    gfx->m_curPassType = _rdrCtxt.getPass();

	gfx->clear(GFX_CLEAR_COLOR, 1.0f, 1.0f, 1.0f, 1.0f);

	_rdrCtxt.getStateCache() = m_drawCallStateCache;
    gfx->SetDrawCallState(_rdrCtxt.getStateCache(), 0xFFffFFff, btrue);
}

void GFXAdapter::MaskRenderPassAlgo::end(RenderPassContext & _rdrCtxt, bbool _resolveTarget)
{
    GFXAdapter * gfx = GFX_ADAPTER;

	gfx->resolve(gfx->m_maskBuffer);

#ifndef ITF_FINAL
    gfx->endRasterForPass(_rdrCtxt.getPass());
#endif // ITF_FINAL
}


void GFXAdapter::OverridenRenderPassAlgo::prepareDrawCallState()
{

}

void GFXAdapter::OverridenRenderPassAlgo::begin( RenderPassContext & _rdrCtxt, bbool _restoreContext)
{
    if (m_overridenAlgo)
        m_overridenAlgo->begin(_rdrCtxt, _restoreContext);
}

void GFXAdapter::OverridenRenderPassAlgo::end(RenderPassContext & _rdrCtxt, bbool _resolveTarget)
{
    if (m_overridenAlgo)
        m_overridenAlgo->end(_rdrCtxt, _resolveTarget);
}

void GFXAdapter::PickingRenderPassAlgo::prepareDrawCallState()
{
    Super::prepareDrawCallState();
#ifndef ITF_PICKING_SUPPORT
    ITF_ASSERT(0);
#else // ITF_PICKING_SUPPORT
    m_drawCallStateCache.setAlphaTest(btrue)
        .setAlphaRef(int(GFX_ADAPTER->m_alphaPickRef))
        .setAlphaWrite(btrue);
#endif
}

void GFXAdapter::PickingRenderPassAlgo::begin( RenderPassContext & _rdrCtxt, bbool _restoreContext)
{
#ifndef ITF_PICKING_SUPPORT
    ITF_ASSERT(0);
#else // ITF_PICKING_SUPPORT
    Super::begin(_rdrCtxt, _restoreContext);

    _rdrCtxt.getStateCache().setAlphaTest(btrue)
        .setAlphaRef(int(GFX_ADAPTER->m_alphaPickRef))
        .setAlphaWrite(btrue);
    _rdrCtxt.getStateCache().computeNewState(DCC_ALPHA);
    
    GFXAdapter * gfx = GFX_ADAPTER;
    gfx->setScissorRect(&gfx->m_pickingScissor);
#endif // ITF_PICKING_SUPPORT
}

void GFXAdapter::ScreenshotRenderPassAlgo::prepareDrawCallState()
{
    Super::prepareDrawCallState();
}

void GFXAdapter::OccluderMapRenderPassAlgo::prepareDrawCallState()
{
    Super::prepareDrawCallState();
    m_drawCallStateCache.setAlphaTest(btrue)
        .setAlphaRef(255)
        .setColorWrite(btrue)
        .setAlphaWrite(btrue)
        .setDepthTest(btrue)
        .setDepthWrite(btrue)
        .setDepthFunc(GFX_CMP_LESS);
}



//----------------------------------------------------------------------------//

GFXAdapter::GFXAdapter()
    : m_SceneAlreadyResolve(bfalse)
    , m_defaultClearColor(1.f, 0.f, 0.f, 0.f)
    , m_curPassType(GFX_ZLIST_PASS_REGULAR)
    , m_currentPrimitiveParam(NULL)
    , m_renderOccluderMap(0)
    , m_defaultPrimitiveParam(GFXPrimitiveParam::getDefaultNoFog())
	, m_whiteOpaqueTexture(NULL)
	, m_blackOpaqueTexture(NULL)
	, m_blackTranspTexture(NULL)
	, m_greyOpaqueTexture(NULL)
{
    m_clearColor = m_defaultClearColor;

    m_HideFlags = 0;
    m_screenWidth = 0;
    m_screenHeight = 0;
    m_windowWidth = 0;
    m_windowHeight = 0;
    m_fontTexture = 0;
    m_debugDrawMode = 0;
    m_LightingEnable = btrue;

	m_isUsingLighting = btrue;
	m_isUsingPostEffects = btrue;
	m_isUsingFluids = btrue;
#if defined(ITF_PROJECT_POP)
	m_isUsingZInject = bfalse;
#else
	m_isUsingZInject = btrue;
#endif

#ifndef ITF_FINAL
    m_showRasters = bfalse;
    m_showAfterFx = btrue;
    m_showDebugInfo = bfalse;
    m_showDebugColor = bfalse;
    m_showDebugRendering = bfalse;
    m_showDebugColorObject = bfalse;

    m_debugColor = 0;
    m_debugBorder = 0.0f;

    m_fenceFrameParity = 0;
#endif // ITF_FINAL

    m_RenderSceneInTarget = bfalse;
    
    resetStencilOverlapRef();

    m_internalIdxBufferTab = newAlloc(mId_GfxAdapter, ITF_IndexBuffer*[patchGridBufferTypesNb]);
    m_internalVtxBufferTab = newAlloc(mId_GfxAdapter, ITF_VertexBuffer*[patchGridBufferTypesNb]);
    ITF_MemSet(m_internalIdxBufferTab, 0, patchGridBufferTypesNb*sizeof(ITF_IndexBuffer*));
    ITF_MemSet(m_internalVtxBufferTab, 0, patchGridBufferTypesNb*sizeof(ITF_VertexBuffer*));

    m_internalSpritesInstancingVertexBuffer = NULL;
    m_internalSpritesInstancingIndexAsVertexBuffer = NULL;
    m_internalSpritesIndexBuffer = NULL;
    m_spriteIndexBufferSize = 0;

    m_debugZListPass = GFX_ZLIST_PASS_REGULAR;
    m_videoCapture         = NULL;

    m_useDepthBuffer = bfalse;
    m_disableRendering2d = bfalse;
	m_disableAfterFx = bfalse;

    m_fPs = 0.f;

    for (u32 i = 0; i < 10; i++)
        FBTexture[i] = NULL;

    m_drawFlag = 0;
    m_currentDrawZCam = 0.f;
    m_currentDrawZCamAbs = 0.f;
    m_currentDrawZWorld = 0.f;

#ifdef ITF_PICKING_SUPPORT
    m_bPickingMode = bfalse;
    m_bPickingDebug = bfalse;
#endif //ITF_PICKING_SUPPORT

    m_paused = bfalse;
    m_useFog = bfalse;
    m_useStaticFog = bfalse;
    m_useReflection = bfalse;
    m_overDrawMode = 0;
    m_showWireFrame = WIRE_FRAME_NONE;
    m_useSeparateAlpha = bfalse;
    

    m_colorFog = Color::zero();
    m_internRTColor = Color::zero();

#ifndef ITF_CONSOLE_FINAL
    m_drawSafeFrameRequested = bfalse;
#endif // ITF_CONSOLE_FINAL

    resetScreenShotRequest();

    m_monitorRefreshRate = 60.0f;
    m_displaydebugstats = btrue;
    m_frameHaveRefractionPass = bfalse;

	m_allowShaderCompil = bfalse;

    for(ux i = 0; i < GFX_ZLIST_PASS_COUNT; ++i)
    {
        m_renderPassAlgo[i] = &m_defaultRenderPassAlgo;
    }
    m_renderPassAlgo[GFX_ZLIST_PASS_3D] = &m_3DRenderPassAlgo;
    m_renderPassAlgo[GFX_ZLIST_PASS_ZPREPASS] = &m_zprepassRenderPassAlgo;
    m_renderPassAlgo[GFX_ZLIST_PASS_REGULAR] = &m_regularRenderPassAlgo;
    m_renderPassAlgo[GFX_ZLIST_PASS_BACK_LIGHT] = &m_lightRenderPassAlgo;
    m_renderPassAlgo[GFX_ZLIST_PASS_FRONT_LIGHT] = &m_lightRenderPassAlgo;
    m_renderPassAlgo[GFX_ZLIST_PASS_REFLECTION_PREPASS] = &m_reflectionPrepassRenderPassAlgo;
    m_renderPassAlgo[GFX_ZLIST_PASS_REFLECTION] = &m_reflectionRenderPassAlgo;
    m_renderPassAlgo[GFX_ZLIST_PASS_REFRACTION] = &m_RefractionRenderPassAlgo;
    m_renderPassAlgo[GFX_ZLIST_PASS_FLUID] = &m_FluidRenderPassAlgo;
    m_renderPassAlgo[GFX_ZLIST_PASS_MASK] = &m_MaskRenderPassAlgo;

    for(ux i = 0; i < GFX_ZLIST_PASS_COUNT; ++i)
    {
        m_renderPassAlgo[i]->prepareDrawCallState();
    }

    for(u32 i = 0; i<e_engineView_Count; i++)
    {
        m_hideMainDrawPass[i] = bfalse;
        m_hide2DDrawPass[i] = bfalse;
    }

	m_lightManager = newAlloc(mId_GfxAdapter, GFX_LightManager(this));
    m_renderParamManager = newAlloc(mId_GfxAdapter, RenderParamManager());
    m_gridFluidManager = newAlloc(mId_GfxAdapter, GFX_GridFluidManager());

#ifdef GFX_USE_ZPASS_BY_DEFAULT
    m_useZPrepassByDefault = btrue;
#else
    m_useZPrepassByDefault = bfalse;
#endif

#ifdef GFX_COMMON_POSTEFFECT
	ITF_MemSet( m_AdditionnalRenderingBuffer, 0, NB_TEX_BFR * sizeof( RenderTarget* ) );
	ITF_MemSet( m_AdditionnalExternalRenderingBuffer, 0, NB_TEX_EXT_BFR * 2 * sizeof( RenderTarget* ) );
	m_DebugRenderingBuffer = nullptr;
	m_MediumZBuffer = nullptr;
	m_currentAFXBlurTexture = nullptr;
	m_currentAFXSceneTexture = nullptr;

	#ifdef ITF_DURANGO
		m_addGlobalContrast = -0.04f;
		m_addGlobalBright = 0.04f;
		m_addGlobalSaturation = 0.0f;
	#else
		m_addGlobalContrast = 0.0f;
		m_addGlobalBright = 0.0f;
		m_addGlobalSaturation = 0.0f;
	#endif

    m_addSceneAndUIBright = 0.0f;
#endif

    m_maskBuffer = nullptr;

    m_gameUICameraTop = 0.f;
    m_gameUICameraLeft = 0.f;
    m_gameUICameraBottom = 0.f;
    m_gameUICameraRight = 0.f;
    m_requestRestoreGameUI2DCamera = bfalse;


    resetGlobalParameters();
}

void GFXAdapter::setOrthoView( f32 _l, f32 _r, f32 _b, f32 _t )
{
    m_worldViewProj.setWorld(Matrix44::identity());
    m_worldViewProj.setView(Matrix44::identity());

    m_worldViewProj.setProjOrtho(_l, _r, _b, _t);
}

void GFXAdapter::setTextureSize(Texture* _texture, const Size& _size )
{
     _texture->setSize((u32)_size.d_width, (u32)_size.d_height);
}

f32 GFXAdapter::getFrontLightBrightness()
{
	if ( m_currentPrimitiveParam->m_useGlobalLighting )
		return m_currentPrimitiveParam->m_frontLightBrigthness + m_globalBrightness;
	else
		return m_currentPrimitiveParam->m_frontLightBrigthness;
}

// Reset global color modifiers.
void GFXAdapter::resetGlobalParameters()
{
    m_globalColorBlend = Color(0.0f, 0.0f, 0.0f, 0.0f);
    m_globalStaticFog = Color(0.0f, 0.0f, 0.0f, 0.0f);
    m_globalFogOpacity = 0.0f;
    m_globalBrightness = 0.0f;

    m_lightManager->setFrontLightClearColor(m_lightManager->getDefaultFrontLightClearColor());
    m_lightManager->setBackLightClearColor(m_lightManager->getDefaultBackLightClearColor());
}

void GFXAdapter::GFX_computeMatrixLookAtRH( Matrix44* _view, Vec3d* _vEyePt, Vec3d* _vLookatPt, Vec3d* _vUpVec )
{
    // compute Look At Matrix RH.
    Vec3d zaxis = (*_vEyePt - *_vLookatPt);
    zaxis.normalize();
    Vec3d xaxis;
    _vUpVec->crossProduct(zaxis, xaxis);
    xaxis.normalize();
    Vec3d yaxis;
    zaxis.crossProduct(xaxis, yaxis);

    _view->setFromFloat(
        xaxis.x(), yaxis.x(), zaxis.x(), 0.f,
        xaxis.y(), yaxis.y(), zaxis.y(), 0.f,
        xaxis.z(), yaxis.z(), zaxis.z(), 0.f,
        -xaxis.dot(*_vEyePt), -yaxis.dot(*_vEyePt), -zaxis.dot(*_vEyePt), 1.f
        );
}

void GFXAdapter::GFX_computeMatrixPerspectiveFovRH( Matrix44* _proj, f32 _fov, f32 _aspect, f32 _zn, f32 _zf )
{
    // compute perspective projection Matrix RH.
    f32 Q = _zf / (_zn-_zf);
    f32 y = 1.f/tanf(_fov*0.5f);
    _proj->setFromFloat(
        y/_aspect,  0.f,    0.f,    0.f,
        0.f,        y,      0.f,    0.f,
        0.f,        0.f,    Q,      -1.f,
        0.f,        0.f,    Q*_zn,  0.f
        );
}

void GFXAdapter::GFX_computeMatrixOrthoOffCenterRH( Matrix44* _proj, f32 _l, f32 _r, f32 _b, f32 _t, f32 _zn, f32 _zf )
{
    // compute orthographic projection Matrix RH.
    f32 Qz = 1.f / (_zn-_zf);
    f32 Qw = 1.f / (_r - _l);
    f32 Qh = 1.f / (_t - _b);
    _proj->setFromFloat(
        2.f*Qw,         0.f,            0.f,    0.f,
        0.f,            2.f*Qh,         0.f,    0.f,
        0.f,            0.f,            Qz,     0.f,
        -(_r+_l)*Qw,    -(_t+_b)*Qh,    _zn*Qz, 1.f
        );
}

void GFXAdapter::setCamera(Camera* _cam)
{
	View*  curView = View::getCurrentView();
    const GFX_Viewport &viewport = curView->getViewport();

    GFX_RECT Viewport;
    Viewport.left = viewport.m_x;
    Viewport.top = viewport.m_y;
    Viewport.right = viewport.m_x + viewport.m_width;
    Viewport.bottom = viewport.m_y + viewport.m_height;	

	setupViewport(&Viewport);

	curView->updateInternalMatrix();

	m_worldViewProj.setWorld(Matrix44::identity());
    m_worldViewProj.setView(curView->getWorldToView());
    m_worldViewProj.setProj(curView->getViewToProj());
	m_worldViewProj.mulProj(curView->getPreViewportMatrix());

    //update parameters
    extractFrustumPlanes(m_worldViewProj.getWorldViewProj(), _cam);
}


void GFXAdapter::setThreadUsageScreenCapture(u32 _uThreadUsage,bbool _bStepByStep)  
{
#ifdef ITF_SUPPORT_VIDEOCAPTURE
    if (!m_videoCapture)
        m_videoCapture = new VideoCapture(_uThreadUsage,_bStepByStep);
#endif //ITF_SUPPORT_VIDEOCAPTURE
}

void GFXAdapter::initShaderConstant()
{
    ITF_shader* shadergroup;

    //RenderPCT fx
    mc_shader_Main = mp_shaderManager.getShaderKey("renderPCT.fx");
    if(mc_shader_Main != shaderGroupKey_invalid)
    {
        //Vertex
        shadergroup = mp_shaderManager.getShaderGroup(mc_shader_Main);    
        mc_entry_main_VS_Default = shadergroup->getEntryID("default_VS", bfalse);
        mc_entry_main_VS_PTAmb = shadergroup->getEntryID("default_PTambiant_VS", bfalse);
        mc_entry_main_VS_PNC3T = shadergroup->getEntryID("frize_PNC3T_VS", bfalse);
        mc_entry_main_VS_Patch = shadergroup->getEntryID("default_PCT_Patch_VS", bfalse);
        mc_entry_main_VS_BezierPatch = shadergroup->getEntryID("default_PCT_BezierPatch_VS", bfalse);
        mc_entry_main_VS_Fluid = shadergroup->getEntryID("fluid_PCT_VS", bfalse);
        mc_entry_main_VS_Fluid2 = shadergroup->getEntryID("fluid2_PCT_VS", bfalse);
        mc_entry_main_VS_Trail = shadergroup->getEntryID("trail_PCT_VS", bfalse);
        mc_entry_main_VS_Spline = shadergroup->getEntryID("spline_PCT_VS", bfalse);
        mc_entry_main_VS_Refraction = shadergroup->getEntryID("refraction_PCT_VS", bfalse);
        mc_entry_main_VS_Overlay = shadergroup->getEntryID("OVERLAY_PCBT_VS", bfalse);
        mc_entry_main_VS_QuadInstancing = shadergroup->getEntryID("QUAD_INSTANCING_VS", bfalse);

        //Pixel
        mc_entry_main_PS_Default = shadergroup->getEntryID("default_PS", btrue);
        mc_entry_main_PS_Debug = shadergroup->getEntryID("debug_PS", btrue);
        mc_entry_main_PS_Refraction = shadergroup->getEntryID("refraction_PCT_PS", btrue);
        mc_entry_main_PS_Fluid = shadergroup->getEntryID("fluid_PCT_PS", btrue);
        mc_entry_main_PS_OverDraw = shadergroup->getEntryID("overDraw_PS", btrue);
        mc_entry_main_PS_RenderInTexture = shadergroup->getEntryID("RenderInTexture_PS", btrue);

        //define
        mc_define_FogBox1 = shadergroup->getDefineID("FOGBOX1");
        mc_define_FogBox2 = shadergroup->getDefineID("FOGBOX2");
        mc_define_Texture = shadergroup->getDefineID("TEXTURE");
        mc_define_VertexNormal = shadergroup->getDefineID("VERTEX_NORMAL");
        mc_define_NormalMap = shadergroup->getDefineID("NORMAL_MAP");
        mc_define_UV2 = shadergroup->getDefineID("UV2");
        mc_define_Light3D = shadergroup->getDefineID("LIGHT3D");
        mc_define_Skin = shadergroup->getDefineID("SKINNING");
        mc_define_Light = shadergroup->getDefineID( "LIGHT");
        mc_define_StaticFog = shadergroup->getDefineID( "STATIC_FOG");
        mc_define_Debug = shadergroup->getDefineID( "DEBUG");
        mc_define_SeparateAlpha = shadergroup->getDefineID( "SEPARATE_ALPHA");
        mc_define_BlendTexture = shadergroup->getDefineID( "BLEND_TEXTURE");
        mc_define_TextureUv2 = shadergroup->getDefineID( "TEXTUREUV2");
        mc_define_ZInject = shadergroup->getDefineID( "ZINJECT");
        mc_define_UseBackLight = shadergroup->getDefineID( "USE_BACKLIGHT");
        mc_define_Reflection = shadergroup->getDefineID( "REFLECTION");
        mc_define_fluidGlow = shadergroup->getDefineID( "FLUIDGLOW");
        mc_define_fluidEmboss = shadergroup->getDefineID( "FLUIDEMBOSS");
		mc_define_fluidVPos = shadergroup->getDefineID( "FLUIDVPOS");
		mc_define_AlphaTest = shadergroup->getDefineID( "ALPHA_TEST");
		mc_define_Outline = shadergroup->getDefineID( "OUTLINE");
		mc_define_ColorMask = shadergroup->getDefineID( "COLOR_MASK");
        mc_define_Saturation = shadergroup->getDefineID( "SATURATION");

#ifdef ITF_GLES2
		shadergroup->disableDefine(mc_define_FogBox1);    // Maybe.
		shadergroup->disableDefine(mc_define_FogBox2);    // Maybe.
		if ( !isUsingLighting() )
			shadergroup->disableDefine(mc_define_Light);
		shadergroup->disableDefine(mc_define_UseBackLight); // OK
		shadergroup->disableDefine(mc_define_Light3D); // OK
		shadergroup->disableDefine(mc_define_Reflection); // ?
		shadergroup->disableDefine(mc_define_NormalMap);
		shadergroup->disableDefine(mc_define_VertexNormal);
#endif
		shadergroup->disableDefine(m_lightManager->getDisabledDefines());
    }

    //Movies fx
    mc_shader_Movie = mp_shaderManager.getShaderKey("movie.fx");
    if(mc_shader_Movie != shaderGroupKey_invalid)
    {
        shadergroup = mp_shaderManager.getShaderGroup(mc_shader_Movie);    
        mc_entry_movies_VS = shadergroup->getEntryID("VS_Movie", bfalse);
        mc_entry_movies_PS = shadergroup->getEntryID("PS_Movie", btrue);

        mc_define_movies_use_alpha_texture = shadergroup->getDefineID( "USE_ALPHA_TEXTURE");
    }

    //Font fx
    mc_shader_Font = mp_shaderManager.getShaderKey("font.fx");
    if(mc_shader_Font != shaderGroupKey_invalid)
    {
        shadergroup = mp_shaderManager.getShaderGroup(mc_shader_Font);    
        mc_entry_font_VS = shadergroup->getEntryID("VertScene", bfalse);
        mc_entry_font_PS = shadergroup->getEntryID("PixFont", btrue);
        mc_define_font_PS_outline = shadergroup->getDefineID("OUTLINE");
        mc_define_font_PS_gradient = shadergroup->getDefineID("GRADIENT");
        mc_define_font_ZInject = shadergroup->getDefineID("ZINJECT");
    }

    //AfterFx fx
    mc_shader_AfterFx = mp_shaderManager.getShaderKey("AfterFx.fx");
    if(mc_shader_AfterFx != shaderGroupKey_invalid)
    {
        shadergroup = mp_shaderManager.getShaderGroup(mc_shader_AfterFx);    
        mc_entry_afterFx_VS_blur = shadergroup->getEntryID("blur_VS", bfalse);
        mc_entry_afterFx_VS_PCT = shadergroup->getEntryID("PCT1_VS", bfalse);
        mc_entry_afterFx_PS_bigBlur = shadergroup->getEntryID("BigBlur_PS", btrue);
        mc_entry_afterFx_PS_addMul = shadergroup->getEntryID("AddMul_PS", btrue);
        mc_entry_afterFx_PS_colorOverDraw = shadergroup->getEntryID("ColorOverDraw_PS", btrue);
        mc_entry_afterFx_PS_copyOverDraw = shadergroup->getEntryID("CopyOverDraw_PS", btrue);
        mc_entry_afterFx_PS_mergedEffect = shadergroup->getEntryID("mergedEffect_PS", btrue);
        mc_entry_afterFx_PS_Kalei = shadergroup->getEntryID("KaleiPass_PS", btrue);
        mc_entry_afterFx_PS_DebugPass = shadergroup->getEntryID("DebugPass_PS", btrue);
        mc_define_afterFx_Glow = shadergroup->getDefineID("GLOW");
        mc_define_afterFx_Refraction = shadergroup->getDefineID("REFRACTION");
        mc_define_afterFx_Tonemap = shadergroup->getDefineID("TONEMAP");
        mc_define_afterFx_Blur = shadergroup->getDefineID("BLUR");
        mc_define_afterFx_Tile = shadergroup->getDefineID("TILE");
        mc_define_afterFx_Mosaic = shadergroup->getDefineID("MOSAIC");
        mc_define_afterFx_EyeFish = shadergroup->getDefineID("EYEFISH");
        mc_define_afterFx_Mirror = shadergroup->getDefineID("MIRROR");
        mc_define_afterFx_OldTV = shadergroup->getDefineID("OLD_TV");
        mc_define_afterFx_Negatif = shadergroup->getDefineID("NEGATIF");
        mc_define_afterFx_Noise = shadergroup->getDefineID("NOISE");
        mc_define_afterFx_Saturation = shadergroup->getDefineID("SATURATION");
        mc_define_afterFx_ColorCorrection = shadergroup->getDefineID("COLOR_CORRECTION");
        mc_define_afterFx_EdgeDetection = shadergroup->getDefineID("EDGEDETECTION");
        mc_define_afterFx_Gauss = shadergroup->getDefineID("GAUSS");
        mc_define_afterFx_Radial = shadergroup->getDefineID("RADIAL");
        mc_define_afterFx_BlendValue = shadergroup->getDefineID("BLEND_VALUE");

#ifdef ITF_GLES2
		shadergroup->disableDefine(mc_define_afterFx_Tonemap);
		shadergroup->disableDefine(mc_define_afterFx_Refraction);
		shadergroup->disableDefine(mc_define_afterFx_Tile);
		shadergroup->disableDefine(mc_define_afterFx_Mosaic);
		shadergroup->disableDefine(mc_define_afterFx_Negatif);
		shadergroup->disableDefine(mc_define_afterFx_EyeFish);
		shadergroup->disableDefine(mc_define_afterFx_Mirror);
		shadergroup->disableDefine(mc_define_afterFx_OldTV);
		shadergroup->disableDefine(mc_define_afterFx_EdgeDetection);
		shadergroup->disableDefine(mc_define_afterFx_Noise);
#endif
	}

    // Impostor fx
    mc_shader_Impostor = mp_shaderManager.getShaderKey("Impostor.fx");
    if(mc_shader_Impostor != shaderGroupKey_invalid)
    {
        shadergroup = mp_shaderManager.getShaderGroup(mc_shader_Impostor);    
        mc_entry_impostor_VS = shadergroup->getEntryID("Impostor_VS", bfalse);
        mc_entry_impostor_PS = shadergroup->getEntryID("Impostor_PS", btrue);
    }

    // Mask materials fx
    mc_shader_MaskMaterials = mp_shaderManager.getShaderKey("MaskMaterials.fx");
    if(mc_shader_MaskMaterials != shaderGroupKey_invalid)
    {
        shadergroup = mp_shaderManager.getShaderGroup(mc_shader_MaskMaterials);    
        mc_entry_mask_VS = shadergroup->getEntryID("mask_VS", bfalse);
		mc_entry_mask_frize_PNC3T_VS = shadergroup->getEntryID("mask_frize_PNC3T_VS", bfalse);
        mc_entry_maskedMat_PS = shadergroup->getEntryID("maskedMat_PS", btrue);
        mc_entry_maskMat_PS = shadergroup->getEntryID("maskMat_PS", btrue);
        mc_define_Mask_AlphaTest = shadergroup->getDefineID( "ALPHA_TEST");
        ITF_ASSERT( mc_define_ZInject == shadergroup->getDefineID("ZINJECT"));
    }
}

void GFXAdapter::destroy()
{
	SF_DEL(m_lightManager);
    SF_DEL(m_renderParamManager);
    SF_DEL(m_gridFluidManager);

#ifdef ITF_SUPPORT_VIDEOCAPTURE
    SF_DEL(m_videoCapture);
#endif //ITF_SUPPORT_VIDEOCAPTURE

    //SF_DEL(m_groupMeshManager);

    SF_DEL_ARRAY(m_internalIdxBufferTab);
    SF_DEL_ARRAY(m_internalVtxBufferTab);
}
   
void GFXAdapter::destroyResources()
{
    if ( m_maskBuffer )
    {
        m_maskBuffer->release();
        m_maskBuffer = NULL;
    }

    releaseGenericTextures();

    /// cleanup vertex buffer.    
    removePatchGridInternalBuffers();

	if( m_internalSpritesIndexBuffer )
	{
		removeIndexBuffer(m_internalSpritesIndexBuffer);
		m_internalSpritesIndexBuffer = NULL;
	}

	m_gridFluidManager->releaseAllTargets();
    mp_shaderManager.destroyAll();

	// We won't run another frame, so clean up now!
	for( int i=0; i<10; i++ )
	{
		// frame delay crap
		cleanBufferEndOfFrame();
	}

    M_RASTER_RELEASE();
}

GFXAdapter::~GFXAdapter()
{
    destroy();
}


void GFXAdapter::setResolution(u32 _windowWidth, u32 _windowHeight, bbool _forceReset /* = bfalse */)
{
    bool sizeChanged = (_windowWidth != m_windowWidth || m_windowHeight != _windowHeight);

    m_windowWidth = _windowWidth;
    m_windowHeight = _windowHeight;

    if (sizeChanged || _forceReset)
        resetDevice();

    Vec2d topLeft = Vec2d(0, 0);
    Vec2d botRight = Vec2d((f32)m_windowWidth, (f32)m_windowHeight);
    m_screenAABB = AABB(topLeft, botRight);

    m_screenWidth = m_windowWidth;
    m_screenHeight = m_windowHeight;

    m_requestRestoreGameUI2DCamera = btrue;
}

#ifdef ITF_SUPPORT_DBGPRIM_MISC
void GFXAdapter::fillTriangleList(const DBGLine& l)
{
    Vec3d p13d(l.p1.x(), l.p1.y(), l.z);
    Vec3d p23d(l.p2.x(), l.p2.y(), l.z);
    Vec3d p12d, p22d;

    compute3DTo2D(p13d, p12d);
    compute3DTo2D(p23d, p22d);

    bbool clip1 = (p12d.z() < 0.f) || (p12d.z() > 1.f);
    bbool clip2 = (p22d.z() < 0.f) || (p22d.z() > 1.f);
    if (clip1 && clip2)
        return;

    Vec3d perp = (p22d - p12d).getPerpendicular();
    perp.setLength(l.size * 0.5f);
    p12d += perp;
    p22d += perp;

    Vec3d p33dZ0, p43dZ0;
    compute2DTo3D(p12d, p33dZ0);
    compute2DTo3D(p22d, p43dZ0);

    Vec3d p3Final = p33dZ0;
    p3Final.z() = l.z;
    Vec3d p4Final = p43dZ0;
    p3Final.z() = l.z;

    /// fill vertex buffer.
    u32 color = Color(l.alpha, l.r, l.g, l.b).getAsU32();
    VertexPC v[6];
    Vec3d perp3d = p3Final - p13d;
    p13d -= perp3d;
    p23d -= perp3d;
    v[0].setData(p13d, color);
    v[1].setData(p4Final, color);
    v[2].setData(p3Final, color);
    v[3].setData(p13d, color);
    v[4].setData(p23d, color);
    v[5].setData(p4Final, color);

    for (i32 i=0;i<6;++i)
    {
        m_DBGTriangleList.push_back(v[i]);
    }
}

void GFXAdapter::fillTriangleList(const DBG3DLine& l)
{
    Vec3d p13d = l.p1;
    Vec3d p23d = l.p2;

    Vec3d p12d, p22d;

    compute3DTo2D(p13d, p12d);
    compute3DTo2D(p23d, p22d);

    bbool clip1 = (p12d.z() <= 0.f);
    bbool clip2 = (p22d.z() <= 0.f);
    if (clip1 || clip2)
        return;

    Vec3d perp = (p22d - p12d).getPerpendicular();
    perp.z() = 0.f;
    perp.setLength(l.size * 0.5f);
    p12d += perp;
    p22d += perp;

    Vec3d p33dZ0, p43dZ0;
    compute2DTo3D(p12d, p33dZ0);
    compute2DTo3D(p22d, p43dZ0);

    Vec3d p3Final = p33dZ0;
    Vec3d p4Final = p43dZ0;

    /// fill vertex buffer.
    u32 color = Color(l.alpha, l.r, l.g, l.b).getAsU32();
    VertexPC v[6];
    Vec3d perp3d = p3Final - p13d;
    p13d -= perp3d;
    p23d -= perp3d;
    v[0].setData(p13d, color);
    v[1].setData(p4Final, color);
    v[2].setData(p3Final, color);
    v[3].setData(p13d, color);
    v[4].setData(p23d, color);
    v[5].setData(p4Final, color);

    for (i32 i=0;i<6;++i)
    {
        m_DBGTriangleList.push_back(v[i]);
    }
}

void GFXAdapter::fillTriangleList( const DBGTriangle3D& _t )
{
    /// fill vertex buffer.
    u32 color = Color(1.f, _t.r, _t.g, _t.b).getAsU32();

    VertexPC v[3];
    v[0].setData(_t.p1, color);
    v[1].setData(_t.p2, color);
    v[2].setData(_t.p3, color);

    for (i32 i=0;i<3;++i)
        m_DBGTriangleList.push_back(v[i]);
}
#endif // ITF_SUPPORT_DBGPRIM_MISC

void GFXAdapter::draw2DPrimitives(View &_view)
{
    PRF_M_SCOPE(draw2D);

	if(isAsyncScreenshotRequested(AsyncScreenshotRenderStep_AfterPostFX)) 
		renderAsyncScreenshot(_view);
	
	u32 zListViewID = _view.getZlistID();
    if(zListViewID == View::ZlistID_invalid)
        return;

    beginGPUEvent("2D Primitives");

    initGameUI2DRender();
	startOverDrawRender();
    drawPrimitives2d(_view);
	endOverDrawRender();
    end2DRender();

    if(isAsyncScreenshotRequested(AsyncScreenshotRenderStep_After2D)) 
        renderAsyncScreenshot(_view);

    endGPUEvent();
}

#ifdef ITF_SUPPORT_DBGPRIM_ANY
void GFXAdapter::cleanAllDebugPrimitives()
{
#ifdef ITF_SUPPORT_DBGPRIM_TEXT
    m_DBGTexts.clear();
    m_DBGLeftStackedTexts.clear();
    m_DBGRightStackedTexts.clear();
#endif

#ifdef ITF_SUPPORT_DBGPRIM_BOX
    m_DBGBoxes.clear();
    m_DBGBox2DList.clear();
#endif // ITF_SUPPORT_DBGPRIM_BOX

#ifdef ITF_SUPPORT_DBGPRIM_MISC
    m_DBGSpheres.clear();
    m_DBGCircles.clear();
    m_DBGLines.clear();
    m_DBGTriangles.clear();
    m_DBGTextureTriangles.clear();
    m_DBGTextureQuads.clear();
    m_DBGPrim3D.clear();
    m_DBGTriangle2DList.clear();
    m_DBGCircle2DList.clear();
    m_DBGSpline.clear();
    m_DBGLinesDuration.clear();
    m_DBGTriangleList.clear();
    m_DBG3DLines.clear();
    
    for (u32 it = 0; it < m_DBGCircleContext.size();it++)
    {
        DBGContextCircle* pContextCircle = m_DBGCircleContext[it];
        SF_DEL(pContextCircle);
    }
    m_DBGCircleContext.clear();
#endif // ITF_DEBUG_LEVEL > 0
}

void GFXAdapter::drawDBGPrimitives()
{
#ifdef ITF_GLES2_RELEASE
    
#ifdef ITF_SUPPORT_DBGPRIM_MISC
    m_DBGCircleContext.clear();
    m_DBGLines.clear();
    m_DBGTriangleList.clear();
#endif // ITF_SUPPORT_DBGPRIM_MISC
    return;
    
#endif // ITF_GLES2_RELEASE
    if(getStrSceenshotResquest() || CONFIG->m_presentationMode)
    {
        GFX_ADAPTER->cleanAllDebugPrimitives();
        return;
    }
#if defined(ITF_SUPPORT_DBGPRIM_BOX) || defined(ITF_SUPPORT_DBGPRIM_MISC)
    RenderPassContext passCtx;
    passCtx.getStateCache().setDepthTest(bfalse)
        .setAlphaBlend(GFX_BLEND_ALPHA);

    SetDrawCallState(passCtx.getStateCache(), 0xFFffFFff, btrue);
    PrimitiveContext primitiveCxt = PrimitiveContext(&passCtx);
    primitiveCxt.setPrimitiveParam((GFXPrimitiveParam*)&m_defaultPrimitiveParam);
    DrawCallContext drawCallCtx = DrawCallContext(&primitiveCxt);

    setDefaultGFXPrimitiveParam();
#endif // ITF_SUPPORT_DBGPRIM_BOX || ITF_SUPPORT_DBGPRIM_MISC

#ifdef ITF_SUPPORT_DBGPRIM_BOX
    for (u32 it = 0; it < m_DBGBoxes.size();)
    {
        DBGBox& b = m_DBGBoxes[it];
        debugDrawStateScope _(this);
        drawBox(drawCallCtx, b.pos, b.angle, b.extent, b.r, b.g, b.b, b.z, b.fill );
        b.duration -= LOGICDT;
        if (b.duration <= 0.0f)
            m_DBGBoxes.eraseKeepOrder(it);
        else
            it++;
    }
#endif // ITF_SUPPORT_DBGPRIM_BOX

#ifdef ITF_SUPPORT_DBGPRIM_MISC
    {
        for (u32 it = 0; it < m_DBGSpheres.size();)
        {
            DBGSphere& c = m_DBGSpheres[it];
            debugDrawStateScope _(this);
            drawSphere(drawCallCtx, c.x, c.y, c.rad, c.r, c.g, c.b, c.z, c.segsCount, c.volume, c.xaxis, c.yaxis, c.zaxis);
            c.duration -= LOGICDT;
            if (c.duration <= 0.0f)
                m_DBGSpheres.eraseKeepOrder(it);
            else
                it++;
        }


        {
            debugDrawStateScope _(this);
            for (u32 it = 0; it < m_DBGCircles.size();)
            {
                DBGCircle& c = m_DBGCircles[it];
            
                drawCircle(drawCallCtx, c.x, c.y, c.rad, c.r, c.g, c.b, c.z, c.segsCount);
                c.duration -= LOGICDT;
                if (c.duration <= 0.0f)
                    m_DBGCircles.eraseKeepOrder(it);
                else
                    it++;
            }
        }


        //Circle context
        for (u32 it = 0; it < m_DBGCircleContext.size();it++)
        {
            DBGContextCircle* pContextCircle = m_DBGCircleContext[it];
            drawCircleContext(drawCallCtx, pContextCircle);
            SF_DEL(pContextCircle);
        }
        m_DBGCircleContext.clear();


        for (u32 it = 0; it < m_DBGPrim3D.size();)
        {
            debugDrawStateScope _(this);
            DBGPrim3D& b = m_DBGPrim3D[it];
            drawPrim3D(drawCallCtx, b.Type, b.M, b.r, b.g, b.b, 1.0f, b.fill );
            b.duration -= LOGICDT;
            if (b.duration <= 0.0f)
                m_DBGPrim3D.eraseKeepOrder(it);
            else
               it++;
        }

        for (u32 it = 0; it < m_DBGSpline.size();)
        {
            debugDrawStateScope _(this);
            DBGSpline& b = m_DBGSpline[it];
            setGlobalColor(b.c);
            drawSpline(primitiveCxt, &b.M, b.t, b.s, b.h );
            debugEndDrawZListNode();
            b.duration -= LOGICDT;
            if (b.duration <= 0.0f)
            {
                m_DBGSpline.eraseKeepOrder(it);
            }
            else
                it++;
        }
    }

    {
        m_DBGTriangleList.reserve((m_DBGLines.size()+m_DBGLinesDuration.size())*6);
     
        for (u32 it = 0; it < m_DBGLines.size();it++)
        {
            fillTriangleList(m_DBGLines[it]);
        }

        for (u32 it = 0; it < m_DBGLinesDuration.size();)
        {
            DBGLine& l = m_DBGLinesDuration[it];

            l.duration -= LOGICDT;
            fillTriangleList(l);

            if (l.duration <= 0.0f)
                m_DBGLinesDuration.eraseKeepOrder(it);
            else
                it++;
        }

        for (u32 it = 0; it < m_DBG3DLines.size();)
        {
            DBG3DLine& l = m_DBG3DLines[it];

            l.duration -= LOGICDT;
            fillTriangleList(l);

            if (l.duration <= 0.0f)
                m_DBG3DLines.eraseKeepOrder(it);
            else
                it++;
        }

        m_DBGLines.clear();
        
    }

    
    {
        for (u32 it = 0; it < m_DBGTriangles.size();)
        {
            DBGTriangle& t = m_DBGTriangles[it];
            debugDrawStateScope _(this);
            drawTriangle(drawCallCtx, t.p1, t.p2, t.p3, t.r, t.g, t.b, t.a, t.z);
            t.duration -= LOGICDT;
            if (t.duration <= 0.0f)
                m_DBGTriangles.eraseKeepOrder(it);
            else
                it++;
        }
    }

    {
        for (u32 it = 0; it < m_DBGTextureTriangles.size();)
        {
            DBGTextureTriangle& t = m_DBGTextureTriangles[it];
            ITF_VECTOR<Vertex>& vtx = *t.pVtx;
            debugDrawStateScope _(this);
            drawTriangle(drawCallCtx, vtx[0], vtx[1], vtx[2], t.t);
            drawTriangle(drawCallCtx, vtx[1], vtx[2], vtx[3], t.t);
            t.duration -= LOGICDT;
            if (t.duration <= 0.0f)
                m_DBGTextureTriangles.eraseKeepOrder(it);
            else
                it++;
        }
    }
    
    if (!isDeviceLost())
    { 
        primitiveCxt.setRenderPassContext(&passCtx);
        primitiveCxt.setPrimitiveParam((GFXPrimitiveParam*)&m_defaultPrimitiveParam);

        ux MaxVertexPerMeshElt = 30000;
 
        u32 listSize = m_DBGTriangleList.size();
        u32 startPos = 0;
        u32 currentSize = listSize;

        currentSize = Min(currentSize, MaxVertexPerMeshElt);

        while (currentSize>0)
        {
            ITF_Mesh meshLines;
            VertexPC    *pdata = 0;
            meshLines.createVertexBuffer
                (
                currentSize,
                VertexFormat_PC,
                sizeof(VertexPC),
                vbLockType_static
                );

            meshLines.addElementAndMaterial();
            meshLines.updatePassFilterFlag();

            ITF_MeshElement & elem = meshLines.getMeshElement();

            elem.m_startVertex                   = 0;
            elem.m_count                        = currentSize;

            meshLines.LockVertexBuffer((void **) &pdata);
            VertexPC    *write = pdata;
            ITF_MemcpyWriteCombined(write, &m_DBGTriangleList[startPos], sizeof(VertexPC) * elem.m_count);
            meshLines.UnlockVertexBuffer();

            debugDrawStateScope _(this);
            drawMesh(primitiveCxt, meshLines);

            startPos += currentSize;
            listSize -= currentSize;
            currentSize = listSize;
            currentSize = Min(currentSize, MaxVertexPerMeshElt);

            meshLines.removeVertexBuffer();
        }
    }
       
    m_DBGTriangleList.clear();
#endif // ITF_SUPPORT_DBGPRIM_MISC
}
#endif // ITF_SUPPORT_DBGPRIM_ANY

//----------------------------------------------------------------------------//

void GFXAdapter::updateCachedScaleValues (Texture* _Tex)
{
    //
    // calculate what to use for x scale
    //
    const float orgW = (float)_Tex->m_datasizeX;
    const float texW = (float)_Tex->getSizeX();

    // if texture and original data width are the same, scale is based
    // on the original size.
    // if texture is wider (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    _Tex->m_texelScaling.x() = 1.0f / ((orgW == texW) ? orgW : texW);

    //
    // calculate what to use for y scale
    //
    const float orgH = (float)_Tex->m_datasizeY;
    const float texH = (float)_Tex->getSizeY();

    // if texture and original data height are the same, scale is based
    // on the original size.
    // if texture is taller (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    _Tex->m_texelScaling.y() = 1.0f / ((orgH == texH) ? orgH : texH);
}

//----------------------------------------------------------------------------//

float GFXAdapter::getSizeNextPOT(float sz) const
{
    u32 size = static_cast<u32>(sz);

    // if not power of 2
    if ((size & (size - 1)) || !size)
    {
        int log = 0;

        // get integer log of 'size' to base 2
        while (size >>= 1)
            ++log;

        // use log to calculate value to use as size.
        size = (2 << log);
    }

    return static_cast<float>(size);
}

//----------------------------------------------------------------------------//
#ifdef ITF_SUPPORT_DBGPRIM_BOX
void GFXAdapter::drawDBGBox( const Vec2d& _pos, f32 _angle, const Vec2d& _extent, float _r, float _g, float _b, float _duration, float _z, bbool _fill )
{
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;

    DBGBox b;
    b.pos = _pos;
    b.angle = _angle;
    b.extent = _extent;
    b.r = _r;
    b.g = _g;
    b.b = _b;
    b.duration = _duration;
    b.z = _z;
    b.fill = _fill;
    m_DBGBoxes.push_back(b);
}

void  GFXAdapter::drawDBG2dBox(const Vec2d& _p, f32 _width, f32 _height, u32 _color0, u32 _color1, u32 _color2, u32 _color3, i32 _rank)
{
    if (hasHideFlags(HideFlag_2DBox))
        return;

    Box2D box;

    const f32 py = (f32)getScreenHeight() - _p.y() - _height;

    /// strip:
    /// 0 -- 2
    /// |    |
    /// 1 -- 3

    box.v[0].m_pos.x() = _p.x();
    box.v[0].m_pos.y() = py;
    box.v[0].m_pos.z() = 0.0f;
    box.v[1].m_pos.x() = _p.x();
    box.v[1].m_pos.y() = py + _height;
    box.v[1].m_pos.z() = 0.0f;
    box.v[2].m_pos.x() = _p.x() + _width;
    box.v[2].m_pos.y() = py;
    box.v[2].m_pos.z() = 0.0f;
    box.v[3].m_pos.x() = _p.x() + _width;
    box.v[3].m_pos.y() = py + _height;
    box.v[3].m_pos.z() = 0.0f;

    box.v[0].m_color = _color0;
    box.v[1].m_color = _color1;
    box.v[2].m_color = _color2;
    box.v[3].m_color = _color3;
    box.m_rank = _rank;

    m_DBGBox2DList.push_back(box);
}
#endif // ITF_SUPPORT_DBGPRIM_BOX

//----------------------------------------------------------------------------//
#ifdef ITF_SUPPORT_DBGPRIM_MISC
void GFXAdapter::drawDBGCircle(float _x, float _y, float _radius, float _r, float _g, float _b, float _duration, float _z, u32 _segsCount)
{
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;

    DBGCircle c;
    c.x = _x;
    c.y = _y;
    c.rad = _radius;
    c.r = _r;
    c.g = _g;
    c.b = _b;
    c.duration = _duration;
    c.z = _z;
    c.segsCount = _segsCount;
    m_DBGCircles.push_back(c);
}

 void DBGContextCircle::init(u32 _countEstimated,u32 _segmentCount)
 {
     segmentCount = _segmentCount;
     m_InternalCircle.reserve(_countEstimated);
 }


 void DBGContextCircle::add(const Vec3d& _pos,f32 _radius,const Color& color)
 {
     DBGContextInternalCircle iC;
     iC.color   = color.getAsU32();
     iC.pos     = _pos;
     iC.radius  = _radius;
     m_InternalCircle.push_back(iC);
     
 }
 
 DBGContextCircle*    GFXAdapter::getDBGContextCircle()
 {
     DBGContextCircle* pContext = new DBGContextCircle;
     m_DBGCircleContext.push_back(pContext);
     return pContext;
 }

 
void GFXAdapter::drawDBGSphere(float _x, float _y, float _radius, float _r, float _g, float _b, float _duration, float _z, u32 _segsCount, bbool _volume, bbool _xaxis, bbool _yaxis, bbool _zaxis)
{
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;

    DBGSphere c;
    c.x = _x;
    c.y = _y;
    c.rad = _radius;
    c.r = _r;
    c.g = _g;
    c.b = _b;
    c.duration = _duration;
    c.z = _z;
    c.segsCount = _segsCount;
    c.volume = _volume;
    c.xaxis = _xaxis;
    c.yaxis = _yaxis;
    c.zaxis = _zaxis;
    m_DBGSpheres.push_back(c);
}

void GFXAdapter::drawDBGPrim3D( i32 _type, const Matrix44 &M, float _r, float _g, float _b, float _duration, bbool _fill )
{
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;

    DBGPrim3D b;
    b.M = M;
    b.r = _r;
    b.g = _g;
    b.b = _b;
    b.duration = _duration;
    b.fill = _fill;
    b.Type = _type;
    m_DBGPrim3D.push_back(b);
}

void GFXAdapter::drawDBGTriangle(   const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r, float _g, float _b, float _a, float _duration, float _z)
{
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;

    DBGTriangle t;
    t.p1 = _p1;
    t.p2 = _p2;
    t.p3 = _p3;
    t.r = _r;
    t.g = _g;
    t.b = _b;
    t.a = _a;
    t.duration = _duration;
    t.z = _z;
    m_DBGTriangles.push_back(t);
}

void  GFXAdapter::drawDBG2dTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, u32 _color, i32 _rank)
{
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;

    VertexPC v[3];

    Triangle2D tri;

    const f32 p1y = (f32)getScreenHeight() - _p1.y();
    const f32 p2y = (f32)getScreenHeight() - _p2.y();
    const f32 p3y = (f32)getScreenHeight() - _p3.y();

    tri.v[0].m_pos.x()  = _p1.x();
    tri.v[0].m_pos.y()  = p1y;
    tri.v[0].m_pos.z()  = 0.0f;
    tri.v[1].m_pos.x()  = _p2.x();
    tri.v[1].m_pos.y()  = p2y;
    tri.v[1].m_pos.z()  = 0.0f;
    tri.v[2].m_pos.x()  = _p3.x();
    tri.v[2].m_pos.y()  = p3y;
    tri.v[2].m_pos.z()  = 0.0f;

    tri.v[0].m_color = _color;
    tri.v[1].m_color = _color;
    tri.v[2].m_color = _color;

    tri.m_rank = _rank;

    m_DBGTriangle2DList.push_back(tri);
}

//----------------------------------------------------------------------------//

void  GFXAdapter::drawDBG2dCircle(const Vec2d& _center, f32 _radius, u32 _color /*= COLOR_WHITE*/, i32 _rank /*= 0*/)
{
    Circle2D circle;    
    f32 WedgeAngle = (2.0f*MTH_PI) / (f32)Circle2D::vertexCount;

    for(u32 i = 0; i < Circle2D::vertexCount; i++)
    {
        //Calculate theta for this vertex
        f32 Theta = i * WedgeAngle;

        //Compute X and Y locations
        f32 cost, sint;
        f32_CosSin(Theta, &cost, &sint);
        f32 X = _center.x() + _radius * cost;
        f32 Y = (getScreenHeight() - _center.y()) - _radius * sint;

        circle.m_vertexes[i].m_pos.x() = X;
        circle.m_vertexes[i].m_pos.y() = Y;
        circle.m_vertexes[i].m_pos.z() = 0.0f;
        circle.m_vertexes[i].m_color = _color;
    }

    circle.m_vertexes[Circle2D::vertexCount - 1] = circle.m_vertexes[0];
    circle.m_rank = _rank;

    m_DBGCircle2DList.push_back(circle);
}
//----------------------------------------------------------------------------//

void GFXAdapter::drawDBG2dLine(const Vec2d& _p1, const Vec2d& _p2, float _size, u32 _startColor, u32 _endColor)
{
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;

    f32 screenHeight = (f32)getScreenHeight();
    Vec2d p1(_p1.x(), screenHeight - _p1.y());
    Vec2d p2(_p2.x(), screenHeight - _p2.y());

    Vec2d dir = (p2 - p1).normalize();
    Vec2d perp = dir.getPerpendicular();
    perp *= _size * 0.5f;

    Box2D box;
    box.v[0].m_pos = (p1 + perp).to3d();
    box.v[1].m_pos = (p2 + perp).to3d();
    box.v[2].m_pos = (p1 - perp).to3d();
    box.v[3].m_pos = (p2 - perp).to3d();

    box.v[0].m_color = _startColor;
    box.v[1].m_color = _endColor;
    box.v[2].m_color = _startColor;
    box.v[3].m_color = _endColor;

    m_DBGBox2DList.push_back(box);
}

void GFXAdapter::drawDBG2dLine(const Vec2d& _p1, const Vec2d& _p2, float _size /* = 1.f */, u32 _color /* = COLOR_WHITE */)
{
    drawDBG2dLine(_p1, _p2, _size, _color, _color);
}

void GFXAdapter::drawDBGTextureTriangle(   ITF_VECTOR<Vertex>* _pVtx, Texture* _tex, float _duration, float _z )
{
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;

    DBGTextureTriangle t;
    t.pVtx = _pVtx;
    t.duration = _duration;
    t.z = _z;
    t.t = _tex;
    m_DBGTextureTriangles.push_back(t);
}

void GFXAdapter::drawDBGQuad (   const Vec2d& _pos, f32 _width, f32 _height, const Texture * _tex, f32 _angle, float _duration , float _z, int _indexInAtlas, bbool _flipX, bbool _flipY, u32 _color )
{
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;
    if (!_tex)
        return;

    f32 c,s;
    f32_CosSin(_angle, &c, &s);
    Vec2d HorizAxis(c, -s);
        
    HorizAxis *= 0.5f;
            
    Vec2d vertAxis = HorizAxis.getPerpendicular();

    HorizAxis *= _width;
    vertAxis *= _height;

    DBGTextureQuad q;
    q.duration = _duration;
    q.z = _z;
    q.t = _tex;
    const UVAtlas *atlas = _tex->getUVAtlas();
    Vec2d uv[4];

    if (atlas)
        atlas->get4UVAt(_indexInAtlas, uv);
    else
    {
         uv[0] = Vec2d(1.f,0.f);
         uv[1] = Vec2d(1.f,1.f);
         uv[2] = Vec2d(0.f,1.f);
         uv[3] = Vec2d(0.f,0.f);
    }

    if (_flipX)
    {
        Swap(uv[0].x(), uv[2].x());
        Swap(uv[1].x(), uv[3].x());
    }

    if (_flipY)
    {
        Swap(uv[0].y(), uv[1].y());
        Swap(uv[2].y(), uv[3].y());
    }

    q.mVtx[0].setData(_pos - HorizAxis - vertAxis, uv[0], _color, _z);
    q.mVtx[1].setData(_pos - HorizAxis + vertAxis, uv[1], _color, _z);
    q.mVtx[2].setData(_pos + HorizAxis - vertAxis, uv[3], _color, _z);
    q.mVtx[3].setData(_pos + HorizAxis + vertAxis, uv[2], _color, _z);

    m_DBGTextureQuads.push_back(q);
}

void GFXAdapter::drawDBGLine(const Vec2d& _p1, const Vec2d& _p2, float _r, float _g, float _b, float _duration, float _size, float _z, float _alpha)
{
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;

    DBGLine l;
    l.p1 = _p1;
    l.p2 = _p2;
    l.r = _r;
    l.g = _g;
    l.b = _b;
    l.size = _size;
    l.duration = _duration;
    l.z  =_z;
    l.alpha = _alpha;

    if (_duration>0.0f)
        m_DBGLinesDuration.push_back(l);
    else
        m_DBGLines.push_back(l);
}

void GFXAdapter::drawDBG3DLine(const Vec3d& _p1, const Vec3d& _p2, float _r, float _g, float _b, float _duration, float _size, float _alpha)
{
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;

    DBG3DLine l;
    l.p1 = _p1;
    l.p2 = _p2;
    l.r = _r;
    l.g = _g;
    l.b = _b;
    l.size = _size;
    l.duration = _duration;
    l.alpha = _alpha;
    m_DBG3DLines.push_back(l);
}

void GFXAdapter::drawDBGAABB(const AABB& _aabb, float _r, float _g, float _b, float _duration, float _size, float _z)
{
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;

    const Vec2d v1(_aabb.getMin());
    const Vec2d v2(_aabb.getMaxXMinY());
    const Vec2d v3(_aabb.getMax());
    const Vec2d v4(_aabb.getMinXMaxY());

    drawDBGLine(v1, v2, _r, _g, _b, _duration, _size, _z);
    drawDBGLine(v2, v3, _r, _g, _b, _duration, _size, _z);
    drawDBGLine(v3, v4, _r, _g, _b, _duration, _size, _z);
    drawDBGLine(v4, v1, _r, _g, _b, _duration, _size, _z);
}

void GFXAdapter::drawDBGSpline(const Spline_Info *_splineInfo, float _duration)
{
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;

    DBGSpline sp;
    sp.M = _splineInfo->m_mat;
    sp.t = _splineInfo->m_texture;
    sp.s = &_splineInfo->m_spline;
    sp.h = _splineInfo->m_height;
    sp.duration = _duration;
    sp.c = _splineInfo->m_color;
    m_DBGSpline.push_back(sp);
}
#endif // ITF_SUPPORT_DBGPRIM_MISC

//----------------------------------------------------------------------------//

void GFXAdapter::beginDisplay(f32 _dt)
{
	m_currentDT = _dt;

    m_defaultClearColor.setAlpha(gAlphaClearColor);
    
    m_clearColor = m_defaultClearColor;
	m_lightManager->beginDisplay(_dt);

    m_currentPrimitiveParam = &m_defaultPrimitiveParam;

    m_VertexBufferManager.DynamicRingVBstartFrame();
    m_screenTransitionManager.update();
}

void GFXAdapter::endDisplay()
{
    m_VertexBufferManager.DynamicRingVBendFrame();

    //reset GFX_Zlist
    const ITF_VECTOR<View*>& views = View::getViews();
    for(u32 iView = 0; iView < views.size(); ++iView)
    {
        View* pCurrentView = views[iView];
        if(pCurrentView->isActive() && pCurrentView->getCamera())
		{
            getZListManager().reset(pCurrentView->getZlistID());
		}
    }

	getGridFluidManager().clearFluidPrimitiveList();
}

void GFXAdapter::beginViewDisplay()
{
	m_lightManager->beginViewDisplay();

    m_frameHaveRefractionPass = bfalse;
}

void GFXAdapter::endViewDisplay()
{
    // ...
}

void GFXAdapter::beginViewportRendering(View &_view)
{
}

void GFXAdapter::endViewportRendering(View &_view)
{
	drawViewFrame(_view);
}

// Reflection Pass

void GFXAdapter::beginReflectionRendering()
{
    const reflectionViewParam reflectionParam = View::getCurrentView()->getReflectionPlaneReference();
    f32 yReflectionPlane = -1.0f * reflectionParam.m_reflectionPlaneY;

    //mirror the camera
    const Matrix44* normalView = getViewMatrix();
    Matrix44 mirrorView = *normalView;
    getWorldViewProjMatrix()->push();

    //translation
    Float4 vtrans = mirrorView.T();
    Vec3d vReflectionTrans;
    storeXYZ((f32*)&vReflectionTrans,vtrans);
    vReflectionTrans.setY(-1.0f * (2 * yReflectionPlane -  vReflectionTrans.getY()));

    //rotation
    mirrorView.setTranslationToZero();

    Float4 vJ = mirrorView.J();
    vJ = neg(vJ);
    mirrorView.setJ(vJ);
    mirrorView.setTranslation(vReflectionTrans);
    getWorldViewProjMatrix()->setView(mirrorView);

    // Clip plane
    GFX_Vector4 vPos = GFX_Vector4(0.0f, reflectionParam.m_reflectionPlaneY, reflectionParam.m_reflectionPlaneZ, 0.0f);
    GFX_Vector4 vNormal = GFX_Vector4(0.0f, 1.0f, 0.0f, 0.0f);
//    f32 dFactor = (vPos.m_x*vNormal.x() + vPos.m_y*vNormal.y() + vPos.z()*vNormal.z())* -1.0f;

    // plane equation in worldspace
    //setClipPlane(btrue, vNormal.x(), vNormal.y(), vNormal.z(), dFactor); //only on PC

    updateShaderScreenDimension(getReflectionPassWidth(), getReflectionPassHeight());

}

void GFXAdapter::endReflectionRendering()
{
    //restore parameter
    getWorldViewProjMatrix()->pop();
    updateShaderScreenDimension(getScreenWidth(), getScreenHeight());
    //setClipPlane(bfalse);
}


void GFXAdapter::setZListPassDebugModeObjectRef(GFX_ZLIST_PASS_TYPE _zlistPass, const ObjectRef & _objRef)
{
    m_zlistPassObjRef[_zlistPass] = _objRef;
}

//////////////////////////////////////////////////////////////////////////
// The picking is rendering every primitive in a specific 4x4 part of the
// picking render target using a specially crafted camera restrained on a
// screen location.
// By re-reading specific pixels, it's then possible to know if the primitive
// has visible parts at this screen location.
void GFXAdapter::drawPickingPrimitives(View &_view, const Vec2d& _pickingXY, u8 _alphaPickRef)
{
#ifdef ITF_PICKING_SUPPORT

    beginViewportRendering(_view);
    // 3 members used at platform gfx device level -> m_bPickingMode, m_PickingPos and m_deltaPicking
    m_bPickingMode = btrue; // inform the rest of the rendering for picking specific hacks
    m_PickingPos = _pickingXY; // stay at the same value during all the picking requests
    m_alphaPickRef = _alphaPickRef; // idem
    m_deltaPicking = Vec2d(0.f, 0.f);

    m_pickingScissor.left = 0;
    m_pickingScissor.right = NbPickingPixelsPerObject;
    m_pickingScissor.top = 0;
    m_pickingScissor.bottom = NbPickingPixelsPerObject;

    // we keep objects in the given order
    const u32 zListViewID = _view.getZlistID();
    if(zListViewID == View::ZlistID_invalid)
        return;

    //////////////////////////////////////////////////////////////////////////
    // Main rendering picking
    //////////////////////////////////////////////////////////////////////////

    GFX_Zlist<GFXPrimitive> & mainZList = getZListManager().getZlistAt<GFX_ZLIST_MAIN>(zListViewID);

    RenderPassContext rdrPassPicking = RenderPassContext(mainZList)
                                                        .setPassType(GFX_ZLIST_PASS_REGULAR)
                                                        .setClear(btrue)
                                                        .setClearColor(Color::zero());
    rdrPassPicking.getStateCache().setAlphaRef(int(m_alphaPickRef))
        .setAlphaTest(btrue)
        .setSeparateAlphaBlend(GFX_ALPHAMASK_NONE);

    PickingRenderPassAlgo pickingRenderPassAlgo(m_renderPassAlgo[rdrPassPicking.getPass()]);
    m_renderPassAlgo[rdrPassPicking.getPass()] = &pickingRenderPassAlgo;
    drawRenderPass(rdrPassPicking);
    m_renderPassAlgo[rdrPassPicking.getPass()] = pickingRenderPassAlgo.getOverridenAlgo();
    mainZList.reset();

    //////////////////////////////////////////////////////////////////////////
    // 3D objects rendering picking
    //////////////////////////////////////////////////////////////////////////

    GFX_Zlist<GFXPrimitive> &  render3DZList = getZListManager().getZlistAt<GFX_ZLIST_3D>(zListViewID);
    RenderPassContext rdrPass3DPicking = RenderPassContext(render3DZList)
                                                            .setPassType(GFX_ZLIST_PASS_3D);
    rdrPass3DPicking.getStateCache().setAlphaRef(int(m_alphaPickRef))
        .setAlphaTest(btrue)
        .setSeparateAlphaBlend(GFX_ALPHAMASK_NONE);

    pickingRenderPassAlgo.setOverridenAlgo(m_renderPassAlgo[rdrPass3DPicking.getPass()]);
    m_renderPassAlgo[rdrPass3DPicking.getPass()] = &pickingRenderPassAlgo;
    drawRenderPass(rdrPass3DPicking);
    m_renderPassAlgo[rdrPass3DPicking.getPass()] = pickingRenderPassAlgo.getOverridenAlgo();
    render3DZList.reset();

    //////////////////////////////////////////////////////////////////////////
    // 2D (UI) objects rendering picking
    //////////////////////////////////////////////////////////////////////////

    init2DRender();
    drawPrimitives2d(_view);
    end2DRender();

    if (m_bPickingDebug)
        copyPickingToDebugRenderTarget();

    // reset stuff
    setScissorRect(NULL);

    resetSceneResolvedFlag();

//    setRenderTarget_BackBuffer();

    endViewportRendering(_view);
    m_bPickingMode = bfalse;
#endif


}

#ifdef ITF_PICKING_SUPPORT
// picking -> tool fct to advance to next item area
static Vec2d advanceOnePickingArea(Vec2d _pickArea, f32 _width)
{
    f32 moveDelta = f32(NbPickingPixelsPerObject);
    _pickArea.x() += moveDelta;
    if (_pickArea.x()>=_width)
    {
        _pickArea.x() = 0;
        _pickArea.y() += moveDelta;
    }
    return _pickArea;
}
#endif // ITF_PICKING_SUPPORT

void GFXAdapter::resolvePickingList(SafeArray<u32> & _pickIdxResult, ux _nbMax)
{
    _pickIdxResult.clear();

#ifdef ITF_PICKING_SUPPORT
/*    void* backbufferPicking = resolvePicking();
    if  (!backbufferPicking)//backbufferPicking could be null if the device is lost ,in this situation we must skip the picking until the device is properly restored.
        return;

    u32 pitch = 0;
    u32* pPixels = lockPickingBuffer(backbufferPicking,pitch);
 
    Vec2d pickingArea = Vec2d::Zero;
    f32 pickingWidth = f32(getPickingWidth());
    

    for(u32 i = 0; i<_nbMax; ++i)
    {
        Color backbufferColor = getBufferValue(pPixels,pitch, u32(pickingArea.x()), u32(pickingArea.y()));
        pickingArea = advanceOnePickingArea(pickingArea, pickingWidth);
        if (backbufferColor != Color::zero())
        {
            _pickIdxResult.push_back(i);
        }
    }

    // second set with 3d objects
    for(u32 i = 0; i<_nbMax; ++i)
    {
        Color backbufferColor = getBufferValue(pPixels,pitch, u32(pickingArea.x()), u32(pickingArea.y()));
        pickingArea = advanceOnePickingArea(pickingArea, pickingWidth);
        if (backbufferColor != Color::zero())
        {
            _pickIdxResult.push_back(i);
        }
    }
    
    // now loop on each 2D priority
    for(ux prio = 0; prio < PriorityCount2d; ++prio)
    {
        for(u32 i = 0; i<_nbMax; ++i)
        {
            Color backbufferColor = getBufferValue(pPixels,pitch, u32(pickingArea.x()), u32(pickingArea.y()));
            pickingArea = advanceOnePickingArea(pickingArea, pickingWidth);
            if (backbufferColor != Color::zero())
            {
                _pickIdxResult.push_back(i);
            }
        }
    }
    
    unlockPickingBuffer(backbufferPicking);*/

    if(resolvePicking())
    {
        u32 pitch = 0;
        u32* pPixels = lockPickingBuffer(pitch);
 
        Vec2d pickingArea = Vec2d::Zero;
        f32 pickingWidth = f32(getPickingWidth());
    

        for(u32 i = 0; i<_nbMax; ++i)
        {
            Color backbufferColor = getBufferValue(pPixels,pitch, u32(pickingArea.x()), u32(pickingArea.y()));
            pickingArea = advanceOnePickingArea(pickingArea, pickingWidth);
            if (backbufferColor != Color::zero())
            {
                _pickIdxResult.push_back(i);
            }
        }

        // second set with 3d objects
        for(u32 i = 0; i<_nbMax; ++i)
        {
            Color backbufferColor = getBufferValue(pPixels,pitch, u32(pickingArea.x()), u32(pickingArea.y()));
            pickingArea = advanceOnePickingArea(pickingArea, pickingWidth);
            if (backbufferColor != Color::zero())
            {
                _pickIdxResult.push_back(i);
            }
        }
    
        // now loop on each 2D priority
        for(ux prio = 0; prio < PriorityCount2d; ++prio)
        {
            for(u32 i = 0; i<_nbMax; ++i)
            {
                Color backbufferColor = getBufferValue(pPixels,pitch, u32(pickingArea.x()), u32(pickingArea.y()));
                pickingArea = advanceOnePickingArea(pickingArea, pickingWidth);
                if (backbufferColor != Color::zero())
                {
                    _pickIdxResult.push_back(i);
                }
            }
        }
    
        unlockPickingBuffer();
    }
#endif
}

void GFXAdapter::drawOccludePrimitives(View &_view) // draw regular primitives with an alpha test and a specific color code for each pickable
{
#ifndef ITF_FINAL
    u32 zListViewID = _view.getZlistID();
    GFX_Zlist<GFXPrimitive> & renderZList = getZListManager().getZlistAt<GFX_ZLIST_MAIN>(zListViewID);
    renderZList.sort();

    toggleDebugColor();

    Float4 widthHeight = loadXYZW(f32(_view.getViewport().m_width), f32(_view.getViewport().m_height), 0.f, 0.f);

    RenderPassContext rdrPassOcclude = RenderPassContext(renderZList)
                                            .setPassType(GFX_ZLIST_PASS_ZPREPASS)
                                            .setRenderTargetSize(widthHeight)
                                            .setReverseOrder();

    bbool savedLightingenabled = m_LightingEnable;
    m_LightingEnable = bfalse;
    OccluderMapRenderPassAlgo occluderMapRdrPassAlgo(m_renderPassAlgo[rdrPassOcclude.getPass()]);

    occluderMapRdrPassAlgo.begin(rdrPassOcclude);

    f32 zCamRef = CAMERA->getPos().z();
    u32 passFilterFlag = rdrPassOcclude.getPassFilterFlag();

    OccluderColorMap occluderColorMap;

    for (ux i = rdrPassOcclude.getEndZListIdx(), n = rdrPassOcclude.getBeginZListIdx();
        i != n;
        i--)
    {
        rdrPassOcclude.setCurrentZListIdx(i-1u);
        const ZList_Node_Prim & node = rdrPassOcclude.getNode(i-1u);
        const BaseObject * baseObj = node.getObjectRef().getObject();
        if ((node.m_primitive->getPassFilterFlag() & passFilterFlag) == 0)
            continue;

        if (baseObj)
        {
            // generate color from address
            Seeder seeder((u32)((uPtr)baseObj&0xFFffFFff));
            seeder.GetU32();
            seeder.GetU32();
            u32 objColor;
            bbool dupFound;
            do
            {
                dupFound = bfalse;
                objColor = seeder.GetU32() | 0xFF000000u;
                ObjectRef *aRef = occluderColorMap.find(objColor);
                dupFound = (m_clearColor.getAsU32() == objColor) || ( (aRef!=0) && (baseObj->getRef() != *aRef ) );
                /*
                OccluderColorMap::const_iterator itDup = occluderColorMap.find(objColor);
                dupFound = (m_clearColor.getAsU32() == objColor)
                        || ((itDup != occluderColorMap.end()) && (baseObj->getRef() != itDup->second));
                 */
            } while(dupFound); // be sure it's unique

            occluderColorMap.insert(objColor, baseObj->getRef() );
            //occluderColorMap.insert(OccluderColorMap::value_type(objColor, baseObj->getRef()));
            setDebugColor(Color(ITFCOLOR_TO_U32(255, objColor>>16, objColor>>8u, objColor)));

            f32 drawZWorld = node.m_depth;
            f32 drawZCam = drawZWorld - zCamRef;// inform the renderer the actual z position in camera space.
            f32 drawZCamAbs = f32_Abs(drawZCam);

            m_currentDrawZWorld = drawZWorld;
            m_currentDrawZCam = drawZCam; 
            m_currentDrawZCamAbs = drawZCamAbs;

            node.m_primitive->directDraw(rdrPassOcclude, this, drawZCam, drawZWorld, drawZCamAbs);
        }

    }

    occluderMapRdrPassAlgo.end(rdrPassOcclude, rdrPassOcclude.getResolveTarget());

    m_LightingEnable = savedLightingenabled;

    toggleDebugColor();

    copyPassToDebugRenderTarget(GFX_ZLIST_PASS_REGULAR);

    computeOccluderStats(occluderColorMap);
#endif // ITF_FINAL
}

void GFXAdapter::computeOccluderStats(const OccluderColorMap & _occluderMap)
{
    m_occluderStats.clear();

#ifndef ITF_FINAL

    u32 width = 0, height = 0, pixelPitch = 0;
    u32 * pixels = lockDebugRenderTarget(width, height, pixelPitch);

    if (!pixels)
        return;

    typedef ITF_HASHMAP<u32, u32> OccluderPixelCount;
    OccluderPixelCount occluderPixelCount;

    for(ux y = 0; y < height; ++y)
    {
        for(ux x = 0; x < width; ++x)
        {
            u32 color = pixels[x + y*pixelPitch] | 0xFF000000u;
            u32 *colorRes = occluderPixelCount.find(color);
            if (colorRes == 0 )
                occluderPixelCount.insert( color, 1 );
            else
                (*colorRes)++;
        }
    }

    unlockDebugRenderTarget();

    f32 nbTotalPixels = f32(width * height);
    if (nbTotalPixels == 0.f) return;

    for(OccluderPixelCount::const_iterator it = occluderPixelCount.begin();
        it!=occluderPixelCount.end();
        ++it)
    {
        ObjectRef *aRef = _occluderMap.find(it.getkey());
        if (aRef != 0)
        {
            m_occluderStats.emplace_back();
            m_occluderStats.back().objRef = *aRef;
            m_occluderStats.back().screenRatio = f32(it.getval()) / nbTotalPixels;
        }
    }
#endif // ITF_FINAL
}

void GFXAdapter::getLastOccluderStats(vector<OccluderStat> & _stats)
{
    std::swap(m_occluderStats, _stats);
    m_occluderStats.clear();
}

void GFXAdapter::drawPrimitives(View &_view)
{
    u32 zListViewID = _view.getZlistID();
    if(zListViewID == View::ZlistID_invalid)
        return;

    PRF_M_SCOPE(drawPrimitives);

	// Compute render param for this rendering.
	m_renderParamManager->processRenderParam(_view);
    // Set if we use ZPrepass.
    computeUsingZPrepass();

#ifdef ITF_USE_REMOTEVIEW
    if(getDrawPassHide((engineView)getCurrentEngineView()))
        return;
#endif // ITF_USE_REMOTEVIEW

    if (m_renderOccluderMap)
    {
        drawOccludePrimitives(_view);

        if (m_renderOccluderMap>1)
        {
            copyDebugRenderTarget();
            endSceneRendering(_view);
            return;
        }
    }

    GFX_Zlist<GFXPrimitive> & render3DList = getZListManager().getZlistAt<GFX_ZLIST_3D>(zListViewID);
    GFX_Zlist<GFXPrimitive> & renderZList = getZListManager().getZlistAt<GFX_ZLIST_MAIN>(zListViewID);
    GFX_Zlist<GFXPrimitive> & maskList = getZListManager().getZlistAt<GFX_ZLIST_MASK>(zListViewID);

    render3DList.sort();
    renderZList.sort();
    
    // build (width, height, 0, 0) Float4
    Float4 widthHeight = loadXYZW(f32(_view.getViewport().m_width), f32(_view.getViewport().m_height), 0.f, 0.f);
	Camera *cam = _view.getCamera();
	f32 farClipDist = cam->getPos().z() - cam->getFarPlaneDist();

    // init pass contexts
    RenderPassContext rdrZPrePass = RenderPassContext(renderZList)
                                            .setPassType(GFX_ZLIST_PASS_ZPREPASS)
                                            .setReverseOrder()
                                            .setRenderTargetSize(widthHeight);

    RenderPassContext rdrPass3D = RenderPassContext(render3DList)
                                            .setPassType(GFX_ZLIST_PASS_3D)
                                            .setReverseOrder() // Front to back
                                            .setRenderTargetSize(widthHeight);

    RenderPassContext rdrMaskPass = RenderPassContext(maskList)
                                            .setPassType(GFX_ZLIST_PASS_MASK)
                                            .setRenderTargetSize(widthHeight);

    RenderPassContext rdrPassRegular = RenderPassContext(renderZList)
                                                            .setRenderTargetSize(widthHeight);

	rdrZPrePass.setFarPlaneDist(farClipDist);
	rdrPass3D.setFarPlaneDist(farClipDist);
	rdrPassRegular.setFarPlaneDist(farClipDist);

    // Reflection
    static bbool skipReflection = bfalse;
    bbool useReflection = View::getCurrentView()->getUseReflectionPlane(CURRENTFRAME) != 0;
    setUseReflection(useReflection);
    if (isUseReflection() && !isPickingMode() && !skipReflection)
    {
       beginGPUEvent("Reflection Pass", GFX_GPUMARKER_Reflection);

       RenderPassContext rdrPassReflectionPrepass = RenderPassContext(rdrPassRegular)
           .setPassType(GFX_ZLIST_PASS_REFLECTION_PREPASS)
           .setClear(btrue)
           .setClearColor(Color::zero())
           .setClearZStencil(btrue)
           .setResolveTarget(bfalse);

       RenderPassContext rdrPassReflection = RenderPassContext(rdrPassRegular)
           .setPassType(GFX_ZLIST_PASS_REFLECTION)
           .setClear(bfalse)
           .setResolveTarget(btrue);

        drawRenderPass(rdrPassReflectionPrepass);

        beginReflectionRendering();
        drawRenderPass(rdrPassReflection);
        endReflectionRendering();
        
		m_lightManager->endDisplay3D();
        m_frameHaveRefractionPass = bfalse;

        //hack for clearZ bug
        rdrZPrePass.setClearZStencil(btrue);

        endGPUEvent(GFX_GPUMARKER_Reflection);
    }

    //Fluid Render Pass
	if ( isUsingFluids() )
	{
		static bbool skipFluidRenderer = bfalse;
		if(!isPickingMode() && !skipFluidRenderer)
		{
			RenderPassContext rdrPassFluid = RenderPassContext(rdrPassRegular)
				.setPassType(GFX_ZLIST_PASS_FLUID)
				.setClear(btrue)
				.setClearColor(Color::zero())
				.setResolveTarget(btrue);

			if(rdrPassFluid.needRenderPass(GFX_ZLIST_PASS_FLUID))
			{
				beginGPUEvent("Fluid Pass", GFX_GPUMARKER_Fluid);
				updateShaderScreenDimension(getFluidPassWidth(), getFluidPassHeight());
				drawRenderPass(rdrPassFluid);
				updateShaderScreenDimension(getScreenWidth() , getScreenHeight());
				endGPUEvent(GFX_GPUMARKER_Fluid);
			}
		}
	}

	// Mask material pass.
	m_maskBufferFilled = false;
	if ( maskList.getNumberEntry() > 0 )
	{
		maskList.sort();
        if( m_maskBuffer != NULL )
        {
		    updateShaderScreenDimension(m_maskBuffer->getSizeX(), m_maskBuffer->getSizeY());
		    drawRenderPass(rdrMaskPass);
        }

		updateShaderScreenDimension(getScreenWidth() , getScreenHeight());
	}

    beginSceneRendering(_view);

    if (isUsingZPrepass())
    {
        setShaderRepartition(20);
        beginGPUEvent("ZPrePass", GFX_GPUMARKER_ZPass);
        drawRenderPass(rdrZPrePass);
        endGPUEvent(GFX_GPUMARKER_ZPass);
        setShaderRepartition(80);
    }

	// Setup global lighting constants.
	m_lightManager->setGlobalLighting(_view);

	startOverDrawRender();

	drawRenderPass(rdrPass3D);
    drawRenderPass(rdrPassRegular);

    // Render sub views
    drawSubViewPrimitives(_view);

	endOverDrawRender();

	resetGlobalParameters();           // Reset render param coloration for the next rendering.
    endSceneRendering(_view);

    DebugCopyZListPass();
}

void extrudeTriangle(VertexPCT *_vertices, f32 _extrudeDist)
{
	Vec3d a, b, c;
	a = _vertices[0].m_pos - _vertices[1].m_pos;
	b = _vertices[0].m_pos - _vertices[2].m_pos;
	c = _vertices[1].m_pos - _vertices[2].m_pos;
	a.normalize();
	b.normalize();
	c.normalize();
	if ( f32_Abs(a.x()) < 0.5f ) a.x() = 0.0f; // Keep a and b axis aligned.
	if ( f32_Abs(a.y()) < 0.5f ) a.y() = 0.0f;
	if ( f32_Abs(b.x()) < 0.5f ) b.x() = 0.0f;
	if ( f32_Abs(b.y()) < 0.5f ) b.y() = 0.0f;
	a.normalize();
	b.normalize();
	c.normalize();
	_vertices[0].m_pos += a * _extrudeDist;
	_vertices[0].m_pos += b * _extrudeDist;
	f32 m1 = f32_Abs(1.0f / b.dot(c));
	_vertices[1].m_pos += c * _extrudeDist * m1;
	f32 m2 = f32_Abs(1.0f / a.dot(c));
	_vertices[2].m_pos -= c * _extrudeDist * m2;
}

void GFXAdapter::clearViewportColor(View &_mainView, View &_subView)
{
	const Vec3d *viewPoints = _subView.getWantedViewportPoints();

	_mainView.setCurrentView();
	setCamera(_mainView.getCamera());

	u32 saveScreenWidth = m_screenWidth;
	u32 saveScreenHeight = m_screenHeight;
	u32 screenx = static_cast<u32>(_subView.getScreenSizeX());
	u32 screeny = static_cast<u32>(_subView.getScreenSizeY());
	forceScreenSize( screenx, screeny );

	VertexPCT v[6];
	static f32 z = -1.0f;
	v[0].m_pos.set(viewPoints[0].x(), viewPoints[0].y(), z);
	v[1].m_pos.set(viewPoints[1].x(), viewPoints[1].y(), z);
	v[2].m_pos.set(viewPoints[2].x(), viewPoints[2].y(), z);

	v[3].m_pos.set(viewPoints[0].x(), viewPoints[0].y(), z);
	v[4].m_pos.set(viewPoints[2].x(), viewPoints[2].y(), z);
	v[5].m_pos.set(viewPoints[3].x(), viewPoints[3].y(), z);

	for ( ux i = 0; i < 6; i++ )
	{
		v[i].m_uv.set(0.0f, 0.0f);
		v[i].m_color = COLOR_WHITE;
		v[i].m_pos.y() = screeny - v[i].m_pos.y();
	}

    m_curPassType = GFX_ZLIST_PASS_2D;

	init2DRender();

	bbool usingZInjectSave = m_isUsingZInject;
	m_isUsingZInject = false;
    RenderPassContext m_passCtx;
    PrimitiveContext m_primitiveCxt(&m_passCtx);
    DrawCallContext drawCallCtx(&m_primitiveCxt);
    drawCallCtx.getStateCache().setAlphaTest(false);
    drawCallCtx.getStateCache().setDepthTest(false);
    drawCallCtx.getStateCache().setDepthWrite(false);
    drawCallCtx.getStateCache().setDepthBias(0.000f);
    drawCallCtx.getStateCache().setDepthFunc(GFX_CMP_ALWAYS);
	drawCallCtx.getStateCache().setColorWrite(true);
	drawCallCtx.getStateCache().setAlphaWrite(false);
	setShaderGroup(m_defaultShaderGroup);
	setGfxMatDefault(drawCallCtx);
	SetDrawCallState(drawCallCtx.getStateCache(), 0xFFffFFff, btrue);
	bindTexture(0, m_whiteOpaqueTexture, btrue);
	setVertexFormat(VertexFormat_PCT);

	setDefaultGFXPrimitiveParam();
    m_vsGlobal.globalColor.x() = m_clearColor.getRed();
    m_vsGlobal.globalColor.y() = m_clearColor.getGreen();
    m_vsGlobal.globalColor.z() = m_clearColor.getBlue();
    m_vsGlobal.globalColor.m_w = 1.0f;
	DrawPrimitive(drawCallCtx, GFX_TRIANGLES, (void*)v, 6);

	end2DRender();
	forceScreenSize( saveScreenWidth, saveScreenHeight );
	m_isUsingZInject = usingZInjectSave;

	_subView.setCurrentView();
	m_curPassType = GFX_ZLIST_PASS_REGULAR;
	setCamera(_subView.getCamera());
}

void GFXAdapter::renderViewportMask(View &_mainView, View &_subView)
{
	const GFX_Viewport& wantedViewport = _subView.getWantedViewport();
	const Vec3d *viewPoints = _subView.getWantedViewportPoints();
	const Vec3d *leftPoint, *rightPoint, *topPoint, *bottomPoint;

	_mainView.setCurrentView();
	setCamera(_mainView.getCamera());

	Vec3d side = viewPoints[1] - viewPoints[0];
	side.normalize();
	if ( ( f32_Abs(side.x()) > 0.99999f ) || ( f32_Abs(side.y()) > 0.99999f ) )
		return; // Rectangle is nearly axis aligned.

	leftPoint = viewPoints + 0;
	rightPoint = viewPoints + 0;
	topPoint = viewPoints + 0;
	bottomPoint = viewPoints + 0;
	for ( ux i = 1; i < 4; i++ )
	{
		if ( leftPoint->x() > viewPoints[i].x() )
			leftPoint = viewPoints + i;
		if ( rightPoint->x() < viewPoints[i].x() )
			rightPoint = viewPoints + i;
		if ( topPoint->y() > viewPoints[i].y() )
			topPoint = viewPoints + i;
		if ( bottomPoint->y() < viewPoints[i].y() )
			bottomPoint = viewPoints + i;
	}

	u32 saveScreenWidth = m_screenWidth;
	u32 saveScreenHeight = m_screenHeight;
	u32 screenx = static_cast<u32>(_subView.getScreenSizeX());
	u32 screeny = static_cast<u32>(_subView.getScreenSizeY());
	forceScreenSize( screenx, screeny );

	VertexPCT v[12];
	static f32 z = -1.0f;
	v[0].m_pos.set(f32(wantedViewport.m_x), f32(wantedViewport.m_y), z);
	v[1].m_pos.set(topPoint->x(), topPoint->y(), z);
	v[2].m_pos.set(leftPoint->x(), leftPoint->y(), z);

	v[3].m_pos.set(f32(wantedViewport.m_x + wantedViewport.m_width), f32(wantedViewport.m_y), z);
	v[4].m_pos.set(rightPoint->x(), rightPoint->y(), z);
	v[5].m_pos.set(topPoint->x(), topPoint->y(), z);

	v[6].m_pos.set(f32(wantedViewport.m_x + wantedViewport.m_width), f32(wantedViewport.m_y + wantedViewport.m_height), z);
	v[7].m_pos.set(bottomPoint->x(), bottomPoint->y(), z);
	v[8].m_pos.set(rightPoint->x(), rightPoint->y(), z);

	v[9].m_pos.set(f32(wantedViewport.m_x), f32(wantedViewport.m_y + wantedViewport.m_height), z);
	v[10].m_pos.set(leftPoint->x(), leftPoint->y(), z);
	v[11].m_pos.set(bottomPoint->x(), bottomPoint->y(), z);

	f32 extrudeDist = 1.0f;
	extrudeTriangle(v, extrudeDist);
	extrudeTriangle(v + 3, extrudeDist);
	extrudeTriangle(v + 6, extrudeDist);
	extrudeTriangle(v + 9, extrudeDist);

	for ( ux i = 0; i < 12; i++ )
	{
		v[i].m_uv.set(0.0f, 0.0f);
		v[i].m_color = COLOR_WHITE;
		v[i].m_pos.y() = screeny - v[i].m_pos.y();
	}

    m_curPassType = GFX_ZLIST_PASS_2D;

	init2DRender();

	bbool usingZInjectSave = m_isUsingZInject;
	m_isUsingZInject = false;
    RenderPassContext m_passCtx;
    PrimitiveContext m_primitiveCxt(&m_passCtx);
    DrawCallContext drawCallCtx(&m_primitiveCxt);
    drawCallCtx.getStateCache().setAlphaTest(false);
    drawCallCtx.getStateCache().setDepthTest(btrue);
    drawCallCtx.getStateCache().setDepthWrite(btrue);
    drawCallCtx.getStateCache().setDepthBias(0.000f);
    drawCallCtx.getStateCache().setDepthFunc(GFX_CMP_ALWAYS);
	drawCallCtx.getStateCache().setColorWrite(false);
	drawCallCtx.getStateCache().setAlphaWrite(false);
	setShaderGroup(m_defaultShaderGroup);
	setGfxMatDefault(drawCallCtx);
	SetDrawCallState(drawCallCtx.getStateCache(), 0xFFffFFff, btrue);
	bindTexture(0, m_whiteOpaqueTexture, btrue);
	setVertexFormat(VertexFormat_PCT);

	setDefaultGFXPrimitiveParam();
	DrawPrimitive(drawCallCtx, GFX_TRIANGLES, (void*)v, 12);

	end2DRender();
	forceScreenSize( saveScreenWidth, saveScreenHeight );
	m_isUsingZInject = usingZInjectSave;

	_subView.setCurrentView();
	m_curPassType = GFX_ZLIST_PASS_REGULAR;
	setCamera(_subView.getCamera());
}

void GFXAdapter::preRenderViews(View & _view)
{
    GFX_ADAPTER->prerenderPass(_view.getZlistID());

    const ITF_VECTOR<View*>& views = View::getViews();
    for(u32 iView = 0; iView < views.size(); ++iView)
    {
        View* pCurrentView = views[iView];

        if(!pCurrentView->isActive() || pCurrentView->isDisabledForRendering())
            continue;

		if(pCurrentView->getParentView() != &_view)
            continue;

        GFX_ADAPTER->prerenderPass(pCurrentView->getZlistID());
    }
}

void GFXAdapter::drawSubViewPrimitives(View &_view)
{
    const ITF_VECTOR<View*>& views = View::getViews();
    for(u32 iView = 0; iView < views.size(); ++iView)
    {
        View* pCurrentView = views[iView];

        if(pCurrentView->getParentView() != &_view)
            continue;

        if(!pCurrentView->isActive()/* || pCurrentView->isDisabledForRendering()*/)
            continue;

        u32 zListSubViewID = pCurrentView->getZlistID();
		if ( zListSubViewID == (u32)-1 )
			continue;

        if(pCurrentView->getWaitForPrefetchBeforeDisplay())
        {
            if(!pCurrentView->isPhysicalReady())
                continue;
        }

		// Set Camera  & viewport
        pCurrentView->setCurrentView();
        Camera *cam = pCurrentView->getCamera();
		m_curPassType = GFX_ZLIST_PASS_REGULAR;
        pCurrentView->apply();
        cam->setDeltaFogZ(cam->getZ());

		if ( (pCurrentView->getViewport().m_width == 0.0f ) || ( pCurrentView->getViewport().m_height == 0.0f ) )
			continue;

		// Compute render param for this rendering.
		m_renderParamManager->processRenderParam(*pCurrentView);

		// clear Z and color if needed
        u32 buffers = GFX_CLEAR_ZBUFFER | GFX_CLEAR_STENCIL;
        if ( pCurrentView->getClearViewport() && ( pCurrentView->getViewportRotation() == 0.0f ) )
            buffers |= GFX_CLEAR_COLOR;
        clear(buffers, m_clearColor.m_r, m_clearColor.m_g, m_clearColor.m_b, 1.0f);
        if ( pCurrentView->getClearViewport() && ( pCurrentView->getViewportRotation() != 0.0f ) )
            clearViewportColor(_view, *pCurrentView);

        // Render list
        GFX_Zlist<GFXPrimitive> & renderZSubList = getZListManager().getZlistAt<GFX_ZLIST_MAIN>(zListSubViewID);
        renderZSubList.sort();

        Float4 widthHeight = loadXYZW(f32(pCurrentView->getViewport().m_width), f32(pCurrentView->getViewport().m_height), 0.f, 0.f);
        f32 farClipDist = cam->getPos().z() - cam->getFarPlaneDist();

        // Z prepass
/*#ifndef ITF_GLES2
		if (isUsingZPrepass())
        {
            RenderPassContext rdrZPrePass = RenderPassContext(renderZSubList)
                .setPassType(GFX_ZLIST_PASS_ZPREPASS)
                .setReverseOrder()
                .setRenderTargetSize(widthHeight);

            rdrZPrePass.setFarPlaneDist(farClipDist);

            setShaderRepartition(20);
            beginGPUEvent("ZPrePass", GFX_GPUMARKER_ZPass);
            drawRenderPass(rdrZPrePass);
            endGPUEvent(GFX_GPUMARKER_ZPass);
            setShaderRepartition(80);
        }
#endif*/


		if ( pCurrentView->getViewportRotation() != 0.0f )
		{
			renderViewportMask(_view, *pCurrentView);
		}

        // Regular
        {
            RenderPassContext rdrSubPassRegular = RenderPassContext(renderZSubList)
                .setRenderTargetSize(widthHeight);

            rdrSubPassRegular.setFarPlaneDist(farClipDist);

            drawRenderPass(rdrSubPassRegular);
        }

		drawViewFrame(*pCurrentView);
    }

    // Restore camera & Viewport
    _view.setCurrentView();
    _view.apply();
}

void GFXAdapter::drawAfterFX(View &_view)
{
	if(isAsyncScreenshotRequested(AsyncScreenshotRenderStep_BeforePostFX)) 
        renderAsyncScreenshot(_view);

    u32 zListViewID = _view.getZlistID();
    if(zListViewID == View::ZlistID_invalid)
        return;
    if (isShowAfterFx() && !isPickingMode() && !isDisableRenderAfterFx())
    {
        setShaderRepartition(95);
        AFXPostProcess postProcess;
        GFX_Zlist<AFXPostProcess> & afterFxZList = getZListManager().getZlistAt<GFX_ZLIST_AFTERFX>(zListViewID);
        u32 nFx = afterFxZList.getNumberEntry();

        for (u32 i = 0; i < nFx; i++)
        {
            postProcess.mergePostProcess(*afterFxZList.getNodeAt(i).m_primitive);
        }

		if ( postProcess.m_colorSetting.m_use )
			postProcess.m_colorSetting.removeFade();  // Translate color fade parameter in color settings instead. Remove a lerp in the shader and allow to add global color settings easily.

		// Add a global color post effect.
#ifdef GFX_COMMON_POSTEFFECT
		postProcess.m_colorSetting.m_contrastScale += m_addGlobalContrast;
		postProcess.m_colorSetting.m_bright += m_addGlobalBright;
		postProcess.m_colorSetting.m_saturation += m_addGlobalSaturation;
#endif
		if ( ( postProcess.m_colorSetting.m_contrastScale != 1.0f ) || ( postProcess.m_colorSetting.m_saturation != 1.0f ) ||
			 ( postProcess.m_colorSetting.m_bright != 0.0f ) )
		{
			postProcess.m_colorSetting.m_use = true;
			postProcess.m_fadeFactor = 1.0f;
		}

		if (nFx || postProcess.m_colorSetting.m_use)
		{
            //Prepare refraction buffer
            if(postProcess.m_refraction.m_use)
            {
                GFX_ZLIST_PASS_TYPE oldPass = m_curPassType;

                GFX_Zlist<GFXPrimitive> & renderZList = getZListManager().getZlistAt<GFX_ZLIST_MAIN>(zListViewID);
                RenderPassContext rdrPassRefraction = RenderPassContext(renderZList)
                    .setPassType(GFX_ZLIST_PASS_REFRACTION)
                    .setClear(btrue)
                    .setClearColor(Color(0.5f,0.5f,0.5f,0.5f))
                    .setResolveTarget(btrue);                
                if(rdrPassRefraction.needRenderPass(GFX_ZLIST_PASS_REFRACTION))
                    drawRenderPass(rdrPassRefraction);
                else
                    postProcess.m_refraction.m_use = bfalse;
                m_curPassType = oldPass;
            }

            RenderPassContext rdrPassAFX;
            rdrPassAFX.getStateCache().setAlphaRef(1)
                .setDepthTest(bfalse)
                .setDepthWrite(bfalse);
            DrawAFXPostProcess(rdrPassAFX, postProcess);
        }
        afterFxZList.reset();
        setShaderRepartition(80);
    }
}

void GFXAdapter::drawBrightnessAfterFX(View &_view)
{
#ifdef GFX_COMMON_POSTEFFECT
    if(_view.getZlistID() == View::ZlistID_invalid)
        return;
		
    if (isShowAfterFx() && !isPickingMode())
    {
        setShaderRepartition(95);
        if (!F32_ALMOST_EQUALS(m_addSceneAndUIBright, 0.0f, MTH_EPSILON))
        {
            AFXPostProcess postProcess;
            postProcess.m_colorSetting.m_bright += m_addSceneAndUIBright;
            postProcess.m_colorSetting.m_use = true;
            postProcess.m_fadeFactor = 1.0f;
            RenderPassContext rdrPassAFX;
            rdrPassAFX.getStateCache().setAlphaRef(1)
                .setDepthTest(bfalse)
                .setDepthWrite(bfalse);
            DrawAFXPostProcess(rdrPassAFX, postProcess);
        }
        setShaderRepartition(80);
    }
#endif //GFX_COMMON_POSTEFFECT
}

void GFXAdapter::DebugCopyZListPass()
{
    if (m_debugZListPass == GFX_ZLIST_PASS_FRONT_LIGHT
        || m_debugZListPass == GFX_ZLIST_PASS_BACK_LIGHT
        || m_debugZListPass == GFX_ZLIST_PASS_REFLECTION
        || m_debugZListPass == GFX_ZLIST_PASS_FLUID)
    {
        if (!m_zlistPassObjRef[m_debugZListPass].isValid())
        {
            copyPassToDebugRenderTarget((GFX_ZLIST_PASS_TYPE)m_debugZListPass);
        }
        copyDebugRenderTarget();
    }
#ifdef ITF_PICKING_SUPPORT
    if (isPickingDebugActive())
    {
        // transparent, offset to middle of the screen
        copyDebugRenderTarget(0.8f, Vec2d(0, f32(getScreenHeight()/2)));
    }
#endif // ITF_PICKING_SUPPORT
}

void GFXAdapter::drawPrimitives2d( View &_view )
{
    u32 zListViewID = _view.getZlistID();
    if(zListViewID == View::ZlistID_invalid)
        return;

    GFX_ZLIST_PASS_TYPE oldPass = m_curPassType;
    m_curPassType = GFX_ZLIST_PASS_2D;

#ifdef ITF_USE_REMOTEVIEW
    f32 width = f32(_view.getViewport().m_width);
    f32 height= f32(_view.getViewport().m_height);

    if(!_view.isMainView())
    {
        if (getWindowWidth()*9 > 16*getWindowHeight())
        {
            height = f32(getWindowHeight());
            width  = height*16.f/9.f;
        }
        else
        {
            width  = f32(getWindowWidth());
            height = width*9.f/16.f;
        }
        // Adjust ortho view because 2d data are constructed with main screen size
        setOrthoView(0.f, width, 0.f, height);
    }

    // build (width, height, 0, 0) Float4
    Float4 widthHeight = loadXYZW(width, height, 0.f, 0.f);
#else // ITF_USE_REMOTEVIEW
    // build (width, height, 0, 0) Float4
    Float4 widthHeight = loadXYZW(f32(_view.getViewport().m_width), f32(_view.getViewport().m_height), 0.f, 0.f);
#endif // ITF_USE_REMOTEVIEW

#ifndef ITF_FINAL
    GFX_ADAPTER->startRasterForPass(GFX_ZLIST_PASS_2D);
#endif // ITF_FINAL

    /// Draw all containers.
    for(i32 count= 0; count<ZlistPriorityCount2d; ++count)
    {
        GFX_Zlist<GFXPrimitive> & renderZList = getZListManager().get2DZlistAt(zListViewID, count);
        RenderPassContext rdr2DPass = RenderPassContext(renderZList)
                                                        .setPassType(GFX_ZLIST_PASS_2D)
                                                        .setRenderTargetSize(widthHeight);

#if defined(ITF_IOS) || defined(ITF_ANDROID)
        // Force pass to write in alpha. (Video blend...)
        if ( GFX_ADAPTER->isForceWriteAlpha() )
        	rdr2DPass.getStateCache().setAlphaWrite(true);
#endif
		rdr2DPass.getStateCache().setDepthTest(bfalse);
        GFX_ADAPTER->SetDrawCallState(rdr2DPass.getStateCache(), 0xFFffFFff, btrue);

        for (ux i = 0; i < renderZList.getNumberEntry(); i++)
        {
            const ZList_Node_Prim & curNode = renderZList.getNodeAt(i);

            debugDrawStateScope _(this);

            {
                PROFILER_DRAWZLISTNODE(curNode.m_ref);
                PROFILER_GPU(curNode.m_ref,GFX_ZLIST_PASS_2D);
                curNode.m_primitive->directDraw(rdr2DPass, this, 0.0f, 0.0f, 0.0f);
            }
        }
        renderZList.reset();
    }

    m_screenTransitionManager.render();

#ifndef ITF_FINAL
    GFX_ADAPTER->endRasterForPass(GFX_ZLIST_PASS_2D);
#endif // ITF_FINAL
    m_curPassType = oldPass;
}

void GFXAdapter::advancePickingToNextPixel()
{
#ifdef ITF_PICKING_SUPPORT
    ITF_ASSERT(m_bPickingMode);
    m_deltaPicking.x() += f32(NbPickingPixelsPerObject);
    ux pickingWidth = getPickingWidth();
    ITF_ASSERT((pickingWidth / NbPickingPixelsPerObject) * NbPickingPixelsPerObject == pickingWidth); // is a multiple
    if (m_deltaPicking.x()>=pickingWidth)
    {
        m_deltaPicking.x() = 0;
        m_deltaPicking.y() += f32(NbPickingPixelsPerObject);
        ITF_ASSERT(m_deltaPicking.y()<getPickingHeight()); // out of pixels for picking
        m_pickingScissor.left = 0;
        m_pickingScissor.right = NbPickingPixelsPerObject;
        m_pickingScissor.top += NbPickingPixelsPerObject;
        m_pickingScissor.bottom += NbPickingPixelsPerObject;
    }
    else
    {
        m_pickingScissor.left += NbPickingPixelsPerObject;
        m_pickingScissor.right += NbPickingPixelsPerObject;
    }

    setScissorRect(&m_pickingScissor);
    // force alpha test (could have been changed by drawcall)
    //setAlphaRef(int(m_alphaPickRef));
    //setAlphaTest(btrue);
#endif
}


void GFXAdapter::vec3dProject(Vec3d* _out, const Vec3d* _v, const GFX_Viewport* _viewp, const Matrix44* _ViewProj)
{
    Float4 v_in = loadXYZW(_v->x(), _v->y(), _v->z(), 1.f);

    Float4 v_out = _ViewProj->transformPoint(v_in);

    GFX_Vector4 getv_out;
    storeXYZW((f32*)&getv_out,v_out);

    const f32 invbw = (getv_out.m_w == 0.f) ? 0.f : 1.f/getv_out.m_w;
    getv_out.x() *= invbw;
    getv_out.y() *= invbw;
    getv_out.z() *= invbw;

    _out->x() = _viewp->m_x + _viewp->m_width  * (1.f + getv_out.m_x) / 2.f;
    _out->y() = _viewp->m_y + _viewp->m_height * (1.f - getv_out.m_y) / 2.f;
    _out->z() = _viewp->m_minZ + getv_out.z() * (_viewp->m_maxZ - _viewp->m_minZ);
}

void GFXAdapter::vec3dUnProject(Vec3d* _out, const Vec3d* _v, const GFX_Viewport* _viewp, const Matrix44* _ViewProj_InvT)
{
	Float4 v_in;
	if ( (_viewp->m_width == 0) || (_viewp->m_height == 0) )
		v_in = loadXYZW(0.0f, 0.0f, (_v->z() - _viewp->m_minZ) / (_viewp->m_maxZ - _viewp->m_minZ), 1.0f );
	else
		v_in = loadXYZW( - 1.0f + (_v->x() - _viewp->m_x) * 2.f / _viewp->m_width,
			             1.0f - (_v->y() - _viewp->m_y) * 2.f / _viewp->m_height,
			            (_v->z() - _viewp->m_minZ) / (_viewp->m_maxZ - _viewp->m_minZ), 1.0f );

    Float4 v_out = _ViewProj_InvT->transformPoint(v_in);

    GFX_Vector4 getv_out;
    storeXYZW((f32*)&getv_out,v_out);

    const f32 invbw = (getv_out.m_w == 0.f) ? 0.f : 1.f/getv_out.m_w;
    _out->x() = getv_out.x() * invbw;
    _out->y() = getv_out.y() * invbw;
    _out->z() = getv_out.z() * invbw;
}

#ifdef ITF_SUPPORT_DBGPRIM_TEXT
bbool GFXAdapter::drawDBGText( const String8 & _text, float _x, float _y, f32 _r, f32 _g, f32 _b, bbool _useBigFont, FONT_ALIGNMENT _alignment, bbool _useMonospaceFont, Color _backgroundColor, f32 _w, f32 _h)
{
    if (hasHideFlags(HideFlag_DBGText))
        return bfalse;

    DBGText txt;
	txt.m_text = _text;

#ifdef ITF_PC
	txt.m_x = _x + getViewPortOffsetX();
	txt.m_y = _y + getViewPortOffsetY();
#else //ITF_PC
	txt.m_x = _x;
	txt.m_y = _y;
#endif //ITF_PC

    txt.m_r = _r;
    txt.m_g = _g;
    txt.m_b = _b;
    txt.m_alignment = _alignment;
    txt.m_useBigFont = _useBigFont;
	txt.m_useMonospaceFont = _useMonospaceFont;
    txt.m_backgroundColor = _backgroundColor;

    if (_x < (ITF_POS2D_AUTO+0.1f))
    {
#ifdef ITF_PC
        txt.m_x = (f32)getViewPortOffsetX();
#else //ITF_PC
        txt.m_x = 0;
#endif //ITF_PC
    }

    if (_w < 0.f)
        txt.m_w = getScreenWidth() - txt.m_x;
    else
        txt.m_w = _w;

    if (_h < 0.f)
        txt.m_h = 20;
    else
        txt.m_h = _h;

    if (_y < (ITF_POS2D_AUTO+0.1f))
    {
        txt.m_y = 0;
        if(_alignment == FONT_ALIGN_LEFT)
        {
            m_DBGLeftStackedTexts.push_back(txt);
            return btrue;
        }
        else if(_alignment == FONT_ALIGN_RIGHT)
        {
            m_DBGRightStackedTexts.push_back(txt);
            return btrue;
        }
    }


    m_DBGTexts.push_back(txt);
    return btrue;
}

bbool GFXAdapter::drawDBGTextFixed( const String8 & _text, float _x, float _y, f32 _r, f32 _g, f32 _b, FONT_ALIGNMENT _alignment )
{
    return drawDBGText(_text, _x, _y, _r, _g, _b, bfalse, _alignment, btrue);
}
#endif // ITF_SUPPORT_DBGPRIM_TEXT

void GFXAdapter::drawFontText(const ITF_VECTOR <class View*>& _views, FontText* _fontText, f32 _depthOffset, const ConstObjectRef & _objectRef)
{
#ifndef ITF_FINAL
    if (hasHideFlags(HideFlag_Font))
        return;
#endif

    if (_fontText->is2D())
        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_2D>(_views, _fontText, _fontText->getPosition().z() + _depthOffset, _objectRef);
    else
        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, _fontText, _fontText->getPosition().z() + _depthOffset, _objectRef);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef ITF_FINAL
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GFXAdapter::toggleRaster()
{
#ifdef RASTER_ENABLED
    if (!m_showRasters)
    {
        RastersManager& rasterManager = RastersManager::get();
        if (!rasterManager.isHistoryEnabled())
        {
            rasterManager.setEnableHistory(btrue);
            rasterManager.setEnable(btrue);
        }
    }

    m_showRasters = !m_showRasters; 
#endif //RASTER_ENABLED
};
#endif //!ITF_FINAL

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ITF_VertexBuffer* GFXAdapter::createVertexBuffer(u32 _vertexNumber, int _vertexformat, u32 _structVertexSize, bbool _dynamic, dbEnumType _type )
{
    return m_VertexBufferManager.createVertexBuffer(_vertexNumber,  _vertexformat, _structVertexSize, _dynamic, _type );
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ITF_IndexBuffer* GFXAdapter::createIndexBuffer(u32 _indexNumber, bbool _dynamic)
{
    return m_VertexBufferManager.createIndexBuffer(_indexNumber, _dynamic);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GFXAdapter::removeVertexBuffer(ITF_VertexBuffer* _VertexBuffer)
{
    if (_VertexBuffer)
        m_VertexBufferManager.removeVertexBuffer(_VertexBuffer);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GFXAdapter::removeIndexBuffer(ITF_IndexBuffer* _IndexBuffer)
{
    if (_IndexBuffer)
        m_VertexBufferManager.removeIndexBuffer(_IndexBuffer);
}

void GFXAdapter::removeVertexBufferDelay(   ITF_VertexBuffer* _VertexBuffer ,u32 _frameCount  )
{
    if (_VertexBuffer)
        m_VertexBufferManager.removeVertexBufferDelay(_VertexBuffer,_frameCount);
}

void GFXAdapter::removeIndexBufferDelay (   ITF_IndexBuffer* _IndexBuffer ,u32 _frameCount  )
{
    if (_IndexBuffer)
        m_VertexBufferManager.removeIndexBufferDelay(_IndexBuffer,_frameCount);
}

void GFXAdapter::cleanBufferEndOfFrame()
{
    m_VertexBufferManager.cleanEndOfFrame();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


Patch32Data::Patch32Data(int _vertexFormat, u32 _maxVertex)
{
    ITF_MemSet(this, 0, sizeof(*this));
    switch (_vertexFormat)
    {
    case VertexFormat_PCT:
#if PATCH32_WORKINGBUFF != 0
        vertexListPCT       = newAlloc(mId_Vertex,VertexPCT[_maxVertex]);
#endif // PATCH32_WORKINGBUFF
        vertexFormat        = VertexFormat_PCT;
        break;

    default:
#if PATCH32_WORKINGBUFF != 0
        vertexListPT        = newAlloc(mId_Vertex,VertexPT[_maxVertex]);
#endif // PATCH32_WORKINGBUFF
        vertexFormat        = VertexFormat_PTa;
        break;
    }

    maxVertexListSize   = _maxVertex;
}

Patch32Data::~Patch32Data()
{
    switch (vertexFormat)
    {
    case VertexFormat_PCT:
#if PATCH32_WORKINGBUFF != 0
        SF_DEL_ARRAY(vertexListPCT);
#endif // PATCH32_WORKINGBUFF
        break;

    default:
#if PATCH32_WORKINGBUFF != 0
        SF_DEL_ARRAY(vertexListPT);
#endif // PATCH32_WORKINGBUFF
        break;
    }
}

void Patch32Data::init(ITF_Mesh    *_mesh, int _vertexFormat)
{
    vertexFormat                                        = _vertexFormat;
    vertexCount                                         = 0;
    mesh                                                = _mesh;
    ITF_MeshElement & meshElt = mesh->getMeshElement();
    meshElt.m_startVertex                 = 0;
    meshElt.m_count                      = 0;
    paused                                              = bfalse;
}

void Patch32Data::start()
{
#if PATCH32_WORKINGBUFF == 0
    mesh->swapVBForDynamicMesh();
    mesh->LockVertexBuffer(&vertexList);
#endif
}

void Patch32Data::pause(bbool _pause)
{
#if PATCH32_WORKINGBUFF == 0
#ifdef VBMANAGER_USE_DYNAMICRING_VB
    ITF_ASSERT_MSG(0, "No pause with Ring Buffer !!!");
    return;
#else
    ITF_VertexBuffer *vtxBuffer = mesh->getCurrentVB();

    if (_pause && !paused)
    {
        mesh->UnlockVertexBufferUsage(vertexCount * vtxBuffer->m_structVertexSize);
    } else if (!_pause && paused)
    {
        mesh->LockVertexBuffer(&vertexList);
    }
    paused = _pause;
#endif // VBMANAGER_USE_DYNAMICRING_VB
#endif
}

void Patch32Data::stop()
{
    ITF_VertexBuffer *vtxBuffer = mesh->getCurrentVB();

#if PATCH32_WORKINGBUFF != 0
    void *pdata;
    GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(vtxBuffer,&pdata);   
    vtxBuffer->wgp_write(vertexList, vertexCount * vtxBuffer->m_structVertexSize);
#endif
    if (!paused)
        GFX_ADAPTER->getVertexBufferManager().UnlockVertexBufferUsage(vtxBuffer, vertexCount * vtxBuffer->m_structVertexSize);
}

// Use max size to be compatible with software use
ITF_IndexBuffer* GFXAdapter::createPatchGridInternalIB(u32 _hdiv, u32 _vdiv)
{
    u32 nbElements = _hdiv <= 2 && _vdiv <= 2 ? 1024 : NB_ELEMENT_IN_VERTEX_BUFFER;
    ITF_IndexBuffer* internalIdxBuffer = createIndexBuffer(  (_hdiv*_vdiv*6)*nbElements, bfalse);

    u16        *idxBuff;
    internalIdxBuffer->Lock((void **) &idxBuff);
    u16        * __restrict idx = idxBuff;
    u16 idxPos  = 0;
    u16 idxLine = (u16)(_hdiv+1);

    for (u32 k = 0 ; k < nbElements; k++)
    {
        for (u32 y=0; y<_vdiv; y++)
        {
            for (u32 x=0; x<_hdiv; x++, idxPos++)
            {
                *(idx++) = idxPos;
                *(idx++) = idxPos + 1;
                *(idx++) = idxPos + idxLine + 1;
                *(idx++) = idxPos;
                *(idx++) = idxPos + idxLine + 1;
                *(idx++) = idxPos + idxLine;
            }
            idxPos++;
        }
        idxPos += idxLine;
    }
    internalIdxBuffer->Unlock();
    return internalIdxBuffer;
}

ITF_VertexBuffer* GFXAdapter::createPatchGridInternalVB(u32 _hdiv, u32 _vdiv)
{
    ITF_VertexBuffer* internalVtxBuffer = createVertexBuffer(  ((_hdiv + 1)*(_vdiv + 1))*NB_ELEMENT_IN_VERTEX_BUFFER,
    VertexFormat_PT, sizeof(VertexPT),bfalse, VB_T_INTERNAL);
    
    VertexPT    *pdata;
    internalVtxBuffer->Lock((void **) &pdata);
    VertexPT    * __restrict write = pdata;

    f32 divx = 1.f / _hdiv;
    f32 divy = 1.f / _vdiv;
    f32 UVu = 0.f;
    f32 UVv = 0.f;
    for (u32 k = 0 ; k < NB_ELEMENT_IN_VERTEX_BUFFER ; k++)
    {
        UVv = 0.f;
        for (u32 j = 0 ; j < (_vdiv + 1); j++)
        {
            UVu = 0.f;
            for (u32 i = 0 ; i < (_hdiv + 1) ; i++)
            {
                write->m_pos.x()    = UVu;
                write->m_pos.y()    = UVv;
                write->m_pos.z()    = (f32)k;
                write->m_uv.x()     = UVu;
                write->m_uv.y()     = UVv;
                UVu  += divx;
                write++;
            }
            UVv += divy;
        }
    }
    internalVtxBuffer->Unlock();
    return internalVtxBuffer;
}

void GFXAdapter::createPatchGridInternalBuffers()
{
    for (u32 i=0; i<patchGridBufferTypesNb; i++)
    {
        m_internalIdxBufferTab[i] = createPatchGridInternalIB(patchGridBufferTypes[i][0], patchGridBufferTypes[i][1]);
        m_internalVtxBufferTab[i] = createPatchGridInternalVB(patchGridBufferTypes[i][0], patchGridBufferTypes[i][1]);
    }
}

void GFXAdapter::removePatchGridInternalBuffers()
{
    for (u32 i=0; i<patchGridBufferTypesNb; i++)
    {
        if (m_internalIdxBufferTab && m_internalIdxBufferTab[i])
        {
            removeIndexBuffer(m_internalIdxBufferTab[i]);
            m_internalIdxBufferTab[i] = NULL;
        }
        if (m_internalVtxBufferTab && m_internalVtxBufferTab[i])
        {
            removeVertexBuffer(m_internalVtxBufferTab[i]);
            m_internalVtxBufferTab[i] = NULL;
        }
    }
}

ITF_IndexBuffer*    GFXAdapter::getPatchGridIndexBuffer(u32 & _hdiv, u32 & _vdiv)
{
    for (u32 i=0; i<patchGridBufferTypesNb; i++)
    {
        if (patchGridBufferTypes[i][0] == _hdiv && patchGridBufferTypes[i][1] == _vdiv)
        {
            return m_internalIdxBufferTab[i];
        }
    }
    ITF_ASSERT(0);
    _hdiv = patchGridBufferTypes[0][0];
    _vdiv = patchGridBufferTypes[0][1];
    return m_internalIdxBufferTab[0];
}

ITF_VertexBuffer*   GFXAdapter::getPatchGridVertexBuffer(u32 & _hdiv, u32 & _vdiv)
{
    for (u32 i=0; i<patchGridBufferTypesNb; i++)
    {
        if (patchGridBufferTypes[i][0] == _hdiv && patchGridBufferTypes[i][1] == _vdiv)
        {
            return m_internalVtxBufferTab[i];
        }
    }
    ITF_ASSERT(0);
    _hdiv = patchGridBufferTypes[0][0];
    _vdiv = patchGridBufferTypes[0][1];
    return m_internalVtxBufferTab[0];
}

void FillSpriteIndexBuffer(ITF_IndexBuffer* _indexbuffer, u32 _numberSprite)
{
    /// Fill Element index Buffer.
    u32 i;
    u16* index;
    _indexbuffer->Lock((void**)&index);
    u16 vertex = 0;

    for (i = 0; i < _numberSprite ; i++)
    {
        index[i * 6 + 0] = vertex + 0;
        index[i * 6 + 1] = vertex + 1;
        index[i * 6 + 2] = vertex + 2;
        index[i * 6 + 3] = vertex + 2;
        index[i * 6 + 4] = vertex + 3;
        index[i * 6 + 5] = vertex + 0;
        vertex += 4;
    }

    _indexbuffer->Unlock();
}

void FillSpriteVertexBuffer(ITF_VertexBuffer* _vertexbuffer)
{
    /// Fill Element index Buffer.
    f32* f;
    _vertexbuffer->Lock((void**)&f);

    f[0] = -1.0f;
    f[1] = -1.0f;

    f[2] = -1.0f;
    f[3] = 1.0f;

    f[4] = 1.0f;
    f[5] = 1.0f;

    f[6] = 1.0f;
    f[7] = -1.0f;

    _vertexbuffer->Unlock();
}

void FillSpriteIndexAsVertexBuffer(ITF_VertexBuffer* _vertexbuffer)
{
    /// Fill Element index Buffer.
    u32* u;
    _vertexbuffer->Lock((void**)&u);

    u[0] = 0;
    u[1] = 1;

    u[2] = 2;
    u[3] = 2;

    u[4] = 3;
    u[5] = 0;

    _vertexbuffer->Unlock();
}
ITF_IndexBuffer* GFXAdapter::needSpriteIndexBuffer(u32 _numberIndex)
{
    /// Create.
    const u32 cMaxInternalIndexBuffer = 32768;
    if (!m_internalSpritesIndexBuffer)
    {
        m_internalSpritesIndexBuffer = createIndexBuffer(  cMaxInternalIndexBuffer, bfalse);
        m_spriteIndexBufferSize = cMaxInternalIndexBuffer;
        FillSpriteIndexBuffer(m_internalSpritesIndexBuffer, cMaxInternalIndexBuffer / 6);
        return m_internalSpritesIndexBuffer;
    }
    
    /// Resize.
    if ( _numberIndex > m_spriteIndexBufferSize )
    {
        ITF_ASSERT_CRASH(0,"resizing the sprite index buffer is not allowed");
    }

    return m_internalSpritesIndexBuffer;
}

ITF_VertexBuffer* GFXAdapter::needSpriteInstancingVertexBuffer()
{
    /// Create.
//    const u32 cSpriteInstancingSize = 4;
    if (!m_internalSpritesInstancingVertexBuffer)
    {
        m_internalSpritesInstancingVertexBuffer = createVertexBuffer(  4, VertexFormat_T, sizeof(VertexT), vbLockType_static, VB_T_INTERNAL);
        FillSpriteVertexBuffer(m_internalSpritesInstancingVertexBuffer);
    }
        return m_internalSpritesInstancingVertexBuffer;
    }

ITF_VertexBuffer* GFXAdapter::needSpriteInstancingIndexAsVertexBuffer()
{
    /// Create.
//    const u32 cSpriteInstancingSize = 6;
    if (!m_internalSpritesInstancingIndexAsVertexBuffer)
    {
        m_internalSpritesInstancingIndexAsVertexBuffer = createVertexBuffer(  6, VertexFormat_I, sizeof(VertexI), vbLockType_static, VB_T_INTERNAL);
        FillSpriteIndexAsVertexBuffer(m_internalSpritesInstancingIndexAsVertexBuffer);
    }
    return m_internalSpritesInstancingIndexAsVertexBuffer;
}

void  GFXAdapter::stopScreenCapture()
{
#ifdef ITF_SUPPORT_VIDEOCAPTURE
    if (m_videoCapture)
        m_videoCapture->stop();
#endif //ITF_SUPPORT_VIDEOCAPTURE
}

void  GFXAdapter::cancelScreenCapture()
{
#ifdef ITF_SUPPORT_VIDEOCAPTURE
    if (m_videoCapture)
        m_videoCapture->cancel();
#endif //ITF_SUPPORT_VIDEOCAPTURE
}

void    GFXAdapter::startRenderPass(RenderPassContext & _rdrCtxt, bbool _restoreContext)
{
    m_renderPassAlgo[_rdrCtxt.getPass()]->begin(_rdrCtxt, _restoreContext);
}

void    GFXAdapter::stopRenderPass(RenderPassContext & _rdrCtxt)
{
    m_renderPassAlgo[_rdrCtxt.getPass()]->end(_rdrCtxt, _rdrCtxt.getResolveTarget());
}

void    GFXAdapter::unpauseRenderPass(RenderPassContext & _rdrCtxt, bbool _restoreContext)
{
    m_renderPassAlgo[_rdrCtxt.getPass()]->unpause(_rdrCtxt, _restoreContext);
}

void    GFXAdapter::pauseRenderPass(RenderPassContext & _rdrCtxt)
{
    m_renderPassAlgo[_rdrCtxt.getPass()]->pause(_rdrCtxt, _rdrCtxt.getResolveTarget());
}


void GFXAdapter::prerenderPass(u32 _zListViewID)
{
	if ( _zListViewID == (u32)-1 )
		return;

	f32 dt = m_currentDT;
    
	GFX_Zlist<GFXPrimitive> & prerenderList = getZListManager().getZlistAt<GFX_ZLIST_PRERENDER_PRIM>(_zListViewID);
	ux n = prerenderList.getNumberEntry();
	for (ux i = 0; i < n; i++)
    {
        const ZList_Node_Prim & node = prerenderList.getNodeAt(i);
		node.m_primitive->prerendering(this, dt, _zListViewID);
    }
}

void GFXAdapter::drawRenderPass(RenderPassContext & _rdrCtxt)
{
    startRenderPass(_rdrCtxt);

    f32 zCamRef = CAMERA->getPos().z(); // temp -> should go in the render pass context

    u32 passFilterFlag = _rdrCtxt.getPassFilterFlag();
    
    if (_rdrCtxt.getReverseOrder())
    {
        for (ux i = _rdrCtxt.getEndZListIdx(), n = _rdrCtxt.getBeginZListIdx();
            i != n;
            i--)
        {
            _rdrCtxt.setCurrentZListIdx(i-1u);
            const ZList_Node_Prim & node = _rdrCtxt.getNode(i-1u);
            if ( ((node.m_primitive->getPassFilterFlag() & passFilterFlag) != 0) && (node.m_depth > _rdrCtxt.getFarPlaneDist()) )
                drawZListNode(_rdrCtxt, node, zCamRef);
        }
    }
    else
    {
        for (ux i = _rdrCtxt.getBeginZListIdx(), n = _rdrCtxt.getEndZListIdx();
            i < n;
            i++)
        {
            _rdrCtxt.setCurrentZListIdx(i);
            const ZList_Node_Prim & node = _rdrCtxt.getNode(i);
            if ( ((node.m_primitive->getPassFilterFlag() & passFilterFlag) != 0) && (node.m_depth > _rdrCtxt.getFarPlaneDist()) )
                drawZListNode(_rdrCtxt, node, zCamRef);
        }
    }
    stopRenderPass(_rdrCtxt);
}

void GFXAdapter::drawZListNode(const RenderPassContext & _rdrCtxt, const ZList_Node_Prim & _curNode, f32 _zCamRef)
{
    PROFILER_DRAWZLISTNODE(_curNode.m_ref);
    PROFILER_GPU(_curNode.m_ref,m_curPassType);

    f32 drawZWorld = _curNode.m_depth;
    f32 drawZCam = drawZWorld - _zCamRef;// inform the renderer the actual z position in camera space.
    f32 drawZCamAbs = f32_Abs(drawZCam);

    m_currentDrawZWorld = drawZWorld;
    m_currentDrawZCam = drawZCam; 
    m_currentDrawZCamAbs = drawZCamAbs;

    debugDrawStateScope _(this, _curNode.getObjectRef());

    _curNode.m_primitive->directDraw(_rdrCtxt, this, drawZCam, drawZWorld, drawZCamAbs);
}

void GFXAdapter::setClearColor(const Color & _clearColor)
{
    m_clearColor = _clearColor;
    m_clearColor.setAlpha(gAlphaClearColor);
}

i32 GFXAdapter::getDebugFontSize() const
{
/*    if (getScreenWidth() > 1250)
        return 25;*/
    return 15;
}

i32 GFXAdapter::getBigDebugFontSize() const
{
  /*  if (getScreenWidth() > 1250)
        return 30;*/
    return 20;
}

void GFXAdapter::setGFXPrimitiveParameters(const GFXPrimitiveParam* _primitiveParam, const RenderPassContext & _rdrPassCtxt)
{
    bbool useGlobalLighting = _primitiveParam->m_useGlobalLighting;
    m_currentPrimitiveParam = _primitiveParam;
	Color color;
	if ( useGlobalLighting && m_globalColorBlend.getAlpha() )
	{
		color = Color::Interpolate(_primitiveParam->m_colorFactor, m_globalColorBlend, m_globalColorBlend.getAlpha());
		color.setAlpha(_primitiveParam->m_colorFactor.getAlpha());
	}
	else
		color = _primitiveParam->m_colorFactor;

    if ( m_curPassType == GFX_ZLIST_PASS_MASK )
    {
        color.setRed(0.0f);
        color.setGreen(0.0f);
        color.setBlue(0.0f);
    }
	setGlobalColor(color);

	GFX_ZLIST_PASS_TYPE pass =  _rdrPassCtxt.getPass();
    if (pass != GFX_ZLIST_PASS_ZPREPASS && pass != GFX_ZLIST_PASS_2D)
    {
		Color fogColor;
		if ( useGlobalLighting && m_globalStaticFog.getAlpha() )
			fogColor = Color::Interpolate(_primitiveParam->m_colorFog, m_globalStaticFog, m_globalStaticFog.getAlpha());
		else
			fogColor = _primitiveParam->m_colorFog;
        if ( useGlobalLighting )
		    fogColor.setAlpha(_primitiveParam->m_colorFog.getAlpha() + m_globalFogOpacity);
        else
		    fogColor.setAlpha(_primitiveParam->m_colorFog.getAlpha());

        setFogColor(fogColor);
        setUseStaticfog(_primitiveParam->m_useStaticFog);
        if(!_primitiveParam->m_useStaticFog)
            computeFogBoxObjectVisibility(_primitiveParam->m_BV);
    }
    else
    {
        setFogColor(_primitiveParam->m_colorFog);
        setUseStaticfog(btrue);
    }
}

void GFXAdapter::prepareGfxMatAlphaFade(const GFX_MATERIAL& _gfxMat)
{
    Color val = getGFXGlobalColor();
    f32 currentalpha = val.getAlpha();

    f32 dmin = _gfxMat.getMatParams().getfParamsAt(0);
    f32 dmax = _gfxMat.getMatParams().getfParamsAt(1);

    f32 coeff = 1.0f;
    f32 dist = m_currentDrawZCamAbs;

    i32 type = _gfxMat.getMatParams().getiParamsAt(0);

    /// compute pos in range 0 -> 1
    f32 d = dmax - dmin;
    f32 d1 = dist - dmin;
    
    coeff =  1.f - ((d - d1)/d);
       
    switch(type)
    {
    case GFX_MaterialParams::AlphaFade_Distance:
        break;
    case GFX_MaterialParams::AlphaFade_ExpDistance:
    case GFX_MaterialParams::AlphaFade_ExpSquaredDistance:
        {
            f32 density = _gfxMat.getMatParams().getfParamsAt(2);
            f32 dd = 1.f / coeff;
            f32 co = (dd * density);
            if (type == GFX_MaterialParams::AlphaFade_ExpSquaredDistance)
                co *= co;
            coeff =  expf(-co);
        } break;
    case GFX_MaterialParams::AlphaFade_Constant:
            coeff = _gfxMat.getMatParams().getfParamsAt(2);
        break;
    }

    /// clip: do not > original alpha.
    if (coeff > 1.f)
        coeff = 1.f;

    currentalpha *= coeff;

    val.m_a = currentalpha;
    setGlobalColor(val);
}

#ifdef ITF_SUPPORT_DEBUGFEATURE
DBGLine* GFXAdapter::getDBGLineBuffer(   u32 _count )
{
    return &m_DBGLines.incrSize(_count);
}

DBG3DLine* GFXAdapter::getDBG3DLineBuffer(   u32 _count )
{
    return &m_DBG3DLines.incrSize(_count);
}
#endif // ITF_CONSOLE_FINAL

bbool GFXAdapter::bCanRenderToCurrentView(u32 _renderFlags)
{
#ifdef ITF_USE_REMOTEVIEW
    if((getCurrentEngineView() == e_engineView_MainScreen) && (_renderFlags & e_RenderFlag_HideInMainScreen))
    {
        return bfalse;
    }
    else if((getCurrentEngineView() == e_engineView_RemoteScreen) && (_renderFlags & e_RenderFlag_HideInRemoteScreen))
    {
        return bfalse;
    }
#endif
   
    return btrue;
}

void GFXAdapter::hideDrawPass( engineView _view, bbool _hideMainPass, bbool _hide2DPass )
{
    if(_view < e_engineView_Count)
    {
        m_hideMainDrawPass[_view] = _hideMainPass;
        m_hide2DDrawPass[_view] = _hide2DPass;
    }
    else
    {
        m_hideMainDrawPass[e_engineView_MainScreen] = _hideMainPass;
        m_hide2DDrawPass[e_engineView_MainScreen] = _hide2DPass;
        m_hideMainDrawPass[e_engineView_RemoteScreen] = _hideMainPass;
        m_hide2DDrawPass[e_engineView_RemoteScreen] = _hide2DPass;
    }
}

bbool GFXAdapter::getDrawPassHide( engineView _view )
{
    if(_view >= 0 && _view < e_engineView_Count)
    {
        return m_hideMainDrawPass[_view];
    }
    return btrue;
}

void GFXAdapter::freezeMainDrawPass( View &_view, bbool _freeze )
{
    if(_freeze)
    {
        if(_view.getFreezeState() == View::DPFreeze_NONE)
            _view.setFreezeState(View::DPFreeze_COPY_BACKBUFFER);
    }
    else
    {
        _view.setFreezeState(View::DPFreeze_NONE);
    }
}

void  GFXAdapter::extractFrustumPlanes(const Matrix44 & _proj, Camera* _cam)
{
    FrustumDesc frustum = _cam->getViewFrustum();

    // view
    {
        Matrix44 trans;
        trans.transpose(_proj);

        Float4 leftPlane, rightPlane, topPlane, bottomPlane, nearPlane;
        leftPlane = neg(add(trans.T(),trans.I()));
        rightPlane = sub(trans.I(),trans.T());
        bottomPlane = neg(add(trans.T(),trans.J()));
        topPlane = sub(trans.J(),trans.T());
        nearPlane = neg(trans.K());

        rightPlane = normalize3(rightPlane);
        leftPlane = normalize3(leftPlane);
        bottomPlane = normalize3(bottomPlane);
        topPlane = normalize3(topPlane);
        nearPlane = normalize3(nearPlane);

        Plane &left = frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Left];
        Plane &right = frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Right];
        Plane &bottom = frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Bottom];
        Plane &top = frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Top];
        Plane &nearP = frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Near];

        left.loadFromFloat4(leftPlane);
        left.m_constant = -left.m_constant;

        right.loadFromFloat4(rightPlane);
        right.m_constant = -right.m_constant;

        top.loadFromFloat4(topPlane);
        top.m_constant = -top.m_constant;

        bottom.loadFromFloat4(bottomPlane);
        bottom.m_constant = -bottom.m_constant;

        nearP.loadFromFloat4(nearPlane);
        nearP.m_constant = nearP.m_normal.dot(_cam->getPos());

        _cam->setViewFrustum(frustum);
    }

    // update
    {
        frustum.enlarge(_cam->getUpdateFrustumEnlargeRange()); // push planes
        _cam->setUpdateFrustum(frustum);
    }

    // loading
    {
        frustum.enlarge(_cam->getLoadingFrustumEnlargeRange() - _cam->getUpdateFrustumEnlargeRange()); // push planes
        _cam->setLoadingFrustum(frustum);
    }
}

bbool GFXAdapter::isAsyncScreenshotDataAvailable() const
{
    ITF_ASSERT(m_asyncScreenshotState.m_requested);
    return m_asyncScreenshotState.m_renderPushedFrame != 0 && CURRENTFRAME >= m_asyncScreenshotState.m_renderPushedFrame+2u; // double buffer
}

bbool GFXAdapter::askAsyncScreenshotData(GFX_AsyncScreenShotResultData & _result)
{
    if(!isAsyncScreenshotDataAvailable())
        return bfalse;

    ux width = m_asyncScreenshotState.m_config.m_width;
    ux height = m_asyncScreenshotState.m_config.m_height;

    _result.allocateBuffer(width, height);

    copyScreenshotSurfaceToRGBABuffer(_result.getImgBuffer(), width, height);
    releaseScreenshotSurface();

    m_asyncScreenshotState.m_requested = bfalse;
    m_asyncScreenshotState.m_renderPushedFrame = 0;

    return btrue;
}

bbool GFXAdapter::isAsyncScreenshotRequested(AsyncScreenshotRenderStep _rdrStep) const
{
	if (m_asyncScreenshotState.m_requested
        && m_asyncScreenshotState.m_renderPushedFrame == 0
        #ifdef ITF_USE_REMOTEVIEW
        && u32(m_asyncScreenshotState.m_config.m_engineView) == getCurrentEngineView()
        #endif
        )
	{
        return (m_asyncScreenshotState.m_config.m_rdrStep == _rdrStep);
	}
	return bfalse;
}

void GFXAdapter::renderAsyncScreenshot(View &_view)
{
    u32 zListViewID = _view.getZlistID();
    if(zListViewID == View::ZlistID_invalid)
        return;

    // ask platform specific copy of the current rt to a screenshot buffer
    ux width = m_asyncScreenshotState.m_config.m_width;
    ux height = m_asyncScreenshotState.m_config.m_height;

    init2DRender();

    setOrthoView(0.f, f32(width), 0.f, f32(height));

    // build (width, height, 0, 0) Float4
    Float4 widthHeight = loadXYZW(f32(width), f32(height), 0.f, 0.f);

    GFX_Zlist<GFXPrimitive> & zlistScreenShot = getZListManager().getZlistAt<GFX_ZLIST_SCREENSHOT_2D>(zListViewID);

    RenderPassContext rdrScreenshot2DPass = RenderPassContext(zlistScreenShot)
                                                .setPassType(GFX_ZLIST_PASS_SCREENSHOT_2D)
                                                .setRenderTargetSize(widthHeight);

    copyRenderTargetToScreenshot(rdrScreenshot2DPass, width, height);

    drawRenderPass(rdrScreenshot2DPass);
    zlistScreenShot.reset();

    end2DRender();
    m_asyncScreenshotState.m_renderPushedFrame = CURRENTFRAME;

    // As 2D is not setting the render target, we put everything back in place
    setRenderTargetForPass(GFX_ZLIST_PASS_REGULAR);
}

bbool GFXAdapter::requestAsyncScreenshot(const AsyncScreenshotConfig & _config)
{
	if(m_asyncScreenshotState.m_requested)
		return bfalse;

	m_asyncScreenshotState.m_requested = btrue;
	m_asyncScreenshotState.m_renderPushedFrame = 0;
	m_asyncScreenshotState.m_config = _config;	
	return btrue;
}


void GFXAdapter::computeUsingZPrepass()
{
    bbool useZPrepass = m_useZPrepassByDefault;
#ifndef ITF_GLES2
    View *currentView = View::getCurrentView();
    ITF_ASSERT(currentView);
    GFXViewZPassOverride zor = currentView->getZPassOverride();
    if ( zor != GFX_VIEW_ZPASS_DEFAULT )
    {
        if ( zor == GFX_VIEW_ZPASS_FORCE_NO_ZPASS )
            useZPrepass = bfalse;
        else if ( zor == GFX_VIEW_ZPASS_FORCE_USE_ZPASS )
            useZPrepass = btrue;
    }
    const SubRenderParam_Misc& rpMisc = static_cast<const SubRenderParam_Misc&>(m_renderParamManager->getCurrentSubRenderParam(RPType_Misc));
    zor = rpMisc.m_ZPassOverride;
    if ( zor != GFX_VIEW_ZPASS_DEFAULT )
    {
        if ( zor == GFX_VIEW_ZPASS_FORCE_NO_ZPASS )
            useZPrepass = bfalse;
        else if ( zor == GFX_VIEW_ZPASS_FORCE_USE_ZPASS )
            useZPrepass = btrue;
    }
#endif

    m_useZPrepassCurrently = useZPrepass && ( m_overDrawMode == 0 ) && !isPickingMode();
}

#ifdef ITF_USE_REMOTEVIEW
void GFXAdapter::setDisplayingView(engineView _view)
{
    if(_view!=e_engineView_None)
    {
        View* pMainView = View::getMainView();
        View* pRemoteView = View::getRemoteView();

        if(_view == e_engineView_MainScreen)
        {
            pMainView->setCurrentView();
            #if defined(ITF_SUPPORT_EDITOR) || defined(CAFE_CHEAT_SUPPORT)
            pRemoteView->setDisableRendering(btrue);//the remote view is not visible ,juste disable it
            #endif
        }
        else if( _view == e_engineView_Both)
        {
            pMainView->setCurrentView();
            #if defined(ITF_SUPPORT_EDITOR) || defined(CAFE_CHEAT_SUPPORT)
            pRemoteView->setDisableRendering(bfalse);
            #endif
        }
        else
        {
            pRemoteView->setCurrentView();
            #if defined(ITF_SUPPORT_EDITOR) || defined(CAFE_CHEAT_SUPPORT)
            pRemoteView->setDisableRendering(bfalse);
            #endif
        }

        GFX_ADAPTER->setCurrentDisplayingView(_view);
    }
}
#endif

void GFXAdapter::createTextureMonoChrome(Texture & _tex, const Color & _color)
{
    const u32 minimalWidth = 16;
    const u32 minimalHeight = 16; 
    const u32 colorAsInt = _color.getAsU32();

    Size size( minimalWidth, minimalHeight);

    setTextureSize(&_tex, size);
    u32* texData = (u32 *)Memory::alignedMalloc(minimalWidth*minimalHeight*sizeof(u32), 32);
    for(ux i = 0; i<minimalHeight; i++)
    {
        for(ux j = 0; j<minimalWidth; j++)
        {
            texData[j+i*minimalWidth] = colorAsInt;
        }
    }
    loadFromMemory((void*)texData, size, &_tex, Texture::PF_RGBA, bfalse, 1);
    Memory::alignedFree(texData);
}

void GFXAdapter::createGenericTextures()
{
    if (!m_whiteOpaqueTexture)
    {
        m_whiteOpaqueTexture = static_cast<Texture *>(RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture));
        createTextureMonoChrome(*m_whiteOpaqueTexture, Color(1.f, 1.f, 1.f, 1.f));
    }
    if (!m_blackOpaqueTexture)
    {
        m_blackOpaqueTexture = static_cast<Texture *>(RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture));
        createTextureMonoChrome(*m_blackOpaqueTexture, Color(1.f, 0.f, 0.f, 0.f));
    }
    if (!m_blackTranspTexture)
    {
        m_blackTranspTexture = static_cast<Texture *>(RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture));
        createTextureMonoChrome(*m_blackTranspTexture, Color(0.f, 0.f, 0.f, 0.f));
    }
    if (!m_greyOpaqueTexture)
    {
        m_greyOpaqueTexture = static_cast<Texture *>(RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture));
        createTextureMonoChrome(*m_greyOpaqueTexture, Color(1.f, 0.5f, 0.5f, 0.5f));
    }
}

void GFXAdapter::releaseGenericTextures()
{
    if (m_whiteOpaqueTexture)
    {
        cleanupTexture(m_whiteOpaqueTexture);
        RESOURCE_MANAGER->releaseEmptyResource(m_whiteOpaqueTexture);
        m_whiteOpaqueTexture = NULL;
    }
    if (m_blackOpaqueTexture)
    {
        cleanupTexture(m_blackOpaqueTexture);
        RESOURCE_MANAGER->releaseEmptyResource(m_blackOpaqueTexture);
        m_blackOpaqueTexture = NULL;
    }
    if (m_blackTranspTexture)
    {
        cleanupTexture(m_blackTranspTexture);
        RESOURCE_MANAGER->releaseEmptyResource(m_blackTranspTexture);
        m_blackTranspTexture = NULL;
    }
    if (m_greyOpaqueTexture)
    {
        cleanupTexture(m_greyOpaqueTexture);
        RESOURCE_MANAGER->releaseEmptyResource(m_greyOpaqueTexture);
        m_greyOpaqueTexture = NULL;
    }
}

#ifndef ITF_FINAL
ITF::String8 GFXAdapter::m_screenshotMapTilerPath;
u32 GFXAdapter::m_screenshotMapTilerSize = 0;
#endif

// just a helper
RenderTarget* GFXAdapter::createTarget(GFXAdapter *_gfxAdapter, u32 _width, u32 height, Texture::PixFormat _pixformat, u32 _flags, u32 _shiftSurfSize, RenderTarget *_pSrcBaseTexture, RenderTarget *_pLinkedTexture )
{
	RenderTargetInfo rtInfo(_width, height, 1, _pixformat);
	rtInfo.m_EDRAMOffset = _shiftSurfSize;
	rtInfo.m_srcBaseRT = _pSrcBaseTexture;
	rtInfo.m_linkedRT = _pLinkedTexture;
	return _gfxAdapter->createRenderTarget(rtInfo);
}

#ifdef GFX_COMMON_POSTEFFECT

void GFXAdapter::getBackBufferAfterOffset(u32 &_afterBackEDRAMOffset, u32 &_afterLightEDRAMOffset)
{
    _afterBackEDRAMOffset = 0;
    _afterLightEDRAMOffset = 0;
}

void GFXAdapter::createAdditionalRenderTarget()
{
	m_AdditionnalRenderingBuffer[TEX_BFR_FULL_1] = createTarget(this, getScreenWidth(), getScreenHeight(), Texture::PF_RGBA);
#if defined( ITF_WINDOWS ) || defined( ITF_DURANGO )
	m_AdditionnalRenderingBuffer[TEX_BFR_FULL_2] = createTarget(this, getScreenWidth(), getScreenHeight(), Texture::PF_RGBA);
#endif
	m_AdditionnalRenderingBuffer[TEX_BFR_QRT_1] = createTarget(this, getScreenWidth()/2, getScreenHeight()/2, Texture::PF_RGBA);

    u32 afterBackEDRAMOffset;
    u32 afterLightEDRAMOffset;
	getBackBufferAfterOffset(afterBackEDRAMOffset, afterLightEDRAMOffset);

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

    afterBackEDRAMOffset = getBufferSizeOffset(m_AdditionnalRenderingBuffer[TEX_BFR_REFLECTION]);

	m_MediumZBuffer = createTarget( this, getReflectionPassWidth(), getReflectionPassHeight(), Texture::PF_D24S8, 0, afterBackEDRAMOffset);
}

void GFXAdapter::releaseAdditionalRenderTarget()
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

u32 GFXAdapter::getViewPortOffsetX() const
{
	View* mainView = View::getMainView();

	return mainView ? ((getWindowWidth() - static_cast<i32>(mainView->getScreenSizeX())) >> 1) : 0;
}

u32 GFXAdapter::getViewPortOffsetY() const
{
	View* mainView = View::getMainView();

	return mainView ? ((getWindowHeight() - static_cast<i32>(mainView->getScreenSizeY())) >> 1) : 0;
}

#endif

bbool GFXAdapter::getDisplayDebugStats()
{
    return m_displaydebugstats && !CONFIG->m_hideDebugInfo;
}

} // namespace ITF
