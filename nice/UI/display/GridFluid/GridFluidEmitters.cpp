#include "precompiled_engine.h"
#ifdef GFX_USE_GRID_FLUIDS

#ifndef _ITF_CORE_MACROS_H_
# include "core/Macros.h"
#endif //_ITF_CORE_MACROS_H_

#ifndef _ITF_GRIDFLUID_H_
#include "engine/display/GridFluid/GridFluid.h"
#endif // _ITF_GRIDFLUID_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef ITF_GFX_ZLIST_H_
#include "GFXZList.h"
#endif // ITF_GFX_ZLIST_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _TRIANGULATE_H_
#include "Core/math/Triangulate.h"
#endif // _TRIANGULATE_H_

#ifndef _ITF_DEPTH_RANGE_H_
#include "engine/scene/DepthRange.h"
#endif //_ITF_DEPTH_RANGE_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/Polyline.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{
	void GFX_GridFluid::drawSprite(GFXAdapter * _gfxDeviceAdapter, const Matrix44 &_World, f32 _px, f32 _py, f32 _width, f32 _height )
    {
        _gfxDeviceAdapter->setObjectMatrix(_World);

        VertexPCT quad[4];
		_width *= 0.5f;
		_height *= 0.5f;

        quad[0].setData( Vec3d( _px - _width, _py - _height, 0.0f ), Vec2d( 0.f, 1.0f ), COLOR_WHITE);
        quad[1].setData( Vec3d( _px - _width, _py + _height, 0.0f ), Vec2d( 0.f, 0.f ), COLOR_WHITE);
        quad[2].setData( Vec3d( _px + _width, _py - _height, 0.0f ), Vec2d( 1.0f, 1.0f ), COLOR_WHITE);
        quad[3].setData( Vec3d( _px + _width, _py + _height, 0.0f ), Vec2d( 1.0f, 0.0f ), COLOR_WHITE);

        _gfxDeviceAdapter->setVertexFormat(VertexFormat_PCT);
        _gfxDeviceAdapter->DrawPrimitive(m_drawCallCtx, GFX_TRIANGLE_STRIP, (void*)quad, 4);
    }

	// Inject new fluid in fluid texture.
	void GFX_GridFluid::renderModifierList(GFXAdapter * _gfxDeviceAdapter, f32 _dt, GFX_Zlist<GFX_GridFluidModifierList> &_modifierLists, ux _maskMode,
		                                   RenderTarget *_target, shaderEntryKey _shaderPS, bbool _bilinear)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		Matrix44 gridMatrix, gridMatrixInv, viewport, transform, transformStatic, transformVP, moveMatrix, moveT;
		gridMatrix.setIdentity();
        gridMatrix.setTranslation(m_roundedPos);
        gridMatrix.mulScale(m_boxSize.to3d(1.f));
		gridMatrixInv.inverse(gridMatrix);
		viewport.setIdentity();
		viewport.setTranslation(Vec3d(_target->getSizeX() * 0.5f, _target->getSizeY() * 0.5f, 0.0f));
		viewport.mulScale(Vec3d(f32(_target->getSizeX()), f32(_target->getSizeY()), 0.0f));
		DepthRange gridZRange(m_boxPos.z());

		ux n = _modifierLists.getNumberEntry();
		for (ux i = 0; i < n; i++)
		{
			const ZList_Node<GFX_GridFluidModifierList> & node = _modifierLists.getNodeAt(i);
			GFX_GridFluidModifierList *modifierList = node.m_primitive;
			if ( !modifierList->m_active || !gridZRange.contains(modifierList->getZ()) )
				continue;
			transformStatic.mul44(modifierList->m_matrix, gridMatrixInv);
			modifierList->update(_gfxDeviceAdapter, _dt);

			ux nbMod = modifierList->m_modList.size();
			for ( ux j = 0; j < nbMod; j++)
			{
				GFX_GridFluidModifier *modifier = &(modifierList->m_modList[j]);
				if ( modifier->m_active && (modifier->m_mode & _maskMode) && modifier->isValid(modifierList->m_curTime)
					 && isEmitterAccepted(modifier->m_emitterFilter) && m_fluidAABB.checkIntersect(modifier->m_AABB) )
				{
					const Vec3d &modCoord = modifier->m_boxPos;
					const Vec2d &modSize = modifier->m_boxSize; 

					moveMatrix.setIdentity();
					moveMatrix.setRotationZ(modifier->m_curRotation * -(MTH_PI / 180.0f));
					moveMatrix.setTranslation(modifier->m_curTranslation.to3d(0.f));
					moveMatrix.mulScale(modifier->m_curScale.to3d(1.f));
					transform.mul44(moveMatrix, transformStatic);
					transformVP.mul44(transform, viewport);

					f32 intensity = modifier->m_intensity * modifier->m_curIntensity * modifier->m_pulseCur * _dt;

					PS_GF_Quad quadconst;

					Texture* texture = static_cast<Texture*>(modifier->m_textureRessource.getResource());
					_gfxDeviceAdapter->bindTexture(TextureSampler0, texture, _bilinear);
					if ( _maskMode == GFX_GRID_MOD_MODE_FORCE )
					{
						if ( modifier->m_isExternal )
							intensity *= m_emitterFactors.m_externalForce;
						quadconst.color.set(modifier->m_force.x() * intensity, modifier->m_force.y() * intensity, modifier->m_forceTexFactor * intensity, -(modifier->m_forceTexFactor * intensity));
					}
					#ifndef ITF_FINAL
					else if ( _maskMode & GFX_GRID_MOD_MODE_FLUID_DEBUG )
					{
						quadconst.color.set(intensity, 0, 0, 1.0f);
					}
					else if ( _maskMode & GFX_GRID_MOD_MODE_FORCE_DEBUG )
					{
						quadconst.color.set(0, intensity, 0, 1.0f);
					}
					#endif
					else if ( _maskMode == GFX_GRID_MOD_MODE_BLOCKER )
					{
						quadconst.color.set(1.0f, 1.0f, 1.0f, 1.0f); // Not used.
					}
					else //if ( _maskMode == GFX_GRID_MOD_MODE_FLUID ) || ( _maskMode == GFX_GRID_MOD_MODE_ATTRACT )
					{
						if ( modifier->m_isExternal )
							intensity *= m_emitterFactors.m_externalFluid;

						if ( m_useRGBFluid )
						{
							Color &col = modifier->m_fluidColor;
							quadconst.color.set(col.getRed(), col.getGreen(), col.getBlue(), intensity * col.getAlpha());
						}
						else
							quadconst.color.set(1.0f, 1.0f, 1.0f, intensity);

						if ( m_useRGBFluid )
							m_drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ALPHA, GFX_ALPHAMASK_ADDALPHA);
						else
							m_drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ADDALPHA, GFX_ALPHAMASK_ADDALPHA);
					}

					f32 *pmove = (f32*)&transform;
					quadconst.transform[0].set(pmove[0], pmove[4], 0.0f, 0.0f);
					quadconst.transform[1].set(-pmove[1], -pmove[5], 0.0f, 0.0f);
					quadconst.transform[0].normalize3D();
					quadconst.transform[1].normalize3D();

					_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Quad, (f32*)&quadconst, 3);
					_gfxDeviceAdapter->setShaderGroup(fluidManager.mc_shader_Fluids, fluidManager.mc_entry_GF_PCT1_VS, _shaderPS);

					drawSprite(_gfxDeviceAdapter, transformVP, modCoord.x(), modCoord.y(), modSize.x(), modSize.y() );
				}
			}
		}
	}

	#define MaxPolyPoint 500

	void GFX_GridFluid::drawOpenPolyline(GFXAdapter * _gfxDeviceAdapter, PolyLine *polyLine )
    {
        VertexPCT vertexList[MaxPolyPoint];
		u32 nbVertex = 0;
		u32 nbSegment = polyLine->getEdgeData().getEdgeData().size();
		f32 texelSize = m_boxSize.x() / m_speedTexSizeX;
		f32 shift1 = texelSize * 0.5f;
		f32 shift2 = texelSize * -1.5f;
		for ( ux i = 0; i < nbSegment-1; i++)
		{
			if ( (!( polyLine->getHoleAt(i) & PolyLineEdge::Hole_Collision )) && ( nbVertex <= MaxPolyPoint-6 ) )
			{
				const Vec2d &posBegin = polyLine->getPosAt(i);
				const Vec2d &posEnd = polyLine->getPosAt(i+1);
				Vec3d normal3D;
				polyLine->getNormalAt(i, normal3D);
				Vec2d normal(normal3D.x(),normal3D.y());
				Vec2d b1, b2, e1, e2;
				b1 = posBegin + normal * shift1;
				b2 = posBegin + normal * shift2;
				e1 = posEnd + normal * shift1;
				e2 = posEnd + normal * shift2;
				vertexList[nbVertex+0].setData( b1.to3d(0.0f), Vec2d( 0.f, 0.f ), COLOR_WHITE);
				vertexList[nbVertex+1].setData( e1.to3d(0.0f), Vec2d( 0.f, 0.f ), COLOR_WHITE);
				vertexList[nbVertex+2].setData( e2.to3d(0.0f), Vec2d( 0.f, 0.f ), COLOR_WHITE);
				vertexList[nbVertex+3].setData( e2.to3d(0.0f), Vec2d( 0.f, 0.f ), COLOR_WHITE);
				vertexList[nbVertex+4].setData( b2.to3d(0.0f), Vec2d( 0.f, 0.f ), COLOR_WHITE);
				vertexList[nbVertex+5].setData( b1.to3d(0.0f), Vec2d( 0.f, 0.f ), COLOR_WHITE);
				nbVertex += 6;
			}
		}

        _gfxDeviceAdapter->DrawPrimitive(m_drawCallCtx, GFX_TRIANGLES, (void*)vertexList, nbVertex);
    }

	bbool GFX_GridFluid::drawClosePolyline(GFXAdapter * _gfxDeviceAdapter, PolyLine *polyLine )
    {
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
        VertexPCT vertexList[MaxPolyPoint];
		u32 nbVertex = 0;
		u32 nbSegment = polyLine->getEdgeData().getEdgeData().size();

		ITF_VECTOR<Vec2d> &pointList = fluidManager.m_pointList;
		ITF_VECTOR<u16> &polygone = fluidManager.m_polygone;

		pointList.clear();
		pointList.reserve(1000);
		polygone.clear();
		polygone.reserve(1000);

		// Check all segment are used.
		for ( ux i = 0; i < nbSegment-1; i++)
		{
			if ( polyLine->getHoleAt(i) & PolyLineEdge::Hole_Collision )
				return false;  // Render as open line.
			pointList.push_back(polyLine->getPosAt(i));
		}

		if ( pointList.size() < 3 )
			return false;         // Not enough segment to encompass a triangle.

		Triangulate::ProcessIndexes(pointList, polygone);

		if ( polygone.size() > MaxPolyPoint )
			return false; // Stop rendering : polygon too big.
		
		for ( ux i = 0; i < polygone.size(); ++i )
		{
			vertexList[nbVertex++].setData( pointList[polygone[i]].to3d(0.0f), Vec2d( 0.f, 0.f ), COLOR_WHITE);
		}

        _gfxDeviceAdapter->DrawPrimitive(m_drawCallCtx, GFX_TRIANGLES, (void*)vertexList, nbVertex);

		return true;
    }

	void GFX_GridFluid::renderPolyLines(GFXAdapter * _gfxDeviceAdapter)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		DepthRange gridZRange(m_boxPos.z());

		Matrix44 gridMatrix, gridMatrixInv, viewport, transformVP;
		gridMatrix.setIdentity();
        gridMatrix.setTranslation(m_roundedPos);
        gridMatrix.mulScale(m_boxSize.to3d(1.f));
		gridMatrixInv.inverse(gridMatrix);
		viewport.setIdentity();
		viewport.setTranslation(Vec3d(m_boundariesTarget->getSizeX() * 0.5f, m_boundariesTarget->getSizeY() * 0.5f, 0.0f));
		viewport.mulScale(Vec3d(f32(m_boundariesTarget->getSizeX()), f32(m_boundariesTarget->getSizeY()), 0.0f));
		transformVP.mul44(gridMatrixInv, viewport);
        _gfxDeviceAdapter->setObjectMatrix(transformVP);

        _gfxDeviceAdapter->setVertexFormat(VertexFormat_PCT);

		PS_GF_Quad quadconst;
		_gfxDeviceAdapter->bindTexture(TextureSampler0, _gfxDeviceAdapter->getWhiteOpaqueTexture(), true);
		quadconst.color.set(1.0f, 1.0f, 1.0f, 1.0f); // Not used.
		quadconst.transform[0].set(0.0f, 0.0f, 0.0f, 0.0f);
		quadconst.transform[1].set(0.0f, 0.0f, 0.0f, 0.0f);
		_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Quad, (f32*)&quadconst, sizeof(quadconst)/16);
		_gfxDeviceAdapter->setShaderGroup(fluidManager.mc_shader_Fluids, fluidManager.mc_entry_GF_PCT1_VS, fluidManager.mc_entry_GF_DrawBoundary_PS);

		ux nbPoly = fluidManager.m_polyLineList.size();
		for ( ux i = 0; i < nbPoly; ++i )
		{
			PolyLine *polyLine = fluidManager.m_polyLineList[i];
			if ( polyLine && isEmitterAccepted(polyLine->m_fluidEmitterFilter) && gridZRange.contains(polyLine->getDepth()) )
			{
				if (m_fluidAABB.checkIntersect(polyLine->getAABB()))
				{
					if (polyLine->isLooping())
					{
						bbool success = drawClosePolyline(_gfxDeviceAdapter, polyLine);
						if ( !success )
							drawOpenPolyline(_gfxDeviceAdapter, polyLine);
					}
					else
					{
						drawOpenPolyline(_gfxDeviceAdapter, polyLine);
					}
				}
			}
		}
	}

	void GFX_GridFluid::renderFluidPrimitives(GFXAdapter * _gfxDeviceAdapter, ux _maskMode)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		ux nbPrim = fluidManager.m_fluidPrimitiveTable.size();
		if ( !nbPrim )
			return;

		DepthRange gridZRange(m_boxPos.z());

		end2DRendering(_gfxDeviceAdapter);

		RenderPassContext passContext;
		passContext.getStateCache().setAlphaWrite(true);
		_gfxDeviceAdapter->SetDrawCallState(passContext.getStateCache(), 0xFFffFFff, btrue);

		Matrix44 viewMat, projMat;
		Vec3d viewTrans;
		WorldViewProj &worldViewProj = _gfxDeviceAdapter->m_worldViewProj;
		worldViewProj.push();
		viewTrans = m_roundedPos;
		viewTrans.z() += 10.0f;
		viewMat.setIdentity();
		viewMat.setTranslation(viewTrans);
		
        worldViewProj.setViewLookAtRH(viewTrans, m_roundedPos, Vec3d(0.0f,1.0f,0.0f));
		worldViewProj.setProjOrtho(m_boxSize.x() * -0.5f, m_boxSize.x() * 0.5f, m_boxSize.y() * -0.5f, m_boxSize.y() * 0.5f, -100.0f, 100.0f);

		f32 zCamRef = CAMERA->getPos().z();

		for ( ux i = 0; i < nbPrim; ++i )
		{
			GFX_GridFluidManager::PrimBatch &desc = fluidManager.m_fluidPrimitiveTable[i];
			GFXPrimitive *primitive = desc.m_primitive;

			if ( primitive && isEmitterAccepted(desc.m_filters)  && (desc.m_mode & _maskMode) && gridZRange.contains(desc.m_depth) )
			{
				if (m_fluidAABB.checkIntersect(desc.m_boundingVolume))
				{
					f32 drawZWorld = desc.m_depth;
					f32 drawZCam = drawZWorld - zCamRef;// inform the renderer the actual z position in camera space.
					f32 drawZCamAbs = f32_Abs(drawZCam);
					bbool lightingSave = _gfxDeviceAdapter->isLightingEnable();
					_gfxDeviceAdapter->setLightingEnable(false);
					fluidManager.m_areDrawingFluidPrimitive = true;
					fluidManager.m_primitiveFactor = m_emitterFactors.m_externalPrimitive;
					primitive->directDraw( passContext, _gfxDeviceAdapter, drawZCam, drawZWorld, drawZCamAbs );
					fluidManager.m_areDrawingFluidPrimitive = false;
					_gfxDeviceAdapter->setLightingEnable(lightingSave);
				}
			}
		}

		worldViewProj.pop();
		begin2DRendering(_gfxDeviceAdapter);
	}

} // namespace ITF


#endif // GFX_USE_GRID_FLUIDS
