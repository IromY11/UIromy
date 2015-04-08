#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    u32 Frise::findLastBuildEdgeInLoop( ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdge ) const
    {
        for ( u32 countEdge = 0; countEdge<m_pRecomputeData->m_edgeListCount; countEdge++ )
        {
            _idEdge = (_idEdge +m_pRecomputeData->m_edgeListCount -1) % m_pRecomputeData->m_edgeListCount;

            if ( _edgeList[_idEdge].m_build )
                break;
        }

        return _idEdge;
    }


    bbool Frise::buildEdgeListInSkew( const FriseConfig* _pConfig, ITF_VECTOR<edgeFrieze>& _edgeList ) const
    {
        if ( _pConfig->m_skewAngle == F32_INVALID )
            return bfalse;

        const Vec2d normalEdge = Vec2d::Right.Rotate(_pConfig->m_skewAngle);

        for ( u32 i=0; i<m_pRecomputeData->m_edgeListCount; i++ )
        {
            edgeFrieze& edge = _edgeList[i];

            edge.m_normal = normalEdge;
            edge.m_cornerNormal = normalEdge;
            edge.m_norm = edge.m_sight.norm();
            
            buildEdgePoints( _pConfig, edge );

            edge.m_interDown = edge.m_points[0];
            edge.m_interUp = edge.m_points[1];
        }

        return btrue;
    }

    void Frise::buildEdgeList( ITF_VECTOR<edgeFrieze>& _edgeList, bbool _snapEdgeLast )
    {        
        const FriseConfig* pConfig = getConfig();
        if ( !pConfig || m_pRecomputeData->m_edgeListCount < 1 )
            return;

        if ( buildEdgeListInSkew( pConfig, _edgeList ) )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        // draw init quads
        for ( u32 i = 0; i<0/*m_pRecomputeData->m_edgeListCount*/; i++ )
        {
            edgeFrieze& edge = _edgeList[i];
            Vec2d p1, p2, p3, p4;
            p1 = edge.m_pos -edge.m_normal *edge.m_heightStart *pConfig->m_visualOffset;
            p2 = edge.m_pos +edge.m_sight -edge.m_normal *edge.m_heightStop *pConfig->m_visualOffset;
            p3 = p1 +edge.m_normal *edge.m_heightStart;
            p4 = p2 +edge.m_normal *edge.m_heightStop;

            p1 = transformPosLocalToWorld(p1);
            p2 = transformPosLocalToWorld(p2);
            p3 = transformPosLocalToWorld(p3);
            p4 = transformPosLocalToWorld(p4);

            f32 size = 0.5f;
            GFX_ADAPTER->drawDBGLine( p1, p3, 1,1,0, 0.f, size, 0.f );
            GFX_ADAPTER->drawDBGLine( p3, p4, 1,1,0, 0.f, size, 0.f );
            GFX_ADAPTER->drawDBGLine( p4, p2, 1,1,0, 0.f, size, 0.f );
            GFX_ADAPTER->drawDBGLine( p2, p1, 1,1,0, 0.f, size, 0.f );
        }

        u32 countRecompute = 0;
#endif

        f32 edgeNornMin = getEdgeNormMin( pConfig );
        f32 edgeRunNormMin = getEdgeRunNormMin(pConfig);

        if ( m_pRecomputeData->m_edgeListCount == 1)
        {
            edgeFrieze& edge = _edgeList[0];

            edge.m_points[0] = edge.m_pos -edge.m_normal *edge.m_heightStart *pConfig->m_visualOffset;
            edge.m_points[1] = edge.m_points[0] +edge.m_normal *edge.m_heightStart;

            edge.m_norm = edge.m_sight.norm();
            f32 normMin = edgeNornMin;
            if ( isEdgeRun(_edgeList, pConfig, 0, 0) )                
                normMin = edgeRunNormMin;   

            if ( edge.m_norm <= normMin )
            {
                edge.m_snap = btrue;
                edge.m_norm = 0.f;
                edge.m_stopCoeff = edge.m_startCoeff = 0.f;
                edge.m_points[2] = edge.m_points[0];
                edge.m_points[3] = edge.m_points[1];
            }
            else
            {
                edge.m_points[2] = edge.m_pos +edge.m_sight -edge.m_normal *edge.m_heightStop *pConfig->m_visualOffset;
                edge.m_points[3] = edge.m_points[2] +edge.m_normal *edge.m_heightStop;
            }

            edge.m_interDown = edge.m_points[0];
            edge.m_interUp = edge.m_points[1];            
            edge.m_cornerNormal = edge.m_normal;           
            edge.m_cornerAngle = 0.f;
        }
        else
        {
            f32 cornerLongMax = pConfig->m_cornerFactor * m_pRecomputeData->m_heightScale;        
            u32 idCurEdge = 1;
            u32 idLastEdge = 0;
            u32* idLastEdgeValidList = newAlloc(mId_Frieze, u32[m_pRecomputeData->m_edgeListCountMax]);
            memset( idLastEdgeValidList, 0, m_pRecomputeData->m_edgeListCountMax *sizeof(u32) );
            f32 dotSight;
            u32 countIteration = 0;
            u32 countIterationMax = Max( 100u, m_pRecomputeData->m_edgeListCount *m_pRecomputeData->m_edgeListCount);

            //  skip edge
            bbool edgeRecompute;
            bbool insertNewEdgeInLoop = btrue;
            do 
            {
                edgeRecompute  = bfalse;

                for ( ; idCurEdge<m_pRecomputeData->m_edgeListCount; idCurEdge++)
                {
                    countIteration++;
                    if ( countIteration > countIterationMax)
                    {
#ifdef DEVELOPER_JAY_FRIEZE
                        String8 txt;
                        txt.setTextFormat("loop infinity");
                        GFX_ADAPTER->drawDBGText(txt);
#endif
                        break;
                    }

                    if ( !_edgeList[idCurEdge].m_build )
                        continue;

                    edgeFrieze& edgeCur = _edgeList[idCurEdge];
                    edgeFrieze& edgeLast = _edgeList[idLastEdge];

#ifdef DEVELOPER_JAY_FRIEZE
                    ITF_ASSERT(edgeCur.m_build);
                    ITF_ASSERT(edgeLast.m_build);                    
#endif

                    if ( !buildEdgeCorner( pConfig, edgeCur, edgeLast, cornerLongMax, dotSight ))
                    {
                        skipEdgeFrieze( _edgeList, idCurEdge, idLastEdge, idLastEdgeValidList[idLastEdge] );
                        updateIdEdgeList( idCurEdge, idLastEdge, idLastEdgeValidList);                        

                        edgeRecompute = btrue;
                        break;
                    }

                    if ( insertNewEdge( _edgeList, pConfig, idCurEdge, idLastEdge ) )
                    {                  
                        updateIdEdgeList( idCurEdge, idLastEdge, idLastEdgeValidList );

                        edgeRecompute = btrue;
                        break;
                    }

                    if ( skipWrongEdge( edgeCur, edgeLast, dotSight ) )
                    {
                        skipEdgeFrieze( _edgeList, idCurEdge, idLastEdge, idLastEdgeValidList[idLastEdge] );
                        updateIdEdgeList( idCurEdge, idLastEdge, idLastEdgeValidList); 

                        edgeRecompute = btrue;
                        break;
                    }

                    setNormEdge( edgeLast );

                    if ( !isLooping() || idLastEdge>0 )
                    {
                        f32 normMin = getNormMinToSnap( _edgeList, pConfig, idCurEdge, idLastEdge, edgeNornMin, edgeRunNormMin);

                        if ( skipSmallEdge( _edgeList, edgeLast, edgeCur, normMin, (i32)idLastEdge -1 ) || edgeLast.m_snap )
                        {             
                            if ( edgeCur.m_cornerAngle == 0.f)
                            {
                                skipEdgeFrieze( _edgeList, idCurEdge, idLastEdge, idLastEdgeValidList[idLastEdge] );
                                updateIdEdgeList( idCurEdge, idLastEdge, idLastEdgeValidList); 

                                edgeRecompute = btrue;
                                break;
                            }
                            else if ( setScaleAverage( edgeCur, edgeLast) )
                            { 
                                edgeLast.m_snap = btrue;

                                updateIdEdgeList( idCurEdge, idLastEdge, idLastEdgeValidList );
                                edgeRecompute = btrue;                    
                                break;
                            }

                            snapEdge( _edgeList, idLastEdge, idCurEdge );     
                        }
                    }

                    //  copy last values
                    idLastEdgeValidList[idCurEdge] = idLastEdge;
                    idLastEdge = idCurEdge;                
                }

                // Loop
                if ( !edgeRecompute && isLooping() && getCountBuildEgdes(_edgeList) >1 )
                {
                    idCurEdge = 0;
                    bbool skipEdgeLast = bfalse;                   
                    edgeFrieze& edgeCur = _edgeList[idCurEdge];
                    edgeFrieze& edgeLast = _edgeList[idLastEdge];

                    if ( countIteration > countIterationMax )
                    {
                        _edgeList.clear();
                        m_pRecomputeData->m_edgeListCount = 0;

#ifdef DEVELOPER_JAY_FRIEZE
                        String8 txt;
                        txt.setTextFormat("loop infinity");
                        GFX_ADAPTER->drawDBGText(txt);
#endif
                    }
                    else
                    {
#ifdef DEVELOPER_JAY_FRIEZE
                        ITF_ASSERT(idLastEdge!=idCurEdge);
                        ITF_ASSERT(edgeCur.m_build);
                        ITF_ASSERT(edgeLast.m_build);
#endif
                        if ( !buildEdgeCorner( pConfig, edgeCur, edgeLast, cornerLongMax, dotSight ))
                        {
                            edgeRecompute = btrue;
                            skipEdgeLast = btrue;

#ifdef DEVELOPER_JAY_FRIEZE
                            String8 txt;
                            txt.setTextFormat("not build edge corner");
                            GFX_ADAPTER->drawDBGText(txt);
#endif
                        }
                        else
                        {
                            if ( insertNewEdgeInLoop && insertNewEdge( _edgeList, pConfig, idCurEdge, idLastEdge ) )
                            {
                                edgeRecompute = btrue;
                                insertNewEdgeInLoop = bfalse;

#ifdef DEVELOPER_JAY_FRIEZE
                                String8 txt;
                                txt.setTextFormat("insert edge");
                                GFX_ADAPTER->drawDBGText(txt);
#endif
                            }
                            else
                            {
                                if ( skipWrongEdge( edgeCur, edgeLast, dotSight ) )
                                {
                                    edgeRecompute = btrue;
                                    skipEdgeLast = btrue;

#ifdef DEVELOPER_JAY_FRIEZE
                                    String8 txt;
                                    txt.setTextFormat("skip wrong edge");
                                    GFX_ADAPTER->drawDBGText(txt);
#endif
                                }
                                else
                                {
                                    setNormEdge( edgeLast );

                                    f32 normMin = getNormMinToSnap( _edgeList, pConfig, idCurEdge, idLastEdge, edgeNornMin, edgeRunNormMin);  

                                    if ( skipSmallEdge( _edgeList, edgeLast, edgeCur, normMin, (i32)idLastEdge -1 ) || edgeLast.m_snap )
                                    {        
                                        if ( edgeCur.m_cornerAngle == 0.f)
                                        {
                                            edgeRecompute = btrue;
                                            skipEdgeLast = btrue;
                                        }
                                        else if ( !isScaleEqual( edgeCur, edgeLast))
                                        {
                                            edgeLast.m_scaleCur = edgeCur.m_scaleCur;
                                            edgeLast.m_snap = btrue;
                                            edgeRecompute = btrue;

#ifdef DEVELOPER_JAY_FRIEZE
                                            String8 txt;
                                            txt.setTextFormat("set scale equal");
                                            GFX_ADAPTER->drawDBGText(txt);
#endif
                                        }
                                        else
                                        {
                                            edgeLast.m_stopCoeff = edgeLast.m_startCoeff;
                                            edgeLast.m_snap = btrue; 
                                            edgeLast.m_norm = 0.f;   

#ifdef DEVELOPER_JAY_FRIEZE
                                            String8 txt;
                                            txt.setTextFormat("snap edge last");
                                            GFX_ADAPTER->drawDBGText(txt);
#endif
                                        }     
                                    }
                                }
                            }
                        }
                    }

                    if ( edgeRecompute )
                    {
                        if ( skipEdgeLast )
                        {
                            u32 idLast2 = idLastEdgeValidList[idLastEdge];
                            if ( idLast2 != 0 )
                            {
                                skipEdgeFrieze( _edgeList, idLastEdge, idLast2, idLastEdgeValidList[idLast2] );
                            }
                            else
                            {
                                _edgeList.clear();
                                m_pRecomputeData->m_edgeListCount = 0;
                                edgeRecompute = bfalse;

#ifdef DEVELOPER_JAY_FRIEZE
                                String8 txt;
                                txt.setTextFormat("impossible to skip edge last");
                                GFX_ADAPTER->drawDBGText(txt);
#endif
                            }

                            idCurEdge = idLastEdgeValidList[idLastEdge];                            
                        }
                        else
                            idCurEdge = idLastEdge;

                        idLastEdge = idLastEdgeValidList[idCurEdge];     

#ifdef DEVELOPER_JAY_FRIEZE
                        String8 txt;
                        txt.setTextFormat("id cur: %i, last: %i", idCurEdge, idLastEdge);
                        GFX_ADAPTER->drawDBGText(txt);
#endif
                    }
                    else
                    {
                        setNormEdge( edgeCur );

                        if ( !isEdgeWithHoleVisual(edgeLast) && _edgeList.size() > 1 )
                        {
                            f32 normMin = edgeNornMin;
                            if ( isEdgeRun(_edgeList, pConfig, 0, idLastEdge) )                
                                normMin = edgeRunNormMin;   

                            u32 idEdgeNext = 1;
                            findNextBuildEdge(_edgeList, idEdgeNext);
                            edgeFrieze& edgeNext = _edgeList[idEdgeNext];

                            bbool allEdgeToSnap = getCountSnapEgdes( _edgeList ) >= getCountBuildEgdes(_edgeList) -1;
                            if ( skipSmallEdge( _edgeList, edgeCur, edgeNext, normMin, (i32)idLastEdge -1) || allEdgeToSnap ) 
                            {
                                // TO DO pb si angle edgeCur = 0
                                edgeCur.m_snap = btrue;
                                edgeCur.m_norm = 0.f;
                                edgeCur.m_stopCoeff = edgeCur.m_startCoeff;

#ifdef DEVELOPER_JAY_FRIEZE
                                String8 txt;
                                txt.setTextFormat("snap edge start");
                                GFX_ADAPTER->drawDBGText(txt);
#endif
                            }
                        }
                    }
                }

            } while (edgeRecompute);

            delete[] idLastEdgeValidList;


            if ( !isLooping())
            {
                // set first edge
                u32 idFirstEdge = 0;
                if ( findNextBuildEdge( _edgeList, idFirstEdge ) )
                {
                    edgeFrieze& edgeFirst = _edgeList[idFirstEdge];
                    edgeFirst.m_cornerNormal = edgeFirst.m_normal;
                    edgeFirst.m_interDown = edgeFirst.m_points[0];
                    edgeFirst.m_interUp = edgeFirst.m_points[1];            
                    edgeFirst.m_cornerAngle = 0.f;
                }

                //  set last edge
                i32 idStopEdge = m_pRecomputeData->m_edgeListCount -1;
                if ( findPreviousBuildEdge( _edgeList, idStopEdge))
                {
                    edgeFrieze& edgeEnd = _edgeList[idStopEdge];

                    if ( (u32)idStopEdge == idFirstEdge )
                    {
                        buildEdgePoints( pConfig, edgeEnd);
                        edgeEnd.m_interDown = edgeEnd.m_points[0];
                        edgeEnd.m_interUp = edgeEnd.m_points[1];   
                    }

                    setNormEdge(edgeEnd);

                    // snap
                    f32 normMin = edgeNornMin;
                    if ( isEdgeRun(_edgeList, pConfig, 0, 0) )                
                        normMin = edgeRunNormMin;  

                    if ( _snapEdgeLast && edgeEnd.m_norm <= normMin )
                    {
                        if ( edgeEnd.m_cornerAngle == 0.f )
                        {
                            edgeEnd.m_build = bfalse;
                        }
                        else
                        {
                            edgeEnd.m_snap = btrue;
                            edgeEnd.m_norm = 0.f;
                            edgeEnd.m_stopCoeff = edgeEnd.m_startCoeff;
                        }
                    }
                }
            }

#ifdef DEVELOPER_JAY_FRIEZE
            String8 txt;
            txt.setTextFormat("Build Edge List Iteration = %i", countIteration);
            GFX_ADAPTER->drawDBGText(txt);
            txt.setTextFormat("Build Edge List Recompute = %i", countRecompute);
            GFX_ADAPTER->drawDBGText(txt);
#endif
        }

        finalizeEdgeFriezeList( pConfig, _edgeList);
    }

} // namespace ITF
