#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    f32 Frise::getCornerEdgeNorm( f32 _angle, f32 _radius ) const
    {
        return ( 2.f * _radius * f32_Sin(_angle * 0.5f) );      
    }

    void Frise::buildOutline_EdgeRunWithCornerRounder( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun* _edgeRun, f32 _offset, f32 _stepFan, u32 _edgeCountStartOffset, u32 _edgeCountStopOffset, bbool _isOutlineCollision ) const
    {
        const u32 idEdgeLast = ( _edgeRun->m_idEdgeStart +_edgeCountStartOffset ) % m_pRecomputeData->m_edgeListCount;
        const edgeFrieze* edgeLast = &_edgeList[idEdgeLast];

#ifdef DEVELOPER_JAY_FRIEZE
        ITF_ASSERT( _edgeRun->m_edgeCount >= _edgeCountStopOffset );
#endif

        for ( u32 edgeCount = 1 +_edgeCountStartOffset; edgeCount < _edgeRun->m_edgeCount -_edgeCountStopOffset; edgeCount++ )
        {
            const u32 idEdgeCur = ( _edgeRun->m_idEdgeStart + edgeCount ) % m_pRecomputeData->m_edgeListCount;
            const edgeFrieze* edgeCur = &_edgeList[idEdgeCur];

            if ( buildOutlineFromHole( _config, _outline, edgeCur, edgeLast, _offset, _offset, _isOutlineCollision ))
            {
            }
            else if ( edgeCur->m_cornerAngle == 0.f )
            {
                _outline.push_back( getPosStartOnNormalEdge( edgeCur, _offset));         
            }
            else
            {
                buildOutline_CornerRounder( _outline, edgeCur, edgeLast, _offset, _stepFan);       
            }

            edgeLast = edgeCur;
        }
    }

    void Frise::buildOutline_EdgeRunWithCornerSquare( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun* _edgeRun, f32 _offset, u32 _edgeCountStartOffset, u32 _edgeCountStopOffset, bbool _isOutlineCollision ) const
    {     
        const u32 idEdgeLast = ( _edgeRun->m_idEdgeStart +_edgeCountStartOffset ) % m_pRecomputeData->m_edgeListCount;
        const edgeFrieze* edgeLast = &_edgeList[idEdgeLast];

#ifdef DEVELOPER_JAY_FRIEZE
        ITF_ASSERT( _edgeRun->m_edgeCount >= _edgeCountStopOffset );
#endif

        for ( u32 edgeCount = 1 +_edgeCountStartOffset; edgeCount < _edgeRun->m_edgeCount -_edgeCountStopOffset; edgeCount++ )
        {
            const u32 idEdgeCur = ( _edgeRun->m_idEdgeStart + edgeCount ) % m_pRecomputeData->m_edgeListCount;
            const edgeFrieze* edgeCur = &_edgeList[idEdgeCur];

            if ( buildOutlineFromHole( _config, _outline, edgeCur, edgeLast, _offset, _offset, _isOutlineCollision ))
            {
            }
            else if ( edgeCur->m_cornerAngle == 0.f )
            {
                _outline.push_back( getPosStartOnNormalEdge( edgeCur, _offset));
            }
            else            
            {
                buildOutline_CornerSquare( _outline, edgeCur, edgeLast, _offset );
            }

            edgeLast = edgeCur;
        }
    }

    void Frise::buildOutline_CornerRounder( SafeArray<Vec2d>& _outline, const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast, f32 _offset, f32 _stepFan ) const
    {
        if ( !isHeightEqual( *_edgeLast, *_edgeCur))
        {
            buildOutline_CornerRounderBlending( _outline, _edgeLast, _edgeCur, _offset, _offset, _stepFan);
            return;
        }

        Vec2d posCorner;
        Vec2d posCenter;
        f32 coeffStepFan = _offset;

        if ( _edgeCur->m_cornerAngle > 0.f ) 
        {
            posCenter = _edgeCur->m_interDown;
        }
        else
        {
            posCenter = _edgeCur->m_interUp;
            coeffStepFan = 1.f -coeffStepFan;
        }

        // corner's edge norm 
        f32 angle = f32_Abs( _edgeCur->m_cornerAngle );
        u32 stepCount = getCornerStepNb( angle, _stepFan *coeffStepFan, _edgeCur->m_scaleCur);
        f32 stepCornerAngle = angle / (f32)stepCount;   
       
        angle = -stepCornerAngle * getSign(_edgeCur->m_cornerAngle);
        f32 rotCos, rotSin;
        f32_CosSin(angle, &rotCos, &rotSin);

        posCorner = getPosStopOnNormalEdge( _edgeLast, _offset);
        _outline.push_back( posCorner );

        for ( u32 i = 1; i <= stepCount; i++ )
        {
            posCorner = posCorner.RotateAroundCS(posCenter, rotCos, rotSin);
            _outline.push_back(posCorner);
        }
    }

    void Frise::buildOutline_CornerRounderBlending( SafeArray<Vec2d>& _outline, const edgeFrieze* _edgeLast, const edgeFrieze* _edgeCur, f32 _offsetLast, f32 _offsetCur, f32 _stepFan, i32 _countOffset, Vec2d* _posCorner ) const
    {
        // bezier curve        
        Vec2d p0 = getPosStopOnNormalEdge( _edgeLast, _offsetLast);       
        Vec2d p3 = getPosStartOnNormalEdge( _edgeCur, _offsetCur);
        Vec2d p1 = p0; 
        Vec2d p2 = p3;

        f32 coeffStepFan = ( _offsetCur +_offsetLast) *0.5f;

        if ( _edgeCur->m_cornerAngle <0.f )
        {
            _offsetCur = 1.f -_offsetCur;
            _offsetLast = 1.f -_offsetLast;
            coeffStepFan = 1.f -coeffStepFan;
        }

        // corner's edge norm 
        f32 angle = f32_Abs( _edgeCur->m_cornerAngle );
        u32 stepCount = Max( 2u, getCornerStepNb( angle, _stepFan *coeffStepFan, getCoeffScaleStepFanToBlend( _edgeCur, _edgeLast)));  

        if ( _posCorner )
        {
            p1 = p2 = *_posCorner;

            if ( stepCount&1 )
                stepCount ++;
        }
        else
            buildOutline_initPosCornerRounderBlending( _edgeLast, _edgeCur, p1, p2, _offsetCur, _offsetLast);

        u32 countMid = 0;
        if ( _countOffset!= 0 )
        {
            if ( stepCount&1 )
                stepCount ++;

            countMid = stepCount /2;                
        }

        f32 timeStep = 1.f /(f32)stepCount;



#ifdef DEVELOPER_JAY_FRIEZE
        GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(p0), transformPosLocalToWorld(p1), Color::green(), 0.f, 15.f, getDepth() );
        GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(p1), transformPosLocalToWorld(p2), Color::green(), 0.f, 15.f, getDepth() );
        GFX_ADAPTER->drawDBGLine( transformPosLocalToWorld(p2), transformPosLocalToWorld(p3), Color::green(), 0.f, 15.f, getDepth() );
