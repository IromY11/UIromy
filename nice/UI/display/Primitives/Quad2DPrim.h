#ifndef ITF_ENGINE_QUAD2DPRIM_H_
#define ITF_ENGINE_QUAD2DPRIM_H_

#ifndef ITF_GFX_PRIMITIVES_H_
#include "GFXPrimitives.h"
#endif

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

namespace ITF
{
    class Quad2DPrim : public GFXPrimitive
    {
    public:

        const Quad2DInfo & getQuadInfo() const { return m_quadInfo; }
        Quad2DInfo & getQuadInfo() { return m_quadInfo; }

        virtual void directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z = 0.0f, f32 _ZWorld = 0.0f, f32 _ZAbs = 0.0f);

    private:
        Quad2DInfo m_quadInfo;
    };
}

#endif // ITF_ENGINE_QUAD2DPRIM_H_
