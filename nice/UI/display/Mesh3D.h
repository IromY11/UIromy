#ifndef _ITF_MESH3D_H_
#define _ITF_MESH3D_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#if defined(ITF_WINDOWS) || defined(ITF_CAFE) || defined(ITF_X360) || defined(ITF_PS3) || defined(ITF_DURANGO) || defined(ITF_ORBIS)
#define HARDWARESKINNING
#endif

#ifndef _ITF_AABB3D_H_
#include "core/boundingvolume/AABB3d.h"
#endif //_ITF_AABB3D_H_

namespace ITF
{
	inline void vec3CompressNormal(u32 *vecc, const Vec3d *pSRC)
	{
		u32 nX, nY, nZ;

		nX = (u32) ((ITF::Min(1.0f, ITF::Max(-1.0f, pSRC->x()))) * 127.0f + 128.0f);
		nY = (u32) ((ITF::Min(1.0f, ITF::Max(-1.0f, pSRC->y()))) * 127.0f + 128.0f);
		nZ = (u32) ((ITF::Min(1.0f, ITF::Max(-1.0f, pSRC->z()))) * 127.0f + 128.0f);
		*vecc = (nX << 0) | (nY << 8) | (nZ << 16);
	}

	inline void vec3UncompressNormal(Vec3d *vecc, u32 vec)
	{
		vecc->setX( ((vec) & 0xff) / 128.0f - 1.0f );
		vecc->setY( ((vec >> 8) & 0xff) / 128.0f - 1.0f );
		vecc->setZ( ((vec >> 16) & 0xff) / 128.0f - 1.0f );
	}


    ///////////////////////////////////////////////////////////////////////////////////////////////
    class ITF_Mesh;
    class Skeleton3D;
    class Mesh3DResource;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Mesh3D class
    // 
    ///////////////////////////////////////////////////////////////////////////////////////////////
    class Mesh3D
    {
    public:
        ///// structures --------------------------------------------------------------------------
        // one indexed triangle
        struct Triangle
        {
            u32 VBIndex[3];
        };
        // one element : material + list of triangles using this material
        struct Element
        {
            i32								m_MaterialID;
            GFX_MATERIAL					m_Material;
            ITF_VECTOR<Mesh3D::Triangle>	m_ListTriangles;
        };
        // one skin element : matrix and list of point affected by
        struct IndexWeight
        {
            f32 m_Weight;
            u32 m_Index;
        };
        /////
        struct SkinElementNeighbor
        {
            u32     m_index;
            u32     m_numPoint;
        };
        /////
        struct SkinElement
        {
            StringID	                    m_MatrixName;
            u32						        m_MatrixIndex;
            Matrix44				        m_MatrixFlash;
            Matrix44				        m_MatrixFlashInvert;
            ITF_VECTOR<IndexWeight>         m_Point;
            ITF_VECTOR<SkinElementNeighbor> m_Neighbor;
            u32                             m_Flags;
        };
        ///// 
        struct SkinVertex
        {
            u32 Index;
            ITF_VECTOR<IndexWeight> m_SkinElements;
        };
        ///// 
        struct SkinVertexOneMatrix
        {
            u32 Index;
            u32 IndexMatrix;
        };
        ///// 
        struct SkinVertexFourMatrices
        {
            u32             Index;
            u32             Number;
            IndexWeight     Skin[4];
        };

        // one unique vertex/UV
        struct UniqueVertex
        {
            Vec3d Position;
            u32	  Normal;
            u32	  Tangent;
            Vec2d UV;
            u32	  color;
        };


        ///// constructor/destructor --------------------------------------------------------------
        Mesh3D() { m_CurListUniqueVertex = NULL; m_BVComputed = bfalse; m_SkinMaxMatrixIndex = 0; };
        ~Mesh3D() {};

        ///// save/load ---------------------------------------------------------------------------
        void			serialize(ArchiveMemory& _Archive, bbool _compressedData);
        bbool			openBinary(const Path& _cookFile, bbool _castWarning = btrue, bbool _compressedData= bfalse);
        bbool			saveBinary(const Path& _cookFile, bbool _compressedData= bfalse);
        void			link() {}

        ///// mesh specific functions -------------------------------------------------------------
        void			MeshGenerate(Mesh3DResource* pMesh3DResource,ITF_Mesh &_mesh, bbool _skinningSoft = bfalse );
        static void		MeshDestroyContent( ITF_Mesh &_mesh );
        void            MeshComputeVertexBuffer(Mesh3DResource* pMesh3DResource, ITF_Mesh &_mesh );

        ///// skinning specific functions -------------------------------------------------------------
        void            MeshComputeSkinning_PerElementToPerVertex( ITF_VECTOR<SkinVertex> &_skinVertex );
        void            MeshComputeSkinning_3BonesMax( ITF_VECTOR<SkinVertex> &_skinVertex );
        void            MeshComputeSkinning_RemoveLowWeight( ITF_VECTOR<SkinVertex> &_skinVertex, f32 _thresh );
        void            MeshComputeSkinning_Normalize( ITF_VECTOR<SkinVertex> &_skinVertex );
        bbool           MeshComputeSkinning_ReduceBones(ITF_VECTOR<SkinVertex> &_skinVertex, u32 _maxBones, u32 _loop = 0);
        void            MeshComputeSkinning_Store(ITF_VECTOR<SkinVertex> &_skinVertex);
        void            MeshComputeSkinning();

        ///// draw functions ----------------------------------------------------------------------
        void			DrawNormals( const Matrix44	&M);
        void            DrawBV( const Matrix44 &M);
                
        ///// BV functions ------------------------------------------------------------------------
        void			BVCompute();
        void			BVtoAABB(AABB &_aabb, const Matrix44 &M );
        void			BVtoAABB3d(AABB3d &_aabb3d, const Matrix44 &M );
        Vec3d&          getBVMin();
        Vec3d&          getBVMax();

        bbool           needSkinning()
        {
            return (m_SkinElements.size() != 0);
        }
    public:
        ///// mesh data ---------------------------------------------------------------------------
        ITF_VECTOR<Element>				m_ListElement;
        ITF_VECTOR<UniqueVertex>        m_ListUniqueVertex;

        ///// skin data ---------------------------------------------------------------------------
        ITF_VECTOR<SkinElement>			    m_SkinElements;
        ITF_VECTOR<SkinVertexFourMatrices>  m_SkinVertex;
        ITF_VECTOR<SkinVertexOneMatrix>     m_SkinVertexOneMatrix;
        ITF_VECTOR<u32>                     m_SkinToGizmo;
        u32                                 m_SkinMaxMatrixIndex;


        ITF_VECTOR<UniqueVertex>        m_ListUniqueVertexSkinned;
        ITF_VECTOR<UniqueVertex>       *m_CurListUniqueVertex;

    private:	

        ///// Bounding volume data ----------------------------------------------------------------
        bbool							m_BVComputed;
        Vec3d							m_BVMin;
        Vec3d							m_BVMax;
    };



} // namespace ITF

#endif // _ITF_MESH3D_H_
