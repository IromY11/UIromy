#ifndef ITF_GFX_CONTEXT_H_
#define ITF_GFX_CONTEXT_H_

#ifndef _ITF_GFX_ADAPTER_CONSTANT_H_
#include "engine/AdaptersInterfaces/GFXAdapter_Constant.h"
#endif //_ITF_GFX_ADAPTER_CONSTANT_H_

#ifndef ITF_GFX_ZLIST_H_
#include "engine/display/GFXZList.h"
#endif // ITF_GFX_ZLIST_H_

#ifndef ITF_GFX_MATERIAL_SHADER_H_
#include "engine/display/material/GFXMaterialShader.h"
#endif // ITF_GFX_MATERIAL_SHADER_H_

#ifndef ITF_GFX_PRIMITIVES_H_
#include "engine/display/Primitives/GFXPrimitives.h"
#endif

namespace ITF
{
    enum DrawCallStateCategoryMask
    {
        DCC_COLOR_ALPHA_WRITE       = 1 << 0,
        DCC_ALPHA_BLEND             = 1 << 1,
        DCC_SEPARATE_ALPHA_BLEND    = 1 << 2,
        DCC_ALPHA                   = 1 << 3,
        DCC_DEPTH                   = 1 << 4,
        DCC_STENCIL                 = 1 << 5,
        DCC_CULLMODE                = 1 << 6,
        DCC_FILLMODE                = 1 << 7,
        ENUM_FORCE_SIZE_32(DrawCallStateCategoryMask)
    };

    class DrawCallStateCache
    {

    public:
        DrawCallStateCache()
            : m_colorMask(GFX_COLOR_MASK_FLAGS(GFX_COL_MASK_RED | GFX_COL_MASK_GREEN | GFX_COL_MASK_BLUE))
            , m_alphaBlend(GFX_BLEND_ALPHA)
            , m_separateAlphaBlend(GFX_ALPHAMASK_NONE)
            , m_alphaTest(bfalse)
            , m_alphaRef(1)
            , m_depthTest(btrue)
            , m_depthWrite(bfalse)
            , m_depthFunc(GFX_CMP_LESSEQUAL)
            , m_depthBias(0.0f)
            , m_stencilTest(bfalse)
            , m_stencilFunc(GFX_CMP_NEVER)
            , m_stencilZPass(GFX_STENCIL_KEEP)
            , m_stencilZFail(GFX_STENCIL_KEEP)
            , m_stencilFail(GFX_STENCIL_KEEP)
            , m_stencilRef(0x00)
            , m_stencilPreMask(0x00)
            , m_stencilWriteMask(0x00)
            , m_cullMode(GFX_CULL_NONE)
            , m_fillMode(GFX_FILL_SOLID)
            , m_stateChange(0)
            , m_stateApply(0)
        {
        }

        DrawCallStateCache(const DrawCallStateCache & _ctxt)
            : m_colorMask(_ctxt.m_colorMask)
            , m_alphaBlend(_ctxt.m_alphaBlend)
            , m_separateAlphaBlend(_ctxt.m_separateAlphaBlend)
            , m_alphaTest(_ctxt.m_alphaTest)
            , m_alphaRef(_ctxt.m_alphaRef)
            , m_depthTest(_ctxt.m_depthTest)
            , m_depthWrite(_ctxt.m_depthWrite)
            , m_depthFunc(_ctxt.m_depthFunc)
            , m_depthBias(_ctxt.m_depthBias)
            , m_stencilTest(_ctxt.m_stencilTest)
            , m_stencilFunc(_ctxt.m_stencilFunc)
            , m_stencilZPass(_ctxt.m_stencilZPass)
            , m_stencilZFail(_ctxt.m_stencilZFail)
            , m_stencilFail(_ctxt.m_stencilFail)
            , m_stencilRef(_ctxt.m_stencilRef)
            , m_stencilPreMask(_ctxt.m_stencilPreMask)
            , m_stencilWriteMask(_ctxt.m_stencilWriteMask)
            , m_cullMode(_ctxt.m_cullMode)
            , m_fillMode(_ctxt.m_fillMode)
            , m_stateChange(_ctxt.m_stateChange)
            , m_stateApply(_ctxt.m_stateApply)
        {
        }

