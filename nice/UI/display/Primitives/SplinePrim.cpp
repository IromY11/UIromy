#include "precompiled_engine.h"

#ifndef ITF_ENGINE_SPLINEPRIM_H_
#include "SplinePrim.h"
#endif //ITF_ENGINE_SPLINEPRIM_H_

namespace ITF
{
    void SplinePrim::directDraw(const RenderPassContext & _rdrPassCtxt, GFXAdapter * _gfxDeviceAdapter, f32 _Z, f32 _ZWorld, f32 _ZAbs)
    {
        PRF_M_SCOPE(drawSpline);

        PrimitiveContext primCtx;
        primCtx.setRenderPassContext((RenderPassContext*)&_rdrPassCtxt)
            .setPrimitiveParam(getCommonParam())
            .setCamZ(_Z,_ZWorld, _ZAbs);

        _gfxDeviceAdapter->setGlobalColor(m_splineInfo.m_color);
        _gfxDeviceAdapter->drawSpline(primCtx, (Matrix44 *)&m_splineInfo.m_mat, m_splineInfo.m_texture, &m_splineInfo.m_spline, m_splineInfo.m_height);
    }
}
