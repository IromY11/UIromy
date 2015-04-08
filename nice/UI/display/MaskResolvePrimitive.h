#ifndef ITF_MASKRESOLVEPRIMITIVE_H_
#define ITF_MASKRESOLVEPRIMITIVE_H_

#ifndef ITF_GFX_PRIMITIVES_H_
#include "engine/display/Primitives/GFXPrimitives.h"
#endif //ITF_GFX_PRIMITIVES_H_

namespace ITF
{

class MaskResolvePrimitive : public GFXPrimitive
{
public:

    MaskResolvePrimitive()
        : GFXPrimitive(GFXPrimitive::TYPE_MASKRESOLVER)
        , m_resolveFrontLightBuffer(btrue)
        , m_resolveBackLightBuffer(btrue)
        , m_clearFrontLight(bfalse)
        , m_clearFrontLightColor(0.f, 0.f, 0.f, 1.f)
        , m_clearBackLight(bfalse)
        , m_clearBackLightColor(0.f, 0.f, 0.f, 1.f)
        , m_resolveFrontLightBufferInverted(bfalse)
        , m_blurFrontLightBuffer(bfalse)
        , m_blurBackLightBuffer(bfalse)
        , m_blurQuality(1)
        , m_blurSize(8)
    {
    }

    ~MaskResolvePrimitive()
    {
    }

    void setFrontLightBufferInverted(bbool _set) { m_resolveFrontLightBufferInverted = _set; }
    void setFrontLightBuffer(bbool _set) { m_resolveFrontLightBuffer = _set; }
    void setBackLightBuffer(bbool _set) { m_resolveBackLightBuffer = _set; }
    void setClearFrontLight(bbool _set) { m_clearFrontLight = _set; }
    void setClearFrontLightColor(const Color & _c) { m_clearFrontLightColor = _c; }
    void setClearBackLight(bbool _set) { m_clearBackLight = _set; }
    void setClearBackLightColor(const Color & _c) { m_clearBackLightColor = _c; }
    void setBlurFrontLight(bbool _set) {m_blurFrontLightBuffer = _set;}
    void setBlurBackLight(bbool _set) {m_blurBackLightBuffer = _set;}
    void setBlurQuality(u32 _set) {m_blurQuality = _set;}
    void setBlurSize(u32 _set) {m_blurSize = _set;}
    

    bbool getFrontLightBuffer() const { return m_resolveFrontLightBuffer; }
    bbool getFrontLightBufferInverted() const { return m_resolveFrontLightBufferInverted; }
    bbool getBackLightBuffer() const { return m_resolveBackLightBuffer; }
    bbool getClearFrontLight() const { return m_clearFrontLight; }
    const Color & getClearFrontLightColor() const { return m_clearFrontLightColor; }
    bbool getClearBackLight() const { return m_clearBackLight; }
    const Color & getClearBackLightColor() const { return m_clearBackLightColor; }
    bbool getBlurFrontLight() const { return m_blurFrontLightBuffer; }
    bbool getBlurBackLight() const { return m_blurBackLightBuffer; }
    u32   getBlurQuality() const { return m_blurQuality; }
    u32   getBlurSize() const { return m_blurSize; }

private:
    virtual void directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z = 0.0f, f32 _ZWorld = 0.0f, f32 _ZAbs = 0.0f);

    bbool               m_resolveFrontLightBufferInverted;
    bbool				m_resolveFrontLightBuffer;
    bbool				m_resolveBackLightBuffer;
    bbool               m_clearFrontLight;
    Color               m_clearFrontLightColor;
    bbool               m_clearBackLight;
    Color               m_clearBackLightColor;
    bbool				m_blurFrontLightBuffer;
    bbool				m_blurBackLightBuffer;
    u32  				m_blurQuality;
    u32  				m_blurSize;
};

} // namespace ITF

#endif // ITF_MASKRESOLVEPRIMITIVE_H_
