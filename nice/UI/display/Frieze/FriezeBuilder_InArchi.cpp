#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{   
    static i32 idTexConfig_InArchi;

    static const u32 idTopZoneUv       = 0;
    static const u32 idRightZoneUv     = 1;
    static const u32 idLeftZoneUv      = 2;
    static const u32 idBottomZoneUv    = 3;

    static u32 ZoneContinuityBefore_Out[]=
    {
        idLeftZoneUv,      // TOP
        idTopZoneUv,       // RIGHT
        idBottomZoneUv,    // LEFT
        idRightZoneUv      // BOTTOM
    };

    static u32 ZoneContinuityBefore_In[]=
    {
        idRightZoneUv,     // TOP
        idBottomZoneUv,    // RIGHT
        idTopZoneUv,       // LEFT
        idLeftZoneUv,      // BOTTOM
    };

    static u32 ZoneContinuityNext_Out[]=
    {
        idRightZoneUv,      // TOP
        idBottomZoneUv,     // RIGHT
        idTopZoneUv,        // LEFT
        idLeftZoneUv        // BOTTOM
    };

    static u32 ZoneContinuityNext_In[]=
    {
        idLeftZoneUv,       // TOP
        idTopZoneUv,        // RIGHT
        idBottomZoneUv,     // LEFT
        idRightZoneUv,      // BOTTOM
    };

    struct VarianceCornerOut_TexSwitch
    {
        VarianceCornerOut_TexSwitch( f32 _inStart, f32 _inStop )
        {
            m_inStart = _inStart;
            m_inStop = _inStop;
        }

        f32 m_inStart;
        f32 m_inStop;
    };

    static VarianceCornerOut_TexSwitch varianceZone[]=
    {
        VarianceCornerOut_TexSwitch( 0.f, 1.f ),       // TOP
        VarianceCornerOut_TexSwitch( 1.f, 0.f ),     // RIGHT
        VarianceCornerOut_TexSwitch( 1.f, 0.f ),      // LEFT
        VarianceCornerOut_TexSwitch( 0.f, 1.f ),    // BOTTOM
    };

    // choix de la bande d'uv sur un edge
    struct UvsByAngle_InArchi
    {
        UvsByAngle_InArchi( f32 _start, f32 _stop, u32 _index )
        {
            m_start = _start;
            m_stop  = _stop;
            m_index = _index;
        }

        f32 m_start;
        f32 m_stop;
        u32 m_index;
    };

    static UvsByAngle_InArchi indexZoneByAngle[]=
    {
        UvsByAngle_InArchi( -MTH_PIBY4,         MTH_PIBY4       , idTopZoneUv ),       // TOP
        UvsByAngle_InArchi( -3.f *MTH_PIBY4,    -MTH_PIBY4      , idRightZoneUv ),     // RIGHT
        UvsByAngle_InArchi( MTH_PIBY4,          3.f *MTH_PIBY4  , idLeftZoneUv ),      // LEFT
        UvsByAngle_InArchi( 3.f *MTH_PIBY4,     MTH_PI          , idBottomZoneUv ),    // BOTTOM
        UvsByAngle_InArchi( -MTH_PI,            -3.f *MTH_PIBY4 , idBottomZoneUv ),    // BOTTOM        
    };

    static UvLinearArchi uv[]=
    {
        UvLinearArchi( 0.125f,   0.f,    1.f,
        Vec2d( 0.f, 0.625f ),   Vec2d( 0.f, 0.5f ),     Vec2d( 0.25f, 0.625f ), Vec2d( 0.25f, 0.5f ), Vec2d( 1.f, 0.f),      // OUT
        Vec2d( 0.25f, 0.75f ),  Vec2d( 0.25f, 0.625f ), Vec2d( 0.5f, 0.75f ),   Vec2d( 0.5f, 0.625f ), Vec2d( 1.f, 0.f) ), // IN 

        // RIGHT
        UvLinearArchi( 0.25f,    0.125f, 1.f,
        Vec2d( 0.25f, 0.5f ),   Vec2d( 0.5f, 0.5f ), Vec2d( 0.25f, 0.625f ), Vec2d( 0.5f, 0.625f ), Vec2d( 1.f, 0.f),   // OUT
        Vec2d( 0.25f, 0.75f ),  Vec2d( 0.5f, 0.75f ), Vec2d( 0.25f, 0.875f ), Vec2d( 0.5f, 0.875f ), Vec2d( 0.f, 0.f) ),   // IN  

        // LEFT
        UvLinearArchi( 0.375f,   0.25f,  1.f,
        Vec2d( 0.25f, 1.f ),    Vec2d( 0.f, 1.f ),   Vec2d( 0.25f, 0.875f ), Vec2d( 0.f, 0.875f ), Vec2d( 0.f, 0.f), // OUT
        Vec2d( 0.25f, 0.75f ),  Vec2d( 0.f, 0.75f ), Vec2d( 0.25f, 0.625f ), Vec2d( 0.f, 0.625f ), Vec2d( 1.f, 0.f) ), // IN 

        // BOTTOM
        UvLinearArchi( 0.375f,   0.5f,   -1.f,
        Vec2d( 0.5f, 0.875f ), Vec2d( 0.5f, 1.f ),      Vec2d( 0.25f, 0.875f ), Vec2d( 0.25f, 1.f ), Vec2d( 0.f, 0.f),   // OUT
        Vec2d( 0.25f, 0.75f ), Vec2d( 0.25f, 0.875f ),  Vec2d( 0.f, 0.75f ),    Vec2d( 0.f, 0.875f ), Vec2d( 0.f, 0.f) ), // IN 
    };

    static UvLinearArchi uvFlipped[]=
    {
        // TOP
        UvLinearArchi( 0.125f,   0.f,    1.f,        
        Vec2d( 0.5f, 0.625f ),   Vec2d( 0.5f, 0.5f ),     Vec2d( 0.25f, 0.625f ),  Vec2d( 0.25f, 0.5f ),    Vec2d( 1.f, 0.f),      // OUT
        Vec2d( 0.25f, 0.75f ),   Vec2d( 0.25f, 0.625f ),  Vec2d( 0.f, 0.75f ),    Vec2d( 0.f, 0.625f ),   Vec2d( 1.f, 0.f) ), // IN 

        // RIGHT
        UvLinearArchi( 0.375f,   0.25f, 1.f,
        Vec2d( 0.25f, 0.5f ),   Vec2d( 0.f, 0.5f ),  Vec2d( 0.25f, 0.625f ), Vec2d( 0.f, 0.625f ), Vec2d( 1.f, 0.f),   // OUT
        Vec2d( 0.25f, 0.75f ),  Vec2d( 0.f, 0.75f ), Vec2d( 0.25f, 0.875f ), Vec2d( 0.f, 0.875f ), Vec2d( 0.f, 0.f) ),   // IN  

        // LEFT
        UvLinearArchi( 0.25f,    0.125f,  1.f,
        Vec2d( 0.25f, 1.f ),    Vec2d( 0.5f, 1.f ),   Vec2d( 0.25f, 0.875f ), Vec2d( 0.5f, 0.875f ), Vec2d( 0.f, 0.f), // OUT
        Vec2d( 0.25f, 0.75f ),  Vec2d( 0.5f, 0.75f ), Vec2d( 0.25f, 0.625f ), Vec2d( 0.5f, 0.625f ), Vec2d( 1.f, 0.f) ), // IN 

        // BOTTOM
        UvLinearArchi( 0.375f,   0.5f,   -1.f,
        Vec2d( 0.f, 0.875f ),   Vec2d( 0.f, 1.f ),      Vec2d( 0.25f, 0.875f ),  Vec2d( 0.25f, 1.f ),   Vec2d( 0.f, 0.f),   // OUT
        Vec2d( 0.25f, 0.75f ),   Vec2d( 0.25f, 0.875f ),  Vec2d( 0.5f, 0.75f ),    Vec2d( 0.5f, 0.875f ), Vec2d( 0.f, 0.f) ), // IN 
    };

    void Frise::setUvByAngleInArchi( ) const
    {        
        const f32 angle = getConfig()->m_wallAngle.ToRadians();

        if ( angle == indexZoneByAngle[0].m_stop )
            return;

        f32 angleStep = MTH_PIBY2 -angle;

        // top
        indexZoneByAngle[0].m_start = -angle;
        indexZoneByAngle[0].m_stop = angle;

        // right
        indexZoneByAngle[1].m_start = -MTH_PIBY2 -angleStep;
        indexZoneByAngle[1].m_stop = -angle;

        // left
        indexZoneByAngle[2].m_start = angle;
        indexZoneByAngle[2].m_stop = MTH_PIBY2 +angleStep;

        // bottom
        indexZoneByAngle[3].m_start = indexZoneByAngle[2].m_stop;
        indexZoneByAngle[4].m_stop = indexZoneByAngle[1].m_start;
    }

    void Frise::buildFrieze_InArchi( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {
        copyEdgeFromPolyline(_edgeList);        

        m_pRecomputeData->m_texRatio = 0.25f;    

        buildEdgeList( _edgeList);

        if ( m_pRecomputeData->m_edgeListCount == 0 )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif
       
        setUvByAngleInArchi();

        if ( !buildEdgeRunList_InArchi( _edgeList, _edgeRunList ) )
            return;

        setEdgeRunListCoeffUv( _edgeList, _edgeRunList );

        UvLinearArchi* uvToApply = m_pRecomputeData->m_useFlippedUV ? uvFlipped : uv;
        buildVB_InArchi_Static( _edgeList, _edgeRunList, uvToApply );
    }

    Frise::edgeFrieze Frise::buildNewEdge_InArchi( ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* config, u32 idCurEdge, u32 idLastEdge) const
    {
        edgeFrieze& edgeCur = _edgeList[idCurEdge];
        edgeFrieze& edgeLast = _edgeList[idLastEdge];
        f32 coeffOffset = edgeCur.m_heightStart * f32_Abs(edgeCur.m_cornerAngle) * MTH_BYPI;
        
        if ( edgeCur.m_cornerAngle <0.f)
            coeffOffset *= Max(0.001f, 1.f-config->m_visualOffset);
        else
            coeffOffset *= Max(0.001f, config->m_visualOffset);

        Vec2d offset = edgeCur.m_cornerNormal.getPerpendicular() *coeffOffset;
        Vec2d posEdgeCur = edgeCur.m_pos;
        Vec2d posEdgeNew = edgeCur.m_pos;

        if ( idCurEdge == 0)
        {
            posEdgeNew += offset *2.f;
        }
        else
        {
            posEdgeCur -= offset;
            posEdgeNew += offset;
        }

#ifdef DEVELOPER_JAY_FRIEZE
        GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(edgeCur.m_pos), transformPosLocalToWorld(edgeCur.m_pos +edgeCur.m_cornerNormal), Color::yellow());
        GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(edgeCur.m_pos), transformPosLocalToWorld(edgeCur.m_pos +offset), Color::red());
