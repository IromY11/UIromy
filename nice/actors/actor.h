#ifndef _ITF_ACTOR_H_
#define _ITF_ACTOR_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

#ifndef _ITF_ACTOR_BIND_H_
#include "engine/actors/ActorBind.h"
#endif // _ITF_ACTOR_BIND_H_

#ifndef ITF_TEMPLATEDATABASE_TYPES_H
#include "engine/TemplateManager/TemplateDatabase_types.h"
#endif // ITF_TEMPLATEDATABASE_TYPES_H

#ifndef _ITF_OBJECTFACTORY_H_
#include "core/serializer/ObjectFactory.h"
#endif //_ITF_OBJECTFACTORY_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif

//tmp fix from engine called from core ...dependencies needs to be cleaned before
#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_



namespace ITF
{

class Event;
class Actor;
class Texture;
class ObjectGroup;
class IEventListener;
class Actor_Template;
struct ActorHotReload;
class ActorDrawEditInterface;
class SerializedObjectContent_Object;
class PhysShape;

#define Actor_CRC ITF_GET_STRINGID_CRC(Actor,2546623115)
class Actor : public Pickable
{
public:

    ITF_DECLARE_NEW_AND_DELETE_OPERATORS_CATEGORY(mId_Actor);
    DECLARE_OBJECT_CHILD_RTTI(Actor,Pickable,2546623115);
    DECLARE_SERIALIZE()

    typedef ITF_VECTOR<class ActorComponent*> ComponentsVector;

    Actor();
    virtual ~Actor();

    virtual void            update( f32 _deltaTime );
	virtual void            onWorldUpdate();
    virtual void            batchPrimitives( const ITF_VECTOR <class View*>& _views );
    virtual void            batchPrimitives2D( const ITF_VECTOR <class View*>& _views );
    virtual void            onLoaded(HotReloadType _hotReload);
    virtual void            onFinalizeLoad( bbool _hotReload );
    virtual void            onStartDestroy( bbool _hotReload );
    
    virtual void			SerializePersistent( CSerializerObject* serializer, u32 flags );
    virtual void            onLoadedCheckpoint();
    virtual void            onLoadedPersistentData();
    virtual void            onSceneActive();
    virtual void            onSceneInactive();
    virtual void            onBecomeActive();
    virtual void            onBecomeInactive();
    virtual void            onEvent(Event* _event);

    virtual void            onApplyGhost(class GhostRecorderInfoBase* _ghostRecorderInfo);
    virtual void            onRecordGhost(class GhostRecorderInfoBase* _ghostRecorderInfo);

    virtual void            requestDestruction();

    virtual void            onDestroy( bbool _hotReload );  // Called by the scene/actorsManager just before deleting it

    virtual void            hotReload( ActorHotReload& _reloadInfo, HotReloadType _hotReloadType );
    void                    fillHotReloadData(ActorHotReload & hotreloadInfo, bbool _useTwoStepHotreload, bbool _skipInstanceDataFile );
    void                    replaceReinitDataByCurrent();
    void                    saveCheckpointData();
    void                    postLoadCheckpointData();

    //Physhape : to allow detection on every kind of actors
private:
    PhysShape*              m_physShape;
public :
    void		            setUpdateDisabled(bbool _val);

    void                    setPhysShape(PhysShape* _shape);
    PhysShape*              getPhysShape();


    void                    AddComponent( class ActorComponent* _actorComponent, bbool _udpateLists = btrue );
    template <class T>
    u32                     GetComponentOrder() const;
    template <class T>
    T*                      GetComponent() const;
    template <class T>
    SafeArray<T*>           GetComponents() const;
    const ComponentsVector& GetAllComponents() const {return m_components; }
    ActorComponent*         getComponentFromStaticClassCRC(StringID::StringIdValueType _staticClassCRC) const;
    void                    ClearComponents(bbool _clearMem,bbool _clearComponents);
    
    ITF_INLINE const class Actor_Template*   getTemplate() const ;

