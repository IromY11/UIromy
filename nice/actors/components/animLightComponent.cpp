#include "precompiled_engine.h"

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_ANIMATIONPATCHES_H_
#include    "engine/animation/AnimationPatches.h"
#endif // _ITF_ANIMATIONPATCHES_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_
                                                         
#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_JOB_H_
#include "engine/scheduler/job.h"
#endif //_ITF_JOB_H_

#ifndef _ITF_SCHEDULER_H_
#include "engine/scheduler/scheduler.h"
#endif //_ITF_SCHEDULER_H_

#ifndef _ITF_JOBUPDATEVISUAL_H_
#include "engine/actors/components/jobUpdateVisual.h"
#endif //_ITF_JOBUPDATEVISUAL_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOME_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_RENDERSIMPLEANIMCOMPONENT_H_
#include "gameplay/Components/Display/RenderSimpleAnimComponent.h"
#endif //_ITF_RENDERSIMPLEANIMCOMPONENT_H_

#ifdef ITF_SUPPORT_EDITOR
#ifndef _ITF_CHEATMANAGER_H_
#include "gameplay/managers/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_PLAYANIM_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayAnim_evt.h"
#endif //_ITF_PLAYANIM_EVT_H_

#endif


namespace ITF
{

BEGIN_SERIALIZATION(BoneMapping)
    SERIALIZE_MEMBER("inputBone",m_inputBone);
    SERIALIZE_MEMBER("outputBone",m_outputBone);
END_SERIALIZATION()

BEGIN_SERIALIZATION(AnimLightFrameInfo)
    SERIALIZE_OBJECT("subAnimFrameInfo", m_subAnimFrameInfo);
    SERIALIZE_MEMBER("weight", m_weight);
    SERIALIZE_MEMBER("usePatches", m_usePatches);
END_SERIALIZATION()

IMPLEMENT_OBJECT_RTTI(AnimLightComponent)
BEGIN_SERIALIZATION_CHILD(AnimLightComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("syncOffset", m_syncOffset);
        SERIALIZE_MEMBER("startOffset", m_startOffset);
        SERIALIZE_OBJECT("subAnimInfo", m_subAnimSet);
        SERIALIZE_MEMBER("MatShader", m_materialShaderOverridePath);
        SERIALIZE_MEMBER("subSkeleton",m_subSkeleton);
        SERIALIZE_BOOL("EmitFluid",m_emitFluid);
        SERIALIZE_BOOL("AttractFluid",m_attractFluid);
        SERIALIZE_BOOL("BasicRender",m_basicRender);
        SERIALIZE_GFX_GRID_FILTER("FluidEmitterFilter", m_fluidEmitterFilter);

		SERIALIZE_ANIM("defaultAnim", m_defaultAnim, this);
		
        SERIALIZE_MEMBER("useZOffset", m_useZOffset);
    END_CONDITION_BLOCK()

    BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint) 
		AnimLightComponent::SerializePersistent( serializer, flags );
	END_CONDITION_BLOCK()

END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(AnimLightComponent)
    VALIDATE_COMPONENT_PARAM("frontZOffset", getTemplate()->getFrontZOffset() >= getTemplate()->getBackZOffset(), "must be positive greater than back !");
    VALIDATE_COMPONENT_PARAM("renderInTexture", !getTemplate()->allowRenderInTexture() || f32_Abs(getTemplate()->getFrontZOffset() - getTemplate()->getBackZOffset()) < MTH_EPSILON, "cannot render in texture with front/back offset !");
    bbool isAnimValid = m_subAnimSet.getSkeleton() != NULL;
    VALIDATE_COMPONENT_PARAM_CATEGORY(Anim, "", isAnimValid, "Anim error in: '%s'- skeleton not found", m_actor->getTemplatePath().toString8().cStr());
    if (isAnimValid)
    {
        String8 error;
        isAnimValid = m_subAnimSet.allTracksOnResourcePackage(&error);
        VALIDATE_COMPONENT_PARAM_CATEGORY(Anim, "", isAnimValid , "%s", error.cStr());
    }
    if (isAnimValid)
    {
        isAnimValid = m_subAnimSet.getSubAnimCount() != 0;
        VALIDATE_COMPONENT_PARAM_CATEGORY(Anim, "", isAnimValid , "Error on animation component in actor (bad version ?): '%s'", m_actor->getTemplatePath().toString8().cStr());
    }
    if (isAnimValid)
    {
        String8 trackName;
        isAnimValid = m_subAnimSet.checkTrackSkeleton(trackName);
        VALIDATE_COMPONENT_PARAM_CATEGORY(Anim, "", isAnimValid , "Anim error in '%s': bad skeleton in track %s", m_actor->getTemplatePath().toString8().cStr(), trackName.cStr());
    }
    if (isAnimValid)
    {
        isAnimValid = m_subAnimSet.isResolveMarkersOk();
        VALIDATE_COMPONENT_PARAM_CATEGORY(Anim, "", isAnimValid, "Anim error in '%s': %s", m_actor->getTemplatePath().toString8().cStr(), m_errorStr.cStr());
    }
     if (isAnimValid)
     {
         isAnimValid = m_subAnimSet.isValidPatchBank();
         VALIDATE_COMPONENT_PARAM_CATEGORY(Anim, "", isAnimValid, "Animation patch conflicts in actor: '%s' -> using same patch on 2 different actor's skeleton", m_actor->getTemplatePath().toString8().cStr());
     }
     if (isAnimValid)
     {
         isAnimValid = m_rootIndex != -1;
         VALIDATE_COMPONENT_PARAM_CATEGORY(Anim, "", isAnimValid, "Invalid root index for %s", m_actor->getTemplatePath().toString8().cStr());
     }

#ifdef ITF_SUPPORT_EDITOR
     if (validateForceEvt != NULL)
     {
         validateForceEvt->forceCurrentFrame(validateForceFrame);
         validateForceEvt = NULL;
     }
#endif

END_VALIDATE_COMPONENT()

void AnimLightComponent::SerializePersistent( CSerializerObject* serializer, u32 flags )
{
    if(flags & ESerialize_Persistent)
        Super::SerializePersistent(serializer, flags);
    SERIALIZE_MEMBER("lastAnim",m_lastAnim);
    SERIALIZE_MEMBER("playAnim",m_playAnimName);
    SERIALIZE_MEMBER("playAnimFrames",m_playAnimNumFrames);
    SERIALIZE_CONTAINER_OBJECT("currentFrameSubAnims", m_currentFrameSubAnims);
}

void AnimLightComponent::SerializeAnim( CSerializerObject* serializer, const char* name, StringID &animName, u32 flags )
{
#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
	BEGIN_CONDITION_BLOCK(ESerializeGroup_PropertyEdit)
		const SubAnimSet* subAnimSet = getSubAnimSet();       
		if (subAnimSet != NULL)
		{
			u32 stringIDToEnum = U32_INVALID;
			const u32 count = subAnimSet->getSubAnimCount();

			BEGIN_CONDITION_BLOCK(ESerialize_PropertyEdit_Save);
				for (u32 i=0; i<count; ++i)
				{
					const SubAnim * subAnim = subAnimSet->getSubAnim(i);
					if(subAnim->getFriendlyName() == animName)
					{
						stringIDToEnum = i;
						break;
					}
				}
			END_CONDITION_BLOCK()

			SERIALIZE_ENUM_BEGIN(name, stringIDToEnum);
				for (u32 i=0; i<count; ++i)
				{
					const SubAnim * subAnim = subAnimSet->getSubAnim(i);
					serializer->SerializeEnumVar(i, subAnim->getFriendlyName().getDebugString());
				}
				serializer->SerializeEnumVar(U32_INVALID,"none");
			SERIALIZE_ENUM_END();

			BEGIN_CONDITION_BLOCK(ESerialize_PropertyEdit_Load);
				if (stringIDToEnum != U32_INVALID && stringIDToEnum < count)
				{
					const SubAnim * subAnim = subAnimSet->getSubAnim(stringIDToEnum);
					animName = subAnim->getFriendlyName();
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
		SERIALIZE_MEMBER(name, animName)
	END_CONDITION_BLOCK()
#else
	SERIALIZE_MEMBER(name, animName)
#endif
}

AnimLightComponent::AnimLightComponent()
: Super()
, m_lastAnimPrio(0)
, m_AnimMeshScene(NULL)
, m_animID()
, m_rootIndex(U32_INVALID)
, m_blendDt(0.f)
, m_playAnimNumFrames(U32_INVALID)
, m_previousLookRight(bfalse)
, m_currentColor(Color::white())
, m_colorSrc(Color::white())
, m_colorDst(Color::white())
, m_colorBlendTime(0.f)
, m_colorBlendTotal(0.f)
, m_currentBlendFrames(0)
, m_vertexFormat(VertexFormat_PTa)
, m_forceDirectUpdate(0)
, m_loadingOK(bfalse)
, m_ignoreEvents(bfalse)
, m_drawEnabled(btrue)
, m_isProcedural(bfalse)
, m_updateAnimTime(btrue)
, m_isLooped(bfalse)
, m_useRootRotation(bfalse)
, m_animDefaultFlip(bfalse)
, m_useScissor(bfalse)
, m_semiForcedDirectUpdate(bfalse)
, m_isVisiblePrediction(btrue)
, m_disableVisiblePrediction(bfalse)
, m_forceSynchro(bfalse)
, m_boneModification(bfalse)
, m_boneModificationLocal(bfalse)
, m_syncOffset(0.f)
, m_startOffset(0.f)
, m_scaleMultiplier(Vec2d::One)
, m_rank2D(0)
, m_instancePositionOffset(0,0)
, m_subSkeletonIdx(-1)
, m_jobUpdateVisual(NULL)
, m_samePrevAnimFrameInfo(bfalse)
, m_polySync(bfalse)
, m_zOffsetActive(btrue)
, m_useZOffset(btrue)
, m_animPlayRate(1.f)
, m_Invisible(false)
, m_needRefreshTextureBank(bfalse)
, mb_forceResetAnim(bfalse)
, m_patchChangeModified(bfalse)
, m_emitFluid(false)
, m_attractFluid(false)
, m_basicRender(true)
, m_fluidEmitterFilter(0x0000ffff)
{
#ifdef ITF_SUPPORT_EDITOR
    validateForceEvt = NULL;
    validateForceFrame = -1;
#endif
}


AnimLightComponent::~AnimLightComponent()
{
}

void AnimLightComponent::fillBoneMap()
{
    m_boneList = getTemplate()->getBoneList();
    for (ITF_VECTOR<BoneMapping>::const_iterator it = m_boneList.begin(); it != m_boneList.end(); ++it)
    {
        m_boneMap[it->m_inputBone] = it->m_outputBone;
    }
}

void AnimLightComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

    if (!getTemplate()->getRestoreStateAtCheckpoint())
        m_currentFrameSubAnims.clear();

    if ( m_currentFrameSubAnims.size() == 0 )
    {
        setFirstAnim();
    }
    else
    {
        for (u32 i=0; i<m_currentFrameSubAnims.size(); i++)
        {
            m_currentFrameSubAnims[i].m_subAnimFrameInfo.setSubAnimSet(&m_subAnimSet);
        }
    }
}

void AnimLightComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_jobUpdateVisual = newAlloc(mId_Animation, JobUpdateVisual);
    m_jobUpdateVisual->setAsManaged(btrue);

    m_instancePositionOffset.clear();
    m_subAnimSet.setTemplate(getTemplate()->getSubAnimTemplate());
    m_subAnimSet.setGlobalSyncOffset(m_syncOffset);

    ITF_WARNING_CATEGORY(LD, NULL,
        (!m_subAnimSet.getAnimResourcePackage().needPack() || m_subAnimSet.getAnimResourcePackage().isPacked()) &&
        (!getTemplate()->getSubAnimTemplate()->getAnimResourcePackage().needPack() || getTemplate()->getSubAnimTemplate()->getAnimResourcePackage().isPacked()), 
        "Actor '%s' should reference its packed ilu !!", m_actor->getTemplatePath().toString8().cStr());

    ITF_WARNING_CATEGORY(GPP, m_actor,
        !getTemplate()->getNotifyAnimUpdate() || m_actor->GetComponent<RenderSimpleAnimComponent>() == NULL,
        "AnimLightComponent::No need to notify of anim update if render simple anim is present");

	ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventChangeGfxMaskInfo_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventPlayAnim_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventReset_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventQueryPosition_CRC,this);
	ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventActivateReflections_CRC,this);
	ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventChangeBankId_CRC,this);
	ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventChangePatch_CRC,this);

    m_subAnimSet.linkSubAnimToTrack(m_actor->getResourceContainer());
    m_currentColor = getTemplate()->getDefaultColor();

    m_subAnimSet.processLocalAnim();

    setFirstAnim();

    fillBoneMap();

    AABB aabb;
    aabb.invalidate();
    if (m_subAnimSet.fillGlobalBoundingBox(aabb))
    {
        transformLocalAABB(aabb);

        GetActor()->growAABB(aabb);
    }

    m_subAnimSet.setMaterialShaderOverride(m_materialShaderOverridePath);
}

