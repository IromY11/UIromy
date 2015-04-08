#include "precompiled_engine.h"

#ifndef _ITF_AFXPOSTPROCESSCOMPONENT_H_
#include "engine/actors/components/AFXPostProcessComponent.h"
#endif //_ITF_AFXPOSTPROCESSCOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(AFXPostProcessComponent)
    BEGIN_SERIALIZATION_CHILD(AFXPostProcessComponent)
        SERIALIZE_OBJECT("blur", m_blur);
        SERIALIZE_OBJECT("glow", m_glow);
        SERIALIZE_OBJECT("colorSetting", m_colorSetting);
        SERIALIZE_OBJECT("refraction", m_refraction);
        SERIALIZE_OBJECT("tile", m_tile);
        SERIALIZE_OBJECT("mosaic", m_mosaic);
        SERIALIZE_OBJECT("negatif", m_negatif);
        SERIALIZE_OBJECT("kaleidoscope", m_kalei);
        SERIALIZE_OBJECT("eyeFish", m_eyeFish);
        SERIALIZE_OBJECT("mirror", m_mirror);
        SERIALIZE_OBJECT("oldTV", m_oldTV);
        SERIALIZE_OBJECT("noise", m_noise);
        SERIALIZE_OBJECT("radial", m_radial);
   END_SERIALIZATION();
    
    AFXPostProcessComponent::AFXPostProcessComponent()
    {
        m_inputColor.setType(InputType_F32);
        m_inputColor.setValue(0.0f);
        m_inputFactor.setType(InputType_F32);
        m_inputFactor.setValue(1.0f);
        m_afterFx = newAlloc(mId_GfxAdapter, AFXPostProcess);
        m_isPause = bfalse;
        m_activated = bfalse;
		m_viewportVisibility = 0x0000ffff;
    }

    AFXPostProcessComponent::~AFXPostProcessComponent()
    {
        SF_DEL(m_afterFx);
    }

    void AFXPostProcessComponent::checkAndUpdateActived( )
    {
        f32 fadeFactor = 0;
        getInputColor(fadeFactor);

        bbool needActivation = (m_blur.m_use
            || m_glow.m_use
            || m_colorSetting.m_use
            || m_refraction.m_use
            || m_tile.m_use
            || m_mosaic.m_use
            || m_negatif.m_use
            || m_kalei.m_use
            || m_eyeFish.m_use
            || m_mirror.m_use
            || m_oldTV.m_use
            || m_noise.m_use
            || m_radial.m_use) && (fadeFactor > 0.0f);
        m_activated = needActivation;
    }

#ifdef ITF_SUPPORT_EDITOR
    void AFXPostProcessComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);
    }

    void AFXPostProcessComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
        Update(0.0f);
    }

#endif // ITF_SUPPORT_EDITOR

    void AFXPostProcessComponent::updatePrimitive()
    {
        if (m_activated)
        {
            f32 fadeFactor = 0;
            getInput(fadeFactor);
            m_afterFx->setFadeFactor(fadeFactor);

            //update instance data
			m_afterFx->setViewportVisibility(m_viewportVisibility);
            m_afterFx->setBlurParam(m_blur);
            m_afterFx->setGlowParam(m_glow);
            m_afterFx->setColorSettingParam(m_colorSetting);
            m_afterFx->setRefraction(m_refraction); 
            m_afterFx->setTileParam(m_tile); 
            m_afterFx->setMosaicParam(m_mosaic); 
            m_afterFx->setNegatifParam(m_negatif); 
            m_afterFx->setKaleiParam(m_kalei); 
            m_afterFx->setEyeFishParam(m_eyeFish); 
            m_afterFx->setMirrorParam(m_mirror); 
            m_afterFx->setOldTVParam(m_oldTV); 
            m_afterFx->setNoiseParam(m_noise); 
            m_afterFx->setRadialParam(m_radial); 

            updateAABB();
        }              
    }

    void AFXPostProcessComponent::Update( f32 _deltaTime )
    {
        checkAndUpdateActived();
        updatePrimitive();
    }

    void AFXPostProcessComponent::batchPrimitives2D( const ITF_VECTOR <class View*>& _views )
    {
        Super::batchPrimitives2D(_views);

        if (m_activated)
        {
            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_AFTERFX>(_views, m_afterFx, m_actor->getDepth(), ObjectRef::InvalidRef);
        }
    }
    
    void AFXPostProcessComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
        Super::batchPrimitives(_views);

        if (m_activated)
        {
            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_AFTERFX>(_views, m_afterFx, m_actor->getDepth(), ObjectRef::InvalidRef);
        }
    }

    void AFXPostProcessComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        updateAABB();

        // Input color
        const InputDesc& inputColorDesc = getTemplate()->m_inputColor;
        if(inputColorDesc.getId().isValid())
        {
            m_inputColor.setId(inputColorDesc.getId());
            m_inputColor.setType(inputColorDesc.getType());

            if ( inputColorDesc.getType() == InputType_F32 )
            {
                m_inputColor.setValue(0.f);
            }
            else
            {
                m_inputColor.setValue(static_cast<u32>(0));
            }
        }

        // Input factor
        const InputDesc& inputFactorDesc = getTemplate()->m_inputFactor;
        if(inputFactorDesc.getId().isValid())
        {
            m_inputFactor.setId(inputFactorDesc.getId());
            if ( inputColorDesc.getType() == InputType_F32 )
            {
                m_inputColor.setValue(0.f);
            }
            else
            {
                m_inputColor.setValue(static_cast<u32>(0));
            }
        }

        if(m_oldTV.m_use)
            m_oldTV.m_customTexture = getTemplate()->m_textureOldTVID;
        
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSetFloatInput_CRC,this);
    }
 
    void AFXPostProcessComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if (EventSetFloatInput * onSetInput = DYNAMIC_CAST(_event,EventSetFloatInput))
        {
            setInput(onSetInput->getInputName(),onSetInput->getInputValue());
        }
    }

    void AFXPostProcessComponent::updateAABB()
    {
        //update actor
        AABB aabb(Vec2d(-0.5, -0.5), Vec2d(0.5, 0.5));
        aabb.Scale(GetActor()->getScale());
        aabb.Translate(GetActor()->get2DPos());
        GetActor()->growAABB(aabb);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(AFXPostProcessComponent_Template)
    BEGIN_SERIALIZATION_CHILD(AFXPostProcessComponent_Template)
      SERIALIZE_OBJECT("input",m_inputColor);
        SERIALIZE_OBJECT("inputFactor",m_inputFactor);
        SERIALIZE_MEMBER("customTexOldTV", m_customTexOldTV);
    END_SERIALIZATION();

    AFXPostProcessComponent_Template::AFXPostProcessComponent_Template()
    {

    }

    bbool AFXPostProcessComponent_Template::onTemplateLoaded( bbool _hotReload )
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);
        if ( !getOldTVTextureFile().isEmpty() )
        {
            m_textureOldTVID = m_actorTemplate->addResource(Resource::ResourceType_Texture, getOldTVTextureFile());
        }
        return bOk;
    }

    void AFXPostProcessComponent_Template::onTemplateDelete( bbool _hotReload )
    {
        Super::onTemplateDelete(_hotReload);
        if ( m_textureOldTVID.isValid() )
        {
            m_actorTemplate->removeResource(m_textureOldTVID);
        }
    }

} // namespace ITF


