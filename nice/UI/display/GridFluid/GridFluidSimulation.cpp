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


namespace ITF
{
	void GFX_GridFluid::clearTargets(GFXAdapter * _gfxDeviceAdapter)
	{
		begin2DRendering(_gfxDeviceAdapter);

		clearOneTarget(_gfxDeviceAdapter, m_fluidTarget, GFX_Vector4(0.0f,0.0f,0.0f,0.0f));
		clearOneTarget(_gfxDeviceAdapter, m_velocityTarget, GFX_Vector4(0.0f,0.0f,0.0f,0.0f));
		clearOneTarget(_gfxDeviceAdapter, m_boundariesTarget, GFX_Vector4(0.0f,0.0f,0.0f,0.0f));
		clearOneTarget(_gfxDeviceAdapter, m_pressureTarget, GFX_Vector4(0.0f,0.0f,0.0f,0.0f));

		end2DRendering(_gfxDeviceAdapter);
	}

	void GFX_GridFluid::clearOneTarget(GFXAdapter * _gfxDeviceAdapter, RenderTarget *_target, const GFX_Vector4 &_color)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
        _gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Init, (f32*)&_color, 1);
		drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_PCT1_VS, fluidManager.mc_entry_GF_Init_PS, _target, false );
	}

	void GFX_GridFluid::begin2DRendering(GFXAdapter * _gfxDeviceAdapter)
	{
		m_curTargetSizeX = 0;
		m_curTargetSizeY = 0;
		m_previousTargetSizeX = _gfxDeviceAdapter->getScreenWidth();
		m_previousTargetSizeY = _gfxDeviceAdapter->getScreenHeight();

		_gfxDeviceAdapter->setDefaultGFXPrimitiveParam();
		_gfxDeviceAdapter->setGfxMatDefault(m_drawCallCtx);

        m_drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY, GFX_ALPHAMASK_COPY);
		m_drawCallCtx.getStateCache().setAlphaWrite(true);

		_gfxDeviceAdapter->init2DRender();
	}
	void GFX_GridFluid::end2DRendering(GFXAdapter * _gfxDeviceAdapter)
	{
		_gfxDeviceAdapter->end2DRender();
		_gfxDeviceAdapter->forceScreenSize(m_previousTargetSizeX, m_previousTargetSizeY);
	}
	void GFX_GridFluid::drawQuad(GFXAdapter * _gfxDeviceAdapter, shaderEntryKey _entryKeyVS, shaderEntryKey _entryKeyPS,
		                         RenderTarget *&_target, bbool _bilinear, i32 _targetSampler, shaderDefineKey _vsDefine, shaderDefineKey _psDefine,
								 f32 _shiftU, f32 _shiftV, GFX_TEXADRESSMODE _addressModeU, GFX_TEXADRESSMODE _addressModeV )
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();

		RenderTarget *inputTarget = nullptr;
		if ( _targetSampler >= 0 )
		{
			inputTarget = _target;
			_target = fluidManager.getTempTarget(inputTarget->getSizeX(), inputTarget->getSizeY(), inputTarget->m_pixFormat);
			_gfxDeviceAdapter->bindTexture(_targetSampler, inputTarget, _bilinear, _addressModeU, _addressModeV);
		}

		setupTarget(_gfxDeviceAdapter, _target);

		_gfxDeviceAdapter->setShaderGroup(fluidManager.mc_shader_Fluids, _entryKeyVS, _entryKeyPS, _vsDefine, _psDefine);

        //_gfxDeviceAdapter->drawScreenQuad(m_drawCallCtx, 0.f, 0.f, (f32)m_curTargetSizeX, (f32)m_curTargetSizeY, 0.f, COLOR_WHITE);
        Matrix44 World;
        World.setIdentity();
        _gfxDeviceAdapter->setObjectMatrix(World);

        VertexPCT quad[4];
		f32 centroid = _gfxDeviceAdapter->m_centroidOffset;
		f32 width = (f32)m_curTargetSizeX;
		f32 height = (f32)m_curTargetSizeY;
        quad[0].setData( Vec3d( -centroid, centroid, 0.0f ), Vec2d( 0.f + _shiftU, 1.0f + _shiftV ), COLOR_WHITE);
        quad[1].setData( Vec3d( -centroid, height + centroid, 0.0f ), Vec2d( 0.f + _shiftU, 0.f + _shiftV ), COLOR_WHITE);
        quad[2].setData( Vec3d( width - centroid, centroid, 0.0f ), Vec2d( 1.0f + _shiftU, 1.0f + _shiftV ), COLOR_WHITE);
        quad[3].setData( Vec3d( width - centroid, height + centroid, 0.0f ), Vec2d( 1.0f + _shiftU, 0.0f + _shiftV ), COLOR_WHITE);

        _gfxDeviceAdapter->setVertexFormat(VertexFormat_PCT);
        _gfxDeviceAdapter->DrawPrimitive(m_drawCallCtx, GFX_TRIANGLE_STRIP, (void*)quad, 4);

		if ( inputTarget != nullptr)
		{
			fluidManager.storeTempTarget(inputTarget);
		}
	}

	void GFX_GridFluid::prerendering(GFXAdapter * _gfxDeviceAdapter, f32 _dt, u32 _zListViewID)
	{
		u32 newFrameCount = CURRENTFRAME;
		if ( m_frameCount != newFrameCount )
		{
			m_frameCount = newFrameCount;

			m_fluidAABB.setMinAndMax(Vec2d(m_boxPos.x(), m_boxPos.y()));
			m_fluidAABB.grow(m_boxSize.x() * 0.5f, m_boxSize.y() * 0.5f);

			GFX_Zlist<GFX_GridFluidModifierList> & modifierLists = _gfxDeviceAdapter->getZListManager().getZlistAt<GFX_ZLIST_GFLUID_MODIFIER>(_zListViewID);

			initTargets(_gfxDeviceAdapter);

			begin2DRendering(_gfxDeviceAdapter);

			computeRoundedCoord();

			// Move texture if necessary.
			moveTextures(_gfxDeviceAdapter);

			//only process if time is positive, or there will be no change
			if ( (_dt > 0.0f) && !m_pause && m_active)
			{
				m_time += _dt;
				if ( m_time >= 256.0f )
					m_time -= 256.0f;

				// Update boundary.
				updateArbitraryBoundary(_gfxDeviceAdapter, _dt, modifierLists);
				updateOffset(_gfxDeviceAdapter);	

				renderEmitterDebug(_gfxDeviceAdapter, _dt, modifierLists);
				renderAttractors(_gfxDeviceAdapter, _dt, modifierLists);

				if ( !m_useAttract )
				{
					//Inject fluid if necessary.
					injectFluid(_gfxDeviceAdapter, _dt, modifierLists);

					perturbVelocity(_gfxDeviceAdapter, _dt, modifierLists);

					updateVelocity(_gfxDeviceAdapter, _dt);

					advectVelocity(_gfxDeviceAdapter, _dt);
					diffuseVelocity(_gfxDeviceAdapter, _dt);

					computeDivergence(_gfxDeviceAdapter, _dt);
					updatePressure(_gfxDeviceAdapter, _dt);

					subtractPressureGradient(_gfxDeviceAdapter, _dt);

					applyAttractSpeed(_gfxDeviceAdapter, _dt);

					advectFluid(_gfxDeviceAdapter, _dt);
					diffuseFluid(_gfxDeviceAdapter, _dt);
				}
		
				applyAttractColor(_gfxDeviceAdapter, _dt);
			}

			processRequests(_gfxDeviceAdapter);

			end2DRendering(_gfxDeviceAdapter);

			freeTempTarget(_gfxDeviceAdapter);
		}
	}

	void GFX_GridFluid::computeRoundedCoord()
	{
		Vec2d texelSize;
		m_prevRoundedPos = m_roundedPos;
		texelSize.set( m_boxSize.x() / m_speedTexSizeX, m_boxSize.y() / m_speedTexSizeY );
		m_roundedPos.x() = f32_Floor( m_boxPos.x() / texelSize.x() ) * texelSize.x();
		m_roundedPos.y() = f32_Floor( m_boxPos.y() / texelSize.y() ) * texelSize.y();
		m_roundedPos.z() = m_boxPos.z();
		m_texelOffset.x() = ( (m_roundedPos.x() - m_prevRoundedPos.x()) / texelSize.x() ) / m_speedTexSizeX;
		m_texelOffset.y() = ( (m_roundedPos.y() - m_prevRoundedPos.y()) / texelSize.y() ) / m_speedTexSizeY;
		m_worldUVOffset.x() = m_roundedPos.x() / m_boxSize.x();
		m_worldUVOffset.y() = m_roundedPos.y() / m_boxSize.y();
	}

	// Setup a target in the GPU for 2D rendering, update constants depending of the target size.
	void GFX_GridFluid::setupTarget(GFXAdapter * _gfxDeviceAdapter, RenderTarget *_target)
	{
		if ( ( m_curTargetSizeX != _target->getSizeX() ) || ( m_curTargetSizeY != _target->getSizeY() ) )
		{
			_gfxDeviceAdapter->end2DRender();
			m_curTargetSizeX = _target->getSizeX();
			m_curTargetSizeY = _target->getSizeY();
			_gfxDeviceAdapter->forceScreenSize(m_curTargetSizeX, m_curTargetSizeY);
			_gfxDeviceAdapter->init2DRender();

			GFX_Vector4 vconst(1.0f / m_curTargetSizeX, 1.0f / m_curTargetSizeY, f32(m_curTargetSizeX), f32(m_curTargetSizeY));
			_gfxDeviceAdapter->setVertexShaderConstantF(VS_Attrib_GF_Common, (f32*)&vconst, 1);
			_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Common, (f32*)&vconst, 1);
		}

		RenderContext rtContext(_target);
		_gfxDeviceAdapter->setRenderContext(rtContext);
	}

	// Render force and fluid emitter in a debug target.
	void GFX_GridFluid::renderEmitterDebug(GFXAdapter * _gfxDeviceAdapter, f32 _dt, GFX_Zlist<GFX_GridFluidModifierList> &_modifierLists)
	{
#ifndef ITF_FINAL
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		if ( ( m_renderMode == GRDFLD_FluidOpaqueEmitter ) || ( m_renderMode == GRDFLD_VelocityEmitter ) )
		{
			if ( !m_emitterTarget)
				m_emitterTarget = fluidManager.getTempTarget(m_particleTexSizeX, m_particleTexSizeY, Texture::PF_RGBA );

			m_drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ADDALPHA, GFX_ALPHAMASK_NONE);

			setupTarget(_gfxDeviceAdapter, m_emitterTarget);
			_gfxDeviceAdapter->clear(GFX_CLEAR_COLOR);

			renderModifierList(_gfxDeviceAdapter, _dt, _modifierLists, GFX_GRID_MOD_MODE_FLUID | GFX_GRID_MOD_MODE_FLUID_DEBUG, m_fluidTarget, fluidManager.mc_entry_GF_DrawQuad_PS, true);
			renderModifierList(_gfxDeviceAdapter, _dt, _modifierLists, GFX_GRID_MOD_MODE_FORCE | GFX_GRID_MOD_MODE_FORCE_DEBUG, m_fluidTarget, fluidManager.mc_entry_GF_DrawQuad_PS, true);

			m_drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY, GFX_ALPHAMASK_COPY);
		}
		else
		{
			if ( m_emitterTarget )
			{
				fluidManager.storeTempTarget(m_emitterTarget);
				m_emitterTarget = nullptr;
			}
		}
