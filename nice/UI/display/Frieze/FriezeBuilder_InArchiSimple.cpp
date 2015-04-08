#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

namespace ITF
{   
    static UvLinearArchi uv[]=
    {
        // TOP
        UvLinearArchi( 0.125f,   0.f,    1.f,
        Vec2d( 0.f, 0.625f ),   Vec2d( 0.f, 0.5f ),     Vec2d( 0.5f, 0.625f ), Vec2d( 0.5f, 0.5f ), Vec2d( 0.f, 0.f),      // OUT
        Vec2d( 0.5f, 0.75f ),  Vec2d( 0.5f, 0.625f ), Vec2d( 1.f, 0.75f ),   Vec2d( 1.f, 0.625f ), Vec2d( 0.f, 0.f) ), // IN 

        // RIGHT
        UvLinearArchi( 0.25f,    0.125f, 1.f,
        Vec2d( 0.5f, 0.5f ),   Vec2d( 1.f, 0.5f ), Vec2d( 0.5f, 0.625f ), Vec2d( 1.f, 0.625f ), Vec2d( 0.f, 0.f),   // OUT
        Vec2d( 0.5f, 0.75f ),  Vec2d( 1.f, 0.75f ), Vec2d( 0.5f, 0.875f ), Vec2d( 1.f, 0.875f ), Vec2d( 0.f, 0.f) ),   // IN  

        // LEFT
        UvLinearArchi( 0.375f,   0.25f,  1.f,
        Vec2d( 0.5f, 1.f ),    Vec2d( 0.f, 1.f ),   Vec2d( 0.5f, 0.875f ), Vec2d( 0.f, 0.875f ), Vec2d( 0.f, 0.f), // OUT
        Vec2d( 0.5f, 0.75f ),  Vec2d( 0.f, 0.75f ), Vec2d( 0.5f, 0.625f ), Vec2d( 0.f, 0.625f ), Vec2d( 0.f, 0.f) ), // IN 

        // BOTTOM
        UvLinearArchi( 0.375f,   0.5f,   -1.f,
        Vec2d( 1.f, 0.875f ), Vec2d( 1.f, 1.f ),      Vec2d( 0.5f, 0.875f ), Vec2d( 0.5f, 1.f ), Vec2d( 0.f, 0.f),   // OUT
        Vec2d( 0.5f, 0.75f ), Vec2d( 0.5f, 0.875f ),  Vec2d( 0.f, 0.75f ),    Vec2d( 0.f, 0.875f ), Vec2d( 0.f, 0.f) ), // IN 
    };

    static UvLinearArchi uvFlipped[]=
    {
        // TOP
        UvLinearArchi( 0.125f,   0.f,    1.f,        
        Vec2d( 1.f, 0.625f ),   Vec2d( 1.f, 0.5f ),     Vec2d( 0.5f, 0.625f ),  Vec2d( 0.5f, 0.5f ),    Vec2d( 0.f, 0.f),      // OUT
        Vec2d( 0.5f, 0.75f ),   Vec2d( 0.5f, 0.625f ),  Vec2d( 0.f, 0.75f ),    Vec2d( 0.f, 0.625f ),   Vec2d( 0.f, 0.f) ), // IN 

        // RIGHT
        UvLinearArchi( 0.375f,   0.25f, 1.f,
        Vec2d( 0.5f, 0.5f ),   Vec2d( 0.f, 0.5f ),  Vec2d( 0.5f, 0.625f ), Vec2d( 0.f, 0.625f ), Vec2d( 0.f, 0.f),   // OUT
        Vec2d( 0.5f, 0.75f ),  Vec2d( 0.f, 0.75f ), Vec2d( 0.5f, 0.875f ), Vec2d( 0.f, 0.875f ), Vec2d( 0.f, 0.f) ),   // IN  

        // LEFT
        UvLinearArchi( 0.25f,    0.125f,  1.f,
        Vec2d( 0.5f, 1.f ),    Vec2d( 1.f, 1.f ),   Vec2d( 0.5f, 0.875f ), Vec2d( 1.f, 0.875f ), Vec2d( 0.f, 0.f), // OUT
        Vec2d( 0.5f, 0.75f ),  Vec2d( 1.f, 0.75f ), Vec2d( 0.5f, 0.625f ), Vec2d( 1.f, 0.625f ), Vec2d( 0.f, 0.f) ), // IN 

        // BOTTOM
        UvLinearArchi( 0.375f,   0.5f,   -1.f,
        Vec2d( 0.f, 0.875f ),   Vec2d( 0.f, 1.f ),      Vec2d( 0.5f, 0.875f ),  Vec2d( 0.5f, 1.f ),   Vec2d( 0.f, 0.f),   // OUT
        Vec2d( 0.5f, 0.75f ),   Vec2d( 0.5f, 0.875f ),  Vec2d( 1.f, 0.75f ),    Vec2d( 1.f, 0.875f ), Vec2d( 0.f, 0.f) ), // IN 
    };

    void Frise::buildFrieze_InArchiSimple( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {
        copyEdgeFromPolyline(_edgeList);        

        m_pRecomputeData->m_texRatio = 0.5f;    

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
 } // namespace ITF