void AnimLightComponent::onActorClearComponents()
{
    Super::onActorClearComponents();

    m_subAnimSet.clear();
    deleteAnimMesh();

    SF_DEL(m_jobUpdateVisual);
}

#ifdef ITF_SUPPORT_EDITOR
void AnimLightComponent::onPrePropertyChange()
{
    m_prevDefaultAnim = m_defaultAnim;
}

void AnimLightComponent::onPostPropertyChange()
{
    Super::onPostPropertyChange();

    // Reset anim in case defaultAnim changed
    if(m_prevDefaultAnim != m_defaultAnim)
        setFirstAnim();

    m_subAnimSet.setGlobalSyncOffset(m_syncOffset);
    m_subAnimSet.setMaterialShaderOverride(m_materialShaderOverridePath);
}
#endif //ITF_SUPPORT_EDITOR

void AnimLightComponent::setFirstAnim()
{
    if (m_defaultAnim.isValid())
    {
        setAnim(m_defaultAnim);
    }
    else if ( getTemplate()->getDefaultAnim().isValid() )
    {
        setAnim(getTemplate()->getDefaultAnim());
    }
    else if ( m_subAnimSet.getSubAnimCount() > 0 )
    {
        setAnim(m_subAnimSet.getSubAnim(0)->getTemplate()->getFriendlyName());
    }
}

void AnimLightComponent::onResourceLoaded()
{
    Super::onResourceLoaded();
    ITF_ASSERT(m_actor->isPhysicalReady());

    m_rootIndex = U32_INVALID;
    AnimSkeleton   * skl = m_subAnimSet.getSkeleton();
    if (m_subAnimSet.getSubAnimCount() == 0 || !skl || !m_subAnimSet.allTracksOnResourcePackage())
    {
        // validated later
        return;
    }

    if (!m_subAnimSet.isResolveMarkersOk())
        m_subAnimSet.resolveMarkers(m_errorStr, GetActor()->getResourceContainer(), getTemplate()->getActorTemplate()->getResourceContainer());
    if (!m_subAnimSet.isResolveMarkersOk())
        return;
    
    if (!m_subAnimSet.computePatchBankBoneInMainSkeleton(bfalse))
        return;

    if (m_subAnimSet.tracksUsesAlpha())
        m_vertexFormat = VertexFormat_PCT;

    m_useScissor = m_subAnimSet.tracksUsesScissor();

    clearSubSkeleton();
    if (!setSubSkeleton(m_subSkeleton))
        setSubSkeleton(getTemplate()->getSubSkeletonID());

    if ( m_currentFrameSubAnims.size() == 0 )
    {
        processAnim();
    }

    if ( m_currentFrameSubAnims.size() == 0 )
    {
        SubAnimFrameInfo frameInfo(&m_subAnimSet);
        frameInfo.setSubAnimIndex(0, m_startOffset);
        addSubAnimToPlay(frameInfo,1.f,btrue);
    }
    else
    {
       resetSubAnimFrameInfoFlags();
       for (u32 i=0; i<m_currentFrameSubAnims.size(); i++)
       {
           const SubAnimFrameInfo& subframeinfo = m_currentFrameSubAnims[i].m_subAnimFrameInfo;
           updateSubAnimFrameInfoFlag(subframeinfo);
       }
    }

    commitSubAnimsToPlay();

    m_loadingOK = btrue;
    createAnimMeshScene();

    m_rootIndex = getBoneIndex(ITF_GET_STRINGID_CRC(root,170057116));

    if (m_visualAABB.isZero())
    {
        if ( getTemplate()->getVisualAABBanimID().isValid())
        {
            AnimTrack *track = m_subAnimSet.getTrack(getTemplate()->getVisualAABBanimID());
            if (track)
                m_visualAABB = track->m_bounding;
        }
    }

    if (m_visualAABB.isZero())
    {
        m_visualAABB = AABB(Vec2d(-0.5f, -0.5f), Vec2d(0.5f, 0.5f));
    }
}

void AnimLightComponent::createAnimMeshScene()
{
    deleteAnimMesh();

    // Force re creation of anim mesh !!
    m_AnimMeshScene = AnimMeshScene::AllocNewMesh(m_vertexFormat, getMyStaticIndexBuffer());
    ITF_ASSERT_CRASH(m_AnimMeshScene, "Cannot allocate anim mesh scene");

#ifndef ITF_FINAL
    m_AnimMeshScene->m_owner = m_actor->getRef();
#endif //!ITF_FINAL

    // update primitive filter flags
    m_AnimMeshScene->cleanPassFilterFlag();

    const KeyArray<TextureBankResourceID> & bankTexResID = m_subAnimSet.getNameToBank();
    for(ux i = 0, n = bankTexResID.size(); i < n; ++i)
    {
        const GFX_MATERIAL * material = bankTexResID.getAt(i).m_material;
        if (material)
        {
            m_AnimMeshScene->addPassFilterFlag(*material, getGfxPrimitiveParam().m_gfxOccludeInfo);
        }
    }

    forceSynchronousFrame();
}

void AnimLightComponent::processColor( f32 _dt )
{
    if ( m_colorSrc == m_colorDst || !m_colorBlendTotal)
    {
        return;
    }

    m_colorBlendTime = Max(m_colorBlendTime-_dt,0.f);

    f32 t = 1.f - ( m_colorBlendTime / m_colorBlendTotal );

    if ( t == 1 )
    {
        m_currentColor = m_colorSrc = m_colorDst;
    }
    else
    {
        m_currentColor = Color::Interpolate(m_colorSrc,m_colorDst,t);
    }
}

bool AnimLightComponent::updateData(f32 _deltaTime)
{
    // VERIFY IF IT WORK HERE BEFORE update
    AnimInfo & animInfo = m_AnimMeshScene->m_AnimInfo;
    if (animInfo.m_prevPercent > 0.f)
    {
        animInfo.m_prevPercent -= m_blendDt/(m_currentBlendFrames/ANIMATION_FPS);
        if (animInfo.m_prevPercent <= 0.f)
        {
            animInfo.clearPrev();
            m_currentBlendFrames = 0;
        }
    }

    return btrue;
}

bool AnimLightComponent::postUpdateData()
{
    return btrue;
}

void AnimLightComponent::processAnim()
{
    if ( m_playAnimName.isValid() )
    {
        commitPlayAnim();
        //as we are changing animation we want to be sure polylines position will be synced
        m_polySync = btrue;
    }
}


void AnimLightComponent::startJob(bbool _updateBone, bbool _updatePatch)
{
    if (!_updateBone && !isVisiblePrediction())
        return;

    JobUpdateVisualContext context(this, _updateBone, _updatePatch && isVisiblePrediction());
    m_jobUpdateVisual->setContext(context);
    Scheduler::get()->pushJob(m_jobUpdateVisual);

    if (_updateBone)
    {
        AnimInfo & animInfo = m_AnimMeshScene->m_AnimInfo;
        ANIM_MANAGER->registerAnimForSyncro(&animInfo, m_actor->getRef());
    }
}

void AnimLightComponent::Update( f32 _deltaTime )
{
    PRF_M_SCOPE(updateAnimLightComponent)

    // Blend dt not affected by animation playrate
    m_blendDt   = _deltaTime;
    // Multiply By global animation playRate
    _deltaTime *= m_animPlayRate;

    STATS_ANIMLIGHTCOMPONENT_UPDATEINC
    m_polySync  = bfalse;

    ITF_ASSERT_MSG(m_AnimMeshScene, "Updating non initialized actor !");
    if (!m_AnimMeshScene || !m_actor->isPhysicalReady() )
    {
        return;
    }

    updatePatchChange();

    if (m_needRefreshTextureBank)
    {
        getSubAnimSet()->refreshTextureBank();
        m_subAnimSet.computePatchBankBoneInMainSkeleton(btrue);

        m_samePrevAnimFrameInfo = bfalse;
        m_needRefreshTextureBank = bfalse;
#ifdef SUPPORT_LASTBML
        m_AnimMeshScene->m_pAnimPatches.m_forceReload = btrue;
#endif //SUPPORT_LASTBML
    }


    m_AnimMeshScene->m_AnimInfo.m_visualAABB = getAnimsAABB();
    // update visual AABB
    updateAABB();
    
    computeVisiblePrediction();
    
    if ( useShadow() )
    {
        f32 angle = 0.f;
		Vec2d offset = Vec2d::Zero;

		if (getNumPlayingSubAnims())
		{
			if (getTemplate()->getUseRootBone())
			{
				AnimSkeleton* skeleton = m_subAnimSet.getSkeleton();
				u32 rootIndex = skeleton->getRootIndex();
				SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getWorkingBoneList();

				if ( rootIndex < boneDynList.size() )
				{
					AnimBoneDyn *rootBone = &boneDynList[rootIndex];
					angle = rootBone->getAngle();
				}
			}
			const StringID & boneName = getTemplate()->getUseBoneName();
			if ( boneName.isValid() )
			{
				u32 boneIdx = getBoneIndex(boneName);
				if ( boneIdx != U32_INVALID )
				{
					Vec2d posBone;
					if( getBonePos(boneIdx, posBone) )
					{
						offset.x() = posBone.x() - m_actor->getPos().x();
						if (m_actor->getIsFlipped() && !getTemplate()->getUseNoColShadow())
							offset.x() *= -1.f;
					}
				}
			}
		}

        processShadow(_deltaTime, angle, offset);
    }
    processAlpha(_deltaTime);
    processColor(_deltaTime);

    processBackupRootInformation();

    if (!updateData(_deltaTime))
        return;

    processAnim();

    if ( m_updateAnimTime )
    {
        updateAnimTime(_deltaTime);
    }

    updateMeshMatrix();

    if (skipBoneAndPatchUpdate()) // same frame as previous, no need to recompute
    {
        AnimInfo *  pAnimInfo                   = &m_AnimMeshScene->m_AnimInfo;
        GFXPrimitiveParam &primParam            = *(m_AnimMeshScene->getCommonParam());
        pAnimInfo->m_z                          = getOffsettedPos().z();
        primParam								= getGfxPrimitiveParam();
        primParam.m_colorFactor					= primParam.m_colorFactor * getColor();
        primParam.m_colorFactor.m_a				*= getAlpha();

        // scissor must be updated...
        if (m_useScissor)
            updateScissor();

        postUpdateData();
        return;
    }

    if (!fillAnimInfo()) // no bones on the current animations -> nothing to compute
        return;

    if ( isSynchronous() )
    {
        mainJobUpdateVisuals(this, btrue, bfalse);

        if ( getTemplate()->getNotifyAnimUpdate() )
        {
            EventAnimUpdated animUpdated(_deltaTime);
            m_actor->onEvent(&animUpdated);
        }

        if (!isProcedural())
            startJob(bfalse, btrue);
    }
    else
    {
        startJob(btrue, btrue);
    }

}

bbool AnimLightComponent::skipBoneAndPatchUpdate()
{
    if (m_samePrevAnimFrameInfo
        && !getSubAnimSet()->bankChanged()
        && !isProcedural()
        && m_AnimMeshScene->m_AnimInfo.m_prevPercent <= 0.f
#ifdef SUPPORT_LASTBML
        && !m_AnimMeshScene->m_pAnimPatches.m_forceReload
#endif //SUPPORT_LASTBML
        && (m_AnimMeshScene->m_pAnimPatches.m_allPatchs.size()!=0 || !isVisiblePrediction()))
        return btrue;

    m_samePrevAnimFrameInfo = btrue;
    getSubAnimSet()->resetBankChanged();

    return bfalse;
}

bbool AnimLightComponent::isLookingRight() const
{
    bbool isRight = !m_actor->getIsFlipped();

    if ( getDefaultAnimFlip() )
    {
        isRight = !isRight;
    }

    if ( getTemplate()->getStartFlip() )
    {
        isRight = !isRight;
    }

    return isRight;
}


void AnimLightComponent::updateScissor()
{
    ITF_ASSERT_CRASH(m_AnimMeshScene, "we enter here only when animMesh is valid!");

    AnimInfo *  pAnimInfo           = &m_AnimMeshScene->m_AnimInfo;

    pAnimInfo->m_scissor.invalidate();

    const ITF_VECTOR<AnimFrameInfo> & vecFrameInfo = pAnimInfo->getVecAnimFrameInfo();

    for (ITF_VECTOR<AnimFrameInfo>::const_iterator iter = vecFrameInfo.begin(), iterEnd = vecFrameInfo.end();
        iter != iterEnd;
        ++iter)
    {
        AnimTrack *trk = m_subAnimSet.getSubAnim((*iter).m_subAnimIdx)->getTrack();
        if (trk->m_scissor.isValid())
        {
            if (pAnimInfo->m_scissor.isValid())
            {
                pAnimInfo->m_scissor.grow(trk->m_scissor);
            } else
            {
                pAnimInfo->m_scissor = trk->m_scissor;
            }
        }
    }
}

