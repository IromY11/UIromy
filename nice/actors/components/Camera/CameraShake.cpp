#include "precompiled_engine.h"

#ifndef _ITF_CAMERASHAKE_H_
#include "engine/actors/components/Camera/CameraShake.h"
#endif // _ITF_CAMERASHAKE_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(CameraShakeCurveParams)
BEGIN_SERIALIZATION_CHILD(CameraShakeCurveParams)
    SERIALIZE_MEMBER("frequency", m_frequency);
    SERIALIZE_MEMBER("amplitude", m_amplitude);
    SERIALIZE_MEMBER("offset", m_offset);
END_SERIALIZATION()

//------------------------------------------------------------------------------
CameraShakeCurveParams::CameraShakeCurveParams()
    : BaseCurveParams() 
    , m_frequency(5.0f)
    , m_amplitude(1.0f)
    , m_offset(0.0f)
    , m_setup(bfalse)
    , m_intensity(1.0f)
    , m_easeInDuration(0.0f)
    , m_easeOutDuration(1.0f)
{
}

//------------------------------------------------------------------------------
void CameraShakeCurveParams::setup(f32 _intensity, f32 _duration, f32 _easeInDuration, f32 _easeOutDuration)
{
    // could be serialized, but lets avoid confusing users with too much params
    m_yMin = -1.0f;
    m_yMax = 1.0f;

    // apply camera shake settings
    m_intensity = _intensity;
    m_xMax = _duration; // CAVEAT: overwrites possible serialization
    m_easeInDuration = _easeInDuration;
    m_easeOutDuration = _easeOutDuration;

    m_setup = btrue;
}

//------------------------------------------------------------------------------
f32 CameraShakeCurveParams::computeValue(f32 _x)
{
    ITF_ASSERT_MSG(m_setup, "Must be setup prior usage");

    // guard, mostly for debug display
    if (_x < m_xMin || _x > m_xMax)
    {
        return 0.0f;
    }

    // ease-in & ease-out
    f32 easeFactor = 1.0f;
    if (_x < m_easeInDuration)
    {
        f32 cursor = _x / m_easeInDuration; // [0->1]
        f32 cospos = (1.0f - cursor) * MTH_PI; // [PI->0]
        easeFactor = 0.5f * (1.0f + f32_Cos(cospos)); // [0->1]
    }
    else if (_x > (m_xMax - m_easeOutDuration))
    {
        f32 cursor = (m_xMax - _x) / m_easeOutDuration; // [1->0]
        f32 cospos = (1.0f - cursor) * MTH_PI; // [0->PI]
        easeFactor = 0.5f * (1.0f + f32_Cos(cospos)); // [1->0]
    }

    // sinus
    return easeFactor * m_amplitude * m_intensity * f32_Sin(MTH_2PI * _x * m_frequency + m_offset);
}

//------------------------------------------------------------------------------
BEGIN_SERIALIZATION(CameraShake)
    SERIALIZE_MEMBER("name", m_name);
    SERIALIZE_MEMBER("intensity", m_intensity);
    SERIALIZE_MEMBER("duration", m_duration);
    SERIALIZE_MEMBER("easeInDuration", m_easeInDuration);
    SERIALIZE_MEMBER("easeOutDuration", m_easeOutDuration);
    SERIALIZE_OBJECT("shakeX", m_shakeX);
    SERIALIZE_OBJECT("shakeY", m_shakeY);
    SERIALIZE_OBJECT("shakeZ", m_shakeZ);
    SERIALIZE_FUNCTION(setup, ESerialize_Data_Load);
END_SERIALIZATION()

//------------------------------------------------------------------------------
CameraShake::CameraShake()
    : m_name()
    , m_intensity(1.0f)
    , m_duration(0.0f)
    , m_easeInDuration(0.0f)
    , m_easeOutDuration(0.0f)
    , m_shakeX()
    , m_shakeY()
    , m_shakeZ()
    , m_setup(bfalse)
{
}

//------------------------------------------------------------------------------
void CameraShake::setup()
{
    // warn and give if something's wrong in serialization
    if (!m_name.isValid())
    {
        ITF_WARNING(NULL, bfalse, "Name required for camera shake definition");
        return;
    }
    if (m_intensity < 0.0f)
    {
        ITF_WARNING(NULL, bfalse, "Positive intensity required (camera shake %s)", m_name.getDebugString());
        return;
    }
    if (m_duration < 0.0f)
    {
        ITF_WARNING(NULL, bfalse, "Positive duration required (camera shake %s)", m_name.getDebugString());
        return;
    }
    if (m_easeInDuration < 0.0f)
    {
        ITF_WARNING(NULL, bfalse, "Positive ease in duration required (camera shake %s)", m_name.getDebugString());
        return;
    }
    if (m_easeOutDuration < 0.0f)
    {
        ITF_WARNING(NULL, bfalse, "Positive ease out duration required (camera shake %s)", m_name.getDebugString());
        return;
    }
    if (m_duration < (m_easeInDuration + m_easeOutDuration))
    {
        ITF_WARNING(NULL, bfalse, "Ease in + ease out durations can't exceed total duration (camera shake %s)", m_name.getDebugString());
        return;
    }

    // apply settings
    m_shakeX.setup(m_intensity, m_duration, m_easeInDuration, m_easeOutDuration);
    m_shakeY.setup(m_intensity, m_duration, m_easeInDuration, m_easeOutDuration);
    m_shakeZ.setup(m_intensity, m_duration, m_easeInDuration, m_easeOutDuration);

    m_setup = btrue;
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(CameraShakeConfig_Template)
BEGIN_SERIALIZATION_CHILD(CameraShakeConfig_Template)
    SERIALIZE_CONTAINER_OBJECT("shakes",m_shakes);
END_SERIALIZATION()

} //namespace ITF
