#include "precompiled_engine.h"

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

namespace ITF
{

AtlasDynamicObject::AtlasDynamicObject()
: m_maxQuad(0)
, m_primitiveParam()
{
}
//////////////////////////////////////////////////////////////////////////
void AtlasDynamicObject::createMeshVertexBuffer()
{
    if (m_mesh.getCurrentVB()) return;
    m_mesh.createVertexBuffer(
        4 * m_maxQuad,
        VertexFormat_PCT,
        sizeof(VertexPCT),
        vbLockType_dynDiscard
        );
  }

void AtlasDynamicObject::initialize(const GFX_MATERIAL & _mat, u32 _maxQuad)
{
    m_maxQuad = _maxQuad;

    createMeshVertexBuffer();

    m_mesh.addElementAndMaterial(_mat);
    m_mesh.getMeshElement().m_indexBuffer = GFX_ADAPTER->needSpriteIndexBuffer(6 * m_maxQuad);
}


//////////////////////////////////////////////////////////////////////////
const Texture * AtlasDynamicObject::getAtlasTexture() const
{
    return m_mesh.getMeshElementMaterial().getTexture();
}


//////////////////////////////////////////////////////////////////////////
void AtlasDynamicObject::clear()
{
    m_mesh.removeVertexBuffer();  
    m_mesh.clearMeshElementList();
    m_mesh.clearMaterialList();
}


//////////////////////////////////////////////////////////////////////////

bbool AtlasDynamicObject::getUVAt( u32 _indexInAtlas , Vec2d* _uvOut)
{
    if (!isValid())
        return bfalse;

    const Texture * t = getAtlasTexture();
    ITF_ASSERT_CRASH(t!=NULL,"No atlas texture");
    const UVAtlas *atlas = t->getUVAtlas();

    if (!atlas)
    {
        f32 halfPixelX = 0.5f/t->getSizeX();
        f32 halfPixelY = 0.5f/t->getSizeY();
        Vec2d uv0(halfPixelX,halfPixelY);
        Vec2d uv1(1-halfPixelX,1-halfPixelY);
        _uvOut[0] = uv0;
        _uvOut[1] = Vec2d(uv0.x(), uv1.y());
        _uvOut[2] = uv1;
        _uvOut[3] = Vec2d(uv1.x(), uv0.y());

    }
    else
    {
        atlas->get4UVAt(_indexInAtlas, _uvOut);
    }

    return btrue;
}

//////////////////////////////////////////////////////////////////////////

void AtlasDynamicObject::addQuad(int _indexInAtlas, const Vec2d& _pt1, const Vec2d& _pt2, const Vec2d& _pt3, const Vec2d& _pt4, f32 _zZoom, u32 _color)
{
    if (m_maxQuad <= m_vertex.size() / 4)
    {
        //ITF_WARNING_CATEGORY(LD, NULL, 0, "You try to draw more quad than the maximum allowed ! ");
        ITF_ASSERT(0);
        return;
    }

    Vec2d uv[4];

    if (!getUVAt(_indexInAtlas, uv))
    {
        return;
    }
  
    VertexPCT* vtx = &m_vertex.incrSize(4);
 
   
    vtx[0].setData(_pt1, uv[0], _color, _zZoom);
    vtx[3].setData(_pt2, uv[1], _color, _zZoom);
    vtx[2].setData(_pt3, uv[2], _color, _zZoom);
    vtx[1].setData(_pt4, uv[3], _color, _zZoom);

}

ITF_VertexBuffer *AtlasDynamicObject::getCurrentVertexBuffer() const
{
    return m_mesh.getCurrentVB();
}

VertexPCT* AtlasDynamicObject::startFillVertexBuffer()
{
    ITF_ASSERT(0 == m_vertex.size());
    createMeshVertexBuffer();

    m_mesh.swapVBForDynamicMesh();
    ITF_VertexBuffer * vertexBuffer = m_mesh.getCurrentVB();

    void* pret;
    GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(vertexBuffer, &pret);
    return (VertexPCT*)pret;
}

void AtlasDynamicObject::endFillVertexBuffer()
{
    ITF_ASSERT(0 == m_vertex.size());
    m_mesh.UnlockVertexBuffer();
}

void AtlasDynamicObject::drawVertexBuffer(const ITF_VECTOR <class View*>& _views, f32 _z, u32 _quadCount, const ConstObjectRef & _objectRef)
{
    ITF_ASSERT(0 == m_vertex.size());

    ITF_MeshElement& elem = m_mesh.getMeshElement();
    elem.m_count = _quadCount* 6;

    GFX_Vector4 vTranslation;
    storeXYZW((float*)&vTranslation, m_mesh.getMatrix().T());
    vTranslation.m_z = _z;
    Float4 f4Translation = loadXYZW((float*)&vTranslation);
    m_mesh.getMatrix().setT(f4Translation);

    m_mesh.setCommonParam(getGfxPrimitiveParam());

    GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_mesh, _z, _objectRef);
}

