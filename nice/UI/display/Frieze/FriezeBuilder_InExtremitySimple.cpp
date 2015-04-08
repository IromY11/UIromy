#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    void Frise::buildFrieze_InExtremitySimple( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return;

        copyEdgeFromPolyline(_edgeList);        

        m_pRecomputeData->m_texRatio = 1.0;
        const f32 extremityRatio = 0.5f;
        const f32 extremityOffset = getExtremityLong(extremityRatio);

        subtractOffsetExtremity( _edgeList, extremityOffset, extremityOffset );

        buildEdgeList( _edgeList );

        if ( m_pRecomputeData->m_edgeListCount == 0 )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        if ( !buildEdgeRunList_InExtremity( _edgeList, _edgeRunList ) )
            return;

        setEdgeRunCoeffUv( config, _edgeList, _edgeRunList[0] );

        Vec2d uvXExtremityStart, uvXExtremityStop;
        setUvXExtremity_InExtremitySimple( uvXExtremityStart, uvXExtremityStop );

        buildVB_InExtremity( _edgeList, _edgeRunList, extremityRatio, uvXExtremityStart, uvXExtremityStop );
    }

    void Frise::setUvXExtremity_InExtremitySimple( Vec2d& _uvXExtremityStart, Vec2d& _uvXExtremityStop ) const
    {
        if ( m_pRecomputeData->m_uvXsign < 0.f )
        {
            _uvXExtremityStart.x() = 0.5f;
            _uvXExtremityStart.y() = 0.0f;

            _uvXExtremityStop.x() = 1.f;
            _uvXExtremityStop.y() = 0.5f;
        }
        else
        {
            _uvXExtremityStart.x() = 0.5f;
            _uvXExtremityStart.y() = 1.f;

            _uvXExtremityStop.x() = 0.f;
            _uvXExtremityStop.y() = 0.5f;
        }
    }
} // namespace ITF
