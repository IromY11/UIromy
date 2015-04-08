#include "precompiled_engine.h"

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef SERIALIZEROBJECTLOGICDB_H
#include "engine/serializer/ZSerializerObjectLogicDB.h"
#endif //SERIALIZEROBJECTLOGICDB_H

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_METAFRIEZE_H_
#include "engine/display/MetaFrieze/MetaFrieze.h"
#endif //_ITF_METAFRIEZE_H_

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifdef ITF_SUPPORT_XML

#ifndef SERIALIZEROBJECTPARSER_H
#include "core/serializer/ZSerializerObjectParser.h"
#endif // SERIALIZEROBJECTPARSER_H

#endif // ITF_SUPPORT_XML

#ifndef _ITF_LUACHUNKMANAGER_H_
#include "core/script/LuaChunkManager.h"
#endif //ITF_SUPPORT_LOGICDATABASE

#ifndef ITF_GFX_MATERIAL_SHADER_MANAGER_H_
#include "engine/display/material/GFXMaterialShaderManager.h"
#endif //ITF_GFX_MATERIAL_MANAGER_H_

#ifdef ITF_SUPPORT_LUA

#ifndef _ITF_LUACHUNKMANAGER_H_
#include "core/script/LuaChunkManager.h"
#endif // _ITF_LUACHUNKMANAGER_H_

#endif

#if defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_COOKING)
#ifndef SERIALIZEROBJECTLUA_H
#include "engine/serializer/ZSerializerObjectLua.h"
#endif // SERIALIZEROBJECTLUA_H

#ifndef _ITF_BASEOBJECTFACTORY_H_
#include "engine/factory/baseobjectFactory.h"
#endif // _ITF_BASEOBJECTFACTORY_H_
#endif // ITF_SUPPORT_EDITOR || ITF_SUPPORT_EDITOR

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

