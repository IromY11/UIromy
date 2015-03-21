#include "precompiled_gameplay.h"

#ifndef _ITF_UIMENUSCROLL_H_
#include "gameplay/components/UI/UIMenuScroll.h"
#endif //_ITF_UIMENUSCROLL_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif // _ITF_SCENE_H_

#ifndef _ITF_UIITEM_H_
#include "gameplay/components/UI/UIItem.h"
#endif //_ITF_UIITEM_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#include "engine/actors/managers/UIMenuManager.h"

namespace ITF
{



    // ----------------------------------------------------------------------------------------------------
    // ACTOR POOL
    // ----------------------------------------------------------------------------------------------------

    ActorPool::ActorPool()
    : m_requestedActorsCount(0)
    {

    }
    ActorPool::~ActorPool()
    {
        clear();
    }

    void ActorPool::setup(const Path& _actorPath, Actor* _owner, u32 _numPreAlloc)
    {
        if(_actorPath != m_actorPath || _owner->getRef()!=m_ownerRef)
        {
            clear();

            if(!_actorPath.isEmpty())
            {
                ACTORSMANAGER->getSpawnPoolManager().registerForRequest(_owner->getRef(), _owner->getResourceContainer(), _actorPath, _numPreAlloc, _numPreAlloc);
            }
            
            m_actorPath = _actorPath;
            m_ownerRef = _owner->getRef();
        }
    }

    void ActorPool::update()
    {
        for(u32 n=m_spawningActors.size(); n>0; --n)
        {
            u32 index = n-1;
            Actor* actor = m_spawningActors[index];
            if(!actor->isAsyncLoading())
            {
                m_spawningActors.removeAtUnordered(index);
                m_spawnedActors.push_back(actor);
            }
        }
    }

    void ActorPool::updatePoolSize()
    {
        // Spawn requested actors count
        if(m_spawnedActors.size() + m_spawningActors.size()<m_requestedActorsCount)
        {
            m_spawnedActors.reserve(Max((u32)m_spawnedActors.capacity(), m_requestedActorsCount));
            m_spawningActors.reserve(Max((u32)m_spawningActors.capacity(), m_requestedActorsCount));
            for(u32 n=m_spawnedActors.size() + m_spawningActors.size(); n<m_requestedActorsCount; ++n)
            {
                if(!m_actorPath.isEmpty())
                {
                    if(Actor* actor = ACTORSMANAGER->getSpawnPoolManager().requestSpawn(m_actorPath))
                    {
                        if(actor->isAsyncLoading())
                        {
                            m_spawningActors.push_back(actor);
                        }
                        else
                        {
                            m_spawnedActors.push_back(actor);
                        }
                    }
                }
            }
        }
        // Destroy too many actor
        else if(m_spawnedActors.size() + m_spawningActors.size()>m_requestedActorsCount)
        {
            u32 destroySpawningActorsCount = 0;
            u32 destroySpawnedActorsCount = 0;
            if(m_spawnedActors.size() >= m_requestedActorsCount)
            {
                destroySpawningActorsCount = m_spawningActors.size();
                destroySpawnedActorsCount = m_spawnedActors.size() - m_requestedActorsCount;
            }
            else if(m_spawnedActors.size() + m_spawningActors.size() >= m_requestedActorsCount)
            {
                destroySpawningActorsCount = (m_spawnedActors.size() + m_spawningActors.size()) - m_requestedActorsCount;
            }

            for(u32 n=0; n<destroySpawningActorsCount; ++n)
            {
                m_spawningActors.back()->requestDestruction();
                m_spawningActors.pop_back();
            }

            for(u32 n=0; n<destroySpawnedActorsCount; ++n)
            {
                m_spawnedActors.back()->requestDestruction();
                m_spawnedActors.pop_back();
            }
        }
    }

    void ActorPool::clear()
    {
        clearActors();

        if(!m_actorPath.isEmpty())
        {
            ACTORSMANAGER->getSpawnPoolManager().unregisterForRequest(m_ownerRef, m_actorPath);
            m_actorPath = Path::EmptyPath;
        }
    }

    u32 ActorPool::requestActor(u32 _count)
    {
        if(m_requestedActorsCount != _count)
        {
            m_requestedActorsCount = _count;
            updatePoolSize();
        }

        return m_spawnedActors.size();
    }

    Actor* ActorPool::popActor(Scene* _scene, Actor* _parent)
    {
        ITF_ASSERT_MSG(m_requestedActorsCount>0, "You try to get a actor without do a request. Use requestActor function to request actor spawn.");
        ITF_ASSERT_MSG(!m_spawnedActors.empty(), "You try to get not ready actor. Use requestActor function to know how many actors are ready.");
        
        if(m_requestedActorsCount>0)
        {
            m_requestedActorsCount--;

            if(!m_spawnedActors.empty())
            {
                Actor* actor = m_spawnedActors.back();
                _scene->registerPickable(actor);
                actor->createParentBind(bfalse, _parent, Bind::Root, 0, btrue, bfalse, bfalse, btrue);

                m_spawnedActors.pop_back();

                return actor;
            }
        }

        return NULL;
    }