void AnimLightComponent::setExternalScissor(f32 _minX, f32 _maxX, f32 _minY, f32 _maxY )
{
    if (m_AnimMeshScene == NULL)
        return;

    AnimInfo *  pAnimInfo           = &m_AnimMeshScene->m_AnimInfo;
    pAnimInfo->m_scissor2D = true;
    pAnimInfo->m_scissor.setMinX(_minX);
    pAnimInfo->m_scissor.setMaxX(_maxX);
    pAnimInfo->m_scissor.setMinY(_minY);
    pAnimInfo->m_scissor.setMaxY(_maxY);
}

#ifdef ITF_SUPPORT_EDITOR
void AnimLightComponent::onEditorMove( bbool _modifyInitialPos )
{
    if (!m_AnimMeshScene)
    {
        return;
    }

    updateMeshMatrix();
    
    // update visual AABB
    m_AnimMeshScene->m_AnimInfo.m_visualAABB = getAnimsAABB();
    updateAABB();
}
#endif // ITF_SUPPORT_EDITOR

bbool AnimLightComponent::getCurrentSubAnimDefaultRight(bbool _forceUseTrackInfo)
{
    if (!getAnimFrameInfo().size())
        return btrue;

    SubAnim * subAnim    = m_subAnimSet.getSubAnim(getAnimFrameInfo()[0].m_subAnimIdx);
    AnimTrack *baseTrack = subAnim->getTrack();
    if (!subAnim->getUseRootFlip() || _forceUseTrackInfo)
        return (baseTrack->m_tags & ANIM_TRACK_IS_DEFAULT_RIGHT) != 0;

    AnimSkeleton             * skl    = m_subAnimSet.getSkeleton();
    const AnimTrackBonesList & bTrack = baseTrack->m_bonesList[skl->getRootIndex()];
    if (!bTrack.m_trackPASSize)
        return (baseTrack->m_tags & ANIM_TRACK_IS_DEFAULT_RIGHT) != 0;

    f32 currentFrame = getAnimFrameInfo()[0].m_fFrame;
    AnimTrackBonePAS * bTrackData = &baseTrack->m_trackListPAS[bTrack.m_trackPASIndex];

    for (u32 pasIndex = 0; pasIndex< bTrack.m_trackPASSize; pasIndex++, bTrackData++)
    {
        if (bTrackData->m_frame >= currentFrame)
            return f32_Cos(bTrackData->getAngle(baseTrack->m_bonesMaxAngle)) > 0.f;
    }

    return f32_Cos((bTrackData-1)->getAngle(baseTrack->m_bonesMaxAngle)) > 0.f;
}

void AnimLightComponent::updateMeshMatrix()
{
    ITF_Mesh * pMesh = m_AnimMeshScene->m_pMesh[0];
    if (!getTemplate()->getDraw2d())
    {
        Matrix44 & matrix = pMesh->getMatrix();
        matrix.setRotationZ(getOffsettedAngle());
        matrix.setTranslation(getOffsettedPos());

        Vec3d scale( getScale().to3d(1.f));
        if ( getCurrentSubAnimDefaultRight() != isLookingRight() )
            scale.x() *= -1.f;

        matrix.mulScale(scale);
    } else
    {
        Vec2d scale = getScale() * g_pixelBy2DUnit;
        if ( getCurrentSubAnimDefaultRight() != isLookingRight() )
            scale.x() *= -1.f;

        pMesh->set2DPosAndScale(getOffsettedPos().truncateTo2D(), scale);
        pMesh->set2DRotation( getOffsettedAngle() );
    }
}

bbool AnimLightComponent::fillAnimInfo()
{
    ITF_ASSERT(m_AnimMeshScene);
    if (!m_AnimMeshScene)
        return bfalse;

    AnimInfo *  pAnimInfo           = &m_AnimMeshScene->m_AnimInfo;
    pAnimInfo->m_subAnimSet         = &m_subAnimSet;

    bbool lookRight = isLookingRight();

    if (m_previousLookRight != lookRight )
    {
        if (pAnimInfo->m_prevPercent > 0.f)
        {
            pAnimInfo->clearPrev();
        }
        m_previousLookRight = lookRight;
    }

    const ITF_VECTOR<AnimFrameInfo> & currentAnimFrameInfo = getAnimFrameInfo();
    pAnimInfo->setVecAnimFrameInfo(&currentAnimFrameInfo);
    pAnimInfo->m_useWorking             = !isSynchronous();
    pAnimInfo->m_IsLookingRight         = lookRight;
    pAnimInfo->m_useRootRotation        = getUseRootRotation();
    pAnimInfo->m_vertexFormat           = getVertexFormat();
    pAnimInfo->m_hDiv                   = getHDiv();
    pAnimInfo->m_vDiv                   = getVDiv();
    pAnimInfo->m_canonizeTransitions    = getCanonizeTransitions();
    pAnimInfo->m_selfIllumColor         = getTemplate()->getSelfIllumColor();
    pAnimInfo->m_z                      = getOffsettedPos().z();

    GFXPrimitiveParam &primParam        = *(m_AnimMeshScene->getCommonParam());
    primParam							= getGfxPrimitiveParam();
    primParam.m_colorFactor				= primParam.m_colorFactor * getColor();
    primParam.m_colorFactor.m_a			*= getAlpha();

    pAnimInfo->m_zOrderExtract          = getTemplate()->getZOrderExtract();

    pAnimInfo->m_minimalBonesUpdate     = !isVisiblePrediction() && !getDisableVisiblePrediction();
    pAnimInfo->m_globalData.clear();

    if (m_zOffsetActive && (getTemplate()->getFrontZOffset() != getTemplate()->getBackZOffset() ))
        pAnimInfo->m_renderPassFrontierPatchIndex = 0; // This will allow use of mid draw
    else
        pAnimInfo->m_renderPassFrontierPatchIndex = U32_INVALID;// No mid draw



    if (m_useScissor)
        updateScissor();

    return pAnimInfo->getVecAnimFrameInfo().size() != 0;
}

void AnimLightComponent::forceSynchronousFrame()
{
    ITF_ASSERT_MSG(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "direct update must be done in main thread!");
    ITF_ASSERT_MSG(m_AnimMeshScene, "Anim light component not loaded correctly");

    if (!m_AnimMeshScene)
        return;

    // clear events ...
    m_frameEvents.clear();

    updateMeshMatrix();
    if (!fillAnimInfo())
        return;
    //force
    m_AnimMeshScene->m_AnimInfo.m_minimalBonesUpdate = bfalse;
    mainJobUpdateVisuals(this, btrue, bfalse);

    // for non procedural anims, copy bone list from working
    if (m_AnimMeshScene->m_AnimInfo.m_useWorking)
    {
        m_AnimMeshScene->m_AnimInfo.copyWorkingToCurrent();
    }
}

void AnimLightComponent::forceUpdateFlip()
{
    if ( m_AnimMeshScene )
    {
        AnimInfo* pAnimInfo = &m_AnimMeshScene->m_AnimInfo;

        if ( pAnimInfo )
        {
            bbool lookRight = isLookingRight();

            if (m_previousLookRight != lookRight )
            {
                if (pAnimInfo->m_prevPercent > 0.f)
                {
                    pAnimInfo->clearPrev();
                }
                m_previousLookRight = lookRight;
            }

            pAnimInfo->m_IsLookingRight = lookRight;
            pAnimInfo->SetFrame();
        }
    }
}

void AnimLightComponent::updateAnimTime( f32 _dt )
{
    u32 numAnims = m_currentFrameSubAnims.size();

    m_frameEvents.clear();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];

        SubAnim* subAnim = anim.m_subAnimFrameInfo.getSubAnim();
        if ( subAnim && subAnim->getTrack() )
        {
            if (!m_forceDirectUpdate || m_semiForcedDirectUpdate)
                anim.m_subAnimFrameInfo.updateAnimTime(_dt,&m_frameEvents);
        }
    }

    commitSubAnimsToPlay();
}

void AnimLightComponent::updateEvents()
{
    if (m_ignoreEvents || m_frameEvents.size() == 0 || !getNumPlayingSubAnims() )
        return;

    const SubAnim* currentSubAnim = getCurSubAnim(0);

    if ( !currentSubAnim )
    {
        return;
    }

    AnimSkeleton* skeleton = m_subAnimSet.getSkeleton();
    u32 rootIndex = skeleton->getRootIndex();
    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getWorkingBoneList();

    if ( rootIndex >= boneDynList.size() )
    {
        return;
    }

    u32 numEvents = m_frameEvents.size();
    
    //AnimBoneDyn *rootBone = &boneDynList[rootIndex];
    //AnimBoneDyn *eventBone;
    //const Matrix44  & matrix = m_AnimMeshScene->m_pMesh[0]->getMatrix();

    for ( u32 i = 0; i < numEvents; i++ )
    {
           AnimMarkerEvent* trackEvent = m_frameEvents[i];
//         eventBone = trackEvent->getBoneParent() != U32_INVALID ? &boneDynList[trackEvent->getBoneParent()] : rootBone;
//         Vec3d pos = (eventBone->m_Pos + AddAngleBTW(Vec2d(eventBone->m_Scale.x(),0) + trackEvent->getPosLocal(), eventBone->m_Angle)).to3d();
//         matrix.transformPoint(pos, pos);
//         trackEvent->setPos(pos.truncateTo2D());

        if ( trackEvent->GetObjectClassCRC() == AnimPolylineEvent_CRC )
        {
            AnimPolylineEvent* polyEvent = static_cast<AnimPolylineEvent*>(trackEvent);
            AnimPolyline *  pPolyline   = getCurrentPolyline(polyEvent->getPolylineID(), btrue);
            if (pPolyline)
            {
                u32             numPoints   = pPolyline->m_PolylinePointList.size();
                const Vec2d*    points      = getPolylinePoint(polyEvent->getPolylineID());
                polyEvent->setPoints(points,numPoints);
            }
            else
            {
#ifndef ITF_FINAL
                if ( m_currentFrameSubAnims.size() )
                {
                    String8 warningString;
                    
                    for (u32 animIdx=0; animIdx<m_currentFrameSubAnims.size(); animIdx++ )
                    {
                        const AnimLightFrameInfo& info = m_currentFrameSubAnims[animIdx];
                        const SubAnim* subAnim = info.m_subAnimFrameInfo.getSubAnim();
                        if (subAnim)
                        {
                            if (animIdx != 0)
                                warningString += " or ";
                            warningString += "'";
                            warningString += subAnim->getAnimPath().toString8();
                            warningString += "'";
                        }
                    }

                    if ( !warningString.isEmpty() )
                    {
#if defined(ITF_DEBUG_STRINGID)
                        ITF_WARNING_CATEGORY(Anim,m_actor, 0, "Actor '%s' has an invalid polyline event '%s' in animation %s: ", m_actor->getTemplatePath().toString8().cStr(),
                                                                                                                       trackEvent->getName().getDebugString(),
                                                                                                                       warningString.cStr());
#else
                        ITF_WARNING_CATEGORY(Anim,m_actor, 0, "Actor '%s' has an invalid polyline event in animation '%s': ", m_actor->getTemplatePath().toString8().cStr(),
                                                                                                                  warningString.cStr());
#endif
                    }
                }
                else
                {
                    ITF_WARNING_CATEGORY(Anim,m_actor, 0, "Actor '%s' has an invalid polyline event: ", m_actor->getTemplatePath().toString8().cStr());
                }
#endif // ITF_FINAL
                continue;
            }
        }

        trackEvent->setSender(m_actor->getRef());
        m_actor->onEvent(trackEvent);
    }
}

AABB AnimLightComponent::getVisualAABB() const
{
    if ( !getTemplate() )
    {
        return GetActor()->getAABB();
    }
    else
    {        
        AABB AABBtemp(m_visualAABB);
        AABBtemp.Scale(getScale());
        if (m_actor->getIsFlipped())
            AABBtemp.FlipHorizontaly(bfalse);
        AABBtemp.Rotate(getOffsettedAngle(), bfalse);
        AABBtemp.Translate(getOffsettedPos().truncateTo2D());
        return AABBtemp;
    }
}


void AnimLightComponent::fillAnimsAABBLocal(AABB &animAABB) const
{
    ITF_VECTOR<AnimFrameInfo>::const_iterator frameInfoInter = m_animFrameInfo.begin();
    for (; frameInfoInter != m_animFrameInfo.end(); ++frameInfoInter)
    {
        AnimTrack * track = m_subAnimSet.getSubAnim((*frameInfoInter).m_subAnimIdx)->getTrack();
        if (animAABB.isValid())
        {
            animAABB.grow(track->m_bounding);
        } else
        {
            animAABB = track->m_bounding;
        }
    }
}

