#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#include "engine/display/Frieze3D/Frieze3DData.h"

#if !defined(ITF_FINAL)
    #ifndef _ITF_WARNINGMANAGER_H_
    #include "core/common/WarningManager.h"
    #endif // _ITF_WARNINGMANAGER_H_
#endif

#include "adapters/gfxadapter_opengles2/gfxadapter_opengles2.h"


namespace ITF
{
   
    void Frise::initDatas( const FriseConfig* _pConfig, i32 _texConfigCount, bbool _isPreComputingForCook )
    {
        SF_DEL(m_meshBuildData);
        clearMeshFluidData();
        clearMeshOverlayData();
        clearMeshAnimData();
        clearMeshStaticData();
        clearVisualData();  
        clearCollisionData();

		SF_DEL(m_pFrieze3DRuntimeData);
		SF_DEL(m_pFrieze3DBuildData);
    
        // settings polyline
        m_pointsList.setGameMaterial( _pConfig->m_gameMaterial.getStringID() );

        // init recompute datas
#ifdef ITF_SUPPORT_EDITOR
        m_quadTab.clear();
#endif //ITF_SUPPORT_EDITOR
        m_pRecomputeData = newAlloc(mId_Frieze, RecomputeData);

        m_pRecomputeData->m_isPreComputingForCook = _isPreComputingForCook;

        //  flip uvs from propterty editor
        if ( m_uvX_flip )
            m_pRecomputeData->m_uvXsign = -1.f;

        m_pRecomputeData->m_isUvFlipY = m_uvY_flip;
                
        //  flip uvs from fcg
        if ( _pConfig->m_isUvFlipX )
            m_pRecomputeData->m_uvXsign *= -1.f;
        if ( _pConfig->m_isUvFlipY )
            m_pRecomputeData->m_isUvFlipY = !m_pRecomputeData->m_isUvFlipY;

        m_pRecomputeData->m_flexibility = _pConfig->m_flexibility;
        m_pRecomputeData->m_heightScale = _pConfig->m_height *m_thickness;
        m_pRecomputeData->m_widthScale = _pConfig->m_width *m_thickness;      
        m_pRecomputeData->m_zVtxDown = _pConfig->m_zVtxExtrudeDown;
        m_pRecomputeData->m_zVtxUp = _pConfig->m_zVtxExtrudeUp;

        for ( i32 i = 0; i < _texConfigCount; i++ )
        {
            // alphaBorder
            u8 alphaTexture = _pConfig->m_textureConfigs[i].m_color.m_rgba.m_a;
            if ( alphaTexture != _pConfig->m_textureConfigs[i].m_alphaBorder && i != _pConfig->m_fill.m_idTex )
            {
                m_pRecomputeData->m_alphaBorderWanted = btrue;
                break;
            }
        }

        // flip frieze without flip uvs 
        if ( _pConfig->m_isUvFlipY || ( getIsFlipped() && !_pConfig->m_useFriezeFlipToFlipUV ) )
        {
            m_pRecomputeData->m_useFlippedUV = btrue;
            m_pRecomputeData->m_uvXsign *= -1.f;
        }        

        m_aabbMinZ = 0.f;
        m_aabbMaxZ = 0.f;

#ifdef ITF_SUPPORT_EDITOR        
        m_isFilling = bfalse;        
        m_useAtlasPolygoneEditor = bfalse;
#endif // ITF_SUPPORT_EDITOR
    }

    void Frise::executeRecomputeLineData()
    {
        ITF_ASSERT(m_querriedRecomputeData);
      
        m_querriedRecomputeData = bfalse;
        checkLoop();
        m_pointsList.executeRecomputeData();
    }