    Actor* ActorPool::releaseActor(Actor* _actor)
    {
        _actor->requestDestruction();

        return NULL;
    }

    void ActorPool::clearActors()
    {
        for(u32 n=0; n>m_spawningActors.size(); ++n)
        {
            m_spawningActors[n]->requestDestruction();
        }
        m_spawningActors.clear();

        for(u32 n=0; n>m_spawnedActors.size(); ++n)
        {
            m_spawnedActors[n]->requestDestruction();
        }
        m_spawnedActors.clear();

        m_requestedActorsCount = 0;
    }

    // ----------------------------------------------------------------------------------------------------
    // UI MENU SCROLL
    // ----------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(UIMenuScroll)
    BEGIN_SERIALIZATION_CHILD(UIMenuScroll)
    SERIALIZE_MEMBER("itemOffset", m_itemOffset);
    SERIALIZE_MEMBER("itemVisiblesCount", m_itemVisiblesCount);
    SERIALIZE_MEMBER("displayOutItems", m_displayOutItems);
    SERIALIZE_MEMBER("separatorModel", m_separatorModel);
    SERIALIZE_MEMBER("itemStartPos", m_itemStartPos);
    SERIALIZE_MEMBER("separatorStartPos", m_separatorStartPos);
    END_SERIALIZATION()

#ifdef ITF_SUPPORT_EDITOR
    void UIMenuScroll::onPostPropertyChange()
    {
        clearItems();

    }
#endif //ITF_SUPPORT_EDITOR

    UIMenuScroll::UIMenuScroll()
    : m_itemVisiblesCount(4)
    , m_firstItemIndex(0)
    , m_firstItemCursor(0.0f)
    , m_itemOffset(Vec2d::Zero)
    , m_areItemDisplayed(btrue)
    , m_currentOffset(Vec2d::Zero)
    , m_movingDuration(0.0f)
    , m_isDragScrolling(bfalse)
    , m_hasTouchBegin(bfalse)
    , m_displayOutItems(bfalse)
    , m_itemStartPos(Vec3d::Zero)
    , m_separatorStartPos(Vec3d::Zero)
    {
    }

    
    UIMenuScroll::~UIMenuScroll()
    {    
    }

    void UIMenuScroll::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

        m_dragScrolling.setupDragBounceTime(getTemplate()->m_movingBounceTime);
        m_dragScrolling.setupDragBounceNorm(getTemplate()->m_movingBounceNorm);
        m_dragScrolling.setupDragStartNorm(getTemplate()->m_movingSelectionNormMax);
        m_dragScrolling.setupMomentum(getTemplate()->m_movingMomentumFriction, getTemplate()->m_movingMomentumDeceleration);

