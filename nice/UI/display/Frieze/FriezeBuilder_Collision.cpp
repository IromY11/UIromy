#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_DEPTH_RANGE_H_
#include "engine/scene/DepthRange.h"
#endif //_ITF_DEPTH_RANGE_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_GAMEMATERIALSMANAGER_H_
#include "gameplay/managers/GameMaterialManager.h"
#endif //_ITF_GAMEMATERIALSMANAGER_H_

#ifndef _ITF_PHYS2D_H_
#include "engine/Phys2d/Phys2d.h"
#endif //_ITF_PHYS2D_H_

namespace ITF
{
    static const f32 deltaConnection = 0.1f;
    static const f32 deltaConnectionSqr = deltaConnection * deltaConnection;    


    void Frise::updateConnections()
    {
        if ( !m_pCollisionData )
            return;    

        PhysContactsContainer   contacts;
        PhysShapeCircle         circle(deltaConnection);

        for ( ITF_VECTOR<PolyLine*>::iterator it = m_pCollisionData->m_worldCollisionList.begin(); it != m_pCollisionData->m_worldCollisionList.end(); ++it )
        {
            PolyLine* pPolRef = *it;

            if ( pPolRef->isLooping() )
                continue;

            const u32 posCountRef = pPolRef->getPosCount();
            
            if ( !pPolRef->getConnection().getPrevious() )
            {
                // check polRef first point
                const Vec2d& startPosRef = pPolRef->getPosAt(0);
                contacts.clear();
                PHYSWORLD->checkEncroachment( startPosRef, startPosRef, 0.f, &circle, ECOLLISIONFILTER_ENVIRONMENT, getDepth(), contacts);

                for ( u32 i = 0; i < contacts.size(); i++ )
                {
                    SCollidableContact& c = contacts[i];

                    if ( c.m_edgeIndex != U32_INVALID )
                    {
                        PolyLine* pPol = static_cast<PolyLine*>(GETOBJECT(c.m_collidableUserData));

                        if ( pPol && pPol != pPolRef && pPol->getOwnerFrise() && !pPol->getConnection().getNext() )
                        {
                            const u32 posCount = pPol->getPosCount();
                            if ( posCount > 1 && c.m_edgeIndex == ( posCount -2 ) )
                            {
                                const Vec2d connection = pPol->getPosAt( posCount -1 );

                                // check distance
                                if ( ( startPosRef -connection).sqrnorm() < deltaConnectionSqr )
                                {
                                    pPolRef->getConnection().setPrevious( pPol, connection );
                                    break;
                                }                            
                            }
                        }
                    }
                }
            }

            if ( !pPolRef->getConnection().getNext() )
            {
                // check polRef last point
                const Vec2d& stopPosRef  = pPolRef->getPosAt(posCountRef -1);
                contacts.clear();
                PHYSWORLD->checkEncroachment( stopPosRef, stopPosRef, 0.f, &circle, ECOLLISIONFILTER_ENVIRONMENT, getDepth(), contacts);

                for ( u32 i = 0; i < contacts.size(); i++ )
                {
                    SCollidableContact& c = contacts[i];

                    if ( c.m_edgeIndex == 0 )
                    {
                        PolyLine* pPol = static_cast<PolyLine*>(GETOBJECT(c.m_collidableUserData));

                        if ( pPol && pPol != pPolRef && pPol->getOwnerFrise() && !pPol->getConnection().getPrevious() )
                        {
                            // check distance
                            if ( ( stopPosRef -pPol->getPosAt(0) ).sqrnorm() < deltaConnectionSqr )
                            {
                                pPol->getConnection().setPrevious( pPolRef, stopPosRef );
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    void Frise::createCollisionData()
    {        
        ITF_ASSERT( !m_pCollisionData );

        m_pCollisionData = newAlloc(mId_Frieze, CollisionData);
    }

    void Frise::deleteCollisionData()
    {
        SF_DEL(m_pCollisionData);

#ifdef USE_BOX2D
        if (m_b2dBody && m_b2dWorld)
        {
            m_b2dWorld->DestroyBody(m_b2dBody);
            m_b2dBody = NULL;
            m_b2dWorld= NULL;
        }
#endif
    }

#ifdef USE_PHYSWORLD
    void Frise::setWorldCollisionPhysBody( PolyLine* pPoly )
    {
        if ( !pPoly->m_physShape )
        {
            if ( m_useMovingCollision )        
                pPoly->m_physShape = newAlloc(mId_Phys,PhysShapeMovingPolyline(pPoly));
            else
                pPoly->m_physShape = newAlloc(mId_Phys,PhysShapePolyline(pPoly));
        }
        else // alloc still done by cooking, just verify if physShape type is the good one
        {            
            if ( m_useMovingCollision ) 
            { 
                ITF_ASSERT_CRASH(pPoly->m_physShape->GetObjectClassCRC()==PhysShapeMovingPolyline::GetClassCRCStatic(), "Invalid type");  
            } 
            else 
            { 
                ITF_ASSERT_CRASH(pPoly->m_physShape->GetObjectClassCRC()==PhysShapePolyline::GetClassCRCStatic(), "Invalid type"); 
            }
        }

        PhysBodyInitCommonInfo bodyInit;

        SET_OWNER(bodyInit, pPoly->getRef());
        bodyInit.m_static = btrue;
        bodyInit.m_pos = Vec2d::Zero;
        bodyInit.m_angle = 0.f;
        bodyInit.m_userData = pPoly->getRef();
        bodyInit.m_weight = 1.f;

        const GameMaterial_Template* gmat = pPoly->getVectorsCount() ? pPoly->getEdgeAt(0).getGameMaterialTemplate() : NULL;

        bodyInit.m_collisionGroup = gmat && gmat->getCollisionGroup() ? gmat->getCollisionGroup() : ECOLLISIONGROUP_POLYLINE;

        String8 debugTrace;

#ifndef ITF_FINAL
        if ( !m_userFriendly.isEmpty() )
        {
            debugTrace = m_userFriendly;
        }
        else if ( !m_templatePath.isEmpty() )
        {
            m_templatePath.toString8(debugTrace);
            debugTrace += "_frise";
        }
        else
        {
            debugTrace = "";
        }
#endif

        pPoly->m_physBody = PHYSWORLD->allocBody(pPoly->m_physShape, bodyInit, debugTrace);
    }
#endif

    void Frise::buildWorldCollision( u32 _collisionCount )
    {        
        ITF_ASSERT( m_pCollisionData->m_worldCollisionList.size() == 0 );
        ITF_ASSERT( _collisionCount );

        m_pCollisionData->m_worldCollisionList.reserve( _collisionCount );

#ifdef USE_BOX2D
        if ( !m_pRecomputeData->m_isPreComputingForCook )
            initPhys2d();
#endif

        for ( u32 i = 0; i < _collisionCount; i++ )
        {
            PolyLine* pPol = newAlloc(mId_Frieze,PolyLine());
            
			pPol->m_fluidEmitterFilter = m_fluidEmitterFilter;
            pPol->setDepth(getDepth());
            pPol->setPointsList( m_pCollisionData->m_localCollisionList[i] );

            if ( !m_pRecomputeData->m_isPreComputingForCook )
            {
#ifdef USE_PHYSWORLD
                initCollisionWorld( pPol );
#endif
#ifdef USE_BOX2D
                // box 2d collisions
                addPhys2dCollision( pPol );
#endif
            }            

            m_pCollisionData->m_worldCollisionList.push_back( pPol );            
        }

        ITF_ASSERT( m_pCollisionData->m_localCollisionList.size() == m_pCollisionData->m_worldCollisionList.size() );

        if ( isActive() && !m_disablePhysic )
        {
            registerPhysic();            
        }
    }

#ifdef USE_PHYSWORLD
    void Frise::initCollisionWorld( PolyLine* _pPol )
    {
        _pPol->m_ownerFrise = const_cast<Frise*>(this);    
        _pPol->setDepth(getDepth());
        setWorldCollisionPhysBody( _pPol );

    }

    void Frise::deletePolylineCollisionAt( u32 _index )
    {
        PolyLine* poly = m_pCollisionData->m_worldCollisionList[_index];
        PHYSWORLD->deallocBody(poly->m_physBody);
        poly->m_physBody = NULL;
        SF_DEL(poly->m_physShape);
        SF_DEL(poly);
    }
#endif

    void Frise::initCollisionData( u32 _collisionWantedCount )
    {
        ITF_ASSERT( _collisionWantedCount );        

        createCollisionData();

        m_pCollisionData->m_localCollisionList.resize( _collisionWantedCount );
    }

    void Frise::clearCollisionData()
    {        
        if ( m_pCollisionData )
        {
            ITF_ASSERT(!m_physicRegistered);

#ifdef USE_PHYSWORLD

            const u32 worldCollisionCount = m_pCollisionData->m_worldCollisionList.size();
            for( u32 i = 0; i < worldCollisionCount; i++)
            {        
                deletePolylineCollisionAt( i );
            }

#endif
            deleteCollisionData();
        }
    }

    bbool Frise::addPoinLastToCloseCollisionDouble( const collisionRun& _colRun, const PolyPointList& _pointList ) const
    {
        return _colRun.m_collisionDouble==DoubleAndLoop && _pointList.getPosCount() >0;
    }

    void Frise::setCollisionExtremityGameMaterial( const collisionRun& _colRun, PolyPointList& _pointList, bbool _startExtremity) const
    {
        if ( _colRun.m_collisionDouble == DoubleAndLoop )
        {
            const FriseConfig* config = getConfig();

            if ( config )
            {
                Path gMat;

                if ( _startExtremity )
                    gMat = config->m_gameMaterialStartExtremity;
                else
                    gMat = config->m_gameMaterialStopExtremity;

                if ( !gMat.isEmpty() )
                {
                    _pointList.setGameMaterialAt( gMat.getStringID(), _pointList.getPosCount() -1 );
                }                
            }
        }
    }

    void Frise::addPointsCollision( const FriseConfig* _config, SafeArray<Vec2d>& _collision, ITF_VECTOR<CollisionByGameMat>& _collisionList, const collisionRun& _colRun )
    {                
        if ( _collision.size() < 2 )
            return;

        Vec2d posLast(Vec2d::Infinity);
        Vec2d posLastOld(-Vec2d::Infinity);
        i32 collisionListCount = _collisionList.size(); 

        PolyPointList& pointList = m_pCollisionData->m_localCollisionList[_colRun.m_index];
        u32 indexPoint = pointList.getPosCount();

        if ( indexPoint > 0 )
        {
            posLast = pointList.getPosAt(indexPoint -1);

            if ( indexPoint > 1 )
            {
                posLastOld = pointList.getPosAt(indexPoint -2);
            }

            setCollisionExtremityGameMaterial( _colRun, pointList, bfalse);
        }

        const bbool flipWanted = _colRun.m_collisionDouble != None ? !_config->m_collisionFrieze.m_isFlip : _config->m_collisionFrieze.m_isFlip;

        if ( flipWanted )
        {
            for ( i32 idCol = collisionListCount -1; idCol >= 0; idCol -- )
            {
                const CollisionByGameMat& collisionCur = _collisionList[idCol];

                if ( !collisionCur.m_gameMat.isEmpty() )
                {
                    for ( i32 idPoint = collisionCur.m_stopId -1; idPoint >= collisionCur.m_startId; idPoint-- )
                    {
                        AddPointCollisionWithGMat( _config, pointList, _collision[idPoint], posLast, posLastOld, indexPoint, collisionCur.m_gameMat );
                    }

                    if ( addPoinLastToCloseCollisionDouble(_colRun, pointList) )
                    {
                        setCollisionExtremityGameMaterial( _colRun, pointList, btrue);

                        Vec2d pointFirst = pointList.getPosAt(0);
                        AddPointCollisionWithGMat( _config, pointList, pointFirst, posLast, posLastOld, indexPoint, collisionCur.m_gameMat );
                    }
                }
                else
                {
                    for ( i32 idPoint = collisionCur.m_stopId -1; idPoint >= collisionCur.m_startId; idPoint-- )
                    {
                        AddPointCollision( _config, pointList, _collision[idPoint], posLast, posLastOld, indexPoint, collisionCur.m_gameMat );
                    }

                    if ( addPoinLastToCloseCollisionDouble(_colRun, pointList) )
                    {
                        setCollisionExtremityGameMaterial( _colRun, pointList, btrue);

                        Vec2d pointFirst = pointList.getPosAt(0);
                        AddPointCollision( _config, pointList, pointFirst, posLast, posLastOld, indexPoint, collisionCur.m_gameMat );
                    }
                }
            }
        }
        else
        {
            for ( i32 idCol = 0; idCol < collisionListCount; idCol ++ )
            {
                const CollisionByGameMat& collisionCur = _collisionList[idCol];

                if ( !collisionCur.m_gameMat.isEmpty() )
                {
                    for ( i32 idPoint = collisionCur.m_startId; idPoint < collisionCur.m_stopId; idPoint++ )
                    {
                        AddPointCollisionWithGMat( _config, pointList, _collision[idPoint], posLast, posLastOld, indexPoint, collisionCur.m_gameMat );
                    }

                    if ( addPoinLastToCloseCollisionDouble(_colRun, pointList) )
                    {
                        setCollisionExtremityGameMaterial( _colRun, pointList, btrue);

                        const Vec2d pointFirst = pointList.getPosAt(0);
                        AddPointCollisionWithGMat( _config, pointList, pointFirst, posLast, posLastOld, indexPoint, collisionCur.m_gameMat );
                    }
                }
                else
                {
                    for ( i32 idPoint = collisionCur.m_startId; idPoint < collisionCur.m_stopId; idPoint++ )
                    {
                        AddPointCollision( _config, pointList, _collision[idPoint], posLast, posLastOld, indexPoint, collisionCur.m_gameMat );
                    }

                    if ( addPoinLastToCloseCollisionDouble(_colRun, pointList) )
                    {
                        setCollisionExtremityGameMaterial( _colRun, pointList, btrue);

                        const Vec2d pointFirst = pointList.getPosAt(0);
                        AddPointCollision( _config, pointList, pointFirst, posLast, posLastOld, indexPoint, collisionCur.m_gameMat );
                    }
                }
            }                                                
        }
    }

    bbool Frise::isParallelEdges( const Vec2d& _posCur, const Vec2d& _posLast, const Vec2d& _posLastOld, f32 _distMax) const
    {
        const Vec2d sight = (_posCur -_posLastOld).getPerpendicular().normalize();
        const f32 dist = f32_Abs(sight.dot(_posLast -_posLastOld));

        if ( dist > _distMax )
            return bfalse;

        return btrue;
    }

    bbool Frise::skipEdgeParallelInCollisionData( const FriseConfig* _config, PolyPointList& _pointList, const Vec2d& _posCur, Vec2d& _posLast, Vec2d& _posLastOld, u32 _indexPoint, const Path& _gameMat ) const
    {
        if ( _indexPoint < 2 )
            return bfalse;

        if ( !isParallelEdges( _posCur, _posLast, _posLastOld, _config->m_collisionFrieze.m_distMaxToSkipParallelEdge ) )
            return bfalse;

        // if parallel edges: posCur isn't added --> move last point added on posCur
        _pointList.setPosAt( _posCur, _indexPoint -1 );

        // set GameMaterial
        _pointList.setGameMaterialAt( _gameMat.getStringID(), _indexPoint -1 );

        // only update last position 
        _posLast = _posCur;

        return btrue;
    }

    bbool Frise::AddPointCollision( const FriseConfig* _config, PolyPointList& _pointList, const Vec2d& _posCur, Vec2d& _posLast, Vec2d& _posLastOld, u32& _indexPoint, const Path& _gameMat )
    {
        if ( ( _posCur -_posLast ).sqrnorm() < deltaConnectionSqr )
            return bfalse;

#ifndef ITF_PROJECT_POP
        // In order to add point for the pathfind.
        if ( skipEdgeParallelInCollisionData( _config, _pointList, _posCur, _posLast, _posLastOld, _indexPoint, _gameMat ) )
            return bfalse;
#endif // ITF_PROJECT_POP

        // add point
        _pointList.addPoint( _posCur );
        _indexPoint++; 

        // update last positions
        _posLastOld = _posLast;
        _posLast = _posCur;

        return btrue;
    }

    void Frise::AddPointCollisionWithGMat( const FriseConfig* _config, PolyPointList& _pointList, const Vec2d& _posCur, Vec2d& _posLast, Vec2d& _posLastOld, u32& _indexPoint, const Path& _gameMat )
    {
        if ( AddPointCollision( _config, _pointList, _posCur, _posLast, _posLastOld, _indexPoint, _gameMat ) )
        {
            _pointList.setGameMaterialAt( _gameMat.getStringID(), _indexPoint -1 );
        }
    }

    void Frise::finalizeCollisionList()
    {
        if ( !m_pCollisionData )
            return;

        for ( ITF_VECTOR<PolyPointList>::iterator it = m_pCollisionData->m_localCollisionList.begin(); it != m_pCollisionData->m_localCollisionList.end(); )
        {
            PolyPointList& pointsList = *it;
            u32 posCount = pointsList.getPosCount();
            
            f32 epsilon = MTH_EPSILON * 10;
            if ( posCount > 1 ) // polyline requires 2 points minimum
            {     
                // remove unecessary points
                bbool repeat = btrue;
                while (repeat)
                {
                    repeat = bfalse;
                    for (u32 i=2; i<posCount; i++)
                    {
                        const Vec2d & p0 = pointsList.getPosAt(i-2);
                        const Vec2d & p1 = pointsList.getPosAt(i-1);
                        const Vec2d & p2 = pointsList.getPosAt(i  );

                        if (f32_Abs((p1 - p0).cross(p2 - p0)) < epsilon &&
                            (p1 - p0).dot(p2 - p1) >= 0.f)
                        {
                            pointsList.erasePosAt(i-1);
                            posCount  = pointsList.getPosCount();
                            repeat    = btrue;
                            break;
                        }
                    }
                }
                
                // loop
                if ( pointsList.getPosAt(0).IsEqual( pointsList.getPosAt( posCount -1 ), 0.1f ) )
                {
                    if ( posCount > 2 ) // looping polyline requires 3 points minimum
                    {
                        pointsList.forceLoop(btrue);
                        ++it;
                    }
                    else
                    {
                        it = m_pCollisionData->m_localCollisionList.erase(it);                                 
                    }  
                }
                else
                {
                    ++it;
                }                                
            }
            else
            {
                it = m_pCollisionData->m_localCollisionList.erase(it);
            }
        }

        const u32 collisionListCount = m_pCollisionData->m_localCollisionList.size();

        if ( collisionListCount == 0 )
        {
            clearCollisionData();
        }
        else
        {
            buildWorldCollision( collisionListCount );
        }        
    }

    bbool Frise::isEdgeRunCollisionable( const FriseConfig* _config, const edgeRun& _edgeRun, const edgeFrieze& _edge ) const
    {
        if ( _edgeRun.m_idTex == -1 )
            return _config->m_collisionFrieze.m_build;

        switch ( _config->m_methode )
        {
        case InFluid:
        case InString:
            return _config->m_textureConfigs[_edgeRun.m_idTex].m_collisionTex.m_build && isEdgeWithHoleVisual( _edge );

        default:
            return _config->m_textureConfigs[_edgeRun.m_idTex].m_collisionTex.m_build != 0;
        }        
    }

    void Frise::getIdEdgeRunStartCollision( const FriseConfig* _friseConfig, ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, u32& _idEdgeRunStart, u32& _edgeCountStart ) const
    {
        if ( !m_pointsList.isLooping() )
            return;

        u32 edgeRunListCount = _edgeRunList.size();
        edgeFrieze& startEdge = _edgeList[_edgeRunList[_idEdgeRunStart].m_idEdgeStart];        

        if ( !isEdgeRunCollisionable( _friseConfig, _edgeRunList[_idEdgeRunStart], startEdge ) || isEdgeRunWithCollisionHole( _edgeList, _edgeRunList[_idEdgeRunStart], _idEdgeRunStart, _edgeCountStart, edgeRunListCount ) )
            return;

        u32 searchId = _edgeRunList.size() -1;

        while ( searchId > 0 && isEdgeRunCollisionable( _friseConfig, _edgeRunList[searchId], _edgeList[_edgeRunList[searchId].m_idEdgeStart] ) )
        {            
            _idEdgeRunStart = searchId; 

            if ( isEdgeRunWithCollisionHole( _edgeList, _edgeRunList[_idEdgeRunStart], _idEdgeRunStart, _edgeCountStart, edgeRunListCount ) )
                return;

            searchId--;
        }
    }

    bbool Frise::isEdgeRunWithCollisionHole( ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun, u32& _idEdgeRunStart, u32& _edgeCountStart, u32 _edgeRunListCount ) const
    {
        ITF_ASSERT( _edgeRun.m_edgeCount > 0 );

        const i32 countStop = _edgeRun.m_edgeCount -1;
        for ( i32 count = countStop; count >= 0 ; count -- )
        {
            const u32 idEdge = ( _edgeRun.m_idEdgeStart + count ) % m_pRecomputeData->m_edgeListCount;

            if ( isEdgeWithHoleCollision( _edgeList[idEdge] ) )
            {
                if ( count == countStop )
                    _idEdgeRunStart = ( _idEdgeRunStart +1 ) % _edgeRunListCount;
                else
                    _edgeCountStart = u32(count +1);

                return btrue;
            }
        }

        return bfalse;
    }

    void Frise::initCollisionRun( const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, collisionRun& _collisionRun, u32 _idEdgeRunStart, u32 _idEdgeStart, u32 _edgeCountStartOffset ) const
    {
        _collisionRun.m_idEdgeRunStart = _idEdgeRunStart;
        _collisionRun.m_edgeRunCount = 1;
        _collisionRun.m_idEdgeStart = _idEdgeStart;
        _collisionRun.m_edgeCount = 1;
        _collisionRun.m_edgeCountStartOffset = _edgeCountStartOffset;
        _collisionRun.m_collisionOffsetLast = -1.f;
        _collisionRun.m_collisionOffsetNext = -1.f;

        // collision offset with edgeRunLast
        if ( _edgeCountStartOffset != 0 )
            return;

        if ( !m_pointsList.isLooping() && _idEdgeRunStart == 0 )
            return;

        u32 edgeRunListCount = _edgeRunList.size();
        u32 idEdgeRunLast = ( _idEdgeRunStart +edgeRunListCount -1 ) % edgeRunListCount;

        _collisionRun.m_collisionOffsetLast = getCollisionOffSet( _friseConfig, &_edgeRunList[idEdgeRunLast], _collisionRun );
    }

    bbool Frise::checkStartCollisionRun( const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, collisionRun& _collisionRun, bbool& _build, u32& _idEdgeRunOld, u32 _idEdgeRun, u32 _idEdge, u32 _edgeCountStartOffset ) const
    {
        if ( _build )
        {
            _collisionRun.m_edgeCount++;

            if ( _idEdgeRunOld != _idEdgeRun )
            {
                _idEdgeRunOld = _idEdgeRun;
                _collisionRun.m_edgeRunCount++;
            }

            return bfalse;
        } 
        else
        {
            _build = btrue;
            _idEdgeRunOld = _idEdgeRun;

            initCollisionRun( _friseConfig, _edgeRunList, _collisionRun, _idEdgeRun, _idEdge, _edgeCountStartOffset );

            return btrue;
        }
    }

    void Frise::finalizeCollisionRun( const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, collisionRun& _collisionRun, u32 _edgeCountStopOffset, u32 _idEdgeRun ) const
    {
        _collisionRun.m_edgeCountStopOffset = _edgeCountStopOffset;

        // collision offset with edgeRunLast
        if ( _edgeCountStopOffset != 0 )
            return;

        u32 edgeRunListCount = _edgeRunList.size();

        if ( !m_pointsList.isLooping() && _idEdgeRun == edgeRunListCount -1 )
            return;

        u32 idEdgeRunNext = ( _idEdgeRun +1 ) % edgeRunListCount;

        _collisionRun.m_collisionOffsetNext = getCollisionOffSet( _friseConfig, &_edgeRunList[idEdgeRunNext], _collisionRun );
    }

    bbool Frise::checkStopCollisionRun( const FriseConfig* _friseConfig, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList, collisionRun& _collisionRun, bbool& _build, u32 _edgeCountStopOffset, u32 _idEdgeRun )
    {
        if ( _build )
        {
            _build = bfalse;
            _collisionRun.m_index = _collisionRunList.size();

            finalizeCollisionRun( _friseConfig, _edgeRunList, _collisionRun, _edgeCountStopOffset, _idEdgeRun );
            _collisionRunList.push_back( _collisionRun );

            return btrue;
        }

        return bfalse;
    }

    bbool Frise::buildCollisionRunList( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {
        const u32 edgeRunListCount = _edgeRunList.size();
        const FriseConfig* pConfig = getConfig();

        if ( !pConfig->m_collisionFrieze.m_build || edgeRunListCount == 0 )
        {
            clearCollisionData();
            return bfalse;
        }

        u32 idEdgeRunStart = 0;
        u32 edgeCountStart = 0;        
        getIdEdgeRunStartCollision( pConfig, _edgeList, _edgeRunList, idEdgeRunStart, edgeCountStart );

        u32 edgeCountStop = edgeCountStart;       
        bbool buildCollisionRun = bfalse;
        collisionRun collisionRunCur;
        u32 idEdgeRunOld = 0;

        for ( u32 edgeRunCount = 0; edgeRunCount < edgeRunListCount; edgeRunCount++ )
        {
            u32 idEdgeRunCur = ( idEdgeRunStart + edgeRunCount ) % edgeRunListCount;
            edgeRun& edgeRunCur = _edgeRunList[idEdgeRunCur];

            if ( isEdgeRunCollisionable( pConfig, edgeRunCur, _edgeList[edgeRunCur.m_idEdgeStart] ) )
            {         
                for ( u32 edgeCount = edgeCountStart; edgeCount < edgeRunCur.m_edgeCount; edgeCount++ )
                {
                    const u32 idEdgeCur = ( edgeRunCur.m_idEdgeStart + edgeCount ) % m_pRecomputeData->m_edgeListCount;

                    if ( isEdgeWithHoleCollision( _edgeList[idEdgeCur] ) )
                    {
                        if ( edgeCount == 0 )
                            checkStopCollisionRun( pConfig, _edgeRunList, _collisionRunList, collisionRunCur, buildCollisionRun, 0, idEdgeRunOld );
                        else
                            checkStopCollisionRun( pConfig, _edgeRunList, _collisionRunList, collisionRunCur, buildCollisionRun, edgeRunCur.m_edgeCount -edgeCount, idEdgeRunOld );
                    }
                    else
                        checkStartCollisionRun( pConfig, _edgeRunList, collisionRunCur, buildCollisionRun, idEdgeRunOld, idEdgeRunCur, idEdgeCur, edgeCount );
                }               
            }
            else
                checkStopCollisionRun( pConfig, _edgeRunList, _collisionRunList, collisionRunCur, buildCollisionRun, 0, idEdgeRunOld );

            edgeCountStart = 0;
        }

        // check startEdgeRun
        const edgeRun& edgeRunCur = _edgeRunList[idEdgeRunStart];
        ITF_ASSERT ( edgeCountStop <= edgeRunCur.m_edgeCount);
        if ( isEdgeRunCollisionable( pConfig, edgeRunCur, _edgeList[edgeRunCur.m_idEdgeStart] ) )
        {                     
            for ( u32 edgeCount = 0; edgeCount < edgeCountStop; edgeCount++ )
            {
                const u32 idEdgeCur = ( edgeRunCur.m_idEdgeStart + edgeCount ) % m_pRecomputeData->m_edgeListCount;

                if ( isEdgeWithHoleCollision( _edgeList[idEdgeCur] ) )
                {
                    if ( edgeCount == 0 )
                        checkStopCollisionRun( pConfig, _edgeRunList, _collisionRunList, collisionRunCur, buildCollisionRun, 0, idEdgeRunOld );
                    else
                        checkStopCollisionRun( pConfig, _edgeRunList, _collisionRunList, collisionRunCur, buildCollisionRun, edgeRunCur.m_edgeCount -edgeCount, idEdgeRunOld);
                }
                else
                    checkStartCollisionRun( pConfig, _edgeRunList, collisionRunCur, buildCollisionRun, idEdgeRunOld, idEdgeRunStart, idEdgeCur, edgeCount );
            }               
        }

        checkStopCollisionRun( pConfig, _edgeRunList, _collisionRunList, collisionRunCur, buildCollisionRun, 0, idEdgeRunOld );

        u32 collisionRunListCount = _collisionRunList.size();
        if ( collisionRunListCount == 0 )
        {
            clearCollisionData();
            return bfalse;
        }

        if ( pConfig->m_collisionFrieze.m_methode )
        {
            collisionRunListCount = buildCollisionRunListDouble( _collisionRunList, pConfig );            
        }

        initCollisionData( collisionRunListCount );

        verifyCollision( _edgeRunList, _collisionRunList );

        return btrue;
    }

    void Frise::verifyCollision( ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList ) const
    {
        if ( !m_pointsList.isLooping() )
            return;

        if ( _edgeRunList.size() > 1 )
            return;

        u32 collisionRunListCount = _collisionRunList.size();
        for ( u32 i = 0; i < collisionRunListCount; i++ )
        {
            ITF_ASSERT(m_pRecomputeData->m_edgeListCount >= _collisionRunList[i].m_edgeCountStartOffset);

            const u32 countOffset = m_pRecomputeData->m_edgeListCount -_collisionRunList[i].m_edgeCountStartOffset;

            if ( countOffset < _collisionRunList[i].m_edgeCountStopOffset )
            {                    
                // update collisionRunList offset                              
                for ( u32 j = 0; j < collisionRunListCount; j++ )
                {
                    collisionRun& colRun = _collisionRunList[j];

                    colRun.m_edgeCountStartOffset = ( colRun.m_edgeCountStartOffset + countOffset ) % m_pRecomputeData->m_edgeListCount;                        
                    colRun.m_edgeCountStopOffset  = ( colRun.m_edgeCountStopOffset - countOffset + m_pRecomputeData->m_edgeListCount ) % m_pRecomputeData->m_edgeListCount;  
                }

                // update edgeRun
                edgeRun& edgeRunCur = _edgeRunList[0];
                edgeRunCur.m_idEdgeStart = ( edgeRunCur.m_idEdgeStart -countOffset + m_pRecomputeData->m_edgeListCount ) % m_pRecomputeData->m_edgeListCount;

                return;
            }
        }
    }

    const Path* Frise::getCollisionGameMaterial( const FriseConfig* _friseConfig, i32 _idTex ) const
    {
        // GMat from texture config
        if ( _idTex > -1 && !_friseConfig->m_textureConfigs[_idTex].m_gameMaterial.isEmpty() )
        {
            return &_friseConfig->m_textureConfigs[_idTex].m_gameMaterial;
        }

        // GMat from frieze config
        if ( !_friseConfig->m_gameMaterial.isEmpty() )
        {
            return &_friseConfig->m_gameMaterial;
        }

        return NULL;
    }

    f32 Frise::getCollisionOffSetSimple( const FriseConfig* config, const edgeRun* _edgeRun ) const
    {
        f32 offsetCur;

        if ( _edgeRun->m_idTex == -1 )
            offsetCur = config->m_collisionFrieze.m_offset;
        else
            offsetCur = config->m_textureConfigs[_edgeRun->m_idTex].m_collisionTex.m_offset;

        offsetCur = Clamp( offsetCur, 0.f, 1.f );

        return offsetCur;
    }

    f32 Frise::getCollisionOffSet( const FriseConfig* config, const edgeRun* _edgeRun, collisionRun& _colRun ) const
    {
        f32 offsetCur = getCollisionOffSetSimple( config, _edgeRun );

        if ( _colRun.m_collisionDouble != None )
            offsetCur = 1.f -offsetCur;

        return offsetCur;
    }

    void Frise::buildCollision_EdgeRunList( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
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

            ITF_VECTOR<CollisionByGameMat> collisionList;

            const Path* gameMat = getCollisionGameMaterial( config, edgeRunCur->m_idTex );
            
            CollisionByGameMat collisionByGMat;

            collisionByGMat.m_startId = 0;

            if ( gameMat )
            {
                collisionByGMat.m_gameMat = *gameMat;
            }

            // countOffsets
            u32 edgeCountStartOffset = colRun.m_edgeCountStartOffset;

            // build start points   
            if ( colRun.m_collisionOffsetLast != -1.f && colRun.m_collisionOffsetLast != offsetCur )
            {
                buildOutline_CornerSquareBlending_startEdgeRun( collision, _edgeList, colRun.m_idEdgeStart, offsetCur, colRun.m_collisionOffsetLast );
            }
            else
            {
                buildCollision_CornerSquare_StartEdgeRun( config, _edgeList, collision, &_edgeList[colRun.m_idEdgeStart], colRun.m_idEdgeStart, offsetCur, colRun );
            }

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
                    buildOutline_StartEdgeRunCornerSquare( config, collision,_edgeList, edgeRunLast, edgeRunCur, offsetCur, offsetLast);

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
                u32 edgeCountStopOffset = 0;
                if ( edgeRunCount == colRun.m_edgeRunCount -1)
                    edgeCountStopOffset = colRun.m_edgeCountStopOffset;

                if ( config->m_collisionFrieze.m_isSmooth )
                    buildOutline_EdgeRunWithCornerRounder( config, collision, _edgeList, edgeRunCur, offsetCur, config->m_collisionFrieze.m_smoothFactor, edgeCountStartOffset, edgeCountStopOffset );
                else
                    buildOutline_EdgeRunWithCornerSquare( config, collision, _edgeList, edgeRunCur, offsetCur, edgeCountStartOffset, edgeCountStopOffset );

                edgeRunLast = edgeRunCur;
                offsetLast = offsetCur;
                edgeCountStartOffset = 0;
            }

            // build stop points          
            const u32 idStopEdge = ( colRun.m_idEdgeStart + colRun.m_edgeCount + m_pRecomputeData->m_edgeListCount -1 ) % m_pRecomputeData->m_edgeListCount;
            if ( colRun.m_collisionOffsetNext != -1.f && colRun.m_collisionOffsetNext != offsetCur )
                buildOutline_CornerSquareBlending_stopEdgeRun( collision, _edgeList, idStopEdge, offsetCur, colRun.m_collisionOffsetNext );
            else
            {          
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

    void fillCheckConnectionList( const ITF_VECTOR<Frise*>& _friezeList, SafeArray<PolyLine*>& _pPolylineList )
    { 
        const u32 friezeListCount = _friezeList.size();
        for ( u32 idFrieze = 0; idFrieze < friezeListCount; idFrieze++)
        {
            const Frise* pFrieze = _friezeList[idFrieze];

            if ( const ITF_VECTOR<PolyLine*>* pCollisionList = pFrieze->getCollisionData_WorldSpace() )
            {
                const u32 collisionCount = pCollisionList->size();
                for ( u32 idPol = 0; idPol < collisionCount; idPol++ )
                {
                    PolyLine* pPoly = (*pCollisionList)[idPol];

                    if ( !pPoly->isLooping() )
                    {
                        if ( !pPoly->getPrevious() || !pPoly->getNext() )
                        {
                            _pPolylineList.push_back(pPoly);
                        }                    
                    }
                }
            }
        }
    }    

    // check connections only for Previous Polyline
    void checkPreviousConnection( PolyLine* _pPolCur, PolyLine* _pPolPrevious, u32 _previousId )
    {
        const Vec2d& connection = _pPolPrevious->getPosAt( _pPolPrevious->getPosCount() -1 );

        if ( _pPolCur->getPosAt(0).IsEqual( connection, deltaConnection ) )
        {
            // test length before connect
            if ( (_pPolCur->getPosAt(1) - connection).sqrnorm() >= deltaConnectionSqr )
            {
                _pPolCur->getConnection().setPrevious( _pPolPrevious, connection );
                _pPolCur->getConnection().setPreviousIdFromDataCooked( _previousId );
            }
        }
    }

    void Frise::checkConnectionsDataCooked( const ITF_VECTOR<Frise*>& _friezeList )
    {
        SafeArray<PolyLine*> pCheckConnectionList;
        fillCheckConnectionList( _friezeList, pCheckConnectionList );

        for ( u32 idPol = 0; idPol < pCheckConnectionList.size(); idPol++ )
        {
            PolyLine* pPolCur = pCheckConnectionList[ idPol ];

#ifdef USE_PHYSWORLD
            ITF_ASSERT_CRASH( pPolCur->m_ownerFrise->isCooked(), "Owner frieze is not cooked");
#endif
            
            
            if ( pPolCur->getConnection().isPreviousIdValid() )
            {
                ITF_ASSERT_CRASH( pPolCur->getConnection().getPreviousId() < pCheckConnectionList.size(), "Invalid previous id");

                PolyLine* pPolPrevious = pCheckConnectionList[ pPolCur->getConnection().getPreviousId() ];

                // connection still done
                ITF_ASSERT_CRASH( !pPolCur->getConnection().getPrevious(), "Connection still done");
                ITF_ASSERT_CRASH( !pPolPrevious->getConnection().getNext(), "Connection still done");

                // depth range
                ITF_ASSERT_CRASH( DepthRange( pPolCur->getDepth() ) == DepthRange( pPolPrevious->getDepth() ), "Depth range invalid"); 
                 
                // check delta position still valid, if not means frieze has moved, connection will be done on loading
                if ( pPolCur->getPosAt(0).IsEqual( pPolPrevious->getPosAt( pPolPrevious->getPosCount() -1 ), deltaConnection ) )
                {
                    // connection still done and serialized, just set previous without connect points
                    pPolCur->getConnection().setPrevious_DontConnectPoints( pPolPrevious );
                }
            }
        }        
    }

    void Frise::checkConnections( const ITF_VECTOR<Frise*>& _friezeList )
    {
        SafeArray<PolyLine*> pCheckConnectionList;
        fillCheckConnectionList( _friezeList, pCheckConnectionList );

        const Vec2d offsetAABB( deltaConnection, deltaConnection);

        for ( u32 idPolRef = 0; idPolRef < pCheckConnectionList.size(); idPolRef++ )
        {
            PolyLine* pPolRef = pCheckConnectionList[idPolRef];

            // connection still done
            if ( pPolRef->getPrevious() && pPolRef->getNext() )
                continue;

            const AABB polRefAABB( pPolRef->getAABB().getMin() -offsetAABB, pPolRef->getAABB().getMax() +offsetAABB );
            const DepthRange polRefDepthRange( pPolRef->getDepth() );

            for ( u32 idPolCompare = idPolRef +1; idPolCompare < pCheckConnectionList.size(); idPolCompare++ )
            {
                PolyLine* pPolCompare = pCheckConnectionList[idPolCompare];

                if ( polRefDepthRange == DepthRange( pPolCompare->getDepth() ) )
                {
                    if ( polRefAABB.checkOverlap( pPolCompare->getAABB() ) )
                    {
                        // check previous pol ref
                        if ( !pPolRef->getPrevious() && !pPolCompare->getNext() )
                        {
                            checkPreviousConnection( pPolRef, pPolCompare, idPolCompare );
                        }

                        // check previous pol compare
                        if ( !pPolRef->getNext() && !pPolCompare->getPrevious() )
                        {
                            checkPreviousConnection( pPolCompare, pPolRef, idPolRef );
                        }

                        // connection done for pol ref
                        if ( pPolRef->getPrevious() && pPolRef->getNext() )
                        {                    
                            break;
                        }
                    }
                }
            }
        }
    }

    Vec2d Frise::buildCollision_getCornerExtremityWithCoord( const FriseTextureConfig& _texConfig, collisionRun& _colRun, edgeFrieze* _edgeCur, edgeFrieze* _edgeLast, f32 _offsetCur, f32 _offsetLast ) const
    {        
        if ( _edgeCur->m_cornerAngle > 0.f )
        {
            if ( _texConfig.m_collisionTex.m_cornerOut.x() != F32_INVALID )
            {                
                Vec2d cornerOut(_texConfig.m_collisionTex.m_cornerOut);
                if ( _colRun.m_collisionDouble != None)
                    cornerOut.y() = 1.f -cornerOut.y();

                Vec2d posSquare = getPosOnCornerEdge( _edgeCur, cornerOut.y());

                if ( _texConfig.m_collisionTex.m_cornerOutCur )
                    posSquare += _edgeCur->m_normal *cornerOut.x() *_edgeCur->m_heightStart;
                else
                    posSquare += _edgeLast->m_normal *cornerOut.x() *_edgeLast->m_heightStop;

                return posSquare;
            }
        }
        else
        {
            if ( _texConfig.m_collisionTex.m_cornerIn.x() != F32_INVALID )
            {
                Vec2d cornerIn(_texConfig.m_collisionTex.m_cornerIn);
                if ( _colRun.m_collisionDouble != None )
                    cornerIn.y() = 1.f -cornerIn.y();

                Vec2d posSquare = getPosOnCornerEdge( _edgeCur, 1.f -cornerIn.y());

                if ( _texConfig.m_collisionTex.m_cornerInCur )
                    posSquare -= _edgeCur->m_normal *cornerIn.x() *_edgeCur->m_heightStart;
                else
                    posSquare -= _edgeLast->m_normal *cornerIn.x() *_edgeLast->m_heightStop;

                return posSquare;
            }
        }

        if ( _offsetCur == _offsetLast || _offsetLast == -1.f )
            return getPosOnCornerEdge( _edgeCur, _offsetCur );

        Vec2d posStart, posStop, posSquare;
        setPos_CornerSquareBlending( posStart, posStop, posSquare, _edgeCur, _edgeLast, _offsetCur, _offsetLast );
        
        return posSquare;
    }

    void Frise::buildCollision_ExtremityWithCoord( const FriseConfig* config, SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun* _edgeRunCur, collisionRun& _colRun, f32 _offsetCur, f32 _offsetLast, u32 _startEdgeRun ) const
    {     
        // edge cur
        edgeFrieze* edgeCur = &_edgeList[_edgeRunCur->m_idEdgeStart];

        // edge last
        const u32 idEdgeLast = ( _edgeRunCur->m_idEdgeStart + m_pRecomputeData->m_edgeListCount -1) % m_pRecomputeData->m_edgeListCount;
        edgeFrieze* edgeLast = &_edgeList[idEdgeLast];

        const FriseTextureConfig& texConfig = config->m_textureConfigs[_edgeRunCur->m_idTex];   

        Vec2d posStart = getPosStopOnNormalEdge( edgeLast, _offsetLast);
        Vec2d posStop = getPosStartOnNormalEdge( edgeCur, _offsetCur);
        Vec2d posSquare = buildCollision_getCornerExtremityWithCoord( texConfig, _colRun, edgeCur, edgeLast, _offsetCur, _offsetLast);

        if ( _startEdgeRun == 1 )
        {
            _outline.push_back( posSquare );
            _outline.push_back( posStop );
        }
        else if ( _startEdgeRun == 0 )
        {
            _outline.push_back( posStart );
            _outline.push_back( posSquare );
        }
        else
        {
            _outline.push_back( posStart );
            _outline.push_back( posSquare );
            _outline.push_back( posStop );
        }
    }

    void Frise::buildCollision_CornerSquare_StartEdgeRun( const FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList, SafeArray<Vec2d>& _outline, edgeFrieze* _edgeCur, u32 _idEdgeStart, f32 _offset, collisionRun& _collisionRun ) const
    {     
        // hole collisions
        if ( _idEdgeStart >0 || isLooping() )
        {
            const u32 idEdgeLast = ( _idEdgeStart + m_pRecomputeData->m_edgeListCount -1) % m_pRecomputeData->m_edgeListCount;
            const edgeFrieze& edgeLast = _edgeList[idEdgeLast];

            if ( isEdgeWithHoleCollision(edgeLast))
            {
                buildOutline_PosStartOnEdgeWithHoleCollision( _config, _outline, *_edgeCur, _offset);
                return;
            }
        }

        if ( _edgeCur->m_cornerAngle == 0.f )
        {          
            // collision 1
            if ( _config->m_collisionFrieze.m_extremity.x() != F32_INFINITY )
            {
                Vec2d extremity(_config->m_collisionFrieze.m_extremity);
                if ( _collisionRun.m_collisionDouble != None )
                {
                    extremity.y() = 1.f -extremity.y();
                }
                
                Vec2d col = _edgeCur->m_points[0] -_edgeCur->m_sightNormalized *extremity.x() *_edgeCur->m_heightStart +_edgeCur->m_normal*extremity.y() *_edgeCur->m_heightStart;
                _outline.push_back( col );
            }

            // collision 2
            if ( _config->m_collisionFrieze.m_extremity2.x() != F32_INFINITY )
            {
                Vec2d extremity2(_config->m_collisionFrieze.m_extremity2);
                if ( _collisionRun.m_collisionDouble != None )
                {
                    extremity2.y() = 1.f -extremity2.y();
                }
                
                Vec2d col = _edgeCur->m_points[0] -_edgeCur->m_sightNormalized *extremity2.x() *_edgeCur->m_heightStart +_edgeCur->m_normal*extremity2.y() *_edgeCur->m_heightStart;
                _outline.push_back( col );
            }

            _outline.push_back( getPosStartOnNormalEdge( _edgeCur, _offset ) );

            return;
        }

        _outline.push_back( getPosOnCornerEdge( _edgeCur, _offset) );
        _outline.push_back( getPosStartOnNormalEdge( _edgeCur, _offset) );
    }

    void Frise::buildCollision_CornerSquare_StopEdgeRun( const FriseConfig* _config, SafeArray<Vec2d>& _outline, ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idEdgeStop, f32 _offset, collisionRun& _collisionRun ) const
    {     
        const edgeFrieze* edgeLast = &_edgeList[_idEdgeStop];
        u32 idEdgeNext;

        if ( m_pointsList.isLooping() )
            idEdgeNext = ( _idEdgeStop + 1 ) % m_pRecomputeData->m_edgeListCount;
        else
        {
            if ( _idEdgeStop == m_pRecomputeData->m_edgeListCount -1 )
            {                                    
                // collision 1
                _outline.push_back( getPosStopOnNormalEdge( edgeLast, _offset));

                // collision 2
                if ( _config->m_collisionFrieze.m_extremity2.x() != F32_INFINITY )
                {
                    Vec2d extremity2(_config->m_collisionFrieze.m_extremity2);
                    if ( _collisionRun.m_collisionDouble != None )
                    {
                        extremity2.y() = 1.f -extremity2.y();
                    }
                    
                    Vec2d col = edgeLast->m_points[2] +edgeLast->m_sightNormalized *extremity2.x() *edgeLast->m_heightStop +edgeLast->m_normal*extremity2.y() *edgeLast->m_heightStop;
                    _outline.push_back( col );
                }

                // collision 3
                if ( _config->m_collisionFrieze.m_extremity.x() != F32_INFINITY )
                {
                    Vec2d extremity(_config->m_collisionFrieze.m_extremity);
                    if ( _collisionRun.m_collisionDouble != None )
                    {
                        extremity.y() = 1.f -extremity.y();
                    }
                    
                    Vec2d col = edgeLast->m_points[2] +edgeLast->m_sightNormalized *extremity.x() *edgeLast->m_heightStop +edgeLast->m_normal*extremity.y() *edgeLast->m_heightStop;
                    _outline.push_back( col );
                }

                return;
            }

            idEdgeNext = _idEdgeStop +1;
        }

        const edgeFrieze* edgeCur = &_edgeList[idEdgeNext];

        // hole collision
        if ( isEdgeWithHoleCollision(*edgeCur))
        {
            buildOutline_PosStopOnEdgeWithHoleCollision( _config, _outline, *edgeLast, _offset, edgeCur->m_idPoint);
            return;
        }

        if ( edgeCur->m_cornerAngle == 0.f )
        {
            _outline.push_back( getPosStartOnNormalEdge( edgeCur, _offset));
            return;
        }

        _outline.push_back( getPosStopOnNormalEdge( edgeLast, _offset) );
        _outline.push_back( getPosOnCornerEdge( edgeCur, _offset));
    }

    u32 Frise::buildCollisionRunListDouble( ITF_VECTOR<collisionRun>& _collisionRunList, const FriseConfig* _config ) const
    {
        u32 collisionRunListCount = _collisionRunList.size();

        if ( m_pointsList.isLooping() && collisionRunListCount == 1 && _collisionRunList[0].m_edgeCount == m_pRecomputeData->m_edgeListCount )
        {
            collisionRun collisionRunDouble = _collisionRunList[0];
            collisionRunDouble.m_collisionDouble = DoubleAndLoop;
            collisionRunDouble.m_index = collisionRunListCount;
            setCollisionRunDoubleOffset(collisionRunDouble);

            _collisionRunList.push_back(collisionRunDouble);

            return 2;
        }

        if ( _config->m_collisionFrieze.m_methode == 1 )
        {
            for ( u32 i = 0; i < collisionRunListCount; i++ )
            {
                collisionRun collisionRunDouble = _collisionRunList[i];
                collisionRunDouble.m_collisionDouble = Double;
                collisionRunDouble.m_index = collisionRunListCount +i;
                setCollisionRunDoubleOffset(collisionRunDouble);

                _collisionRunList.push_back(collisionRunDouble);
            }

            return collisionRunListCount *2;
        }

        for ( u32 i = 0; i < collisionRunListCount; i++ )
        {
            collisionRun collisionRunDouble = _collisionRunList[i];
            collisionRunDouble.m_collisionDouble = DoubleAndLoop;
            setCollisionRunDoubleOffset(collisionRunDouble);

            _collisionRunList.push_back(collisionRunDouble);
        }

        return collisionRunListCount;

    }

    void Frise::setCollisionRunDoubleOffset( collisionRun& _collisionRunDouble ) const
    {
        if ( _collisionRunDouble.m_collisionOffsetLast != -1.f )
        {
            _collisionRunDouble.m_collisionOffsetLast = 1.f -_collisionRunDouble.m_collisionOffsetLast;
        }
        if ( _collisionRunDouble.m_collisionOffsetNext != -1.f )
        {
            _collisionRunDouble.m_collisionOffsetNext = 1.f -_collisionRunDouble.m_collisionOffsetNext;
        }
    }

#ifdef USE_BOX2D
    void Frise::initPhys2d()
    {
        if (m_b2dBody && m_b2dWorld)
        {
            m_b2dWorld->DestroyBody(m_b2dBody);
            m_b2dBody  = NULL;
            m_b2dWorld = NULL;
        }

        m_b2dWorld = PHYS2D->getWorld(getScene(), getDepth());
        if (!m_b2dWorld)
            return;

        const FriseConfig* config = getConfig();
        if (!config)
            return;


        b2BodyDef bodyDef;
        bodyDef.type            = (b2BodyType)m_bodyType;
        bodyDef.position.Set(get2DPos().x(), get2DPos().y());
        bodyDef.angle           = getAngle();
        bodyDef.active          = 0;
        bodyDef.linearDamping   = config->m_phys2d.m_linearDamping;
        bodyDef.angularDamping  = config->m_phys2d.m_angularDamping;
        bodyDef.gravityScale    = config->m_phys2d.m_gravityMupliplier;

        bodyDef.userData= this;
        m_b2dBody       = m_b2dWorld->CreateBody(&bodyDef);

        if (bodyDef.type != b2_staticBody)
            getWorldUpdateElement()->setUpdateOnlyOnChange(bfalse);
    }

    void Frise::createChainFixture(b2Vec2 * list, i32 nbElemts, bbool isLoop)
    {
        const FriseConfig* config = getConfig();
        if (!config)
            return;

        b2FixtureDef fixDef;
        
        
        fixDef.density              = config->m_phys2d.m_density;
        fixDef.friction             = config->m_phys2d.m_friction;
        fixDef.restitution          = config->m_phys2d.m_restitution;
        fixDef.isSensor             = config->m_phys2d.m_isSensor != bfalse;
        fixDef.filter.categoryBits  = m_categoryBits;
        fixDef.filter.maskBits      = m_maskBits;

        //b2ChainShape frieze2dShape;
        //if (isLoop)
        //    frieze2dShape.CreateLoop(list, nbElemts);
        //else
        //    frieze2dShape.CreateChain(list, nbElemts);
        //fixDef.shape                = &frieze2dShape;
        //m_b2dBody->CreateFixture(&fixDef);

        for (i32 i=0; i<nbElemts-1; i++)
        {
            b2EdgeShape edge2dShape;
            if ((list[i] - list[i+1]).LengthSquared() < MTH_EPSILON)
				continue;

			edge2dShape.Set(list[i], list[i+1]);
            fixDef.shape = &edge2dShape;
			
			if (nbElemts > 2)
			{
				if ((isLoop && i == 0) || i > 0)
				{
					edge2dShape.m_hasVertex0 = true;
					edge2dShape.m_vertex0 = list[(i + nbElemts - 1) % nbElemts];
				}

				if ((isLoop && (i == nbElemts-2)) || i < (nbElemts-2))
				{
					edge2dShape.m_hasVertex3 = true;
					edge2dShape.m_vertex3 = list[(i + 2) % nbElemts];
				}
			}

            m_b2dBody->CreateFixture(&fixDef);
        }

        if (isLoop && nbElemts > 1 && (list[nbElemts-1]- list[0]).LengthSquared() > MTH_EPSILON)
        {
            b2EdgeShape edge2dShape;
            edge2dShape.Set(list[nbElemts-1], list[0]);
            fixDef.shape = &edge2dShape;

			if (nbElemts > 2)
			{
				edge2dShape.m_hasVertex0 = true;
				edge2dShape.m_vertex0 = list[nbElemts-2];

				edge2dShape.m_hasVertex3 = true;
				edge2dShape.m_vertex3 = list[1];
			}

            m_b2dBody->CreateFixture(&fixDef);
        }

    }

    void Frise::addPhys2dCollision(const PolyPointList & _polyList)
    {
        if (!m_b2dBody)
            return;

        i32 nbPoly = _polyList.getPosCount();
        if (nbPoly <= 1)
            return;

        b2Vec2 * pbList  = newAlloc(mId_Phys, b2Vec2[nbPoly+1]);
        i32      begin   = 0;
        i32      current = 0;
        Vec2d    scale   = getScale();
        if (getIsFlipped())
            scale.x() *= -1.f;

        for (; current<nbPoly; current++)
        {
            Vec2d pos = _polyList.getPosAt(current);
            pos *= scale;

            pbList[current].Set(pos.x(), pos.y());

            if (_polyList.getHoleAt(current) & PolyLineEdge::Hole_Collision)
            {
                if (current > begin)
                {
                    createChainFixture(pbList + begin, current - begin, bfalse);
                }
                begin = current + 1;
            }
        }
        
        if (current > begin)
        {
            if (_polyList.isLooping() && begin == 0)
            {
                createChainFixture(pbList, nbPoly - 1, btrue);
            } else 
            {
                createChainFixture(pbList + begin, current - begin, bfalse);
            }
        }

        SF_DEL_ARRAY(pbList);

        if (isActive())
            m_b2dBody->SetActive(true);
    }

    void Frise::addPhys2dCollision(PolyLine * _poly)
    {
        addPhys2dCollision(_poly->getEdgeData());
    }
#endif

} // namespace ITF


