#include "precompiled_engine.h"

#ifndef _ITF_ANIMMESHVERTEXCOMPONENT_H_
#include "engine/actors/components/AnimMeshVertexComponent.h"
#endif //_ITF_ANIMMESHVERTEXCOMPONENT_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RESOURCE_CONTAINER_H_
#include "engine/resources/ResourceContainer.h"
#endif //_ITF_RESOURCE_CONTAINER_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#include "core/container/algorithm.h"

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AnimMeshVertexComponent_Template)

    BEGIN_SERIALIZATION_CHILD(AnimMeshVertexComponent_Template)
        SERIALIZE_MEMBER("defaultUpdate",m_defaultUpdate);     
        SERIALIZE_MEMBER("draw2D",m_draw2D); 
        SERIALIZE_MEMBER("amvPath", m_amvPath)
        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
            SERIALIZE_MEMBER("texture", m_material.getTexturePathSet().getTexturePath());
        END_CONDITION_BLOCK();
        SERIALIZE_OBJECT("material", m_material)
        SERIALIZE_MEMBER("useEditor", m_useEditor);
        SERIALIZE_MEMBER("useDataAnims", m_useDataAnims);
        SERIALIZE_MEMBER("useActorScale", m_useActorScale);        
		SERIALIZE_MEMBER("stopDuration", m_stopDuration);
    END_SERIALIZATION()

    AnimMeshVertexComponent_Template::AnimMeshVertexComponent_Template()
    : Super()
    , m_defaultUpdate(bfalse)
    , m_draw2D(bfalse)
    , m_useEditor(btrue)
    , m_useDataAnims(btrue)
    , m_useActorScale(btrue)
	, m_stopDuration(0.0f)
    {
    }

    AnimMeshVertexComponent_Template::~AnimMeshVertexComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(AnimMeshVertexComponent)

    BEGIN_SERIALIZATION_CHILD(AnimMeshVertexComponent)
        SERIALIZE_CONTAINER_OBJECT("anims", m_animList)
        SERIALIZE_MEMBER("mergeRange", m_mergeRange)
        BEGIN_CONDITION_BLOCK_NOT(ESerialize_Instance)
        SERIALIZE_OBJECT("aabb", m_localAABB)
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(AnimMeshVertexComponent)
        VALIDATE_COMPONENT_PARAM("", animsOk(), "Referencing invalid animation !");
    END_VALIDATE_COMPONENT()

    AnimMeshVertexComponent::AnimMeshVertexComponent()
    : Super()
    , m_uid(U32_INVALID)
    , m_lastFrame(0.0f)
    , m_useActorTransform(btrue)
    , m_useEditor(btrue)
    , m_autoComputeAABB(bfalse)
    , m_mergeRange(10.f)
	, m_forcePartition(bfalse)
    , m_drawEnabled(btrue)
	, m_playRate(1.0f)
	, m_stopTime(0.0f)
	, m_stopWanted(bfalse)
#if defined(ITF_SUPPORT_EDITOR)
    , m_underMouseIndex(U32_INVALID)