namespace ITF
{

ActorsManager::ActorsManager()
{
#ifdef ITF_WINDOWS
    m_actorCooking                  = bfalse;
#endif //ITF_WINDOWS
	Synchronize::createCriticalSection(&m_cs);

}

ActorsManager::~ActorsManager()
{
    for (ActorDataContainerMap::iterator it = m_serializeActorData.begin(); it != m_serializeActorData.end(); ++it)
    {
        ActorDataContainer& data = it->second;
        data.dispose();
    }

    for (FriseDataContainerMap::iterator it = m_serializeFriseData.begin(); it != m_serializeFriseData.end(); ++it)
    {
        FriseDataContainer& data = it->second;
        data.dispose();
    }

#if defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_COOKING)
    for(InstanceDataCacheMap::iterator it = m_instanceDataCache.begin(); it != m_instanceDataCache.end(); ++it)
        destroyInstanceDataCacheEntry(it->second);

	Synchronize::destroyCriticalSection(&m_cs);

#endif //ITF_SUPPORT_EDITOR || ITF_SUPPORT_COOKING
}

#ifdef ITF_SUPPORT_HOTRELOAD_TEMPLATE
void ActorsManager::hotReloadFromFile(const Path& _path)
{
    if ( _path.getExtension() == "tpl" )
    {
        hotReloadFromFileTemplate(_path);
    }
#ifdef ITF_SUPPORT_EDITOR
    else if ( _path.getExtension() == "act" || _path.getExtension() == "frz" || _path.getExtension() == "mrz" )
    {
        hotReloadFromFileInstance(_path);
    }
#endif //ITF_SUPPORT_EDITOR
    else if( _path.getExtension() == "tia")
    {
        // do nothing
    }
    else
    {
        ITF_ASSERT(0);
    }
}

void ActorsManager::hotReloadFromFileTemplate( const Path& _path )
{
    ITF_VECTOR<ActorHotReload> reloadActors;

    ITF_VECTOR<ObjectRef> objListSrc;
    
    TEMPLATEDATABASE->getTemplateClients(_path,objListSrc);

    if ( objListSrc.size() == 0 )
    {
        TEMPLATEDATABASE->reloadTemplate<Actor_Template>(_path);
        return;
    }

    if (objListSrc.size() > 0)
    {
        reloadActors.reserve(objListSrc.size());
        ITF_VECTOR<ObjectRef>::const_iterator sit = objListSrc.begin();
        ITF_VECTOR<ObjectRef>::const_iterator send = objListSrc.end();

        for (; sit != send; ++sit)
        {
            ObjectRef objRef = *sit;
            Pickable* pickable = static_cast<Pickable*>(GETOBJECT(objRef));
            ITF_ASSERT(pickable); //must be not null;if null the object has been deleted and still used in this list..

            if ( pickable && !pickable->isDestructionRequested() )
            {
                if ( pickable->getObjectType() == BaseObject::eActor )
                {
                    if ( pickable->GetObjectClassCRC() == SubSceneActor_CRC)
                    {
                        // Skip reloading for groups
                        continue;
                    }

                    Actor* act = static_cast<Actor*>(pickable);

    #ifndef ITF_FINAL
                    if ( act->getOverrideTemplate() == NULL)
    #endif
                    {
                        reloadActors.resize(reloadActors.size()+1);
                        ActorHotReload& curData = reloadActors[reloadActors.size()-1];
                
                        act->fillHotReloadData(curData,bfalse,bfalse);
                        act->hotReloadReset(bfalse);

                        ActorsManager::ActorDataContainer* dataContainer = getSerializeDataActor(act);
                        if (dataContainer)
                        {
                            // Invalidate the checkpoint data in case a component was added/removed
                            SF_DEL(dataContainer->m_checkpoint);
                        }
                    }
                }
                else if ( pickable->getObjectType() == BaseObject::eFrise )
                {
                    Frise* frise = static_cast<Frise*>(pickable);
                    ActorsManager::FriseDataContainer* dataContainer = getSerializeDataFrise(frise);
                    if (dataContainer)
                    {
                        // Invalidate the checkpoint data in case a component was added/removed
                        SF_DEL(dataContainer->m_checkpoint);
                    }
                }
            }
        }
    }

    TEMPLATEDATABASE->reloadTemplate<Actor_Template>(_path);

    for ( u32 i = 0; i < reloadActors.size(); i++ )
    {
        ActorHotReload& hotreloadInfo = reloadActors[i];

        Actor* pActor = hotreloadInfo.m_actorPtr;

        pActor->hotReload(hotreloadInfo,Pickable::HotReloadType_DataChanged);

        if ( pActor->isSerializable() )
        {
            pActor->replaceReinitDataByCurrent();
        }
    }

    m_spawnPoolManager.onFileModified(_path);

    PLUGINGATEWAY->OnTemplateReload(_path);

 }

#ifdef ITF_SUPPORT_EDITOR
void ActorsManager::hotReloadFromFileInstance( const Path& _path )
{
    FixedArray <ActorHotReload,512> actorHotReloadList;
    FixedArray <Pickable*,512> reloadList;
    u32 numWorlds = WORLD_MANAGER->getWorldCount();
    ActorHotReload hotReloadData;

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

                if ( pick->getInstanceDataFile() == _path )
                {
                    reloadList.push_back(pick);

                    if ( pick->getObjectType() == BaseObject::eActor )
                    {
                        Actor* actor = static_cast<Actor*>(pick);

                        actor->fillHotReloadData(hotReloadData,bfalse,bfalse);
                        actorHotReloadList.push_back(hotReloadData);

                        ActorsManager::ActorDataContainer* dataContainer = getSerializeDataActor(actor);
                        if (dataContainer)
                        {
                            // Invalidate the checkpoint data in case a component was added/removed
                            SF_DEL(dataContainer->m_checkpoint);
                        }
                    }
                    else if ( pick->getObjectType() == BaseObject::eFrise )
                    {
                        Frise* frise = static_cast<Frise*>(pick);

                        ActorsManager::FriseDataContainer* dataContainer = getSerializeDataFrise(frise);
                        if (dataContainer)
                        {
                            // Invalidate the checkpoint data in case a component was added/removed
                            SF_DEL(dataContainer->m_checkpoint);
                        }
                    }

                    pick->hotReloadReset(btrue);
                }
            }
        }
    }

    u32 numReloads = reloadList.size();
    u32 actorHotReloadCount = 0;

    for ( u32 reloadIndex = 0; reloadIndex < numReloads; reloadIndex++ )
    {
        Pickable* pick = reloadList[reloadIndex];

        if ( pick->getObjectType() == BaseObject::eActor )
        {
            Actor* actor = static_cast<Actor*>(pick);

            ActorHotReload& hotReloadData = actorHotReloadList[actorHotReloadCount];

            actor->hotReload(hotReloadData,Pickable::HotReloadType_DataChanged);

            if ( actor->isSerializable() )
            {
                actor->replaceReinitDataByCurrent();
            }

            actorHotReloadCount++;
        }
        else
        {
            pick->forceReload(Pickable::HotReloadType_DataChanged);
        }
    }

    m_spawnPoolManager.onFileModified(_path);
}

