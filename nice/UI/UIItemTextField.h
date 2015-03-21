#ifndef _ITF_UIITEMTEXTFIELD_H_
#define _ITF_UIITEMTEXTFIELD_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_UIITEMBASIC_H_
#include "gameplay/components/UI/UIItemBasic.h"
#endif //_ITF_UIITEMBASIC_H_

namespace ITF
{
	class UIItemTextField : public UIItemBasic
	{
		DECLARE_OBJECT_CHILD_RTTI(UIItemTextField, UIItemBasic, 1368743795);

	public:

		DECLARE_SERIALIZE()

		UIItemTextField();
		virtual ~UIItemTextField();

		virtual void				Update(f32 _dt);
		virtual bbool				onValidate(const bbool _isValidate, const i32 _controllerID = -1);

		void						refreshDialog();
		void						setDialogLoc(LocalisationId _locId);
		void						setDialogName(const String8 & _dialogName);

		ITF_INLINE LocalisationId	getDialogLoc() { return m_dialogLoc; }
		ITF_INLINE String8		    getDialogName() { return m_dialogName; }

		ITF_INLINE void				setIsPassword(bbool _isPassword);

	protected:
		ITF_INLINE const class UIItemTextField_Template* getTemplate() const;
	
		bbool				m_isPassword;
		bbool				m_waitingForKeyboard;
		String8				m_dialogName;
		String8				m_dialogNameRaw;
		String8				m_originalText;
		LocalisationId		m_dialogLoc;
		InputAdapter::VKeyboardStyle m_inputStyle;
        u32                 m_dialogMaxChar;
        bbool               m_dialogAcceptSpace;
	};

	//////////////////////////////////////////////////////////////////////////

	class UIItemTextField_Template : public UIItemBasic_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(UIItemTextField_Template, UIItemBasic_Template, 3827081158);
		DECLARE_SERIALIZE()
		DECLARE_ACTORCOMPONENT_TEMPLATE(UIItemTextField);

	public:
		UIItemTextField_Template();
		~UIItemTextField_Template();
	};

	ITF_INLINE const UIItemTextField_Template* UIItemTextField::getTemplate() const
	{
		return static_cast<const UIItemTextField_Template*>(m_template);
	}
}
#endif
