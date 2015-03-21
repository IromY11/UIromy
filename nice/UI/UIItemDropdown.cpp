#include "precompiled_gameplay.h"

#ifndef _ITF_UIITEMDROPDOWN_H_
#include "gameplay/components/UI/UIItemDropdown.h"
#endif //_ITF_UIITEMDROPDOWN_H_

#include "engine/actors/managers/UIMenuManager.h"

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(UIItemDropdown)
	BEGIN_SERIALIZATION_CHILD(UIItemDropdown)
END_SERIALIZATION()

UIItemDropdown::UIItemDropdown()
	: m_selectedItem(ObjectRef::InvalidRef)
	, m_isExpanded(bfalse)
	, m_lastExpanded(bfalse)
    , m_defaultItemVisiblesCount(1)
{
    addUIState(UI_State_SmartItem);
}

UIItemDropdown::~UIItemDropdown()
{

}

void UIItemDropdown::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_defaultItemVisiblesCount = m_itemVisiblesCount;
    m_itemVisiblesCount = 1;
}

bbool UIItemDropdown::valide(i32 _controllerID, StringID _input)
{
    if(Super::valide(_controllerID, _input))
    {
        m_isExpanded = bfalse;
        m_itemVisiblesCount = 1;
        gotoFirstItemIndex(getItemIndex(getSelectedItem()));
        UI_MENUMANAGER->removeUIMenu(this);

        return btrue;
    }

    return bfalse;
}

bbool UIItemDropdown::back(i32 _controllerID, StringID _input)
{
    if(Super::back(_controllerID, _input))
    {
        m_isExpanded = bfalse;
        m_itemVisiblesCount = 1;
        gotoFirstItemIndex(getItemIndex(getSelectedItem()));
        UI_MENUMANAGER->removeUIMenu(this);

        return btrue;
    }

    return bfalse;
}

bbool UIItemDropdown::onValidate(const bbool _isValidate, const i32 _controllerID /*= -1*/ )
{
    if(Super::onValidate(_isValidate, _controllerID))
    {
        if(_isValidate)
        {
            m_isExpanded = btrue;
            m_itemVisiblesCount = m_defaultItemVisiblesCount;
            UI_MENUMANAGER->insertUIMenu(getParentComponent<UIMenu>(), this);
            gotoFirstItemIndex(getItemIndex(getSelectedItem()));
        }

        return btrue;
    }

    return bfalse;
}


void UIItemDropdown::onFinalizeLoad()
{
	Super::onFinalizeLoad();

	onSelected(bfalse);
}

void UIItemDropdown::Update( f32 _dt )
{
	Super::Update(_dt);

	if(m_isExpanded != m_lastExpanded)
	{
		// selected this frame
		if(m_isExpanded == btrue && m_lastExpanded == bfalse)
		{
			for(u32 n = 0; n < m_currentItems.size(); ++n)
			{
				UIComponent* item = m_currentItems[n].getUIComponent();

				// to foreground
				item->setAbsoluteDepth(item->getAbsoluteDepth() + 10);
			}
		}
		// deselected this frame
		else if(m_isExpanded == bfalse && m_lastExpanded == btrue)
		{
			for(u32 n = 0; n < m_currentItems.size(); ++n)
			{
				UIComponent* item = m_currentItems[n].getUIComponent();

				// to background
				item->setAbsoluteDepth(item->getAbsoluteDepth() - 10);
			}
		}
	}

	m_lastExpanded = m_isExpanded;
}

bbool UIItemDropdown::onSelected( const bbool _isSelected )
{
    if(Super::onSelected(_isSelected))
    {
        if(UIItem* curItem = getSelectedItem())
        {
            curItem->onSelected(_isSelected);
        }

        return btrue;
    }

    return bfalse;
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(UIItemDropdown_Template)
	BEGIN_SERIALIZATION_CHILD(UIItemDropdown_Template)
END_SERIALIZATION()

UIItemDropdown_Template::UIItemDropdown_Template()
{

}

}
