#ifndef _ITF_UIMENUUPLAYPOPUP_H_
#define _ITF_UIMENUUPLAYPOPUP_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    #define UIMenuUplayPopUp_CRC ITF_GET_STRINGID_CRC(UIMenuUplayPopUp,919121118)
    #define UIMenuUplayPopUp_Template_CRC ITF_GET_STRINGID_CRC(UIMenuUplayPopUp_Template,780725034)

    ///////////////////////////////////////////////////////////////////////////////////////////

    class UIMenuUplayPopUp : public UIMenu
    {
        DECLARE_OBJECT_CHILD_RTTI(UIMenuUplayPopUp, UIMenu, UIMenuUplayPopUp_CRC);
        DECLARE_SERIALIZE()

    public:
        UIMenuUplayPopUp  ();
        virtual ~UIMenuUplayPopUp ();

        virtual void Update(f32 _dt);
        virtual void onInput(const i32 _controllerID, const f32 _axis, const StringID _input);
		virtual void onTouchInput(const TouchDataArray & _touchArray, u32 _i = 0);
        virtual void onFinalizeLoad();
        virtual void onBecomeActive();
        bbool isOnBottom() const;
        f32 getCurrentShift() const;

        void                         setText(const String8& _txt, const String8& _val = String8::emptyString);
        bbool                        animEnd() const { return m_animEnd; }
    protected:
        class MultiTextBoxComponent *getMessageTextBox();
        Vec2d    m_finalPos;
		Vec2d	 m_sourcePos;
        StringID m_defaultItemFriendly;
        bbool    m_animEnd;
        f32      m_updateTime;

        Vec2d    m_finalPosRatio;
        Vec2d    m_screenSize;

#ifdef ITF_SUPPORT_EDITOR
        bbool m_bselected;
#endif // ITF_SUPPORT_EDITOR

        const class UIMenuUplayPopUp_Template* getTemplate() const;
    };

    class UIMenuUplayPopUp_Template : public UIMenu_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIMenuUplayPopUp_Template, UIMenu_Template, UIMenuUplayPopUp_Template_CRC)
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIMenuUplayPopUp);

    public:

        UIMenuUplayPopUp_Template()
        : m_translationTime(1.f)
        , m_noTextTime(0.1f)
        , m_textTime(1.f)
        {
        }

        ~UIMenuUplayPopUp_Template()
        {
        }

        f32 m_translationTime;
        f32 m_noTextTime;
        f32 m_textTime;
    };

    ITF_INLINE const UIMenuUplayPopUp_Template* UIMenuUplayPopUp::getTemplate() const
    {
        return static_cast<const UIMenuUplayPopUp_Template*>(m_template);
    }
}
#endif // _ITF_UIMENUUPLAYPOPUP_H_
