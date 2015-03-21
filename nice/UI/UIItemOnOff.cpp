#include "precompiled_gameplay.h"

#ifndef _ITF_UIITEM_ON_OFF_H_
#include "gameplay/Components/UI/UIItemOnOff.h"
#endif //_ITF_UIITEM_ON_OFF_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_RENDERBOXCOMPONENT_H_
#include "gameplay/Components/UI/RenderBoxComponent.h"
#endif //_ITF_RENDERBOXCOMPONENT_H_

#ifndef _ITF_ANIMMESHVERTEXCOMPONENT_H_
#include "engine/actors/components/AnimMeshVertexComponent.h"
#endif //_ITF_ANIMMESHVERTEXCOMPONENT_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT_H_
#include "engine/actors/components/texturegraphiccomponent.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

namespace ITF
{
	///////////////////////////////////////////////////////////////////////////////////////////

	IMPLEMENT_OBJECT_RTTI(UIItemOnOff)
	
	BEGIN_SERIALIZATION_CHILD(UIItemOnOff)
	SERIALIZE_MEMBER("selectTextStyle", m_selectTextStyle);
	SERIALIZE_MEMBER("selectAnimMeshVertex", m_selectAnimMeshVertex);
	END_SERIALIZATION()

	UIItemOnOff::UIItemOnOff() 
	: mb_isOn(bfalse)
	, mb_isActivating(bfalse)
	, mb_isActive(bfalse)
	{
	}

	UIItemOnOff::~UIItemOnOff()
	{    
	}

	void UIItemOnOff::onActorLoaded(Pickable::HotReloadType _type)
	{
		Super::onActorLoaded(_type);
	}

	void UIItemOnOff::onFinalizeLoad()
	{
		Super::onFinalizeLoad();
	}

	void UIItemOnOff::onStartDestroy( bbool _hotReload )
{
		Super::onStartDestroy(_hotReload);
	}

	StringID UIItemOnOff::getAnimSelected()
	{
		return (Super::getAnimSelected());
	}

	StringID UIItemOnOff::getAnimUnselected()
	{
		return (Super::getAnimUnselected());
	}

	StringID UIItemOnOff::getAnimOn()
	{
		return getTemplate()->m_animOn;
	}

	StringID UIItemOnOff::getAnimOff()
	{
		return getTemplate()->m_animOff;
	}

	StringID UIItemOnOff::getAnimActivating()
	{
		return getTemplate()->m_animActivating;
	}

	StringID UIItemOnOff::getAnimActive()
	{
		return getTemplate()->m_animActive;
	}

	void UIItemOnOff::setOn()
	{
		mb_isOn = btrue;
		setAnim(getAnimOn());
		mb_isActivating = bfalse;
		mb_isActive = bfalse;
	}

	void UIItemOnOff::setOff()
	{
		mb_isOn = bfalse;
		setAnim(getAnimOff());
		mb_isActivating = bfalse;
		mb_isActive = bfalse;
	}

	void UIItemOnOff::setActive()
	{
		mb_isOn = bfalse;
		mb_isActivating = bfalse;
		mb_isActive = btrue;
		setAnim(getAnimActive());
	}

	bbool UIItemOnOff::onSelected(const bbool _isSelected)
	{
		return (Super::onSelected(_isSelected));
	}

	bbool UIItemOnOff::onLocked(const bbool _isLocked)
	{
		return(Super::onLocked(_isLocked));
	}

	bbool UIItemOnOff::onDown(const bbool _isDown)
	{
		return(Super::onDown(_isDown));
	}

	bbool UIItemOnOff::onActivating(const bbool _isActivate, const i32 _controllerID)
	{
		if( mb_isOn && _isActivate )
		{
			mb_isActivating = btrue;
			setAnim(getAnimActivating());
			return btrue;
		}
		else 
			return bfalse;
	}

	bbool UIItemOnOff::onIsActivating(f32 _dt)
	{
		return(Super::onIsActivating(_dt));
	}

	void UIItemOnOff::Update(f32 _dt)
	{
		Super::Update(_dt);

		if( mb_isActivating )
		{
			if( m_animComponent->isSubAnimFinished() )
			{
				if( getTemplate()->m_animActive.isValid() )
				{
					setActive();
				}	
				else
				{
					setOff();
				}
			}
		}
	}


	///////////////////////////////////////////////////////////////////////////////////////////

	IMPLEMENT_OBJECT_RTTI(UIItemOnOff_Template)

	BEGIN_SERIALIZATION_CHILD(UIItemOnOff_Template)
// 	SERIALIZE_MEMBER("animUnselectedOff", m_animUnselectedOff);
// 	SERIALIZE_MEMBER("animSelectedOff", m_animSelectedOff);
	SERIALIZE_MEMBER("animOn", m_animOn);
	SERIALIZE_MEMBER("animOff", m_animOff);
	SERIALIZE_MEMBER("animActivating", m_animActivating);
	SERIALIZE_MEMBER("animActive", m_animActive);
	END_SERIALIZATION()


	UIItemOnOff_Template::UIItemOnOff_Template()
	{  
		m_animOn.invalidate();
		m_animOff.invalidate();
		m_animActivating.invalidate();
		m_animActive.invalidate();
	}

	UIItemOnOff_Template::~UIItemOnOff_Template()
	{    
	}

}
