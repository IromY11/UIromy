#ifndef _ITF_UISCROLLBAR_H_
#define _ITF_UISCROLLBAR_H_


#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

namespace ITF
{

	class UIScrollbar : public UIComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(UIScrollbar, UIComponent, 1801987393);

	public:
		DECLARE_SERIALIZE()

		UIScrollbar();
		virtual ~UIScrollbar();

		virtual     void        Update              ( f32 _deltaTime );

	protected:
		ITF_INLINE const class UIScrollbar_Template*  getTemplate() const;

		f32						m_speed;
		f32						m_scrollValue;

		StringID				m_textBoxName;
	};

	//---------------------------------------------------------------------------------------------------

	class UIScrollbar_Template : public UIComponent_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(UIScrollbar_Template, UIComponent_Template, 59015731);
		DECLARE_SERIALIZE()
		DECLARE_ACTORCOMPONENT_TEMPLATE(UIScrollbar);

	public:

		struct Style
		{
			DECLARE_SERIALIZE()

			Style();


		};

		UIScrollbar_Template();
		~UIScrollbar_Template() {}

		ITF_VECTOR<Style>		m_styles;
	};


	ITF_INLINE const UIScrollbar_Template*  UIScrollbar::getTemplate() const {return static_cast<const UIScrollbar_Template*>(m_template);}
}
#endif // _ITF_UISCROLLBAR_H_
