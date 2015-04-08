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
    u32 Frise::getIndexPreviousAnimVertex( i32 _index ) const 
    {
        for ( i32 i = _index; i >= 0; i-- )
        {
            const VertexPNC3T& vtx = m_meshBuildData->m_animVertexList[i];

            if ( vtx.m_uv3.x() != 0.f || vtx.m_uv3.y() != 0.f ) // test amplitude x and y
                return (u32)i;
        }

        return U32_INVALID;
    }

    void Frise::setVtxAnimOnEdge( const VertexAnim& _anim, const edgeFrieze& _edge, VertexPNC3T& _vtx )
    {      
        setVtxAnim( _anim, _vtx );

        if ( _anim.m_angleUsed)
        {
            _vtx.m_uv3.m_w = _anim.m_angle.ToRadians();
        }
        else
        {            
            _vtx.m_uv3.m_w = atan2( _edge.m_sightNormalized.y(), _edge.m_sightNormalized.x() );
        }         
    }

    void Frise::setVtxAnimOnEdge( const VertexAnim& _anim, Vec2d& _vector, VertexPNC3T& _vtx )
    {      
        setVtxAnim( _anim, _vtx );

        if ( _anim.m_angleUsed)
        {
            _vtx.m_uv3.m_w = _anim.m_angle.ToRadians();
        }
        else
        {            
            _vtx.m_uv3.m_w = atan2( _vector.y(), _vector.x() );
        }       
    }

    void Frise::setVtxAnimOnCorner( const VertexAnim& _anim, const edgeFrieze& _edge, VertexPNC3T& _vtx )
    {
        setVtxAnim( _anim, _vtx );

        if ( _anim.m_angleUsed)
        {
            _vtx.m_uv3.m_w = _anim.m_angle.ToRadians();
        }
        else
        {
            _vtx.m_uv3.m_w = atan2( -_edge.m_cornerNormal.x(), _edge.m_cornerNormal.y() );
        } 
    }

    void Frise::setVtxAnim( const VertexAnim& _anim, VertexPNC3T& _vtx )
    {
        _vtx.m_uv2.x() = _anim.m_speedX;
        _vtx.m_uv2.y() = _anim.m_speedY;
        _vtx.m_uv2.z() = m_pRecomputeData->m_animSyncXCur;
        _vtx.m_uv2.m_w = m_pRecomputeData->m_animSyncYCur;

        _vtx.m_uv3.x() = _anim.m_amplitudeX; 
        _vtx.m_uv3.y() = _anim.m_amplitudeY;
        _vtx.m_uv3.z() = m_pRecomputeData->m_animSyncCur;

        _vtx.m_uv4 = Vec2d::Zero;  

        m_pRecomputeData->m_animSyncCur += _anim.m_sync;
        m_pRecomputeData->m_animSyncXCur += _anim.m_syncX;
        m_pRecomputeData->m_animSyncYCur += _anim.m_syncY;
    }

    void Frise::copyVtxAnim( VertexPNC3T& _vtxTo, VertexPNC3T& _vtxFrom ) const
    {                
        _vtxTo.m_uv2 = _vtxFrom.m_uv2;                 
        _vtxTo.m_uv3 = _vtxFrom.m_uv3;
        _vtxTo.m_uv4 = _vtxFrom.m_uv4;
    }

    void Frise::setVtxAnimNull( VertexPNC3T& _vtx ) const
    {
        _vtx.m_uv2.x() = 0.f;
        _vtx.m_uv2.y() = 0.f;
        _vtx.m_uv2.z() = 0.f;
        _vtx.m_uv2.m_w = 0.f;

        _vtx.m_uv3.x() = 0.f;
        _vtx.m_uv3.y() = 0.f;
        _vtx.m_uv3.z() = 0.f;
        _vtx.m_uv3.m_w = 0.f;

        _vtx.m_uv4 = Vec2d::Zero;
    }

    void Frise::clearMeshAnimData()
    {        
        if ( !m_pMeshAnimData )
            return;

        ITF_Mesh & mesh = m_pMeshAnimData->m_mesh;

        if (mesh.getCurrentVB())
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

        SF_DEL(m_pMeshAnimData);
    }

    void Frise::buildMesh_AnimFinal( )
    {
        ITF_ASSERT_CRASH( m_pMeshAnimData, "Invalid pointer");
        ITF_ASSERT_CRASH( m_pMeshAnimData->m_mesh.getNbMeshElement()==0, "Mesh still builded");

        const ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;
        const u32 vtxCount = vtxList.size();
        ITF_ASSERT_CRASH( vtxCount, "Vertex list is empty");        

        const u32 indexListCount = m_meshBuildData->m_animIndexList.size();
        ITF_ASSERT_CRASH( indexListCount > 0, "Index list is empty" );

        const FriseConfig* pConfig = getConfig();
        ITF_ASSERT_CRASH( pConfig, "Config file is missing" );          

        // copy vertexTempList
        ITF_Mesh & mesh = m_pMeshAnimData->m_mesh;
        mesh.createVertexBuffer
            (
            vtxCount,
            VertexFormat_PNC3T,
            sizeof(VertexPNC3T),
            vbLockType_static,
            VB_T_FRIEZEANIM
            );

        bbool invertMeshOrder = pConfig->m_invertMeshOrder;
        // copy all element       
        for ( u32 i = 0; i < indexListCount; i++ )
        {            
            IndexList& indexList = invertMeshOrder ? m_meshBuildData->m_animIndexList[indexListCount-i-1] : m_meshBuildData->m_animIndexList[i];
            WeldMeshDataAnim(indexList);

            const u32 indexTableSize = indexList.m_list.size();
            ITF_ASSERT_CRASH( indexTableSize >= 3, "Empty index table");

            addMeshElement(mesh, pConfig->m_textureConfigs[indexList.m_idTexConfig], indexTableSize );

            const ux meshEltIdx = mesh.getNbMeshElement() - 1u;
            mesh.getMeshElementMaterial(meshEltIdx).setMatModifier(GFX_MAT_MOD_FRIEZEANIM);

            ITF_MeshElement & elem = mesh.getMeshElement(meshEltIdx);

            u16* pindexelem;
            elem.m_indexBuffer->Lock((void**)&pindexelem);
            ITF_MemcpyWriteCombined(pindexelem ,&indexList.m_list[0], sizeof(u16) * indexTableSize );
            elem.m_indexBuffer->Unlock();
        }

        VertexPNC3T    *pdata = 0;
        mesh.LockVertexBuffer((void **) &pdata);
        ITF_MemcpyWriteCombined(pdata, &vtxList[0], sizeof(VertexPNC3T) *vtxCount);
        mesh.UnlockVertexBuffer();                        

        ITF_ASSERT( mesh.getNbMeshElement() == 1); // to match with renderFrieze() mesh anim

		WorldUpdateElement* elem = getWorldUpdateElement();

		if ( elem != NULL )
			elem->setUpdateOnlyOnChange(bfalse);
    }
    
    void Frise::buildMesh_Anim( )
    {
        ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;
        const u32 vtxCount = vtxList.size();

        if ( vtxCount < 3 )
        {
            vtxList.clear();
            m_meshBuildData->m_animIndexList.clear();

            return;
        }

        cleanIndexListArray( m_meshBuildData->m_animIndexList );

        const u32 indexListCount = m_meshBuildData->m_animIndexList.size();
        if ( indexListCount == 0 )
        {
            vtxList.clear();
            return;
        }

        m_pMeshAnimData = newAlloc(mId_Frieze, MeshAnimData); 

        buildMesh_AnimFinal();

        // set mesh aabb local
        m_pMeshAnimData->m_aabbLocal.setMinAndMax( vtxList[0].m_pos );
        for( u32 i = 1; i < vtxCount; i++ )
        {
            m_pMeshAnimData->m_aabbLocal.grow( vtxList[i].m_pos );
        }

        // grow aabb from vertexAnim
        growAabbLocalFromVertexAnim( m_pMeshAnimData->m_aabbLocal );
    }

    void Frise::buildVB_Anim_CornerRounderBlending( IndexList& _indexList, CornerRounder& _corner )
    {
        const u16 indexVtxStart = *_corner.m_indexVtxCorner;            

        // corner's edge norm 
        const f32 angle = f32_Abs(_corner.m_edgeCur->m_cornerAngle);
        const u32 stepCount = getCornerStepNb( angle, _corner.m_config->m_smoothFactorVisual, getCoeffScaleStepFanToBlend( _corner.m_edgeCur, _corner.m_edgeLast) );  
        const f32 timeStep = 1.f /(f32)stepCount;

        // bezier curve        
        const Vec2d p0 = _corner.m_startPos;     
        const Vec2d p3 = _corner.m_stopPos; 
        Vec2d p1 = p0; 
        Vec2d p2 = p3; 

        buildOutline_initPosCornerRounderBlending( _corner.m_edgeLast, _corner.m_edgeCur, p1, p2);

        Vec2d posLast = p0;
        f32 uvCornerTotal = 0.f;
        Vec2d uvCur = *_corner.m_uvCorner;
        const f32 coeffUv = getStepCornerCoeff( _corner.m_edgeRun->m_coeff, angle ) *(*_corner.m_uvXsign);

        ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;
        const u32 vtxListCount = vtxList.size();
        vtxList.resize( vtxListCount + Max( 1u, stepCount ) );

        *_corner.m_indexVtxCorner = (u16)vtxListCount;
        u16 indexVtxCorner = *_corner.m_indexVtxCorner -1;

        for ( u32 i = 1; i < stepCount; i++ )
        {
            const f32 time = (f32)i * timeStep;
            const f32 timeInv = 1.f - time;

            const Vec2d pos =   ( p0 *timeInv *timeInv *timeInv ) 
                + ( p1 *3.f *time *timeInv *timeInv ) 
                + ( p2 *3.f *time *time *timeInv ) 
                + ( p3 *time *time *time );

            // update uv
            f32 uvToAdd = ( pos -posLast).norm() *coeffUv;
            uvCur.x() += uvToAdd;
            uvCornerTotal += uvToAdd;

            VertexPNC3T& vtxCorner = vtxList[*_corner.m_indexVtxCorner];
            setVtxPos( pos, vtxCorner.m_pos, _corner.m_zVtxCorner);
            vtxCorner.setUv( uvCur);
            vtxCorner.setColor( _corner.m_colorCorner.getAsU32() );
            copyVtxAnim( vtxCorner, vtxList[indexVtxStart] );

            //  add vertex to create a triangle
            _indexList.push_back( *_corner.m_indexVtxCorner );

            // add two index for next triangle
            _indexList.push_back( *_corner.m_indexVtxCenter );
            _indexList.push_back( *_corner.m_indexVtxCorner );

            *_corner.m_indexVtxCorner += 1;

            posLast = pos;
        }

        uvCornerTotal += ( p3 -posLast).norm() *coeffUv;

        // set uvs corner with offset
        f32 offset = uvCornerTotal *0.5f;

        // update uv quad edge last
        updateEdgeCornerUvs( _corner, vtxList[indexVtxStart].m_uv.x(), offset, offset);

        if ( _corner.m_edgeCur->m_snap && *_corner.m_edgeCountCur == _corner.m_edgeRun->m_edgeCount -1)
            offset *= 2.f;

        // update uv corner
        if ( !_corner.m_edgeLast->m_snap)
        {
            for ( u32 i = 1; i < stepCount; i++ )
            {
                vtxList[indexVtxCorner +i ].m_uv.x() -= offset;
            }
        }

        // update uv for next vtx
        _corner.m_uvCorner->x() += uvCornerTotal;
    }

    void Frise::buildVB_Anim_CornerRounder( IndexList& _indexList, CornerRounder& _corner )
    {
        const u16 vtxStartId = *_corner.m_indexVtxCorner;

        u32 stepNb;
        f32 stepCornerAngle;  
        f32 uvXStep;                
        f32 arcBy2;
        setCornerRounderDatas( _corner.m_config, _corner.m_edgeCur, _corner.m_edgeRun->m_coeff, stepNb, stepCornerAngle, uvXStep, arcBy2, *_corner.m_uvXsign );

        ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;
        const u32 vtxListCount = vtxList.size();
        vtxList.resize( vtxListCount + Max( 1u, stepNb ) );

        // update uvs
        updateEdgeCornerUvs( _corner, vtxList[*_corner.m_indexVtxCorner].m_uv.x(), arcBy2, arcBy2);

        *_corner.m_indexVtxCorner = (u16)vtxListCount;

        f32 rotCos, rotSin;
        f32_CosSin( -stepCornerAngle *getSign(_corner.m_edgeCur->m_cornerAngle), &rotCos, &rotSin);

        for ( u32 i = 1; i < stepNb; i++ )
        {
            // update uv
            _corner.m_uvCorner->x() += uvXStep;
            // update rotation
            _corner.m_startPos = _corner.m_startPos.RotateAroundCS( _corner.m_centerPos, rotCos, rotSin );

            VertexPNC3T& vtxCorner = vtxList[*_corner.m_indexVtxCorner];
            setVtxPos( _corner.m_startPos, vtxCorner.m_pos, _corner.m_zVtxCorner);
            vtxCorner.setUv( *_corner.m_uvCorner);
            vtxCorner.setColor( _corner.m_colorCorner.getAsU32() );
            copyVtxAnim(vtxCorner, vtxList[vtxStartId]);

            //  add vertex to create a triangle
            _indexList.push_back( *_corner.m_indexVtxCorner );

            // add two index for next triangle
            _indexList.push_back( *_corner.m_indexVtxCenter );
            _indexList.push_back( *_corner.m_indexVtxCorner );

            *_corner.m_indexVtxCorner += 1;
        }

        // update uv
        _corner.m_uvCorner->x() += uvXStep;
    }

    void Frise::buildVB_Anim_CornerSquare( IndexList& _indexList, CornerRounder& _corner )
    {
        const u16 indexVtxStart = *_corner.m_indexVtxCorner; 

        const f32 uvLeftCorner = getCornerSquareArcBy2( _corner.m_edgeCur->m_cornerAngle, _corner.m_edgeRun->m_coeff, _corner.m_startPos - _corner.m_cornerPos, *_corner.m_uvXsign );
        f32 uvRightCorner = uvLeftCorner;

        if ( !isHeightEqual( *_corner.m_edgeLast, *_corner.m_edgeCur) )
        {
            uvRightCorner = getCornerSquareArcBy2( _corner.m_edgeCur->m_cornerAngle, _corner.m_edgeRun->m_coeff, _corner.m_stopPos - _corner.m_cornerPos, *_corner.m_uvXsign );
        }    

        ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;
        const u32 vtxListCount = vtxList.size();
        vtxList.resize( vtxListCount + 2 );

        // update uvs
        updateEdgeCornerUvs( _corner, vtxList[*_corner.m_indexVtxCorner].m_uv.x(), uvLeftCorner, uvRightCorner);

        *_corner.m_indexVtxCorner = (u16)vtxListCount;        

        // update uv
        _corner.m_uvCorner->x() += uvLeftCorner;

        VertexPNC3T& vtxCorner = vtxList[*_corner.m_indexVtxCorner];
        setVtxPos( _corner.m_cornerPos, vtxCorner.m_pos, _corner.m_zVtxCorner);
        vtxCorner.setUv( *_corner.m_uvCorner);
        vtxCorner.setColor( _corner.m_colorCorner.getAsU32() );
        copyVtxAnim(vtxCorner, vtxList[indexVtxStart]);

        //  add vertex to create a triangle
        _indexList.push_back( *_corner.m_indexVtxCorner );

        // add two index for next triangle
        _indexList.push_back( *_corner.m_indexVtxCenter );
        _indexList.push_back( *_corner.m_indexVtxCorner );

        *_corner.m_indexVtxCorner += 1;

        // update uv
        _corner.m_uvCorner->x() += uvRightCorner;
    }

    void Frise::buildVB_Anim_Corner( IndexList& _indexList, CornerRounder& _corner )
    { 
        ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;

        if ( _corner.m_edgeCur->m_cornerAngle == 0.f )
            return;

        u16 indexVtxAnimToCopy;

        if ( _corner.m_edgeCur->m_cornerAngle > 0.f )
        {
            _corner.m_indexVtxCenter = _corner.m_indexVtxDown;
            _corner.m_indexVtxCorner = _corner.m_indexVtxUp;
            _corner.m_zVtxCorner = m_pRecomputeData->m_zVtxUp;

            _corner.m_startPos = _corner.m_edgeLast->m_points[3];
            _corner.m_centerPos = _corner.m_edgeCur->m_interDown;
            _corner.m_stopPos = _corner.m_edgeCur->m_points[1];
            _corner.m_cornerPos = _corner.m_edgeCur->m_interUp;

            _corner.m_uvCorner = _corner.m_uvUp;
            _corner.m_uvXoffsetCorner = _corner.m_uvUpXOffSet;

            _corner.m_colorCorner = vtxList[*_corner.m_indexVtxUp].m_color;
            indexVtxAnimToCopy = *_corner.m_indexVtxUp;
        }
        else
        {
            _corner.m_indexVtxCenter = _corner.m_indexVtxUp;
            _corner.m_indexVtxCorner = _corner.m_indexVtxDown;
            _corner.m_zVtxCorner = m_pRecomputeData->m_zVtxDown;

            _corner.m_startPos = _corner.m_edgeLast->m_points[2];
            _corner.m_centerPos = _corner.m_edgeCur->m_interUp;
            _corner.m_stopPos = _corner.m_edgeCur->m_points[0];
            _corner.m_cornerPos = _corner.m_edgeCur->m_interDown;

            _corner.m_uvCorner = _corner.m_uvDown;
            _corner.m_uvXoffsetCorner = _corner.m_uvDownXOffSet;

            _corner.m_colorCorner = vtxList[*_corner.m_indexVtxDown].m_color;
            indexVtxAnimToCopy = *_corner.m_indexVtxDown;
        }

        _indexList.push_back( *_corner.m_indexVtxCenter );
        _indexList.push_back( *_corner.m_indexVtxCorner );

        if ( _corner.m_config->m_isSmoothVisual )
        {
            if ( isHeightEqual( *_corner.m_edgeLast, *_corner.m_edgeCur))
            {
                buildVB_Anim_CornerRounder( _indexList, _corner );
            }
            else
            {
                buildVB_Anim_CornerRounderBlending( _indexList, _corner );
            }
        }
        else
        {
            buildVB_Anim_CornerSquare( _indexList, _corner );
        }

        // add last vertex to create last triangle
        VertexPNC3T& vtx = vtxList[*_corner.m_indexVtxCorner];
        setVtxPos( _corner.m_stopPos, vtx.m_pos, _corner.m_zVtxCorner);
        vtx.setUv( *_corner.m_uvCorner);
        vtx.setColor(_corner.m_colorCorner.getAsU32() );
        copyVtxAnim(vtx, vtxList[indexVtxAnimToCopy]);

        _indexList.push_back( *_corner.m_indexVtxCorner );
    }

    void Frise::buildVB_Anim_EdgeRunCorner( const FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, IndexList& _indexList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32& _uvUpXOffSet, f32& _uvDownXOffSet, f32 _uvXsign )
    {
        u32 edgeCount = 1;

        CornerRounder corner;
        corner.m_edgeCur = NULL;
        corner.m_edgeLast = &_edgeList[_edgeRun.m_idEdgeStart];
        corner.m_config = config;
        corner.m_edgeRun = &_edgeRun;    
        corner.m_indexVtxDown = &_indexVtxDown;
        corner.m_indexVtxUp = &_indexVtxUp;
        corner.m_uvXsign = &_uvXsign;
        corner.m_uvDown = &_uvDown;
        corner.m_uvUp = &_uvUp;
        corner.m_uvDownXOffSet = &_uvDownXOffSet;
        corner.m_uvUpXOffSet = &_uvUpXOffSet;
        corner.m_edgeCountCur = &edgeCount;

        //  edgeRun's corners
        for (; edgeCount < _edgeRun.m_edgeCount; edgeCount++ )
        {
            u32 idEdge = ( _edgeRun.m_idEdgeStart + edgeCount ) % m_pRecomputeData->m_edgeListCount;

            corner.m_edgeCur = &_edgeList[idEdge];

            if ( isEdgeWithHoleVisual(*corner.m_edgeLast))
            {
                m_pRecomputeData->m_edgeRunStartBuildVB = bfalse;

                // update uv 
                updateUvs( _edgeRun, corner.m_edgeLast, _uvUp, _uvDown, _uvXsign, _uvUpXOffSet, _uvDownXOffSet );

                corner.m_edgeLast = corner.m_edgeCur;
                continue;
            }

            // build quad edge last
            if ( !corner.m_edgeLast->m_snap )
            {       
                if ( !m_pRecomputeData->m_edgeRunStartBuildVB )
                {
                    buildVB_Anim_addVtxOnStartPosEdge( config->m_vtxAnim, *corner.m_edgeLast, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown );
                }

                // update quad uv 
                updateUvs( _edgeRun, corner.m_edgeLast, _uvUp, _uvDown, _uvXsign, _uvUpXOffSet, _uvDownXOffSet );

                ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;
                const u32 vtxListCount = vtxList.size();
                vtxList.resize( vtxListCount + 2 );

                VertexPNC3T& vtxDown = vtxList[ vtxListCount ];                
                VertexPNC3T& vtxUp  = vtxList[ vtxListCount +1];

                setVtxBorderRightPosDown( *corner.m_edgeLast, vtxDown.m_pos);
                vtxDown.setUv( _uvDown);
                vtxDown.setColor(_vtxColorDown);
                setVtxAnimNull(vtxDown);

                setVtxBorderRightPosUp( *corner.m_edgeLast, vtxUp.m_pos);
                vtxUp.setUv( _uvUp);
                vtxUp.setColor(_vtxColorUp);
                setVtxAnimOnCorner( config->m_vtxAnim, *corner.m_edgeCur, vtxUp);

                addIndex_Anim_Quad( _indexList, _indexVtxDown, _indexVtxUp, corner.m_edgeCur->m_cornerAngle <0.f );

                // reset uvXoffset
                _uvUpXOffSet = 0.f;
                _uvDownXOffSet = 0.f;
            } // end build quad edge last

            //  build corner's edge cur
            if ( isEdgeWithHoleVisual(*corner.m_edgeCur))
            {
                m_pRecomputeData->m_edgeRunStartBuildVB = bfalse;
                corner.m_edgeLast = corner.m_edgeCur;
                continue;
            }

            if ( !m_pRecomputeData->m_edgeRunStartBuildVB)
            {
                buildVB_Anim_addVtxOnStopPosEdge( config->m_vtxAnim, *corner.m_edgeLast, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown );
            }

            buildVB_Anim_Corner( _indexList, corner);

            corner.m_edgeLast = corner.m_edgeCur;
        }

        if ( isEdgeWithHoleVisual(*corner.m_edgeLast))
        {
            updateUvs( _edgeRun, corner.m_edgeLast, _uvUp, _uvDown, _uvXsign, _uvUpXOffSet, _uvDownXOffSet );
        }
    }

    void Frise::buildVB_Anim_addVtxOnStartPosEdge( const VertexAnim& _anim, const edgeFrieze& edge, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown )
    {
        m_pRecomputeData->m_edgeRunStartBuildVB = btrue;

        ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;
        const u32 vtxListCount = vtxList.size();
        vtxList.resize( vtxListCount + 2 );

        _indexVtxDown = (u16)vtxListCount;
        _indexVtxUp = _indexVtxDown +1;

        VertexPNC3T& vtxDown    = vtxList[_indexVtxDown];            
        VertexPNC3T& vtxUp      = vtxList[_indexVtxUp];

        setVtxBorderLeftPosDown( edge, vtxDown.m_pos);
        vtxDown.setUv( _uvDown);
        vtxDown.setColor( _vtxColorDown);
        setVtxAnimNull(vtxDown);

        setVtxBorderLeftPosUp( edge, vtxUp.m_pos);
        vtxUp.setUv( _uvUp);
        vtxUp.setColor(_vtxColorUp);
        setVtxAnimOnEdge(_anim, edge, vtxUp);
    }

    void Frise::buildVB_Anim_addVtxOnStopPosEdge( const VertexAnim& _anim, const edgeFrieze& edge, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown )
    {
        m_pRecomputeData->m_edgeRunStartBuildVB = btrue;

        ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;
        const u32 vtxListCount = vtxList.size();
        vtxList.resize( vtxListCount + 2 );

        _indexVtxDown = (u16)vtxListCount;
        _indexVtxUp = _indexVtxDown +1;

        VertexPNC3T& vtxDown = vtxList[_indexVtxDown];            
        VertexPNC3T& vtxUp = vtxList[_indexVtxUp];

        setVtxBorderRightPosDown( edge, vtxDown.m_pos);
        vtxDown.setUv( _uvDown);
        vtxDown.setColor( _vtxColorDown);
        setVtxAnimNull(vtxDown);

        setVtxBorderRightPosUp( edge, vtxUp.m_pos);
        vtxUp.setUv( _uvUp);
        vtxUp.setColor( _vtxColorUp);
        setVtxAnimOnEdge(_anim, edge, vtxUp);
    }

    void Frise::buildVB_Anim_EdgeRun( const FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, IndexList& _indexList, u32 _vtxColorUp, u32 _vtxColorDown, u32 _vtxColorUpExtremity, u32 _vtxColorDownExtremity, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32 _uvXsign )
    {
        if (buildVB_Static_EdgeRunMesh(config, _edgeList, _edgeRun, _indexList, _vtxColorUp, _vtxColorDown, _vtxColorUpExtremity, _vtxColorDownExtremity, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown, _uvXsign))
            return;

        edgeFrieze* edgeCur;        
        f32 uvUpXOffSet = 0.f;
        f32 uvDownXOffSet = 0.f;   
        m_pRecomputeData->m_edgeRunStartBuildVB = bfalse;

        //  start edge
        edgeCur = &_edgeList[_edgeRun.m_idEdgeStart];

        // left vertexs'edge start if it's not snaped
        if ( !isEdgeWithHoleVisual(*edgeCur))
        {
            if ( !edgeCur->m_snap )
            {    
                bbool build = bfalse;

                // hole visual
                if ( _edgeRun.m_idEdgeStart > 0 || m_pointsList.isLooping())
                {
                    u32 idEgdeLast = ( _edgeRun.m_idEdgeStart -1 +m_pRecomputeData->m_edgeListCount) % m_pRecomputeData->m_edgeListCount;
                    edgeFrieze* edgeLast = &_edgeList[idEgdeLast];

                    if ( isEdgeWithHoleVisual(*edgeLast) )
                    {
                        m_pRecomputeData->m_edgeRunStartBuildVB = btrue;
                        build = btrue;

                        ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;
                        const u32 vtxListCount = vtxList.size();
                        vtxList.resize( vtxListCount +2 );

                        _indexVtxDown = (u16)vtxListCount;
                        _indexVtxUp = _indexVtxDown +1;

                        VertexPNC3T& vtxDown    = vtxList[_indexVtxDown];            
                        VertexPNC3T& vtxUp      = vtxList[_indexVtxUp];

                        Vec2d pos = edgeCur->m_points[0] -edgeCur->m_sightNormalized *config->m_visualOffset *m_pRecomputeData->m_heightScale;
                        setVtxPosDown( pos, vtxDown.m_pos);
                        vtxDown.setUv( _uvDown);
                        vtxDown.setColor( _vtxColorDown);
                        setVtxAnimNull(vtxDown);

                        pos +=  getEdgeBorderLeft(*edgeCur);
                        setVtxPosUp( pos, vtxUp.m_pos);
                        vtxUp.setUv( _uvUp);
                        vtxUp.setColor( _vtxColorUp);
                        setVtxAnimOnEdge( config->m_vtxAnim, *edgeCur, vtxUp);
                    }
                }

                if ( !build )
                    buildVB_Anim_addVtxOnStartPosEdge( config->m_vtxAnim, *edgeCur, _vtxColorUpExtremity, _vtxColorDownExtremity, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown );
            }
        }

        //  corner rounder buid way
        buildVB_Anim_EdgeRunCorner( config, _edgeList, _edgeRun, _indexList, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown, uvUpXOffSet, uvDownXOffSet, _uvXsign );

        // build quad edge last
        u32 idEdgeStop = ( _edgeRun.m_idEdgeStart + _edgeRun.m_edgeCount +m_pRecomputeData->m_edgeListCount -1) % m_pRecomputeData->m_edgeListCount;
        edgeFrieze* edgeStop = &_edgeList[idEdgeStop];

        if ( !edgeStop->m_snap && !isEdgeWithHoleVisual(*edgeStop))
        {
            if ( !m_pRecomputeData->m_edgeRunStartBuildVB)
            {
                buildVB_Anim_addVtxOnStartPosEdge( config->m_vtxAnim, *edgeStop, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown  );
            }

            Vec2d holeOffset(0.f,0.f);
            if ( idEdgeStop < m_pRecomputeData->m_edgeListCount -1 || m_pointsList.isLooping() )
            {
                u32 idEdgeNext = ( idEdgeStop +1) % m_pRecomputeData->m_edgeListCount;
                if ( isEdgeWithHoleVisual(_edgeList[idEdgeNext]))
                {
                    holeOffset = edgeStop->m_sightNormalized * config->m_visualOffset *m_pRecomputeData->m_heightScale;
                }
            }

            ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;
            const u32 vtxListCount = vtxList.size();
            vtxList.resize( vtxListCount + 2 );

            VertexPNC3T& vtxDown = vtxList[ vtxListCount ];            
            VertexPNC3T& vtxUp  = vtxList[ vtxListCount +1 ];

            // update uv 
            f32 uvXQuadOffSet = edgeStop->m_normUv * _edgeRun.m_coeff *_uvXsign;
            _uvUp.x() += uvXQuadOffSet + uvUpXOffSet;
            _uvDown.x() += uvXQuadOffSet + uvDownXOffSet;

            Vec2d pos = edgeStop->m_points[2] + holeOffset;
            setVtxPosDown( pos, vtxDown.m_pos);
            vtxDown.setUv( _uvDown);
            vtxDown.setColor(_vtxColorDownExtremity); 
            setVtxAnimNull(vtxDown);

            pos += getEdgeBorderRight( *edgeStop);
            setVtxPosUp( pos, vtxUp.m_pos);
            vtxUp.setUv( _uvUp);
            vtxUp.setColor(_vtxColorUpExtremity);
            setVtxAnimOnEdge( config->m_vtxAnim, *edgeStop, vtxUp);

            addIndex_Anim_Quad( _indexList, _indexVtxDown, _indexVtxUp, flipEdge(uvUpXOffSet-uvDownXOffSet) );
        }
    }

    void Frise::addIndex_Anim_Quad( IndexList& _indexList, u16& _indexVtxDown, u16& _indexVtxUp, bbool _flip )
    {
        /*
        Flip     !Flip  
        1__3     1__3              
        |\ |     | /|   
        |_\|     |/_|   
        0  2     0  2   
        */

        u16 _vertexCount = (u16)m_meshBuildData->m_animVertexList.size();

        _indexList.push_back( _indexVtxDown );
        _indexList.push_back( _indexVtxUp );

        setFlipQuadWithAlphaBorder( _flip, m_meshBuildData->m_animVertexList[_indexVtxDown].m_color, m_meshBuildData->m_animVertexList[_vertexCount -2].m_color);

        if ( _flip )
        {
            _indexVtxDown = _vertexCount -2;
            _indexList.push_back( _indexVtxDown );
            _indexList.push_back( _indexVtxUp );

            _indexVtxUp = _vertexCount -1;
            _indexList.push_back( _indexVtxUp );
            _indexList.push_back( _indexVtxDown );
        }
        else
        {
            _indexVtxUp = _vertexCount -1;
            _indexList.push_back( _indexVtxUp );
            _indexList.push_back( _indexVtxDown );
            _indexList.push_back( _indexVtxUp );

            _indexVtxDown = _vertexCount -2;
            _indexList.push_back( _indexVtxDown );
        }
    }

    void Frise::buildVB_Anim_Quad( const VertexAnim& _anim, IndexList& _indexList, const Vec2d* _pos, const Vec2d* _uv, const u32* _color, bbool _flip, bbool _vtxAnimStart, bbool _vtxAnimStop, const f32* _zExtrude /*= NULL*/ )
    {
        /*
        v1...v3
        .    .
        .    .
        v0...v2
        */
        if (buildVB_Static_QuadMeshAlone(_pos, _uv))
            return;

        ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;
        const u32 vtxListCount = vtxList.size();
        vtxList.resize( vtxListCount + 4 );

        Vec2d vector = _pos[2] -_pos[0];
        vector.normalize();

        u16 indexVtxDown = (u16)vtxListCount;
        u16 indexVtxUp = indexVtxDown +1;

        VertexPNC3T& vtx0 = vtxList[ indexVtxDown ];
        setVtxPosDown( _pos[0], vtx0.m_pos);
        vtx0.setColor(_color[0]);
        vtx0.setUv( _uv[0]);   
        setVtxAnimNull(vtx0);

        VertexPNC3T& vtx1 = vtxList[ indexVtxUp ];
        setVtxPosUp( _pos[1], vtx1.m_pos);
        vtx1.setColor(_color[1]);
        vtx1.setUv( _uv[1]);
        if ( _vtxAnimStart )
            setVtxAnimOnEdge( _anim, vector, vtx1);
        else
            setVtxAnimNull( vtx1);

        VertexPNC3T& vtx2 = vtxList[ indexVtxDown +2];
        setVtxPosDown( _pos[2], vtx2.m_pos);
        vtx2.setColor(_color[2]);
        vtx2.setUv( _uv[2]);
        setVtxAnimNull(vtx2);

        VertexPNC3T& vtx3 = vtxList[ indexVtxUp +2];
        setVtxPosUp( _pos[3], vtx3.m_pos);
        vtx3.setColor(_color[3]);
        vtx3.setUv( _uv[3]);
        if ( _vtxAnimStop )
            setVtxAnimOnEdge( _anim, vector, vtx3);
        else
            setVtxAnimNull( vtx3);

        if ( _zExtrude )
        {
            vtx0.m_pos.z() += _zExtrude[0];
            vtx1.m_pos.z() += _zExtrude[1];
            vtx2.m_pos.z() += _zExtrude[2];
            vtx3.m_pos.z() += _zExtrude[3];
        }

        addIndex_Anim_Quad( _indexList, indexVtxDown, indexVtxUp, _flip );
    }

    void Frise::randomizeAnimGlobalSync()
    {
        m_animGlobalSync = Seeder::getSharedSeeder().GetFloat( 0.f, MTH_2PI );
    }

} // namespace ITF
