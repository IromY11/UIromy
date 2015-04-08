#ifndef ITF_ENGINE_SPLINEPRIM_H_
#define ITF_ENGINE_SPLINEPRIM_H_

#ifndef ITF_GFX_PRIMITIVES_H_
#include "GFXPrimitives.h"
#endif

namespace ITF
{
    class SplinePrim : public GFXPrimitive
    {
    public:

        const Spline_Info & getSplineInfo() const { return m_splineInfo; }
        Spline_Info & getSplineInfo() { return m_splineInfo; }

        virtual void directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z = 0.0f, f32 _ZWorld = 0.0f, f32 _ZAbs = 0.0f);

    private:
        Spline_Info m_splineInfo;
    };
}

#endif // ITF_ENGINE_SPLINEPRIM_H_
