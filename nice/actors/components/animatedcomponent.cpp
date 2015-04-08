#include "precompiled_engine.h"

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_METRONOME_H_
#include "engine/sound/Metronome.h"
#endif //_ITF_METRONOME_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_GHOSTMANAGER_H_
#include "engine/actors/managers/ghostmanager.h"
#endif //_ITF_GHOSTMANAGER_H_

#ifndef _ITF_ANIMTREENODEPLAYANIM_H_
#include "engine/blendTree/animTree/AnimTreeNodePlayAnim.h"
#endif //_ITF_ANIMTREENODEPLAYANIM_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(AnimatedComponent)
    
BEGIN_SERIALIZATION_CHILD(AnimatedComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(AnimatedComponent)
    VALIDATE_COMPONENT_PARAM("", m_animTree->validate(), "Anim Tree has errors");
END_VALIDATE_COMPONENT()


void AnimatedComponent::SerializeAnim( CSerializerObject* serializer, const char* name, StringID &animName, u32 flags )
{
#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
	BEGIN_CONDITION_BLOCK(ESerializeGroup_PropertyEdit)
		if( m_animTree != NULL && m_animTree->isValid() )
		{
			u32 nbNode = getNumAnimTreeNodes();
			u32 stringIDToEnum = U32_INVALID;
			BEGIN_CONDITION_BLOCK(ESerialize_PropertyEdit_Save);
				for ( u32 i = 0; i < nbNode; i++ )
				{
					if(getAnimsName(i) == animName)
					{
						stringIDToEnum = i;
						break;
					}
				}
			END_CONDITION_BLOCK()
			SERIALIZE_ENUM_BEGIN(name, stringIDToEnum);
				for ( u32 i = 0; i < nbNode; i++ )
				{
					serializer->SerializeEnumVar(i, getAnimsName(i).getDebugString());
				}
				serializer->SerializeEnumVar(U32_INVALID,"none");
			SERIALIZE_ENUM_END();

			BEGIN_CONDITION_BLOCK(ESerialize_PropertyEdit_Load);
				if (stringIDToEnum != U32_INVALID && stringIDToEnum < nbNode)
				{
					animName = getAnimsName(stringIDToEnum);
					ITF_ASSERT(m_animTree->isNodeValid(animName));
				}
				else
				{
					animName = "";
				}
			END_CONDITION_BLOCK()
		}
		else
		{
			SERIALIZE_MEMBER(name, animName);
		}
	}
	else
	{
		SERIALIZE_MEMBER(name, animName);
	END_CONDITION_BLOCK()
#else
	SERIALIZE_MEMBER(name, animName)
#endif
}

const f32 AnimatedComponent::s_MagicBoxLockTimer = 0.1f;

AnimatedComponent::AnimatedComponent()
: Super()
, m_animInputUpdate(NULL)
, m_animTree(NULL)
, m_lockMagicBox(bfalse)
, m_lockMagicBoxPosFrom(Vec3d::Zero)
, m_lockMagicBoxPosTo(Vec3d::Zero)
, m_lockMagicBoxTimer(0.f)
, m_lockMagicBoxTotalTime(0.f)
, m_lockMagicBoxResetFrom(bfalse)
, m_magicBoxIndex(U32_INVALID)
, m_disableInputUpdate(bfalse)
, m_forceResetTree(bfalse)
, m_currentDt(0.f)
{
}

//////////////////////////////////////////////////////////////////////////
AnimatedComponent::~AnimatedComponent()
    //////////////////////////////////////////////////////////////////////////
{
    if ( m_animTree )
    {
        m_animTree->~AnimTree();
        char* memBuffer = reinterpret_cast<char*>(m_animTree);
        ITF::Memory::free(memBuffer);
        memBuffer = NULL;
    }
}

void AnimatedComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_inputList.resize(getTemplate()->getInputList().size());

    for ( u32 i = 0; i < m_inputList.size(); i++ )
    {
        Input* input = &m_inputList[i];
        const InputDesc& inputDesc = getTemplate()->getInputList()[i];

        input->setId(inputDesc.getId());
        input->setType(inputDesc.getType());

        if ( inputDesc.getType() == InputType_F32 )
        {
            input->setValue(0.f);
        }
        else
        {
            input->setValue(static_cast<u32>(0));
        }
    }

    if (!m_vertexs.size())
    {
        for (u32 i=0;i<4;i++)
        {
            Vertex v;
            m_vertexs.push_back(v);
        }
    }

    AnimTreeInitData initData;

    initData.m_loadInPlace = btrue;
    initData.m_inputs = &m_inputList;
#ifdef ITF_BLENDTREE_EVENTS
    initData.m_owner = m_actor->getRef();
#endif //ITF_BLENDTREE_EVENTS


    m_animTree = getTemplate()->createTreeInstance();
    m_animTree->init(&initData,&getTemplate()->getAnimTreeTemplate());
    

	ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventChangeGfxMaskInfo_CRC,this);
	ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventPlayAnim_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventReset_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSetFloatInput_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSetUintInput_CRC,this);
}

