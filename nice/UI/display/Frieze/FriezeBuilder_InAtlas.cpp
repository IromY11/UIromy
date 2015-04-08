#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_COOKERFACTORY_H_
#include "tools/plugins/CookerPlugin/CookerFactory.h"
#endif //_ITF_COOKERFACTORY_H_

#ifndef _ITF_NGON2D_H_
#include "Core/math/Ngon2d.h"
#endif // _ITF_NGON2D_H_

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

namespace ITF
{
    Vec2d getCenterUvsFromAABB( const ITF_VECTOR<Vec2d>& _uvsList, const u32 _uvsListCount )
    {
        AABB centerAABB(_uvsList[0]);

        for (u32 i = 1; i < _uvsListCount; i++)
        {
            centerAABB.grow(_uvsList[i]);
        }

        return centerAABB.getCenter();
    }

    // deprecated
    Vec2d getCenterUvsFromBarycentre( const ITF_VECTOR<Vec2d>& _uvsList, const u32 _uvsListCount )
    {
        Vec2d centerOffset( _uvsList[0]);

        for ( u32 i = 1; i < _uvsListCount; i++ )
        {
            centerOffset += _uvsList[i];
        }

        centerOffset /= (f32)_uvsListCount;  

        return centerOffset;
    }

    void Frise::buildFrieze_InAtlas( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, u32& _atlasIndex )
    {
        const FriseConfig* pConfig = getConfig();
        if ( !pConfig || pConfig->m_textureConfigs.size() == 0 )
            return;

        const i32 idTex = getFirstNoFillTextureIndex( pConfig );
        if ( idTex == -1)
            return;

        if (pConfig->m_mesh3d.m_mesh3dList.size() == 1) // atlas need only ONE mesh list element
        {
            Vec2d uv[4] = { Vec2d::Zero, Vec2d::Up, Vec2d::Right, Vec2d::One  };
            buildVB_Static_QuadMeshAlone(uv, uv, m_atlasIndex);
            return;
        }


        const UVdata* pUvData = NULL;
        const UVparameters* pUvParam = NULL; 
        getUvData( pConfig->m_textureConfigs[idTex], pUvData, pUvParam );

        if ( pUvData == NULL )
            return;

        const u32 uvsListCount = pUvData->getUVs().size();
        if ( uvsListCount == 0 )
            return;

        if ( uvsListCount == 2 )
        {
            buildFrieze_InAtlasFromRectangle( pConfig, pUvData, _edgeList, _edgeRunList );
        }
        else if ( pUvParam && pUvParam->getTriangles().size() )
        {
            buildFrieze_InAtlasFromMesh( pUvData, pUvParam, idTex );
        }
        else
        {
            buildFrieze_InAtlasFromListOfPoints( pUvData, pUvParam, idTex );
        }        
    }

    f32 getVtxExtrudeZ_InAtlas( const FriseConfig* _pConfig, const Vec2d& _deltaPos, const Vec2d _deltaExtrude, const Vec2d& _coeffExtrude )
    {
        return ( _pConfig->m_zVtxExtrudeStart + _deltaPos.x() * _coeffExtrude.x() * _deltaExtrude.x() + _pConfig->m_zVtxExtrudeDown + _deltaPos.y() * _coeffExtrude.y() * _deltaExtrude.y() );  
    }

