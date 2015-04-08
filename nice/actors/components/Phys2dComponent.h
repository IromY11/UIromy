#ifndef _ITF_PHYS2DCOMPONENT_H_
#define _ITF_PHYS2DCOMPONENT_H_

#define Phys2dComponent_CRC ITF_GET_STRINGID_CRC(Phys2dComponent,2041187582)
#define Phys2dComponent_Template_CRC ITF_GET_STRINGID_CRC(Phys2dComponent_Template,4146242593)

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/ActorComponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_EDITABLESHAPE_H_
#include "gameplay/tools/EditableShape.h"
#endif // _ITF_EDITABLESHAPE_H_

#ifndef _ITF_PHYS2D_JOINT_DETAILS_H_
#include "engine/Phys2d/Phys2dJointType.h"
#endif //_ITF_PHYS2D_JOINT_DETAILS_H_

#ifdef ITF_SUPPORT_EDITOR
# ifndef ITF_ENGINE_MESH_H_
#  include "engine/display/Primitives/Mesh.h"
# endif //ITF_ENGINE_MESH_H_#ifdef USE_BOX2D
#endif //ITF_SUPPORT_EDITOR

#ifdef USE_BOX2D

#include "Box2d/Box2d.h"

namespace ITF
{
    class Phys2dComponent;
    class LinkComponent;

    class JointInfo : public BaseObject
    {
        DECLARE_SERIALIZE()

    public: 
        JointInfo();
        JointInfo(const ObjectPath & _objPath);
        ~JointInfo();

        Pickable *  getChild(Pickable * _parentPick) const;

        bbool       m_isActive;
        ObjectPath  m_childPath;
        Vec2d       m_localPosA;
        Vec2d       m_localPosB;
        bbool       m_invertAB;

        class JointDetails * m_jointDetail;
        bbool       m_manualyDestroyed;

    };

    class JointInstance
    {
    public:
        JointInstance();

        void        deleteJoint();
        bbool       createJoint(JointInfo * _jointInfo, Pickable * _parentPick, b2Body * _parentBody, bbool _applyScale);
        bbool       updateJoint(JointInfo * _jointInfo, bbool _applyScale);
        b2Joint    *getJoint() const { return m_joint; }

    private:
        b2Body     *m_parent;
        b2Vec2      m_parentLocalPos;

        b2Body     *m_child;
        b2Vec2      m_childLocalPos;

        b2Joint    *m_joint;
    };

    class Phys2dComponentListener
    {
    public:
		// Called when entering in contact with a new element
        virtual void beginContact   (Pickable* _pickableA, Pickable* _pickableB, b2Contact* contact) {}
		// Called when a contact situation with an element has been broken
        virtual void endContact     (Pickable* _pickableA, Pickable* _pickableB, b2Contact* contact) {}
		// Called before each resolution step with a contact. Can be called several time when a contact is continuous into time
        virtual void preSolve       (Pickable* _pickableA, Pickable* _pickableB, b2Contact* contact, const b2Manifold* oldManifold)  {}
		// Called after each resolution step with a contact. Can be called several time when a contact is continuous into time
        virtual void postSolve      (Pickable* _pickableA, Pickable* _pickableB, b2Contact* contact, const b2ContactImpulse* impulse)  {}
    };

    
    class Phys2dComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Phys2dComponent, ActorComponent, Phys2dComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Phys2dComponent();
        virtual ~Phys2dComponent();

        virtual bbool       needsUpdate() const { return btrue; }
#ifdef ITF_SUPPORT_EDITOR
        virtual bbool       needsDraw() const { return true; }
#else
        virtual bbool       needsDraw() const { return bfalse; }
#endif
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual bbool       needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onFinalizeLoad();
        virtual void        onResourceReady();
        virtual void        onSceneInactive();
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        Update( f32 _dt );
		virtual void onStartDestroy(bbool _hotReload);
        virtual void        onEvent( Event* _event);
		virtual void		teleportTo(const Vec3d& _pos);
        virtual void		teleportTo(const Vec3d& _pos, f32 _angle);

#ifdef ITF_SUPPORT_EDITOR
        virtual void        batchPrimitives( const ITF_VECTOR<class View*>& _views );

        virtual void        onEditorMove(bbool _modifyInitialPos);
        virtual void        drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags )  const;
        virtual void        onEditorCreated( class Actor* _original );
        virtual void        onPostPropertyChange();
                void        centerActorAndEditableShape();
