#ifndef ITF_ENGINE_MESH3DPRIM_H_
#define ITF_ENGINE_MESH3DPRIM_H_

#ifndef ITF_ENGINE_MESH_H_
#include "engine/display/Primitives/Mesh.h"
#endif //ITF_ENGINE_MESH_H_

namespace ITF
{
    class Mesh3DPrim : public GFXPrimitive
    {
    public:

        virtual void directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z = 0.0f, f32 _ZWorld = 0.0f, f32 _ZAbs = 0.0f);

        // TODO -> place stuff there in private

        ITF_Mesh m_mesh;

        //3D BV
        Vec3d  m_bvMin; 
        Vec3d  m_bvMax; 

        // skinning gizmos
        ITF_VECTOR<Matrix44> m_gizmos;
    };
}

#endif // ITF_ENGINE_MESH3DPRIM_H_