    virtual bbool           isStartPaused() const;
    bool                    isLuaLoaded() const { return m_components.size() > 0; }
    f32                     getRadius() const { return m_radius; }
    void                    setRadius(f32 _radius) { m_radius = _radius; } // physical contact radius

    BindHandler&            getChildrenBindHandler()        { return m_childrenHandler; }
    const BindHandler&      getChildrenBindHandler()const   { return m_childrenHandler; }
    Bind*                   getParentBind()const            { return m_pParentBind; }
    void                    setParentBind(Bind* _newParent, bbool _isInitial = bfalse);
    class ActorRef          getParent()const;
    void                    createParentBind( bbool _temporaryBind, Actor* _parent, Bind::Type _type, u32 _typeData, bbool _useParentFlip =btrue, bbool _useParentScale=bfalse, bbool _useRelativeZ = bfalse, bbool _removeWithParent=bfalse);
    ITF_INLINE bbool        isParentBindRunTimeDisabled() const { return m_pParentBind && m_pParentBind->isRuntimeDisabled(); }
    void                    clearChildBinds(bbool _onlyDynamic);
    void                    unbindFromParent();
    void                    onLoadProcessBind( HotReloadType _hotReload );

    void                    onForceMove();
    void                    forceMoveSkipComponent( const Vec3d& _pos, f32 _angle, class ActorComponent* _skip );

    virtual void            teleportTo( const Vec3d& _pos );

    virtual void            getParentWorldTransform( Vec3d& _parentPos, f32& _parentAngle, bbool& _flipped, Vec2d& _scale ) const;

    Vec3d                   getBoundWorldInitialPos(bbool _initialFromCurrentParent) const;
    Vec3d                   getBoundLocalInitialPos() const;
    void                    setBoundWorldInitialPos( const Vec3d& _pos, bbool _fromCurrentParentTransform );
    void                    setBoundWorldPos( const Vec3d& _pos );
    Vec3d                   getBoundLocalPos() const;
    void                    setBoundLocalPos( const Vec3d& _pos );

    const Vec2d&            getBoundLocalInitial2DPos() const;
    Vec2d                   getBoundLocal2DPos() const;
    void                    setBoundLocal2DPos( const Vec2d& _pos );

    f32                     getBoundWorldInitialAngle(bbool _initialFromCurrentParent) const;
    f32                     getBoundLocalInitialAngle() const;
    void                    setBoundWorldInitialAngle(f32 _angle, bbool _fromCurrentParentTransform);
    void                    setBoundWorldAngle(f32 _angle );
    f32                     getBoundLocalAngle() const;
    void                    setBoundLocalAngle( f32 _angle );

    virtual void            computeInitialWorldTransformFromSerializedData( Vec3d& _pos, f32& _angle, bbool& _flipped, Vec2d& _scale ) const;

    // call this after the local pos/angle have changed to recompute the world pos/angle
    void                    updateWorldCoordinatesFromBoundParent();
    void                    updateWorldCoordinatesForBoundChildren( bbool _initPos = btrue );

    void                    setTransferEventToLinkChildren( bbool _val );

    void                    registerEvent( StringID::StringIdValueType _crc, IEventListener* _actorComponent );
    void                    unregisterEvent( StringID::StringIdValueType _crc, IEventListener* _listener );
    bbool                   isEventRegistered( StringID::StringIdValueType _crc, IEventListener* _listener = NULL );

    Vec3d                   getBase() const;

    const ResourceContainer* getTemplateResourceContainer() const;

    bbool                   isCleared() { return m_components.size() == 0; }

    void                    resetTimeFactor() { m_timeFactor = 1.f; }
    void                    setTimeFactor(f32 _timeFactor) { m_timeFactor = f32_Max(0.01f, _timeFactor); }  // Zero drives NAN because of Zero division.
    f32                     getTimeFactor() const { return m_timeFactor; }

