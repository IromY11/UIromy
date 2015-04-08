#include "precompiled_engine.h"


#ifndef _ITF_LIGHTMANAGER_MODEL1_H_
#include "engine/display/lighting/LightManager_Model1.h"
#endif // _ITF_LIGHTMANAGER_MODEL1_H_

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
	shaderDefineKey GFX_LightManager_Model1::getDisabledDefines()
	{
		shaderDefineKey defines = GFX_LightManager_Base::getDisabledDefines();
		defines &= ~m_adapter->mc_define_Light;
		defines &= ~m_adapter->mc_define_UseBackLight;
		return defines;
	}

	///// constructor/destructor --------------------------------------------------------------
	GFX_LightManager_Model1::GFX_LightManager_Model1(GFXAdapter *_adapter) : GFX_LightManager_Base(_adapter)
	{
		m_frontLightClearColor = getDefaultFrontLightClearColor();
		m_backLightClearColor = getDefaultBackLightClearColor();
		m_frontLightUseClearConstant = btrue;
		m_backLightUseClearConstant = btrue;
		m_currentFrontLightBufferType = LIGHT_BFR_FULL;
		m_currentBackLightBufferType = LIGHT_BFR_FULL;
	}
	GFX_LightManager_Model1::~GFX_LightManager_Model1()
	{

	}

	// Init before a new rendering (one call per frame).
	void GFX_LightManager_Model1::beginDisplay(f32 _dt)
	{
		m_frontLightClearColor = getDefaultFrontLightClearColor();
		m_backLightClearColor = getDefaultBackLightClearColor();
		m_frontLightUseClearConstant = btrue;
		m_backLightUseClearConstant = btrue;
	}

	// Init before a new view rendering (one call per view).
	void GFX_LightManager_Model1::beginViewDisplay()
	{
		m_frontLightUseClearConstant = btrue;
		m_backLightUseClearConstant = btrue;
		m_currentFrontLightBufferType = LIGHT_BFR_FULL;
		m_currentBackLightBufferType = LIGHT_BFR_FULL;
	}

	// End of the main 3d rendering (one call per view).
	void GFX_LightManager_Model1::endDisplay3D()
	{
        m_frontLightUseClearConstant = btrue;
        m_backLightUseClearConstant = btrue;
	}

	// Set global states for lighting.
	void GFX_LightManager_Model1::setGlobalLighting(View &_view)
	{
					}

	// Set per material lighting states (for derivate classes).
	void GFX_LightManager_Model1::setMaterialLightingInternal( const GFX_MATERIAL* _gfxMat, shaderDefineKey &vertexDefine, shaderDefineKey &pixelDefine, void * constPtr)
		{
		PS_LightParam_Object &psLightParam = *(PS_LightParam_Object*)constPtr;

		GFX_ZLIST_PASS_TYPE curPassType = m_adapter->getCurPassType();
		GFXAdapter *adapter = m_adapter;

        // If current Pass is ZPREPASS, we can SKIP the lighting
        if(curPassType != GFX_ZLIST_PASS_ZPREPASS)
        {
        bbool useLighting = curPassType==GFX_ZLIST_PASS_REGULAR || curPassType==GFX_ZLIST_PASS_REFLECTION;
        GFXMaterialLightingShader lightingShader = useLighting?_gfxMat->getMatLighting():GFX_MAT_SHADER_LIGHT_LAYERED_DEFAULT_FRONT_FLAG;

        if ((lightingShader & GFX_MAT_SHADER_LIGHT_LAYERED)!=0)
        {
            Texture * backTex = _gfxMat->getTexture(TEXSET_ID_BACK_LIGHT);

			if ( backTex )
			{
				adapter->bindTexture(3, backTex, true, backTex->getUVAddressModeX(), backTex->getUVAddressModeY()); // backtex not null (or the default flag should be set)
			}
			else
				adapter->bindTexture(3, adapter->getBlackOpaqueTexture()); // backtex not null (or the default flag should be set)
			if ( adapter->isUsingLighting())
			{
					adapter->bindTexture(7, adapter->getBackLightTexture(m_currentBackLightBufferType));

					adapter->bindTexture(6, adapter->getFrontLightTexture(m_currentFrontLightBufferType));
			}
        }
        else if (adapter->getCurPassType() == GFX_ZLIST_PASS_3D)
        {
            Texture * backTex = _gfxMat->getTexture(TEXSET_ID_BACK_LIGHT);
            if (backTex)
                adapter->bindTexture(3, backTex, true, backTex->getUVAddressModeX(), backTex->getUVAddressModeY());
			else
                adapter->bindTexture(3, adapter->getBlackOpaqueTexture());
        }

			f32 brightness = adapter->getFrontLightBrightness();
		if(adapter->isLightingEnable())
        {
				psLightParam.BrightContrast.set(brightness, adapter->getFrontLightContrast(),
				                            adapter->getBackLightBrightness(), adapter->getBackLightContrast());
            if ( ((lightingShader & GFX_MAT_SHADER_LIGHT_LAYERED)!=0) && !_gfxMat->useNormalMapLighting())
            {
					pixelDefine |= adapter->mc_define_Light;

				if ((lightingShader & GFX_MAT_SHADER_LIGHT_LAYERED_DEFAULT_FRONT_FLAG) != 0)
                {
                    psLightParam.frontLightColor.set(0.5f, 0.5f, 0.5f, 1.f);
				}
				else
				{
					m_frontLightClearColor.setAlpha(m_frontLightUseClearConstant ? 1.0f : 0.0f);
					psLightParam.frontLightColor = *(GFX_Vector4*)&m_frontLightClearColor;
				}
                if ((lightingShader & GFX_MAT_SHADER_LIGHT_LAYERED_DEFAULT_BACK_FLAG) != 0)
                {
                    psLightParam.backLightColor.set(0.0f, 0.0f, 0.0f, 1.f);
				}
				else
				{
                    // use back light texture
						pixelDefine |= adapter->mc_define_UseBackLight;
					m_backLightClearColor.setAlpha(m_backLightUseClearConstant ? 1.0f : 0.0f);
                    psLightParam.backLightColor = *(GFX_Vector4*)&m_backLightClearColor;
				}
            }
			else
			{
				if ((lightingShader & GFX_MAT_SHADER_LIGHT_LAYERED_DEFAULT_FRONT_FLAG) != 0)
					psLightParam.frontLightColor.set(0.5f, 0.5f, 0.5f, 1.f);
				else
					psLightParam.frontLightColor = *(GFX_Vector4*)&m_frontLightClearColor;
				f32 c= adapter->getFrontLightContrast() * 2.0f;

					psLightParam.frontLightColor.m_x = psLightParam.frontLightColor.m_x * c + brightness;
					psLightParam.frontLightColor.m_y = psLightParam.frontLightColor.m_y * c + brightness;
					psLightParam.frontLightColor.m_z = psLightParam.frontLightColor.m_z * c + brightness;
			}
		}
		else
		{
			if ((lightingShader & GFX_MAT_SHADER_LIGHT_LAYERED_DEFAULT_FRONT_FLAG) != 0)
				psLightParam.frontLightColor.set(0.5f, 0.5f, 0.5f, 1.f);
			else
				psLightParam.frontLightColor = *(GFX_Vector4*)&m_frontLightClearColor;
			f32 c= adapter->getFrontLightContrast() * 2.0f;

				psLightParam.frontLightColor.m_x = psLightParam.frontLightColor.m_x * c + brightness;
				psLightParam.frontLightColor.m_y = psLightParam.frontLightColor.m_y * c + brightness;
				psLightParam.frontLightColor.m_z = psLightParam.frontLightColor.m_z * c + brightness;
        }
		}
	}

	// Set per material lighting states.
    void GFX_LightManager_Model1::setMaterialLighting( const GFX_MATERIAL* _gfxMat)
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

	// find next light resolver primitive
	static ux findNextFrontMaskResolve(const RenderPassContext & _rdrCtxt)
	{
		for (ux i = _rdrCtxt.getBeginZListIdx(), n = _rdrCtxt.getEndZListIdx();
			i < n;
			i++)
		{
			GFXPrimitive * prim = _rdrCtxt.getNode(i).m_primitive;
			if (prim->getType() == GFXPrimitive::TYPE_MASKRESOLVER)
			{
				MaskResolvePrimitive * pResolvePrim = static_cast<MaskResolvePrimitive *>(prim);
				if (pResolvePrim->getFrontLightBuffer())
				{
					return i;
				}
			}
		}
		return _rdrCtxt.getEndZListIdx();
	}

	// find prev back light resolver primitive
	static ux findPrevBackMaskResolve(const RenderPassContext & _rdrCtxt)
	{
        if(_rdrCtxt.getEndZListIdx() > 0)
        {
            for (ux i = _rdrCtxt.getEndZListIdx()-1; i > 0; i--)
            {
                GFXPrimitive * prim = _rdrCtxt.getNode(i).m_primitive;
                if (prim->getType() == GFXPrimitive::TYPE_MASKRESOLVER)
                {
                    MaskResolvePrimitive * pResolvePrim = static_cast<MaskResolvePrimitive *>(prim);
                    if (pResolvePrim->getBackLightBuffer())
                    {
                        return i;
                    }
                }
            }
        }
        return 0;
	}

	void GFX_LightManager_Model1::drawLightingPass( RenderPassContext & _rdrCtxt, const MaskResolvePrimitive &_resolvePrimitive, f32 _Z, f32 _ZWorld, f32 _ZAbs)
	{
		GFXAdapter *adapter = m_adapter;
		if(adapter->isLightingEnable())
		{
			if ( _rdrCtxt.getPass() == GFX_ZLIST_PASS_REGULAR || _rdrCtxt.getPass() == GFX_ZLIST_PASS_REFLECTION)
			{
				if (_resolvePrimitive.getFrontLightBuffer() || _resolvePrimitive.getBackLightBuffer())
				{
					m_frontLightRenderPass = _rdrCtxt;
					m_frontLightRenderPass.setPassType(GFX_ZLIST_PASS_FRONT_LIGHT);
					m_frontLightRenderPass.setReverseOrder();

					m_backLightRenderPass = _rdrCtxt;
					m_backLightRenderPass.setPassType(GFX_ZLIST_PASS_BACK_LIGHT);

					ux curNodeIdx = _rdrCtxt.getCurrentZListIdx();
					adapter->beginGPUEvent("LightPass");
					adapter->pauseRenderPass(_rdrCtxt);
					if (_resolvePrimitive.getFrontLightBuffer())
					{
						m_frontLightRenderPass.setClear(btrue);
						m_frontLightRenderPass.setClearColor(_resolvePrimitive.getClearFrontLight()?_resolvePrimitive.getClearFrontLightColor():getFrontLightClearColor());
						m_frontLightRenderPass.setBeginZListIdx(curNodeIdx+1); // set the begin to the node following mask resolve
						m_frontLightRenderPass.setEndZListIdx(_rdrCtxt.getEndZListIdx());
						m_frontLightRenderPass.setResolveTarget(btrue);
						m_frontLightRenderPass.setReverseOrder(!_resolvePrimitive.getFrontLightBufferInverted());

						setFrontLightUseClearConstant(bfalse);
						ux lastIdx = findNextFrontMaskResolve(m_frontLightRenderPass); // limit rendering to the next mask resolve of front light
						m_frontLightRenderPass.setEndZListIdx(lastIdx);
						u32 blurQuality = _resolvePrimitive.getBlurQuality();
						eLightTexBuffer lightTexType = (_resolvePrimitive.getBlurFrontLight() ? ((blurQuality%2)==0?LIGHT_BFR_QRT_MOD_0:LIGHT_BFR_QRT_MOD_1):LIGHT_BFR_FULL);
						setFrontLightBufferType(lightTexType);
						adapter->drawRenderPass(m_frontLightRenderPass);

						if (_resolvePrimitive.getBlurFrontLight())
						{
							#ifndef ITF_FINAL
							GFX_ADAPTER->startRasterForPass(GFX_ZLIST_PASS_FRONT_LIGHT);
							#endif // ITF_FINAL
							adapter->blurFrontLightRenderBuffer(_rdrCtxt, blurQuality, _resolvePrimitive.getBlurSize());
							#ifndef ITF_FINAL
							GFX_ADAPTER->endRasterForPass(GFX_ZLIST_PASS_FRONT_LIGHT);
							#endif // ITF_FINAL
						}

						if ( adapter->m_debugZListPass == GFX_ZLIST_PASS_FRONT_LIGHT
						&&  adapter->m_zlistPassObjRef[adapter->m_debugZListPass] == _rdrCtxt.getNode(curNodeIdx).getObjectRef())
							adapter->copyPassToDebugRenderTarget(GFX_ZLIST_PASS_FRONT_LIGHT);
					}
					if (_resolvePrimitive.getBackLightBuffer())
					{
						bbool clearBackLight = _resolvePrimitive.getClearBackLight();
						if (getBackLightUseClearConstant())
						{
							m_backLightRenderPass.setClear(btrue);
							m_backLightRenderPass.setClearColor(clearBackLight?_resolvePrimitive.getClearBackLightColor():getBackLightClearColor());
						}
						else
						{
							m_backLightRenderPass.setClear(clearBackLight);
							m_backLightRenderPass.setClearColor(_resolvePrimitive.getClearBackLightColor());
						}
						m_backLightRenderPass.setEndZListIdx(curNodeIdx);
						ux firstIdx = findPrevBackMaskResolve(m_backLightRenderPass); // limit rendering to the previous mask resolve of back light
						m_backLightRenderPass.setBeginZListIdx(firstIdx);
						m_backLightRenderPass.setResolveTarget(btrue);
						setBackLightUseClearConstant(bfalse);
						u32 blurQuality = _resolvePrimitive.getBlurQuality();

						eLightTexBuffer nextLightTexType = (_resolvePrimitive.getBlurBackLight() ? ((blurQuality%2)==0?LIGHT_BFR_QRT_MOD_0:LIGHT_BFR_QRT_MOD_1):LIGHT_BFR_FULL);

						bbool bNeedRestoreBackLight = (nextLightTexType != m_currentBackLightBufferType);
						if( !_resolvePrimitive.getClearBackLight() && bNeedRestoreBackLight)
						{
							//if needed, restore fullLight
							#ifndef ITF_FINAL
							GFX_ADAPTER->startRasterForPass(GFX_ZLIST_PASS_BACK_LIGHT);
							#endif // ITF_FINAL

							adapter->restoreBackLightRenderBuffer(_rdrCtxt, m_currentBackLightBufferType);

							#ifndef ITF_FINAL
							GFX_ADAPTER->endRasterForPass(GFX_ZLIST_PASS_BACK_LIGHT);
							#endif // ITF_FINAL
						}
						setBackLightBufferType(nextLightTexType);

						adapter->drawRenderPass(m_backLightRenderPass);

						if (_resolvePrimitive.getBlurBackLight())
						{
							#ifndef ITF_FINAL
							GFX_ADAPTER->startRasterForPass(GFX_ZLIST_PASS_BACK_LIGHT);
							#endif // ITF_FINAL

							adapter->blurBackLightRenderBuffer(_rdrCtxt, blurQuality, _resolvePrimitive.getBlurSize());
                        
							#ifndef ITF_FINAL
							GFX_ADAPTER->endRasterForPass(GFX_ZLIST_PASS_BACK_LIGHT);
							#endif // ITF_FINAL
						}

						m_backLightRenderPass.setBeginZListIdx(curNodeIdx);
						if ( adapter->m_debugZListPass == GFX_ZLIST_PASS_BACK_LIGHT
						&&  adapter->m_zlistPassObjRef[adapter->m_debugZListPass] == _rdrCtxt.getNode(curNodeIdx).getObjectRef())
							adapter->copyPassToDebugRenderTarget(GFX_ZLIST_PASS_BACK_LIGHT);
					}
					adapter->unpauseRenderPass(_rdrCtxt, bfalse);
					adapter->endGPUEvent();
				}
			}
		}
    }
} // namespace ITF

