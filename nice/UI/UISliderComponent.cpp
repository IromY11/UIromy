#include "precompiled_gameplay.h"

#ifndef _ITF_UISLIDERCOMPONENT_H_
#include "gameplay/components/UI/UISliderComponent.h"
#endif //_ITF_UISLIDERCOMPONENT_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

namespace ITF
{
    const f32 Slider_DeadZone = 2.0f;

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UISliderComponent)
    BEGIN_SERIALIZATION_CHILD(UISliderComponent)
        SERIALIZE_MEMBER("itemCursorID", m_itemCursorID);
        SERIALIZE_MEMBER("itemBarID", m_itemBarID);
        SERIALIZE_MEMBER("slideBarWidth", m_slideBarWidth);
        SERIALIZE_MEMBER("fixPosX", m_fixPos_x);
        SERIALIZE_MEMBER("fixPosY", m_fixPos_y);
        SERIALIZE_MEMBER("maxValue", m_maxValue);
        SERIALIZE_MEMBER("cursorSpeed", m_cursorSpeed);
        SERIALIZE_MEMBER("needPressValidateToSlide", m_needPressValidateToSlide);
        SERIALIZE_MEMBER("enableValueText", m_enableValueText);
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UISliderComponent::UISliderComponent()
    : m_itemCursor(NULL)
    , m_itemBar(NULL)
    , m_slideDir(SlideDirection_None)
    , m_slideMode(SlideMode_None)
    , m_valuePercentage(0.0f)
    , m_maxValue(1.0f)
    , m_slideBarWidth(100.0f)
    , m_fixPos_x(0.0f)
    , m_fixPos_y(0.0f)
    , m_cursorSpeed(0.01f)
    , m_needPressValidateToSlide(btrue)
    , m_enableValueText(bfalse)
    , m_lastTouchPos(Vec2d::Zero)
    , m_strokeOffset(Vec2d::Zero)
    , m_forceUpdateCursorPos(btrue)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UISliderComponent::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

        m_itemCursor = static_cast<UIItem*>(getChildComponent(m_itemCursorID));
        m_itemBar = static_cast<UIItem*>(getChildComponent(m_itemBarID));

        if (m_itemCursor && m_itemBar)
        {
            m_itemCursor->GetActor()->setBoundLocal2DPos(Vec2d::Zero + Vec2d(m_fixPos_x, m_fixPos_y));
            m_itemBar->GetActor()->setBoundLocal2DPos(Vec2d::Zero);

            m_itemCursor->setUIState(UI_STATE_CanNotBeSelected, btrue);
            m_itemBar->setUIState(UI_STATE_CanNotBeSelected, btrue);
        }
    }

    void UISliderComponent::onStartDestroy( bbool _hotReload )
    {
        m_itemBar = NULL;
        m_itemCursor = NULL;

        Super::onStartDestroy(_hotReload);
    }

    void UISliderComponent::onBecomeActive()
    {
        if (m_itemCursor && m_itemBar)
        {
            m_itemCursor->GetActor()->setBoundLocal2DPos(Vec2d::Zero + Vec2d(m_fixPos_x, m_fixPos_y));
            m_itemBar->GetActor()->setBoundLocal2DPos(Vec2d::Zero);

            m_itemCursor->setUIState(UI_STATE_CanNotBeSelected, btrue);
            m_itemBar->setUIState(UI_STATE_CanNotBeSelected, btrue);

            showValueText();
        }

        m_forceUpdateCursorPos = btrue;

        Super::onBecomeActive();
    }

#ifdef ITF_SUPPORT_EDITOR
    void UISliderComponent::onPostPropertyChange()
    {
        Super::onPostPropertyChange();

        ITF_ASSERT(m_slideBarWidth > 0.0f);
        ITF_ASSERT(m_maxValue > 0.0f);
    }
