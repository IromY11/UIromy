#include "precompiled_engine.h"

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef SERIALIZEROBJECTLUA_H
#include "engine/serializer/ZSerializerObjectLUA.h"
#endif // SERIALIZEROBJECTLUA_H

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_BASEOBJECTFACTORY_H_
#include "engine/factory/baseObjectFactory.h"
#endif //_ITF_BASEOBJECTFACTORY_H_

#ifndef _ITF_GHOSTMANAGER_H_
#include "engine/actors/managers/ghostmanager.h"
#endif //_ITF_GHOSTMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_CHARACTERDBUGGERCOMPONENT_H_
#include "gameplay/components/Common/CharacterDebuggerComponent.h"
#endif //_ITF_CHARACTERDBUGGERCOMPONENT_H_

#include "engine/profiler/profilerMacro.h"

#ifdef ITF_SUPPORT_EDITOR
    #include "editor/Editor.h"
#endif //ITF_SUPPORT_EDITOR

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

namespace ITF
{

#define COMPONENTS_TAG "COMPONENTS"

IMPLEMENT_OBJECT_RTTI(Actor)

DECLARE_RASTER(actorUpdate, RasterGroup_ActorUpdate, Color::blue());
DECLARE_RASTER(componentUpdate, RasterGroup_ComponentUpdate, Color::green());

BEGIN_SERIALIZATION_CHILD(Actor)

    SERIALIZE_FUNCTION(preSaveData, ESerialize_Data_Save);

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)

        SERIALIZE_MEMBER_DESCRIPTION("LUA","this is the lua file used for the template");
        SERIALIZE_MEMBER("LUA",m_templatePath);
        
        BEGIN_CONDITION_BLOCK_NOT(ESerialize_Instance)
            SERIALIZE_OBJECT("parentBind", m_pParentBind_Initial);
        END_CONDITION_BLOCK()

    END_CONDITION_BLOCK()

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable|ESerializeGroup_Checkpoint|ESerialize_ForcedValues)
        SERIALIZE_CONTAINER_WITH_FACTORY(COMPONENTS_TAG,m_components,GAMEINTERFACE->getActorComponentsFactory());
    END_CONDITION_BLOCK()

END_SERIALIZATION()

///////////////////////////////////////////////////////////////////////////////////////////
static SafeArray<StringID::StringIdValueType> g_rasterColorRegistry;

#ifdef ITF_SUPPORT_EDITOR
    Vec2d ActorDrawEditInterface::m_currentMouse2d;
    Vec3d ActorDrawEditInterface::m_currentMouse3d;
#endif

///////////////////////////////////////////////////////////////////////////////////////////

Actor::Actor()
: Pickable()
, m_pParentBind(NULL)
, m_pParentBind_Initial(NULL)
, m_overrideTemplateActor(NULL)
, m_deleteOverrideTemplateWithActor(0)
, m_transferEventsToLinkChildren(0)
, m_actorActiveDone(bfalse)
, m_actorSceneActiveDone(bfalse)
, m_actorComponentProcessed(bfalse)
, m_actorComponentFinalizeLoadDone(bfalse)
, m_onlineState(Offline)
, m_radius(0.f)
, m_timeFactor(1.f)
, m_physShape(NULL)
{
    setObjectType(BaseObject::eActor);
    m_childrenHandler.setOwner(this);

#ifdef ASSERT_ENABLED
    m_hotReloadingStatus = HotReloadType_None;
#endif  //ASSERT_ENABLED
}

Actor::~Actor()
{
    u32 numComponents = m_components.size();

    for ( u32 it = 0; it < numComponents; ++it )
    {
        DELETE_SERIALIZED_OBJECT(m_components[it]);
    }

    if (m_physShape!= NULL)
    {
        SF_DEL(m_physShape);
        m_physShape = NULL;
    }

    DELETE_SERIALIZED_OBJECT(m_pParentBind_Initial);
}

void Actor::onDestroy( bbool _hotReload )
{
    ClearComponents(btrue,btrue);

    if ( m_overrideTemplateActor && !_hotReload )
    {
#if ITF_SUPPORT_EDITOR
        TEMPLATEDATABASE->removeClient(m_overrideTemplateActor->getFile(),getRef());
#endif

        if ( m_deleteOverrideTemplateWithActor )
        {
            SF_DEL(m_overrideTemplateActor);
        }
    }

    m_registeredEvents.clear();

    if ( _hotReload )
    {
        m_transferEventsToLinkChildren = bfalse;
    }

    Super::onDestroy(_hotReload);

    // RO2-3789
    // The normal unbind process is done in onStartDestroy(...)
    // We also unbind here because hotreloading a binded actors in the middle of it's async loading (before onFinalizeLoad(...) call) won't unbind
    // As the bind is registered in onLoaded(...) we unbind in the symetric call : onDestroy(...)
    {
        unbindFromParent();

        if ( !_hotReload )
        {
            clearChildBinds(bfalse);
        }
    }
}

void Actor::preSaveData()
{
    // if it's a temporary bind (runtime), delete the object before serialization
    // it's not re-created after the serialization => why ? because it needs actorbind copy, creation of a member...
    // a better management would be bind "a bind manager" maybe ??
    if ( ( m_pParentBind != NULL ) && m_pParentBind->m_temporaryBind )
    {
        unbindFromParent();
    }
}

   
void Actor::requestDestruction()
{
    if ( !isDestructionRequested() )
    {
        if ( isAsyncLoading() || !getScene() )
        {
            setDestructionRequested(btrue);
            WORLD_MANAGER->deletePickable(this);
        }
        else
        {
            m_childrenHandler.requestChildrenDestruction();

            unbindFromParent();
            clearChildBinds(bfalse);

            Super::requestDestruction();
        }
    }
}