void AnimLightComponent::transformLocalAABB(AABB& _aabb) const
{
    _aabb.Scale(getScale() * (m_actor->getIs2D() ? g_pixelBy2DUnit : 1.f) );
    if (m_actor->getIsFlipped())
        _aabb.FlipHorizontaly(bfalse);
    if (m_actor->getIs2D())
        _aabb.FlipVerticaly(bfalse);
    _aabb.Rotate(getOffsettedAngle(), bfalse);
    _aabb.Translate(getOffsettedPos().truncateTo2D());
}

AABB AnimLightComponent::getAnimsAABB() const
{
    AABB animAABB;
    animAABB.invalidate();

    fillAnimsAABBLocal(animAABB);
    if (!animAABB.isValid())
    {
        return getVisualAABB();
    }
    
    transformLocalAABB(animAABB);
    
    return animAABB;
}

void AnimLightComponent::updateAABB()
{
    ITF_ASSERT(isLoaded());
    Actor * actor = GetActor();

    actor->growAABB(m_AnimMeshScene->m_AnimInfo.m_visualAABB);
}

void AnimLightComponent::computeVisiblePrediction()
{
    bbool isFullVisiblePrediction = bfalse;

    if (m_alpha <= MTH_EPSILON )
    {
        isFullVisiblePrediction =  bfalse;
    } else if (needsDraw2D()/* || getDisableVisiblePrediction()*/ )
    {
        isFullVisiblePrediction = btrue;
    } else
    {
        const Actor * actor = GetActor();

        const ITF_VECTOR<View*>& views = WORLD_MANAGER->getWorldUpdate().getViews();
        for(u32 iView = 0; iView < views.size() && !isFullVisiblePrediction; ++iView)
        {
            View* pCurrentView = views[iView];        
            if(!pCurrentView->isActive())
                continue;

            if(!pCurrentView->getCamera()) 
                continue;
                
            isFullVisiblePrediction |= pCurrentView->getCamera()->getViewFrustum().isInFrustum(actor->getAABB3d().getMin(),actor->getAABB3d().getMax());
        }
    }

    if (m_isVisiblePrediction != u32(isFullVisiblePrediction))
    {
        m_isVisiblePrediction = isFullVisiblePrediction;
        // force recompute when visible prediction change
        m_samePrevAnimFrameInfo = bfalse;
    }
}

Vec2d AnimLightComponent::getOffset() const
{
    if (getUseBase())
    {
        Vec2d offset = m_posOffset+m_instancePositionOffset;
        offset.y() -= m_actor->getRadius();
        return offset.Rotate( getOffsettedAngle() );
    }
    else
    {
        Vec2d offset = m_posOffset+m_instancePositionOffset;
        return offset.Rotate( getOffsettedAngle() );
    }
}