    void                    setOverrideTemplate(Actor_Template*  _overrideTemplateActor, bbool _deleteWithActor = bfalse);
    class Actor_Template*   getOverrideTemplate(  ) { return m_overrideTemplateActor; }
    ITF_INLINE bbool        isUsingTemplate(const class TemplateObj* _templateActor);

#ifdef ASSERT_ENABLED
    const HotReloadType&    getHotReloadingStatus() {return  m_hotReloadingStatus;}
#endif  //ASSERT_ENABLED

#ifdef ITF_SUPPORT_EDITOR
    virtual Pickable*       clone( Scene* _pDstScene, bbool _callOnLoaded ) const;
    virtual void            forceReload(HotReloadType _hotReload);
    void                    drawEdit( ActorDrawEditInterface* drawInterface, u32 _flags  );
    virtual void            onEditorMove(bbool _modifyInitialPos = btrue);
    virtual void            onEditorCreated( Pickable* _original = NULL );  // the actor has just been created in the editor (drag'n'drop from a lua, ctrl+drag clone, or copy-paste clone)

    virtual void            onPrePropertyChange() override;
    virtual void            onPostPropertyChange() override;

    virtual void            onPreEditorSave() override;
    virtual void            onPostEditorSave() override;

#endif // ITF_SUPPORT_EDITOR

#ifndef ITF_FINAL
    virtual bbool           validate();
    // for debug readability
    u32                     getUpdateComponentsCount() const {return m_updateComponents.size();}
    u32                     computeNbBones() const;
    u32                     getNbBones() const {return m_nbBones;}
#endif

#ifdef ITF_SUPPORT_COOKING
    void                    cookSetTemplate( const TemplatePickable* _pickable ) { m_templatePickable = _pickable; }
#endif

    enum OnlineState
    {
        Offline = 0,
        WaitingState,
        Master, 
        Replica,
		Autonomous,
    };
    void                    setOnlineState(OnlineState state);
    OnlineState             getOnlineState() const { return m_onlineState; }
    bbool                   isOffline() const { return getOnlineState() == Offline; }
    bbool                   isMaster() const { return getOnlineState() == Master; }
    bbool                   isReplica() const { return getOnlineState() == Replica; }
    bbool                   isWaitingOnlineState() const { return getOnlineState() == WaitingState; }
	bbool                   isAutonomous() const { return getOnlineState() == Autonomous; }
private:

    void                    preSaveData();
    void                    requestTemplateType();
    virtual const TemplatePickable* requestTemplateType(const Path& _path) const;

protected:


    void                    onLoadProcessTemplate();
    void                    onLoadPreProcessComponents( HotReloadType _hotReload );
    void                    onLoadProcessComponents( HotReloadType _hotReload );
    void                    onLoadClearNULLComponents();

    virtual void            onResourceLoaded();
    virtual void            onResourceUnloaded();
    
    virtual void            onScaleChanged(const Vec2d& _prevScale);
    virtual void            onDepthChanged( f32 _prevDepth );
    virtual void            onFlipChanged();
    virtual void            onPosChanged( const Vec3d& _prevPos ) override;
    virtual void            onAnglePosChanged( f32 _prevAngle, const Vec3d& _prevPos ) override;

    void                    fixComponentsFromTemplate( const class Actor_Template* _template );

#if defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_COOKING)
    ITF_VECTOR<StringID>    m_componentBeforeInstanceData;
    virtual void            loadInstanceDataFromObj( Pickable* _pickable, const class SerializedObjectContent_Object* _content );
#endif

    typedef SafeArray<IEventListener*> RegisteredListenerArray;
    struct RegisteredEvent
    {
        const ObjectFactory::ClassInfo*     m_eventClass;
        RegisteredListenerArray             m_listeners;
    };
    typedef ITF_VECTOR<RegisteredEvent> RegisteredEventsArray;

    Actor_Template*                 m_overrideTemplateActor;