#endif
	}

	// Inject new fluid in fluid texture.
	void GFX_GridFluid::injectFluid(GFXAdapter * _gfxDeviceAdapter, f32 _dt, GFX_Zlist<GFX_GridFluidModifierList> &_modifierLists)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		m_drawCallCtx.getStateCache().setAlphaWrite(true);

		setupTarget(_gfxDeviceAdapter, m_fluidTarget);

		renderModifierList(_gfxDeviceAdapter, _dt, _modifierLists, GFX_GRID_MOD_MODE_FLUID, m_fluidTarget, fluidManager.mc_entry_GF_DrawQuad_PS, true);

		renderFluidPrimitives(_gfxDeviceAdapter, GFX_GRID_MOD_MODE_FLUID);

		m_drawCallCtx.getStateCache().setAlphaWrite(true);
        m_drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY, GFX_ALPHAMASK_COPY);
	}

	// Prepare attract target if needed.
	void GFX_GridFluid::renderAttractors(GFXAdapter *_gfxDeviceAdapter, f32 _dt, GFX_Zlist<GFX_GridFluidModifierList> &_modifierLists)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		DepthRange gridZRange(m_boxPos.z());
		bbool useAttract = false;

		// First check if there is some attractor in fluid range.
		ux n = _modifierLists.getNumberEntry();
		for (ux i = 0; i < n; i++)
		{
			const ZList_Node<GFX_GridFluidModifierList> & node = _modifierLists.getNodeAt(i);
			GFX_GridFluidModifierList *modifierList = node.m_primitive;
			if ( !modifierList->m_active || !gridZRange.contains(modifierList->getZ()) )
				continue;

			ux nbMod = modifierList->m_modList.size();
			for ( ux j = 0; j < nbMod; j++)
			{
				GFX_GridFluidModifier *modifier = &(modifierList->m_modList[j]);
				if ( modifier->m_active && (modifier->m_mode & GFX_GRID_MOD_MODE_ATTRACT) && modifier->isValid(modifierList->m_curTime)
					 && isEmitterAccepted(modifier->m_emitterFilter) && m_fluidAABB.checkIntersect(modifier->m_AABB) )
				{
					useAttract = true;
					break;
				}
			}
			if ( useAttract )
				break;
		}

		if ( !useAttract )
		{
			ux nbPrim = fluidManager.m_fluidPrimitiveTable.size();
			for ( ux i = 0; i < nbPrim; ++i )
			{
				GFX_GridFluidManager::PrimBatch &desc = fluidManager.m_fluidPrimitiveTable[i];
				GFXPrimitive *primitive = desc.m_primitive;

				if ( primitive && isEmitterAccepted(desc.m_filters)  && (desc.m_mode & GFX_GRID_MOD_MODE_ATTRACT) && gridZRange.contains(desc.m_depth) && m_fluidAABB.checkIntersect(desc.m_boundingVolume) )
				{
					useAttract = true;
					break;
				}
			}
		}

		m_wasUsingAttract = m_useAttract;
		m_useAttract = useAttract;

		if ( useAttract )
		{
			m_attractSourceTarget = fluidManager.getTempTarget(m_particleTexSizeX, m_particleTexSizeY, Texture::PF_RGBA);
			m_attractSpeedTarget = fluidManager.getTempTarget(m_speedTexSizeX, m_speedTexSizeY, Texture::PF_RG_FLT16);

			// Render attractors.
			clearOneTarget(_gfxDeviceAdapter, m_attractSourceTarget, GFX_Vector4(m_neutralColor.getRed(), m_neutralColor.getGreen(), m_neutralColor.getBlue(),0.f));
//			_gfxDeviceAdapter->clear(GFX_CLEAR_COLOR,m_neutralColor.getRed(), m_neutralColor.getGreen(), m_neutralColor.getBlue(),0.f);

			setupTarget(_gfxDeviceAdapter, m_attractSourceTarget);
			m_drawCallCtx.getStateCache().setAlphaWrite(true);

			renderModifierList(_gfxDeviceAdapter, _dt, _modifierLists, GFX_GRID_MOD_MODE_ATTRACT, m_attractSourceTarget, fluidManager.mc_entry_GF_DrawQuad_PS, true);

			renderFluidPrimitives(_gfxDeviceAdapter, GFX_GRID_MOD_MODE_ATTRACT);

			// Compute attractor speed.
			_gfxDeviceAdapter->bindTexture(AttractSourceSampler, m_attractSourceTarget);
			drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_PCT1_VS, fluidManager.mc_entry_GF_AttractCopy_PS, m_attractSpeedTarget, true );

			blurTarget(_gfxDeviceAdapter, m_attractSpeedTarget, nullptr, 10.0f, 20, 0.8f);

			drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_PCT5_VS, fluidManager.mc_entry_GF_AttractComputeSpeed_PS, m_attractSpeedTarget, true, AttractSpeedSampler );

			_gfxDeviceAdapter->bindTexture(AttractSourceSampler, m_attractSourceTarget);
			m_divFieldTarget = fluidManager.getTempTarget(m_speedTexSizeX, m_speedTexSizeY, Texture::PF_R_FLT16);
			drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_PCT1_VS, fluidManager.mc_entry_GF_AttractCopy_PS, m_divFieldTarget, true );
			updatePressure(_gfxDeviceAdapter, _dt);

			m_drawCallCtx.getStateCache().setAlphaWrite(true);
			m_drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY, GFX_ALPHAMASK_COPY);
		}
		else if (m_wasUsingAttract)
		{
			clearOneTarget(_gfxDeviceAdapter, m_velocityTarget, GFX_Vector4(0.0f,0.0f,0.0f,0.0f));
			clearOneTarget(_gfxDeviceAdapter, m_pressureTarget, GFX_Vector4(0.0f,0.0f,0.0f,0.0f));
		}
	}

	void GFX_GridFluid::applyAttractSpeed(GFXAdapter * _gfxDeviceAdapter, f32 _dt)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();

		if ( m_attractSpeedTarget )
		{
			GFX_Vector4 attractConst( m_attractForce * _dt * 10.0f, m_attractExpand * _dt * 10.0f, m_attractColorize * _dt * 10.0f, 0.0f);
			_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Attract, (f32*)&attractConst, sizeof(attractConst)/16);

			_gfxDeviceAdapter->bindTexture(AttractSpeedSampler, m_attractSpeedTarget, false);
			drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_PCT1_VS, fluidManager.mc_entry_GF_AttractApplySpeed_PS, m_velocityTarget, false, VelocitySampler );
		}
	}
	void GFX_GridFluid::applyAttractColor(GFXAdapter * _gfxDeviceAdapter, f32 _dt)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		shaderDefineKey vsDefine = 0;
		shaderDefineKey psDefine = 0;

		if ( m_attractSourceTarget )
		{
			PS_GF_Attract attractConstPS;
			attractConstPS.param.set( m_attractForce * _dt * 10.0f, m_attractExpand * _dt * 10.0f, m_attractColorize * _dt * 10.0f, 0.0f);
			attractConstPS.advect.set(_dt * 512.0f / m_particleTexSizeX, _dt * 512.0f / m_particleTexSizeY, powf(1.0f - m_FluidLoss / 100.0f, _dt), 1.0f);
			attractConstPS.neutralColor.set(m_neutralColor.getRed(), m_neutralColor.getGreen(), m_neutralColor.getBlue(), 1.0f);

			m_attractNoise.setupNoise(_gfxDeviceAdapter, m_time, _dt, vsDefine, psDefine, &attractConstPS.noise, m_worldUVOffset);

			_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Attract, (f32*)&attractConstPS, sizeof(attractConstPS)/16);

			if ( m_useRGBFluid )
				psDefine |= fluidManager.mc_define_GF_UseRGBFluid;

			_gfxDeviceAdapter->bindTexture(AttractSourceSampler, m_attractSourceTarget, false);
			_gfxDeviceAdapter->bindTexture(AttractSpeedSampler, m_attractSpeedTarget, false);
			_gfxDeviceAdapter->bindTexture(PressureSampler, m_pressureTarget, true);
			drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_AttractApplyColor_VS, fluidManager.mc_entry_GF_AttractApplyColor_PS, m_fluidTarget, false, FluidSampler, vsDefine, psDefine );
		}
	}

	// Add perturbation in velocity texture.
	void GFX_GridFluid::perturbVelocity(GFXAdapter * _gfxDeviceAdapter, f32 _dt, GFX_Zlist<GFX_GridFluidModifierList> &_modifierLists)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
        m_drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ADDALPHA, GFX_ALPHAMASK_NONE);

		setupTarget(_gfxDeviceAdapter, m_velocityTarget);

		renderModifierList(_gfxDeviceAdapter, _dt, _modifierLists, GFX_GRID_MOD_MODE_FORCE, m_velocityTarget, fluidManager.mc_entry_GF_DrawVelocity_PS, true);

        m_drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY, GFX_ALPHAMASK_COPY);
	}

	// Draw boundary texture.
	void GFX_GridFluid::updateArbitraryBoundary(GFXAdapter * _gfxDeviceAdapter, f32 _dt, GFX_Zlist<GFX_GridFluidModifierList> &_modifierLists)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
        m_drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ADDALPHA, GFX_ALPHAMASK_NONE);

		setupTarget(_gfxDeviceAdapter, m_boundariesTarget);
		_gfxDeviceAdapter->clear(GFX_CLEAR_COLOR,0.f,0.f,0.f,0.f);

		renderModifierList(_gfxDeviceAdapter, _dt, _modifierLists, GFX_GRID_MOD_MODE_BLOCKER, m_boundariesTarget, fluidManager.mc_entry_GF_DrawBoundary_PS, false);

		renderPolyLines(_gfxDeviceAdapter);

        m_drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY, GFX_ALPHAMASK_COPY);
	}

	// Compute boundary offset texture from boundary.
	void GFX_GridFluid::updateOffset(GFXAdapter * _gfxDeviceAdapter)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		f32 alpha = 1.0f / 11.0f;
		f32 beta = (1.0f - alpha) / 8.0f;
		GFX_Vector4 psBlurConst(alpha, beta, 0.0f, 0.0f);
		_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Blur, (f32*)&psBlurConst, 1);

		GFX_Vector4 vsBlurConst[4];
		vsBlurConst[0].set(-0.5f, -1.5f, +0.5f, -1.5f);
		vsBlurConst[1].set(-0.5f, +1.5f, +0.5f, +1.5f);
		vsBlurConst[2].set(-1.5f, -0.5f, -1.5f, +0.5f);
		vsBlurConst[3].set(+1.5f, -0.5f, +1.5f, +0.5f);
		_gfxDeviceAdapter->setVertexShaderConstantF(VS_Attrib_GF_Blur, (f32*)&vsBlurConst, 4);

		_gfxDeviceAdapter->bindTexture(TextureSampler0, m_boundariesTarget);
		drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_CalculateOffsets_VS, fluidManager.mc_entry_GF_CalculateOffsets_PS, m_boundariesTarget, bfalse, BoundariesSampler );
	}

	// Translate textures when the grid is moving.
	void GFX_GridFluid::moveTextures(GFXAdapter * _gfxDeviceAdapter)
	{
		if ( !( f32_IsNull(m_texelOffset.x()) && f32_IsNull(m_texelOffset.y()) ) )
		{
			GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();

			PS_GF_Quad quadconst;
			quadconst.color.set(1.0f, 1.0f, 1.0f, 1.0f);
			_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Quad, (f32*)&quadconst, 1);

			drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_PCT1_VS, fluidManager.mc_entry_GF_DrawQuad_PS, m_fluidTarget, bfalse, TextureSampler0,
				     0, 0, m_texelOffset.x(), -m_texelOffset.y(), GFX_TEXADRESSMODE_BORDER, GFX_TEXADRESSMODE_BORDER );
			drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_PCT1_VS, fluidManager.mc_entry_GF_DrawQuad_PS, m_velocityTarget, bfalse, TextureSampler0,
				     0, 0, m_texelOffset.x(), -m_texelOffset.y(), GFX_TEXADRESSMODE_BORDER, GFX_TEXADRESSMODE_BORDER );
			drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_PCT1_VS, fluidManager.mc_entry_GF_DrawQuad_PS, m_pressureTarget, bfalse, TextureSampler0,
				     0, 0, m_texelOffset.x(), -m_texelOffset.y(), GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP );
		}
	}

	ITF_INLINE f32 f32_SignFrac(f32 _value)
	{
		return _value - f32_Trunc(_value);
	}

	void GFX_GridFluidNoise::setupNoise(GFXAdapter * _gfxDeviceAdapter, f32 _time, f32 _dt, shaderDefineKey &vsDefine, shaderDefineKey &psDefine,
		                                GFX_Vector4 *_psConst, const Vec2d &_worldUVOffset)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		if ( m_textureRessource.isValid() && ( (m_intensity > 0.0f) || (m_modulation > 0.0f) ) )
		{
			Texture *noiseTexture = static_cast<Texture*>(m_textureRessource.getResource());
			_gfxDeviceAdapter->bindTexture(GFX_GridFluid::NoiseSampler, noiseTexture, true, GFX_TEXADRESSMODE_WRAP, GFX_TEXADRESSMODE_WRAP);

			GFX_Vector4 noiseconstVS(m_scaleX, m_scaleY, m_speedX * m_scaleX * _time + f32_SignFrac(_worldUVOffset.x() * m_scaleX), m_speedY * m_scaleY * _time - f32_SignFrac(_worldUVOffset.y() * m_scaleY));
			_gfxDeviceAdapter->setVertexShaderConstantF(VS_Attrib_GF_Noise, (f32*)&noiseconstVS, 1);

			f32 noiseIntensity = m_intensity * _dt * 1000.0f;
			_psConst[0].set(noiseIntensity, m_freq * _time, m_modulation * _dt * 1000.0f, 0.0f);

			vsDefine |= fluidManager.mc_define_GF_UseNoise;
			psDefine |= fluidManager.mc_define_GF_UseNoise;
		}
	}

	// Update velocity : process weight + boundary + dissipation + noise on velocity texture
	void GFX_GridFluid::updateVelocity(GFXAdapter * _gfxDeviceAdapter, f32 _dt)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		shaderDefineKey vsDefine = 0;
		shaderDefineKey psDefine = 0;

		PS_GF_UpdateVelocity updateconst;
		ITF_MemSet(&updateconst, 0, sizeof(updateconst));

		m_velocityNoise.setupNoise(_gfxDeviceAdapter, m_time, _dt, vsDefine, psDefine, &updateconst.noise, m_worldUVOffset);

		updateconst.param.set(m_weight * _dt, powf(1.0f - m_VelocityLoss / 100.0f, _dt), 0.0f, 0.0f);
		_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_UpdateVelocity, (f32*)&updateconst, 2);
		
		_gfxDeviceAdapter->bindTexture(FluidSampler, m_fluidTarget, false);
		_gfxDeviceAdapter->bindTexture(BoundariesSampler, m_boundariesTarget, false);

		if ( m_useRGBFluid )
			psDefine |= fluidManager.mc_define_GF_UseRGBFluid;

		drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_Noise_VS, fluidManager.mc_entry_GF_UpdateVelocity_PS,
			     m_velocityTarget, false, VelocitySampler, vsDefine, psDefine );
	}

	// Advect velocity itself.
	void GFX_GridFluid::advectVelocity(GFXAdapter * _gfxDeviceAdapter, f32 _dt)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
        GFX_Vector4 advectConst(_dt * 128.0f / m_speedTexSizeX, _dt * 128.0f / m_speedTexSizeY, 1.0f, 10000.0f);
		_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Advect, (f32*)&advectConst, 1);

		_gfxDeviceAdapter->bindTexture(VelocitySampler, m_velocityTarget);
		_gfxDeviceAdapter->bindTexture(BoundariesSampler, m_boundariesTarget, false);

		drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_PCT1_VS, fluidManager.mc_entry_GF_AdvectVelocity_PS, m_velocityTarget, true, FluidSampler );     // Hack to apply advection to velocity itself.
	}

	void GFX_GridFluid::blurTarget(GFXAdapter * _gfxDeviceAdapter, RenderTarget *_target, RenderTarget *_source, f32 _intensity, u32 nbIter, f32 threshold)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		/**********/
		// First pass
		f32 alpha = 1.0f / (1.0f + _intensity);
		f32 beta = (1.0f - alpha) / 8.0f;
		GFX_Vector4 psBlurConst(alpha, beta, 0.0f, 0.0f);
		_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Blur, (f32*)&psBlurConst, 1);

		GFX_Vector4 vsBlurConst[4];
		vsBlurConst[0].set(-0.5f, -1.5f, +0.5f, -1.5f);
		vsBlurConst[1].set(-0.5f, +1.5f, +0.5f, +1.5f);
		vsBlurConst[2].set(-1.5f, -0.5f, -1.5f, +0.5f);
		vsBlurConst[3].set(+1.5f, -0.5f, +1.5f, +0.5f);
		_gfxDeviceAdapter->setVertexShaderConstantF(VS_Attrib_GF_Blur, (f32*)&vsBlurConst, 4);
		for (u32 i=0; i<nbIter; i++)
		{
			if ( _source && (i==0) )
			{
				_gfxDeviceAdapter->bindTexture(TextureSampler0, _source);
				drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_Blur_VS, fluidManager.mc_entry_GF_Blur_PS, _target, true );
			}
			else
				drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_Blur_VS, fluidManager.mc_entry_GF_Blur_PS, _target, true, TextureSampler0 );
		}

		/**********/
		// Second pass
		if ( alpha > threshold )
			return;
		alpha += 1.0f - threshold;
		beta = (1.0f - alpha) / 8.0f;
		psBlurConst.set(alpha, beta, 0.0f, 0.0f);
		_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Blur, (f32*)&psBlurConst, 1);

		vsBlurConst[0].set(-1.5f, -2.5f, +1.5f, -2.5f);
		vsBlurConst[1].set(-1.5f, +2.5f, +1.5f, +2.5f);
		vsBlurConst[2].set(-2.5f, -1.5f, -2.5f, +1.5f);
		vsBlurConst[3].set(+2.5f, -1.5f, +2.5f, +1.5f);
		_gfxDeviceAdapter->setVertexShaderConstantF(VS_Attrib_GF_Blur, (f32*)&vsBlurConst, 4);
		for (u32 i=0; i<nbIter; i++)
		{
			drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_Blur_VS, fluidManager.mc_entry_GF_Blur_PS, _target, true, TextureSampler0 );
		}

		/**********/
		// Third pass
		if ( alpha > threshold )
			return;
		alpha += 1.0f - threshold;
		beta = (1.0f - alpha) / 8.0f;
		psBlurConst.set(alpha, beta, 0.0f, 0.0f);
		_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Blur, (f32*)&psBlurConst, 1);

		vsBlurConst[0].set(-1.5f, -3.5f, +1.5f, -3.5f);
		vsBlurConst[1].set(-1.5f, +3.5f, +1.5f, +3.5f);
		vsBlurConst[2].set(-3.5f, -1.5f, -3.5f, +1.5f);
		vsBlurConst[3].set(+3.5f, -1.5f, +3.5f, +1.5f);
		_gfxDeviceAdapter->setVertexShaderConstantF(VS_Attrib_GF_Blur, (f32*)&vsBlurConst, 4);
		for (u32 i=0; i<nbIter; i++)
		{
			drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_Blur_VS, fluidManager.mc_entry_GF_Blur_PS, _target, true, TextureSampler0 );
		}
	}

	// Viscosity on velocity (is a blur).
	void GFX_GridFluid::diffuseVelocity(GFXAdapter * _gfxDeviceAdapter, f32 _dt)
	{
		float curViscosity = _dt * m_viscosity * 60.0f;
		if ( curViscosity < 0.001f )
			return;

		blurTarget(_gfxDeviceAdapter, m_velocityTarget, nullptr, curViscosity, 2, 0.8f);
	}

	void GFX_GridFluid::computeDivergence(GFXAdapter * _gfxDeviceAdapter, f32 _dt)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();

		m_divFieldTarget = fluidManager.getTempTarget(m_speedTexSizeX, m_speedTexSizeY, Texture::PF_R_FLT16);

		// Calculate Divergence Field 
		GFX_Vector4 divFieldConst(1.0f, 1.0f, 0.0f, 0.0f);
		_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_DivField, (f32*)&divFieldConst, 1);

		_gfxDeviceAdapter->bindTexture(VelocitySampler, m_velocityTarget, false);

		drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_PCT5_VS, fluidManager.mc_entry_GF_DivField_PS, m_divFieldTarget, false );

	}

	void GFX_GridFluid::updatePressure(GFXAdapter * _gfxDeviceAdapter, f32 _dt)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();

		// Find pressure using jacobi iterations
		f32 alpha = -m_pressureDiffusion;
		f32 beta = (1.0f / 4.0f) * 0.99f; // 0.99 => Reduce previous frame intensity.
		GFX_Vector4 jacobiConst(alpha, beta, 0.0f, 0.0f);
		_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Jacobi, (f32*)&jacobiConst, 1);

		for (u32 i=0;i<m_nbIter;i++)
		{
			_gfxDeviceAdapter->bindTexture(JacobiBSampler, m_divFieldTarget, false);

			if ( i != 0)
			{
				f32 alpha = -0.25f / 4.0f;
				f32 beta = 1.0f / 4.0f;
				GFX_Vector4 jacobiConst(alpha, beta, 0.0f, 0.0f);
				_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Jacobi, (f32*)&jacobiConst, 1);
			}

			drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_PCT5_VS, fluidManager.mc_entry_GF_F1Jacobi_PS, m_pressureTarget, false, JacobiXSampler );
		}

		fluidManager.storeTempTarget(m_divFieldTarget);
		m_divFieldTarget = nullptr;
	}

	void GFX_GridFluid::subtractPressureGradient(GFXAdapter * _gfxDeviceAdapter, f32 _dt)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		GFX_Vector4 subPressureConst( m_pressureIntensity, 1.0f, 0.0f, 0.0f);
		_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_SubPressure, (f32*)&subPressureConst, 1);

		_gfxDeviceAdapter->bindTexture(PressureSampler, m_pressureTarget, false);

		m_drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_SUB, GFX_ALPHAMASK_NONE);
		drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_PCT5_VS, fluidManager.mc_entry_GF_SubtractPressureGradient_PS, m_velocityTarget, false );
        m_drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY, GFX_ALPHAMASK_COPY);
	}

	void GFX_GridFluid::advectFluid(GFXAdapter * _gfxDeviceAdapter, f32 _dt)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		shaderDefineKey vsDefine = 0;
		shaderDefineKey psDefine = 0;

		PS_GF_Advect advectConst;
		ITF_MemSet(&advectConst, 0, sizeof(advectConst));

		m_fluidNoise.setupNoise(_gfxDeviceAdapter, m_time, _dt, vsDefine, psDefine, &advectConst.noise, m_worldUVOffset);

		advectConst.param.set(_dt * 512.0f / m_particleTexSizeX, _dt * 512.0f / m_particleTexSizeY, powf(1.0f - m_FluidLoss / 100.0f, _dt), 1.0f);
		advectConst.neutralColor.set(m_neutralColor.getRed(), m_neutralColor.getGreen(), m_neutralColor.getBlue(), 1.0f);
		_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Advect, (f32*)&advectConst, sizeof(PS_GF_Advect) / 16);

		if ( m_useRGBFluid )
			psDefine |= fluidManager.mc_define_GF_UseRGBFluid;
		_gfxDeviceAdapter->bindTexture(VelocitySampler, m_velocityTarget);
		_gfxDeviceAdapter->bindTexture(BoundariesSampler, m_boundariesTarget, false);

		drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_Noise_VS, fluidManager.mc_entry_GF_AdvectFluid_PS, m_fluidTarget, true,
			     FluidSampler, vsDefine, psDefine );
	}
	void GFX_GridFluid::diffuseFluid(GFXAdapter * _gfxDeviceAdapter, f32 _dt)
	{
		float curBlur = _dt * m_fluidDiffusion * 60.0f;
		if ( curBlur < 0.001f )
			return;

		blurTarget(_gfxDeviceAdapter, m_fluidTarget, nullptr, curBlur, 2, 0.8f);
	}

	void GFX_GridFluid::freeTempTarget(GFXAdapter * _gfxDeviceAdapter)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		if ( m_attractSourceTarget )
		{
			fluidManager.storeTempTarget(m_attractSourceTarget);
			m_attractSourceTarget = nullptr;
		}
		if ( m_attractSpeedTarget )
		{
			fluidManager.storeTempTarget(m_attractSpeedTarget);
			m_attractSpeedTarget = nullptr;
		}
	}

	void GFX_GridFluid::processRequests(GFXAdapter * _gfxDeviceAdapter)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		for ( ux i = 0; i < m_requestList.size(); i++ )
		{
			Request &request = m_requestList[i];
			if ( request.m_status == GRD_FLD_REQUEST_GPU_COMPUTING )
			{
				retrieveGPURequest(_gfxDeviceAdapter, request);
				if ( request.m_target )
				{
					fluidManager.storeTempTarget(request.m_target);
					request.m_target = NULL;
				}
				request.m_status = GRD_FLD_REQUEST_PROCESSED;
			}
			else if ( request.m_status == GRD_FLD_REQUEST_NOT_PROCESSED )
			{
				sendGPURequest(_gfxDeviceAdapter, request);
				request.m_status = GRD_FLD_REQUEST_GPU_COMPUTING;
			}
		}
	}

	void GFX_GridFluid::sendGPURequest(GFXAdapter * _gfxDeviceAdapter, Request &request)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		Matrix44 gridMatrix, gridMatrixInv, viewport, transform;
		Vec3d uvWorld[4], uvGrid[4];

		// Compute world coordinates.
		uvWorld[0] = ((request.m_pos - request.m_halfWidth) + request.m_halfHeight).to3d(0.0f);
		uvWorld[1] = ((request.m_pos + request.m_halfWidth) + request.m_halfHeight).to3d(0.0f);
		uvWorld[2] = ((request.m_pos + request.m_halfWidth) - request.m_halfHeight).to3d(0.0f);
		uvWorld[3] = ((request.m_pos - request.m_halfWidth) - request.m_halfHeight).to3d(0.0f);

		// Transform world coordinates to grid UV coordinates.
		gridMatrix.setIdentity();
        gridMatrix.setTranslation(m_roundedPos);
        gridMatrix.mulScale(m_boxSize.to3d(1.f) * 0.5f);
		gridMatrixInv.inverse(gridMatrix);
		viewport.setIdentity();
		viewport.setTranslation(Vec3d(0.5f, 0.5f, 0.0f));
		viewport.mulScale(Vec3d(0.5f, -0.5f, 0.0f));
		transform.mul44(gridMatrixInv, viewport);
		for ( ux i = 0; i < 4; i++ )
		{
			transform.transformPoint(uvGrid[i], uvWorld[i]);
		}

		// Choose initial target size.
		Vec3d gridWidth, gridHeight;
		gridWidth = uvGrid[1] - uvGrid[0];
		gridHeight = uvGrid[2] - uvGrid[1];
		f32 size = ITF::Max( gridWidth.norm() * m_particleTexSizeX, gridHeight.norm() * m_particleTexSizeY );
		u32 captureSize = 4;
		while ( ( captureSize < size ) && (captureSize < 64) ) captureSize *= 2;
		RenderTarget *target0 = fluidManager.getTempTarget(captureSize, captureSize, Texture::PF_R_FLT32);

		setupTarget(_gfxDeviceAdapter, target0);

		// Fill first quad.
        VertexPCT vertexList[4];
		f32 centroid = _gfxDeviceAdapter->m_centroidOffset;
        vertexList[0].setData( Vec3d( -centroid, centroid, 0.0f ), Vec2d( uvGrid[0].x(), uvGrid[0].y() ), COLOR_WHITE);
        vertexList[1].setData( Vec3d( -centroid, captureSize + centroid, 0.0f ), Vec2d( uvGrid[1].x(), uvGrid[1].y() ), COLOR_WHITE);
        vertexList[2].setData( Vec3d( captureSize - centroid, centroid, 0.0f ), Vec2d( uvGrid[2].x(), uvGrid[2].y() ), COLOR_WHITE);
        vertexList[3].setData( Vec3d( captureSize - centroid, captureSize + centroid, 0.0f ), Vec2d( uvGrid[3].x(), uvGrid[3].y() ), COLOR_WHITE);

		shaderDefineKey vsDefine = 0;
		shaderDefineKey psDefine = 0;
		if ( m_useRGBFluid )
			psDefine |= fluidManager.mc_define_GF_UseRGBFluid;

		_gfxDeviceAdapter->setShaderGroup(fluidManager.mc_shader_Fluids, fluidManager.mc_entry_GF_PCT1_VS, fluidManager.mc_entry_GF_RequestCopy_PS, vsDefine, psDefine);

		_gfxDeviceAdapter->bindTexture(TextureSampler0, m_fluidTarget, btrue, GFX_TEXADRESSMODE_BORDER, GFX_TEXADRESSMODE_BORDER);

		PS_GF_Quad quadconst;
		quadconst.color.set(1.0f, 1.0f, 1.0f, 1.0f);
		_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_Quad, (f32*)&quadconst, 1);

		Matrix44 World;
        World.setIdentity();
        _gfxDeviceAdapter->setObjectMatrix(World);

        _gfxDeviceAdapter->setVertexFormat(VertexFormat_PCT);
        _gfxDeviceAdapter->DrawPrimitive(m_drawCallCtx, GFX_TRIANGLE_STRIP, (void*)vertexList, 4);

		// Reduce target.
		while ( target0->getSizeX() > 4 )
		{
			u32 newCaptureSize = ITF::Max(target0->getSizeX() / 2, u32(1));
			RenderTarget *target1 = fluidManager.getTempTarget(newCaptureSize, newCaptureSize, Texture::PF_R_FLT32);

			_gfxDeviceAdapter->bindTexture(TextureSampler0, target0);
			drawQuad(_gfxDeviceAdapter, fluidManager.mc_entry_GF_PCT1_VS, fluidManager.mc_entry_GF_DrawQuad_PS, target1, false );

			fluidManager.storeTempTarget(target0);
			target0 = target1;
		}

		_gfxDeviceAdapter->prepareLockableRTAccess(target0);
		request.m_target = target0;
	}

	void GFX_GridFluid::retrieveGPURequest(GFXAdapter * _gfxDeviceAdapter, Request &request)
	{
		LOCKED_TEXTURE lockInfo;
		f32 sum = 0.0f;
		_gfxDeviceAdapter->lockRenderTarget(request.m_target,&lockInfo);
		ux sizeX = request.m_target->getSizeX();
		ux sizeY = request.m_target->getSizeY();
		f32 *ptr = (f32*)lockInfo.mp_Bits;
		for ( ux y = 0; y < sizeY; y++ )
		{
			ptr = (f32*)( ((u8*)lockInfo.mp_Bits) + lockInfo.m_pitch * y );
			for ( ux x = 0; x < sizeX; x++ )
			{
				sum += *(ptr++);
			}
		}
		
		_gfxDeviceAdapter->unlockRenderTarget(request.m_target);

		sum *= 1.0f / f32(sizeX * sizeY);
		request.m_result = sum;
	}
} // namespace ITF


#endif // GFX_USE_GRID_FLUIDS
