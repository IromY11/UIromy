#ifndef ITF_ENGINE_MESH_H_
#define ITF_ENGINE_MESH_H_

#ifndef SIMD_MATRIX44_H
#include "core/math/SIMD/SIMD_Matrix44.h"
#endif //SIMD_MATRIX44_H

#ifndef _ITF_GFX_ADAPTER_H_
# include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef ITF_GFX_MATERIAL_H_
#include "engine/display/material/GFXMaterial.h"
#endif //ITF_GFX_MATERIAL_H_
#include "engine/AdaptersInterfaces/GFXAdapter_VertexBufferManager.h"

#ifndef _ITF_GFXADAPTER_VERTEXBUFFERMANAGER_H_
#include "engine/AdaptersInterfaces/GFXAdapter_VertexBufferManager.h"
#endif //_ITF_GFXADAPTER_VERTEXBUFFERMANAGER_H_

#ifndef _ITF_GFX_ADAPTER_H_
# include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

namespace ITF
{

    class ITF_VertexBuffer;
    class ITF_IndexBuffer;

    struct ITF_MeshElement
    {
        ITF_MeshElement()
            :   m_indexBuffer(nullptr)
            ,   m_materialIdx(0)
            ,   m_startVertex(0)
            ,   m_count(0)
        {
        };

        ITF_IndexBuffer*    m_indexBuffer;
        ux                  m_materialIdx; // index of the material in the mesh
        u32                 m_startVertex;
        u32                 m_count;
    };

    enum VBLockType
    {
        // static lock, could not be lock if already sent to gpu. Read/Write access
        vbLockType_static = 0,
        // use many buffers, old data still present according to swap count. May be used when only one part of buffer changed. Read/write access
        vbLockType_dynKeep,
        // may use a ring buffer, old data are lost, need to fill all buffer. Write acces only
        vbLockType_dynDiscard,
        // used for one draw, need to fill all buffer. Write access only.
        vbLockType_dynOnce
    };

    class ITF_Mesh : public GFXPrimitive
    {
    public:
        enum { MaxBufferBuffered = GFX_BUFFERED_FRAME_COUNT };

        ITF_Mesh()
            :m_userBuffer(0)
            ,m_vtxBuffer(NULL)     
            ,m_pMatrixOverlay(NULL)
            ,m_vbLockType(vbLockType_static)
            ,m_posAndScale2D(packXY(splatZero(), splatOne()))
            ,m_rotation2D(0.f)
        {
            for(u32 i = 0; i < MaxBufferBuffered; ++i)
                m_vtxBufferBuffered[i] = NULL;
            m_Matrix.setIdentity();
            cleanPassFilterFlag(); // by default the mesh is fully filtered
        };

        void createVertexBuffer(u32 _vertexNumber, int _vertexformat, u32 _structVertexSize, VBLockType _lockType, dbEnumType _type = VB_T_MESH);
        void removeVertexBuffer(u32 _delayframeCount = 1);
        void LockVertexBuffer( void** _data  );
        void UnlockVertexBuffer();
        void UnlockVertexBufferUsage(u32 _usage);

        virtual void directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z = 0.0f, f32 _ZWorld = 0.0f, f32 _ZAbs = 0.0f);

        void directDraw2D(const class PrimitiveContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter);
        void directDrawRegular(const class PrimitiveContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter);

        ITF_INLINE void                 allocateMatrixOverlay() { m_pMatrixOverlay = newAllocAlign(mId_Frieze, 16, Matrix44);}
        ITF_INLINE void                 deleteMatrixOverlay()   { deleteAlign(m_pMatrixOverlay);m_pMatrixOverlay=NULL;}
        Matrix44*                       getMatrixOverlay()      { return m_pMatrixOverlay;}

        ITF_INLINE bbool                hasMatrixOverlay() {return (m_pMatrixOverlay!=NULL);}

        ITF_INLINE Matrix44&            getMatrix() {return m_Matrix;}
        ITF_INLINE const Matrix44&      getMatrix() const {return m_Matrix;}
        ITF_INLINE void                 setMatrix(const Matrix44& _matrix) {m_Matrix = _matrix;}
                   void                 set2DPosAndScale(const Vec2d & _pos, const Vec2d & _scale);
                   void                 set2DPosAndScale(Float4 SIMD_ARG _pos4, Float4 SIMD_ARG _scale4); // ignore _pos4.zw and_scale4.zw
                   void                 set2DPosAndScale(Float4 SIMD_ARG _posAndScale) { m_posAndScale2D = _posAndScale; }; // pos in xy, scale in zw
                   Float4               get2DPosAndScale() const { return m_posAndScale2D; }

        // Only use if necessary
                   void                 set2DRotation(f32 _rot) { m_rotation2D = _rot; }
                   f32                  get2DRotation() const { return m_rotation2D; }

        bbool                           isDataReady() const;

