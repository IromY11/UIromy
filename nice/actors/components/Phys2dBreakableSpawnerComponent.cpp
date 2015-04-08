#include "precompiled_engine.h"
#ifdef USE_BOX2D

#ifndef _ITF_PHYS2DBREAKABLESPAWNERCOMPONENT_H_
#include "engine/actors/components/Phys2dBreakableSpawnerComponent.h"
#endif //_ITF_PHYS2DBREAKABLESPAWNERCOMPONENT_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_RENDERSIMPLEANIMCOMPONENT_H_
#include "gameplay/Components/Display/RenderSimpleAnimComponent.h"
#endif //_ITF_RENDERSIMPLEANIMCOMPONENT_H_

#ifndef _ITF_PHYS2D_H_
#include "engine/Phys2d/Phys2d.h"
#endif //_ITF_PHYS2D_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Phys2dBreakableSpawnerComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Phys2dBreakableSpawnerComponent_Template)
    END_SERIALIZATION()

    Phys2dBreakableSpawnerComponent_Template::Phys2dBreakableSpawnerComponent_Template()
    : Super()
    {
    }

    Phys2dBreakableSpawnerComponent_Template::~Phys2dBreakableSpawnerComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Phys2dBreakableSpawnerComponent)

    BEGIN_SERIALIZATION_CHILD(Phys2dBreakableSpawnerComponent)
        SERIALIZE_MEMBER("startAnim", m_startAnim);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Phys2dBreakableSpawnerComponent)
        VALIDATE_COMPONENT_PARAM("Component's Dependence", m_animComponent, "AngleAnimatedComponent requires an AnimLightComponent");
        VALIDATE_COMPONENT_PARAM("Component's Dependence", m_phys2dComponent, "AngleAnimatedComponent requires a Phys2dComponent");
    END_VALIDATE_COMPONENT()

    Phys2dBreakableSpawnerComponent::Phys2dBreakableSpawnerComponent()
    : Super()
    , m_state(eFull)
    , m_startAnim(U32_INVALID)
    {
    }


    Phys2dBreakableSpawnerComponent::~Phys2dBreakableSpawnerComponent()
    {
    }

    void Phys2dBreakableSpawnerComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        if (m_animComponent && m_startAnim.isValid())
        {
            m_animComponent->setAnim(m_startAnim);
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,AnimGameplayEvent_CRC, this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,AnimPolylineEvent_CRC, this);
    }

    void Phys2dBreakableSpawnerComponent::createPiece( StringID _polyline )
    {
        if(AnimPolyline* polyline = m_animComponent->getCurrentPolyline(_polyline))
        {
            sPieceSpawner piece;
            piece.m_boneIndex = polyline->m_PolylinePointList[0].m_BonePtr->getIndex();
            piece.m_polylineID = _polyline;

            Actor newPiece;
            BinaryClone(m_actor, &newPiece);
            
            if(Phys2dComponent* newPiecePhys2d = newPiece.GetComponent<Phys2dComponent>())
            {
                newPiecePhys2d->polylinesClear();
                newPiecePhys2d->polylineAdd(_polyline);
            }

            if(PolylineComponent* newPiecePolyline = newPiece.GetComponent<PolylineComponent>())
            {
//                 newPiecePolyline->polylinesClear();
//                 newPiecePolyline->polylineAdd(_polyline);
            }

            if(PhantomComponent* newPiecePhantom = newPiece.GetComponent<PhantomComponent>())
            {
                newPiecePhantom->setPolyline(_polyline);
            }

            if(Phys2dBreakableSpawnerComponent* newPieceBreakable = newPiece.GetComponent<Phys2dBreakableSpawnerComponent>())
            {
                newPieceBreakable->m_startAnim = _polyline;
            }

            m_phys2dComponent->polylinesRemove(_polyline, btrue);

            CSerializerObjectBinary serializeBin;
            ArchiveMemory actArchive;
            serializeBin.Init(&actArchive);
            newPiece.Serialize(&serializeBin, ESerialize_Data_Save);
            actArchive.rewindForReading();

            SpawnActorInfo info;
            Vec2d position;
            m_animComponent->getBonePos(piece.m_boneIndex, position);
            info.setStartPos(Vec3d(position.x(), position.y(), m_actor->getDepth()));
            info.setStartFlip(m_actor->getIsFlipped());
            Scene * pScene = m_actor->getScene();
            info.setSceneRef(pScene ? pScene->getRef() : ITF_INVALID_OBJREF);
            info.setDoInit(btrue);
            info.setArchive(actArchive);
            info.setParentResourceContainer(m_actor->getResourceContainer());

            String8 friendly = m_actor->getUserFriendly();
            Pickable::computeUniqueName(friendly, m_actor->getScene(), friendly);
            info.setFriendlyName(friendly);

            Actor* spawnedPiece = WORLD_MANAGER->spawnActor(info);
            piece.m_actor = spawnedPiece->getRef();

            //computeBody(&piece, Phys2d::Vec2dToB2Vec2(Vec2d::Zero), Vec2d::Zero, 0.0f);//TODO: calculate the center of the parent and his velocities

            m_pieces.push_back(piece);
        }
    }

    void Phys2dBreakableSpawnerComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);
        if (AnimGameplayEvent* animEvent = DYNAMIC_CAST(_event, AnimGameplayEvent))
        {
            if(animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Break,2513276570))
            {
                m_state = eBroke;
            }
        }
        else if (AnimPolylineEvent* animEvent = DYNAMIC_CAST(_event, AnimPolylineEvent))
        {
            m_state = eDamaged;
            if(animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Piece_Add,2736105987))
            {
                //AddAnim
                m_animComponent->addSubAnimToPlay(animEvent->getPolylineID(), 1.0f, btrue);
            }
            else if (animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Piece_Deattach,1664944732))
            {
                createPiece(animEvent->getPolylineID());
                m_animComponent->removeSubAnimToPlay(animEvent->getPolylineID());
            }
        }
    }


#ifdef ITF_SUPPORT_EDITOR
    void Phys2dBreakableSpawnerComponent::drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
//         if (!(_flags & DrawEditFlag_Collisions))
//             return;
// 
        for (ITF_VECTOR<sPieceSpawner>::const_iterator it = m_pieces.begin(); it != m_pieces.end(); it++)
        {
            if(Actor* child = AIUtils::getActor(it->m_actor))
            {
                DebugDraw::arrow2D(child->get2DPos(), m_actor->get2DPos(), m_actor->getDepth(), Color::cyan(), 1.0f,10.0f,7.0f,0.5f);
//                 for (b2Fixture* fixturePtr = it->m_body->GetFixtureList(); fixturePtr; fixturePtr = fixturePtr->GetNext())
//                 {
//                     b2Shape* shape = fixturePtr->GetShape();
//                     DebugDraw::shapeB2(Phys2d::b2Vec2ToVec2d(it->m_body->GetPosition()), m_actor->getDepth(), it->m_body->GetAngle(), shape, Color::yellow());
//                 }
            }
        }
    }
#endif

	void Phys2dBreakableSpawnerComponent::breakObject()
	{
//        m_animComponent->setAnim(getTemplate()->getDefaultAnimBreak());
        if (PolylineComponent* polyline = m_actor->GetComponent<PolylineComponent>())
        {
            polyline->setDisabled(btrue);
        }
	}
}

#endif
