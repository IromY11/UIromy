#ifndef ITF_ENGINE_QUAD3DPRIM_H_
#define ITF_ENGINE_QUAD3DPRIM_H_

#ifndef ITF_GFX_PRIMITIVES_H_
#include "GFXPrimitives.h"
#endif // ITF_GFX_PRIMITIVES_H_

#ifndef ITF_GFX_MATERIAL_SHADER_H_
#include "engine/display/material/GFXMaterialShader.h"
#endif //ITF_GFX_MATERIAL_SHADER_H_

#ifndef _ITF_UVDATA_H_
#include "engine/display/UVData.h"
#endif //_ITF_UVDATA_H_


namespace ITF
{

    struct GFX_3DQUAD
    {
        GFX_3DQUAD()
        {
            m_blendMode = GFX_BLEND_ALPHA;
            m_rotation = Vec3d::Zero;
        }
        Vec3d m_pos;
        Vec2d m_pivot;
        Vec2d m_size;
        u32 m_color;
        UVdata m_uvData;
        GFX_BLENDMODE   m_blendMode;
        Vec3d  m_rotation;
        AABB m_BV;
    };

    class Quad3DPrim : public GFX_3DQUAD, public GFXPrimitive
    {
    public:
        Quad3DPrim() : m_gfxMaterial(nullptr) {}

        virtual void directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z = 0.0f, f32 _ZWorld = 0.0f, f32 _ZAbs = 0.0f);

        void setMaterial(const GFX_MATERIAL * _gfxMaterial) { m_gfxMaterial = _gfxMaterial; if (m_gfxMaterial) setPassFilterFlag(*m_gfxMaterial); };
        const GFX_MATERIAL * getMaterial() const { return m_gfxMaterial; };

    protected:
        const GFX_MATERIAL * m_gfxMaterial; // ideally should not be a pointer
    };


    class Quad3DPrimImpostor : public Quad3DPrim
    {
    public:
        Quad3DPrimImpostor(){}

        virtual void directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z = 0.0f, f32 _ZWorld = 0.0f, f32 _ZAbs = 0.0f);
    };
}

#endif // ITF_ENGINE_QUAD3DPRIM_H_