        // get
        GFX_COLOR_MASK_FLAGS getColorMask() const       { return m_colorMask; }
        bbool           getColorWrite() const       { return (m_colorMask & (GFX_COL_MASK_RED | GFX_COL_MASK_GREEN | GFX_COL_MASK_BLUE)) != 0; }
        bbool           getAlphaWrite() const       { return (m_colorMask & GFX_COL_MASK_ALPHA) != 0; }
        GFX_BLENDMODE   getAlphaBlend() const       { return m_alphaBlend; }
        GFX_ALPHAMASK   getSeparateAlphaBlend() const{ return m_separateAlphaBlend; }
        bbool           getAlphaTest() const        { return m_alphaTest; }
        u8              getAlphaRef() const         { return m_alphaRef; }
        bbool           getDepthTest() const        { return m_depthTest; }
        bbool           getDepthWrite() const       { return m_depthWrite; }
        GFX_CMPFUNC     getDepthFunc() const        { return m_depthFunc; }
        f32             getDepthBias() const        { return m_depthBias; }
        bbool           getStencilTest() const      { return m_stencilTest; }
        GFX_CMPFUNC     getStencilFunc() const      { return m_stencilFunc; }
        GFX_STENCILFUNC getStencilZPass() const     { return m_stencilZPass; }
        GFX_STENCILFUNC getStencilZFail() const     { return m_stencilZFail; }
        GFX_STENCILFUNC getStencilFail() const      { return m_stencilFail; }
        u8              getStencilRef() const       { return m_stencilRef; }
        u8              getStencilPreMask() const   { return m_stencilPreMask; }
        u8              getStencilWriteMask() const { return m_stencilWriteMask; }
        GFX_CULLMODE    getCullMode() const         { return m_cullMode; }
        GFX_FILLMODE    getFillMode() const         { return m_fillMode; }
        u32             getStateChange() const      { return m_stateChange; }
        u32             getStateApply() const       { return m_stateApply; }
        
        // set (chainable)
        DrawCallStateCache & setColorMask(GFX_COLOR_MASK_FLAGS _param)
        {
            m_colorMask = _param;
            m_stateChange |= DCC_COLOR_ALPHA_WRITE;
            return *this;
        }
        DrawCallStateCache & setColorWrite(bbool _param)
        {
            if ( _param )
                m_colorMask |= GFX_COL_MASK_RED | GFX_COL_MASK_GREEN | GFX_COL_MASK_BLUE;
            else
                m_colorMask &= ~(GFX_COL_MASK_RED | GFX_COL_MASK_GREEN | GFX_COL_MASK_BLUE);
            m_stateChange |= DCC_COLOR_ALPHA_WRITE;
            return *this;
        }
        DrawCallStateCache & setAlphaWrite(bbool _param)
        {
            if ( _param )
                m_colorMask |= GFX_COL_MASK_ALPHA;
            else
                m_colorMask &= ~GFX_COL_MASK_ALPHA;
            m_stateChange |= DCC_COLOR_ALPHA_WRITE;
            return *this;
        }
        DrawCallStateCache & setAlphaBlend(GFX_BLENDMODE _param)       { m_alphaBlend = _param;     m_stateChange |= DCC_ALPHA_BLEND; return *this;}
        DrawCallStateCache & setSeparateAlphaBlend(GFX_ALPHAMASK _param) { m_separateAlphaBlend = _param; m_stateChange |= DCC_SEPARATE_ALPHA_BLEND;return *this;}
        DrawCallStateCache & setAlphaBlend(GFX_BLENDMODE _param1, GFX_ALPHAMASK _param2)
		{
			m_alphaBlend = _param1;
			m_separateAlphaBlend = _param2;
			m_stateChange |= DCC_ALPHA_BLEND | DCC_SEPARATE_ALPHA_BLEND;
			return *this;
		}
        DrawCallStateCache & setAlphaTest(bbool _param)                { m_alphaTest = _param;      m_stateChange |= DCC_ALPHA;      return *this;}
        DrawCallStateCache & setAlphaRef(u8 _param)                    { m_alphaRef = _param;       m_stateChange |= DCC_ALPHA;      return *this;}
        DrawCallStateCache & setDepthTest(bbool _param)                { m_depthTest = _param;      m_stateChange |= DCC_DEPTH;      return *this;}
        DrawCallStateCache & setDepthWrite(bbool _param)               { m_depthWrite = _param;     m_stateChange |= DCC_DEPTH;      return *this;}
        DrawCallStateCache & setDepthFunc(GFX_CMPFUNC _param)          { m_depthFunc = _param;      m_stateChange |= DCC_DEPTH;      return *this;}
        DrawCallStateCache & setDepthBias(f32 _param)                  { m_depthBias = _param;      m_stateChange |= DCC_DEPTH;      return *this;}
        DrawCallStateCache & setStencilTest(bbool _param)              { m_stencilTest = _param;    m_stateChange |= DCC_STENCIL;    return *this;}
        DrawCallStateCache & setStencilFunc(GFX_CMPFUNC _param)        { m_stencilFunc = _param;    m_stateChange |= DCC_STENCIL;    return *this;}
        DrawCallStateCache & setStencilZPass(GFX_STENCILFUNC _param)   { m_stencilZPass = _param;   m_stateChange |= DCC_STENCIL;    return *this;}
        DrawCallStateCache & setStencilZFail(GFX_STENCILFUNC _param)   { m_stencilZFail = _param;   m_stateChange |= DCC_STENCIL;    return *this;}
        DrawCallStateCache & setStencilFail(GFX_STENCILFUNC _param)    { m_stencilFail = _param;    m_stateChange |= DCC_STENCIL;    return *this;}
        DrawCallStateCache & setStencilRef(u8 _param)                  { m_stencilRef = _param;     m_stateChange |= DCC_STENCIL;    return *this;}
        DrawCallStateCache & setStencilPreMask(u8 _param)              { m_stencilPreMask = _param; m_stateChange |= DCC_STENCIL;    return *this;}
        DrawCallStateCache & setStencilWriteMask(u8 _param)            { m_stencilWriteMask = _param; m_stateChange |= DCC_STENCIL;  return *this;}
        DrawCallStateCache & setCullMode(GFX_CULLMODE _param)          { m_cullMode = _param;       m_stateChange |= DCC_CULLMODE;   return *this;}
        DrawCallStateCache & setFillMode(GFX_FILLMODE _param)          { m_fillMode = _param;       m_stateChange |= DCC_FILLMODE;   return *this;}