void AnimatedComponent::setFirstAnim()
{
    if (!m_action.isValid())
    {
        if (m_defaultAnim.isValid())
        {
            setAnim(m_defaultAnim);
        }
        else if ( getTemplate()->getDefaultAnim().isValid())
        {
            setAnim(getTemplate()->getDefaultAnim());
        }
        else 
        {
            if (m_animTree && m_animTree->getNumNodes() )
            {
                setAnim(m_animTree->getNodeID(0));
            }
            else if ( getTemplate()->getAnimTreeTemplate().getNodeList().size() )
            {
                setAnim(getTemplate()->getAnimTreeTemplate().getNodeList()[0]->getId());
            }
        }
    }
    else
    {
        setAnim(m_action);
    }
}

void AnimatedComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( EventReset* eventReset = DYNAMIC_CAST(_event,EventReset) )
    {
        resetLockMagicBox();
    }
    else if (EventSetFloatInput * onSetInput = DYNAMIC_CAST(_event,EventSetFloatInput))
    {
        setInput(onSetInput->getInputName(),onSetInput->getInputValue());
    }
    else if (EventSetUintInput * onSetInput = DYNAMIC_CAST(_event,EventSetUintInput))
    {
        setInput(onSetInput->getInputName(),onSetInput->getInputValue());
    }
}

void AnimatedComponent::onResourceLoaded()
{
    Super::onResourceLoaded();

    m_magicBoxIndex = getBoneIndex(ITF_GET_STRINGID_CRC(B_SnapOn,18328722));

    AnimTreeLoadResource resData;

    resData.m_subAnimSet = &m_subAnimSet;

    m_animTree->onLoadResource(&resData);
}

bool AnimatedComponent::updateData(f32 _deltaTime)
{
    m_currentDt = _deltaTime;
    if ( m_animInputUpdate && m_currentDt && !m_disableInputUpdate )
    {
        m_animInputUpdate->updateAnimInput();
    }

    return Super::updateData(_deltaTime);
}

void AnimatedComponent::resetTree()
{
    m_animTree->reset();
}

void AnimatedComponent::resetTransition()
{
    m_animTree->resetTransition();
}

bool AnimatedComponent::postUpdateData()
{
    if ( isLockMagicBox() )
    {
        updateLockMagicBox();
    }
    return btrue;
}

void AnimatedComponent::processAnim()
{
    if (m_forceDirectUpdate)
    {
        Super::processAnim();
        return;
    } 
    if ( m_action.isValid() )
    {
        m_animTree->playNode(m_action, m_forceResetTree);
        m_action.invalidate();
        //as we are changing animation we want to be sure polylines position will be synced
        m_polySync = btrue;
    }
}

f32 AnimatedComponent::getPrevBlending() const
{
    if ( m_AnimMeshScene )
    {
        const AnimInfo& animInfo = m_AnimMeshScene->m_AnimInfo;
        return animInfo.m_prevPercent;
    }
    else
    {
        return 0.f;
    }
}

#define TranstionFlag_Progressive       0x1

