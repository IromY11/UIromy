#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

namespace ITF
{
    void Frise::registerFluidShaper(u32 _layerIndex, FluidShaper *_shaper) 
    {
        ITF_ASSERT_MSG( m_pMeshFluidData, "No data fluid");

        if ( m_pMeshFluidData )
        {
            m_pMeshFluidData->m_data.addFluidShaper(_layerIndex, _shaper);
        }        
    }

    void Frise::unregisterFluidShaper(u32 _layerIndex, FluidShaper *_shaper) 
    {
        ITF_ASSERT_MSG( m_pMeshFluidData, "No data fluid");

        if ( m_pMeshFluidData )
        {
            m_pMeshFluidData->m_data.removeFluidShaper(_layerIndex, _shaper);
        }        
    }

    const SafeArray<ObjectRef>* Frise::getStringWaveGenerators() const 
    {
        ITF_ASSERT_MSG( m_pMeshFluidData, "No data fluid");

        if ( m_pMeshFluidData )
            return &m_pMeshFluidData->m_data.getStringWaveGenerators();

        return NULL;
    }

    void Frise::setFluidLayerCollisionHeightMultiplier(u32 _layerIndex, f32 _multiplier)
    {
        ITF_ASSERT_MSG( m_pMeshFluidData, "No data fluid");

        if ( m_pMeshFluidData )
        {
            m_pMeshFluidData->m_data.setLayerCollisionHeightMultiplier(_layerIndex, _multiplier);
        }        
    }

    void Frise::clearMeshFluidData()
    {
        if ( m_pMeshFluidData )
        {
            m_pMeshFluidData->m_data.clear();
            SF_DEL(m_pMeshFluidData);
        }
    }

    void Frise::initDataFluid()
    {
        const FriseConfig* config = getConfig();

        m_pMeshFluidData = newAlloc(mId_Frieze, MeshFluidData);

        m_pMeshFluidData->m_data.setFrieze(this);
        m_pMeshFluidData->m_data.setIsCushion(config->m_collisionFrieze.m_build);
        m_pMeshFluidData->m_data.setLayers(config->m_fluid.m_layers);
        m_pMeshFluidData->m_data.setWeightMultiplier(config->m_fluid.m_weightMultiplier);
        
        if ( config->m_fluid.m_perpendicularBack )
        {
            const f32 offset = config->m_fluid.m_perpendicularBackPosZ - config->m_fluid.m_perpendicularBackScale;
            m_aabbMinZ = f32_Min( m_aabbMinZ, offset );
            m_aabbMaxZ = f32_Max( m_aabbMaxZ, offset );
        }

        if ( config->m_fluid.m_perpendicularFront )
        {
            const f32 offset = config->m_fluid.m_perpendicularFrontPosZ + config->m_fluid.m_perpendicularFrontScale;
            m_aabbMinZ = f32_Min( m_aabbMinZ, offset );
            m_aabbMaxZ = f32_Max( m_aabbMaxZ, offset );
        }
        
    }

