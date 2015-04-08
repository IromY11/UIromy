#include "precompiled_engine.h"
#if (GFX_USE_LIGHTINGMODEL == 2)

#ifndef _ITF_LIGHTENVIRONEMENTCOMPONENT_H_
#include "engine/actors/components/LightEnvironementComponent.h"
#endif //_ITF_LIGHTENVIRONEMENTCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(LightEnvironementComponent_Template)

    BEGIN_SERIALIZATION_CHILD(LightEnvironementComponent_Template)
    END_SERIALIZATION()

    LightEnvironementComponent_Template::LightEnvironementComponent_Template()
        : Super()
    {
    }

    LightEnvironementComponent_Template::~LightEnvironementComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(LightEnvironementComponent)

    BEGIN_SERIALIZATION_CHILD(LightEnvironementComponent)
      BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("LightDir0",m_lightParam.m_lightDir[0]);
        SERIALIZE_MEMBER("LightDir1",m_lightParam.m_lightDir[1]);
        SERIALIZE_MEMBER("LightDir2",m_lightParam.m_lightDir[2]);
        SERIALIZE_MEMBER("LightCol0",m_lightParam.m_lightCol[0]);
        SERIALIZE_MEMBER("LightCol1",m_lightParam.m_lightCol[1]);
        SERIALIZE_MEMBER("LightCol2",m_lightParam.m_lightCol[2]);
        SERIALIZE_MEMBER("RimLightDirection",m_lightParam.m_rimLightDirection);
        SERIALIZE_MEMBER("RimLightColor",m_lightParam.m_rimLightColor);
        SERIALIZE_MEMBER("BoxSize",m_lightParam.m_boxSize);
        SERIALIZE_MEMBER("BoxFadeDist",m_lightParam.m_boxFadeDist);
        SERIALIZE_MEMBER("LightColMultiplier",m_lightParam.m_lightColMultiplier);
      END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(LightEnvironementComponent)
    END_VALIDATE_COMPONENT()

    LightEnvironementComponent::LightEnvironementComponent()
        : Super()
    {
    }

    LightEnvironementComponent::~LightEnvironementComponent()
    {
    }


    void LightEnvironementComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);
		updateAABB();
    }

    void LightEnvironementComponent::Update( const f32 _dt )
    {
        Super::Update(_dt);

		m_lightParam.m_boxPos = m_actor->getPos();
		updateAABB();
	}

    void LightEnvironementComponent::onEvent( Event* _event)
    {
        Super::onEvent(_event);
    }

	void LightEnvironementComponent::updateAABB()
	{
		AABB box(Vec2d(-m_lightParam.m_boxSize.x() * 0.5f,-m_lightParam.m_boxSize.y() * 0.5f), Vec2d(m_lightParam.m_boxSize.x() * 0.5f,m_lightParam.m_boxSize.y() * 0.5f));
		box.Scale(m_actor->getScale());
		box.Translate(m_actor->get2DPos());
		m_actor->growAABB(box);
	}

#ifdef ITF_SUPPORT_EDITOR
    void LightEnvironementComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (m_actor->isSelected())
        {
			Vec2d innerSize = m_lightParam.m_boxSize;
			innerSize.x() = ITF::Max( innerSize.x() - m_lightParam.m_boxFadeDist, 0.0f );
			innerSize.y() = ITF::Max( innerSize.y() - m_lightParam.m_boxFadeDist, 0.0f );
			Vec3d pos = m_actor->getPos();
			Vec2d pos2d(pos.x(), pos.y());
            DebugDraw::rectangle( pos2d- innerSize*0.5f, innerSize, pos.z(), Color::grey(), 2);
            DebugDraw::rectangle( pos2d-m_lightParam.m_boxSize*0.5f, m_lightParam.m_boxSize, pos.z(), Color::red(), 2);
        }
    }

    void LightEnvironementComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
        Update(0.0f);
    }
#endif // ITF_SUPPORT_EDITOR

    void LightEnvironementComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
		Super::batchPrimitives(_views);

        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_LIGHT_3D>(_views, &m_lightParam, GetActor()->getDepth(), GetActor()->getRef()); 
    }
}

#endif // (GFX_USE_LIGHTINGMODEL == 2)
