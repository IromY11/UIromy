#include "precompiled_engine.h"

#ifndef ITF_ENGINE_QUADFRAMEBLENDPRIM_H_
#include "QuadFrameBlendPrim.h"
#endif //ITF_ENGINE_QUADFRAMEBLENDPRIM_H_

namespace ITF
{
    void QuadFrameBlendPrim::directDraw(const RenderPassContext & _rdrPassCtxt, GFXAdapter * _gfxDeviceAdapter, f32 _Z, f32 _ZWorld, f32 _ZAbs)
    {
        PrimitiveContext primCtx;
        primCtx.setRenderPassContext((RenderPassContext*)&_rdrPassCtxt)
            .setPrimitiveParam(getCommonParam())
            .setCamZ(_Z,_ZWorld, _ZAbs);
        DrawCallContext drawCallCtx;
        drawCallCtx.setPrimitiveContext(&primCtx);

        _gfxDeviceAdapter->setGFXPrimitiveParameters(getCommonParam(), _rdrPassCtxt);
        bbool doDraw = bfalse;
        if (m_gfxMaterial)
            doDraw = _gfxDeviceAdapter->setGfxMaterial(drawCallCtx, *m_gfxMaterial);
        else
            doDraw = _gfxDeviceAdapter->setGfxMatDefault(drawCallCtx);
        if (doDraw)
            _gfxDeviceAdapter->drawFrameBlendQuad(drawCallCtx, this);
    }

}