    ComponentsVector                m_components;
    ComponentsVector                m_updateComponents;
    ComponentsVector                m_drawComponents;
    ComponentsVector                m_draw2DComponents;
    RegisteredEventsArray           m_registeredEvents;

    u32                             m_deleteOverrideTemplateWithActor : 1,
                                    m_transferEventsToLinkChildren : 1,
                                    m_actorActiveDone : 1,
                                    m_actorSceneActiveDone : 1,
                                    m_actorComponentProcessed : 1,
                                    m_actorComponentFinalizeLoadDone : 1;

    BindHandler                     m_childrenHandler;  // runtime children list filled from children loading
    
#ifdef ASSERT_ENABLED
    HotReloadType                   m_hotReloadingStatus;
#endif  //ASSERT_ENABLEDonLoadedFromScene

    OnlineState                     m_onlineState;
    Bind*                           m_pParentBind_Initial;
    Bind*                           m_pParentBind;      // parent bind info to parent

    f32                             m_radius;
    f32                             m_timeFactor;

#ifndef ITF_FINAL
public:
    u32                             m_drawCount;
    mutable u32                     m_nbBones;
#endif // ITF_FINAL

#ifndef ITF_STRIPPED_DATA
    
public:
    virtual void                    updateForcedValues( const SerializedObjectContent_Object* _content );


#endif // !ITF_STRIPPED_DATA
};

template <class T>
T* Actor::GetComponent() const
{
     for ( u32 it = 0; it < m_components.size(); ++it )
     {
         ActorComponent* pComponent = m_components[it];
         ITF_ASSERT_MSG(pComponent, "An actor could not have a NULL component");

         if ( pComponent && pComponent->IsClassCRC(T::GetClassCRCStatic()) )
         {
             return static_cast<T*>(pComponent);
         }
     }

     return NULL;
}

template <class T>
SafeArray<T*> Actor::GetComponents() const
{
    SafeArray<T*> componentsFound;

    for ( u32 it = 0; it < m_components.size(); ++it )
    {
        ActorComponent* pComponent = m_components[it];
        ITF_ASSERT_MSG(pComponent, "An actor could not have a NULL component");

        if ( pComponent && pComponent->IsClassCRC(T::GetClassCRCStatic()) )
        {
            componentsFound.push_back(static_cast<T*>(pComponent));
        }
    }

    return componentsFound;
}

template <class T>
u32 Actor::GetComponentOrder() const
{
    for ( u32 it = 0; it < m_components.size(); ++it )
    {
        ActorComponent* pComponent = m_components[it];
        ITF_ASSERT_MSG(pComponent, "An actor could not have a NULL component");

        if ( pComponent && pComponent->IsClassCRC(T::GetClassCRCStatic()) )
        {
            return it;
        }
    }

    return U32_INVALID;
}


#define ACTOR_REGISTER_EVENT_COMPONENT(actor,crc,component) actor->registerEvent(crc,component);

//////////////////////////////////////////////////////////////////////////////////////////////////////

class Actor_Template : public TemplatePickable
{
    DECLARE_OBJECT_CHILD_RTTI(Actor_Template,TemplatePickable,461732814)
    DECLARE_SERIALIZE_VIRTUAL()

public:

    typedef ITF_VECTOR<class ActorComponent_Template*> ComponentList;

    Actor_Template();
    ~Actor_Template();

    virtual bbool           onTemplateLoaded( bbool _hotReload );
    virtual void            onTemplateDelete( bbool _hotReload );

    template <class T>
    T*                      GetComponent() const;

    void                    addTemplateComponent(class ActorComponent_Template* _templateActorComponent) { m_components.push_back(_templateActorComponent); }

    void                    delTemplateComponent(const String8 & _component, bbool _clearMem);
    void                    insertTemplateComponent( ActorComponent_Template* _actorComponent, u32 _index);
    ActorComponent_Template* getTemplateComponentFromString(const String8 & _component) const;
    ActorComponent_Template* getTemplateComponentFromString(const char* _component) const;
    void                    moveTemplateComponent( const String8 & _component, u32 _newPos);
    i32                     getIndexTemplateComponentFromString(const String8 & _component) const;
    i32                     getIndexTemplateComponentFromString(const char* _component) const;

