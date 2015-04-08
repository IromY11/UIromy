#ifndef _ITF_PHYS2DBREAKABLECOMPONENT_H_
#define _ITF_PHYS2DBREAKABLECOMPONENT_H_

#define Phys2dBreakableComponent_CRC ITF_GET_STRINGID_CRC(Phys2dBreakableComponent,2387200703)
#define Phys2dBreakableComponent_Template_CRC ITF_GET_STRINGID_CRC(Phys2dBreakableComponent_Template,3716764065)

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/ActorComponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_PHYS2DCOMPONENT_H_
#include "engine/actors/components/Phys2dComponent.h"
#endif //_ITF_PHYS2DCOMPONENT_H_

#ifdef USE_BOX2D

#include "Box2d/Box2d.h"

namespace ITF
{
    class AnimLightComponent;

    enum eBreakableBehaveAfterBreak
    {
        eDoNothing = 0,
        eDestroyAfterBreak,
        eRebuildAfterBreak
    };

    class Phys2dBreakableComponent : public ActorComponent, public Phys2dComponentListener
    {
        DECLARE_OBJECT_CHILD_RTTI(Phys2dBreakableComponent, ActorComponent, Phys2dBreakableComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        struct sPiece
        {
            u32         m_boneID;
            b2Body*     m_body;
            bbool       m_isAwake;
            f32         m_radius;
        };

    public:

        Phys2dBreakableComponent();
        virtual ~Phys2dBreakableComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual bbool       needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void onStartDestroy(bbool _hotReload);
        virtual void        onResourceReady();


        virtual void        onEvent(Event * _event);
                
        virtual void        beginContact(Pickable* _pickableA, Pickable* _pickableB, b2Contact* contact);
        virtual void        postSolve(Pickable* _pickableA, Pickable* _pickableB, b2Contact* contact, const b2ContactImpulse* impulse);
        void                addFakeCollision( Vec2d _impulse, Vec2d _pos, u32 _filter = 0xFFFFFFFF);

		virtual void		setPercentualDamage(Vec2d _axis, f32 _damage, u32 _filter);
        virtual void		breakObject(Vec2d &_axis);
        virtual void		hitObject(Vec2d &_axis);
		virtual void		registerOnDestroyListener(Actor* _actor);


        virtual ITF_INLINE bbool    isBroken() {return bfalse;}
        virtual ITF_INLINE f32      getResistance() {return m_resitance;}
        virtual ITF_INLINE bbool    checkFilter(u32 _filter);

    protected:
        ITF_INLINE const class Phys2dBreakableComponent_Template* getTemplate() const;

        AnimLightComponent*         m_animComponent;
        Phys2dComponent*            m_phys2dComponent;

        f32                         m_resitance;
		ITF_VECTOR<Actor*>	        m_onDestroyListeners;
    };

    #define sBreakAnim_CRC ITF_GET_STRINGID_CRC(sBreakAnim,646233453)
    class sBreakAnim : IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI(sBreakAnim,IRTTIObject, sBreakAnim_CRC);
        DECLARE_SERIALIZE()

        sBreakAnim() : m_animBreak(StringID::Invalid), m_angle(Angle::Angle360) {}

        Angle       m_angle;
        StringID    m_animBreak;
    
    };

    class Phys2dBreakableComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Phys2dBreakableComponent_Template, ActorComponent_Template, Phys2dBreakableComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(Phys2dBreakableComponent)
        DECLARE_SERIALIZE()

    public:

//         struct sBreakAnim
//         {
//             Angle       m_angle;
//             StringID    m_animBreak;
//         };


        Phys2dBreakableComponent_Template();
        virtual ~Phys2dBreakableComponent_Template();

        const ITF_VECTOR<sBreakAnim>*               getDefaultAnimHitAngle() const { return &m_animHitAngle; }
        ITF_INLINE StringID                         getDefaultAnimHit() const { return m_animHit; }
        ITF_INLINE u32                              getAnimHitPriority() const { return m_animHitPriority; }

        const ITF_VECTOR<sBreakAnim>*               getDefaultAnimBreakAngle() const { return &m_animBreakAngle; }
        ITF_INLINE StringID                         getDefaultAnimBreak() const { return m_animBreak; }
        ITF_INLINE u32                              getAnimBreakPriority() const { return m_animBreakPriority; }

        ITF_INLINE StringID                         getDefaultAnimRebuild() const { return m_animRebuild; }

        ITF_INLINE f32                              getBreakingImpulse() const { return m_breakingImpulse; }
        ITF_INLINE f32                              getBehaveDelay() const { return m_behaveDelay; }
        ITF_INLINE u32                              getCollisionFilter() const { return m_collisionFilter; }
        ITF_INLINE eBreakableBehaveAfterBreak       getBehaveAfterBreak() const { return m_behaveAfterBreak; }

    private:
        f32                         m_breakingImpulse;
        StringID                    m_animBreak;
        ITF_VECTOR<sBreakAnim>      m_animBreakAngle;
        u32                         m_animBreakPriority;

        StringID                    m_animHit;
        ITF_VECTOR<sBreakAnim>      m_animHitAngle;
        u32                         m_animHitPriority;

        StringID                    m_animRebuild;

        eBreakableBehaveAfterBreak  m_behaveAfterBreak;
        f32                         m_behaveDelay;

        u32                         m_collisionFilter;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Phys2dBreakableComponent_Template* Phys2dBreakableComponent::getTemplate() const
    {
        return static_cast<const Phys2dBreakableComponent_Template*>(m_template);
    }
}

#endif // USE_BOX2D

#endif // _ITF_PHYS2DBREAKABLECOMPONENT_H_


