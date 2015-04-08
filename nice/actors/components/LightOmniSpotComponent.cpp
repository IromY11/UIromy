#include "precompiled_engine.h"
#if (GFX_USE_LIGHTINGMODEL == 2)

#ifndef _ITF_LIGHTOMNISPOTCOMPONENT_H_
#include "engine/actors/components/LightOmniSpotComponent.h"
#endif //_ITF_LIGHTOMNISPOTCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(LightOmniSpotComponent_Template)

    BEGIN_SERIALIZATION_CHILD(LightOmniSpotComponent_Template)
    END_SERIALIZATION()

    LightOmniSpotComponent_Template::LightOmniSpotComponent_Template()
        : Super()
    {
    }

    LightOmniSpotComponent_Template::~LightOmniSpotComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(LightOmniSpotComponent)

    BEGIN_SERIALIZATION_CHILD(LightOmniSpotComponent)
      BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("LightCol",m_lightParam.m_lightCol);
        SERIALIZE_MEMBER("Near",m_lightParam.m_near);
        SERIALIZE_MEMBER("Far",m_lightParam.m_far);
        SERIALIZE_MEMBER("SpotDir",m_lightParam.m_spotDir);
        SERIALIZE_MEMBER("SpotLittleAngle",m_lightParam.m_spotLittleAngle);
        SERIALIZE_MEMBER("SpotBigAngle",m_lightParam.m_spotBigAngle);
        SERIALIZE_MEMBER("LightColMultiplier",m_lightParam.m_lightColMultiplier);
      END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(LightOmniSpotComponent)
    END_VALIDATE_COMPONENT()

    LightOmniSpotComponent::LightOmniSpotComponent()
        : Super()
    {
    }

    LightOmniSpotComponent::~LightOmniSpotComponent()
    {
    }


    void LightOmniSpotComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);
		updateAABB();
    }

	void LightOmniSpotComponent::updateAABB()
	{
		AABB box(Vec2d(-m_lightParam.m_far,-m_lightParam.m_far), Vec2d(m_lightParam.m_far,m_lightParam.m_far));
		box.Scale(m_actor->getScale());
		box.Translate(m_actor->get2DPos());
		m_actor->growAABB(box);
	}

	void LightOmniSpotComponent::Update( const f32 _dt )
    {
        Super::Update(_dt);

		m_lightParam.m_boxPos = m_actor->getPos();
		m_lightParam.m_boxSize.set(m_lightParam.m_far, m_lightParam.m_far);

		updateAABB();
	}

    void LightOmniSpotComponent::onEvent( Event* _event)
    {
        Super::onEvent(_event);
    }

#ifdef ITF_SUPPORT_EDITOR
    void LightOmniSpotComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (m_actor->isSelected())
        {
			if ( ( !m_lightParam.m_spotDir.isNullEpsilon() ) && ( m_lightParam.m_spotBigAngle > 0.0f ) )
			{
				Vec3d spotDir = m_lightParam.m_spotDir;
				spotDir.normalize();
				Vec3d posDest = m_actor->getPos();
				posDest += spotDir * (CAMERA->getZ()-m_actor->getPos().z()) * 0.1f;
				DebugDraw::arrow3D( m_actor->getPos(), posDest, Color::orange(), Color::orange(), 5.0f, 40.f, 30.f, 1.0f);

				posDest = m_actor->getPos();
				posDest += spotDir * m_lightParam.m_far;
				DebugDraw::cone( m_actor->getPos(), posDest, 0.f, ::tan(m_lightParam.m_spotLittleAngle) * m_lightParam.m_far, Color::grey(), 1.f );
				DebugDraw::cone( m_actor->getPos(), posDest, 0.f, ::tan(m_lightParam.m_spotBigAngle) * m_lightParam.m_far, Color::yellow(), 1.f );
			}
			else
			{
				DebugDraw::circle( m_actor->getPos(), m_lightParam.m_near, Vec3d::XAxis, Vec3d::YAxis, 16, Color::grey());
				DebugDraw::circle( m_actor->getPos(), m_lightParam.m_far, Vec3d::XAxis, Vec3d::YAxis, 16, Color::yellow());
			}
        }
    }

    void LightOmniSpotComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
        Update(0.0f);
    }
#endif // ITF_SUPPORT_EDITOR

    void LightOmniSpotComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
		Super::batchPrimitives(_views);

        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_LIGHT_3D>(_views, &m_lightParam, GetActor()->getDepth(), GetActor()->getRef()); 
    }
}

#endif // (GFX_USE_LIGHTINGMODEL == 2)
