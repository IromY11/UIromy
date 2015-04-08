#include "precompiled_engine.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef ITF_ENGINE_MESH_H_
#include "engine/display/Primitives/Mesh.h"
#endif //ITF_ENGINE_MESH_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef ITF_GFX_CONTEXT_H_
#include "engine/display/GFXContext.h"
#endif //ITF_GFX_CONTEXT_H_

namespace ITF 
{

ux ITF_Mesh::addElementAndMaterial()
{
    ux nbMaterial = m_materialList.size();
    m_materialList.resize(nbMaterial + 1u);
    // updatePassFilterFlag(m_materialList.back()); -> not done here, caller is supposed to set a material at a point
    ux newMaterialIdx = nbMaterial;

    ux nbElement = m_ElementList.size();
    m_ElementList.resize(nbElement + 1u);
    m_ElementList.back().m_materialIdx = newMaterialIdx;
    return nbElement;
}

ux ITF_Mesh::addElementAndMaterial(const GFX_MATERIAL & _material)
{
    addPassFilterFlag(_material);

    ux newMaterialIdx = m_materialList.size();
    m_materialList.push_back(_material);

    ux nbElement = m_ElementList.size();
    m_ElementList.resize(nbElement + 1u);
    m_ElementList.back().m_materialIdx = newMaterialIdx;
    return nbElement;
}

ux ITF_Mesh::addElement(ux _materialIdx) // directly give the material idx, caller is supposed to add the corresponding material
{
    ux nbElement = m_ElementList.size();
    m_ElementList.resize(nbElement+1u);
    m_ElementList.back().m_materialIdx = _materialIdx;
    return nbElement;
}

void ITF_Mesh::createVertexBuffer(u32 _vertexNumber, int _vertexformat, u32 _structVertexSize, VBLockType _lockType, dbEnumType _type)
{
    bbool bDyn = _lockType >= vbLockType_dynDiscard;

    m_vtxBuffer = GFX_ADAPTER->createVertexBuffer(_vertexNumber, _vertexformat, _structVertexSize, bDyn, _type);
    ITF_ASSERT(m_vtxBuffer);

    bool allocSwapBuffers;
#ifndef VBMANAGER_USE_DYNAMICRING_VB
    switch( _lockType )
    {
    case vbLockType_static:
    case vbLockType_dynOnce:
        allocSwapBuffers = false;
        break;
    default:
        allocSwapBuffers = true;
        break;
    }
#else
    switch( _lockType )
    {
    case vbLockType_static:
    case vbLockType_dynOnce:
    case vbLockType_dynDiscard:
        allocSwapBuffers = false;
        break;
    default:
        allocSwapBuffers = true;
        break;
    }
#endif

    if(allocSwapBuffers)
    {
        m_vtxBufferBuffered[0] = m_vtxBuffer;
        for(u32 i = 1; i < MaxBufferBuffered; ++i)
        {
            m_vtxBufferBuffered[i] = GFX_ADAPTER->createVertexBuffer(_vertexNumber, _vertexformat, _structVertexSize, bDyn, _type);
            ITF_ASSERT(m_vtxBufferBuffered[i]);
        }
    }
}

void ITF_Mesh::removeVertexBuffer(u32 _delayframeCount)
{
    if(!m_vtxBuffer)
        return;

    if(m_vtxBufferBuffered[0])
    {
        _delayframeCount += MaxBufferBuffered-1; // avoid destruction of vb used by gpu in previous frames.
        for(u32 i = 0; i < MaxBufferBuffered; ++i)
        {
            if(_delayframeCount)
                GFX_ADAPTER->removeVertexBufferDelay(m_vtxBufferBuffered[i], _delayframeCount);
            else
                GFX_ADAPTER->removeVertexBuffer(m_vtxBufferBuffered[i]);
            m_vtxBufferBuffered[i] = NULL;
        }
    }
    else
    {
        if(_delayframeCount)
            GFX_ADAPTER->removeVertexBufferDelay(m_vtxBuffer, _delayframeCount);
        else
            GFX_ADAPTER->removeVertexBuffer(m_vtxBuffer);
    }

    m_vtxBuffer = NULL;
}

void ITF_Mesh::initStaticIndexMesh(int _vertexFormat, ITF_IndexBuffer *_staticIndex, u32 _bufferSize, VBLockType _lockType)
{
    if (!_bufferSize) _bufferSize = MAX_VERTEX_BY_MESH;

    switch(_vertexFormat)
    {
    case VertexFormat_PCT:
        createVertexBuffer(_bufferSize, VertexFormat_PCT, sizeof(VertexPCT), _lockType);
        break;

    default: // VertexFormat_PTa
        createVertexBuffer(_bufferSize, VertexFormat_PTa, sizeof(VertexPT), _lockType);
        break;
    }

    addElementAndMaterial();
    m_ElementList[0].m_indexBuffer = _staticIndex;
}

void ITF_Mesh::emptyIndexedMesh()
{
    removeVertexBuffer();
}

bbool ITF_Mesh::isDataReady() const
{
    for (u32 i = 0; i < m_ElementList.size(); i++)
    {
        if (m_ElementList[i].m_materialIdx >= m_materialList.size() 
            || !getMeshElementMaterial(i).hasResourceLoaded())
        {
            return bfalse;
        }
    }
    return btrue;
}

void ITF_Mesh::copyVBFromMesh(const ITF_Mesh & _mesh)
{
    m_vtxBuffer = _mesh.m_vtxBuffer;
    ITF_MEMCOPY(m_vtxBufferBuffered, _mesh.m_vtxBufferBuffered, sizeof(m_vtxBufferBuffered));
}

void ITF_Mesh::directDraw(const RenderPassContext & _rdrPassCtxt, GFXAdapter * _gfxDeviceAdapter, f32 _Z, f32 _ZWorld, f32 _ZAbs)
{
    PRF_M_SCOPE(drawMesh);

    PrimitiveContext primCtx;
    primCtx.setRenderPassContext((RenderPassContext*)&_rdrPassCtxt)
        .setPrimitiveParam(getCommonParam())
        .setCamZ(_Z,_ZWorld, _ZAbs);

    if (_rdrPassCtxt.getPass() == GFX_ZLIST_PASS_2D)
    {
        directDraw2D(primCtx, _gfxDeviceAdapter);
    }
    else
    {
        directDrawRegular(primCtx, _gfxDeviceAdapter);
    }
}

void ITF_Mesh::directDraw2D(const PrimitiveContext & _primCtxt, GFXAdapter * _gfxDeviceAdapter)
{
    Float4 widthHeight = _primCtxt.getRenderPassContext()->getRenderTargetSize();
    Float4 trans2D = packXY(m_posAndScale2D, baseY()); // build x,y,0,1

    // we need to invert y : (height - trans2D.y)
    Float4 subHT = widthHeight - trans2D;
    Float4 trans2DYInverted = shuffle<0, 5, 2, 3>(trans2D, subHT); // take Y on subHT

    // now invert also Y scale
    Float4 scale2D = rot2(m_posAndScale2D); // ZW -> XY
    Float4 scale2DYNeg = shuffle<0, 5, 2, 3>(scale2D, -scale2D); // take Y on -scale2D


    // update the matrix
    m_Matrix.setIJK(scale2DYNeg * baseX(), scale2DYNeg * baseY(), baseZ());
    if (m_rotation2D)
    {
        Matrix44 rotMatrix;
        rotMatrix.setRotationZ(m_rotation2D);

        m_Matrix.mul33_inline(m_Matrix, rotMatrix);
    }
    m_Matrix.setTranslation(trans2DYInverted);

    _gfxDeviceAdapter->setGFXPrimitiveParameters(getCommonParam(), *_primCtxt.getRenderPassContext());
    _gfxDeviceAdapter->drawMesh(_primCtxt, *this);
}

void ITF_Mesh::directDrawRegular(const PrimitiveContext & _primCtxt, GFXAdapter * _gfxDeviceAdapter)
{
    _gfxDeviceAdapter->setGFXPrimitiveParameters(getCommonParam(), *_primCtxt.getRenderPassContext());  
    _gfxDeviceAdapter->drawMesh(_primCtxt, *this);
}


void ITF_Mesh::set2DPosAndScale(const Vec2d & _pos, const Vec2d & _scale)
{
    m_posAndScale2D = loadXYZW(_pos.x(), _pos.y(), _scale.x(), _scale.y());
}

void ITF_Mesh::set2DPosAndScale(Float4 SIMD_ARG _pos4, Float4 SIMD_ARG _scale4)
{
    m_posAndScale2D = packXY(_pos4, _scale4);
}

// update pass filter flag according to the material list
void ITF_Mesh::updatePassFilterFlag()  
{
    cleanPassFilterFlag();
    for(vector<GFX_MATERIAL>::const_iterator it = m_materialList.begin(), itEnd = m_materialList.end();
        it!=itEnd;
        ++it)
    {
        addPassFilterFlag(*it);
    }
}

void ITF_ClippingMesh::directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z, f32 _ZWorld, f32 _ZAbs)
{
    PRF_M_SCOPE(drawClippingMesh);

    if (m_clippingAABB.isValid())
    {
        Vec3d       projected;
        i32 px, py;
        GFX_RECT    rcClipping;

        _gfxDeviceAdapter->compute3DTo2D(m_clippingAABB.getMin().to3d(m_clippingZ), projected);
        px                  = (i32)projected.x();
        py                  = (i32)projected.y();
        rcClipping.left      = px;
        rcClipping.right     = px;
        rcClipping.top       = py;
        rcClipping.bottom    = py;

        _gfxDeviceAdapter->compute3DTo2D(m_clippingAABB.getMax().to3d(m_clippingZ), projected);
        px                  = (i32)projected.x();
        py                  = (i32)projected.y();
        if (rcClipping.left  > px) rcClipping.left    = px;
        if (rcClipping.right < px) rcClipping.right   = px;
        if (rcClipping.top   > py) rcClipping.top     = py;
        if (rcClipping.bottom< py) rcClipping.bottom  = py;

        _gfxDeviceAdapter->compute3DTo2D(m_clippingAABB.getMinXMaxY().to3d(m_clippingZ), projected);
        px                  = (i32)projected.x();
        py                  = (i32)projected.y();
        if (rcClipping.left  > px) rcClipping.left    = px;
        if (rcClipping.right < px) rcClipping.right   = px;
        if (rcClipping.top   > py) rcClipping.top     = py;
        if (rcClipping.bottom< py) rcClipping.bottom  = py;

        _gfxDeviceAdapter->compute3DTo2D(m_clippingAABB.getMaxXMinY().to3d(m_clippingZ), projected);
        px                  = (i32)projected.x();
        py                  = (i32)projected.y();
        if (rcClipping.left  > px) rcClipping.left    = px;
        if (rcClipping.right < px) rcClipping.right   = px;
        if (rcClipping.top   > py) rcClipping.top     = py;
        if (rcClipping.bottom< py) rcClipping.bottom  = py;
        GFX_Viewport viewport;
        _gfxDeviceAdapter->getViewport(viewport);

        rcClipping.left   = Clamp(rcClipping.left,  i32(viewport.m_x), i32(viewport.m_x+viewport.m_width));
        rcClipping.right  = Clamp(rcClipping.right, i32(viewport.m_x), i32(viewport.m_x+viewport.m_width));
        rcClipping.top    = Clamp(rcClipping.top,   i32(viewport.m_y), i32(viewport.m_y+viewport.m_height));
        rcClipping.bottom = Clamp(rcClipping.bottom,i32(viewport.m_y), i32(viewport.m_y+viewport.m_height));

        _gfxDeviceAdapter->setScissorRect(&rcClipping);

    }

    ITF_Mesh::directDraw(_rdrPassCtxt, _gfxDeviceAdapter, _Z, _ZWorld, _ZAbs);

    if (m_clippingAABB.isValid())
    {
        _gfxDeviceAdapter->setScissorRect(NULL);
    }
}


} // ITF