void ActorsManager::hotReloadFromFileFrieze(const Path& _path)
{
    ITF_ASSERT(_path.getExtension() == "fcg");
    ITF_VECTOR<Frise*> reloadFrises;
    ITF_VECTOR<ObjectRef> objListSrc;
    
    TEMPLATEDATABASE->getTemplateClients(_path,objListSrc);

    if ( objListSrc.size() == 0 )
    {
        TEMPLATEDATABASE->reloadTemplate<FriseConfig>(_path);
        return;
    }

    if (objListSrc.size() > 0)
    {
        reloadFrises.reserve(objListSrc.size());
        ITF_VECTOR<ObjectRef>::const_iterator sit = objListSrc.begin();
        ITF_VECTOR<ObjectRef>::const_iterator send = objListSrc.end();

        for (; sit != send; ++sit)
        {
            Frise* frise = static_cast<Frise*>(GETOBJECT(*sit));
            ITF_ASSERT(frise); //must be not null;if null the object has been deleted and still used in this list..

            if (frise && !frise->isDestructionRequested())
            {
                reloadFrises.push_back(frise);
                frise->hotReloadReset(bfalse);               
            }
        }
    }

    TEMPLATEDATABASE->reloadTemplate<FriseConfig>(_path);

    for ( u32 i = 0; i < reloadFrises.size(); i++ )
    {
        Frise* frise = reloadFrises[i];
        
        frise->finalizeReload(btrue,Pickable::HotReloadType_DataChanged);

        if ( frise->hasMetaFriezeOwner() )
        {
            MetaFrieze* pMetaFrieze = DYNAMIC_CAST(frise->getMetaFriezeOwner().getObject(), MetaFrieze);
            ITF_ASSERT( pMetaFrieze );

            if ( pMetaFrieze )
            {
                pMetaFrieze->recomputeData();
            }            
        } 
    }
}

#endif //ITF_SUPPORT_EDITOR
void ActorsManager::hotReloadFromFileMetaFrieze(const Path& _path)
{
    ITF_ASSERT(_path.getExtension() == "mcg");
    ITF_VECTOR<MetaFrieze*> reloadMeta;
    ITF_VECTOR<ObjectRef> objListSrc;
    
    TEMPLATEDATABASE->getTemplateClients(_path,objListSrc);

    if ( objListSrc.size() == 0 )
    {
        TEMPLATEDATABASE->reloadTemplate<MetaFriezeConfig>(_path);
        return;
    }

    if (objListSrc.size() > 0)
    {
        reloadMeta.reserve(objListSrc.size());
        ITF_VECTOR<ObjectRef>::const_iterator sit = objListSrc.begin();
        ITF_VECTOR<ObjectRef>::const_iterator send = objListSrc.end();

        for (; sit != send; ++sit)
        {
            MetaFrieze* meta = static_cast<MetaFrieze*>(GETOBJECT(*sit));
            ITF_ASSERT(meta); //must be not null;if null the object has been deleted and still used in this list..

            if (meta && !meta->isDestructionRequested())
            {
                reloadMeta.push_back(meta);
                meta->hotReloadReset(bfalse);
            }
        }
    }

    TEMPLATEDATABASE->reloadTemplate<MetaFriezeConfig>(_path);

    for ( u32 i = 0; i < reloadMeta.size(); i++ )
    {
        MetaFrieze* meta = reloadMeta[i];
        meta->finalizeReload(btrue,Pickable::HotReloadType_DataChanged);
        PLUGINGATEWAY->onObjectChanged(meta);
    }
}