    void Frise::buildFrieze_InAtlasFromMesh( const UVdata* _pUvData, const UVparameters* _pUvParam, u32 _idTex )
    {
#ifdef ITF_SUPPORT_EDITOR
        m_useAtlasPolygoneEditor = btrue;
#endif // ITF_SUPPORT_EDITOR

        m_pRecomputeData->m_useAtlasPolygone = btrue;

        const ITF_VECTOR<Vec2d>& uvsList = _pUvData->getUVs();
        const u32 uvsListCount = uvsList.size();   

        // get center offset
        const Vec2d centerOffset = getCenterUvsFromAABB( uvsList, uvsListCount );  

        // update points position
        AABB posListAABB(Vec2d::Zero);    

        ITF_VECTOR<Vec2d> posList;
        posList.resize(uvsListCount);

        for ( u32 i = 0; i < uvsListCount; i++ )
        {
            const Vec2d& uv = uvsList[i];

            Vec2d& pos = posList[i];
            pos.x() = ( uv.x() - centerOffset.x() ) * m_pRecomputeData->m_widthScale;
            pos.y() = ( centerOffset.y() - uv.y() ) * m_pRecomputeData->m_heightScale;

            // grow aabb
            if ( i == 0 )
            {
                posListAABB.setMinAndMax( pos );
            }
            else
            {
                posListAABB.grow( pos );
            }            
        } 

        const FriseConfig* pConfig = getConfig();
        const u32 color = pConfig->m_textureConfigs[_idTex].m_color.getAsU32();
        IndexList* pIndexList = NULL;

        const Vec2d deltaExtrude( pConfig->m_zVtxExtrudeStop - pConfig->m_zVtxExtrudeStart, pConfig->m_zVtxExtrudeUp - pConfig->m_zVtxExtrudeDown);            
        Vec2d coeffExtrude(Vec2d::Zero);
        if ( posListAABB.getWidth() > MTH_EPSILON )
            coeffExtrude.x() =  1.f / posListAABB.getWidth();
        if ( posListAABB.getHeight() > MTH_EPSILON )
            coeffExtrude.y() =  1.f / posListAABB.getHeight();

        const bbool paramAvailable = _pUvParam->getParams().size() > 0; 
        
        if ( m_pRecomputeData->m_anim )
        {       
            // set indexList
            pIndexList = &m_meshBuildData->m_animIndexList[ _idTex ];

            const f32 animAngle = getAnimAngle_InAtlas(pConfig);

            ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;                
            vtxList.resize( uvsListCount );  

            // set vertexs
            for ( u32 i = 0; i < uvsListCount; i++ )
            {
                const Vec2d& pos = posList[i];

                VertexPNC3T& vtx = vtxList[ i ];                
                vtx.m_pos.x() = pos.x();
                vtx.m_pos.y() = pos.y();
                vtx.m_pos.z() = getVtxExtrudeZ_InAtlas( pConfig, pos - posListAABB.getMin(), deltaExtrude, coeffExtrude );
                vtx.setColor( color );
                vtx.setUv( uvsList[i] );     

                setVtxAnim( pConfig->m_vtxAnim, vtx);
                vtx.m_uv3.m_w = animAngle;                    

                if ( paramAvailable )
                {
                    const UVparameters::Parameters& uvParam = _pUvParam->getParamAt(i);

                    // add depth offset
                    vtx.m_pos.z() += uvParam.m_depth;

                    m_aabbMinZ = f32_Min( m_aabbMinZ, vtx.m_pos.z() );
                    m_aabbMaxZ = f32_Max( m_aabbMaxZ, vtx.m_pos.z() );

                    // set weight anim
                    vtx.m_uv3.x() *= uvParam.m_Weight; 
                    vtx.m_uv3.y() *= uvParam.m_Weight;
                }
            }
        }
        else
        {
            // set indexList
            pIndexList = &m_meshBuildData->m_staticIndexList[ _idTex ];

            ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
            vtxList.resize( uvsListCount );                         

            for ( u32 i = 0; i < uvsListCount; i++ )
            {
                const Vec2d& pos = posList[i];

                VertexPCT& vtx = vtxList[ i ];                
                vtx.m_pos.x() = pos.x();
                vtx.m_pos.y() = pos.y();
                vtx.m_pos.z() = getVtxExtrudeZ_InAtlas( pConfig, pos - posListAABB.getMin(), deltaExtrude, coeffExtrude );
                vtx.setColor( color );
                vtx.setUV( uvsList[i] );

                if ( paramAvailable )
                {
                    // add depth offset
                    vtx.m_pos.z() += _pUvParam->getParamAt(i).m_depth;

                    m_aabbMinZ = f32_Min( m_aabbMinZ, vtx.m_pos.z() );
                    m_aabbMaxZ = f32_Max( m_aabbMaxZ, vtx.m_pos.z() );
                }
            }
        }

        // set indexs
        const ITF_VECTOR<UVparameters::Triangle>& triangleList = _pUvParam->getTriangles();        
        const u32 triangleListCount = triangleList.size();

        pIndexList->reserve( triangleListCount * 3 );

        for ( u32 i = 0; i < triangleListCount; i++ )
        {
            const UVparameters::Triangle& triangle = triangleList[i];

            pIndexList->push_back( (u16)triangle.m_index[0] );
            pIndexList->push_back( (u16)triangle.m_index[1] );
            pIndexList->push_back( (u16)triangle.m_index[2] );
        }
    }

