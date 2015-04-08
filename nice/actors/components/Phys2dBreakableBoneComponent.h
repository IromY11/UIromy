#ifndef _ITF_PHYS2DBREAKABLEBONECOMPONENT_H_
#define _ITF_PHYS2DBREAKABLEBONECOMPONENT_H_


#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/ActorComponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifdef USE_BOX2D

#include "Box2d/Box2d.h"

#ifndef _ITF_PHYS2DBREAKABLECOMPONENT_H_
#include "engine/actors/components/Phys2dBreakableComponent.h"
#endif //_ITF_PHYS2DBREAKABLECOMPONENT_H_

namespace ITF
{
    class AnimLightComponent;
    class RenderSimpleAnimComponent;

#define MRK_BREAK       ITF_GET_STRINGID_CRC(Break,3643266711)
#define MRK_REBUILD     ITF_GET_STRINGID_CRC(Rebuild,3778667020)

#define Phys2dBreakableBoneComponent_CRC ITF_GET_STRINGID_CRC(Phys2dBreakableBoneComponent,3043432787)
#define Phys2dBreakableBoneComponent_Template_CRC ITF_GET_STRINGID_CRC(Phys2dBreakableBoneComponent_Template,1521378362)

    class Phys2dBreakableBoneComponent : public Phys2dBreakableComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Phys2dBreakableBoneComponent, Phys2dBreakableComponent, Phys2dBreakableBoneComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        struct sPosAndAngle
        {
            Vec2d       m_pos;
            f32         m_angle;
        };
        
        struct sPiece
        {
            u32                 m_boneID;
            b2Body*             m_body;
            bbool               m_isAwake;
            f32                 m_radius;
            sPosAndAngle        m_previousInfo;
            sPosAndAngle        m_currentInfo;
        };

    public:

        Phys2dBreakableBoneComponent();
        virtual ~Phys2dBreakableBoneComponent();
        
        virtual void onStartDestroy(bbool _hotReload);
        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onResourceReady();


        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        
        virtual void        Update(f32 _dt);
        virtual void        onEvent( Event * _event);
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags )  const;
#endif
        
		virtual void		breakObject(Vec2d &_axis);
        virtual void		hitObject(Vec2d &_axis);


        ITF_INLINE bbool isBroken() {return m_broke;}
        ITF_INLINE f32 getResistance() {return m_resitance;}

    private:
        ITF_INLINE const class Phys2dBreakableBoneComponent_Template* getTemplate() const;
        void                clearBodies();
        void                updateBodies();
        void                computeBodies(f32 _dt);
        void                createPieces();
        void                updateBones();
        void                rebuild();
        void                saveBonesPosAndAngle();

        void                createFixtureList( b2Body* body, b2FixtureDef* _fixtureDef );
        bbool               isOnSegment(const Vec2d & p, const Vec2d & p1, const Vec2d & p2);

        AnimLightComponent*         m_animComponent;
        RenderSimpleAnimComponent*  m_renderComponent;
        Phys2dComponent*            m_phys2dComponent;

        b2World*            m_world;
        ITF_VECTOR<sPiece>  m_pieces;
        
        bbool               m_break;
        bbool               m_broke;

        b2Vec2              m_initVelocity;
        f32                 m_initAngularVelocity;
        f32                 m_resitance;

        f32                 m_currBehaveDelay;
    };


    //---------------------------------------------------------------------------------------------------
    #define sPieceTemplate_CRC ITF_GET_STRINGID_CRC(sPieceTemplate,3033304368)
    struct sPieceTemplate : IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI(sPieceTemplate,IRTTIObject, sPieceTemplate_CRC);
        DECLARE_SERIALIZE()
         
        sPieceTemplate() : m_bodyType(b2_dynamicBody){}

        StringID    m_boneName;
        StringID    m_polyName;
        b2BodyType  m_bodyType;

    };

    class Phys2dBreakableBoneComponent_Template : public Phys2dBreakableComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Phys2dBreakableBoneComponent_Template, Phys2dBreakableComponent_Template, Phys2dBreakableBoneComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(Phys2dBreakableBoneComponent)
        DECLARE_SERIALIZE()

    public:

        Phys2dBreakableBoneComponent_Template();
        virtual ~Phys2dBreakableBoneComponent_Template();

        StringID            getBoneName(u32 _index) const;
        StringID            getPolyName(u32 _index) const;
        b2BodyType          getBodyType(u32 _index) const;
        ITF_INLINE u32      getPiecesCount() const{return m_pieces.size();}
        ITF_INLINE bbool    hasBackPieces() const { return m_hasBackPieces; }

    private:
        ITF_VECTOR<sPieceTemplate>  m_pieces;
        bbool                       m_hasBackPieces;
        
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Phys2dBreakableBoneComponent_Template* Phys2dBreakableBoneComponent::getTemplate() const
    {
        return static_cast<const Phys2dBreakableBoneComponent_Template*>(m_template);
    }
}

#endif // USE_BOX2D

#endif // _ITF_PHYS2DCOMPONENT_H_