AnimBoneDyn* AnimLightComponent::getBoneForUpdate(u32 _boneIndex, bbool _updateLocal)
{
    if (!isLoaded())
        return NULL;

    ITF_WARNING(m_actor, isProcedural(), "Modifying bone on non procedural animation !");
    m_boneModification = btrue;
    m_boneModificationLocal = m_boneModificationLocal || _updateLocal;

    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (!boneDynList.size())
        return NULL;

    if (_boneIndex == U32_INVALID || !isBoneDataOk(_boneIndex))
    {
        _boneIndex = m_rootIndex;
    }

    ITF_ASSERT_CRASH( _boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");
    return &boneDynList[_boneIndex];
}

//////////////////////////////////////////////////////////////////////////
bbool AnimLightComponent::getBonePos (u32 _boneIndex, Vec2d& _dest, bbool _getBoneEnd) const
    //////////////////////////////////////////////////////////////////////////
{
    if (!isLoaded())
        return bfalse;

    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (!boneDynList.size())
        return bfalse;

    if (!isBoneDataOk(_boneIndex))
    {
        _boneIndex = m_rootIndex;
    }

    ITF_ASSERT_CRASH( _boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");

    if (GetActor()->getIs2D())
    {
        _dest = boneDynList[_boneIndex].m_Pos;
        if (_getBoneEnd )
        {
            _dest += boneDynList[_boneIndex].m_XAxe;
        }
    } else
    {
        Vec3d dest;
        m_AnimMeshScene->m_pMesh[0]->getMatrix().transformPoint(dest, boneDynList[_boneIndex].m_Pos.to3d());
        _dest = dest.truncateTo2D();
        if (_getBoneEnd )
        {
            Vec3d axe;
            m_AnimMeshScene->m_pMesh[0]->getMatrix().transformVector(axe, boneDynList[_boneIndex].m_XAxe.to3d());
            _dest += axe.truncateTo2D();
        }
    }
    return btrue;
}
//////////////////////////////////////////////////////////////////////////
bbool AnimLightComponent::getBonePos (u32 _boneIndex, Vec3d& _dest, bbool _getBoneEnd) const
    //////////////////////////////////////////////////////////////////////////
{
    Vec2d res;
    if (!getBonePos(_boneIndex, res, _getBoneEnd) )
    {
        return bfalse;
    }
    
    // get bone infos
    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (!boneDynList.size())
    {
        return bfalse;
    }

    if (!isBoneDataOk(_boneIndex))
    {
        _boneIndex = m_rootIndex;
    }

    ITF_ASSERT_CRASH(_boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");
    
    // use actor's depth and apply bone Z offset
    f32 depth = m_actor->getDepth();
    if (m_zOffsetActive)
        depth += boneDynList[_boneIndex].m_Zorder > 0.0f ? getTemplate()->getFrontZOffset() : getTemplate()->getBackZOffset();

    _dest.set(res.x(), res.y(), depth);
    
    return btrue;
}
//////////////////////////////////////////////////////////////////////////
bbool AnimLightComponent::getBoneAngle(u32 _boneIndex, f32& _angle, bbool _useFlip) const
    //////////////////////////////////////////////////////////////////////////
{
    if ( _useFlip )
    {
        Vec2d orientation;
        if (getBoneOrientation(_boneIndex, orientation))
        {
            _angle = orientation.getAngle();
            return btrue;
        }
        return bfalse;
    }
    else
    {
        if (!isLoaded())
            return bfalse;

        SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
        if (!boneDynList.size())
            return bfalse;

        if (!isBoneDataOk(_boneIndex))
        {
            _boneIndex = m_rootIndex;
        }

        ITF_ASSERT_CRASH( _boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");


        _angle = boneDynList[_boneIndex].m_Angle + GetActor()->getAngle();
        return btrue;
    }
}

//////////////////////////////////////////////////////////////////////////
bbool AnimLightComponent::getBoneOrientation (u32 _boneIndex, Vec2d& _orientation) const
    //////////////////////////////////////////////////////////////////////////
{
    if (!isLoaded())
        return bfalse;

    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (!boneDynList.size())
        return bfalse;

    if (!isBoneDataOk(_boneIndex))
    {
        _boneIndex = m_rootIndex;
    }

    ITF_ASSERT_CRASH( _boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");
    
    if (GetActor()->getIs2D())
    {
        _orientation = boneDynList[_boneIndex].m_XAxe;
    } else
    {
        Vec3d orientation;
        m_AnimMeshScene->m_pMesh[0]->getMatrix().transformVector(orientation, boneDynList[_boneIndex].m_XAxe.to3d());
        _orientation = orientation.truncateTo2D();
        _orientation.normalize();
    }
    return btrue;
}
//////////////////////////////////////////////////////////////////////////
bbool AnimLightComponent::getBoneScale(u32 _boneIndex, Vec2d& _dest) const
    //////////////////////////////////////////////////////////////////////////
{
    if (!isLoaded())
        return bfalse;

    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (!boneDynList.size())
        return bfalse;
    
    if (!isBoneDataOk(_boneIndex))
    {
        _boneIndex = m_rootIndex;
    }

    ITF_ASSERT_CRASH( _boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");

    _dest = boneDynList[_boneIndex].m_Scale;
    return btrue;
}

//////////////////////////////////////////////////////////////////////////
bbool AnimLightComponent::setBoneScale(u32 _boneIndex, Vec2d& _scale)
    //////////////////////////////////////////////////////////////////////////
{
    if (!isLoaded())
        return bfalse;

    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (!boneDynList.size())
        return bfalse;

    if (!isBoneDataOk(_boneIndex))
    {
        _boneIndex = m_rootIndex;
    }

    ITF_ASSERT_CRASH( _boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");

    boneDynList[_boneIndex].m_Scale = _scale;
    return btrue;
}

//////////////////////////////////////////////////////////////////////////
bbool AnimLightComponent::getBoneAlpha(u32 _boneIndex, f32& _alpha) const
    //////////////////////////////////////////////////////////////////////////
{
    if (!isLoaded())
        return bfalse;

    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (!boneDynList.size())
        return bfalse;

    if (!isBoneDataOk(_boneIndex))
    {
        _boneIndex = m_rootIndex;
    }

    ITF_ASSERT_CRASH( _boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");

    _alpha = boneDynList[_boneIndex].m_Alpha;
    return btrue;
}

//////////////////////////////////////////////////////////////////////////
bbool AnimLightComponent::setBoneAlpha(u32 _boneIndex, f32& _alpha)
    //////////////////////////////////////////////////////////////////////////
{
    if (!isLoaded())
        return bfalse;

    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (!boneDynList.size())
        return bfalse;

    if (!isBoneDataOk(_boneIndex))
    {
        _boneIndex = m_rootIndex;
    }

    ITF_ASSERT_CRASH( _boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");

    boneDynList[_boneIndex].m_Alpha = _alpha;
    return btrue;
}

//////////////////////////////////////////////////////////////////////////
bbool AnimLightComponent::getBoneLenght(u32 _boneIndex, f32& _lenght) const
    //////////////////////////////////////////////////////////////////////////
{
    if (!isLoaded())
        return bfalse;

    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (!boneDynList.size())
        return bfalse;

    if (!isBoneDataOk(_boneIndex))
    {
        _boneIndex = m_rootIndex;
    }

    ITF_ASSERT_CRASH( _boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");

    //m_AnimMeshScene->m_pMesh[0]->getMatrix().transformVector(_orientation, boneDynList[_boneIndex].m_XAxe);
    _lenght = boneDynList[_boneIndex].m_XAxeLenght * GetActor()->getScale().x();
    return btrue;
}
//////////////////////////////////////////////////////////////////////////
i32  AnimLightComponent::getBoneIndex (const StringID& _boneName) const
    //////////////////////////////////////////////////////////////////////////
{
    if (!isLoaded() )
        return U32_INVALID;
    AnimSkeleton *skeleton = m_subAnimSet.getSkeleton();
    return skeleton->getBoneIndex(_boneName);
}


u32  AnimLightComponent::getBoneCount() const
{
    if (!isLoaded() || !getNumPlayingSubAnims() )
        return U32_INVALID;

    AnimSkeleton *skeleton = m_subAnimSet.getSkeleton();
    return skeleton->m_BonesCurFrame.size();
}

AnimPolyline * AnimLightComponent::getCurrentPolyline(const StringID & _polyId, bbool _useWorking) const
{
     //ITF_ASSERT_MSG(isLoaded() && getNumPlayingSubAnims(), "Trying to get polyline on invalid animation");
    if (!isLoaded() || !getNumPlayingSubAnims() )
        return NULL;

    AnimSkeleton                    *skeleton       = m_AnimMeshScene->m_AnimInfo.getSkeleton();
    ITF_VECTOR<StringID>            &polyRefList    = _useWorking ? m_AnimMeshScene->m_AnimInfo.getWorkingPolyList() : m_AnimMeshScene->m_AnimInfo.getCurrentPolyList();

    for ( ITF_VECTOR<StringID>::const_iterator iter = polyRefList.begin();
        iter != polyRefList.end(); iter++ )
    {
        const StringID& polyRef = *iter;
        if (_polyId != polyRef)
            continue;

        AnimPolyline *pPolyline;
        if ( skeleton->getPolyline(polyRef, pPolyline) )
        {
            return pPolyline;
        }
    }
    return NULL;
}

Vec2d * AnimLightComponent::getCurrentPolylinePointBuffer(const StringID & _polyId, u32 * _size, AnimPolyline::PolylineType * _type)
{
    if (!isLoaded() || !getNumPlayingSubAnims() )
    {
        if (_size)
            *_size = 0;
        if (_type)
            *_type = AnimPolyline::shape_error;
        return NULL;
    }

    AnimPolyline* pPolyline = getCurrentPolyline(_polyId);
    if (!pPolyline)
    {
        if (_size)
            *_size = 0;
        if (_type)
            *_type = AnimPolyline::shape_error;
        return NULL;
    }

    if (_size)
        *_size = pPolyline->m_PolylinePointList.size();
    if (_type)
        *_type = pPolyline->m_type;
    return getPolylinePoint(_polyId);
}


Vec2d * AnimLightComponent::getPolylinePoint(const StringID & _polyId)
{
    AnimPolyline* pPolyline = getCurrentPolyline(_polyId);
    if (!pPolyline)
        return NULL;

    u32 numPoints = pPolyline->m_PolylinePointList.size();
    Vec2d* points       = m_AnimMeshScene->m_AnimInfo.getPolylinePointBuffer(_polyId, btrue);
    Vec2d* pointsBegin  = m_AnimMeshScene->m_AnimInfo.getPolylinePointBufferBegin();
    u32    bufferSize   = m_AnimMeshScene->m_AnimInfo.getPolylinePointBufferSize();

    u32 offset = uSize(points - pointsBegin);
    m_globalPolylinePoint.resize(bufferSize);
    Matrix44 & m  = m_AnimMeshScene->m_pMesh[0]->getMatrix();
    Vec3d tmpVect;
    for (u32 i=0; i<numPoints; i++)
    {
        m.transformPoint(tmpVect, points[i].to3d());
        m_globalPolylinePoint[i+offset] = tmpVect.truncateTo2D();
    }
    return &m_globalPolylinePoint[offset];
}


Vec2d * AnimLightComponent::getCurrentFirstPolylinePoint(const StringID & _polyPointName, AnimPolyline ** _polyline, u32 * _index)
{
    ITF_ASSERT_MSG(isLoaded() && getNumPlayingSubAnims(), "Trying to get polyline on invalid animation");
    if (!isLoaded() || !getNumPlayingSubAnims() )
        return NULL;

    AnimSkeleton                    *skeleton   = m_AnimMeshScene->m_AnimInfo.getSkeleton();
    ITF_VECTOR<StringID>&           polyRefList = m_AnimMeshScene->m_AnimInfo.getCurrentPolyList();

    for ( ITF_VECTOR<StringID>::const_iterator iter = polyRefList.begin();
        iter != polyRefList.end(); iter++ )
    {
        const StringID& polyRef = *iter;
        AnimPolyline *pPolyline;
        if ( skeleton->getPolyline(polyRef, pPolyline) )
        {
            for ( NamesMap::Iterator it(pPolyline->m_NameToPolylinePoint); !it.isEnd(); it.increment() )
            {
                if ( it.first() == _polyPointName )
                {
                    if (_polyline)
                        *_polyline = pPolyline;
                    if (_index)
                        *_index = it.second();

                    Vec2d* points = getPolylinePoint(polyRef);
                    return &points[it.second()];
                }
            }
        }
    }
    return NULL;
}

bbool AnimLightComponent::animToQuad(View* _refView)
{
    if (!m_AnimMeshScene->m_renderInTexture)
        return bfalse;
    RenderInTexture * renderInTexture = m_AnimMeshScene->m_renderInTexture;

    Quad3DPrimImpostor & quad = renderInTexture->m_quad;
    if (!quad.getMaterial() || !quad.getMaterial()->getTexture(TEXSET_ID_DIFFUSE))
    {
        renderInTexture->m_material.setTexture(TEXSET_ID_DIFFUSE, renderInTexture->m_renderTarget);
        renderInTexture->m_material.setBlendMode(GFX_BLEND_ALPHAPREMULT);
        quad.setMaterial(&renderInTexture->m_material);
    }

    quad.adjustZPassFilterFlag(GFX_OCCLUDE_INFO_SMALL_OR_TRANSPARENT);

    AABB & visualAABB = m_AnimMeshScene->m_AnimInfo.m_visualAABB;
    renderInTexture->m_aabb = AABB(-visualAABB.getSize()*0.5f, 
                                    visualAABB.getSize()*0.5f);

    quad.m_size             = visualAABB.getSize();
    quad.m_pos              = visualAABB.getCenter().to3d(GetActor()->getDepth());
    quad.m_rotation         = Vec3d::Zero;
    quad.m_color            = m_AnimMeshScene->getCommonParam()->m_colorFactor.getAsU32();

    if (!renderInTexture->computeViewport(_refView))
        return bfalse;

    renderInTexture->m_material.setLinearFiltering(quad.m_uvData.getUV1().x() >= 1.f || quad.m_uvData.getUV1().y() >= 1.f);

    ITF_Mesh  * pMesh = m_AnimMeshScene->m_pMesh[0];
    Matrix44 & matrix = pMesh->getMatrix();
    matrix.setTranslation(getOffsettedPos() - quad.m_pos);

    return btrue;
}

#ifdef ITF_SUPPORT_EDITOR
void AnimLightComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    Super::drawEdit(_drawInterface, _flags);

    if (!CHEATMANAGER || !CHEATMANAGER->isDisplayBones()) return;
    if (!m_AnimMeshScene) return;
    if (!m_AnimMeshScene->m_pMesh[0] ) return;

    AnimInfo    *pAnimInfo = &m_AnimMeshScene->m_AnimInfo;
    Matrix44    *pMatrix = &m_AnimMeshScene->m_pMesh[0]->getMatrix();
    AnimSkeleton * skl = pAnimInfo->getSkeleton();
    if (!skl) return;


    SafeArray<AnimBoneDyn> & boneDynList = pAnimInfo->getCurrentBoneList();
    u32 bCount = boneDynList.size();

    u32 bestIndex = U32_INVALID;
    f32 bestDistance = 65;
    Vec2d P0, P1;

    for (u32 bIdx = 0; bIdx < bCount; bIdx++)
    {
        const AnimBoneDyn& bone = boneDynList[bIdx];
        Vec3d temp, p0, p1;
        pMatrix->transformPoint(temp, bone.m_Pos.to3d());
        View::getCurrentView()->compute3DTo2D(temp, p0 );
        if (p0.z() < -0.01) continue;
        pMatrix->transformPoint(temp, (bone.m_Pos + bone.m_XAxe).to3d());
        View::getCurrentView()->compute3DTo2D(temp, p1 );
        if (p1.z() < -0.01) continue;

        Vec2d p0p1 = p1.truncateTo2D() - p0.truncateTo2D();
        f32 ratio = p0p1.sqrnorm();
        if (ratio == 0) continue;
        Vec2d p0m = _drawInterface->m_currentMouse2d - p0.truncateTo2D();
        ratio = p0p1.dot(p0m) / ratio;
        if (ratio < 0 || ratio > 1) continue;

        Vec2d distVec = p0m - ratio * p0p1;
        ratio = distVec.sqrnorm();
        if (ratio > bestDistance) continue;

        bestIndex = bIdx;
        bestDistance = ratio;
        P0 = p0.truncateTo2D();
        P1 = p1.truncateTo2D();
    }

    if (bestIndex != U32_INVALID)
    {
        AnimBone *boneStatic = skl->getBoneAt(bestIndex);
        if (!boneStatic) return;
        
        String8 text;
        const char *boneName = boneStatic->m_Name.getDebugString();
        if (boneName[0] == '0' && boneName[1] == 'x')
            text.setTextFormat("[%d]", boneStatic->getIndex());
        else
            text.setTextFormat("[%d] %s", boneStatic->getIndex(), boneStatic->m_Name.getDebugString() );

        f32 width, height;
        if (!GFX_ADAPTER->drawDBGTextSize( text, width, height)) return;

        Vec2d posText = (P0 + P1) / 2;
        posText.x() -= width / 2 - 4;
        posText.y() -= height / 2 - 4;

        GFX_ADAPTER->drawDBG2dLine( P0, P1, 5.f, Color::red().getAsU32() );

        u32 C = Color::red().getAsU32();
        GFX_ADAPTER->drawDBG2dBox( posText, width + 8, height + 8, C, C, C, C, 0 );
        C = 0xC0000000;
        posText.x() += 1;
        posText.y() += 1;
        GFX_ADAPTER->drawDBG2dBox( posText, width + 6, height + 6, C, C, C, C, 1 );
        posText.x() += 3;
        posText.y() += 3;
        GFX_ADAPTER->drawDBGText(text, Color::red(), posText.x(), posText.y() );
        
    }

}
#endif

void AnimLightComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
{
    // Exit on Component Inactive
    if (getInvisible())
        return;
                                            
    STATS_ANIMLIGHTCOMPONENT_DRAWINC

    ITF_ASSERT_CRASH(m_AnimMeshScene, "Trying to draw non loaded actor!");
    if (!getNumPlayingSubAnims() || !isVisiblePrediction())
        return;

    // do that because if primitive parameters are changed by some tools while editor is in pause
    // m_AnimMeshScene primitive parameters are not updated
    GFXPrimitiveParam &primParam        = *(m_AnimMeshScene->getCommonParam());
    primParam							= getGfxPrimitiveParam();
    primParam.m_colorFactor				= primParam.m_colorFactor * getColor();
    primParam.m_colorFactor.m_a			*= getAlpha();

    f32 depth = m_actor->getDepth() + getDepthOffset();

    if (getTemplate()->allowRenderInTexture() &&
        m_AnimMeshScene->getCommonParam()->m_colorFactor.m_a < (1.f - MTH_EPSILON))
    {
        m_AnimMeshScene->allocRenderTarget(GFX_ADAPTER, getTemplate()->renderInTectureSizeX(), getTemplate()->renderInTectureSizeY());

        bbool registerOnce = false;
        for ( ux i = 0; i < _views.size(); i++ )
        {
            if ( _views[i]->isObjectVisibleInViewport(m_AnimMeshScene->getCommonParam()->m_viewportVisibility) )
            {
                ITF_WARNING(m_actor, registerOnce == false, "Cannot use RenderInTarger with more than 1 view");
                if ( ( registerOnce == false ) && (!_views[i]->isDisabledForRendering()) && animToQuad(_views[i]) )
                {
					m_AnimMeshScene->getCommonParam()->m_colorFactor = Color::white();
                    GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_PRERENDER_PRIM>(*_views[i], m_AnimMeshScene, depth, GetActor()->getRef()); 
                    GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(*_views[i], &m_AnimMeshScene->m_renderInTexture->m_quad, depth, GetActor()->getRef()); 
                    registerOnce = true;
                }
            }
        }

        return;
    }


    if ( useShadow() )
        drawShadow(_views);

    if (m_AnimMeshScene->m_pAnimDraw.getPassZList().size())
    {
        f32 zDelta = getTemplate()->getZAmplify();
        if (zDelta == 0.f)
            zDelta = 10*MTH_EPSILON; // prevent z Problems

        const ITF_VECTOR<f32> & zPassList = m_AnimMeshScene->m_pAnimDraw.getPassZList();
        ITF_VECTOR<f32>::const_iterator zIter = zPassList.begin();
        for (u32 renderPassIdx = 0; zIter != zPassList.end(); ++zIter, renderPassIdx++)
        {
            addMeshSceneForRenderPass(_views, renderPassIdx, depth + zDelta*(*zIter) );
        }
    }
    else if ( m_zOffsetActive && (getTemplate()->getFrontZOffset() != getTemplate()->getBackZOffset()))
    {
        if (m_AnimMeshScene->m_AnimInfo.m_renderPassFrontierPatchIndex) // avoid to send the mesh in case it's invalid
        {
            addMeshSceneForRenderPass(_views, 0, depth + getTemplate()->getBackZOffset());
        }
        addMeshSceneForRenderPass(_views, 1, depth + getTemplate()->getFrontZOffset());
    }
    else
    {
        if (m_zOffsetActive)
            depth += getTemplate()->getFrontZOffset();
        
        if (m_basicRender)
            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, m_AnimMeshScene, depth, m_actor->getRef());
        if (m_emitFluid)
            GFX_ADAPTER->getGridFluidManager().batchFluidPrimitive(m_AnimMeshScene, m_AnimMeshScene->m_AnimInfo.m_visualAABB, depth, m_fluidEmitterFilter, GFX_GRID_MOD_MODE_FLUID);
        if (m_attractFluid)
            GFX_ADAPTER->getGridFluidManager().batchFluidPrimitive(m_AnimMeshScene, m_AnimMeshScene->m_AnimInfo.m_visualAABB, depth, m_fluidEmitterFilter, GFX_GRID_MOD_MODE_ATTRACT);
    }
}


    
void AnimLightComponent::addMeshSceneForRenderPass(const ITF_VECTOR <class View*>& _views, u32 _renderPass, f32 _depth)
{
    m_AnimMeshSceneAllPass.resize(std::max( (size_t)(_renderPass + 1), (size_t)m_AnimMeshSceneAllPass.size()), NULL);
    if (m_AnimMeshSceneAllPass[_renderPass] == NULL)
    {
        m_AnimMeshSceneAllPass[_renderPass] = newAlloc(mId_Animation, AnimMeshScenePass) (m_AnimMeshScene, _renderPass); 
        m_AnimMeshSceneAllPass[_renderPass]->setPassFilterFlag(m_AnimMeshScene->getPassFilterFlag());
    }
    m_AnimMeshSceneAllPass[_renderPass]->getCommonParam()->m_gfxMaskInfo = getGfxPrimitiveParam().m_gfxMaskInfo;
    if (m_basicRender)
        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, m_AnimMeshSceneAllPass[_renderPass], _depth, m_actor->getRef());
    if (m_emitFluid)
        GFX_ADAPTER->getGridFluidManager().batchFluidPrimitive(m_AnimMeshSceneAllPass[_renderPass], m_AnimMeshScene->m_AnimInfo.m_visualAABB, _depth, m_fluidEmitterFilter, GFX_GRID_MOD_MODE_FLUID);
    if (m_attractFluid)
        GFX_ADAPTER->getGridFluidManager().batchFluidPrimitive(m_AnimMeshSceneAllPass[_renderPass], m_AnimMeshScene->m_AnimInfo.m_visualAABB, _depth, m_fluidEmitterFilter, GFX_GRID_MOD_MODE_ATTRACT);
}

void AnimLightComponent::addMeshSceneForRenderPass2d(const ITF_VECTOR <class View*>& _views, u32 _renderPass, f32 _depth)
{
    m_AnimMeshSceneAllPass.resize(std::max( (size_t)(_renderPass + 1), (size_t)m_AnimMeshSceneAllPass.size()), NULL);
    if (m_AnimMeshSceneAllPass[_renderPass] == NULL)
    {
        m_AnimMeshSceneAllPass[_renderPass] = newAlloc(mId_Animation, AnimMeshScenePass) (m_AnimMeshScene, _renderPass); 
        m_AnimMeshSceneAllPass[_renderPass]->setPassFilterFlag(m_AnimMeshScene->getPassFilterFlag());
    }
    GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_2D>(_views, m_AnimMeshSceneAllPass[_renderPass], _depth, m_actor->getRef());
}

void AnimLightComponent::batchPrimitives2D( const ITF_VECTOR <class View*>& _views )
{
    ITF_ASSERT_CRASH(m_AnimMeshScene, "Trying to draw non loaded actor!");
    if (!getNumPlayingSubAnims() || !isVisiblePrediction())
        return;


    f32 depth = m_actor->getDepth() + getDepthOffset()  + m_rank2D;
    if ( m_zOffsetActive && (getTemplate()->getFrontZOffset() != getTemplate()->getBackZOffset()))
    {
        if (m_AnimMeshScene->m_AnimInfo.m_renderPassFrontierPatchIndex) // avoid to send the mesh in case it's invalid
        {
            addMeshSceneForRenderPass2d(_views, 0, f32_Max(0.0f, depth + getTemplate()->getBackZOffset()));
        }
        addMeshSceneForRenderPass2d(_views, 1, f32_Max(0.0f, depth + getTemplate()->getFrontZOffset()));
    } else
    {
        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_2D>(_views, m_AnimMeshScene, f32_Max(0.0f, depth), GetActor()->getRef());
    }
}

void AnimLightComponent::onUnloadResources()
{
    deleteAnimMesh();
}

bbool AnimLightComponent::isLoaded() const
{
    return m_actor && m_AnimMeshScene != NULL && m_actor->isPhysicalReady();
}

void AnimLightComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( EventPlayAnim* eventPlayAnim = DYNAMIC_CAST(_event,EventPlayAnim) )
    {
        processEventPlayAnim(eventPlayAnim);
    }
	else if( EventChangeGfxMaskInfo* eventChangeGfxMaskInfo = DYNAMIC_CAST(_event,EventChangeGfxMaskInfo) )
	{
		GFXPrimitiveParam& primitiveParam =  getGfxPrimitiveParam();
		primitiveParam.m_gfxMaskInfo = static_cast<GFXMaskInfo> (eventChangeGfxMaskInfo->getGfxMaskInfo());
	}
    else if ( EventReset* eventReset = DYNAMIC_CAST(_event,EventReset) )
    {
        processEventReset(eventReset);
    }
    else if ( EventShow* eventShow = DYNAMIC_CAST(_event,EventShow) )
    {
        processEventShow(eventShow);
    }
    else if (EventQueryPosition* onQueryPos = DYNAMIC_CAST(_event,EventQueryPosition))
    {
        const StringID & boneId = onQueryPos->getBoneId();
        if (boneId.isValid())
        {
            u32 boneIndex = getBoneIndex(boneId);
            if (boneIndex != U32_INVALID)
            {
                Vec3d pos;
                Vec2d orientation;
                if (getBonePos(boneIndex, pos, bfalse) && getBoneOrientation(boneIndex, orientation))
                {
                    onQueryPos->setPos(pos);
                    onQueryPos->setAngle(orientation.getAngle());
                }
            }
        }
    }
	else if(EventActivateReflections* eventReflections = DYNAMIC_CAST(_event, EventActivateReflections))
	{
		m_primitiveParam.m_renderInReflection = eventReflections->getRenderInReflections();
	}
	else if(EventChangeBankId* changeBankEvt = DYNAMIC_CAST(_event, EventChangeBankId))
	{
		if(changeBankEvt->getBankId().isValid())
		{
			m_subAnimSet.clearBankIdChange();
			m_subAnimSet.addBankIdChange(changeBankEvt->getBankId());
		}
	}
	else if(EventChangePatch* changePatchEvt = DYNAMIC_CAST(_event, EventChangePatch))
	{
		if(changePatchEvt->getClear())
			clearPatchChange();

		if( changePatchEvt->getBoneId().isValid() )
			setPatchChange( changePatchEvt->getBoneId(),  changePatchEvt->getPatchId());
	}
}

void  AnimLightComponent::processEventPlayAnim( EventPlayAnim* _playAnimEvent )
{
        if (setAnim(_playAnimEvent->getAnimToPlay(), _playAnimEvent->getNbBlendFrame(), _playAnimEvent->getForceReset(), _playAnimEvent->getAnimPriority()))
            resetCurTime();
}

void  AnimLightComponent::processEventReset( EventReset* _resetEvent )
{
    resetColor();
    m_colorSrc = m_colorDst = m_currentColor;
    m_colorBlendTime = m_colorBlendTotal = 0.f;
}

void  AnimLightComponent::processEventShow( EventShow* _eventShow )
{
    if ( _eventShow->getIsOverrideColor())
    {
        m_colorSrc = m_currentColor;
        m_colorBlendTotal = _eventShow->getTransitionTime();
        m_colorBlendTime = m_colorBlendTotal;

        if(_eventShow->getIsOverrideColor())
            m_colorDst = _eventShow->getOverrideColor();
        else
            m_colorDst = m_currentColor;
        
        if(_eventShow->getIsOverrideAlpha())
        {
            if(!_eventShow->getIsAlphaRatio())
                m_colorDst.setAlpha(_eventShow->getAlpha());
            else
                m_colorDst.setAlpha(m_currentColor.getAlpha()); // When it's an alpha ratio, it's handled directly in GraphicComponent::onEvent
        }
        else
        {
            m_colorDst.setAlpha(m_currentColor.getAlpha());
        }

        if ( m_colorBlendTotal <= 0.f )
        {
            m_colorSrc = m_currentColor = m_colorDst;
        }
    }
}

 bbool AnimLightComponent::setAnim(const StringID & _subAnim, u32 _blendFrames /*= U32_INVALID*/, bbool _forceResetTree /*= bfalse*/, u32 _priority /*= 0*/)
{
    if ( _priority >= m_lastAnimPrio || isAnimToPlayFinished())
    {
        // Play the animation if Prio is >= as current prio or if the animation if finished whatever the prio
        m_lastAnim = m_playAnimName = _subAnim;
        m_playAnimNumFrames = _blendFrames;
        mb_forceResetAnim = _forceResetTree;
        m_lastAnimPrio = _priority;
        return btrue;
    }
    else
        return bfalse;
}

void AnimLightComponent::commitPlayAnim()
{
    setCurrentBlendFrames(m_playAnimNumFrames);

    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        SubAnim* pSubAnim = anim.m_subAnimFrameInfo.getSubAnim();
        ITF_ASSERT(pSubAnim);
        if(pSubAnim)
        {
            EventAnimChanged onAnimChanged(pSubAnim->getFriendlyName());
            m_actor->onEvent(&onAnimChanged);
        }
    }

    resetSubAnimsToPlay();

    i32 newsubAnimIndex = m_subAnimSet.getSubAnimIndex(m_playAnimName);

    if ( newsubAnimIndex != -1 )
    {
        SubAnimFrameInfo frameInfo(&m_subAnimSet);
        frameInfo.setSubAnimIndex(newsubAnimIndex, m_startOffset);
        addSubAnimToPlay(frameInfo,1.f,btrue);
    }

    ITF_WARNING(m_actor, getNumPlayingSubAnims() > 0, "Trying to play an unexisting anim %s", m_playAnimName.getDebugString());

    m_playAnimName.invalidate();

    if( mb_forceResetAnim )
    {
        mb_forceResetAnim = bfalse;
        resetCurTime();
    }
} 


void AnimLightComponent::resetSubAnimsToPlay()
{
    m_currentFrameSubAnims.clear();

    resetSubAnimFrameInfoFlags();
}

i32 AnimLightComponent::addSubAnimToPlay(const StringID & _subAnim, f32 _weight, bbool _usePatches )
{
    i32 curAnimIndex = m_subAnimSet.getSubAnimIndex(_subAnim);

    ITF_WARNING(m_actor, curAnimIndex != -1, "trying to set invalid animation %s", _subAnim.getDebugString());
    if ( curAnimIndex != -1 )
    {
        SubAnimFrameInfo frameInfo(&m_subAnimSet);
        frameInfo.setSubAnimIndex(curAnimIndex, m_startOffset);
        return addSubAnimToPlay(frameInfo,_weight,_usePatches);
    }
    return -1;
}

bbool AnimLightComponent::removeSubAnimToPlay(const StringID & _subAnim )
{
    for (ITF_VECTOR<AnimLightFrameInfo>::iterator it = m_currentFrameSubAnims.begin(); it != m_currentFrameSubAnims.end(); it++)
    {
        SubAnim* subAnim = it->m_subAnimFrameInfo.getSubAnim();
        if(subAnim->getFriendlyName() == _subAnim)
        {
            m_currentFrameSubAnims.erase(it);
            return btrue;
        }
    }
    return bfalse;
}

void AnimLightComponent::setFrameForSubAnim(u32 _subAnimIndex, f32 _time, AnimEventContainer* _eventList)
{
    ITF_ASSERT(_subAnimIndex<m_currentFrameSubAnims.size());

    SubAnimFrameInfo &frameInfo = m_currentFrameSubAnims[_subAnimIndex].m_subAnimFrameInfo;
    frameInfo.setCurTime(frameInfo.getSubAnim()->getStart() + _time, _eventList);
}

void AnimLightComponent::resetSubAnimFrameInfoFlags()
{
    m_useRootRotation = bfalse;
    m_animDefaultFlip = bfalse;
    m_isLooped = bfalse;

    if (useShadow())
        setShadowMul(1.f);
}

void AnimLightComponent::updateSubAnimFrameInfoFlag(const SubAnimFrameInfo& _subAnimFrame)
{
    m_useRootRotation |= _subAnimFrame.getSubAnim()->getUseRootRotation();
    m_animDefaultFlip |= _subAnimFrame.getSubAnim()->getDefaultFlip();
    m_isLooped |= _subAnimFrame.getIsLooped();
    
    if (useShadow())
        setShadowMul(_subAnimFrame.getSubAnim()->getShadowMul());
}

i32 AnimLightComponent::addSubAnimToPlay( const SubAnimFrameInfo& _subAnimFrame, f32 _weight, bbool _usePatches )
{
    i32 index = (i32)m_currentFrameSubAnims.size();
    updateSubAnimFrameInfoFlag(_subAnimFrame);

    ITF_ASSERT_MSG(_subAnimFrame.getSubAnim(), "adding null sub anim !");
    m_currentFrameSubAnims.push_back(AnimLightFrameInfo(_subAnimFrame,_weight,_usePatches));
    return index;
}

void AnimLightComponent::commitSubAnimsToPlay()
{
    u32 numAnims = m_currentFrameSubAnims.size();
//    u32 prevNumAnims = m_animFrameInfo.size();

    if (m_animFrameInfo.size() != numAnims)
    {
        m_samePrevAnimFrameInfo = bfalse;
        m_animFrameInfo.resize(numAnims);
    }

    m_zOffsetActive = m_useZOffset && getTemplate()->getUseZOffset();

    AnimSkeleton * skl = m_subAnimSet.getSkeleton();
    m_currentPolylineList.clear();
    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        AnimFrameInfo& frameInfo = m_animFrameInfo[i];

        f32 prevFrame = frameInfo.m_fFrame;
        f32 prevPercent = frameInfo.m_percent;
        u32 prevSubAnimIdx = frameInfo.m_subAnimIdx;

        if ( getTemplate()->getSmoothAnim() ) 
            frameInfo.m_fFrame = f32_Max(0.f, anim.m_subAnimFrameInfo.getCurrentTime() / ANIMATION_FRAMERATE);
        else
            frameInfo.m_fFrame = f32_Max(0.0f, timeToFrame(anim.m_subAnimFrameInfo.getCurrentTime(),ANIMATION_FRAMERATE));
        frameInfo.m_subAnimIdx = anim.m_subAnimFrameInfo.getSubAnimIdx();
        frameInfo.m_percent    = anim.m_weight;
        frameInfo.m_useVisuals = anim.m_usePatches;

        m_samePrevAnimFrameInfo =   m_samePrevAnimFrameInfo
                                && (frameInfo.m_subAnimIdx == prevSubAnimIdx)
                                && (f32_Abs(frameInfo.m_fFrame - prevFrame) < MTH_EPSILON)
                                && (f32_Abs(frameInfo.m_percent - prevPercent) < MTH_EPSILON);

        if (m_subSkeletonIdx >= 0)
            frameInfo.m_bonesUsed  = skl->m_SubSkeletonConfig[m_subSkeletonIdx];
        
        SubAnim * subAnim = m_subAnimSet.getSubAnim(frameInfo.m_subAnimIdx);
        if(subAnim)
        {
            m_zOffsetActive       |= subAnim->getTemplate()->getIsForceZOffset();

            AnimTrack * track = subAnim->getTrack();
            if(track)
            {
                AnimTrackPolyline * poly = track->GetAnimPolylineFrame(frameInfo.m_fFrame);
                if (poly && poly->m_polylineRefs.size())
                {
                    m_currentPolylineList.insert(m_currentPolylineList.end(), poly->m_polylineRefs.begin(), poly->m_polylineRefs.end());
                }
            }
        }
    }

    if (m_currentPolylineList.empty() && m_currentPolylineList.size() == m_previusPolylineList.size())
        return;

    if (m_currentPolylineList == m_previusPolylineList)
        return;

    m_polySync = btrue;
    m_previusPolylineList.swap(m_currentPolylineList);

}

void AnimLightComponent::deleteAnimMesh()
{
    for(u32 i = 0; i<m_AnimMeshSceneAllPass.size(); ++i)
    {
        SF_DEL(m_AnimMeshSceneAllPass[i]);
    }
    m_AnimMeshSceneAllPass.resize(0);
    SF_DEL(m_AnimMeshScene);
}

void AnimLightComponent::setCurFrame(f32 _frame, AnimEventContainer* _eventList)
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        anim.m_subAnimFrameInfo.setCurTime(_frame * ANIMATION_FRAMERATE,_eventList);
    }
}

void AnimLightComponent::setCurFrameFromStart(f32 _frame, AnimEventContainer* _eventList)
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        anim.m_subAnimFrameInfo.setCurTime((anim.m_subAnimFrameInfo.getSubAnim()->getStart()+_frame) * ANIMATION_FRAMERATE,_eventList);
    }
}

