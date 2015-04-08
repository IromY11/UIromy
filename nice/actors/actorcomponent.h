#ifndef _ITF_ACTORCOMPONENT_H_
#define _ITF_ACTORCOMPONENT_H_

#ifndef _ITF_EVENTLISTENER_H_
#include "engine/events/IEventListener.h"
#endif //_ITF_EVENTLISTENER_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif // _ITF_PICKABLE_H_

#ifndef ITF_TEMPLATEDATABASE_TYPES_H
#include "engine/TemplateManager/TemplateDatabase_types.h"
#endif // ITF_TEMPLATEDATABASE_TYPES_H

//struct lua_State;

#define DECLARE_VALIDATE_COMPONENT() virtual void validate(bbool &_isComponentValidated);
#define BEGIN_VALIDATE_COMPONENT(_classname_) void _classname_::validate(bbool &_isComponentValidated) {Super::validate(_isComponentValidated);
#define END_VALIDATE_COMPONENT() }

namespace ITF
{

class Event;
class GhostPlayerInfo;
class GhostRecorderInfoBase;
class SerializedObjectContent_Object;

class ActorComponent : public IRTTIObject, public IEventListener
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(ActorComponent,IRTTIObject,2669192659)

public:
    DECLARE_SERIALIZE_VIRTUAL()

    ActorComponent();
    virtual ~ActorComponent();

#ifdef ITF_SUPPORT_EDITOR
    enum DrawEditFlags
    {   // powers of 2 only
        DrawEditFlag_Nothing    = 0,
        DrawEditFlag_Collisions = 1,
        DrawEditFlag_Sounds = 2,
        DrawEditFlag_All = 0xffffffff
    };
#endif // ITF_SUPPORT_EDITOR

    virtual bbool       needsUpdate() const = 0;
    virtual bbool       needsDraw() const = 0;
    virtual bbool       needsDraw2D() const = 0;
    // We can have have 2d components with no drawing, use is2D virtual to set them
    virtual bbool       is2D() const { return needsDraw2D(); }
	virtual bbool       needsDraw2DNoScreenRatio() const = 0;
    virtual bbool       keepAlive() { return bfalse; } // Must return true for component life dependent

    virtual void        teleportTo(const Vec3d& _pos) {} 

    virtual void        Update( f32 _deltaTime ) {}
	virtual void        onWorldUpdate() {}
    virtual void        batchPrimitives( const ITF_VECTOR <class View*>& _views ) {}
    virtual void        batchPrimitives2D( const ITF_VECTOR <class View*>& _views ) {}
#ifdef ITF_SUPPORT_EDITOR
    virtual void        drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const {}
    virtual void        onEditorMove(bbool _modifyInitialPos = btrue) {}
    virtual void        onEditorCreated( class Actor* _original ) {}  // the actor has just been created in the editor (drag'n'drop from a lua, ctrl+drag clone, or copy-paste clone)
    virtual void        onPrePropertyChange( ) {}
    virtual void        onPostPropertyChange( ) {}
    virtual void        onPreEditorSave() {}
    virtual void        onPostEditorSave() {}
#endif // ITF_SUPPORT_EDITOR
#ifdef ITF_SUPPORT_COOKING
    virtual void        onCookProcess() {}
#endif // ITF_SUPPORT_COOKING
    virtual void        onForceMove() {}
    virtual void        onSceneActive() {}
    virtual void        onSceneInactive() {}
    virtual void        onActorLoaded(Pickable::HotReloadType _hotReload ) {}
    virtual void        onFinalizeLoad() {}
    virtual void		onStartDestroy(bbool _hotReload) {}
    virtual void        onCheckpointLoaded() {}
	virtual void        SerializePersistent( CSerializerObject* serializer, u32 flags ) {}
	static void			SerializePersistentActorRef( CSerializerObject* serializer, u32 flags, const char *_name, class ActorRef& _ref, const ActorComponent *const _component );
    virtual void        onPersistentLoaded();
	virtual void        onBecomeActive() {}
    virtual void        onBecomeInactive() {}
    virtual void        onDepthChanged( f32 _oldZ, f32 _newZ ) {}
    virtual void        onScaleChanged( const Vec2d& _oldScale, const Vec2d& _newScale ) {}
    virtual void		onFlipChanged() {}
    virtual void        onPosChanged( const Vec3d& _oldPos, const Vec3d& _newPos ) {}
    virtual void        onActorClearComponents() {}
    virtual bbool       isActorDestructionValidated() { return btrue; }              // each component can unvalidate the actor destruction request by returning false, but it MUST BE controlled !!!    
    virtual void        onApplyGhost(GhostRecorderInfoBase* _ghostRecorderInfo){}
    virtual void        onRecordGhost(GhostRecorderInfoBase* _ghostRecorderInfo){}