void Actor::setOnlineState(OnlineState state)
{
	m_onlineState = state;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Actor::requestTemplateType()
{
    m_templatePickable = (Actor_Template*)requestTemplateType(m_templatePath);
}

const TemplatePickable* Actor::requestTemplateType(const Path& _path) const
{
    return TEMPLATEDATABASE->requestTemplate<Actor_Template>(_path,getRef());
}

void Actor::onLoaded(HotReloadType _hotReload)
{
    Super::onLoaded(_hotReload);

#ifndef ITF_FINAL
    onLoadClearNULLComponents();
#endif

#ifdef ASSERT_ENABLED
    m_hotReloadingStatus = _hotReload;
#endif  //ASSERT_ENABLED

    onLoadProcessTemplate();

#ifndef ITF_FINAL
    if ( hasDataError() )
    {
#ifdef ASSERT_ENABLED
        m_hotReloadingStatus = HotReloadType_None;
#endif  //ASSERT_ENABLED
        return;
    }
#endif

    if ( getTemplate()->getForceEnvironment() )
    {
        setUpdateGroup(WorldUpdate::UpdateGroup_Environment);
    }

	if (  getTemplate()->getForceAlwaysActive())
		WORLD_MANAGER->getWorldUpdate().setAlwaysActive(this, btrue);
	else if ( ! getIs2D() )
		WORLD_MANAGER->getWorldUpdate().setAlwaysActive(this, bfalse);

    onLoadPreProcessComponents(_hotReload);
    onLoadProcessBind(_hotReload);
    onLoadProcessComponents(_hotReload);

#ifdef ASSERT_ENABLED
    m_hotReloadingStatus = HotReloadType_None;
#endif  //ASSERT_ENABLED
}

void Actor::onFinalizeLoad( bbool _hotReload )
{
    Super::onFinalizeLoad(_hotReload);

#ifndef ITF_FINAL
    if ( hasDataError() )
    {
        return;
    }
#endif

    if (getTemplate())
    {
        for ( ComponentsVector::iterator it = m_components.begin(); it != m_components.end(); ++it )
        {
            (*it)->onFinalizeLoad();
        }
        
        m_actorComponentFinalizeLoadDone =  btrue;
    }

    //grow bind parent AABB with self one (we have to do that after components load)
    if(Actor * const parent = getParent().getActor())
    {
        parent->growAABB3d(getAABB3d());
    }
}

void Actor::onStartDestroy( bbool _hotReload )
{
	//Unregister SoundActor if it exists
	if(AUDIO_ADAPTER)
		AUDIO_ADAPTER->unregisterObjectRef(getRef());


    if (getTemplate() && m_actorComponentFinalizeLoadDone)
    {
        for ( ComponentsVector::iterator it = m_components.begin(); it != m_components.end(); ++it )
        {
            (*it)->onStartDestroy(_hotReload);
        }
        
        m_actorComponentFinalizeLoadDone = bfalse;
    }

    if ( m_overrideTemplateActor )
    {
        if ( m_templatePickable )
        {
            removeResourceContainer(m_overrideTemplateActor->getResourceContainer());
            m_templatePickable = NULL;
        }
    }

    Super::onStartDestroy(_hotReload);

    unbindFromParent();

    if ( !_hotReload )
    {
        clearChildBinds(bfalse);
    }
}

void Actor::setOverrideTemplate(Actor_Template* _overrideTemplateActor, bbool _deleteWithActor /* = bfalse */ )
{
    if ( m_overrideTemplateActor != _overrideTemplateActor )
    {
#if ITF_SUPPORT_HOTRELOAD_TEMPLATE
        if ( m_overrideTemplateActor )
        {
            TEMPLATEDATABASE->removeClient(m_overrideTemplateActor->getFile(),getRef());
        }
#endif //ITF_SUPPORT_HOTRELOAD_TEMPLATE

        m_overrideTemplateActor = _overrideTemplateActor;
        m_deleteOverrideTemplateWithActor = _deleteWithActor;

        if ( m_overrideTemplateActor )
        {
#if ITF_SUPPORT_HOTRELOAD_TEMPLATE
            TEMPLATEDATABASE->addClient(m_overrideTemplateActor->getFile(),getRef());
#endif //ITF_SUPPORT_HOTRELOAD_TEMPLATE
        }
        else
        {
            if ( isTemplateAcquired() )
            {
               requestTemplateType();
            }
        }
    }
}

void Actor::onLoadClearNULLComponents()
{
    ComponentsVector::iterator it = m_components.begin();

    while ( it != m_components.end() )
    {
        if ( !(*it))
        {
            it = m_components.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Actor::onLoadProcessTemplate()
{
#ifdef ITF_SUPPORT_EDITOR
    static const String8 tplExt = "tpl";
    static const String8 actExt = "act";
    if ( m_templatePath.getExtension() != tplExt || ( !m_instanceDataFile.isEmpty() && m_instanceDataFile.getExtension() != actExt ) )
    {
#ifndef ITF_FINAL
        setTemplateError();
#endif
        return;
    }
#endif

    if ( m_overrideTemplateActor )
    {
        if (m_templatePickable)
        {
            TEMPLATEDATABASE->releaseTemplate(m_templatePickable->getFile(),getRef());
        }
        m_templatePickable = m_overrideTemplateActor;
    }

    if ( !m_templatePickable )
    {
        for ( ComponentsVector::iterator it = m_components.begin(); it != m_components.end(); ++it )
        {
            ActorComponent* pComponent = *it;
            if(pComponent)
            {
                pComponent->SetActor(this);
            }
        }

#ifndef ITF_FINAL
        setTemplateError();
#endif
        return;
    }

    //GG David ,we have to investigate this issue
//#ifdef ITF_SUPPORT_EDITOR
    fixComponentsFromTemplate(getTemplate());
//#endif // ITF_SUPPORT_EDITOR
}

void Actor::onLoadProcessBind( HotReloadType _hotReload )
{
    m_pParentBind = m_pParentBind_Initial;

    // If there is a parent bind, warn the parent at the loading time
    if(m_pParentBind)
    {
        const ObjectPath& pathToParent = m_pParentBind->m_parent;

        // Warn invalid hierarchy binding
        ITF_WARNING_PARAM_CATEGORY(LD, this, "m_pParentBind", !Bind::relativePathGoingUp(pathToParent), "Binding across scene hierarchy not allowed!");

        ITF_ASSERT(!pathToParent.getIsAbsolute());
        Pickable* pParent = SceneObjectPathUtils::getObjectFromRelativePath(this, pathToParent);

        if(pParent)
        {
            ITF_ASSERT_CRASH(pParent->getObjectType() == BaseObject::eActor, "bad type");
            Actor* pActorParent = (Actor*)pParent;
            pActorParent->getChildrenBindHandler().addChild(this,btrue); // here the children make the parent to compute curr and init pos info
        }
        else
        {
            ITF_STDSTRING str;
            pathToParent.toString(str);

            ITF_WARNING_PARAM_CATEGORY(LD,this, "m_pParentBind", 0, "Dead bind! Cannot find parent: '%s'", str.c_str());
        }
    }

    // The parent is positionning his children at load time
    // It's done from parent and children because of the call order of onLoaded(...)
    const ObjectRefList& bindedChildren = m_childrenHandler.getChildren();
    for(u32 i = 0; i < bindedChildren.size(); ++i)
    {
        Actor* pChild = static_cast<Actor*>(bindedChildren[i].getObject());

        pChild->resetTransformationToInitial();
        m_childrenHandler.updateWorldCoordinates(pChild, pChild->getParentBind());
        m_childrenHandler.computeInitialTransform(pChild);
    }
}

void Actor::onLoadPreProcessComponents( HotReloadType _hotReload )
{
    if(!m_components.size())
        return;
    Actor_Template::ComponentList::const_iterator itTemplate = getTemplate()->getComponents().begin();
    ComponentsVector::iterator it = m_components.begin();

    for (; it != m_components.end(); it++, itTemplate++ )
    {
        ActorComponent* pComponent = *it;
        ActorComponent_Template* pTemplate = *itTemplate;
        ITF_ASSERT_MSG(pComponent, "An actor could not have a NULL component");
        if(pComponent && pTemplate)
        {
            pComponent->setTemplate(pTemplate);
            pComponent->SetActor(this);

            if (pComponent->is2D())
                setIs2D(btrue);

            if (pComponent->needsDraw2DNoScreenRatio())
                setIs2DNoScreenRatio(btrue);
        }
    }
}

void Actor::onLoadProcessComponents( HotReloadType _hotReload )
{
    if(!m_components.size())
        return;
    Actor_Template::ComponentList::const_iterator itTemplate = getTemplate()->getComponents().begin();
    ComponentsVector::iterator it = m_components.begin();

    while ( it != m_components.end() )
    {
        ActorComponent* pComponent = *it;
        ActorComponent_Template* pTemplate = *itTemplate;
        ITF_ASSERT_MSG(pComponent, "An actor could not have a NULL component");
        if(pComponent && pTemplate)
        {
            pComponent->onActorLoaded(_hotReload);

            if ( !m_transferEventsToLinkChildren )
            {
                LinkComponent* link = pComponent->DynamicCast<LinkComponent>(ITF_GET_STRINGID_CRC(LinkComponent,1144483611));

                if ( link )
                {
                    const LinkComponent_Template* linkTemplate = static_cast<const LinkComponent_Template*>(pComponent->getTemplate());

                    if ( linkTemplate )
                    {
                        m_transferEventsToLinkChildren = linkTemplate->isTransferEventsToChildren();
                    }
                }
            }

            it++;
        }
        else
        {
#ifndef ITF_FINAL
            setDataError("Null component");
#endif
            it = m_components.erase(it);
        }

        itTemplate++;
    }
    
    m_actorComponentProcessed = btrue;

#ifndef ITF_FINAL
    if (!hasDataError())
#endif
    {  
        // Now that all the components are initialized, add them to the update lists
        // This needs to be done after loading ALL the components, in case one modifies another
        for ( u32 it = 0; it < m_components.size(); ++it )
        {
            ActorComponent* actorComponent = m_components[it];
            if(!actorComponent)
                continue;

            if (actorComponent->needsDraw() )
            {
#ifndef ITF_FINAL
                if (m_drawComponents.size() >= m_components.size())
                {
                    ITF_ASSERT(bfalse);
                    setDataError("Bad .tpl file? m_drawComponents.size() >= m_components.size()");
                }
                if (m_drawComponents.find(actorComponent) >= 0)
                {
                    ITF_ASSERT(bfalse);
                    setDataError("Bad .tpl file? m_drawComponents.find(actorComponent) >= 0");
                }
                if (!hasDataError())
#endif // ITF_FINAL
                {
                    m_drawComponents.push_back(actorComponent);
                }
            }
            else if (actorComponent->needsDraw2D())
            {
#ifndef ITF_FINAL
                if (m_draw2DComponents.size() >= m_components.size())
                {
                    ITF_ASSERT(bfalse);
                    setDataError("Bad .tpl file? m_draw2DComponents.size() >= m_components.size()");
                }
                if (m_draw2DComponents.find(actorComponent) >= 0)
                {
                    ITF_ASSERT(bfalse);
                    setDataError("Bad .tpl file? m_draw2DComponents.find(actorComponent) >= 0");
                }
                if (!hasDataError())
#endif // ifndef ITF_FINAL
                {
                    m_draw2DComponents.push_back(actorComponent);
                }
            }

            if ( actorComponent->needsUpdate() 
#ifndef ITF_FINAL
                && !hasDataError()
#endif
                )
            {
                ITF_ASSERT_CRASH(m_updateComponents.size() < m_components.size(),"m_updateComponents.size() < m_components.size()");
                ITF_ASSERT_CRASH(m_updateComponents.find(actorComponent) < 0,"m_updateComponents.find(actorComponent)");
                m_updateComponents.push_back(actorComponent);
            }
        }
    }
}

void Actor::onForceMove()
{
#ifndef ITF_FINAL
    if(!hasDataError())
#endif
    {
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* pComponent = m_components[it];
            ITF_ASSERT(pComponent);

            pComponent->onForceMove();
        }
    }
}

void Actor::forceMoveSkipComponent( const Vec3d& _pos, f32 _angle, ActorComponent* _skip )
{
    setPos(_pos);
    setAngle(_angle);

#ifndef ITF_FINAL
    if(!hasDataError())
#endif
    {
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* pComponent = m_components[it];

            ITF_ASSERT(pComponent);

            if ( pComponent != _skip ) 
            {
                pComponent->onForceMove();
            }
        }
    }
}

void Actor::AddComponent( ActorComponent* _actorComponent, bbool _udpateLists )
{
    ITF_ASSERT_MSG(_actorComponent, "Do not add a null component to an actor");
    if(_actorComponent == NULL)
        return;

    m_components.push_back(_actorComponent);
    _actorComponent->SetActor(this);

    if (!_udpateLists)
        return;

    if ( _actorComponent->needsDraw() )
    {
        ITF_ASSERT(m_drawComponents.size() < m_components.size());
        ITF_ASSERT(m_drawComponents.find(_actorComponent) < 0);
        m_drawComponents.push_back(_actorComponent);
    }
    else if (_actorComponent->needsDraw2D())
    {
        ITF_ASSERT(m_draw2DComponents.size() < m_components.size());
        ITF_ASSERT(m_draw2DComponents.find(_actorComponent) < 0);
        m_draw2DComponents.push_back(_actorComponent);
    }

    if ( _actorComponent->needsUpdate() )
    {
        ITF_ASSERT_CRASH(m_updateComponents.size() < m_components.size(),"m_updateComponents.size() < m_components.size()");
        ITF_ASSERT_CRASH(m_updateComponents.find(_actorComponent) < 0,"m_updateComponents.find(_actorComponent) < 0");
        m_updateComponents.push_back(_actorComponent);
    }
}

ActorComponent* Actor::getComponentFromStaticClassCRC(StringID::StringIdValueType _staticClassCRC) const
{
     for ( u32 it = 0; it < m_components.size(); ++it )
     {
         ActorComponent* pComponent = m_components[it];
         ITF_ASSERT_MSG(pComponent, "An actor could not have a NULL component");

         if ( pComponent && pComponent->IsClassCRC(_staticClassCRC) )
         {
             return static_cast<ActorComponent*>(pComponent);
         }
     }

     return NULL;
}

void Actor::ClearComponents(bbool _clearMem,bbool _clearcomponents)
{
    if ( _clearcomponents && m_actorComponentProcessed )
    {
        for ( u32 it = 0; it < m_components.size(); ++it )
        {
            ActorComponent* component = m_components[it];
            component->onActorClearComponents();
        }
    }

    if (_clearMem)
    {
        for ( u32 it = 0; it < m_components.size(); ++it )
        {
            ActorComponent* component = m_components[it];

            for ( u32 i = 0; i < m_registeredEvents.size(); i++ )
            {
                RegisteredEvent& registeredEvent = m_registeredEvents[i];
                u32 numListeners = registeredEvent.m_listeners.size();

                for ( u32 j = 0; j < numListeners; j++ )
                {
                    if ( registeredEvent.m_listeners[j] == component )
                    {
                        registeredEvent.m_listeners.eraseNoOrder(j);
                        break;
                    }
                }
            }

            SF_DEL(m_components[it]);
        }
    }
    m_components.clear();
    m_drawComponents.clear();
    m_draw2DComponents.clear();
    m_updateComponents.clear();
    m_actorComponentProcessed = bfalse;

#if defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_COOKING)
    m_componentBeforeInstanceData.clear();
#endif // ITF_SUPPORT_EDITOR || ITF_SUPPORT_COOKING
}

#if defined ITF_WINDOWS && !defined (ITF_FINAL)
#define CHECK_SCENE() {const Scene* pScene = getScene();ITF_ASSERT_CRASH(pScene,"scene associated to actor can't be null %s",m_userFriendly.cStr());}
#else
#define CHECK_SCENE() {}
#endif //defined ITF_WINDOWS && !defined (ITF_FINAL)

void Actor::update( f32 _deltaTime)
{
   PROFILER_UPDATE_PICKABLE(this,getRef());
   PRF_M_SCOPE(updateActor)

    //---------------------------------------------------------------------------------------------
    // DEBUG BREAKPOINT
    #ifdef ITF_SUPPORT_DEBUGFEATURE
        DebugInfo::stopOnRef(this);//set ITF::DebugInfo::m_breakOnRef to 0 to disable it
        if (DEBUGINFO->breakpointTest(getRef()))
        {
            #if defined(ITF_PS3) || defined(ITF_IOS)  || defined(ITF_ANDROID)
                __DEBUG_BREAK();
            #else
                if (ISDEBUGGER_PRESENT ) 
                    { __DEBUG_BREAK(); }
                #ifdef ITF_SUPPORT_EDITOR
                    else
                    {
                        String8 pauseMessage;
                        pauseMessage.setTextFormat("  Break on update actor: %s", m_userFriendly.cStr() );
                        DEBUGINFO->breakpointPauseEditor(pauseMessage);
                    }
                #endif  // ITF_SUPPORT_EDITOR
            #endif // ITF_PS3
        }
    #endif  //ITF_SUPPORT_DEBUGFEATURE
    //---------------------------------------------------------------------------------------------
    
#ifndef ITF_FINAL
    m_drawCount = 0;
#endif // ITF_FINAL

    CHECK_SCENE()

    ITF_ASSERT_MSG(isActive(),"Something is very wrong, update is being called without calling onBecomeActive");

    STATS_ACTOR_UPDATEINC
    
    setAABB(get2DPos());

#ifndef ITF_FINAL
    if (hasDataError())
    {
        return;
    }
#endif

    for ( u32 it = 0; it < m_updateComponents.size(); ++it )
    {
        ActorComponent* pActorComponent = m_updateComponents[it];
        PROFILER_COMPONENT(getRef(),pActorComponent->GetObjectClassCRC());

        //---------------------------------------------------------------------------------------------
        // DEBUG BREAKPOINT
        #ifdef ITF_SUPPORT_DEBUGFEATURE
            if (DEBUGINFO->breakpointTest(getRef(), pActorComponent->GetObjectClassCRC()))
            {
                #if defined(ITF_PS3) || defined(ITF_IOS)  || defined(ITF_ANDROID)
                    __DEBUG_BREAK();
                #else
                    if ( ISDEBUGGER_PRESENT )
                        { __DEBUG_BREAK();}
                    #ifdef ITF_SUPPORT_EDITOR
                        else
                        {
                            String8 pauseMessage;
                            pauseMessage.setTextFormat("  Break on update actor component: %s / %s", m_userFriendly.cStr(), pActorComponent->GetObjectClassName() );
                            DEBUGINFO->breakpointPauseEditor(pauseMessage);
                        }
                    #endif  // ITF_SUPPORT_EDITOR
                #endif // ITF_PS3
            }
        #endif  //ITF_SUPPORT_DEBUGFEATURE
        //---------------------------------------------------------------------------------------------

        if (!pActorComponent->getUpdateDisabled())
        {
            pActorComponent->Update(_deltaTime * getTimeFactor());
        }
    }

    m_childrenHandler.update();
}

void Actor::onWorldUpdate()
{
    PROFILER_UPDATE_PICKABLE(this,getRef());
    PRF_M_SCOPE(worldUpdateActor)

    //---------------------------------------------------------------------------------------------
    // DEBUG BREAKPOINT
    #ifdef ITF_SUPPORT_DEBUGFEATURE
        DebugInfo::stopOnRef(this);//set ITF::DebugInfo::m_breakOnRef to 0 to disable it
        if (DEBUGINFO->breakpointTest(getRef()))
        {

            #if defined(ITF_PS3) || defined(ITF_IOS)  || defined(ITF_ANDROID)
                __DEBUG_BREAK();
            #else
                if ( ISDEBUGGER_PRESENT )
                    { __DEBUG_BREAK();}
                #ifdef ITF_SUPPORT_EDITOR
                    else
                    {
                        String8 pauseMessage;
                        pauseMessage.setTextFormat("  Break on world update actor: %s", m_userFriendly.cStr() );
                        DEBUGINFO->breakpointPauseEditor(pauseMessage);
                    }
                #endif  // ITF_SUPPORT_EDITOR
            #endif // ITF_PS3

        }
    #endif  //ITF_SUPPORT_DEBUGFEATURE
    //---------------------------------------------------------------------------------------------

#ifndef ITF_FINAL
    if (hasDataError())
        return;
#endif

    for ( u32 it = 0; it < m_updateComponents.size(); ++it )
    {
        ActorComponent* pActorComponent = m_updateComponents[it];
        PROFILER_COMPONENT(getRef(),pActorComponent->GetObjectClassCRC());

        //---------------------------------------------------------------------------------------------
        // DEBUG BREAKPOINT
        #ifdef ITF_SUPPORT_DEBUGFEATURE
            if (DEBUGINFO->breakpointTest(getRef(), pActorComponent->GetObjectClassCRC()))
            {
                #if defined(ITF_PS3) || defined(ITF_IOS)  || defined(ITF_ANDROID)
                    __DEBUG_BREAK();
                #else
                    if ( ISDEBUGGER_PRESENT )
                        { __DEBUG_BREAK();}
                    #ifdef ITF_SUPPORT_EDITOR
                        else
                        {
                            String8 pauseMessage;
                            pauseMessage.setTextFormat("  Break on world update actor component: %s / %s", m_userFriendly.cStr(), pActorComponent->GetObjectClassName() );
                            DEBUGINFO->breakpointPauseEditor(pauseMessage);
                        }
                    #endif  // ITF_SUPPORT_EDITOR
                #endif // ITF_PS3
            }
        #endif  //ITF_SUPPORT_DEBUGFEATURE
        //---------------------------------------------------------------------------------------------

        pActorComponent->onWorldUpdate();
    }
}


void Actor::batchPrimitives( const ITF_VECTOR <class View*>& _views )
{
    PROFILER_BATCHPRIMITIVE_PICKABLE(this,getRef());

#ifndef ITF_FINAL
    if ( hasDataError() )
        return;
#endif

    STATS_ACTOR_DRAWINC

#ifndef ITF_FINAL
    m_drawCount++;
#endif // ITF_FINAL

    for ( u32 it = 0; it < m_drawComponents.size(); ++it )
        m_drawComponents[it]->batchPrimitives(_views);
}

void Actor::batchPrimitives2D( const ITF_VECTOR <class View*>& _views )
{
    PROFILER_BATCHPRIMITIVE_PICKABLE(this,getRef());

#ifndef ITF_FINAL
    if ( hasDataError() )
        return;
#endif

    STATS_ACTOR_DRAW2DINC

    for ( u32 it = 0; it < m_draw2DComponents.size(); ++it )
        m_draw2DComponents[it]->batchPrimitives2D(_views);
}


void Actor::onSceneActive()
{
    Super::onSceneActive();

#ifndef ITF_FINAL
    if(!hasDataError())
#endif
    {
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* pComponent = m_components[it];
            ITF_ASSERT(pComponent);
            pComponent->onSceneActive();
        }

        m_actorSceneActiveDone = btrue;
    }
    
    getChildrenBindHandler().onSceneActive();
    if(m_pParentBind)
        m_pParentBind->onSceneActive(this);
}

void Actor::onSceneInactive()
{
    if(m_pParentBind)
        m_pParentBind->onSceneInactive(this);
    getChildrenBindHandler().onSceneInactive();
    
#ifndef ITF_FINAL
    if( m_actorSceneActiveDone )
#endif
    {
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* pComponent = m_components[it];
            ITF_ASSERT(pComponent);
            pComponent->onSceneInactive();
        }

        m_actorSceneActiveDone = bfalse;
    }
    
    Super::onSceneInactive();
}