void AnimatedComponent::updateAnimTime( f32 _dt )
{
    if (m_forceDirectUpdate)
    {
        Super::updateAnimTime( _dt );
        return;
    }

    m_previousAnimsPlaying.clear();
    getResultSubAnimNames(*m_animTree->getResult(), m_previousAnimsPlaying);

    if ( _dt || getAnimFrameInfo().size() == 0 )
    {
        m_animTree->update(_dt,m_inputList);
    }

    if ( m_animTree->getBlendFramesResult() != U32_INVALID )
    {
        setCurrentBlendFrames(m_animTree->getBlendFramesResult());
        u32 flags = m_animTree->getTransitionFlags();
        if (flags & TranstionFlag_Progressive)
            transfertTimeCursor(0);   // First Animation
    }
    
    const AnimTreeResult* result = m_animTree->getResult();
    m_currentAnimsPlaying.clear();
    getResultSubAnimNames(*result, m_currentAnimsPlaying);

    u32 numPrevAnims = m_previousAnimsPlaying.size();
    for ( u32 i = 0; i < numPrevAnims; i++ )
    {
        const StringID& strId = m_previousAnimsPlaying[i];

        if ( m_currentAnimsPlaying.find(strId) == U32_INVALID )
        {
            EventAnimChanged onAnimChanged(strId);
            m_actor->onEvent(&onAnimChanged);
        }
    }

    u32 numAnims = result->m_anims.size();

    m_frameEvents.clear();

    resetSubAnimsToPlay();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        const AnimTreeResultLeaf & anim = result->m_anims[i];

        addSubAnimToPlay(*anim.m_subAnim, anim.m_weight, anim.m_usePatches);

        if ( anim.m_useEvents )
        {
            u32 numEvents = anim.m_events.size();
            #if defined(ITF_WINDOWS) && !defined(ITF_FINAL) 
                if (numEvents > MAX_EVENTPROCESSCONTAINER_EVENTS)
                {
                    ITF_WARNING(m_actor, bfalse, "too many events in animation");
                }
            #endif
            #ifndef ITF_FINAL
            if (numEvents > MAX_EVENTPROCESSCONTAINER_EVENTS)
                numEvents = MAX_EVENTPROCESSCONTAINER_EVENTS;
            #endif
            for ( u32 j = 0; j < numEvents; j++ )
            {
                m_frameEvents.push_back(anim.m_events[j]);
            }
        }
    }

    commitSubAnimsToPlay();

    ITF_ASSERT_MSG(m_animFrameInfo.size() || m_actor->isPendingUnregister(), "trying to play empty anim");
}

void   AnimatedComponent::onRecordGhost(GhostRecorderInfoBase* _ghostRecorderInfo)
{   
    if( _ghostRecorderInfo->getRecorderType() != Ghost_AnimInputType )
        return ; 

    GhostRecorderAnimInfo* _ghostRecorderAnimInfo = (GhostRecorderAnimInfo*)_ghostRecorderInfo;

    u32 inputCount = getNumInputs();
    ITF_VECTOR<GhostInputInfo> inputs;
    for ( u32 i = 0; i < inputCount; i++)
    {
        bbool saveInput = bfalse;
        GhostInputInfo inputInfo; 

        InputType input_type = getInputType(i);
        if(input_type == InputType_F32)
        {
            f32 val = 0.0f;
            getInputValue(i, val);
            //if(val != 0.0f)
            {
                saveInput = btrue;
                inputInfo.val.f32Val = val;
            }
        }
        else if (input_type == InputType_U32)
        {
            u32 val = 0;
            getInputValue(i, val);
            //if(val != 0)
            {
                saveInput = btrue;
                inputInfo.val.u32Val = val;
            }
        }
           
        if(saveInput)
        {   
            inputInfo.type = input_type;
            inputInfo.id = getInputID(i);
            inputs.push_back(inputInfo);
        }
    }

    _ghostRecorderAnimInfo->m_inputInfosCount   = inputs.size();
    if(_ghostRecorderAnimInfo->m_inputInfosCount > 0)
    {
        _ghostRecorderAnimInfo->m_inputInfos    = (GhostInputInfo*)Memory::malloc(sizeof(GhostInputInfo)*_ghostRecorderAnimInfo->m_inputInfosCount);
        ITF_Memcpy(_ghostRecorderAnimInfo->m_inputInfos, &inputs[0], sizeof(GhostInputInfo)*_ghostRecorderAnimInfo->m_inputInfosCount);
    } 
    else
    {
        _ghostRecorderAnimInfo->m_inputInfos    = NULL;
    }

    _ghostRecorderAnimInfo->m_mainAnimId        = m_lastAction;
    _ghostRecorderAnimInfo->m_IsLookingRight    = m_AnimMeshScene != NULL ? m_AnimMeshScene->m_AnimInfo.m_IsLookingRight : bfalse;
    _ghostRecorderAnimInfo->m_AnimTime          = getNumPlayingSubAnims() == 0 ? 0 : getCurTime();
}

