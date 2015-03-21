#ifndef _ITF_UIMENUBASIC_H_
#define _ITF_UIMENUBASIC_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#define UIMenuBasic_CRC ITF_GET_STRINGID_CRC(UIMenuBasic, 2477684654)
#define UIMenuBasic_Template_CRC ITF_GET_STRINGID_CRC(UIMenuBasic_Template, 2351617092)

namespace ITF
{
    class UIMenuBasic : public UIMenu
    {
    DECLARE_OBJECT_CHILD_RTTI(UIMenuBasic,UIMenu,UIMenuBasic_CRC);

    public:
        DECLARE_SERIALIZE()  

        UIMenuBasic  ();
        virtual ~UIMenuBasic ();

        virtual void Update( f32 _deltaTime );
        virtual void onInput(const i32 _controllerID, const f32 _axis, const StringID _input);
		virtual void onTouchInput(const TouchDataArray & _touchArray, u32 _i = 0);
        virtual void onActorLoaded(Pickable::HotReloadType _hot);
        virtual void onFinalizeLoad();
        virtual bbool valide(i32 _controllerID = -1, StringID _input = StringID::InvalidId);
        virtual bbool back(i32 _controllerID = -1, StringID _input = StringID::InvalidId);
        virtual bbool action(i32 _controllerID = -1, StringID _input = StringID::InvalidId);
		virtual void slide(StringID _input = StringID::InvalidId);

        virtual void select(UIItem* _item);
        virtual UIItem* getSelectedItem() const;

#ifdef ITF_W1W_MOBILE
        virtual UIItem* getValidateItem(bbool _waitActivatingEnd = btrue) const;
#else
        virtual UIItem* getValidateItem(bbool _waitActivatingEnd = bfalse) const;
#endif //ITF_W1W_MOBILE

        virtual UIItem* getBackItem(bbool _waitActivatingEnd = bfalse) const;
        virtual UIItem* getActionItem(bbool _waitActivatingEnd = bfalse) const;
		virtual void	resetValidateState();
        virtual i32 getValidateItemControllerID() const {return m_validateItemControllerID; }
        virtual i32 getBackItemControllerID() const {return m_backItemControllerID; }
        virtual i32 getActionItemControllerID() const {return m_actionItemControllerID; }
        virtual StringID getValidateItemInput() const {return m_validateItemInput; }
        virtual StringID getBackItemInput() const {return m_backItemInput; }
        virtual StringID getActionItemInput() const {return m_actionItemInput; }
        virtual MenuType getMenuType() const;
        virtual void    onCanBack(bbool _canBack);
        virtual  UIItem* computeNextItem(const Vec2d& _direction, i32 _neededState = UI_STATE_None, i32 _forbiddenState = UI_STATE_None) const;

        void setDefaultItem(StringID _friendID) {m_defaultItemFriendly = _friendID;}
        StringID getDefaultItemID() const {return m_defaultItemFriendly;}

        void setValidateAllowedInput(StringID _input, bbool _allowed);
        void setBackAllowedInput(StringID _input, bbool _allowed);
        void setActionAllowedInput(StringID _input, bbool _allowed);
		void setSlideAllowedInput(StringID _input, bbool _allowed);

		ITF_VECTOR<StringID> getWwisOnValidateSounds() const;

        void setBackItemNew(ObjectRef _item) {m_backItemNew = _item;} // This back item will be validated as soon as we press the back button

#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
		void resetLastPadCursorSelectedItem();
		void forcePadCursorToLastSelectedItem();
		void forcePadCursorToDefaultItem();
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD

#ifdef ITF_PC
        virtual void onMouseButton(const Vec2d& _pos, InputAdapter::MouseButton _but, InputAdapter::PressStatus _status);
        virtual void onMousePos(const Vec2d& _pos);

    protected:
        bbool m_isMouseLocked;
#endif //ITF_PC

    protected:
        const class UIMenuBasic_Template* getTemplate() const;

        virtual void onBecomeActive();
        virtual void onHiddingBegin();
        virtual void onBecomeInactive();


        StringID m_defaultItemFriendly;
        StringID m_backItemFriendly;
        ITF_VECTOR<StringID> m_validateAllowedInput;
        ITF_VECTOR<StringID> m_backAllowedInput;
        ITF_VECTOR<StringID> m_actionAllowedInput;
		ITF_VECTOR<StringID> m_slideAllowedInput;

        ObjectRef m_selectedItem;
#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
        UIItem*	  m_lastPadCursorSelectedItem;
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD
        ObjectRef m_validateItem;
        ObjectRef m_backItem;
        ObjectRef m_backItemNew; // This back item will be validated as soon as we press the back button
        ObjectRef m_actionItem;
        StringID  m_validateItemInput;
        StringID  m_backItemInput;
        StringID  m_actionItemInput;
        i32 m_validateItemControllerID;
        i32 m_backItemControllerID;
        i32 m_actionItemControllerID;
    };


    class UIMenuBasic_Template : public UIMenu_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIMenuBasic_Template, UIMenu_Template, UIMenuBasic_Template_CRC);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIMenuBasic);

    public:

        UIMenuBasic_Template();
        ~UIMenuBasic_Template();

        struct ValidateItemSound
        {
            DECLARE_SERIALIZE()
            StringID m_itemId;
            StringID m_mainSoundId;
            StringID m_remoteSoundId;
		};

		ITF_VECTOR<StringID> getWwisOnActivateSounds() const {return m_WwisOnActivate;}
		ITF_VECTOR<StringID> getWwisOnDeactivateSounds() const {return m_WwisOnDeactivate;}
		ITF_VECTOR<StringID> getWwisOnValidateSounds() const {return m_WwisOnValidate;}
		ITF_VECTOR<StringID> getWwisOnBackSounds() const {return m_WwisOnBack;}

		ITF_VECTOR<StringID> m_WwisOnActivate;
		ITF_VECTOR<StringID> m_WwisOnDeactivate;
		ITF_VECTOR<StringID> m_WwisOnValidate;
		ITF_VECTOR<StringID> m_WwisOnBack;

        bbool getValidateItemSound(StringID _itemId, StringID& _mainSound, StringID& _drcSound) const;
        ITF_VECTOR<ValidateItemSound> m_validateItemSounds;
    };

    ITF_INLINE const UIMenuBasic_Template* UIMenuBasic::getTemplate() const
    {
        return static_cast<const UIMenuBasic_Template*>(m_template);
    }
}
#endif // _ITF_UIMENUITEMTEXT_H_