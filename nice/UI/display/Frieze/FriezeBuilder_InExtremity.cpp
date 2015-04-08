#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    f32 Frise::getExtremityLong( f32 _ratio ) const
    {
        return m_pRecomputeData->m_widthScale *_ratio;
    }

    void Frise::subtractOffsetExtremity( ITF_VECTOR<edgeFrieze>& _edgeList, f32 _startOffset, f32 _stopOffset ) const
    {
        if ( m_pRecomputeData->m_edgeListCount == 0 )
            return;
        
        const f32 epsilon = 0.001f;
        const f32 visualOffset = -getExtremityLong(getConfig()->m_offsetExtremity);

        // start edge
        edgeFrieze& startEdge = _edgeList[0];
        Vec2d newPos;
        f32 edgeNorm = startEdge.m_sight.norm();    
        f32 startOffset = (_startOffset + visualOffset ) * startEdge.m_scaleCur;

        if (m_pRecomputeData->m_edgeListCount == 1 && startOffset > edgeNorm *0.5f +epsilon)
        {
            newPos = startEdge.m_pos + startEdge.m_sightNormalized *(edgeNorm *0.5f -epsilon); 
        }
        else
        {
            if ( edgeNorm > startOffset +epsilon )
            {
                newPos = startEdge.m_pos + startEdge.m_sightNormalized *startOffset;            
            }
            else
            {
                newPos = startEdge.m_pos + startEdge.m_sightNormalized *( edgeNorm -epsilon);
            }
        }
        updateEdgeFriezePosition( startEdge, newPos );

        // stop edge
        edgeFrieze& stopEdge = _edgeList[m_pRecomputeData->m_edgeListCount -1];
        Vec2d newSight;
        f32 stopOffset = (_stopOffset + visualOffset) * stopEdge.m_heightStop * m_pRecomputeData->m_heightScaleInv;

        if (  stopEdge.m_sight.norm() > stopOffset +epsilon )
        {
            newSight = stopEdge.m_sight -stopEdge.m_sightNormalized *stopOffset;
        }
        else
        {
            newSight = stopEdge.m_sightNormalized *epsilon;
        }

        updateEdgeFriezeVector( stopEdge, newSight );
    }

    void Frise::buildFrieze_InExtremity( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, const f32 _texRatio )
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return;

        copyEdgeFromPolyline(_edgeList);        

        m_pRecomputeData->m_texRatio = _texRatio;
        const f32 extremityRatio = 0.25f;
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
        setUvXExtremity_InExtremity( uvXExtremityStart, uvXExtremityStop );
            
        buildVB_InExtremity( _edgeList, _edgeRunList, extremityRatio, uvXExtremityStart, uvXExtremityStop );
    }

    bbool Frise::buildEdgeRunList_InExtremity( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return bfalse;

        _edgeRunList.reserve(1);

        edgeRun edgeRunCur;
        edgeRunCur.m_edgeCount = m_pRecomputeData->m_edgeListCount;

        edgeRunCur.m_idTex = getFirstNoFillTextureIndex( config );

        _edgeRunList.push_back(edgeRunCur);

        if ( edgeRunCur.m_idTex == -1 )
            return bfalse;

        return btrue;
    }

    void Frise::buildVB_InExtremity( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, f32 _extremityRatio, const Vec2d& _uvXExtremityStart, const Vec2d& _uvXExtremityStop )
    {        
        const FriseConfig* config = getConfig();
        
        u16 indexVtxUp = 0;
        u16 indexVtxDown = 0;

        const edgeRun& edgeRunCur = _edgeRunList[0];

        //  colors vertexs 
        u32 vtxColorDown, vtxColorUp, vtxColorUpExtremity, vtxColorDownExtremity;
        setVtxColorWithExtrem( config->m_textureConfigs[edgeRunCur.m_idTex], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity );

        // uv
        f32 uvYup = 0.f;
        f32 uvYdown = 0.5f;

        f32 uvYupExt = 0.5f;
        f32 uvYdownExt = 1.f;

        if ( m_pRecomputeData->m_isUvFlipY )
        {
            uvYup = 0.5f;
            uvYdown = 0.f;

            uvYupExt = 1.f;
            uvYdownExt = 0.5f;
        }

        Vec2d uvDown( getUvX_Start(), uvYdown);
        Vec2d uvUp( uvDown.x(), uvYup);
        
        // Extremity
        const f32 extremityLong = getExtremityLong( _extremityRatio );
        const f32 uvXOffsetExtremity = _extremityRatio * m_pRecomputeData->m_uvXsign;
        Vec2d posExtremity[4];
        Vec2d uvExtremity[4];
        f32 zExtrude[4];
        u32 colorExtremity[4];

        // scale extremity
        const edgeFrieze& edgeStart = _edgeList[0];
        f32 heightMidExtremity = edgeStart.m_heightStart;
        bbool snapExtremity = bfalse;

        if ( m_pRecomputeData->m_edgeListCount == 1 && edgeStart.m_snap )
        {
            snapExtremity = btrue;
            heightMidExtremity = ( edgeStart.m_heightStart + edgeStart.m_heightStop) *0.5f;
        }

        // build start extremity   
        if ( !isEdgeWithHoleVisual(edgeStart))
        {
            const f32 extremityLongStart = extremityLong *edgeStart.m_heightStart * m_pRecomputeData->m_heightScaleInv;
            setQuadPos_StartExtremity( config, edgeStart, &posExtremity[0], heightMidExtremity, extremityLongStart, snapExtremity, &zExtrude[0] );
            setQuadColor_StartExtremity( &colorExtremity[0], vtxColorDown, vtxColorUp, vtxColorDownExtremity, vtxColorUpExtremity );
        }

        if ( config->m_switchExtremityAuto )
        {
            uvExtremity[0] = Vec2d( uvDown.x() -uvXOffsetExtremity, uvYdownExt );
            uvExtremity[1] = Vec2d( uvExtremity[0].x(), uvYupExt );
            uvExtremity[2] = Vec2d( uvDown.x(), uvYdownExt );
            uvExtremity[3] = Vec2d( uvUp.x(), uvYupExt );            
        }
        else
        {
            uvExtremity[0] = Vec2d( _uvXExtremityStart.x(), uvYdownExt );
            uvExtremity[1] = Vec2d( _uvXExtremityStart.x(), uvYupExt );
            uvExtremity[2] = Vec2d( _uvXExtremityStart.y(), uvYdownExt );
            uvExtremity[3] = Vec2d( _uvXExtremityStart.y(), uvYupExt );
        }


        if ( m_pRecomputeData->m_anim )
        {
            IndexList& indexList = m_meshBuildData->m_animIndexList[ edgeRunCur.m_idTex ];

            if ( !isEdgeWithHoleVisual(edgeStart))
                buildVB_Anim_Quad( config->m_vtxAnim, indexList, &posExtremity[0], &uvExtremity[0], &colorExtremity[0], btrue, btrue, snapExtremity, &zExtrude[0] );    

            buildVB_Anim_EdgeRun( config, _edgeList, edgeRunCur, indexList, vtxColorUp, vtxColorDown, vtxColorUp, vtxColorDown, indexVtxUp, indexVtxDown, uvUp, uvDown, m_pRecomputeData->m_uvXsign );   

            // anim start extremity
            if ( !isEdgeWithHoleVisual(edgeStart) && !snapExtremity && m_meshBuildData->m_animVertexList.size() > 5 )
            {
                copyVtxAnim( m_meshBuildData->m_animVertexList[3], m_meshBuildData->m_animVertexList[5] );
            }
        }
        else
        {
            IndexList& indexList = m_meshBuildData->m_staticIndexList[edgeRunCur.m_idTex];

            if ( !isEdgeWithHoleVisual(edgeStart))
                buildVB_Static_Quad( indexList, &posExtremity[0], &uvExtremity[0], &colorExtremity[0], btrue, &zExtrude[0] );

            buildVB_Static_EdgeRun( config, _edgeList, edgeRunCur, indexList, vtxColorUp, vtxColorDown, vtxColorUp, vtxColorDown, indexVtxUp, indexVtxDown, uvUp, uvDown, m_pRecomputeData->m_uvXsign );   
        }

        // build stop extremity
        const u32 idLastEdge = m_pRecomputeData->m_edgeListCount -1;
        const edgeFrieze& edgeStop = _edgeList[idLastEdge];

        if ( !isEdgeWithHoleVisual(edgeStop))
        {
            if ( !snapExtremity )
                heightMidExtremity = edgeStop.m_heightStop;

            const f32 extremityLongStop = extremityLong *edgeStop.m_heightStop * m_pRecomputeData->m_heightScaleInv;
            setQuadPos_StopExtremity( config, edgeStop, &posExtremity[0], heightMidExtremity, extremityLongStop, snapExtremity, &zExtrude[0] );
            setQuadColor_StopExtremity( &colorExtremity[0], vtxColorDown, vtxColorUp, vtxColorDownExtremity, vtxColorUpExtremity );

            if ( config->m_switchExtremityAuto )
            {
                uvExtremity[0].x() = uvDown.x();
                uvExtremity[1].x() = uvExtremity[0].x();
                uvExtremity[2].x() = uvExtremity[0].x() +uvXOffsetExtremity;
                uvExtremity[3].x() = uvExtremity[2].x();
            }
            else
            {                
                uvExtremity[0].x() = _uvXExtremityStop.x();
                uvExtremity[1].x() = _uvXExtremityStop.x();
                uvExtremity[2].x() = _uvXExtremityStop.y();
                uvExtremity[3].x() = _uvXExtremityStop.y();                
            }

            if ( m_pRecomputeData->m_anim )
            {
                buildVB_Anim_Quad( config->m_vtxAnim, m_meshBuildData->m_animIndexList[edgeRunCur.m_idTex], &posExtremity[0], &uvExtremity[0], &colorExtremity[0], bfalse, bfalse, btrue, &zExtrude[0] );

                // stop extremity
                const u32 vertexCount = m_meshBuildData->m_animVertexList.size();
                if ( vertexCount > 6 )
                {
                    u32 indexCopy = vertexCount -5;
                    if ( edgeStop.m_snap && edgeStop.m_cornerAngle < 0.f )
                    {
                        indexCopy = getIndexPreviousAnimVertex( indexCopy);
                    }

                    if ( indexCopy != U32_INVALID )
                    {
                        copyVtxAnim( m_meshBuildData->m_animVertexList[ vertexCount -3], m_meshBuildData->m_animVertexList[ indexCopy ] );                  
                    }
                }                
            }
            else
            {
                buildVB_Static_Quad( m_meshBuildData->m_staticIndexList[edgeRunCur.m_idTex], &posExtremity[0], &uvExtremity[0], &colorExtremity[0], bfalse, &zExtrude[0] );
            }
        }        
    }

    void Frise::buildCollision_InExtremity( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {       
        if ( !buildCollisionRunList( _edgeList, _edgeRunList, _collisionRunList ) )
            return;

        buildCollision_EdgeRunList ( _edgeList, _edgeRunList, _collisionRunList );

        finalizeCollisionList();
    }

    void Frise::setUvXExtremity_InExtremity( Vec2d& _uvXExtremityStart, Vec2d&  _uvXExtremityStop ) const
    {
        if ( m_pRecomputeData->m_uvXsign < 0.f )
        {
            _uvXExtremityStart.x() = 0.25f;
            _uvXExtremityStart.y() = 0.0f;

            _uvXExtremityStop.x() = 0.5f;
            _uvXExtremityStop.y() = 0.25f;
        }
        else
        {
            _uvXExtremityStart.x() = 0.25f;
            _uvXExtremityStart.y() = 0.5f;

            _uvXExtremityStop.x() = 0.f;
            _uvXExtremityStop.y() = 0.25f;
        }

        if ( m_switchExtremityStart )
        {
            _uvXExtremityStart.x() += 0.5f;
            _uvXExtremityStart.y() += 0.5f;
        }

        if ( m_switchExtremityStop )
        {
            _uvXExtremityStop.x() += 0.5f;
            _uvXExtremityStop.y() += 0.5f;
        }
    }
} // namespace ITF