bool AnimatedComponent::getMagicBox(Vec3d & _pos, bbool _local )
{
    if (m_magicBoxIndex == U32_INVALID)
        return bfalse;

    if (!getBonePos(m_magicBoxIndex, _pos))
        return bfalse;

    if ( _local )
    {
        Vec2d root;
        if (m_rootIndex == U32_INVALID || !getBonePos(m_rootIndex, root))
            return false;

        _pos -= root.to3d();
    }
    return true;
}

bbool AnimatedComponent::getRoot( Vec3d& _pos ) const
{
    if ( m_rootIndex == U32_INVALID )
        return bfalse;

    return getBonePos(m_rootIndex, _pos);
}

void AnimatedComponent::lockMagicBox( const Vec3d& _pos, f32 _interpolationTime )
{
    if ( !m_lockMagicBox )
    {
        m_lockMagicBox = btrue;
        m_lockMagicBoxTotalTime = m_lockMagicBoxTimer = _interpolationTime;
        resetLockMagicBoxFrom();
    }

    ITF_ASSERT_CRASH(_pos==_pos,"Position is not valid");

    m_lockMagicBoxPosTo = _pos;
}

void AnimatedComponent::updateLockMagicBox()
{
    if ( m_lockMagicBoxResetFrom )
    {
        getMagicBox(m_lockMagicBoxPosFrom,bfalse);
        m_lockMagicBoxResetFrom = bfalse;
    }

    m_lockMagicBoxTimer = Max(m_lockMagicBoxTimer-m_currentDt,0.f);

    f32 t = 1.f - (m_lockMagicBoxTimer/m_lockMagicBoxTotalTime);

    t = Clamp(t,0.f,1.f);

    Vec3d destMagicBox = Interpolate(m_lockMagicBoxPosFrom,m_lockMagicBoxPosTo,t);

    Vec3d currentMagicBox;

    getMagicBox(currentMagicBox,bfalse);

    Vec3d dif = destMagicBox - currentMagicBox;
    // only affect x and y
    dif.z() = 0;

	m_actor->setPos( m_actor->getPos()+dif );
    updateMeshMatrix();
    //m_subAnimSet.getSkeleton()->Translate(&m_AnimMeshScene->m_AnimInfo.getCurrentBoneList(), dif.truncateTo2D(), m_AnimMeshScene->m_AnimInfo.m_globalData, !isVisiblePrediction() && !getDisableVisiblePrediction());
}

u32 AnimatedComponent::findInputIndex( StringID _id ) const
{
    return getTemplate()->findInputIndex(_id);
}

u32 AnimatedComponent::getPlayingAnimFlags(u32 _layer  /*= 0*/)
{
    if (m_animTree && m_animTree->getResult() && m_animTree->getResult()->m_anims.size()>_layer)
    {
        const AnimTreeResult *result = m_animTree->getResult();
        u32 flags = result->m_anims[_layer].m_subAnim->getSubAnim()->getFlags();
        return flags;
    }
    return 0;
}

void AnimatedComponent::getResultSubAnimNames(const AnimTreeResult & _result, AnimsPlayingContainer& _anims)
{
    u32 numAnims = _result.m_anims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        const AnimTreeResultLeaf & anim = _result.m_anims[i];
        _anims.push_back(anim.m_subAnim->getSubAnim()->getFriendlyName());
    }
}

u32 AnimatedComponent::getNumAnimsPlaying() const
{
    AnimTreeResult* res = m_animTree->getResult();
    if (res)
    {
        return res->m_playingNodes.size();
    }
    return 0;
}

const StringID& AnimatedComponent::getAnimsPlayingName(u32 _index)
{
    AnimTreeResult* res = m_animTree->getResult();
    if(res && _index < res->m_playingNodes.size() )
    {
        AnimTreeNode* node = res->m_playingNodes[_index];
        if(node)
            return node->getId();
    }
    
    return StringID::Invalid;
}

