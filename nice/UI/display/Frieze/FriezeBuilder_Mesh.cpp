#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_FRISE3DDATA_H_
#include "engine/display/Frieze3D/Frieze3DData.h"
#endif // _ITF_FRISE3DDATA_H_ 

namespace ITF
{
    void Frise::setVtxColor( const FriseTextureConfig& _texConfig, u32& _vtxColorUp, u32& _vtxColorDown ) const
    {
        _vtxColorDown = _texConfig.m_color.getAsU32();
        _vtxColorUp = ITFCOLOR_SET_A(_texConfig.m_color.getAsU32(), _texConfig.m_alphaUp);
    }

    void Frise::setVtxColorWithExtrem( const FriseTextureConfig& _texConfig, u32& _vtxColorUp, u32& _vtxColorDown, u32& _vtxColorUpExt, u32& _vtxColorDownExt, bbool _forceAlpha ) const
    {
        setVtxColor( _texConfig, _vtxColorUp, _vtxColorDown );

        if ( isLooping() && !_forceAlpha )
        {
            _vtxColorDownExt = _vtxColorDown;
            _vtxColorUpExt = _vtxColorUp;
        }
        else
        {
            // down
            _vtxColorDownExt = ITFCOLOR_SET_A(_texConfig.m_color.getAsU32(), _texConfig.m_alphaBorder);

            // up
            u8 alphaTexture = _texConfig.m_color.m_rgba.m_a;
            u8 alphaUp;

            if ( _texConfig.m_alphaUp == alphaTexture )
                alphaUp = _texConfig.m_alphaBorder;
            else if ( _texConfig.m_alphaBorder == alphaTexture )
                alphaUp = _texConfig.m_alphaUp;
            else
                alphaUp = u8( ( _texConfig.m_alphaBorder + _texConfig.m_alphaUp ) *0.5f);

            _vtxColorUpExt = ITFCOLOR_SET_A(_texConfig.m_color.getAsU32(), alphaUp);
        }
    }

    void Frise::addMeshElement( ITF_Mesh & _mesh, const FriseTextureConfig& _texConfig, u32 _indexTableSize ) const
    {
        const ux meshEltIdx = _mesh.addElementAndMaterial( _texConfig.getGFXMaterial() );

        if (m_matShaderOverride) // instance override of texture config material
        {
            _mesh.getMeshElementMaterial(meshEltIdx).setShaderTemplate(m_matShaderOverride);
        }

#ifdef ITF_FORCE_BLEND_COPY_ON_OPAQUE_FRIEZE
        // override blend mode to force copy if possible
        if ( _texConfig.m_alphaBorder==255u && _texConfig.m_alphaUp==255u && _texConfig.m_color.getF32Alpha()==1.f && m_primitiveParamRunTime.m_colorFactor.m_a==1.f)
        {
            GFX_MATERIAL & mat = _mesh.getMeshElementMaterial(meshEltIdx);         
            if (mat.getTexture() && mat.getTexture()->getOpaqueRatio() == 1.f && mat.getBlendMode() == GFX_BLEND_ALPHA) 
            { 
                // force blend copy 
                mat.setBlendMode(GFX_BLEND_COPY); 
            }
        }
#endif // ITF_FORCE_BLEND_COPY_ON_OPAQUE_FRIEZE

        ITF_MeshElement & elem = _mesh.getMeshElement(meshEltIdx);
        elem.m_indexBuffer = GFX_ADAPTER->createIndexBuffer(_indexTableSize , bfalse);
        elem.m_startVertex = 0;
        elem.m_count = _indexTableSize;
        
        GFX_UVANIM & uvAnim = _mesh.getMeshElementMaterial(meshEltIdx).getUVAnim(0);

        // scrolling
        if ( _texConfig.m_scrollingSpeedTrans.x() != 0.f || _texConfig.m_scrollingSpeedTrans.y() != 0.f )
        {
            uvAnim.m_uvFlagAnim |= GFX_UVANIM::UVANIM_F_TRANS;
            uvAnim.m_speedTrans.x() = _texConfig.m_scrollingSpeedTrans.x();
            uvAnim.m_speedTrans.y() = _texConfig.m_scrollingSpeedTrans.y();
        }

        // rotation
        if ( _texConfig.m_scrollingSpeedRotate != 0.f )
        {
            uvAnim.m_uvFlagAnim |= GFX_UVANIM::UVANIM_F_ROTATE;
            uvAnim.m_speedRotate = _texConfig.m_scrollingSpeedRotate;
        }

        if ( _texConfig.m_scrollingUseUV2 )
        {
            // UV2 scrolling
            GFX_UVANIM & uvAnim2 = _mesh.getMeshElementMaterial(meshEltIdx).getUVAnim(1);
            uvAnim2.m_uvFlagAnim |= GFX_UVANIM::UVANIM_F_SCALE;
            uvAnim2.m_scale = _texConfig.m_scrollingUV2Scale;
            if ( _texConfig.m_scrollingSpeedTransUV2.x() != 0.f || _texConfig.m_scrollingSpeedTransUV2.y() != 0.f )
            {
                uvAnim2.m_uvFlagAnim |= GFX_UVANIM::UVANIM_F_TRANS;
                uvAnim2.m_speedTrans.x() = _texConfig.m_scrollingSpeedTransUV2.x();
                uvAnim2.m_speedTrans.y() = _texConfig.m_scrollingSpeedTransUV2.y();
            }

            // UV2 rotation
            if ( _texConfig.m_scrollingSpeedRotateUV2 != 0.f )
            {
                uvAnim2.m_uvFlagAnim |= GFX_UVANIM::UVANIM_F_ROTATE;
                uvAnim2.m_speedRotate = _texConfig.m_scrollingSpeedRotateUV2;
            }
        }
    }