void ActorsManager::hotReloadActorsUsingResource(const Path& _path)
{
    ITF_SET<Path> actorPathList;
    ResourceID res = RESOURCE_MANAGER->findResourceSlow(_path);

    for (u32 iWorld = 0; iWorld < WORLD_MANAGER->getWorldCount(); iWorld++)
    {
        World* curWorld = WORLD_MANAGER->getWorldAt(iWorld);
        if (!curWorld)
            continue;

        for (u32 iScene = 0; iScene < curWorld->getSceneCount(); iScene++)
        {
            Scene* curScene = curWorld->getSceneAt(iScene);
            if (!curScene)
                continue;
            const ITF_VECTOR <Actor*>&  actors = curScene->getActors();
            ITF_VECTOR<ObjectRef> actorsRefs;
            actorsRefs.reserve(actors.size());
            for (u32 itActor = 0; itActor < actors.size(); itActor++)
            {
                actorsRefs.push_back(actors[itActor]->getRef());
            }
            
            for (u32 itActor = 0; itActor < actorsRefs.size(); itActor++)
            {
                Actor* pActor = static_cast<Actor*>(actorsRefs[itActor].getObject());
                if (!pActor)
                    continue;
                ResourceContainer* container = pActor->getResourceContainer();
                if (container && container->usesResource(res))
                {
                    actorPathList.insert(pActor->getTemplatePath());
                }
            }
        }
    }

    for (ITF_SET<Path> ::const_iterator iter = actorPathList.begin();
        iter != actorPathList.end();iter++)
    {
        hotReloadFromFile(*iter);
    }
}


void ActorsManager::registerActDependency( const Path& _act, const StringID& _dep )
{
    ITF_MAP<StringID,ActDependencies>::iterator it = m_actDependencies.find(_act.getStringID());

    if ( it == m_actDependencies.end() )
    {
        ActDependencies act;
        act.m_path = _act;
        act.m_dependencies.push_back(_dep);
        m_actDependencies[_act.getStringID()] = act;
    }
    else
    {
        ActDependencies& act = it->second;
        if ( act.m_dependencies.find(_dep) == -1 )
        {
            act.m_dependencies.push_back(_dep);
        }
    }
}

void ActorsManager::onModifiedFile(const Path& _path)
{
#ifdef ITF_SUPPORT_LUA
    LUACHUNKMANAGER->clearFileCache(_path);
#endif

    const String8 extension = _path.getExtension();
    ITF_VECTOR<Path> actFiles;

#ifdef ITF_SUPPORT_EDITOR
    if (extension == "atl")
    {
        hotReloadFromAtl(_path);
    }

    else if ( extension == "fcg" )
    {
        hotReloadFromFileFrieze(_path);
    }
#endif //ITF_SUPPORT_EDITOR

    if (extension == "msh")
    {
        GFXMaterialShaderManager::getInstance()->onModifiedFile(_path);
    }
    else if (extension == "tpl" || extension == "act" || extension == "frz" || extension == "mrz" )
    {
        hotReloadFromFile(_path);
    }
    else if (extension == "ilu" || extension == "tbl" || extension == "tbll")
    {
#ifdef ITF_SUPPORT_LOGICDATABASE
        const ITF_MAP<Path,LogicDataTableDependencies*> & dependenciesTables = LOGICDATABASE->getDependenciesTables();

        for (ITF_MAP<Path,LogicDataTableDependencies*>::const_iterator it = dependenciesTables.begin(); it != dependenciesTables.end(); ++it)
        {
            if (it->second->hasDependency(_path))
            {
                Path pathToAdd(it->first);
                if (pathToAdd != _path)//prevent self inclusion
                {
                    actFiles.push_back(pathToAdd);
                    continue;
                }
            }
        }

        for ( ITF_MAP<StringID,ActDependencies>::const_iterator it = m_actDependencies.begin(); it != m_actDependencies.end(); ++it )
        {
            const ActDependencies& actDep = it->second;

            if ( actDep.m_dependencies.find(_path.getStringID()) != -1 )
            {
                actFiles.push_back(actDep.m_path);
            }
        }

        for (ITF_VECTOR<Path>::const_iterator it = actFiles.begin(); it != actFiles.end(); ++it)
        {
            if(it->getExtension() == "isg")
            {
                EventSingletonConfigChanged configChanged(*it);
                EVENTMANAGER->broadcastEvent(&configChanged);
            }
            else
            {
                onModifiedFile(*it);
            }
        }
#endif //ITF_SUPPORT_LOGICDATABASE
    }
    else if (extension == "skl" ||
             extension == "anm" ||
             extension == "pbk" ||
             extension == "m3d" || 
             extension == "s3d" ||
             extension == "a3d"
             ) // resources only need to reload resource !
    {
        if ( RESOURCE_MANAGER->addResourceForCook(_path) )
        {
            hotReloadActorsUsingResource(_path);
        }
    }
    else if ( extension == "frt" )
    {
        FEEDBACKFX_MANAGER->reloadFeedbackFXManager();
    }
    else if ( extension == "mcg" )
    {
        hotReloadFromFileMetaFrieze(_path);
    }
    else if ( extension == "tsc" )
    {
        hotReloadFromSceneFile(_path);
    }
}


