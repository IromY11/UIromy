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


#include "core/types.h"

namespace ITF
{

    void Frise::clearMeshStaticData()
    {        
        if ( !m_pMeshStaticData )
            return;

        ITF_Mesh & mesh = m_pMeshStaticData->m_mesh;

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

        SF_DEL(m_pMeshStaticData);
    }

    void Frise::buildMesh_StaticFinal()
    {
        ITF_ASSERT_CRASH( m_pMeshStaticData, "Invalid pointer");
        ITF_ASSERT_CRASH( m_pMeshStaticData->m_mesh.getNbMeshElement()==0, "Mesh still builded");

        const ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
        const u32 vtxCount = vtxList.size();
        ITF_ASSERT_CRASH( vtxCount, "Vertex list is empty");       

        const u32 indexListCount = m_meshBuildData->m_staticIndexList.size();
        ITF_ASSERT_CRASH( indexListCount > 0, "Index list is empty" );

        const FriseConfig* pConfig = getConfig();
        ITF_ASSERT_CRASH( pConfig, "Config file is missing" );
        
        // copy vertexTempList
        ITF_Mesh & mesh = m_pMeshStaticData->m_mesh;

        mesh.createVertexBuffer
            (
            vtxCount,
            VertexFormat_PCT,
            sizeof(VertexPCT),
            vbLockType_static,
            VB_T_FRIEZESTATIC
            );

        bbool invertMeshOrder = pConfig->m_invertMeshOrder;
        // copy all element       
        for ( u32 i = 0; i < indexListCount; i++ )
        {            
            IndexList& indexList = invertMeshOrder ? m_meshBuildData->m_staticIndexList[indexListCount-i-1] : m_meshBuildData->m_staticIndexList[i];
            WeldMeshDataStatic( indexList);

            const u32 indexTableSize = indexList.m_list.size();     

            ITF_ASSERT_CRASH( indexTableSize >= 3, "Empty index table");

            addMeshElement( mesh, pConfig->m_textureConfigs[indexList.m_idTexConfig], indexTableSize );

            
            const ux meshEltIdx = mesh.getNbMeshElement() - 1u;
            ITF_MeshElement & elem = mesh.getMeshElement(meshEltIdx);

            u16* pindexelem;
            elem.m_indexBuffer->Lock((void**)&pindexelem);
            ITF_MemcpyWriteCombined(pindexelem ,&indexList.m_list[0], sizeof(u16) * indexTableSize );
            elem.m_indexBuffer->Unlock();
        }

        VertexPCT    *pdata = 0;
        mesh.LockVertexBuffer((void **) &pdata);
        ITF_MemcpyWriteCombined(pdata, &vtxList[0], sizeof(VertexPCT) *vtxCount);
        mesh.UnlockVertexBuffer();      

		mesh.updatePassFilterFlag();
    }

    void Frise::buildMesh_Static( )
    {
        ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
        const u32 vtxCount = vtxList.size();

        if ( vtxCount < 3 )
        {
            vtxList.clear();
            m_meshBuildData->m_staticIndexList.clear();

            return;
        }

        cleanIndexListArray( m_meshBuildData->m_staticIndexList );

        const u32 indexListCount = m_meshBuildData->m_staticIndexList.size();
        if ( indexListCount == 0 )
        {
            vtxList.clear();
            return;
        }
       
        m_pMeshStaticData = newAlloc(mId_Frieze, MeshStaticData);

        buildMesh_StaticFinal();

        // set mesh aabb local
        m_pMeshStaticData->m_aabbLocal.setMinAndMax( vtxList[0].m_pos );
        for( u32 i = 1; i < vtxCount; i++ )
        {
            m_pMeshStaticData->m_aabbLocal.grow( vtxList[i].m_pos );
        }
    }

