#include "precompiled_engine.h"

#ifndef _ITF_MESH3DRESOURCE_H_
#include "engine/display/Mesh3DResource.h"
#endif //_ITF_MESH3DRESOURCE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_MESH3D_H_
#include    "engine/display/Mesh3D.h"
#endif // _ITF_MESH3D_H_

#ifndef _ITF_PREFETCH_FCT_H_
#include "engine/boot/PrefetchFct.h"
#endif  //_ITF_PREFETCH_FCT_H_

#ifndef ITF_ENGINE_MESH_H_
# include "engine/display/Primitives/Mesh.h"
#endif //ITF_ENGINE_MESH_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(Mesh3DResource)

#ifdef ITF_SUPPORT_COOKING
    Mesh3DResource::cookFileCallback	Mesh3DResource::m_cookCallback = NULL;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
bool PrefetchMesh3D( const Path& path, PrefetchFactory::Params& params )
{
    return btrue;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Constructor
//
Mesh3DResource::Mesh3DResource(const Path &path) : Resource(path),  m_mesh3D(0), m_canLoadBin(btrue), m_archiveLinker(NULL)
{
    m_vertexBuffer = NULL;
#ifdef ENABLE_RESOURCE_HISTORY
    ResourceHistory record;
    record.m_opType = ResourceHistoryOp_CreateResource;
    record.m_opTime = CURRENTFRAME;
    record.m_resourceRef = getRef();
    record.m_path = path.cStr();
    ResourceHistoryManager::instance()->addResourceHistory(record);
#endif // ENABLE_RESOURCE_HISTORY
}

///////////////////////////////////////////////////////////////////////////////////////////////
// destructor
//
Mesh3DResource::~Mesh3DResource()
{
#ifdef ENABLE_RESOURCE_HISTORY
    ResourceHistory record;
    record.m_opType = ResourceHistoryOp_DeleteResource;
    record.m_opTime = CURRENTFRAME;
    record.m_resourceRef = getRef();
    record.m_path = m_path.getString().cStr();
    ResourceHistoryManager::instance()->addResourceHistory(record);
#endif // ENABLE_RESOURCE_HISTORY
    flushPhysicalData();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// flush data
//
void Mesh3DResource::flushPhysicalData()
{
    for (ITF_VECTOR<ITF_IndexBuffer*>::iterator iter =  m_indexBuffer.begin();iter!=m_indexBuffer.end();iter++)
    {
        if( ITF_IndexBuffer* pIndexBuffer = *iter ) 
            GFX_ADAPTER->removeIndexBufferDelay( pIndexBuffer, 3);
    }

    m_indexBuffer.clear();

    if( m_vertexBuffer ) 
    {
        GFX_ADAPTER->removeVertexBufferDelay( m_vertexBuffer, 3);
        m_vertexBuffer = NULL;
    }

    SF_DEL(m_mesh3D);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// loading functions
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
bbool Mesh3DResource::tryLoadFile()
{
    ITF_ASSERT_MSG(!isPhysicallyLoaded(), "duplicate resource loading can lead to crashes");
    return tryLoadFileByPlatform(FILESERVER->getPlatform());
}

///////////////////////////////////////////////////////////////////////////////////////////////
bbool Mesh3DResource::tryLoadFileByPlatform( const Platform& platform)
{
    ITF_ASSERT(!isPhysicallyLoaded());

    //Path cookFile = FILESERVER->getCookedNamePlatform(getPath(), platform);
    loadFile( getPath(), platform, bfalse, btrue );
    postProcess(getPath(),platform,bfalse);
    return btrue;
}

///////////////////////////////////////////////////////////////////////////////////////////////
bbool Mesh3DResource::loadFile(const Path& _path, const Platform& _szPlatform, bbool _tryLoadingBin, bbool _raiseError )
{
    Path cookFile = FILESERVER->getCookedNamePlatform(_path,_szPlatform);

#ifdef ITF_SUPPORT_COOKING
    // cook ??
    if ( m_cookCallback && mustBeCooked() && !fileCookAvailable() && RESOURCE_MANAGER->cookNeeded(getPath()) )
        //Mesh3DCooker::cookFile( _szPlatform, _path);
        m_cookCallback( _szPlatform, _path );
#endif

    m_mesh3D = newAlloc(mId_Mesh,Mesh3D());
    if (!m_mesh3D->openBinary(cookFile,btrue,_szPlatform == Platform::WII))
        return bfalse;

    postProcess(_path,_szPlatform,_tryLoadingBin);
    return btrue;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  Resources and misc functions
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
void Mesh3DResource::postProcess( const Path& _filename, const Platform& _szPlatform, bbool _tryLoadingBin )
{
    if (!getMesh3D()) return;

    prepareIndexAndVertexBuffer();


    // compute BV
    m_mesh3D->BVCompute();
}


///////////////////////////////////////////////////////////////////////////////////////////////
void Mesh3DResource::prepareIndexAndVertexBuffer()
{
    //create mesh element index buffer
    m_indexBuffer.reserve( m_mesh3D->m_ListElement.size());
    for (u32 element = 0; element < m_mesh3D->m_ListElement.size(); element++ )
    {
        ITF_IndexBuffer * pIndexBuffer = GFX_ADAPTER->createIndexBuffer(3 * m_mesh3D->m_ListElement[element].m_ListTriangles.size(), bfalse);

        u32 triangleCount = m_mesh3D->m_ListElement[element].m_ListTriangles.size();
        u32 bufferSize = triangleCount*3*sizeof(u16);
        u16* indexBufferCopy = newAlloc(mId_Temporary,u16[triangleCount*3]);
        for (u32 i = 0; i < triangleCount ; i++)
        {
            const Mesh3D::Triangle &T = m_mesh3D->m_ListElement[element].m_ListTriangles[i];
            indexBufferCopy[i * 3 + 0] = (u16) T.VBIndex[0];
            indexBufferCopy[i * 3 + 1] = (u16) T.VBIndex[1];
            indexBufferCopy[i * 3 + 2] = (u16) T.VBIndex[2];
        }

        u16* indexBuffer;
        pIndexBuffer->Lock((void**)&indexBuffer);
        ITF_MemcpyWriteCombined(indexBuffer, (u8*)indexBufferCopy, bufferSize);
        pIndexBuffer->Unlock();
        SF_DEL_ARRAY(indexBufferCopy);

        m_indexBuffer.push_back(pIndexBuffer);
    }

	u32 bufferSize;
	u8* originalData;

    const ITF_VECTOR<Mesh3D::UniqueVertex>&  listUniqueVertex = m_mesh3D->m_ListUniqueVertex;
	if (m_mesh3D->needSkinning())
	{
		m_vertexBuffer = GFX_ADAPTER->createVertexBuffer(listUniqueVertex.size() , VertexFormat_PNCTBIBW, sizeof(VertexPNCTBIBW), vbLockType_static, VB_T_MESH3D);

		//fill the vertex buffer
		bufferSize  = listUniqueVertex.size()*sizeof(VertexPNCTBIBW);

		originalData =newAlloc(mId_Temporary,u8[sizeof(VertexPNCTBIBW)*listUniqueVertex.size()]);
		VertexPNCTBIBW* workingData =(VertexPNCTBIBW*) originalData;


		for (u32 i = 0; i < listUniqueVertex.size(); i++)
		{
			const Mesh3D::UniqueVertex &unique = listUniqueVertex[i];
			workingData->m_pos = unique.Position;
			workingData->m_normal = unique.Normal;
			workingData->m_tangent = unique.Tangent;
			workingData->m_color = COLOR_WHITE;
			workingData->m_uv = unique.UV;

			Mesh3D::SkinVertexFourMatrices &SV4M = m_mesh3D->m_SkinVertex[i];
			if (SV4M.Number == 1)
				workingData->setBlend( SV4M.Skin[0].m_Index, SV4M.Skin[0].m_Weight);
			else if (SV4M.Number == 2)
				workingData->setBlend( SV4M.Skin[0].m_Index, SV4M.Skin[0].m_Weight, SV4M.Skin[1].m_Index, SV4M.Skin[1].m_Weight);
			else if (SV4M.Number == 3)
				workingData->setBlend( SV4M.Skin[0].m_Index, SV4M.Skin[0].m_Weight, SV4M.Skin[1].m_Index, SV4M.Skin[1].m_Weight,SV4M.Skin[2].m_Index, SV4M.Skin[2].m_Weight);
			else if (SV4M.Number == 4)
				workingData->setBlend( SV4M.Skin[0].m_Index, SV4M.Skin[0].m_Weight, SV4M.Skin[1].m_Index, SV4M.Skin[1].m_Weight,SV4M.Skin[2].m_Index, SV4M.Skin[2].m_Weight,SV4M.Skin[3].m_Index, SV4M.Skin[3].m_Weight);

			workingData++;
		}
	}
	else
	{
		m_vertexBuffer = GFX_ADAPTER->createVertexBuffer(listUniqueVertex.size() , VertexFormat_PNCT, sizeof(VertexPNCT), vbLockType_static, VB_T_MESH3D);

		//fill the vertex buffer
		bufferSize  = listUniqueVertex.size()*sizeof(VertexPNCT);

		originalData =newAlloc(mId_Temporary,u8[sizeof(VertexPNCT)*listUniqueVertex.size()]);
		VertexPNCT* workingData =(VertexPNCT*) originalData;


		for (u32 i = 0; i < listUniqueVertex.size(); i++)
		{
			const Mesh3D::UniqueVertex &unique = listUniqueVertex[i];
			workingData->m_pos = unique.Position;
			workingData->m_normal = unique.Normal;
			workingData->m_tangent = unique.Tangent;
			workingData->m_color = COLOR_WHITE;
			workingData->m_uv = unique.UV;

			workingData++;
		}
	}

	void * meshBuffer = NULL;

	GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(m_vertexBuffer, &meshBuffer);
	ITF_MemcpyWriteCombined(meshBuffer, (u8*)originalData, bufferSize);
	GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(m_vertexBuffer);
	SF_DEL_ARRAY(originalData);
}

///////////////////////////////////////////////////////////////////////////////////////////////
void Mesh3DResource::SetMaterial(ITF_Mesh &_mesh, const ITF_VECTOR<GFXMaterialSerializable> & _materialInstanceList, const ITF_VECTOR<GFXMaterialSerializable> & _materialTplList)
{
    _mesh.clearMaterialList();

    for (u32 i = 0, n = _mesh.getNbMeshElement(); i < n; i++)
    {
        ux materialID = m_mesh3D->m_ListElement[i].m_MaterialID;

        // choose material between instance or template
        const GFX_MATERIAL * material = NULL;
        if (materialID < _materialInstanceList.size())
        {
            material = &_materialInstanceList[materialID];
        }
        if (   (!material || !material->hasValidResource())
            && materialID < _materialTplList.size()
            )

        {
            material = &_materialTplList[materialID];
        }
        // assign it
        if (material)
        {
            _mesh.getMeshElement(i).m_materialIdx = _mesh.addMaterial(*material);
        }
    }

    // be sure to get at least 1 material to idx 0
    if (_mesh.getNbMaterial() == 0)
    {
        _mesh.addMaterial(GFX_MATERIAL());
    }
}

void Mesh3DResource::ChangeMaterial(ITF_Mesh &_mesh, u32 elementIndex, u32 materialID, const ITF_VECTOR<GFXMaterialSerializable> & _materialInstanceList, const ITF_VECTOR<GFXMaterialSerializable> & _materialTplList)
{
	if (_mesh.getNbMeshElement() >= elementIndex)
	{
		// choose material between instance or template
		const GFX_MATERIAL * material = NULL;
		if (materialID < _materialInstanceList.size())
		{
			material = &_materialInstanceList[materialID];
		}
		if ( (!material || !material->hasValidResource())
			&& materialID < _materialTplList.size()
			)

		{
			material = &_materialTplList[materialID];
		}
		// assign it
		if (material)
		{
			_mesh.setMaterial(_mesh.getMeshElement(elementIndex).m_materialIdx, *material);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
void Mesh3DResource::DetachMaterial(ITF_Mesh &_mesh)
{
    // assign material idx to 0
    for (ux i = 0, n = _mesh.getNbMeshElement(); i < n; i++)
    {
        _mesh.getMeshElement(i).m_materialIdx = 0;
    }
    // clean all material
    _mesh.clearMaterialList();
    _mesh.addMaterial(GFX_MATERIAL());
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  Editor functions
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ITF_SUPPORT_EDITOR
bbool Mesh3DResource::isValidFilename( const Path &_filename )
{
    if (_filename.isEmpty()) return btrue;
    if( _filename.getExtension() == "m3d") return btrue;
    return bfalse;
}
#endif
}
