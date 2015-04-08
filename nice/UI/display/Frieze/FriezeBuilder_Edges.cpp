#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    static const f32 holeOffset = 0.001f;

    f32 Frise::getEdgeNormMin( const FriseConfig* config ) const
    {
        return m_pRecomputeData->m_widthScale * m_pRecomputeData->m_texRatio *config->m_snapCoeff;
    }

    f32 Frise::getEdgeRunNormMin( const FriseConfig* config ) const 
    {
        return m_pRecomputeData->m_widthScale * m_pRecomputeData->m_texRatio *config->m_snapCoeffUv;
    }

    void Frise::copyEdgeFromPolyline( ITF_VECTOR<edgeFrieze>& _edgeList )
    {
        u32 edgeCount = m_pointsList.getPosCount();
        if ( edgeCount < 2 )
            return;

        _edgeList.reserve( edgeCount );

        edgeFrieze edgeCur;

        const PolyLineEdge& edgeStart = m_pointsList.getEdgeAt(0);
        edgeCur.m_pos = edgeStart.getPos();
        edgeCur.m_holeMode = edgeStart.m_holeMode; 
        edgeCur.m_scaleInit = edgeStart.getScale();
        edgeCur.m_scaleCur = edgeCur.m_scaleInit;
        edgeCur.m_switchTexture = edgeStart.m_switchTexture;        
        edgeCur.m_heightStart = edgeCur.m_scaleCur *m_pRecomputeData->m_heightScale;
        edgeCur.m_idPoint = 0;

        for (u32 i =1; i<edgeCount; i++)
        {           
            const PolyLineEdge& edge = m_pointsList.getEdgeAt(i);

            if ( !updateEdgeFriezeVector(edgeCur, edge.getPos() -edgeCur.m_pos) )
                continue;

            edgeCur.m_heightStop = edge.getScale() *m_pRecomputeData->m_heightScale;
            _edgeList.push_back(edgeCur);

            edgeCur.m_pos = edge.getPos();
            edgeCur.m_holeMode = edge.m_holeMode;            
            edgeCur.m_scaleInit = edge.getScale();
            edgeCur.m_scaleCur = edgeCur.m_scaleInit;
            edgeCur.m_heightStart = edgeCur.m_scaleCur *m_pRecomputeData->m_heightScale;
            edgeCur.m_switchTexture = edge.m_switchTexture;   
            edgeCur.m_idPoint = i;
        }

        if ( _edgeList.size()== 0) // all points have same position
        {
            const PolyLineEdge& edge = m_pointsList.getEdgeAt(edgeCount-1);
            edgeCur.m_pos = edge.getPos();
            edgeCur.m_scaleInit = edge.getScale();
            edgeCur.m_scaleCur = edgeCur.m_scaleInit;
            edgeCur.m_heightStart = edgeCur.m_scaleCur *m_pRecomputeData->m_heightScale;
            edgeCur.m_heightStop = edgeCur.m_heightStart;
            edgeCur.m_idPoint = edgeCount-1;

            edgeCur.m_pos.x() -= 0.01f;

            updateEdgeFriezeVector(edgeCur, Vec2d(0.02f,0.f));
            _edgeList.push_back(edgeCur);
        }

        m_pRecomputeData->m_edgeListCount = _edgeList.size();
        m_pRecomputeData->m_edgeListCountMax = m_pRecomputeData->m_edgeListCount *3;   
    }

    bbool Frise::skipEdgeFrieze( ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idToSkip, u32 _idToUpdate, u32 _idLast ) const
    {
        edgeFrieze& _edgeToSkip = _edgeList[_idToSkip];
        edgeFrieze& _edgeToUpdate = _edgeList[_idToUpdate];

        _edgeToSkip.m_build = bfalse;

        _edgeToUpdate.m_snap = bfalse;
        _edgeToUpdate.m_scaleCur = _edgeToUpdate.m_scaleInit;
        _edgeToUpdate.m_heightStop = _edgeToSkip.m_heightStop;
        _edgeToUpdate.m_startCoeff = 0.f;
        _edgeToUpdate.m_stopCoeff = 1.f;

        if ( _idLast != _idToUpdate )
        {
            edgeFrieze& _edgeLast = _edgeList[_idLast];
#ifdef DEVELOPER_JAY_FRIEZE
            ITF_ASSERT(_edgeLast.m_build);
#endif

            if ( _edgeLast.m_snap )
            {
                _edgeLast.m_snap = bfalse;
                _edgeToUpdate.m_pos = _edgeLast.m_pos + _edgeLast.m_sight;
            }
        }

        return updateEdgeFriezeVector( _edgeToUpdate, _edgeToSkip.m_pos + _edgeToSkip.m_sight - _edgeToUpdate.m_pos );
    }

    bbool Frise::updateEdgeFriezePosition( edgeFrieze& _edge, Vec2d& _pos ) const 
    {
        Vec2d sightDest = _edge.m_pos + _edge.m_sight;

        _edge.m_pos = _pos;

        return updateEdgeFriezeVector( _edge, sightDest -_pos);
    }

    bbool Frise::updateEdgeFriezeVector( edgeFrieze& _edgeToUpdate, Vec2d _vector) const
    {
        _edgeToUpdate.m_sight = _vector;
        _edgeToUpdate.m_sightNormalized = _edgeToUpdate.m_sight;
        _edgeToUpdate.m_sightNormalized.normalize();
        _edgeToUpdate.m_normal = _edgeToUpdate.m_sightNormalized.getPerpendicular();

        return ( _edgeToUpdate.m_sightNormalized != Vec2d::Zero );
    }

    bbool Frise::isSnapAllEdgeInBubble( ITF_VECTOR<edgeFrieze>& _edgeList) const
    {
        if ( !isLooping())
            return bfalse;

        if ( m_pRecomputeData->m_edgeListCount <=1 )
            return btrue;

        return bfalse;
    }

    void Frise::finalizeEdgeFriezeList( const FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList )
    {
        u32 newEdgeCount = 0;
        u32 edgeToOrder = 0;

        for ( u32 edgeCur = 0; edgeToOrder <m_pRecomputeData->m_edgeListCount; edgeCur++, edgeToOrder++ )
        {            
            //  search next edge valid
            for ( ; edgeToOrder < m_pRecomputeData->m_edgeListCount; edgeToOrder++ )
            {
                edgeFrieze& edge = _edgeList[edgeToOrder];

                if ( edge.m_build )
                {
#ifdef DEVELOPER_JAY_FRIEZE
                    ITF_ASSERT(edge.m_sightNormalized.isNormalized());
                    ITF_ASSERT( edge.m_interUp != edge.m_interDown );
#endif

                    const Vec2d upSight = edge.m_points[3] -edge.m_points[1];
                    const Vec2d downSight = edge.m_points[2] -edge.m_points[0];

                    edge.m_points[2] = edge.m_points[0] +downSight *edge.m_stopCoeff;                    
                    edge.m_points[3] = edge.m_points[1] +upSight *edge.m_stopCoeff;

                    edge.m_points[0] = edge.m_points[0] +downSight *edge.m_startCoeff;
                    edge.m_points[1] = edge.m_points[1] +upSight *edge.m_startCoeff;

                    if ( edgeCur != edgeToOrder )
                        _edgeList[edgeCur] = _edgeList[edgeToOrder];

                    newEdgeCount ++;

                    break;
                }
            }
        }

        m_pRecomputeData->m_edgeListCount = newEdgeCount;    

        // if m_pRecomputeData->m_edgeListCount =1 et snap=1 et noLoop et methode!=extremity --> bug prévisible, réajuster l'edge pour qu'il affiche qqch
        if ( isSnapAllEdgeInBubble(_edgeList) )
        {
            snapAllEdge(_edgeList);            
        }
        else
        {
            snapEdgeListPointsWithInter( _config, _edgeList);
        }        
    }

    void Frise::buildEdgePoints( const FriseConfig* _config, edgeFrieze& _edgeCur) const
    {
        Vec2d edgeHeightStart = _edgeCur.m_normal *_edgeCur.m_heightStart;
        Vec2d edgeHeightStop = _edgeCur.m_normal *_edgeCur.m_heightStop;

        _edgeCur.m_points[0] = _edgeCur.m_pos -edgeHeightStart *_config->m_visualOffset;
        _edgeCur.m_points[1] = _edgeCur.m_points[0] +edgeHeightStart;
        _edgeCur.m_points[2] = _edgeCur.m_pos + _edgeCur.m_sight -edgeHeightStop *_config->m_visualOffset;                                      
        _edgeCur.m_points[3] = _edgeCur.m_points[2] +edgeHeightStop;
    }

    bbool Frise::skipBadIntersection( const edgeFrieze& _edge, const Vec2d& _interLast, const Vec2d& _interCur) const
    {
        if ( _edge.m_sight.dot(_interLast -_edge.m_pos) < -MTH_EPSILON || _edge.m_sight.dot(_interCur -_edge.m_pos) < -MTH_EPSILON )
            return btrue;

        return bfalse;
    }

    bbool Frise::setIntersectionDown( const edgeFrieze& _edgeLast, edgeFrieze& _edgeCur) const
    {
        Vec2d cornerNormal = _edgeCur.m_pos +_edgeCur.m_cornerNormal;
        Vec2d edgeLastInter, edgeCurInter;

        //  intersection down       
        intersectionLineLine( _edgeCur.m_pos, cornerNormal, _edgeLast.m_points[0], _edgeLast.m_points[2], edgeLastInter );        
        intersectionLineLine( _edgeCur.m_pos, cornerNormal, _edgeCur.m_points[0], _edgeCur.m_points[2], edgeCurInter );              

        // skip bad intersection
        if ( _edgeCur.m_cornerAngle > 0.f && skipBadIntersection( _edgeCur, edgeLastInter, edgeCurInter) )         
            return bfalse;

        _edgeCur.m_interDown = ( edgeLastInter +edgeCurInter) *0.5f;

        return btrue;
    }

    bbool Frise::setIntersectionUp( const edgeFrieze& _edgeLast, edgeFrieze& _edgeCur) const
    {
        Vec2d cornerNormal = _edgeCur.m_pos +_edgeCur.m_cornerNormal;
        Vec2d edgeLastInter, edgeCurInter;

        intersectionLineLine( _edgeCur.m_pos, cornerNormal, _edgeLast.m_points[1], _edgeLast.m_points[3], edgeLastInter );
        intersectionLineLine( _edgeCur.m_pos, cornerNormal, _edgeCur.m_points[1], _edgeCur.m_points[3], edgeCurInter );      

        // skip bad intersection
        if ( _edgeCur.m_cornerAngle <0.f && skipBadIntersection( _edgeCur, edgeLastInter, edgeCurInter))
            return bfalse;

        _edgeCur.m_interUp = ( edgeLastInter +edgeCurInter) *0.5f;

        return btrue;
    }

    bbool Frise::buildEdgeCorner( const FriseConfig* _config, edgeFrieze& _edgeCur, edgeFrieze& _edgeLast, f32 _cornerLongMax, f32& _dotSight ) const
    {
        if ( !_edgeCur.m_sightNormalized.isNormalized() || !_edgeLast.m_sightNormalized.isNormalized())
            return bfalse;

        // apply scale
        _cornerLongMax *= _edgeCur.m_scaleCur;
        _edgeCur.m_heightStart = _edgeCur.m_scaleCur *m_pRecomputeData->m_heightScale;
        _edgeLast.m_heightStop = _edgeCur.m_heightStart;

        buildEdgePoints( _config, _edgeCur);
        buildEdgePoints( _config, _edgeLast);

        //  edge parallel?
        _dotSight = _edgeLast.m_sightNormalized.dot(_edgeCur.m_sightNormalized);        

        if ( _dotSight + MTH_EPSILON >= 1.f ) // angle = 0
        {
            _dotSight = 1.f;
            _edgeCur.m_interDown    = _edgeCur.m_points[0];
            _edgeCur.m_interUp    = _edgeCur.m_points[1];     

            _edgeCur.m_cornerAngle  = 0.f;
            _edgeCur.m_cornerNormal = _edgeCur.m_normal;    
            return btrue;
        }

        if ( _dotSight - MTH_EPSILON <= -1.f ) // angle = MTH_PI
        {    
            _dotSight = -1.f;

            if ( _config->m_visualOffset == 1.f )
            {
                _edgeCur.m_interUp = _edgeCur.m_pos; 
                _edgeCur.m_interDown = _edgeCur.m_pos +_edgeLast.m_sightNormalized *_cornerLongMax;

                _edgeCur.m_cornerNormal = -_edgeLast.m_sightNormalized;
                _edgeCur.m_cornerAngle = -MTH_PI;
                return btrue;
            }
            else if ( _config->m_visualOffset == 0.f )
            {
                _edgeCur.m_interDown = _edgeCur.m_pos; 
                _edgeCur.m_interUp = _edgeCur.m_interDown + _edgeLast.m_sightNormalized *_cornerLongMax;
                _edgeCur.m_cornerNormal = _edgeLast.m_sightNormalized;
                _edgeCur.m_cornerAngle = MTH_PI;
                return btrue;
            }

            return bfalse;
        }

        //  normal coin
        _edgeCur.m_cornerNormal = ( _edgeCur.m_normal +_edgeLast.m_normal) *0.5f;
        _edgeCur.m_cornerNormal.normalize();

        //  angle coin
        _edgeCur.m_cornerAngle   = _edgeCur.m_normal.getShortestAngle(_edgeLast.m_normal);
        _edgeCur.m_cornerAngle   *= -getSign(_edgeCur.m_sight.dot(_edgeCur.m_cornerNormal));

        //  set intersections 
        if ( _config->m_visualOffset == 1.f && _edgeCur.m_cornerAngle < 0.f )
        {
            if ( !setIntersectionDown( _edgeLast, _edgeCur) )
                return bfalse;

            _edgeCur.m_interUp = _edgeCur.m_pos;            
        }
        else if ( _config->m_visualOffset == 0.f && _edgeCur.m_cornerAngle > 0.f )
        {
            _edgeCur.m_interDown = _edgeCur.m_pos;

            if ( !setIntersectionUp( _edgeLast, _edgeCur))
                return bfalse;
        }
        else
        {
            if ( !setIntersectionDown( _edgeLast, _edgeCur))
                return bfalse;

            if ( !setIntersectionUp( _edgeLast, _edgeCur))
                return bfalse;
        }

        //  clamp intersection up and down
        if ( (_edgeCur.m_interUp -_edgeCur.m_interDown).sqrnorm() > _cornerLongMax *_cornerLongMax )
        {
            if ( _edgeCur.m_cornerAngle >= 0.f )
                _edgeCur.m_interUp = _edgeCur.m_interDown +_edgeCur.m_cornerNormal *_cornerLongMax;
            else
                _edgeCur.m_interDown = _edgeCur.m_interUp -_edgeCur.m_cornerNormal *_cornerLongMax;
        }   

        return btrue;
    }

    bbool Frise::skipWrongEdge( edgeFrieze& _edgeCur, edgeFrieze& _edgeLast, f32& _dotSight ) const
    {
        if ( _dotSight == 1.f || _dotSight == -1.f )
        {
            _edgeLast.m_stopCoeff = 1.f;
            _edgeCur.m_startCoeff = 0.f;

            return bfalse;
        }

        Vec2d edgeInter = _edgeCur.m_interDown;
        if ( _edgeCur.m_cornerAngle <0.f )
            edgeInter = _edgeCur.m_interUp;

        // edge last stop coeff
        Vec2d p4 = edgeInter -_edgeLast.m_normal;
        bbool intersection = intersectionLineLineCoeff( _edgeLast.m_pos, _edgeCur.m_pos, edgeInter, p4, _edgeLast.m_stopCoeff );

        if (    !intersection
            ||  (_edgeLast.m_stopCoeff + MTH_BIG_EPSILON) < 0.f
            ||  (_edgeLast.m_stopCoeff + m_pRecomputeData->m_flexibility) < _edgeLast.m_startCoeff
            ||  (_edgeLast.m_stopCoeff - MTH_BIG_EPSILON) > 1.f )
            return btrue;

        // edge cur start coeff
        Vec2d p2 = _edgeCur.m_pos +_edgeCur.m_sight;
        p4 = edgeInter -_edgeCur.m_normal;
        intersection = intersectionLineLineCoeff( _edgeCur.m_pos, p2, edgeInter, p4, _edgeCur.m_startCoeff );

        if (    !intersection
            ||  (_edgeCur.m_startCoeff - MTH_BIG_EPSILON) > 1.f
            ||  (_edgeCur.m_startCoeff + MTH_BIG_EPSILON) < 0.f )
            return btrue;

        // clamp edgeLast coeff
        _edgeLast.m_stopCoeff = Max( _edgeLast.m_stopCoeff, _edgeLast.m_startCoeff);

#ifdef DEVELOPER_JAY_FRIEZE
        ITF_ASSERT( _edgeCur.m_startCoeff >= 0.f && _edgeCur.m_startCoeff <= 1.f );
        ITF_ASSERT( _edgeCur.m_stopCoeff >= 0.f && _edgeCur.m_stopCoeff <= 1.f );
#endif

        return bfalse;
    }

    void Frise::updateIdEdgeList( u32& _idCurEdge, u32& _idLastEdge, u32* _idLastEdgeValidList ) const
    {             
        if ( _idLastEdge == 0 )
            _idCurEdge = 1;
        else
            _idCurEdge = _idLastEdge; 

        _idLastEdge = _idLastEdgeValidList[_idCurEdge];
    }

    bbool Frise::insertNewEdge( ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* config, u32 idCurEdge, u32 idLastEdge )
    {        
        if ( _edgeList[idCurEdge].m_cornerAngle == 0.f || isEdgeWithHoleVisual(_edgeList[idCurEdge]) || isEdgeWithHoleVisual(_edgeList[idLastEdge]))
            return bfalse;

        if ( m_pRecomputeData->m_edgeListCount >= m_pRecomputeData->m_edgeListCountMax )
        {
#ifdef DEVELOPER_JAY_FRIEZE
            ITF_ASSERT_MSG( 0, "InsertEdge() will be called in infinity"); 
#endif            
            return bfalse;
        }

        switch ( config->m_methode )
        {
        case InArchitecture:
        case InArchitectureSimple:
            return insertNewEdge_InArchi( _edgeList, config, idCurEdge, idLastEdge );

        case InPipe:
            return insertNewEdge_InPipe( _edgeList, config, idCurEdge, idLastEdge );

        default:
            return bfalse;
        }        
    }

    bbool Frise::getIdLastLastEdgeBuild( ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idLastEdge, u32& _idLastLastEdge ) const
    {
        if ( m_pointsList.isLooping())
        {
            _idLastLastEdge = findLastBuildEdgeInLoop( _edgeList, _idLastEdge);
            return _idLastEdge!=_idLastLastEdge;      
        }
        else
        {
            i32 idEdge = (i32)_idLastEdge -1;
            if ( findPreviousBuildEdge( _edgeList, idEdge) )
            {
                _idLastLastEdge = idEdge;
                return _idLastEdge!=_idLastLastEdge;
            }  
        } 

        return bfalse;
    }

    bbool Frise::isEdgeRun( ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* _config, u32 _idCurEdge, u32 _idLastEdge ) const
    {       
        switch ( _config->m_methode )
        {
        case InRoundness:
            {
                u32 idLastLastEdge = U32_INVALID;
                return getIdLastLastEdgeBuild(_edgeList, _idLastEdge, idLastLastEdge) && isEdgeRun_InRoundness( _edgeList, _config, _idCurEdge, _idLastEdge, idLastLastEdge );
            }                

        case InArchitecture:
        case InArchitectureSimple:
            {
                u32 idLastLastEdge = U32_INVALID;
                return getIdLastLastEdgeBuild(_edgeList, _idLastEdge, idLastLastEdge) && isEdgeRun_InArchi( _edgeList, _idCurEdge, _idLastEdge, idLastLastEdge );
            }

        case InPipe:
            {
                u32 idLastLastEdge = U32_INVALID;
                return getIdLastLastEdgeBuild(_edgeList, _idLastEdge, idLastLastEdge) && isEdgeRun_InPipe( _edgeList, _idCurEdge, _idLastEdge, idLastLastEdge );
            }

        case InExtremity:
        case InExtremitySimple:
        case InExtremityLong:
            return getCountBuildEgdes(_edgeList)==1;

        default:
            return bfalse;
        }        
    }

    bbool Frise::setScaleAverage( edgeFrieze& _edgeCur, edgeFrieze& _edgeLast ) const
    {
        if ( isScaleEqual( _edgeCur, _edgeLast))        
            return bfalse;

        f32 average = ( _edgeCur.m_scaleCur + _edgeLast.m_scaleCur ) *0.5f;
        _edgeCur.m_scaleCur = average;
        _edgeLast.m_scaleCur = average;

        return btrue;
    }

    void Frise::updateEdgeHeight( edgeFrieze& _edgeCur, edgeFrieze& _edgeLast) const
    {
        f32 epsilonHeight = 0.01f;

        Vec2d borderRight = getEdgeBorderRight(_edgeLast);
        Vec2d borderLeft = getEdgeBorderLeft(_edgeCur);
        f32 borderRightSqrNorm = borderRight.sqrnorm();
        f32 borderLeftSqrNorm = borderLeft.sqrnorm();

        if ( borderLeftSqrNorm < borderRightSqrNorm -epsilonHeight || borderLeftSqrNorm > borderRightSqrNorm +epsilonHeight || _edgeCur.m_snap)
        {
            _edgeLast.m_heightStop = sqrt(borderRightSqrNorm);
            _edgeCur.m_heightStart = sqrt(borderLeftSqrNorm);

            if ( _edgeCur.m_heightStart == _edgeLast.m_heightStop )
                _edgeCur.m_heightStart -= MTH_EPSILON;
        }
    }

    void Frise::snapEdgeListPointsWithInter( const FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList ) const
    {
        if ( m_pRecomputeData->m_edgeListCount > 1)
        {
            edgeFrieze* edgeLast2 = NULL;

            bbool allEdgeSnaped = bfalse;
            u32 indexStart = 1;

            if ( isLooping())
            {
                indexStart = 0;
                allEdgeSnaped = getCountSnapEgdes(_edgeList) == m_pRecomputeData->m_edgeListCount;

                if ( m_pRecomputeData->m_edgeListCount >2)
                {
                    u32 indexLast2 = ( m_pRecomputeData->m_edgeListCount -2) %m_pRecomputeData->m_edgeListCount;
                    edgeLast2 = &_edgeList[indexLast2];
                }
            }
            
            for ( u32 indexCur =indexStart; indexCur<m_pRecomputeData->m_edgeListCount; indexCur++)
            {
                u32 indexLast = ( indexCur -1 +m_pRecomputeData->m_edgeListCount) %m_pRecomputeData->m_edgeListCount;

                edgeFrieze& _edgeLast = _edgeList[indexLast];
                edgeFrieze& _edgeCur = _edgeList[indexCur];

                snapEdgesPointsWithInter( _edgeCur, _edgeLast, edgeLast2, allEdgeSnaped);

                edgeLast2 = &_edgeLast;
            }

            if ( allEdgeSnaped )
            {
                edgeFrieze& edgeLast = _edgeList[m_pRecomputeData->m_edgeListCount -1];
                if( edgeLast.m_cornerAngle >0.f)
                {
                    Vec2d pos = edgeLast.m_interDown;
                    for (u32 i=0; i<m_pRecomputeData->m_edgeListCount; i++)
                        _edgeList[i].m_points[0] = _edgeList[i].m_points[2] = _edgeList[i].m_interDown = pos;
                }
                else
                {
                    Vec2d pos = edgeLast.m_interUp;
                    for (u32 i=0; i<m_pRecomputeData->m_edgeListCount; i++)
                        _edgeList[i].m_points[1] = _edgeList[i].m_points[3] = _edgeList[i].m_interUp = pos;
                }
            }
            else if ( isLooping() )
            {
                if ( m_pRecomputeData->m_edgeListCount>3 )
                {
                    edgeFrieze& edgeLast = _edgeList[m_pRecomputeData->m_edgeListCount -1];
                    if ( edgeLast.m_snap )
                    {
                        edgeLast.m_points[2] = edgeLast.m_points[0];
                        edgeLast.m_points[3] = edgeLast.m_points[1];

                        edgeFrieze& edgeStart = _edgeList[0];

                        if ( edgeStart.m_cornerAngle > 0.f)
                        {
                            if ( !edgeStart.m_snap) 
                            edgeStart.m_points[0] = edgeStart.m_interDown = edgeLast.m_points[2];

                            if ( edgeStart.m_snap) 
                                edgeStart.m_points[2] = edgeStart.m_points[0];
                        }
                        else
                        {
                            edgeStart.m_points[1] = edgeStart.m_interUp = edgeLast.m_points[3];

                            if ( edgeStart.m_snap) 
                                edgeStart.m_points[3] = edgeStart.m_points[1];
                        }                       
                    }
                }
            }
            else
            {
                edgeFrieze& edgeLast = _edgeList[m_pRecomputeData->m_edgeListCount -1];
                if ( edgeLast.m_snap)
                {
                    edgeLast.m_points[2] = edgeLast.m_points[0];
                    edgeLast.m_points[3] = edgeLast.m_points[1];
                }                
            }
        }
    }

    void Frise::snapEdgesPointsWithInter( edgeFrieze& _edgeCur, edgeFrieze& _edgeLast, edgeFrieze* edgeLast2, bbool _allEdgeSnaped) const
    {
        if ( _edgeLast.m_snap )
        {
            _edgeLast.m_points[2] = _edgeLast.m_points[0];
            _edgeLast.m_points[3] = _edgeLast.m_points[1];
        }

        if (  _edgeCur.m_cornerAngle ==MTH_PI || _edgeCur.m_cornerAngle ==-MTH_PI )
            return; 

        if ( _edgeLast.m_snap && getSign(_edgeLast.m_cornerAngle) == getSign(_edgeCur.m_cornerAngle) )
        {
            if ( _edgeCur.m_cornerAngle > 0.f)
            {                
                if ( edgeLast2 && ( !edgeLast2->m_snap || edgeLast2->m_cornerAngle < 0.f || _allEdgeSnaped) )
                {
                    Vec2d inter = (_edgeCur.m_interDown +_edgeLast.m_interDown) *0.5f;
                    Vec2d offset = inter -_edgeLast.m_interDown;

                    _edgeLast.m_points[2] = _edgeCur.m_points[0] = _edgeCur.m_interDown = _edgeLast.m_interDown = inter;
                    _edgeLast.m_points[3] += offset;
                    edgeLast2->m_points[2] = _edgeLast.m_points[2];

                    if ( edgeLast2->m_snap )
                        edgeLast2->m_points[0] = edgeLast2->m_points[2];
                }
                else
                {
                    _edgeLast.m_points[2] = _edgeCur.m_points[0] = _edgeCur.m_interDown = _edgeLast.m_interDown;
                }
            }
            else
            {
                if ( edgeLast2 && ( !edgeLast2->m_snap || edgeLast2->m_cornerAngle > 0.f || _allEdgeSnaped) )
                {
                    Vec2d inter = (_edgeCur.m_interUp +_edgeLast.m_interUp) *0.5f;
                    Vec2d offset = inter -_edgeLast.m_interUp;

                    _edgeLast.m_points[3] = _edgeCur.m_points[1] = _edgeCur.m_interUp = _edgeLast.m_interUp = inter;
                    _edgeLast.m_points[2] += offset;
                    edgeLast2->m_points[3] = _edgeLast.m_points[3];

                    if ( edgeLast2->m_snap)
                        edgeLast2->m_points[1] = edgeLast2->m_points[3];
                }
                else
                {
                    _edgeLast.m_points[3] = _edgeCur.m_points[1] = _edgeCur.m_interUp = _edgeLast.m_interUp;
                }                
            }
        }
        else
        {
            if ( _edgeCur.m_cornerAngle > 0.f)
            {
                _edgeLast.m_points[2] = _edgeCur.m_points[0] = _edgeCur.m_interDown;
            }
            else
            {
                _edgeLast.m_points[3] = _edgeCur.m_points[1] = _edgeCur.m_interUp;
            }
        }

        if ( _edgeLast.m_snap )
        {
            _edgeLast.m_points[0] = _edgeLast.m_points[2];
            _edgeLast.m_points[1] = _edgeLast.m_points[3];
        }

        updateEdgeHeight( _edgeCur, _edgeLast);

#ifdef DEVELOPER_JAY_FRIEZE
        if ( _edgeLast.m_snap )
        {
            ITF_ASSERT(_edgeLast.m_points[0] == _edgeLast.m_points[2]);
            ITF_ASSERT(_edgeLast.m_points[1] == _edgeLast.m_points[3]);

            if ( _edgeLast.m_cornerAngle <0.f && _edgeCur.m_cornerAngle >0.f )
            {
                ITF_ASSERT( _edgeLast.m_points[1] == _edgeLast.m_interUp);
                ITF_ASSERT( _edgeLast.m_points[0] == _edgeCur.m_interDown);
            }
            else if ( _edgeLast.m_cornerAngle >0.f && _edgeCur.m_cornerAngle <0.f )
            {
                ITF_ASSERT( _edgeLast.m_points[1] == _edgeCur.m_interUp);
                ITF_ASSERT( _edgeLast.m_points[0] == _edgeLast.m_interDown);
            }
            else if ( _edgeLast.m_cornerAngle >0.f )
            {
                ITF_ASSERT( _edgeLast.m_points[0] == _edgeLast.m_interDown);
                ITF_ASSERT( _edgeLast.m_points[0] == _edgeCur.m_interDown);
            }
            else if ( _edgeLast.m_cornerAngle <0.f )
            {
                ITF_ASSERT( _edgeLast.m_points[1] == _edgeLast.m_interUp);
                ITF_ASSERT( _edgeLast.m_points[1] == _edgeCur.m_interUp);
            }    
            else
            {

            }
        }
#endif
    }

    f32 Frise::getNormMinToSnap( ITF_VECTOR<edgeFrieze>& _edgeList, const FriseConfig* _config, u32 _idCurEdge, u32 _idLastEdge, f32 _normMinPoint, f32 _normMinUv) const
    {
        f32 normMin = _normMinPoint;

        if ( isEdgeRun( _edgeList, _config, _idCurEdge, _idLastEdge ) )
            normMin = _normMinUv;

        normMin *= ( _edgeList[_idCurEdge].m_scaleCur +_edgeList[_idLastEdge].m_scaleCur) *0.5f;

        return normMin;
    }

    void Frise::setNormEdge( edgeFrieze& _edge ) const
    {        
        _edge.m_norm = Max ( 0.f, _edge.m_sight.norm() *( _edge.m_stopCoeff -_edge.m_startCoeff) );
    }

    bbool Frise::skipSmallEdge( ITF_VECTOR<edgeFrieze>& _edgeList, edgeFrieze& _edgeLast, edgeFrieze& _edgeCur, f32 _normMin, i32 _idEdgeLastLast ) const
    {
        if ( isEdgeWithHoleVisual(_edgeLast) || isEdgeWithHoleVisual(_edgeCur) )
            return bfalse;

        if ( _idEdgeLastLast > -1 && _idEdgeLastLast < (i32)m_pRecomputeData->m_edgeListCount )
        {
            if ( isEdgeWithHoleVisual(_edgeList[_idEdgeLastLast]) )
                return bfalse;
        }

        if ( _edgeLast.m_norm <= _normMin /*&& _edgeCur.m_cornerAngle != 0.f*/ )
            return btrue;

        return bfalse;
    }

    void Frise::snapEdge( ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdgeToSnap, u32 _idEdgeToUpdate ) const
    {
        edgeFrieze& edgeToSnap = _edgeList[_idEdgeToSnap]; 
        edgeFrieze& edgeToUpdate = _edgeList[_idEdgeToUpdate];

        // edge to snap
        f32 normSnap = edgeToSnap.m_norm;
        edgeToSnap.m_snap = btrue;
        edgeToSnap.m_norm = 0.f;        
        edgeToSnap.m_stopCoeff = edgeToSnap.m_startCoeff;

        if ( normSnap < 0.01f)
            return;

        u32 idNextEdgeToUpdate = _idEdgeToUpdate +1;
        bbool isNextBuildEdge = findNextBuildEdge( _edgeList, idNextEdgeToUpdate );

        if ( !isNextBuildEdge && isLooping())
            return;
        
        // edge to update
        Vec2d snapTranslation = edgeToSnap.m_sightNormalized * normSnap;
        edgeToUpdate.m_pos -= snapTranslation;
        edgeToUpdate.m_interUp -= snapTranslation;
        edgeToUpdate.m_interDown -= snapTranslation;

        edgeToUpdate.m_points[0] -= snapTranslation;
        edgeToUpdate.m_points[1] -= snapTranslation;
        edgeToUpdate.m_points[2] -= snapTranslation;
        edgeToUpdate.m_points[3] -= snapTranslation;

        if ( isNextBuildEdge )
        {
            edgeFrieze& edgeNext = _edgeList[idNextEdgeToUpdate];
            Vec2d sightDest = edgeNext.m_pos + edgeNext.m_sight;

            edgeNext.m_pos = edgeToUpdate.m_pos + edgeToUpdate.m_sight;
            updateEdgeFriezeVector( edgeNext, sightDest - edgeNext.m_pos );

#ifdef DEVELOPER_JAY_FRIEZE
            ITF_ASSERT( edgeNext.m_sightNormalized != Vec2d::Zero );
#endif
        }
    }

    bbool Frise::findNextBuildEdge( ITF_VECTOR<edgeFrieze>& _edgeList, u32 &_idEdge ) const
    {            
        for ( ; _idEdge<m_pRecomputeData->m_edgeListCount; _idEdge++ )
        {
            if ( _edgeList[_idEdge].m_build )
                return btrue;
        }

        return bfalse;
    }

    bbool Frise::findPreviousBuildEdge( ITF_VECTOR<edgeFrieze>& _edgeList, i32 &_idEdge ) const
    {            
        for ( ; _idEdge >= 0; _idEdge-- )
        {
            if ( _edgeList[_idEdge].m_build )
                return btrue;
        }

        return bfalse;
    }

    void Frise::snapAllEdge(ITF_VECTOR<edgeFrieze>& _edgeList)
    {
        const FriseConfig* config = getConfig();

        if ( !config )
            return;

        m_pRecomputeData->m_edgeListCount = 4;
        _edgeList.clear();

        Vec2d startPos = Vec2d::Zero;
        u32 polyPointListCount = m_pointsList.getPosCount() -1;

        for(u32 i=0; i<polyPointListCount; i++)
        {
            startPos += m_pointsList.getEdgeAt(i).getPos();
        }

        startPos = startPos / (f32)polyPointListCount;
        Vec2d normalEdge = m_pointsList.getEdgeAt(1).getPos() -m_pointsList.getEdgeAt(0).getPos();
        normalEdge.normalize();

        if ( normalEdge.IsEqual(Vec2d::Zero))        
            normalEdge = Vec2d::Up;        
        else
            normalEdge = normalEdge.getPerpendicular();

        Vec2d startRot = startPos + normalEdge;

#ifdef DEVELOPER_JAY_FRIEZE
        GFX_ADAPTER->drawDBGCircle( transformPosLocalToWorld(startPos), m_pRecomputeData->m_heightScale*1.5f, Color::pink());
#endif

        f32 ratio = 1.f / (f32)m_pRecomputeData->m_edgeListCount;
        f32 angleStep = MTH_2PI *ratio;
        f32 angleEdgeNormal = 0.f;
        f32 angleCornerNormal = angleStep *0.5f;
        f32 cornerLong = m_pRecomputeData->m_heightScale *config->m_cornerFactor;

        bbool invertQuad = normalEdge.dot( m_pointsList.getEdgeAt(0).getPos() -startPos) < -0.001f;
        if( invertQuad)
        {
            angleStep = -angleStep;
            angleCornerNormal = -angleCornerNormal;
        }

        for(u32 i=0; i<m_pRecomputeData->m_edgeListCount; i++)
        {                
            edgeFrieze edge;

            edge.m_pos = startPos;            
            edge.m_snap = btrue;
            edge.m_norm = 0.f;
            edge.m_startCoeff = 0.f;
            edge.m_stopCoeff = 0.f;
            edge.m_normal = startRot.RotateAround(startPos, angleEdgeNormal) - startPos;
            edge.m_cornerAngle = angleStep;
            edge.m_cornerNormal = startRot.RotateAround(startPos, angleCornerNormal) - startPos;
            edge.m_heightStart = m_pRecomputeData->m_heightScale;
            edge.m_heightStop = m_pRecomputeData->m_heightScale;

            if( !invertQuad)
            {
                edge.m_sight = -edge.m_normal.getPerpendicular();
                edge.m_interUp = edge.m_pos + edge.m_cornerNormal * cornerLong;
                edge.m_interDown = edge.m_pos;
                edge.m_points[0] = edge.m_pos;
                edge.m_points[1] = edge.m_pos +edge.m_normal *m_pRecomputeData->m_heightScale;
            }
            else
            {
                edge.m_sight = edge.m_normal.getPerpendicular();
                edge.m_interUp = edge.m_pos;
                edge.m_interDown = edge.m_pos + edge.m_cornerNormal * cornerLong;                
                edge.m_points[0] = edge.m_pos +edge.m_normal *m_pRecomputeData->m_heightScale;
                edge.m_points[1] = edge.m_pos;
            }

            edge.m_sightNormalized = edge.m_sight;
            edge.m_sightNormalized.normalize();
            edge.m_points[2] = edge.m_points[0];                    
            edge.m_points[3] = edge.m_points[1];

            angleEdgeNormal -= angleStep;
            angleCornerNormal -= angleStep;

            _edgeList.push_back(edge);
        }

    }

    bbool Frise::setOffsetEdge( edgeFrieze& curEdge, edgeFrieze& lastEdge, Vec2d& _posInit, f32 _offset, f32 longMax, f32 longMaxSqr ) const
    {
        //  edge parallel?
        f32 dotSight = lastEdge.m_sightNormalized.dot(curEdge.m_sightNormalized);
        if ( dotSight + MTH_EPSILON >= 1.f ) // angle = 0
        {
            curEdge.m_pos -= curEdge.m_normal *_offset;
            return updateEdgeFriezeVector( lastEdge, curEdge.m_pos - lastEdge.m_pos );
        }

        if ( dotSight - MTH_EPSILON <= -1.f ) // angle = MTH_PI             
            return btrue;

        //  corner position
        Vec2d p1 = lastEdge.m_pos;
        Vec2d p2 = p1 + lastEdge.m_sight;
        Vec2d p3 = curEdge.m_pos -curEdge.m_normal *_offset;
        Vec2d p4 = p3 + curEdge.m_sight;

        intersectionLineLine( p1, p2, p3, p4, curEdge.m_interUp );

        Vec2d sight = curEdge.m_interUp -_posInit;
        if ( sight.sqrnorm() > longMaxSqr )
        {
            sight.normalize();
            curEdge.m_pos = _posInit +sight *longMax;
        }
        else
            curEdge.m_pos = curEdge.m_interUp;

        return updateEdgeFriezeVector( lastEdge, curEdge.m_pos - lastEdge.m_pos );
    }

    bbool Frise::isEdgeWithHoleCollision( const edgeFrieze& _edge ) const
    {
        return ( _edge.m_holeMode & PolyLineEdge::Hole_Collision );
    }

    bbool Frise::isEdgeWithHoleVisual( const edgeFrieze& _edge ) const
    {
        return ( m_pRecomputeData->m_isHoleVisual && _edge.m_holeMode & PolyLineEdge::Hole_Visual );
    }

    bbool Frise::isEdgeWithHole( const edgeFrieze& _edge ) const
    {
        return ( isEdgeWithHoleCollision( _edge ) || isEdgeWithHoleVisual( _edge ) );
    }

    bbool Frise::isEdgeRunWithSwitchTexture( edgeRun& _edgeRun, ITF_VECTOR<edgeFrieze>& _edgeList ) const
    {
        for ( u32 i = 0; i < _edgeRun.m_edgeCount; i++ )
        {
            u32 idCurEdge = ( _edgeRun.m_idEdgeStart + i) % m_pRecomputeData->m_edgeListCount;

            if ( _edgeList[idCurEdge].m_switchTexture )
                return btrue;
        }

        return bfalse;
    }

    void Frise::setScaleNewEdge( edgeFrieze& _edgeNew, edgeFrieze& _edgeLast, edgeFrieze& _edgeNext ) const
    {
        _edgeNew.m_scaleCur = _edgeNext.m_scaleCur;
        _edgeNew.m_heightStart = _edgeLast.m_heightStop;
        _edgeNew.m_heightStop = _edgeNext.m_heightStart;
    }

    Vec2d Frise::getPosStartOnNormalEdge( const edgeFrieze* _edge, f32 _offset ) const
    {         
        return _edge->m_points[0] +getEdgeBorderLeft(*_edge) *_offset;
    }

    Vec2d Frise::getPosStopOnNormalEdge( const edgeFrieze* _edge, f32 _offset ) const
    {        
        return _edge->m_points[2] +getEdgeBorderRight(*_edge) *_offset;
    }

    Vec2d Frise::getPosOnCornerEdge( const edgeFrieze* _edge, f32 _offset ) const
    {
        return _edge->m_interDown + ( _edge->m_interUp -_edge->m_interDown ) *_offset;      
    }

    Vec2d Frise::getEdgeBorderLeft( const edgeFrieze& _edge) const
    {
        return _edge.m_points[1] -_edge.m_points[0];
    }

    Vec2d Frise::getEdgeBorderRight( const edgeFrieze& _edge) const
    {
        return _edge.m_points[3] -_edge.m_points[2];
    }

    bbool Frise::isHeightEqual( const edgeFrieze& _edgeLast, const edgeFrieze& _edgeCur) const
    {
        return _edgeLast.m_heightStop == _edgeCur.m_heightStart;
    }

    u32 Frise::getCountBuildEgdes( ITF_VECTOR<edgeFrieze>& _edgeList ) const
    {
#ifdef DEVELOPER_JAY_FRIEZE
        ITF_ASSERT(m_pRecomputeData->m_edgeListCount <= _edgeList.size());
#endif

        u32 count = 0;

        for ( u32 i = 0; i < m_pRecomputeData->m_edgeListCount; i++)
        {
            if( _edgeList[i].m_build )
                count++;
        }

        return count;
    }

    u32 Frise::getCountSnapEgdes( ITF_VECTOR<edgeFrieze>& _edgeList ) const
    {
#ifdef DEVELOPER_JAY_FRIEZE
        ITF_ASSERT(m_pRecomputeData->m_edgeListCount <= _edgeList.size());
#endif

        u32 count = 0;

        for ( u32 i = 0; i < m_pRecomputeData->m_edgeListCount; i++)
        {
            if( _edgeList[i].m_build && _edgeList[i].m_snap)
                count++;
        }

        return count;
    }

    bbool Frise::isScaleEqual( const edgeFrieze& _edgeLast, const edgeFrieze& _edgeCur) const
    {
        if ( f32_Abs(_edgeLast.m_scaleCur -_edgeLast.m_scaleCur) <= MTH_EPSILON )
            return btrue;

        return bfalse;
    }

    void Frise::setPosStartOnEdgeWithHoleVisual( const FriseConfig* _config, const edgeFrieze& edge, Vec2d& _down, Vec2d& _up) const
    {
        const PolyLineEdge& polyEdge = getEdgeAt(edge.m_idPoint);
        const Vec2d polyEdgeNormal = polyEdge.m_normalizedVector.getPerpendicular();
        const f32 edgeHeight = polyEdge.getScale() *m_pRecomputeData->m_heightScale;

        Vec2d offset = polyEdge.m_normalizedVector *holeOffset;
        _down = polyEdge.getPos() -polyEdgeNormal *edgeHeight *_config->m_visualOffset -offset;
        _up = _down +polyEdgeNormal *edgeHeight;
    }

    void Frise::setPosStopOnEdgeWithHoleVisual( const FriseConfig* _config, const edgeFrieze& edge, Vec2d& _down, Vec2d& _up, u32 _idPoint) const
    {
        const PolyLineEdge& polyEdge = getEdgeAt(edge.m_idPoint);
        const Vec2d polyEdgeNormal = polyEdge.m_normalizedVector.getPerpendicular();
        const f32 edgeHeight = getEdgeAt(_idPoint).getScale() *m_pRecomputeData->m_heightScale;

        Vec2d offset = polyEdge.m_normalizedVector *holeOffset;
        _down = getPosAt(_idPoint) -polyEdgeNormal *edgeHeight *_config->m_visualOffset +offset;
        _up = _down +polyEdgeNormal *edgeHeight;
    }

    u32 Frise::getIdEdgeNext( u32 _id) const
    {
        return ( _id +1) %m_pRecomputeData->m_edgeListCount;
    }

    u32 Frise::getIdEdgeLast( u32 _id) const
    {
        return ( _id +m_pRecomputeData->m_edgeListCount -1) %m_pRecomputeData->m_edgeListCount;
    }

    void Frise::insertNewEdgeInEdgeList( ITF_VECTOR<edgeFrieze>& _edgeList, edgeFrieze& _newEdge, u32 _index )
    {
        if ( _index == 0 )
        {
            _edgeList.push_back( _newEdge );
        }
        else
        {
            ITF_VECTOR<edgeFrieze>::iterator it;
            it = _edgeList.begin() +_index;
            _edgeList.insert( it, _newEdge );
        }

        m_pRecomputeData->m_edgeListCount++;
    }

