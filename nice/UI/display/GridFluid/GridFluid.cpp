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
	u32		GFX_GridFluid::m_curTargetSizeX;
	u32		GFX_GridFluid::m_curTargetSizeY;
	u32		GFX_GridFluid::m_previousTargetSizeX;
	u32		GFX_GridFluid::m_previousTargetSizeY;
    RenderPassContext GFX_GridFluid::m_passCtx;
    PrimitiveContext GFX_GridFluid::m_primitiveCxt(&GFX_GridFluid::m_passCtx);
    DrawCallContext GFX_GridFluid::m_drawCallCtx(&GFX_GridFluid::m_primitiveCxt);

	GFX_GridFluidNoise::GFX_GridFluidNoise()
	{
		setObjectType(BaseObject::eFluidObject);
		m_scaleX = 1.0f;
		m_scaleY = 1.0f;
		m_intensity = 1.0f;
		m_freq = 1.0f;
		m_speedX = 0.01f;
		m_speedY = 0.01f;
		m_modulation = 0.0f;
	}

    IMPLEMENT_OBJECT_RTTI(GFX_GridFluidNoise)
	BEGIN_SERIALIZATION(GFX_GridFluidNoise)
      BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("Texture",m_texturePath);
        SERIALIZE_MEMBER("ScaleX",m_scaleX);
		SERIALIZE_MEMBER("ScaleY",m_scaleY);
        SERIALIZE_MEMBER("Intensity",m_intensity);
        SERIALIZE_MEMBER("Modulation",m_modulation);
        SERIALIZE_MEMBER("Freq",m_freq);
        SERIALIZE_MEMBER("SpeedX",m_speedX);
        SERIALIZE_MEMBER("SpeedY",m_speedY);
	  END_CONDITION_BLOCK()
    END_SERIALIZATION()

	GFX_GridFluidFlowTex::GFX_GridFluidFlowTex()
	{
		setObjectType(BaseObject::eFluidObject);
		m_deformation = 1.0f;
		m_intensity = 1.0f;
		m_speedX1 = 0.27f;
		m_speedY1 = 0.13f;
		m_speedX2 = -0.14f;
		m_speedY2 = 0.07f;
		m_speedX3 = 0.16f;
		m_speedY3 = -0.23f;
		m_scaleX1 = 10.0f;
		m_scaleY1 = 10.0f;
		m_scaleX2 = 16.0f;
		m_scaleY2 = 13.0f;
		m_scaleX3 = 27.0f;
		m_scaleY3 = 31.0f;
		m_rgbMultiplier = 1.0f;
		m_alphaMultiplier = 1.0f;
		m_densityFactor = 0.0f;
		m_velocityPower = 0.25f;
	}

    IMPLEMENT_OBJECT_RTTI(GFX_GridFluidFlowTex)
	BEGIN_SERIALIZATION(GFX_GridFluidFlowTex)
      BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("Texture",m_texturePath);
        SERIALIZE_MEMBER("Deformation",m_deformation);
        SERIALIZE_MEMBER("Intensity",m_intensity);
        SERIALIZE_MEMBER("SpeedX1",m_speedX1);
        SERIALIZE_MEMBER("SpeedY1",m_speedY1);
        SERIALIZE_MEMBER("SpeedX2",m_speedX2);
        SERIALIZE_MEMBER("SpeedY2",m_speedY2);
        SERIALIZE_MEMBER("SpeedX3",m_speedX3);
        SERIALIZE_MEMBER("SpeedY3",m_speedY3);
        SERIALIZE_MEMBER("ScaleX1",m_scaleX1);
		SERIALIZE_MEMBER("ScaleY1",m_scaleY1);
        SERIALIZE_MEMBER("ScaleX2",m_scaleX2);
		SERIALIZE_MEMBER("ScaleY2",m_scaleY2);
        SERIALIZE_MEMBER("ScaleX3",m_scaleX3);
		SERIALIZE_MEMBER("ScaleY3",m_scaleY3);
        SERIALIZE_MEMBER("RGBMultiplier",m_rgbMultiplier);
        SERIALIZE_MEMBER("AlphaMultiplier",m_alphaMultiplier);
        SERIALIZE_MEMBER("DensityFactor",m_densityFactor);
        SERIALIZE_MEMBER("VelocityPower",m_velocityPower);
	  END_CONDITION_BLOCK()
    END_SERIALIZATION()

	GFX_GridFluidDuDvTex::GFX_GridFluidDuDvTex()
	{
		setObjectType(BaseObject::eFluidObject);
		m_intensity = 1.0f;
		m_speedX1 = 0.27f;
		m_speedY1 = 0.13f;
		m_scaleX1 = 1.0f;
		m_scaleY1 = 1.0f;
	}

    IMPLEMENT_OBJECT_RTTI(GFX_GridFluidDuDvTex)
	BEGIN_SERIALIZATION(GFX_GridFluidDuDvTex)
      BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("Texture",m_texturePath);
        SERIALIZE_MEMBER("Intensity",m_intensity);
        SERIALIZE_MEMBER("SpeedX1",m_speedX1);
        SERIALIZE_MEMBER("SpeedY1",m_speedY1);
        SERIALIZE_MEMBER("ScaleX1",m_scaleX1);
		SERIALIZE_MEMBER("ScaleY1",m_scaleY1);
	  END_CONDITION_BLOCK()
    END_SERIALIZATION()

	GFX_GridFluidAdditionnalRender::GFX_GridFluidAdditionnalRender()
	{
		setObjectType(BaseObject::eFluidObject);
		m_offset.set(0.0f,0.0f,0.0f);
		m_fluidCol = COLOR_WHITE;
		m_fluidCol.setAlpha(0.5f);
		m_blendMode = GFX_BLEND_ALPHA;
	}

    IMPLEMENT_OBJECT_RTTI(GFX_GridFluidAdditionnalRender)
	BEGIN_SERIALIZATION(GFX_GridFluidAdditionnalRender)
      BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("Offset",m_offset);
        SERIALIZE_MEMBER("FluidCol",m_fluidCol);
        SERIALIZE_MEMBER("ColorTex",m_colorTexPath);
		SERIALIZE_GFX_BLENDMODE2("BlendMode",m_blendMode);
        SERIALIZE_OBJECT("FlowTexture",m_flowTexture);
        SERIALIZE_OBJECT("DuDvTexture",m_dudvTexture);
	  END_CONDITION_BLOCK()
    END_SERIALIZATION()

	GFX_GridFluidEmitterFactors::GFX_GridFluidEmitterFactors()
	{
		setObjectType(BaseObject::eFluidObject);
		m_externalForce = 1.0f;
		m_externalFluid = 1.0f;
		m_externalPrimitive = 1.0f;
	}

    IMPLEMENT_OBJECT_RTTI(GFX_GridFluidEmitterFactors)
	BEGIN_SERIALIZATION(GFX_GridFluidEmitterFactors)
      BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("ExternalForce",m_externalForce);
        SERIALIZE_MEMBER("ExternalFluid",m_externalFluid);
        SERIALIZE_MEMBER("ExternalPrimitive",m_externalPrimitive);
	  END_CONDITION_BLOCK()
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(GFX_GridFluid)
    BEGIN_SERIALIZATION(GFX_GridFluid)
		BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
			SERIALIZE_MEMBER("AttractForce",m_attractForce);
			SERIALIZE_MEMBER("AttractExpand",m_attractExpand);
			SERIALIZE_MEMBER("AttractColorize",m_attractColorize);
			SERIALIZE_OBJECT("AttractNoise",m_attractNoise);
			SERIALIZE_MEMBER("NbIter",m_nbIter);
			SERIALIZE_MEMBER("PressureDiffusion",m_pressureDiffusion);
			SERIALIZE_MEMBER("PressureIntensity",m_pressureIntensity);
		END_CONDITION_BLOCK()
		BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
			SERIALIZE_MEMBER("BoxSize",m_boxSize);
			SERIALIZE_MEMBER("Offset",m_mainRender.m_offset);
			SERIALIZE_BOOL("Reinit",m_reinit);
			SERIALIZE_BOOL("Active",m_active);
			SERIALIZE_BOOL("Pause",m_pause);
			SERIALIZE_MEMBER("ParticleTexSizeX",m_particleTexSizeX);
			SERIALIZE_MEMBER("ParticleTexSizeY",m_particleTexSizeY);
			SERIALIZE_MEMBER("SpeedTexSizeX",m_speedTexSizeX);
			SERIALIZE_MEMBER("SpeedTexSizeY",m_speedTexSizeY);

			SERIALIZE_MEMBER("Weight",m_weight);
			SERIALIZE_MEMBER("Viscosity",m_viscosity);
			SERIALIZE_MEMBER("FluidDiffusion",m_fluidDiffusion);
			SERIALIZE_MEMBER("FluidLoss",m_FluidLoss);
			SERIALIZE_MEMBER("VelocityLoss",m_VelocityLoss);
			SERIALIZE_OBJECT("FluidNoise",m_fluidNoise);
			SERIALIZE_OBJECT("VelocityNoise",m_velocityNoise);
			SERIALIZE_OBJECT("EmitterFactors", m_emitterFactors);
			SERIALIZE_GFX_GRID_FILTER("RequiredFilter",m_requiredFilter);
			SERIALIZE_GFX_GRID_FILTER("RejectFilter",m_rejectFilter);

			SERIALIZE_ENUM_GFX_GRID_RENDER_MODE("RenderMode",m_renderMode);
			SERIALIZE_GFX_BLENDMODE2("BlendMode",m_mainRender.m_blendMode);
			SERIALIZE_MEMBER("FluidCol",m_mainRender.m_fluidCol);
			SERIALIZE_MEMBER("ColorTex",m_mainRender.m_colorTexPath);
			SERIALIZE_MEMBER("MaskTexture",m_maskPath);
			SERIALIZE_BOOL("UseRGBFluid",m_useRGBFluid);
			SERIALIZE_MEMBER("NeutralColor",m_neutralColor);
			SERIALIZE_OBJECT("FlowTexture",m_mainRender.m_flowTexture);
			SERIALIZE_OBJECT("DuDvTexture",m_mainRender.m_dudvTexture);
			SERIALIZE_CONTAINER_OBJECT("AdditionnalRender",m_addRenderList);
			SERIALIZE_OBJECT("PrimitiveParam", *getCommonParam());
		END_CONDITION_BLOCK()
    END_SERIALIZATION()

	GFX_GridFluid::GFX_GridFluid() :
	  m_requiredFilter(0), // no bit required => everything is visible
	  m_rejectFilter(0)    // no bit rejected => everything is visible
	{
		m_active = true;
		m_pause = false;
		m_nbIter = 6;
		m_lastRequestId = 0;

		m_roundedPos.set(0.0f, 0.0f, 0.0f);
		m_prevRoundedPos = m_roundedPos;
		m_texelOffset.set(0.0f, 0.0f);
		m_worldUVOffset.set(0.0f, 0.0f);

		m_boxPos.set(0.0f,0.0f, 0.0f);
		m_boxSize.set(10.0f,10.0f);
		m_particleTexSizeX = 512;
		m_particleTexSizeY = 512;
		m_speedTexSizeX = 128;
		m_speedTexSizeY = 128;

		m_time = 0.0f;
		m_weight = 0.15f;//0.015f;
		m_viscosity = 0.02f;
		m_fluidDiffusion = 0.0f;
		m_FluidLoss = 0.01f;
		m_VelocityLoss = 0.01f;
		m_pressureDiffusion = 0.25f / 4.0f;
		m_pressureIntensity = 1.0f;
		m_attractForce = 1.0f;
		m_attractExpand = 1.0f;
		m_attractColorize = 1.0f;
		m_renderMode = GRDFLD_FluidOpaque;
		m_reinit = false;
		m_useRGBFluid = false;
		m_neutralColor = COLOR_BLACK;
		m_useAttract = false;
		m_wasUsingAttract = false;

		m_velocityTarget = nullptr;
		m_boundariesTarget = nullptr;
		m_pressureTarget = nullptr;
		m_divFieldTarget = nullptr;
		m_fluidTarget = nullptr;
		m_emitterTarget = nullptr;
		m_attractSourceTarget = nullptr;
		m_attractSpeedTarget = nullptr;

		setPassFilterFlag(GFX_ZLIST_PASS_REGULAR_FLAG);

		m_frameCount = 0;
	}
	GFX_GridFluid::~GFX_GridFluid()
	{
		releaseTargets();
	}

	void GFX_GridFluid::releaseTargets()
	{
		if ( GFX_ADAPTER )
		{
			deleteAllRequest();

			GFX_GridFluidManager& fluidManager = GFX_ADAPTER->getGridFluidManager();
			if ( m_emitterTarget )
			{
				fluidManager.storeTempTarget(m_emitterTarget);
				m_emitterTarget = nullptr;
			}
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
			if ( m_velocityTarget )
			{
				m_velocityTarget->release();
				m_velocityTarget = nullptr;
			}
			if ( m_boundariesTarget )
			{
				m_boundariesTarget->release();
				m_boundariesTarget = nullptr;
			}
			if ( m_pressureTarget )
			{
				m_pressureTarget->release();
				m_pressureTarget = nullptr;
			}
			if ( m_fluidTarget )
			{
				m_fluidTarget->release();
				m_fluidTarget = nullptr;
			}
		}
	}

	void GFX_GridFluid::initTargets(GFXAdapter * _gfxDeviceAdapter)
	{
		Texture::PixFormat fluidFormat;
		if ( m_useRGBFluid )
			fluidFormat = Texture::PF_RGBA_FLT16;
		else
			fluidFormat = Texture::PF_R_FLT16;

		if ( ( m_fluidTarget && ( (m_fluidTarget->getSizeX() != m_particleTexSizeX) || (m_fluidTarget->getSizeY() != m_particleTexSizeY) || (m_fluidTarget->getPixFormat() != fluidFormat) ) ) ||
			 ( m_velocityTarget && ( (m_velocityTarget->getSizeX() != m_speedTexSizeX) || (m_velocityTarget->getSizeY() != m_speedTexSizeY) ) ) )
		{
			releaseTargets();
		}

		if ( !m_fluidTarget )
		{
			RenderTargetInfo fluidRTInfo(m_particleTexSizeX, m_particleTexSizeY, 1, fluidFormat);
			m_fluidTarget = _gfxDeviceAdapter->createRenderTarget(fluidRTInfo);

			RenderTargetInfo velocityRTInfo(m_speedTexSizeX, m_speedTexSizeY, 1, Texture::PF_RG_FLT16);
			m_velocityTarget = _gfxDeviceAdapter->createRenderTarget(velocityRTInfo);

			RenderTargetInfo boundariesRTInfo(m_speedTexSizeX, m_speedTexSizeY, 1, Texture::PF_RGBA);
			m_boundariesTarget = _gfxDeviceAdapter->createRenderTarget(boundariesRTInfo);

			RenderTargetInfo pressureRTInfo(m_speedTexSizeX, m_speedTexSizeY, 1, Texture::PF_R_FLT16);
			m_pressureTarget = _gfxDeviceAdapter->createRenderTarget(pressureRTInfo);

			m_reinit = true;
		}
		if ( m_reinit )
		{
			computeRoundedCoord();
			computeRoundedCoord(); // Called twice to begin with an offset null.
			clearTargets(_gfxDeviceAdapter);
			m_reinit = false;
		}
	}

	// Get list of texture ressources adresses.
	void GFX_GridFluid::getTextureRessourceList( ITF_VECTOR<GF_TEXTURE_RESSOURCE> &_list )
	{
		_list.push_back(GF_TEXTURE_RESSOURCE(m_fluidNoise.m_texturePath, m_fluidNoise.m_textureRessource));
		_list.push_back(GF_TEXTURE_RESSOURCE(m_velocityNoise.m_texturePath, m_velocityNoise.m_textureRessource));
		_list.push_back(GF_TEXTURE_RESSOURCE(m_attractNoise.m_texturePath, m_attractNoise.m_textureRessource));
		_list.push_back(GF_TEXTURE_RESSOURCE(m_mainRender.m_flowTexture.m_texturePath, m_mainRender.m_flowTexture.m_textureRessource));
		_list.push_back(GF_TEXTURE_RESSOURCE(m_mainRender.m_dudvTexture.m_texturePath, m_mainRender.m_dudvTexture.m_textureRessource));

		for ( ux i = 0; i < m_addRenderList.size(); i++ )
		{
			GFX_GridFluidAdditionnalRender &param = m_addRenderList[i];
			_list.push_back(GF_TEXTURE_RESSOURCE(param.m_flowTexture.m_texturePath, param.m_flowTexture.m_textureRessource));
			_list.push_back(GF_TEXTURE_RESSOURCE(param.m_dudvTexture.m_texturePath, param.m_dudvTexture.m_textureRessource));
			_list.push_back(GF_TEXTURE_RESSOURCE(param.m_colorTexPath, param.m_colorTexRessource));
		}

		_list.push_back(GF_TEXTURE_RESSOURCE(m_maskPath, m_maskRessource));
		_list.push_back(GF_TEXTURE_RESSOURCE(m_mainRender.m_colorTexPath, m_mainRender.m_colorTexRessource));
	}

	// Request a density computation. Return an id allowing to retrieve result next frame.
	u32 GFX_GridFluid::sendRequestDensity(const Vec2d &_pos, const Vec2d &_halfWidth, const Vec2d &_halfHeight)
	{
		Request request;
		request.m_id = ++m_lastRequestId;
		request.m_pos = _pos;
		request.m_halfWidth = _halfWidth;
		request.m_halfHeight = _halfHeight;
		request.m_status = GRD_FLD_REQUEST_NOT_PROCESSED;
		request.m_result = -1.0f;  // Invalid value;
		request.m_target = NULL;
		m_requestList.push_back(request);
		return request.m_id;
	}

	// Retrieve request result.
	GFX_GridFluid_RequestStatus GFX_GridFluid::getRequestDensity( u32 _id, f32 &_result )
	{
		GFX_GridFluidManager& fluidManager = GFX_ADAPTER->getGridFluidManager();
		for ( ux i = 0; i < m_requestList.size(); i++ )
		{
			Request &request = m_requestList[i];
			if ( request.m_id == _id )
			{
				GFX_GridFluid_RequestStatus status = request.m_status;
				if ( status == GRD_FLD_REQUEST_PROCESSED )
				{
					_result = request.m_result;
					if ( request.m_target )
						fluidManager.storeTempTarget(request.m_target);
					m_requestList.removeAtUnordered(i);
				}
				return status;
			}
		}
		return GRD_FLD_REQUEST_NOT_FOUND;
	}

	// Delete a request.
	void GFX_GridFluid::deleteRequest( u32 _id )
	{
		GFX_GridFluidManager& fluidManager = GFX_ADAPTER->getGridFluidManager();
		for ( ux i = 0; i < m_requestList.size(); i++ )
		{
			Request &request = m_requestList[i];
			if ( request.m_id == _id )
			{
				if ( request.m_target )
					fluidManager.storeTempTarget(request.m_target);
				m_requestList.removeAtUnordered(i);
				break;
			}
		}
	}

	// Delete all request.
	void GFX_GridFluid::deleteAllRequest()
	{
		GFX_GridFluidManager& fluidManager = GFX_ADAPTER->getGridFluidManager();
		for ( ux i = 0; i < m_requestList.size(); i++ )
		{
			Request &request = m_requestList[i];
			if ( request.m_target )
				fluidManager.storeTempTarget(request.m_target);
		}
		m_requestList.clear();
	}
} // namespace ITF


#endif // GFX_USE_GRID_FLUIDS
