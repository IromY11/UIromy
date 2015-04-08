#include "precompiled_engine.h"
#ifdef GFX_USE_GRID_FLUIDS

#ifndef _ITF_FLUIDBANKCOMPONENT_H_
#include "engine/actors/components/FluidBankComponent.h"
#endif //_ITF_FLUIDBANKCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "../actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif // _ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

namespace ITF
{

    enum
    {
        FLUIDBANKCOMPONENT_MAX_OF_MAX_NUM_FX_INSTANCES = 250, // should be kept below 32K because of the handles being limited
        FLUIDBANKCOMPONENT_DEFAULT_MAX_NUM_FX_INSTANCES = 20,
    }; 

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(FluidBankComponent_Template)
        BEGIN_SERIALIZATION_CHILD(FluidBankComponent_Template)
        SERIALIZE_CONTAINER_OBJECT("ModifierTemplates",m_modifierTemplateList);
		SERIALIZE_MEMBER("MaxActiveInstance", m_maxActiveInstance);
    END_SERIALIZATION()

    FluidBankComponent_Template::FluidBankComponent_Template() :
        m_maxActiveInstance(FLUIDBANKCOMPONENT_DEFAULT_MAX_NUM_FX_INSTANCES)
    {
    }

    FluidBankComponent_Template::~FluidBankComponent_Template()
    {
    }

