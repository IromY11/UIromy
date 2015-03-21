#ifndef _ITF_UISLIDERCOMPONENT_H_
#define _ITF_UISLIDERCOMPONENT_H_

#ifndef _ITF_UIITEMBASIC_H_
#include "gameplay/components/UI/UIItemBasic.h"
#endif //_ITF_UIITEMBASIC_H_

namespace ITF
{
    class UISliderComponent : public UIItemBasic
    {
        DECLARE_OBJECT_CHILD_RTTI(UISliderComponent, UIItemBasic, ITF_GET_STRINGID_CRC(UISliderComponent,236227314));

        enum SlideDirection
        {
            SlideDirection_None,
            SlideDirection_Left,
            SlideDirection_Right
        };

        enum SlideMode
        {
            SlideMode_None,
            SlideMode_Controller,
            SlideMode_TouchPadStep,
            SlideMode_TouchPadStroke
        };

    public:
        DECLARE_SERIALIZE()

        UISliderComponent();

        virtual void    onFinalizeLoad();
        virtual void    onStartDestroy(bbool _hotReload);
        virtual void    onBecomeActive();

#ifdef ITF_SUPPORT_EDITOR
        virtual void    onPostPropertyChange();
#endif //ITF_SUPPORT_EDITOR

        virtual bbool   onValidate(const bbool _isValidate, const i32 _controllerID = -1);
        virtual bbool   onSelected(const bbool _isSelected);

        virtual bbool   isSlidable() { return btrue; }
        virtual void    onSlide(const StringID& _input);

        virtual void    onSlideBegin(const Vec2d& _pos);
        virtual void    onSliding(const Vec2d& _pos);
        virtual void    onSlideEnd(const Vec2d& _pos);

        virtual void    Update(f32 _deltaTime);

        void            updateSliderValue();
        void            showValueText();

        f32             getValue() const { return m_valuePercentage * m_maxValue; }
        void            setValuePercentage(f32 _val, bbool _updatePos = btrue);
        void            setValue(f32 _val, bbool _updatePos = btrue);
        void            updateCursorPosition(f32 _val);

        bbool           isSliding();

    private:
        ITF_INLINE const class  UISliderComponent_Template * getTemplate() const;

        void            slideByTouch(Vec2d _pos);
        void            slideByStroke(Vec2d _pos);
        UIItem*         m_itemCursor;
        UIItem*         m_itemBar;
        StringID        m_itemCursorID;
        StringID        m_itemBarID;

        SlideDirection  m_slideDir;
        SlideMode       m_slideMode;
        f32             m_valuePercentage;
        f32             m_slideBarWidth;
        f32             m_fixPos_x;
        f32             m_fixPos_y;
        f32             m_cursorSpeed;
        f32             m_maxValue;

        bbool           m_needPressValidateToSlide;
        bool            m_enableValueText;
        Vec2d           m_lastTouchPos;
        Vec2d           m_strokeOffset;

        bbool           m_forceUpdateCursorPos;
    };

    //---------------------------------------------------------------------------------------------------

    class UISliderComponent_Template : public UIItemBasic_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UISliderComponent_Template,UIItemBasic_Template,3650720017);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UISliderComponent);

    public:
        UISliderComponent_Template();
        ~UISliderComponent_Template();
    };

    ITF_INLINE const UISliderComponent_Template* UISliderComponent::getTemplate() const
    {
        return static_cast<const UISliderComponent_Template*>(m_template);
    }

}
#endif // _ITF_UISLIDERCOMPONENT_H_