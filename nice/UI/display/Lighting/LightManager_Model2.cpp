#include "precompiled_engine.h"


#ifndef _ITF_LIGHTMANAGER_MODEL2_H_
#include "engine/display/lighting/LightManager_Model2.h"
#endif // _ITF_LIGHTMANAGER_MODEL2_H_

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
	GFX_LightEnvironement::GFX_LightEnvironement()
	{
		m_type = LightType_Environement;
		for ( ux i = 0; i < NBL_nbrLightPerEnv; i++ )
		{
			m_lightDir[i].set(1.0f, 0.0f, 0.0f);
			m_lightCol[i] = Color(0.0f, 0.0f, 0.0f, 0.0f);
		}
		m_rimLightDirection.set(1.0f, 0.0f, 0.0f);
		m_rimLightColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
		m_boxFadeDist = 1.0f;
		m_lightColMultiplier = 1.0f;
	}

	GFX_LightOmniSpot::GFX_LightOmniSpot()
	{
		m_type = LightType_OmniSpot;
		m_near = 1.0f;
		m_far = 10.0f;
		m_lightCol = Color(1.0f, 1.0f, 1.0f, 1.0f);
		m_spotDir.set(0.0f, 0.0f, 0.0f);
		m_spotLittleAngle = 0;
		m_spotBigAngle = 0.0f;
		m_lightColMultiplier = 1.0f;
	}

	shaderDefineKey GFX_LightManager_Model2::getDisabledDefines()
	{
		shaderDefineKey defines = GFX_LightManager_Model1::getDisabledDefines();
		defines &= ~m_adapter->mc_define_Light3D;
		defines &= ~m_adapter->mc_define_NormalMap;
		return defines;
	}

	///// constructor/destructor --------------------------------------------------------------
	GFX_LightManager_Model2::GFX_LightManager_Model2(GFXAdapter *_adapter) : GFX_LightManager_Model1(_adapter)
	{
	}
	GFX_LightManager_Model2::~GFX_LightManager_Model2()
	{

	}

	// Init before a new rendering (one call per frame).
	void GFX_LightManager_Model2::beginDisplay(f32 _dt)
	{
		GFX_LightManager_Model1::beginDisplay(_dt);
	}

	// Init before a new view rendering (one call per view).
	void GFX_LightManager_Model2::beginViewDisplay()
	{
		GFX_LightManager_Model1::beginViewDisplay();
	}

	// End of the main 3d rendering (one call per view).
	void GFX_LightManager_Model2::endDisplay3D()
	{
		GFX_LightManager_Model1::endDisplay3D();
	}

	// Set global states for lighting.
	void GFX_LightManager_Model2::setGlobalLighting(View &_view)
	{
		GFX_LightManager_Model1::setGlobalLighting(_view);

		PS_LightParam_Global psLightParam;

		ux envIndex = 0;
		ux omniSpotIndex = 0;
		memset(&psLightParam, 0, sizeof(psLightParam));
		Vec3d viewPosInWorld = GFX_ADAPTER->getWorldViewProjMatrix()->getViewPosInWorld();
		psLightParam.f4_ViewPosInWorld.set(viewPosInWorld, 0.0f);

		u32 zListViewID = _view.getZlistID();
		if(zListViewID == View::ZlistID_invalid)
			return;
        GFX_Zlist<GFX_Light3D> & lightList = GFX_ADAPTER->getZListManager().getZlistAt<GFX_ZLIST_LIGHT_3D>(zListViewID);
        u32 lightNbr = lightList.getNumberEntry();
        if (lightNbr)
        {
            for (u32 i = 0; i < lightNbr; i++)
            {
				const GFX_Light3D *light = lightList.getNodeAt(i).m_primitive;
				switch(light->m_type)
				{
				case LightType_Environement:
					{
						if ( envIndex >= NBL_nbrLightEnv )
							break;
						const GFX_LightEnvironement *lightEnv = (const GFX_LightEnvironement *)light;
						f32 multiplier = lightEnv->m_lightColMultiplier;
						psLightParam.f4_LightDir0[envIndex].set( lightEnv->m_lightDir[0], 0.0f );
						psLightParam.f4_LightDir0[envIndex].normalize3D();
						psLightParam.f4_LightDirCol0[envIndex].mul(lightEnv->m_lightCol[0], multiplier);
						psLightParam.f4_LightDir1[envIndex].set( lightEnv->m_lightDir[1], 0.0f );
						psLightParam.f4_LightDir1[envIndex].normalize3D();
						psLightParam.f4_LightDirCol1[envIndex].mul(lightEnv->m_lightCol[1], multiplier);
						psLightParam.f4_LightDir2[envIndex].set( lightEnv->m_lightDir[2], 0.0f );
						psLightParam.f4_LightDir2[envIndex].normalize3D();
						psLightParam.f4_LightDirCol2[envIndex].mul(lightEnv->m_lightCol[2], multiplier);
						psLightParam.f4_LightDirBoxPos[envIndex].set(lightEnv->m_boxPos, 0.0f);

						Vec2d boxAttM, boxAttA, nearDist, farDist;
						nearDist.x() = ITF::Max(lightEnv->m_boxSize.x() - lightEnv->m_boxFadeDist, 0.0f);
						nearDist.y() = ITF::Max(lightEnv->m_boxSize.y() - lightEnv->m_boxFadeDist, 0.0f);
						farDist = lightEnv->m_boxSize;
						boxAttM.x() = -1.0f / (farDist.x() - nearDist.x());
						boxAttM.y() = -1.0f / (farDist.y() - nearDist.y());
						boxAttA.x() = -(boxAttM.x() * farDist.x());
						boxAttA.y() = -(boxAttM.y() * farDist.y());
						psLightParam.f4_LightDirBoxAtt[envIndex].set(boxAttM, boxAttA);

						psLightParam.f4_RimDir[envIndex].set( lightEnv->m_rimLightDirection, 0.0f );
						psLightParam.f4_RimDir[envIndex].normalize3D();
						psLightParam.f4_RimColor[envIndex].mul( lightEnv->m_rimLightColor, multiplier );

						envIndex++;
					}
					break;
				case LightType_OmniSpot:
					{
						if ( omniSpotIndex >= NBL_nbrLightOmniSpot )
							break;
						const GFX_LightOmniSpot *lightOmniSpot = (const GFX_LightOmniSpot *)light;
						f32 multiplier = lightOmniSpot->m_lightColMultiplier;
						f32 omniAttM, omniAttA, spotAttM, spotAttA;
						f32 nearDist = lightOmniSpot->m_near;
						f32 farDist = lightOmniSpot->m_far;
						nearDist = ITF::Min(nearDist, farDist - 0.001f);
						nearDist *= nearDist;
						farDist *= farDist;
						omniAttM = -1.0f / (farDist - nearDist);
						omniAttA = -(omniAttM * farDist);
						if ( ( !lightOmniSpot->m_spotDir.isNullEpsilon() ) && ( lightOmniSpot->m_spotBigAngle > 0.0f ) )
						{
							f32 littleAngle = lightOmniSpot->m_spotLittleAngle;
							f32 bigAngle = lightOmniSpot->m_spotBigAngle;
							f32 littleCos = f32_Cos(ITF::Min(littleAngle, bigAngle - 0.001f));
							f32 bigCos = f32_Cos(bigAngle);
							spotAttM = -1.0f / (bigCos - littleCos);
							spotAttA = -(spotAttM * bigCos);
						}
						else
						{
							spotAttM = 0.0f;
							spotAttA = 1.0f;
						}

						psLightParam.f4_OmniSpotPos[omniSpotIndex].set( lightOmniSpot->m_boxPos, 0.0f );
						psLightParam.f4_OmniSpotAtt[omniSpotIndex].set( omniAttM, omniAttA, spotAttM, spotAttA );
						psLightParam.f4_OmniSpotDir[omniSpotIndex].set( lightOmniSpot->m_spotDir, 0.0f );
						psLightParam.f4_OmniSpotDir[omniSpotIndex].normalize3D();
						psLightParam.f4_OmniSpotCol[omniSpotIndex].mul( lightOmniSpot->m_lightCol, multiplier );
						omniSpotIndex++;
					}
					break;
				}
            }
		}


/*		for ( u32 lightIndex = 0; lightIndex < m_lightList.size(); lightIndex++ )
		{
			const GFX_Light3D *light = m_lightList[lightIndex];
		}*/
		psLightParam.f4_LightNbr.set(f32(envIndex), f32(omniSpotIndex), 0.0f, 0.0f);

		m_adapter->setPixelShaderConstantF( PS_Attrib_lightParamGlobal, (f32*)&psLightParam, sizeof(psLightParam) / 16 );
	}

	// Set per material lighting states.
    void GFX_LightManager_Model2::setMaterialLighting( const GFX_MATERIAL* _gfxMat)
    {
		GFXAdapter *adapter = m_adapter;
		PS_LightParam_Object psLightParam;
		ITF_MemSet(&psLightParam, 0, sizeof(psLightParam));
		shaderDefineKey vertexDefine = 0;
		shaderDefineKey pixelDefine = 0;

		setMaterialLightingInternal( _gfxMat, vertexDefine, pixelDefine, &psLightParam);

        if (_gfxMat->useNormalMapLighting())
        {
			bbool useNormalMap;
            Texture* normalMap = _gfxMat->getTexture(TEXSET_ID_NORMAL);
            if(normalMap)
			{
                adapter->bindTexture(8, normalMap);
				useNormalMap = btrue;
			}
			else
				useNormalMap = bfalse;
            Texture* specularMap = _gfxMat->getTexture(TEXSET_ID_SPECULAR);
            if(specularMap)
			{
                adapter->bindTexture(9, specularMap);
			}
			else
                adapter->bindTexture(9, adapter->getGreyOpaqueTexture());

			const Matrix44 &world = adapter->m_worldViewProj.getWorld();
            vertexDefine |= adapter->mc_define_Light3D;
            pixelDefine |= adapter->mc_define_Light3D;
            pixelDefine |= adapter->mc_define_Light;
			if ( useNormalMap )
				pixelDefine |= adapter->mc_define_NormalMap;

			NormalLightingParam *normalMapParam = adapter->getNormalMapLightParam();
			Matrix44 transformMat;
// 			f32 rimPower = 0.0f;
			if ( normalMapParam )
			{
				psLightParam.f4_LightContrast.set(normalMapParam->m_lightContrast, normalMapParam->m_lightBrigthness, normalMapParam->m_rimPower, 0.0f);

				Matrix44 rotMat, transformI, transformIT;
				const Vec3d& rot = normalMapParam->m_rotation;
				rotMat.setRotationXYZ(rot.x(), rot.y(), rot.z());
				transformMat.mul(world, rotMat);
				transformI.inverse(world);
				Color &rimColL = normalMapParam->m_rimLightColor;
				psLightParam.f4_RimColor.set(rimColL);
				psLightParam.f4_specParam.set(normalMapParam->m_specIntensity, normalMapParam->m_specSize, 0.0f, 0.0f);
			}
			else
			{
				transformMat = world;
				psLightParam.f4_LightContrast.set(1.0f, 0.0f, 0.0f, 0.0f);
				psLightParam.f4_RimColor.set(1.0f, 1.0f, 1.0f, 1.0f);
				psLightParam.f4_specParam.set(0.0f, 30.0f, 0.0f, 0.0f);
			}
			const GFX_Vector4 *transformVec;
			transformVec = (GFX_Vector4*)&transformMat;
			psLightParam.f4_rotMatrix[0] = transformVec[0];
			psLightParam.f4_rotMatrix[1] = transformVec[1];
			psLightParam.f4_rotMatrix[2] = transformVec[2];
		}

		adapter->setPixelShaderConstantF( PS_Attrib_lightParamObject, (f32*)&psLightParam, sizeof(psLightParam)/16 );
		adapter->addVertexShaderDefines(vertexDefine);
		adapter->addPixelShaderDefines(pixelDefine);
    }
} // namespace ITF

