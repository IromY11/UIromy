#include "precompiled_engine.h"
#ifdef GFX_USE_GRID_FLUIDS

#ifndef _ITF_GRIDFLUIDLISTCOMPONENT_H_
#include "engine/actors/components/GridFluidListComponent.h"
#endif //_ITF_GRIDFLUIDLISTCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include    "engine/animation/AnimationTrack.h"
#endif // _ITF_ANIMATIONTRACK_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(GridFluidListComponent_Template)

    BEGIN_SERIALIZATION_CHILD(GridFluidListComponent_Template)
    END_SERIALIZATION()

    GridFluidListComponent_Template::GridFluidListComponent_Template()
        : Super()
    {
    }

    GridFluidListComponent_Template::~GridFluidListComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(GridFluidListComponent)

    BEGIN_SERIALIZATION_CHILD(GridFluidListComponent)
      BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
         SERIALIZE_CONTAINER_OBJECT("GridList",m_fluidList);
         SERIALIZE_MEMBER("DelayDesactivate", m_delayDesactivate)
	  END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(GridFluidListComponent)
    END_VALIDATE_COMPONENT()

    GridFluidListComponent::GridFluidListComponent()
        : Super()
        , m_delayDesactivate(10.0f)
    {
    }

    GridFluidListComponent::~GridFluidListComponent()
    {
    }


    void GridFluidListComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);


		ACTOR_REGISTER_EVENT_COMPONENT(m_actor,AnimGameplayEvent_CRC, this);
		ACTOR_REGISTER_EVENT_COMPONENT(m_actor,GridFluidDensityRequest_CRC, this);

#ifdef GRID_MAP
        for (ITF_MAP<StringID, GFX_GridFluid>::iterator it = m_fluidList.begin(); it != m_fluidList.end(); it++)
        {
			ITF_VECTOR<GF_TEXTURE_RESSOURCE> texlist;
			(it->second).getTextureRessourceList( texlist );

			for ( ux i = 0; i < texlist.size(); ++i )
			{
				GF_TEXTURE_RESSOURCE &tex = texlist[i];
				if ( !(*tex.m_texturePath).isEmpty() )
				{
					(*tex.m_textureRessource) = m_actor->addResource(Resource::ResourceType_Texture, (*tex.m_texturePath));
				}
			}
        }
#else
        for (ITF_VECTOR<GFX_GridFluid>::iterator it = m_fluidList.begin(); it != m_fluidList.end(); it++)
        {
			ITF_VECTOR<GF_TEXTURE_RESSOURCE> texlist;
			it->getTextureRessourceList( texlist );

			for ( ux i = 0; i < texlist.size(); ++i )
			{
				GF_TEXTURE_RESSOURCE &tex = texlist[i];
				if ( !(*tex.m_texturePath).isEmpty() )
				{
					(*tex.m_textureRessource) = m_actor->addResource(Resource::ResourceType_Texture, (*tex.m_texturePath));
				}
			}
        }