        void                            initStaticIndexMesh(int _vertexFormat, ITF_IndexBuffer *_staticIndex, u32 _bufferSize = 0, VBLockType _lockType = vbLockType_dynDiscard);
        void                            emptyIndexedMesh();

        // addElementAndMaterial:
        // add a new element and a new material, assign the new element material idx to the new created material, return the element idx
        ux                              addElementAndMaterial();
        ux                              addElementAndMaterial(const GFX_MATERIAL & _material); // same but with explicit material
        ux                              addElement(ux _materialIdx); // directly give the material idx

        ux                              getNbMeshElement() const { return m_ElementList.size(); }
        const ITF_MeshElement  &        getMeshElement(ux _idx = 0) const { return m_ElementList[_idx]; }
        ITF_MeshElement  &              getMeshElement(ux _idx = 0) { return m_ElementList[_idx]; }
        void                            clearMeshElementList() { return m_ElementList.resize(0); }

        bool							isValid() const;

        ux                              getNbMaterial() const { return m_materialList.size(); }
        GFX_MATERIAL &                  getMaterial(ux _idx) { return m_materialList[_idx]; }
        const GFX_MATERIAL &            getMaterial(ux _idx) const { return m_materialList[_idx]; }
        void                            setMaterial(ux _idx, const GFX_MATERIAL & _material) { m_materialList[_idx] = _material; addPassFilterFlag(_material); }
        ux                              addMaterial(const GFX_MATERIAL & _material) { ux nbMat = getNbMaterial();  m_materialList.push_back(_material); addPassFilterFlag(_material); return nbMat; }

        const GFX_MATERIAL &            getMeshElementMaterial(ux _eltIdx = 0) const { return m_materialList[m_ElementList[_eltIdx].m_materialIdx]; }
        GFX_MATERIAL &                  getMeshElementMaterial(ux _eltIdx = 0) { return m_materialList[m_ElementList[_eltIdx].m_materialIdx]; }
        void                            clearMaterialList() { m_materialList.resize(0); cleanPassFilterFlag(); }

        // Update pass filter flag according to the material list. Most of the time, if materials are set using the interface, it's not needed to be called
        void                            updatePassFilterFlag();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// if created a dynamic vertex buffer, alternate vertex buffers. Some implementations may not need it.
        void                            swapVBForDynamicMesh();
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get current VertexBuffer
        ITF_VertexBuffer                *getCurrentVB() const { return m_vtxBuffer; }
        void                            copyVBFromMesh(const ITF_Mesh & _mesh);

        void                              setVertexBuffer(ITF_VertexBuffer* vtxBuffer)  {m_vtxBuffer = vtxBuffer;}

    private:
        vector<ITF_MeshElement>         m_ElementList;
        vector<GFX_MATERIAL>            m_materialList;
        VBLockType                      m_vbLockType;
        ITF_VertexBuffer*               m_vtxBuffer;
        ITF_VertexBuffer*               m_vtxBufferBuffered[MaxBufferBuffered];
        u32                             m_userBuffer;

        Matrix44*                       m_pMatrixOverlay;
        Matrix44                        m_Matrix;
        Float4                          m_posAndScale2D;
        f32                             m_rotation2D;
    };

	// Mesh inline implementation
	ITF_INLINE void ITF_Mesh::swapVBForDynamicMesh()
	{
		if(m_vtxBufferBuffered[0])
		{
			m_userBuffer = (m_userBuffer+1)%MaxBufferBuffered;
			ITF_ASSERT(m_vtxBufferBuffered[m_userBuffer]);
			m_vtxBuffer = m_vtxBufferBuffered[m_userBuffer];
		}
	}

	ITF_INLINE bool ITF_Mesh::isValid() const
	{
		return (m_vtxBuffer != NULL);
	}

	ITF_INLINE void ITF_Mesh::LockVertexBuffer( void** _data )
	{
		GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(getCurrentVB(), _data);
	}

	ITF_INLINE void ITF_Mesh::UnlockVertexBuffer()
	{
		GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(getCurrentVB());
	}

	ITF_INLINE void ITF_Mesh::UnlockVertexBufferUsage( u32 _usage )
	{
		GFX_ADAPTER->getVertexBufferManager().UnlockVertexBufferUsage(getCurrentVB(), _usage);
	}

    class ITF_ClippingMesh : public ITF_Mesh
    {
    public:
        ITF_ClippingMesh()
            : ITF_Mesh()
            , m_clippingZ(0.f)
        {
            m_clippingAABB.invalidate();
        }

        void            setClipping(const AABB & _clipping, f32 _z) { m_clippingAABB = _clipping; m_clippingZ = _z; }
        const AABB &    getClippingAABB() const { return m_clippingAABB; }
        f32             getClippingZ() const { return m_clippingZ; }

        virtual void directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z = 0.0f, f32 _ZWorld = 0.0f, f32 _ZAbs = 0.0f);

    protected:
        AABB        m_clippingAABB;
        f32         m_clippingZ;
    };


}

#endif // ITF_ENGINE_MESH_H_