void AnimLightComponent::resetCurTime(bbool _sendChangeEvent)
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        anim.m_subAnimFrameInfo.resetCurTime(m_startOffset);
    }
}


bbool AnimLightComponent::isAnimationsProcedural()
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        if(!anim.m_subAnimFrameInfo.getIsProcedural())
            return bfalse;
    }

    return btrue;
}

void AnimLightComponent::setProceduralCursor(f32 _cursor)
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        ITF_ASSERT(anim.m_subAnimFrameInfo.getIsProcedural());
        anim.m_subAnimFrameInfo.setCursor(_cursor);
    }
}

void AnimLightComponent::setProceduralCursorForSubAnim(u32 animIndex, f32 _cursor)
{
    ITF_ASSERT(animIndex<m_currentFrameSubAnims.size());

    AnimLightFrameInfo& anim = m_currentFrameSubAnims[animIndex];
    ITF_ASSERT(anim.m_subAnimFrameInfo.getIsProcedural());
    anim.m_subAnimFrameInfo.setCursor(_cursor);
}

bbool AnimLightComponent::isSubAnimFinished() const
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        const AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];

#if !defined(ITF_DISABLE_WARNING)
        SubAnim* subAnim = anim.m_subAnimFrameInfo.getSubAnim();
        ITF_WARNING_CATEGORY(GPP,m_actor, !subAnim->getIsLooped(), "Testing isSubAnimFinished on looping animation: %s",
            subAnim->getFriendlyName().getDebugString());