    void Frise::buildFrieze_InAtlasFromListOfPoints( const UVdata* _pUvData, const UVparameters* _pUvParam, u32 _idTex )
    {
#ifdef ITF_SUPPORT_EDITOR
        m_useAtlasPolygoneEditor = btrue;
#endif // ITF_SUPPORT_EDITOR

        m_pRecomputeData->m_useAtlasPolygone = btrue;

        const ITF_VECTOR<Vec2d>& uvsList = _pUvData->getUVs();
        const u32 uvsListCount = uvsList.size();   

        m_atlasTessellationIndex = Min( m_atlasTessellationIndex, uvsListCount -1);

        // get center offset
        const Vec2d centerOffset = getCenterUvsFromBarycentre( uvsList, uvsListCount );       

        // set points position
        AABB posListAABB(Vec2d::Zero);    

        ITF_VECTOR<Vec2d> posList;
        for ( u32 i = 0; i < uvsListCount; i++ )
        {
            const u32 indexUv = ( i + m_atlasTessellationIndex ) % uvsListCount;
            const Vec2d& uv = uvsList[indexUv];          
            const Vec2d pos( ( uv.x() - centerOffset.x() ) * m_pRecomputeData->m_widthScale, ( centerOffset.y() - uv.y() ) * m_pRecomputeData->m_heightScale );

            posList.push_back( pos );

            if ( i == 0 )
            {
                posListAABB.setMinAndMax( pos );
            }
            else
            {
                posListAABB.grow( pos );
            }            
        } 

        // build indexs
        u16* outputIndexes = newAlloc(mId_Frieze, u16[uvsListCount *4]);
        const u32 outputIndexesCount = FillNgon( &posList[0], uvsListCount, outputIndexes) * 3;        

        if ( outputIndexesCount > 2 )
        {
            const FriseConfig* pConfig = getConfig();
            const u32 color = pConfig->m_textureConfigs[_idTex].m_color.getAsU32();
            IndexList* pIndexList = NULL;

            const bbool paramAvailable = _pUvParam && _pUvParam->getParams().size() > 0; 

            const Vec2d deltaExtrude( pConfig->m_zVtxExtrudeStop - pConfig->m_zVtxExtrudeStart, pConfig->m_zVtxExtrudeUp - pConfig->m_zVtxExtrudeDown);            
            Vec2d coeffExtrude(Vec2d::Zero);
            if ( posListAABB.getWidth() > MTH_EPSILON )
                coeffExtrude.x() =  1.f / posListAABB.getWidth();
            if ( posListAABB.getHeight() > MTH_EPSILON )
                coeffExtrude.y() =  1.f / posListAABB.getHeight();

            // set vertexs
            if ( m_pRecomputeData->m_anim )
            {              
                // set indexList
                pIndexList = &m_meshBuildData->m_animIndexList[ _idTex ];

                const f32 animAngle = getAnimAngle_InAtlas(pConfig);

                ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;                
                vtxList.resize( uvsListCount );  

                for ( u32 i = 0; i < uvsListCount; i++ )
                {
                    const u32 indexUv = ( i + m_atlasTessellationIndex ) % uvsListCount;
                    const Vec2d& pos = posList[i];

                    VertexPNC3T& vtx = vtxList[ i ];                
                    vtx.m_pos.x() = pos.x();
                    vtx.m_pos.y() = pos.y();
                    vtx.m_pos.z() = getVtxExtrudeZ_InAtlas( pConfig, pos - posListAABB.getMin(), deltaExtrude, coeffExtrude );
                    vtx.setColor( color );
                    vtx.setUv( uvsList[indexUv] ); 

                    setVtxAnim( pConfig->m_vtxAnim, vtx);
                    vtx.m_uv3.m_w = animAngle;

                    if ( paramAvailable )
                    {
                        const UVparameters::Parameters& uvParam = _pUvParam->getParamAt(indexUv);

                        // add depth offset
                        vtx.m_pos.z() += uvParam.m_depth;

                        m_aabbMinZ = f32_Min( m_aabbMinZ, vtx.m_pos.z() );
                        m_aabbMaxZ = f32_Max( m_aabbMaxZ, vtx.m_pos.z() );

                        // set weight anim
                        vtx.m_uv3.x() *= uvParam.m_Weight; 
                        vtx.m_uv3.y() *= uvParam.m_Weight;
                    }
                }
            }
            else
            {
                // set indexList
                pIndexList = &m_meshBuildData->m_staticIndexList[ _idTex ];

                ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
                vtxList.resize( uvsListCount );                         

                for ( u32 i = 0; i < uvsListCount; i++ )
                {
                    const u32 indexUv = ( i + m_atlasTessellationIndex ) % uvsListCount;
                    const Vec2d& pos = posList[i];

                    VertexPCT& vtx = vtxList[ i ];                
                    vtx.m_pos.x() = pos.x();
                    vtx.m_pos.y() = pos.y();
                    vtx.m_pos.z() = getVtxExtrudeZ_InAtlas( pConfig, pos - posListAABB.getMin(), deltaExtrude, coeffExtrude );
                    vtx.setColor( color );
                    vtx.setUV( uvsList[indexUv] );

                    if ( paramAvailable )
                    {
                        // add depth offset
                        vtx.m_pos.z() += _pUvParam->getParamAt(indexUv).m_depth;

                        m_aabbMinZ = f32_Min( m_aabbMinZ, vtx.m_pos.z() );
                        m_aabbMaxZ = f32_Max( m_aabbMaxZ, vtx.m_pos.z() );
                    }
                }
            }

            // set indexs
            pIndexList->reserve( outputIndexesCount );

            for ( u32 i = 0; i < outputIndexesCount; i++ )
            {
                pIndexList->push_back( outputIndexes[i] );
            }
        }

        delete[] outputIndexes;
    }

