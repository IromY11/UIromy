#include "precompiled_engine.h"

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_MESH3D_H_
#include    "engine/display/Mesh3D.h"
#endif // _ITF_MESH3D_H_

#ifndef _ITF_PREFETCH_FCT_H_
#include "engine/boot/PrefetchFct.h"
#endif  //_ITF_PREFETCH_FCT_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifdef ITF_SUPPORT_COOKING
#ifndef _ITF_MESH3DCOOKER_H_
#include    "tools/plugins/CookerPlugin/Mesh3DCooker.h"
#endif // _ITF_MESH3DCOOKER_H_
#endif

#ifndef _ITF_FILE_H_
# include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef ITF_ENGINE_MESH_H_
# include "engine/display/Primitives/Mesh.h"
#endif //ITF_ENGINE_MESH_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Mesh3DComponent class
    // 
    ///////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Mesh3D class
    // 
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // load from binary data
    //
    bbool Mesh3D::openBinary(const Path& _cookFile, bbool _castWarning, bbool _compressedData )
    {
        if (!_castWarning &&
            !FILEMANAGER->fileExists(_cookFile))
            return bfalse;

        File*  binaryFile = FILEMANAGER->openFile(_cookFile ,ITF_FILE_ATTR_READ);
        if (binaryFile)
        {
            u32 lenght = 0;

            lenght = (u32) binaryFile->getLength();
            ITF_ASSERT(lenght);
            if (lenght == 0)
            {
                FILEMANAGER->closeFile(binaryFile);
                return bfalse;
            }

            u32 length = (u32) binaryFile->getLength();

            ArchiveMemory rd(length,length,btrue);
            binaryFile->read(rd.getPtrForWrite(),length);
            rd.rewindForReading();
            FILEMANAGER->closeFile(binaryFile);

            rd.createLinker();
            serialize(rd,_compressedData);
            link();

            return btrue;
        }

        return bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // save binary data
    //
    bbool Mesh3D::saveBinary(const Path& _cookFile , bbool _compressedData)
    {
        ArchiveMemory amw;
        amw.createLinker();
        serialize(amw,_compressedData);

        File* writeBinary = FILEMANAGER->openFile(_cookFile,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

        ITF_WARNING(NULL, amw.getSize(), "Trying to save %s with corrupted data !", _cookFile.toString8().cStr() );
        if (writeBinary)
        {
            writeBinary->write(amw.getData(),amw.getSize());
            writeBinary->flush();
            FILEMANAGER->closeFile(writeBinary);
            return btrue;
        }

        ITF_ASSERT(0);
        return bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // serialize
    //
    void Mesh3D::serialize(ArchiveMemory& _Archive, bbool )
    {
        u32 size, size2;

        //m_archiveLinker = _pArchive.getLinker();

        ITF_VERIFY(Versioning::serializeVersion(_Archive,Versioning::mesh3DVer()));

        _Archive.registerLink(this);

        // Serialize list of element
        if (_Archive.isReading())
        {
            _Archive.serialize(size);
            m_ListElement.resize(size);
        } 
        else
        {
            size = m_ListElement.size();
            _Archive.serialize(size);
        }
        for (u32 i=0; i < size; ++i)
        {
            _Archive.serialize(m_ListElement[i].m_MaterialID);
            if (_Archive.isReading())
            {
                _Archive.serialize(size2);
                m_ListElement[i].m_ListTriangles.resize(size2);
            } 
            else
            {
                size2 = m_ListElement[i].m_ListTriangles.size();
                _Archive.serialize(size2);
            }
            for (u32 j=0; j < size2; ++j)
            {
                _Archive.serialize( m_ListElement[i].m_ListTriangles[j].VBIndex[0]);
                _Archive.serialize( m_ListElement[i].m_ListTriangles[j].VBIndex[1]);
                _Archive.serialize( m_ListElement[i].m_ListTriangles[j].VBIndex[2]);
            }
        }

        // Serialize list of VertexBufferItems
        if (_Archive.isReading())
        {
            _Archive.serialize(size);
            m_ListUniqueVertex.resize(size);
        } 
        else
        {
            size = m_ListUniqueVertex.size();
            _Archive.serialize(size);
        }
        for (u32 i=0; i < size; ++i)
        {
            m_ListUniqueVertex[i].Position.serialize(_Archive);
            _Archive.serialize( m_ListUniqueVertex[i].Normal );
            _Archive.serialize( m_ListUniqueVertex[i].Tangent );
            m_ListUniqueVertex[i].UV.serialize(_Archive);
            _Archive.serialize( m_ListUniqueVertex[i].color);
        }

        // Serialize skinning
        if (_Archive.isReading())
        {
            _Archive.serialize(size);
            m_SkinElements.resize(size);
        } 
        else
        {
            size = m_SkinElements.size();
            _Archive.serialize(size);
        }
        for (u32 i=0; i < size; ++i)
        {
            _Archive.serialize(m_SkinElements[i].m_MatrixIndex);
            m_SkinElements[i].m_MatrixName.serialize(_Archive);

            m_SkinElements[i].m_MatrixFlash.serialize(_Archive);
            if (_Archive.isReading())
                m_SkinElements[i].m_MatrixFlashInvert.inverse44(m_SkinElements[i].m_MatrixFlash);

            if (_Archive.isReading())
            {
                _Archive.serialize(size2);
                m_SkinElements[i].m_Point.resize(size2);
            } 
            else
            {
                size2 = m_SkinElements[i].m_Point.size();
                _Archive.serialize(size2);
            }
            for (u32 j=0; j < size2; ++j)
            {
                _Archive.serialize( m_SkinElements[i].m_Point[j].m_Index);
                _Archive.serialize( m_SkinElements[i].m_Point[j].m_Weight);
            }
        }

        if (_Archive.isReading() && m_SkinElements.size() && m_ListUniqueVertex.size() )
            MeshComputeSkinning();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // destroy content
    //
    void Mesh3D::MeshDestroyContent( ITF_Mesh &_mesh )
    {
        _mesh.setVertexBuffer(NULL);
        _mesh.clearMeshElementList();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // mesh generation / update
    ///////////////////////////////////////////////////////////////////////////////////////////////
    

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Generate mesh (static or dynamic depending on   variable)
    //
    void Mesh3D::MeshGenerate(Mesh3DResource* pMesh3DResource,ITF_Mesh &_mesh, bbool _skinningSoft )
    {
        MeshComputeVertexBuffer(pMesh3DResource,_mesh);

        m_CurListUniqueVertex = &m_ListUniqueVertex;
            }


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Compute BV
    //
    void Mesh3D::BVCompute()
    {
        if (m_BVComputed) return;
        m_BVComputed = btrue;

        m_BVMin = Vec3d::Infinity;
        m_BVMax = -m_BVMin;

        for (u32 i = 0; i < m_ListUniqueVertex.size(); i++)
        {
            if ( m_ListUniqueVertex[i].Position.x() < m_BVMin.x() ) m_BVMin.x() = m_ListUniqueVertex[i].Position.x();
            if ( m_ListUniqueVertex[i].Position.y() < m_BVMin.y() ) m_BVMin.y() = m_ListUniqueVertex[i].Position.y();
            if ( m_ListUniqueVertex[i].Position.z() < m_BVMin.z() ) m_BVMin.z() = m_ListUniqueVertex[i].Position.z();
            if ( m_ListUniqueVertex[i].Position.x() > m_BVMax.x() ) m_BVMax.x() = m_ListUniqueVertex[i].Position.x();
            if ( m_ListUniqueVertex[i].Position.y() > m_BVMax.y() ) m_BVMax.y() = m_ListUniqueVertex[i].Position.y();
            if ( m_ListUniqueVertex[i].Position.z() > m_BVMax.z() ) m_BVMax.z() = m_ListUniqueVertex[i].Position.z();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Try to move 3D BV to 2D AABB (do not work properly)
    //
    void Mesh3D::BVtoAABB( AABB &_aabb, const Matrix44 &M )
    {
        //AABB aabb( Vec2d::Infinity, -Vec2d::Infinity);
        _aabb.invalidate();

        /*
        Matrix44 m44, m44view, m44final;
        const Matrix44* mview = GFX_ADAPTER->getViewMatrix();
        mview->copyToMatrix44( m44view );
        _gm44.copyToMatrix44( m44 );
        m44final.mul44(m44, m44view);
        */


        for (u32 i = 0; i < 8; i++ )
        {
            Vec3d Vin, Vout, A, B, C;

            Vin.x() = (i & 1) ? m_BVMax.x() : m_BVMin.x();
            Vin.y() = (i & 2) ? m_BVMax.y() : m_BVMin.y();
            Vin.z() = (i & 4) ? m_BVMax.z() : m_BVMin.z();

            //m44final.transformPoint( Vout, Vin);
            M.transformPoint( Vout, Vin);

            GFX_ADAPTER->compute3DTo2D( Vout, A );
            float temp[3];
            storeXYZ(temp,M.T());
            Vout.z() = temp[2]; 
            GFX_ADAPTER->compute3DTo2D( Vout, B );
            A.z() = B.z();
            GFX_ADAPTER->compute2DTo3D(A, B);
            
            _aabb.grow( B );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // build 3D aabb
    //
    void Mesh3D::BVtoAABB3d( AABB3d &_aabb3d, const Matrix44 &M )
    {
        _aabb3d.invalidate();

        for (u32 i = 0; i < 8; i++ )
        {
            Vec3d Vin, Vout, A, B, C;

            Vin.x() = (i & 1) ? m_BVMax.x() : m_BVMin.x();
            Vin.y() = (i & 2) ? m_BVMax.y() : m_BVMin.y();
            Vin.z() = (i & 4) ? m_BVMax.z() : m_BVMin.z();

            M.transformPoint( Vout, Vin);
            _aabb3d.grow( Vout );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // get the BV
    //
    Vec3d& Mesh3D::getBVMin()
    {
        return m_BVMin;
    }

    Vec3d& Mesh3D::getBVMax()
    {
        return m_BVMax;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Try to move 3D BV to 2D AABB (do not work properly)
    //
    void Mesh3D::DrawNormals( const Matrix44 &M)
    {
#ifdef ITF_SUPPORT_DEBUGFEATURE
        static u32 step = 4;        // Just draw a subset of the normals, for perf.
        static f32 length = 0.1f;
        Vec3d A, B;
        
        for (u32 v = 0; v < m_CurListUniqueVertex->size(); v += step)
        {
            const UniqueVertex& vertex = (*m_CurListUniqueVertex)[v];
			Vec3d normal;
			vec3UncompressNormal(&normal, vertex.Normal);
            M.transformPoint(A, vertex.Position);
            M.transformPoint(B, vertex.Position + normal * length);
            GFX_ADAPTER->drawDBG3DLine(A, B);
            }
#endif
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Try to move 3D BV to 2D AABB (do not work properly)
    //
    void Mesh3D::DrawBV( const Matrix44 &M)
    {
#ifdef ITF_SUPPORT_DEBUGFEATURE
        Vec3d		A, Pt[8];

        for (u32 i = 0; i < 8; i++ )
        {
            A.x() = (i & 1) ? m_BVMax.x() : m_BVMin.x();
            A.y() = (i & 2) ? m_BVMax.y() : m_BVMin.y();
            A.z() = (i & 4) ? m_BVMax.z() : m_BVMin.z();

            M.transformPoint(Pt[i], A );
        }

        GFX_ADAPTER->drawDBG3DLine( Pt[0], Pt[1], 0,1,0 );
        GFX_ADAPTER->drawDBG3DLine( Pt[1], Pt[3], 0,1,0 );
        GFX_ADAPTER->drawDBG3DLine( Pt[3], Pt[2], 0,1,0 );
        GFX_ADAPTER->drawDBG3DLine( Pt[2], Pt[0], 0,1,0 );
        GFX_ADAPTER->drawDBG3DLine( Pt[0], Pt[4], 0,1,0 );
        GFX_ADAPTER->drawDBG3DLine( Pt[1], Pt[5], 0,1,0 );
        GFX_ADAPTER->drawDBG3DLine( Pt[2], Pt[6], 0,1,0 );
        GFX_ADAPTER->drawDBG3DLine( Pt[3], Pt[7], 0,1,0 );
        GFX_ADAPTER->drawDBG3DLine( Pt[4], Pt[5], 0,1,0 );
        GFX_ADAPTER->drawDBG3DLine( Pt[5], Pt[7], 0,1,0 );
        GFX_ADAPTER->drawDBG3DLine( Pt[7], Pt[6], 0,1,0 );
        GFX_ADAPTER->drawDBG3DLine( Pt[6], Pt[4], 0,1,0 );
#endif
    }

} // namespace ITF