void AtlasDynamicObject::draw(const ITF_VECTOR <class View*>& _views, f32 _z, f32 _depthOffset, bool _clearAfter, const ConstObjectRef & _objectRef)
{
    u32 count = m_vertex.size();
    if (!count)
        return;

    if (GFX_ADAPTER->isDeviceLost())
    {
        m_vertex.clear();
        return;
    }

    // Lost device ?? then try to recreate VB.
    createMeshVertexBuffer();

    m_mesh.setCommonParam(getGfxPrimitiveParam());

    m_mesh.swapVBForDynamicMesh();

    void *pdata = NULL;
    m_mesh.LockVertexBuffer(&pdata);
    if(pdata)
    {
        ITF_MemcpyWriteCombined(pdata, &m_vertex[0], sizeof(VertexPCT) * count);
        m_mesh.UnlockVertexBuffer();

        ITF_MeshElement& elem = m_mesh.getMeshElement();
        elem.m_count = (count / 4) * 6;
        GFX_Vector4 vTranslation;
        storeXYZW((float*)&vTranslation, m_mesh.getMatrix().T());
        vTranslation.m_z = _z;
        Float4 f4Translation = loadXYZW((float*)&vTranslation);
        m_mesh.getMatrix().setT(f4Translation);

        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_mesh, _z + _depthOffset, _objectRef);
    }

    if(_clearAfter)
    {
        m_vertex.clear();
    }
}

void AtlasDynamicObject::draw2d(const ITF_VECTOR <class View*>& _views, f32 _priority, bool _clearAfter, const ConstObjectRef & _objectRef)
{
    u32 count = m_vertex.size();
    if (!count)
        return;

    if (GFX_ADAPTER->isDeviceLost())
    {
        m_vertex.clear();
        return;
    }

    // Lost device ?? then try to recreate VB.
    createMeshVertexBuffer();

    m_mesh.swapVBForDynamicMesh();

    void *pdata = NULL;
    m_mesh.LockVertexBuffer(&pdata);
    if(pdata)
    {
        ITF_MemcpyWriteCombined(pdata, &m_vertex[0], sizeof(VertexPCT) * count);
        m_mesh.UnlockVertexBuffer();

        ITF_MeshElement& elem = m_mesh.getMeshElement();
        elem.m_count = (count / 4) * 6;
        m_mesh.set2DPosAndScale(baseZ()+baseW()); // pos 0, 0 and scale 1, 1
        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_2D>(_views, &m_mesh, _priority, _objectRef);
    }
    if (_clearAfter)
    {
        m_vertex.clear();
    }
}

bbool AtlasDynamicObject::isValid() const
{
    if(m_mesh.getNbMaterial() == 0)
    {
        return bfalse;
    }

    const Texture * t = getAtlasTexture();

    if (!t || !t->isPhysicallyLoaded())
        return bfalse;

    return btrue;
}

u32 AtlasDynamicObject::getNumIndices()
{
    const Texture* t = getAtlasTexture();

    if (!t || !t->isPhysicallyLoaded())
        return U32_INVALID;


    const UVAtlas *atlas = t->getUVAtlas();
    if (!atlas)
        return 1;

    return atlas->getNumberUV();
}

AtlasDynamicObject::~AtlasDynamicObject()
{
    clear();
}

}