#endif //ITF_SUPPORT_EDITOR

    bbool UISliderComponent::onValidate(const bbool _isValidate, const i32 _controllerID /*= -1*/)
    {
        bbool result = Super::onValidate(_isValidate, _controllerID);
        if (m_itemCursor)
        {
            m_itemCursor->onValidate(_isValidate);
        }
        return result;
    }

    bbool UISliderComponent::onSelected(const bbool _isSelected)
    {
        bbool result = Super::onSelected(_isSelected);
        if (m_itemCursor)
        {
            m_itemCursor->onSelected(_isSelected);
        }

        if (!_isSelected)
        {
            m_slideDir = SlideDirection_None;
            m_slideMode = SlideMode_None;
        }

        return result;
    }

    void UISliderComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        if (m_forceUpdateCursorPos)
        {
            if (!getIsInTransition())
            {
                m_forceUpdateCursorPos = bfalse;
            }

            updateCursorPosition(m_valuePercentage);
        }
        else if (isSliding())
        {
            updateSliderValue();
        }
    }

    void UISliderComponent::updateSliderValue()
    {
        if (m_itemCursor && m_itemBar)
        {
            if (m_slideMode == SlideMode_TouchPadStep)
            {
                if (F32_ALMOST_EQUALS(m_lastTouchPos.x(), m_itemCursor->getAbsolutePosition().x(), Slider_DeadZone))
                {
                    m_slideDir = SlideDirection_None;
                }
            }

            if (m_slideDir != SlideDirection_None)
            {
                f32 value = (m_slideDir == SlideDirection_Left) ? -m_cursorSpeed : m_cursorSpeed;
                setValuePercentage(m_valuePercentage + value);
            }
        }
    }

    void UISliderComponent::showValueText()
    {
        if (m_enableValueText)
        {
            String8 valueText;
            valueText.setTextFormat("%.0f", getValue());
            setText(valueText);
        }
        else
        {
            setText("");
        }
    }

    void UISliderComponent::onSlide(const StringID& _input)
    {
        if (m_needPressValidateToSlide)
        {
            if (_input == input_actionID_onPressed)
            {
                m_slideMode = SlideMode_Controller;
                setUIState(UI_STATE_IsSliding, btrue);
            }
            else if (_input == input_actionID_onReleased)
            {
                m_slideMode = SlideMode_None;
                setUIState(UI_STATE_IsSliding, bfalse);
            }
        }
        else if (_input == input_actionID_LeftHold || _input == input_actionID_RightHold)
        {
            m_slideMode = SlideMode_Controller;
            setUIState(UI_STATE_IsSliding, btrue);
        }

        if (hasUIState(UI_STATE_IsSliding))
        {
            if (_input == input_actionID_LeftHold)
            {
                m_slideDir = SlideDirection_Left;
            }
            else if(_input == input_actionID_RightHold)
            {
                m_slideDir = SlideDirection_Right;
            }
            else if (_input == input_actionID_LeftStickRelease)
            {
                m_slideDir = SlideDirection_None;

                if (!m_needPressValidateToSlide)
                {
                    m_slideMode = SlideMode_None;
                    setUIState(UI_STATE_IsSliding, bfalse);
                }
            }
        }
    }

    void UISliderComponent::onSlideBegin(const Vec2d& _pos)
    {
        if (m_itemCursor)
        {
            if (m_itemCursor->contains(_pos))
            {
                m_slideMode = SlideMode_TouchPadStroke;
                m_strokeOffset = _pos - m_itemCursor->getAbsolutePosition();
            }
            else
            {
                m_slideMode = SlideMode_TouchPadStep;
                slideByTouch(_pos);
            }
        }

        setUIState(UI_STATE_IsSliding, btrue);
        m_lastTouchPos = _pos;
    }

    void UISliderComponent::onSliding(const Vec2d& _pos)
    {
        if (m_slideMode == SlideMode_TouchPadStep)
        {
            slideByTouch(_pos);
        }
        else if (m_slideMode == SlideMode_TouchPadStroke)
        {
            slideByStroke(_pos);
        }

        m_lastTouchPos = _pos;
    }

    void UISliderComponent::onSlideEnd(const Vec2d& _pos)
    {
        m_slideDir = SlideDirection_None;
        setUIState(UI_STATE_IsSliding, bfalse);
    }

    void UISliderComponent::setValuePercentage(f32 _val, bbool _updatePos)
    {
        _val = Clamp(_val, 0.0f, 1.0f);

        m_valuePercentage = _val;

        if (_updatePos)
            updateCursorPosition(_val);
        
        showValueText();
    }

    void UISliderComponent::setValue(f32 _val, bbool _updatePos)
    {
        setValuePercentage(_val / m_maxValue, _updatePos);
    }

    void UISliderComponent::updateCursorPosition(f32 _val)
    {
        if (m_itemCursor && m_itemBar)
        {
            f32 x = m_itemBar->getAbsolutePosition().x() + m_slideBarWidth * _val * (f32)GFX_ADAPTER->getScreenWidth();
            f32 y = m_itemBar->getAbsolutePosition().y();
            m_itemCursor->setAbsolutePosition(Vec2d(x + m_fixPos_x, y + m_fixPos_y));
        }
    }

    void UISliderComponent::slideByTouch(Vec2d _pos)
    {
        if (m_itemBar && m_itemCursor)
        {
            if(!m_itemBar->contains(_pos))
            {
                m_slideDir = SlideDirection_None;
            }
            else
            {
                f32 cursorAbsolutePositionX = m_itemCursor->getAbsolutePosition().x();
                if (_pos.x() < cursorAbsolutePositionX)
                {
                    m_slideDir = SlideDirection_Left;
                }
                else if (_pos.x() > cursorAbsolutePositionX)
                {
                    m_slideDir = SlideDirection_Right;
                }
            }
        }
    }

    void UISliderComponent::slideByStroke(Vec2d _pos)
    {
        if(m_itemBar && m_itemCursor)
        {
            f32 cursorPosX = m_itemBar->getAbsolutePosition().x() + m_fixPos_x;
            f32 cursorPosY = m_itemBar->getAbsolutePosition().y() + m_fixPos_y;
            f32 width = m_slideBarWidth * (f32)GFX_ADAPTER->getScreenWidth();
            cursorPosX = Clamp(_pos.x() - m_strokeOffset.x(), cursorPosX, cursorPosX + width);
            m_itemCursor->setAbsolutePosition(Vec2d(cursorPosX, cursorPosY));
            f32 value = (cursorPosX - m_itemBar->getAbsolutePosition().x()) / width;
            setValuePercentage(value, bfalse);
        }
    }

    bbool UISliderComponent::isSliding()
    {
        return ((hasUIState(UI_STATE_IsSliding) && m_slideMode != SlideMode_None)
            && (m_slideDir != SlideDirection_None || m_slideMode == SlideMode_TouchPadStroke));
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UISliderComponent_Template)
        BEGIN_SERIALIZATION_CHILD(UISliderComponent_Template)
    END_SERIALIZATION()

    UISliderComponent_Template::UISliderComponent_Template()
    {
    }

    UISliderComponent_Template::~UISliderComponent_Template()
    {    
    }
}