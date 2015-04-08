#include "precompiled_engine.h"

#ifndef _ITF_AFTERFXCOMPONENT_H_
#include "engine/actors/components/AfterFxComponent.h"
#endif //_ITF_AFTERFXCOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(AfterFxComponent)
    BEGIN_SERIALIZATION_CHILD(AfterFxComponent)
    END_SERIALIZATION();
    
    AfterFxComponent::AfterFxComponent()
    {
        m_afterFx = newAlloc(mId_GfxAdapter, AFTERFX);
        m_isPause = bfalse;
        m_afterFxInputUpdate = NULL;

        m_renderintarget = 0;
        m_colorRTarget = Color::black();
        m_linkComponent = NULL;
        m_isStarted = bfalse;
        m_timer = 0.0f;
        m_activated = btrue;
    }

    AfterFxComponent::~AfterFxComponent()
    {
        SF_DEL(m_afterFx);
    }

    void AfterFxComponent::start()
    {
        m_isStarted = btrue;
        pause(bfalse);
    }
    void AfterFxComponent::stop()
    {
        m_isStarted = bfalse;
        pause(btrue);
    }

    void AfterFxComponent::Update( f32 _deltaTime )
    {
        if (m_activated)
        {
            if ( m_afterFxInputUpdate )
            {
                m_afterFxInputUpdate->updateAfterFxInput();
            }

            if (getTemplate()->getColorRTargetInputData().isValid())
            {
                f32 value = getTemplate()->getColorRTargetInputData().getValue(0.0f,_deltaTime,m_inputList);
                setColorRTarget( Color::Interpolate(getTemplate()->getColorRTargetMin(),getTemplate()->getColorRTargetMax(), value) );
            }

            if (m_renderintarget == 2)
            {
                //deprecated
                ITF_ASSERT_CRASH(0, "DEPRECATED !");
            }

            if (getTemplate()->getType() == AFX_Fade || getTemplate()->getType() == AFX_BorderBright)
                m_renderintarget = 0;

            if (getTemplate()->getType() != AFX_None)
            {
                u32 numProceduralf = getTemplate()->getParamFProceduralInput().size();

                u32 nParamf = getTemplate()->getParamF().size();
                u32 nParami = getTemplate()->getParamI().size();
                u32 nParamv = getTemplate()->getParamV().size();
                u32 nParamc = getTemplate()->getParamC().size();

                for (u32 i=0; i < 8; i++ )
                {
                    if (i < numProceduralf && getTemplate()->getParamFProceduralInput()[i].isValid())
                        m_afterFx->setParamfAt(i,getTemplate()->getParamFProceduralInput()[i].getValue(m_afterFx->getParamfAt(i),_deltaTime,m_inputList));
                    else
                    {
                        if (i < nParamf)
                            m_afterFx->setParamfAt(i, getParamfAt(i));
                    }
                }

                for (u32 i=0; i < nParami; i++ )
                    m_afterFx->setParamiAt(i, getParamiAt(i));

                for (u32 i=0; i < nParamv; i++ )
                    m_afterFx->setParamvAt(i, getParamvAt(i));

                for (u32 i=0; i < nParamc; i++ )
                    m_afterFx->setParamcAt(i, getParamcAt(i));
            }
            if (m_isStarted)
            {
                setInput(ITF_GET_STRINGID_CRC(timer,480648428),m_timer);
                updateAlpha(_deltaTime);
                m_timer += _deltaTime;
            }
   

        }


        updateAABB();


    }
    
    void AfterFxComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
        Super::batchPrimitives(_views);

        if ( m_activated && getTemplate()->getType() != AFX_None)
        {
            m_afterFx->SetRenderInTarget(m_renderintarget);
            f32 zs = 0.f;
            f32 zr = getTemplate()->getRenderRank() + m_actor->getDepth() * 5.f;
            //f32 zp = zr;
            if ( m_renderintarget == 1 )
            {
                m_afterFx->setColorTarget(m_colorRTarget);
                zs = getTemplate()->getStartRank() + m_actor->getDepth() * 5.f;
                //zp = zs;
            }
            m_afterFx->setZStart(zs);
            m_afterFx->setZRender(zr);
            //GFX_ADAPTER->getZListManager().AddPrimitiveInZList(_views, GFX_ZLIST_MAIN,(void*)m_afterFx, GFX_ITF_AFTERFX, NULL, zp, bfalse, ObjectRef::InvalidRef);
        }
    }

    void AfterFxComponent::preloadInit()
    {
    }

    void AfterFxComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_afterFx->setTexture(0, getTemplate()->getTextureID());

        setColorRTarget(getTemplate()->getColorRTarget());
        m_renderintarget = getTemplate()->getRenderInTarget();
        m_afterFx->SetRenderInTarget(m_renderintarget);
        m_afterFx->setType(getTemplate()->getType());
        m_afterFx->setCustomVal(getTemplate()->getFinalBlendMode());

        for ( u32 i = 0; i < m_inputList.size(); i++ )
        {
            Input* input = &m_inputList[i];

            if ( input->getType() == InputType_F32 )
            {
                input->setValue(0.f);
            }
            else
            {
                input->setValue(static_cast<u32>(0));
            }
        }

        m_inputList.resize(getTemplate()->getInputs().size());

        for ( u32 i = 0; i < m_inputList.size(); i++ )
        {
            Input* input = &m_inputList[i];
            const InputDesc& inputDesc = getTemplate()->getInputs()[i];

            input->setId(inputDesc.getId());
            input->setType(inputDesc.getType());

            if ( inputDesc.getType() == InputType_F32 )
            {
                input->setValue(0.f);
            }
            else
            {
                input->setValue(static_cast<u32>(0));
            }
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventTrigger_CRC,this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSetFloatInput_CRC,this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSetUintInput_CRC,this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventResetAfterFxAlpha_CRC,this);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_isStarted = bfalse;
        m_timer = 0.0f;
        
        updateAABB();
    }

    void AfterFxComponent::onEvent( Event * _event)
    {
        Super::onEvent(_event);

        if (EventTrigger * evt = DYNAMIC_CAST(_event,EventTrigger))
        {
            if (evt->getActivated())
                start();
            else
                stop();
        }
        else if (EventSetFloatInput * onSetInput = DYNAMIC_CAST(_event,EventSetFloatInput))
        {
            setInput(onSetInput->getInputName(),onSetInput->getInputValue());
        }
        else if (EventSetUintInput * onSetInput = DYNAMIC_CAST(_event,EventSetUintInput))
        {
            setInput(onSetInput->getInputName(),onSetInput->getInputValue());
        }
        else if (EventResetAfterFxAlpha * onEventReset = DYNAMIC_CAST(_event,EventResetAfterFxAlpha))
        {
            m_timer = 0;
            updateAlpha(0);
        }
    }
 
    void AfterFxComponent::addAfterFx(AFTERFX& _afx ) const
    {
    }

    void AfterFxComponent::updateAlpha(f32 _deltaTime)
    {
        if (m_timer + _deltaTime > getTemplate()->getAlphaFadeTotalDuration())
        {
            if (m_timer <= getTemplate()->getAlphaFadeTotalDuration() &&
                getTemplate()->getDeactivateAfterFade())
                m_activated = bfalse;
            return;
        }
        switch(getTemplate()->getAlphaMode())
        {
        case AfterFxComponent_Template::NONE:
            break;

        case AfterFxComponent_Template::UNIFORM:
            if (m_linkComponent)
            {
                const f32 ratio = Clamp((m_timer)/getTemplate()->getAlphaFadeTime(),0.0f,1.0f);
                const f32 alphaMax = getTemplate()->getAlphaFadeIn()?1.0f:0.0f;
                const f32 alphaMin = 1.0f - alphaMax;
                const f32 value = (ratio * (alphaMax - alphaMin)) + alphaMin;

                const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
                for ( LinkComponent::ChildrenList::const_iterator it = children.begin(); it != children.end(); ++it )
                {
                    const ObjectPath& obj = it->getPath();
                    Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,obj);
                    if (!pickable) continue;
                    AIUtils::setAlpha(pickable,value);
                }
            }
            break;

        case AfterFxComponent_Template::RADIAL:
            if (m_linkComponent)
            {
                const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
                for ( LinkComponent::ChildrenList::const_iterator it = children.begin(); it != children.end(); ++it )
                {
                    const ObjectPath& obj = it->getPath();
                    Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,obj);
                    if (!pickable) continue;
                    AIUtils::setRadialAlpha(pickable,m_actor->get2DPos(),getTemplate()->getAlphaRadialProgressionSpeed(),m_timer,getTemplate()->getAlphaFadeTime(),getTemplate()->getAlphaFadeIn());
                }
            }
            break;
        }
    }
    void AfterFxComponent::updateAABB()
    {
        AABB forcedaabb = getTemplate()->getForcedAABB();
        forcedaabb.Scale(m_actor->getScale());
        forcedaabb.setMin(forcedaabb.getMin() + m_actor->get2DPos());
        forcedaabb.setMax(forcedaabb.getMax() + m_actor->get2DPos());

        AABB aabb(m_actor->getPos());

        aabb.grow(forcedaabb);

        m_actor->growAABB(aabb);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(AfterFxComponent_Template)
    BEGIN_SERIALIZATION_CHILD(AfterFxComponent_Template)

        SERIALIZE_ENUM_BEGIN("afxtype",m_type);
            SERIALIZE_ENUM_VAR(AFX_None);
            SERIALIZE_ENUM_VAR(AFX_Blur);
            SERIALIZE_ENUM_VAR(AFX_Glow);
            SERIALIZE_ENUM_VAR(AFX_Remanence);
            SERIALIZE_ENUM_VAR(AFX_DOF);
            SERIALIZE_ENUM_VAR(AFX_RayCenter);
            SERIALIZE_ENUM_VAR(AFX_ColorSetting);
            SERIALIZE_ENUM_VAR(AFX_ColorRemap);
            SERIALIZE_ENUM_VAR(AFX_ColorLevels);
            SERIALIZE_ENUM_VAR(AFX_Fade);
            SERIALIZE_ENUM_VAR(AFX_Bright);
            SERIALIZE_ENUM_VAR(AFX_ToneMap);
            SERIALIZE_ENUM_VAR(AFX_AddSceneAndMul);
            SERIALIZE_ENUM_VAR(AFX_objectsGlow);
        SERIALIZE_ENUM_END();

        SERIALIZE_ENUM_BEGIN("finalblend",m_finalblend);
            SERIALIZE_ENUM_VAR(GFX_BLEND_UNKNOWN);
            SERIALIZE_ENUM_VAR(GFX_BLEND_COPY);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHA);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAPREMULT);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADEST);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADESTPREMULT);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ADD);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ADDALPHA);
            SERIALIZE_ENUM_VAR(GFX_BLEND_SUBALPHA);
            SERIALIZE_ENUM_VAR(GFX_BLEND_SUB);
            SERIALIZE_ENUM_VAR(GFX_BLEND_MUL);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAMUL);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAMUL);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHA);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAPREMULT);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADEST);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADESTPREMULT);
            SERIALIZE_ENUM_VAR(GFX_BLEND_MUL2X);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHATOCOLOR);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHATOCOLOR);
            SERIALIZE_ENUM_VAR(GFX_BLEND_SCREEN);
        SERIALIZE_ENUM_END();

        SERIALIZE_MEMBER("startRank",m_startRank);
        SERIALIZE_MEMBER("renderRank",m_renderRank);
        SERIALIZE_MEMBER("renderintarget",m_renderintarget);
        SERIALIZE_MEMBER("colorRTarget",m_colorRTarget);

        SERIALIZE_CONTAINER("paramf", m_Paramf);
        SERIALIZE_CONTAINER("parami", m_Parami);
        SERIALIZE_CONTAINER("paramv", m_Paramv);
        SERIALIZE_CONTAINER("paramc", m_Paramc);
        SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
        SERIALIZE_CONTAINER_OBJECT("ParamfProceduralInput",m_ParamfProceduralInput);
        SERIALIZE_MEMBER("colorRTargetMin",m_colorRTargetMin);
        SERIALIZE_MEMBER("colorRTargetMax",m_colorRTargetMax);
        SERIALIZE_OBJECT("colorRTargetInputData",m_colorRTargetInputData);
        SERIALIZE_MEMBER("customTex0", m_customTex0);

        SERIALIZE_ENUM_BEGIN("alphaMode",m_alphaMode);
        SERIALIZE_ENUM_VAR(NONE);
        SERIALIZE_ENUM_VAR(UNIFORM);
        SERIALIZE_ENUM_VAR(RADIAL);
        SERIALIZE_ENUM_END();

        SERIALIZE_MEMBER("alphaRadialProgressionSpeed", m_alphaRadialProgressionSpeed);
        SERIALIZE_MEMBER("alphaFadeTime", m_alphaFadeTime);
        SERIALIZE_MEMBER("alphaFadeIn", m_alphaFadeIn);
        SERIALIZE_MEMBER("alphaFadeTotalDuration", m_alphaFadeTotalDuration);
        SERIALIZE_OBJECT("forcedAABB", m_forcedAABB);
        SERIALIZE_MEMBER("deactivateAfterFade", m_deactivateAfterFade);
    END_SERIALIZATION();

    AfterFxComponent_Template::AfterFxComponent_Template()
    {
        m_type = AFX_None;
        m_startRank = -1000;
        m_renderRank = 1000;
        m_renderintarget = 0;
        m_finalblend = GFX_BLEND_ALPHA;
        m_colorRTarget = Color::black();
        m_colorRTargetMin = Color::black();
        m_colorRTargetMax = Color::black();

        for (u32 i=0;i<8;i++)
            m_Paramf.push_back(1.f);
        for (u32 i=0;i<8;i++)
            m_Parami.push_back(0);
        for (u32 i=0;i<8;i++)
            m_Paramv.push_back(Vec3d::Zero);
        for (u32 i=0;i<2;i++)
            m_Paramc.push_back(Color::zero());
        m_alphaMode = NONE;
        m_alphaRadialProgressionSpeed = 1.0f;
        m_alphaFadeTime = 1.0f;
        m_alphaFadeIn = bfalse;
        m_alphaFadeTotalDuration = 5.0f;
        m_deactivateAfterFade = bfalse;
    }

    bbool AfterFxComponent_Template::onTemplateLoaded( bbool _hotReload )
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);

        if ( !getTextureFile().isEmpty() )
        {
            m_textureID = m_actorTemplate->addResource(Resource::ResourceType_Texture, getTextureFile());
        }

        return bOk;
    }

    void AfterFxComponent_Template::onTemplateDelete( bbool _hotReload )
    {
        Super::onTemplateDelete(_hotReload);

        if ( m_textureID.isValid() )
        {
            m_actorTemplate->removeResource(m_textureID);
        }
    }

} // namespace ITF