        const ITF_VECTOR<Path>& modelActorPath = getTemplate()->m_modelItemPaths;
        m_modelItemPools.resize(modelActorPath.size());
        for(u32 n=0; n<modelActorPath.size(); ++n)
        {
            m_modelItemPools[n].setup(modelActorPath[n], m_actor, m_itemVisiblesCount);
        }
    }

    void UIMenuScroll::onStartDestroy( bbool _hotReload )
{
        Super::onStartDestroy(_hotReload);

        clearItems();

        for(u32 n=0; n<m_modelItemPools.size(); ++n)
        {
            m_modelItemPools[n].clear();
        }
    }


    Vec3d UIMenuScroll::getItemAbsoluteStartPos() const
    {
        Vec3d pos = m_actor->getPos() + (m_itemStartPos.truncateTo2D() * m_actor->getScale()).to3d(m_itemStartPos.z());

        return pos;
    }

    Vec3d UIMenuScroll::getSeparatorAbsoluteStartPos() const
    {
        Vec3d pos = m_actor->getPos() + (m_separatorStartPos.truncateTo2D() * m_actor->getScale()).to3d(m_separatorStartPos.z());

        return pos;
    }

    UIItem* UIMenuScroll::computeNextItem(const Vec2d& _direction, i32 _neededState, i32 _forbiddenState) const
    {
        UIItem* currentSelectedItem = getSelectedItem();

        if(!currentSelectedItem || !currentSelectedItem->hasUIState(UI_STATE_ListElement))
        {
            UIItem* selectedItem = (UIItem*) getChildComponentToDirection(currentSelectedItem, _direction, btrue, UI_STATE_Item | _neededState, UI_STATE_CanNotBeSelected | UI_STATE_ListElement | UI_STATE_Hidden | _forbiddenState);

            if(selectedItem == currentSelectedItem)
            {
                return (UIItem*) getChildComponentToDirection(currentSelectedItem, _direction, btrue, UI_STATE_Item | UI_STATE_ListElement | _neededState, UI_STATE_CanNotBeSelected | _forbiddenState);
            }

            return selectedItem;
        }


        UIItem* selectedItem = (UIItem*) getChildComponentToDirection(currentSelectedItem, _direction, btrue, UI_STATE_Item | UI_STATE_ListElement | _neededState, UI_STATE_CanNotBeSelected | _forbiddenState);

        if(selectedItem == currentSelectedItem)
        {
            return (UIItem*) getChildComponentToDirection(currentSelectedItem, _direction, btrue, UI_STATE_Item | _neededState, UI_STATE_CanNotBeSelected | UI_STATE_ListElement | UI_STATE_Hidden | _forbiddenState);
        }

        return selectedItem;
    }

    void UIMenuScroll::onInput(const i32 _controllerID, const f32 _axis, const StringID _input)
    {
        Vec2d direction = getInputDirection(_input);

        if(direction != Vec2d::Zero)
        {
            UIItem* selectedItem = computeNextItem(direction);

            if(selectedItem && selectedItem->GetActor()->getRef()!=m_selectedItem)
            {
                select(selectedItem);
                UI_MENUMANAGER->playSound(UIMenu_Sound_Move,UIMenu_Sound_Move_DRC,m_actor);

                i32 itemIndex = getItemIndex(selectedItem);
                if(itemIndex!=-1)
                {
                    if(f32(itemIndex)<m_firstItemCursor)
                    {
                        gotoFirstItemIndex(Max<i32>(itemIndex, 0));
                    }
                    else if(f32(itemIndex)> m_firstItemCursor + m_itemVisiblesCount - 1.0f)
                    { 
                        gotoFirstItemIndex(Min<i32>(itemIndex - m_itemVisiblesCount + 1, m_currentItems.size() - m_itemVisiblesCount));
                    }
                }
            }
        }
        else if(m_validateAllowedInput.find(_input)!=U32_INVALID)
        {
            valide(_controllerID, _input);
        }
        else if(m_backAllowedInput.find(_input)!=U32_INVALID)
        {
            back(_controllerID, _input);
        }
    }

    void UIMenuScroll::onTouchInput( const TouchDataArray & _touchArray, u32 _i )
    {
        const TouchData& data = _touchArray[_i];
        Vec2d position = data.getCurrentMainViewPos();

        Vec2d dir = m_itemOffset;
        dir.normalize();
        f32 scrollingPos = dir.dot((position - getItemAbsoluteStartPos().truncateTo2D()) / m_actor->getScale());

        UIItem* curItem = (UIItem*)getChildComponentFromPosition(position, UI_STATE_Item, UI_STATE_Hidden | UI_STATE_CanNotBeSelected);

        switch(data.state)
        {
        case TouchData::Invalid:
            {
                break;
            }
        case TouchData::Begin:
            {
                if(!m_hasTouchBegin)
                {
                    m_dragScrolling.dragReset(scrollingPos);
                    m_movingDuration = 0.0f;
                    m_isDragScrolling = (getItemIndex(curItem)!=-1);
                    //if (m_isDragScrolling)
                    //{
                    //    UI_MENUMANAGER->playSound(UIMenu_Sound_DRCMessage_Touch,UIMenu_Sound_DRCMessage_Touch_DRC,m_actor);
                    //}
                    m_hasTouchBegin = btrue;
                }
                break;
            }
        case TouchData::Moving:
            {
                if(m_hasTouchBegin)
                {
                    if(!m_isDragScrolling && (getItemIndex(curItem)!=-1))
                    {
                        m_isDragScrolling = btrue;
                        m_dragScrolling.dragReset(scrollingPos);
                        //if (m_isDragScrolling)
                        //{
                        //    UI_MENUMANAGER->playSound(UIMenu_Sound_DRCMessage_Touch,UIMenu_Sound_DRCMessage_Touch_DRC,m_actor);
                        //}
                    }

                    if(m_isDragScrolling)
                    {
                        if(m_dragScrolling.dragMove(scrollingPos))
                        {
                            select(NULL);
                        }
                        else if(m_movingDuration>getTemplate()->m_movingSelectionDelay)
                        {
                            select(curItem);
                        }
                    }
                    else if(curItem || isMenuType(MenuType_DoNotListenPad) || isMenuType(MenuType_DoNotListenNavigation))
                    {
                        select(curItem);
                    }
                }
                break;
            }
        case TouchData::End:
            {
                if(m_hasTouchBegin)
                {
                    if(!m_dragScrolling.dragEnd(scrollingPos))
                    {
                        if(curItem || isMenuType(MenuType_DoNotListenPad) || isMenuType(MenuType_DoNotListenNavigation))
                        {
                            select(curItem);
                        }

                        if(curItem)
                        {
                            valide(data.controllerId);
                        }
                    }
                    else
                    {
                        UI_MENUMANAGER->playSound(UIMenu_Sound_DRCMessage_Release,UIMenu_Sound_DRCMessage_Release_DRC,m_actor);
                    }

                    if(isMenuType(MenuType_DoNotListenPad) || isMenuType(MenuType_DoNotListenNavigation))
                    {
                        select(NULL);
                    }



                    m_hasTouchBegin = bfalse;
                }
                break;
            }
        }
    }

    UIMenuScroll_Template::~UIMenuScroll_Template()
    {    
    }

    void UIMenuScroll::onBecomeActive()
    {
        m_hasTouchBegin = bfalse;
        m_dragScrolling.dragReset(0.0f);



        Super::onBecomeActive();
    }


    void UIMenuScroll::onBecomeInactive()
    {
        Super::onBecomeInactive();
    }

    void UIMenuScroll::gotoFirstItemIndex(i32 _index, bbool _isInstant)
    {
        Vec2d dir = m_itemOffset;
        dir.normalize();
        Vec2d wantedOffset = -m_itemOffset * f32(_index);
        f32 currPos = dir.dot(m_currentOffset);
        f32 wantedPos = dir.dot(wantedOffset);
        
        if(_isInstant)
        {
            m_dragScrolling.transitionCancel();
            setCurrentOffset(wantedOffset);
        }
        else
        {
            m_dragScrolling.dragTo(currPos, wantedPos);
        }
    }

    void UIMenuScroll::gotoCenterItemIndex(i32 _index, bbool _isInstant)
    {
        i32 firstIndex = Max<i32>(0, _index - (i32(m_itemVisiblesCount * 0.5f) - 1));

        if(firstIndex + m_itemVisiblesCount > i32(m_currentItems.size()))
        {
            firstIndex = Max<i32>(0, i32(m_currentItems.size()) - m_itemVisiblesCount);
        }

        gotoFirstItemIndex(firstIndex, _isInstant);
    }

    void UIMenuScroll::setFirstItemCursor(f32 _cursor)
    {
        if(m_firstItemCursor!=_cursor)
    {
            i32 index = i32(_cursor);
            if(m_firstItemIndex!=index)
        {
                displayUiComponent(index, m_itemVisiblesCount, m_currentItems);
                displayUiComponent(index, m_itemVisiblesCount, m_currentSeparators);

            if(m_currentSeparators.size()>0)
            {
                if(UIComponent* item = m_currentSeparators[m_currentSeparators.size()-1].getUIComponent())
                {
                    item->setIsDisplay(bfalse);
                }
            }

                m_firstItemIndex = index;
            }

            m_firstItemCursor = _cursor;
        }
    }

    void UIMenuScroll::displayUiComponent(i32 _startIndex, i32 _count, ItemList& _componentsList) const
    {
        for(i32 n = 0; n<_startIndex  && n>=0 && n<i32(_componentsList.size()); ++n)
        {
            if(UIComponent* item = _componentsList[n].getUIComponent())
            {
                item->setIsDisplay(m_displayOutItems && m_areItemDisplayed);
            }
        }

        for(i32 n = _startIndex; n<_startIndex + _count && n<i32(_componentsList.size()) && n>=0; ++n)
        {
            if(UIComponent* item = _componentsList[n].getUIComponent())
            {
                item->setIsDisplay(m_areItemDisplayed);
            }
        }

        for(i32 n = _startIndex + _count; n<i32(_componentsList.size()) && n>=0; ++n)
        {
            if(UIComponent* item = _componentsList[n].getUIComponent())
            {
                item->setIsDisplay(m_displayOutItems && m_areItemDisplayed);
            }
        }
    }

    void UIMenuScroll::setCurrentOffset(const Vec2d& offset)
    {
        if(offset!=m_currentOffset)
        {
            Vec2d scale = m_actor->getScale();
            Vec2d offsetDiff = (offset - m_currentOffset) * scale;

            for(ItemList::iterator it = m_currentItems.begin(); it!=m_currentItems.end(); ++it)
            {
                if(UIComponent* item = it->getUIComponent())
                {
                    item->setAbsolutePosition(item->getAbsolutePosition() + offsetDiff);
                }
            }

            for(ItemList::iterator it = m_currentSeparators.begin(); it!=m_currentSeparators.end(); ++it)
            {
                if(UIComponent* item = it->getUIComponent())
                {
                    item->setAbsolutePosition(item->getAbsolutePosition() + offsetDiff);
                }
            }

            for(ItemList::iterator it = m_erasedItems.begin(); it!=m_erasedItems.end(); ++it)
            {
                if(UIComponent* item = it->getUIComponent())
                {
                    item->setAbsolutePosition(item->getAbsolutePosition() + offsetDiff);
                }
            }

            Vec2d dir = m_itemOffset;
            dir.normalize();
            f32 currPos = dir.dot(offset);

            setFirstItemCursor(-(currPos / m_itemOffset.norm()));

            m_currentOffset = offset;
        }
    }

    void UIMenuScroll::offsetUpdate(f32 _dt)
    {
        Vec2d scale = m_actor->getScale();
        Vec2d extendSpeed = getTemplate()->m_extendSpeed;
        Vec2d colapseSpeed = getTemplate()->m_colapseSpeed;
        Vec2d newOffset(0.0f, 0.0f);
        Vec2d diffOffset(0.0f, 0.0f);

        for(ItemList::iterator it = m_currentItems.begin(); it!=m_currentItems.end(); ++it)
        {
            diffOffset = it->computeNewOffset(newOffset, colapseSpeed, extendSpeed, _dt) - it->getOffset();
            it->setOffset(newOffset);
            if(UIComponent* item = it->getUIComponent())
            {
                item->setAbsolutePosition(item->getAbsolutePosition() + diffOffset*scale);
            }
        }

        for(ItemList::iterator it = m_currentSeparators.begin(); it!=m_currentSeparators.end(); ++it)
        {
            diffOffset = it->computeNewOffset(newOffset, colapseSpeed, extendSpeed, _dt) - it->getOffset();
            it->setOffset(newOffset);
            if(UIComponent* item = it->getUIComponent())
            {
                item->setAbsolutePosition(item->getAbsolutePosition() + diffOffset*scale);
            }
        }

        for(ItemList::iterator it = m_erasedItems.begin(); it!=m_erasedItems.end(); ++it)
        {
            diffOffset = it->computeNewOffset(newOffset, colapseSpeed, extendSpeed, _dt) - it->getOffset();
            it->setOffset(newOffset);
            if(UIComponent* item = it->getUIComponent())
            {
                item->setAbsolutePosition(item->getAbsolutePosition() + diffOffset*scale);
            }
        }
    }

    void UIMenuScroll::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        for(u32 n=0; n<m_modelItemPools.size(); ++n)
        {
            m_modelItemPools[n].update();
        }

        releaseUpdate();
        offsetUpdate(_deltaTime);

