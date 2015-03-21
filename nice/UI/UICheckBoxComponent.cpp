#include "precompiled_gameplay.h"

#ifndef _ITF_UICHECKBOXCOMPONENT_H_
#include "gameplay/components/UI/UICheckBoxComponent.h"
#endif //_ITF_UICHECKBOXCOMPONENT_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UICheckBoxComponent)
    BEGIN_SERIALIZATION_CHILD(UICheckBoxComponent)
        SERIALIZE_MEMBER("checkBoxCheckedID", m_checkBoxCheckedID);
        SERIALIZE_MEMBER("checkBoxUncheckedID", m_checkBoxUncheckedID);
    END_SERIALIZATION()

    UICheckBoxComponent::UICheckBoxComponent()
    : m_checkBoxChecked(NULL)
    , m_checkBoxUnchecked(NULL)
    , m_isChecked(btrue)
    {

    }

    void UICheckBoxComponent::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

        m_checkBoxChecked = getChildComponent<UIItem>(m_checkBoxCheckedID);
        m_checkBoxUnchecked = getChildComponent<UIItem>(m_checkBoxUncheckedID);

        if (m_checkBoxChecked && m_checkBoxUnchecked)
        {
            m_checkBoxChecked->setIsDisplay(btrue);
            m_checkBoxUnchecked->setIsDisplay(btrue);

            m_checkBoxChecked->GetActor()->setAABB(m_checkBoxChecked->GetActor()->getPos());
            m_checkBoxUnchecked->GetActor()->setAABB(m_checkBoxChecked->GetActor()->getPos());

            m_checkBoxChecked->GetActor()->setBoundLocal2DPos(Vec2d::Zero);
            m_checkBoxUnchecked->GetActor()->setBoundLocal2DPos(Vec2d::Zero);

            m_checkBoxChecked->setUIState(UI_STATE_CanNotBeSelected, btrue);
            m_checkBoxUnchecked->setUIState(UI_STATE_CanNotBeSelected, btrue);
        }
    }

    void UICheckBoxComponent::onStartDestroy( bbool _hotReload )
    {
        m_checkBoxChecked = NULL;
        m_checkBoxUnchecked = NULL;

        Super::onStartDestroy(_hotReload);
    }

    void UICheckBoxComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        setIsChecked(m_isChecked);
    }

    bbool UICheckBoxComponent::onValidate(const bbool _isValidate, const i32 _controllerID)
    {
        bbool result = Super::onValidate(_isValidate, _controllerID);

        if (_isValidate && m_checkBoxChecked && m_checkBoxUnchecked)
        {
            setIsChecked(!m_isChecked);
            onSelected(btrue);
        }

        return result;
    }

    ITF::bbool UICheckBoxComponent::onSelected(const bbool _isSelected)
    {
        if (m_checkBoxUnchecked && m_checkBoxChecked)
        {
            m_checkBoxChecked->onSelected(_isSelected && m_isChecked);
            m_checkBoxUnchecked->onSelected(_isSelected && !m_isChecked);
        }
        return Super::onSelected(_isSelected);
    }

    void UICheckBoxComponent::setIsChecked(bbool _isChecked)
    {
        m_isChecked = _isChecked;

        if (m_checkBoxChecked && m_checkBoxUnchecked)
        {
            m_checkBoxChecked->setIsDisplay(btrue);
            m_checkBoxUnchecked->setIsDisplay(btrue);

            m_checkBoxChecked->setIsDisplay(m_isChecked);
            m_checkBoxUnchecked->setIsDisplay(!m_isChecked);
        }
    }

    void UICheckBoxComponent::setIsDisplay(const bbool _isDisplay)
    {
        Super::setIsDisplay(_isDisplay);

        if (m_checkBoxChecked && m_checkBoxUnchecked)
        {
            m_checkBoxChecked->setIsDisplay(_isDisplay);
            m_checkBoxUnchecked->setIsDisplay(_isDisplay);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UICheckBoxComponent_Template)
        BEGIN_SERIALIZATION_CHILD(UICheckBoxComponent_Template)
    END_SERIALIZATION()

    UICheckBoxComponent_Template::UICheckBoxComponent_Template()
    {
    }

    UICheckBoxComponent_Template::~UICheckBoxComponent_Template()
    {    
    }
}
