#ifndef _ITF_UIITEM_H_
#define _ITF_UIITEM_H_

#define UIItem_CRC ITF_GET_STRINGID_CRC(UIItem,1170992940)
#define UIItem_Template_CRC ITF_GET_STRINGID_CRC(UIItem_Template,1459452472)

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

namespace ITF
{
    class UIItem : public UIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(UIItem, UIComponent, UIItem_CRC);

    public:

        UIItem();
        virtual ~UIItem();

        virtual void Update(f32 _dt);
        virtual void onHiddingBegin();
        virtual bbool onValidate(const bbool _isValidate, const i32 _controllerID = -1);
        virtual bbool onBack(const bbool _isBack, const i32 _controllerID = -1);
        virtual bbool onAction(const bbool _isAction, const i32 _controllerID = -1);
        virtual bbool onSelected(const bbool _isSelected);
        virtual bbool onLocked(const bbool _isLocked);
        virtual bbool onDown(const bbool _isDown);
        virtual bbool onActivating(const bbool _isActivate, const i32 _controllerID = -1);
        virtual bbool onIsActivating(f32 _dt);

		virtual bbool   isSlidable() { return bfalse; }
        virtual void    onSlide(const StringID& _input) {}
        virtual void    onSlideBegin(const Vec2d& _pos) {}
        virtual void    onSliding(const Vec2d& _pos) {}
        virtual void    onSlideEnd(const Vec2d& _pos) {}

        void setIsLocked(const bbool _isLocked, const UIState _lockedFlag = UI_STATE_LockedByDefault);
        bbool getIsLocked() const;

        void setIsDown(const bbool _isDown);
        bbool getIsDown() const;

    protected:

        const class UIItem_Template* getTemplate() const;

    };


    class UIItem_Template : public UIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIItem_Template, UIComponent_Template, UIItem_Template_CRC)

    public:

        UIItem_Template() {}
        ~UIItem_Template() {}
    };

    ITF_INLINE const UIItem_Template* UIItem::getTemplate() const
    {
        return static_cast<const UIItem_Template*>(m_template);
    }
}
#endif // _ITF_UIITEM_H_