    void Frise::setQuadPos_StartExtremity( const FriseConfig* _pConfig, const edgeFrieze& _edge, Vec2d* _pos, f32 _heightMid, f32 _width, bbool _isSnaped, f32* _zExtrude ) const
    {
        _pos[0] = _edge.m_points[0] - _edge.m_sightNormalized *_width; 
        _pos[1] = _edge.m_points[1] - _edge.m_sightNormalized *_width;

        if ( _isSnaped )
        {
            _pos[2] = _edge.m_pos -_edge.m_normal * _pConfig->m_visualOffset *_heightMid;
            _pos[3] = _pos[2] +_edge.m_normal *_heightMid;
        }
        else
        {
            _pos[2] = _edge.m_points[0];
            _pos[3] = _edge.m_points[1];
        }

        _zExtrude[0] = _pConfig->m_zVtxExtrudeExtremityStart;
        _zExtrude[1] = _pConfig->m_zVtxExtrudeExtremityStart;
        _zExtrude[2] = 0.f;
        _zExtrude[3] = 0.f;
    }

    void Frise::setQuadPos_StopExtremity( const FriseConfig* _pConfig, const edgeFrieze& _edge, Vec2d* _pos, f32 _heightMid, f32 _width, bbool _isSnaped, f32* _zExtrude ) const
    {
        if ( _isSnaped)
        {
            _pos[0] = _edge.m_pos -_edge.m_normal * _pConfig->m_visualOffset *_heightMid;
            _pos[1] = _pos[0] +_edge.m_normal *_heightMid;
            _pos[2] = _edge.m_pos + _edge.m_sightNormalized *_width -_edge.m_normal *_pConfig->m_visualOffset *_edge.m_heightStop;
            _pos[3] = _pos[2] +_edge.m_normal *_edge.m_heightStop;
        }
        else
        {
            _pos[0] = _edge.m_points[2];
            _pos[1] = _edge.m_points[3];
            _pos[2] = _pos[0] + _edge.m_sightNormalized *_width;
            _pos[3] = _pos[1] + _edge.m_sightNormalized *_width;
        }

        _zExtrude[0] = 0.f;
        _zExtrude[1] = 0.f;
        _zExtrude[2] = _pConfig->m_zVtxExtrudeExtremityStop;
        _zExtrude[3] = _pConfig->m_zVtxExtrudeExtremityStop;
    }

    void Frise::setQuadColor_StartExtremity( u32* _color, u32 _vtxColorDown, u32 _vtxColorUp, u32 _vtxColorDownExtremity, u32 _vtxColorUpExtremity ) const
    {
        _color[0] = _vtxColorDownExtremity;
        _color[1] = _vtxColorUpExtremity;
        _color[2] = _vtxColorDown;
        _color[3] = _vtxColorUp;
    }

    void Frise::setQuadColor_StopExtremity( u32* _color, u32 _vtxColorDown, u32 _vtxColorUp, u32 _vtxColorDownExtremity, u32 _vtxColorUpExtremity ) const
    {
        _color[0] = _vtxColorDown;
        _color[1] = _vtxColorUp;
        _color[2] = _vtxColorDownExtremity;
        _color[3] = _vtxColorUpExtremity;
    }

