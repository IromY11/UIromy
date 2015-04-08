#ifndef _ITF_LIGHTMANAGER_MODEL2_H_
#define _ITF_LIGHTMANAGER_MODEL2_H_

#ifndef _ITF_LIGHTMANAGER_BASE_H_
#include "engine/display/lighting/LightManager_Base.h"
#endif // _ITF_LIGHTMANAGER_BASE_H_

#ifndef _ITF_LIGHTMANAGER_MODEL1_H_
#include "engine/display/Lighting/LightManager_Model1.h"
#endif // _ITF_LIGHTMANAGER_MODEL1_H_

class GFX_Vector4;
namespace ITF
{
	class GFX_LightEnvironement : public GFX_Light3D
	{
	public:
        ///// constructor ----------------------------------------------------------
        GFX_LightEnvironement();

		Vec3d	m_lightDir[NBL_nbrLightPerEnv];
		Color	m_lightCol[NBL_nbrLightPerEnv];
		Vec3d	m_rimLightDirection;
		Color	m_rimLightColor;
		f32		m_boxFadeDist;
		f32		m_lightColMultiplier;
	};

	class GFX_LightOmniSpot : public GFX_Light3D
	{
	public:
        ///// constructor ----------------------------------------------------------
        GFX_LightOmniSpot();

        f32     m_near;
        f32     m_far;
        Color   m_lightCol;
		Vec3d	m_spotDir;
		f32		m_spotLittleAngle;
		f32		m_spotBigAngle;
		f32		m_lightColMultiplier;
	};

	// Light manager implementation class (initially lighting for pop project).
	class GFX_LightManager_Model2 : public GFX_LightManager_Model1
	{
	public:

		///// constructor/destructor --------------------------------------------------------------
		GFX_LightManager_Model2(GFXAdapter *_adapter);
		~GFX_LightManager_Model2();

		// Set global states for lighting.
		void setGlobalLighting(View &_view);
		// Set per material lighting states.
		void setMaterialLighting( const GFX_MATERIAL* _gfxMat);

		// Init before a new rendering (one call per frame).
		void beginDisplay(f32 _dt);
		// Init before a new view rendering (one call per view).
		void beginViewDisplay();
		// End of the main 3d rendering (one call per view).
		void endDisplay3D();

		virtual shaderDefineKey getDisabledDefines();

	protected:
	};

	#if (GFX_USE_LIGHTINGMODEL == 2)
		typedef GFX_LightManager_Model2 GFX_LightManager_Implementation;
	#endif
} // namespace ITF

#endif // _ITF_LIGHTMANAGER_MODEL2_H_
