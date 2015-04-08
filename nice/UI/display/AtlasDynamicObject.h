#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#define _ITF_ATLASDYNAMICOBJECT_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_   

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

#ifndef ITF_ENGINE_MESH_H_
# include "engine/display/Primitives/Mesh.h"
#endif //ITF_ENGINE_MESH_H_

namespace ITF
{

class Vec2d;
class Vec3d;
class Texture;

class AtlasDynamicObject
{
public:
    //////////////////////////////////////////////////////////////////////////
    ///Constructor
    AtlasDynamicObject();
    ~AtlasDynamicObject();
    
    //p1 ----- p4
    //|        |
    //|        |
    //p2 ----- p3
    void                    addQuad(int _indexInAtlas, const Vec2d& _pt1, const Vec2d& _pt2, const Vec2d& _pt3, const Vec2d& _pt4,  f32 _zZoom = 0.0f, u32 _color = COLOR_WHITE);

    void                    initialize(const GFX_MATERIAL & _mat, u32 _maxQuad);

    void                    clear(); //DEALLOCATES/REINITIALIZES ALL

    void                    clearVertices() {m_vertex.clear();}

    void                    draw(const ITF_VECTOR <class View*>& _views, f32 _z, f32 _depthOffset,  bool _clearAfter, const ConstObjectRef & _objectRef);
    void                    draw2d(const ITF_VECTOR <class View*>& _views, f32 _priority, bool _clearAfter, const ConstObjectRef & _objectRef);
    ITF_VertexBuffer *      getCurrentVertexBuffer() const;
    VertexPCT*              startFillVertexBuffer();
    void                    endFillVertexBuffer();
    void                    drawVertexBuffer(const ITF_VECTOR <class View*>& _views, f32 _z, u32 _quadCount, const ConstObjectRef & _objectRef);
    u32                     getMaxQuad() const {return m_maxQuad;}

    ITF_INLINE ux           getNumberUV() const;

    bbool                   getUVAt(u32 _indexInAtlas, Vec2d* _uvOut);
    typedef SafeArray<VertexPCT, 4 * 256> VertexPCTList;
    ITF_INLINE VertexPCTList & getVertex() { return m_vertex; }
    bbool                   isValid() const;

    ITF_INLINE const GFXPrimitiveParam& getGfxPrimitiveParam() const { return m_primitiveParam;}
    ITF_INLINE       GFXPrimitiveParam& getGfxPrimitiveParam() { return m_primitiveParam;}
    ITF_INLINE void                     setGFXPrimitiveParam( const GFXPrimitiveParam& _param){ m_primitiveParam = _param;}

    u32                     getNumIndices();

    const GFX_MATERIAL &          getMaterial() const
    {
        return m_mesh.getMaterial(0u);
    }

    GFX_MATERIAL &          getMaterial()
    {
        return m_mesh.getMaterial(0u);
    }

	///////////////////////////////////////////////////////////////////////////////////////////
	/// get texture atlas (contains all icons)
	const Texture *         getAtlasTexture() const;

private:
    
    void                    createMeshVertexBuffer();
    void                    attachMaterial(ResourceContainer* _container, const class GFXMaterial_Template * _materialTpl);
    
	u32                     m_maxQuad;

    GFX_MATERIAL            m_atlasMaterial;
    ITF_Mesh                m_mesh;
    VertexPCTList m_vertex;
    GFXPrimitiveParam       m_primitiveParam;
};

ITF_INLINE ux AtlasDynamicObject::getNumberUV() const
{
    if (isValid())
    {
        const UVAtlas *atlas = getAtlasTexture()->getUVAtlas();
        if (atlas)
        {
            return atlas->getNumberUV();
        }
    }
    return 1;
}

}

#endif // _ITF_ATLASDYNAMICOBJECT_H_