    void Frise::buildFrieze_InString( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {
        buildFrieze_InFluid( _edgeList, _edgeRunList );
    }

    void Frise::buildFrieze_InFluid( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {
        // reset scale point
        m_pointsList.resetAllPointsScale();

        copyEdgeFromPolyline(_edgeList);

        m_pRecomputeData->m_texRatio = 0.25f;

        buildEdgeList( _edgeList );

        if ( m_pRecomputeData->m_edgeListCount == 0 )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        if ( !buildEdgeRunList_InFluid( _edgeList, _edgeRunList ) )
            return;

        if ( !setEdgeRunListCoeffUv( _edgeList, _edgeRunList ) )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeRun( _edgeRunList );             
#endif
        
       initDataFluid();
       buildEdgeFluidList( _edgeList, _edgeRunList );
       

       Transform2d xf( get2DPos(), getAngle(), getScale(), getIsFlipped() );
       updateFluidData( xf );

       buildVisualPolypointInFluid( _edgeList );
       buildVB_FillGradient( _edgeList ); 
    }

    bbool Frise::buildEdgeRunList_InFluid( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return bfalse;

        _edgeRunList.reserve(m_pRecomputeData->m_edgeListCount);

        edgeRun edgeRunCur;        
        edgeRunCur.m_idTex = getFirstNoFillTextureIndex( config );

        if ( edgeRunCur.m_idTex == -1 )
        {
            edgeRunCur.m_edgeCount = m_pRecomputeData->m_edgeListCount;
            _edgeRunList.push_back(edgeRunCur);
            ITF_WARNING(this, 0, "Wrong method or missing texture, to use regionId only use method 1");
            return bfalse;
        }

        // only one edge by edgeRun
        edgeRunCur.m_edgeCount = 1;

        for ( u32 i = 0; i< m_pRecomputeData->m_edgeListCount; i++ )
        {
            edgeRunCur.m_idEdgeStart = i;
            _edgeRunList.push_back(edgeRunCur);
        }

        return btrue;
    }

    i32 Frise::getIdStartEdge_InFluid( ITF_VECTOR<edgeFrieze>& _edgeList )
    {  
        for ( u32 i = 0; i < m_pRecomputeData->m_edgeListCount; i++ )
        {
            if ( isEdgeValid_InFluid( _edgeList[i] ) )
                return (i32)i;
        }

        return -1;
    }

    bbool Frise::isEdgeValid_InFluid( const edgeFrieze& _edge ) const
    {
        if ( _edge.m_snap )
            return bfalse;

        if ( isEdgeWithHoleVisual( _edge ) )
            return bfalse;

        return btrue;        
    }

    bbool Frise::isEdgeLastValid_InFluid( const ITF_VECTOR<edgeFrieze>& _edgeList, u32 _indexCur ) const
    {
        u32 indexLast;

        if ( _indexCur > 0 )
        {
            indexLast = _indexCur -1;
        }
        else if ( isLooping() )
        {
            indexLast = ( _indexCur +m_pRecomputeData->m_edgeListCount -1) %m_pRecomputeData->m_edgeListCount;
        }
        else
        {
            return bfalse;
        }

        return isEdgeValid_InFluid( _edgeList[ indexLast ] );
    }

    void Frise::setEdgeFluidPosWithHoleVisualOnPreviousEdge( EdgeFluid& _edge, const Vec2d& _sightNormalized )
    {
        f32 dist = (_edge.m_pos[1] -_edge.m_pos[0]).dot(_sightNormalized);

        _edge.m_pos[0] += _sightNormalized *dist;
    }

    void Frise::setEdgeFluidPosWithHoleVisualOnNextEdge( EdgeFluid& _edge, const Vec2d& _sightNormalized )
    {
        f32 dist = (_edge.m_pos[3] -_edge.m_pos[2]).dot(_sightNormalized);

        _edge.m_pos[2] += _sightNormalized *dist;
    }

    void Frise::buildEdgeFluidList( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList)
    {
        const FriseConfig* config = getConfig();
        if ( !config || _edgeRunList[0].m_idTex == -1 )
            return;
        
        i32 idStartEdge = getIdStartEdge_InFluid( _edgeList );
        if ( idStartEdge == -1 )
            return;

        const FriseTextureConfig& texConfig = config->m_textureConfigs[_edgeRunList[0].m_idTex];

        // init uvs
        f32 uvYup = 0.f;
        f32 uvYdown = 0.f;

        if ( m_pRecomputeData->m_isUvFlipY )
        {
            uvYdown = 0.f + m_uvYoffSet;
            uvYup = uvYup + m_uvY_tile;    
        }
        else
        {
            uvYup = 0.f + m_uvYoffSet;
            uvYdown = uvYup + m_uvY_tile;
        }
        
        f32 uvXcur = getUvX_Start();
        for ( i32 i = 0; i < idStartEdge; i++ )
        {
            uvXcur += _edgeList[i].m_normUv * _edgeRunList[i].m_coeff * m_pRecomputeData->m_uvXsign;
        }

        /*
                1__3 
                |\ | 
                |_\| 
                0  2 
        */
                    
        // Start
        edgeFrieze& edgeStart = _edgeList[idStartEdge];
        EdgeFluid   edgeFluidCur;
        u32         flag = EDGEFLUID_MAIN_DATA | EDGEFLUID_VISUAL;


        edgeFluidCur.m_material = texConfig.getGFXMaterial();
        if (m_matShaderOverride) // instance override of texture config material
        {
            edgeFluidCur.m_material.setShaderTemplate(m_matShaderOverride);
        }

        edgeFluidCur.m_uvAnimTrans = texConfig.m_scrollingSpeedTrans;
        edgeFluidCur.m_depth = getDepth();
        edgeFluidCur.m_delta    = config->m_fluid.m_levelDelta;
        edgeFluidCur.m_deltaUV  = config->m_fluid.m_UVDelta;
        edgeFluidCur.m_visualProcess.m_id = idStartEdge;
         
        edgeFluidCur.m_pos[0] = edgeStart.m_interDown;     
        edgeFluidCur.m_pos[1] = edgeStart.m_interUp;
        if ( !isEdgeLastValid_InFluid( _edgeList, idStartEdge) )
            setEdgeFluidPosWithHoleVisualOnPreviousEdge( edgeFluidCur, edgeStart.m_sightNormalized );        

        edgeFluidCur.m_uv[0] = Vec2d( uvXcur, uvYdown );     
        edgeFluidCur.m_uv[1] = Vec2d( uvXcur, uvYup );        
        edgeFluidCur.m_uv[2].y() = uvYdown;     
        edgeFluidCur.m_uv[3].y() = uvYup;    

        if ( config->m_collisionFrieze.m_build && !isEdgeWithHoleCollision( edgeStart ) )
        {
            edgeFluidCur.m_collisionProcesses[0].m_id = 0; 
            edgeFluidCur.m_collisionProcesses[1].m_id = 0;
            flag |= EDGEFLUID_COLLISION;
        }
        else
        {
            edgeFluidCur.m_collisionProcesses[0].m_id = -1;
            edgeFluidCur.m_collisionProcesses[1].m_id = -1;
        }

        // update uv
        uvXcur += edgeStart.m_normUv * _edgeRunList[idStartEdge].m_coeff * m_pRecomputeData->m_uvXsign;

        // list
        ITF_VECTOR<EdgeFluid> & edgeFluidList = m_pMeshFluidData->m_data.getEdgeFluid();
        bbool addEdgeFluidInList = btrue;

        for ( u32 i = idStartEdge +1; i<m_pRecomputeData->m_edgeListCount; i++ )
        {
            edgeFrieze& edgeCur = _edgeList[i];
 
            if ( addEdgeFluidInList )
            {
                addEdgeFluidInList = bfalse;

                // stop last
                edgeFluidCur.m_pos[2] = edgeCur.m_interDown;
                edgeFluidCur.m_pos[3] = edgeCur.m_interUp;
                if ( !isEdgeValid_InFluid( edgeCur ) )
                    setEdgeFluidPosWithHoleVisualOnNextEdge( edgeFluidCur, _edgeList[edgeFluidCur.m_visualProcess.m_id].m_sightNormalized ); 

                edgeFluidCur.m_uv[2].x() = uvXcur;     
                edgeFluidCur.m_uv[3].x() = uvXcur; 
                edgeFluidCur.updateData(config, flag);
                edgeFluidList.push_back(edgeFluidCur);
            }

            if ( isEdgeValid_InFluid( edgeCur ) )
            {
                addEdgeFluidInList = btrue;
                flag = EDGEFLUID_MAIN_DATA | EDGEFLUID_VISUAL;

                // start next                 
                edgeFluidCur.m_pos[0] = edgeCur.m_interDown;
                edgeFluidCur.m_pos[1] = edgeCur.m_interUp;
                if ( !isEdgeLastValid_InFluid( _edgeList, i) )
                    setEdgeFluidPosWithHoleVisualOnPreviousEdge( edgeFluidCur, edgeCur.m_sightNormalized );  
                
                edgeFluidCur.m_uv[0].x() = uvXcur;     
                edgeFluidCur.m_uv[1].x() = uvXcur; 
                edgeFluidCur.m_visualProcess.m_id = i;

                if ( config->m_collisionFrieze.m_build && !isEdgeWithHoleCollision( edgeCur ) )
                {
                    edgeFluidCur.m_collisionProcesses[0].m_id = 0; 
                    edgeFluidCur.m_collisionProcesses[1].m_id = 0; 
                    flag |= EDGEFLUID_COLLISION;
                }
                else
                {
                    edgeFluidCur.m_collisionProcesses[0].m_id = -1;
                    edgeFluidCur.m_collisionProcesses[1].m_id = -1;
                }
            }

            // update uv
            uvXcur += edgeCur.m_normUv * _edgeRunList[i].m_coeff * m_pRecomputeData->m_uvXsign;
        }

        if ( addEdgeFluidInList )
        {
            // EdgeFluid Stop
            if ( m_pointsList.isLooping() )
            {
                const edgeFrieze& edgeCur = _edgeList[0];
                edgeFluidCur.m_pos[2] = edgeCur.m_interDown; 
                edgeFluidCur.m_pos[3] = edgeCur.m_interUp;

                if ( !isEdgeValid_InFluid( edgeCur ) )
                    setEdgeFluidPosWithHoleVisualOnNextEdge( edgeFluidCur, _edgeList[edgeFluidCur.m_visualProcess.m_id].m_sightNormalized );
            }
            else
            {
                edgeFrieze& edgeStop = _edgeList[m_pRecomputeData->m_edgeListCount -1];

                edgeFluidCur.m_pos[2] = edgeStop.m_points[2];
                edgeFluidCur.m_pos[3] = edgeStop.m_points[3];
            }

            edgeFluidCur.m_uv[2].x() = uvXcur;     
            edgeFluidCur.m_uv[3].x() = uvXcur; 

            edgeFluidCur.updateData(config, flag);
            if (config->m_methode == InString)
            {
                ITF_WARNING_CATEGORY(LD,this, edgeFluidCur.m_lastIndex<(u32)(NB_FLUID_LEVELS-1), "frieze edge is too long, wave may loose details");                
            }
            edgeFluidList.push_back(edgeFluidCur);
        }        
        
        // set vertexs color
        setEdgeFluidVertexColors( texConfig, edgeFluidList );

        m_pMeshFluidData->m_data.computeEdgeFluidLevels(config->m_fluid.m_levelsFront, config->m_fluid.m_levelsBack, config->m_fluid.m_perpendicularBack || config->m_fluid.m_perpendicularFront);

        f32 scaleTotal = 0.f;
        if (config->m_fluid.m_perpendicularBack)
            scaleTotal += config->m_fluid.m_perpendicularBackScale;
        if (config->m_fluid.m_perpendicularFront)
            scaleTotal += config->m_fluid.m_perpendicularFrontScale;

        if (config->m_fluid.m_perpendicularBack)
        {        
            m_pMeshFluidData->m_data.addEdgeFluidLevelPerpendicular(-1, config->m_fluid.m_perpendicularBackScale, scaleTotal, config->m_fluid.m_perpendicularBackPos, config->m_fluid.m_perpendicularBackPosZ, config->m_fluid.m_perpendicularBackZ);
        }
        if (config->m_fluid.m_perpendicularFront)
        {
            m_pMeshFluidData->m_data.addEdgeFluidLevelPerpendicular(1, config->m_fluid.m_perpendicularFrontScale, scaleTotal, config->m_fluid.m_perpendicularFrontPos, config->m_fluid.m_perpendicularFrontPosZ, config->m_fluid.m_perpendicularFrontZ);
        }

        m_pMeshFluidData->m_data.computeAABB( m_pMeshFluidData->m_aabbLocal);        
    }

    void Frise::setEdgeFluidVertexColors( const FriseTextureConfig& _texConfig, ITF_VECTOR<EdgeFluid>& _edgeFluidList ) const
    {
        // init vertex color
        u32 vtxColorDown, vtxColorUp, vtxColorUpExtremity, vtxColorDownExtremity;
        setVtxColorWithExtrem( _texConfig, vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity, btrue );

        const u32 edgeFluidCount = _edgeFluidList.size();
        ITF_ASSERT( edgeFluidCount );

        const FriseConfig *config = getConfig();
        ITF_ASSERT_CRASH(config, "no config!");
        // set colors
        u32 idEdgeLast = edgeFluidCount -1;
        for ( u32 i =0; i<edgeFluidCount; i++ )
        {
            EdgeFluid& edgeCur = _edgeFluidList[i];
            EdgeFluid& edgeLast = _edgeFluidList[idEdgeLast];
            idEdgeLast = i;

            if ( edgeCur.m_pos[0].IsEqual( edgeLast.m_pos[2], MTH_EPSILON ) )
            {
                edgeCur.m_colors[0] = vtxColorDown;
                edgeCur.m_colors[1] = vtxColorUp;
            }
            else
            {
                edgeCur.m_colors[0] = vtxColorDownExtremity;
                edgeCur.m_colors[1] = vtxColorUpExtremity;
            }

            for (u32 layerIndex=1; layerIndex<config->m_fluid.m_layers.size(); layerIndex++)
            {
                u32 colorIndexBase = 4*layerIndex;
                edgeCur.m_colors[colorIndexBase] = edgeCur.m_colors[colorIndexBase+1] = edgeCur.m_colors[colorIndexBase+2] = edgeCur.m_colors[colorIndexBase+3] = config->m_fluid.m_layers[layerIndex].m_color.getAsU32();
            }

            edgeLast.m_colors[2] = edgeCur.m_colors[0];
            edgeLast.m_colors[3] = edgeCur.m_colors[1];
        }      

        // check colors 
        for ( u32 i =0; i<edgeFluidCount; i++ )
        {
            EdgeFluid& edgeCur = _edgeFluidList[i];

            if (edgeCur.m_colors[0] == vtxColorDownExtremity && edgeCur.m_colors[2] == vtxColorDownExtremity)
            {
                edgeCur.m_colors[2] = vtxColorDown;
                edgeCur.m_colors[3] = vtxColorUp;
            }
        }
    }

    void Frise::buildCollision_InString( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {
        if ( buildCollisionRunList( _edgeList, _edgeRunList, _collisionRunList ) )
        {
            buildCollision_EdgeRunList ( _edgeList, _edgeRunList, _collisionRunList );
        }

        const FriseConfig* config = getConfig();
        if ( !config || !config->m_collisionFrieze.m_build )
            return;

        // build collision for edge fluids 
        for (u32 i=0; i<config->m_fluid.m_sideCount; i++)
        {
            buildCollision_EdgeFluidList( config, i );
        }

        finalizeCollisionList();
    }

    void Frise::buildCollision_InFluid( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {
        if ( buildCollisionRunList( _edgeList, _edgeRunList, _collisionRunList ) )
        {
            buildCollision_EdgeRunList ( _edgeList, _edgeRunList, _collisionRunList );
        }

        const FriseConfig* config = getConfig();
        if ( !config || !config->m_collisionFrieze.m_build )
            return;

        buildCollision_EdgeFluidList( config, 0 );
        finalizeCollisionList();
    }

    void Frise::buildCollision_EdgeFluidList( const FriseConfig* _config, u32 _sideIndex )
    {
        if ( !m_pMeshFluidData )
            return;

        ITF_VECTOR<EdgeFluid>& edgeFluidList = m_pMeshFluidData->m_data.getEdgeFluid();

        u32 edgeFluidListCount = edgeFluidList.size();
        const StringID &gameMaterial = _config->m_gameMaterial.getStringID();

        i32 edgeIndex_Start, edgeIndex_End, edgeIndex_Step;

        if (_sideIndex)
        {
            edgeIndex_Start = (i32)edgeFluidListCount-1;
            edgeIndex_End = -1;
            edgeIndex_Step = -1;
        }
        else
        {
            edgeIndex_Start = 0;
            edgeIndex_End = (i32)edgeFluidListCount;
            edgeIndex_Step = 1;
        }


        for ( i32 i = edgeIndex_Start; i != edgeIndex_End; i+=edgeIndex_Step )
        {
            EdgeFluid& edgeFluidCur = edgeFluidList[i];
            EdgeProcessData &collisionProcess = edgeFluidCur.m_collisionProcesses[_sideIndex];

            // no collision wanted
            if ( collisionProcess.m_id == -1 )
            {
                continue;
            }

            if ( !m_pCollisionData )
                createCollisionData();

            // set id collision
            collisionProcess.m_id = m_pCollisionData->m_localCollisionList.size();

            PolyPointList pointsList;
            edgeFluidCur.fillPolyline( pointsList, EDGEFLUID_COLLISION, _sideIndex);
            pointsList.setGameMaterial( gameMaterial );

            m_pCollisionData->m_localCollisionList.push_back( pointsList );
        }
    }

    void Frise::buildVisualPolypointInFluid(const ITF_VECTOR<edgeFrieze>& _edgeList)
    {
        const FriseConfig *config = getConfig();
        if (!config)
            return;

        if ( config->m_regionId != s_RegionName_Swim ||
             m_pMeshFluidData->m_data.getEdgeFluid().size() == 0 || _edgeList.size() == 0)
            return;      
                
        // process points
        i32 currentIndex = 0;
        ITF_VECTOR<EdgeFluid>& edgeFluidList = m_pMeshFluidData->m_data.getEdgeFluid();

        u32 edgeFluidListCount = edgeFluidList.size();

        for ( u32 i = 0; i < edgeFluidListCount; i++, currentIndex++ )
        {
            EdgeFluid& edgeFluidCur = edgeFluidList[i];
            for (;currentIndex < edgeFluidCur.m_visualProcess.m_id; currentIndex++)
            {
                m_visualPolyPointLocal.addPoint(_edgeList[currentIndex].m_pos);       
            }
            edgeFluidCur.m_visualProcess.m_indexStart   = m_visualPolyPointLocal.getPosCount();
            edgeFluidCur.fillPolyline( m_visualPolyPointLocal, EDGEFLUID_VISUAL, 0);
            edgeFluidCur.m_visualProcess.m_indexEnd     = m_visualPolyPointLocal.getPosCount() - 1;
        }
        if (currentIndex < (i32)_edgeList.size())
        {
            for (;currentIndex < (i32)_edgeList.size(); currentIndex++)
            {
                m_visualPolyPointLocal.addPoint(_edgeList[currentIndex].m_pos);       
            }
           
            if ( !isLooping())
            {
                const u32 lastEdgeIndex = _edgeList.size()-1;
                m_visualPolyPointLocal.addPoint(_edgeList[lastEdgeIndex].m_pos +_edgeList[lastEdgeIndex].m_sight);
            }            
        }
                
        // make it cycle
        m_visualPolyPointLocal.addPoint( m_visualPolyPointLocal.getPosAt(0));
        m_visualPolyPointLocal.forceLoop(btrue);
    }

    void Frise::initDataCooked_InFluid()
    {
        const FriseConfig* pConfig = getConfig();
        ITF_ASSERT_CRASH( pConfig, "Invalid config");

        const i32 idTex = getFirstNoFillTextureIndex( pConfig );
        ITF_ASSERT_CRASH( idTex != -1, "Invalid texture config");

        const GFX_MATERIAL& material = pConfig->m_textureConfigs[ idTex ].getGFXMaterial();
               
        ITF_VECTOR<EdgeFluid>& edgeFluidList            = m_pMeshFluidData->m_data.getEdgeFluid();
        ITF_VECTOR<EdgeFluidLevel>& edgeFluidLevelsList = m_pMeshFluidData->m_data.getEdgeFluidLevels();            

        // set frieze ref
        m_pMeshFluidData->m_data.setFrieze(this);

        // init edgeFluid
        const u32 edgeFluidListCount = edgeFluidList.size();
        for ( u32 i = 0; i < edgeFluidListCount; i++ )
        {
            EdgeFluid& edgeFluidCur = edgeFluidList[i];

            // set material
            edgeFluidCur.m_material = material;

            if (m_matShaderOverride) // instance override of texture config material
            {
                edgeFluidCur.m_material.setShaderTemplate(m_matShaderOverride);
            }
        }

        // init edgeFluidLevels
        const u32 count = edgeFluidLevelsList.size();
        for ( u32 i = 0; i < count; i++ )
        {
            EdgeFluidLevel& edgeFluidLevel = edgeFluidLevelsList[i];

            // set pointer to edgeFluid
            edgeFluidLevel.m_edgeFluid = &edgeFluidList[ edgeFluidLevel.m_idEdgeFluid ];
        }
    }
} // namespace ITF