#endif
    {
        m_localAABB.invalidate();
    }

    AnimMeshVertexComponent::~AnimMeshVertexComponent()
    {
    }


    void AnimMeshVertexComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);
		ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventTrigger_CRC,this);

        if (m_localAABB.isValid())
            m_actor->growAABB(getAABB());
    }

    AABB AnimMeshVertexComponent::getAABB() const
    {
        AABB animAABB(m_localAABB);
        animAABB.Scale(m_actor->getScale());
        if (m_actor->getIsFlipped())
            animAABB.FlipHorizontaly(bfalse);
        animAABB.Rotate(m_actor->getAngle(), bfalse);
        animAABB.Translate(m_actor->get2DPos());

        return animAABB;
    }

    void AnimMeshVertexComponent::addFramesToAllAMV(u32 _nbFrames)
    {
        AnimMeshVertex * amnMeshVertex = getTemplate()->getAnimMeshVertex();
        u32 maxAnims = m_animListRuntime.size();
        for (u32 i=0; i<maxAnims; i++)
        {
            SingleAnimDataRuntime & aData = m_animListRuntime[i];
            if (aData.getAnim() < amnMeshVertex->getNbAnims())
            aData.setFrame((aData.getFrame() + _nbFrames) % amnMeshVertex->getNbFrameForAnim(aData.getAnim()));
        }
    }


    Transform3d AnimMeshVertexComponent::getActorTransform()
    {
        return m_useActorTransform  ? getWorldTransform(m_actor, btrue, getTemplate()->getUseActorScale()) : Transform3d::Identity();
    }

    void AnimMeshVertexComponent::updateAABB(bbool _fillLocal)
    {
        if (!_fillLocal && m_localAABB.isValid())
        {
            GetActor()->growAABB(getAABB());
            return;
        }
        
        u32 maxAnims = m_animListRuntime.size();
        if (maxAnims == 0)
            return;

        AABB aabbLoc, aabbGlob;
        AnimMeshVertex * amnMeshVertex = getTemplate()->getAnimMeshVertex();
        
        const Transform3d actTransform = (m_useActorTransform && !_fillLocal) ? getWorldTransform(m_actor, btrue, getTemplate()->getUseActorScale()) : Transform3d::Identity();
		bbool init = bfalse;
        for (u32 i=0; i<maxAnims; i++)
        {
            SingleAnimDataRuntime & aData = m_animListRuntime[i];
            const Transform3d & transform = actTransform * aData.getTransform3d();

            aabbLoc = amnMeshVertex->getAnimAABB(aData.getAnim());

			if(aabbLoc.isValid())
			{
				AABB temp = transform.transformPos(aabbLoc.getMinXMaxY()).truncateTo2D();
				if(init == bfalse)
				{
					aabbGlob = temp;
					init = btrue;
				}
				else
					aabbGlob.grow(temp);

				aabbGlob.grow(transform.transformPos(aabbLoc.getMaxXMinY()).truncateTo2D());
				aabbGlob.grow(transform.transformPos(aabbLoc.getMin()).truncateTo2D());
				aabbGlob.grow(transform.transformPos(aabbLoc.getMax()).truncateTo2D());
			}
        }

        if (_fillLocal)
        {
            m_localAABB = aabbGlob;
        }
        else
        {
            GetActor()->growAABB(aabbGlob);
        }
    }


    void AnimMeshVertexComponent::onEvent( Event* _event)
    {
        Super::onEvent(_event);

        if(EventTrigger * eventTrigger = DYNAMIC_CAST(_event, EventTrigger))
		{
			if(eventTrigger->getActivated())
			{
				m_stopWanted = bfalse;
				m_stopTime = 0.0f;
				m_lastFrame = (f32)CURRENTFRAME;
			}
			else
			{
				m_stopWanted = btrue;
				m_stopTime = getTemplate()->getStopDuration();
			}
    }
    }

	void AnimMeshVertexComponent::computePlayRate(const f32 _dt)
	{
		if(getTemplate()->getStopDuration() > 0.0f)
		{
			if(m_stopWanted)
			{
				m_stopTime = f32_Clamp(m_stopTime - _dt, 0.0f, getTemplate()->getStopDuration());
			}
			else
			{
				m_stopTime = f32_Clamp(m_stopTime + _dt, 0.0f, getTemplate()->getStopDuration());
			}

			m_playRate = m_stopTime / getTemplate()->getStopDuration();
		}
	}

    void AnimMeshVertexComponent::Update( const f32 _dt )
    {
        PRF_M_SCOPE(updateAnimMeshVertexComponent)

        Super::Update(_dt);

        if (getTemplate()->useDefaultUpdate())
        {
			computePlayRate(_dt);

			f32 currentFrame = (f32)CURRENTFRAME;

            f32 delta = (currentFrame - m_lastFrame) * m_playRate;

			if(delta >= 1.0f)
			{
				u32 intDelta = (u32)delta;
				addFramesToAllAMV(intDelta);

				m_lastFrame += delta;
			}
        }

        if (useEditor() || getAutoComputeAABB())
        updateAABB();
    }

    void AnimMeshVertexComponent::fillAMVBaseData(const ITF_VECTOR <View*>& _views)
    {
        m_amvBaseData = AMVRenderData(m_uid,
                                    GetActor()->getDepth() + getDepthOffset(),
                                    AMVRenderData::computeMaskFromViews(_views), 
                                    getGfxPrimitiveParam(),
                                    GetActor()->getRef());

        m_amvBaseData.setAlpha(m_alpha * m_amvBaseData.getAlpha());

        m_amvBaseData.growAABB(GetActor()->getAABB());
    }

    void AnimMeshVertexComponent::batchPrimitives( const ITF_VECTOR <View*>& _views )
    {
        Super::batchPrimitives(_views);

        if (!m_drawEnabled)
            return;

        AnimMeshVertex *    amnMeshVertex   = getTemplate()->getAnimMeshVertex();
        fillAMVBaseData(_views);

        
        if (!m_amvPartitionList.size())
        {
            if (m_useActorTransform)
            {
                const Transform3d tansform = getWorldTransform(m_actor, btrue, getTemplate()->getUseActorScale());
                ANIM_MANAGER->computeAnimMeshVertex(*amnMeshVertex, m_animListRuntime, m_amvBaseData, tansform);
            } else
            {
                ANIM_MANAGER->computeAnimMeshVertex(*amnMeshVertex, m_animListRuntime, m_amvBaseData, Transform3d::Identity());
            }
        } else
        {
            f32 baseZ = m_amvBaseData.getZ();
            for (ITF_VECTOR<AMVPartition>::iterator partIter = m_amvPartitionList.begin();
                partIter != m_amvPartitionList.end(); ++partIter)
            {
                m_amvBaseData.setZ(baseZ + partIter->m_z);
                if (m_useActorTransform)
                {
                    const Transform3d tansform = getWorldTransform(m_actor, btrue, getTemplate()->getUseActorScale());
                    ANIM_MANAGER->computeAnimMeshVertex(*amnMeshVertex, m_animListRuntime.begin() + partIter->m_begin, 
                                                                        m_animListRuntime.begin() + (partIter->m_begin + partIter->m_size),
                                                                        m_amvBaseData, tansform);
                } else
                {
                    ANIM_MANAGER->computeAnimMeshVertex(*amnMeshVertex, m_animListRuntime.begin() + partIter->m_begin, 
                                                                        m_animListRuntime.begin() + (partIter->m_begin + partIter->m_size), 
                                                                        m_amvBaseData, Transform3d::Identity());
                }
            }

        }
    }

    void AnimMeshVertexComponent::batchPrimitives2D( const ITF_VECTOR <View*>& _views )
    {
        Super::batchPrimitives2D(_views);

        if (!m_drawEnabled)
            return;

        AnimMeshVertex *    amnMeshVertex   = getTemplate()->getAnimMeshVertex();
        fillAMVBaseData(_views);

        if (m_useActorTransform)
        {
            Transform3d tansform = getWorld2DTransform(m_actor, btrue, getTemplate()->getUseActorScale());
            tansform.setZ(0.0f);
            ANIM_MANAGER->computeAnimMeshVertex(*amnMeshVertex, m_animListRuntime, m_amvBaseData, tansform, btrue);
        } else
        {
            ANIM_MANAGER->computeAnimMeshVertex(*amnMeshVertex, m_animListRuntime, m_amvBaseData, Transform3d::Identity(), btrue);
        }
    }

    void AnimMeshVertexComponent::onResourceLoaded()
    {
        Super::onResourceLoaded();

        m_uid = ANIM_MANAGER->getAnimMeshVertexIdFromMaterial(getTemplate()->getMaterial());
        m_lastFrame = ((f32) CURRENTFRAME) * m_playRate;
        computeRuntimeData(bfalse, btrue);
        
        ITF_VECTOR <View*> emptyView;
        fillAMVBaseData(emptyView);
        if (useEditor())
        updateAABB(btrue);
        else
            m_localAABB.invalidate();
    }

    void AnimMeshVertexComponent::onUnloadResources()
    {
        Super::onUnloadResources();

        ANIM_MANAGER->releaseAnimMeshVertexId(m_uid);
        m_uid = U32_INVALID;

#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
        for (VectorAnim<SingleAnimData>::iterator animIter = m_animList.begin();
            animIter != m_animList.end(); ++animIter)
        {
            animIter->m_amv             = NULL;
        }
#endif
    }

    AnimMeshVertex * AnimMeshVertexComponent::getAnimMeshVertex() const
    {
        return getTemplate()->getAnimMeshVertex();
    }

    bool sortZ(const SingleAnimDataRuntime & a, const SingleAnimDataRuntime & b) 
    {
        return a.getTransform3d().m_z < b.getTransform3d().m_z; 
    }

    void AnimMeshVertexComponent::computeRuntimeData(bbool _fullCompute, bbool _updateFrames)
    {
        AnimMeshVertex * amv = getTemplate()->getAnimMeshVertex();
        if (m_animList.size() == 0 || !amv)
        {
            return;
        }

#if defined(ITF_SUPPORT_EDITOR)
        m_underMouseIndex = U32_INVALID;
#endif

        if (!_fullCompute && m_animListRuntime.size())
        {
#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
            for (VectorAnim<SingleAnimData>::iterator animIter = m_animList.begin();
                animIter != m_animList.end(); ++animIter)
            {
                animIter->m_amv             = amv;
            }
#endif
            return;
        }

        m_animListRuntime.resize(m_animList.size());
        if (_updateFrames)
            m_lastFrame = ((f32) CURRENTFRAME) * m_playRate;
        SingleAnimDataRuntimeList::iterator animRuntimeIter = m_animListRuntime.begin();
        for (SingleAnimDataList::iterator animIter = m_animList.begin();
            animIter != m_animList.end(); ++animIter, ++animRuntimeIter)
        {
#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
            animIter->m_amv             = amv;
            if (!animIter->m_animName.isValid())
            {
                if (animIter->m_anim < amv->m_animNames.size())
                    animIter->m_animName = StringID(amv->getAnimName(animIter->m_anim));
                else
                    animIter->m_animName = StringID(amv->getAnimName(0));
            }
#endif

            animRuntimeIter->setAnimByName(amv, animIter->m_animName);
            animRuntimeIter->setColor(animIter->m_color);
            animRuntimeIter->setTransform3dFrom(animIter->m_pos, animIter->m_angle.ToRadians(), animIter->m_scale, animIter->m_flip);
            if (_updateFrames && animRuntimeIter->getAnim() < amv->getNbAnims())
                animRuntimeIter->setFrame((((u32) m_lastFrame) + animIter->m_frameOffset) % amv->getNbFrameForAnim(animRuntimeIter->getAnim()));
        }
        std::sort(m_animListRuntime.begin(), m_animListRuntime.end(), sortZ);
		
		computePartition(bfalse);

        if (_fullCompute)
        {
            updateAABB(btrue); // compute aabb
            updateAABB(bfalse); // apply
        }
    }
	
    void AnimMeshVertexComponent::computePartition(bbool _deactivateEditorMode)
	{
		if (_deactivateEditorMode)
            m_useEditor = bfalse;
        
        m_amvPartitionList.clear();
        if ((!useEditor() && !m_forcePartition) || m_mergeRange > (10.f - MTH_EPSILON))
			return;

		AMVPartition * currentPartition = NULL;
		for (SingleAnimDataRuntimeList::iterator amvIter = m_animListRuntime.begin();
			amvIter != m_animListRuntime.end(); ++amvIter)
		{
			if (!currentPartition || amvIter->getTransform3d().m_z - currentPartition->m_z > m_mergeRange)
			{
				m_amvPartitionList.emplace_back();
				currentPartition = &m_amvPartitionList.back();

                currentPartition->m_begin = uSize(amvIter - m_animListRuntime.begin());
				currentPartition->m_size  = 1;
				currentPartition->m_z     = amvIter->getTransform3d().m_z;
			} else
				currentPartition->m_size++;
		}
	}