    void Frise::buildFrieze_InAtlasFromRectangle( const FriseConfig* _pConfig, const UVdata* _pUvData, ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList )
    {
        const Vec2d& uvData1( _pUvData->getUV1() );
        Vec2d uvUp( _pUvData->getUV0() );
        Vec2d uvDown( uvUp.x(), uvData1.y() );

        m_pRecomputeData->m_texRatio = f32_Abs( uvData1.x() - uvUp.x());

        const f32 longEdgeListWanted = setRatioFixed_InAtlas( _pConfig, _edgeList, &uvUp, &uvDown );

        copyEdgeFromPolyline(_edgeList);
        buildEdgeList( _edgeList);

        if ( m_pRecomputeData->m_edgeListCount == 0 )
            return;

        MatchEdgesWithRatioFixed( _edgeList, longEdgeListWanted);

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        if ( !buildEdgeRunList_InGeneric( _edgeList, _edgeRunList ) )
            return;

        edgeRun& edgeRunCur = _edgeRunList[0]; 
        setEdgeRunNormUv( _pConfig, _edgeList, edgeRunCur);
        setEdgeRunCoeffUv_InAtlas( _edgeList, edgeRunCur);

        buildVB_InAtlas( _edgeList, edgeRunCur, &uvUp, &uvDown );  
    }

    void Frise::MatchEdgesWithRatioFixed( ITF_VECTOR<edgeFrieze>& _edgeList, f32 _longWanted) const
    {
        if ( _longWanted == 0.f || m_pRecomputeData->m_edgeListCount == 0 )
            return;

        f32 longCurrent = 0.f;
        for ( u32 i=0; i<m_pRecomputeData->m_edgeListCount; i++)
            longCurrent += _edgeList[i].m_norm;

        f32 deltaLongBy2 = (_longWanted -longCurrent) *0.5f;

        if ( deltaLongBy2 > MTH_EPSILON)
        {
            edgeFrieze& edgeStart = _edgeList[0]; 
            Vec2d offset = deltaLongBy2 *edgeStart.m_sightNormalized;

            edgeStart.m_snap = bfalse;
            edgeStart.m_sight += offset;
            edgeStart.m_norm += deltaLongBy2;
            edgeStart.m_pos -= offset;
            edgeStart.m_points[0] -= offset;
            edgeStart.m_points[1] -= offset;
            edgeStart.m_interDown = edgeStart.m_points[0];
            edgeStart.m_interUp = edgeStart.m_points[1];

            u32 idEdgeLast = m_pRecomputeData->m_edgeListCount -1;            
            edgeFrieze& edgeLast = _edgeList[idEdgeLast];

            offset = deltaLongBy2 *edgeLast.m_sightNormalized;
            edgeLast.m_snap = bfalse;
            edgeLast.m_sight += offset;
            edgeLast.m_norm += deltaLongBy2;
            edgeLast.m_points[2] += offset;
            edgeLast.m_points[3] += offset;
        }
    }

    f32 Frise::setRatioFixed_InAtlas( const FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList, Vec2d* _uvUp, Vec2d* _uvDown )
    {     
        f32 longEdgeList = 0.f;
        u32 edgeCount = m_pointsList.getPosCount();

        if ( _config->m_isRatioFixed && m_pRecomputeData->m_texRatio>0.f && edgeCount >1 )
        {
            edgeCount --;

            const f32 deltaY = f32_Abs( _uvUp->y() - _uvDown->y());
            const f32 ratio = ( deltaY * m_pRecomputeData->m_heightScale ) / ( m_pRecomputeData->m_widthScale * m_pRecomputeData->m_texRatio );            

            for ( u32 i=0; i<edgeCount; i++)
                longEdgeList += m_pointsList.getEdgeAt(i).m_length;

            m_pRecomputeData->m_heightScale =  longEdgeList *ratio;
        }

        return longEdgeList;
    }