#ifdef ITF_SUPPORT_EDITOR
void ActorsManager::hotReloadFromAtl(const Path& _path)
{
    Path dir = _path.getDirectory();
    String8 basename = _path.getBasenameWithoutExtension();

    // try to find tga ressource.
    Path tga = _path.copyAndChangeBasename(basename + ".tga");
    ResourceID resourceId = RESOURCE_MANAGER->findResourceSlow(tga);

    if (!resourceId.isValid())
    {
        Path png = _path.copyAndChangeBasename(basename + ".png");
        resourceId = RESOURCE_MANAGER->findResourceSlow(png);
    }

    if (resourceId.isValid())
    {
#ifdef ITF_SUPPORT_COOKING
        UVATLASMANAGER->removeCookedAtlas(_path);
#endif //ITF_SUPPORT_COOKING

        Texture* pTexture = (Texture*)resourceId.getResource();
        pTexture->createAtlasIfExist();

        //check if the texture is used by some fcg
        ITF_VECTOR<const TemplateObj*> _list;
        TEMPLATEDATABASE->getTemplatesByClass(FriseConfig::GetClassCRCStatic(),_list);

        ITF_VECTOR<Path> _friseConfigMatched;

        for (ITF_VECTOR<const TemplateObj*>::const_iterator iter =_list.begin();iter!=_list.end();iter++)
        {
            const FriseConfig* pConfig = static_cast<const FriseConfig*>(*iter);
            for ( ITF_VECTOR<FriseTextureConfig>::const_iterator iterTextureConfig  = pConfig->m_textureConfigs.begin();iterTextureConfig!=pConfig->m_textureConfigs.end();iterTextureConfig++)
            {
                const FriseTextureConfig& friseTextureConfig = (*iterTextureConfig);
                if (friseTextureConfig.getGFXMaterial().getTextureResID(TEXSET_ID_DIFFUSE) == resourceId)
                {
                    _friseConfigMatched.push_back(pConfig->getFile());
                }
            }
        }

        for (ITF_VECTOR<Path>::const_iterator iterPath =_friseConfigMatched.begin();iterPath!=_friseConfigMatched.end();iterPath++)
            hotReloadFromFileFrieze(*iterPath);
    }

    
}
#endif //ITF_SUPPORT_EDITOR

static bool serializeSceneRecursive(const Path& _path, Scene* _scene)
{
    bool result = false;
#ifdef ITF_SUPPORT_XML
    CSerializerObjectParser parser;
    if ( parser.Open(_path, btrue) )
    {
        _scene->Serialize(&parser, ESerialize_Data_Load);
        _scene->setPath(_path);
        parser.Close();
        result = true;
    }

    const ITF_VECTOR <Actor*>& actors = _scene->getActors(); // Cannot use SubSceneActor list here as onLoaded was not called
    u32 count = actors.size();
    for ( u32 i = 0; i < count; i++ )
    {
        if(SubSceneActor* subsceneActor = DYNAMIC_CAST(actors[i], SubSceneActor))
        {
            Scene* subScene = newAlloc(mId_Scene, Scene);
            
            if(serializeSceneRecursive(subsceneActor->getSubScenePath(), subScene))
            {
                subsceneActor->setLoadedSubScene(subScene);
            }
            else
            {
                SF_DEL(subScene);
            }
        }
    }
#endif // ITF_SUPPORT_XML

    return result;
}