#ifdef ITF_SUPPORT_EDITOR
        
        if(isEditionMode(m_actor, bfalse))
        {
            if(m_currentItems.size()==0)
            {
                if(m_modelItemPools.size()>0 && m_modelItemPools[0].isValid() && requestItems(1, 0) == 1)
                {
                    addItem(0);
                }
            }
            else
            {
                if(UIComponent* item = getItemAt(0))
                {
                    Vec2d startPos = item->getAbsolutePosition() - m_actor->get2DPos();
                    m_itemStartPos = (startPos / m_actor->getScale()).to3d(item->getAbsoluteDepth() - m_actor->getDepth());
                }
                
                if(UIComponent* separator = getSeparatorAt(0))
                {
                    Vec2d startPos = separator->getAbsolutePosition() - m_actor->get2DPos();
                    m_separatorStartPos = (startPos/ m_actor->getScale()).to3d(separator->getAbsoluteDepth() - m_actor->getDepth());
                }
            }
        }
#endif

        Vec2d dir = m_itemOffset;
        dir.normalize();

        f32 snapDist = 0.0f;
        if(getTemplate()->m_movingSnapOnItem)
        {
            snapDist = m_itemOffset.norm();
        }
        
        f32 currPos = dir.dot(m_currentOffset);
        f32 minPos = -Max(0, i32(m_currentItems.size() - m_itemVisiblesCount)) * m_itemOffset.norm();
        f32 maxPos = 0;
        f32 newPos = m_dragScrolling.dragUpdate(currPos, minPos, maxPos, snapDist, _deltaTime);
        setCurrentOffset(dir * newPos);

        if(m_dragScrolling.transitionIsJustEnd())
        {
            UI_MENUMANAGER->playSound(UIMenu_Sound_DRCMessage_Snap,UIMenu_Sound_DRCMessage_Snap_DRC, m_actor);
        }

        m_movingDuration += _deltaTime;
    }

    u32 UIMenuScroll::requestItems(u32 _itemsCount, i32 _modelIndex)
    {
        u32 readyItemsCount = 0;

        if(m_modelItemPools[_modelIndex].isValid())
        {
            readyItemsCount = m_modelItemPools[_modelIndex].requestActor(_itemsCount);

            i32 modelSeparatorIndex = getTemplate()->m_modelSperatorIndex;
            if(modelSeparatorIndex>=0 && modelSeparatorIndex<i32(m_modelItemPools.size()))
            {
                u32 neededSeparatorsCount = 0;
                for(ITF_VECTOR<ActorPool>::iterator it= m_modelItemPools.begin(); it!=m_modelItemPools.end(); ++it)
                {
                    neededSeparatorsCount += it->getRequestedActorsCount();
                }

                u32 readySeparatorsCount = m_modelItemPools[modelSeparatorIndex].requestActor(neededSeparatorsCount);

                readyItemsCount = Min(readyItemsCount, readySeparatorsCount);
            }
        }

        return readyItemsCount;
    }

    i32 UIMenuScroll::getItemIndex(UIComponent* _item) const
    {
        if(_item != NULL)
        {
            for(i32 i =0; i<i32(m_currentItems.size()); ++i)
            {
                if(_item == m_currentItems[i].getUIComponent())
                {
                    return i;
                }
            }

            return getItemIndex(_item->getParentComponent());
        }

        return -1;
    }

    UIComponent* UIMenuScroll::setItem(i32 _index, i32 _modelIndex)
    {
        UIComponent* item = NULL;
        UIComponent* separateur = NULL;

        if(_modelIndex>=0 && _modelIndex<i32(m_modelItemPools.size()) && m_modelItemPools[_modelIndex].isValid())
        {
            if(Actor* actor = m_modelItemPools[_modelIndex].popActor(m_actor->getScene(), m_actor))
            {
                item = actor->GetComponent<UIComponent>();
            }
        }

        i32 modelSeparatorIndex = getTemplate()->m_modelSperatorIndex;
        if(modelSeparatorIndex>=0 && modelSeparatorIndex<i32(m_modelItemPools.size()))
        {
            if(Actor* actor = m_modelItemPools[modelSeparatorIndex].popActor(m_actor->getScene(), m_actor))
            {
                separateur = actor->GetComponent<UIComponent>();
            }
        }

        setUiComponent(item, _modelIndex, m_currentItems, m_erasedItems, _index, getItemAbsoluteStartPos());
        setUiComponent(separateur, modelSeparatorIndex, m_currentSeparators, m_erasedItems, _index, getSeparatorAbsoluteStartPos());

        if(_index>0 && _index+1 == i32(m_currentSeparators.size()))
        {
            if(UIComponent* item = m_currentSeparators[_index].getUIComponent())
            {
                item->setIsDisplay(bfalse);
            }
        }

        return item;
    }

    UIComponent* UIMenuScroll::addItem(i32 _modelIndex, bbool _extendUp)
    {
        return addItemBefore(NULL, _modelIndex, _extendUp);
    }

    UIComponent* UIMenuScroll::addItemBefore(UIComponent* beforeItem, i32 _modelIndex, bbool _extendUp)
    {
        UIComponent* item = NULL;
        UIComponent* separateur = NULL;

        if(_modelIndex>=0 && _modelIndex<i32(m_modelItemPools.size()) && m_modelItemPools[_modelIndex].isValid())
        {
            if(Actor* actor = m_modelItemPools[_modelIndex].popActor(m_actor->getScene(), m_actor))
            {
                item = actor->GetComponent<UIComponent>();
            }
        }

        i32 modelSeparatorIndex = getTemplate()->m_modelSperatorIndex;
        if(modelSeparatorIndex>=0 && modelSeparatorIndex<i32(m_modelItemPools.size()))
        {
            if(Actor* actor = m_modelItemPools[_modelIndex].popActor(m_actor->getScene(), m_actor))
            {
                separateur = actor->GetComponent<UIComponent>();
            }
        }

        i32 itemIndex = getItemIndex(beforeItem);

        insertUiComponent(item, _modelIndex, m_currentItems, itemIndex, getItemAbsoluteStartPos());
        insertUiComponent(separateur, modelSeparatorIndex, m_currentSeparators, itemIndex, getSeparatorAbsoluteStartPos());

        if(_extendUp)
        {
            setCurrentOffset(m_currentOffset-m_itemOffset);
        }

        if(m_currentSeparators.size()>0)
        {
            i32 previousIndex = i32(m_currentSeparators.size())-2;
            if(previousIndex>=0 && previousIndex>=m_firstItemIndex && previousIndex<m_itemVisiblesCount)
            {
                if(UIComponent* item = m_currentSeparators[previousIndex].getUIComponent())
                {
                    item->setIsDisplay(m_areItemDisplayed);
                }
            }

            if(UIComponent* item = m_currentSeparators[m_currentSeparators.size()-1].getUIComponent())
            {
                item->setIsDisplay(bfalse);
            }
        }

        return item;
    }

    bbool UIMenuScroll::setUiComponent(UIComponent* _component, i32 _modelIndex, ItemList& _componentsList, ItemList& _erasedComponentsList, i32 _componentIndex, const Vec3d& _startPos) const
    {
        if(_componentIndex>=0 && _componentIndex<i32(_componentsList.size()))
        {
            if(UIComponent* currentItem = _componentsList[_componentIndex].getUIComponent())
            {
                currentItem->setIsDisplay(bfalse);
                _erasedComponentsList.push_back(_componentsList[_componentIndex]);
                _componentsList[_componentIndex].setUIComponent(NULL, -1);
            }

            if(_component)
            {
                Vec2d scale = m_actor->getScale();
                Vec2d offset = (m_itemOffset * f32(_componentIndex) + m_currentOffset + _componentsList[_componentIndex].getOffset()) * scale;
                Vec2d pos = _startPos.truncateTo2D() + offset;

                _component->addUIState(UI_STATE_ListElement);
                _component->setAbsolutePosition(pos);
                _component->setAbsoluteDepth(i32(_startPos.z()));

                if(_componentIndex<m_firstItemIndex+m_itemVisiblesCount && _componentIndex>=m_firstItemIndex)
                {
                    _component->setIsDisplay(m_areItemDisplayed);
                }
                else
                {
                    _component->setIsDisplay(m_displayOutItems && m_areItemDisplayed);
                }

                _componentsList[_componentIndex].setUIComponent(_component, _modelIndex);
            }

            return btrue;
        }

        return bfalse;
    }

    void UIMenuScroll::insertUiComponent(UIComponent* _component, i32 _modelIndex, ItemList& _componentsList, i32 _componentIndex, const Vec3d& _startPos) const
    {
        ItemObject itemObject(_component, _modelIndex);
        if(_componentIndex!=-1)
        {
            itemObject.setOffset(_componentsList[_componentIndex].getOffset());
            _componentsList.insertAt(_componentIndex, itemObject);
        }
        else
        {
            if(_componentsList.size()>0)
            {
                itemObject.setOffset(_componentsList.back().getOffset());
            }
            _componentIndex = _componentsList.size();
            _componentsList.push_back(itemObject);
        }


        Vec2d scale = m_actor->getScale();
        Vec2d offset = (m_itemOffset * f32(_componentIndex) + m_currentOffset + itemObject.getOffset()) * scale;
        Vec2d pos = _startPos.truncateTo2D() + offset;

        if(_component)
        {
            _component->addUIState(UI_STATE_ListElement);
            _component->setAbsolutePosition(pos);
            _component->setAbsoluteDepth(i32(_startPos.z()));
            _component->GetActor()->setWorldInitialScale(_component->GetActor()->getWorldInitialScale() * scale * _component->GetActor()->currentResolutionToReferenceFactor(), bfalse);
            _component->GetActor()->setScale(_component->GetActor()->getScale() * scale * _component->GetActor()->currentResolutionToReferenceFactor());

            // State
            if(_componentIndex<m_firstItemIndex+m_itemVisiblesCount && _componentIndex>=m_firstItemIndex)
            {
                _component->setIsDisplay(m_areItemDisplayed);
            }
            else
            {
                _component->setIsDisplay(m_displayOutItems && m_areItemDisplayed);
            }
        }

        for(i32 n = _componentIndex + 1; n<i32(_componentsList.size()); ++n)
        {
            _componentsList[n].setOffset(_componentsList[n].getOffset() - m_itemOffset);
        }

        i32 lastVisibleIndex = m_firstItemIndex + m_itemVisiblesCount;
        if(lastVisibleIndex < i32(_componentsList.size()))
        {
            if(UIComponent* lastVisibleItem = _componentsList[lastVisibleIndex].getUIComponent())
            {
                lastVisibleItem->setIsDisplay(m_displayOutItems && m_areItemDisplayed);
            }
        }
    }

    void UIMenuScroll::clearItems()
    {
        resetValidateState();
        select(NULL);

        for(i32 n=m_currentItems.size()-1; n>=0; n--)
        {
            if(UIComponent* item = m_currentItems[n].getUIComponent())
            {
                releaseItem(item, m_currentItems[n].getModelIndex());
            }
        }
        m_currentItems.clear();

        for(i32 n=m_currentSeparators.size()-1; n>=0; n--)
        {
            if(UIComponent* item = m_currentSeparators[n].getUIComponent())
            {
                releaseItem(item, m_currentSeparators[n].getModelIndex());
            }
        }
        m_currentSeparators.clear();

        for(i32 n=m_erasedItems.size()-1; n>=0; n--)
        {
            if(UIComponent* item = m_erasedItems[n].getUIComponent())
            {
                releaseItem(item, m_erasedItems[n].getModelIndex());
            }
        }
        m_erasedItems.clear();

        for(u32 n=0; n<m_modelItemPools.size(); ++n)
        {
            m_modelItemPools[n].clearActors();
        }

        setFirstItemCursor(0.0f);
        setCurrentOffset(Vec2d::Zero);
        m_dragScrolling.dragReset(0.0f);
    }

    void UIMenuScroll::releaseUpdate()
    {
        for(i32 n=m_erasedItems.size()-1; n>=0; n--)
        {
            if(UIComponent* item = m_erasedItems[n].getUIComponent())
            {
                if(!item->GetActor()->isActive())
                {
                    releaseItem(item, m_erasedItems[n].getModelIndex());
                    m_erasedItems.removeAtUnordered(n);
                }
            }
        }
    }

    UIComponent* UIMenuScroll::removeItem(i32 _index, bbool _colapseDown)
    {
        if(_index>=0 && _index<i32(m_currentItems.size()))
        {
            if( UIComponent* item = m_currentItems[_index].getUIComponent())
            {
                 UIItem* selectedItem = getSelectedItem();

                 if(item == selectedItem)
                 {
                     select((UIItem*) getItemAt(_index-1)); //!\\ TODO need dynamic cast
                 }
            }

            eraseUiComponent(_index, m_currentItems, m_erasedItems);
            eraseUiComponent(_index, m_currentSeparators, m_erasedItems);

            // Collapse down
            if(_colapseDown)
            {
                setCurrentOffset(m_currentOffset + m_itemOffset);
            }

            if(m_currentSeparators.size()>0)
            {
                if(UIComponent* item = m_currentSeparators[m_currentSeparators.size()-1].getUIComponent())
                {
                    item->setIsDisplay(bfalse);
                }
            }
        }

        return NULL;
    }

    void UIMenuScroll::eraseUiComponent(i32 _index, ItemList& _componentsList, ItemList& _erasedComponentsList) const
    {
        if(_index>=0 && _index<i32(_componentsList.size()))
        {
            if( UIComponent* item = _componentsList[_index].getUIComponent())
            {
                item->setIsDisplay(bfalse);
                _erasedComponentsList.push_back(_componentsList[_index]);
            }

            _componentsList.removeAt(_index);


            for(i32 n = _index; n<i32(_componentsList.size()); ++n)
            {
                _componentsList[n].setOffset(_componentsList[n].getOffset() + m_itemOffset);
            }

            i32 lastVisibleIndex = m_firstItemIndex + m_itemVisiblesCount - 1;
            if(lastVisibleIndex < i32(_componentsList.size()))
            {
                if(UIComponent* lastVisibleItem = _componentsList[lastVisibleIndex].getUIComponent())
                {
                    lastVisibleItem->setIsDisplay(m_areItemDisplayed);
                }
            }
        }
    }

    void UIMenuScroll::setAreItemsDisplayed( bbool _val )
    {
        m_areItemDisplayed = _val;

        displayUiComponent(m_firstItemIndex, m_itemVisiblesCount, m_currentItems);
        displayUiComponent(m_firstItemIndex, m_itemVisiblesCount, m_currentSeparators);

        if(m_currentSeparators.size()>0)
        {
            if(UIComponent* item = m_currentSeparators[m_currentSeparators.size()-1].getUIComponent())
            {
                item->setIsDisplay(bfalse);
            }
        }
    }

    UIComponent* UIMenuScroll::getItemAt(i32 _index) 
    {
        if(_index>=0 && _index<i32(m_currentItems.size())) 
            return m_currentItems[_index].getUIComponent(); 
        
        return NULL;
    }

    UIComponent* UIMenuScroll::getSeparatorAt(i32 _index) 
    {
        if(_index>=0 && _index<i32(m_currentSeparators.size())) 
            return m_currentSeparators[_index].getUIComponent();

        return NULL;
    }

    void UIMenuScroll::releaseItem(UIComponent* _component, i32 _modelIndex)
    {
        if(_component && _modelIndex>=0 && _modelIndex<i32(m_modelItemPools.size()))
        {
            m_modelItemPools[_modelIndex].releaseActor(_component->GetActor());
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////


    IMPLEMENT_OBJECT_RTTI(UIMenuScroll_Template)
    BEGIN_SERIALIZATION_CHILD(UIMenuScroll_Template)
    SERIALIZE_CONTAINER("modelActorPaths", m_modelItemPaths);
    SERIALIZE_MEMBER("modelSperatorIndex", m_modelSperatorIndex);
    SERIALIZE_MEMBER("movingSelectionDelay", m_movingSelectionDelay);
    SERIALIZE_MEMBER("movingSelectionNormMax", m_movingSelectionNormMax);
    SERIALIZE_MEMBER("movingBounceTime", m_movingBounceTime);
    SERIALIZE_MEMBER("movingBounceNorm", m_movingBounceNorm);
    SERIALIZE_MEMBER("movingSnapOnItem", m_movingSnapOnItem);
    SERIALIZE_MEMBER("movingMomentumFriction", m_movingMomentumFriction);
    SERIALIZE_MEMBER("movingMomentumDeceleration", m_movingMomentumDeceleration);
    SERIALIZE_MEMBER("extendSpeed", m_extendSpeed);
    SERIALIZE_MEMBER("colapseSpeed", m_colapseSpeed);
    END_SERIALIZATION()

    UIMenuScroll_Template::UIMenuScroll_Template()
    : m_movingSelectionDelay(0.2f)
    , m_movingSelectionNormMax(10.0f)
    , m_movingBounceTime(0.5f)
    , m_movingBounceNorm(80.0f)
    , m_movingSnapOnItem(bfalse)
    , m_movingMomentumFriction(2.5f)
    , m_movingMomentumDeceleration(1.2f)
    , m_modelSperatorIndex(-1)
    , m_extendSpeed(Vec2d(F32_INFINITY, F32_INFINITY))
    , m_colapseSpeed(Vec2d(F32_INFINITY, F32_INFINITY))
    {  

    }



}