    void Frise::buildVB_Static_CornerRounderBlending( IndexList& _indexList, CornerRounder& _corner )
    {
        const u16 indexVtxStart = *_corner.m_indexVtxCorner;            

        // corner's edge norm 
        const f32 angle = f32_Abs(_corner.m_edgeCur->m_cornerAngle);
        const u32 stepCount = getCornerStepNb( angle, _corner.m_config->m_smoothFactorVisual, getCoeffScaleStepFanToBlend( _corner.m_edgeCur, _corner.m_edgeLast) );  
        const f32 timeStep = 1.f /(f32)stepCount;

        ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
        const u32 vtxListCount = vtxList.size();
        vtxList.resize( vtxListCount + Max( 1u, stepCount ) );

        // bezier curve        
        const Vec2d p0 = _corner.m_startPos;     
        const Vec2d p3 = _corner.m_stopPos; 
        Vec2d p1 = p0; 
        Vec2d p2 = p3; 

        buildOutline_initPosCornerRounderBlending( _corner.m_edgeLast, _corner.m_edgeCur, p1, p2);

#ifdef DEVELOPER_JAY_FRIEZE
        GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(p0), transformPosLocalToWorld(p1), Color::red(), 0.f, 5.f, getDepth() );
        GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(p1), transformPosLocalToWorld(p2), Color::red(), 0.f, 5.f, getDepth() );
        GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(p2), transformPosLocalToWorld(p3), Color::red(), 0.f, 5.f, getDepth() );