void Actor::onBecomeActive()
{
    Super::onBecomeActive();

#ifndef ITF_FINAL
    if(!hasDataError())
#endif
    {
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* pComponent = m_components[it];
            ITF_ASSERT(pComponent);
            pComponent->onBecomeActive();
        }

        m_actorActiveDone = btrue;
    }
}

void Actor::onBecomeInactive()
{
    Super::onBecomeInactive();

#ifndef ITF_FINAL
    if( m_actorActiveDone )
#endif
    {
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* pComponent = m_components[it];
            pComponent->onBecomeInactive();
        }

        m_actorActiveDone = bfalse;
    }
}

void Actor::onResourceLoaded()
{
    ITF_ASSERT(isPhysicalReady());

#ifndef ITF_FINAL
    resetDataError();
#endif

     const u32 uSize = m_components.size();
     for ( u32 it = 0; it < uSize; ++it )
     {
         ActorComponent* pComponent = m_components[it];
         ITF_ASSERT_MSG(pComponent, "NULL component !!!");
         pComponent->onResourceLoaded();
     }

     for ( u32 it = 0; it < uSize; ++it )
     {
         ActorComponent* pComponent = m_components[it];

         ITF_ASSERT_MSG(pComponent, "NULL component !!!");
         pComponent->onResourceReady();
     }

#ifndef ITF_FINAL
    computeNbBones();//profile purpose
#endif

    Super::onResourceLoaded();
}

void Actor::onResourceUnloaded()
{
    Super::onResourceUnloaded();

#ifndef ITF_FINAL
    if(!hasDataError())
#endif
    {
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* pComponent = m_components[it];
            ITF_ASSERT(pComponent);
            pComponent->onUnloadResources();
        }
    }
} 

Vec3d Actor::getBase() const
{
    f32 radius = getRadius();
    if (radius == 0.f)
    {
        return getPos();
    }

    Vec2d HorizAxis;
    f32_CosSin(getAngle(), &HorizAxis.x(), &HorizAxis.y());
    Vec2d vertAxis = HorizAxis.getPerpendicular();
    Vec2d contact = Vec2d(getPos().x(), getPos().y());

    contact -= vertAxis * getRadius();
    return Vec3d(contact.x(), contact.y(), getDepth());
}

void Actor::registerEvent( StringID::StringIdValueType _eventName, IEventListener* _listener )
{
    const ObjectFactory* eventFactory = GAMEINTERFACE->getEventFactory();

    const ObjectFactory::ClassInfo* eventClass = eventFactory->GetClassInfo(_eventName);
    ITF_ASSERT_MSG(eventClass, "Events must be registered in the event factory!");
    if (!eventClass)
    {
        return;
    }

    RegisteredListenerArray* registeredListeners(NULL);

    for (RegisteredEventsArray::iterator it = m_registeredEvents.begin(); it != m_registeredEvents.end(); )
    {
        RegisteredEvent& registeredEvent = *it;
        StringID::StringIdValueType registerEventName = registeredEvent.m_eventClass->m_crc;

        if ( _eventName == registerEventName )
        {
            // this event has already been registered
            registeredListeners = &registeredEvent.m_listeners;
        }
        else if ( eventClass->IsClass(registerEventName) )
        {
            // a parent event has already been registered...
            RegisteredListenerArray& components = registeredEvent.m_listeners;
            if ( components.find(_listener) != -1 )
            {
                // ... by this component: we're covered.
                return;
            }
        }
        else if ( registeredEvent.m_eventClass->IsClass(_eventName) )
        {
            // a child event has already been registered...
            RegisteredListenerArray& components = registeredEvent.m_listeners;
            i32 index = components.find(_listener);
            if ( index != -1 )
            {
                // ... by this component: remove it (it'll be replaced with this one)
                components.eraseNoOrder(index);

                if (components.size() == 0)
                {
                    it = m_registeredEvents.erase(it);
                    continue;   // erase returns the next element or end
                }
            }
        }

        it++;
    }


    if ( !registeredListeners )
    {
        // this event has never been registered, add an entry
#ifdef ITF_WII
        MEM_M_PushExt(MEM_C_MEM1_ALLOC);
#endif
        m_registeredEvents.push_back(RegisteredEvent());
#ifdef ITF_WII
        MEM_M_PopExt();
#endif
        RegisteredEvent& registeredEvent = m_registeredEvents.back();
        registeredEvent.m_eventClass = eventClass;
        registeredListeners = &registeredEvent.m_listeners;
    }

    // add our component if not already present
    if ( registeredListeners->find(_listener) == -1 )
    {
        registeredListeners->push_back(_listener);
    }
}