        void                 setStateChange(u32 _state)                { m_stateChange = _state;}
        void                 setStateApply(u32 _state)                 { m_stateChange = _state;}

        void    reset();
        u32     neededStateApply();
        u32     neededStateRestore();
        void    computeNewState( u32 _applyFlags );

    private:

        // Color
        GFX_COLOR_MASK_FLAGS m_colorMask;

        // Alpha
        GFX_BLENDMODE   m_alphaBlend;
        GFX_ALPHAMASK   m_separateAlphaBlend;
        bbool           m_alphaTest;
        u8              m_alphaRef;

        // Depth
        bbool           m_depthTest;
        bbool           m_depthWrite;
        GFX_CMPFUNC     m_depthFunc;
        f32             m_depthBias;

        // Stencil
        bbool           m_stencilTest;
        GFX_CMPFUNC     m_stencilFunc;
        GFX_STENCILFUNC m_stencilZPass;
        GFX_STENCILFUNC m_stencilZFail;
        GFX_STENCILFUNC m_stencilFail;
        u8              m_stencilRef;
        u8              m_stencilPreMask;
        u8              m_stencilWriteMask;

        // Other
        GFX_CULLMODE    m_cullMode;
        GFX_FILLMODE    m_fillMode;

        //check if state change
        u32             m_stateChange;
        u32             m_stateApply;
    };




    enum GFX_ZLIST_PASS_TYPE
    {
        GFX_ZLIST_PASS_ZPREPASS = 0,
        GFX_ZLIST_PASS_3D,
        GFX_ZLIST_PASS_REGULAR,
        GFX_ZLIST_PASS_FRONT_LIGHT,
        GFX_ZLIST_PASS_BACK_LIGHT,
        GFX_ZLIST_PASS_2D,
        GFX_ZLIST_PASS_REFRACTION,
        GFX_ZLIST_PASS_REFLECTION_PREPASS,
        GFX_ZLIST_PASS_REFLECTION,
        GFX_ZLIST_PASS_FLUID,
        GFX_ZLIST_PASS_SCREENSHOT_2D, // rendering of additional 2D elements on a screen shot (logo, descriptions of map)
        GFX_ZLIST_PASS_MASK,
        GFX_ZLIST_PASS_COUNT,
    };

