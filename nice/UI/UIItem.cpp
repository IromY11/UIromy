#include "precompiled_gameplay.h"

#ifndef _ITF_UIITEM_H_
#include "gameplay/components/UI/UIItem.h"
#endif //_ITF_UIITEM_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(UIItem_Template)
    IMPLEMENT_OBJECT_RTTI(UIItem)

    UIItem::UIItem()
    {
        addUIState(UI_STATE_Item);
    }
    
    UIItem::~UIItem()
    {    
    }

    void UIItem::onHiddingBegin()
    {
        Super::onHiddingBegin();

        onActivating(bfalse, U32_INVALID);
    }

    void UIItem::Update(f32 _dt)
    {
        Super::Update(_dt);

        if(hasUIState(UI_STATE_IsActivating) && !onIsActivating(_dt))
        {
            onActivating(bfalse, U32_INVALID);
        }
    }

    void UIItem::setIsLocked(const bbool _isLocked, const UIState _lockedFlag)
    {
        ITF_WARNING_CATEGORY(GPP, m_actor, (_lockedFlag & ~UI_STATE_LockedMask)==0, "You can't lock item with a no locked mask");

        if(hasUIState(_lockedFlag)!=_isLocked)
        {
            setUIState(_lockedFlag, _isLocked);

            bbool isLocked = !matchUIState(UI_STATE_None, UI_STATE_LockedMask);

            if(onLocked(isLocked))
            {
                const ObjectRefList& componentObjectList = getChildObjectsList();
                for(ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
                {
                    const Actor* actor = (const Actor*) iter->getObject();
                    if(UIItem* component = actor->GetComponent<UIItem>())
                    {
                        component->setIsLocked(isLocked, UI_STATE_LockedByParent);
                    }
                }
            }
        }
    }

    void UIItem::setIsDown(const bbool _isDown)
    {
        if(hasUIState(UI_STATE_DownByDefault)!=_isDown)
        {
            setUIState(UI_STATE_DownByDefault, _isDown);
            onDown(hasUIState(UI_STATE_IsSelected) || hasUIState(UI_STATE_IsValidate) || hasUIState(UI_STATE_DownByDefault) || hasUIState(UI_STATE_IsActivating) );
        }
    }

    bbool UIItem::getIsLocked() const
    {
        return hasUIState(UI_STATE_IsLocked);
    }

    bbool UIItem::getIsDown() const
    {
        return hasUIState(UI_STATE_IsDown);
    }

    bbool UIItem::onValidate(const bbool _isValidate, const i32 _controllerID)
    {
        if((!_isValidate || matchUIState(UI_STATE_None, UI_STATE_CanNotBeValidate)) && hasUIState(UI_STATE_IsValidate)!=_isValidate)
        {
            setUIState(UI_STATE_IsValidate, _isValidate);
            onDown(hasUIState(UI_STATE_IsSelected) ||hasUIState(UI_STATE_IsValidate) || hasUIState(UI_STATE_DownByDefault) || hasUIState(UI_STATE_IsActivating));

            if(_isValidate)
            {
                onActivating(_isValidate, _controllerID);
            }

            return btrue;
        }
        
        onActivating(_isValidate, _controllerID);

        return bfalse;
    }

    bbool UIItem::onBack(const bbool _isBack, const i32 _controllerID)
    {
        if(hasUIState(UI_STATE_IsBack)!=_isBack)
        {
            setUIState(UI_STATE_IsBack, _isBack);

            if(_isBack && hasUIState(UI_STATE_ActivateByBack))
            {
                onActivating(_isBack, _controllerID);
            }
            

            return btrue;
        }

        return bfalse;
    }

    bbool UIItem::onAction(const bbool _isAction, const i32 _controllerID)
    {
        if(hasUIState(UI_STATE_IsAction)!=_isAction)
        {
            setUIState(UI_STATE_IsAction, _isAction);

            if(_isAction && hasUIState(UI_STATE_ActivateByAction))
            {
                onActivating(_isAction, _controllerID);
            }


            return btrue;
        }

        return bfalse;
    }

    bbool UIItem::onActivating(const bbool _isActivate, const i32 _controllerID)
    {
        if(hasUIState(UI_STATE_IsActivating)!=_isActivate)
        {
            setUIState(UI_STATE_IsActivating, _isActivate);
            onDown(hasUIState(UI_STATE_IsSelected) ||hasUIState(UI_STATE_IsValidate) || hasUIState(UI_STATE_DownByDefault) || hasUIState(UI_STATE_IsActivating));

            return btrue;
        }

        return bfalse;
    }

    bbool UIItem::onIsActivating(f32 _dt)
    {
        return bfalse;
    }

    bbool UIItem::onSelected(const bbool _isSelected)
    {
        if(hasUIState(UI_STATE_IsSelected)!=_isSelected)
        {
            setUIState(UI_STATE_IsSelected, _isSelected);
            onDown(hasUIState(UI_STATE_IsSelected) ||hasUIState(UI_STATE_IsValidate) || hasUIState(UI_STATE_DownByDefault) || hasUIState(UI_STATE_IsActivating));

            return btrue;
        }

        return bfalse;
    }

    bbool UIItem::onLocked(const bbool _isLocked) 
    {
        if(hasUIState(UI_STATE_IsLocked)!=_isLocked)
        {
            setUIState(UI_STATE_IsLocked, _isLocked);
            return btrue;
        }

        return bfalse;
    }

    bbool UIItem::onDown(const bbool _isDown) 
    {
        if(hasUIState(UI_STATE_IsDown)!=_isDown)
        {
            setUIState(UI_STATE_IsDown, _isDown);
            return btrue;
        }

        return bfalse;
    }

}