void ActorsManager::hotReloadFromSceneFile( const Path& _path )
{
#ifdef ITF_SUPPORT_XML
    const u32 worldCount = WORLD_MANAGER->getWorldCount();
    for(u32 iW = 0; iW < worldCount; ++iW)
    {
        World* world = WORLD_MANAGER->getWorldAt(iW);

        if(world->isSceneAsyncLoadRunning())
            continue;

        Scene* rootScene = world->getRootScene();
        ITF_VECTOR <SubSceneActor*> ssaList = rootScene->getSubSceneActors();

        while(ssaList.size() > 0)
        {
            SubSceneActor* ssa = ssaList[0];
            ssaList.removeAt(0);
            
            if(Scene* subscene = ssa->getSubScene())
            {
                if(ssa->getSubScenePath() == _path)
                {
                    bbool wasActive = subscene->isActive();
                    if(wasActive)
                    {
                        subscene->setContentInactive();
                        subscene->onSceneInactive();
                    }

                    PickableList content = subscene->getPickableList();
                    for(u32 i = 0; i < content.size(); ++i)
                        subscene->unregisterPickable(content[i]);
                    subscene->flushPending(btrue, btrue);
                    subscene->clear();

                    serializeSceneRecursive(ssa->getSubScenePath(), subscene);

                    subscene->onLoaded();
                    subscene->onFinalizeLoad();

                    if(wasActive)
                        subscene->onSceneActive();
                }
                else
                {
                    ssaList.insert(ssaList.end(), subscene->getSubSceneActors().begin(), subscene->getSubSceneActors().end());
                }
            }
        }
    }
#endif // ITF_SUPPORT_XML
}

#endif //ITF_SUPPORT_HOTRELOAD_TEMPLATE

#if defined(ITF_SUPPORT_EDITOR) || defined(ITF_SUPPORT_COOKING)
Pickable* ActorsManager::getInstanceDataFile(const Path &_filename, bbool _forceLoad, const SerializedObjectContent_Object** _content)
{
	csAutoLock cs(m_cs);
    InstanceDataCacheEntry entry;
    InstanceDataCacheMap::iterator it = m_instanceDataCache.find(_filename);

    if(it != m_instanceDataCache.end())
    {
        if(_forceLoad)
        {
            destroyInstanceDataCacheEntry(it->second);
            it->second = loadInstanceDataFile(_filename);
        }

        entry = it->second;
    }
    else
    {
        entry = loadInstanceDataFile(_filename);
        m_instanceDataCache[_filename] = entry;
    }

    if(_content)
        *_content = entry.m_content;

    return entry.m_instanceDataObj;
}

ActorsManager::InstanceDataCacheEntry ActorsManager::loadInstanceDataFile(const Path &_filename)
{
    InstanceDataCacheEntry entry = { 0 };

    CSerializerObjectLua serializer;

    if(serializer.OpenToRead(_filename))
    {
        serializer.SerializeObject("params",entry.m_instanceDataObj, &BASEOBJECT_FACTORY->m_objectFactory, ESerialize_Data_Load | ESerialize_InstanceLoad);
        serializer.Close();

        if(entry.m_instanceDataObj)
        {
            entry.m_content = newAlloc(mId_Serialization, SerializedObjectContent_Object());

            if ( serializer.getContent()->getNumObjects() )
            {
                entry.m_content->copyFrom(*serializer.getContent()->getObject(0));
            }

            // No need to register the cached object in the IDServer. 
            entry.m_instanceDataObj->unregisterPickable();
        }
    }

    return entry;
}

void ActorsManager::destroyInstanceDataCacheEntry(InstanceDataCacheEntry &_entry)
{
    SF_DEL(_entry.m_instanceDataObj);
    SF_DEL(_entry.m_content);
}
#endif // ITF_SUPPORT_EDITOR || ITF_SUPPORT_COOKING

