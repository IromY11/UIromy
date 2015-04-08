#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    void Frise::buildFrieze_InGeneric( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {
        copyEdgeFromPolyline(_edgeList);

        buildEdgeList( _edgeList);

        if ( m_pRecomputeData->m_edgeListCount  == 0 )
            return;
    
#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        if ( !buildEdgeRunList_InGeneric( _edgeList, _edgeRunList ) )
        {
            buildVB_FillGradient( _edgeList );
            return;
        }

        const FriseConfig* config = getConfig();

        if ( m_pointsList.isLooping() )
            setEdgeRunCoeffUv( config, _edgeList, _edgeRunList[0] );
        else
        {
            setEdgeRunListNormUv( _edgeList, _edgeRunList );
        }       

        buildVB_InGeneric( _edgeList, _edgeRunList );  
    }

    bbool Frise::buildEdgeRunList_InGeneric( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        const FriseConfig* config = getConfig();

        _edgeRunList.reserve(1);

        edgeRun edgeRunCur;
        edgeRunCur.m_edgeCount = m_pRecomputeData->m_edgeListCount;
        edgeRunCur.m_idTex = getFirstNoFillTextureIndex( config );

        _edgeRunList.push_back(edgeRunCur);

        if ( edgeRunCur.m_idTex == -1 )
            return bfalse;

        return btrue;
    }

    void Frise::buildVB_InGeneric( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return;

        u16 indexVtxUp = 0;
        u16 indexVtxDown = 0;

        const edgeRun& edgeRunCur = _edgeRunList[0];

        u32 vtxColorDown, vtxColorUp, vtxColorUpExtremity, vtxColorDownExtremity;
        setVtxColorWithExtrem( config->m_textureConfigs[edgeRunCur.m_idTex], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity );

        // uv
        f32 uvYup = m_uvYoffSet;
        f32 uvYdown = uvYup + m_uvY_tile;

        if ( m_pRecomputeData->m_isUvFlipY )
        {
            f32 temp = uvYup;
            uvYup = uvYdown;
            uvYdown = temp;
        }

        Vec2d uvUp( m_uvXoffSet, uvYup);
        Vec2d uvDown( m_uvXoffSet, uvYdown);
        
        if ( m_pRecomputeData->m_anim )
        {
            buildVB_Anim_EdgeRun( config, _edgeList, edgeRunCur, m_meshBuildData->m_animIndexList[edgeRunCur.m_idTex], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity, indexVtxUp, indexVtxDown, uvUp, uvDown, m_pRecomputeData->m_uvXsign );   
        }
        else
        {
            buildVB_Static_EdgeRun( config, _edgeList, edgeRunCur, m_meshBuildData->m_staticIndexList[edgeRunCur.m_idTex], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity, indexVtxUp, indexVtxDown, uvUp, uvDown, m_pRecomputeData->m_uvXsign );   
        }
    }

    void Frise::buildCollision_InGeneric( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {
        if ( !buildCollisionRunList( _edgeList, _edgeRunList, _collisionRunList ) )
            return;

        buildCollision_EdgeRunList ( _edgeList, _edgeRunList, _collisionRunList );

        finalizeCollisionList();
    }

} // namespace ITF
