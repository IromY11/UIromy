#include "precompiled_engine.h"

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_GAMEMATERIALSMANAGER_H_
#include "gameplay/managers/GameMaterialManager.h"
#endif //_ITF_GAMEMATERIALSMANAGER_H_

#ifndef _ITF_GAMEINTERFACE_H_
#include "gameplay/GameInterface.h"
#endif //_ITF_GAMEINTERFACE_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#define DESCRIPTOR_POOL_MAX_SIZE	128

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    //	FXCONTROL
    ///////////////////////////////////////////////////////////////////////////////////////////

    BEGIN_SERIALIZATION(FXControl)
        SERIALIZE_MEMBER("name",m_name);
        SERIALIZE_MEMBER("fxStopOnEndAnim",m_fxStopOnEndAnim);
        SERIALIZE_MEMBER("fxPlayOnce",m_fxPlayOnce);
		SERIALIZE_BOOL("pickColorFromFreeze",m_pickColorFromFreeze);
        SERIALIZE_MEMBER("fxInstanceOnce",m_fxInstanceOnce);
        SERIALIZE_MEMBER("fxEmitFromBase",m_fxEmitFromBase);
        SERIALIZE_MEMBER("fxUseActorSpeed",m_fxUseActorSpeed);
        SERIALIZE_MEMBER("fxUseActorOrientation",m_fxUseActorOrientation);
        SERIALIZE_MEMBER("fxUseActorAlpha",m_fxUseActorAlpha);
        SERIALIZE_MEMBER("fxBoneName",m_fxBoneName);
        SERIALIZE_ENUM_BEGIN("fxUseBoneOrientation",m_fxUseBoneOrientation);
            SERIALIZE_ENUM_VAR(BOOL_false );
            SERIALIZE_ENUM_VAR(BOOL_true);
            SERIALIZE_ENUM_VAR(BOOL_cond);
        SERIALIZE_ENUM_END();
        SERIALIZE_CONTAINER("sounds",m_sounds);
        SERIALIZE_CONTAINER("particles",m_particles);
        SERIALIZE_CONTAINER("fluids",m_fluids);
        SERIALIZE_MEMBER("music", m_music);
        SERIALIZE_MEMBER("busMix", m_busMix);
        SERIALIZE_MEMBER("owner",m_owner);
        SERIALIZE_BOOL("busMixActivate",m_busMixActivate);
        SERIALIZE_BOOL("fxDontStopSound", m_fxDontStopSound);
        SERIALIZE_BOOL("fxAttach", m_fxAttach);
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////

    void FXControl::init()
    {
        m_fxFlags           = 0;

        for (u32 i = 0; i < m_sounds.size(); ++i)
        {
            if (m_sounds[i].isValid())
            {
                m_fxFlags |= FXControl::flag_sound;
                break;
            }
        }

        for (u32 i = 0; i < m_particles.size(); ++i)
        {
            if (m_particles[i].isValid())
            {
                m_fxFlags |= FXControl::flag_particle;
                break;
            }
        }

        for (u32 i = 0; i < m_fluids.size(); ++i)
        {
            if (m_fluids[i].isValid())
            {
                m_fxFlags |= FXControl::flag_fluid;
                break;
            }
        }

        if ( m_music.isValid() )
        {
            m_fxFlags |= FXControl::flag_music;
        }

        if ( m_busMix.isValid() )
        {
            m_fxFlags |= FXControl::flag_busMix;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //	ACTIONMAP
    ///////////////////////////////////////////////////////////////////////////////////////////

    BEGIN_SERIALIZATION(Action)
        SERIALIZE_CONTAINER_OBJECT("actions",m_actions);
    END_SERIALIZATION()
    Action::Action()
    {}

    Action::~Action()
    {}

    ///////////////////////////////////////////////////////////////////////////////////////////

    FXControl* Action::getFeedbacks(const StringID& _action)
    {
        ITF_MAP<StringID, FXControl>::iterator action_it = m_actions.find(_action);
        if ( action_it == m_actions.end() )
            return NULL;

        return &(action_it->second);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    const FXControl * Action::getFeedbacks(const StringID& _action) const
    {
        ITF_MAP<StringID, FXControl>::const_iterator action_it = m_actions.find(_action);
        if ( action_it == m_actions.end() )
            return NULL;

        return &(action_it->second);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //	TARGETMAP
    ///////////////////////////////////////////////////////////////////////////////////////////
    BEGIN_SERIALIZATION(Target)
        SERIALIZE_CONTAINER_OBJECT("targets",m_targets);
    END_SERIALIZATION()
    Target::Target()
    {}

    Target::~Target()
    {}

    ///////////////////////////////////////////////////////////////////////////////////////////

    Action* Target::getActions(const StringID& _target)
    {
        ITF_MAP<StringID, Action>::iterator target_it = m_targets.find(_target);
        if ( target_it == m_targets.end() )
            return NULL;

        return &(target_it->second);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    const Action* Target::getActions (const StringID& _target) const
    {
        ITF_MAP<StringID, Action>::const_iterator target_it = m_targets.find(_target);
        if ( target_it == m_targets.end() )
            return NULL;

        return &(target_it->second);
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    //  FEEDBACK FX MANAGER TEMPLATE
    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(FeedbackFXManager_Template)

    BEGIN_SERIALIZATION_CHILD(FeedbackFXManager_Template)
        SERIALIZE_CONTAINER_OBJECT("soundDescriptors",m_soundDescriptorTemplates);
        SERIALIZE_CONTAINER_OBJECT("FxDescriptors",m_FxDescriptorTemplates);
        SERIALIZE_CONTAINER_OBJECT("actors",m_actors);
        SERIALIZE_CONTAINER_OBJECT("busList",m_busList);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(FeedbackFXManager_Template,buses)
        SERIALIZE_MEMBER("actorType",m_actorType);
        SERIALIZE_MEMBER("bus",m_bus);
    END_SERIALIZATION()

    FeedbackFXManager_Template::~FeedbackFXManager_Template()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void FeedbackFXManager_Template::onTemplateDelete( bbool _hotReload )
    {
        for (u32 i = 0 ; i < m_soundDescriptorTemplates.size() ; i++)
        {
            m_soundDescriptorTemplates[i].onTemplateDelete();
        }

        Super::onTemplateDelete(_hotReload);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    bbool FeedbackFXManager_Template::onTemplateLoaded( bbool _hotReload )
    {
        bbool ret =  Super::onTemplateLoaded(_hotReload);

        for (u32 i = 0 ; i < m_soundDescriptorTemplates.size() ; i++)
        {
            m_soundDescriptorTemplates[i].onTemplateLoaded();
        }

        return ret;
    }

    void FeedbackFXManager_Template::setLoadedInPlace()
    {
        Super::setLoadedInPlace();
#ifdef ITF_SUPPORT_RAKI
        for (u32 i = 0 ; i < m_soundDescriptorTemplates.size() ; i++)
        {
            m_soundDescriptorTemplates[i].setLoadedInPlace();
        }
#endif
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //  FEEDBACK FX MANAGER
    ///////////////////////////////////////////////////////////////////////////////////////////

    FeedbackFXManager::FeedbackFXManager ()
    : m_template(NULL)
#ifdef ITF_SUPPORT_EDITOR
    , m_debugRules(bfalse)
#endif // ITF_SUPPORT_EDITOR
    {
        m_soundDescPool.resize(DESCRIPTOR_POOL_MAX_SIZE);
        m_FxDescPool.resize(DESCRIPTOR_POOL_MAX_SIZE);

        m_defaultMaterial   = StringID::StrToCRC("DefaultMaterial");
        m_defaultActor      = StringID::StrToCRC("DefaultActor");
    }

    FeedbackFXManager::~FeedbackFXManager ()
    {
        if (TEMPLATEDATABASE && m_template)
        {
            TEMPLATEDATABASE->releaseTemplate(m_template->getFile());
        }

        m_actionsPerActors.clear();
        m_actionsPerTargets.clear();
        m_owners.clear();
        m_actors.clear();
        m_SDTemplateMap.clear();
        m_FXTemplateMap.clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

#ifdef ITF_SUPPORT_HOTRELOAD_TEMPLATE

    void FeedbackFXManager::reloadFeedbackFXManager()
    {
        // JP : TEMP until I fix it definitely !
        return;

        //// get all previous owner (before reloading file)
        //ITF_VECTOR<StringID>    ownerListToReload;
        //ownerListToReload.reserve(m_owners.size());

        //OwnerMap::iterator  owner_it = m_owners.begin();
        //OwnerMap::iterator  owner_it_end = m_owners.end();

        //for ( ; owner_it != owner_it_end ; ++owner_it )
        //{
        //    ownerListToReload.push_back(owner_it->first);
        //}

        //// Reload file
        //ITF_ASSERT(m_template);
        //m_template = TEMPLATEDATABASE->reloadTemplate<FeedbackFXManager_Template>(m_template->getFile());

        //// Clear bunch of data
        //m_actors.clear();
        //m_busMap.clear();
        //m_owners.clear();

        //m_actors = m_template->getActorsMap(); // copy

        //const ITF_VECTOR<FeedbackFXManager_Template::buses>& busList = m_template->getBusList();
        //for( u32 i = 0 ; i < busList.size() ; ++i )
        //{
        //    m_busMap[busList[i].m_actorType] = busList[i].m_bus;
        //}

        //// InitInternal will fill in all arrays needed (including m_owners map)
        //initInternal();

        //// Check owner differences (changes done in config file may have changed owners : some may not be owner anymore
        //// and some may have been added. Both of them need to be reloaded).
        //owner_it      = m_owners.begin();
        //owner_it_end  = m_owners.end();

        //bbool bfound = bfalse;

        //for ( ; owner_it != owner_it_end ; ++owner_it )
        //{
        //    bfound = bfalse;
        //    u32 listSize = ownerListToReload.size();
        //    for ( u32 i = 0 ; i < listSize && !bfound ; ++i )
        //    {
        //        if ( ownerListToReload[i] == owner_it->first )
        //            bfound = btrue;
        //    }

        //    // New owner found ?
        //    if ( !bfound )
        //        ownerListToReload.push_back(owner_it->first);
        //}

        //// Get list of actors that needs to be reloaded
        //ITF_VECTOR<ActorRef> actorsToReload;
        //ActorsManager::ActorDataContainerMap::iterator actorRef_it      = ACTORSMANAGER->getActorDataContainerMapStart();
        //ActorsManager::ActorDataContainerMap::iterator actorRef_itEnd   = ACTORSMANAGER->getActorDataContainerMapEnd();
        //for ( ; actorRef_it != actorRef_itEnd ; actorRef_it++ )
        //{
        //    Actor * actor = static_cast<Actor*>(actorRef_it->first.getActor());
        //    if ( actor )
        //    {
        //        FXControllerComponent * fxControllerCmp = actor->GetComponent<FXControllerComponent>();
        //        if ( fxControllerCmp )
        //        {
        //            const ITF_VECTOR<StringID>& actorTags = fxControllerCmp->getFeedbackTags();
        //            bfound = bfalse;
        //            ITF_ASSERT_CRASH(actorTags.size()>0, "List of tags is empty, check this !");
        //            // Last element of actorTags is default type which SHOULD NEVER be a owner -> no need to test it then
        //            for ( u32 i = 0 ; i < (actorTags.size()-1) && !bfound; i++ )
        //            {
        //                for ( u32 j = 0 ; j < ownerListToReload.size() && !bfound; j++ )
        //                {
        //                    if ( actorTags[i] == ownerListToReload[j] )
        //                        bfound = btrue;

        //                    if ( bfound )
        //                        actorsToReload.push_back(actorRef_it->first);
        //                }
        //            }
        //        }
        //    }
        //}

        //// Reload actors
        //for ( u32 i = 0 ; i < actorsToReload.size() ; ++i )
        //{
        //    Actor * actor = static_cast<Actor*>(actorsToReload[i].getActor());

        //    if ( actor )
        //    {
        //        ACTORSMANAGER->onModifiedFile(actor->getTemplate()->getFile());
        //    }
        //}

        //// Get game materials to reload
        //KeyArray<const GameMaterial_Template *> gameMaterials = GAMEMATERIAL_MANAGER->getGameMaterials();
        //for ( u32 i = 0 ; i < gameMaterials.size() ; i++ )
        //{
        //    ITF_ASSERT_CRASH(gameMaterials.getAt(i), "GameMaterial_Template list contains a NULL element !");

        //    bfound = bfalse;
        //    for ( u32 j = 0 ; j < ownerListToReload.size() && !bfound ; ++j )
        //    {
        //        if ( gameMaterials.getAt(i)->getType() == ownerListToReload[j]
        //            || gameMaterials.getAt(i)->getArchetype() == ownerListToReload[j] )
        //        {
        //            TEMPLATEDATABASE->reloadTemplate<GameMaterial_Template>(gameMaterials.getAt(i)->getFile());
        //        }
        //    }
        //}

        //// Clear data
        //ownerListToReload.clear();
        //actorsToReload.clear();
    }
    
#endif // ITF_SUPPORT_HOTRELOAD_TEMPLATE

    ///////////////////////////////////////////////////////////////////////////////////////////

    void FeedbackFXManager::init()
    {
        const Path FRTFile(GETPATH_ALIAS("feedbacks"));

        if ( !FRTFile.isEmpty() )
        {
            m_template = TEMPLATEDATABASE->requestTemplate<FeedbackFXManager_Template>(FRTFile);

            if ( m_template )
            {
                m_actors = m_template->getActorsMap(); // copy

                const ITF_VECTOR<FeedbackFXManager_Template::buses>& busList = m_template->getBusList();
                for( u32 i = 0 ; i < busList.size() ; ++i )
                {
                    m_busMap[busList[i].m_actorType] = busList[i].m_bus;
                }
            }
        }

        initInternal();
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void FeedbackFXManager::acquireExtraResources( const ITF_VECTOR<StringID>& _actorTags, ITF_VECTOR<const GFX_MATERIAL *> & _materials, ITF_VECTOR<Path>& _sounds)
    {
        // Gather resources that this archetype is the owner
        ITF_ASSERT_CRASH(_actorTags.size() > 0, "List of tags is empty, check this !");
        for ( u32 i = 0 ; i < (_actorTags.size()-1) ; i++ )
        {
            FeedbackFXManager::OwnerMap::const_iterator owner_it = m_owners.find(_actorTags[i]);

            if ( owner_it != m_owners.end() )
            {
                FeedbackFXManager::FXControls::const_iterator fxControl_it = owner_it->second.begin();
                for ( ; fxControl_it != owner_it->second.end() ; fxControl_it++ )
                {
#ifdef ITF_SUPPORT_RAKI
                    getExtraSounds(*fxControl_it, _sounds);
#endif
                    acquireExtraMaterials(*fxControl_it, _materials);
                }
            }
        }
    }

    void FeedbackFXManager::releaseExtraResources( const ITF_VECTOR<StringID>& _actorTags)
    {
        // Gather resources that this archetype is the owner
        ITF_ASSERT_CRASH(_actorTags.size() > 0, "List of tags is empty, check this !");
        for ( u32 i = 0 ; i < (_actorTags.size()-1); i++ )
        {
            FeedbackFXManager::OwnerMap::const_iterator owner_it = m_owners.find(_actorTags[i]);

            if ( owner_it != m_owners.end() )
            {
                FeedbackFXManager::FXControls::const_iterator fxControl_it = owner_it->second.begin();
                for ( ; fxControl_it != owner_it->second.end() ; fxControl_it++ )
                {
                    releaseExtraMaterials(*fxControl_it);
                }
            } 
        }       
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    bbool FeedbackFXManager::hasFeedback( const StringID& _actor
                                        , const StringID& _actorArchetype
                                        , const StringID& _defaultActor
                                        , const StringID& _action
                                        , const StringID& _target
                                        , const StringID& _targetArchetype
                                        , const StringID& _defaultTarget)
    {
        // actor vs target
        bbool ret = hasFeedbackImpl(_actor, _action, _target);
        // actor archetype vs target
        if (!ret)
            ret = hasFeedbackImpl(_actorArchetype, _action, _target);
        // actor vs target archetype
        if (!ret)
            ret = hasFeedbackImpl(_actor, _action, _targetArchetype);
        // actor archetype vs target archetype
        if (!ret)
            ret = hasFeedbackImpl(_actorArchetype, _action, _targetArchetype);

        // TESTING DEFAULT
        // actor vs default target
        if (!ret)
            ret = hasFeedbackImpl(_actor, _action, _defaultTarget);
        // actor archetype vs default target
        if (!ret)
            ret = hasFeedbackImpl(_actorArchetype, _action, _defaultTarget);
        // default actor vs target
        if (!ret)
            ret = hasFeedbackImpl(_defaultActor, _action, _target);
        // default actor vs target archetype
        if (!ret)
            ret = hasFeedbackImpl(_defaultActor, _action, _targetArchetype);
        // default actor vs default target
        if (!ret)
            ret = hasFeedbackImpl(_defaultActor, _action, _defaultTarget);

        return ret;
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////

    bbool FeedbackFXManager::hasFeedbackImpl( const StringID& _actor, const StringID& _action, const StringID& _target )
    {
        if ( _actor == StringID::Invalid || _action == StringID::Invalid ||_target == StringID::Invalid )
            return bfalse;

        FeedbackFXManager::ActionsPerActorMap::const_iterator actor_it = m_actionsPerActors.find(_actor);
        if (actor_it != m_actionsPerActors.end())
        {
            FeedbackFXManager::ActionsPerTargetMap::const_iterator target_it = m_actionsPerTargets.find(_target);
            if (target_it != m_actionsPerTargets.end())
            {
                // Find action in actor's action list
                bbool bFoundActionInActor = bfalse;
                for (u32 i = 0 ; i < actor_it->second.size() && !bFoundActionInActor ; ++i)
                {
                    if ( actor_it->second[i] == _action )
                        bFoundActionInActor = btrue;
                }

                // Find action in target's action list
                bbool bFoundActionInTarget = bfalse;
                for (u32 i = 0 ; i < target_it->second.size() && !bFoundActionInTarget ; ++i)
                {
                    if ( target_it->second[i] == _action )
                        bFoundActionInTarget = btrue;
                }

                return (bFoundActionInActor && bFoundActionInTarget);
            }
        }

        return bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    const FXControl * FeedbackFXManager::getFeedback( const ITF_VECTOR<StringID>& _actorTags
                                                    , const StringID& _action
                                                    , const ITF_VECTOR<StringID>& _targetTags)
    {
        const FXControl * ret = NULL;
        ITF_ASSERT(_actorTags.size() > 0 && _targetTags.size() > 0);

        if( _actorTags.size() > 0 && _targetTags.size() > 0 )
        {
            // Check actor Tags VS target Tags
            u32 actorTagsSizeWithoutDefault = _actorTags.size()-1;
            u32 targetTagsSizeWithoutDefault = _targetTags.size()-1;
            for ( u32 i = 0 ; i < targetTagsSizeWithoutDefault && !ret ; i++ )
            {
                for ( u32 j = 0 ; j < actorTagsSizeWithoutDefault && !ret ; j++ )
                {
                    if ( hasFeedbackImpl(_actorTags[actorTagsSizeWithoutDefault-1-j],_action,_targetTags[targetTagsSizeWithoutDefault-1-i]) )
                    {
                        ret = getFeedbackImpl(_actorTags[actorTagsSizeWithoutDefault-1-j],_action,_targetTags[targetTagsSizeWithoutDefault-1-i]);

#ifdef ITF_SUPPORT_EDITOR
                        if ( ret )
                            storeLastRule(_actorTags[actorTagsSizeWithoutDefault-1-j],_action,_targetTags[targetTagsSizeWithoutDefault-1-i],ret->m_name);
#endif // ITF_SUPPORT_EDITOR
                    }
                }
            }

            // Check actor Tags VS default target Tag
            const StringID& actorDefaultType = _actorTags.back();
            const StringID& targetDefaultType = _targetTags.back();
            for ( u32 i = 0 ; i < actorTagsSizeWithoutDefault && !ret ; i++ )
            {
                if ( hasFeedbackImpl(_actorTags[actorTagsSizeWithoutDefault-1-i],_action,targetDefaultType) )
                {
                    ret = getFeedbackImpl(_actorTags[actorTagsSizeWithoutDefault-1-i],_action,targetDefaultType);

#ifdef ITF_SUPPORT_EDITOR
                    if ( ret )
                        storeLastRule(_actorTags[actorTagsSizeWithoutDefault-1-i],_action,targetDefaultType,ret->m_name);
#endif // ITF_SUPPORT_EDITOR
                }
            }

            // Check default actor Tag VS target Tags
            for ( u32 i = 0 ; i < targetTagsSizeWithoutDefault && !ret ; i++ )
            {
                if ( hasFeedbackImpl(actorDefaultType,_action,_targetTags[targetTagsSizeWithoutDefault-1-i]) )
                {
                    ret = getFeedbackImpl(actorDefaultType,_action,_targetTags[targetTagsSizeWithoutDefault-1-i]);

#ifdef ITF_SUPPORT_EDITOR
                    if ( ret )
                        storeLastRule(actorDefaultType,_action,_targetTags[targetTagsSizeWithoutDefault-1-i],ret->m_name);
#endif // ITF_SUPPORT_EDITOR
                }
            }

            // Default VS default, last chance to find something
            if (!ret && hasFeedbackImpl(actorDefaultType, _action, targetDefaultType))
            {
                ret = getFeedbackImpl(actorDefaultType, _action, targetDefaultType);

#ifdef ITF_SUPPORT_EDITOR
                if ( ret )
                    storeLastRule(actorDefaultType,_action,targetDefaultType,ret->m_name);
#endif // ITF_SUPPORT_EDITOR
            }
        }

#ifdef ITF_SUPPORT_EDITOR
        if ( !ret )
        {
            resetLastRule();
            m_lastActorTags.clear();
            m_lastTargetTags.clear();
            m_lastAction.invalidate();
            for (u32 i = 0 ; i < _actorTags.size() ; i++)
            {
                m_lastActorTags.push_back(_actorTags[i]);
            }
            for (u32 i = 0 ; i < _targetTags.size() ; i++)
            {
                m_lastTargetTags.push_back(_targetTags[i]);
            }
            m_lastAction = _action;
        }
#endif // ITF_SUPPORT_EDITOR

        return ret;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    const FXControl* FeedbackFXManager::getFeedbackImpl( const StringID& _actor
                                                        , const StringID& _action
                                                        , const StringID& _target )
    {
        // Get targets associated to given actor
        FeedbackFXManager::ActorMap::iterator actor_it = m_actors.find(_actor);
        if (actor_it == m_actors.end())
            return NULL;

        Target* pTarget = &(actor_it->second);

        // Get targets associated to given actor
        if (!pTarget)
            return NULL;

        // Get actions associated to given target
        Action* pAction = pTarget->getActions(_target);
        if (!pAction)
            return NULL;

        // Get list of feedbacks associated to given action
        return pAction->getFeedbacks(_action);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    SoundDescriptor * FeedbackFXManager::getNewSoundDescriptor( const Actor * _owner, const SoundDescriptor_Template * _soundDesc, class SoundComponent * _component)
    {
        if ( !(_owner && _soundDesc) )
            return NULL;

        ITF_ASSERT_MSG( m_soundDescPool.size() > 0,"Sound descriptor pool has not been correctly initialized !");

        // Look for a free slot in sound descriptor pool
        for ( u32 i = 0 ; i < m_soundDescPool.size() ; ++i )
        {
            if ( m_soundDescPool[i].m_isFree )
            {
                m_soundDescPool[i].m_isFree		= false;
                m_soundDescPool[i].m_owner		= _owner;
                // Associate template to this descriptor so that it can have access to resources already loaded
                if ( m_soundDescPool[i].m_descriptor.init(_soundDesc, _component) )
                    return &(m_soundDescPool[i].m_descriptor);
                return NULL;
            }
        }

        ITF_ASSERT_MSG( 0,"Found no free slot in sound descriptor pool. Need to increase its size ?");

        return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void FeedbackFXManager::releaseSoundDescriptors( const Actor * _owner, SoundDescriptor * _soundDesc )
    {
        for ( u32 i = 0 ; i < m_soundDescPool.size() ; ++i )
        {
            if ( !m_soundDescPool[i].m_isFree
                && m_soundDescPool[i].m_owner == _owner
                && ( (_soundDesc && &(m_soundDescPool[i].m_descriptor) == _soundDesc)
                    || !_soundDesc ) )
            {
                releaseSoundDescriptor(i);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void FeedbackFXManager::releaseSoundDescriptor(u32 _index)
    {
        ITF_ASSERT_MSG(_index <  m_soundDescPool.size(), "Index out of range !");

        m_soundDescPool[_index].m_isFree	= true;
        m_soundDescPool[_index].m_owner		= NULL;
        m_soundDescPool[_index].m_descriptor.uninit();

        //ITF_VECTOR<ResourceID>& descResourceList = m_soundDescPool[_index].m_descriptor.getResourceList();
        //descResourceList.clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    FxDescriptor * FeedbackFXManager::getNewFxDescriptor( const Actor * _owner, const FxDescriptor_Template * _FxDesc)
    {
        if ( !(_owner && _FxDesc) )
            return NULL;

        ITF_ASSERT_MSG( m_FxDescPool.size() > 0,"FX descriptor pool has not been correctly initialized !");

            // Look for a free slot in sound descriptor pool
            for ( u32 i = 0 ; i < m_FxDescPool.size() ; ++i )
            {
                if ( m_FxDescPool[i].m_isFree )
                {
                    m_FxDescPool[i].m_isFree	= false;
                    m_FxDescPool[i].m_owner		= _owner;
                    
                    // Associate template to this descriptor so that it can have access to resources already loaded
                    m_FxDescPool[i].m_descriptor.onActorLoaded(_FxDesc);

                    return &(m_FxDescPool[i].m_descriptor);
                }
            }

            ITF_ASSERT_MSG( 0,"Found no free slot in sound descriptor pool. Need to increase its size ?");

                return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void FeedbackFXManager::releaseFxDescriptors( const Actor * _owner, FxDescriptor * _FxDesc )
    {
        for ( u32 i = 0 ; i < m_FxDescPool.size() ; ++i )
        {
            if ( !m_FxDescPool[i].m_isFree
                && m_FxDescPool[i].m_owner == _owner
                && ( (_FxDesc && &(m_FxDescPool[i].m_descriptor) == _FxDesc)
                || !_FxDesc ) )
            {
                releaseFxDescriptor(i);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void FeedbackFXManager::releaseFxDescriptor(u32 _index)
    {
        ITF_ASSERT_MSG(_index <  m_FxDescPool.size(), "Index out of range !");

        m_FxDescPool[_index].m_isFree	= true;
        m_FxDescPool[_index].m_owner	= NULL;
        m_FxDescPool[_index].m_descriptor.clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void FeedbackFXManager::initInternal()
    {
        m_SDTemplateMap.clear();
        m_FXTemplateMap.clear();

        if ( m_template )
        {
            // Init Descriptors templates maps
            ITF_VECTOR<SoundDescriptor_Template>::const_iterator sound_it = m_template->getSDTemplates().begin();
            u32 i = 0;
            for ( ; sound_it != m_template->getSDTemplates().end() ; ++sound_it )
            {
                const StringID& soundDescID = sound_it->getName();

                ITF_ASSERT(soundDescID.isValid());

                if ( soundDescID.isValid() )
                {
                    // Update sound descriptor template map
                    m_SDTemplateMap[soundDescID] = i;
                }
                ++i;
            }

            i = 0;
            for (u32 n = m_template->getNbFXTemplate();
                 i!=n;
                 ++i)
            {
                const StringID& FXDescID = m_template->getFXTemplate(i).GetName();

                ITF_ASSERT(FXDescID.isValid());
                if ( FXDescID.isValid() )
                {
                    // Update FX descriptor template map
                    m_FXTemplateMap[FXDescID] = RefCountedTemplateID(i);
                }
            }
        }

        updateInternal();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void FeedbackFXManager::updateInternal()
    {
        FeedbackFXManager::ActorMap::iterator actor_it = m_actors.begin();

        // For each actors
        for (  ; actor_it != m_actors.end() ; actor_it++ )
        {
            ActionIDs& actionPerActorIDs = m_actionsPerActors[actor_it->first];

            Target::TargetMap* pFRTTargetMap	= actor_it->second.getTargetMap();

            // for each targets
            Target::TargetMap::iterator target_it = pFRTTargetMap->begin();
            for (  ; target_it != pFRTTargetMap->end() ; target_it++ )
            {
                ActionIDs& actionPerTargetIDs = m_actionsPerTargets[target_it->first];

                Action::ActionMap* pFRTActionMap	= target_it->second.getActionMap();

                // for each actions
                Action::ActionMap::iterator action_it = pFRTActionMap->begin();
                for ( ; action_it != pFRTActionMap->end() ; action_it++ )
                {
                    u32 i = 0;
                    bbool b_actorFound = false;
                    // add actor to action per actor map if it's not already done
                    for ( i = 0 ; i < actionPerActorIDs.size() ; i++ )
                    {
                        if ( actionPerActorIDs[i] == action_it->first )
                        {
                            b_actorFound = true;
                            // This action is already referenced, no need to continue then
                            break;
                        }
                    }
                    // This action is not referenced for this actor yet
                    if ( !b_actorFound )
                    {
                        actionPerActorIDs.push_back(action_it->first);
                    }

                    bbool b_targetFound = false;
                    // add actor to action per target map if it's not already done
                    for ( i = 0 ; i < actionPerTargetIDs.size() ; i++ )
                    {
                        if ( actionPerTargetIDs[i] == action_it->first )
                        {
                            b_targetFound = true;
                            // This action is already referenced, no need to continue then
                            break;
                        }
                    }
                    // This action is not referenced for this actor yet
                    if ( !b_targetFound )
                    {
                        actionPerTargetIDs.push_back(action_it->first);
                    }

                    // Init FXcontrol data
                    action_it->second.init();

                    // Add this fxcontrol to owner table
                    addFXControlInOwnerTable(&(action_it->second));
                }
            }
        }
    }

    void FeedbackFXManager::addFXControlInOwnerTable( FXControl * _fxControl )
    {
        ITF_ASSERT(_fxControl);

        if( _fxControl )
        {

            // There is a owner defined ?
            ITF_WARNING_CATEGORY(GPP,NULL,_fxControl->m_owner.isValid(),"FXControl %s has no owner set. Check with sound designers / FX",_fxControl->m_name.getDebugString());
            if ( _fxControl->m_owner.isValid() )
            {
                FeedbackFXManager::OwnerMap::iterator owner_it = m_owners.find(_fxControl->m_owner);
                // This owner already exists
                bbool bExists = bfalse;
                if ( owner_it != m_owners.end() )
                {
                    ITF_VECTOR<FXControl *>& fxControls = owner_it->second;
                    // Make sure _fxControl has not been previously added (which is bad !)
                    for ( u32 i = 0 ; i < fxControls.size() || bExists ; i++ )
                    {
                        ITF_ASSERT(fxControls[i] != _fxControl);
                        if ( fxControls[i] == _fxControl )
                            bExists = btrue;
                    }
                }

                // Add the new one
                if ( !bExists )
                    m_owners[_fxControl->m_owner].push_back(_fxControl);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void FeedbackFXManager::acquireAdditionnalFXDescriptors(const ITF_VECTOR<StringID>& _actorTags, ITF_VECTOR<FxDescriptor_Template *>& _fxDescriptors)
    {
        // Gather resources that this archetype is the owner
        //ITF_ASSERT_CRASH(_actorTags.size() > 0, "List of tags is empty, check this !");
        if (_actorTags.size() == 0)
        {
            ITF_WARNING_CATEGORY(FX, NULL, 0, "List of tags is empty, check this !");
            return;
        }

        for ( u32 i = 0 ; i < (_actorTags.size()-1) ; i++ )
        {
            OwnerMap::const_iterator owner_it = m_owners.find(_actorTags[i]);

            if ( owner_it != m_owners.end() )
            {
                FXControls::const_iterator fxControl_it = owner_it->second.begin();
                for ( ; fxControl_it != owner_it->second.end() ; fxControl_it++ )
                {
                    FXControl * pFxControl = (*fxControl_it);
                    for ( u32 i = 0 ; i < pFxControl->m_particles.size() ; i++ )
                    {
                        FxDescriptor_Template * fxdescTpl = acquireFXTemplate(pFxControl->m_particles[i]);
                        if ( fxdescTpl )
                        {
                            _fxDescriptors.push_back(fxdescTpl);
                        }
                    }
                }
            }
        }
    }

    void FeedbackFXManager::releaseAdditionnalFXDescriptors(const ITF_VECTOR<StringID>& _actorTags)
    {
        // Gather resources that this archetype is the owner
        //ITF_ASSERT_CRASH(_actorTags.size() > 0, "List of tags is empty, check this !");
        if (_actorTags.size() == 0)
        {
            ITF_WARNING_CATEGORY(FX, NULL, 0, "List of tags is empty, check this !");
            return;
        }

        for ( u32 i = 0 ; i < (_actorTags.size()-1) ; i++ )
        {
            OwnerMap::const_iterator owner_it = m_owners.find(_actorTags[i]);

            if ( owner_it != m_owners.end() )
            {
                FXControls::const_iterator fxControl_it = owner_it->second.begin();
                for ( ; fxControl_it != owner_it->second.end() ; fxControl_it++ )
                {
                    FXControl * pFxControl = (*fxControl_it);
                    for ( u32 i = 0 ; i < pFxControl->m_particles.size() ; i++ )
                    {
                        releaseFXTemplate(pFxControl->m_particles[i]);
                    }
                }
            }
        }
    }

    void FeedbackFXManager::acquireExtraMaterials( FXControl * _fxControl, ITF_VECTOR<const GFX_MATERIAL *> & _materials)
    {
        for ( u32 i = 0 ; i < _fxControl->m_particles.size() ; i++ )
        {
            const FxDescriptor_Template * fxdescTpl = acquireFXTemplate(_fxControl->m_particles[i]);
            if (fxdescTpl)
            {
                _materials.push_back(&fxdescTpl->getMaterial());
            }
        }
    }

    void FeedbackFXManager::releaseExtraMaterials(FXControl * _fxControl)
    {
        for ( u32 i = 0 ; i < _fxControl->m_particles.size() ; i++ )
        {
            releaseFXTemplate(_fxControl->m_particles[i]);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void FeedbackFXManager::getAdditionnalSoundDescriptors(const ITF_VECTOR<StringID>& _actorTags, ITF_VECTOR<const SoundDescriptor_Template *>& _soundDescriptors)
    {
        // Gather resources that this archetype is the owner
        ITF_ASSERT_CRASH(_actorTags.size() > 0, "List of tags is empty, check this !");
        for ( u32 i = 0 ; i < (_actorTags.size()-1) ; i++ )
        {
            FeedbackFXManager::OwnerMap::const_iterator owner_it = m_owners.find(_actorTags[i]);

            if ( owner_it != m_owners.end() )
            {
                FeedbackFXManager::FXControls::const_iterator fxControl_it = owner_it->second.begin();
                for ( ; fxControl_it != owner_it->second.end() ; fxControl_it++ )
                {
                    FXControl * pFxControl = (*fxControl_it);
                    for ( u32 i = 0 ; i < pFxControl->m_sounds.size() ; i++ )
                    {
                        ITF_MAP<StringID, u32>::const_iterator soundDesc_it = m_SDTemplateMap.find(pFxControl->m_sounds[i]);
                        if ( soundDesc_it != m_SDTemplateMap.end() )
                        {
                            _soundDescriptors.push_back(&m_template->getSDTemplates()[soundDesc_it->second]);
                        }
                    }
                }
            }
        }
    }

#ifdef ITF_SUPPORT_RAKI
    void FeedbackFXManager::getExtraSounds( FXControl * _fxControl, ITF_VECTOR<Path>& _sounds )
    {
        for ( u32 i = 0 ; i < _fxControl->m_sounds.size() ; i++ )
        {
            ITF_MAP<StringID, u32>::const_iterator soundDesc_it = m_SDTemplateMap.find(_fxControl->m_sounds[i]);
            if ( soundDesc_it != m_SDTemplateMap.end() )
            {
                const ITF_VECTOR<Path> sounds = m_template->getSDTemplates()[soundDesc_it->second].getFiles();
                for ( u32 j = 0 ; j < sounds.size() ; j++ )
                {
                    _sounds.push_back(sounds[j]);
                }
            }
        }
    }
#endif
    ///////////////////////////////////////////////////////////////////////////////////////////

    const StringID& FeedbackFXManager::getCustomCategory(const ITF_VECTOR<StringID>& _tags)
    {
        u32 sizeWithoutDefault = _tags.size()-1;
        for ( u32 i = 0 ; i < sizeWithoutDefault ; i++ )
        {
            const StringID& cat = getCustomCategory(_tags[sizeWithoutDefault - i - 1]);
            if ( cat.isValid() )
                return cat;
        }

        return StringID::Invalid;
    }

    const StringID& FeedbackFXManager::getCustomCategory(const StringID& _type)
    {
        ITF_MAP<StringID,StringID>::const_iterator  it = m_busMap.find(_type);

        if ( it != m_busMap.end() )
            return it->second;

        return StringID::Invalid;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    const SoundDescriptor_Template* FeedbackFXManager::getSDTemplate(const StringID& _name) const
    {
        ITF_MAP<StringID, u32>::const_iterator SDIndex_it = m_SDTemplateMap.find(_name);
        if ( SDIndex_it != m_SDTemplateMap.end() )
            return &m_template->getSDTemplates()[SDIndex_it->second];

        return NULL;
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    const FxDescriptor_Template* FeedbackFXManager::getFXTemplate(const StringID& _name) const
    {
        ITF_MAP<StringID, RefCountedTemplateID>::const_iterator FXIndex_it = m_FXTemplateMap.find(_name);
        if ( FXIndex_it != m_FXTemplateMap.end() )
            return &m_template->getFXTemplate(FXIndex_it->second.templateID);

        return NULL;
    }

    FxDescriptor_Template * FeedbackFXManager::acquireFXTemplate(const StringID& _name)
    {
        ITF_MAP<StringID, RefCountedTemplateID>::iterator FXIndex_it = m_FXTemplateMap.find(_name);
        if ( FXIndex_it != m_FXTemplateMap.end() )
        {
            u32 & refCount = FXIndex_it->second.refCount;
            u32 & templateID = FXIndex_it->second.templateID;
            refCount++;
            if (refCount == 1)
            {
                GFXMaterialSerializable & material = m_template->getFXTemplate(templateID).getMaterial();
                material.onLoaded();
            }
            return &m_template->getFXTemplate(templateID);
        }

        return NULL;
    }

    void FeedbackFXManager::releaseFXTemplate(const StringID& _name)
    {
        ITF_MAP<StringID, RefCountedTemplateID>::iterator FXIndex_it = m_FXTemplateMap.find(_name);
        if ( FXIndex_it != m_FXTemplateMap.end() )
        {
            u32 & refCount = FXIndex_it->second.refCount;
            u32 & templateID = FXIndex_it->second.templateID;
            ITF_ASSERT(refCount>0);
            refCount--;
            if (refCount == 0)
            {
                GFXMaterialSerializable & material = m_template->getFXTemplate(templateID).getMaterial();
                material.onUnLoaded();
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // BUNDLE
    ///////////////////////////////////////////////////////////////////////////////////////////

    void FeedbackFXManager::getDependencies( const StringID& _archetype, const StringID& _type,  ITF_VECTOR<Path>& _dependencies )
    {
        // Gather resources that this archetype / type is the owner
        FeedbackFXManager::OwnerMap::const_iterator owner_it = m_owners.find(_type);
        if ( owner_it == m_owners.end() )
            owner_it = m_owners.find(_archetype);

        if ( owner_it != m_owners.end() )
        {
            FeedbackFXManager::FXControls::const_iterator fxControl_it = owner_it->second.begin();
            for ( ; fxControl_it != owner_it->second.end() ; fxControl_it++ )
            {
#ifdef ITF_SUPPORT_RAKI
                // Gather all sound paths
                getSoundPaths(*fxControl_it, _dependencies);
#endif
                // Gather all VFX paths
                getVFXPaths(*fxControl_it, _dependencies);
            }
        }
    }

    void FeedbackFXManager::getVFXPaths(FXControl * _fxControl, ITF_VECTOR<Path>& _paths)
    {
        for ( u32 i = 0 ; i < _fxControl->m_particles.size() ; i++ )
        {
            const FxDescriptor_Template * fxDescTpl = getFXTemplate(_fxControl->m_particles[i]);
            if (fxDescTpl)
            {
                const GFXMaterialTexturePathSet & matPathSet = fxDescTpl->getMaterial().getTexturePathSet();
                for(ux i = 0; i<TEXSET_ID_COUNT; ++i)
                {
                    const Path & pathTex = matPathSet.getTexturePath(TextureSetSubID(i));
                    if(!pathTex.isEmpty())
                    {
                        _paths.push_back(pathTex);
                    }
                }
                if (!matPathSet.getAnimPath().isEmpty())
                    _paths.push_back(matPathSet.getAnimPath());
            }
        }
    }

#ifdef ITF_SUPPORT_RAKI
    void FeedbackFXManager::getSoundPaths(FXControl * _fxControl, ITF_VECTOR<Path>& _paths)
    {
        for ( u32 i = 0 ; i < _fxControl->m_sounds.size() ; i++ )
        {
            ITF_MAP<StringID, u32>::const_iterator soundDesc_it = m_SDTemplateMap.find(_fxControl->m_sounds[i]);
            if ( soundDesc_it != m_SDTemplateMap.end() )
            {
                const ITF_VECTOR<Path>& soundList = m_template->getSDTemplates()[soundDesc_it->second].getFiles();
                for ( u32 j = 0 ; j < soundList.size() ; j++ )
                {
                    _paths.push_back(soundList[j]);
                }
            }
        }
    }
#endif

#ifdef ITF_SUPPORT_EDITOR
    void FeedbackFXManager::updateDebugInfo()
    {
        String8 dbgString;

        if ( isDebugRules() )
        {
            if ( m_lastSelectedRule.m_found )
            {
                if ( m_lastSelectedRule.m_actorName != StringID::Invalid )
                {
                    dbgString.setTextFormat("    Last Rule : %s | %s | %s -> %s (Actor | Action | Target -> FXControl Name)"
                        , m_lastSelectedRule.m_actorName.getDebugString(), m_lastSelectedRule.m_actionName.getDebugString(), m_lastSelectedRule.m_targetName.getDebugString(), m_lastSelectedRule.m_FXControlName.getDebugString());
                }
                else
                {
                    dbgString.setText("    No rule used yet");
                }
            }
            else if ( m_lastAction.isValid() )
            {
                String8 temp;
                // List of target tags
                dbgString = "       Target : ";
                for ( u32 i = 0 ; i < m_lastTargetTags.size() ; i++ )
                {
                    temp.setTextFormat("%s ", m_lastTargetTags[i].getDebugString());
                    dbgString += temp;
                }
                GFX_ADAPTER->drawDBGText(dbgString);

                // Action
                dbgString.setTextFormat("       Action : %s", m_lastAction.getDebugString());
                GFX_ADAPTER->drawDBGText(dbgString);

                // List of actor tags
                dbgString = "       Actor : ";
                for ( u32 i = 0 ; i < m_lastActorTags.size() ; i++ )
                {
                    temp.setTextFormat("%s ", m_lastActorTags[i].getDebugString());
                    dbgString += temp;
                }
                GFX_ADAPTER->drawDBGText(dbgString);

                dbgString.setTextFormat("    Last Rule : didn't find anything matching these criteria :");
            }
            else
            {
                //String8 actorsString;
                //char strInfo[256] = "";
                //for (u32 i = 0 ; i < m_lastActorTags.size() ; i++)
                //{
                //    actorsString.setTextFormat(" %s", m_lastActorTags[i]);
                //    strcat(strInfo[0],actorsString);
                //}
                dbgString.setText("    Last Rule : didn't find the last one (action was not set correctly ?)");
            }

            GFX_ADAPTER->drawDBGText(dbgString);

            dbgString.setText("FEEDBACK INFO :");
            GFX_ADAPTER->drawDBGText(dbgString);
        }

    }
#endif // ITF_SUPPORT_EDITOR

}
