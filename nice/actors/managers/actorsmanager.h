#ifndef _ITF_ACTORSMANAGER_H_
#define _ITF_ACTORSMANAGER_H_

#ifndef _ITF_WATCHDIRECTORYLISTENER_H
#include "core/file/WatchDirectoryListener.h"
#endif  //_ITF_WATCHDIRECTORYLISTENER_H

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ACTORSPAWNPOOLMANAGER_H_
#include "gameplay/Managers/ActorSpawnPoolManager.h"
#endif // _ACTORSPAWNPOOLMANAGER_H_

namespace ITF
{
    class Scene;
    class SubSceneActor;

    class ActorsManager :public WatchDirectoryListener
    {
    public:
        
#ifdef ITF_SUPPORT_HOTRELOAD_TEMPLATE
        struct ActDependencies
        {
            Path m_path;
            ITF_VECTOR <StringID>   m_dependencies;
        };
#endif

        struct ActorDataContainer
        {
            ActorDataContainer() : m_instance(NULL), m_internal(NULL), m_checkpoint(NULL) {}

            void dispose() { SF_DEL(m_instance); SF_DEL(m_internal); SF_DEL(m_checkpoint); }
            void clearCheckpointArchive() {SF_DEL(m_checkpoint);}

            ArchiveMemory*  m_instance;
            ArchiveMemory*  m_internal;
            ArchiveMemory*  m_checkpoint;
        };
        typedef ITF_MAP<ActorRef,ActorDataContainer> ActorDataContainerMap;

        struct FriseDataContainer
        {
            FriseDataContainer() : m_checkpoint(NULL) {}

            void dispose() { SF_DEL(m_checkpoint); }
            void clearCheckpointArchive() {SF_DEL(m_checkpoint);}

            ArchiveMemory*  m_checkpoint;
        };
        typedef ITF_MAP<ObjectRef,FriseDataContainer> FriseDataContainerMap;

#if defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_COOKING)
        struct InstanceDataCacheEntry
        {
            Pickable*                       m_instanceDataObj;
            SerializedObjectContent_Object* m_content;
        };
        typedef ITF_MAP<Path, InstanceDataCacheEntry> InstanceDataCacheMap;
#endif // ITF_SUPPORT_EDITOR) || ITF_SUPPORT_COOKING


        ActorsManager();
        ~ActorsManager();

#ifdef ITF_SUPPORT_HOTRELOAD_TEMPLATE
        void                            onModifiedFile(const Path& _path);
        void                            registerActDependency( const Path& _act, const StringID& _dep );
#endif //ITF_SUPPORT_HOTRELOAD_TEMPLATE

        bbool                           isActorRegistered(class Actor* _actor)const;
        void                            registerSerializeDataActor(class Actor* _actor);
        void                            unregisterSerializeDataActor(class Actor* _actor);
        ActorDataContainer*             getSerializeDataActor(class Actor* _actor);
        ActorDataContainerMap::iterator getActorDataContainerMapStart() {return m_serializeActorData.begin();}
        ActorDataContainerMap::iterator getActorDataContainerMapEnd() {return m_serializeActorData.end();}
        bbool                           isFriseRegistered(class Frise* _frise)const;
        void                            registerSerializeDataFrise(class Frise* _frise);
        void                            unregisterSerializeDataFrise(class Frise* _frise);
        FriseDataContainer*             getSerializeDataFrise(class Frise* _frise);
        FriseDataContainerMap::iterator getFriseDataContainerMapStart() {return m_serializeFriseData.begin();}
        FriseDataContainerMap::iterator getFriseDataContainerMapEnd() {return m_serializeFriseData.end();}
        ActorSpawnPoolManager&          getSpawnPoolManager() { return m_spawnPoolManager; }

#ifdef ITF_WINDOWS
        bbool                           isActorCooking()                     {return m_actorCooking;}
        void                            setActorCooking(bbool _actorCooking) {m_actorCooking = _actorCooking;}
#endif //ITF_WINDOWS

#if defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_COOKING)
        Pickable*                       getInstanceDataFile(const Path &_filename, bbool _forceLoad = bfalse, const SerializedObjectContent_Object** _content = NULL);
#endif // ITF_SUPPORT_EDITOR || ITF_SUPPORT_COOKING

    private:


#ifdef ITF_SUPPORT_HOTRELOAD_TEMPLATE
        void                            hotReloadFromFile(const Path& _path);
        void                            hotReloadFromFileTemplate(const Path& _path);
        void                            hotReloadFromFileMetaFrieze(const Path& _path);
        void                            hotReloadFromSceneFile(const Path& _path);

        ITF_MAP <StringID,ActDependencies>  m_actDependencies;
#endif //ITF_SUPPORT_HOTRELOAD_TEMPLATE

#if defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_COOKING)
        InstanceDataCacheEntry          loadInstanceDataFile(const Path &_filename);
        void                            destroyInstanceDataCacheEntry(InstanceDataCacheEntry &_entry);
        void                            hotReloadFromFileInstance(const Path& _path);
        void                            hotReloadFromAtl(const Path& _path);
        void                            hotReloadFromFileFrieze(const Path& _path);

        InstanceDataCacheMap            m_instanceDataCache;
#endif // ITF_SUPPORT_EDITOR || ITF_SUPPORT_COOKING

        void                            hotReloadActorsUsingResource(const Path& _path);

        ActorSpawnPoolManager           m_spawnPoolManager;

        ActorDataContainerMap           m_serializeActorData;
        FriseDataContainerMap           m_serializeFriseData;

		mutable ITF_THREAD_CRITICAL_SECTION     m_cs;


#ifdef ITF_WINDOWS
        bbool                           m_actorCooking;
#endif //ITF_WINDOWS
    };

}

#endif // _ITF_ACTORSMANAGER_H_