    // the pass filter flag is used to filter primitives according to the passes
    template<GFX_ZLIST_PASS_TYPE _passType> inline u32 PassFilterFlag() { return 1u << _passType; }
    inline u32 buildPassFilterFlag(GFX_ZLIST_PASS_TYPE _passType) { return 1u << _passType; }

    enum GFX_ZLIST_PASS_FILTER_FLAG
    {
        GFX_ZLIST_PASS_ZPREPASS_FLAG = 1u << GFX_ZLIST_PASS_ZPREPASS,
        GFX_ZLIST_PASS_REGULAR_FLAG = 1u << GFX_ZLIST_PASS_REGULAR,
        GFX_ZLIST_PASS_3D_FLAG = 1u << GFX_ZLIST_PASS_3D,
        GFX_ZLIST_PASS_2D_FLAG = 1u << GFX_ZLIST_PASS_2D,
        GFX_ZLIST_PASS_SCREENSHOT_2D_FLAG = 1u << GFX_ZLIST_PASS_SCREENSHOT_2D,
        GFX_ZLIST_PASS_FRONT_LIGHT_FLAG = 1u << GFX_ZLIST_PASS_FRONT_LIGHT,
        GFX_ZLIST_PASS_BACK_LIGHT_FLAG = 1u << GFX_ZLIST_PASS_BACK_LIGHT,
        GFX_ZLIST_PASS_REFRACTION_FLAG = 1u << GFX_ZLIST_PASS_REFRACTION,
        GFX_ZLIST_PASS_REFLECTION_PREPASS_FLAG = 1u << GFX_ZLIST_PASS_REFLECTION_PREPASS,
        GFX_ZLIST_PASS_REFLECTION_FLAG = 1u << GFX_ZLIST_PASS_REFLECTION,
        GFX_ZLIST_PASS_FLUID_FLAG = 1u << GFX_ZLIST_PASS_FLUID,
        GFX_ZLIST_PASS_MASK_FLAG = 1u << GFX_ZLIST_PASS_MASK,
        GFX_ZLIST_PASS_ALL_FLAG = GFX_ZLIST_PASS_ZPREPASS_FLAG 
        | GFX_ZLIST_PASS_REGULAR_FLAG
        | GFX_ZLIST_PASS_3D_FLAG
        | GFX_ZLIST_PASS_2D_FLAG
        | GFX_ZLIST_PASS_SCREENSHOT_2D_FLAG
        | GFX_ZLIST_PASS_FRONT_LIGHT_FLAG
        | GFX_ZLIST_PASS_BACK_LIGHT_FLAG
        | GFX_ZLIST_PASS_REFRACTION_FLAG
        | GFX_ZLIST_PASS_REFLECTION_PREPASS_FLAG
        | GFX_ZLIST_PASS_REFLECTION_FLAG
        | GFX_ZLIST_PASS_FLUID_FLAG,
        GFX_ZLIST_PASS_ALL_SPECIAL_FLAG = GFX_ZLIST_PASS_REFRACTION_FLAG
        | GFX_ZLIST_PASS_FLUID_FLAG
        | GFX_ZLIST_PASS_MASK_FLAG
        | GFX_ZLIST_PASS_REFLECTION_PREPASS_FLAG,
    };


    class RenderPassContext
    {
    public:
        RenderPassContext()
            : m_zlist(NULL)
            , m_pass(GFX_ZLIST_PASS_REGULAR)
            , m_passFilterFlag(PassFilterFlag<GFX_ZLIST_PASS_REGULAR>())
            , m_beginPassIdx(0)
            , m_currentPassIdx(0)
            , m_endPassIdx(0)
            , m_clear(bfalse)
            , m_clearZStencil(bfalse)
            , m_clearColor(0.f, 0.f, 0.f, 1.f)
            , m_reverseOrder(bfalse)
            , m_resolveTarget(bfalse)
            , m_passRenderTargetSize(splatZero())
            , m_stateCache(DrawCallStateCache())
			, m_farPlaneDist(-10000.0f)
        {
        }

        explicit RenderPassContext(const GFX_Zlist<GFXPrimitive> & _zlist)
            : m_zlist(&_zlist)
            , m_pass(GFX_ZLIST_PASS_REGULAR)
            , m_passFilterFlag(PassFilterFlag<GFX_ZLIST_PASS_REGULAR>())
            , m_beginPassIdx(0)
            , m_currentPassIdx(0)
            , m_endPassIdx(_zlist.getNumberEntry())
            , m_clear(bfalse)
            , m_clearZStencil(bfalse)
            , m_clearColor(0.f, 0.f, 0.f, 1.f)
            , m_reverseOrder(bfalse)
            , m_resolveTarget(bfalse)
            , m_passRenderTargetSize(splatZero())
            , m_stateCache(DrawCallStateCache())
			, m_farPlaneDist(-10000.0f)
        {
        }

