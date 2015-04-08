
#ifndef _ITF_PHYSCOMPONENT_H_
#define _ITF_PHYSCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_
#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

namespace ITF
{
    class EventAddForce;

#define PhysComponent_CRC ITF_GET_STRINGID_CRC(PhysComponent,1286338096) 
#define PhysComponent_Template_CRC ITF_GET_STRINGID_CRC(PhysComponent_Template,2661944379) 
class PhysComponent : public ActorComponent
{
	DECLARE_OBJECT_CHILD_RTTI(PhysComponent,ActorComponent, PhysComponent_CRC);

public:
	DECLARE_SERIALIZE()

	PhysComponent();
	virtual ~PhysComponent();

    virtual bbool               needsUpdate() const { return bfalse; }
    virtual bbool               needsDraw() const { return bfalse; }
    virtual bbool               needsDraw2D() const { return bfalse; }
	virtual	bbool				needsDraw2DNoScreenRatio() const { return bfalse; }
    virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                onCheckpointLoaded();
    virtual void                onEvent( Event * _event);

	virtual   void              setDisabled( bbool _val) { m_disabled = _val; }
	ITF_INLINE   bbool          getDisabled() const { return m_disabled; }

	virtual void                setSpeed( const Vec2d& _speed ) { ITF_ASSERT_CRASH(_speed==_speed,"Invalid speed"); m_speed = _speed; }
	ITF_INLINE   const Vec2d&   getSpeed() const { return m_speed; }
    ITF_INLINE   void           resetSpeedX() { m_speed.x() = 0.f; }
    ITF_INLINE   void           resetSpeedY() { m_speed.y() = 0.f; }

    virtual void                addForceEvent(EventAddForce * _onAddForce);
	ITF_INLINE virtual void     addForce( const Vec2d& _force ) { ITF_ASSERT_CRASH(_force==_force,"Invalid force"); m_force += _force;}
	ITF_INLINE const Vec2d&     getForce() const { return m_force; }
	ITF_INLINE virtual void     resetForces() { m_force = Vec2d::Zero; m_impulses.clear(); }

	ITF_INLINE virtual void		addImpulse(const Vec2d& _impulse) { m_impulses += _impulse; }
	ITF_INLINE	Vec2d			getImpulses() const { return m_impulses; }
	ITF_INLINE	Vec2d			getCurrentVelocity() const { return m_speed + m_impulses; }

	ITF_INLINE   void           setGravityMultiplier( f32 _mult ) { ITF_ASSERT_CRASH(_mult==_mult,"Invalid multiplier"); m_gravityMultiplier = _mult; }
	ITF_INLINE   f32            getGravityMultiplier() const { return m_gravityMultiplier; }
	ITF_INLINE   f32            getInitialGravityMultiplier() const;

	const Vec2d&                getCurrentGravity() const { return m_currentGravity; }
	const Vec2d&                getCurrentGravityDir() const { return m_currentGravityDir; }

    virtual void                onForceMove();
protected:

	ITF_INLINE const class      PhysComponent_Template*  getTemplate() const;

	virtual void                processQueryPhysics( class EventQueryPhysicsData* _query ) const;
	//////


    f32                         m_mass;                     // Mass this body
    f32						    m_friction;			       
    f32                         m_frictionMultiplier;       // Friction Multiplier for this body
    f32                         m_initialFrictionMultiplier;
    f32                         m_gravityMultiplier;        // Gravity Multiplier for this body
    f32                         m_initialGravityMultiplier;

	Vec2d                       m_speed;                    // Current Speed
	Vec2d                       m_force;                    // The force we add to move it
	Vec2d						m_impulses;			        // real, cinetic-based, Newton-expressed forces that will be applied to the object's cinetic
	Vec2d                       m_currentGravity;           // The current gravity
	Vec2d                       m_currentGravityDir;        // The current gravity direction

	bbool                       m_disabled;                 // If the physics are disabled
};

//---------------------------------------------------------------------------------------------------

class PhysComponent_Template : public ActorComponent_Template
{
	DECLARE_OBJECT_CHILD_RTTI(PhysComponent_Template,ActorComponent_Template,PhysComponent_Template_CRC);
	DECLARE_SERIALIZE()
	DECLARE_ACTORCOMPONENT_TEMPLATE(PhysComponent);

public:

	PhysComponent_Template();
	~PhysComponent_Template() {}

#ifndef ITF_PROJECT_POP
    f32                         getInitialGravityMultiplier() const { return m_initialGravityMultiplier; }

private:
    f32                         m_initialGravityMultiplier;
#endif

};



const PhysComponent_Template*  PhysComponent::getTemplate() const {return static_cast<const PhysComponent_Template*>(m_template);}
#ifndef ITF_PROJECT_POP
f32            PhysComponent::getInitialGravityMultiplier() const { return getTemplate()->getInitialGravityMultiplier(); }
#else
f32            PhysComponent::getInitialGravityMultiplier() const { return m_initialGravityMultiplier; }
#endif

}

#endif // _ITF_PHYSCOMPONENT_H_