#endif
        
		

		updateAABB();
    }


    void GridFluidListComponent::onStartDestroy( bbool _hotReload )
    {
        Super::onStartDestroy(_hotReload);
    }


	void GridFluidListComponent::updateAABB()
	{
#ifdef GRID_MAP
        for (ITF_MAP<StringID, GFX_GridFluid>::iterator it = m_fluidList.begin(); it != m_fluidList.end(); it++)
        {
            AABB box(Vec2d(-(it->second).m_boxSize.x() * 0.5f,-(it->second).m_boxSize.y() * 0.5f), Vec2d((it->second).m_boxSize.x() * 0.5f,(it->second).m_boxSize.y() * 0.5f));
            box.Scale(m_actor->getScale());
            box.Translate(m_actor->get2DPos());
            m_actor->growAABB(box);
        }
#else
        for (ITF_VECTOR<GFX_GridFluid>::iterator it = m_fluidList.begin(); it != m_fluidList.end(); it++)
        {
            AABB box(Vec2d(- it->m_boxSize.x() * 0.5f,- it->m_boxSize.y() * 0.5f), Vec2d( it->m_boxSize.x() * 0.5f, it->m_boxSize.y() * 0.5f));
            box.Scale(m_actor->getScale());
            box.Translate(m_actor->get2DPos());
            m_actor->growAABB(box);
        }
#endif
        
	}

	void GridFluidListComponent::Update( const f32 _dt )
    {
        Super::Update(_dt);

#ifdef GRID_MAP
        for (ITF_MAP<StringID, GFX_GridFluid>::iterator it = m_fluidList.begin(); it != m_fluidList.end(); it++)
        {
            (it->second).m_boxPos = m_actor->getPos();
        }
#else
        for (ITF_VECTOR<GFX_GridFluid>::iterator it = m_fluidList.begin(); it != m_fluidList.end(); it++)
        {
            it->m_boxPos = m_actor->getPos();
        }
#endif
        
		bbool breaking = bfalse;
		for(ITF_MAP<ActorRef, DensityStructList>::iterator it = m_densityList.begin(); it != m_densityList.end(); it++)
		{
			f32 result = 0.0f;
			u32 counter = 0;
			for(ITF_VECTOR<GFX_GridFluid>::iterator it2 = m_fluidList.begin(); it2 != m_fluidList.end(); it2++)
			{
				f32 tempResult = 0.0f;
				GFX_GridFluid_RequestStatus status = (it2)->getRequestDensity(it->second.ids[counter], tempResult);
				if(status == GRD_FLD_REQUEST_PROCESSED)
				{
					result += tempResult;
					counter++;
				}
				else 
				{
					if(status == GRD_FLD_REQUEST_NOT_FOUND)
						it = m_densityList.erase(it);
					breaking = btrue;
					break;
				}
			}
			if(breaking)
				break;

			GridFluidDensityResult resultEvent;
			resultEvent.setResult(result);
			resultEvent.setActorHit(it->first);
			it->second.actorToSendTo.getActor()->onEvent(&resultEvent);
			it = m_densityList.erase(it);
		}

        updateDeactivate(_dt);

		updateAABB();
	}

    void GridFluidListComponent::onEvent( Event* _event)
    {
        Super::onEvent(_event);

        if (AnimGameplayEvent* gameplayEvent = DYNAMIC_CAST(_event, AnimGameplayEvent))
        {
            if (gameplayEvent->getName() == MRK_ActivateGrid)
            {
                // if the value has 1000, means to use the animated component. The reste is the index of the grid
                u32 gridIndex = (u32)f32_Modulo(gameplayEvent->getValue(), 1000.0f);
                bbool useAnim = ((u32)gameplayEvent->getValue() - gridIndex == 1000);
                ITF_MAP<u32, f32>::iterator it = m_pendingDeactivate.find(gridIndex);
                if(it != m_pendingDeactivate.end())
                {
                    m_pendingDeactivate.erase(it);
                }
                
                setAcivateGrid(gridIndex, btrue, useAnim);
            }
            else if (gameplayEvent->getName() == MRK_DeactivateGrid)
            {
                u32 gridIndex = (u32)f32_Modulo(gameplayEvent->getValue(), 1000.0f);
                m_pendingDeactivate[gridIndex] = m_delayDesactivate;


                AnimLightComponent* animLight = m_actor->GetComponent<AnimLightComponent>();
                u32 required = animLight->getFluidEmitterFilter().getValue();
                required = required - (required & m_fluidList[gridIndex].m_requiredFilter.getValue());

                //if(required == 0)
                {
                    animLight->setFluidEmitterFilter(required);
                }


            }
        }
		if (GridFluidDensityRequest* fluidEvent = DYNAMIC_CAST(_event, GridFluidDensityRequest))
		{
			if(m_densityList.find(fluidEvent->getActorDetecting()) == m_densityList.end())
			{
				m_densityList.insert(fluidEvent->getActorDetecting(), DensityStructList(fluidEvent->getSender()));
				DensityStructList* densStruct = &(m_densityList.find(fluidEvent->getSender())->second);
				for(ITF_VECTOR<GFX_GridFluid>::iterator it = m_fluidList.begin(); it != m_fluidList.end(); it++)
				{
					u32 id = it->sendRequestDensity(fluidEvent->getCenterPos(),fluidEvent->getHalfWidth(),fluidEvent->getHalfHeight());
					densStruct->ids.push_back(id);
				}
			}
		}
    }