        RenderPassContext(const RenderPassContext & _ctxt)
            : m_zlist(_ctxt.m_zlist)
            , m_pass(_ctxt.m_pass)
            , m_passFilterFlag(_ctxt.m_passFilterFlag)
            , m_beginPassIdx(_ctxt.m_beginPassIdx)
            , m_currentPassIdx(_ctxt.m_currentPassIdx)
            , m_endPassIdx(_ctxt.m_endPassIdx)
            , m_clear(_ctxt.m_clear)
            , m_clearZStencil(_ctxt.m_clearZStencil)
            , m_clearColor(_ctxt.m_clearColor)
            , m_reverseOrder(_ctxt.m_reverseOrder)
            , m_resolveTarget(_ctxt.m_resolveTarget)
            , m_passRenderTargetSize(_ctxt.m_passRenderTargetSize)
            , m_stateCache(_ctxt.m_stateCache)
			, m_farPlaneDist(_ctxt.m_farPlaneDist)
        {
        }

        // get
        GFX_ZLIST_PASS_TYPE getPass() const { return m_pass; }
        u32 getPassFilterFlag() const { return m_passFilterFlag; }
        void setPassFilterFlag(ux _filter) { m_passFilterFlag = _filter; }
        ux getBeginZListIdx() const { return m_beginPassIdx; }
        ux getCurrentZListIdx() const { return m_currentPassIdx; }
        ux getEndZListIdx() const { return m_endPassIdx; }
        bbool getClear() const { return m_clear;}
        bbool getClearZStencil() const { return m_clearZStencil;}
        const Color & getClearColor() const { return m_clearColor;}
        bbool getReverseOrder() const { return m_reverseOrder;}
        bbool getResolveTarget() const { return m_resolveTarget;}
        Float4 getRenderTargetSize() const { return m_passRenderTargetSize; }
        DrawCallStateCache& getStateCache() { return m_stateCache; }
		f32 getFarPlaneDist() { return m_farPlaneDist; }

        const ZList_Node_Prim & getNode(ux _idx) const { return m_zlist->getNodeAt(_idx); } 
        u32 needRenderPass(GFX_ZLIST_PASS_TYPE _pass) { return m_zlist->getPassFlag() & buildPassFilterFlag(_pass);}

        // set (chainable)
        RenderPassContext & setPassType(GFX_ZLIST_PASS_TYPE _pass) { m_pass = _pass; m_passFilterFlag = buildPassFilterFlag(_pass);  return *this;}
        RenderPassContext & setBeginZListIdx(ux _beginIdx) { m_beginPassIdx = _beginIdx; return *this;}
        RenderPassContext & setCurrentZListIdx(ux _currentIdx) { m_currentPassIdx = _currentIdx; return *this;}
        RenderPassContext & setEndZListIdx(ux _endIdx) { m_endPassIdx = _endIdx; return *this;}
        RenderPassContext & setClear(bbool _setOn = btrue) { m_clear = _setOn; return *this;}
        RenderPassContext & setClearZStencil(bbool _setOn = btrue) { m_clearZStencil = _setOn; return *this;}
        RenderPassContext & setClearColor(const Color & _c) { m_clearColor = _c; return *this;}
        RenderPassContext & setReverseOrder(bbool _setOn = btrue) { m_reverseOrder = _setOn; return *this;}
        RenderPassContext & setResolveTarget(bbool _setOn = btrue) { m_resolveTarget = _setOn; return *this;}
        RenderPassContext & setRenderTargetSize(f32 _width, f32 _height) { m_passRenderTargetSize = loadXYZW(_width, _height, 0, 0); return *this;}
        RenderPassContext & setRenderTargetSize(Float4 _widthHeight) { m_passRenderTargetSize = _widthHeight; return *this;}
        RenderPassContext & setFarPlaneDist(f32 _far) { m_farPlaneDist= _far; return *this;}