#endif // ITF_DISABLE_WARNING

        if ( anim.m_subAnimFrameInfo.isSubAnimFinished() )
        {
            return btrue;
        }
    }

    return bfalse;
}

void AnimLightComponent::setAnimLoop(bbool _loop)
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        anim.m_subAnimFrameInfo.setIsLooped(_loop);
    }
}

void AnimLightComponent::dbgGetCurAnims( u32 _index, String8 & _text) const
{
#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)

    if ( _index < m_currentFrameSubAnims.size() )
    {
        const AnimLightFrameInfo& anim = m_currentFrameSubAnims[_index];

        _text.setTextFormat("%s (weight: %.f%%, time %.2f, frame: %d of animation: %s)",
            anim.m_subAnimFrameInfo.getSubAnim()->getFriendlyName().getDebugString(),
            anim.m_weight * 100,
            anim.m_subAnimFrameInfo.getCurrentTime() - anim.m_subAnimFrameInfo.getSubAnim()->getStart(),
            static_cast<u32>(timeToFrame(anim.m_subAnimFrameInfo.getCurrentTime(),ANIMATION_FRAMERATE)),
            anim.m_subAnimFrameInfo.getSubAnim()->getAnimPath().getBasename());
    }
#endif
}

// ================================================================================================================
// Backup Old RootOffset for this animation.
u32 AnimLightComponent::getFirstRootedAnimationIndex()
{
    for (u32 index = 0; index < getNumPlayingSubAnims() ; index++)
    {
        if (canGetRootDelta(index))
            return index;
    }
    return U32_INVALID;
}

// ================================================================================================================
// Backup Old RootOffset for this animation.
void AnimLightComponent::processBackupRootInformation()
{
    u32 index = getFirstRootedAnimationIndex();
    if (index!= U32_INVALID)
    {
        m_previousRootInfo.m_rootOffsetFromStart = getRootDeltaCumulated(btrue, bfalse, index, &m_previousRootInfo.m_rootSubAnimIndex);
        m_previousRootInfo.m_rootAnimIndex = index;
    }
    else
        m_previousRootInfo.m_rootSubAnimIndex = U32_INVALID;
}


Vec2d AnimLightComponent::getPreviousRootDelta(f32 _dt, bbool * _ptr /*= NULL*/ )
{
    if (_ptr != NULL)
        *_ptr = btrue;

    // Get the Root Movment from the previous frame to the current frame
    Vec2d currentRootPosition;
    u32 currentRootSubAnimIndex =  U32_INVALID;
    f32 currentAnimPosition = 0.0f;
    u32 currentRootAnimIndex = getFirstRootedAnimationIndex();
    if (currentRootAnimIndex != U32_INVALID)
        currentRootPosition = getRootDeltaCumulated(btrue, bfalse, currentRootAnimIndex, &currentRootSubAnimIndex, &currentAnimPosition);
    u32 lastRootAnimIndex =  m_previousRootInfo.m_rootAnimIndex;
    u32 lastRootSubAnimIndex =  m_previousRootInfo.m_rootSubAnimIndex;
    Vec2d lastRootPosition = m_previousRootInfo.m_rootOffsetFromStart;
    if (lastRootSubAnimIndex == currentRootSubAnimIndex && currentRootAnimIndex == lastRootAnimIndex && lastRootSubAnimIndex!= U32_INVALID)
    {
        // Both last & current belong to this same valid animation.
        return currentRootPosition - lastRootPosition;
    }
    else if (currentRootSubAnimIndex != U32_INVALID)
    {
        // Current Animation if valid but we cannot use the last animation info
        f32 nextAnimPosition = currentAnimPosition + (_dt/LOGICDT);
        Vec2d nextRootPosition = getRootDeltaCumulated(btrue, bfalse, 0, NULL, NULL, &nextAnimPosition);
        return nextRootPosition - currentRootPosition;
    }
    if (_ptr != NULL)
        *_ptr = bfalse;
    return Vec2d::Zero;
}

Vec2d AnimLightComponent::getRootDelta(u32 _index, bbool * _ptr /*= NULL*/ )
{
    if (m_AnimMeshScene)
    {
        const ITF_VECTOR<AnimFrameInfo> &frameInfo = m_AnimMeshScene->m_AnimInfo.getVecAnimFrameInfo();

        if (frameInfo.size() > _index)
        {
            if (_ptr) *_ptr = btrue;

            const AnimFrameInfo &firstFrameInfo = frameInfo[_index];
            const SubAnim* subAnim = m_subAnimSet.getSubAnim(firstFrameInfo.m_subAnimIdx);
            AnimTrack * track = subAnim->getTrack();
            ITF_ASSERT(track);
            ITF_WARNING_CATEGORY(Anim,m_actor, track->m_rootPos.size(), "get delta on animation with Root Pos not set, please update data");
            if (track->m_rootPos.size() == 0)
            {
                if (_ptr) *_ptr = bfalse;
                return Vec2d::Zero;
            }

            u32 startFrame  = (u32)timeToFrame(subAnim->getStart(), ANIMATION_FRAMERATE);
            u32 stopFrame   = (u32)timeToFrame(subAnim->getStop(), ANIMATION_FRAMERATE);

            u32 nextFrame   = (u32)(firstFrameInfo.m_fFrame + 0.5f);
            u32 frame       = nextFrame-1;
            if (subAnim->getIsReversed())
                frame = nextFrame+1;

//             u32 frame       = (u32)(firstFrameInfo.m_fFrame + 0.5f);
//             u32 nextFrame   = frame+1;
//             if (subAnim->getIsReversed())
//                 nextFrame   = frame-1;

            if (frame < startFrame)
            {
                // We are between firstFrame & previous undefined frame so we use the speed from first to second Frame
                frame+= 1;
                nextFrame+= 1;
            }
            else if (frame == U32_INVALID)
            {
                frame = 0;
                nextFrame = 1;
            }

            ITF_ASSERT( startFrame <= frame && frame < stopFrame);
            frame = Min(frame, (u32)track->m_rootPos.size() - 1);
            if (nextFrame >= stopFrame)
            {
                if (_ptr) *_ptr = bfalse;
                nextFrame = frame;
            }

            Vec2d           rootDelta       = track->m_rootPos[nextFrame] - track->m_rootPos[frame];
            bbool           inverted        = getCurrentSubAnimDefaultRight() != isLookingRight();

            rootDelta      *= getScale();
            rootDelta.Rotate(inverted ? -m_actor->getAngle() : m_actor->getAngle());
            if (inverted)
                rootDelta.x()  *= -1.0f;

            return rootDelta * m_actor->getTimeFactor() * GAMEMANAGER->getSpeedFactor() * m_actor->getScale() * subAnim->getPlayRate();
        }
    }

    ITF_ASSERT_MSG(0, "trying to get root delta without loading animation");
    if (_ptr) *_ptr = bfalse;
    return Vec2d::Zero;
}

