#ifndef _ITF_CAMERASHAKE_H_
#define _ITF_CAMERASHAKE_H_

#ifndef _ITF_CURVE_H_
#include "gameplay/tools/Curve.h"
#endif // _ITF_CURVE_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

namespace ITF
{

//------------------------------------------------------------------------------
/// Params for one single shake curve (one axis)
class CameraShakeCurveParams : public BaseCurveParams
{
public:
    DECLARE_OBJECT_CHILD_RTTI(CameraShakeCurveParams,BaseCurveParams,3257154548);
    DECLARE_SERIALIZE()

    CameraShakeCurveParams();

    void setup(f32 _intensity, f32 _duration, f32 _easeInDuration, f32 _easeOutDuration);

    ITF_INLINE f32 getFrequency() const { return m_frequency; }
    ITF_INLINE f32 getAmplitude() const { return m_amplitude; }
    ITF_INLINE f32 getOffset() const { return m_offset; }

private:
    virtual f32 computeValue(f32 _x);
    virtual f32 getDrawPeriod() {return (m_xMax - m_xMin);}

    // serialized
    f32 m_frequency;
    f32 m_amplitude;
    f32 m_offset;

    // initialized by setup()
    bbool m_setup;
    f32 m_intensity;
    f32 m_easeInDuration;
    f32 m_easeOutDuration;
};

//------------------------------------------------------------------------------
/// Camera shake definition
class CameraShake
{
public:
    DECLARE_SERIALIZE()

    CameraShake();

    void setup();

    ITF_INLINE const StringID& getName() const { return m_name; }
    ITF_INLINE f32 getIntensity() const { return m_intensity; }
    ITF_INLINE f32 getDuration() const { return m_duration; }
    ITF_INLINE f32 getEaseInDuration() const { return m_easeInDuration; }
    ITF_INLINE f32 getEaseOutDuration() const { return m_easeOutDuration; }
    ITF_INLINE CameraShakeCurveParams& getShakeX() { return m_shakeX; }
    ITF_INLINE CameraShakeCurveParams& getShakeY() { return m_shakeY; }
    ITF_INLINE CameraShakeCurveParams& getShakeZ() { return m_shakeZ; }
    ITF_INLINE bbool isSetup() const { return m_setup; }

private:
    // serialized
    StringID m_name;
    f32 m_intensity;
    f32 m_duration;
    f32 m_easeInDuration;
    f32 m_easeOutDuration;
    CameraShakeCurveParams m_shakeX;
    CameraShakeCurveParams m_shakeY;
    CameraShakeCurveParams m_shakeZ;

    // initialized by setup()
    bbool m_setup;
};

//------------------------------------------------------------------------------
/// Collection of camera shake definitions
struct CameraShakeConfig_Template : public TemplateObj
{
    DECLARE_OBJECT_CHILD_RTTI(CameraShakeConfig_Template, TemplateObj,73546132);
    DECLARE_SERIALIZE()

public:
    CameraShakeConfig_Template() : Super() {}

    ITF_INLINE const ITF_VECTOR<CameraShake>& getShakes() const { return m_shakes; }

private:
    ITF_VECTOR<CameraShake> m_shakes;
};

}

#endif //_ITF_CAMERASHAKE_H_
