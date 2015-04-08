#ifndef _ITF_ACTOR_BIND_H_
#define _ITF_ACTOR_BIND_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef SERIALIZEROBJECTDEFINES_H
#include "core/serializer/ZSerializerObject_Defines.h"
#endif // SERIALIZEROBJECTDEFINES_H

namespace ITF
{
    class Actor;
    
    struct Bind
    {
        DECLARE_SERIALIZE()

        enum Type { Root, BoneName, ProceduralBoneName };
        
        Bind();

        ObjectPath  m_parent;
        bbool       m_useParentScale;
        bbool       m_useParentFlip;
        bbool       m_useParentAlpha;
        bbool       m_removeWithParent;
        Type        m_type;
        u32         m_typeData;
        bbool       m_temporaryBind;

        ObjectRef   m_runtimeParent;

        ITF_INLINE const Vec3d& getInitialPosOffset() const { return m_initialPositionOffset; }
        ITF_INLINE void         setInitialPositionOffset( const Vec3d& _value ) { m_initialPositionOffset = _value; }

        ITF_INLINE const Vec3d& getPosOffset() const { return m_positionOffset; }
        ITF_INLINE void         setPosOffset( const Vec3d& _value ) { m_positionOffset = _value; }
        ITF_INLINE void         set2DPosOffset( const Vec2d& _value ) { m_positionOffset.x() = _value.x(); m_positionOffset.y() = _value.y(); }

        ITF_INLINE f32          getInitialAngleOffset() const { return m_initialAngleOffset; }
        ITF_INLINE void         setInitialAngleOffset( f32 _value ) { m_initialAngleOffset = _value; }

        ITF_INLINE f32          getAngleOffset() const { return m_angleOffset; }
        ITF_INLINE void         setAngleOffset( f32 _value ) { m_angleOffset = _value; }

        ITF_INLINE bbool        isRuntimeDisabled() const { return m_runtimeDisabled; }
        ITF_INLINE void         setRuntimeDisabled(ITF::bbool _val) { m_runtimeDisabled = _val; }
        
        void                    onSceneActive(Pickable* _owner);
        void                    onSceneInactive(Pickable* _owner);

        static bbool            relativePathGoingUp( const ObjectPath& _relativePath );

    private:

        Vec3d       m_initialPositionOffset;
        Vec3d       m_positionOffset;
        f32         m_initialAngleOffset;
        f32         m_angleOffset;
        
        bbool       m_runtimeDisabled;

        void postLoadInstance()
        {
            m_positionOffset = m_initialPositionOffset;
            m_angleOffset = m_initialAngleOffset;
        }
    };

    class BindHandler
    {
    public:
        BindHandler();
        ~BindHandler()  {}

        void                    clear();
        void                    setOwner(Actor* _owner);

        void                    addChild(Actor* _child, bbool _updatePosRecursive = bfalse);
        void                    removeChild(Actor* _child);
        const ObjectRefList&    getChildren() const {return m_children; }
        ObjectRefList&			getChildren() {return m_children; }
        void                    requestChildrenDestruction();

        void                    update();
        void                    updateWorldCoordinates( Actor* _childActor, Bind* _childBind, bbool _recursive = bfalse ) const;
        void                    growAABB();
        void                    replaceReinitDataByCurrent();

        bbool                   getTransform( const Bind* _childBind, Vec3d& _pos, f32& _angle ) const;
        bbool                   computeWorldCoordinates( const Bind* _childBind, const Vec3d& _localPos, const f32 _localAngle, Vec3d& _worldPos, f32& _worldAngle ) const;
        bbool                   computeLocalCoordinates( const Bind* _childBind, const Vec3d& _worldPos, const f32 _worldAngle, Vec3d& _localPos, f32& _localAngle ) const;
        bbool                   computeWorldInitialCoordinates( const Bind* _childBind, const Vec3d& _localPos, const f32 _localAngle, Vec3d& _worldPos, f32& _worldAngle ) const;
        bbool                   computeLocalInitialCoordinates( const Bind* _childBind, const Vec3d& _worldPos, const f32 _worldAngle, Vec3d& _localInitPos, f32& _localInitAngle ) const;
        
        void                    onSceneActive();
        void                    onSceneInactive();

        void                    computeInitialTransform(  Actor* _bindedChild ) const;
        
#ifdef ITF_SUPPORT_EDITOR
        void                    onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

    private:
        bbool                   hasChild(Actor* _child) const;
        Actor*                  m_owner;
        ObjectRefList           m_children;
    };

} // namespace ITF

#endif // _ITF_ACTOR_BIND_H_