// get the root delta compared to frame 0
Vec2d AnimLightComponent::getRootDeltaCumulated( bbool _useFloatingFrame, bbool _useOriginAsStart, u32 _animIndex /*= 0*/, u32 * _animID /*= NULL*/, f32 * _animPos /*= NULL*/, f32 * _time /*= NULL*/ )
{
    // Default AnimID is Invalid
    if (_animID != NULL)
        *_animID = U32_INVALID;

    if (m_AnimMeshScene)
    {
        const ITF_VECTOR<AnimFrameInfo> &frameInfo = m_AnimMeshScene->m_AnimInfo.getVecAnimFrameInfo();
        ITF_ASSERT_MSG(frameInfo.size() > _animIndex, "Index out of range");
        if (frameInfo.size() > 0)
        {
            const AnimFrameInfo &firstFrameInfo = frameInfo[_animIndex];
            const SubAnim* subAnim = m_subAnimSet.getSubAnim(firstFrameInfo.m_subAnimIdx);
            AnimTrack * track = subAnim->getTrack();
            ITF_ASSERT(track);
            ITF_WARNING_CATEGORY(Anim,m_actor, track->m_rootPos.size(), "get delta on animation with Root Pos not set, please update data");
            if (track->m_rootPos.size() == 0)
                return Vec2d::Zero;

            // Return Current Anim Idx + Anim Position
            if (_animID != NULL)
                *_animID = firstFrameInfo.m_subAnimIdx;
            if (_animPos != NULL)
                *_animPos = firstFrameInfo.m_fFrame;

            Vec2d rootDelta;
            if (!_useFloatingFrame)
            {
                u32 frame = (u32)(firstFrameInfo.m_fFrame + 0.5f);
                if (_time != NULL)
                    frame = (u32)(*_time + 0.5f);

                u32 startFrame = (u32)timeToFrame(subAnim->getStart(), ANIMATION_FRAMERATE);
                u32 stopFrame  = (u32)timeToFrame(subAnim->getStop(), ANIMATION_FRAMERATE);

                ITF_ASSERT( startFrame <= frame && frame <= stopFrame);
                frame = Clamp(frame, startFrame, stopFrame - 1);

                rootDelta = _useOriginAsStart ? track->m_rootPos[frame] : track->m_rootPos[frame] - track->m_rootPos[startFrame] ;
            }
            else
            {
                f32 frame = firstFrameInfo.m_fFrame;
                if (_time != NULL)
                    frame = *_time;

                f32 frameBegin = floor(frame + 0.5f);
                f32 frameEnd = ceil(frame + 0.5f);
                f32 ratio = frame - frameBegin;

                f32 startFrame = timeToFrame(subAnim->getStart(), ANIMATION_FRAMERATE);
                f32 stopFrame  = timeToFrame(subAnim->getStop(), ANIMATION_FRAMERATE);

                if (frameBegin == stopFrame && ratio<= MTH_EPSILON) // Specific case when we call the function on the last frame of the animation
                    frameEnd = frameBegin;

                ITF_ASSERT( startFrame <= frameBegin && frameEnd <= stopFrame );
                frameBegin = Clamp(frameBegin, startFrame, stopFrame - 1);
                frameEnd = Clamp(frameEnd, startFrame, stopFrame - 1);

                rootDelta = _useOriginAsStart ? track->m_rootPos[(u32)frameBegin]*(1-ratio) + track->m_rootPos[(u32)frameEnd]*ratio : track->m_rootPos[(u32)frameBegin]*(1-ratio) + track->m_rootPos[(u32)frameEnd]*ratio - track->m_rootPos[(u32)startFrame];
            }

            bbool           inverted        = getCurrentSubAnimDefaultRight() != isLookingRight();

            rootDelta      *= getScale();
            rootDelta.Rotate(inverted ? -m_actor->getAngle() : m_actor->getAngle());
            if (inverted)
                rootDelta.x()  *= -1.0f;

            return rootDelta;
        }
    }

    ITF_ASSERT_MSG(0, "trying to get root delta without loading animation");
    return Vec2d::Zero;
}

void AnimLightComponent::setCurrentBlendFrames( u32 _blendFrames )
{
    if (!isLoaded())
    {
        return;
    }

    if ( _blendFrames != U32_INVALID )
    {
        if ( _blendFrames )
        {
            m_currentBlendFrames = static_cast<f32>(_blendFrames);
            m_AnimMeshScene->m_AnimInfo.setPrev(1.0f - m_blendDt/(m_currentBlendFrames/ANIMATION_FPS));
            if ( getCurrentSubAnimDefaultRight() != getCurrentSubAnimDefaultRight(btrue))
                m_AnimMeshScene->m_AnimInfo.flipPrev();
        }
        else
        {
            if ( m_currentBlendFrames )
            {
                m_AnimMeshScene->m_AnimInfo.clearPrev();
                m_currentBlendFrames = 0;
            }
        }
    }
    else
    {
        setCurrentBlendFrames(getTemplate()->getDefaultBlendFrames());
    }
}

f32 AnimLightComponent::getCurTime( u32 _index /*= 0*/ ) const
{
    ITF_ASSERT(_index < m_currentFrameSubAnims.size());

    const SubAnimFrameInfo& frameInfo = m_currentFrameSubAnims[_index].m_subAnimFrameInfo;
    SubAnim* subAnim = frameInfo.getSubAnim();

    if (frameInfo.getIsReversed())
    {
        return subAnim->getStop() - frameInfo.getCurrentTime();
    }
    else
    {
        return frameInfo.getCurrentTime() - subAnim->getStart();
    }
}

void AnimLightComponent::setCurTime( f32 _time, u32 _index /*= 0*/ )
{
    ITF_ASSERT(_index < m_currentFrameSubAnims.size());

    SubAnimFrameInfo& frameInfo = m_currentFrameSubAnims[_index].m_subAnimFrameInfo;
    SubAnim* subAnim = frameInfo.getSubAnim();

    if (frameInfo.getIsReversed())
    {
        frameInfo.setCurTime(subAnim->getStop() - _time);
    }
    else
    {
        frameInfo.setCurTime(subAnim->getStart() + _time);
    }
}

bbool AnimLightComponent::canGetRootDelta(u32 _index) const
{
    if (m_AnimMeshScene)
    {
        const ITF_VECTOR<AnimFrameInfo> &frameInfo = m_AnimMeshScene->m_AnimInfo.getVecAnimFrameInfo();
        if (frameInfo.size() > _index)
        {
            const AnimFrameInfo &firstFrameInfo = frameInfo[_index];
            AnimTrack * track = m_subAnimSet.getSubAnim(firstFrameInfo.m_subAnimIdx)->getTrack();
            ITF_ASSERT(track);
            return track->m_rootPos.size() > 0;
        }
    }
    return bfalse;
}

ITF::AABB AnimLightComponent::getPatchAABB() const
{
    AABB aabb;
    fillAnimsAABBLocal(aabb);
    return aabb;
}

bbool AnimLightComponent::isBoneDataOk( i32 _boneIndex ) const
{
    if (isVisiblePrediction() || getDisableVisiblePrediction() || !getNumPlayingSubAnims())
        return btrue;

    AnimSkeleton *skeleton = m_subAnimSet.getSkeleton();
    if(_boneIndex < (i32)skeleton->m_BonesCurFrame.size())
        return (skeleton->m_BonesCurFrame[_boneIndex].m_PatchFlags & BONE_MINIMAL_COMPUTE) != 0;

    return bfalse;
}


Vec2d AnimLightComponent::getScale() const
{
    return m_actor->getScale() * m_scaleMultiplier * getTemplate()->getScale();
}

Vec3d AnimLightComponent::getOffsettedPos() const
{
     return m_actor->getPos() + getOffset().to3d();
}

f32 AnimLightComponent::getOffsettedAngle() const
{
    return m_actor->getAngle() + getTemplate()->getRotOffset().ToRadians();
}

bbool AnimLightComponent::setSubSkeleton( StringID _subSkeletonName )
{
    if (!_subSkeletonName.isValid())
        return bfalse;

    AnimSkeleton   * skl = m_subAnimSet.getSkeleton();
    if (!skl)
        return bfalse;

    i32 idx = skl->m_NameToSubSkeleton.find(_subSkeletonName);
    if (idx >= 0)    
        m_subSkeletonIdx = skl->m_NameToSubSkeleton.getAt(idx);

    return m_subSkeletonIdx >= 0;
}

// IAnimCommonInterface
void AnimLightComponent::IAnimsetAnim( const StringID& _id )
{
    setAnim(_id);
}

bbool AnimLightComponent::IAnimisAnimFinished() const
{
    return isAnimToPlayFinished();
}

bbool AnimLightComponent::IAnimisAnimLooped() const
{
    return getIsLooped();
}

u32 AnimLightComponent::IAnimgetNumAnimsPlaying() const
{
    return getNumAnimsPlaying();
}

void AnimLightComponent::IAnimdbgGetCurAnim( u32 _index, String8& _out ) const
{
    return dbgGetCurAnims(_index,_out);
}

i32 AnimLightComponent::getCurSubAnimIndex( StringID _friendlyName ) const
{
    for (u32 index = 0; index < m_currentFrameSubAnims.size(); index++)
    {
        if (m_currentFrameSubAnims[index].m_subAnimFrameInfo.getSubAnim()->getFriendlyName() == _friendlyName)
        {
            return index;
        }
    }

    return -1;
}

void AnimLightComponent::setPatchChange( const StringID & _source, const StringID & _dest )
{
    m_patchChange[_source] = _dest;
    m_patchChangeModified  = btrue;
}

void AnimLightComponent::removePatchChange( const StringID & _source)
{
    m_patchChange.erase(_source);
    m_patchChangeModified  = btrue;
}

void AnimLightComponent::clearPatchChange()
{
    m_patchChange.clear();
    m_patchChangeModified  = btrue;
}

void AnimLightComponent::updatePatchChange()
{
    if (!m_patchChangeModified)
        return;

    m_subAnimSet.setPatchChangeMap(m_patchChange);

#ifdef SUPPORT_LASTBML
    m_AnimMeshScene->m_pAnimPatches.m_forceReload = btrue;
#endif //SUPPORT_LASTBML
    
    m_patchChangeModified  = bfalse;

}

void AnimLightComponent::setFluidEmitterFilter( GFX_GridFluidFilter val )
{
     m_fluidEmitterFilter = val;

//      if (m_emitFluid)
//          GFX_ADAPTER->getGridFluidManager().batchFluidPrimitive(m_AnimMeshScene, getVisualAABB(), depth, m_fluidEmitterFilter);
// 
// 
//      GFX_ADAPTER->getGridFluidManager().batchFluidPrimitive(m_AnimMeshScene, getVisualAABB(), depth, m_fluidEmitterFilter);
// 
//      if (m_emitFluid)
//          GFX_ADAPTER->getGridFluidManager().batchFluidPrimitive(m_AnimMeshSceneAllPass[_renderPass], getVisualAABB(), _depth, m_fluidEmitterFilter);
}


///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(AnimLightComponent_Template)
BEGIN_SERIALIZATION_CHILD(AnimLightComponent_Template)

    SERIALIZE_MEMBER("useBase",m_useBase); 
    SERIALIZE_MEMBER("smoothAnim",m_smoothAnim);
    SERIALIZE_MEMBER("canonizeTransitions",m_canonizeTransitions); 
    SERIALIZE_MEMBER("defaultBlendFrames",m_defaultBlendFrames); 
    SERIALIZE_MEMBER("draw2D",m_draw2D); 
    SERIALIZE_MEMBER("visualAABBanimID",m_visualAABBanimID); 
    BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
        SERIALIZE_OBJECT("animSet",m_subAnimSet);
    END_CONDITION_BLOCK()
    SERIALIZE_MEMBER("defaultAnimation",m_defaultAnimation);
    SERIALIZE_MEMBER("subSkeleton",m_subSkeleton);
    SERIALIZE_MEMBER("defaultColor",m_defaultColor); 
    SERIALIZE_MEMBER("flip",m_startFlip); 
    SERIALIZE_MEMBER("backZOffset",m_backZOffset); 
    SERIALIZE_MEMBER("frontZOffset",m_frontZOffset); 
    SERIALIZE_MEMBER("zOrderExtract",m_zOrderExtract); 
    SERIALIZE_MEMBER("zAmplify",m_zAmplify); 
    SERIALIZE_MEMBER("restoreStateAtCheckpoint",m_restoreStateAtCheckpoint);
    SERIALIZE_MEMBER("disableVisiblePrediction", m_disableVisiblePrediction );
    SERIALIZE_MEMBER("notifyAnimUpdate", m_notifyAnimUpdate );
    SERIALIZE_MEMBER("scale", m_scale );
    SERIALIZE_MEMBER("useZOffset", m_useZOffset);
    SERIALIZE_BOOL("allowRenderInTexture", m_allowRenderInTexture);
    SERIALIZE_MEMBER("renderInTectureSizeX", m_renderInTectureSizeX);
    SERIALIZE_MEMBER("renderInTectureSizeY", m_renderInTectureSizeY);

    SERIALIZE_CONTAINER_OBJECT("boneList",m_boneList);

END_SERIALIZATION()

AnimLightComponent_Template::AnimLightComponent_Template()
: m_useBase(bfalse)
, m_smoothAnim(bfalse)
, m_canonizeTransitions(btrue)
, m_defaultBlendFrames(0)
, m_draw2D(bfalse)
, m_defaultColor(Color::white())
, m_startFlip(bfalse)
, m_backZOffset(0.f)
, m_frontZOffset(0.f)
, m_zOrderExtract(0.f)
, m_zAmplify(0.f)
, m_isSynchronous(bfalse)
, m_restoreStateAtCheckpoint(bfalse)
, m_disableVisiblePrediction( bfalse )
, m_notifyAnimUpdate(bfalse)
, m_scale(Vec2d::One)
, m_useZOffset(btrue)
, m_allowRenderInTexture(bfalse)
, m_renderInTectureSizeX(2048)
, m_renderInTectureSizeY(2048)
{
}

bbool AnimLightComponent_Template::onTemplateLoaded( bbool _hotReload )
{
    bbool bOk = Super::onTemplateLoaded(_hotReload);    
    m_subAnimSet.postSerialize(m_actorTemplate->getResourceContainer());
    m_isSynchronous = bfalse;
    return bOk;
}

void AnimLightComponent_Template::onTemplateDelete( bbool _hotReload )
{
    Super::onTemplateDelete(_hotReload);

    m_subAnimSet.onTemplateDelete();
}

} //namespace ITF


