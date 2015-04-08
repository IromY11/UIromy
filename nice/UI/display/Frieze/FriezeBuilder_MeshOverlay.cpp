#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_FRIEZESTATS_H_
#include "engine/display/Frieze/FriezeStats.h"
#endif // _ITF_FRIEZESTATS_H_

namespace ITF
{
    void Frise::clearMeshOverlayData()
    {       
        if ( !m_pMeshOverlayData )
            return;

        ITF_Mesh & mesh = m_pMeshOverlayData->m_mesh;

        if ( mesh.getCurrentVB() )
        {            
            mesh.removeVertexBuffer(3);
        }

        for (u32 i = 0, n = mesh.getNbMeshElement(); i < n; i++)
        {
            if( ITF_IndexBuffer* pIndexBuffer = mesh.getMeshElement(i).m_indexBuffer ) 
            {
                GFX_ADAPTER->removeIndexBufferDelay( pIndexBuffer, 3);                
            }
        }

        if (mesh.hasMatrixOverlay())
        {
           mesh.deleteMatrixOverlay();
        }

        SF_DEL(m_pMeshOverlayData);
    }

    void Frise::buildMesh_OverlayFinal()
    {
        ITF_ASSERT_CRASH( m_pMeshOverlayData, "Invalid pointer");
        ITF_ASSERT_CRASH( m_pMeshOverlayData->m_mesh.getNbMeshElement()==0, "Mesh still builded");

        const ITF_VECTOR<VertexPCBT>& vtxList = m_meshBuildData->m_overlayVertexList;               
        const u32 vtxCount = vtxList.size();
        ITF_ASSERT_CRASH( vtxCount, "Vertex list is empty");                

        const IndexList& indexList = m_meshBuildData->m_overlayIndexList; 
        const u32 indexCount = indexList.m_list.size();
        ITF_ASSERT_CRASH( indexCount >= 3, "Empty index list" );

        const FriseConfig* pConfig = getConfig();
        ITF_ASSERT_CRASH( pConfig, "Config file is missing" );

        // new alloc
        m_pMeshOverlayData->m_mesh.allocateMatrixOverlay();

        // copy vertexTempList
        ITF_Mesh & mesh = m_pMeshOverlayData->m_mesh;
        mesh.createVertexBuffer
            (
            vtxCount,
            VertexFormat_PCBT,
            sizeof(VertexPCBT),
            vbLockType_static,
            VB_T_FRIEZEOVERLAY
            );

        VertexPCBT    *pdata = 0;
        mesh.LockVertexBuffer((void **) &pdata);         
        ITF_MemcpyWriteCombined(pdata, &vtxList[0], sizeof(VertexPCBT)*vtxCount);
        m_pMeshOverlayData->m_mesh.UnlockVertexBuffer();                        

        // copy element                  
        addMeshElement(mesh, pConfig->m_textureConfigs[ indexList.m_idTexConfig ], indexCount );
        mesh.getMeshElementMaterial(0).setMatType(GFX_MAT_FRIEZEOVERLAY);

        ITF_MeshElement & elem = mesh.getMeshElement(0);

        u16* pindexelem;
        elem.m_indexBuffer->Lock((void**)&pindexelem);
        ITF_MemcpyWriteCombined(pindexelem , &indexList.m_list[0], sizeof(u16) *indexCount );
        elem.m_indexBuffer->Unlock();   
    }

    void Frise::buildMesh_Overlay( u32 _idTexConfig )
    {        
        ITF_VECTOR<VertexPCBT>& vtxList = m_meshBuildData->m_overlayVertexList;
        IndexList& indexList            = m_meshBuildData->m_overlayIndexList;        

        const u32 vtxCount = vtxList.size();
        if ( vtxCount < 3 )
        {
            vtxList.clear();
            indexList.clear();

            return;
        }
        
        const u32 indexCount = indexList.m_list.size();
        if ( indexCount < 3 )
        {           
            vtxList.clear();
            indexList.clear();

            return;
        }

        // set index from texture config
        indexList.m_idTexConfig = _idTexConfig;

        m_pMeshOverlayData = newAlloc(mId_Frieze, MeshOverlayData);

        buildMesh_OverlayFinal();

        // set mesh aabb local            
        m_pMeshOverlayData->m_aabbLocal.setMinAndMax( vtxList[0].m_pos );            
        for( u32 i = 1; i < vtxCount; i++ )
        {
            m_pMeshOverlayData->m_aabbLocal.grow( vtxList[i].m_pos );
        }  
     
        // grow aabb from vertexAnim
        growAabbLocalFromVertexAnim( m_pMeshOverlayData->m_aabbLocal );                          
    }
   
    void Frise::buildVB_Overlay_Quad( Vec2d* _pos, Vec2d* _uv, u32* _color, u32 _idAnim )
    {
        /*
        v1...v3
        .    .
        .    .
        v0...v2
        */

        // add vertexs
        ITF_VECTOR<VertexPCBT>& vtxList = m_meshBuildData->m_overlayVertexList;

        const u16 indexVtxDown = (u16)vtxList.size();
        const u16 indexVtxUp = indexVtxDown +1;
        vtxList.resize( indexVtxDown + 4 ); 

        VertexPCBT& vtx0 = vtxList[ indexVtxDown ];
        vtx0.setData( _pos[0], _color[0], 0, _uv[0]);

        VertexPCBT& vtx1 = vtxList[ indexVtxUp ];
        vtx1.setData( _pos[1], _color[1], _idAnim, _uv[1]);

        VertexPCBT& vtx2 = vtxList[ indexVtxDown +2];
        vtx2.setData( _pos[2], _color[2], 0, _uv[2]);

        VertexPCBT& vtx3 = vtxList[ indexVtxUp +2];
        vtx3.setData( _pos[3], _color[3], _idAnim, _uv[3]);

        // add index
        IndexList& indexList = m_meshBuildData->m_overlayIndexList;

        indexList.push_back(indexVtxDown);
        indexList.push_back(indexVtxUp);
        indexList.push_back(indexVtxDown +2);

        indexList.push_back(indexVtxUp);  
        indexList.push_back(indexVtxUp +2);
        indexList.push_back(indexVtxDown +2);        
    }

    void Frise::setMeshOverlayDataMatrix( const VertexAnim* _anim )
    {
        f32 step = 0.f;
        const f32 timeGlobal = m_time * _anim->m_globalSpeed + getAnimGlobalSync();

        f32 fTab[16];
        for ( u32 i=0; i<8; i++)
        {
            const f32 timeCur = timeGlobal + _anim->m_sync * step;
            const f32 x1 = f32_Cos( timeCur * _anim->m_speedX + _anim->m_syncX * step) * _anim->m_amplitudeX;
            const f32 y1 = f32_Sin( timeCur * _anim->m_speedY + _anim->m_syncY * step) * _anim->m_amplitudeY;

            const u32 idMatrix = i * 2;
            fTab[idMatrix]     = x1 * m_pMeshOverlayData->m_cosAngle - y1 * m_pMeshOverlayData->m_sinAngle;
            fTab[idMatrix +1]  = x1 * m_pMeshOverlayData->m_sinAngle + y1 * m_pMeshOverlayData->m_cosAngle;

            step += 1.f;
        }

        m_pMeshOverlayData->m_mesh.getMatrixOverlay()->load((const float*)fTab);
    }

} // namespace ITF
