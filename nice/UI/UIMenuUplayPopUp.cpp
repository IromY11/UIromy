#include "precompiled_gameplay.h"

#ifndef _ITF_UIMENUUPLAYPOPUP_H_
#include "gameplay/components/UI/UIMenuUplayPopUp.h"
#endif //_ITF_UIMENUUPLAYPOPUP_H_

#ifndef _ITF_MULTITEXTBOXCOMPONENT_H_
#include "gameplay/components/UI/MultiTextBoxComponent.h"
#endif //_ITF_MULTITEXTBOXCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT_H_
#include "engine/actors/components/texturegraphiccomponent.h"
#endif // _ITF_TEXTUREGRAPHICCOMPONENT_H_

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIMenuUplayPopUp)
    BEGIN_SERIALIZATION_CHILD(UIMenuUplayPopUp)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("defaultItem", m_defaultItemFriendly);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    UIMenuUplayPopUp::UIMenuUplayPopUp()
        : UIMenu()
        , m_finalPos(Vec2d::Zero)
		, m_sourcePos(Vec2d::Zero)
        , m_animEnd(bfalse)
        , m_updateTime(0.f)
        , m_finalPosRatio(Vec2d::Zero)
        , m_screenSize(Vec2d::Zero)
#ifdef ITF_SUPPORT_EDITOR
        , m_bselected(bfalse)
#endif // ITF_SUPPORT_EDITOR
    {
    }
    
    UIMenuUplayPopUp::~UIMenuUplayPopUp()
    {    

    }

    void UIMenuUplayPopUp::onInput(const i32 _controllerID, const f32 _axis, const StringID _input)
    {
    }

    void UIMenuUplayPopUp::onTouchInput(const TouchDataArray & _touchArray, u32 _i)
    {
    }

    MultiTextBoxComponent *UIMenuUplayPopUp::getMessageTextBox()
    {
        return getChildComponent<MultiTextBoxComponent>(m_defaultItemFriendly);
    }

    void UIMenuUplayPopUp::setText(const String8& _txt, const String8& _val /* = String8::emptyString */)
    {
        if( MultiTextBoxComponent *txtBox = getMessageTextBox() )
		{
			txtBox->setText(txtBox->getTextBoxCount()-2, _txt);
			txtBox->setText(txtBox->getTextBoxCount()-1, _val);
		}
    }

    bbool UIMenuUplayPopUp::isOnBottom() const
    {
        return !(AIUtils::getAIPlatform() == Platform::DURANGO || AIUtils::getAIPlatform() == Platform::ORBIS);
    }

    f32 UIMenuUplayPopUp::getCurrentShift() const
    {
        f32 shift = m_actor->get2DPos().y() - m_sourcePos.y();
        shift -= (m_finalPos.y() - m_sourcePos.y()) * .4f;
        if (shift > 0)
            shift = 0;
        return shift;
    }

    void UIMenuUplayPopUp::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

		m_sourcePos	= getAbsolutePosition();
        m_finalPos	= getAbsolutePosition();

        m_screenSize = Pickable::getScreenSize();
        m_finalPosRatio = m_finalPos / m_screenSize;
        
        if (AIUtils::getAIPlatform() == Platform::DURANGO || AIUtils::getAIPlatform() == Platform::ORBIS)
        {
		    f32 itemHeight = 128.0f;
		    f32 itemWidth  = 1024;
		    if(TextureGraphicComponent* graphicComponent = getChildComponent<TextureGraphicComponent>(m_defaultItemFriendly))
		    {
			    //itemWidth	= graphicComponent->getSize().x();
			    //itemHeight	= graphicComponent->getSize().y();
			    itemWidth	= graphicComponent->getVisualAABB().getWidth();
			    itemHeight	= graphicComponent->getVisualAABB().getHeight();
		    }

		    m_sourcePos.x()	= m_finalPos.x() = m_screenSize.x() - itemWidth * 0.25f - m_screenSize.x() * 0.1f;
		    m_finalPos.y()	= m_screenSize.y() * 0.05f;
		    m_sourcePos.y() = -itemHeight;
        }
        else
        {
		    m_sourcePos.y() = m_screenSize.y();
        }
    }

    void UIMenuUplayPopUp::onBecomeActive()
    {
        Super::onBecomeActive();

        if ( MultiTextBoxComponent* defaultItem = getMessageTextBox() )
        {
            defaultItem->setMainAlpha(0.f);
        }
        m_animEnd = bfalse;
        m_updateTime = 0.f;

        m_actor->set2DPos(m_sourcePos);
    }

    void UIMenuUplayPopUp::Update(f32 _dt)
    {
        Super::Update(_dt);

#ifdef ITF_SUPPORT_EDITOR
        if(m_actor->isSelected()) // /!\ call on button mouse up
        {
            if(m_bselected != m_actor->isSelected())
            {// set final state
                if ( MultiTextBoxComponent* defaultItem = getMessageTextBox() )
                {
                    defaultItem->setMainAlpha(1.f);
                    m_actor->set2DPos(m_finalPos);
                }
            }
            m_finalPos = getAbsolutePosition(); // update final position
            m_bselected = btrue;
            m_animEnd = bfalse;
            m_updateTime = 0.f;
            return;
        }
        m_bselected = bfalse;
#endif // ITF_SUPPORT_EDITOR

        // don't update menu while fading (in case if fading append after menu show), to maintain display time.
        if(GAMEMANAGER->areFadeInProgress())
            return;

        if(m_animEnd)
            return;

        m_updateTime += _dt;

        const f32 appearP = getTemplate()->m_translationTime>0.f ? m_updateTime / getTemplate()->m_translationTime : 2.f;
        const f32 disapearP = getTemplate()->m_translationTime>0.f ? (m_updateTime - getTemplate()->m_translationTime - getTemplate()->m_noTextTime - getTemplate()->m_textTime) / getTemplate()->m_translationTime : 2.f;

        // recompute the final and sorce position when the screen size change
        Vec2d screenSize = Pickable::getScreenSize();
        if (m_screenSize != screenSize)
        {
            m_screenSize = screenSize;
            m_finalPos = m_finalPosRatio * m_screenSize;
            m_sourcePos.x() = m_finalPos.x();
            m_sourcePos.y() = m_screenSize.y();
        }

        if(appearP < 1.5f)
        {
			Vec2d	newPos;
			newPos.Lerp(m_sourcePos, m_finalPos, f32_Clamp(appearP, 0.0f, 1.0f));
			m_actor->set2DPos(newPos);
        }

        if(disapearP > 0.f)
        {
			Vec2d newPos;
			newPos.Lerp(m_finalPos, m_sourcePos, f32_Clamp(disapearP, 0.0f, 1.0f));
			m_actor->set2DPos(newPos);

            if(disapearP > 1.f)
                m_animEnd = btrue;
        }

        if(getOpenDuration() > getTemplate()->m_translationTime + getTemplate()->m_noTextTime)
        {
            if ( MultiTextBoxComponent* defaultItem = getMessageTextBox() )
            {
                defaultItem->setMainAlpha(f32_Clamp(defaultItem->getMainAlpha()+0.15f, 0.f, 1.f)); // tiny alpha fade
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(UIMenuUplayPopUp_Template)
    BEGIN_SERIALIZATION_CHILD(UIMenuUplayPopUp_Template)
    SERIALIZE_MEMBER("translationTime", m_translationTime);
    SERIALIZE_MEMBER("noTextTime", m_noTextTime);
    SERIALIZE_MEMBER("textTime", m_textTime);
    END_SERIALIZATION()
}
