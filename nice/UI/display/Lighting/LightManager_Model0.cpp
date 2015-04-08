#include "precompiled_engine.h"


#ifndef _ITF_LIGHTMANAGER_MODEL0_H_
#include "engine/display/lighting/LightManager_Model0.h"
#endif // _ITF_LIGHTMANAGER_MODEL0_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef ITF_GFX_ZLIST_H_
#include "GFXZList.h"
#endif // ITF_GFX_ZLIST_H_

#ifndef ITF_MASKRESOLVEPRIMITIVE_H_
#include "engine/display/MaskResolvePrimitive.h"
#endif //ITF_MASKRESOLVEPRIMITIVE_H_


namespace ITF
{
	shaderDefineKey GFX_LightManager_Model0::getDisabledDefines()
	{
		shaderDefineKey defines = GFX_LightManager_Base::getDisabledDefines();
		defines &= ~m_adapter->mc_define_Light;
		defines &= ~m_adapter->mc_define_UseBackLight;
		return defines;
	}

	///// constructor/destructor --------------------------------------------------------------
	GFX_LightManager_Model0::GFX_LightManager_Model0(GFXAdapter *_adapter) : GFX_LightManager_Base(_adapter)
	{
		m_frontLightClearColor = getDefaultFrontLightClearColor();
		m_backLightClearColor = getDefaultBackLightClearColor();
		m_frontLightUseClearConstant = btrue;
		m_backLightUseClearConstant = btrue;
		m_currentFrontLightBufferType = LIGHT_BFR_FULL;
		m_currentBackLightBufferType = LIGHT_BFR_FULL;
	}
	GFX_LightManager_Model0::~GFX_LightManager_Model0()
	{

	}

	// Init before a new rendering (one call per frame).
	void GFX_LightManager_Model0::beginDisplay(f32 _dt)
	{
		m_frontLightClearColor = getDefaultFrontLightClearColor();
		m_backLightClearColor = getDefaultBackLightClearColor();
		m_frontLightUseClearConstant = btrue;
		m_backLightUseClearConstant = btrue;
	}

	// Init before a new view rendering (one call per view).
	void GFX_LightManager_Model0::beginViewDisplay()
	{
		m_frontLightUseClearConstant = btrue;
		m_backLightUseClearConstant = btrue;
		m_currentFrontLightBufferType = LIGHT_BFR_FULL;
		m_currentBackLightBufferType = LIGHT_BFR_FULL;
	}

	// End of the main 3d rendering (one call per view).
	void GFX_LightManager_Model0::endDisplay3D()
	{
        m_frontLightUseClearConstant = btrue;
        m_backLightUseClearConstant = btrue;
	}

	// Set global states for lighting.
	void GFX_LightManager_Model0::setGlobalLighting(View &_view)
	{
	}

	// Set per material lighting states (for derivate classes).
	void GFX_LightManager_Model0::setMaterialLightingInternal( const GFX_MATERIAL* _gfxMat, shaderDefineKey &vertexDefine, shaderDefineKey &pixelDefine, void * constPtr)
	{
        PS_LightParam_Object &psLightParam = *(PS_LightParam_Object*)constPtr;
        GFXAdapter *adapter = m_adapter;

        f32 brightness = adapter->getFrontLightBrightness();

		psLightParam.frontLightColor = *(GFX_Vector4*)&m_frontLightClearColor;

        f32 c= adapter->getFrontLightContrast() * 2.0f;

        psLightParam.frontLightColor.m_x = psLightParam.frontLightColor.m_x * c + brightness;
        psLightParam.frontLightColor.m_y = psLightParam.frontLightColor.m_y * c + brightness;
        psLightParam.frontLightColor.m_z = psLightParam.frontLightColor.m_z * c + brightness;
	}

	// Set per material lighting states.
    void GFX_LightManager_Model0::setMaterialLighting( const GFX_MATERIAL* _gfxMat)
    {
        GFXAdapter *adapter = m_adapter;
        PS_LightParam_Object psLightParam;
        ITF_MemSet(&psLightParam, 0, sizeof(psLightParam));
        shaderDefineKey vertexDefine = 0;
        shaderDefineKey pixelDefine = 0;
        setMaterialLightingInternal( _gfxMat, vertexDefine, pixelDefine, &psLightParam);

        adapter->setPixelShaderConstantF( PS_Attrib_lightParamObject, (f32*)&psLightParam, 3 ); // 3 consts for RO2 lighting part
        adapter->addVertexShaderDefines(vertexDefine);
        adapter->addPixelShaderDefines(pixelDefine);
    }


	void GFX_LightManager_Model0::drawLightingPass( RenderPassContext & _rdrCtxt, const MaskResolvePrimitive &_resolvePrimitive, f32 _Z, f32 _ZWorld, f32 _ZAbs)
	{
    }
} // namespace ITF

