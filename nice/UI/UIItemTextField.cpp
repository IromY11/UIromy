#include "precompiled_gameplay.h"

#ifndef _ITF_UIITEMTEXTFIELD_H_
#include "gameplay/components/UI/UIItemTextField.h"
#endif //_ITF_UIITEMTEXTFIELD_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_


namespace ITF
{
	///////////////////////////////////////////////////////////////////////////////////////////

	IMPLEMENT_OBJECT_RTTI(UIItemTextField)
	BEGIN_SERIALIZATION_CHILD(UIItemTextField)
		BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
			SERIALIZE_BOOL("isPassword", m_isPassword);
            SERIALIZE_MEMBER("dialogMaxChar", m_dialogMaxChar);
            SERIALIZE_MEMBER("dialogAcceptSpace", m_dialogAcceptSpace);
			SERIALIZE_MEMBER("dialogNameRaw", m_dialogNameRaw);
			SERIALIZE_MEMBER("dialogNameLoc", m_dialogLoc);
			SERIALIZE_ENUM_BEGIN("style", m_inputStyle);
				SERIALIZE_ENUM_VAR(InputAdapter::VK_style_full);
				SERIALIZE_ENUM_VAR(InputAdapter::VK_style_num);
				SERIALIZE_ENUM_VAR(InputAdapter::VK_style_email);
			SERIALIZE_ENUM_END(); 
		END_CONDITION_BLOCK()
	END_SERIALIZATION()

	UIItemTextField::UIItemTextField() :
		m_isPassword			(bfalse),
		m_waitingForKeyboard	(bfalse),
		m_dialogName			(""),
		m_dialogNameRaw			(""),
		m_inputStyle			(InputAdapter::VK_style_full),
        m_dialogMaxChar         ((u32)-1),
        m_dialogAcceptSpace     (btrue)
	{
		
	}

	UIItemTextField::~UIItemTextField()
	{
		
	}

	void UIItemTextField::Update( f32 _dt )
	{
		Super::Update(_dt);

#ifdef INPUT_USE_VKEYBOARD
        // check if dialog was open
		if(m_waitingForKeyboard == btrue)
		{
			// if closing && not canceled
			InputAdapter::VKeyboardState vKeystate = INPUT_ADAPTER->VKeyboardGetCurrentState();
			if(vKeystate != InputAdapter::VK_state_open)
			{
				m_waitingForKeyboard = bfalse;
				
			}
			// set textbox text from vKeyboard
			if(UITextBox* textBox = m_actor->GetComponent<UITextBox>())
			{
				String8 dialogOutput = INPUT_ADAPTER->VKeyboardGetText();
				if ( vKeystate == InputAdapter::VK_state_canceled )
				{
					textBox->setText(m_originalText);					 
				} 
				else
				{
					if ( dialogOutput != textBox->getText() )
						textBox->setText(dialogOutput);
				}
			}
			
		}
#endif // INPUT_USE_VKEYBOARD
	}

	bbool UIItemTextField::onValidate(const bbool _isValidate, const i32 _controllerID /*= -1*/ )
	{
        if(Super::onValidate(_isValidate))
        {
#ifdef INPUT_USE_VKEYBOARD
            if(_isValidate)
            {
                InputAdapter::VKeyboardOpenParams openParams;
                openParams.m_style = m_inputStyle;
                openParams.m_textDialog = m_dialogNameRaw;
                openParams.m_password = m_isPassword;
                openParams.m_textMaxSize = m_dialogMaxChar;
                openParams.m_textAcceptSpace = m_dialogAcceptSpace;
                openParams.m_controllerID = _controllerID;

                if(UITextBox* textBox = m_actor->GetComponent<UITextBox>())
                {
                    if (!m_isPassword)
                        openParams.m_textDefault = textBox->getText();
                    else
                        openParams.m_textDefault = "";

                    m_originalText = textBox->getText();

                    // sync password states.
                    FontTextArea& area = textBox->getTextArea();
                    area.setIsPassword(m_isPassword);
                }
                else
                {
                    openParams.m_textDefault = "";
                }

                m_waitingForKeyboard = btrue;
                INPUT_ADAPTER->VKeyboardOpen(openParams);
            }
#endif // INPUT_USE_VKEYBOARD

            return btrue;
        }

        return bfalse;
	}

	void UIItemTextField::setDialogLoc( LocalisationId _locId )
	{
		 m_dialogLoc = _locId;
		 refreshDialog();
	}

	void UIItemTextField::setDialogName( const String8 & _dialogName )
	{
		 m_dialogNameRaw = _dialogName;
		 refreshDialog();
	}

	void UIItemTextField::refreshDialog()
	{
		if(m_dialogNameRaw.isEmpty() && m_dialogLoc!=LocalisationId::Invalid)
		{
			m_dialogName = LOCALISATIONMANAGER->getText(m_dialogLoc);
		}
		else
		{
			m_dialogName = m_dialogNameRaw;
		}
	}

	ITF_INLINE void UIItemTextField::setIsPassword( bbool _isPassword )
	{
		m_isPassword = _isPassword;

		if(UITextBox* textBox = m_actor->GetComponent<UITextBox>())
		{
			FontTextArea& area = textBox->getTextArea();
			area.setIsPassword(m_isPassword);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	IMPLEMENT_OBJECT_RTTI(UIItemTextField_Template)
	BEGIN_SERIALIZATION_CHILD(UIItemTextField_Template)
	END_SERIALIZATION()


	UIItemTextField_Template::UIItemTextField_Template()
	{  
	}


	UIItemTextField_Template::~UIItemTextField_Template()
	{    
	}

}