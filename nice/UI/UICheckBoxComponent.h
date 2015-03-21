#ifndef _ITF_UICHECKBOXCOMPONENT_H_
#define _ITF_UICHECKBOXCOMPONENT_H_

#ifndef _ITF_UIITEM_H_
#include "gameplay/components/UI/UIItem.h"
#endif //_ITF_UIITEM_H_

namespace ITF
{

    class UICheckBoxComponent: public UIItem
    {
        DECLARE_OBJECT_CHILD_RTTI(UICheckBoxComponent, UIItem, 1290785978);

    public:
        DECLARE_SERIALIZE()

        UICheckBoxComponent();

        virtual void	onBecomeActive();
        virtual void	onFinalizeLoad();
        virtual void	onStartDestroy(bbool _hotReload);

        virtual bbool	onValidate(const bbool _isValidate, const i32 _controllerID = -1);
        virtual bbool	onSelected(const bbool _isSelected);

        virtual void    setIsDisplay(const bbool _isDisplay);

        bbool           getIsChecked(){return m_isChecked;};
        void            setIsChecked(bbool _isChecked);

    protected:

    private:
        ITF_INLINE const class UICheckBoxComponent_Template * getTemplate() const;
        UIItem*		m_checkBoxChecked;
        UIItem*		m_checkBoxUnchecked;
        StringID	m_checkBoxCheckedID;
        StringID	m_checkBoxUncheckedID;

        bbool		m_isChecked;
    };

    //---------------------------------------------------------------------------------------------------

    class UICheckBoxComponent_Template : public UIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UICheckBoxComponent_Template,UIComponent_Template,1524121409);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UICheckBoxComponent);

    public:
        UICheckBoxComponent_Template();
        ~UICheckBoxComponent_Template();
    };

    ITF_INLINE const UICheckBoxComponent_Template* UICheckBoxComponent::getTemplate() const
    {
        return static_cast<const UICheckBoxComponent_Template*>(m_template);
    }

}
#endif //_ITF_UICHECKBOXCOMPONENT_H_