#ifndef _ITF_ATLASGRAPHICCOMPONENT_H_
#define _ITF_ATLASGRAPHICCOMPONENT_H_

#define AtlasGraphicComponent_CRC ITF_GET_STRINGID_CRC(AtlasGraphicComponent,3138797560)
#define AtlasGraphicComponent_Template_CRC ITF_GET_STRINGID_CRC(AtlasGraphicComponent_Template,2893345270)

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

namespace ITF
{
    class AtlasGraphicComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AtlasGraphicComponent, GraphicComponent, AtlasGraphicComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        AtlasGraphicComponent();
        virtual ~AtlasGraphicComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual bbool       needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onResourceLoaded();
        virtual void        onUnloadResources();
        virtual void        Update( f32 _dt );
        virtual void        batchPrimitives( const ITF_VECTOR<class View*>& _views );
        virtual void        onEvent( Event* _event);
        virtual void        onPostPropertyChange( );

        void                            setMaterial(const GFX_MATERIAL& _material) { m_material = _material; }
        ITF_INLINE const GFX_MATERIAL & getMaterial() { return m_material; }

    private:
        ITF_INLINE const class AtlasGraphicComponent_Template* getTemplate() const;
        void                    createMesh();
        void                    clearMesh();
        void                    updateAABB();
        Vec3d                   getFinalPos() { return m_actor->getPos() + m_offset; }

        void                    fillRectangle(ITF_VECTOR<VertexPCT> & _ptList, ITF_VECTOR<u16> & _idxList, const UVdata * pUvData, const UVparameters * pUvParam);
        void                    fillVertex(ITF_VECTOR<VertexPCT> & _ptList, const UVdata * pUvData, const UVparameters * pUvParam);
        void                    fillTriangles(ITF_VECTOR<VertexPCT> & _ptList, ITF_VECTOR<u16> & _idxList, const UVdata * pUvData, const UVparameters * pUvParam);
        void                    fillPointList(ITF_VECTOR<VertexPCT> & _ptList, ITF_VECTOR<u16> & _idxList, const UVdata * pUvData, const UVparameters * pUvParam);

        GFX_MATERIAL            m_material;
        TextureSetSubID         m_textureLayer;
        u32                     m_atlasIndex;
        f32                     m_extrudeFactor;               
        Vec3d                   m_offset;

        GFXMaterialSerializable m_materialSerializable;
        TextureSetSubID         m_textureLayerSerializable;
        ITF_Mesh                m_mesh;
        ITF_IndexBuffer*        m_indexBuffer;
        AABB                    m_aabb;
        Vec2d                   m_zMinMax;
    };


    //---------------------------------------------------------------------------------------------------

    class AtlasGraphicComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AtlasGraphicComponent_Template, GraphicComponent_Template, AtlasGraphicComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(AtlasGraphicComponent)
        DECLARE_SERIALIZE()

    public:

        AtlasGraphicComponent_Template();
        virtual ~AtlasGraphicComponent_Template();

        ITF_INLINE const GFX_MATERIAL & getMaterial() const {return m_material;}
        TextureSetSubID getTextureLayer() const { return m_textureLayer; }
        const Vec2d & getTexelRatio() const { return m_texelRatio; }


        virtual bbool  onTemplateLoaded( bbool _hotReload );
        virtual void   onTemplateDelete( bbool _hotReload );

    private:
        GFXMaterialSerializable m_material;
        TextureSetSubID         m_textureLayer;
        Vec2d                   m_texelRatio;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const AtlasGraphicComponent_Template* AtlasGraphicComponent::getTemplate() const
    {
        return static_cast<const AtlasGraphicComponent_Template*>(m_template);
    }
}

#endif // _ITF_ATLASGRAPHICCOMPONENT_H_
