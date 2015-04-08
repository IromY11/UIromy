#include "precompiled_engine.h"

#ifndef ITF_ENGINE_MESH3DPRIM_H_
#include "Mesh3DPrim.h"
#endif //ITF_ENGINE_MESH3DPRIM_H_

namespace ITF
{

    void Mesh3DPrim::directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z, f32 _ZWorld, f32 _ZAbs)
    {
        PRF_M_SCOPE(drawMesh3D);

        ITF_ASSERT(_rdrPassCtxt.getPass() != GFX_ZLIST_PASS_2D); // not a valid primitive for the 2D pass

        PrimitiveContext primCtx;
        primCtx.setRenderPassContext((RenderPassContext*)&_rdrPassCtxt)
            .setPrimitiveParam(getCommonParam())
            .setCamZ(_Z,_ZWorld, _ZAbs);

        if (m_gizmos.size())
            _gfxDeviceAdapter->setShaderBonesMatrices(m_gizmos);

        m_mesh.directDrawRegular(primCtx, _gfxDeviceAdapter);
    }
}