#if defined(ITF_SUPPORT_EDITOR)
    void AnimMeshVertexComponent::processUnderMouse(u32 _index)
    {
        if (m_underMouseIndex == _index)
            return;
        
        AnimMeshVertex * amv = getTemplate()->getAnimMeshVertex();
        if (!amv)
            return;
        

        m_underMouseIndex = _index;

        ITF_ASSERT_MSG(m_animListRuntime.size() == m_animList.size(), "Not already computed, strange !");

        m_animListRuntime.resize(m_animList.size());
        VectorAnim<SingleAnimDataRuntime>::iterator animRuntimeIter = m_animListRuntime.begin();
        VectorAnim<SingleAnimData>::iterator animIter = m_animList.begin();
        for (u32 idx=0; animIter != m_animList.end(); ++animIter, ++animRuntimeIter, ++idx)
        {
            Color color = animIter->m_color;
            if (m_underMouseIndex != U32_INVALID && m_underMouseIndex != idx)
                color = color*Color::grey();

            //ICI
            animRuntimeIter->setColor(color);
        }
        std::sort(m_animListRuntime.begin(), m_animListRuntime.end(), sortZ);
    }

    void AnimMeshVertexComponent::onEditorMove(bbool _modifyInitialPos)
    {
        computeRuntimeData(btrue, bfalse);
    }

