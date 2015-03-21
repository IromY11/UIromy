#ifndef _ITF_UIITEMDROPDOWN_H_
#define _ITF_UIITEMDROPDOWN_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_UIMENUSCROLL_H_
#include "gameplay/components/UI/UIMenuScroll.h"
#endif //_ITF_UIMENUSCROLL_H_

#ifndef _ITF_UISCROLLBAR_H_
#include "gameplay/Components/UI/UIScrollbar.h"
#endif // _ITF_UISCROLLBAR_H_

#ifndef _ITF_UIITEMBASIC_H_
#include "gameplay/components/UI/UIItemBasic.h"
#endif //_ITF_UIITEMBASIC_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

namespace ITF
{
	class UIItemDropdown : public UIMenuScroll
	{
		DECLARE_OBJECT_CHILD_RTTI(UIItemDropdown, UIMenuScroll, 2757273146);

	public:

		DECLARE_SERIALIZE()

		UIItemDropdown();
		virtual ~UIItemDropdown();

		virtual void		Update(f32 _dt);
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual bbool       valide(i32 _controllerID = -1, StringID _input = StringID::InvalidId);
        virtual bbool       back(i32 _controllerID = -1, StringID _input = StringID::InvalidId);
		virtual bbool		onValidate(const bbool _isValidate, const i32 _controllerID = -1);
		//virtual void		onInput(const i32 _controllerID, const f32 _axis, const StringID _input);
		virtual void		onFinalizeLoad();
		virtual bbool		onSelected(const bbool _isSelected);

		ITF_INLINE bbool	getIsExpanded() const { return m_isExpanded; }
		
	protected:
		ITF_INLINE const class UIItemDropdown_Template* getTemplate() const;
				
		ObjectRef				m_selectedItem;
		bbool					m_isExpanded;
		bbool					m_lastExpanded;
        i32                     m_defaultItemVisiblesCount;
	};

	//////////////////////////////////////////////////////////////////////////

	class UIItemDropdown_Template : public UIMenuScroll_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(UIItemDropdown_Template, UIMenuScroll_Template, 2373982399);
		DECLARE_SERIALIZE()
		DECLARE_ACTORCOMPONENT_TEMPLATE(UIItemDropdown);

	public:
		UIItemDropdown_Template();
		~UIItemDropdown_Template(){};
	};

	ITF_INLINE const UIItemDropdown_Template* UIItemDropdown::getTemplate() const
	{
		return static_cast<const UIItemDropdown_Template*>(m_template);
	}
}

#endif
