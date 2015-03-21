#include "precompiled_gameplay.h"

#ifndef _ITF_UISCROLLBAR_H_
#include "gameplay/components/UI/UIScrollbar.h"
#endif //_ITF_UISCROLLBAR_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenuBasic.h"
#endif //_ITF_UIMENU_H_

namespace ITF
{

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(UIScrollbar)
BEGIN_SERIALIZATION_CHILD(UIScrollbar)
	BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
		SERIALIZE_MEMBER("textBoxFriendly", m_textBoxName);
		SERIALIZE_MEMBER("speed", m_speed);
	END_CONDITION_BLOCK()
END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////
UIScrollbar::UIScrollbar()
: m_speed	(100.0f)
, m_scrollValue	(0.0f)
{
	
}

UIScrollbar::~UIScrollbar()
{

}

void UIScrollbar::Update( f32 _deltaTime )
{
	Super::Update(_deltaTime);

	if( UIComponent* parentMenu = getParentComponent() )
	{
		UIMenu* menu = static_cast<UIMenu*>(parentMenu);
		
		// scrolling
		float scrollDelta = -menu->getAnalogDir().y();
		m_scrollValue += scrollDelta;

		// scroll for textbox
		if(UITextBox* textBox = menu->getChildComponent<UITextBox>(m_textBoxName))
		{
			// scroll the text area
			FontTextArea& area = textBox->getTextArea();
			area.setScrollPx((scrollDelta * m_speed) * _deltaTime);
		}
	}
}

//-------------------------------------------------------------------------------------
BEGIN_SERIALIZATION_SUBCLASS(UIScrollbar_Template,Style)
END_SERIALIZATION()

UIScrollbar_Template::Style::Style()
{

}

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(UIScrollbar_Template)
	BEGIN_SERIALIZATION_CHILD(UIScrollbar_Template)
	SERIALIZE_CONTAINER_OBJECT("styles", m_styles);
END_SERIALIZATION()

UIScrollbar_Template::UIScrollbar_Template() : 
m_styles(1)
{

}

} 