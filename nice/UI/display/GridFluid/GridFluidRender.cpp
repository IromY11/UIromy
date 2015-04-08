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
    void GFX_GridFluid::directDraw(const class RenderPassContext & _rdrPassCtxt, GFXAdapter * _gfxDeviceAdapter, f32 _Z, f32 _ZWorld, f32 _ZAbs)
	{
		if ( !m_active )
			return;

        PRF_M_SCOPE(drawFluid);

		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();
		_gfxDeviceAdapter->setDefaultGFXPrimitiveParam();
		_gfxDeviceAdapter->setGfxMatDefault(m_drawCallCtx);

		float sx = m_boxSize.x() * 0.5f;
		float sy = m_boxSize.y() * 0.5f;

		const Vec2d uv0(0.0f,0.0f);
		const Vec2d uv1(1.0f,1.0f);

		_gfxDeviceAdapter->bindTexture(FluidSampler, m_fluidTarget);
		_gfxDeviceAdapter->bindTexture(VelocitySampler, m_velocityTarget);
		_gfxDeviceAdapter->bindTexture(PressureSampler, m_pressureTarget);
		_gfxDeviceAdapter->bindTexture(BoundariesSampler, m_boundariesTarget);
		_gfxDeviceAdapter->bindTexture(EmitterSampler, m_emitterTarget);

		VertexPCT quad[4];
		quad[0].setData( Vec3d( -sx, -sy, 0.f ), Vec2d(uv0.x(), uv1.y()), 0xFFFFFFFF);
		quad[1].setData( Vec3d( sx, -sy, 0.f ),  uv1,                     0xFFFFFFFF);
		quad[3].setData( Vec3d( sx, sy, 0.f ),   Vec2d(uv1.x(), uv0.y()), 0xFFFFFFFF);
		quad[2].setData( Vec3d( -sx, sy, 0.f ),  uv0,                     0xFFFFFFFF);

		// Additionnal rendering.
		for ( ux i = 0; i <= m_addRenderList.size(); i++ )
		{
			GFX_GridFluidAdditionnalRender *param;
			if ( i < m_addRenderList.size() )
			{
				param = &m_addRenderList[i];
				if ( m_renderMode != GRDFLD_FluidTransp )
					continue;
			}
			else
			{
				param = &m_mainRender;
			}

			// Just draw the level corresponding to the Z node.
			f32 curZ= m_boxPos.z() + param->m_offset.z() - (m_addRenderList.size()-i) * F32_EPSILON * 2.0f;
			if ( !f32_IsNull( curZ - _ZWorld) )
				continue;

			Matrix44 World;
			World.setIdentity34();
			World.setTranslation(m_roundedPos + param->m_offset);
			_gfxDeviceAdapter->setObjectMatrix(World);

			shaderDefineKey vsDefine = 0;
			shaderDefineKey psDefine = 0;

			VS_GF_DrawFluid vs_DrawFluid;
			ITF_MemSet(&vs_DrawFluid, 0, sizeof(vs_DrawFluid));

			PS_GF_DrawFluid ps_DrawFluid;
			ITF_MemSet(&ps_DrawFluid, 0, sizeof(ps_DrawFluid));
			ps_DrawFluid.param.set(1.0f, 0.0f, 0.0f, 0.0f);

			if (m_renderMode == GRDFLD_FluidTransp)
			{
				if ( m_maskRessource.isValid() )
				{
					Texture *maskTexture = static_cast<Texture*>(m_maskRessource.getResource());
					_gfxDeviceAdapter->bindTexture(MaskSampler, maskTexture);
					psDefine |= fluidManager.mc_define_GF_UseMask;
				}

				param->m_flowTexture.setupFlowTex(_gfxDeviceAdapter, m_time, vsDefine, psDefine, &vs_DrawFluid.flow0, &ps_DrawFluid.flow, m_worldUVOffset);
				param->m_dudvTexture.setupDuDvTex(_gfxDeviceAdapter, m_time, vsDefine, psDefine, &vs_DrawFluid.dudv, &ps_DrawFluid.dudv, m_worldUVOffset);

				if ( param->m_colorTexRessource.isValid() )
				{
					Texture *colorTexture = static_cast<Texture*>(param->m_colorTexRessource.getResource());
					_gfxDeviceAdapter->bindTexture(ColorSampler, colorTexture);
					psDefine |= fluidManager.mc_define_GF_UseColorTex;
				}

				m_drawCallCtx.getStateCache().setAlphaBlend(param->m_blendMode, GFX_ALPHAMASK_NONE);

				switch (param->m_blendMode)
				{
				case GFX_BLEND_MUL:
					psDefine |= fluidManager.mc_define_GF_UseAlphaLerp;
					ps_DrawFluid.transpColor.set(1.0f, 1.0f, 1.0f, 0.0f);
					break;
				case GFX_BLEND_MUL2X:
					psDefine |= fluidManager.mc_define_GF_UseAlphaLerp;
					ps_DrawFluid.transpColor.set(0.5f, 0.5f, 0.5f, 0.0f);
					break;
				case GFX_BLEND_ADD:
				case GFX_BLEND_SUB:
				case GFX_BLEND_ADDSMOOTH:
					psDefine |= fluidManager.mc_define_GF_UseAlphaLerp;
					ps_DrawFluid.transpColor.set(0.0f, 0.0f, 0.0f, 0.0f);
					break;
				}
			}
			else
				m_drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ALPHA, GFX_ALPHAMASK_NONE);

			if ( m_useRGBFluid )
				psDefine |= fluidManager.mc_define_GF_UseRGBFluid;

			if ( m_renderMode & 1 )
				psDefine |= fluidManager.mc_define_GF_RenderMode0;
			if ( m_renderMode & 2 )
				psDefine |= fluidManager.mc_define_GF_RenderMode1;
			if ( m_renderMode & 4 )
				psDefine |= fluidManager.mc_define_GF_RenderMode2;

			if ( ( m_renderMode == GRDFLD_Velocity ) || ( m_renderMode == GRDFLD_VelocityEmitter ))
				ps_DrawFluid.param.x() = 4.0f;
			else if ( m_renderMode == GRDFLD_Pressure )
				ps_DrawFluid.param.x() = 1.0f;
			_gfxDeviceAdapter->setPixelShaderConstantF(PS_Attrib_GF_DrawFluid, (f32*)&ps_DrawFluid, sizeof(ps_DrawFluid)/16);

			vs_DrawFluid.color.set(param->m_fluidCol.getRed(), param->m_fluidCol.getGreen(), param->m_fluidCol.getBlue(), param->m_fluidCol.getAlpha() * 2.0f);
			_gfxDeviceAdapter->setVertexShaderConstantF(VS_Attrib_GF_DrawFluid, (f32*)&vs_DrawFluid, sizeof(vs_DrawFluid)/16);

			_gfxDeviceAdapter->setShaderGroup(fluidManager.mc_shader_Fluids, fluidManager.mc_entry_GF_DrawFluid_VS, fluidManager.mc_entry_GF_DrawFluid_PS, vsDefine, psDefine);
        
			_gfxDeviceAdapter->setVertexFormat(VertexFormat_PCT);
			_gfxDeviceAdapter->DrawPrimitive(m_drawCallCtx, GFX_TRIANGLE_STRIP, (void*)quad, 4);
		}
	}

	ITF_INLINE f32 f32_SignFrac(f32 _value)
	{
		return _value - f32_Trunc(_value);
	}

	void GFX_GridFluidFlowTex::setupFlowTex(GFXAdapter * _gfxDeviceAdapter, f32 _time, shaderDefineKey &vsDefine, shaderDefineKey &psDefine,
		                                    GFX_Vector4 *_vsConst, GFX_Vector4 *_psConst, const Vec2d &_worldUVOffset)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();

		if ( m_textureRessource.isValid() && (m_intensity>0.0f) )
		{
			Texture *flowTexture = static_cast<Texture*>(m_textureRessource.getResource());
			_gfxDeviceAdapter->bindTexture(GFX_GridFluid::FlowSampler, flowTexture, true, GFX_TEXADRESSMODE_WRAP, GFX_TEXADRESSMODE_WRAP);

			_vsConst[0].set(m_scaleX1, m_scaleY1, m_speedX1 * m_scaleX1 * _time + f32_SignFrac(_worldUVOffset.x() * m_scaleX1), m_speedY1 * m_scaleX1 * _time - f32_SignFrac(_worldUVOffset.y() * m_scaleY1));
			_vsConst[1].set(m_scaleX2, m_scaleY2, m_speedX2 * m_scaleX2 * _time + f32_SignFrac(_worldUVOffset.x() * m_scaleX2), m_speedY2 * m_scaleX2 * _time - f32_SignFrac(_worldUVOffset.y() * m_scaleY2));
			_vsConst[2].set(m_scaleX3, m_scaleY3, m_speedX3 * m_scaleX3 * _time + f32_SignFrac(_worldUVOffset.x() * m_scaleX3), m_speedY3 * m_scaleX3 * _time - f32_SignFrac(_worldUVOffset.y() * m_scaleY3));

			_psConst[0].set(m_deformation, m_intensity, m_rgbMultiplier / 3.0f, m_alphaMultiplier / 3.0f);
			_psConst[1].set(2.0f - m_densityFactor, m_velocityPower, 0.0f, 0.0f);

			vsDefine |= fluidManager.mc_define_GF_UseFlow;
			psDefine |= fluidManager.mc_define_GF_UseFlow;
		}
	}

	void GFX_GridFluidDuDvTex::setupDuDvTex(GFXAdapter * _gfxDeviceAdapter, f32 _time, shaderDefineKey &vsDefine, shaderDefineKey &psDefine,
		                                    GFX_Vector4 *_vsConst, GFX_Vector4 *_psConst, const Vec2d &_worldUVOffset)
	{
		GFX_GridFluidManager& fluidManager = _gfxDeviceAdapter->getGridFluidManager();

		if ( m_textureRessource.isValid() && (m_intensity>0.0f) )
		{
			Texture *dudvTexture = static_cast<Texture*>(m_textureRessource.getResource());
			_gfxDeviceAdapter->bindTexture(GFX_GridFluid::DuDvSampler, dudvTexture, true, GFX_TEXADRESSMODE_WRAP, GFX_TEXADRESSMODE_WRAP);

			_vsConst[0].set(m_scaleX1, m_scaleY1, m_speedX1 * m_scaleX1 * _time + f32_SignFrac(_worldUVOffset.x() * m_scaleX1),
				                                  m_speedY1 * m_scaleY1 * _time - f32_SignFrac(_worldUVOffset.y() * m_scaleY1));
			_psConst[0].set(m_intensity / 50.0f, 0.0f, 0.0f, 0.0f);

			vsDefine |= fluidManager.mc_define_GF_UseDuDv;
			psDefine |= fluidManager.mc_define_GF_UseDuDv;
		}
	}

} // namespace ITF


#endif // GFX_USE_GRID_FLUIDS