void ActorsManager::registerSerializeDataActor(Actor * _actor)
{
	csAutoLock cs(m_cs);
    ITF_ASSERT_MSG(_actor, "trying to registerSerializeDataActor a null actor");

    if (!_actor->isSerializable())
        return;

    ActorDataContainerMap::iterator it = m_serializeActorData.find(_actor->getRef());
    if (it == m_serializeActorData.end())
    {
        ActorDataContainer actorData;
        
        actorData.m_instance = newAlloc(mId_Editor,ArchiveMemory);
        CSerializerObjectBinary serializeBin;
        serializeBin.Init(actorData.m_instance);
        _actor->Serialize(&serializeBin,ESerialize_Data_Save);

        if (!actorData.m_checkpoint)
        {
            actorData.m_checkpoint = newAlloc(mId_SavegameManager,ArchiveMemory);
            actorData.m_checkpoint->reserve(128);//it's the average size met in levels,avoid to allocate in the first checkpoint
        }

        m_serializeActorData[_actor->getRef()] = actorData;
      

    }
}

void ActorsManager::registerSerializeDataFrise(Frise* _frise)
{
	csAutoLock cs(m_cs);
    ITF_ASSERT_MSG(_frise, "trying to registerSerializeDataActor a null frise");

    FriseDataContainerMap::iterator it = m_serializeFriseData.find(_frise->getRef());
    if (it == m_serializeFriseData.end())
    {
        FriseDataContainer friseData;

        if (!friseData.m_checkpoint)
        {
            friseData.m_checkpoint = newAlloc(mId_SavegameManager,ArchiveMemory);
            friseData.m_checkpoint->reserve(28);//it's the average size met in levels,avoid to allocate in the first checkpoint
        }

        m_serializeFriseData[_frise->getRef()] = friseData;
        

    }
}

void ActorsManager::unregisterSerializeDataActor(Actor* _actor)
{
	csAutoLock cs(m_cs);
    ITF_ASSERT_MSG(_actor, "trying to unregister a null actor");

    if (!_actor->isSerializable())
        return;

    ActorDataContainerMap::iterator it = m_serializeActorData.find(_actor->getRef());
    ITF_ASSERT_MSG(it != m_serializeActorData.end(),"Actor %s already unregistered ",_actor->getUserFriendly().cStr());
    if (it != m_serializeActorData.end())
    {
        it->second.dispose();
        m_serializeActorData.erase(it);
    }
}

void ActorsManager::unregisterSerializeDataFrise(Frise* _frise)
{
	csAutoLock cs(m_cs);
    ITF_ASSERT_MSG(_frise, "trying to unregister a null actor");

    FriseDataContainerMap::iterator it = m_serializeFriseData.find(_frise->getRef());
    ITF_ASSERT_MSG(it != m_serializeFriseData.end(),"Frise %s already unregistered ",_frise->getUserFriendly().cStr());
    if (it != m_serializeFriseData.end())
    {
        it->second.dispose();
        m_serializeFriseData.erase(it);
    }
}

ActorsManager::ActorDataContainer* ActorsManager::getSerializeDataActor(Actor * _actor)
{
	csAutoLock cs(m_cs);
    ITF_ASSERT_MSG(_actor, "trying to getSerializeDataActor a null actor");

    ActorDataContainerMap::iterator it = m_serializeActorData.find(_actor->getRef());
    if (it!= m_serializeActorData.end())
    {
        return &it->second;
    }
    return NULL;
}

ActorsManager::FriseDataContainer* ActorsManager::getSerializeDataFrise(Frise* _frise)
{
	csAutoLock cs(m_cs);
    ITF_ASSERT_MSG(_frise, "trying to getSerializeDataActor a null frise");

    FriseDataContainerMap::iterator it = m_serializeFriseData.find(_frise->getRef());
    if (it!= m_serializeFriseData.end())
    {
        return &it->second;
    }
    return NULL;
}

bbool ActorsManager::isActorRegistered( Actor * _actor ) const
{
	csAutoLock cs(m_cs);
    ITF_ASSERT_MSG(_actor, "trying to isActorRegistered a null actor");
    return m_serializeActorData.find(_actor->getRef()) != m_serializeActorData.end();
}

bbool ActorsManager::isFriseRegistered( Frise* _frise ) const
{
	csAutoLock cs(m_cs);
    ITF_ASSERT_MSG(_frise, "trying to isActorRegistered a null frise");
    return m_serializeFriseData.find(_frise->getRef()) != m_serializeFriseData.end();
}

}
