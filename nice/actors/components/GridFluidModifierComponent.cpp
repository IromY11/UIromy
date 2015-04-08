#include "precompiled_engine.h"
#ifdef GFX_USE_GRID_FLUIDS

#ifndef _ITF_GRIDFLUIDMODIFIERCOMPONENT_H_
#include "engine/actors/components/GridFluidModifierComponent.h"
#endif //_ITF_GRIDFLUIDMODIFIERCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_ACTORUTILS_H_
#include "engine/Actors/ActorUtils.h"
#endif //_ITF_ACTORUTILS_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(GridFluidModifierComponent_Template)

    BEGIN_SERIALIZATION_CHILD(GridFluidModifierComponent_Template)
    END_SERIALIZATION()

    GridFluidModifierComponent_Template::GridFluidModifierComponent_Template()
        : Super()
    {
    }

    GridFluidModifierComponent_Template::~GridFluidModifierComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(GridFluidModifierComponent)

    BEGIN_SERIALIZATION_CHILD(GridFluidModifierComponent)
      BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("Reinit", m_reinit)
        SERIALIZE_OBJECT("FluidModifier", m_modifierList)
	  END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(GridFluidModifierComponent)
    END_VALIDATE_COMPONENT()

    GridFluidModifierComponent::GridFluidModifierComponent()
        : Super()
    {
		m_reinit = false;
    }

    GridFluidModifierComponent::~GridFluidModifierComponent()
    {
    }


    void GridFluidModifierComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

		if ( m_modifierList.m_modList.size() == 0 )
			m_modifierList.addOneModList();

		ux nbMod = m_modifierList.m_modList.size();
		for (ux i = 0; i < nbMod; i++)
		{
			GFX_GridFluidModifier &modifier = m_modifierList.m_modList[i];
			if ( !modifier.m_texturePath.isEmpty() )
			{
				modifier.m_textureRessource = m_actor->addResource(Resource::ResourceType_Texture, modifier.m_texturePath);
			}
		}

		getPickableMatrix(m_actor, m_modifierList.m_matrix);
		m_modifierList.reInit();

		updateAABB();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventFluidModifier_CRC,this);
	}

	void GridFluidModifierComponent::updateAABB()
	{
		const u32 margin = 10;
		ux nbMod = m_modifierList.m_modList.size();
		for (ux i = 0; i < nbMod; i++)
		{
			GFX_GridFluidModifier &modifier = m_modifierList.m_modList[i];
			AABB box(Vec2d(-modifier.m_boxSize.x() * 0.5f - margin,-modifier.m_boxSize.y() * 0.5f - margin), Vec2d(modifier.m_boxSize.x() * 0.5f + margin,modifier.m_boxSize.y() * 0.5f + margin));
			box.Scale(m_actor->getScale());
			box.Translate(m_actor->get2DPos());
			m_actor->growAABB(box);
		}
	}

	void GridFluidModifierComponent::Update( const f32 _dt )
    {
        Super::Update(_dt);

		getPickableMatrix(m_actor, m_modifierList.m_matrix);

		if ( m_reinit )
		{
			m_reinit = false;
			m_modifierList.reInit();
		}
		
		updateAABB();
	}

    void GridFluidModifierComponent::onEvent( Event* _event)
    {
        Super::onEvent(_event);

		EventFluidModifier * evt = DYNAMIC_CAST(_event,EventFluidModifier);
        if (evt)
        {
            if (evt->getActivated())
                m_modifierList.m_active = true;
            else
                m_modifierList.m_active = false;
        }
}

#ifdef ITF_SUPPORT_EDITOR
    void GridFluidModifierComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (m_actor->isSelected())
        {
			// Draw edition helpers...
        }
    }

    void GridFluidModifierComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
        Update(0.0f);
    }
#endif // ITF_SUPPORT_EDITOR

    void GridFluidModifierComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
		Super::batchPrimitives(_views);

		GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_GFLUID_MODIFIER>(_views, &m_modifierList, GetActor()->getDepth(), GetActor()->getRef()); 
    }

    u32 GridFluidModifierComponent::getModifierSize( )
    {
        return m_modifierList.size();
    }
    
	void GridFluidModifierComponent::setActive( u16 idx, bbool _active )
	{
		if(idx < m_modifierList.size())
		{
			m_modifierList[idx]->m_active = _active;
            if (_active)
			    m_modifierList[idx]->m_startDelay += m_modifierList.m_curTime;
		}
	}
    
	void GridFluidModifierComponent::setActiveAll(bbool _active)
	{
		for(u32 i = 0; i < m_modifierList.size(); i++)
		{
			m_modifierList[i]->m_active = _active;
            if (_active)
			    m_modifierList[i]->m_startDelay += m_modifierList.m_curTime;
		}
	}

	void GridFluidModifierComponent::setFluidPosition( u32 idx, Vec3d _pos )
	{
		if(idx < m_modifierList.size())
			m_modifierList[idx]->m_boxPos = _pos;
	}

    ITF_INLINE void GridFluidModifierComponent::setUpdateDisabled( bbool _val )
    {
         Super::setUpdateDisabled(_val); 
         m_modifierList.m_active = !_val;
    }

}

#endif
