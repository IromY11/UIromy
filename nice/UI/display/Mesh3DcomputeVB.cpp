#include "precompiled_engine.h"

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_MESH3D_H_
#include    "engine/display/Mesh3D.h"
#endif // _ITF_MESH3D_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_PREFETCH_FCT_H_
#include "engine/boot/PrefetchFct.h"
#endif  //_ITF_PREFETCH_FCT_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_MESH3DRESOURCE_H_
#include "engine/display/Mesh3DResource.h"
#endif //_ITF_MESH3DRESOURCE_H_

#ifdef ITF_SUPPORT_COOKING
#ifndef _ITF_MESH3DCOOKER_H_
#include    "tools/plugins/CookerPlugin/Mesh3DCooker.h"
#endif // _ITF_MESH3DCOOKER_H_
#endif

#ifndef ITF_ENGINE_MESH_H_
# include "engine/display/Primitives/Mesh.h"
#endif //ITF_ENGINE_MESH_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Generate dynamic mesh 
    //		- assign vertex buffer and index buffer from the mesh resource
    //
    void Mesh3D::MeshComputeVertexBuffer(Mesh3DResource* pMesh3DResource,ITF_Mesh &_mesh )
    {
        // destroy content of given mesh
        MeshDestroyContent(_mesh);

        if (!m_ListUniqueVertex.size() || !m_ListElement.size() )
            return;

        // count number of triangles
        i32 triangles = 0;
        for (u32 i = 0; i < m_ListElement.size(); i++)
            triangles += m_ListElement[i].m_ListTriangles.size();

        // fill mesh
        _mesh.setMatrix(Matrix44::identity());

        //create mesh element
        while ( _mesh.getNbMeshElement() < m_ListElement.size() )
        {
            _mesh.addElement(0); // assign all the new elements to material 0
        }

        for (u32 element = 0; element < m_ListElement.size(); element++ )
        {
            ITF_MeshElement & meshElt = _mesh.getMeshElement(element);
            //create mesh element index buffer
            meshElt.m_indexBuffer = pMesh3DResource->getIndexBuffer(element);
            //set up mesh element
            meshElt.m_count = m_ListElement[element].m_ListTriangles.size() * 3;
        }

        _mesh.setVertexBuffer(pMesh3DResource->getVertexBuffer());
    }
  
} // namespace ITF