#ifdef DEVELOPER_JAY_FRIEZE
    void Frise::debugDrawEdgeRun( ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        String8 txt;
        txt.setTextFormat("");
        GFX_ADAPTER->drawDBGText(txt);

        const u32 size = 0;//_edgeRunList.size();
        for ( u32 i= 0; i<size; i++ )
        {         
            edgeRun& edge = _edgeRunList[i];

            txt.setTextFormat("EdgeRun Start = %i, Count = %i, idTex = %i, coeff = %f", edge.m_idEdgeStart, edge.m_edgeCount, edge.m_idTex, edge.m_coeff);
            GFX_ADAPTER->drawDBGText(txt);
        }
    }

    void Frise::debugDrawEdgeList(ITF_VECTOR<edgeFrieze>& _edgeList) const
    {
        f32 size = 1.f;
        f32 z = getDepth();
        Color edgeColor(Color::yellow());
        Color cornerColor(Color::red());

        for ( u32 i = 0; i<m_pRecomputeData->m_edgeListCount; i++ )
        {
            edgeFrieze& edge = _edgeList[i];

            if ( edge.m_cornerAngle != 0.f)
            {
                u32 idEdgeLast = ( i +m_pRecomputeData->m_edgeListCount -1)%m_pRecomputeData->m_edgeListCount;
                edgeFrieze& edgeLast = _edgeList[idEdgeLast];
                if ( edge.m_cornerAngle > 0.f)
                {
                    GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(edgeLast.m_points[3]), transformPosLocalToWorld(edge.m_interUp), cornerColor, 0.f, size, z );
                    GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(edge.m_interUp), transformPosLocalToWorld(edge.m_points[1]), cornerColor, 0.f, size, z );
                }
                else if ( edge.m_cornerAngle < 0.f)
                {
                    GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(edgeLast.m_points[2]), transformPosLocalToWorld(edge.m_interDown), cornerColor, 0.f, size, z );
                    GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(edge.m_interDown), transformPosLocalToWorld(edge.m_points[0]), cornerColor, 0.f, size, z );
                }                
            }
            
            GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(edge.m_points[0]), transformPosLocalToWorld(edge.m_points[1]), edgeColor, 0.f, size, z );
            GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(edge.m_points[1]), transformPosLocalToWorld(edge.m_points[3]), edgeColor, 0.f, size, z );
            GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(edge.m_points[3]), transformPosLocalToWorld(edge.m_points[2]), edgeColor, 0.f, size, z );
            GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(edge.m_points[2]), transformPosLocalToWorld(edge.m_points[0]), edgeColor, 0.f, size, z );
            GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(edge.m_interDown), transformPosLocalToWorld(edge.m_interUp), cornerColor, 0.f, size, z );

            GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(edge.m_pos), transformPosLocalToWorld(edge.m_pos +edge.m_sight), Color::white(), 0.f, size, z );


            debugDrawFloat( transformPosLocalToWorld(edge.m_pos), edge.m_scaleCur, Color::pink());
        }
    }

    void Frise::debugDrawInt( Vec2d pos2d, u32 _i, Color _color ) const
    {
        Vec3d pos3dIn(pos2d.x(), pos2d.y(), getDepth());
        Vec3d pos3dOut;
        GFX_ADAPTER->compute3DTo2D(pos3dIn, pos3dOut); 

        String8 txt;
        txt.setTextFormat("%i", _i);
        GFX_ADAPTER->drawDBGText(txt, _color, pos3dOut.x(), pos3dOut.y() );
    }

    void Frise::debugDrawFloat( Vec2d pos2d, f32 _f, Color _color ) const
    {
        Vec3d pos3dIn(pos2d.x(), pos2d.y(), getDepth());
        Vec3d pos3dOut;
        GFX_ADAPTER->compute3DTo2D(pos3dIn, pos3dOut); 

        String8 txt;
        txt.setTextFormat("%f", _f);
        GFX_ADAPTER->drawDBGText(txt, _color, pos3dOut.x(), pos3dOut.y() );
    }
#endif


} // namespace ITF
