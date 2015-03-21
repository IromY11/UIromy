
#include "precompiled_gameplay.h"

#ifndef _ITF_RO2_UICONTROLLERCOMPONENT_H_
#include "gameplay/components/UI/UIControllerComponent.h"
#endif //_ITF_RO2_UICONTROLLERCOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_ACTOR_BIND_H_
#include "engine/actors/ActorBind.h"
#endif // _ITF_ACTOR_BIND_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(UIControllerComponent)
    BEGIN_SERIALIZATION_CHILD(UIControllerComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    UIControllerComponent::UIControllerComponent()
    : m_animComponent(NULL)
    {

    }

    UIControllerComponent::~UIControllerComponent()
    {

    }

    void UIControllerComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if(m_textSpawner.isRegistered() && getTemplate()->m_ButtonActions.size() > 0)
        {
            // Initialize new textboxes
            for(u32 i = 0; i < getTemplate()->m_ButtonActions.size(); ++i)
            {
                Actor* textboxActor = m_spawns[i].m_actRef.getActor();
                if(m_spawns[i].m_isLoading && !textboxActor->isAsyncLoading())
                {
                    UpdateButtonTextPos();

                    m_spawns[i].m_isLoading = bfalse;
                }
            }

            //DBG
            UpdateButtonTextPos();

            // Update Spawner
            m_textSpawner.update(GetActor()->getScene());
        }
    }

    void UIControllerComponent::onBecomeActive()
    {
        Super::onBecomeActive();
        m_textSpawner.registerInPool(m_actor->getRef(), m_actor->getResourceContainer(), getTemplate()->getTextboxPath(), getTemplate()->m_ButtonActions.size(), 8);

        SpawnTextActors();
    }

    void UIControllerComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
        DespawnTextActors();
    }

    void UIControllerComponent::onFinalizeLoad()
    {
        Super::onFinalizeLoad();
        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        ITF_WARNING(GetActor(), m_animComponent, "requires an animLightComponent");
    }


    void UIControllerComponent::UpdateButtonTextPos()
    {
        if(!m_textSpawner.isRegistered())
        {
            return;
        }

        for(u32 i = 0; i < getTemplate()->m_ButtonActions.size(); ++i)
        {
            if(m_spawns[i].m_isLoading || m_spawns[i].m_actRef == ActorRef::InvalidRef)
            {
                continue;
            }

            StringID dbgBoneName = getTemplate()->m_ButtonActions[i].m_boneName;
            i32 idx = m_animComponent->getBoneIndex(dbgBoneName);
            if(idx != U32_INVALID)
            {
                Actor* tbAct = m_spawns[i].m_actRef.getActor();
                Vec2d pos;
                if(tbAct && m_animComponent->getBonePos(idx, pos))
                {
                    Transform2d xf;
                    xf.m_pos = m_actor->get2DPos();
                    xf.m_rot.m_col1 = Vec2d::Right;
                    xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

                    pos = mulTransform2d(xf,pos * m_actor->getScale() * Vec2d(g_pixelBy2DUnit, -g_pixelBy2DUnit));

                    tbAct->set2DPos(pos);
                    tbAct->setDepth(m_actor->getDepth() + 1.0f);

                    if(!tbAct->getParentBind() && tbAct != m_actor)
                    {
                        tbAct->createParentBind(bfalse, m_actor, Bind::Root, 0, btrue, btrue, bfalse, btrue);
                    }

                    if(UITextBox* textBox = tbAct->GetComponent<UITextBox>())
                    {
                        if(getTemplate()->m_ButtonActions[i].m_locId != LocalisationId::Invalid)
                        {
                            textBox->setLoc(getTemplate()->m_ButtonActions[i].m_locId);
                        }
                        else
                        {
                            textBox->setText("Action LocID Missing");
                        }
                        
                        textBox->setColor(getTemplate()->m_ButtonActions[i].m_color);

                        FontTextArea& area = textBox->getTextArea();
                        area.setAnchor(getTemplate()->m_ButtonActions[i].m_anchor);
                        area.setHAlignment(getTemplate()->m_ButtonActions[i].m_hAlign);
                        area.setVAlignment(getTemplate()->m_ButtonActions[i].m_vAlign);

                        //area.setShadowColor(COLOR_RED);
                        area.setShadowOffset(Vec2d(2.0f, 2.0f));
                    }
                }
            } else
            {
                ITF_WARNING(GetActor(), 0, "Missing buttonBone or invalid index");
            }
        }
    }

    void UIControllerComponent::SpawnTextActors()
    {
        if(!m_textSpawner.isRegistered())
        {
            return;
        }

        m_spawns.resize(getTemplate()->m_ButtonActions.size());

        for(u32 i = 0; i < getTemplate()->m_ButtonActions.size(); ++i)
        {
            ActorSpawner::Spawn spawn = m_textSpawner.spawnActor();

            m_spawns[i].m_actRef = spawn.m_actor->getRef();
            m_spawns[i].m_isLoading = btrue;
        }
    }

    void UIControllerComponent::DespawnTextActors()
    {
        for(u32 i = 0; i < getTemplate()->m_ButtonActions.size(); ++i)
        {
            if(Actor* act = m_spawns[i].m_actRef.getActor())
            {
                act->requestDestruction();
            }
        }

        m_textSpawner.clear();
        m_spawns.clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(UIControllerComponent_Template)
    BEGIN_SERIALIZATION_CHILD(UIControllerComponent_Template)
        SERIALIZE_CONTAINER_OBJECT("buttonActions", m_ButtonActions);
        SERIALIZE_MEMBER("textboxPath", m_textboxPath);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(UIControllerComponent_Template, ControllerTextObject)
        SERIALIZE_MEMBER("boneName", m_boneName);
        SERIALIZE_MEMBER("locId", m_locId);
        SERIALIZE_MEMBER("color", m_color);
        SERIALIZE_ENUM_H_ALIGNMENT("hAlign", m_hAlign);
        SERIALIZE_ENUM_V_ALIGNMENT("vAlign", m_vAlign);
        SERIALIZE_ENUM_BEGIN("anchor", m_anchor);
            SERIALIZE_ENUM_VAR(AREA_ANCHOR_NONE);
            SERIALIZE_ENUM_VAR(AREA_ANCHOR_TOP_LEFT);
            SERIALIZE_ENUM_VAR(AREA_ANCHOR_MIDDLE_CENTER); 
            SERIALIZE_ENUM_VAR(AREA_ANCHOR_MIDDLE_LEFT);
            SERIALIZE_ENUM_VAR(AREA_ANCHOR_MIDDLE_RIGHT);
            SERIALIZE_ENUM_VAR(AREA_ANCHOR_TOP_CENTER);
            SERIALIZE_ENUM_VAR(AREA_ANCHOR_TOP_RIGHT);
            SERIALIZE_ENUM_VAR(AREA_ANCHOR_BOTTOM_CENTER);
            SERIALIZE_ENUM_VAR(AREA_ANCHOR_BOTTOM_LEFT);
            SERIALIZE_ENUM_VAR(AREA_ANCHOR_BOTTOM_RIGHT);
        SERIALIZE_ENUM_END(); 
    END_SERIALIZATION()

    UIControllerComponent_Template::UIControllerComponent_Template()
    {  
    }


    UIControllerComponent_Template::~UIControllerComponent_Template()
    {    
    }

}