    void Frise::setVtxBorderLeftPosDown( const edgeFrieze& _edge, Vec3d& _vtxPos) const
    {
        _vtxPos.x() = _edge.m_points[0].x();
        _vtxPos.y() = _edge.m_points[0].y();
        _vtxPos.z() = m_pRecomputeData->m_zVtxDown;
    }

    void Frise::setVtxBorderLeftPosUp( const edgeFrieze& _edge, Vec3d& _vtxPos) const
    {
        _vtxPos.x() = _edge.m_points[1].x();
        _vtxPos.y() = _edge.m_points[1].y();
        _vtxPos.z() = m_pRecomputeData->m_zVtxUp;
    }

    void Frise::setVtxBorderRightPosDown( const edgeFrieze& _edge, Vec3d& _vtxPos) const
    {
        _vtxPos.x() = _edge.m_points[2].x();
        _vtxPos.y() = _edge.m_points[2].y();
        _vtxPos.z() = m_pRecomputeData->m_zVtxDown;
    }

    void Frise::setVtxBorderRightPosUp( const edgeFrieze& _edge, Vec3d& _vtxPos) const
    {
        _vtxPos.x() = _edge.m_points[3].x();
        _vtxPos.y() = _edge.m_points[3].y();
        _vtxPos.z() = m_pRecomputeData->m_zVtxUp;
    }

    void Frise::setVtxPosUp( const Vec2d& _pos, Vec3d& _vtxPos) const
    {
        _vtxPos.x() = _pos.x();
        _vtxPos.y() = _pos.y();
        _vtxPos.z() = m_pRecomputeData->m_zVtxUp;
    }

    void Frise::setVtxPosDown( const Vec2d& _pos, Vec3d& _vtxPos) const
    {
        _vtxPos.x() = _pos.x();
        _vtxPos.y() = _pos.y();
        _vtxPos.z() = m_pRecomputeData->m_zVtxDown;
    }
 
    void Frise::setVtxPos( const Vec2d& _pos, Vec3d& _vtxPos, f32 _z) const
    {
        _vtxPos.x() = _pos.x();
        _vtxPos.y() = _pos.y();
        _vtxPos.z() = _z;
    }

    void Frise::setFlipQuadWithAlphaBorder( bbool& _flip, u32 _colorLeft, u32 _colorRight) const
    {
        f32 alphaLeft = Color::getAlphaFromU32(_colorLeft);
        f32 alphaRight = Color::getAlphaFromU32(_colorRight);

        if ( alphaLeft == alphaRight)
            return;

        if ( alphaLeft < alphaRight)
            _flip = btrue;
        else
            _flip = bfalse;
    }

    void Frise::updateMeshMatrixAndAabb( const Transform2d& _xf )
    {
        Matrix44 pMatrixTransfo;
        setLocalToWorldMatrix( &pMatrixTransfo );

        if ( m_pMeshStaticData )
        {
            m_pMeshStaticData->m_mesh.setMatrix(pMatrixTransfo);
            transformLocalAabbToWorld( m_pMeshStaticData->m_aabbLocal, m_pMeshStaticData->m_aabbWorld, _xf );
        }

        if ( m_pMeshAnimData )
        {
            m_pMeshAnimData->m_mesh.setMatrix(pMatrixTransfo);
            transformLocalAabbToWorld( m_pMeshAnimData->m_aabbLocal, m_pMeshAnimData->m_aabbWorld, _xf );
        }

        if ( m_pMeshOverlayData )
        {
            m_pMeshOverlayData->m_mesh.setMatrix(pMatrixTransfo);
            transformLocalAabbToWorld( m_pMeshOverlayData->m_aabbLocal, m_pMeshOverlayData->m_aabbWorld, _xf );
        }

		if ( m_pFrieze3DRuntimeData )
		{
			m_pFrieze3DRuntimeData->m_matrix = pMatrixTransfo;
			m_pFrieze3DRuntimeData->updateMeshesMatrices();
			transformLocalAabbToWorld( m_pFrieze3DRuntimeData->m_aabbLocal, m_pFrieze3DRuntimeData->m_aabbWorld, _xf );
			//m_pFrieze3DRuntimeData->computeWorldBBs(_xf);
		}
    }

