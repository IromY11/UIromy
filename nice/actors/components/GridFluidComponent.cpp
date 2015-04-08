#include "precompiled_engine.h"
#ifdef GFX_USE_GRID_FLUIDS

#ifndef _ITF_GRIDFLUIDCOMPONENT_H_
#include "engine/actors/components/GridFluidComponent.h"
#endif //_ITF_GRIDFLUIDCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(GridFluidComponent_Template)

    BEGIN_SERIALIZATION_CHILD(GridFluidComponent_Template)
    END_SERIALIZATION()

    GridFluidComponent_Template::GridFluidComponent_Template()
        : Super()
    {
    }

    GridFluidComponent_Template::~GridFluidComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(GridFluidComponent)

    BEGIN_SERIALIZATION_CHILD(GridFluidComponent)
      BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
        SERIALIZE_MEMBER("AttractForce",m_fluid.m_attractForce);
        SERIALIZE_MEMBER("AttractExpand",m_fluid.m_attractExpand);
        SERIALIZE_MEMBER("AttractColorize",m_fluid.m_attractColorize);
		SERIALIZE_OBJECT("AttractNoise",m_fluid.m_attractNoise);
        SERIALIZE_MEMBER("NbIter",m_fluid.m_nbIter);
        SERIALIZE_MEMBER("PressureDiffusion",m_fluid.m_pressureDiffusion);
        SERIALIZE_MEMBER("PressureIntensity",m_fluid.m_pressureIntensity);
	  END_CONDITION_BLOCK()
      BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("BoxSize",m_fluid.m_boxSize);
		SERIALIZE_MEMBER("Offset",m_fluid.m_mainRender.m_offset);
		SERIALIZE_BOOL("Reinit",m_fluid.m_reinit);
        SERIALIZE_BOOL("Active",m_fluid.m_active);
        SERIALIZE_BOOL("Pause",m_fluid.m_pause);
        SERIALIZE_MEMBER("ParticleTexSizeX",m_fluid.m_particleTexSizeX);
        SERIALIZE_MEMBER("ParticleTexSizeY",m_fluid.m_particleTexSizeY);
        SERIALIZE_MEMBER("SpeedTexSizeX",m_fluid.m_speedTexSizeX);
        SERIALIZE_MEMBER("SpeedTexSizeY",m_fluid.m_speedTexSizeY);

		SERIALIZE_MEMBER("Weight",m_fluid.m_weight);
		SERIALIZE_MEMBER("Viscosity",m_fluid.m_viscosity);
        SERIALIZE_MEMBER("FluidDiffusion",m_fluid.m_fluidDiffusion);
        SERIALIZE_MEMBER("FluidLoss",m_fluid.m_FluidLoss);
        SERIALIZE_MEMBER("VelocityLoss",m_fluid.m_VelocityLoss);
		SERIALIZE_OBJECT("FluidNoise",m_fluid.m_fluidNoise);
		SERIALIZE_OBJECT("VelocityNoise",m_fluid.m_velocityNoise);
		SERIALIZE_OBJECT("EmitterFactors", m_fluid.m_emitterFactors);
        SERIALIZE_GFX_GRID_FILTER("RequiredFilter",m_fluid.m_requiredFilter);
        SERIALIZE_GFX_GRID_FILTER("RejectFilter",m_fluid.m_rejectFilter);

        SERIALIZE_ENUM_GFX_GRID_RENDER_MODE("RenderMode",m_fluid.m_renderMode);
		SERIALIZE_GFX_BLENDMODE2("BlendMode",m_fluid.m_mainRender.m_blendMode);
		SERIALIZE_MEMBER("FluidCol",m_fluid.m_mainRender.m_fluidCol);
		SERIALIZE_MEMBER("ColorTex",m_fluid.m_mainRender.m_colorTexPath);
        SERIALIZE_MEMBER("MaskTexture",m_fluid.m_maskPath);
        SERIALIZE_BOOL("UseRGBFluid",m_fluid.m_useRGBFluid);
        SERIALIZE_MEMBER("NeutralColor",m_fluid.m_neutralColor);
		SERIALIZE_OBJECT("FlowTexture",m_fluid.m_mainRender.m_flowTexture);
		SERIALIZE_OBJECT("DuDvTexture",m_fluid.m_mainRender.m_dudvTexture);
		SERIALIZE_CONTAINER_OBJECT("AdditionnalRender",m_fluid.m_addRenderList);
		SERIALIZE_OBJECT("PrimitiveParam", *m_fluid.getCommonParam());
	  END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(GridFluidComponent)
    END_VALIDATE_COMPONENT()

    GridFluidComponent::GridFluidComponent()
        : Super()
    {
    }

    GridFluidComponent::~GridFluidComponent()
    {
    }


    void GridFluidComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

		m_densityList.clear();

		ACTOR_REGISTER_EVENT_COMPONENT(m_actor,GridFluidDensityRequest_CRC, this);

		ITF_VECTOR<GF_TEXTURE_RESSOURCE> texlist;
		m_fluid.getTextureRessourceList( texlist );

		for ( ux i = 0; i < texlist.size(); ++i )
		{
			GF_TEXTURE_RESSOURCE &tex = texlist[i];
			if ( !(*tex.m_texturePath).isEmpty() )
			{
				(*tex.m_textureRessource) = m_actor->addResource(Resource::ResourceType_Texture, (*tex.m_texturePath));
			}
		}

		updateAABB();
    }

	void GridFluidComponent::onStartDestroy( bbool _hotReload )
{
		Super::onStartDestroy(_hotReload);
	}

	void GridFluidComponent::updateAABB()
	{
		AABB box(Vec2d(-m_fluid.m_boxSize.x() * 0.5f,-m_fluid.m_boxSize.y() * 0.5f), Vec2d(m_fluid.m_boxSize.x() * 0.5f,m_fluid.m_boxSize.y() * 0.5f));
		box.Scale(m_actor->getScale());
		box.Translate(m_actor->get2DPos());
		m_actor->growAABB(box);
	}

	void GridFluidComponent::Update( const f32 _dt )
    {
        Super::Update(_dt);

		m_fluid.m_boxPos = m_actor->getPos();

		updateAABB();

		for(ITF_MAP<ActorRef, DensityStruct>::iterator it = m_densityList.begin(); it != m_densityList.end(); it++)
		{
			f32 result = 0.0f;
			GFX_GridFluid_RequestStatus status = m_fluid.getRequestDensity(it->second.id, result);
			if(status == GRD_FLD_REQUEST_PROCESSED)
			{
				GridFluidDensityResult resultEvent;
				resultEvent.setResult(result);
				resultEvent.setActorHit(it->first);
				it->second.actorToSendTo.getActor()->onEvent(&resultEvent);
				it = m_densityList.erase(it);
			}
			else if(status == GRD_FLD_REQUEST_NOT_FOUND)
			{
				it = m_densityList.erase(it);
			}
		}
	}

    void GridFluidComponent::onEvent( Event* _event)
    {
        Super::onEvent(_event);

		if (GridFluidDensityRequest* fluidEvent = DYNAMIC_CAST(_event, GridFluidDensityRequest))
		{
			if(m_densityList.find(fluidEvent->getActorDetecting()) == m_densityList.end())
			{
				u32 id = m_fluid.sendRequestDensity(fluidEvent->getCenterPos(),fluidEvent->getHalfWidth(),fluidEvent->getHalfHeight());
				m_densityList.insert(fluidEvent->getActorDetecting(), DensityStruct(id,fluidEvent->getSender()));
			}
		}
    }

#ifdef ITF_SUPPORT_EDITOR
    void GridFluidComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (m_actor->isSelected())
        {
			// Draw edition helpers...
        }
    }

    void GridFluidComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
        Update(0.0f);
    }
#endif // ITF_SUPPORT_EDITOR

    void GridFluidComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
		Super::batchPrimitives(_views);

        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_PRERENDER_PRIM>(_views, &m_fluid, GetActor()->getDepth(), GetActor()->getRef()); 
        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_fluid, GetActor()->getDepth() + m_fluid.m_mainRender.m_offset.z(), GetActor()->getRef()); 
		ux count = m_fluid.m_addRenderList.size();
		for ( ux i = 0; i < count; ++i )
		{
			f32 z = GetActor()->getDepth() + m_fluid.m_addRenderList[i].m_offset.z() - (count-i) * F32_EPSILON * 2.0f;
			GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_fluid, z, GetActor()->getRef());
		}
    }
}

#endif