#endif

        // create new edge
        edgeFrieze edgeNew;
        edgeNew.m_pos = posEdgeNew;
        edgeNew.m_holeMode = edgeCur.m_holeMode;
        edgeNew.m_idPoint = edgeCur.m_idPoint;
        edgeNew.m_snap = btrue;
        setScaleNewEdge( edgeNew, edgeLast, edgeCur );
        updateEdgeFriezeVector( edgeNew, posEdgeCur -posEdgeNew);

        // update cur edge
        Vec2d sightDest = edgeCur.m_pos + edgeCur.m_sight;
        edgeCur.m_pos = posEdgeCur;
        updateEdgeFriezeVector( edgeCur, sightDest -posEdgeCur );

        // update last edge
        updateEdgeFriezeVector( edgeLast, posEdgeNew -edgeLast.m_pos);

        return edgeNew;
    }
       
    bbool Frise::insertNewEdge_InArchi( ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* config, u32 idCurEdge, u32 idLastEdge )
    {
        if ( m_lockTexture != LockTexture_None )
            return bfalse;

        edgeFrieze& edgeCur = _edgeList[idCurEdge];
        edgeFrieze& edgeLast = _edgeList[idLastEdge];

        u32 idZoneCurEdge = getZoneId_InArchi( edgeCur.m_sightNormalized );
        u32 idZoneLastEdge = getZoneId_InArchi( edgeLast.m_sightNormalized );
        u32 idZoneRequiered = 0;

        // same uv id
        if ( idZoneLastEdge == idZoneCurEdge )
            return bfalse;

        // check continuity
        if ( edgeCur.m_cornerAngle > 0.f )
        {
            idZoneRequiered = ZoneContinuityBefore_Out[idZoneCurEdge];

            if ( idZoneLastEdge == idZoneRequiered )
                return bfalse;
        }
        else 
        {
            idZoneRequiered = ZoneContinuityBefore_In[idZoneCurEdge];

            if( idZoneLastEdge == idZoneRequiered )
                return bfalse;
        }

        // back up edge cur and last
        edgeFrieze edgeCurBackUp = edgeCur;
        edgeFrieze edgeLastBackUp = edgeLast;
               
        // build edge new
        edgeFrieze edgeNew = buildNewEdge_InArchi( _edgeList, config, idCurEdge, idLastEdge);

        // check result
        if ( edgeNew.m_sightNormalized == Vec2d::Zero || edgeCur.m_sightNormalized == Vec2d::Zero || edgeLast.m_sightNormalized == Vec2d::Zero 
            || idZoneRequiered != getZoneId_InArchi( edgeNew.m_sightNormalized )
            || idZoneCurEdge != getZoneId_InArchi( edgeCur.m_sightNormalized ) 
            || idZoneLastEdge != getZoneId_InArchi( edgeLast.m_sightNormalized) )
        {
            edgeCur = edgeCurBackUp;
            edgeLast = edgeLastBackUp;
            return bfalse;
        }
        else
        {
            insertNewEdgeInEdgeList( _edgeList, edgeNew, idCurEdge );
            return btrue;
        }
    }

    u32 Frise::getZoneId_InArchi( const Vec2d& _sight ) const
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

    u32 Frise::getIdStartEdge_InArchi( ITF_VECTOR<edgeFrieze>& _edgeList ) const
    {
        if ( !m_pointsList.isLooping() || m_lockTexture == LockTexture_Switch )
            return 0;

        u32 idZoneEdgeStart = getZoneId_InArchi( _edgeList[0].m_sightNormalized );

        for (u32 i=1; i<m_pRecomputeData->m_edgeListCount; i++)
        {
            if ( idZoneEdgeStart != getZoneId_InArchi( _edgeList[i].m_sightNormalized ) )
                return i;
        }

        return 0;
    } 

    bbool Frise::buildEdgeRunList_InArchi( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return bfalse;

        _edgeRunList.reserve(m_pRecomputeData->m_edgeListCount);

        edgeRun edgeRunCur;
        edgeRunCur.m_edgeCount = m_pRecomputeData->m_edgeListCount; 
        edgeRunCur.m_idTex = getFirstNoFillTextureIndex( config );

        // frieze texture
        idTexConfig_InArchi = edgeRunCur.m_idTex;

        if ( idTexConfig_InArchi == -1 )
        {
            _edgeRunList.push_back(edgeRunCur);
            return bfalse;
        }

        const u32 idEdgeStart = getIdStartEdge_InArchi( _edgeList );
        u32 idZone = getZoneId_InArchi( _edgeList[idEdgeStart].m_sightNormalized );  
        
        for ( u32 countEdge=0, idCurEdge=idEdgeStart; countEdge<m_pRecomputeData->m_edgeListCount; )
        {
            edgeRunCur.m_idEdgeStart = idCurEdge;            
            edgeRunCur.m_idZone = idZone;
#ifdef DEVELOPER_JAY_FRIEZE
            ITF_ASSERT(edgeRunCur.m_idZone <4);
#endif

            if ( config->m_FriseTextureConfigIndexByZone[edgeRunCur.m_idZone] != -1 )
                edgeRunCur.m_idTex = config->m_FriseTextureConfigIndexByZone[edgeRunCur.m_idZone];
            else
                edgeRunCur.m_idTex = idTexConfig_InArchi;

            ++countEdge;
            ++idCurEdge;
            edgeRunCur.m_edgeCount = 1;

            if ( m_lockTexture == LockTexture_Switch )
            {
                idCurEdge = idCurEdge % m_pRecomputeData->m_edgeListCount;

                if ( _edgeList[idCurEdge].m_cornerAngle >0.f )
                    idZone = ZoneContinuityNext_Out[idZone];
                else
                    idZone = ZoneContinuityNext_In[idZone];                
            }
            else if ( !isEdgeWithHoleVisual(_edgeList[edgeRunCur.m_idEdgeStart]))
            {
                for( ; countEdge<m_pRecomputeData->m_edgeListCount; countEdge++, idCurEdge++ )
                {
                    idCurEdge = idCurEdge % m_pRecomputeData->m_edgeListCount;
                    const edgeFrieze& edgeCur = _edgeList[idCurEdge];

                    if ( isEdgeWithHoleVisual( edgeCur))
                        break;

                    idZone = getZoneId_InArchi( edgeCur.m_sightNormalized );

                    if ( edgeRunCur.m_idZone != idZone )    
                        break;

                    edgeRunCur.m_edgeCount++;
                }
            }
            else
            {
                idCurEdge = idCurEdge % m_pRecomputeData->m_edgeListCount;
                idZone = getZoneId_InArchi( _edgeList[idCurEdge].m_sightNormalized );
            }

            _edgeRunList.push_back(edgeRunCur);
        }

        return btrue;
    }

    void Frise::buildVB_InArchi_Static( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, UvLinearArchi* _uvLinear )
    {
        const FriseConfig* pConfig = getConfig();
        ITF_ASSERT_CRASH( pConfig, "Config file is missing");

        u16 indexVtxUp = 0;
        u16 indexVtxDown = 0;
        const u32 edgeRunListCount = _edgeRunList.size();

        // init uv corner variance
        _uvLinear[idTopZoneUv].m_uvSquareIn[4].y() = 0.f;
        _uvLinear[idTopZoneUv].m_uvSquareOut[4].y() = 0.f;
        _uvLinear[idRightZoneUv].m_uvSquareOut[4].y() = 0.f;
        _uvLinear[idLeftZoneUv].m_uvSquareIn[4].y() = 0.f;
        
        //  colors vertexs 
        u32 vtxColorDownInit, vtxColorUpInit;
        setVtxColor( pConfig->m_textureConfigs[idTexConfig_InArchi], vtxColorUpInit, vtxColorDownInit ); 

        // tex switch
        bbool isTexSwitch = bfalse;
        bbool texHasSwitched = bfalse;
        u32 vtxColorDownSwitch = 0;
        u32 vtxColorUpSwitch = 0;
       
        if (  pConfig->m_idTexSwitch != -1 && pConfig->m_textureConfigs[pConfig->m_idTexSwitch].isDrawnable() )
        {
            isTexSwitch = btrue;

            setVtxColor( pConfig->m_textureConfigs[pConfig->m_idTexSwitch], vtxColorUpSwitch, vtxColorDownSwitch ); 

            if ( m_pointsList.isLooping() && edgeRunListCount > 1 )
            {
                if ( isEdgeRunWithSwitchTexture( _edgeRunList[edgeRunListCount-1], _edgeList ) )
                    texHasSwitched = btrue;
            }
        }
        
        for( u32 idEdgeRun = 0; idEdgeRun < edgeRunListCount; idEdgeRun ++ )
        {
            edgeRun& edgeRunCur = _edgeRunList[idEdgeRun];            

            // start edge
            edgeFrieze& edgeCur = _edgeList[edgeRunCur.m_idEdgeStart];

            // id Tex
            i32 idTexEdgeRun = idTexConfig_InArchi;
            i32 idTexExt = idTexConfig_InArchi;

            // color
            u32 vtxColorDownEdgeRun = vtxColorDownInit;
            u32 vtxColorUpEdgeRun = vtxColorUpInit;
            u32 vtxColorDownExt = vtxColorDownInit;
            u32 vtxColorUpExt = vtxColorUpInit;

            f32 forceVariance = -1.f;

            if ( isTexSwitch )
            {
                if ( isEdgeRunWithSwitchTexture( edgeRunCur, _edgeList ) )
                {
                    texHasSwitched = btrue;
                    idTexEdgeRun = pConfig->m_idTexSwitch;
                    idTexExt = pConfig->m_idTexSwitch;
                    
                    // color edgeRun
                    vtxColorDownEdgeRun = vtxColorDownSwitch;
                    vtxColorUpEdgeRun = vtxColorUpSwitch;

                    // color ext
                    vtxColorDownExt = vtxColorDownSwitch;
                    vtxColorUpExt = vtxColorUpSwitch;

                    // set variance in texSwitch
                    forceVariance = varianceZone[edgeRunCur.m_idZone].m_inStart;

                }
                else if ( texHasSwitched )
                {
                    texHasSwitched = bfalse;
                    idTexExt = pConfig->m_idTexSwitch;

                    // color ext
                    vtxColorDownExt = vtxColorDownSwitch;
                    vtxColorUpExt = vtxColorUpSwitch;

                    // set variance in texSwitch
                    forceVariance = varianceZone[edgeRunCur.m_idZone].m_inStop;
                }
            }                

            // build extremity on start edge
            if ( edgeCur.m_cornerAngle != 0.f && !isEdgeWithHoleVisual(edgeCur))
            {
                // edge Last
                const u32 idEdgeLast = ( edgeRunCur.m_idEdgeStart +m_pRecomputeData->m_edgeListCount -1 ) %m_pRecomputeData->m_edgeListCount;
                edgeFrieze& edgeLast = _edgeList[idEdgeLast];
 
                if ( !isEdgeWithHoleVisual(edgeLast))
                {
                    if ( edgeCur.m_cornerAngle >0.f )
                        buildVB_Static_CornerSquareExt( pConfig, m_meshBuildData->m_staticIndexList[idTexExt], edgeCur, edgeLast, &_uvLinear[edgeRunCur.m_idZone].m_uvSquareOut[0], vtxColorUpExt, vtxColorDownExt, btrue, forceVariance );
                    else
                        buildVB_Static_CornerSquareExt( pConfig, m_meshBuildData->m_staticIndexList[idTexExt], edgeCur, edgeLast, &_uvLinear[edgeRunCur.m_idZone].m_uvSquareIn[0], vtxColorUpExt, vtxColorDownExt, bfalse, forceVariance );
                }                               
            }

            // build VB edgeRun
            Vec2d uvUp( getUvX_Start(), _uvLinear[edgeRunCur.m_idZone].m_uvYup );
            Vec2d uvDown( uvUp.x(), _uvLinear[edgeRunCur.m_idZone].m_uvYdown );

            buildVB_Static_EdgeRun( pConfig, _edgeList, edgeRunCur, m_meshBuildData->m_staticIndexList[idTexEdgeRun], vtxColorUpEdgeRun, vtxColorDownEdgeRun, vtxColorUpEdgeRun, vtxColorDownEdgeRun, indexVtxUp, indexVtxDown, uvUp, uvDown, _uvLinear[edgeRunCur.m_idZone].m_uvXsign * m_pRecomputeData->m_uvXsign );             
        }
    } 

    void Frise::buildVB_Static_CornerSquareExt( const FriseConfig* config, IndexList& _indexList, const edgeFrieze& _edgeCur, const edgeFrieze& _edgeLast, Vec2d* _uv, u32 _colorUp, u32 _colorDown, bbool _flipEdge, f32 _varianceForced )
    {        
    /*  edgeLast
        .    . 
        .    .
        .    .
        v1 . v3 . .
        .    .
        .    .   --> edgeCur
        .    .      
        v0 . v2 . .
        .    .
        .    .
        .    . 
        edgeLast    */
       
        f32 zVtx[4];
        Vec2d v[4];
        Vec2d uv[4];


        // uv variance
        Vec2d uvVariance(Vec2d::Zero);
        if ( _varianceForced != -1.f )
        {
            uvVariance.x() = 0.5f *_varianceForced;
        }
        else if ( _uv[4].x() != 0.f )
        {
            uvVariance.x() = 0.5f *_uv[4].y();
            _uv[4].y() = f32_Modulo( _uv[4].y() +1.f, 2.f );
        }

        if ( _edgeCur.m_cornerAngle >0.f )
        {
            v[0] = _edgeLast.m_points[3];
            v[1] = _edgeCur.m_interUp;

            zVtx[0] = m_pRecomputeData->m_zVtxUp;
            zVtx[1] = m_pRecomputeData->m_zVtxUp;
            zVtx[2] = m_pRecomputeData->m_zVtxDown;
            zVtx[3] = m_pRecomputeData->m_zVtxUp;
        }
        else
        {
            v[0] = _edgeCur.m_interDown;
            v[1] = _edgeLast.m_points[2];

            zVtx[0] = m_pRecomputeData->m_zVtxDown;
            zVtx[1] = m_pRecomputeData->m_zVtxDown;
            zVtx[2] = m_pRecomputeData->m_zVtxDown;
            zVtx[3] = m_pRecomputeData->m_zVtxUp;
        }
                
        v[2] = _edgeCur.m_points[0];
        v[3] = _edgeCur.m_points[1];
        
        for (u32 i=0; i<4; i++) 
            uv[i] = _uv[i] + uvVariance;

        if (buildVB_Static_QuadMeshAlone(v, uv))
            return;

        ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
        const u32 vtxListCount = vtxList.size();
        vtxList.resize( vtxListCount + 4 );

        u16 indexVtxDown    = (u16)vtxListCount;
        u16 indexVtxUp      = indexVtxDown +1;

                
        VertexPCT& vtx0 = vtxList[ indexVtxDown ];
        vtx0.m_pos.x() = v[0].x();
        vtx0.m_pos.y() = v[0].y();
        vtx0.m_pos.z() = zVtx[0];
        vtx0.m_uv = uv[0];

        VertexPCT& vtx1 = vtxList[ indexVtxUp ];
        vtx1.m_pos.x() = v[1].x();
        vtx1.m_pos.y() = v[1].y();
        vtx1.m_pos.z() = zVtx[1];
        vtx1.m_uv = uv[1];
        
        VertexPCT& vtx2 = vtxList[ indexVtxDown +2];
        vtx2.m_pos.x() = v[2].x();
        vtx2.m_pos.y() = v[2].y();
        vtx2.m_pos.z() = zVtx[2];
        vtx2.m_uv = uv[2];
        
        VertexPCT& vtx3 = vtxList[ indexVtxUp +2];
        vtx3.m_pos.x() = v[3].x();
        vtx3.m_pos.y() = v[3].y();
        vtx3.m_pos.z() = zVtx[3];
        vtx3.m_uv = uv[3];

        // color
        if ( _edgeCur.m_cornerAngle > 0.f )
        {
            vtx2.setColor( _colorDown);
            vtx0.setColor( _colorUp);
            vtx1.setColor( _colorUp);
            vtx3.setColor( _colorUp);            
        }
        else
        {
            vtx3.setColor( _colorUp);            
            vtx0.setColor( _colorDown);         
            vtx1.setColor( _colorDown);
            vtx2.setColor( _colorDown);
        }

        addIndex_Static_Quad( _indexList, indexVtxDown, indexVtxUp, _flipEdge );
    }

    void Frise::buildCollision_InArchi( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList  )
    {
        if ( !buildCollisionRunList( _edgeList, _edgeRunList, _collisionRunList ) )
            return;

        buildCollision_InArchi_CornerSquare( _edgeList, _edgeRunList, _collisionRunList );

        finalizeCollisionList();
    }

    void Frise::buildCollision_InArchi_CornerSquare( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return;

        SafeArray<Vec2d> collision;
        collision.reserve( m_pRecomputeData->m_edgeListCount *3 +3 );

        edgeRun* edgeRunLast = NULL;
        f32 offsetLast = -1.f;
        u32 edgeRunListCount = _edgeRunList.size();
        u32 collisionRunListCount = _collisionRunList.size();

        for ( u32 idCol = 0; idCol < collisionRunListCount; idCol++ )
        {
            collisionRun& colRun = _collisionRunList[idCol];            
            edgeRun* edgeRunCur = &_edgeRunList[colRun.m_idEdgeRunStart];

            // init offset position
            f32 offsetCur = getCollisionOffSet( config, edgeRunCur, colRun);

            const Path* gameMat = getCollisionGameMaterial( config, edgeRunCur->m_idTex );
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

                        buildOutline_PosStartOnEdgeWithHoleCollision(config, collision, edgeCur, offsetCur);
                    }
                }

                if ( !build)
                    buildCollision_ExtremityWithCoord( config, collision, _edgeList, edgeRunCur, colRun, offsetCur, colRun.m_collisionOffsetLast, btrue );
            }
            else
                buildCollision_CornerSquare_StartEdgeRun( config, _edgeList, collision, &_edgeList[colRun.m_idEdgeStart], colRun.m_idEdgeStart, offsetCur, colRun );


            for ( u32 edgeRunCount = 0; edgeRunCount < colRun.m_edgeRunCount; edgeRunCount++ )
            {
                u32 idEdgeRun = ( edgeRunCount + colRun.m_idEdgeRunStart ) % edgeRunListCount;
                edgeRunCur = &_edgeRunList[idEdgeRun];

                // update offset position
                offsetCur = getCollisionOffSet( config, edgeRunCur, colRun);         

                if ( edgeRunLast )
                {
                    u32 buildCount = collision.size();

                    // transition between two edgeRun
                    buildCollision_ExtremityWithCoord( config, collision, _edgeList, edgeRunCur, colRun, offsetCur, offsetLast );

                    const Path* gameMat = getCollisionGameMaterial(config, edgeRunCur->m_idTex );

                    // add collision
                    if ( gameMat )
                    {
                        u32 buildAddBy2 = Max( 0, i32(collision.size() -buildCount) -2);
                        collisionByGMat.m_stopId = buildCount + buildAddBy2;

                        collisionList.push_back(collisionByGMat);

                        collisionByGMat.m_startId = collisionByGMat.m_stopId;
                        collisionByGMat.m_gameMat = *gameMat;
                    }
                }

                // outline edgeRun
                u32 countStopOffset = 0;
                if ( edgeRunCount == colRun.m_edgeRunCount -1)
                    countStopOffset = colRun.m_edgeCountStopOffset;

                if ( config->m_collisionFrieze.m_isSmooth )
                    buildOutline_EdgeRunWithCornerRounder( config, collision, _edgeList, edgeRunCur, offsetCur, config->m_collisionFrieze.m_smoothFactor, edgeCountStartOffset, countStopOffset );
                else
                    buildOutline_EdgeRunWithCornerSquare( config, collision, _edgeList, edgeRunCur, offsetCur, edgeCountStartOffset, countStopOffset );

                edgeRunLast = edgeRunCur;
                offsetLast = offsetCur;
                edgeCountStartOffset = 0;
            }
 
            // build stop points  
            u32 idEdgeRunNext = ( colRun.m_idEdgeRunStart +colRun.m_edgeRunCount ) % edgeRunListCount;
            edgeRun* edgeRunNext = &_edgeRunList[idEdgeRunNext];
                      
            if ( edgeRunNext->m_idTex != -1 && colRun.m_collisionOffsetNext != -1.f /*&& colRun.m_collisionOffsetNext != offsetCurReal*/ )
            {
                bbool build = bfalse;
                edgeFrieze& edgeNext = _edgeList[ _edgeRunList[idEdgeRunNext].m_idEdgeStart];

                if ( isEdgeWithHoleCollision(edgeNext) && isEdgeWithHoleVisual(edgeNext) )
                {
                    ITF_ASSERT( edgeRunLast );
                    if ( edgeRunLast )
                    {
                        build = btrue;
                        const u32 idEdgeCur = ( edgeRunLast->m_idEdgeStart +edgeRunLast->m_edgeCount +m_pRecomputeData->m_edgeListCount -1)%m_pRecomputeData->m_edgeListCount;
                        const edgeFrieze& edgeCur = _edgeList[idEdgeCur];

                        buildOutline_PosStopOnEdgeWithHoleCollision( config, collision, edgeCur, offsetCur, edgeNext.m_idPoint);
                    }
                }                

                if( !build)
                    buildCollision_ExtremityWithCoord( config, collision, _edgeList, edgeRunNext, colRun, colRun.m_collisionOffsetNext, offsetCur, bfalse );
            }
            else    
            {
                const u32 idStopEdge = ( colRun.m_idEdgeStart + colRun.m_edgeCount +m_pRecomputeData->m_edgeListCount -1 ) % m_pRecomputeData->m_edgeListCount;
                buildCollision_CornerSquare_StopEdgeRun( config, collision, _edgeList, idStopEdge, offsetCur, colRun );
            }

            // add collision
            collisionByGMat.m_stopId = collision.size();
            collisionList.push_back(collisionByGMat);

            addPointsCollision( config, collision, collisionList, colRun );
            collision.clear();

            edgeRunLast = NULL;            
        }       
    }

    bbool Frise::isEdgeRun_InArchi( ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idCurEdge, u32 _idLastEdge, u32 _idLastLastEdge ) const
    {        
        if ( m_lockTexture == LockTexture_Switch )
            return btrue;

        u32 idZoneLastEdge = getZoneId_InArchi( _edgeList[_idLastEdge].m_sightNormalized );

        if ( getZoneId_InArchi( _edgeList[_idLastLastEdge].m_sightNormalized ) == idZoneLastEdge )
            return bfalse;

        if ( getZoneId_InArchi( _edgeList[_idCurEdge].m_sightNormalized ) == idZoneLastEdge )
            return bfalse;

        return btrue;
    }

} // namespace ITF