    void Frise::getUvData( const FriseTextureConfig& _texConfig, const UVdata*& _pUVdata, const UVparameters*& _pUVparameter )
    {
        if ( _texConfig.hasDiffuseTexture() )
        {            
            const UVAtlas* pAtlas = NULL;
         
#ifdef ITF_SUPPORT_COOKING
            cookerFactory* pFactory = cookerFactory::getInstance();
            pAtlas = pFactory->getAtlasFromTextureFile( _texConfig.getTexturePath() );   
#else
            if ( const Texture* pTexture = _texConfig.getGFXMaterial().getTexture() )
            {                    
                pAtlas = pTexture->getUVAtlas();
            }
#endif 

            if ( pAtlas )
            {
                const u32 uvDataCount = pAtlas->getNumberUV();
                if ( uvDataCount > 0 )
                {
                    m_atlasIndex = Min( m_atlasIndex, uvDataCount -1 );

                    _pUVdata = &pAtlas->getUVDatabyIndex( m_atlasIndex );
                    _pUVparameter = pAtlas->getUvParameterByIndex( m_atlasIndex );           
                }
#ifndef ITF_FINAL
                else
                {
                    String8 str;
                    str.setTextFormat("Empty texture atlas file: \"%s\"", _texConfig.getTexturePath().toString8().cStr());
                    setDataError(str);
                }
#endif //!ITF_FINAL
            }
#ifndef ITF_FINAL
            else
            {
                String8 str;
                str.setTextFormat("Missing texture atlas file: \"%s\"", _texConfig.getTexturePath().toString8().cStr());
                setDataError(str);
            }
#endif //!ITF_FINAL
        }
    }

    void Frise::setEdgeRunCoeffUv_InAtlas( const ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun) const
    {
        f32 normUvCount = 0.f;

        for ( u32 count = 0; count < m_pRecomputeData->m_edgeListCount; count ++ )        
            normUvCount += _edgeList[count].m_normUv;

        if ( normUvCount >0.f )
            _edgeRun.m_coeff = m_pRecomputeData->m_texRatio /normUvCount;
    }

    void Frise::buildVB_InAtlas( ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun, Vec2d* _uvUp, Vec2d* _uvDown )
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return;

        if ( m_pRecomputeData->m_isUvFlipY )
        {
            f32 temp = _uvUp->y();
            _uvUp->y() = _uvDown->y();
            _uvDown->y() = temp;
        }

        if ( m_pRecomputeData->m_uvXsign <0.f)
        {
            _uvUp->x() += m_pRecomputeData->m_texRatio;
            _uvDown->x() = _uvUp->x();
        }

        u16 indexVtxUp = 0;
        u16 indexVtxDown = 0;
        u32 vtxColorDown, vtxColorUp, vtxColorUpExtremity, vtxColorDownExtremity;

        if ( m_pRecomputeData->m_edgeListCount >1 )
        {
            setVtxColorWithExtrem( config->m_textureConfigs[_edgeRun.m_idTex], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity );
        }
        else
        {
            setVtxColor( config->m_textureConfigs[_edgeRun.m_idTex], vtxColorUp, vtxColorDown);
            vtxColorUpExtremity = vtxColorUp;
            vtxColorDownExtremity = vtxColorDown;
        }

        if ( m_pRecomputeData->m_anim )
        {
            buildVB_Anim_EdgeRun( config, _edgeList, _edgeRun, m_meshBuildData->m_animIndexList[_edgeRun.m_idTex], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity, indexVtxUp, indexVtxDown, *_uvUp, *_uvDown, m_pRecomputeData->m_uvXsign );   
        }
        else
        {
            buildVB_Static_EdgeRun( config, _edgeList, _edgeRun, m_meshBuildData->m_staticIndexList[_edgeRun.m_idTex], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity, indexVtxUp, indexVtxDown, *_uvUp, *_uvDown, m_pRecomputeData->m_uvXsign );   
        }
    }

    f32 Frise::getAnimAngle_InAtlas( const FriseConfig* _pConfig ) const
    {
        f32 animAngle = 0.f;

        if ( _pConfig->m_vtxAnim.m_angleUsed )
        {
            animAngle = _pConfig->m_vtxAnim.m_angle.ToRadians();
        }
        else if ( getPosCount() > 1 )
        {
            const PolyLineEdge& edge = getEdgeAt(0);

            if ( edge.m_normalizedVector.isNormalized() )
            {
                animAngle = atan2( edge.m_normalizedVector.y(), edge.m_normalizedVector.x() );
            }
        }

        return animAngle;
    }
} // namespace ITF
