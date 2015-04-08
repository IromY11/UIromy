#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    static i32 idTexConfig_InPipe;

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

    // choix de la bande d'uv sur un edge
    struct idZoneByAngle_InPipe
    {
        idZoneByAngle_InPipe( f32 _start, f32 _stop, u32 _index )
        {
            m_start = _start;
            m_stop  = _stop;
            m_index = _index;
        }

        f32 m_start;
        f32 m_stop;
        u32 m_index;
    };

    static idZoneByAngle_InPipe indexZoneByAngle[]=
    {
        idZoneByAngle_InPipe( -MTH_PIBY4,         MTH_PIBY4       , idTopZoneUv ),       // TOP
        idZoneByAngle_InPipe( -3.f *MTH_PIBY4,    -MTH_PIBY4      , idRightZoneUv ),     // RIGHT
        idZoneByAngle_InPipe( MTH_PIBY4,          3.f *MTH_PIBY4  , idLeftZoneUv ),      // LEFT
        idZoneByAngle_InPipe( 3.f *MTH_PIBY4,     MTH_PI          , idBottomZoneUv ),    // BOTTOM
        idZoneByAngle_InPipe( -MTH_PI,            -3.f *MTH_PIBY4 , idBottomZoneUv ),    // BOTTOM        
    };

    //  settings de chaque zone
    struct UvLinear
    {
        UvLinear( 
            f32 _uvYdown, 
            f32 _uvYup, 
            f32 _uvXsign, 
            Vec2d _uvSquareOut_0,
            Vec2d _uvSquareOut_1,
            Vec2d _uvSquareOut_2,
            Vec2d _uvSquareOut_3,
            Vec2d _uvSquareIn_0,
            Vec2d _uvSquareIn_1,
            Vec2d _uvSquareIn_2,
            Vec2d _uvSquareIn_3,
            Vec2d _uvStartExt_0,
            Vec2d _uvStartExt_1,
            Vec2d _uvStartExt_2,
            Vec2d _uvStartExt_3,
            Vec2d _uvStopExt_0,
            Vec2d _uvStopExt_1,
            Vec2d _uvStopExt_2,
            Vec2d _uvStopExt_3,
            Vec2d _uvStartExtSwitch_0,
            Vec2d _uvStartExtSwitch_1,
            Vec2d _uvStartExtSwitch_2,
            Vec2d _uvStartExtSwitch_3,
            Vec2d _uvStopExtSwitch_0,
            Vec2d _uvStopExtSwitch_1,
            Vec2d _uvStopExtSwitch_2,
            Vec2d _uvStopExtSwitch_3
            )
            : m_uvYdown(_uvYdown)
            , m_uvYup(_uvYup)
            , m_uvXsign(_uvXsign)

        {
            m_uvSquareOut[0] = _uvSquareOut_0;
            m_uvSquareOut[1] = _uvSquareOut_1;
            m_uvSquareOut[2] = _uvSquareOut_2;
            m_uvSquareOut[3] = _uvSquareOut_3;

            m_uvSquareIn[0] = _uvSquareIn_0;
            m_uvSquareIn[1] = _uvSquareIn_1;
            m_uvSquareIn[2] = _uvSquareIn_2;
            m_uvSquareIn[3] = _uvSquareIn_3;

            m_uvStartExt[0] = _uvStartExt_0;
            m_uvStartExt[1] = _uvStartExt_1;
            m_uvStartExt[2] = _uvStartExt_2;
            m_uvStartExt[3] = _uvStartExt_3;

            m_uvStopExt[0] = _uvStopExt_0;
            m_uvStopExt[1] = _uvStopExt_1;
            m_uvStopExt[2] = _uvStopExt_2;
            m_uvStopExt[3] = _uvStopExt_3;

            m_uvStartExtSwitch[0] = _uvStartExtSwitch_0;
            m_uvStartExtSwitch[1] = _uvStartExtSwitch_1;
            m_uvStartExtSwitch[2] = _uvStartExtSwitch_2;
            m_uvStartExtSwitch[3] = _uvStartExtSwitch_3;

            m_uvStopExtSwitch[0] = _uvStopExtSwitch_0;
            m_uvStopExtSwitch[1] = _uvStopExtSwitch_1;
            m_uvStopExtSwitch[2] = _uvStopExtSwitch_2;
            m_uvStopExtSwitch[3] = _uvStopExtSwitch_3;
        }

        f32 m_uvYdown;
        f32 m_uvYup;
        f32 m_uvXsign;
        Vec2d m_uvSquareOut[4];
        Vec2d m_uvSquareIn[4];   
        Vec2d m_uvStartExt[4];
        Vec2d m_uvStopExt[4];
        Vec2d m_uvStartExtSwitch[4];
        Vec2d m_uvStopExtSwitch[4];
    };

    static UvLinear uv[]=
    {
        // TOP
        UvLinear( 0.25f,   0.f,    1.f,
        Vec2d( 0.f, 0.75f ),   Vec2d( 0.f, 0.5f ),     Vec2d( 0.25f, 0.75f ), Vec2d( 0.25f, 0.5f ), // OUT
        Vec2d( 0.f, 1.f ),  Vec2d( 0.f, 0.75f ), Vec2d( 0.25f, 1.f ),   Vec2d( 0.25f, 0.75f ),      // IN
        Vec2d( 0.5f, 0.5f ),  Vec2d( 0.75f, 0.5f ), Vec2d( 0.5f, 0.75f ),   Vec2d( 0.75f, 0.75f ), // START
        Vec2d( 0.5f, 0.75f ),  Vec2d( 0.75f, 0.75f ), Vec2d( 0.5f, 1.f ),   Vec2d( 0.75f, 1.f ),  // STOP         
        Vec2d( 0.f, 0.f ),  Vec2d( 0.5f, 0.f ), Vec2d( 0.f, 0.5f ), Vec2d( 0.5f, 0.5f ), // START SWITCH
        Vec2d( 0.f, 0.5f ),  Vec2d( 0.5f, 0.5f ), Vec2d( 0.f, 1.f ),   Vec2d( 0.5f, 1.f ) ),  // STOP SWITCH

        // RIGHT
        UvLinear( 0.5f,    0.25f, 1.f,
        Vec2d( 0.25f, 0.5f ),   Vec2d( 0.5f, 0.5f ), Vec2d( 0.25f, 0.75f ), Vec2d( 0.5f, 0.75f ),    // OUT
        Vec2d( 0.f, 0.5f ),  Vec2d( 0.25f, 0.5f ), Vec2d( 0.f, 0.75f ), Vec2d( 0.25f, 0.75f ), // IN
        Vec2d( 0.75f, 0.5f ),  Vec2d( 1.f, 0.5f ), Vec2d( 0.75f, 0.75f ),   Vec2d( 1.f, 0.75f ), // START
        Vec2d( 0.75f, 0.75f ),  Vec2d( 1.f, 0.75f ), Vec2d( 0.75f, 1.f ),   Vec2d( 1.f, 1.f ),  // STOP 
        Vec2d( 0.5f, 0.f ),  Vec2d( 1.f, 0.f ), Vec2d( 0.5f, 0.5f ),   Vec2d( 1.f, 0.5f ), // START SWITCH
        Vec2d( 0.5f, 0.5f ),  Vec2d( 1.f, 0.5f ), Vec2d( 0.5f, 1.f ),   Vec2d( 1.f, 1.f ) ),  // STOP SWITCH

        // LEFT
        UvLinear( 0.25f,   0.5f,  -1.f,
        Vec2d( 0.25f, 1.f ),    Vec2d( 0.f, 1.f ),   Vec2d( 0.25f, 0.75f ), Vec2d( 0.f, 0.75f ),  // OUT
        Vec2d( 0.5f, 1.f ),  Vec2d( 0.25f, 1.f ), Vec2d( 0.5f, 0.75f ), Vec2d( 0.25f, 0.75f ), // IN
        Vec2d( 1.f, 1.f ),  Vec2d( 0.75f, 1.f ), Vec2d( 1.f, 0.75f ),   Vec2d( 0.75f, 0.75f ), // START
        Vec2d( 1.f, 0.75f ),  Vec2d( 0.75f, 0.75f ), Vec2d( 1.f, 0.5f ),   Vec2d( 0.75f, 0.5f ),  // STOP
        Vec2d( 1.f, 1.f ),  Vec2d( 0.5f, 1.f ), Vec2d( 1.f, 0.5f ),   Vec2d( 0.5f, 0.5f ), // START SWITCH
        Vec2d( 1.f, 0.5f ),  Vec2d( 0.5f, 0.5f ), Vec2d( 1.f, 0.f ),   Vec2d( 0.5f, 0.f ) ),  // STOP SWITCH

        // BOTTOM
        UvLinear( 0.f,   0.25f,   -1.f,
        Vec2d( 0.5f, 0.75f ), Vec2d( 0.5f, 1.f ),      Vec2d( 0.25f, 0.75f ), Vec2d( 0.25f, 1.f ),    // OUT
        Vec2d( 0.5f, 0.5f ), Vec2d( 0.5f, 0.75f ),  Vec2d( 0.25f, 0.5f ),    Vec2d( 0.25f, 0.75f ), // IN
        Vec2d( 0.75f, 1.f ),  Vec2d( 0.5f, 1.f ), Vec2d( 0.75f, 0.75f ),   Vec2d( 0.5f, 0.75f ), // START
        Vec2d( 0.75f, 0.75f ),  Vec2d( 0.5f, 0.75f ), Vec2d( 0.75f, 0.5f ),   Vec2d( 0.5f, 0.5f ),  // STOP
        Vec2d( 0.5f, 1.f ),  Vec2d( 0.f, 1.f ), Vec2d( 0.5f, 0.5f ),   Vec2d( 0.f, 0.5f ), // START SWITCH
        Vec2d( 0.5f, 0.5f ),  Vec2d( 0.f, 0.5f ), Vec2d( 0.5f, 0.f ),   Vec2d( 0.f, 0.f ) ),  // STOP SWITCH
    };

    static UvLinear uvFlipped[]=
    {
        // TOP
        UvLinear( 0.25f,   0.f,    1.f,
        Vec2d( 0.5f, 0.75f ),   Vec2d( 0.5f, 0.5f ), Vec2d( 0.25f, 0.75f ), Vec2d( 0.25f, 0.5f ),    // OUT
        Vec2d( 0.5f, 1.f ), Vec2d( 0.5f, 0.75f ),  Vec2d( 0.25f, 1.f ), Vec2d( 0.25f, 0.75f ), // IN
        Vec2d( 0.5f, 1.f ),   Vec2d( 0.75f, 1.f ), Vec2d( 0.5f, 0.75f ),  Vec2d( 0.75f, 0.75f ), // START
        Vec2d( 0.5f, 0.75f ),   Vec2d( 0.75f, 0.75f ), Vec2d( 0.5f, 0.5f ),  Vec2d( 0.75f, 0.5f ),  // STOP         
        Vec2d( 0.f, 1.f ),   Vec2d( 0.5f, 1.f ), Vec2d( 0.f, 0.5f ),  Vec2d( 0.5f, 0.5f ),  // START SWITCH
        Vec2d( 0.f, 0.5f ),   Vec2d( 0.5f, 0.5f ), Vec2d( 0.f, 0.f ),  Vec2d( 0.5f, 0.f ) ),   // STOP SWITCH

        // RIGHT
        UvLinear( 0.25f,   0.5f,  -1.f,
        Vec2d( 0.25f, 0.5f ),   Vec2d( 0.f, 0.5f ),     Vec2d( 0.25f, 0.75f ), Vec2d( 0.f, 0.75f ), // OUT
        Vec2d( 0.5f, 0.5f ), Vec2d( 0.25f, 0.5f ), Vec2d( 0.5f, 0.75f ), Vec2d( 0.25f, 0.75f ), // IN
        Vec2d( 1.f, 0.5f ), Vec2d( 0.75f, 0.5f ),  Vec2d( 1.f, 0.75f ), Vec2d( 0.75f, 0.75f ),   // START
        Vec2d( 1.f, 0.75f ), Vec2d( 0.75f, 0.75f ),  Vec2d( 1.f, 1.f ), Vec2d( 0.75f, 1.f ),    // STOP 
        Vec2d( 1.f, 0.f ), Vec2d( 0.5f, 0.f ),  Vec2d( 1.f, 0.5f ), Vec2d( 0.5f, 0.5f ),   // START SWITCH
        Vec2d( 1.f, 0.5f ), Vec2d( 0.5f, 0.5f ),   Vec2d( 1.f, 1.f ),  Vec2d( 0.5f, 1.f ) ),  // STOP SWITCH

        // LEFT
        UvLinear( 0.5f,    0.25f, 1.f,        
        Vec2d( 0.25f, 1.f ), Vec2d( 0.5f, 1.f ),      Vec2d( 0.25f, 0.75f ),Vec2d( 0.5f, 0.75f ),    // OUT
        Vec2d( 0.f, 1.f ),  Vec2d( 0.25f, 1.f ), Vec2d( 0.f, 0.75f ),   Vec2d( 0.25f, 0.75f ),      // IN
        Vec2d( 0.75f, 1.f ), Vec2d( 1.f, 1.f ), Vec2d( 0.75f, 0.75f ),  Vec2d( 1.f, 0.75f ),   // START
        Vec2d( 0.75f, 0.75f ), Vec2d( 1.f, 0.75f ), Vec2d( 0.75f, 0.5f ),  Vec2d( 1.f, 0.5f ),    // STOP
        Vec2d( 0.5f, 1.f ), Vec2d( 1.f, 1.f ), Vec2d( 0.5f, 0.5f ), Vec2d( 1.f, 0.5f ),    // START SWITCH
        Vec2d( 0.5f, 0.5f ), Vec2d( 1.f, 0.5f ),  Vec2d( 0.5f, 0.f ), Vec2d( 1.f, 0.f ) ),  // STOP SWITCH

        // BOTTOM
        UvLinear( 0.f,   0.25f,   -1.f,
        Vec2d( 0.f, 0.75f ),    Vec2d( 0.f, 1.f ),   Vec2d( 0.25f, 0.75f ), Vec2d( 0.25f, 1.f ),  // OUT
        Vec2d( 0.f, 0.5f ),  Vec2d( 0.f, 0.75f ), Vec2d( 0.25f, 0.5f ), Vec2d( 0.25f, 0.75f ), // IN
        Vec2d( 0.75f, 0.5f ),   Vec2d( 0.5f, 0.5f ), Vec2d( 0.75f, 0.75f ),  Vec2d( 0.5f, 0.75f ),  // START
        Vec2d( 0.75f, 0.75f ),   Vec2d( 0.5f, 0.75f ),Vec2d( 0.75f, 1.f ),  Vec2d( 0.5f, 1.f ),    // STOP
        Vec2d( 0.5f, 0.f ),   Vec2d( 0.f, 0.f ), Vec2d( 0.5f, 0.5f ),  Vec2d( 0.f, 0.5f ), // START SWITCH
        Vec2d( 0.5f, 0.5f ),   Vec2d( 0.f, 0.5f ),Vec2d( 0.5f, 1.f ),  Vec2d( 0.f, 1.f ) ),  // STOP SWITCH
    };

    void Frise::buildFrieze_InPipe( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return;

        copyEdgeFromPolyline(_edgeList);

        m_pRecomputeData->m_texRatio = 0.25f;    

        if ( !m_pointsList.isLooping() )
            subtractOffsetExtremity( _edgeList, m_pRecomputeData->m_heightScale, m_pRecomputeData->m_heightScale );

        buildEdgeList( _edgeList, btrue);

        if ( m_pRecomputeData->m_edgeListCount < 1 )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        if ( !buildEdgeRunList_InPipe( _edgeList, _edgeRunList ) )
            return;

        setEdgeRunListCoeffUv( _edgeList, _edgeRunList );

        buildVB_InPipe_Static( _edgeList, _edgeRunList );        
    }

    bbool Frise::insertNewEdge_InPipe( ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* config, u32 idCurEdge, u32 idLastEdge )
    {
        if ( m_lockTexture != LockTexture_None )
            return bfalse;

        edgeFrieze& edgeCur = _edgeList[idCurEdge];
        edgeFrieze& edgeLast = _edgeList[idLastEdge];

        u32 idZoneCurEdge = getZoneId_InPipe( edgeCur.m_sightNormalized );
        u32 idZoneLastEdge = getZoneId_InPipe( edgeLast.m_sightNormalized );
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

            if( idZoneLastEdge == idZoneRequiered)
                return bfalse;
        }

        // back up edge cur and last
        edgeFrieze edgeCurBackUp = edgeCur;
        edgeFrieze edgeLastBackUp = edgeLast;
               
        // build edge new
        edgeFrieze edgeNew = buildNewEdge_InArchi( _edgeList, config, idCurEdge, idLastEdge);

        // check result
        if ( edgeNew.m_sightNormalized == Vec2d::Zero || edgeCur.m_sightNormalized == Vec2d::Zero || edgeLast.m_sightNormalized == Vec2d::Zero 
            || idZoneRequiered != getZoneId_InPipe( edgeNew.m_sightNormalized )
            || idZoneCurEdge != getZoneId_InPipe( edgeCur.m_sightNormalized ) 
            || idZoneLastEdge != getZoneId_InPipe( edgeLast.m_sightNormalized) )
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

    u32 Frise::getZoneId_InPipe( const Vec2d& _sight ) const
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

    u32 Frise::getIdStartEdge_InPipe( ITF_VECTOR<edgeFrieze>& _edgeList, f32 _nornMin ) const
    {
        if ( !m_pointsList.isLooping() || m_lockTexture == LockTexture_Switch )
            return 0;

        u32 idZoneEdgeStart = getZoneId_InPipe( _edgeList[0].m_sightNormalized );

        for (u32 i=1; i<m_pRecomputeData->m_edgeListCount; i++)
        {
            if ( checkEdgeIsValid_InPipe( _edgeList[i], _nornMin ) && idZoneEdgeStart != getZoneId_InPipe( _edgeList[i].m_sightNormalized ) )
                return i;
        }

        return 0;
    } 

    bbool Frise::checkEdgeIsValid_InPipe( const edgeFrieze& _edge, f32 _nornMin ) const
    {
        f32 angleAbs = f32_Abs(_edge.m_cornerAngle);

        return ( angleAbs >= MTH_3PIBY8 || ( angleAbs > MTH_PIBY3 && _edge.m_norm >= _nornMin ) );
    }

    bbool Frise::buildEdgeRunList_InPipe( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        const FriseConfig* config = getConfig();

        _edgeRunList.reserve(m_pRecomputeData->m_edgeListCount);

        edgeRun edgeRunCur;
        edgeRunCur.m_edgeCount = m_pRecomputeData->m_edgeListCount; 
        edgeRunCur.m_idTex = getFirstNoFillTextureIndex( config );

        // frieze texture
        idTexConfig_InPipe = edgeRunCur.m_idTex;

        if ( idTexConfig_InPipe == -1 )
        {
            _edgeRunList.push_back(edgeRunCur);
            return bfalse;
        }

        f32 edgeRunNornMin = getEdgeRunNormMin( config );
        u32 idEdgeStart = getIdStartEdge_InPipe( _edgeList, edgeRunNornMin );
        u32 idZone = getZoneId_InPipe( _edgeList[idEdgeStart].m_sightNormalized );  
        
#ifdef DEVELOPER_JAY_FRIEZE
        ITF_ASSERT(edgeRunNornMin >= 0.f);
#endif

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
                edgeRunCur.m_idTex = idTexConfig_InPipe;

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
                u32 exeptionCount = 0;

                for( ; countEdge<m_pRecomputeData->m_edgeListCount; countEdge++, idCurEdge++ )
                {
                    idCurEdge = idCurEdge % m_pRecomputeData->m_edgeListCount;
                    const edgeFrieze& edgeCur = _edgeList[idCurEdge];

                    if ( isEdgeWithHoleVisual( edgeCur))
                        break;

                    idZone = getZoneId_InPipe( edgeCur.m_sightNormalized );

                    if ( edgeRunCur.m_idZone != idZone )    
                    {
                        if ( exeptionCount == 0 && !checkEdgeIsValid_InPipe( edgeCur, edgeRunNornMin )/*f32_Abs(edgeCur.m_cornerAngle) <= MTH_PIBY3 && edgeCur.m_norm < edgeRunNornMin*/ )
                        {
    #ifdef DEVELOPER_JAY_FRIEZE
                            GFX_ADAPTER->drawDBGCircle(edgeCur.m_pos, m_heightScale*0.5f, Color(Color::orange()) );
    #endif
                            if ( countEdge<m_pRecomputeData->m_edgeListCount-1  )
                            {
                                u32 idEdgeNext = ( idCurEdge +1 ) % m_pRecomputeData->m_edgeListCount;
                                u32 idZoneNext = getZoneId_InPipe( _edgeList[idEdgeNext].m_sightNormalized );

                                if ( edgeRunCur.m_idZone == idZoneNext ) 
                                    exeptionCount++;
                                else
                                    break;
                            }
                            else
                                break;
                        }
                        else
                            break;
                    }
                    else
                        exeptionCount = 0;

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

    void Frise::buildVB_InPipe_Static( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {
        const FriseConfig* config = getConfig();

        UvLinear* uvToApply = m_pRecomputeData->m_useFlippedUV ? uvFlipped : uv;
        const bbool useTexSwitch = config->m_idTexSwitch != -1 && !isLooping() && config->m_textureConfigs[config->m_idTexSwitch].isDrawnable();

        u16 indexVtxUp = 0;
        u16 indexVtxDown = 0;
       
        //  colors vertexs 
        u32 vtxColorDown, vtxColorUp, vtxColorUpExtremity, vtxColorDownExtremity;
        setVtxColorWithExtrem( config->m_textureConfigs[idTexConfig_InPipe], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity );

        // Extremity
        Vec2d posExtremity[4];
        f32 zExtrude[4];
        u32 colorExtremity[4];
        f32 heightMidExtremity = _edgeList[0].m_heightStart;
        bbool snapExtremity = bfalse;
        bbool switchTextureExtStart = bfalse;
        bbool switchTextureExtStop = bfalse;

        if ( m_pRecomputeData->m_edgeListCount == 1 )
        {
            if ( _edgeList[0].m_snap )
            {
                snapExtremity = btrue;
                heightMidExtremity = ( _edgeList[0].m_heightStart + _edgeList[0].m_heightStop) *0.5f;
            }

            if ( useTexSwitch )
            {
                switch (m_switchTexturePipeExtremity)
                {
                case 1:
                    switchTextureExtStart = btrue;
                    break;

                case 2:
                    switchTextureExtStop = btrue;
                    break;

                case 3:
                    switchTextureExtStart = btrue;
                    switchTextureExtStop = btrue;
                    break;

                default:
                    break;
                }
            }
        }

        IndexList& indexList = m_meshBuildData->m_staticIndexList[idTexConfig_InPipe];

        const u32 edgeRunListCount = _edgeRunList.size();
        for( u32 idEdgeRun = 0; idEdgeRun < edgeRunListCount; idEdgeRun ++ )
        {
            const edgeRun& edgeRunCur = _edgeRunList[idEdgeRun];

            // start edge
            edgeFrieze& edgeCur = _edgeList[edgeRunCur.m_idEdgeStart];

            // build extremity on start edge
            if ( !isEdgeWithHoleVisual(edgeCur))
            {
                if ( edgeCur.m_cornerAngle != 0.f )
                {
                    // edge Last
                    u32 idEdgeLast = ( edgeRunCur.m_idEdgeStart +m_pRecomputeData->m_edgeListCount -1 ) %m_pRecomputeData->m_edgeListCount;
                    edgeFrieze& edgeLast = _edgeList[idEdgeLast];

                    if ( !isEdgeWithHoleVisual(edgeLast))
                    {
                        if ( edgeCur.m_cornerAngle >0.f )
                            buildVB_Static_CornerSquareExt_InPipe( config, indexList, edgeCur, edgeLast, &uvToApply[edgeRunCur.m_idZone].m_uvSquareOut[0], vtxColorUp, vtxColorDown, btrue );
                        else
                            buildVB_Static_CornerSquareExt_InPipe( config, indexList, edgeCur, edgeLast, &uvToApply[edgeRunCur.m_idZone].m_uvSquareIn[0], vtxColorUp, vtxColorDown, bfalse );                
                    }                    
                }
                else
                {
                    // check edge Last
                    bbool buildExtremity = btrue;
                    if ( edgeRunCur.m_idEdgeStart > 0 || isLooping() )
                    {
                        const u32 idEdgeLast = ( edgeRunCur.m_idEdgeStart +m_pRecomputeData->m_edgeListCount -1 ) %m_pRecomputeData->m_edgeListCount;
                        edgeFrieze& edgeLast = _edgeList[idEdgeLast];

                        buildExtremity = !isEdgeWithHoleVisual(edgeLast);
                    }

                    if ( buildExtremity )
                    {
                        setQuadPos_StartExtremity( config, edgeCur, &posExtremity[0], heightMidExtremity, edgeCur.m_heightStart, snapExtremity, &zExtrude[0] );
                        setQuadColor_StartExtremity( &colorExtremity[0], vtxColorDown, vtxColorUp, vtxColorDownExtremity, vtxColorUpExtremity );

                        if ( ( m_pRecomputeData->m_edgeListCount==1 && switchTextureExtStart ) || ( m_pRecomputeData->m_edgeListCount>1 && useTexSwitch && edgeCur.m_switchTexture ) )
                            buildVB_Static_Quad( m_meshBuildData->m_staticIndexList[config->m_idTexSwitch], &posExtremity[0], &uvToApply[edgeRunCur.m_idZone].m_uvStartExtSwitch[0], &colorExtremity[0], btrue, &zExtrude[0] );
                        else
                            buildVB_Static_Quad( indexList, &posExtremity[0], &uvToApply[edgeRunCur.m_idZone].m_uvStartExt[0], &colorExtremity[0], btrue, &zExtrude[0] );
                    }
                }
            }

            // build VB edgeRun
            Vec2d uvUp( getUvX_Start(), uvToApply[edgeRunCur.m_idZone].m_uvYup );
            Vec2d uvDown( uvUp.x(), uvToApply[edgeRunCur.m_idZone].m_uvYdown );

            buildVB_Static_EdgeRun( config, _edgeList, edgeRunCur, indexList, vtxColorUp, vtxColorDown, vtxColorUp, vtxColorDown, indexVtxUp, indexVtxDown, uvUp, uvDown, uvToApply[edgeRunCur.m_idZone].m_uvXsign *m_pRecomputeData->m_uvXsign );             
        }

        if ( !m_pointsList.isLooping() )
        {
            // stop edge
            edgeRun& edgeRunLast = _edgeRunList[edgeRunListCount -1];            
            u32 idEdgeLast = ( edgeRunLast.m_idEdgeStart +edgeRunLast.m_edgeCount +m_pRecomputeData->m_edgeListCount -1 ) %m_pRecomputeData->m_edgeListCount;
            edgeFrieze& edgeLast = _edgeList[idEdgeLast];

            if ( !isEdgeWithHoleVisual(edgeLast))
            {
                if ( !snapExtremity )
                    heightMidExtremity = edgeLast.m_heightStop;

                setQuadPos_StopExtremity( config, edgeLast, &posExtremity[0], heightMidExtremity, edgeLast.m_heightStop, snapExtremity, &zExtrude[0] );
                setQuadColor_StopExtremity( &colorExtremity[0], vtxColorDown, vtxColorUp, vtxColorDownExtremity, vtxColorUpExtremity );

                if ( ( m_pRecomputeData->m_edgeListCount==1 && switchTextureExtStop ) || ( m_pRecomputeData->m_edgeListCount>1 && useTexSwitch && edgeLast.m_switchTexture) )
                    buildVB_Static_Quad( m_meshBuildData->m_staticIndexList[config->m_idTexSwitch], &posExtremity[0], &uvToApply[edgeRunLast.m_idZone].m_uvStopExtSwitch[0], &colorExtremity[0], bfalse, &zExtrude[0] );
                else
                    buildVB_Static_Quad( indexList, &posExtremity[0], &uvToApply[edgeRunLast.m_idZone].m_uvStopExt[0], &colorExtremity[0], bfalse, &zExtrude[0] );
            }
        }
    } 

    void Frise::buildVB_Static_CornerSquareExt_InPipe( const FriseConfig* config, IndexList& _indexList, edgeFrieze& _edgeCur, edgeFrieze& _edgeLast, Vec2d* _uv, u32 _colorUp, u32 _colorDown, bbool _flipEdge )
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
       
        ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
        const u32 vtxListCount = vtxList.size();
        vtxList.resize( vtxListCount + 4 );

        u16 indexVtxDown = (u16)vtxListCount;
        u16 indexVtxUp = indexVtxDown +1;

        Vec2d *v0, *v3, *v2, *v1;
        f32 zVtx[4];

        if ( _edgeCur.m_cornerAngle >0.f )
        {
            v0 = &_edgeLast.m_points[3];
            v1 = &_edgeCur.m_interUp;

            zVtx[0] = m_pRecomputeData->m_zVtxUp;
            zVtx[1] = m_pRecomputeData->m_zVtxUp;
            zVtx[2] = m_pRecomputeData->m_zVtxDown;
            zVtx[3] = m_pRecomputeData->m_zVtxUp;
        }
        else
        {
            v0 = &_edgeCur.m_interDown;
            v1 = &_edgeLast.m_points[2];

            zVtx[0] = m_pRecomputeData->m_zVtxDown;
            zVtx[1] = m_pRecomputeData->m_zVtxDown;
            zVtx[2] = m_pRecomputeData->m_zVtxDown;
            zVtx[3] = m_pRecomputeData->m_zVtxUp;
        }
                
        v2 = &_edgeCur.m_points[0];
        v3 = &_edgeCur.m_points[1];
                
        VertexPCT& vtx0 = vtxList[ indexVtxDown ];
        vtx0.m_pos.x() = v0->x();
        vtx0.m_pos.y() = v0->y();
        vtx0.m_pos.z() = zVtx[0];
        vtx0.m_uv = _uv[0];        

        VertexPCT& vtx1 = vtxList[ indexVtxUp ];
        vtx1.m_pos.x() = v1->x();
        vtx1.m_pos.y() = v1->y();
        vtx1.m_pos.z() = zVtx[1];
        vtx1.m_uv = _uv[1];
        
        VertexPCT& vtx2 = vtxList[ indexVtxDown +2];
        vtx2.m_pos.x() = v2->x();
        vtx2.m_pos.y() = v2->y();
        vtx2.m_pos.z() = zVtx[2];
        vtx2.m_uv = _uv[2]; 
        
        VertexPCT& vtx3 = vtxList[ indexVtxUp +2];
        vtx3.m_pos.x() = v3->x();
        vtx3.m_pos.y() = v3->y();
        vtx3.m_pos.z() = zVtx[3];
        vtx3.m_uv = _uv[3];       

        // color
        if ( _edgeCur.m_cornerAngle > 0.f )
        {
            vtx2.setColor(_colorDown);
            vtx0.setColor(_colorUp);
            vtx1.setColor(_colorUp);
            vtx3.setColor(_colorUp);            
        }
        else
        {
            vtx3.setColor( _colorUp);            
            vtx0.setColor(_colorDown);         
            vtx1.setColor(_colorDown);
            vtx2.setColor(_colorDown);
        }

        addIndex_Static_Quad( _indexList, indexVtxDown, indexVtxUp, _flipEdge );
    }

    void Frise::buildCollision_InPipe( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList  )
    {
        if ( !buildCollisionRunList( _edgeList, _edgeRunList, _collisionRunList ) )
            return;

        buildCollision_InPipe_CornerSquare( _edgeList, _edgeRunList, _collisionRunList );

        finalizeCollisionList();
    }

    void Frise::buildCollision_InPipe_CornerSquare( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
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
                if ( config->m_collisionFrieze.m_isSmooth)
                {
                    u32 idEdgeCur = edgeRunCur->m_idEdgeStart;
                    edgeFrieze* edgeCur = &_edgeList[idEdgeCur];
                    u32 idEdgeLast = ( idEdgeCur +m_pRecomputeData->m_edgeListCount -1) %m_pRecomputeData->m_edgeListCount;
                    edgeFrieze* edgeLast = &_edgeList[idEdgeLast];

                    Vec2d posSquare = buildCollision_getCornerExtremityWithCoord( config->m_textureConfigs[edgeRunCur->m_idTex], colRun, edgeCur, edgeLast, offsetCur, offsetLast);
                    buildOutline_CornerRounderBlending( collision, edgeLast, edgeCur, colRun.m_collisionOffsetLast, offsetCur, config->m_collisionFrieze.m_smoothFactor, 1, &posSquare );  
                }
                else
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
                    // transition between two edgeRun
                    if ( config->m_collisionFrieze.m_isSmooth)
                    {
                        u32 idEdgeCur = edgeRunCur->m_idEdgeStart;
                        edgeFrieze* edgeCur = &_edgeList[idEdgeCur];
                        u32 idEdgeLast = ( idEdgeCur +m_pRecomputeData->m_edgeListCount -1) %m_pRecomputeData->m_edgeListCount;
                        edgeFrieze* edgeLast = &_edgeList[idEdgeLast];

                        Vec2d posSquare = buildCollision_getCornerExtremityWithCoord( config->m_textureConfigs[edgeRunCur->m_idTex], colRun, edgeCur, edgeLast, offsetCur, offsetLast);
                        buildOutline_CornerRounderBlending( collision, edgeLast, edgeCur, offsetLast, offsetCur, config->m_collisionFrieze.m_smoothFactor, 0, &posSquare );                        
                    }
                    else
                        buildCollision_ExtremityWithCoord( config, collision, _edgeList, edgeRunCur, colRun, offsetCur, offsetLast );                     
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

            if ( edgeRunNext->m_idTex != -1 && colRun.m_collisionOffsetNext != -1.f )
            {
                if ( config->m_collisionFrieze.m_isSmooth)
                {
                    u32 idEdgeCur = edgeRunNext->m_idEdgeStart;
                    edgeFrieze* edgeCur = &_edgeList[idEdgeCur];
                    u32 idEdgeLast = ( idEdgeCur +m_pRecomputeData->m_edgeListCount -1) %m_pRecomputeData->m_edgeListCount;
                    edgeFrieze* edgeLast = &_edgeList[idEdgeLast];

                    Vec2d posSquare = buildCollision_getCornerExtremityWithCoord( config->m_textureConfigs[edgeRunNext->m_idTex], colRun, edgeCur, edgeLast, offsetCur, offsetLast);
                    buildOutline_CornerRounderBlending( collision, edgeLast, edgeCur, offsetCur, colRun.m_collisionOffsetNext, config->m_collisionFrieze.m_smoothFactor, -1, &posSquare );
                }
                else
                    buildCollision_ExtremityWithCoord( config, collision, _edgeList, edgeRunNext, colRun, colRun.m_collisionOffsetNext, offsetCur, bfalse );
            }
            else       
            {
                u32 idStopEdge = ( colRun.m_idEdgeStart + colRun.m_edgeCount +m_pRecomputeData->m_edgeListCount -1 ) % m_pRecomputeData->m_edgeListCount;
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

    bbool Frise::isEdgeRun_InPipe( ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idCurEdge, u32 _idLastEdge, u32 _idLastLastEdge ) const
    {        
        if ( m_lockTexture == LockTexture_Switch )
            return btrue;

        u32 idZoneLastEdge = getZoneId_InPipe( _edgeList[_idLastEdge].m_sightNormalized );

        if ( getZoneId_InPipe( _edgeList[_idLastLastEdge].m_sightNormalized ) == idZoneLastEdge )
            return bfalse;

        if ( getZoneId_InPipe( _edgeList[_idCurEdge].m_sightNormalized ) == idZoneLastEdge )
            return bfalse;

        return btrue;
    }

} // namespace ITF