void AnimatedComponent::dbgGetCurRule(const Color& _color)
{
#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)
	AnimTreeResult* res = m_animTree->getResult();
	u32 numNodes = res->m_playingNodes.size();

	String8 _text = "";
	String8 pretext = "Playing nodes: ";
	String8 spaces = "                            ";

	for ( u32 i = 0; i < numNodes; i++ )
	{
		AnimTreeNode* node = res->m_playingNodes[i];

		String8 rulename;
		if (node->getId().isValid())
		{
			rulename = node->getId().getDebugString();
		}
		else
		{
			rulename = "unnamed";
		}
		if(DYNAMIC_CAST(node, AnimTreeNodePlayAnim))
		{
			if(i!=0) GFX_ADAPTER->drawDBGText(spaces+_text, _color);
			_text = rulename;
		}
		else
		{
			rulename += " > ";
			_text = rulename + _text;
		}
	}
	GFX_ADAPTER->drawDBGText(pretext+_text, _color);
#endif
}

void AnimatedComponent::dbgGetCurRuleNoPrint(String8 & _text)
{
#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)
    AnimTreeResult* res = m_animTree->getResult();
    u32 numNodes = res->m_playingNodes.size();

    for ( u32 i = 0; i < numNodes; i++ )
    {
        AnimTreeNode* node = res->m_playingNodes[i];

        String8 rulename;
        if (node->getId().isValid())
        {
            rulename = node->getId().getDebugString();
        }
        else
        {
            rulename = "unnamed";
        }
        if(DYNAMIC_CAST(node, AnimTreeNodePlayAnim))
        {
            rulename += " * ";
        }
        else
            rulename += " - ";

        _text = rulename + _text;
    }
#endif
}

void AnimatedComponent::resetCurTime(bbool _sendChangeEvent)
{
    AnimTreeResult * result = m_animTree->getResult();
    if (result)
    {
        u32 numAnims = result->m_anims.size();

        for ( u32 i = 0; i < numAnims; i++ )
        {
            result->m_anims[i].m_subAnim->resetCurTime();

            if(_sendChangeEvent && result->m_anims[i].m_subAnim->getSubAnim() )
            {
                EventAnimChanged onAnimChanged(result->m_anims[i].m_subAnim->getSubAnim()->getFriendlyName());
                m_actor->onEvent(&onAnimChanged);
            }
        }
    }
}

void AnimatedComponent::transfertTimeCursor(u32 _animIndex)
{
    AnimTreeResult * result = m_animTree->getResult();
    if (result)
    {
        if (result->m_anims.size()>_animIndex)
        {
            AnimTreeResultLeaf& resultLeaf =  result->m_anims[_animIndex];
            if (SubAnim* subAnim = resultLeaf.m_subAnim->getSubAnim())
            {
                const f32 duration = subAnim->getDuration();
                resultLeaf.m_subAnim->setCurTime(resultLeaf.m_subAnim->getStart() + duration * m_animTree->getPreviousFrameCoef());
                resultLeaf = resultLeaf;
            }
        }
    }
}


void AnimatedComponent::setCurTimeCursor(f32 _value)
{
    const f32 ratio = Clamp(_value, 0.0f, 1.0f);

    AnimTreeResult * result = m_animTree->getResult();
    if (result)
    {
        u32 numAnims = result->m_anims.size();

        for ( u32 i = 0; i < numAnims; i++ )
        {
            AnimTreeResultLeaf& resultLeaf =  result->m_anims[i];

            if (SubAnim* subAnim = resultLeaf.m_subAnim->getSubAnim())
            {
                const f32 duration = subAnim->getDuration();
                resultLeaf.m_subAnim->setCurTime(resultLeaf.m_subAnim->getStart() + (duration * ratio));
            }
        }
    }
}

void  AnimatedComponent::setCurrentTime(u32 _animIndex, f32 _value)
{
    AnimTreeResult * result = m_animTree->getResult();
    if (result)
    {
        u32 numAnims = result->m_anims.size();

        if (_animIndex < numAnims)
        {
            AnimTreeResultLeaf& resultLeaf =  result->m_anims[_animIndex];
            if (resultLeaf.m_subAnim)
            {
                AnimTreeResultLeaf& resultLeaf =  result->m_anims[_animIndex];
                resultLeaf.m_subAnim->setCurTime(_value);
            }
        }
    }
}

