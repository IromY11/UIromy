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

namespace ITF
{
	///// constructor/destructor --------------------------------------------------------------
	GFX_GridFluidManager::GFX_GridFluidManager()
	{
		m_fluidPrimitiveTable.reserve(20);
		m_areDrawingFluidPrimitive = false;
		m_primitiveFactor = 1.0f;
	}
	GFX_GridFluidManager::~GFX_GridFluidManager()
	{
	}

	void GFX_GridFluidManager::releaseAllTargets()
	{
		ux nbTarget = m_tempTargetTable.size();
		for ( ux i = 0; i < nbTarget; ++i )
		{
			RenderTarget *target = m_tempTargetTable[i];
			target->release();
		}
		m_tempTargetTable.clear();
	}

	// Get a temporary render target.
	RenderTarget *GFX_GridFluidManager::getTempTarget(ux _width, ux _height, Texture::PixFormat _format)
	{
		ux nbTarget = m_tempTargetTable.size();
		for ( ux i = 0; i < nbTarget; ++i )
		{
			RenderTarget *target = m_tempTargetTable[i];
			if ( ( target->getSizeX() == _width ) && ( target->getSizeY() == _height ) && ( target->m_pixFormat == _format ) )
			{
				m_tempTargetTable.removeAtUnordered(i);
				return target;
			}
		}

		RenderTargetInfo RTInfo(_width, _height, 1, _format);
		RenderTarget *target = GFX_ADAPTER->createRenderTarget(RTInfo);

		return target;
	}

	// Store an unused temporary texture.
	void GFX_GridFluidManager::storeTempTarget(RenderTarget *_target)
	{
		m_tempTargetTable.push_back(_target);
	}

	bbool GFX_GridFluidManager::loadShader(GFXAdapter_shaderManager& _shaderManager)
	{
        bbool loadSuccess = _shaderManager.loadShaderGroup("GridFluid.fx");

		// Fluids fx
		mc_shader_Fluids = _shaderManager.getShaderKey("GridFluid.fx");
		if(mc_shader_Fluids != shaderGroupKey_invalid)
		{
			ITF_shader* shadergroup = _shaderManager.getShaderGroup(mc_shader_Fluids);    
			mc_entry_GF_PCT1_VS = shadergroup->getEntryID("GF_PCT1_VS", bfalse);
			mc_entry_GF_PCT5_VS = shadergroup->getEntryID("GF_PCT5_VS", bfalse);
			mc_entry_GF_Noise_VS = shadergroup->getEntryID("GF_Noise_VS", bfalse);
			mc_entry_GF_DrawFluid_VS = shadergroup->getEntryID("GF_DrawFluid_VS", bfalse);
			mc_entry_GF_Blur_VS = shadergroup->getEntryID("GF_Blur_VS", bfalse);
			mc_entry_GF_CalculateOffsets_VS = shadergroup->getEntryID("GF_CalculateOffsets_VS", bfalse);
			mc_entry_GF_AttractApplyColor_VS = shadergroup->getEntryID("GF_AttractApplyColor_VS", bfalse);
			mc_entry_GF_Init_PS = shadergroup->getEntryID("GF_Init_PS", btrue);
			mc_entry_GF_CalculateOffsets_PS = shadergroup->getEntryID("GF_CalculateOffsets_PS", btrue);
			mc_entry_GF_UpdateVelocity_PS = shadergroup->getEntryID("GF_UpdateVelocity_PS", btrue);
			mc_entry_GF_AdvectFluid_PS = shadergroup->getEntryID("GF_AdvectFluid_PS", btrue);
			mc_entry_GF_AdvectVelocity_PS = shadergroup->getEntryID("GF_AdvectVelocity_PS", btrue);
			mc_entry_GF_F4Jacobi_PS = shadergroup->getEntryID("GF_F4Jacobi_PS", btrue);
			mc_entry_GF_F1Jacobi_PS = shadergroup->getEntryID("GF_F1Jacobi_PS", btrue);
			mc_entry_GF_DivField_PS = shadergroup->getEntryID("GF_DivField_PS", btrue);
			mc_entry_GF_SubtractPressureGradient_PS = shadergroup->getEntryID("GF_SubtractPressureGradient_PS", btrue);
			mc_entry_GF_DrawQuad_PS = shadergroup->getEntryID("GF_DrawQuad_PS", btrue);
			mc_entry_GF_DrawVelocity_PS = shadergroup->getEntryID("GF_DrawVelocity_PS", btrue);
			mc_entry_GF_DrawBoundary_PS = shadergroup->getEntryID("GF_DrawBoundary_PS", btrue);
			mc_entry_GF_DrawFluid_PS = shadergroup->getEntryID("GF_DrawFluid_PS", btrue);
			mc_entry_GF_Blur_PS = shadergroup->getEntryID("GF_Blur_PS", btrue);
			mc_entry_GF_AttractCopy_PS = shadergroup->getEntryID("GF_AttractCopy_PS", btrue);
			mc_entry_GF_AttractComputeSpeed_PS = shadergroup->getEntryID("GF_AttractComputeSpeed_PS", btrue);
			mc_entry_GF_AttractApplySpeed_PS = shadergroup->getEntryID("GF_AttractApplySpeed_PS", btrue);
			mc_entry_GF_AttractApplyColor_PS = shadergroup->getEntryID("GF_AttractApplyColor_PS", btrue);
			mc_entry_GF_RequestCopy_PS = shadergroup->getEntryID("GF_RequestCopy_PS", btrue);

			mc_define_GF_UseNoise = shadergroup->getDefineID("USE_NOISE");
			mc_define_GF_UseFlow = shadergroup->getDefineID("USE_FLOW");
			mc_define_GF_UseDuDv = shadergroup->getDefineID("USE_DUDV");
			mc_define_GF_RenderMode0 = shadergroup->getDefineID("RENDER_MODE_0");
			mc_define_GF_RenderMode1 = shadergroup->getDefineID("RENDER_MODE_1");
			mc_define_GF_RenderMode2 = shadergroup->getDefineID("RENDER_MODE_2");
			mc_define_GF_UseMask = shadergroup->getDefineID("USE_MASK");
			mc_define_GF_UseAlphaLerp = shadergroup->getDefineID("USE_ALPHA_LERP");
			mc_define_GF_UseColorTex = shadergroup->getDefineID("USE_COLOR_TEX");
			mc_define_GF_UseRGBFluid = shadergroup->getDefineID("USE_RGB_FLUID");
		}

		return loadSuccess;
	}