    void Frise::updateMeshMatrixAndAabb( const Vec2d & _deltaPos )
    {
        Float4 deltaPos4 = STATIC_FLOAT4(_deltaPos.x(), _deltaPos.y(), 0.f, 0.f);

        if ( m_pMeshStaticData )
        {
            Matrix44 & mat = m_pMeshStaticData->m_mesh.getMatrix();
            mat.setT(add(mat.T(), deltaPos4));
            m_pMeshStaticData->m_aabbWorld.Translate(_deltaPos);
        }

        if ( m_pMeshAnimData )
        {
            Matrix44 & mat = m_pMeshAnimData->m_mesh.getMatrix();
            mat.setT(add(mat.T(), deltaPos4));
            m_pMeshAnimData->m_aabbWorld.Translate(_deltaPos);
        }

        if ( m_pMeshOverlayData )
        {
            Matrix44 & mat = m_pMeshOverlayData->m_mesh.getMatrix();
            mat.setT(add(mat.T(), deltaPos4));
            m_pMeshOverlayData->m_aabbWorld.Translate(_deltaPos);
        }

		if ( m_pFrieze3DRuntimeData )
		{
			Matrix44 & mat = m_pFrieze3DRuntimeData->m_matrix;
            mat.setT(add(mat.T(), deltaPos4));
			m_pFrieze3DRuntimeData->updateMeshesMatrices();
			m_pFrieze3DRuntimeData->m_aabbWorld.Translate(_deltaPos);
		}
    }


    void Frise::growAabbLocalFromVertexAnim( AABB& _aabb)
    {
        if ( const FriseConfig* config = getConfig() )
        {
            f32 amplitudeMax = Max( f32_Abs( config->m_vtxAnim.m_amplitudeX), f32_Abs( config->m_vtxAnim.m_amplitudeY));
            Vec2d offset( amplitudeMax, amplitudeMax);

            _aabb.grow( _aabb.getMin() - offset );
            _aabb.grow( _aabb.getMax() + offset );
        }        
    }

    void setVtxExtrudeExtremityZ( Vec3d& _posVtx, const Vec2d& _posStart, const Vec2d& _dir, f32 _extrudeStart, f32 _extrudeDelta, f32 _lengthCoeff )
    {
        const Vec2d pos( _posVtx.x() -_posStart.x(), _posVtx.y() -_posStart.y() );
        const f32 extrudeCoeff = Clamp( pos.dot(_dir) *_lengthCoeff, 0.f, 1.f);

        _posVtx.z() += _extrudeStart + extrudeCoeff * _extrudeDelta;
    }

    void Frise::setExtrudeExtremityZ( const FriseConfig* _pConfig )
    {
        if ( _pConfig->m_zVtxExtrudeStart == 0.f && _pConfig->m_zVtxExtrudeStop == 0.f)
            return;

        if ( isLooping() || getPosCount() <2 || m_pRecomputeData->m_useAtlasPolygone )
            return;

        const Vec2d posStart = getPosAt(0);
        Vec2d polDir = getPosAt( getPosCount()-1 ) -posStart;
        f32 polLength = polDir.norm();

        if ( polLength < MTH_EPSILON )
            return;

        polDir.normalize(); 
        const f32 extrudeDela = _pConfig->m_zVtxExtrudeStop -_pConfig->m_zVtxExtrudeStart;
        const f32 lengthCoeff = 1.f /polLength;

        const u32 staticVtxCount = m_meshBuildData->m_staticVertexList.size();
        for ( u32 i=0; i<staticVtxCount; i++ )
        {
            setVtxExtrudeExtremityZ( m_meshBuildData->m_staticVertexList[i].m_pos, posStart, polDir, _pConfig->m_zVtxExtrudeStart, extrudeDela, lengthCoeff);
        }

        const u32 animVtxCount = m_meshBuildData->m_animVertexList.size();
        for ( u32 i=0; i<animVtxCount; i++ )
        {
            setVtxExtrudeExtremityZ( m_meshBuildData->m_animVertexList[i].m_pos, posStart, polDir, _pConfig->m_zVtxExtrudeStart, extrudeDela, lengthCoeff);
        }
    }

    void Frise::cleanIndexListArray( ITF_VECTOR<IndexList>& _indexListArray )
    {      
        const i32 startId = (i32)_indexListArray.size() -1;

        for ( i32 i = startId; i >= 0; i-- )
        {
            IndexList& indexList = _indexListArray[i];

            if ( indexList.m_list.size() < 3 )
            {
                _indexListArray.removeAt( i ); // order must be conserved for rendering mesh elements
                continue;
            }

            indexList.m_idTexConfig = i; // set id from texture config
        }        
    }

} // namespace ITF