    private:
        Float4 m_passRenderTargetSize; // (width, height, 0, 0)
        Color m_clearColor;
        const GFX_Zlist<GFXPrimitive> * m_zlist;
        GFX_ZLIST_PASS_TYPE m_pass;
        u32 m_passFilterFlag;
        ux m_beginPassIdx;
        ux m_currentPassIdx;
        ux m_endPassIdx;
        bbool m_clear;
        bbool m_clearZStencil;
        bbool m_reverseOrder;
        bbool m_resolveTarget;
		f32	  m_farPlaneDist;
        DrawCallStateCache m_stateCache;
    };




    class PrimitiveContext
    {
    public:
        PrimitiveContext()
            : m_camZ(0.0f)
            , m_camZWorld(0.0f)
            , m_camZAbs(0.0f)
            , m_primitiveParam(NULL)
            , m_renderPassContext(NULL)
        {
        }

        PrimitiveContext(RenderPassContext* _ctx)
            : m_camZ(0.0f)
            , m_camZWorld(0.0f)
            , m_camZAbs(0.0f)
            , m_primitiveParam(NULL)
            , m_renderPassContext(_ctx)
        {
        }

        PrimitiveContext(GFXPrimitiveParam* _primitiveParam, RenderPassContext* _ctx, f32 _camZ, f32 _camZWorld, f32 _camZAbs)
            : m_camZ(_camZ)
            , m_camZWorld(_camZWorld)
            , m_camZAbs(_camZAbs)
            , m_primitiveParam(_primitiveParam)
            , m_renderPassContext(_ctx)
        {
        }

        PrimitiveContext(const PrimitiveContext & _ctxt)
            : m_camZ(_ctxt.m_camZ)
            , m_camZWorld(_ctxt.m_camZWorld)
            , m_camZAbs(_ctxt.m_camZAbs)
            , m_primitiveParam(_ctxt.m_primitiveParam)
            , m_renderPassContext(_ctxt.m_renderPassContext)
        {
        }

        // get
        f32 getCamZ() const { return m_camZ; }
        f32 getCamZWorld() const { return m_camZWorld; }
        f32 getCamZAbs() const { return m_camZAbs; }
        const GFXPrimitiveParam* getPrimitiveParam() const { return m_primitiveParam; }
        RenderPassContext* getRenderPassContext() const { return m_renderPassContext; }

        // set (chainable)
        PrimitiveContext & setCamZ(f32 _camZ, f32 _camZWorld, f32 _camZAbs) { m_camZ = _camZ; m_camZWorld = _camZWorld; m_camZAbs = _camZAbs; return *this;}
        PrimitiveContext & setPrimitiveParam(GFXPrimitiveParam* _param) { m_primitiveParam = _param; return *this;}
        PrimitiveContext & setRenderPassContext(RenderPassContext* _pContext) { m_renderPassContext = _pContext; return *this;}

    private:

        f32                 m_camZ;
        f32                 m_camZWorld;
        f32                 m_camZAbs;
        GFXPrimitiveParam*  m_primitiveParam;
        RenderPassContext*  m_renderPassContext;
    };




    class DrawCallContext
    {
    public:
        DrawCallContext()
            : m_primitiveContext(NULL)
            , m_stateCache(DrawCallStateCache())
        {
        }

        DrawCallContext(PrimitiveContext* _ctx)
            : m_primitiveContext(_ctx)
            , m_stateCache(DrawCallStateCache())
        {
        }

        DrawCallContext(const DrawCallContext & _ctxt)
            : m_primitiveContext(_ctxt.m_primitiveContext)
            , m_stateCache(_ctxt.m_stateCache)
        {
        }

        // get
        PrimitiveContext*   getPrimitiveContext() const {return m_primitiveContext;}
        DrawCallStateCache& getStateCache() {return m_stateCache;}


        // set (chainable)
        DrawCallContext & setPrimitiveContext(PrimitiveContext* _ctx) { m_primitiveContext = _ctx; return *this;}
        DrawCallContext & setStateCache(DrawCallStateCache _stateCache) { m_stateCache = _stateCache; return *this;}

        f32 getCurrentZbias()
        {
            if(m_stateCache.getStateChange() & DCC_DEPTH)
                return m_stateCache.getDepthBias();
            else
                return m_primitiveContext->getRenderPassContext()->getStateCache().getDepthBias();
        }

    private:
        PrimitiveContext*  m_primitiveContext;
        DrawCallStateCache m_stateCache;
    };

}
#endif // ITF_GFX_CONTEXT_H_