    bbool FluidBankComponent_Template::onTemplateLoaded( bbool _hotReload )
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);

        m_maxActiveInstance = std::min(u32(FLUIDBANKCOMPONENT_MAX_OF_MAX_NUM_FX_INSTANCES), m_maxActiveInstance); // limit

        // Load resources assigned on this actor
		ux nbTemplate = m_modifierTemplateList.size();
		for ( ux t = 0; t < nbTemplate; t++ )
		{
			GFX_GridFluidModifierList &templateMod = m_modifierTemplateList[t];
			ux nbMod = templateMod.m_modList.size();
			for (ux i = 0; i < nbMod; i++)
			{
				GFX_GridFluidModifier &modifier = templateMod.m_modList[i];
				if ( !modifier.m_texturePath.isEmpty() )
				{
					modifier.m_textureRessource = m_actorTemplate->addResource(Resource::ResourceType_Texture, modifier.m_texturePath);
				}
			}
		}

        return bOk;
    }

    void FluidBankComponent_Template::onTemplateDelete( bbool _hotReload )
    {
        Super::onTemplateDelete(_hotReload);
    }

    IMPLEMENT_OBJECT_RTTI(FluidBankComponent)
    BEGIN_SERIALIZATION_CHILD(FluidBankComponent)
	BEGIN_CONDITION_BLOCK(ESerialize_DataRaw)
    END_CONDITION_BLOCK()
    END_SERIALIZATION()

    FluidBankComponent::FluidBankComponent() : Super()
    , m_animComponent(nullptr)
    {
    }


    FluidBankComponent::~FluidBankComponent()
    {
        clear();
    }

    void FluidBankComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = GetActor()->GetComponent<AnimLightComponent>();
    }

    void FluidBankComponent::onBecomeInactive()
    {
		m_modifierInstanceList.clear();
		m_paramInstanceList.clear();
    }

    void FluidBankComponent::Update( f32 _deltaTime )
    {
        Super::Update( _deltaTime );

		u32 nbInst = m_modifierInstanceList.size();
		for (ux i = 0; i < nbInst; i++)
		{
			GFX_GridFluidModifierList &instanceMod = m_modifierInstanceList[i];
			playFluidParams &params = m_paramInstanceList[i];
			if ( params.m_attach )
			{
				if ( instanceMod.isValid() )
				{
					updateInstanceMatrix(instanceMod, params);
				}
				else
					params.m_attach = false;
			}
		}
    }

    void FluidBankComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
		u32 nbInst = m_modifierInstanceList.size();
        for (u32 i = 0; i < nbInst; i++)
        {
			GFX_GridFluidModifierList &instanceMod = m_modifierInstanceList[i];
			if ( instanceMod.isValid() )
			{
				f32 translation[3];
				storeXYZ(translation, instanceMod.m_matrix.T());
				GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_GFLUID_MODIFIER>(_views, &instanceMod, translation[2], GetActor()->getRef()); 
			}
		}
    }

    void FluidBankComponent::clear()
    {
		m_modifierInstanceList.clear();
		m_paramInstanceList.clear();
    }

	void FluidBankComponent::updateInstanceMatrix( GFX_GridFluidModifierList &_modifierInstance, const playFluidParams &_params )
	{
		bbool fliped =  m_actor->getIsFlipped();
		Vec3d pos = m_actor->getPos();
        f32 angle = _params.m_angle < F32_INFINITY? _params.m_angle : 0.0f;               //world angle
		if (_params.m_boneIndex != U32_INVALID)
		{
			if (m_animComponent)
			{
				if (m_animComponent->getBonePos(_params.m_boneIndex, pos))
				{
					if ( _params.m_useBoneOrientation )
					{
						m_animComponent->getBoneAngle(_params.m_boneIndex, angle, false);
					}
				}
			}
		}
		else if ( _params.m_emitFromBase )
		{
			pos = m_actor->getBase();
		}
        else if (_params.m_pos.x() < F32_INFINITY)
        {
            pos = _params.m_pos;
        }

		if ( _params.m_useActorOrientation )
		{
			angle = m_actor->getAngle();
		}

		_modifierInstance.m_matrix.setIdentity();
		_modifierInstance.m_matrix.setRotationZ(angle);
		_modifierInstance.m_matrix.setTranslation(pos);
		Vec3d scale( 1.0f, 1.0f, 1.0f );
		if (fliped && _params.m_angle >= F32_INFINITY)
			scale.x() *= -1.f;
		_modifierInstance.m_matrix.mulScale(scale);
	}

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    FluidHandle FluidBankComponent::playFluid( StringID _name, const playFluidParams& _params)
    {
		FluidHandle fh = InvalidFluidHandle;
		const FluidBankComponent_Template* fluidBankTemplate = getTemplate();
		if ( m_modifierInstanceList.size() >= fluidBankTemplate->m_maxActiveInstance )
			return fh;
		ux nbTemplate = fluidBankTemplate->m_modifierTemplateList.size();
		for ( ux t = 0; t < nbTemplate; t++ )
		{
			const GFX_GridFluidModifierList &templateMod = fluidBankTemplate->m_modifierTemplateList[t];
			if ( ( templateMod.m_name == _name ) && templateMod.isValid() )
			{
				// Search a unused instance or alloc a new one.
				u32 nbInst = m_modifierInstanceList.size();
				u32 i;
				for (i = 0; i < nbInst; i++)
				{
					GFX_GridFluidModifierList &instanceMod = m_modifierInstanceList[i];
					if ( !instanceMod.isValid() )
					{
						instanceMod = templateMod;
						m_paramInstanceList[i] = _params;
						fh = i;
						break;
					}
				}
				if ( i >= nbInst )
				{
					// Not found, alloc...
					m_modifierInstanceList.push_back(templateMod);
					m_paramInstanceList.push_back(_params);
					fh = m_modifierInstanceList.size() - 1;
				}
			}
		}

		if ( fh != InvalidFluidHandle )
		{
			GFX_GridFluidModifierList &instanceMod = m_modifierInstanceList[fh];
			updateInstanceMatrix( instanceMod, _params );
			instanceMod.reInit();
		}

		return fh;
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    void FluidBankComponent::stopFluid(FluidHandle _hdl)
    {
		if ( _hdl < m_modifierInstanceList.size() )
		{
			GFX_GridFluidModifierList &instanceMod = m_modifierInstanceList[_hdl];
			instanceMod.invalidate();
			instanceMod.m_modList.clear();
			m_paramInstanceList[_hdl].m_attach = false;
		}
    }

    void FluidBankComponent::setFXPos( FluidHandle _hdl, Vec3d _pos)
    {
        if ( _hdl < m_paramInstanceList.size() )
        {
            m_paramInstanceList[_hdl].m_pos = _pos;
            m_paramInstanceList[_hdl].m_attach = btrue;
            updateInstanceMatrix( m_modifierInstanceList[_hdl], m_paramInstanceList[_hdl] );
        }
    }

    void FluidBankComponent::setFXAngle( FluidHandle _hdl, f32 _angle )
    {
        if ( _hdl < m_paramInstanceList.size() )
        {
            m_paramInstanceList[_hdl].m_angle = _angle;
            m_paramInstanceList[_hdl].m_attach = btrue;
            updateInstanceMatrix( m_modifierInstanceList[_hdl], m_paramInstanceList[_hdl] );
        }
    }
}

#endif
