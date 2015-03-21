#include "precompiled_gameplay.h"

#ifndef _ITF_UIITEMCHECK_H_
#include "gameplay/components/UI/UIItemCheck.h"
#endif //_ITF_UIITEMCHECK_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#include "engine/actors/managers/UIMenuManager.h"

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(UIItemCheck)
    BEGIN_SERIALIZATION_CHILD(UIItemCheck)
        SERIALIZE_MEMBER("isChecked", m_initChecked);
    END_SERIALIZATION()

    UIItemCheck::UIItemCheck() 
    : m_initChecked(0)
    , m_currentChecked(0)
    {
        addUIState(UI_State_SmartItem);
    }

    
    UIItemCheck::~UIItemCheck()
    {    
    }

    void UIItemCheck::onActorLoaded(Pickable::HotReloadType _type)
    {
        Super::onActorLoaded(_type);

    }

    void UIItemCheck::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

        if(UITextBox* textBox = m_actor->GetComponent<UITextBox>())
        {
            textBox->setActorScaleFactor(getTemplate()->m_iconScaleFactor);
        }

        applyChecked(m_initChecked);
        m_currentChecked = m_initChecked;
    }

    

    void UIItemCheck::Update(f32 _dt)
    {
        Super::Update(_dt);

       
    }

    bbool UIItemCheck::onValidate(const bbool _isValidate, const i32 _controllerID)
    {
        bbool res = Super::onValidate(_isValidate, _controllerID);

        if(_isValidate)
        {
            if(onChecked((m_currentChecked+1)%getTemplate()->m_tagText.size()))
            {
                UI_MENUMANAGER->playSound(UIMenu_Sound_Check, UIMenu_Sound_Check_DRC, m_actor);
            }
        }

        return res;
    }

    bbool UIItemCheck::onSelected(const bbool _isSelected)
    {
        if(Super::onSelected(_isSelected))
        {

            return btrue;
        }

        return bfalse;
    }

    bbool UIItemCheck::onChecked(const u32 _checked)
    {
        if(m_currentChecked!=_checked)
        {   
            applyChecked(_checked);
            m_currentChecked = _checked;

            return btrue;
        }

        return bfalse;
    }

    void UIItemCheck::applyChecked(u32 _checked)
    {
        if(_checked<getTemplate()->m_tagText.size())
        {
            if( UITextBox* textBox = m_actor->GetComponent<UITextBox>())
            {
                String8 text;
                text.setTextFormat("%s%s", getTemplate()->m_tagText[_checked].getText().cStr(), textBox->getInitText().cStr());
                textBox->setText(text);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(UIItemCheck_Template)
    BEGIN_SERIALIZATION_CHILD(UIItemCheck_Template)
    SERIALIZE_MEMBER("iconScalefactor", m_iconScaleFactor);
    SERIALIZE_OBJECT("tagUnchecked", m_tagText[0]);
    SERIALIZE_OBJECT("tagChecked", m_tagText[1]);
    SERIALIZE_CONTAINER_OBJECT("tagText", m_tagText);
    END_SERIALIZATION()


    UIItemCheck_Template::UIItemCheck_Template()
    : m_iconScaleFactor(1.0f)
    , m_tagText(2)
    { 
        m_tagText[0] = SmartLocId("[_] ", LocalisationId::Invalid);
        m_tagText[1] = SmartLocId("[X] ", LocalisationId::Invalid);
    }

    
    UIItemCheck_Template::~UIItemCheck_Template()
    {    
    }

}