	void GFX_GridFluidManager::releaseShader(GFXAdapter_shaderManager& _shaderManager)
	{
        _shaderManager.removeShaderGroup(_shaderManager.getShaderGroup(mc_shader_Fluids));
	}

	// Register a collision.
	void GFX_GridFluidManager::addPolyLine(PolyLine *_polyLine)
	{
		i32 pos = m_polyLineList.find(_polyLine);
		if ( pos < 0 )
			m_polyLineList.push_back(_polyLine);
	}

	// Unregister a collision.
	void GFX_GridFluidManager::removePolyLine(PolyLine *_polyLine)
	{
		i32 pos = m_polyLineList.find(_polyLine);
		if ( pos >= 0 )
			m_polyLineList.removeAtUnordered(pos);
	}

	// Register primitive as fluid emitter.
	void GFX_GridFluidManager::batchFluidPrimitive(GFXPrimitive *_primitive, const AABB &_boundingVolume, f32 _depth,
		                                           GFX_GridFluidFilter _filters, GFX_GridModifierMode _mode)
	{
		PrimBatch batch;
		batch.m_primitive = _primitive;
		batch.m_boundingVolume = _boundingVolume;
		batch.m_depth = _depth;
		batch.m_filters = _filters;
		batch.m_mode = _mode;
		m_fluidPrimitiveTable.push_back(batch);
	}

	// Clear fluid primitive list.
	void GFX_GridFluidManager::clearFluidPrimitiveList()
	{
		m_fluidPrimitiveTable.clear();
	}

	// Set fluid states for a primitive rendering in a fluid.
	void GFX_GridFluidManager::setFluidStates(GFXAdapter * _gfxDeviceAdapter)
	{
		if ( m_areDrawingFluidPrimitive )
		{
			_gfxDeviceAdapter->m_drawCallStates.psDefineVariant &= ~(_gfxDeviceAdapter->mc_define_Light | _gfxDeviceAdapter->mc_define_Light3D | _gfxDeviceAdapter->mc_define_UseBackLight);
			_gfxDeviceAdapter->m_vsGlobal.globalColor.w() *= m_primitiveFactor;
            _gfxDeviceAdapter->setAlphaBlend(GFX_BLEND_ALPHA, GFX_ALPHAMASK_ADDALPHA);
		}
	}
} // namespace ITF


#endif // GFX_USE_GRID_FLUIDS