f32  AnimatedComponent::getCurrentTime(u32 _animIndex)
{
    AnimTreeResult * result = m_animTree->getResult();
    if (result)
    {
        u32 numAnims = result->m_anims.size();

        if (_animIndex < numAnims)
        {
            AnimTreeResultLeaf& resultLeaf =  result->m_anims[_animIndex];
            if (resultLeaf.m_subAnim)
            {
                return resultLeaf.m_subAnim->getCurrentTime();
            }
        }
    }
    return -1.0f;
}

f32 AnimatedComponent::getAnimDuration( StringID _friendlyName ) const
{
    SubAnim* subAnim = getSubAnim(_friendlyName);
    if (subAnim)
    {
        return subAnim->getDuration();
    }
    return 0.0f;
}

bbool AnimatedComponent::isNodeValid(const StringID& _nodeName) const
{
	return m_animTree->isNodeValid(_nodeName);
}

ITF::bbool AnimatedComponent::setAnim( const StringID & _anim, u32 _blendFrames /*= U32_INVALID*/, bbool _forceResetTree /*= bfalse*/, u32 _priority /*= 0*/ )
{
    if (m_forceDirectUpdate)
        return Super::setAnim(_anim, _blendFrames, _forceResetTree, _priority);
    ITF_ASSERT(_blendFrames == U32_INVALID);
    m_action = m_lastAction = _anim;
    m_forceResetTree = _forceResetTree;
    return btrue;
}

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(AnimatedComponent_Template)
BEGIN_SERIALIZATION_CHILD(AnimatedComponent_Template)
    BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
        SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
        SERIALIZE_OBJECT("tree",m_animTree);
    END_CONDITION_BLOCK()
END_SERIALIZATION()

AnimatedComponent_Template::AnimatedComponent_Template()
{
    m_useBase = btrue;
}

AnimatedComponent_Template::~AnimatedComponent_Template()
{
}

bbool AnimatedComponent_Template::onTemplateLoaded( bbool _hotReload )
{
    bbool bOk = Super::onTemplateLoaded(_hotReload);
    
    bOk &= m_animTree.onTemplateLoaded(m_actorTemplate->getFile());

    AnimTree instanceTree;

    instanceTree.createFromTemplate(&m_animTree);

    CSerializerLoadInPlace serializer;

    serializer.setFactory(GAMEINTERFACE->getAnimTreeNodeInstanceFactory());
    serializer.Init(&m_instanceData);

    instanceTree.Serialize(&serializer,ESerialize_Data_Save);
    serializer.close();

    return bOk;
}

void AnimatedComponent_Template::onTemplateDelete( bbool _hotReload )
{
    Super::onTemplateDelete(_hotReload);

    m_animTree.onTemplateDelete();
    m_instanceData.clear();
}

AnimTree* AnimatedComponent_Template::createTreeInstance() const
{
    ArchiveMemory archLoad(m_instanceData.getData(),m_instanceData.getSize());
    CSerializerLoadInPlace serializer;

    serializer.setFactory(GAMEINTERFACE->getAnimTreeNodeInstanceFactory());
    serializer.Init(&archLoad);

    AnimTree* ret = serializer.loadInPlace<AnimTree>();

    ret->Serialize(&serializer,ESerialize_Data_Load);

    return ret;
}

u32 AnimatedComponent_Template::findInputIndex( const StringID& _id ) const
{
    u32 numInputs = m_inputList.size();

    for ( u32 i = 0; i < numInputs; i++ )
    {
        const InputDesc& input = m_inputList[i];

        if ( input.getId() == _id )
        {
            return i;
        }
    }

    return U32_INVALID;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION_SUBCLASS(AnimatedWithSubstitionTemplatesComponent,AnimSubst)
	SERIALIZE_MEMBER("original", original);
	SERIALIZE_MEMBER("final", final);
END_SERIALIZATION()

BEGIN_SERIALIZATION_SUBCLASS(AnimatedWithSubstitionTemplatesComponent,AnimSubstsTemplate)
	SERIALIZE_CONTAINER_OBJECT("substitutedAnimsList", substsList);
END_SERIALIZATION()

IMPLEMENT_OBJECT_RTTI(AnimatedWithSubstitionTemplatesComponent)
BEGIN_SERIALIZATION_CHILD(AnimatedWithSubstitionTemplatesComponent)
	SERIALIZE_CONTAINER_OBJECT("AnimSbustitionTemplates", m_AnimSubstTemplates_List);
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(AnimatedWithSubstitionTemplatesComponent)
END_VALIDATE_COMPONENT()

void AnimatedWithSubstitionTemplatesComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
	AnimatedComponent::onActorLoaded(_hotReload);

	ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventChangeSubstitutionTemplateAnim_CRC,this);
}

