#include "precompiled_engine.h"

#ifndef ITF_MASKRESOLVEPRIMITIVE_H_
#include "MaskResolvePrimitive.h"
#endif //ITF_MASKRESOLVEPRIMITIVE_H_

#ifndef _ITF_LIGHTMANAGER_H_
#include "engine/display/lighting/LightManager.h"
#endif // _ITF_LIGHTMANAGER_H_

namespace ITF
{

    void MaskResolvePrimitive::directDraw(const class RenderPassContext & _rdrPassCtxt, GFXAdapter * _gfxDeviceAdapter, f32 _Z, f32 _ZWorld, f32 _ZAbs)
    {
        PRF_M_SCOPE(drawMaskResolve);

        _gfxDeviceAdapter->m_lightManager->drawLightingPass((class RenderPassContext &)_rdrPassCtxt, *this, _Z, _ZWorld, _ZAbs);
    }

} // namespace ITF