#ifdef ITF_SUPPORT_EDITOR
    void GridFluidListComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (m_actor->isSelected())
        {
			// Draw edition helpers...
        }
    }

    void GridFluidListComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
        Update(0.0f);
    }
#endif // ITF_SUPPORT_EDITOR

    void GridFluidListComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
		Super::batchPrimitives(_views);

#ifdef GRID_MAP
        for (ITF_MAP<StringID, GFX_GridFluid>::iterator it = m_fluidList.begin(); it != m_fluidList.end(); it++)
        {
            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_PRERENDER_PRIM>(_views, &(it->second), GetActor()->getDepth(), GetActor()->getRef()); 
            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &(it->second), GetActor()->getDepth() + it->second.m_mainRender.m_offset.z(), GetActor()->getRef()); 
            ux count = (it->second).m_addRenderList.size();
            for ( ux i = 0; i < count; ++i )
            {
                f32 z = GetActor()->getDepth() + (it->second).m_addRenderList[i].m_offset.z() - (count-i) * F32_EPSILON * 2.0f;
                GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &(it->second), z, GetActor()->getRef());
            }
        }
#else
        for (ITF_VECTOR<GFX_GridFluid>::iterator it = m_fluidList.begin(); it != m_fluidList.end(); it++)
        {
            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_PRERENDER_PRIM>(_views, &(*it), GetActor()->getDepth(), GetActor()->getRef()); 
            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &(*it), GetActor()->getDepth() + (*it).m_mainRender.m_offset.z(), GetActor()->getRef()); 
            ux count = it->m_addRenderList.size();
            for ( ux i = 0; i < count; ++i )
            {
                f32 z = GetActor()->getDepth() + it->m_addRenderList[i].m_offset.z() - (count-i) * F32_EPSILON * 2.0f;
                GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &(*it), z, GetActor()->getRef());
            }
        }
#endif
        
    }

    void GridFluidListComponent::setUpdateDisabled( bbool _val )
    {
        Super::setUpdateDisabled(_val); 

#ifdef GRID_MAP
        for (ITF_MAP<StringID, GFX_GridFluid>::iterator it = m_fluidList.begin(); it != m_fluidList.end(); it++)
        {
            (it->second).m_active = !_val;
        }
#else
        for (ITF_VECTOR<GFX_GridFluid>::iterator it = m_fluidList.begin(); it != m_fluidList.end(); it++)
        {
            it->m_active = !_val;
        }
#endif
    }

    void GridFluidListComponent::setAcivateGrid( u32 _gridIndex, bbool _active, bbool _useAnim )
    {
        if(_gridIndex >= m_fluidList.size())
            return;

        m_fluidList[_gridIndex].m_active = _active;

        if(_useAnim)
        {
            AnimLightComponent* animLight = m_actor->GetComponent<AnimLightComponent>();

            u32 required = animLight->getFluidEmitterFilter().getValue();

            if (_active)
            {
                required |= m_fluidList[_gridIndex].m_requiredFilter.getValue();
                animLight->setEmitFluid(btrue);
            }
            else
            {
                required = required - (required & m_fluidList[_gridIndex].m_requiredFilter.getValue());
            }

            animLight->setFluidEmitterFilter(required);
        }
        
    }

    void GridFluidListComponent::updateDeactivate(f32 _dt)
    {
        for (ITF_MAP<u32, f32>::iterator it = m_pendingDeactivate.begin();it != m_pendingDeactivate.end();)
        {
            it->second -= _dt;
            if (it->second <= 0.0f)
            {
                setAcivateGrid(it->first, bfalse, btrue);
                it = m_pendingDeactivate.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

}

#endif