	virtual void		preUpdatePos( Vec3d &_pos ) {}
    
	class World*        getWorld() const;


    class Actor*        GetActor() const { return m_actor; }
    void                SetActor( class Actor* _actor ) { ITF_ASSERT(m_actor==NULL || m_actor == _actor); m_actor = _actor; }
    void                ClearActor() { m_actor = NULL; }

    const class ActorComponent_Template*    getTemplate() const { return m_template; }
    void                                    setTemplate( const class ActorComponent_Template* _template );
    void                                    clearTemplate() { m_template = NULL; }

    virtual void        onEvent(Event * _event);
    virtual bool        onEventOnMaster(Event * _event, bool fromLocal) { return true; }; // TODO: pure virtual
    virtual bool        onEventOnReplica(Event * _event, bool fromLocal) { return true; }; // TODO: pure virtual

    /*
    *   callback invoked when the owner actor unloads its resources
    */
    virtual void        onUnloadResources() {};
    virtual void        onResourceLoaded()  {};     // all resources are loaded; components that have resources should process them now
    virtual void        onResourceReady()   {};     // all resources are loaded and processed; components that depend on others can use them now

    virtual void        validate(bbool &_isComponentValidated) {_isComponentValidated = btrue;}

#ifndef ITF_STRIPPED_DATA

    virtual void        updateForcedValues( const SerializedObjectContent_Object* _content ) {}

#endif // !ITF_STRIPPED_DATA

	ITF_INLINE virtual void		setUpdateDisabled(bbool _val) { m_updateDisabled = _val; }
	ITF_INLINE bbool	        getUpdateDisabled() { return m_updateDisabled; }

protected:

    class Actor*							m_actor;
    const class ActorComponent_Template*	m_template;
	bbool									m_updateDisabled;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////

class ActorComponent_Template : public TemplateObjBase
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(ActorComponent_Template,TemplateObjBase,2376312954)
    DECLARE_SERIALIZE()

public:

    ActorComponent_Template()
        : m_actorTemplate(NULL)
    {}
    ~ActorComponent_Template() {}

    virtual class ActorComponent* createComponent() const = 0;
    virtual StringID::StringIdValueType getComponentClassCRC() const = 0;
    virtual bbool onTemplateLoaded( bbool _hotReload ) { return btrue; }
    virtual void onTemplateDelete( bbool _hotReload ) {}

    void setActorTemplate( class Actor_Template * _template ) { m_actorTemplate = _template; }
    const class Actor_Template* getActorTemplate() const { return m_actorTemplate; }

#ifdef ITF_SUPPORT_COOKING
    virtual void            onCookClean( const Platform& _platform ) {}
#endif // ITF_SUPPORT_COOKING

protected:

    class Actor_Template    * m_actorTemplate;
};

class IEventForwarder
{
public:
    virtual void ForwardEvent(ActorComponent *_actor, Event *_event) = 0;
    virtual ~IEventForwarder() {}
};

#ifndef ITF_SUPPORT_ONLINE_MULTIPLAYER
class DefaultEventForwarder : public IEventForwarder
{
public:
    virtual void ForwardEvent(ActorComponent *_actor, Event *_event);
};
#endif

}

#endif // _ITF_ACTORCOMPONENT_H_
