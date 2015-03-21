#include "precompiled_gameplay.h"

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Tweening/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_TEXTANIMSCALECOMPONENT_H_
#include "gameplay/components/UI/TextAnimScaleComponent.h"
#endif //_ITF_TEXTANIMSCALECOMPONENT_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif//_ITF_UIMENUMANAGER_H_

#ifndef _ITF_SETCHILDREN2DNOSCREENRATIOCOMPONENT_H_
#include "engine/actors/components/ScreenRatio/SetChildren2DNoScreenRatioComponent.h"
#endif//_ITF_SETCHILDREN2DNOSCREENRATIOCOMPONENT_H_

#if defined(ITF_IOS) || defined(ITF_ANDROID) || defined(ITF_SUPPORT_EDITOR)
	#define _RATIO_ENABLED_
#endif

#if defined(ITF_SUPPORT_EDITOR)
    #ifndef _ITF_CONFIG_H_
        #include "core/Config.h"
    #endif //_ITF_CONFIG_H_
#endif //ITF_SUPPORT_EDITOR

namespace ITF
{
    const f32 UIComponent_Template::g_transitionTimeBase = 0.81f;
    static const f32 c_deadZoneValue = 0.5f;

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIComponent)
    BEGIN_SERIALIZATION_CHILD(UIComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("transition", m_useTransition);
            SERIALIZE_MEMBER("display", m_initDisplay);
            SERIALIZE_MEMBER("leftComponentID", m_leftComponentID);
            SERIALIZE_MEMBER("rightComponentID", m_rightComponentID);
            SERIALIZE_MEMBER("upComponentID", m_upComponentID);
            SERIALIZE_MEMBER("downComponentID", m_downComponentID);
            SERIALIZE_MEMBER("cursorOffset", m_cursorOffset);
            SERIALIZE_MEMBER("cursorOffset_4_3", m_cursorOffset_4_3);
            SERIALIZE_MEMBER("cursorAngle", m_cursorAngle);

            SERIALIZE_ENUM_EDITABLE_VIEW_IDS("displayMask", m_displayMask);
        END_CONDITION_BLOCK()
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
            BEGIN_CONDITION_BLOCK_NOT(ESerialize_Instance)     
                SERIALIZE_MEMBER("screenSpace", m_screenSpace);
            END_CONDITION_BLOCK_NOT()
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerialize_DataRaw)
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

        ///////////////////////////////////////////////////////////////////////////////////////////
        UIComponent::UIComponent()
        : Super()
        , m_isMasterComponent(btrue)
        , m_wantDisplay(btrue)
        , m_initDisplay(btrue)
        , m_UIState(UI_STATE_Hidden)
        , m_screenSpace(Pickable::s_screenSpaceReference)
        , m_currentScreenSpace(Pickable::s_screenSpaceReference)
        , m_baseScale(Vec2d::One)
        , m_transitionMode(UI_TRANSITION_FarToClose)
        , m_useTransition(bfalse)
        , m_needReactive(bfalse)
        , m_SpringVelocity(Vec2d::Zero)
        , m_SpringLength(Vec2d::One)
        , m_displayMask( View::MainAndRemote )
        , m_isTransitionReady(bfalse)
        , m_fadeCursor(1.0f)
        , m_lastViewportRatio(1.0f)
        , m_cursorOffset(Vec2d::Zero)
        , m_cursorOffset_4_3(Vec2d::Zero)
        , m_cursorAngle(0.f)
   {   
    }

#ifdef ITF_SUPPORT_EDITOR
    ///////////////////////////////////////////////////////////////////////////////////////////
    // update the display mask of binded actor 
    void UIComponent::onPostPropertyChange()
    {
        if(m_isMasterComponent)
        {
            setUIDisplayViewId( m_displayMask );
        }
    }