    bbool                   getIsProcedural() const { return m_isProcedural; }
    bbool                   getStartPaused() const { return m_startPaused; }
    bbool                   getForceEnvironment() const { return m_forceEnvironment; }
	bbool					getForceAlwaysActive() const { return m_forceAlwaysActive; }

    const ComponentList&    getComponents() const { return m_components; }

#ifndef ITF_STRIPPED_DATA
    void                    getTags( ITF_VECTOR<String8>& _tagList ) const;
#endif

#ifdef ITF_SUPPORT_COOKING
    virtual void            onCookClean( const Platform& _platform );
#endif // ITF_SUPPORT_COOKING

private:

    ComponentList           m_components;
    bbool                   m_isProcedural;
    bbool                   m_startPaused;
    bbool                   m_forceEnvironment;
	bbool					m_forceAlwaysActive;
};

template <class T>
T* Actor_Template::GetComponent() const
{
    for ( u32 it = 0; it < m_components.size(); ++it )
    {
        ActorComponent_Template* pComponent = m_components[it];
        ITF_ASSERT_MSG(pComponent, "An actor template could not have a NULL component template");

        if ( pComponent && pComponent->IsClassCRC(T::GetClassCRCStatic()) )
        {
            return static_cast<T*>(pComponent);
        }
    }

    return NULL;
}

class ActorRef : public ObjectRef
{
public:
    ActorRef() : ObjectRef() {}

    ActorRef(const ObjectRef & _other)
    {
        m_directValue = _other.getValue();
#ifdef GPP_DEBUG
        m_pointer = getActor();
#endif // GPP_DEBUG
    }

    class Actor* getActor() const
    {
        BaseObject* pObj = getObject();
        if ( pObj )
        {
            ITF_ASSERT_CRASH(pObj->getObjectType() == BaseObject::eActor,"ActorRef used for a non-actor object");
            return static_cast<Actor*>(pObj);
        }
        else
        {
            return NULL;
        }
    }

    void setActor( const Actor* _actor )
    {
        m_directValue = _actor->getRef().getValue();
#ifdef GPP_DEBUG
        m_pointer = _actor;
#endif // GPP_DEBUG
    }

    ActorRef & operator =(const ObjectRef & _other)
    {
        m_directValue = _other.getValue();
#ifdef GPP_DEBUG
        m_pointer = _other.m_pointer;
#endif // GPP_DEBUG
        return *this;
    }
};

class ActorDrawEditInterface
{
public:

    virtual void drawPolyLine(const class PolyLine* _pPolyLine, bbool _bSubElement = bfalse) = 0;
#ifdef ITF_SUPPORT_EDITOR
    static Vec2d m_currentMouse2d;
    static Vec3d m_currentMouse3d;
#endif
};

struct ActorHotReload
{
    Actor*          m_actorPtr;
    u32             m_enabled : 1,
                    m_skipInstanceDataFile : 1,
                    m_useTwoStepHotreload : 1;
    ObjectRefList   m_childrenBeforeClear;
};

ITF_INLINE const class Actor_Template*   Actor::getTemplate() const { return static_cast<const class Actor_Template*>(Super::getTemplate()); }
ITF_INLINE bbool Actor::isUsingTemplate(const class TemplateObj* _templateActor) { return getTemplate() == static_cast<const Actor_Template *>(_templateActor); }
ITF_INLINE bbool Actor::isStartPaused() const { return ( m_templatePickable ? ( static_cast<const class Actor_Template*>(m_templatePickable)->getStartPaused() || Super::isStartPaused()): bfalse ); }
ITF_INLINE ActorRef Actor::getParent() const { return m_pParentBind ? m_pParentBind->m_runtimeParent : ITF_INVALID_OBJREF; }
}

#endif // _ITF_ACTOR_H_
