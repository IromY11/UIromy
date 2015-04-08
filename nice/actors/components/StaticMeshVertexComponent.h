#ifndef _ITF_STATICMESHVERTEXCOMPONENT_H_
#define _ITF_STATICMESHVERTEXCOMPONENT_H_

#define StaticMeshVertexComponent_CRC ITF_GET_STRINGID_CRC(StaticMeshVertexComponent,713257486)
#define StaticMeshVertexComponent_Template_CRC ITF_GET_STRINGID_CRC(StaticMeshVertexComponent_Template,3703147453)

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

namespace ITF
{
    struct StaticMeshElement
    {
        DECLARE_SERIALIZE()
        
        StaticMeshElement()
            : m_pos(Vec3d::Zero)
            , m_color(Color::white())
			, m_animated(bfalse)
        {
        }

        Vec3d       m_pos;
        Color       m_color;
		bbool		m_animated;
        ObjectPath  m_frisePath;

        void        appyColor();
        void        postChangeProperties();

        ITF_VECTOR<u16>                 m_staticIndexList;
        ITF_VECTOR<VertexPNC3T>         m_staticVertexList;

#ifdef ITF_SUPPORT_EDITOR
        Vec3d m_posBeforeEdit;
#endif
    };

    struct MeshZData
    {
        u32     m_begin;
        u32     m_size;
		bbool	m_animated;

        u32     m_indexSize;
        u32     m_vertexSize;

        f32     m_z;
    };

    class StaticMeshVertexComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(StaticMeshVertexComponent, GraphicComponent, StaticMeshVertexComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        StaticMeshVertexComponent();
        virtual ~StaticMeshVertexComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onActorClearComponents();
#ifdef ITF_SUPPORT_EDITOR
        virtual void        onBecomeActive();
#endif
        virtual void        Update( f32 _dt );
        virtual void        batchPrimitives( const ITF_VECTOR <class View*>& _views );

        bbool               isFriseListMergeable(const ITF_VECTOR<Frise *> & _frise);
        bbool               addFriseList(const ITF_VECTOR<class Frise *> & _frise);
        void                updateAABB();
        
        const ITF_VECTOR<StaticMeshElement>&  getStaticMeshElments()  const { return m_staticMeshElements; }

        void                setFriseGroup(ObjectRef _group);
        const ObjectRef &   getFriseGroup() const { return m_friseGroup; }
        Scene *             getFriseGroupScene() const;

#if defined(ITF_SUPPORT_EDITOR)
        bbool               computeFriseGroup(bbool _force);
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
        virtual void        onPrePropertyChange();
        virtual void        onPostPropertyChange();
#endif // ITF_SUPPORT_EDITOR

        static bbool        areFrisesMergeable(const ITF_VECTOR<class Frise *> & _frise, String8 & _error, const GFXMaterialSerializable * & _usingMat);
        static bbool        areFrisesMergeable(const ITF_VECTOR<class Frise *> & _frise, String8 & _error);

        bbool               canAddFrises(const ITF_VECTOR<class Frise *> & _frise);

    private:
        friend class StaticMeshVertex_Plugin;
        friend class SMV_PickingShapeData;
        friend class ColorTweakerPlugin;

        void                setColor(u32 _index, const Color & _color);

        void                updateMesh();
        void                clearStaticMesh();
        void                fillStaticMeshFromData();

        u32                 addFrise(class Frise * _frise);

#ifdef ITF_SUPPORT_EDITOR
        void                updateFriseColors();
        void                actorDataToSubSceneData(bbool _init = bfalse);
        void                transformElement(u32 index, const Transform3d & _transform);

        Pickable *          getElement(u32 _index);

        void                moveElement(u32 _index, const Vec3d & _move);
        void                rotateElement(u32 _index, f32 _angle);
        void                scaleElement(u32 _index, const Vec2d & _scale);
#endif
        void                sortElements();
        void                computeLocalAABB();

        ITF_INLINE const class StaticMeshVertexComponent_Template* getTemplate() const;

        ITF_VECTOR<StaticMeshElement>   m_staticMeshElements;
        ITF_VECTOR<u32>                 m_sortedMeshElements;

        GFXMaterialSerializable         m_material;

        ITF_VECTOR<ITF_Mesh>            m_staticMeshList;
        ITF_VECTOR<MeshZData>           m_staticMeshZData;

        bbool                           m_meshNeedUpdate;
        bbool                           m_meshNeedFullUpdate;
        bbool                           m_forceMatrixUpdate;
        AABB                            m_localAABB;
        f32                             m_mergeRange;

        ObjectRef                       m_friseGroup;
        ObjectPath                      m_friseGroupPath;
    };


    //---------------------------------------------------------------------------------------------------

    class StaticMeshVertexComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(StaticMeshVertexComponent_Template, GraphicComponent_Template, StaticMeshVertexComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(StaticMeshVertexComponent)
        DECLARE_SERIALIZE()

    public:

        StaticMeshVertexComponent_Template();
        virtual ~StaticMeshVertexComponent_Template();


    private:

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const StaticMeshVertexComponent_Template* StaticMeshVertexComponent::getTemplate() const
    {
        return static_cast<const StaticMeshVertexComponent_Template*>(m_template);
    }
}

#endif // _ITF_STATICMESHVERTEXCOMPONENT_H_