    void Frise::executeRecomputeDataCooked()
    {        
        m_querriedRecomputeData = bfalse;
 
         Matrix44 pMatrixTransfo;
         setLocalToWorldMatrix( &pMatrixTransfo );

        if ( m_pMeshStaticData )
        {
            buildMesh_StaticFinal();
            m_pMeshStaticData->m_mesh.setMatrix(pMatrixTransfo);
        }

        if ( m_pMeshAnimData )
        {
            buildMesh_AnimFinal();
            m_pMeshAnimData->m_mesh.setMatrix(pMatrixTransfo);
        }

        if ( m_pMeshOverlayData )
        {
            buildMesh_OverlayFinal();
            m_pMeshOverlayData->m_mesh.setMatrix(pMatrixTransfo);
        }

        if ( m_pMeshFluidData )
        {
            initDataCooked_InFluid();
        }

        const Transform2d xf( get2DPos(), getAngle(), getScale(), getIsFlipped() );
        updateAABB(xf);

        if ( m_pCollisionData )
        {
            const u32 polCount = m_pCollisionData->m_worldCollisionList.size();

#ifdef USE_BOX2D
            // box 2d collisions
            initPhys2d();
#endif
            for ( u32 i = 0; i < polCount; i++ )
            {
                PolyLine* pPol = m_pCollisionData->m_worldCollisionList[i];
#ifdef USE_PHYSWORLD
                initCollisionWorld( pPol );
#endif

#ifdef USE_BOX2D
                // box 2d collisions
                addPhys2dCollision( m_pCollisionData->m_localCollisionList[i] );
#endif
            }
            initializeMovingPolylinesAfterRecomputeData();
        }           

        
      
#ifdef ITF_GLES2
		// Synchro point on OpenGL : due to bufferized commands
		GFXAdapter_OpenGLES2::waitEndOfProcessing();
#endif
        
        
        SF_DEL(m_meshBuildData);
    }