#endif //ITF_SUPPORT_EDITOR

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIComponent::~UIComponent()
    {

    }   

    UIComponent* UIComponent::getUIComponent (ObjectRef _ref)
    {
        const Actor* actor = (const Actor*)_ref.getObject();

        if (actor)
        {
            return actor->GetComponent<UIComponent>();
        }

        return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////


    Actor* UIComponent::getChildActor( const StringID _friendly, bbool _rec )
    {
        const ObjectRefList& componentObjectList = getChildObjectsList();

        for (ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
        {
            Actor* actor = (Actor*) iter->getObject();
            UIComponent *component = actor->GetComponent<UIComponent>();

            if ( actor->getUserFriendlyID() == _friendly)
                return actor;

            if(_rec)
            {
                Actor *child = component->getChildActor(_friendly, _rec);
                if(child)
                    return child;
            }
        }

        return NULL;
    }

	UIComponent* UIComponent::getChildComponent( const StringID _friendly, bbool _rec ) const
	{
		const ObjectRefList& componentObjectList = getChildObjectsList();

		for (ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
		{
			const Actor* actor = (const Actor*) iter->getObject();
			UIComponent *component = actor->GetComponent<UIComponent>();

			if ( actor->getUserFriendlyID() == _friendly)
				return component;

			if(_rec)
			{
				UIComponent *childComponent = component->getChildComponent(_friendly, _rec);
				if(childComponent)
					return childComponent;
			}
		}

		return NULL;
	}

#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
	UIComponent* UIComponent::getStackedChildComponent( const StringID _friendly, bbool _rec ) const
	{
		const ObjectRefList& componentObjectList =  UI_MENUMANAGER->getStackedChildObjectsList();

		for (ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
		{
			const Actor* actor = (const Actor*) iter->getObject();
			UIComponent *component = actor->GetComponent<UIComponent>();

			if ( actor->getUserFriendlyID() == _friendly)
				return component;

			if(_rec)
			{
				UIComponent *childComponent = component->getChildComponent(_friendly, _rec);
				if(childComponent)
					return childComponent;
			}
		}

		return NULL;
	}
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD

    ObjectRefList& UIComponent::getChildObjectsList() const
    {
        return m_actor->getChildrenBindHandler().getChildren();
    }

    ObjectRef UIComponent::getChildObject(const StringID _friendly) const
    {
        const ObjectRefList& componentObjectList = getChildObjectsList();

        for(ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
        {
            const Actor* actor = (const Actor*) iter->getObject();

            if(actor->getUserFriendlyID() == _friendly)
            {
                return *iter;
            }
        }

        return ObjectRef::InvalidRef;
    }

    ObjectRef UIComponent::getParentObject() const
    {
        if(m_actor)
        {
            return m_actor->getParent();
        }

        return ObjectRef::InvalidRef;
    }

    UIComponent* UIComponent::getParentComponent() const
    {
        return getUIComponent(getParentObject());
    }

    bbool UIComponent::contains(const Vec2d& _position) const
    {
        if (m_actor->getIs2D())
            return m_actor->getAABB().contains(_position);

        bbool result = bfalse;
        const ITF_VECTOR<View*> &views = m_actor->getWorldUpdateElement()->getViews();
        for (ITF_VECTOR<View*>::const_iterator it = views.begin(); it != views.end(); ++it)
        {
            const View *view = *it;
            Vec3d position3d;
            view->compute2DTo3D(_position, m_actor->getDepth(), position3d);
            result |= m_actor->getAABB3d().contains(position3d);
        }
        return result;
    }

    UIComponent* UIComponent::getChildComponentFromPosition( const Vec2d& _position, i32 _neededState /*= UI_STATE_None*/, i32 _forbiddenState /*= UI_STATE_None*/ ) const
    {
        const ObjectRefList& componentObjectList = getChildObjectsList();
        for(ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
        {
            const Actor* actor = (const Actor*) iter->getObject();
            UIComponent* component = actor->GetComponent<UIComponent>();

            if(component && component->matchUIState(_neededState, _forbiddenState))
            {
                if(UIComponent* childComponent = component->getChildComponentFromPosition(_position, _neededState, _forbiddenState))
                {
                    return childComponent;
                }

                if(component->contains(_position))
                {
                    return component;
                }
            }
        }

        return NULL;
    }

    UIComponent* UIComponent::getChildComponentByDirection(UIComponent* _currentComponent, const Vec2d& _direction) const
    {
        UIComponent* nextComponent = NULL;
        StringID nextComponentID;
        if (_direction.x() >= c_deadZoneValue)
        {
            nextComponentID = _currentComponent->getRightComponentID();
        }
        else if (_direction.x() <= -c_deadZoneValue)
        {
            nextComponentID = _currentComponent->getLeftComponentID();
        }
        else if (_direction.y() >= c_deadZoneValue)
        {
            nextComponentID = _currentComponent->getDownComponentID();
        }
        else if (_direction.y() <= -c_deadZoneValue)
        {
            nextComponentID = _currentComponent->getUpComponentID();
        }
        if (nextComponentID.isValid())
        {
#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
            nextComponent = getStackedChildComponent(nextComponentID);
#else
            nextComponent = getChildComponent(nextComponentID);
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD
            if (nextComponent && !nextComponent->getIsDisplay())
                return getChildComponentByDirection(nextComponent, _direction);
        }
        return nextComponent;
    }

    UIComponent* UIComponent::getChildComponentToDirection(UIComponent* _currentComponent, Vec2d _direction, bbool _closest, i32 _neededState, i32 _forbiddenState, f32 _maxDot, f32* _bestDistance) const
    {
        UIComponent* currentComponent = _currentComponent;
        if(currentComponent && !currentComponent->matchUIState(_neededState, _forbiddenState))
        {
            currentComponent = NULL;
        }

        f32 bestDistance = _closest ? F32_INFINITY : 0.0f;
        if(_bestDistance)
        {
            bestDistance = *_bestDistance;
        }

        Vec2d currentPosition = getAbsolutePosition();
        if(currentComponent != NULL)
        {
            currentPosition = currentComponent->getAbsolutePosition();

            UIComponent* const tempComponent = getChildComponentByDirection(currentComponent, _direction);
            if (tempComponent)
            {
                if(tempComponent->matchUIState(_neededState, _forbiddenState))
                    return tempComponent;
                else
                {
                    // This item is not valid but we will keep in looking starting from this one
                    Vec2d newPosition = tempComponent->getAbsolutePosition();
                    // Change direction ?
                    Vec2d direction = newPosition - currentPosition;
                    if(direction.dot(_direction) < 0)
                        _direction *= -1.0f;
                    currentPosition = newPosition;
                }
            }
        }

#ifdef ITF_W1W_MOBILE_MENU_USEGAMEPAD
        const ObjectRefList& componentObjectList =  UI_MENUMANAGER->getStackedChildObjectsList();
#else
        const ObjectRefList& componentObjectList = getChildObjectsList();
#endif //ITF_W1W_MOBILE_MENU_USEGAMEPAD

        for(ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
        {
            const Actor* actor = (const Actor*) iter->getObject();
            if(UIComponent* component = actor->GetComponent<UIComponent>())
            {
                if(component->matchUIState(_neededState, _forbiddenState))
                {
                    if(component!=_currentComponent)
                    {
                        Vec2d position = component->getAbsolutePosition();
                        Vec2d direction = position - currentPosition;
                        f32 distance = direction.norm();

                        if ( distance > MTH_EPSILON ) direction/=distance;

                        f32 dot = direction.dot(_direction);

                        if(dot > _maxDot && ((_closest && distance <= bestDistance) || (!_closest && distance >= bestDistance)) )
                        {
                            bestDistance = distance;
                            currentComponent = component;
                        }
                    }
                }
            }
        }

        if(_bestDistance)
        {
            *_bestDistance = bestDistance;
        }

        return currentComponent;
    }

    u32 UIComponent::getChildComponentCount(i32 _neededState, i32 _forbiddenState) const
    {
        u32 matchCount = 0;
        const ObjectRefList& componentObjectList = getChildObjectsList();
        for(ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
        {
            const Actor* actor = (const Actor*) iter->getObject();
            if(UIComponent* component = actor->GetComponent<UIComponent>())
            {
                if(component->matchUIState(_neededState, _forbiddenState))
                {
                    matchCount++;
                }
            }
        }

        return matchCount;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////

    StringID UIComponent::getFriendlyID() const
    {
        return m_actor->getUserFriendlyID();
    }

    bbool UIComponent::is2D() const
    {
        return getTemplate()->getIs2D();
    }

    bbool UIComponent::needsDraw2DNoScreenRatio() const
    {
        return getTemplate()->getIs2DNoScreenRatio();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_isMasterComponent = (m_actor->GetComponent<UIComponent>()==this);

        if(!getParentComponent())
        {
            m_wantDisplay = m_actor->isEnabled();
        }
        else
        {
            m_wantDisplay = m_initDisplay;
        }

        checkComponentState(btrue);
        // Fix UI
        checkDisplayState();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventQueryBackgroundBoxResizeAllowed_CRC,this);
    }

	void UIComponent::updateUIPositionForDeviceAspectRatio()
	{
		// Fix Aspect Ratio
		bool moveit=true;

		// Do not reposition UIMenu, as the whole menus would move
		if( IsClassCRC(UIMenu_CRC) )
			moveit=false;

		// Reposition the UI, based on the current device
		if(  m_actor->getIs2D() && moveit && !m_actor->getIs2DNoScreenRatio() )
		{
			if( getIsMasterComponent() )
			{
				Vec2d absPos = getAbsolutePosition();
				Vec2d bound2dPos = fixUIPositionForAspectRatio(absPos);
				setAbsolutePosition(bound2dPos, true);
			}
		}
	}

    void UIComponent::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

		// fix Aspect Ratio 
		updateUIPositionForDeviceAspectRatio();

        if(!getParentComponent())
        {
            m_wantDisplay = m_actor->isEnabled();
        }
        else
        {
            m_wantDisplay = m_initDisplay;
        }

        checkDisplayState();

        m_baseScale = m_actor->getLocalInitialScale();

        if(UIComponent* parent = getParentComponent())
            m_useTransition |= parent->getUseTransition();

        if(m_isMasterComponent)
        {
            if(UI_MENUMANAGER->useRemoteUI())
            {
                m_actor->getWorldUpdateElement()->setViewMask( View::getMaskIdFromEditableViewId( m_displayMask ) );
            }
            else
            {
                m_actor->getWorldUpdateElement()->setViewMask( View::MASKID_MAIN );
            }
        }

#ifdef ITF_ANDROID
		if(m_isMasterComponent)
        {
			const f32 ratio_16_9 = 1.775f; //16.0f / 9.0f; 
			const f32 screenRatio = (f32) GFX_ADAPTER->getScreenWidth() / (f32) GFX_ADAPTER->getScreenHeight();
			f32 ds = 1.0f - (screenRatio / ratio_16_9);
			ds = std::max(ds, 0.0f);
			
			m_actor->setScale(m_actor->getScale() + Vec2d(0.0f, m_actor->getScale().y() * ds));
		}
#endif
    }

    void UIComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);

        if ( EventQueryBackgroundBoxResizeAllowed* eventQuery = DYNAMIC_CAST(_event,EventQueryBackgroundBoxResizeAllowed) )
        {   
            if(hasUIState(UI_STATE_Hiding))
            {
                eventQuery->setIsResizeAllowed(bfalse);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::Update ( f32 _deltaTime )
    {
        Super::Update(_deltaTime);
        
        checkComponentState();
        checkDisplayState(_deltaTime);

#ifdef ITF_SUPPORT_EDITOR
        if(m_isMasterComponent && isEditionMode(m_actor, bfalse))
        {
            m_actor->getWorldUpdateElement()->setViewMask( View::getMaskIdFromEditableViewId( m_displayMask ) | View::MASKID_MAIN );
        }
#endif
    }

    void UIComponent::resetInitState(bbool _recursive)
    {
        if(_recursive)
        {
            const ObjectRefList& componentObjectList = getChildObjectsList();
            for(ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
            {
                Actor* actor = (Actor*) iter->getObject();
                if(UIComponent* component = actor->GetComponent<UIComponent>())
                {
                    component->resetInitState(btrue);
                }
            }
        }

        m_actor->setPos(m_actor->getWorldInitialPos());
        m_actor->setScale(m_actor->getWorldInitialScale());
        EventShow show(1.0f, 0.0f);
        m_actor->onEvent(&show);
    }

    void UIComponent::setUseTransition(bbool _inTransition, bbool _recursive) 
    { 
        if(_recursive)
        {
            const ObjectRefList& componentObjectList = getChildObjectsList();
            for(ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
            {
                Actor* actor = (Actor*) iter->getObject();
                if(UIComponent* component = actor->GetComponent<UIComponent>())
                {
                    component->setUseTransition(_inTransition, btrue);
                }
            }
        }

        m_useTransition = _inTransition; 
    }

    void UIComponent::checkDisplayState(f32 _deltaTime)
    {
        if(getIsMasterComponent())
        {
            bbool mustDisplay = m_wantDisplay && !hasUIState(UI_STATE_IsMasked);
            UIComponent* parent = getParentComponent();
            if(parent && parent->GetActor())
            {
                mustDisplay &= parent->getIsDisplay() || (parent->GetActor()->isEnabled() && !parent->hasUIState(UI_STATE_Hiding));
            }

            updateState(mustDisplay, _deltaTime);

            // Init or Enable or Disable children
            if(mustDisplay!=getIsDisplay() || _deltaTime == 0.0f)
            {
                const ObjectRefList& componentObjectList = getChildObjectsList();
                for(ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
                {
                    Actor* actor = (Actor*) iter->getObject();
                    if(UIComponent* component = actor->GetComponent<UIComponent>())
                    {
                        component->checkDisplayState(_deltaTime);
                    }
                }
            }
        }
    }

    void UIComponent::onBecomeActive()
    {
        if(m_isMasterComponent)
        {
            removeUIState(UI_STATE_DisplayMask);
            addUIState(UI_STATE_Showing);

            if(m_useTransition)
            {
                setIsInTransition( bfalse );
            }
            

            if(!getParentComponent())
            {
                m_wantDisplay = btrue;
            }
        }
    }

    bbool UIComponent::onIsShowing(f32 _dt)
    {
        if(m_useTransition)
            return updateTransition(_dt);
        else
            return bfalse;
    }

    void UIComponent::onShowingEnd()
    {
        removeUIState(UI_STATE_DisplayMask);
        addUIState(UI_STATE_Showed);
    }

    void UIComponent::onHiddingBegin()
    {
        removeUIState(UI_STATE_DisplayMask);
        addUIState(UI_STATE_Hiding);

        if(m_useTransition)
        {
            setIsInTransition( btrue );
        }
    }

    bbool UIComponent::onIsHidding(f32 _dt)
    {
        if(m_useTransition)
            return updateTransition(_dt);
        else
            return bfalse;
    }

    void UIComponent::onBecomeInactive()
    {
        if(m_isMasterComponent)
        {
            bbool wasHidding = hasUIState(UI_STATE_Hiding);

            // Keep standard state cycle
            if(hasUIState(UI_STATE_Showing))
            {
                onShowingEnd();
            }

            if(hasUIState(UI_STATE_Showed))
            {
                onHiddingBegin();
            }


            removeUIState(UI_STATE_DisplayMask);
            addUIState(UI_STATE_Hidden);

            
            if(!getParentComponent() && !hasUIState(UI_STATE_IsMasked)) // todo clean up
            {
                if(wasHidding)
                {
                    m_actor->disable();
                }
                else
                {
                    m_wantDisplay = m_actor->isEnabled();
                }
            }

            // Check if wantDisplay
            setIsDisplay(m_needReactive || m_wantDisplay);
            m_needReactive = bfalse;
        }
    }

    void UIComponent::updateState (bbool _mustDisplay, f32 _deltaTime)
    {
        if(hasUIState(UI_STATE_Hidden)) 
        { 
            if(_mustDisplay) 
            { 
                m_actor->enable(); 
            } 
            else 
            { 
                m_actor->disable(); 
            } 
        } 
        else if(hasUIState(UI_STATE_Showing)) 
        { 
            if(!_mustDisplay || !onIsShowing(_deltaTime)) 
            { 
                onShowingEnd(); 
            } 
        } 

        if(hasUIState(UI_STATE_Showed)) 
        { 
            onUpdate(_deltaTime);   

            if(!_mustDisplay) 
            { 
                onHiddingBegin(); 
            } 
        } 
        
        // JR : if there is no transition we mustn't wait one frame to disable the actor
        // so don't put "else if" here
        if(hasUIState(UI_STATE_Hiding)) 
        { 
            if(_mustDisplay || !onIsHidding(_deltaTime)) 
            { 
                m_actor->disable(); 
                m_needReactive = _mustDisplay;
            } 
        } 
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void UIComponent::setUIDisplayViewId( View::EditableViewIds _editableViewId, bbool _recursive) 
    { 
        if(_recursive)
        {
            AIUtils::ActorBindIterator bindIt( m_actor );
            while( Actor * child = bindIt.getNextChild() )
            {
                if ( UIComponent * uiComp = child->GetComponent<UIComponent>() )
                {
                    uiComp->setUIDisplayViewId( _editableViewId );
                }
            }
        }

        m_actor->getWorldUpdateElement()->setViewMask( View::getMaskIdFromEditableViewId( _editableViewId ) );

        m_displayMask = _editableViewId; 
    }

    void UIComponent::setUIDisplayMask( View::MaskIds _mask, bbool _recursive)
    {
        if(_recursive)
        {
            AIUtils::ActorBindIterator bindIt( m_actor );
            while( Actor * child = bindIt.getNextChild() )
            {
                if ( UIComponent * uiComp = child->GetComponent<UIComponent>() )
                {
                    uiComp->setUIDisplayMask( _mask );
                }
            }
        }

        m_actor->getWorldUpdateElement()->setViewMask( _mask );
    }

    View::MaskIds UIComponent::getUIDisplayMask()  const 
    { 
        return (View::MaskIds)(m_actor->getWorldUpdateElement()->getViewMask()); 
    }

    void UIComponent::setIsDisplay(const bbool _isDisplay) 
    {
        if(m_wantDisplay!=_isDisplay || getIsDisplay() !=_isDisplay)
        {
            m_wantDisplay = _isDisplay;

            if(!m_wantDisplay)
                m_needReactive = bfalse;

            checkComponentState();
            checkDisplayState();
        }
    }

    void UIComponent::setIsMasked(const bbool _isMasked) 
    {
        if(hasUIState(UI_STATE_IsMasked)!=_isMasked)
        {
            setUIState(UI_STATE_IsMasked, _isMasked);
            checkDisplayState();
        }
    }

	Vec2d UIComponent::fixUIPositionForAspectRatio( Vec2d& _inPos )
	{
		Vec2d updatedPos;

		updatedPos.x() = _inPos.x();	// Ne change pas en x
		updatedPos.y() = _inPos.y();
        
#ifdef _RATIO_ENABLED_
		#if defined(ITF_SUPPORT_EDITOR)
			// Move the UI only if the UITestRatio flag is set in the config. 
			if( !CONFIG->m_editorOptions.m_UITestRatio )
				return updatedPos;
		#endif


		// Aspect Ratio repositioning

		// go back to original pos
		_inPos.y() = _inPos.y() / m_lastViewportRatio; // default is 1.0f


        m_lastViewportRatio = GFX_ADAPTER->getScreenWidth() * 9.0f / 16.0f;
        m_lastViewportRatio = GFX_ADAPTER->getScreenHeight() / m_lastViewportRatio;
        
        updatedPos.y() = _inPos.y() * m_lastViewportRatio; // we should take in account the size ?

        #if defined(ITF_SUPPORT_EDITOR)
            UI_MENUMANAGER->setAspectRatioChanged(btrue);
        #endif
#endif
        
        // TO DO !!!
		// Version evoluée :
		// bouger uniquement une partie des éléments ?

		return updatedPos;
	}

    void UIComponent::checkComponentState(bbool _forceSetPos)
    {
        if(!getIsMasterComponent() || !m_actor->getIs2D())
            return;

		if (m_screenSpace != Pickable::s_screenSpaceReference)
		{
			// 2d actor has bad initial pos reference !!
			{
				if(Bind* parentBind = m_actor->getParentBind())
				{
					m_actor->updateWorldCoordinatesFromBoundParent();
					Vec3d boundInitialPos = m_actor->getBoundWorldInitialPos(btrue);
					if(boundInitialPos != m_actor->getPos()) // Is different only if the resolution change in runtime
					{

						Vec3d initPos((boundInitialPos.truncateTo2D() * m_actor->referenceToCurrentResolutionFactor()).to3d(boundInitialPos.z()));
						m_actor->setBoundWorldInitialPos(initPos, btrue);
						m_actor->setWorldInitialPos(initPos, btrue);
						// Use new initial pos offset as current pos offset
						parentBind->setPosOffset(parentBind->getInitialPosOffset());

						if(parentBind->m_useParentScale)
						{
							m_actor->setWorldInitialScale(m_actor->getWorldInitialScale() * m_actor->referenceToCurrentResolutionFactor(), btrue);
						}
					}

					if(!parentBind->m_useParentScale)
					{
						m_actor->setWorldInitialScale(m_actor->getWorldInitialScale() * m_actor->referenceToCurrentResolutionFactor(), btrue);
						m_actor->setScale(m_actor->getScale() * m_actor->referenceToCurrentResolutionFactor());
					}
				}
				else
				{
					Vec2d actorPos = m_actor->referencePos2dToCurrentResolution(m_actor->getWorldInitialPos().truncateTo2D());
					m_actor->setWorldInitialPos((actorPos).to3d(m_actor->getWorldInitialZ()), btrue);
					m_actor->setWorldInitialScale(m_actor->getWorldInitialScale() * m_actor->referenceToCurrentResolutionFactor(), btrue);
				}
				m_screenSpace = Pickable::s_screenSpaceReference;
#ifdef ITF_SUPPORT_EDITOR
				m_actor->setDirty(btrue);
#endif // ITF_SUPPORT_EDITOR
			}
			_forceSetPos = btrue;
		}

		if (_forceSetPos/* || m_currentScreenSpace != Pickable::getScreenSize()*/)
		{
			m_actor->setPos(m_actor->getWorldInitialPos());
			m_actor->setScale(m_actor->getWorldInitialScale());

			m_currentScreenSpace = Pickable::getScreenSize();
		}

		m_baseScale = m_actor->getLocalInitialScale();
    }

    void UIComponent::setAbsolutePosition(const Vec2d& _position, bbool _setInit) 
    {
        Bind* bind = m_actor->getParentBind();
        if(bind)
        {
            if(Actor* parent = (Actor*) bind->m_runtimeParent.getObject())
            {
                Vec3d localPos;
                f32 localAngle;
                if (parent->getChildrenBindHandler().computeLocalCoordinates(bind, _position.to3d(m_actor->getDepth()), m_actor->getAngle(), localPos, localAngle))
                {
                    // z is not concerned !!
                    localPos.z() = bind->getPosOffset().z();

                    bind->setPosOffset(localPos);

                    if(_setInit)
                        bind->setInitialPositionOffset(localPos);
                }
            }
        }

        m_actor->set2DPos(_position);

        if(_setInit)
        {
            Vec3d pos3d = _position.to3d(m_actor->getBoundWorldInitialPos(btrue).z());
            m_actor->setWorldInitialPos(pos3d, btrue);
        }
    }

    void UIComponent::setRelativePosition(const Vec2d& _position, bbool _setInit) 
    {
        setAbsolutePosition(_position * Pickable::getScreenSize(), _setInit);
    }

    void UIComponent::setAbsoluteDepth(const i32 _depth) 
    {
        Bind* bind = m_actor->getParentBind();
        if(bind)
        {
            if(Actor* parent = (Actor*) bind->m_runtimeParent.getObject())
            {
                Vec3d offset = bind->getPosOffset();
                offset.z() = f32(_depth) - parent->getDepth();

                bind->setPosOffset(offset);
                bind->setInitialPositionOffset(offset);
            }
        }
        m_actor->setDepth((f32) _depth);
        m_actor->setWorldInitialZ((f32) _depth, btrue);
    }

    void UIComponent::setAbsoluteScale(const Vec2d& _scale) 
    {
        m_actor->setScale(_scale);
    }

    i32 UIComponent::getAbsoluteDepthMax()  const
    {  
        i32 depthMax = getAbsoluteDepth();

        const ObjectRefList& componentObjectList = getChildObjectsList();

        for(ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
        {
            const Actor* actor = (const Actor*) iter->getObject();
            UIComponent* component = actor->GetComponent<UIComponent>();

            if(component)
            {
                i32 depth = component->getAbsoluteDepthMax();

                if(depth>depthMax)
                {
                    depthMax = depth;
                }
            }
        }

        return depthMax;
    }

    i32 UIComponent::getAbsoluteDepth() const 
    {
        return i32(m_actor->getDepth());
    }

    Vec2d UIComponent::getAbsolutePosition() const 
    {
        return m_actor->get2DPos();
    }

    Vec2d UIComponent::getAbsoluteScale() const 
    {
        return m_actor->getScale();
    }

    Vec2d UIComponent::getRelativePosition() const 
    {
        if (m_actor->getIs2D())
            return m_actor->get2DPos() / m_screenSpace;
        else
            return m_actor->get2DPos();
    }

    bbool UIComponent::getIsDisplay() const 
    {
        return m_actor->isActive() && !hasUIState(UI_STATE_Hiding);
    }

    const String8& UIComponent::getText() const
    {
        UITextBox* textBox = m_actor->GetComponent<UITextBox>();

        if(textBox)
        {
            return textBox->getText();
        }

        return String8::emptyString;
    }

    void UIComponent::setText(const String8 & _text)
    {
        UITextBox* textBox = m_actor->GetComponent<UITextBox>();

        if(textBox)
        {
            textBox->setText(_text);
        }
	}

	void UIComponent::setLoc(const LocalisationId& _locId)
	{
		UITextBox* textBox = m_actor->GetComponent<UITextBox>();

		if(textBox)
		{
			textBox->setLoc(_locId);
		}
	}


    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool UIComponent::isTransitionReady() const
    {
        if(!m_isTransitionReady)
        {
            if(TextAnimScaleComponent* textAnimScaleComponent = m_actor->GetComponent<TextAnimScaleComponent>())
            {
                if(!textAnimScaleComponent->getIsReady())
                {
                    return bfalse;
                }
            }

            const ObjectRefList& componentObjectList = getChildObjectsList();
            for(ObjectRefList::const_iterator iter=componentObjectList.begin(); iter!=componentObjectList.end(); iter++)
            {
                const Actor* actor = (const Actor*) iter->getObject();

                if(UIComponent* component = actor->GetComponent<UIComponent>())
                {
                    if(component->m_useTransition && component->m_wantDisplay && !component->m_isTransitionReady)
                    {
                        return bfalse;
                    }
                }
            }
        }

        return btrue;
    }

    ITF::bbool UIComponent::getIsInTransition() const
    {
        return (hasUIState(UI_STATE_Showing) || hasUIState(UI_STATE_Hiding));
    }

    void UIComponent::setIsInTransition( bbool _hide)
    {
        if(_hide)
        {
            m_transitionTime = getTemplate()->g_transitionTimeBase;
        }
        else
        {
            m_transitionTime = f32_Clamp(m_transitionTime, 0.f, getTemplate()->g_transitionTimeBase);
        }
        
        m_isTransitionReady = bfalse;
        m_SpringLength = Vec2d(0.2f, 0.4f);
        m_SpringVelocity = Vec2d(0.0f, 0.0f);
        updateTransition(0.0f);
    }

    bbool UIComponent::updateTransition( const f32 _dt )
    {
        // This component does not behave with a poor dt, 20 FPS is the lowest
        f32 dt = _dt;
        if( dt > 1.0f/20.0f )
            dt = 1.0f/20.0f;
        
        bbool isInTransition = bfalse;
        Vec2d newScale = m_actor->getWorldInitialScale();

        f32 hidingDuration = getTemplate()->getHidingFadeDuration();
        f32 showingDuration = getTemplate()->getShowingFadeDuration();
        f32 fadeCursor = 1.0f;

        if(hasUIState(UI_STATE_Showing))
        {
            // TODO : Use event to use textAnimScaleComponent
            m_isTransitionReady = isTransitionReady();
            UIComponent* parent = getParentComponent();
            isInTransition = btrue;

            // tweak values
            const Vec2d stiffness			= Vec2d(600.0f, 400.0f);
            const Vec2d damping				= Vec2d(15.5f, 8.0f);
            const Vec2d m_SpringRestLength	= Vec2d::One;

            Vec2d compressionRate = m_SpringRestLength - m_SpringLength;
            Vec2d force = (compressionRate * stiffness) - (m_SpringVelocity * damping);

            if(m_isTransitionReady && ( !parent || parent->m_isTransitionReady || !parent->m_useTransition))
            {
                m_transitionTime += dt;

                if(showingDuration>0.0f)
                {
                    if(dt==0.0f && m_fadeCursor==1.0f) // first update
                    {
                        fadeCursor = 0.0f;
                    }
                    else
                    {
                        fadeCursor = m_fadeCursor + dt / showingDuration;
                    }

                    if(fadeCursor>=1.0f)
                    {
                        fadeCursor = 1.0f;
                        isInTransition = bfalse;
                    }
                }
                else
                {
                    m_SpringVelocity += (force * dt);
                    m_SpringLength += (m_SpringVelocity * dt);

                    // done with the spring sim.
                    if(	compressionRate.IsEqual(Vec2d::Zero, MTH_BIG_EPSILON) && 
                        force.x() < 0.01f && force.y() < 0.01f &&
                        m_SpringVelocity.x() < 0.01f && m_SpringVelocity.y() < 0.01f)
                    {
                        isInTransition = bfalse;
                    }
                }
            }
            else
            {
                fadeCursor = 0.0f;
            }

            if(m_transitionMode == UI_TRANSITION_FarToClose)
            {
                newScale *= m_SpringLength;
            }
            else if(m_transitionMode == UI_TRANSITION_LeftToRight)
            {
                Vec2d newPosition = m_actor->getWorldInitialPos().truncateTo2D();
                newPosition.x() = Interpolate(-100.0f, newPosition.x(), m_SpringLength.y());
                setAbsolutePosition(newPosition, bfalse);
            }
        }
        else if(hasUIState(UI_STATE_Hiding))
        {
            m_transitionTime -= dt;
            isInTransition = btrue;


            if(hidingDuration>0.0f)
            {
                fadeCursor = m_fadeCursor - dt / hidingDuration;

                if(fadeCursor<=0.0f)
                {
                    fadeCursor = 0.0f;
                    isInTransition = bfalse;
                }
            }
            else
            {
                
                f32 transitionFractor = m_transitionTime / getTemplate()->g_transitionTimeBase;
                transitionFractor *= transitionFractor;
                transitionFractor *= transitionFractor;
                transitionFractor *= transitionFractor;
                transitionFractor *= transitionFractor;

                newScale *= f32_Clamp(transitionFractor, 0.0f, 1.3f);

                if(transitionFractor <= 0.001f)
                {
                    isInTransition = bfalse;
                }
            }
        }

        f32 alphaFactor = 1.f;
        bbool needEventShow = bfalse;
        if(hidingDuration==0.0f && showingDuration==0.0f)
        {
            static const f32 treeshold = 0.001f;

            if(newScale.x() <treeshold ) { newScale.x() = 0.001f; alphaFactor = 0.f; needEventShow = btrue;}
            if(newScale.y() <treeshold ) { newScale.y() = 0.001f; alphaFactor = 0.f; needEventShow = btrue;}

            if(newScale.x() >treeshold && m_actor->getScale().x()<=treeshold) { alphaFactor = 1.f; needEventShow = btrue;}
            if(newScale.y() >treeshold && m_actor->getScale().y()<=treeshold) { alphaFactor = 1.f; needEventShow = btrue;}

            m_actor->setScale(newScale);
            setBaseScale(newScale);
        }

        if(fadeCursor!=m_fadeCursor || needEventShow)
        {
            f32 alpha = sin(fadeCursor * MTH_PIBY2);
            EventShow show(alpha * alphaFactor, 0.0f);
            m_actor->onEvent(&show);
            m_fadeCursor = fadeCursor;
        }

        

        return isInTransition;
    }

#ifdef ITF_SUPPORT_EDITOR
    void UIComponent::drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags ) const
    {
        Bind * parentBind = m_actor->getParentBind();
        Actor * parent = m_actor;
        while ( parentBind != NULL )
        {
            parent = SAFE_DYNAMIC_CAST( SceneObjectPathUtils::getObjectFromPath(parent, parentBind->m_parent), Actor);
            if ( parent  ) //SAFE_DYNAMIC_CAST( SceneObjectPathUtils::getObjectFromPath( m_actor, m_actor->getParentBind()->m_parent ), Actor ) )
            {
                if ( UIComponent * uiComp = parent->GetComponent<UIComponent>() )
                {
                     ITF_WARNING( parent, !m_isMasterComponent || (m_displayMask == uiComp->m_displayMask), "UI component has not the same display mask as his parent, please update the main parent and save !" );
                }
                parentBind = parent->getParentBind();
            }
            else 
            {
                parentBind = NULL;
            }
        }        
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::onEditorCreated( class Actor* _original )
    {
        Super::onEditorCreated(_original);

        if(getIsMasterComponent() && m_actor->getIs2D())
        {
            Vec3d pos = m_actor->getPos();
            GFX_ADAPTER->compute3DTo2D(m_actor->getPos(), pos);
            pos.z() = 0;

            m_actor->setWorldInitialPos(pos, btrue);
            m_actor->set2DPos(pos.truncateTo2D());
            m_actor->setAABB(pos.truncateTo2D());

            m_actor->setLocalInitialScale(Vec2d::One);
            m_actor->setScale(m_actor->getLocalInitialScale());
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIComponent::drawBox2D(const Vec2d & _minBound, const Vec2d & _maxBound, const f32 _border, const Color _color)
    {
        Vec2d bottomRight(_maxBound.x(), _minBound.y());
        Vec2d topLeft(_minBound.x(), _maxBound.y());
        GFX_ADAPTER->drawDBG2dLine(_minBound, topLeft, _border, _color.getAsU32());
        GFX_ADAPTER->drawDBG2dLine(topLeft, _maxBound, _border, _color.getAsU32());
        GFX_ADAPTER->drawDBG2dLine(_maxBound, bottomRight, _border, _color.getAsU32());
        GFX_ADAPTER->drawDBG2dLine(bottomRight, _minBound, _border, _color.getAsU32());
    }

    void UIComponent::drawBox3D(const Vec2d & _minBound, const Vec2d & _maxBound, const f32 _border, const Color _color) const
    {
        Vec2d bottomRight(_maxBound.x(), _minBound.y());
        Vec2d topLeft(_minBound.x(), _maxBound.y());
        DebugDraw::line2D(_minBound, topLeft, f32(getAbsoluteDepth()), _color, _border);
        DebugDraw::line2D(topLeft, _maxBound, f32(getAbsoluteDepth()), _color, _border);
        DebugDraw::line2D(_maxBound, bottomRight, f32(getAbsoluteDepth()), _color, _border);
        DebugDraw::line2D(bottomRight, _minBound, f32(getAbsoluteDepth()), _color, _border);
    }

#endif // ITF_SUPPORT_EDITOR

    bbool UIComponent::isEditionMode(const Actor* actor, bbool _checkSelected)
    {
        #ifdef ITF_SUPPORT_EDITOR
        if(actor)
        {
            if(GameScreen * gamescreen = GAMEMANAGER->getCurrentGameScreenInstance())
            {
                bbool isEdition = (gamescreen->getWorld() == actor->getWorld()) && (!_checkSelected || actor->isSelected());
                return isEdition;
            } 
        }
        #endif

        return bfalse;
    }

    void UIComponent::setBaseScale( const Vec2d& _inBaseScale )
    {
        if (GetActor()->getIs2D())
        {
            m_baseScale = _inBaseScale * GetActor()->currentResolutionToReferenceFactor();
        }
        else
        {
            m_baseScale = _inBaseScale;
        }
    }

    Vec2d UIComponent::getBaseScale()
    {
        if (GetActor()->getIs2D())
        {
            return m_baseScale * GetActor()->referenceToCurrentResolutionFactor();
        } else
        {
            return m_baseScale;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(UIComponent_Template)
        SERIALIZE_BOOL("is2D", m_is2D);
        SERIALIZE_BOOL("draw2DNoScreenRatio", m_is2DNoScreenRatio);
        SERIALIZE_MEMBER("showingFadeDuration", m_showingFadeDuration);
        SERIALIZE_MEMBER("hidingFadeDuration", m_hidingFadeDuration);
    END_SERIALIZATION()

    UIComponent_Template::UIComponent_Template()
    : m_is2D(btrue)
    , m_is2DNoScreenRatio(bfalse)
    , m_showingFadeDuration(0.0f)
    , m_hidingFadeDuration(0.0f)
    {
    }

    UIComponent_Template::~UIComponent_Template()
    {
    }
}
