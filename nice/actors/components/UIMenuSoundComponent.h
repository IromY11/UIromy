#ifndef _ITF_UIMENUSOUNDCOMPONENT_H_
#define _ITF_UIMENUSOUNDCOMPONENT_H_

namespace ITF
{
    class UIMenuSoundComponent_Template;
    class SoundComponent;

    //////////////////////////////////////////////////////////////////////////////////////////
    // manage sound for UI menu
    #define UIMenuSoundComponent_CRC	ITF_GET_STRINGID_CRC(UIMenuSoundComponent, 3788816759)
    class UIMenuSoundComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UIMenuSoundComponent,ActorComponent,UIMenuSoundComponent_CRC);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:
        UIMenuSoundComponent();

        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return bfalse; }
        virtual bbool needsDraw2D() const { return bfalse; }
		virtual	bbool needsDraw2DNoScreenRatio() const { return bfalse; }

		virtual void  onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void onFinalizeLoad();
        virtual void onActorClearComponents();
        virtual void Update(f32 _dt);

        void onAction(const StringID & _menu, const StringID & _action, const StringID & _selection);
        void stopAllSounds();

    private :
        ITF_INLINE const UIMenuSoundComponent_Template * getTemplate() const;

#ifdef ITF_SUPPORT_EDITOR
        void updateDebug(const f32 _dt);
#endif //ITF_SUPPORT_EDITOR

    private:

        SoundComponent * m_soundComponent;
        bbool m_registeredToUIManager;

#ifdef ITF_SUPPORT_EDITOR
        struct MenuActionDebug
        {
            MenuActionDebug()
            : m_menu(StringID::Invalid)
            , m_action(StringID::Invalid)
            , m_selection(StringID::Invalid)
            {
            }
            MenuActionDebug(const StringID & _menu, const StringID & _action, const StringID & _selection)
            : m_menu(_menu)
            , m_action(_action)
            , m_selection(_selection)
            {
            }
            StringID m_menu;
            StringID m_action;
            StringID m_selection;
        };
        SafeArray<MenuActionDebug> m_debugActionList;
        static const u32 MAX_DEBUG_DRAW_ELEMENTS;
#endif //ITF_SUPPORT_EDITOR
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    //store an association between a menu action and a sound
    class UIMenuActionSound
    {
        DECLARE_SERIALIZE()

    public:
        UIMenuActionSound();

        ITF_INLINE const StringID & getActionName() const;
        ITF_INLINE const StringID & getSelection() const;
        ITF_INLINE const StringID & getSoundName() const;

    private:
        StringID m_action;
        StringID m_selection;
        StringID m_sound;
    };

    ITF_INLINE const StringID & UIMenuActionSound::getActionName() const
    {
        return m_action;
    }

    ITF_INLINE const StringID & UIMenuActionSound::getSelection() const
    {
        return m_selection;
    }

    ITF_INLINE const StringID & UIMenuActionSound::getSoundName() const
    {
        return m_sound;
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    // manage sound config for each menu page
    class UIMenuPageSoundConfig
    {
        DECLARE_SERIALIZE()

    public:
        UIMenuPageSoundConfig();

        ITF_INLINE const StringID & getMenuPageId() const;
        bbool getDataFromActionSelection(const StringID & _action, const StringID & _selection, StringID & _sound) const;

    private:
        StringID m_menuPageId;
        ITF_VECTOR<UIMenuActionSound> m_actionSoundList;
    };

    ITF_INLINE const StringID & UIMenuPageSoundConfig::getMenuPageId() const
    {
        return m_menuPageId;
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    //component template
#define UIMenuSoundComponent_Template_CRC ITF_GET_STRINGID_CRC(UIMenuSoundComponent_Template, 473615840)
    class UIMenuSoundComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIMenuSoundComponent_Template, ActorComponent_Template, UIMenuSoundComponent_Template_CRC);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIMenuSoundComponent);

    public:

        UIMenuSoundComponent_Template();

        bbool getDataFromMenuActionSelection(const StringID & _menu, const StringID & _action, const StringID & _selection, StringID & _sound) const;
#ifdef ITF_SUPPORT_EDITOR
        ITF_INLINE bbool debugMode() const;
#endif //ITF_SUPPORT_EDITOR

    private:
        ITF_VECTOR<UIMenuPageSoundConfig> m_pageConfigList;
#ifdef ITF_SUPPORT_EDITOR
        bbool m_debugMode;
#endif //ITF_SUPPORT_EDITOR
    };

    ITF_INLINE const UIMenuSoundComponent_Template * UIMenuSoundComponent::getTemplate() const
    {
        return static_cast<const UIMenuSoundComponent_Template *>(m_template);
    }

#ifdef ITF_SUPPORT_EDITOR
    ITF_INLINE bbool UIMenuSoundComponent_Template::debugMode() const
    {
        return m_debugMode;
    }
#endif //ITF_SUPPORT_EDITOR
} // namespace ITF

#endif // _ITF_UIMENUSOUNDCOMPONENT_H_