    void Frise::executeRecomputeData( bbool _isPreComputingForCook /*= bfalse*/  )
    {

        
#ifdef DEVELOPER_JAY_COOK
        m_hasBeenRecomputed = btrue;
#endif // DEVELOPER_JAY_COOK

#ifdef TRACK_PICKABLE_FRIENDLY_NAME
        if(strstr(getUserFriendly().cStr(), TRACK_PICKABLE_FRIENDLY_NAME))
        {
            LOG("[Track pickable activation] Frame: %u Frise::executeRecomputeData()", CURRENTFRAME);
        }
#endif // TRACK_PICKABLE_FRIENDLY_NAME

        ITF_ASSERT(m_querriedRecomputeData);

/*  too much warning is better then missed warnings, RO1 port
#if !defined(ITF_FINAL)
        WarningManager::clearWarningsForObject(this);
#endif*/

        executeRecomputeLineData();
        unregisterPhysic();
        unregisterRegion();

        const FriseConfig* pConfig = getConfig();
        if ( !pConfig )
        {
            clearComputedData();
            updateTransformationFromLocalToWorld();
            return;
        }

#ifndef ITF_FINAL
        if (hasDataError())
        {
            clearComputedData();
            updateTransformationFromLocalToWorld();
            return;
        }
#endif
        
        const u32 texConfigCount = pConfig->m_textureConfigs.size();

        initDatas( pConfig, (i32)texConfigCount, _isPreComputingForCook );
        checkDataConsistency( pConfig );
        
        ITF_ASSERT(!m_meshBuildData);

		if (!is3D())
		{
			// temp build vectors
			ITF_VECTOR<edgeFrieze> edgeList;
			ITF_VECTOR<edgeRun> edgeRunList;
			ITF_VECTOR<collisionRun> collisionRunList;

            if (m_seed == 0)
                m_seed = u32((uPtr)this&0xffFFffFF);
            m_seeder.SetSeed(m_seed);
            m_seeder.Reset();

			m_meshBuildData = newAlloc(mId_Frieze, MeshBuildData());
			m_meshBuildData->m_staticIndexList.resize( texConfigCount );
			m_meshBuildData->m_animIndexList.resize( texConfigCount );

			switch( pConfig->m_methode )
			{
			case InRoundness:
				buildFrieze_InRoundness( edgeList, edgeRunList );
				orderEdgeRunListByTextureId( edgeRunList );
				buildCollision_InRoundness( edgeList, edgeRunList, collisionRunList );
				break;

			case InGeneric:
				buildFrieze_InGeneric( edgeList, edgeRunList );                      
				buildCollision_InGeneric( edgeList, edgeRunList, collisionRunList );
				break;

			case InExtremity:          
				buildFrieze_InExtremity( edgeList, edgeRunList, 0.5f );                      
				buildCollision_InExtremity( edgeList, edgeRunList, collisionRunList );
				break;

			case InExtremityLong:          
				buildFrieze_InExtremity( edgeList, edgeRunList, 0.25f );                      
				buildCollision_InExtremity( edgeList, edgeRunList, collisionRunList );
				break;

			case InExtremitySimple:          
				buildFrieze_InExtremitySimple( edgeList, edgeRunList );                      
				buildCollision_InExtremity( edgeList, edgeRunList, collisionRunList );
				break;

			case InArchitecture:
				buildFrieze_InArchi( edgeList, edgeRunList ); 
				orderEdgeRunListByTextureId( edgeRunList );
				buildCollision_InArchi( edgeList, edgeRunList, collisionRunList );
				break;

			case InArchitectureSimple:
				buildFrieze_InArchiSimple( edgeList, edgeRunList ); 
				orderEdgeRunListByTextureId( edgeRunList );
				buildCollision_InArchi( edgeList, edgeRunList, collisionRunList );
				break;

			case InFluid:
				buildFrieze_InFluid( edgeList, edgeRunList );            
				buildCollision_InFluid( edgeList, edgeRunList, collisionRunList );
				orderEdgeRunListByTextureId( edgeRunList ); // order must be called after collision here
				m_pRecomputeData->m_isHoleVisual = bfalse;
				break;

			case InString:
				buildFrieze_InString( edgeList, edgeRunList );            
				buildCollision_InString( edgeList, edgeRunList, collisionRunList );
				orderEdgeRunListByTextureId( edgeRunList ); // order must be called after collision here
				break;

			case InPipe:
				buildFrieze_InPipe( edgeList, edgeRunList ); 
				orderEdgeRunListByTextureId( edgeRunList );
				buildCollision_InPipe( edgeList, edgeRunList, collisionRunList );
				break;

			case InAtlas:
				buildFrieze_InAtlas( edgeList, edgeRunList, m_atlasIndex ); 
				clearCollisionData();
				break;

			case InFrame:
				buildFrieze_InFrame( edgeList, edgeRunList ); 
				clearCollisionData();
				break;

			case InOverlay:
				{
					m_meshBuildData->m_staticIndexList.resize( texConfigCount );

					buildFrieze_InOverlay( edgeList );                      
					clearCollisionData();                

					if ( m_pMeshOverlayData )
					{
						f32_CosSin( pConfig->m_vtxAnim.m_angle.ToRadians(), &m_pMeshOverlayData->m_cosAngle, &m_pMeshOverlayData->m_sinAngle);
					}
				}
				break;

			case InPipePatchSimple:
				buildFrieze_InPipePatchSimple( edgeList, edgeRunList );           
				buildCollision_InPipePatchSimple( edgeList, edgeRunList, collisionRunList );
				break;

			default:
				ITF_ASSERT_MSG(0, "Unknown method!");
				break;
			}

	#ifdef DEVELOPER_JAY_FRIEZE
			debugDrawEdgeRun( edgeRunList );  

			for ( u32 i = 0; i < m_edgeListCount; i++ )
			{
				Vec2d pos = edgeList[i].m_interUp;
				debugDrawInt( transformPosLocalToWorld(pos), edgeList[i].m_idPoint, Color::yellow() );
			}
	#endif
       
			//build meshes 
			buildFilling( edgeList, edgeRunList );
			setExtrudeExtremityZ( pConfig ); 
			buildMesh_Anim( );        
			buildMesh_Static( );  

			if ( !_isPreComputingForCook ) // keep build data only for cooking
			{
				SF_DEL(m_meshBuildData);
			}   
		}
		else
		{
			SF_DEL(m_pFrieze3DBuildData);
			SF_DEL(m_pFrieze3DRuntimeData);
			m_pFrieze3DBuildData = newAlloc(mId_Frieze, Frieze3DBuildData());
			m_pFrieze3DRuntimeData = newAlloc(mId_Frieze, Frieze3DRuntimeData());

			m_pFrieze3DBuildData->generateBezier(m_pointsList);

			const CollisionFrieze& colConfig = pConfig->m_collisionFrieze;
			if (colConfig.m_build)
			{
				initCollisionData(1);
				m_pFrieze3DBuildData->generateCollision(m_pCollisionData->m_localCollisionList[0],colConfig.m_isFlip);
				finalizeCollisionList();
			}
			else
				clearCollisionData();		

			m_pFrieze3DRuntimeData->generate(*m_pFrieze3DBuildData, getConfig()->m_frieze3DConfig, getConfig()->m_textureConfigs, getConfig()->m_frieze3DConfig.m_orient);
		}

		setVisualData( pConfig );                 

		updateTransformationFromLocalToWorld();
		initializeMovingPolylinesAfterRecomputeData();

		SF_DEL( m_pRecomputeData );
    }

