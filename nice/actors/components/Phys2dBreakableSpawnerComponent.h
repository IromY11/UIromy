#ifndef _ITF_PHYS2DBREAKABLESPAWNERCOMPONENT_H_
#define _ITF_PHYS2DBREAKABLESPAWNERCOMPONENT_H_

#define Phys2dBreakableSpawnerComponent_CRC ITF_GET_STRINGID_CRC(Phys2dBreakableSpawnerComponent,29712375)
#define Phys2dBreakableSpawnerComponent_Template_CRC ITF_GET_STRINGID_CRC(Phys2dBreakableSpawnerComponent_Template,1739367125)

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/ActorComponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifdef USE_BOX2D

#ifndef _ITF_PHYS2DBREAKABLECOMPONENT_H_
#include "engine/actors/components/Phys2dBreakableComponent.h"
#endif //_ITF_PHYS2DBREAKABLECOMPONENT_H_


#include "Box2d/Box2d.h"


namespace ITF
{
    class AnimLightComponent;

    class Phys2dBreakableSpawnerComponent : public Phys2dBreakableComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Phys2dBreakableSpawnerComponent, Phys2dBreakableComponent, Phys2dBreakableSpawnerComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        struct sPieceSpawner
        {
            u32         m_boneIndex;
            StringID    m_polylineID;
            ActorRef    m_actor;
            b2Body*     m_body;
            bbool       m_isAwake;
            f32         m_radius;
        };

        enum eBreakingState
        {
            eFull = 0,
            eDamaged,
            eBroke,
        };

    public:

        Phys2dBreakableSpawnerComponent();
        virtual ~Phys2dBreakableSpawnerComponent();
        
        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        
        virtual void        onEvent( Event * _event);
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags )  const;
#endif
        virtual void		breakObject();

        ITF_INLINE bbool    isBroken() {return m_state == eBroke;}

    protected:
        ITF_INLINE const class Phys2dBreakableSpawnerComponent_Template* getTemplate() const;

        void                createPiece( StringID _polyline );

        ITF_VECTOR<sPieceSpawner>  m_pieces;
        
        eBreakingState      m_state;
        StringID            m_startAnim;

    };


    class Phys2dBreakableSpawnerComponent_Template : public Phys2dBreakableComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Phys2dBreakableSpawnerComponent_Template, Phys2dBreakableComponent_Template, Phys2dBreakableSpawnerComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(Phys2dBreakableSpawnerComponent)
        DECLARE_SERIALIZE()

    public:

        Phys2dBreakableSpawnerComponent_Template();
        virtual ~Phys2dBreakableSpawnerComponent_Template();
        
    private:
        

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Phys2dBreakableSpawnerComponent_Template* Phys2dBreakableSpawnerComponent::getTemplate() const
    {
        return static_cast<const Phys2dBreakableSpawnerComponent_Template*>(m_template);
    }
}

#endif // USE_BOX2D

#endif // _ITF_PHYS2DBREAKABLESPANWERCOMPONENT_H_


