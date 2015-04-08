#ifndef _ITF_ANIMMESHVERTEXCOMPONENT_H_
#define _ITF_ANIMMESHVERTEXCOMPONENT_H_

#define AnimMeshVertexComponent_CRC ITF_GET_STRINGID_CRC(AnimMeshVertexComponent,2546232545)
#define AnimMeshVertexComponent_Template_CRC ITF_GET_STRINGID_CRC(AnimMeshVertexComponent_Template,904004162)

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_


#ifndef _ITF_ANIMATIONMESHVERTEX_H_
#include    "engine/animation/AnimationMeshVertex.h"
#endif // _ITF_ANIMATIONMESHVERTEX_H_

#ifndef _ITF_SUBANIMLIGHTSET_H_
#include    "engine/animation/SubAnimSet.h"
#endif //_ITF_SUBANIMLIGHTSET_H_

namespace ITF
{

    struct AMVPartition
    {
        u32     m_begin;
        u32     m_size;
        f32     m_z;
    };

    class AnimMeshVertexComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimMeshVertexComponent, GraphicComponent, AnimMeshVertexComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        AnimMeshVertexComponent();
        virtual ~AnimMeshVertexComponent();

        virtual bbool       needsUpdate() const;
        virtual bbool       needsDraw() const;
        virtual bbool       needsDraw2D() const;

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        Update( f32 _dt );
        virtual void        batchPrimitives( const ITF_VECTOR <class View*>& _views );
        virtual void        batchPrimitives2D( const ITF_VECTOR <class View*>& _views );
        virtual void        onEvent( Event* _event);
        virtual void        onResourceLoaded();
        virtual void        onUnloadResources();

        ITF_INLINE void     enableDraw(bbool _enable) { m_drawEnabled = _enable;}

		class AnimMeshVertex *				getAnimMeshVertex() const;
        void                                setUseActorTransform(bbool _use) { m_useActorTransform = _use; }
        bbool                               getUseActorTransform() const { return m_useActorTransform; }

        void                                computeRuntimeData(bbool _recomputeAABB, bbool _updateFrames);

#if defined(ITF_SUPPORT_EDITOR)
        void                                processUnderMouse(u32 _index);
        virtual void                        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        bbool                               animsOk();

        u32                                 getAMVListSize() const { return m_animListRuntime.size(); }
        void                                resizeAMVList(u32 _size, bbool _deactivateEditorMode = btrue) { m_animListRuntime.resize(_size); computePartition(_deactivateEditorMode); }

        void                                addToAMVList(const SingleAnimDataRuntime & _single) { m_animListRuntime.push_back(_single); ; computePartition(btrue); }
        void                                deleteAMVFromList(u32 _idx, u32 _nbToDelete = 1, bbool _deactivateEditorMode = btrue) { m_animListRuntime.erase(m_animListRuntime.begin() + _idx, m_animListRuntime.begin() + (_idx + _nbToDelete)); computePartition(_deactivateEditorMode);}

        void                                swapAMVList(SingleAnimDataRuntimeList & _amvList, bbool _deactivateEditorMode = btrue) { m_animListRuntime.swap(_amvList); computePartition(_deactivateEditorMode); }

        void                                updateAABB(bbool _fillLocal = bfalse);
        void                                addFramesToAllAMV(u32 _nbFrames = 1);
        
        void                                setAMV(u32 _idx, const SingleAnimDataRuntime & _single) { m_animListRuntime[_idx] = _single; }
        const SingleAnimDataRuntime &       getAMV(u32 _idx) { return m_animListRuntime[_idx]; }

        SingleAnimDataRuntimeList &			getAMVList() { return m_animListRuntime; }

        void                                setAMVFrame(u32 _idx, u32 _frame) { m_animListRuntime[_idx].setFrame(_frame); }
        u32                                 getAMVFrame(u32 _idx) const { return m_animListRuntime[_idx].getFrame(); }

        void                                setAMVAnim(u32 _idx, u32 _animIndex);
        u32                                 getAMVAnim(u32 _idx) const { return m_animListRuntime[_idx].getAnim(); }
        void                                setAMVAnimByName(u32 _idx, const StringID & _animName);

        Transform3d                         getActorTransform();
        void                                setAMVTransform(u32 _idx, const Transform3d & _trans) { m_animListRuntime[_idx].setTransform3d(_trans); }
        void                                setAMVTransformFrom(u32 _idx, const Vec3d& _pos, f32 _angle, const Vec2d& _scale, const bbool _flip ) { m_animListRuntime[_idx].setTransform3dFrom(_pos, _angle, _scale, _flip); }
        void                                setAMVTransformFrom(u32 _idx, const Vec3d& _pos, const Vec2d& _dir, const Vec2d& _scale, const bbool _flip ) { m_animListRuntime[_idx].setTransform3dFrom(_pos, _dir, _scale, _flip); }
        const Transform3d &                 getAMVTransform(u32 _idx) const { return m_animListRuntime[_idx].getTransform3d(); }

        void                                setAMVColor(u32 _idx, const Color & _color) { m_animListRuntime[_idx].setColor(_color); }
        const Color &                       getAMVColor(u32 _idx) const { return m_animListRuntime[_idx].getColor(); }

