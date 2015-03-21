#ifndef _ITF_UIITEM_ON_OFF_H_
#define _ITF_UIITEM_ON_OFF_H_

#ifndef _ITF_UIITEMBASIC_H_
#include "gameplay/components/UI/UIItemBasic.h"
#endif //_ITF_UIITEMBASIC_H_

#ifndef _ITF_UIITEM_H_
#include "gameplay/components/UI/UIItem.h"
#endif //_ITF_UIITEM_H_

#ifndef _ITF_UIUTILS_H_
#include "gameplay/components/UI/UIUtils.h"
#endif //_ITF_UIUTILS_H_

namespace ITF
{
	#define UIItemOnOff_CRC		ITF_GET_STRINGID_CRC(UIItemOnOff, 946021010)
	class UIItemOnOff : public UIItemBasic
	{
		DECLARE_OBJECT_CHILD_RTTI(UIItemOnOff, UIItemBasic, UIItemOnOff_CRC);

	public:
		DECLARE_SERIALIZE()  

		UIItemOnOff();
		virtual ~UIItemOnOff();

		virtual void onActorLoaded(Pickable::HotReloadType _type);
		virtual void onFinalizeLoad();
		virtual void onStartDestroy(bbool _hotReload);
		virtual void Update(f32 _dt);
		virtual bbool onSelected(const bbool _isSelected);
		virtual bbool onLocked(const bbool _isLocked);
		virtual bbool onDown(const bbool _isDown);
		virtual bbool onActivating(const bbool _isActivate, const i32 _controllerID = -1);
		virtual bbool onIsActivating(f32 _dt);

		virtual StringID getAnimSelected();
		virtual StringID getAnimUnselected();

		StringID getAnimOn();
		StringID getAnimOff();
		StringID getAnimActivating();
		StringID getAnimActive();


		bbool	mb_isOn;
		bbool	mb_isActivating;
		bbool	mb_isActive;
		bbool	isOn() const {return mb_isOn;}
		bbool	isActivating() const {return mb_isActivating;}
		bbool	isActive() const {return mb_isActive;}

		void	setOn();
		void	setOff();
		void	setActive();


	protected:

		ITF_INLINE const class UIItemOnOff_Template* getTemplate() const;
	};


	#define UIItemOnOff_Template_CRC		ITF_GET_STRINGID_CRC(UIItemOnOff_Template, 1499580980)
	class UIItemOnOff_Template : public UIItemBasic_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(UIItemOnOff_Template, UIItemBasic_Template, 1499580980);
		DECLARE_SERIALIZE()
		DECLARE_ACTORCOMPONENT_TEMPLATE(UIItemOnOff);

	public:

		UIItemOnOff_Template();
		~UIItemOnOff_Template();

		StringID m_animOn;
		StringID m_animOff;
		StringID m_animActivating;
		StringID m_animActive;

	};

	ITF_INLINE const class UIItemOnOff_Template* UIItemOnOff::getTemplate() const
	{
		return static_cast<const UIItemOnOff_Template*>(m_template);
	}

}
#endif // _ITF_UIITEM_ON_OFF_H_