#endif

        void                registerListener(Phys2dComponentListener * _listener);
        void                unregisterListener(Phys2dComponentListener * _listener);
        void                unregisterAllListener();

        b2Body            * getBody() { return m_body; }
		b2Fixture         * getFixture() { return m_fixtureList[0]; }

		void				setBodyType(b2BodyType _type) { m_bodyType = _type; computeBodies();}
        
        ITF_VECTOR<JointInfo> &         getJointInfoList() { return m_jointInfoList; }
        ITF_VECTOR<JointInstance> &     getJointInstanceList() { return m_jointInstanceList; }
        ITF_VECTOR<JointInfo *>         getJointInfoByType(JointDetailsType _type);

        void                            updateJointList();

        b2World *               getPhys2dWorld();
        void                    setPhys2dWorld(b2World * _world);
		ITF_INLINE f32			getDefaultFriction() const { return m_friction; }
		ITF_INLINE f32			getDefaultLinearDamping() const { return m_linearDamping; }
        void                    setJointDeleted(b2Joint * _joint);

        void                    setActive( bbool _active );
        bbool                   isActive();

        ITF_INLINE b2BodyType   getBodyType() const { return m_bodyType; }
        ITF_INLINE f32          getDensity() const { return m_density; }
        ITF_INLINE f32          getFriction() const { return m_friction; }
        ITF_INLINE f32          getRestitution() const { return m_restitution; }

        ITF_INLINE f32          getLinearDamping() const { return m_linearDamping; }
        ITF_INLINE f32          getAngularDamping() const { return m_angularDamping; }
        ITF_INLINE bbool        isFixedRotation() const { return m_fixedRotation; }
        ITF_INLINE f32          getGravityMultiplier() const { return m_gravityMultiplier; }
        ITF_INLINE u32          isBullet() const { return m_isBullet; }
		ITF_INLINE bbool		isSensor() const { return m_isSensor; }

        ITF_INLINE u16          getCategoryBits() const { return m_categoryBits; }
        ITF_INLINE u16          getMaskBits() const { return m_maskBits; }
		ITF_INLINE void			setCategoryBits(u16 _categoryBits) { m_categoryBits = _categoryBits; }
		ITF_INLINE void			setMaskBits(u16 _maskBits) { m_maskBits = _maskBits; }

		ITF_INLINE void			setLinkRotationToActor(const bbool _toSet) { m_linkRotationToActor = _toSet; }
		ITF_INLINE bbool		getLinkRotationToActor() const { return m_linkRotationToActor; }
        ITF_INLINE bbool		getApplyScaleToJoints() const { return m_applyScaleToJoints; }

        virtual void            beginContact( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact );
        virtual void            endContact( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact );
        virtual void            preSolve( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact, const b2Manifold* _oldManifold );
        virtual void            postSolve( Pickable* _pickableA, Pickable* _pickableB, b2Contact* _contact, const b2ContactImpulse* _impulse );
        
        ITF_INLINE void         polylinesClear(){m_polylines.clear();}
        void                    polylineAdd(StringID _polylineID, bbool _recomputeBody = bfalse);
        void                    polylinesRemove(StringID _polylineID, bbool _recomputeBody = bfalse);
        void                    updateBodies();
		ITF_INLINE void			setPhysicEnabled(bbool _bVal) { m_enablePhysic = _bVal; }
        const class GameMaterial_Template*  getGameMaterial() const;


		ITF::Vec2d				getPhysScale() const { return m_physScale; }
		void					setPhysScale(ITF::Vec2d val) { m_physScale = val; computeBodies();}

    private:
        friend class Phys2dContactListener;
        friend class Phys2dJoint_Plugin;

        ITF_INLINE const class Phys2dComponent_Template* getTemplate() const;

    protected:
        void                fillFixtureDef(b2FixtureDef & _fixtureDef);
        void                clearBodies();
        //void                updateBodies();
        virtual void        computeBodies();
        void                computeBodiesIntern();
        void                clearSourceJoints();
        void                updateJointDestruction();
        Vec2d               getEditableShapeGlobalOffset(bbool _rotate) const;
        void                updateAABB();
        bbool               isFixtureListValid();
        void                createFixtureList(b2FixtureDef * _fixtureDef);
        bbool               fixtureListTestPoint(const b2Vec2 & _point);

        ITF_VECTOR<StringID> m_polylines;         // the name of a polyline (from the animation) to create a shape from

        b2World             *m_world;
        b2World             *m_forcedWorld;

        b2Body                  *m_body;
        ITF_VECTOR<b2Fixture*>   m_fixtureList;

        b2BodyType          m_bodyType;
        f32                 m_density;
        f32                 m_friction;
        f32                 m_restitution;
        f32                 m_linearDamping;
        f32                 m_angularDamping;
        f32                 m_gravityMultiplier;
        bbool               m_fixedRotation;
        bbool               m_enablePhysicInit;
        bbool               m_enablePhysic;
        bbool               m_startAwake;
        bbool               m_isAwake;
        bbool               m_needUpdateJoints;
        u32                 m_isBullet;
		bbool				m_isSensor;
        bbool               m_applyScaleToJoints;

        u16                 m_categoryBits;
        u16                 m_maskBits;

		bbool				m_linkRotationToActor;

        ITF_VECTOR<Phys2dComponentListener *> m_listernerList;
        EditableShape       m_editableShape;
        bbool               m_editableShapeCentered;

        LinkComponent               *m_linkComponent;
        ITF_VECTOR<JointInfo>       m_jointInfoList;
        ITF_VECTOR<JointInstance>   m_jointInstanceList;

        bbool                       m_shapeShow;
		Color                       m_shapeColor;

		Vec2d						m_physScale;