#endif

        Vec2d posLast = p0;
        f32 uvCornerTotal = 0.f;
        Vec2d uvCur = *_corner.m_uvCorner;
        const f32 coeffUv = getStepCornerCoeff( _corner.m_edgeRun->m_coeff, angle ) *(*_corner.m_uvXsign);

        *_corner.m_indexVtxCorner = (u16)vtxListCount;
        u16 indexVtxCorner = *_corner.m_indexVtxCorner -1;        

        for ( u32 i = 1; i < stepCount; i++ )
        {
            const f32 time = (f32)i * timeStep;
            const f32 timeInv = 1.f - time;

            Vec2d pos =   ( p0 *timeInv *timeInv *timeInv ) 
                + ( p1 *3.f *time *timeInv *timeInv ) 
                + ( p2 *3.f *time *time *timeInv ) 
                + ( p3 *time *time *time );

            // update uv
            const f32 uvToAdd = ( pos -posLast).norm() *coeffUv;
            uvCur.x() += uvToAdd;
            uvCornerTotal += uvToAdd;

            VertexPCT& vtxCorner = vtxList[*_corner.m_indexVtxCorner];
            setVtxPos( pos, vtxCorner.m_pos, _corner.m_zVtxCorner);
            vtxCorner.m_uv = uvCur;
            vtxCorner.setColor( _corner.m_colorCorner.getAsU32() );

            //  add vertex to create a triangle
            _indexList.push_back(*_corner.m_indexVtxCorner);

            // add two index for next triangle
            _indexList.push_back(*_corner.m_indexVtxCenter);
            _indexList.push_back(*_corner.m_indexVtxCorner);

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

    void Frise::buildVB_Static_CornerRounder( IndexList& _indexList, CornerRounder& _corner )
    {
        u32 stepNb;
        f32 stepCornerAngle;  
        f32 uvXStep;                
        f32 arcBy2;
        setCornerRounderDatas( _corner.m_config, _corner.m_edgeCur, _corner.m_edgeRun->m_coeff, stepNb, stepCornerAngle, uvXStep, arcBy2, *_corner.m_uvXsign );

        ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
        const u32 vtxListCount = vtxList.size();
        vtxList.resize( vtxListCount + Max( 1u, stepNb )  );

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

            VertexPCT& vtxCorner = vtxList[*_corner.m_indexVtxCorner];
            setVtxPos( _corner.m_startPos, vtxCorner.m_pos, _corner.m_zVtxCorner);
            vtxCorner.m_uv = *_corner.m_uvCorner;
            vtxCorner.setColor( _corner.m_colorCorner.getAsU32() );

            //  add vertex to create a triangle
            _indexList.push_back(*_corner.m_indexVtxCorner);

            // add two index for next triangle
            _indexList.push_back(*_corner.m_indexVtxCenter);
            _indexList.push_back(*_corner.m_indexVtxCorner);

            *_corner.m_indexVtxCorner += 1;
        }

        // update uv
        _corner.m_uvCorner->x() += uvXStep;
    }

    void Frise::buildVB_Static_CornerSquare( IndexList& _indexList, CornerRounder& _corner )
    {
        ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
        const u32 vtxListCount = vtxList.size();
        vtxList.resize( vtxListCount + 2 );

        f32 uvLeftCorner = getCornerSquareArcBy2( _corner.m_edgeCur->m_cornerAngle, _corner.m_edgeRun->m_coeff, _corner.m_startPos - _corner.m_cornerPos, *_corner.m_uvXsign );
        f32 uvRightCorner = uvLeftCorner;

        if ( !isHeightEqual( *_corner.m_edgeLast, *_corner.m_edgeCur) )
        {
            uvRightCorner = getCornerSquareArcBy2( _corner.m_edgeCur->m_cornerAngle, _corner.m_edgeRun->m_coeff, _corner.m_stopPos - _corner.m_cornerPos, *_corner.m_uvXsign );
        }    

        // update uvs
        updateEdgeCornerUvs( _corner, vtxList[*_corner.m_indexVtxCorner].m_uv.x(), uvLeftCorner, uvRightCorner);

        *_corner.m_indexVtxCorner = (u16)vtxListCount;        

        // update uv
        _corner.m_uvCorner->x() += uvLeftCorner;

        VertexPCT& vtxCorner = vtxList[*_corner.m_indexVtxCorner];
        setVtxPos( _corner.m_cornerPos, vtxCorner.m_pos, _corner.m_zVtxCorner);
        vtxCorner.m_uv = *_corner.m_uvCorner;
        vtxCorner.setColor( _corner.m_colorCorner.getAsU32() );

        //  add vertex to create a triangle
        _indexList.push_back(*_corner.m_indexVtxCorner);

        // add two index for next triangle
        _indexList.push_back(*_corner.m_indexVtxCenter);
        _indexList.push_back(*_corner.m_indexVtxCorner);

        *_corner.m_indexVtxCorner += 1;

        // update uv
        _corner.m_uvCorner->x() += uvRightCorner;
    }

    void Frise::buildVB_Static_Corner( IndexList& _indexList, CornerRounder& _corner )
    { 
        if ( _corner.m_edgeCur->m_cornerAngle == 0.f )
            return;

        ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;        

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
        }

        _indexList.push_back(*_corner.m_indexVtxCenter);
        _indexList.push_back(*_corner.m_indexVtxCorner);

        if ( _corner.m_config->m_isSmoothVisual )
        {
            if ( isHeightEqual( *_corner.m_edgeLast, *_corner.m_edgeCur))
            {
                buildVB_Static_CornerRounder( _indexList, _corner );
            }
            else
            {
                buildVB_Static_CornerRounderBlending( _indexList, _corner );
            }
        }
        else
        {
            buildVB_Static_CornerSquare( _indexList, _corner );
        }

        // add last vertex to create last triangle
        VertexPCT& vtx = vtxList[*_corner.m_indexVtxCorner];
        setVtxPos( _corner.m_stopPos, vtx.m_pos, _corner.m_zVtxCorner);
        vtx.m_uv = *_corner.m_uvCorner;
        vtx.setColor(_corner.m_colorCorner.getAsU32() );

        _indexList.push_back(*_corner.m_indexVtxCorner);
    }

    void Frise::buildVB_Static_EdgeRunCorner( const FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, IndexList& _indexList, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32& _uvUpXOffSet, f32& _uvDownXOffSet, f32 _uvXsign )
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
                    buildVB_Static_addVtxOnStartPosEdge( config, *corner.m_edgeLast, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown );
                }

                // update quad uv 
                updateUvs( _edgeRun, corner.m_edgeLast, _uvUp, _uvDown, _uvXsign, _uvUpXOffSet, _uvDownXOffSet );

                ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
                const u32 vtxListCount = vtxList.size();
                vtxList.resize( vtxListCount + 2 );

                VertexPCT& vtxDown  = vtxList[ vtxListCount ];                
                VertexPCT& vtxUp    = vtxList[ vtxListCount +1 ];

                if ( isEdgeWithHoleVisual(*corner.m_edgeCur))
                {
                    Vec2d posDown, posUp;
                    setPosStopOnEdgeWithHoleVisual( config, *corner.m_edgeLast, posDown, posUp, corner.m_edgeCur->m_idPoint);
                    setVtxPosDown( posDown, vtxDown.m_pos);
                    setVtxPosUp( posUp, vtxUp.m_pos);
                }
                else
                {
                    setVtxBorderRightPosDown( *corner.m_edgeLast, vtxDown.m_pos);
                    setVtxBorderRightPosUp( *corner.m_edgeLast, vtxUp.m_pos);
                }

                vtxDown.m_uv = _uvDown;
                vtxDown.setColor(_vtxColorDown);

                vtxUp.m_uv = _uvUp;
                vtxUp.setColor(_vtxColorUp);

                addIndex_Static_Quad( _indexList, _indexVtxDown, _indexVtxUp, corner.m_edgeCur->m_cornerAngle <0.f/*flipEdge(_uvDownXOffSet-_uvUpXOffSet)*/ );

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
                buildVB_Static_addVtxOnStopPosEdge( *corner.m_edgeLast, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown );
            }

            buildVB_Static_Corner( _indexList, corner);

            corner.m_edgeLast = corner.m_edgeCur;
        }

        if ( isEdgeWithHoleVisual(*corner.m_edgeLast))
        {
            updateUvs( _edgeRun, corner.m_edgeLast, _uvUp, _uvDown, _uvXsign, _uvUpXOffSet, _uvDownXOffSet );
        }
    }

    void Frise::buildVB_Static_addVtxOnStartPosEdge( const FriseConfig* _config, const edgeFrieze& _edge, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, edgeFrieze* _edgeLast )
    {
        ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
        const u32 vtxListCount = vtxList.size();
        vtxList.resize( vtxListCount + 2 );

        m_pRecomputeData->m_edgeRunStartBuildVB = btrue;

        _indexVtxDown = (u16)vtxListCount;
        _indexVtxUp = _indexVtxDown +1;

        VertexPCT& vtxDown  = vtxList[_indexVtxDown];            
        VertexPCT& vtxUp    = vtxList[_indexVtxUp];

        if ( _edgeLast && isEdgeWithHoleVisual(*_edgeLast) )
        {
            Vec2d posDown, posUp;
            setPosStartOnEdgeWithHoleVisual( _config, _edge, posDown, posUp);

            setVtxPosDown( posDown, vtxDown.m_pos);
            setVtxPosUp( posUp, vtxUp.m_pos);
        }
        else
        {
            setVtxBorderLeftPosDown( _edge, vtxDown.m_pos);
            setVtxBorderLeftPosUp( _edge, vtxUp.m_pos);
        }

        vtxDown.m_uv = _uvDown;
        vtxDown.setColor( _vtxColorDown);

        vtxUp.m_uv = _uvUp;
        vtxUp.setColor(_vtxColorUp);
    }

    void Frise::buildVB_Static_addVtxOnStopPosEdge( const edgeFrieze& edge, u32 _vtxColorUp, u32 _vtxColorDown, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown )
    {
        m_pRecomputeData->m_edgeRunStartBuildVB = btrue;

        ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
        const u32 vtxListCount = vtxList.size();
        vtxList.resize( vtxListCount + 2 );

        _indexVtxDown = (u16)vtxListCount;
        _indexVtxUp = _indexVtxDown +1;

        VertexPCT& vtxDown  = vtxList[_indexVtxDown];            
        VertexPCT& vtxUp    = vtxList[_indexVtxUp];

        setVtxBorderRightPosDown( edge, vtxDown.m_pos);
        vtxDown.m_uv = _uvDown;
        vtxDown.setColor( _vtxColorDown);

        setVtxBorderRightPosUp( edge, vtxUp.m_pos);
        vtxUp.m_uv = _uvUp;
        vtxUp.setColor( _vtxColorUp);
    }

    void Frise::buildVB_Static_EdgeRun( const FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, IndexList& _indexList, u32 _vtxColorUp, u32 _vtxColorDown, u32 _vtxColorUpExtremity, u32 _vtxColorDownExtremity, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32 _uvXsign )
    {
        if (buildVB_Static_EdgeRunMesh(config, _edgeList, _edgeRun, _indexList, _vtxColorUp, _vtxColorDown, _vtxColorUpExtremity, _vtxColorDownExtremity, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown, _uvXsign))
            return;

        ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;

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

                        const u32 vtxListCount = vtxList.size();
                        vtxList.resize( vtxListCount +2 );

                        _indexVtxDown = (u16)vtxListCount;
                        _indexVtxUp = _indexVtxDown +1;

                        VertexPCT& vtxDown  = vtxList[_indexVtxDown];            
                        VertexPCT& vtxUp    = vtxList[_indexVtxUp];

                        Vec2d posDown, posUp;
                        setPosStartOnEdgeWithHoleVisual( config, *edgeCur, posDown, posUp);

                        setVtxPosDown( posDown, vtxDown.m_pos);
                        vtxDown.m_uv = _uvDown;
                        vtxDown.setColor( _vtxColorDown);

                        setVtxPosUp( posUp, vtxUp.m_pos);
                        vtxUp.m_uv = _uvUp;
                        vtxUp.setColor( _vtxColorUp);
                    }
                }

                if ( !build )
                    buildVB_Static_addVtxOnStartPosEdge( config, *edgeCur, _vtxColorUpExtremity, _vtxColorDownExtremity, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown );
            }
        }

        //  corner rounder buid way
        buildVB_Static_EdgeRunCorner( config, _edgeList, _edgeRun, _indexList, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown, uvUpXOffSet, uvDownXOffSet, _uvXsign );

        // build quad edge last
        const u32 idEdgeStop = ( _edgeRun.m_idEdgeStart + _edgeRun.m_edgeCount +m_pRecomputeData->m_edgeListCount -1) % m_pRecomputeData->m_edgeListCount;
        const edgeFrieze* edgeStop = &_edgeList[idEdgeStop];

        if ( !edgeStop->m_snap && !isEdgeWithHoleVisual(*edgeStop))
        {
            if ( !m_pRecomputeData->m_edgeRunStartBuildVB)
            {
                edgeFrieze* edgeLast = NULL;

                if ( idEdgeStop >0 || m_pointsList.isLooping() )
                {
                    u32 idEdgeLast = ( idEdgeStop +m_pRecomputeData->m_edgeListCount -1) % m_pRecomputeData->m_edgeListCount;
                    edgeLast = &_edgeList[idEdgeLast];
                }

                buildVB_Static_addVtxOnStartPosEdge( config, *edgeStop, _vtxColorUp, _vtxColorDown, _indexVtxUp, _indexVtxDown, _uvUp, _uvDown, edgeLast );
            }

            Vec2d posDown(edgeStop->m_points[2]);
            Vec2d posUp = posDown +getEdgeBorderRight( *edgeStop);

            if ( idEdgeStop < m_pRecomputeData->m_edgeListCount -1 || m_pointsList.isLooping() )
            {
                u32 idEdgeNext = ( idEdgeStop +1) % m_pRecomputeData->m_edgeListCount;
                const edgeFrieze& edgeNext = _edgeList[idEdgeNext];
                if ( isEdgeWithHoleVisual(edgeNext))
                {
                    setPosStopOnEdgeWithHoleVisual( config, *edgeStop, posDown, posUp, edgeNext.m_idPoint);
                }
            }

            const u32 vtxListCount = vtxList.size();
            vtxList.resize( vtxListCount +2 );

            VertexPCT& vtxDown  = vtxList[ vtxListCount ];            
            VertexPCT& vtxUp    = vtxList[ vtxListCount +1 ];

            // update uv 
            f32 uvXQuadOffSet = edgeStop->m_normUv * _edgeRun.m_coeff *_uvXsign;
            _uvUp.x() += uvXQuadOffSet + uvUpXOffSet;
            _uvDown.x() += uvXQuadOffSet + uvDownXOffSet;

            setVtxPosDown( posDown, vtxDown.m_pos);
            vtxDown.m_uv = _uvDown;
            vtxDown.setColor(_vtxColorDownExtremity); 

            setVtxPosUp( posUp, vtxUp.m_pos);
            vtxUp.m_uv = _uvUp;
            vtxUp.setColor(_vtxColorUpExtremity);

            addIndex_Static_Quad( _indexList, _indexVtxDown, _indexVtxUp, flipEdge(uvUpXOffSet-uvDownXOffSet) );
        }
    }

    void Frise::addIndex_Static_Quad( IndexList& _indexList, u16& _indexVtxDown, u16& _indexVtxUp, bbool _flip )
    {
        /*
        Flip     !Flip  
        1__3     1__3              
        |\ |     | /|   
        |_\|     |/_|   
        0  2     0  2   
        */

        ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
        const u32 vtxListCount = vtxList.size();

        u16 _vertexCount = (u16)vtxListCount;

        _indexList.push_back(_indexVtxDown);
        _indexList.push_back(_indexVtxUp);

        setFlipQuadWithAlphaBorder( _flip, vtxList[_indexVtxDown].m_color, vtxList[_vertexCount -2].m_color);

        if ( _flip )
        {
            _indexVtxDown = _vertexCount -2;
            _indexList.push_back(_indexVtxDown);
            _indexList.push_back(_indexVtxUp);

            _indexVtxUp = _vertexCount -1;
            _indexList.push_back(_indexVtxUp);
            _indexList.push_back(_indexVtxDown);
        }
        else
        {
            _indexVtxUp = _vertexCount -1;
            _indexList.push_back(_indexVtxUp);
            _indexList.push_back(_indexVtxDown);
            _indexList.push_back(_indexVtxUp);

            _indexVtxDown = _vertexCount -2;
            _indexList.push_back(_indexVtxDown);
        }
    }

    void Frise::buildVB_Static_Quad( IndexList& _indexList, const Vec2d* _pos, const Vec2d* _uv, const u32* _color, bbool _flip, const f32* _zExtrude /*= NULL*/ )
    {
        /*
        v1...v3
        .    .
        .    .
        v0...v2
        */
        if (buildVB_Static_QuadMeshAlone(_pos, _uv))
            return;

        ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
        const u32 vtxListCount = vtxList.size();
        vtxList.resize( vtxListCount + 4 );

        u16 indexVtxDown = (u16)vtxListCount;
        u16 indexVtxUp = indexVtxDown +1;        

        VertexPCT& vtx0 = vtxList[ indexVtxDown ];
        setVtxPosDown( _pos[0], vtx0.m_pos);
        vtx0.setColor(_color[0]);
        vtx0.m_uv = _uv[0];            

        VertexPCT& vtx1 = vtxList[ indexVtxUp ];
        setVtxPosUp( _pos[1], vtx1.m_pos);
        vtx1.setColor(_color[1]);
        vtx1.m_uv = _uv[1];

        VertexPCT& vtx2 = vtxList[ indexVtxDown +2];
        setVtxPosDown( _pos[2], vtx2.m_pos);
        vtx2.setColor(_color[2]);
        vtx2.m_uv = _uv[2];

        VertexPCT& vtx3 = vtxList[ indexVtxUp +2];
        setVtxPosUp( _pos[3], vtx3.m_pos);
        vtx3.setColor(_color[3]);
        vtx3.m_uv = _uv[3];

        if ( _zExtrude )
        {
            vtx0.m_pos.z() += _zExtrude[0];
            vtx1.m_pos.z() += _zExtrude[1];
            vtx2.m_pos.z() += _zExtrude[2];
            vtx3.m_pos.z() += _zExtrude[3];
        }

        addIndex_Static_Quad( _indexList, indexVtxDown, indexVtxUp, _flip );
    }


} // namespace ITF
