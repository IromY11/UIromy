#include "precompiled_engine.h"

#ifndef _ITF_BASICIKCOMPONENT_H_
#include "engine/actors/components/BasicIKComponent.h"
#endif //_ITF_BASICIKCOMPONENT_H_

#ifndef _ITF_RENDERSIMPLEANIMCOMPONENT_H_
#include "gameplay/Components/Display/RenderSimpleAnimComponent.h"
#endif //_ITF_RENDERSIMPLEANIMCOMPONENT_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    ITF_INLINE f32 GetAngleBTWNormalized(const Vec2d& _v1, const Vec2d& _v2)
    {
        return - atan2(_v1.dot(Turn90(_v2)), _v1.dot(_v2));
    }

    IMPLEMENT_OBJECT_RTTI(BasicIKComponent_Template)
        
    BEGIN_SERIALIZATION_CHILD(BasicIKComponent_Template)
        SERIALIZE_CONTAINER("bones", m_bones);
        SERIALIZE_BOOL("defaultProcess", m_defaultProcess);
    END_SERIALIZATION()

    BasicIKComponent_Template::BasicIKComponent_Template()
        : Super()
        , m_defaultProcess(bfalse)
    {
    }

    BasicIKComponent_Template::~BasicIKComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(BasicIKComponent)

    BEGIN_SERIALIZATION_CHILD(BasicIKComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(BasicIKComponent)
        VALIDATE_COMPONENT_PARAM("AnimLightComponent", m_animComponent, "AnimLightComponent mandatory");
        VALIDATE_COMPONENT_PARAM("RenderSimpleAnimComponent", m_renderSimpleAnimComponent, "RenderSimpleAnimComponent mandatory");
        VALIDATE_COMPONENT_PARAM("", m_error.isEmpty(), m_error.cStr());
    END_VALIDATE_COMPONENT()

    BasicIKComponent::BasicIKComponent()
        : Super()
        , m_animComponent(NULL)
        , m_renderSimpleAnimComponent(NULL)
    {
    }

    BasicIKComponent::~BasicIKComponent()
    {
    }


    void BasicIKComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = GetActor()->GetComponent<AnimLightComponent>();
        m_renderSimpleAnimComponent = GetActor()->GetComponent<RenderSimpleAnimComponent>();
        m_targets.resize(getTemplate()->getBones().size());
    }

    void BasicIKComponent::onResourceReady()
    {
        if (!m_animComponent)
            return;

         ITF_VECTOR<StringID>::const_iterator boneIter   = getTemplate()->getBones().begin();
         ITF_VECTOR<TargetIK>::iterator       targerIter = m_targets.begin();
         for (;boneIter != getTemplate()->getBones().end(); ++boneIter, ++targerIter)
         {
             targerIter->m_boneIndex = m_animComponent->getBoneIndex(*boneIter);
             targerIter->m_process = getTemplate()->getDefaultProcess();
             if (targerIter->m_boneIndex < 0)
             {
                 m_error.setTextFormat("Animation has no bone %s", boneIter->getDebugString());
                 return;
             }
         }
    }



    void BasicIKComponent::computeIK(TargetIK & _target)
    {
        Vec3d pos3d = _target.m_position.to3d(GetActor()->getDepth());

        SafeArray<AnimBoneDyn> & dyn = m_animComponent->getAnimMeshScene()->m_AnimInfo.getWorkingBoneList();
        AnimSkeleton *           skl = m_animComponent->getSubAnimSet()->getSkeleton();

        Matrix44 matrix =  m_animComponent->getAnimMeshScene()->m_pMesh[0]->getMatrix();
        matrix.inverse(matrix);
        matrix.transformPoint(pos3d, pos3d);
        Vec2d lockPos   = pos3d.truncateTo2D();

        u32             rootIdx = skl->getRootIndex();
        AnimBoneDyn     root = dyn[rootIdx];
        root.ComputeCosSin();

        lockPos.y() =  1.f - lockPos.y();

        const AnimBone*       B     = &skl->m_BonesCurFrame[_target.m_boneIndex];
        AnimBoneDyn*          BDyn = &dyn[_target.m_boneIndex];

        if(!B->m_BoneParent)
        {
            const Vec2d dirToLockPosNormalized = (lockPos - BDyn->m_Pos).normalize();
            f32 diff = GetAngleBTWNormalized(Vec2d::XAxis, dirToLockPosNormalized) - BDyn->m_Angle;
            diff = canonizeAngle(diff);
            BDyn->m_Angle += diff;
#ifdef NO_COMPUTECOSSIN
            Vec2d::CosSinOpt(&BDyn->m_CosSin, BDyn->m_Angle);
#endif
            BDyn->m_PosLocal   = lockPos - BDyn->GetLocalEnd();
            BDyn->m_AngleLocal = BDyn->m_Angle;
        }
        else
        {
            AnimBone*       P = B->m_BoneParent;
            AnimBoneDyn*    PDyn = &dyn[P->getIndex()];
            ITF_ASSERT(P);
            f32 diff, diff0, diff1;
            Vec2d NewP0, NewP1;
            bbool pO1Defined = bfalse;

            Vec2d tmp0;
            Vec2d::Mul(&tmp0, &BDyn->m_PosLocal, &PDyn->m_Scale);

            const f32 r0 = tmp0.norm();
            const f32 r1 = BDyn->m_Scale.x();
            if (f32_Abs(PDyn->m_Pos.y() - lockPos.y()) >= f32_Abs(PDyn->m_Pos.x() - lockPos.x()))
            {
                pO1Defined = AnimSkeleton::resolveIntercection(NewP0, NewP1, 
                    PDyn->m_Pos.x(), PDyn->m_Pos.y(), r0,
                    lockPos.x(), lockPos.y(), r1);
            } else
            {
                pO1Defined = AnimSkeleton::resolveIntercection(NewP0, NewP1, 
                    PDyn->m_Pos.y(), PDyn->m_Pos.x(), r0,
                    lockPos.y(), lockPos.x(), r1);
                if (pO1Defined)
                {
                    Vec2d::SwapXY(&NewP0, &NewP0);
                    Vec2d::SwapXY(&NewP1, &NewP1);
                }
            }

            const Vec2d localBoneDirFromParentNormalized = tmp0.normalize();

            if(pO1Defined)
            {
                diff0 = GetAngleBTWNormalized(localBoneDirFromParentNormalized, (NewP0 - PDyn->m_Pos).normalize());
                diff1 = GetAngleBTWNormalized(localBoneDirFromParentNormalized, (NewP1 - PDyn->m_Pos).normalize());

                Vec2d newPos;
                f32 newPAngle, newAngle, newAngleLocal;
                if(_target.m_angleSign == 0)
                {
                    diff0 = canonizeAngle(diff0 - PDyn->m_Angle);
                    diff1 = canonizeAngle(diff1 - PDyn->m_Angle);

                    if(f32_Abs(diff0) < f32_Abs(diff1))
                    {
                        newPos = NewP0;
                        newPAngle = PDyn->m_Angle + diff0;
                    }
                    else
                    {
                        newPos = NewP1;
                        newPAngle = PDyn->m_Angle + diff1;
                    }
                }
                else
                {
                    newPAngle = PDyn->m_Angle + canonizeAngle(diff0 - PDyn->m_Angle);
                    newPos = NewP0;
                    newAngle = GetAngleBTWNormalized(Vec2d::XAxis, (lockPos - newPos).normalize()) - BDyn->m_Angle;
                    newAngle = canonizeAngle(newAngle);
                    newAngle += BDyn->m_Angle;
                    newAngleLocal = canonizeAngle(newAngle - newPAngle);

                    if ((newAngleLocal > 0.f && (_target.m_angleSign & 0x2)) ||
                        (newAngleLocal < 0.f && (_target.m_angleSign & 0x1)))
                    {
                        newPos        = NewP1;
                        newPAngle    = PDyn->m_Angle + canonizeAngle(diff1 - PDyn->m_Angle);
                    }
                }

                BDyn->m_Pos = newPos;
                PDyn->m_Angle = newPAngle;
#ifdef NO_COMPUTECOSSIN
                Vec2d::CosSinOpt(&PDyn->m_CosSin, PDyn->m_Angle);
#endif
                diff = GetAngleBTWNormalized(Vec2d::XAxis, (lockPos - newPos).normalize()) - BDyn->m_Angle;
                diff = canonizeAngle(diff);
                BDyn->m_Angle += diff;
#ifdef NO_COMPUTECOSSIN
                Vec2d::CosSinOpt(&BDyn->m_CosSin, BDyn->m_Angle);
#endif
            }
            else
            {
                // B and P are aligned : use vector

                const Vec2d dir = (PDyn->m_Pos - lockPos).normalize();
                BDyn->m_Pos = lockPos + dir * r1;
                const Vec2d dirToLockPosNormalized = (lockPos - BDyn->m_Pos).normalize();
                diff = GetAngleBTWNormalized(Vec2d::XAxis, dirToLockPosNormalized) - BDyn->m_Angle;
                diff = canonizeAngle(diff);
                BDyn->m_Angle += diff;

                PDyn->m_Pos = BDyn->m_Pos + dir * r0;
                diff = GetAngleBTWNormalized(localBoneDirFromParentNormalized, (BDyn->m_Pos - PDyn->m_Pos).normalize()) - PDyn->m_Angle;
                diff = canonizeAngle(diff);
                PDyn->m_Angle += diff;
#ifdef NO_COMPUTECOSSIN
                Vec2d::CosSinOpt(&PDyn->m_CosSin, PDyn->m_Angle);
#endif
            }

            if(P->m_BoneParent)
            {
                AnimBoneDyn*    PPDyn = &dyn[P->m_BoneParent->getIndex()];
                PDyn->m_AngleLocal = PDyn->m_Angle - PPDyn->m_Angle;
            }
            else
            {
                PDyn->m_AngleLocal = PDyn->m_Angle;
            }
            BDyn->m_AngleLocal = BDyn->m_Angle - PDyn->m_Angle;
        }
    }

    void BasicIKComponent::computeBoneAngle(BoneAngle & _boneAngle)
    {
        f32 angle = _boneAngle.m_angle - GetActor()->getAngle();
        SafeArray<AnimBoneDyn> & dyn = m_animComponent->getAnimMeshScene()->m_AnimInfo.getWorkingBoneList();

        AnimBoneDyn*          BDyn = &dyn[_boneAngle.m_boneIndex];
        BDyn->m_AngleLocal = angle - BDyn->m_Angle + BDyn->m_AngleLocal;
    }


    void BasicIKComponent::Update( const f32 _dt )
    {
        PRF_M_SCOPE(updateBasicIKComponent)

        Super::Update(_dt);

        m_animComponent->setIsBoneModification(btrue);

        bbool needComputeRender = bfalse;
        bbool swapY = btrue;
        for (ITF_VECTOR<TargetIK>::iterator targetIter = m_targets.begin();
            targetIter != m_targets.end(); ++targetIter)
        {
            if (!targetIter->m_process)
                continue;

            if (swapY)
            {
                SafeArray<AnimBoneDyn> & dyn = m_animComponent->getAnimMeshScene()->m_AnimInfo.getWorkingBoneList();
                // pos must be in genanim mode with y inversion !!!
                const u32 count = dyn.size();
                AnimBoneDyn * pBoneDyn = &dyn[0];
                for (u32 i=0; i<count; ++i, ++pBoneDyn)
					pBoneDyn->m_Pos.set( pBoneDyn->m_Pos.x(), 1.f - pBoneDyn->m_Pos.y() );

                swapY = bfalse;
            }
            
            computeIK(*targetIter);
            needComputeRender = btrue;
        }

        if (m_boneAngles.size())
        {
            // first do full process ...
            if (needComputeRender)
            {
                m_animComponent->getAnimMeshScene()->m_AnimInfo.m_globalData.clear();
                m_animComponent->getAnimMeshScene()->m_AnimInfo.ComputeBonesFromLocalToWorld(btrue);
                
                needComputeRender = bfalse;
            }

            for (ITF_VECTOR<BoneAngle>::iterator baIt = m_boneAngles.begin();
                baIt != m_boneAngles.end(); ++baIt)
            {
                computeBoneAngle(*baIt);
                
                needComputeRender = btrue;
            }
        }

        m_renderSimpleAnimComponent->setComputeLocalToGlobal(needComputeRender);
        m_animComponent->setIsBoneModificationLocal(needComputeRender);
    }

    void BasicIKComponent::onEvent( Event* _event)
    {
        Super::onEvent(_event);
    }

    i32 BasicIKComponent::getBoneIndex(const StringID & _boneName)
    {
        for (ITF_VECTOR<StringID>::const_iterator boneIter = getTemplate()->getBones().begin();
            boneIter != getTemplate()->getBones().end(); ++boneIter)
        {
            if (*boneIter == _boneName)
                return (i32) (boneIter - getTemplate()->getBones().begin());
        }
        return -1;
    }


    void BasicIKComponent::setBoneAngle(const StringID & _bone, f32 _angle)
    {
        for (ITF_VECTOR<BoneAngle>::iterator baIt = m_boneAngles.begin();
            baIt != m_boneAngles.end(); ++baIt)
        {
            if (baIt->m_name == _bone)
            {
                baIt->m_angle = _angle;
                return;
            }
        }

        i32 boneIndex = m_animComponent->getBoneIndex(_bone);
        if (boneIndex < 0)
            return;

        m_boneAngles.push_back(BoneAngle(_bone, boneIndex, _angle));
    }

}