    i32 Frise::getFirstNoFillTextureIndex( const FriseConfig* config ) const
    {
        i32 textureConfigCount = config->m_textureConfigs.size(); 
        for ( i32 i = 0; i < textureConfigCount; i++)
        {
            if ( i != config->m_idTexSwitch && i != config->m_fill.m_idTex && config->m_textureConfigs[i].isDrawnable() )
                return i;
        }

        return -1;
    }

    void Frise::checkDataConsistency( const FriseConfig* _pConfig )
    {
        m_pRecomputeData->m_heightScale = Max( f32_Abs(m_pRecomputeData->m_heightScale), 0.01f );
        m_pRecomputeData->m_widthScale = Max( f32_Abs(m_pRecomputeData->m_widthScale), 0.01f );
        m_pRecomputeData->m_heightScaleInv = 1.f/m_pRecomputeData->m_heightScale;

        // anims vertexs
        if ( _pConfig->m_vtxAnim.m_globalSpeed != 0.f && ( _pConfig->m_vtxAnim.m_amplitudeX!=0.f || _pConfig->m_vtxAnim.m_amplitudeY!=0.f ) )
        {
            m_pRecomputeData->m_anim = btrue;
            m_pRecomputeData->m_animSyncCur = 0.f;
            m_pRecomputeData->m_animSyncXCur = 0.f;
            m_pRecomputeData->m_animSyncYCur = 0.f;

            if ( _pConfig->m_vtxAnim.m_globalRotSpeed.ToRadians() == 0.f )
                m_animGlobalRotCur.SetRadians(0.f);
        }
    }