void AnimatedWithSubstitionTemplatesComponent::onEvent( Event* _event )
{
	AnimatedComponent::onEvent(_event);

	if (EventChangeSubstitutionTemplateAnim* eventChangeSubstitutionTemplate = DYNAMIC_CAST(_event,EventChangeSubstitutionTemplateAnim) )
		if (!eventChangeSubstitutionTemplate->getHasToSet())
			AnimSubstsTemplates_UnSet();
		else
			AnimSubstsTemplates_SetAtId(eventChangeSubstitutionTemplate->getTemplateToSet());
}


const StringID& AnimatedWithSubstitionTemplatesComponent::AnimSubstsTemplates_GetTemplated(const StringID& _untemplatedAnimID) const
{
	if (m_AnimSubstsTemplates_CurrentTemplateIndex != U32_INVALID)
	{
		ITF_ASSERT(m_AnimSubstsTemplates_CurrentTemplateIndex < m_AnimSubstTemplates_List.size());
		const AnimSubstsTemplate& substsTemplateToConsider = m_AnimSubstTemplates_List[m_AnimSubstsTemplates_CurrentTemplateIndex];
		for (ITF_VECTOR<AnimSubst>::const_iterator itAnimSubstsList = substsTemplateToConsider.substsList.begin(); itAnimSubstsList != substsTemplateToConsider.substsList.end(); ++itAnimSubstsList)
			if (_untemplatedAnimID == (*itAnimSubstsList).original)
				return (*itAnimSubstsList).final;
	}
	return _untemplatedAnimID;
}

const StringID& AnimatedWithSubstitionTemplatesComponent::AnimSubstsTemplates_GetUntemplated(const StringID& _templatedAnimID) const
{
	if (m_AnimSubstsTemplates_CurrentTemplateIndex != U32_INVALID)
	{
		ITF_ASSERT(m_AnimSubstsTemplates_CurrentTemplateIndex < m_AnimSubstTemplates_List.size());
		const AnimSubstsTemplate& substsTemplateToConsider = m_AnimSubstTemplates_List[m_AnimSubstsTemplates_CurrentTemplateIndex];
		for (ITF_VECTOR<AnimSubst>::const_iterator itAnimSubstsList = substsTemplateToConsider.substsList.begin(); itAnimSubstsList != substsTemplateToConsider.substsList.end(); ++itAnimSubstsList)
			if (_templatedAnimID == (*itAnimSubstsList).final)
				return (*itAnimSubstsList).original;
	}
	return _templatedAnimID;
}

bbool AnimatedWithSubstitionTemplatesComponent::setAnim(const StringID & _anim, u32 _blendFrames/* = U32_INVALID*/, bbool _forceResetTree/* = bfalse*/, u32 _priority/* = 0*/)
{
	return AnimatedComponent::setAnim(AnimSubstsTemplates_GetTemplated(_anim), _blendFrames, _forceResetTree, _priority);
}

const SubAnim*	AnimatedWithSubstitionTemplatesComponent::getCurSubAnim( u32 _index ) const
{
	const SubAnim* toReturn = AnimatedComponent::getCurSubAnim(_index);
	if (toReturn)
		toReturn = AnimatedComponent::getCurSubAnim(AnimatedComponent::getCurSubAnimIndex(AnimSubstsTemplates_GetTemplated(toReturn->getFriendlyName())));
	return toReturn;
}


IMPLEMENT_OBJECT_RTTI(AnimatedWithSubstitionTemplatesComponent_Template)
BEGIN_SERIALIZATION_CHILD(AnimatedWithSubstitionTemplatesComponent_Template)
END_SERIALIZATION()

} //namespace ITF

