#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    f32 step_correspondanceTexture  = MTH_PIBY8;
    f32 angleBreak = MTH_PIBY6;

    static f32 angleRoundness[] = 
    {   
        3.f * MTH_PIBY4,
        3.f * MTH_PIBY4,
        MTH_PIBY2,
        MTH_PIBY4,        
        0.f,
        MTH_PIBY4, 
        MTH_PIBY2,
        3.f * MTH_PIBY4,
        3.f * MTH_PIBY4,
    };

    static const f32 uvOffSet = cosf(MTH_PIBY4) * 0.25f;

    struct UvsByAngle_InRoundness
    {
        UvsByAngle_InRoundness(){}

        UvsByAngle_InRoundness(const Vec2d& _center, const Vec2d& _start, const Vec2d& _stop)
        {
            m_center      = _center;
            m_start       = _start;
            m_stop        = _stop;
        }

        Vec2d   m_center;
        Vec2d   m_start;
        Vec2d   m_stop;
    };

    static UvsByAngle_InRoundness uvLeft[]=
    {
        UvsByAngle_InRoundness( Vec2d(0.75f,0.75f),  Vec2d(0.75f -uvOffSet,0.75f +uvOffSet), Vec2d(0.75f,0.50f) ),
        UvsByAngle_InRoundness( Vec2d(0.75f,0.75f),  Vec2d(0.75f -uvOffSet,0.75f +uvOffSet), Vec2d(0.75f,0.50f) ),
        UvsByAngle_InRoundness( Vec2d(0.25f,0.50f),  Vec2d(0.00f,0.50f),                     Vec2d(0.25f,0.25f) ),
        UvsByAngle_InRoundness( Vec2d(0.25f,0.75f),  Vec2d(0.25f -uvOffSet,0.75f -uvOffSet), Vec2d(0.25f,0.50f) ),
        UvsByAngle_InRoundness( Vec2d::Zero,         Vec2d::Zero,                            Vec2d::Zero        ),
        UvsByAngle_InRoundness( Vec2d(0.75f,0.75f),  Vec2d(0.75f,1.00f), Vec2d(0.75f -uvOffSet,0.75f +uvOffSet) ),
        UvsByAngle_InRoundness( Vec2d(0.50f,0.25f),  Vec2d(0.50f,0.50f), Vec2d(0.25f,0.25f) ),
        UvsByAngle_InRoundness( Vec2d(0.25f,0.75f),  Vec2d(0.25f,1.00f), Vec2d(0.25f -uvOffSet,0.75f -uvOffSet) ),
        UvsByAngle_InRoundness( Vec2d(0.25f,0.75f),  Vec2d(0.25f,1.00f), Vec2d(0.25f -uvOffSet,0.75f -uvOffSet) ),
    };

    static UvsByAngle_InRoundness uvRight[]=
    {        
        UvsByAngle_InRoundness( Vec2d(0.25f,0.75f),  Vec2d(0.25f,0.50f), Vec2d(0.25f +uvOffSet,0.75f +uvOffSet) ),
        UvsByAngle_InRoundness( Vec2d(0.25f,0.75f),  Vec2d(0.25f,0.50f), Vec2d(0.25f +uvOffSet,0.75f +uvOffSet) ),
        UvsByAngle_InRoundness( Vec2d(0.75f,0.50f),  Vec2d(0.75f,0.25f), Vec2d(1.00f,0.50f) ),
        UvsByAngle_InRoundness( Vec2d(0.75f,0.75f),  Vec2d(0.75f,0.50f), Vec2d(0.75f +uvOffSet,0.75f -uvOffSet) ),
        UvsByAngle_InRoundness( Vec2d::Zero,         Vec2d::Zero,        Vec2d::Zero        ),
        UvsByAngle_InRoundness( Vec2d(0.25f,0.75f),  Vec2d(0.25f +uvOffSet,0.75f +uvOffSet), Vec2d(0.25f,1.00f) ),
        UvsByAngle_InRoundness( Vec2d(0.50f,0.25f),  Vec2d(0.75f,0.25f),                     Vec2d(0.50f,0.50f) ),
        UvsByAngle_InRoundness( Vec2d(0.75f,0.75f),  Vec2d(0.75f +uvOffSet,0.75f -uvOffSet), Vec2d(0.75f,1.00f) ),
        UvsByAngle_InRoundness( Vec2d(0.75f,0.75f),  Vec2d(0.75f +uvOffSet,0.75f -uvOffSet), Vec2d(0.75f,1.00f) ),
    };

    u32 Frise::buildRoundness_getIndexUv(f32 _angle) const
    {
        if ( _angle >= 0 )
            _angle =  Max( _angle, MTH_PIBY6 );
        else
            _angle =  Min( _angle, -MTH_PIBY6 );

        u32 indexRoundness = u32( 8.f -floor( 0.5f + 8.f *( MTH_PI +_angle ) /MTH_2PI ) );
#ifdef DEVELOPER_JAY_FRIEZE
        ITF_ASSERT(indexRoundness<9 && indexRoundness!=4);
#endif

        return indexRoundness;
    }

    i32 Frise::getTexIdBySlope_InRoundness( Vec2d _sight, const FriseConfig* _pConfig ) const
    {
        f32 angle = atan2( _sight.y(), _sight.x()) +MTH_PI;

        u32 indexTexture = u32(floor( 0.5f +angle /step_correspondanceTexture) );

        return _pConfig->m_FriseTextureConfigIndexBySlope[indexTexture];
    }

    u32 Frise::getIdStartEdge_InRoundness( ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* _pConfig ) const
    {
        if ( !m_pointsList.isLooping() )
            return 0;

        if ( _edgeList[0].m_cornerAngle >= angleBreak || _edgeList[0].m_cornerAngle <= -angleBreak )
            return 0;

        i32 idTexEdgeStart = getTexIdBySlope_InRoundness( _edgeList[0].m_sightNormalized, _pConfig );

        for (u32 i=1; i<m_pRecomputeData->m_edgeListCount; i++)
        {
            if ( _edgeList[i].m_cornerAngle >= angleBreak || _edgeList[i].m_cornerAngle <= -angleBreak )
                return i;

            if ( idTexEdgeStart != getTexIdBySlope_InRoundness( _edgeList[i].m_sightNormalized, _pConfig ) )
                return i;
        }

        return 0;
    } 

    void Frise::buildFrieze_InRoundness( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {
        copyEdgeFromPolyline(_edgeList);

        m_pRecomputeData->m_texRatio = 0.25f;
        m_pRecomputeData->m_flexibility = 0.f;

        buildEdgeList( _edgeList);

        if ( m_pRecomputeData->m_edgeListCount == 0 )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        if ( !buildEdgeRunList_InRoundness( _edgeList, _edgeRunList ) )
            return;

        if ( !setEdgeRunListCoeffUv( _edgeList, _edgeRunList ) )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeRun( _edgeRunList );             
#endif

        buildVB_InRoundness_Static(_edgeList, _edgeRunList );      
    }

    bbool Frise::isAngleBreak_InRoundness( edgeFrieze& _edge ) const
    {
        return _edge.m_cornerAngle <= -angleBreak || _edge.m_cornerAngle >= angleBreak;
    }

    bbool Frise::buildEdgeRunList_InRoundness( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return bfalse;

        _edgeRunList.reserve(m_pRecomputeData->m_edgeListCount);

        edgeRun edgeRunCur;
        edgeRunCur.m_edgeCount = m_pRecomputeData->m_edgeListCount;
       
        if ( getFirstNoFillTextureIndex( config ) == -1 )
        {
            _edgeRunList.push_back(edgeRunCur);
            return bfalse;
        }

        u32 idEdgeStart = getIdStartEdge_InRoundness(_edgeList, config );
        i32 idTex = getTexIdBySlope_InRoundness( _edgeList[idEdgeStart].m_sightNormalized, config );
        
        for ( u32 countEdge=0, idCurEdge=idEdgeStart; countEdge<m_pRecomputeData->m_edgeListCount; )
        {
            edgeRunCur.m_idEdgeStart = idCurEdge;            
            edgeRunCur.m_idTex = idTex;

            ++countEdge;
            ++idCurEdge;
            edgeRunCur.m_edgeCount = 1;

            for( ; countEdge<m_pRecomputeData->m_edgeListCount; countEdge++, idCurEdge++ )
            {
                idCurEdge = idCurEdge % m_pRecomputeData->m_edgeListCount;

                idTex = getTexIdBySlope_InRoundness( _edgeList[idCurEdge].m_sightNormalized, config );

                if ( edgeRunCur.m_idTex != idTex || isAngleBreak_InRoundness( _edgeList[idCurEdge] ) )
                    break;

                edgeRunCur.m_edgeCount++;
            }

            _edgeRunList.push_back(edgeRunCur);
        }

        return btrue;
    }

    void Frise::buildVB_InRoundness_Static( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return;

        u16 indexVtxUp = 0;
        u16 indexVtxDown = 0;

        const u32 edgeRunListCount = _edgeRunList.size();
        for( u32 idEdgeRun = 0; idEdgeRun < edgeRunListCount; idEdgeRun ++ )
        {
            const edgeRun& edgeRunCur = _edgeRunList[idEdgeRun];

            if ( edgeRunCur.m_idTex == -1 )
                continue;

            // Index list
            IndexList& curIndexList = m_meshBuildData->m_staticIndexList[edgeRunCur.m_idTex];

            //  colors vertexs 
            u32 vtxColorDown, vtxColorUp;
            setVtxColor( config->m_textureConfigs[edgeRunCur.m_idTex], vtxColorUp, vtxColorDown ); 

            // start edge
            edgeFrieze& edge = _edgeList[edgeRunCur.m_idEdgeStart];

            // build roundness on start edge
            if ( edge.m_cornerAngle != 0.f )
            {
                // edgeRun Last
                u32 idEdgeRunLast = ( idEdgeRun +edgeRunListCount -1 ) %edgeRunListCount;
                const edgeRun& edgeRunLast = _edgeRunList[idEdgeRunLast];

                // edge Last
                u32 idEdgeLast = ( edgeRunLast.m_idEdgeStart +edgeRunLast.m_edgeCount +m_pRecomputeData->m_edgeListCount -1 ) %m_pRecomputeData->m_edgeListCount;
                const edgeFrieze& edgeLast = _edgeList[idEdgeLast];

                //  colors vertexs edgeRun Last
                u32 vtxColorUpLast = 0;
                u32 vtxColorDownLast = 0;

                if ( edgeRunLast.m_idTex != -1 )
                {
                    // down
                    vtxColorDownLast = config->m_textureConfigs[edgeRunLast.m_idTex].m_color.getAsU32();

                    // up
                    vtxColorUpLast = ITFCOLOR_SET_A(config->m_textureConfigs[edgeRunLast.m_idTex].m_color.getAsU32(), config->m_textureConfigs[edgeRunLast.m_idTex].m_alphaUp);
                }

                if ( edge.m_cornerAngle >0.f )
                {    
                    Vec2d  posStart = edgeLast.m_points[3];
                    Vec2d  posStop = edge.m_points[1];;

                    // roundness end edge last
                    if ( edgeRunLast.m_idTex != -1 )
                        buildRoundness_Static( config, m_meshBuildData->m_staticIndexList[edgeRunLast.m_idTex], uvRight, edge.m_interDown, posStart, posStop, edge.m_cornerAngle, vtxColorDownLast, vtxColorUpLast );                   

                    // roundness start edge cur
                    buildRoundness_Static( config, curIndexList, uvLeft, edge.m_interDown, posStart, posStop, edge.m_cornerAngle, vtxColorDown, vtxColorUp );
                }
                else
                {    
                    Vec2d posStart = edge.m_points[0];
                    Vec2d posStop = edgeLast.m_points[2];

                    // roundness end edge last
                    if ( edgeRunLast.m_idTex != -1 )
                        buildRoundness_Static( config, m_meshBuildData->m_staticIndexList[edgeRunLast.m_idTex], uvRight, edge.m_interUp, posStart, posStop, edge.m_cornerAngle, vtxColorUpLast, vtxColorDownLast );

                    // roundness start edge cur
                    buildRoundness_Static( config, curIndexList, uvLeft, edge.m_interUp, posStart, posStop, edge.m_cornerAngle, vtxColorUp, vtxColorDown );
                }
            }

            // build VB edgeRun
            Vec2d uvUp( getUvX_Start(), 0.f );
            Vec2d uvDown( uvUp.x(), 0.25f );
            buildVB_Static_EdgeRun( config, _edgeList, edgeRunCur, curIndexList, vtxColorUp, vtxColorDown, vtxColorUp, vtxColorDown, indexVtxUp, indexVtxDown, uvUp, uvDown, m_pRecomputeData->m_uvXsign );             
        }
    }

    void Frise::buildRoundness_Static( const FriseConfig* config, IndexList& _indexList, UvsByAngle_InRoundness* _uvTab, Vec2d& _vCenter, Vec2d& _vStart, Vec2d& _vStop, f32 _angle, u32 _vtxColorCenter, u32 _vtxColorCorner )
    {   
        ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;

        //  uvs
        const u32 indexRoundness = buildRoundness_getIndexUv(_angle);
      
        const u32 vtxListCount = vtxList.size();
        const u16 indexVtxCenter = (u16)vtxListCount;
        u16 indexVtxCorner = indexVtxCenter +1;

        f32 zVtxCenter;
        f32 zVtxCorner;

        if ( _angle > 0.f )
        {
            zVtxCenter = m_pRecomputeData->m_zVtxDown;
            zVtxCorner = m_pRecomputeData->m_zVtxUp;
        }
        else
        {
            zVtxCenter = m_pRecomputeData->m_zVtxUp;
            zVtxCorner = m_pRecomputeData->m_zVtxDown;
        }

        //  step count
        _angle = f32_Abs(_angle);
        u32 stepCount = getCornerStepNb( _angle, config->m_smoothFactorVisual, 1.f);

        // grow list
        vtxList.resize( vtxListCount + stepCount + 2 );

        //  vertex center 
        VertexPCT& vtxCenter = vtxList[indexVtxCenter];
        vtxCenter.m_pos.x() = _vCenter.x();
        vtxCenter.m_pos.y() = _vCenter.y();
        vtxCenter.m_pos.z() = zVtxCenter;
        vtxCenter.m_uv = _uvTab[indexRoundness].m_center;
        vtxCenter.setColor(_vtxColorCenter);

        //  vertex start 
        VertexPCT& vtxStart = vtxList[indexVtxCorner];
        vtxStart.m_pos.x() = _vStart.x();
        vtxStart.m_pos.y() = _vStart.y();
        vtxStart.m_pos.z() = zVtxCorner;
        vtxStart.m_uv = _uvTab[indexRoundness].m_start;
        vtxStart.setColor(_vtxColorCorner);

        _indexList.push_back(indexVtxCenter);
        _indexList.push_back(indexVtxCorner);
        indexVtxCorner++;

        if ( stepCount > 1 )
        {
            Vec2d uvRot  = _uvTab[indexRoundness].m_start;

            f32 stepFan = -_angle /(f32)stepCount;               
            f32 stepUv = angleRoundness[indexRoundness] / (f32)stepCount;

            f32 fanRotCos, fanRotSin;
            f32_CosSin(stepFan, &fanRotCos, &fanRotSin);

            f32 uvRotCos, uvRotSin;
            f32_CosSin(stepUv, &uvRotCos, &uvRotSin);

            Vec2d vCorner = _vStart;
            for ( u32 i = 1; i < stepCount; i++ )
            {
                // rotation
                vCorner = vCorner.RotateAroundCS(_vCenter, fanRotCos, fanRotSin);
                uvRot = uvRot.RotateAroundCS( _uvTab[indexRoundness].m_center, uvRotCos, uvRotSin );

                VertexPCT& vtxCorner = vtxList[indexVtxCorner];                
                vtxCorner.m_pos.x() = vCorner.x();
                vtxCorner.m_pos.y() = vCorner.y();
                vtxCorner.m_pos.z() = zVtxCorner;
                vtxCorner.m_uv = uvRot;
                vtxCorner.setColor( _vtxColorCorner);

                //  add vertex to create a triangle
                _indexList.push_back(indexVtxCorner);

                // add two index for next triangle
                _indexList.push_back(indexVtxCenter);
                _indexList.push_back(indexVtxCorner);

                indexVtxCorner++;
            }
        }

        //  vertex stop
        VertexPCT& vtxStop = vtxList[indexVtxCorner];
        vtxStop.m_pos.x() = _vStop.x();
        vtxStop.m_pos.y() = _vStop.y();
        vtxStop.m_pos.z() = zVtxCorner;
        vtxStop.m_uv = _uvTab[indexRoundness].m_stop;
        vtxStop.setColor( _vtxColorCorner);

        //  add vertex to create last triangle
        _indexList.push_back(indexVtxCorner);
    }

    void Frise::buildCollision_InRoundness( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {        
        if ( !buildCollisionRunList( _edgeList, _edgeRunList, _collisionRunList ) )
            return;
        
        buildCollision_EdgeRunList ( _edgeList, _edgeRunList, _collisionRunList );

        finalizeCollisionList();
    }

    bbool Frise::isEdgeRun_InRoundness( ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* _config, u32 _idCurEdge, u32 _idLastEdge, u32 _idLastLastEdge ) const
    {      
        i32 texIdLastEdge = getTexIdBySlope_InRoundness( _edgeList[_idLastEdge].m_sightNormalized, _config );        

        if ( getTexIdBySlope_InRoundness( _edgeList[_idLastLastEdge].m_sightNormalized, _config ) == texIdLastEdge && !isAngleBreak_InRoundness( _edgeList[_idLastEdge]) )
            return bfalse;

        if ( getTexIdBySlope_InRoundness( _edgeList[_idCurEdge].m_sightNormalized, _config ) == texIdLastEdge && !isAngleBreak_InRoundness( _edgeList[_idCurEdge]) )
            return bfalse;

        return btrue;
    }

} // namespace ITF