    void Frise::updateAABB( const Transform2d& _xf )
    {
        AABB aabb( _xf.getPos() );
        
        // grow from pointList local
        const u32 posCount = getPosCount();
        for (u32 i = 0; i<posCount ; i++)
        {
            aabb.grow( _xf.transformPos( getPosAt(i) ) );
        }

        if ( m_pMeshFluidData )
        {
            aabb.grow(m_pMeshFluidData->m_aabbWorld );
            Actor * fxActor = m_pMeshFluidData->m_data.getFxActor();
            if (fxActor)
                fxActor->growAABB(m_pMeshFluidData->m_aabbWorld );
        }

        if ( m_pMeshStaticData )
            aabb.grow(m_pMeshStaticData->m_aabbWorld ); 

        if( m_pMeshAnimData )
            aabb.grow(m_pMeshAnimData->m_aabbWorld );

        if ( m_pMeshOverlayData )			
            aabb.grow( m_pMeshOverlayData->m_aabbWorld );

		if ( m_pFrieze3DRuntimeData )
			aabb.grow( m_pFrieze3DRuntimeData->m_aabbWorld );

        if ( m_pCollisionData )
        {            
            for ( u32 i=0; i<m_pCollisionData->m_worldCollisionList.size(); i++ )
            {
                aabb.grow( m_pCollisionData->m_worldCollisionList[i]->getAABB() );
            }
        }

        if ( m_visualPolyline.getPosCount() )
            aabb.grow( m_visualPolyline.getAABB());

        m_primitiveParamRunTime.m_BV = aabb;

        setAABB( aabb );

        if ( const FriseConfig* config = getConfig() )
        {   
			if ( m_pFrieze3DRuntimeData )
			{
				AABB3d aabb3D(aabb.getMin().to3d( m_pFrieze3DRuntimeData->getLocalMinZ() + getDepth() ), aabb.getMax().to3d( m_pFrieze3DRuntimeData->getLocalMaxZ() + getDepth() ) );
				setAABB3d( aabb3D );
			}
            else if ( config->m_zVtxExtrudeUp || config->m_zVtxExtrudeDown || config->m_zVtxExtrudeStart || config->m_zVtxExtrudeStop 
                || config->m_zVtxExtrudeExtremityStart || config->m_zVtxExtrudeExtremityStop || m_aabbMinZ || m_aabbMaxZ )
            {                
                f32 minZ = f32_Min( 0.f, config->m_zVtxExtrudeUp );
                minZ = f32_Min( minZ, config->m_zVtxExtrudeDown );
                minZ = f32_Min( minZ, config->m_zVtxExtrudeStart );
                minZ = f32_Min( minZ, config->m_zVtxExtrudeStop );
                minZ = f32_Min( minZ, config->m_zVtxExtrudeExtremityStart );
                minZ = f32_Min( minZ, config->m_zVtxExtrudeExtremityStop );
                minZ = f32_Min( minZ, m_aabbMinZ );

                f32 maxZ = f32_Max( 0.f, config->m_zVtxExtrudeUp );
                maxZ = f32_Max( maxZ, config->m_zVtxExtrudeDown );
                maxZ = f32_Max( maxZ, config->m_zVtxExtrudeStart );
                maxZ = f32_Max( maxZ, config->m_zVtxExtrudeStop );
                maxZ = f32_Max( maxZ, config->m_zVtxExtrudeExtremityStart );
                maxZ = f32_Max( maxZ, config->m_zVtxExtrudeExtremityStop );
                maxZ = f32_Max( maxZ, m_aabbMaxZ );

                const f32 scaleCur = ( getScale().x() + getScale().y() ) * 0.5f;
                AABB3d aabb3D( aabb.getMin().to3d( getDepth() +minZ*scaleCur ), aabb.getMax().to3d( getDepth() +maxZ*scaleCur ) );

                setAABB3d( aabb3D );
            }
        }        
    }

    void Frise::setVisualData( const FriseConfig* _pConfig )
    {
        ITF_ASSERT( m_visualPolyline.getPosCount() == 0);

        if ( _pConfig->m_regionId.isValid() )
        {            
            // visual polypoint
            u32 polyPointCount = m_visualPolyPointLocal.getPosCount();

            if ( polyPointCount == 0 )
            {
                polyPointCount = getPosCount();

                for (u32 itPt = 0; itPt < polyPointCount; itPt++)
                {
                    m_visualPolyPointLocal.addPoint( getPosAt(itPt) );                        
                }                

                m_visualPolyPointLocal.forceLoop( isLooping());
            }

            // visual polyline
            for (u32 itPt = 0; itPt < polyPointCount; itPt++)
            {
                Vec2d pos( m_visualPolyPointLocal.getPosAt(itPt) );
                transformLocalPosToWorld( pos );

                m_visualPolyline.addPoint( pos );
            }
           
            m_visualPolyline.setLoop( m_visualPolyPointLocal.isLooping() );
            m_visualPolyline.setDepth(getDepth());
            m_visualPolyline.setGameMaterial( _pConfig->m_gameMaterial.getStringID());
            m_visualPolyline.forceRecomputeData();            

            if (isActive())
            {
                registerRegion( _pConfig );
            }
        }
    }

} // namespace ITF