#endif
              
        u32 i = 0;
        if ( _countOffset >0)
            i = countMid;
        else if ( _countOffset <0)
            stepCount = countMid;

        for (  ; i <= stepCount; i++ )
        {
            f32 time = (f32)i * timeStep;
            f32 timeInv = 1.f - time;

            Vec2d pos =   ( p0 *timeInv *timeInv *timeInv ) 
                + ( p1 *3.f *time *timeInv *timeInv ) 
                + ( p2 *3.f *time *time *timeInv ) 
                + ( p3 *time *time *time );

            _outline.push_back(pos);
        }
    }

    bbool Frise::buildOutline_isStartEdgeRunWithRounder( const FriseConfig* _config ) const
    {     
        bbool isRounder = btrue;

        switch ( _config->m_methode)
        {
            case InArchitecture:
            case InArchitectureSimple:
            case InPipe:
                isRounder = bfalse;
            break;
               
            default:
                isRounder = btrue;
            break;
        }

        return isRounder;
    }

    bbool Frise::buildOutlineFromHole( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze* edgeCur, const edgeFrieze* edgeLast, f32 _offsetLast, f32 _offsetCur, bbool _isOutlineCollision ) const
    {
        if ( _isOutlineCollision )
            return buildOutlineFromHoleCollision( _config, _outline, edgeCur, edgeLast, _offsetLast, _offsetCur );

        return buildOutlineFromHoleVisual( _config, _outline, edgeCur, edgeLast, _offsetLast, _offsetCur );
    }

    bbool Frise::buildOutlineFromHoleVisual( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze* edgeCur, const edgeFrieze* edgeLast, f32 _offsetLast, f32 _offsetCur ) const
    {
        if ( isEdgeWithHoleVisual(*edgeCur) && isEdgeWithHoleVisual(*edgeLast))
        {
            _outline.push_back( getPosAt(edgeCur->m_idPoint));
            return btrue;
        }

        if ( isEdgeWithHoleVisual(*edgeCur))
        {
            buildOutline_PosStopOnEdgeWithHole( _config, _outline, *edgeLast, _offsetLast, edgeCur->m_idPoint);
            return btrue;
        }
        
        if ( isEdgeWithHoleVisual(*edgeLast))
        {
            buildOutline_PosStartOnEdgeWithHole( _config, _outline, *edgeCur, _offsetCur);
            return btrue;
        }

        return bfalse;
    }

    bbool Frise::buildOutlineFromHoleCollision( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze* edgeCur, const edgeFrieze* edgeLast, f32 _offsetLast, f32 _offsetCur ) const
    {
        if ( isEdgeWithHoleCollision(*edgeCur) && isEdgeWithHoleCollision(*edgeLast))
        {
            _outline.push_back( getPosAt(edgeCur->m_idPoint));
            return btrue;
        }

        if ( isEdgeWithHoleCollision(*edgeCur))
        {
            buildOutline_PosStopOnEdgeWithHole( _config, _outline, *edgeLast, _offsetLast, edgeCur->m_idPoint);
            return btrue;
        }

        if ( isEdgeWithHoleCollision(*edgeLast))
        {
            buildOutline_PosStartOnEdgeWithHole( _config, _outline, *edgeCur, _offsetCur);
            return btrue;
        }

        return bfalse;
    }

    void Frise::buildOutline_StartEdgeRunCornerRounder( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun* _edgeRunLast, const edgeRun* _edgeRunCur, f32 _offsetLast, f32 _offsetCur, f32 _stepFan, bbool _isOutlineCollision ) const
    {     
        if ( _edgeRunLast == NULL )
            return;

        // edge cur
        const edgeFrieze* edgeCur = &_edgeList[_edgeRunCur->m_idEdgeStart];

        // edge last
        const u32 idEdgeLast = (_edgeRunLast->m_idEdgeStart + _edgeRunLast->m_edgeCount +m_pRecomputeData->m_edgeListCount -1) % m_pRecomputeData->m_edgeListCount;
        const edgeFrieze* edgeLast = &_edgeList[idEdgeLast];

        if ( buildOutlineFromHole( _config, _outline, edgeCur, edgeLast, _offsetLast, _offsetCur, _isOutlineCollision ))
        {
            return;
        }

        if ( _offsetCur == _offsetLast )
        {
            buildOutline_CornerRounder( _outline, edgeCur, edgeLast, _offsetCur, _stepFan );
        }
        else
        {
            buildOutline_CornerRounderBlending( _outline, edgeLast, edgeCur, _offsetLast, _offsetCur, _stepFan);
        }        
    }

    void Frise::buildOutline_CornerSquare( SafeArray<Vec2d>& _outline, const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast, f32 _offset ) const
    {
        _outline.push_back( getPosStopOnNormalEdge( _edgeLast, _offset) );
        _outline.push_back( getPosOnCornerEdge( _edgeCur, _offset) );
        _outline.push_back( getPosStartOnNormalEdge( _edgeCur, _offset) );
    }

    void Frise::buildOutline_CornerSquareBlending( SafeArray<Vec2d>& _outline, const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast, f32 _offsetCur, f32 _offsetLast ) const
    {
        Vec2d posStart, posStop, posSquare;
        setPos_CornerSquareBlending( posStart, posStop, posSquare, _edgeCur, _edgeLast, _offsetCur, _offsetLast );

        _outline.push_back( posStart );
        _outline.push_back( posSquare );
        _outline.push_back( posStop );
    }

    void Frise::buildOutline_StartEdgeRunCornerSquare( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun* _edgeRunLast, const edgeRun* _edgeRunCur, f32 _offsetCur, f32 _offsetLast, bbool _isOutlineCollision) const
    {     
        if ( _edgeRunLast == NULL )
            return;

        // edge cur
        const edgeFrieze* edgeCur = &_edgeList[_edgeRunCur->m_idEdgeStart];

        // edge last
        const u32 idEdgeLast = (_edgeRunLast->m_idEdgeStart + _edgeRunLast->m_edgeCount +m_pRecomputeData->m_edgeListCount -1) % m_pRecomputeData->m_edgeListCount;
        const edgeFrieze* edgeLast = &_edgeList[idEdgeLast];

        if ( buildOutlineFromHole( _config, _outline, edgeCur, edgeLast, _offsetLast, _offsetCur, _isOutlineCollision ))
        {
            return;
        }

        if ( _offsetCur == _offsetLast )
        {
            buildOutline_CornerSquare( _outline, edgeCur, edgeLast, _offsetCur);
        }
        else
        {
            buildOutline_CornerSquareBlending( _outline, edgeCur, edgeLast, _offsetCur, _offsetLast );
        }        
    }

    void Frise::setPos_CornerSquareBlending( Vec2d& _posStart, Vec2d& _posStop, Vec2d& _posSquare, const edgeFrieze* _edgeCur, const edgeFrieze* _edgeLast, f32 _offsetCur, f32 _offsetLast ) const
    {              
        _posStart = getPosStopOnNormalEdge( _edgeLast, _offsetLast);
        _posStop = getPosStartOnNormalEdge( _edgeCur, _offsetCur);

        Vec2d p1 = _posStart; 
        Vec2d p2 = _posStop;

        if ( _edgeCur->m_cornerAngle <0.f )
        {
            _offsetCur = 1.f -_offsetCur;
            _offsetLast = 1.f -_offsetLast;
        }

        buildOutline_initPosCornerRounderBlending( _edgeLast, _edgeCur, p1, p2, _offsetCur, _offsetLast);

        _posSquare = ( _posStart + _posStop + (p1 + p2)*3.f ) *0.5f *0.5f *0.5f;
    }
   
    void Frise::buildOutline_CornerSquareBlending_startEdgeRun( SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdgeCur, f32 _offsetHeightCur, f32 _offsetHeightLast ) const
    {     
        const u32 idEdgeLast = ( _idEdgeCur +m_pRecomputeData->m_edgeListCount -1 ) % m_pRecomputeData->m_edgeListCount;
        edgeFrieze* edgeCur = &_edgeList[_idEdgeCur];        
        edgeFrieze* edgeLast = &_edgeList[idEdgeLast];

        Vec2d posStart, posStop, posSquare;
        setPos_CornerSquareBlending( posStart, posStop, posSquare, edgeCur, edgeLast, _offsetHeightCur, _offsetHeightLast );

        _outline.push_back( posSquare );
        _outline.push_back( posStop );
    }

    void Frise::buildOutline_CornerSquareBlending_stopEdgeRun( SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdgeCur, f32 _offsetHeightCur, f32 _offsetHeightNext ) const
    {     
        const u32 idEdgeNext = ( _idEdgeCur +1 ) % m_pRecomputeData->m_edgeListCount;
        edgeFrieze* edgeCur = &_edgeList[_idEdgeCur];        
        edgeFrieze* edgeNext = &_edgeList[idEdgeNext];

        Vec2d posStart, posStop, posSquare;
        setPos_CornerSquareBlending( posStart, posStop, posSquare, edgeNext, edgeCur, _offsetHeightNext, _offsetHeightCur );

        _outline.push_back( posStart );
        _outline.push_back( posSquare );
    }

    void Frise::buildOutline_initPosCornerRounderBlending( const edgeFrieze* _edgeLast, const edgeFrieze* _edgeCur, Vec2d &_p1, Vec2d &_p2, f32 _offsetCur, f32 _offsetLast) const
    {
        f32 angleRatio = f32_Abs(_edgeCur->m_cornerAngle) * MTH_BYPI;
        Vec2d sightEdgeLast = _edgeLast->m_sightNormalized;
        Vec2d sightEdgeCur = -_edgeCur->m_sightNormalized;

        if ( _edgeCur->m_cornerAngle >0.f )
        {
            if ( !_edgeLast->m_snap)
            {
                sightEdgeLast = _edgeLast->m_points[3] -_edgeLast->m_points[1]; 
                sightEdgeLast.normalize();
            }      
           
            if ( !_edgeCur->m_snap)
            {
                sightEdgeCur= _edgeCur->m_points[1] -_edgeCur->m_points[3];    
                sightEdgeCur.normalize();
            }
        }
        else
        {
            if ( !_edgeLast->m_snap)
            {
                sightEdgeLast = _edgeLast->m_points[2] -_edgeLast->m_points[0];            
                sightEdgeLast.normalize();
            }

            if ( !_edgeCur->m_snap)
            {
                sightEdgeCur = _edgeCur->m_points[0] -_edgeCur->m_points[2];            
                sightEdgeCur.normalize();
            }            
        }

        _p1 += sightEdgeLast *_edgeCur->m_heightStart *angleRatio *_offsetCur;
        _p2 += sightEdgeCur *_edgeLast->m_heightStop *angleRatio *_offsetLast;
    }

    void Frise::buildOutline_PosStartOnEdgeWithHole( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze& edge, f32 _ratio) const
    {
        const PolyLineEdge& polyEdge = getEdgeAt(edge.m_idPoint);
        const Vec2d polyEdgeNormal = polyEdge.m_normalizedVector.getPerpendicular();
        const f32 edgeHeight = polyEdge.getScale() *m_pRecomputeData->m_heightScale;

        Vec2d pos = polyEdge.getPos() -polyEdgeNormal *edgeHeight *_config->m_visualOffset;
        _outline.push_back(pos);
        pos += polyEdgeNormal *edgeHeight *_ratio;
        _outline.push_back(pos);
    }

    void Frise::buildOutline_PosStopOnEdgeWithHole( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze& edge, f32 _ratio, u32 _idPoint) const
    {
        const PolyLineEdge& polyEdge = getEdgeAt(edge.m_idPoint);
        const Vec2d polyEdgeNormal = polyEdge.m_normalizedVector.getPerpendicular();
        const f32 edgeHeight = getEdgeAt(_idPoint).getScale() *m_pRecomputeData->m_heightScale;

        Vec2d pos0 = getPosAt(_idPoint) -polyEdgeNormal *edgeHeight *_config->m_visualOffset;
        Vec2d pos1 = pos0 +polyEdgeNormal *edgeHeight *_ratio;

        _outline.push_back(pos1);
        _outline.push_back(pos0);
    }

    void Frise::buildOutline_PosStartOnEdgeWithHoleCollision( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze& edge, f32 _ratio) const
    {
        const PolyLineEdge& polyEdge = getEdgeAt(edge.m_idPoint);
        const Vec2d polyEdgeNormal = polyEdge.m_normalizedVector.getPerpendicular();
        const f32 edgeHeight = polyEdge.getScale() *m_pRecomputeData->m_heightScale;

        Vec2d pos = polyEdge.getPos() -polyEdgeNormal *edgeHeight *_config->m_visualOffset;
        pos += polyEdgeNormal *edgeHeight *_ratio;
        _outline.push_back(pos);
    }

    void Frise::buildOutline_PosStopOnEdgeWithHoleCollision( const FriseConfig* _config, SafeArray<Vec2d>& _outline, const edgeFrieze& edge, f32 _ratio, u32 _idPoint) const
    {
        const PolyLineEdge& polyEdge = getEdgeAt(edge.m_idPoint);
        const Vec2d polyEdgeNormal = polyEdge.m_normalizedVector.getPerpendicular();
        const f32 edgeHeight = getEdgeAt(_idPoint).getScale() *m_pRecomputeData->m_heightScale;

        Vec2d pos = getPosAt(_idPoint) -polyEdgeNormal *edgeHeight *_config->m_visualOffset;
        pos += polyEdgeNormal *edgeHeight *_ratio;

        _outline.push_back(pos);
    }
} // namespace ITF