#ifdef ITF_SUPPORT_EDITOR
        void                        clearCurrentDrag();
        bbool                       m_linkEdition;
        static Pickable *           s_currentDrag;
        static b2Body   *           s_gound;
        static b2MouseJoint *       s_joint;

        ITF_Mesh                    m_shapeMesh;
        GFX_MATERIAL                m_shapeMat;

        void                        clearMeshFromBodyFixtures();
        void                        createMeshFromBodyFixtures();
#endif
#if !defined(ITF_DISABLE_WARNING)
        String8                     m_error;
#endif
    };


    //---------------------------------------------------------------------------------------------------

    class Phys2dComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Phys2dComponent_Template, ActorComponent_Template, Phys2dComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(Phys2dComponent)
        DECLARE_SERIALIZE()

    public:

        Phys2dComponent_Template();
        virtual ~Phys2dComponent_Template();


        const PhysShape *   getShape() const { return m_shape; }
        f32                 getDensity() const { return m_density; }
        f32                 getFriction() const { return m_friction; }
        f32                 getRestitution() const { return m_restitution; }

        f32                 getLinearDamping() const { return m_linearDamping; }
        f32                 getAngularDamping() const { return m_angularDamping; }
        bbool               isFixedRotation() const { return m_fixedRotation; }
        f32                 getGravityMultiplier() const { return m_gravityMultiplier; }

        const class GameMaterial_Template*  getGameMaterial() const { return m_gameMaterialTemplate; }

//         ITF_INLINE ITF_VECTOR<StringID>::const_iterator getPolylinesNameBegin() const { return m_polylines.begin(); }
//         ITF_INLINE ITF_VECTOR<StringID>::const_iterator getPolylinesNameEnd() const { return m_polylines.end(); }
        ITF_INLINE u32 getPolylinesNameSize() const { return m_polylines.size(); }
        ITF_VECTOR<StringID> getPolylinesNames() const { return m_polylines; }
        ITF_INLINE u32 isBullet() const { return m_isBullet; }
		ITF_INLINE bbool isSensor() const { return m_isSensor; }
        ITF_INLINE bbool applyScaleToJoints() const { return m_applyScaleToJoints; }

        virtual    bbool onTemplateLoaded( bbool _hotReload );

    private:
        PhysShape * m_shape;
        ITF_VECTOR<StringID> m_polylines;         // the name of a polyline (from the animation) to create a shape from

        f32                 m_density;
        f32                 m_friction;
        f32                 m_restitution;
        f32                 m_linearDamping;
        f32                 m_angularDamping;
        f32                 m_gravityMultiplier;
        bbool               m_fixedRotation;
        u32                 m_isBullet;
		bbool				m_isSensor;
        bbool               m_applyScaleToJoints;

        Path                                m_gameMaterial;
        const class GameMaterial_Template*  m_gameMaterialTemplate;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Phys2dComponent_Template* Phys2dComponent::getTemplate() const
    {
        return static_cast<const Phys2dComponent_Template*>(m_template);
    }

    ITF_INLINE const class GameMaterial_Template*  Phys2dComponent::getGameMaterial() const
    { 
        return getTemplate()->getGameMaterial(); 
    }

}

#endif // USE_BOX2D

#endif // _ITF_PHYS2DCOMPONENT_H_


