#include "precompiled_engine.h"

#ifndef ITF_ENGINE_QUAD2DPRIM_H_
#include "Quad2DPrim.h"
#endif //ITF_ENGINE_QUAD2DPRIM_H_

namespace ITF
{
    void Quad2DPrim::directDraw(const RenderPassContext & _rdrPassCtxt, GFXAdapter * _gfxDeviceAdapter, f32 _Z, f32 _ZWorld, f32 _ZAbs)
    {
        PRF_M_SCOPE(drawQuad2D);

        _gfxDeviceAdapter->setGFXPrimitiveParameters(getCommonParam(), _rdrPassCtxt);
        PrimitiveContext primCtx;
        primCtx.setRenderPassContext((RenderPassContext*)&_rdrPassCtxt)
            .setPrimitiveParam(getCommonParam())
            .setCamZ(_Z,_ZWorld, _ZAbs);
        DrawCallContext drawcallCtx = DrawCallContext(&primCtx);

        _gfxDeviceAdapter->drawQuad2D(drawcallCtx, m_quadInfo.m_pos2D.x(), m_quadInfo.m_pos2D.y(), m_quadInfo.m_width, m_quadInfo.m_height, m_quadInfo.m_pos2D.z(), m_quadInfo.m_angle, m_quadInfo.m_pivot, m_quadInfo.m_color, m_quadInfo.m_texture, &m_quadInfo.m_uvStart, &m_quadInfo.m_uvEnd);
    }
}
