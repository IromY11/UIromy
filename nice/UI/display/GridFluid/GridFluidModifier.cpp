#include "precompiled_engine.h"
#ifdef GFX_USE_GRID_FLUIDS

#ifndef _ITF_CORE_MACROS_H_
# include "core/Macros.h"
#endif //_ITF_CORE_MACROS_H_

#ifndef _ITF_GRIDFLUID_H_
#include "engine/display/GridFluid/GridFluid.h"
#endif // _ITF_GRIDFLUID_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef ITF_GFX_ZLIST_H_
#include "GFXZList.h"
#endif // ITF_GFX_ZLIST_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(GFX_GridFluidModifier)
	BEGIN_SERIALIZATION(GFX_GridFluidModifier)
      BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("BoxOffset",m_boxPos);
        SERIALIZE_MEMBER("BoxSize",m_boxSize);
        SERIALIZE_MEMBER("Force",m_force);
        SERIALIZE_MEMBER("ForceTexFactor",m_forceTexFactor);
        SERIALIZE_ENUM_GFX_GRID_MOD_MODE("Mode",m_mode);
        SERIALIZE_MEMBER("Intensity",m_intensity);
        SERIALIZE_MEMBER("FluidColor",m_fluidColor);
        SERIALIZE_BOOL("Active",m_active);
        SERIALIZE_BOOL("SpeedToForce",m_speedToForce);
		SERIALIZE_BOOL("IsExternal",m_isExternal);
        SERIALIZE_MEMBER("Texture",m_texturePath);
        SERIALIZE_MEMBER("LifeTime",m_lifeTime);
        SERIALIZE_MEMBER("StartDelay",m_startDelay);
        SERIALIZE_MEMBER("FadeInLength",m_fadeInLength);
        SERIALIZE_MEMBER("FadeOutLength",m_fadeOutLength);
        SERIALIZE_MEMBER("ScaleInit",m_scaleInit);
        SERIALIZE_MEMBER("ScaleEnd",m_scaleEnd);
        SERIALIZE_MEMBER("RotationInit",m_rotationInit);
        SERIALIZE_MEMBER("RotationEnd",m_rotationEnd);
        SERIALIZE_MEMBER("Speed",m_speed);
        SERIALIZE_GFX_GRID_FILTER("EmitterFilter",m_emitterFilter);
        SERIALIZE_ENUM_GFX_GRID_MOD_PULSE("PulseMode",m_pulseMode);
        SERIALIZE_MEMBER("PulseFreq",m_pulseFreq);
        SERIALIZE_MEMBER("PulsePhase",m_pulsePhase);
        SERIALIZE_MEMBER("PulseAmplitude",m_pulseAmplitude);
	  END_CONDITION_BLOCK()
    END_SERIALIZATION()

	///// constructor/destructor --------------------------------------------------------------
    GFX_GridFluidModifier::GFX_GridFluidModifier() : m_emitterFilter(0x0000ffff)
	{
		setObjectType(BaseObject::eFluidObject);
		m_boxPos.set(0.0f,0.0f, 0.0f);
		m_boxSize.set(1.0f,1.0f);
		m_intensity = 1.0f;
		m_mode = GFX_GRID_MOD_MODE_FLUID;
		m_active = true;
        m_force.set(0.0f,0.0f);
		m_forceTexFactor = 0.0f;
		m_speedToForce = false;
		m_isExternal = false;
		m_fluidColor = COLOR_WHITE;

		m_lifeTime = 0.0f;
		m_startDelay = 0.0f;
		m_fadeInLength = 0.0f;
		m_fadeOutLength = 0.0f;

		m_scaleInit.set(1.0f, 1.0f);
		m_scaleEnd.set(1.0f, 1.0f);
		m_rotationInit = 0.0f;
		m_rotationEnd = 0.0f;
		m_speed.set(0.0f, 0.0f);

		m_curIntensity = 1.0f;
		m_curScale = m_scaleInit;
		m_curRotation = m_rotationInit;
		m_curTranslation.set(0.0f, 0.0f);

		m_pulseMode = GFX_GRID_MOD_PULSE_NONE;
		m_pulseFreq = 1.0f;
		m_pulseCur = 1.0f;
		m_pulsePhase = 0.0f;
		m_pulseAmplitude = 1.0f;
	}
    GFX_GridFluidModifier::~GFX_GridFluidModifier()
	{
	}

	// Update.
	void GFX_GridFluidModifier::update(f32 _curTime, const Vec3d &_speed, const Matrix44 &_matrix)
	{
		m_curIntensity = 1.0f;
		m_curScale = m_scaleInit;
		m_curRotation = m_rotationInit;
		m_curTranslation.set(0.0f, 0.0f);
		m_pulseCur = 1.0f;

		// Process life time
		if ( m_lifeTime > 0.0f )
		{
			if ( _curTime >= m_startDelay )
			{
				// Current time from delay.
				_curTime -= m_startDelay;

				// Compute fades.
				if ( _curTime >= m_lifeTime )
					m_curIntensity = 0.0f;
				else if ( _curTime > m_lifeTime - m_fadeOutLength )
					m_curIntensity = 1.0f - (_curTime - (m_lifeTime - m_fadeOutLength)) / m_fadeOutLength;

				// Compute scale
				if ( _curTime < m_lifeTime )
					m_curScale.Lerp(m_scaleInit, m_scaleEnd, _curTime / m_lifeTime);
				else
					m_curScale = m_scaleEnd;

				// Compute rotation.
				if ( _curTime < m_lifeTime )
					m_curRotation = f32_Lerp(m_rotationInit, m_rotationEnd, _curTime / m_lifeTime);
				else
					m_curRotation = m_rotationEnd;

				// Compute translation.
				m_curTranslation = m_speed * _curTime;
			}
			else
				m_curIntensity = 0.0f;
		}

		if ( m_pulseMode != GFX_GRID_MOD_PULSE_NONE )
		{
			// Compute pulse value.
			f32 myTime = _curTime;
			if ( m_startDelay )
			{
				if ( myTime > m_startDelay )
					myTime -= m_startDelay;
				else
					myTime = 0.0f;
			}
			f32 pulse = f32_Frac( myTime * m_pulseFreq + m_pulsePhase );
			switch ( m_pulseMode )
			{
			case GFX_GRID_MOD_PULSE_NONE:
				m_pulseCur = 1.0f;
				break;
			case GFX_GRID_MOD_PULSE_SINUS:
				m_pulseCur = f32_Sin(pulse * MTH_2PI) * 0.5f + 0.5f;
				break;
			case GFX_GRID_MOD_PULSE_TRIANGLE1:
				m_pulseCur = pulse;
				break;
			case GFX_GRID_MOD_PULSE_TRIANGLE2:
				m_pulseCur = 1.0f - pulse;
				break;
			case GFX_GRID_MOD_PULSE_TRIANGLE3:
				if ( pulse <= 0.5f )
					m_pulseCur = pulse * 2.0f;
				else
					m_pulseCur = (1.0f - pulse) * 2.0f;
				break;
			case GFX_GRID_MOD_PULSE_PALIER1:
				m_pulseCur = (pulse<0.25f) ? 1.0f : 0.0f;
				break;
			case GFX_GRID_MOD_PULSE_PALIER2:
				m_pulseCur = (pulse<0.5f) ? 1.0f : 0.0f;
				break;
			case GFX_GRID_MOD_PULSE_PALIER3:
				m_pulseCur = (pulse<0.75f) ? 1.0f : 0.0f;
				break;
			}
			m_pulseCur = m_pulseCur * m_pulseAmplitude + (1.0f - m_pulseAmplitude);
		}

		if ( m_speedToForce )
		{
			Matrix44 matrixInv;
			Vec3d localSpeed;
			f32 speedNorm = _speed.norm();
			if ( speedNorm > MTH_EPSILON)
			{
				matrixInv.inverse(_matrix);
				Vec3d tmpSpeed = _speed;
//				tmpSpeed.y() *= -1.0f;
				matrixInv.transformVector(localSpeed, _speed);
				localSpeed.normalize(); // Remove possible scale from matrice.
				localSpeed *= speedNorm * 10.0f;
				m_force.set(localSpeed.x(),localSpeed.y());
			}
			else
				m_force.set(0.0f,0.0f);
		}

		// Update BV.
		m_AABB.setMinAndMax(Vec2d(m_boxPos.x(), m_boxPos.y()));
		m_AABB.grow(m_boxSize.x() * 0.5f, m_boxSize.y() * 0.5f);
		m_AABB.Scale(m_curScale);
		m_AABB.transform(_matrix);
	}

    IMPLEMENT_OBJECT_RTTI(GFX_GridFluidModifierList)
	BEGIN_SERIALIZATION(GFX_GridFluidModifierList)
      BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("Name", m_name);
        SERIALIZE_BOOL("Active", m_active);
        SERIALIZE_CONTAINER_OBJECT("ModifierList", m_modList);
	  END_CONDITION_BLOCK()
    END_SERIALIZATION()

	GFX_GridFluidModifierList::GFX_GridFluidModifierList()
	{
		setObjectType(BaseObject::eFluidObject);
		m_matrix.setIdentity();
		m_prevPos.set(0.0f,0.0f,0.0f);
		m_frameCount = 0;
		m_curTime = 0.0f;
		m_active = true;
	}
    GFX_GridFluidModifierList::~GFX_GridFluidModifierList()
	{
	}

	// Reinit modifier : current time = 0.
	void GFX_GridFluidModifierList::reInit()
	{
		m_frameCount = 0;
		m_curTime = 0.0f;
		storeXYZ((f32*)&m_prevPos, m_matrix.T());
	}

	// Update.
	void GFX_GridFluidModifierList::update(GFXAdapter * _gfxDeviceAdapter, f32 _dt)
	{
		u32 newFrameCount = CURRENTFRAME;
		if ( m_frameCount != newFrameCount )
		{
			// Increment current time.
			m_curTime += _dt;

			// Process modifier velocity.
			Vec3d speed;
			Vec3d curPos;
			storeXYZ((f32*)&curPos, m_matrix.T());
			if ( m_frameCount + 1 == newFrameCount )
			{
				const f32 alpha = 0.5f;
				speed = curPos - m_prevPos;
				m_prevPos = m_prevPos * (1-alpha) + curPos * alpha;
			}
			else
			{
				// if emitter was not active last frame, we can't compute speed.
				speed.set(0.0f, 0.0f, 0.0f);
				m_prevPos = curPos;
			}


			// Update modifiers.
			u32 nbMod = m_modList.size();
			for ( ux i = 0; i < nbMod; i++ )
			{
				GFX_GridFluidModifier &mod = m_modList[i];
				mod.update(m_curTime, speed, m_matrix);
			}

			m_frameCount = newFrameCount;
		}
	}

	bbool GFX_GridFluidModifierList::isValid() const
	{
		if (!m_name.isValid())
			return false;
		u32 nbMod = m_modList.size();
		for ( ux i = 0; i < nbMod; i++ )
		{
			const GFX_GridFluidModifier &mod = m_modList[i];
			if ( ! mod.isValid(m_curTime) )
				return false;
		}
		return true;
	}

	// Copy from other modifier list.
	void GFX_GridFluidModifierList::operator= (const GFX_GridFluidModifierList &_src)
	{
		m_matrix = _src.m_matrix;
		m_name = _src.m_name;
		m_modList = _src.m_modList;
	}

} // namespace ITF


#endif // GFX_USE_GRID_FLUIDS