#endif // ITF_SUPPORT_EDITOR

    bbool AnimMeshVertexComponent::animsOk()
    {
        if (!getTemplate()->getUseDataAnims())
            return btrue;

        AnimMeshVertex * amv = getTemplate()->getAnimMeshVertex();
        if (!amv)
            return bfalse;

        u32 nbAnimsInAnimMeshVertex = amv->m_frameIndexToMeshDataByAnim.size();
        for (SingleAnimDataList::iterator animIter = m_animList.begin();
            animIter != m_animList.end(); ++animIter)
        {
            if (amv->getAnimIndexByFriendly(animIter->m_animName) >= nbAnimsInAnimMeshVertex)
			{
				ITF_WARNING_CATEGORY(Anim, m_actor, bfalse, "[AnimMeshVertex] invalid anim name : %s", animIter->m_animName.getDebugString());
				return bfalse;
			}
        }

        return btrue;
    }

    void AnimMeshVertexComponent::setAMVAnim( u32 _idx, u32 _animIndex )
    {
        m_animListRuntime[_idx].setAnim(_animIndex);
    }

    void AnimMeshVertexComponent::setAMVAnimByName(u32 _idx, const StringID & _animName)
    {
        AnimMeshVertex * amv = getTemplate()->getAnimMeshVertex();

        if(_animName.isValid())
        {
            m_animListRuntime[_idx].setAnimByName(amv, _animName);
        }
        else
        {
            m_animListRuntime[_idx].setAnimByName(amv, m_animList[_idx].m_animName);
        }
    }


    bbool AnimMeshVertexComponent_Template::onTemplateLoaded( bbool _hotReload )
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);    

        ResourceContainer * resContainer = m_actorTemplate->getResourceContainer();
        m_amvResourceID     = resContainer->addResourceFromFile(Resource::ResourceType_AnimMeshVertex, m_amvPath);
        m_material.onLoaded(resContainer);

        return bOk;
    }

    void AnimMeshVertexComponent_Template::onTemplateDelete(bbool _hotReload)
    {
        ResourceContainer * resContainer = m_actorTemplate->getResourceContainer();
        m_material.onUnLoaded(resContainer);

        Super::onTemplateDelete(_hotReload);
    }


    AnimMeshVertex * AnimMeshVertexComponent_Template::getAnimMeshVertex() const
    {
        AnimMeshVertexResource* res = static_cast<AnimMeshVertexResource *>(m_amvResourceID.getResource());
        return res ? res->getOwnerData() : NULL;
    }

}
