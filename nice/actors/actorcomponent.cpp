#include "precompiled_engine.h"

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

//#ifndef ITF_TEMPLATEDATABASE_H
//#include "engine/TemplateManager/TemplateDatabase.h"
//#endif // ITF_TEMPLATEDATABASE_H

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(ActorComponent)

BEGIN_SERIALIZATION_ABSTRACT(ActorComponent)

#ifdef ITF_SUPPORT_EDITOR
    BEGIN_CONDITION_BLOCK(ESerialize_PropertyEdit)
        SERIALIZE_MEMBER("UpdateDisabled", m_updateDisabled);
    END_CONDITION_BLOCK()
#endif // ITF_SUPPORT_EDITOR
END_SERIALIZATION()

// To avoid including a bunch of heavy stuff in engine.
IEventForwarder *g_EventForwarder = NULL;


ActorComponent::ActorComponent()
: m_actor(NULL)
, m_template(NULL)
, m_updateDisabled(bfalse)
{
}

ActorComponent::~ActorComponent()
{
}

World* ActorComponent::getWorld() const
{
    ITF_ASSERT(m_actor);
    return m_actor->getWorld();
}

void ActorComponent::setTemplate( const ActorComponent_Template* _template )
{
    ITF_ASSERT(_template && _template->getComponentClassCRC()==GetObjectClassCRC());
    m_template = _template;
}

void ActorComponent::onEvent(Event * _event)
{
    // Filter events that have to be forwarded to remote clients
    if (_event->isNetworkedEventClass())
    {
        if (g_EventForwarder)
            g_EventForwarder->ForwardEvent(this, _event);
    }
}

#ifndef ITF_SUPPORT_ONLINE_MULTIPLAYER
// In theoretical case events having isNetworkedEventClass() are used in non multiplayer builds
void DefaultEventForwarder::ForwardEvent(ActorComponent *_actor, Event *_event)
{
    _actor->onEventOnMaster(_event, false);
}
#endif

void ActorComponent::onPersistentLoaded()
{
    onCheckpointLoaded();
}

void ActorComponent::SerializePersistentActorRef( CSerializerObject* serializer, u32 flags, const char *_name, ActorRef& _ref, const ActorComponent *const _component )
{
    u32 val = ITF_INVALID_OBJREF;
    if(flags & ESerialize_Data_Load)
    {
        SERIALIZE_MEMBER(_name,val);
        if(val == ITF_INVALID_OBJREF)
        {
            _ref.invalidate();
            return;
        }

        u32 numWorlds = WORLD_MANAGER->getWorldCount();
        for ( u32 worldIndex = 0; worldIndex < numWorlds; worldIndex++ )
        {
            World* world = WORLD_MANAGER->getWorldAt(worldIndex);
            u32 numScenes = world->getSceneCount();

            for ( u32 sceneIndex = 0; sceneIndex < numScenes; sceneIndex++ )
            {
                Scene* scene = world->getSceneAt(sceneIndex);
                const PickableList& pickableList = scene->getPickableList();

                for ( u32 pickableIndex = 0; pickableIndex < pickableList.size(); pickableIndex++ )
                {
                    Pickable* pick = pickableList[pickableIndex];

                    if ( pick->getObjectType() == BaseObject::eActor )
                    {
                        Actor* actor = static_cast<Actor*>(pick);

                        u32 v = actor->computePersistentID();
                        if(v == val)
                        {
                            _ref = actor->getRef();
                            return;
                        }
                    }
                }
            }
		}
#if !defined(ITF_FINAL) && defined(ITF_ENABLE_RTTI_CRC_CHECK)
		String8 str;
		str.setTextFormat("Couldn't find actor reference for %s::%s", _component ? _component->GetObjectClassName() : "", _name);
		ITF_WARNING(_component ? _component->GetActor() : NULL, 0, str.cStr());
#endif
    }
    else
    {
        const Actor * pActorRef = _ref.getActor();
        if(pActorRef)
            val = pActorRef->computePersistentID();
        SERIALIZE_MEMBER(_name,val);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(ActorComponent_Template)

BEGIN_SERIALIZATION_ABSTRACT(ActorComponent_Template)
END_SERIALIZATION()

}
