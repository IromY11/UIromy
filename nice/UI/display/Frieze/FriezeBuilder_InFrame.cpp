#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{   
    static i32 idTexConfig_InFrame;

    static const u32 idTopZoneUv       = 0;
    static const u32 idRightZoneUv     = 1;
    static const u32 idLeftZoneUv      = 2;
    static const u32 idBottomZoneUv    = 3;

    struct UvsByAngle
    {
        UvsByAngle( f32 _start, f32 _stop, u32 _index )
        {
            m_start = _start;
            m_stop  = _stop;
            m_index = _index;
        }

        f32 m_start;
        f32 m_stop;
        u32 m_index;
    };

    static UvsByAngle indexZoneByAngle[]=
    {
        UvsByAngle( -MTH_PIBY4,         MTH_PIBY4       , idTopZoneUv ),       // TOP
        UvsByAngle( -3.f *MTH_PIBY4,    -MTH_PIBY4      , idRightZoneUv ),     // RIGHT
        UvsByAngle( MTH_PIBY4,          3.f *MTH_PIBY4  , idLeftZoneUv ),      // LEFT
        UvsByAngle( 3.f *MTH_PIBY4,     MTH_PI          , idBottomZoneUv ),    // BOTTOM
        UvsByAngle( -MTH_PI,            -3.f *MTH_PIBY4 , idBottomZoneUv ),    // BOTTOM        
    };

    struct UvLinear
    {
         UvLinear( 
             f32 _uvYdown, 
             f32 _uvYup, 
             Vec2d _uvSquare_0,
             Vec2d _uvSquare_1,
             Vec2d _uvSquare_2,
             Vec2d _uvSquare_3
             )
             : m_uvYdown(_uvYdown)
             , m_uvYup(_uvYup)

         {
             m_uvSquare[0] = _uvSquare_0;
             m_uvSquare[1] = _uvSquare_1;
             m_uvSquare[2] = _uvSquare_2;
             m_uvSquare[3] = _uvSquare_3;
             m_uvSquare[4] = Vec2d::Zero;
         }

        f32 m_uvYdown;
        f32 m_uvYup;
        Vec2d m_uvSquare[5];      
    };

    static UvLinear uv[]=
    {
        // TOP
        UvLinear( 0.125f,   0.f,
        Vec2d( 0.f, 0.625f ),  Vec2d( 0.f, 0.5f ), Vec2d( 1.f, 0.625f ), Vec2d( 1.f, 0.5f) ), 

        // RIGHT
        UvLinear( 0.25f,    0.125f,
        Vec2d( 0.f, 0.625f ),  Vec2d( 1.f, 0.625f ), Vec2d( 0.f, 0.75f ), Vec2d( 1.f, 0.75f) ),  

        // LEFT
        UvLinear( 0.5f,   0.375f,
        Vec2d( 1.f, 1.f ),  Vec2d( 0.f, 1.f ), Vec2d( 1.f, 0.875f ), Vec2d( 0.f, 0.875f) ),

        // BOTTOM
        UvLinear( 0.375f,   0.25f,
        Vec2d( 1.f, 0.75f ), Vec2d( 1.f, 0.875f ),  Vec2d( 0.f, 0.75f ), Vec2d( 0.f, 0.875f) ),
    };

    u32 getZoneId( const Vec2d& _sight )
    {
        f32 angle = atan2( _sight.y(), _sight.x() );

        for ( u32 i = 0; i < 5; i++ )
        {
            if ( angle >= indexZoneByAngle[i].m_start && angle <= indexZoneByAngle[i].m_stop )
                return indexZoneByAngle[i].m_index;
        }

        return 0;
    }

    void Frise::buildFrieze_InFrame( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {
        copyEdgeFromPolyline(_edgeList);        

        m_pRecomputeData->m_texRatio = 1.f;    

        buildEdgeList( _edgeList);

        if ( m_pRecomputeData->m_edgeListCount == 0 )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        if ( !buildEdgeRunList_InFrame( _edgeList, _edgeRunList ) )
            return;

        setEdgeRunListCoeffUv( _edgeList, _edgeRunList );

        buildVB_InFrame_Static( _edgeList, _edgeRunList );        
    }

    u32 Frise::getIdStartEdge_InFrame( ITF_VECTOR<edgeFrieze>& _edgeList, f32 _nornMin ) const
    {
        if ( !m_pointsList.isLooping() )
            return 0;

        u32 idZoneEdgeStart = getZoneId( _edgeList[0].m_sightNormalized );

        for (u32 i=1; i<m_pRecomputeData->m_edgeListCount; i++)
        {
            if ( idZoneEdgeStart != getZoneId( _edgeList[i].m_sightNormalized ) )
                return i;
        }

        return 0;
    } 

    bbool Frise::buildEdgeRunList_InFrame( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return bfalse;

        _edgeRunList.reserve(m_pRecomputeData->m_edgeListCount);

        edgeRun edgeRunCur;
        edgeRunCur.m_edgeCount = m_pRecomputeData->m_edgeListCount; 
        edgeRunCur.m_idTex = getFirstNoFillTextureIndex( config );

        // frieze texture
        idTexConfig_InFrame = edgeRunCur.m_idTex;

        if ( idTexConfig_InFrame == -1 )
        {
            _edgeRunList.push_back(edgeRunCur);
            return bfalse;
        }

        f32 edgeRunNornMin = getEdgeRunNormMin( config );
        u32 idEdgeStart = getIdStartEdge_InFrame( _edgeList, edgeRunNornMin );
        u32 idZone = getZoneId( _edgeList[idEdgeStart].m_sightNormalized );  
        
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
                edgeRunCur.m_idTex = idTexConfig_InFrame;

            ++countEdge;
            ++idCurEdge;
            edgeRunCur.m_edgeCount = 1;

            if ( !isEdgeWithHoleVisual(_edgeList[edgeRunCur.m_idEdgeStart]))
            {
                for( ; countEdge<m_pRecomputeData->m_edgeListCount; countEdge++, idCurEdge++ )
                {
                    idCurEdge = idCurEdge % m_pRecomputeData->m_edgeListCount;
                    const edgeFrieze& edgeCur = _edgeList[idCurEdge];

                    if ( isEdgeWithHoleVisual( edgeCur))
                        break;

                    idZone = getZoneId( edgeCur.m_sightNormalized );

                    if ( edgeRunCur.m_idZone != idZone )    
                        break;

                    edgeRunCur.m_edgeCount++;
                }
            }
            else
            {
                idCurEdge = idCurEdge % m_pRecomputeData->m_edgeListCount;
                idZone = getZoneId( _edgeList[idCurEdge].m_sightNormalized );
            }

            _edgeRunList.push_back(edgeRunCur);
        }

        return btrue;
    }

    void Frise::buildVB_InFrame_Static( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {
        const FriseConfig* pConfig = getConfig();
        ITF_ASSERT_CRASH( pConfig, "Config file is missing");

        u16 indexVtxUp = 0;
        u16 indexVtxDown = 0;
        const u32 edgeRunListCount = _edgeRunList.size();
        
        //  colors vertexs 
        u32 vtxColorDownInit, vtxColorUpInit;
        setVtxColor( pConfig->m_textureConfigs[idTexConfig_InFrame], vtxColorUpInit, vtxColorDownInit ); 
       
        IndexList& indexList = m_meshBuildData->m_staticIndexList[idTexConfig_InFrame];

        for( u32 idEdgeRun = 0; idEdgeRun < edgeRunListCount; idEdgeRun ++ )
        {
            edgeRun& edgeRunCur = _edgeRunList[idEdgeRun];            

            // start edge
            edgeFrieze& edgeCur = _edgeList[edgeRunCur.m_idEdgeStart];

            // color
            u32 vtxColorDownEdgeRun = vtxColorDownInit;
            u32 vtxColorUpEdgeRun = vtxColorUpInit;
            u32 vtxColorDownExt = vtxColorDownInit;
            u32 vtxColorUpExt = vtxColorUpInit;
            
            // build extremity on start edge
            if ( edgeCur.m_cornerAngle != 0.f && !isEdgeWithHoleVisual(edgeCur))
            {
                // edge Last
                const u32 idEdgeLast = ( edgeRunCur.m_idEdgeStart +m_pRecomputeData->m_edgeListCount -1 ) %m_pRecomputeData->m_edgeListCount;
                const edgeFrieze& edgeLast = _edgeList[idEdgeLast];
 
                if ( !isEdgeWithHoleVisual(edgeLast))
                {
                    buildVB_Static_CornerSquareExt( pConfig, indexList, edgeCur, edgeLast, &uv[edgeRunCur.m_idZone].m_uvSquare[0], vtxColorUpExt, vtxColorDownExt, btrue );
                }                               
            }

            // build VB edgeRun
            Vec2d uvUp( getUvX_Start(), uv[edgeRunCur.m_idZone].m_uvYup );
            Vec2d uvDown( uvUp.x(), uv[edgeRunCur.m_idZone].m_uvYdown );

            buildVB_Static_EdgeRun( pConfig, _edgeList, edgeRunCur, indexList, vtxColorUpEdgeRun, vtxColorDownEdgeRun, vtxColorUpEdgeRun, vtxColorDownEdgeRun, indexVtxUp, indexVtxDown, uvUp, uvDown, m_pRecomputeData->m_uvXsign );             
        }
    } 
} // namespace ITF