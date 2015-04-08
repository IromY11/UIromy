#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    static i32 idTexConfig_InPipePatch;

    static const f32 s_coeffOffsetExtremity = 0.25f;
    static const f32 s_coeffOffsetCornerExtremity = 0.5f;

    static const u32 idTopZoneUv       = 0;
    static const u32 idRightZoneUv     = 1;
    static const u32 idLeftZoneUv      = 2;
    static const u32 idBottomZoneUv    = 3;

    // choix de la bande d'uv sur un edge
    struct idZoneByAngle_InPipePatch
    {
        idZoneByAngle_InPipePatch( f32 _start, f32 _stop, u32 _index )
        {
            m_start = _start;
            m_stop  = _stop;
            m_index = _index;
        }

        f32 m_start;
        f32 m_stop;
        u32 m_index;
    };

    static idZoneByAngle_InPipePatch indexZoneByAngle[]=
    {
        idZoneByAngle_InPipePatch( -MTH_PIBY4,         MTH_PIBY4       , idTopZoneUv ),       // TOP
        idZoneByAngle_InPipePatch( -3.f *MTH_PIBY4,    -MTH_PIBY4      , idRightZoneUv ),     // RIGHT
        idZoneByAngle_InPipePatch( MTH_PIBY4,          3.f *MTH_PIBY4  , idLeftZoneUv ),      // LEFT
        idZoneByAngle_InPipePatch( 3.f *MTH_PIBY4,     MTH_PI          , idBottomZoneUv ),    // BOTTOM
        idZoneByAngle_InPipePatch( -MTH_PI,            -3.f *MTH_PIBY4 , idBottomZoneUv ),    // BOTTOM        
    };

    //  settings de chaque zone
    static UvLinear_InPipePatch uv[]=
    {
        // TOP RIGHT LEFT AND BOTTOM have all the same params for this methode

        // TOP
        UvLinear_InPipePatch( 0.5f,   0.f,    1.f,
        Vec2d( 0.25f, 1.f ),   Vec2d( 0.25f, 0.5f ),     Vec2d( 0.5f, 1.f ), Vec2d( 0.5f, 0.5f ), // CORNER
        Vec2d( 0.f, 1.f ),  Vec2d( 0.f, 0.5f ), Vec2d( 0.125f, 1.f ),   Vec2d( 0.125f, 0.5f ), // EXT START
        Vec2d( 0.125f, 1.f ),  Vec2d( 0.125f, 0.5f ), Vec2d( 0.25f, 1.f ),   Vec2d( 0.25f, 0.5f ),  // EXT STOP
        Vec2d( 0.5f, 1.f ),  Vec2d( 0.5f, 0.5f ), Vec2d( 0.75f, 1.f ),   Vec2d( 0.75f, 0.5f ), // EXT START CORNER
        Vec2d( 0.75f, 1.f ),  Vec2d( 0.75f, 0.5f ), Vec2d( 1.f, 1.f ),   Vec2d( 1.f, 0.5f ) ),  // EXT STOP CORNER

        // RIGHT
        UvLinear_InPipePatch( 0.5f,   0.f,    1.f,
        Vec2d( 0.25f, 1.f ),   Vec2d( 0.25f, 0.5f ),     Vec2d( 0.5f, 1.f ), Vec2d( 0.5f, 0.5f ), // CORNER
        Vec2d( 0.f, 1.f ),  Vec2d( 0.f, 0.5f ), Vec2d( 0.125f, 1.f ),   Vec2d( 0.125f, 0.5f ), // EXT START
        Vec2d( 0.125f, 1.f ),  Vec2d( 0.125f, 0.5f ), Vec2d( 0.25f, 1.f ),   Vec2d( 0.25f, 0.5f ),  // EXT STOP
        Vec2d( 0.5f, 1.f ),  Vec2d( 0.5f, 0.5f ), Vec2d( 0.75f, 1.f ),   Vec2d( 0.75f, 0.5f ), // EXT START CORNER
        Vec2d( 0.75f, 1.f ),  Vec2d( 0.75f, 0.5f ), Vec2d( 1.f, 1.f ),   Vec2d( 1.f, 0.5f ) ),  // EXT STOP CORNER

        // LEFT
        UvLinear_InPipePatch( 0.5f,   0.f,    1.f,
        Vec2d( 0.25f, 1.f ),   Vec2d( 0.25f, 0.5f ),     Vec2d( 0.5f, 1.f ), Vec2d( 0.5f, 0.5f ), // CORNER
        Vec2d( 0.f, 1.f ),  Vec2d( 0.f, 0.5f ), Vec2d( 0.125f, 1.f ),   Vec2d( 0.125f, 0.5f ), // EXT START
        Vec2d( 0.125f, 1.f ),  Vec2d( 0.125f, 0.5f ), Vec2d( 0.25f, 1.f ),   Vec2d( 0.25f, 0.5f ),  // EXT STOP
        Vec2d( 0.5f, 1.f ),  Vec2d( 0.5f, 0.5f ), Vec2d( 0.75f, 1.f ),   Vec2d( 0.75f, 0.5f ), // EXT START CORNER
        Vec2d( 0.75f, 1.f ),  Vec2d( 0.75f, 0.5f ), Vec2d( 1.f, 1.f ),   Vec2d( 1.f, 0.5f ) ),  // EXT STOP CORNER

        // BOTTOM
        UvLinear_InPipePatch( 0.5f,   0.f,    1.f,
        Vec2d( 0.25f, 1.f ),   Vec2d( 0.25f, 0.5f ),     Vec2d( 0.5f, 1.f ), Vec2d( 0.5f, 0.5f ), // CORNER
        Vec2d( 0.f, 1.f ),  Vec2d( 0.f, 0.5f ), Vec2d( 0.125f, 1.f ),   Vec2d( 0.125f, 0.5f ), // EXT START
        Vec2d( 0.125f, 1.f ),  Vec2d( 0.125f, 0.5f ), Vec2d( 0.25f, 1.f ),   Vec2d( 0.25f, 0.5f ),  // EXT STOP
        Vec2d( 0.5f, 1.f ),  Vec2d( 0.5f, 0.5f ), Vec2d( 0.75f, 1.f ),   Vec2d( 0.75f, 0.5f ), // EXT START CORNER
        Vec2d( 0.75f, 1.f ),  Vec2d( 0.75f, 0.5f ), Vec2d( 1.f, 1.f ),   Vec2d( 1.f, 0.5f ) ),  // EXT STOP CORNER

    };

    void Frise::buildFrieze_InPipePatchSimple( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {        
        copyEdgeFromPolyline(_edgeList);           

        if ( !isLooping() )
        {
            const f32 startCoeffOffset = m_switchExtremityStart ? s_coeffOffsetCornerExtremity : s_coeffOffsetExtremity; 
            const f32 stopCoeffOffset = m_switchExtremityStop ? s_coeffOffsetCornerExtremity : s_coeffOffsetExtremity; 

            subtractOffsetExtremity( _edgeList, m_pRecomputeData->m_heightScale * startCoeffOffset, m_pRecomputeData->m_heightScale * stopCoeffOffset );
        }

        buildEdgeList( _edgeList );

        if ( m_pRecomputeData->m_edgeListCount == 0 )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        if ( !buildEdgeRunList_InPipePatch( _edgeList, _edgeRunList ) )
            return;

        transformEdgeList_InPipePatch( _edgeList, _edgeRunList );

        setEdgeRunListNormUv( _edgeList, _edgeRunList );

        buildVB_InPipePatch_Static( _edgeList, _edgeRunList, &uv[0] );  
    }

    void Frise::setNormOffsetRatioAndValidEdge( const FriseConfig* _pConfig, edgeFrieze& _edge, bbool _start, bbool _isSingleEdge, edgeFrieze* _pEdgeNext ) const
    {
        if ( _edge.m_snap )
        {
            _edge.m_snap = bfalse;

            if ( _isSingleEdge )
            {
                _edge.m_heightStart = _edge.m_scaleCur *m_pRecomputeData->m_heightScale;

                if ( _pEdgeNext)
                {
                    _edge.m_heightStop = _pEdgeNext->m_scaleCur *m_pRecomputeData->m_heightScale;
                }
            }

            buildEdgePoints( _pConfig, _edge );
        }

        const f32 length = _edge.m_sight.norm();

        ITF_ASSERT( length > 0.f );

        if ( length > 0.f )
        {
            if ( _start )
                _edge.m_startCoeff = _edge.m_heightStart * _pConfig->m_patchCoeffOffset / length;
            else
                _edge.m_stopCoeff = 1.f - ( _edge.m_heightStop * _pConfig->m_patchCoeffOffset / length );
        }

        _edge.m_norm = Max ( 0.f, length *( _edge.m_stopCoeff - _edge.m_startCoeff ) );
    }

    Vec2d Frise::getAndSetBorderStartOffset_InPipePatch( const FriseConfig* _pConfig, edgeFrieze& _edge, bbool _isSingleEdge, edgeFrieze* _pEdgeNext ) const
    {
        setNormOffsetRatioAndValidEdge( _pConfig, _edge, btrue, _isSingleEdge, _pEdgeNext );

        return _edge.m_sightNormalized * _edge.m_heightStart * _pConfig->m_patchCoeffOffset;
    }

    Vec2d Frise::getAndSetBorderStopOffset_InPipePatch( const FriseConfig* _pConfig, edgeFrieze& _edge, bbool _isSingleEdge, edgeFrieze* _pEdgeNext ) const 
    {        
        setNormOffsetRatioAndValidEdge( _pConfig, _edge, bfalse, _isSingleEdge, _pEdgeNext );

        return -_edge.m_sightNormalized * _edge.m_heightStop * _pConfig->m_patchCoeffOffset;
    }

    void Frise::offsetStartBorderEdgeRun_InPipePatch( const FriseConfig* _pConfig, const edgeRun& _edgeRun, ITF_VECTOR<edgeFrieze>& _edgeList ) const
    {          
        edgeFrieze& edgeStart = _edgeList[ _edgeRun.m_idEdgeStart ];
        const Vec2d edgeHeightStart = edgeStart.m_normal *edgeStart.m_heightStart;
        const bbool isSingleEdgeOnEdgeRun = _edgeRun.m_edgeCount == 1;

        edgeFrieze* pEdgeNext = NULL;
        if ( isSingleEdgeOnEdgeRun )
        {
            if ( _edgeRun.m_idEdgeStart < m_pRecomputeData->m_edgeListCount -1 || isLooping() )
            {
                const u32 idNext = ( _edgeRun.m_idEdgeStart +1 ) % m_pRecomputeData->m_edgeListCount;
                pEdgeNext = &_edgeList[idNext];
            }
        }

        Vec2d& edgeStartPoint0 = edgeStart.m_points[0];
        edgeStartPoint0 = edgeStart.m_pos -edgeHeightStart * _pConfig->m_visualOffset + getAndSetBorderStartOffset_InPipePatch( _pConfig, edgeStart, isSingleEdgeOnEdgeRun, pEdgeNext );
        edgeStart.m_points[1] = edgeStartPoint0 + edgeHeightStart;
    }

    void Frise::offsetStopBorderEdgeRun_InPipePatch( const FriseConfig* _pConfig, const edgeRun& _edgeRun, ITF_VECTOR<edgeFrieze>& _edgeList ) const
    {
        const u32 indexStop = ( _edgeRun.m_idEdgeStart + _edgeRun.m_edgeCount + m_pRecomputeData->m_edgeListCount -1 ) % m_pRecomputeData->m_edgeListCount;
        edgeFrieze& edgeStop = _edgeList[ indexStop ];
        const Vec2d edgeHeightStop = edgeStop.m_normal * edgeStop.m_heightStop;
        const bbool isSingleEdgeOnEdgeRun = _edgeRun.m_edgeCount == 1;

        edgeFrieze* pEdgeNext = NULL;
        if ( isSingleEdgeOnEdgeRun )
        {
            if ( _edgeRun.m_idEdgeStart < m_pRecomputeData->m_edgeListCount -1 || isLooping() )
            {
                const u32 idNext = ( _edgeRun.m_idEdgeStart +1 ) % m_pRecomputeData->m_edgeListCount;
                pEdgeNext = &_edgeList[idNext];
            }
        }

        Vec2d& edgeStopPoint2 = edgeStop.m_points[2];
        edgeStopPoint2 = edgeStop.m_pos + edgeStop.m_sight - edgeHeightStop * _pConfig->m_visualOffset  + getAndSetBorderStopOffset_InPipePatch( _pConfig, edgeStop, isSingleEdgeOnEdgeRun, pEdgeNext );
        edgeStop.m_points[3] = edgeStopPoint2 + edgeHeightStop;
    }

    void Frise::transformEdgeList_InPipePatch( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        const FriseConfig* pConfig = getConfig();
        const u32 edgeRunListCount = _edgeRunList.size();

        u32 idEdgeRunStart = 0;
        u32 idEdgeRunStop = edgeRunListCount -1;

        // offset borders
        if ( !isLooping() )
        {         
            if ( idEdgeRunStop > 0 )
            {
                offsetStopBorderEdgeRun_InPipePatch( pConfig, _edgeRunList[idEdgeRunStart], _edgeList );
                offsetStartBorderEdgeRun_InPipePatch( pConfig, _edgeRunList[idEdgeRunStop], _edgeList );
            }            

            // update index still done
            idEdgeRunStart ++;  

            if ( idEdgeRunStop > 0 )
                idEdgeRunStop --;       
        }

        for ( u32 idEdgeRun = idEdgeRunStart; idEdgeRun <= idEdgeRunStop; idEdgeRun++ )
        {
            const edgeRun& edgeRunCur = _edgeRunList[idEdgeRun];            

            offsetStartBorderEdgeRun_InPipePatch( pConfig, edgeRunCur, _edgeList );
            offsetStopBorderEdgeRun_InPipePatch( pConfig, edgeRunCur, _edgeList );            
        }
    }

    u32 Frise::getZoneId_InPipePatch( const Vec2d& _sight ) const
    {
        switch ( m_lockTexture )
        {
        case LockTexture_Top :
        case LockTexture_Right :
        case LockTexture_Left :
        case LockTexture_Bottom :
            return m_lockTexture;

        default:
            break;
        }

        f32 angle = atan2( _sight.y(), _sight.x() );

        for ( u32 i = 0; i < 5; i++ )
        {
            if ( angle >= indexZoneByAngle[i].m_start && angle <= indexZoneByAngle[i].m_stop )
                return indexZoneByAngle[i].m_index;
        }

        return 0;
    }

    u32 Frise::getIdStartEdge_InPipePatch( const ITF_VECTOR<edgeFrieze>& _edgeList ) const
    {
        if ( !m_pointsList.isLooping() || m_lockTexture == LockTexture_Switch )
            return 0;

        const u32 idZoneEdgeStart = getZoneId_InPipePatch( _edgeList[0].m_sightNormalized );

        for (u32 i=1; i<m_pRecomputeData->m_edgeListCount; i++)
        {
            if ( idZoneEdgeStart != getZoneId_InPipePatch( _edgeList[i].m_sightNormalized ) )
                return i;
        }

        return 0;
    } 

    bbool Frise::buildEdgeRunList_InPipePatch( const ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        const FriseConfig* config = getConfig();

        _edgeRunList.reserve(m_pRecomputeData->m_edgeListCount);

        edgeRun edgeRunCur;
        edgeRunCur.m_edgeCount = m_pRecomputeData->m_edgeListCount; 
        edgeRunCur.m_idTex = getFirstNoFillTextureIndex( config );

        // frieze texture
        idTexConfig_InPipePatch = edgeRunCur.m_idTex;

        if ( idTexConfig_InPipePatch == -1 )
        {
            _edgeRunList.push_back(edgeRunCur);
            return bfalse;
        }

        const u32 idEdgeStart = getIdStartEdge_InPipePatch( _edgeList );
        u32 idZone = getZoneId_InPipePatch( _edgeList[idEdgeStart].m_sightNormalized );  

        for ( u32 countEdge=0, idCurEdge=idEdgeStart; countEdge<m_pRecomputeData->m_edgeListCount; )
        {
            edgeRunCur.m_idEdgeStart = idCurEdge;            
            edgeRunCur.m_idZone = idZone;

            if ( config->m_FriseTextureConfigIndexByZone[edgeRunCur.m_idZone] != -1 )
                edgeRunCur.m_idTex = config->m_FriseTextureConfigIndexByZone[edgeRunCur.m_idZone];
            else
                edgeRunCur.m_idTex = idTexConfig_InPipePatch;

            ++countEdge;
            ++idCurEdge;
            edgeRunCur.m_edgeCount = 1;

            for( ; countEdge<m_pRecomputeData->m_edgeListCount; countEdge++, idCurEdge++ )
            {
                idCurEdge = idCurEdge % m_pRecomputeData->m_edgeListCount;
                const edgeFrieze& edgeCur = _edgeList[idCurEdge];

                idZone = getZoneId_InPipePatch( edgeCur.m_sightNormalized );

                if ( config->m_patchAngleMin.ToRadians() > 0.f )
                {
                    if ( f32_Abs(edgeCur.m_cornerAngle) > config->m_patchAngleMin.ToRadians() )
                    {
                        break;
                    }
                }
                else if ( edgeRunCur.m_idZone != idZone || m_lockTexture == LockTexture_Switch  )
                {
                    break;
                }

                edgeRunCur.m_edgeCount++;
            }

            _edgeRunList.push_back(edgeRunCur);
        }

        return btrue;
    }

    void Frise::setPositionExtremityOnStartEdgeRun_InPipePatch( const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, Vec2d* _pos, const f32 _coeffOffset, const Vec2d& _scale ) const
    {
        const edgeFrieze& edgeStart = _edgeList[ _edgeRun.m_idEdgeStart ];

        const Vec2d center = ( edgeStart.m_points[0] + edgeStart.m_points[1] ) *0.5f;
        const Vec2d perp  = edgeStart.m_sightNormalized.getPerpendicular();

        Vec2d offset( edgeStart.m_points[1] + edgeStart.m_sightNormalized * edgeStart.m_heightStart * _coeffOffset *_scale.x() );
        offset += perp *edgeStart.m_heightStart *0.5f *(_scale.y() -1.f);
        offset -= center;              
                
        const Vec2d deltaVal  = perp*(2*offset.dot(perp));

        // set pos
        _pos[0] = center - offset; 
        _pos[1] = _pos[0] + deltaVal;
        _pos[3] = center + offset;
        _pos[2] = _pos[3] - deltaVal;
    }

    void Frise::setPositionExtremityOnStopEdgeRun_InPipePatch( const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, Vec2d* _pos, const f32 _coeffOffset, const Vec2d& _scale ) const
    {
        const u32 idEdgeStop = ( _edgeRun.m_idEdgeStart + _edgeRun.m_edgeCount + m_pRecomputeData->m_edgeListCount -1 ) % m_pRecomputeData->m_edgeListCount;
        const edgeFrieze& edgeStop = _edgeList[ idEdgeStop ];

        const Vec2d center = ( edgeStop.m_points[2] + edgeStop.m_points[3] ) *0.5f;
        const Vec2d perp  = edgeStop.m_sightNormalized.getPerpendicular();

        Vec2d offset( edgeStop.m_points[3] + edgeStop.m_sightNormalized * edgeStop.m_heightStop * _coeffOffset *_scale.x() );
        offset += perp *edgeStop.m_heightStop *0.5f *(_scale.y() -1.f);
        offset -= center;              

        const Vec2d deltaVal  = perp*(2*offset.dot(perp));

        // set pos
        _pos[0] = center - offset; 
        _pos[1] = _pos[0] + deltaVal;
        _pos[3] = center + offset;
        _pos[2] = _pos[3] - deltaVal;
    }

    void Frise::buildVB_InPipePatch_Static( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, const UvLinear_InPipePatch* uvToApply )
    {
        u32 orderBuildWay = 0; 
        const FriseConfig* pConfig = getConfig();
        const FriseTextureConfig& texConfig = pConfig->m_textureConfigs[idTexConfig_InPipePatch];

        u16 indexVtxUp = 0;
        u16 indexVtxDown = 0;
        u32 indexStartToBuildCorner = 1;

        //  colors vertexs 
        u32 vtxColorDown, vtxColorUp, vtxColorUpExtremity, vtxColorDownExtremity;
        setVtxColorWithExtrem( texConfig, vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity );

        IndexList& indexList = m_meshBuildData->m_staticIndexList[idTexConfig_InPipePatch];
        Vec2d uvUp( Vec2d::Zero );

        // build VB edgeRun
        const u32 edgeRunListCount = _edgeRunList.size();
        for( u32 idEdgeRun = 0; idEdgeRun < edgeRunListCount; idEdgeRun ++ )
        {
            const edgeRun& edgeRunCur = _edgeRunList[idEdgeRun];

            uvUp.y() = uvToApply[edgeRunCur.m_idZone].m_uvYup; // update only y to keep x continuity
            Vec2d uvDown( uvUp.x(), uvToApply[edgeRunCur.m_idZone].m_uvYdown );

            buildVB_Static_EdgeRun( pConfig, _edgeList, edgeRunCur, indexList, vtxColorUp, vtxColorDown, vtxColorUp, vtxColorDown, indexVtxUp, indexVtxDown, uvUp, uvDown, uvToApply[edgeRunCur.m_idZone].m_uvXsign *m_pRecomputeData->m_uvXsign );             
        }

        // build VB extremities
        if ( !isLooping() )
        {
            Vec2d posExtremity[4];
            u32 colorExtremity[4];
            const Vec2d* uvExtremity;
            f32 coeffOffsetExtremity;

            // start extremity
            if ( !isEdgeWithHoleVisual( _edgeList[0] ) )
            {
                const edgeRun& edgeRunStart = _edgeRunList[0];
                              
                if ( m_switchExtremityStart )
                {
                    coeffOffsetExtremity = s_coeffOffsetCornerExtremity;
                    uvExtremity = &uvToApply[ edgeRunStart.m_idZone ].m_uvStartExtCorner[0];
                }
                else
                {
                    coeffOffsetExtremity = s_coeffOffsetExtremity;
                    uvExtremity = &uvToApply[ edgeRunStart.m_idZone ].m_uvStartExt[0];
                }

                setQuadColor_StartExtremity( &colorExtremity[0], vtxColorDown, vtxColorUp, vtxColorDownExtremity, vtxColorUpExtremity );
                setPositionExtremityOnStartEdgeRun_InPipePatch( _edgeList, edgeRunStart, &posExtremity[0], coeffOffsetExtremity, pConfig->m_extremityScale );
                buildVB_Static_Quad( indexList, &posExtremity[0], uvExtremity, &colorExtremity[0], btrue );
            }

            // stop extremity
            if ( !isEdgeWithHoleVisual( _edgeList[ m_pRecomputeData->m_edgeListCount -1] ) )
            {
                const edgeRun& edgeRunStop = _edgeRunList[ edgeRunListCount -1 ];

                if ( m_switchExtremityStop )
                {
                    coeffOffsetExtremity = s_coeffOffsetCornerExtremity;
                    uvExtremity = &uvToApply[ edgeRunStop.m_idZone ].m_uvStopExtCorner[0];
                }
                else
                {
                    coeffOffsetExtremity = s_coeffOffsetExtremity;
                    uvExtremity = &uvToApply[ edgeRunStop.m_idZone ].m_uvStopExt[0];
                }

                setPositionExtremityOnStopEdgeRun_InPipePatch( _edgeList, edgeRunStop, &posExtremity[0], coeffOffsetExtremity, pConfig->m_extremityScale );
                setQuadColor_StopExtremity( &colorExtremity[0], vtxColorDown, vtxColorUp, vtxColorDownExtremity, vtxColorUpExtremity );
                buildVB_Static_Quad( indexList, &posExtremity[0], uvExtremity, &colorExtremity[0], btrue );
            }
        }
        else
        {
            // build corner on start edgeRun
            indexStartToBuildCorner = 0;
        }

        // build corner for each edge run        
        u32 colorCorner[4];
        Vec2d posExtremity[4];
        colorCorner[0] = colorCorner[2] = vtxColorDown;
        colorCorner[1] = colorCorner[3] = vtxColorUp;

        for( ; indexStartToBuildCorner < edgeRunListCount; indexStartToBuildCorner ++ )
        {
            const edgeRun& edgeRunCur = _edgeRunList[indexStartToBuildCorner];
            const edgeFrieze& edgeCur = _edgeList[ edgeRunCur.m_idEdgeStart ];

            const u32 idEdgeLast = ( edgeRunCur.m_idEdgeStart + m_pRecomputeData->m_edgeListCount - 1 ) % m_pRecomputeData->m_edgeListCount;
            const edgeFrieze& edgeLast = _edgeList[ idEdgeLast ];

            const u32 idEdgeRunLast = ( indexStartToBuildCorner + edgeRunListCount -1 ) % edgeRunListCount;
            const edgeRun& edgeRunLast = _edgeRunList[idEdgeRunLast];

            orderBuildWay ++;

            if( orderBuildWay&1 )
            {                
                // build corner stop extremity
                if ( !isEdgeWithHoleVisual( edgeLast ) )
                {
                    setPositionExtremityOnStopEdgeRun_InPipePatch( _edgeList, edgeRunLast, &posExtremity[0], s_coeffOffsetCornerExtremity, Vec2d::One );
                    buildVB_Static_Quad( indexList, &posExtremity[0], &uvToApply[ edgeRunLast.m_idZone ].m_uvStopExtCorner[0], &colorCorner[0], btrue );
                }

                // build corner start extremity
                if ( !isEdgeWithHoleVisual( edgeCur ) )
                {
                    setPositionExtremityOnStartEdgeRun_InPipePatch( _edgeList, edgeRunCur, &posExtremity[0], s_coeffOffsetCornerExtremity, Vec2d::One );
                    buildVB_Static_Quad( indexList, &posExtremity[0], &uvToApply[ edgeRunCur.m_idZone ].m_uvStartExtCorner[0], &colorCorner[0], btrue );
                }                
            }
            else
            {
                // build corner start extremity
                if ( !isEdgeWithHoleVisual( edgeCur ) )
                {
                    setPositionExtremityOnStartEdgeRun_InPipePatch( _edgeList, edgeRunCur, &posExtremity[0], s_coeffOffsetCornerExtremity, Vec2d::One );
                    buildVB_Static_Quad( indexList, &posExtremity[0], &uvToApply[ edgeRunCur.m_idZone ].m_uvStartExtCorner[0], &colorCorner[0], btrue );
                }

                // build corner stop extremity
                if ( !isEdgeWithHoleVisual( edgeLast ) )
                {
                    setPositionExtremityOnStopEdgeRun_InPipePatch( _edgeList, edgeRunLast, &posExtremity[0], s_coeffOffsetCornerExtremity, Vec2d::One );
                    buildVB_Static_Quad( indexList, &posExtremity[0], &uvToApply[ edgeRunLast.m_idZone ].m_uvStopExtCorner[0], &colorCorner[0], btrue );
                }
            }
         
            // build patch
            if ( !isEdgeWithHoleVisual( edgeCur ) || !isEdgeWithHoleVisual( edgeLast ) )
            {
                const f32 offset = edgeCur.m_heightStart * 0.5f * pConfig->m_patchScale;

                Vec2d offset0( -offset, -offset );
                Vec2d offset1( -offset, offset );
                Vec2d offset2( offset, -offset );
                Vec2d offset3( offset, offset );

                if ( pConfig->m_patchOriented )
                {
                    Vec2d cosSin;
                    f32_CosSin( edgeCur.m_cornerNormal.getAngle() - MTH_PIBY2, &cosSin.x(), &cosSin.y() );

                    offset0 = offset0.RotateCS( cosSin );
                    offset1 = offset1.RotateCS( cosSin );
                    offset2 = offset2.RotateCS( cosSin );
                    offset3 = offset3.RotateCS( cosSin );
                }

                Vec2d center;
                getCenterPatch_InPipePatch( edgeCur, center );

                posExtremity[0] = center + offset0;
                posExtremity[1] = center + offset1;
                posExtremity[2] = center + offset2;
                posExtremity[3] = center + offset3;

                buildVB_Static_Quad( indexList, &posExtremity[0], &uvToApply[ edgeRunCur.m_idZone ].m_uvCorner[0], &colorCorner[0], bfalse );
            }
        }
    } 

    void Frise::getCenterPatch_InPipePatch( const edgeFrieze& _edge, Vec2d& _pos ) const
    {
        _pos = _edge.m_pos; // TODO add visual offset
    }

    void Frise::buildCollision_InPipePatchSimple( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {
        if ( !buildCollisionRunList( _edgeList, _edgeRunList, _collisionRunList ) )
            return;

        buildCollision_InPipePatch( _edgeList, _edgeRunList, _collisionRunList );

        finalizeCollisionList();
    }

    void Frise::buildCollision_InPipePatch( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {
        const FriseConfig* pConfig = getConfig();
        const u32 edgeRunListCount = _edgeRunList.size();
        const u32 collisionRunListCount = _collisionRunList.size();

        SafeArray<Vec2d> collision;
        collision.reserve( m_pRecomputeData->m_edgeListCount *3 +3 );

        edgeRun* edgeRunLast = NULL;
        f32 offsetLast = -1.f;

        for ( u32 idCol = 0; idCol < collisionRunListCount; idCol++ )
        {
            collisionRun& colRun = _collisionRunList[idCol];            
            edgeRun* edgeRunCur = &_edgeRunList[colRun.m_idEdgeRunStart];

            // init offset position
            f32 offsetCur = getCollisionOffSet( pConfig, edgeRunCur, colRun);

            const Path* gameMat = getCollisionGameMaterial( pConfig, edgeRunCur->m_idTex );

            ITF_VECTOR<CollisionByGameMat> collisionList;
            CollisionByGameMat collisionByGMat;
            collisionByGMat.m_startId = 0;

            if ( gameMat )
            {
                collisionByGMat.m_gameMat = *gameMat;
            }

            // countOffsets
            u32 edgeCountStartOffset = colRun.m_edgeCountStartOffset;

            // build start points   
            if ( edgeRunCur->m_idTex != -1 && colRun.m_collisionOffsetLast != -1.f )       
            {
                bbool build = bfalse;
                if ( colRun.m_idEdgeRunStart>0 || m_pointsList.isLooping())
                {
                    u32 idEdgeRunLast = ( colRun.m_idEdgeRunStart -1 +edgeRunListCount) % edgeRunListCount;
                    u32 idEdgeLastOfEdgeRunLast = ( _edgeRunList[idEdgeRunLast].m_idEdgeStart +_edgeRunList[idEdgeRunLast].m_edgeCount +m_pRecomputeData->m_edgeListCount -1) % m_pRecomputeData->m_edgeListCount;
                    const edgeFrieze& edgeLast = _edgeList[idEdgeLastOfEdgeRunLast];

                    if ( isEdgeWithHoleCollision(edgeLast) && isEdgeWithHoleVisual(edgeLast) )
                    {
                        build = btrue;
                        const edgeFrieze& edgeCur = _edgeList[edgeRunCur->m_idEdgeStart];

                        buildOutline_PosStartOnEdgeWithHoleCollision(pConfig, collision, edgeCur, offsetCur);
                    }
                }

                if ( !build)
                {
                    buildCollision_OnPatch( pConfig, collision, _edgeList, edgeRunCur, colRun, offsetCur, colRun.m_collisionOffsetLast, btrue );     
                }
            }
            else
                buildCollision_CornerSquare_StartEdgeRun( pConfig, _edgeList, collision, &_edgeList[colRun.m_idEdgeStart], colRun.m_idEdgeStart, offsetCur, colRun );


            for ( u32 edgeRunCount = 0; edgeRunCount < colRun.m_edgeRunCount; edgeRunCount++ )
            {
                u32 idEdgeRun = ( edgeRunCount + colRun.m_idEdgeRunStart ) % edgeRunListCount;
                edgeRunCur = &_edgeRunList[idEdgeRun];

                // update offset position
                offsetCur = getCollisionOffSet( pConfig, edgeRunCur, colRun);         

                if ( edgeRunLast )
                {
//                    u32 buildCount = collision.size();

                    // transition between two edgeRun
                    buildCollision_OnPatch( pConfig, collision, _edgeList, edgeRunCur, colRun, offsetCur, offsetLast );
                }

                // outline edgeRun
                u32 countStopOffset = 0;
                if ( edgeRunCount == colRun.m_edgeRunCount -1)
                    countStopOffset = colRun.m_edgeCountStopOffset;

                if ( pConfig->m_collisionFrieze.m_isSmooth )
                    buildOutline_EdgeRunWithCornerRounder( pConfig, collision, _edgeList, edgeRunCur, offsetCur, pConfig->m_collisionFrieze.m_smoothFactor, edgeCountStartOffset, countStopOffset );
                else
                    buildOutline_EdgeRunWithCornerSquare( pConfig, collision, _edgeList, edgeRunCur, offsetCur, edgeCountStartOffset, countStopOffset );

                edgeRunLast = edgeRunCur;
                offsetLast = offsetCur;
                edgeCountStartOffset = 0;
            }

            // build stop points  
            const u32 idEdgeRunNext = ( colRun.m_idEdgeRunStart +colRun.m_edgeRunCount ) % edgeRunListCount;
            edgeRun* edgeRunNext = &_edgeRunList[idEdgeRunNext];

            if ( edgeRunNext->m_idTex != -1 && colRun.m_collisionOffsetNext != -1.f /*&& colRun.m_collisionOffsetNext != offsetCurReal*/ )
            {
                const u32 idEdgeNext = _edgeRunList[idEdgeRunNext].m_idEdgeStart;
                const edgeFrieze& edgeNext = _edgeList[ idEdgeNext ];

                ITF_ASSERT( isLooping() || idEdgeNext > 0 );

                const u32 idEdgeCur = ( idEdgeNext + m_pRecomputeData->m_edgeListCount -1) % m_pRecomputeData->m_edgeListCount;
                const edgeFrieze& edgeCur = _edgeList[idEdgeCur];

                if ( isEdgeWithHoleCollision(edgeNext) && isEdgeWithHoleVisual(edgeNext) )
                {
                    buildOutline_PosStopOnEdgeWithHoleCollision( pConfig, collision, edgeCur, offsetCur, edgeNext.m_idPoint);
                }                
                else
                {
                    buildCollision_OnPatch( pConfig, collision, _edgeList, edgeRunNext, colRun, offsetCur, colRun.m_collisionOffsetNext, bfalse );     
                }
            }
            else    
            {
                const u32 idStopEdge = ( colRun.m_idEdgeStart + colRun.m_edgeCount +m_pRecomputeData->m_edgeListCount -1 ) % m_pRecomputeData->m_edgeListCount;
                buildCollision_CornerSquare_StopEdgeRun( pConfig, collision, _edgeList, idStopEdge, offsetCur, colRun );
            }

            // add collision
            collisionByGMat.m_stopId = collision.size();
            collisionList.push_back(collisionByGMat);

            addPointsCollision( pConfig, collision, collisionList, colRun );
            collision.clear();

            edgeRunLast = NULL;            
        }       
    }

    void Frise::buildCollision_OnPatch( const FriseConfig* _pConfig, SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun* _edgeRunCur, collisionRun& _colRun, f32 _offsetCur, f32 _offsetLast, u32 _startEdgeRun /*= U32_INVALID*/ ) const
    {     
        // edge cur
        const edgeFrieze& edgeCur = _edgeList[_edgeRunCur->m_idEdgeStart];

        // edge last
        const u32 idEdgeLast = ( _edgeRunCur->m_idEdgeStart + m_pRecomputeData->m_edgeListCount -1) % m_pRecomputeData->m_edgeListCount;
        const edgeFrieze& edgeLast = _edgeList[idEdgeLast]; 

        Vec2d centerPatch;
        getCenterPatch_InPipePatch( edgeCur, centerPatch );                

        if ( ( edgeCur.m_cornerAngle >= 0.f && _offsetCur >= 0.5f ) || ( edgeCur.m_cornerAngle < 0.f && _offsetCur < 0.5f ) )
        {            
            const f32 height = edgeCur.m_heightStart/* * _pConfig->m_patchScale*/;
            const Vec2d posStart = centerPatch + edgeLast.m_normal * height * ( _offsetLast - 0.5f );            
            const Vec2d posStop = centerPatch + edgeCur.m_normal * height * ( _offsetCur - 0.5f );            
            Vec2d posMiddle = centerPatch + edgeCur.m_cornerNormal * height* ( (_offsetCur + _offsetLast)*0.5f - 0.5f );

            if ( _pConfig->m_patchCornerFactorCollision > 1.f )
            {
                Vec2d posInter;
                if ( intersectionLineLine( getPosStartOnNormalEdge( &edgeLast, _offsetLast), getPosStopOnNormalEdge( &edgeLast, _offsetLast), 
                    getPosStartOnNormalEdge( &edgeCur, _offsetCur), getPosStopOnNormalEdge( &edgeCur, _offsetCur), posInter ) > 0 )
                {
                    posMiddle = posInter;

                    const f32 normInter = (posMiddle -centerPatch).norm();
                    const f32 normMax = _pConfig->m_patchCornerFactorCollision * height * 0.5f;

                    if ( normInter>normMax)
                    {
                        posMiddle = centerPatch + normMax*(posMiddle -centerPatch)/normInter;
                    }
                }  
            }

            if ( _startEdgeRun == 1 )
            {            
                _outline.push_back( posMiddle );
                _outline.push_back( posStop );
            }
            else if ( _startEdgeRun == 0 )
            {
                _outline.push_back( posStart );
                _outline.push_back( posMiddle );
            }
            else
            {
                _outline.push_back( posStart );
                _outline.push_back( posMiddle );
                _outline.push_back( posStop );
            }
        }
        else
        {
            Vec2d posInter;
            
            if ( intersectionLineLine( getPosStartOnNormalEdge( &edgeLast, _offsetLast), getPosStopOnNormalEdge( &edgeLast, _offsetLast), 
                getPosStartOnNormalEdge( &edgeCur, _offsetCur), getPosStopOnNormalEdge( &edgeCur, _offsetCur), posInter ) > 0 )
            {
                _outline.push_back( posInter );
            }            
        }
    }
} // namespace ITF