        void                                setAMVUVRedirectPtr(u32 _idx, SafeArray<u32>* _uvRedirect) { m_animListRuntime[_idx].setUVRedirectPtr(_uvRedirect); }
        void                                setUseEditor(bbool _val) { m_useEditor = _val; computePartition(!m_useEditor);  }
        bbool                               useEditor()  const;

        // AABB auto compute can be set ON when not in useEditorMode.
        // By default, in non-editor mode and to avoid expensive AABB computation, it's AI responsibility to compute the AABB.
        void                                setAutoComputeAABB(bbool _bAuto) { m_autoComputeAABB = _bAuto; }
        bbool                               getAutoComputeAABB() const { return m_autoComputeAABB; }
		
		void								setForcePartition(bbool _val) { m_forcePartition = _val; }
        void								computePartition(bbool _deactivateEditorMode);

		u32                                 getAnimListSize() const { return m_animList.size(); }
        const Vec3d		                    &getOriginalAMVPos(u32 _idx) const { return m_animList[_idx].m_pos; }

#if defined(ITF_SUPPORT_EDITOR)
        u32                                 getOriginalAMVListSize() const { return m_animList.size(); }
        void                                setOriginalAMVColor(u32 _idx, const Color & _color) { m_animList[_idx].m_color = _color; }
        const Color &                       getOriginalAMVColor(u32 _idx) const { return m_animList[_idx].m_color; }
#endif
    private:
        ITF_INLINE const class AnimMeshVertexComponent_Template*    getTemplate() const;
        AABB                                                        getAABB() const;

		void								computePlayRate(const f32 _dt);

        SingleAnimDataList              m_animList;
        String8                         m_error;
        f32                             m_lastFrame;
    
    protected:
        friend class AnimMeshVertex_Plugin;
        friend class AMV_PickingShapeData;
        SingleAnimDataList&			    getAnimList() { return m_animList; }

        void                            fillAMVBaseData(const ITF_VECTOR <class View*>& _views);

        bbool                           m_autoComputeAABB;
        bbool                           m_useEditor;
		bbool                           m_forcePartition;
        AMVRenderData                   m_amvBaseData;
        ITF_VECTOR<AMVPartition>        m_amvPartitionList;
        bbool                           m_useActorTransform;
        SingleAnimDataRuntimeList       m_animListRuntime;
        AABB                            m_localAABB;
        u32                             m_uid;
        f32                             m_mergeRange;
        bbool                           m_drawEnabled;
		f32								m_playRate;
		f32								m_stopTime;
		bbool							m_stopWanted;

#if defined(ITF_SUPPORT_EDITOR)
        u32                             m_underMouseIndex;
#endif
    };


    //---------------------------------------------------------------------------------------------------

    class AnimMeshVertexComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimMeshVertexComponent_Template, GraphicComponent_Template, AnimMeshVertexComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(AnimMeshVertexComponent)
        DECLARE_SERIALIZE()

    public:

        AnimMeshVertexComponent_Template();
        virtual ~AnimMeshVertexComponent_Template();

        bbool onTemplateLoaded( bbool _hotReload );
        void  onTemplateDelete(bbool _hotReload);

        class AnimMeshVertex *      getAnimMeshVertex() const;
        const GFX_MATERIAL &        getMaterial() const { return m_material; }
        const Path &                getAmvPath() const { return m_amvPath; }

        bbool                       useDefaultUpdate() const { return m_defaultUpdate; }
        bbool                       getDraw2d() const { return m_draw2D; }
        bbool                       getUseDataAnims() const { return m_useDataAnims; }
        bbool                       useEditor() const { return m_useEditor; }

        bbool                       getUseActorScale() const { return m_useActorScale; }

		ITF_INLINE const f32		getStopDuration() const { return m_stopDuration; }

    private:
        bbool                       m_useEditor;

        bbool                       m_defaultUpdate;
        bbool                       m_useDataAnims;
        bbool                       m_draw2D;

        Path                        m_amvPath;
        GFXMaterialSerializable     m_material;

        bbool                       m_useActorScale;

        // computed resource
        ResourceID                  m_amvResourceID;

		f32							m_stopDuration;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const AnimMeshVertexComponent_Template* AnimMeshVertexComponent::getTemplate() const
    {
        return static_cast<const AnimMeshVertexComponent_Template*>(m_template);
    }

    ITF_INLINE bbool AnimMeshVertexComponent::needsUpdate() const 
    { 
        return btrue; 
    }


    ITF_INLINE bbool AnimMeshVertexComponent::needsDraw() const 
    { 
        return getTemplate() && !getTemplate()->getDraw2d(); 
    }

    ITF_INLINE bbool AnimMeshVertexComponent::needsDraw2D() const
    { 
        return getTemplate() && getTemplate()->getDraw2d();
    }

    ITF_INLINE bbool AnimMeshVertexComponent::useEditor() const
    { 
        return m_useEditor && getTemplate()->useEditor(); 
    }
}

#endif // _ITF_ANIMMESHVERTEXCOMPONENT_H_
