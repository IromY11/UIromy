#include "precompiled_engine.h"

#ifndef ITF_ENGINE_QUAD3DPRIM_H_
#include "Quad3DPrim.h"
#endif //ITF_ENGINE_QUAD3DPRIM_H_

namespace ITF
{
    void Quad3DPrim::directDraw(const RenderPassContext & _rdrPassCtxt, GFXAdapter * _gfxDeviceAdapter, f32 _Z, f32 _ZWorld, f32 _ZAbs)
    {
        PRF_M_SCOPE(drawQuad3D);

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
            _gfxDeviceAdapter->draw3DQuad(drawCallCtx, this);
    }


    void Quad3DPrimImpostor::directDraw(const RenderPassContext & _rdrPassCtxt, GFXAdapter * _gfxDeviceAdapter, f32 _Z, f32 _ZWorld, f32 _ZAbs)
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
            return;

        if (doDraw)
        {
            float sx = m_size.x() * 0.5f;
            float sy = m_size.y() * 0.5f;

            const Vec2d uv0 = m_uvData.getUV0();
            const Vec2d uv1 = m_uvData.getUV1();

            Matrix44 World;
            World.setIdentity34();
            World.setTranslation(m_pos);

            _gfxDeviceAdapter->setObjectMatrix(World);
            
            _gfxDeviceAdapter->setShaderGroup(_gfxDeviceAdapter->mc_shader_Impostor, _gfxDeviceAdapter->mc_entry_impostor_VS, _gfxDeviceAdapter->mc_entry_impostor_PS, 0, 0);

            VertexPCT quad[4];
            quad[0].setData( Vec3d( -sx, -sy, 0.f ), Vec2d(uv0.x(), uv1.y()), m_color);
            quad[1].setData( Vec3d( sx, -sy, 0.f ),  uv1,                     m_color);
            quad[3].setData( Vec3d( sx, sy, 0.f ),   Vec2d(uv1.x(), uv0.y()), m_color);
            quad[2].setData( Vec3d( -sx, sy, 0.f ),  uv0,                     m_color);

            _gfxDeviceAdapter->setVertexFormat(VertexFormat_PCT);
            _gfxDeviceAdapter->DrawPrimitive(drawCallCtx, GFX_TRIANGLE_STRIP, (void*)quad, 4);
        }
    }
}
