#ifndef _ITF_RO2_UICONTROLLERCOMPONENT_H_
#define _ITF_RO2_UICONTROLLERCOMPONENT_H_

#ifndef _ITF_UIITEMBASIC_H_
#include "gameplay/components/UI/UIItemBasic.h"
#endif //_ITF_UIITEMBASIC_H_

#ifndef _ITF_ACTORSPAWNER_H_
#include "gameplay/AI/Utils/ActorSpawner.h"
#endif //_ITF_ACTORSPAWNER_H_

#ifndef _ITF_FONTTEXTAREA_H_
#include "engine/display/FontTextArea.h"
#endif //_ITF_FONTTEXTAREA_H_


namespace ITF
{
    class UIControllerComponent : public UIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UIControllerComponent, UIComponent, 2272485);

    public:

        DECLARE_SERIALIZE()

        UIControllerComponent();
        virtual ~UIControllerComponent();

        virtual void				Update(f32 _dt);
        virtual void                onBecomeActive();
        virtual void                onBecomeInactive();
        virtual void                onFinalizeLoad();

    protected:
        ITF_INLINE const class UIControllerComponent_Template* getTemplate() const;

        void            SpawnTextActors();
        void            UpdateButtonTextPos();
        void            DespawnTextActors();

        struct SpawnedText
        {
            SpawnedText() : m_actRef(ActorRef::InvalidRef), m_isLoading(bfalse) {}
            ~SpawnedText() {};

            ActorRef    m_actRef;
            bbool       m_isLoading;
        };

        class AnimLightComponent*   m_animComponent;
        ActorSpawner                m_textSpawner;
        ITF_VECTOR<SpawnedText>     m_spawns;
    };

    //////////////////////////////////////////////////////////////////////////

    class UIControllerComponent_Template : public UIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIControllerComponent_Template, UIComponent_Template, 1894088632);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIControllerComponent);

    public:
        UIControllerComponent_Template();
        ~UIControllerComponent_Template();

        struct ControllerTextObject
        {
            DECLARE_SERIALIZE()

            ControllerTextObject() 
                : m_boneName(StringID::Invalid)
                , m_locId(LocalisationId::Invalid)
                , m_color(COLOR_WHITE)
                , m_anchor(AREA_ANCHOR_NONE)
                , m_hAlign(FONT_ALIGN_NONE)
                , m_vAlign(FONT_ALIGN_NONE)
            {

            }

            ~ControllerTextObject() {}

            StringID            m_boneName;
            LocalisationId      m_locId;
            Color               m_color;
            TEXT_AREA_ANCHOR    m_anchor;
            FONT_ALIGNMENT      m_hAlign;
            FONT_ALIGNMENT      m_vAlign;
        };

        ITF_INLINE const Path&          getTextboxPath() const                  { return m_textboxPath; }

        ITF_VECTOR<ControllerTextObject>    m_ButtonActions;
        Path                                m_textboxPath;
    };

    ITF_INLINE const UIControllerComponent_Template* UIControllerComponent::getTemplate() const
    {
        return static_cast<const UIControllerComponent_Template*>(m_template);
    }
}

#endif
