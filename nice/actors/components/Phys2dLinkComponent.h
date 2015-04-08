#ifndef _ITF_PHYS2DLINKCOMPONENT_H_
#define _ITF_PHYS2DLINKCOMPONENT_H_

#define Phys2dLinkComponent_CRC ITF_GET_STRINGID_CRC(Phys2dLinkComponent,2096871320)
#define Phys2dLinkComponent_Template_CRC ITF_GET_STRINGID_CRC(Phys2dLinkComponent_Template,3880572630)

#ifdef USE_BOX2D

class b2World;
class b2Body;
class b2MouseJoint;
class b2Joint;

namespace ITF
{
    class LinkComponent;

    struct Phys2dLinkInfo
    {
        Phys2dLinkInfo()
            : m_linkedActor(NULL)
            , m_localPos(Vec2d::Zero)
            , m_localAngle(0.f)
            , m_ground(NULL)
            , m_joint(NULL)
            , m_useJoint(btrue)
        {
        }

        void            deleteLink(b2World * _world);
        bbool           computeLocalData(Pickable *_parent);
        bbool           getGlobalPosAndAngle(Pickable *_parent, Vec2d & _pos, f32 & _angle);

        
        Actor *         m_linkedActor;
        Vec2d           m_localPos;
        f32             m_localAngle;

        b2Body        * m_ground;
        b2MouseJoint  * m_joint;

        bbool           m_useJoint;
    };

    class Phys2dLinkComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Phys2dLinkComponent, ActorComponent, Phys2dLinkComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Phys2dLinkComponent();
        virtual ~Phys2dLinkComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual bbool       needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onFinalizeLoad();

        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();

        virtual void onStartDestroy(bbool _hotReload);

        virtual void        Update( f32 _dt );
        virtual void        onEvent( Event* _event);
#ifdef ITF_SUPPORT_EDITOR
        virtual void        onEditorMove(bbool _modifyInitialPos /* = btrue */);
        virtual void        onPostPropertyChange();
#endif

    private:
        void                deleteJoints();
        void                createJoints();
        void                setJointDeleted(b2Joint * _joint);

        ITF_INLINE const class Phys2dLinkComponent_Template* getTemplate() const;
        b2World       * m_world2d;
        LinkComponent * m_linkComponent;
        bbool           m_needUpdateJoints;

        bbool           m_snapChildren;
        ITF_VECTOR<Phys2dLinkInfo>  m_linkList;
    };


    //---------------------------------------------------------------------------------------------------

    class Phys2dLinkComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Phys2dLinkComponent_Template, ActorComponent_Template, Phys2dLinkComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(Phys2dLinkComponent)
        DECLARE_SERIALIZE()

    public:

        Phys2dLinkComponent_Template();
        virtual ~Phys2dLinkComponent_Template();

        f32 getDampingRatio() const { return m_dampingRatio; }
        f32 getFrequencyHz() const { return m_frequencyHz; }
        f32 getMaxForce() const { return m_maxForce; }

    private:
        f32         m_dampingRatio;
        f32         m_frequencyHz;
        f32         m_maxForce;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Phys2dLinkComponent_Template* Phys2dLinkComponent::getTemplate() const
    {
        return static_cast<const Phys2dLinkComponent_Template*>(m_template);
    }
}
#endif // USE_BOX2D

#endif // _ITF_PHYS2DLINKCOMPONENT_H_