void Actor::unregisterEvent( StringID::StringIdValueType _crc, IEventListener* _listener )
{
#ifdef ITF_SUPPORT_EDITOR
    bbool bfound = bfalse;
#endif //ITF_SUPPORT_EDITOR

    for (RegisteredEventsArray::iterator it = m_registeredEvents.begin(); it != m_registeredEvents.end(); it++)
    {
        RegisteredEvent& registeredEvent = *it;
        
        if ( registeredEvent.m_eventClass->IsClass(_crc) )
        {
            // a child event has already been registered...
            RegisteredListenerArray& components = registeredEvent.m_listeners;
            i32 index = components.find(_listener);
            if ( index != -1 )
            {
                // ... by this component: remove it
                components.eraseNoOrder(index);

#ifdef ITF_SUPPORT_EDITOR
                bfound = btrue;
#endif //ITF_SUPPORT_EDITOR

                if (components.size() == 0)
                {
                    it = m_registeredEvents.erase(it);
                }
                break;
            }
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    ITF_ASSERT_CRASH(bfound,"Trying to unregister an event that was not registered. Check this !");
#endif //ITF_SUPPORT_EDITOR
}

bbool Actor::isEventRegistered( StringID::StringIdValueType _crc, IEventListener* _listener )
{
    const ObjectFactory* eventFactory = GAMEINTERFACE->getEventFactory();

    const ObjectFactory::ClassInfo* eventClass = eventFactory->GetClassInfo(_crc);
    ITF_ASSERT_MSG(eventClass, "Events must be registered in the event factory!");
    if (!eventClass)
    {
        return bfalse;
    }

    for (RegisteredEventsArray::iterator it = m_registeredEvents.begin(); it != m_registeredEvents.end(); it++ )
    {
        RegisteredEvent& registeredEvent = *it;

        if ( registeredEvent.m_eventClass->IsClass(_crc) )
        {
            if (_listener == NULL)
                return btrue;
            RegisteredListenerArray& components = registeredEvent.m_listeners;
            i32 index = components.find(_listener);
            if ( index != -1 )
                return btrue;
        }
    }
    return bfalse;
}

void Actor::onEvent(Event* _event)
{
    STATS_ONACTOREVENTINC
    Super::onEvent(_event);

    if ( isDestructionRequested() )
    {
        return;
    }

    if ( EventTeleport* pTeleportEvent = DYNAMIC_CAST(_event,EventTeleport) )
    {
        if (pTeleportEvent->getApplyPosAndAngle())
        {
            setPos(pTeleportEvent->getPos());
            setAngle(pTeleportEvent->getAngle());

            if ( pTeleportEvent->getReset() )
            {
                onForceMove();
            }
        }
    }
    else if ( EventPause* onPause = DYNAMIC_CAST(_event,EventPause) )
    {
        if (onPause->getPause())
            disable();
        else
            enable();
    }
    else if ( EventTimeFactor* tf = DYNAMIC_CAST(_event,EventTimeFactor) )
    {
        if (tf->getActorTimeFactor()>MTH_EPSILON)
            setTimeFactor(tf->getActorTimeFactor());
	}
	else if ( EventDestroySpawned* evt = DYNAMIC_CAST(_event,EventDestroySpawned) )
	{
		if (isSerializable())
			disable();
		else
			requestDestruction();
	}

    LinkComponent* linkComponent;

    if ( m_transferEventsToLinkChildren )
    {
        linkComponent = GetComponent<LinkComponent>();
    }
    else
    {
        linkComponent = NULL;
    }

    u32 registeredEventsSize = m_registeredEvents.size();
    bbool sentToLink = bfalse;

    if ( registeredEventsSize )
    {
        RegisteredEvent* pregisteredEvent = &m_registeredEvents[0];

        for ( u32 i = 0; i < registeredEventsSize; i++, pregisteredEvent++ )
        {
            if ( _event->IsClassCRC(pregisteredEvent->m_eventClass->m_crc) )
            {
                const u32 listenersSize = pregisteredEvent->m_listeners.size();
                if(listenersSize)
                {
                    ITF_ASSERT(pregisteredEvent);
                    IEventListener **ppListener = &pregisteredEvent->m_listeners[0];

                    for ( u32 j = 0; j < listenersSize; j++, ppListener++ )
                    {
                        (*ppListener)->onEvent(_event);
                        
                        if ( *ppListener == linkComponent )
                        {
                            sentToLink = btrue;
                        }
                    }

                    // don't break because components could have registered at different inheritance levels
                }
            }
        }
    }

    if (EventQueryPosition* onQueryPos = DYNAMIC_CAST(_event,EventQueryPosition))
    {
        if (onQueryPos->getRequestData() && !onQueryPos->isSet())
        {
            onQueryPos->setPos(getPos());
            onQueryPos->setAngle(getAngle());
            onQueryPos->setScale(getScale());
        }
    }

    if ( !sentToLink && m_transferEventsToLinkChildren && linkComponent )
    {
        linkComponent->sendEventToChildren(_event);
    }
}

void Actor::setTransferEventToLinkChildren( bbool _val )
{
    if ( m_transferEventsToLinkChildren != static_cast<u32>(_val) )
    {
        if ( _val )
        {
            LinkComponent* linkComponent = GetComponent<LinkComponent>();

            if ( linkComponent )
            {
                m_transferEventsToLinkChildren = btrue;
            }
            else
            {
                ITF_ASSERT_MSG(0,"Trying to send events to the linked children but there is no link component");
            }
        }
        else
        {
            m_transferEventsToLinkChildren = bfalse;
        }
    }
}

void Actor::saveCheckpointData()
{
#ifndef ITF_FINAL
    if ( !hasDataError() )
#endif
    {
        ActorsManager::ActorDataContainer* dataContainer = ACTORSMANAGER->getSerializeDataActor(this);
        if (dataContainer)
        {
            if (dataContainer->m_checkpoint)
                dataContainer->m_checkpoint->rewindForWriting();
            else
                dataContainer->m_checkpoint = newAlloc(mId_SavegameManager,ArchiveMemory);

            CSerializerObjectBinary serializeBin;
            serializeBin.Init(dataContainer->m_checkpoint);
            Serialize(&serializeBin,ESerialize_Checkpoint_Save);
        }
    }
}


void Actor::postLoadCheckpointData()
{
#ifndef ITF_FINAL
    if ( !hasDataError() )
#endif
    {
        ActorsManager::ActorDataContainer* dataContainer = ACTORSMANAGER->getSerializeDataActor(this);
        if (dataContainer && dataContainer->m_checkpoint && dataContainer->m_checkpoint->getSize())
        {
            dataContainer->m_checkpoint->rewindForReading();
            CSerializerObjectBinary serializeBin;
            serializeBin.Init(dataContainer->m_checkpoint);
            Serialize(&serializeBin,ESerialize_Checkpoint_Load);
        }
        getChildrenBindHandler().growAABB();
        onLoadedCheckpoint(); // call onLoadedCheckpoint() outside of the if() statement for newly created objects
    }
}

void Actor::onLoadedCheckpoint()
{
#ifndef ITF_FINAL
    if(!hasDataError())
#endif
    {
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* pComponent = m_components[it];
            ITF_ASSERT(pComponent);

            pComponent->onCheckpointLoaded();
        }
    }
}

void Actor::onLoadedPersistentData()
{
#ifndef ITF_FINAL
    if(!hasDataError())
#endif
    {
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* pComponent = m_components[it];
            ITF_ASSERT(pComponent);

            pComponent->onPersistentLoaded();
        }
    }
}

void Actor::SerializePersistent( CSerializerObject* serializer, u32 flags )
{
#ifndef ITF_FINAL
    if(serializer->m_traceSerialization)
    {
        LOG("-- %c Actor 0x%x [%f %s] '%s' ID0x%x",
             flags&ESerialize_Data_Load ? '<' : '>',
             this,
             getLocalInitialPos2D().x(),
             getTemplatePath().getBasenameWithoutExtension().cStr(),
             getUserFriendly().cStr(),
             computePersistentID()
            );
    }
#endif

    Super::SerializePersistent(serializer, flags);
    const ComponentsVector& components = GetAllComponents();
    for(ComponentsVector::const_iterator itComponent = components.cbegin(); itComponent != components.cend(); ++itComponent)
    {
        ActorComponent* pComponent = *itComponent;
#if !defined(ITF_FINAL) && defined(ITF_ENABLE_RTTI_CRC_CHECK)
        if(serializer->m_traceSerialization)
        {
            LOG("- Component %s", pComponent->GetObjectClassName());
        }
#endif

        pComponent->SerializePersistent(serializer, flags);
    }
}

void Actor::fillHotReloadData(ActorHotReload &_reloadInfo, bbool _useTwoStepHotreload, bbool _skipInstanceDataFile )
{
    _reloadInfo.m_actorPtr          = this;
    _reloadInfo.m_enabled           = isEnabled();
    _reloadInfo.m_useTwoStepHotreload = _useTwoStepHotreload;
    _reloadInfo.m_skipInstanceDataFile = _skipInstanceDataFile;
}

void Actor::hotReload( ActorHotReload& _reloadInfo, HotReloadType _hotReloadType )
{
    ITF_ASSERT(_reloadInfo.m_actorPtr == this);

    hotReloadReset(bfalse);

#if !defined(ITF_FINAL)
    WarningManager::clearWarningsForObject(this);
#endif

    _reloadInfo.m_childrenBeforeClear = m_childrenHandler.getChildren();

    // If the object was saved in the scene it will have some data to load from it
    if ( isSerializable() )
    {
        ActorsManager::ActorDataContainer* dataContainer = ACTORSMANAGER->getSerializeDataActor(this);

        if ( dataContainer )
        {
            // Load data
            ArchiveMemory* arc = dataContainer->m_instance;

            // Load the instance data
            ITF_ASSERT(arc);
            arc->rewindForReading();
            CSerializerObjectBinary instanceSerializer;
            instanceSerializer.Init(arc);
            // Apply the data saved in the scene
            Serialize(&instanceSerializer,ESerialize_Data_Load);
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    // If the object has an act file, load the data from it (these values will be locked, they override the scene values)
    if ( !getInstanceDataFile().isEmpty() && !_reloadInfo.m_useTwoStepHotreload )
    {
        loadInstanceData(bfalse);
    }
#endif

    // If we don't want to do the reload in two steps we finalize it now
    if (!_reloadInfo.m_useTwoStepHotreload)
        finalizeReload( _reloadInfo.m_enabled, _hotReloadType );
}

void Actor::replaceReinitDataByCurrent()
{
    if ( !isSerializable() )
    {
        return;
    }

    if(ACTORSMANAGER->isActorRegistered(this))
    {
        ACTORSMANAGER->unregisterSerializeDataActor(this);
        ACTORSMANAGER->registerSerializeDataActor(this);
    }

    m_childrenHandler.replaceReinitDataByCurrent();

    SubSceneActor* group = DYNAMIC_CAST(this, SubSceneActor);
    if(group)
    {
        Scene *scene = group->getSubScene();
        PickableList content = scene->getPickableList();
        for(u32 iC = 0; iC < content.size(); ++iC)
        {
            content[iC]->replaceReinitDataByCurrent();
        }
    }
}

#ifndef ITF_FINAL
bbool Actor::validate()
{
    bbool isValid = Super::validate();
    // TODO GG -> Change to String8
    String8 errorMsg;
#ifdef ITF_SUPPORT_EDITOR
    errorMsg = getDataErrorLabel();
#endif //ITF_SUPPORT_EDITOR

    if (!isValid)
    {
        ITF_WARNING_CATEGORY(GPP,this, bfalse, errorMsg.cStr());
    }
    else if (!m_templatePickable)
    {
        String8 error;
        error.setTextFormat("actor with missing template %s",getTemplatePath().toString8().cStr());
        ITF_WARNING_CATEGORY(GPP,this, bfalse, error.cStr());

        errorMsg += error;
        isValid = bfalse;
    }
    else
    {
        bbool subValidation = btrue;
        String8 errorLabel;
        
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* pComponent = m_components[it];

            if(!pComponent)
            {
                ITF_WARNING_CATEGORY(GPP,this, pComponent!=NULL, "Actor has a NULL component : '%s'", getTemplatePath().toString8().cStr());
                continue;
            }
            bbool isComponentValid = btrue;

            pComponent->validate(isComponentValid);

            if (!isComponentValid && isValid)
            {
                if(errorLabel.getLen() == 0)
                    errorLabel = "Invalid component(s): ";
                else
                    errorLabel += ", ";
                errorLabel += StringID(pComponent->GetObjectClassCRC()).getDebugString();
                const ActorComponent_Template* tpl = pComponent->getTemplate();
                if(tpl)
                {
                    const Actor_Template* actTpl = tpl->getActorTemplate();
                    if(actTpl)
                    {
                        const Path& actTplPath = actTpl->getFile();
                        ITF_WARNING_CATEGORY(LD,this, isComponentValid, "Invalid component in file : %s", actTplPath.toString8().cStr());                                               
                    }
                    
                }
                
                subValidation = bfalse;
            }
        }

        if(!subValidation)
        {
            errorMsg += errorLabel;
            isValid = bfalse;
        }
    }

    if ( !isValid )
    {
        setDataError(errorMsg);
    }

    return isValid;
}
#endif //!ITF_FINAL


void Actor::fixComponentsFromTemplate( const Actor_Template* _template )
{
    bool changed = false;

    ComponentsVector::iterator itComponents = m_components.begin();

    // Remove NULL components
    while ( itComponents != m_components.end() )
    {
        if ( *itComponents == NULL )
        {
            itComponents = m_components.erase(itComponents);
            changed = btrue;
        }
        else
        {
            ++itComponents;
        }
    }

    // Fix any difference between the instance data and the template
    // In the final data (cooked bundles) this should not happen, so we don't do it
    // First delete all the component duplicates
    const Actor_Template::ComponentList& components = static_cast<const Actor_Template*>(_template)->getComponents();
    u32 numTemplateComponents = components.size();

    for ( u32 templateIndex = 0; templateIndex < numTemplateComponents; templateIndex++ )
    {
        const ActorComponent_Template* compTemplate = components[templateIndex];
        u32 occurrences = 0;

        ComponentsVector::iterator it = m_components.begin();

        while ( it != m_components.end() )
        {
            ActorComponent* component = *it;

            if ( component && compTemplate && component->GetObjectClassCRC() == compTemplate->getComponentClassCRC() )
            {
                occurrences++;

                if ( occurrences > 1 )
                {
                    // Delete duplicates
                    SF_DEL(component);
                    it = m_components.erase(it);
                    changed = true;
                }
                else
                {
                    ++it;
                }
            }
            else
            {
                ++it;
            }
        }
    }

    // Then delete all the components that are not present on the template
    ComponentsVector::iterator it = m_components.begin();

    while ( it != m_components.end() )
    {
        ActorComponent* component = *it;
        bbool found = bfalse;

        for ( u32 templateIndex = 0; templateIndex < numTemplateComponents; templateIndex++ )
        {
            const ActorComponent_Template* compTemplate = components[templateIndex];
            if ( component && compTemplate && component->GetObjectClassCRC() == compTemplate->getComponentClassCRC() )
            {
                found = btrue;
                break;
            }
        }

        if ( !found )
        {
#if defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_COOKING)
            // Detect when the .act contains a component that is no more in the component template list
            if(!m_instanceDataFile.isEmpty() && m_componentBeforeInstanceData.find(component->GetObjectClassCRC()) == -1)
            {
                ITF_WARNING_CATEGORY(GPP, NULL, 0, "Edit: '%s' and remove the component: '%s'", m_instanceDataFile.toString8().cStr(), component->GetObjectClassName());
            }
            else
#endif // ITF_SUPPORT_EDITOR || ITF_SUPPORT_COOKING
            {
                // changed = true; // don't set dirty for this reason because each time the user move a component he is asked to resave all scene using the template (and we don't want it)
            }

            SF_DEL(component);
            it = m_components.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Finally arrange the components order to match the one in the template
    for ( u32 templateIndex = 0; templateIndex < numTemplateComponents; templateIndex++ )
    {
        const ActorComponent_Template* compTemplate = components[templateIndex];
        if(!compTemplate)
            continue;
        StringID::StringIdValueType componentClassCRC = compTemplate->getComponentClassCRC();
        bbool found = bfalse;

        for ( u32 componentIndex = templateIndex; componentIndex < m_components.size(); componentIndex++ )
        {
            ActorComponent* comp = m_components[componentIndex];

            if ( comp && comp->GetObjectClassCRC() == componentClassCRC )
            {
                found = btrue;

                if ( componentIndex != templateIndex )
                {
                    if ( templateIndex >= m_components.size() )
                    {
                        m_components.resize(templateIndex+1);
                    }

                    ActorComponent* prevComp = m_components[templateIndex];
                    m_components[templateIndex] = comp;
                    m_components[componentIndex] = prevComp;
                    // changed = true; // don't set dirty for this reason because each time the user move a component he is asked to resave all scene using the template (and we don't want it)
                }

                break;
            }
        }

        if (!found)
        {
            ActorComponent* comp = compTemplate->createComponent();

            if ( templateIndex >= m_components.size() )
            {
                m_components.push_back(comp);
                // changed = true;  // don't set dirty for this reason because each time the user add a component he is asked to resave all scene using the template (and we don't want it)
            }
            else
            {
                u32 counter = 0;
                for ( ComponentsVector::iterator it = m_components.begin(); it != m_components.end(); ++it, counter++ )
                {
                    if ( counter == templateIndex )
                    {
                        m_components.insert(it,comp);
                        // changed = true;
                        break;
                    }
                }
            }
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    if(changed)
        setDirty(btrue);
#endif // ITF_SUPPORT_EDITOR
}


void Actor::createParentBind( bbool _temporaryBind,
                              Actor* _parent, 
                              Bind::Type _type, 
                              u32 _typeData, 
                              bbool _useParentFlip /*=btrue*/, 
                              bbool _useParentScale/*=bfalse*/, 
                              bbool _useRelativeZ /*= bfalse*/,
                              bbool _removeWithParent /*=bfalse*/ )
{
    ITF_ASSERT( _parent != NULL );
    if ( _parent == NULL )
    {
        return;
    }

    Bind* bind = newAlloc(mId_Gameplay, Bind);

    bind->m_temporaryBind      = _temporaryBind;
    bind->m_type               = _type;
    bind->m_typeData           = _typeData;
    bind->m_useParentFlip      = _useParentFlip;
    bind->m_useParentScale     = _useParentScale;
    bind->m_removeWithParent   = _removeWithParent;

    Vec3d localPos;
    f32 localAngle;
    if( _parent->getChildrenBindHandler().computeLocalCoordinates(bind, getPos(), getAngle(), localPos, localAngle))
    {
        bind->setInitialAngleOffset(localAngle);
        bind->setAngleOffset(localAngle);

        bind->setInitialPositionOffset(localPos);
        bind->setPosOffset(localPos);

        SceneObjectPathUtils::getRelativePathFromObject(this, _parent, bind->m_parent);

        if ( !bind->m_parent.isValid() )
        {
            SceneObjectPathUtils::getAbsolutePathFromObject( _parent, bind->m_parent );
        }

        setParentBind(bind);
        _parent->getChildrenBindHandler().updateWorldCoordinates(this, bind);
        storeCurrentTransformationToInitial();
    }
    else
    {
        ITF_ASSERT(0);

        SF_DEL(bind);
    }
}

void Actor::setParentBind( Bind* _newParent, bbool _isInitial /*= bfalse*/)
{
    unbindFromParent();

    if( _isInitial )
        m_pParentBind_Initial = _newParent;

    m_pParentBind = _newParent;

    if(_newParent)
    {
        Pickable* pPickable = NULL;
        const ObjectPath& parentPath = _newParent->m_parent;
        if(parentPath.getIsAbsolute())
        {
            pPickable = SceneObjectPathUtils::getObjectFromAbsolutePath(parentPath);
        }
        else
        {
            pPickable = SceneObjectPathUtils::getObjectFromRelativePath(this, parentPath);
        }

        ITF_ASSERT_CRASH( pPickable != NULL , "Can't find parent object !");

        if ( pPickable != NULL )
        {
            if ( Actor* pParent = DYNAMIC_CAST(pPickable,Actor) )
                //if(Actor* pParent = static_cast<Actor*>(SceneObjectPathUtils::getObjectFromRelativePath(this, _newParent->m_parent)))
            {
                ITF_ASSERT_CRASH(pParent != this , "Binding to 'this' !!!");
                pParent->getChildrenBindHandler().addChild(this);
            }
        }
    }
}

void Actor::unbindFromParent()
{
    // Unbind from parent
    if(m_pParentBind)
    {
        if(Actor* pParent = static_cast<Actor*>(m_pParentBind->m_runtimeParent.getObject()))
        {
            pParent->getChildrenBindHandler().removeChild(this);
        }
    }

    if( m_pParentBind_Initial )
    {
        SF_DEL(m_pParentBind_Initial);
        m_pParentBind = NULL;
    }
    else
        SF_DEL(m_pParentBind);
}

void Actor::clearChildBinds(bbool _onlyDynamic)
{
    if(_onlyDynamic)
    {
        ObjectRefList children = m_childrenHandler.getChildren();
        const u32 uChildCount = children.size();
        for(u32 iChild = 0; iChild < uChildCount; ++iChild)
        {
            if(Actor* child = static_cast<Actor*>(children[iChild].getObject()))
            {
                Bind* bind = child->getParentBind();
                if(bind->m_temporaryBind)
                {
                    getChildrenBindHandler().removeChild(child);
                }
            }
        }
    }
    else
    {
        m_childrenHandler.clear();
    }
}

void Actor::updateWorldCoordinatesFromBoundParent()
{
    if (m_pParentBind)
    {
        Actor* parent = getParent().getActor();
        ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
        if ( parent )
        {
            parent->getChildrenBindHandler().updateWorldCoordinates(this, m_pParentBind);
        }
    }
}

void Actor::updateWorldCoordinatesForBoundChildren(bbool _initPos)
{
    const ObjectRefList& bindedChildren = m_childrenHandler.getChildren();
    for(u32 i = 0; i < bindedChildren.size(); ++i)
    {
        Actor* pChild = static_cast<Actor*>(bindedChildren[i].getObject());

        m_childrenHandler.updateWorldCoordinates(pChild, pChild->getParentBind());

        if (_initPos)
        {
            pChild->setWorldInitialPos(pChild->getPos(), btrue);
            pChild->setWorldInitialRot(pChild->getAngle(), btrue);
        }
    }
}

void Actor::onScaleChanged(const Vec2d& _prevScale)
{
    Super::onScaleChanged(_prevScale);

#ifndef ITF_FINAL
    if(!hasDataError())
#endif
    {
        Vec2d newScale = getScale();
        const u32 numComponents = m_components.size();

        for ( u32 i = 0; i < numComponents; i++ )
        {
            m_components[i]->onScaleChanged(_prevScale,newScale);
        }
    }
}

void Actor::onDepthChanged( f32 _prevDepth )
{
    Super::onDepthChanged(_prevDepth);

    if( m_templatePickable
#ifndef ITF_FINAL
        && !hasDataError()
#endif
        )
    {
        f32 depth = getDepth();
        const u32 numComponents = m_components.size();

        for ( u32 i = 0; i < numComponents; i++ )
        {
            m_components[i]->onDepthChanged(_prevDepth,depth);
        }
    }
}

void Actor::onFlipChanged()
{
    Super::onFlipChanged();

#ifndef ITF_FINAL
    if(!hasDataError())
#endif
    {
        const u32 numComponents = m_components.size();

        for ( u32 i = 0; i < numComponents; i++ )
        {
            m_components[i]->onFlipChanged();
        }
    }
}

void Actor::onPosChanged( const Vec3d& _prevPos )
{
    Super::onPosChanged(_prevPos);

    if( m_templatePickable
#ifndef ITF_FINAL
        && !hasDataError()
#endif
        )
    {
        const u32 numComponents = m_components.size();
        for ( u32 i = 0; i < numComponents; i++ )
        {
            m_components[i]->onPosChanged(_prevPos, m_pos);
        }
    }
}

void Actor::onAnglePosChanged(f32 _prevAngle, const Vec3d& _prevPos)
{
    Super::onAnglePosChanged(_prevAngle, _prevPos);

    if( m_templatePickable
#ifndef ITF_FINAL
        && !hasDataError()
#endif
        )
    {
        const u32 numComponents = m_components.size();
        for ( u32 i = 0; i < numComponents; i++ )
        {
            m_components[i]->onPosChanged(_prevPos, m_pos);
        }
    }
}

Vec3d Actor::getBoundWorldInitialPos(bbool _initialFromCurrentParent) const
{
    if(_initialFromCurrentParent)
    {
        if ( m_pParentBind )
        {
            Actor* parent = getParent().getActor();
            ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
            if ( parent )
            {
                Vec3d worldPos;
                f32 worldAngle;
                if(parent->getChildrenBindHandler().computeWorldCoordinates(
                    m_pParentBind, m_pParentBind->getInitialPosOffset(), m_pParentBind->getInitialAngleOffset(),
                    worldPos, worldAngle))
                {
                    return worldPos;
                }
                // See the comment below
                //else
                //{
                //    ITF_ASSERT_MSG(0, "Can't compute world initial position");
                //}
            }
        }

        // same as Pickable::getWorldInitialPos except we use the scene's current pos instead of initial pos
        Scene* myScene = getScene();
        Vec2d myPos = getLocalInitialPos().truncateTo2D();
        f32 myZ = getLocalInitialZ();

        if ( myScene )
        {
            SubSceneActor* subActor = myScene->getSubSceneActorHolder();

            if ( subActor )
            {
                Vec3d subScenePos = subActor->getPos();
                f32 subSceneRot = subActor->getAngle();

                if(subActor->getIsFlipped())
                    myPos.x() = -myPos.x();

                myPos.x() *= subActor->getScale().x();
                myPos.y() *= subActor->getScale().y();

                myPos = myPos.Rotate(subSceneRot) + subScenePos.truncateTo2D();
                myZ += subScenePos.z();
            }
            else if ( !getIs2D() )
            {
                World* myWorld = myScene->getWorld();
                const Vec3d& worldPos = myWorld->getPos();
                const f32 worldAngle = myWorld->getAngle();
                const Vec2d& worldScale = myWorld->getScale();

                if (myWorld->getIsFlipped())
                    myPos.x() = -myPos.x();

                myPos.x() *= worldScale.x();
                myPos.y() *= worldScale.y();

                myPos = myPos.Rotate(worldAngle) + worldPos.truncateTo2D();
                myZ += worldPos.z();
            }
        }

        return myPos.to3d(myZ);
    }
    else
    {
        if ( m_pParentBind )
        {
            Actor* parent = getParent().getActor();
            ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
            if ( parent )
            {
                Vec3d worldPos;
                f32 worldAngle;
                if(parent->getChildrenBindHandler().computeWorldInitialCoordinates(
                    m_pParentBind, m_pParentBind->getInitialPosOffset(), m_pParentBind->getInitialAngleOffset(),
                    worldPos, worldAngle))
                {
                    return worldPos;
                }
                // onActorLoaded this will always fail if we are bound on a bone, so there is no point asserting, we have to assume that it can happen
                // in the bad case, we return the worldinitialpos, which should have been set when the binding was done, so it should be a good one
                //else
                //{
                    //ITF_ASSERT_MSG(0, "Can't compute world initial position");
                //}
            }
        }

        return getWorldInitialPos();
    }
}

void Actor::setBoundWorldInitialPos( const Vec3d& _pos, bbool _fromCurrentParentTransform )
{
    if ( m_pParentBind )
    {
        Actor* parent = getParent().getActor();
        ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
        if ( parent )
        {
            Vec3d localPos;
            f32 localAngle;
            const bbool apply = _fromCurrentParentTransform ?
                parent->getChildrenBindHandler().computeLocalCoordinates(m_pParentBind, _pos, getAngle(), localPos, localAngle) :
                parent->getChildrenBindHandler().computeLocalInitialCoordinates(m_pParentBind, _pos, getAngle(), localPos, localAngle);

            if(apply)
            {
                m_pParentBind->setInitialPositionOffset(localPos);
            }
            else
            {
                ITF_ASSERT_MSG(0, "Can't compute local initial position");
            }
        }
    }
    else
    {
        setWorldInitialPos(_pos, _fromCurrentParentTransform);
    }
}

void Actor::setBoundWorldPos( const Vec3d& _pos )
{
    if ( m_pParentBind )
    {
        Actor* parent = getParent().getActor();
        ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
        if ( parent )
        {
            Vec3d localPos;
            f32 localAngle;
            if(parent->getChildrenBindHandler().computeLocalCoordinates(m_pParentBind, _pos, getAngle(), localPos, localAngle))
            {
                m_pParentBind->setPosOffset(localPos);
            }
            else
            {
                ITF_ASSERT_MSG(0, "Can't compute local initial position");
            }
        }
    }
    
    setPos(_pos);
}

f32 Actor::getBoundWorldInitialAngle(bbool _initialFromCurrentParent) const
{
    if(_initialFromCurrentParent)
    {
        if ( m_pParentBind )
        {
            Actor* parent = getParent().getActor();
            ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
            if ( parent )
            {
                Vec3d worldPos;
                f32 worldAngle;
                if(parent->getChildrenBindHandler().computeWorldCoordinates(
                    m_pParentBind, m_pParentBind->getInitialPosOffset(), m_pParentBind->getInitialAngleOffset(),
                    worldPos, worldAngle))
                {
                    return worldAngle;
                }
            }
        }

        // same as Pickable::getWorldInitialRot except we use the scene's current angle instead of initial angle
        Scene* myScene = getScene();
        f32 myRot = getLocalInitialRot();

        if ( myScene )
        {
            SubSceneActor* subActor = myScene->getSubSceneActorHolder();

            if ( subActor )
            {
                if (subActor->getIsFlipped())
                    myRot = -myRot;

                myRot += subActor->getAngle();
            }
            else
            {
                World* myWorld = myScene->getWorld();

                if (myWorld->getIsFlipped())
                    myRot = -myRot;

                myRot += myWorld->getAngle();
            }
        }

        return myRot;
    }
    else
    {
        if ( m_pParentBind )
        {
            Actor* parent = getParent().getActor();
            ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
            if ( parent )
            {
                Vec3d worldPos;
                f32 worldAngle;
                if(parent->getChildrenBindHandler().computeWorldInitialCoordinates(
                    m_pParentBind, m_pParentBind->getInitialPosOffset(), m_pParentBind->getInitialAngleOffset(),
                    worldPos, worldAngle))
                {
                    return worldAngle;
                }
            }
        }

        return getWorldInitialRot();
    }
}

Vec3d Actor::getBoundLocalInitialPos() const
{
    if ( m_pParentBind )
    {
        Vec3d offset = m_pParentBind->getInitialPosOffset();
        return offset;
    }

    return getLocalInitialPos();
}

const Vec2d& Actor::getBoundLocalInitial2DPos() const
{
    if ( m_pParentBind )
    {
        return m_pParentBind->getInitialPosOffset().truncateTo2D();
    }

    return m_initialPos;
}

Vec3d Actor::getBoundLocalPos() const
{
    if ( m_pParentBind )
    {
        return m_pParentBind->getPosOffset();
    }

    return getLocalPos();
}

Vec2d Actor::getBoundLocal2DPos() const
{
    if ( m_pParentBind )
    {
        return m_pParentBind->getPosOffset().truncateTo2D();
    }

    return getLocal2DPos();
}

void Actor::setBoundLocalPos( const Vec3d& _pos )
{
    if ( m_pParentBind )
    {
        m_pParentBind->setPosOffset(_pos);
        updateWorldCoordinatesFromBoundParent();
    }
    else
    {
        setLocalPos(_pos);
    }
}

void Actor::setBoundLocal2DPos( const Vec2d& _pos )
{
    if ( m_pParentBind )
    {
        m_pParentBind->set2DPosOffset(_pos);
        updateWorldCoordinatesFromBoundParent();
    }
    else
    {
        setLocal2DPos(_pos);
    }
}

f32 Actor::getBoundLocalInitialAngle() const
{
    if ( m_pParentBind )
    {
        return m_pParentBind->getInitialAngleOffset();
    }

    return getLocalInitialRot();
}

f32 Actor::getBoundLocalAngle() const
{
    if ( m_pParentBind )
    {
        return m_pParentBind->getAngleOffset();
    }

    return getLocalAngle();
}

void Actor::setBoundWorldInitialAngle(f32 _angle, bbool _fromCurrentParentTransform)
{
    if ( m_pParentBind )
    {
        Actor* parent = getParent().getActor();
        ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
        if ( parent )
        {
            Vec3d localPos;
            f32 localAngle;
            const bbool apply = _fromCurrentParentTransform ?
                parent->getChildrenBindHandler().computeLocalCoordinates(m_pParentBind, getPos(), _angle, localPos, localAngle) :
                parent->getChildrenBindHandler().computeLocalInitialCoordinates(m_pParentBind, getPos(), _angle, localPos, localAngle);

            if(apply)
            {
                m_pParentBind->setInitialAngleOffset(localAngle);
            }
            else
            {
                ITF_ASSERT_MSG(0, "Can't compute local initial angle");
            }
        }
    }
    else
    {
        setWorldInitialRot(_angle, _fromCurrentParentTransform);
    }
}


void Actor::setBoundWorldAngle(f32 _angle)
{
    if ( m_pParentBind )
    {
        Actor* parent = getParent().getActor();
        ITF_ASSERT_MSG(parent, "When called from the gameplay thread, we should have a parent; if we can be called from other threads, then this assert is unnecessary.");
        if ( parent )
        {
            Vec3d localPos;
            f32 localAngle;
            if(parent->getChildrenBindHandler().computeLocalCoordinates(m_pParentBind, getPos(), _angle, localPos, localAngle))
            {
                m_pParentBind->setAngleOffset(localAngle);
            }
            else
            {
                ITF_ASSERT_MSG(0, "Can't compute local initial angle");
            }
        }
    }
    
    setAngle(_angle);
}

void Actor::setBoundLocalAngle( f32 _angle )
{
    if ( m_pParentBind )
    {
        m_pParentBind->setAngleOffset(_angle);
    }
    else
    {
        setLocalAngle(_angle);
    }
}

void Actor::computeInitialWorldTransformFromSerializedData( Vec3d& _pos, f32& _angle, bbool& _flipped, Vec2d& _scale ) const
{
    if ( m_pParentBind )
    {
        Actor* parent = getParent().getActor();

        if ( parent )
        {
            Vec3d worldPos;
            f32 worldAngle;
            Vec2d scale;
            
            if( parent->getChildrenBindHandler().computeWorldCoordinates(m_pParentBind,
                                                                         m_pParentBind->getInitialPosOffset(),
                                                                         m_pParentBind->getInitialAngleOffset(),
                                                                         worldPos, worldAngle))
            {
                _pos = worldPos;
                _angle = worldAngle;
                _scale = getLocalInitialScale();
                _flipped = getLocalInitialFlip();

                Scene* myScene = getScene();

                if ( myScene )
                {
                    World* world = myScene->getWorld();
                    SubSceneActor* subActor = myScene->getSubSceneActorHolder();

                    if ( subActor )
                    {
                        Vec3d pos;
                        f32 angle;
                        Vec2d scale;
                        bbool isFlipped;

                        subActor->computeInitialWorldTransformFromSerializedData(pos,angle,isFlipped,scale);

                        if(isFlipped)
                        {
                            _flipped = !_flipped;
                        }

                        _scale *= scale;
                    }
                    else if ( world )
                    {
                        if (world->getIsFlipped())
                        {
                            _flipped = !_flipped;
                        }

                        _scale *= world->getScale();
                    }
                }

                return;
            }
        }
    }

    Super::computeInitialWorldTransformFromSerializedData(_pos,_angle,_flipped,_scale);
}

void Actor::getParentWorldTransform( Vec3d& _parentPos, f32& _parentAngle, bbool& _flipped, Vec2d& _scale ) const
{
    if ( m_pParentBind )
    {
        const Actor* parent = getParent().getActor();
        ITF_ASSERT(parent);
        if (parent)
        {
            parent->getChildrenBindHandler().getTransform(m_pParentBind, _parentPos, _parentAngle);
            return;
        }
    }

    Super::getParentWorldTransform(_parentPos,_parentAngle,_flipped,_scale);
}

void Actor::onApplyGhost(class GhostRecorderInfoBase* _ghostRecorderInfo)
{
    for ( u32 it = 0; it < m_components.size(); ++it )
    {
        m_components[it]->onApplyGhost(_ghostRecorderInfo);
    }
}

void Actor::onRecordGhost(GhostRecorderInfoBase* _ghostRecorderInfo)
{
    _ghostRecorderInfo->m_Pos = getPos();

    for ( u32 it = 0; it < m_components.size(); ++it )
    {
        m_components[it]->onRecordGhost(_ghostRecorderInfo);
    }
}

#ifndef ITF_FINAL
u32 Actor::computeNbBones() const
{
    u32 Bones = U32_INVALID;
    AnimLightComponent* pAnimLightComponent =  GetComponent<AnimLightComponent>();
    if (pAnimLightComponent!=0)
    {
        Bones = pAnimLightComponent->getBoneCount();
    }

    if (Bones !=U32_INVALID)
        m_nbBones = Bones;
    else
        m_nbBones = 0;

    return m_nbBones;
}
#endif //ITF_FINAL

const ResourceContainer* Actor::getTemplateResourceContainer() const
{
    return getTemplate()->getResourceContainer();
}

#ifdef ITF_SUPPORT_EDITOR
Pickable* Actor::clone( Scene* _pDstScene, bbool _callOnLoaded ) const
{
    ITF_ASSERT_MSG(_pDstScene, "cloning actor inside an empy scene");

    Actor* newActor = static_cast<Actor*>(CreateNewObject());

    BaseObjectClone(const_cast<Actor*>(this), newActor);

    newActor->onCreate();

    if (_pDstScene)
    {
        if(_pDstScene->getPickableFromUserFriendly(newActor->getUserFriendly()))
            newActor->generateUniqueName(_pDstScene);
        _pDstScene->registerPickable(newActor);

        Vec3d worldPos = getWorldInitialPos();
        Vec2d worldScale = getWorldInitialScale();
        f32 worldRot = getWorldInitialRot();

        newActor->setWorldInitialPos(worldPos, btrue);
        newActor->setWorldInitialRot(worldRot, btrue);
        newActor->setWorldInitialScale(worldScale, btrue);

        newActor->setPos(getPos());
        newActor->setScale(getScale());
        newActor->setAngle(getAngle());
    }
    else if(_callOnLoaded)
    {
        newActor->onLoaded(HotReloadType_Default);
        newActor->onFinalizeLoad(bfalse);
    }

    return newActor;
}

bbool useFlipBefore = bfalse;

void Actor::onPrePropertyChange()
{
    Super::onPrePropertyChange();

    u32 numComponents = m_components.size();
    for ( u32 it = 0; it < numComponents; ++it )
    {
        ActorComponent* component = m_components[it];
        component->onPrePropertyChange();
    }

    if (m_pParentBind)
        useFlipBefore = m_pParentBind->m_useParentFlip;
}

void Actor::onPostPropertyChange()
{
    Super::onPostPropertyChange();

    u32 numComponents = m_components.size();
    for ( u32 it = 0; it < numComponents; ++it )
    {
        ActorComponent* component = m_components[it];
        component->onPostPropertyChange();
    }

    // prevent the tool from adding new components...
    u32 i = 0;

    while ( i < numComponents )
    {
        ActorComponent* c = m_components[i];

        if ( c->GetActor() == NULL )
        {
            m_components.removeAtUnordered(i);
            numComponents--;
        }
        else
        {
            i++;
        }
    }

    if ( !m_templatePickable || m_templatePath != m_templatePickable->getFile() )
    {
        hotReloadReset(btrue);
        setHotReloadAfterModif();
    }

    // This code assume that the initial pos of the actor may be edited by the propertiesEditor
    // But the binding position offset may not.
    if(m_pParentBind_Initial && m_pParentBind)
    {
        if (useFlipBefore && !m_pParentBind->m_useParentFlip)
        {
            setWorldInitialRot(getBoundWorldInitialAngle(true), true );
            setWorldInitialPos(getBoundWorldInitialPos(true), true );
        }
        else if (!useFlipBefore && m_pParentBind->m_useParentFlip)
        {
            setWorldInitialRot(getBoundWorldInitialAngle(false), true );
            setWorldInitialPos(getBoundWorldInitialPos(false), true );
        }

        // Set runtime initial offset from initial pos
        setBoundWorldInitialPos(getWorldInitialPos(), bfalse);
        setBoundWorldInitialAngle(getWorldInitialRot(), bfalse);

        // Store inside serialized binding data
        m_pParentBind_Initial->setInitialPositionOffset(m_pParentBind->getInitialPosOffset());
        m_pParentBind_Initial->setInitialAngleOffset(m_pParentBind->getInitialAngleOffset());

        // Set runtime current offset
        m_pParentBind->setPosOffset(m_pParentBind->getInitialPosOffset());
        m_pParentBind->setAngleOffset(m_pParentBind->getInitialAngleOffset());

        // Recompute new coordinates
        if(Actor* parent = static_cast<Actor*>(m_pParentBind->m_runtimeParent.getObject()))
        {
            parent->getChildrenBindHandler().updateWorldCoordinates(this, m_pParentBind, btrue);
        }
    }
}

void Actor::onPreEditorSave()
{
    Super::onPreEditorSave();

#ifndef ITF_FINAL
    if(!hasDataError())
#endif
    {
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* const pComponent = m_components[it];
            ITF_ASSERT(pComponent);
            pComponent->onPreEditorSave();
        }
    }
}

void Actor::onPostEditorSave()
{
    Super::onPostEditorSave();

#ifndef ITF_FINAL
    if(!hasDataError())
#endif
    {
        const u32 uSize = m_components.size();
        for ( u32 it = 0; it < uSize; ++it )
        {
            ActorComponent* const pComponent = m_components[it];
            ITF_ASSERT(pComponent);
            pComponent->onPostEditorSave();
        }
    }
}

void Actor::drawEdit( ActorDrawEditInterface* drawInterface, u32 _flags)
{
    if ( !isPhysicalReady() || hasDataError())
        return;

    u32 numComponents = m_components.size();

    for ( u32 it = 0; it < numComponents; ++it )
    {
        ActorComponent* component = m_components[it];

        component->drawEdit(drawInterface, _flags);
    }
}

void Actor::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    setAABB(getPos());

    if(_modifyInitialPos)
    {
        if(Actor* parent = getParent().getActor())
        {
            parent->getChildrenBindHandler().computeInitialTransform(this);
        }
    }

    if (!hasDataError())
    {
        u32 numComponents = m_components.size();

        for ( u32 it = 0; it < numComponents; ++it )
        {
            ActorComponent* component = m_components[it];

            component->onEditorMove(_modifyInitialPos);
        }
    }

    getChildrenBindHandler().onEditorMove(_modifyInitialPos);
}

void Actor::onEditorCreated( Pickable* _original /*= NULL*/ )
{
    if(!hasDataError())
    {
        Actor* original = (Actor*)_original;

        u32 numComponents = m_components.size();
        for ( u32 it = 0; it < numComponents; ++it )
        {
            ActorComponent* component = m_components[it];
            component->onEditorCreated(original);
        }
    }
}

void Actor::forceReload(HotReloadType _hotReload)
{
    hotReloadReset(btrue);
    ActorHotReload hotReloadData;
    fillHotReloadData(hotReloadData,bfalse,bfalse);
    hotReload(hotReloadData, _hotReload);
}


#endif // ITF_SUPPORT_EDITOR

void Actor::teleportTo( const Vec3d& _pos )
{
    u32 numComponents = m_components.size();
    for ( u32 it = 0; it < numComponents; ++it )
    {
        ActorComponent* component = m_components[it];
        component->teleportTo(_pos);
    }
}

void Actor::setPhysShape(PhysShape* _shape)
{
    m_physShape = _shape;
}

PhysShape* Actor::getPhysShape()
{
    return m_physShape;
}

void Actor::setUpdateDisabled( bbool _val )
{
    for (u32 index = 0; index < m_components.size(); index++)
    {
        m_components[index]->setUpdateDisabled(_val);
    }
}

#if defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_COOKING)
void Actor::loadInstanceDataFromObj( Pickable* _pickable, const class SerializedObjectContent_Object* _content )
{
#ifdef ITF_SUPPORT_LUA
    // Copy each component separatedly, in case the instance removes/adds components that are not in the template
    Actor* instanceActor = DYNAMIC_CAST(_pickable,Actor);

    if ( !instanceActor )
    {
        return;
    }

    m_componentBeforeInstanceData.clear();
    u32 numComponents = m_components.size();
    for (u32 j = 0; j < numComponents; ++j)
    {
        ActorComponent* comp = m_components[j];
        if (comp)
        {
            m_componentBeforeInstanceData.push_back(comp->GetObjectClassCRC());
        }
    }

    CSerializerObjectLua serializer;
    u32 numComponentsInstance = instanceActor->m_components.size();
    ITF_STDSTRING buffer;
    const SerializedObjectContent_Object* components = _content->getObjectWithTag(COMPONENTS_TAG);

    for (u32 i = 0; i < numComponentsInstance; ++i)
    {
        ActorComponent* myComponent = NULL;
        ActorComponent* instanceComponent = instanceActor->m_components[i];
        StringID::StringIdValueType instanceCRC = instanceComponent->GetObjectClassCRC();
        for (u32 j = 0; j < numComponents; ++j)
        {
            if ( m_components[j] && m_components[j]->GetObjectClassCRC() == instanceCRC )
            {
                myComponent = m_components[j];
                break;
            }
        }

        if ( !myComponent )
        {
            myComponent = static_cast<ActorComponent*>(instanceComponent->CreateNewObject());
            m_components.push_back(myComponent);
            numComponents++;
        }

        const SerializedObjectContent_Object* componentContent = components ? components->getObjectWithTag(instanceComponent->GetObjectClassName()) : NULL;

        // Save in a buffer only the information that existed in the instance file
        serializer.OpenBufferToWrite("component", btrue, instanceComponent->GetObjectClassName());
        serializer.setWriteContent(componentContent);
        instanceComponent->Serialize(&serializer,ESerialize_Data_Save|ESerialize_InstanceSave);
        buffer = serializer.CloseBuffer();
        // Read that buffer in our component, only changing the properties that are present
        serializer.OpenBufferToRead(buffer.c_str());
        serializer.SerializeObject("component",myComponent,GAMEINTERFACE->getActorComponentsFactory(),ESerialize_Data_Load|ESerialize_InstanceLoad);
    }

    // Copy the data from the actor
    ComponentsVector myComponents = m_components;
    ComponentsVector instanceComponents = instanceActor->m_components;
    Actor* myActor = this;

    m_components.clear();
    instanceActor->m_components.clear();

    // Save in a buffer only the information that existed in the instance file
    serializer.OpenBufferToWrite("actor", btrue, NULL);
    serializer.setWriteContent(_content);
    instanceActor->Serialize(&serializer,ESerialize_Data_Save|ESerialize_InstanceSave);
    buffer = serializer.CloseBuffer();
    // Read that buffer in our actor, only changing the properties that are present
    serializer.OpenBufferToRead(buffer.c_str());
    serializer.SerializeObject("actor",myActor,ESerialize_Data_Load|ESerialize_InstanceLoad);

    m_components = myComponents;
    instanceActor->m_components = instanceComponents;
#endif
}

#endif // defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_COOKING)

#ifndef ITF_STRIPPED_DATA
    
void Actor::updateForcedValues( const SerializedObjectContent_Object* _content )
{
    Super::updateForcedValues(_content);

    const SerializedObjectContent_Object* componentListContent = (_content ? _content->getObjectWithTag(COMPONENTS_TAG) : NULL);

    for(u32 i = 0; i < m_components.size(); ++i)
    {
        ActorComponent* compo = m_components[i];

        const SerializedObjectContent_Object* componentContent = (componentListContent ? componentListContent->getObjectWithTag(compo->GetObjectClassName()) : NULL);

        compo->updateForcedValues(componentContent);
    }
}

#endif // !ITF_STRIPPED_DATA


//////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(Actor_Template)

BEGIN_SERIALIZATION_CHILD(Actor_Template)

SERIALIZE_BOOL("PROCEDURAL",m_isProcedural);
SERIALIZE_BOOL("STARTPAUSED",m_startPaused);
SERIALIZE_BOOL("FORCEISENVIRONMENT",m_forceEnvironment); 
SERIALIZE_BOOL("FORCEALWAYSACTIVE",m_forceAlwaysActive); 
SERIALIZE_CONTAINER_WITH_FACTORY(COMPONENTS_TAG,m_components,GAMEINTERFACE->getComponentsFactory());
END_SERIALIZATION()

Actor_Template::Actor_Template()
: m_isProcedural(bfalse)
, m_startPaused(bfalse)
, m_forceEnvironment(bfalse)
, m_forceAlwaysActive(bfalse)
{
}

Actor_Template::~Actor_Template()
{
    if ( !m_loadedInPlace )
    {
        u32 numComponents = m_components.size();

        for ( u32 i = 0; i < numComponents; i++ )
        {
            ActorComponent_Template* comp = m_components[i];
            SF_DEL(comp);
        }

        m_components.clear();
    }
}

bbool Actor_Template::onTemplateLoaded( bbool _hotReload )
{
    bbool isOk = Super::onTemplateLoaded(_hotReload);

#ifndef ITF_FINAL
    ComponentList::iterator itComponents = m_components.begin();

    while ( itComponents != m_components.end() )
    {
        if ( *itComponents == NULL )
        {
            String8 path;
            getFile().toString8(path);
            ITF_WARNING(NULL, 0, "Invalid component template in %s", path.cStr());
            itComponents = m_components.erase(itComponents);
        }
        else
        {
            ++itComponents;
        }
    }
#endif

    u32 numComponents = m_components.size();

    for ( u32 i = 0; i < numComponents; i++ )
    {
        if(ActorComponent_Template* comp = m_components[i])
        {
            comp->setActorTemplate(this);
            comp->onTemplateLoaded(_hotReload);
#ifndef ITF_CONSOLE // in console the data is cooked and valid
            u32 j = i+1;

            while ( j < numComponents )
            {
                ActorComponent_Template* temp = m_components[j];

                if ( temp->GetObjectClassCRC() == comp->GetObjectClassCRC() )
                {
                    isOk = bfalse;
                    SF_DEL(temp);
                    m_components.removeAt(j);
                    numComponents--;
                }
                else
                {
                    j++;
                }
            }
#endif // ITF_CONSOLE
        }
    }
    return isOk;
}

void Actor_Template::onTemplateDelete( bbool _hotReload )
{
    u32 numComponents = m_components.size();

    for ( u32 i = 0; i < numComponents; i++ )
    {
        ActorComponent_Template* comp = m_components[i];
        comp->onTemplateDelete(_hotReload);
    }

    Super::onTemplateDelete(_hotReload);
}

void Actor_Template::delTemplateComponent(const String8 & _component, bbool _clearMem)
{
    ActorComponent_Template* component = getTemplateComponentFromString(_component);
    if (!component)
        return;

    int pos = m_components.find(component);
    if (pos  >= 0)
    {
        m_components.removeAt(pos);
        return;
    }
    else
    {
        ITF_ASSERT_MSG(0, "Del template component error: component not found with name (%s)", _component.cStr());
    }

}

void Actor_Template::insertTemplateComponent( ActorComponent_Template* _actorComponent, u32 _index)
{
    if (_index >= m_components.size())
    {
        addTemplateComponent( _actorComponent);
        return;
    }
    m_components.insertAt(_index,_actorComponent);
}

ActorComponent_Template* Actor_Template::getTemplateComponentFromString(const String8 & _component) const
{
    return getTemplateComponentFromString(_component.cStr());
}

ActorComponent_Template* Actor_Template::getTemplateComponentFromString(const char* _component) const
{
    StringID objId = _component;
    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent_Template* pComponent = m_components[it];

        if(!pComponent)
            continue;

        if ( objId.GetValue() == pComponent->GetObjectClassCRC() )
            return pComponent;
    }

    return NULL;
}

i32 Actor_Template::getIndexTemplateComponentFromString(const String8 & _component) const
{
    return getIndexTemplateComponentFromString(_component.cStr());
}

i32 Actor_Template::getIndexTemplateComponentFromString(const char* _component) const
{
    StringID objId = _component;
    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent_Template* pComponent = m_components[it];

        if(!pComponent)
            continue;

        if ( objId.GetValue() == pComponent->GetObjectClassCRC() )
            return (i32)it;
    }

    return -1;
}

void Actor_Template::moveTemplateComponent( const String8 & _component, u32 _newPos)
{
    if (_newPos >= m_components.size())
        return;

    ActorComponent_Template* actorComponent = getTemplateComponentFromString(_component);
    if (!actorComponent)
        return;

    delTemplateComponent( _component, bfalse );
    insertTemplateComponent( actorComponent, _newPos);
}


#ifndef ITF_STRIPPED_DATA
void Actor_Template::getTags( ITF_VECTOR<String8>& _tagList ) const
{
    Super::getTags(_tagList);
        
    if(_tagList.size() == 0)
        _tagList.push_back("actor");
}
#endif // ITF_STRIPPED_DATA

#ifdef ITF_SUPPORT_COOKING
void Actor_Template::onCookClean( const Platform& _platform )
{
    Super::onCookClean(_platform);

    const u32 uSize = m_components.size();
    for ( u32 it = 0; it < uSize; ++it )
    {
        ActorComponent_Template* pComponent = m_components[it];

        if(!pComponent)
            continue;

        pComponent->onCookClean(_platform);
    }
}
#endif // ITF_SUPPORT_COOKING


} //namespace ITF

