#ifndef _ITF_LIGHTMANAGER_MODEL1_H_
#define _ITF_LIGHTMANAGER_MODEL1_H_

#ifndef _ITF_LIGHTMANAGER_BASE_H_
#include "engine/display/lighting/LightManager_Base.h"
#endif // _ITF_LIGHTMANAGER_BASE_H_

class GFX_Vector4;
namespace ITF
{
	// Light manager implemantation class (from RO2 lighting).
	class GFX_LightManager_Model1 : public GFX_LightManager_Base
	{
	public:

		///// constructor/destructor --------------------------------------------------------------
		GFX_LightManager_Model1(GFXAdapter *_adapter);
		~GFX_LightManager_Model1();

		// Set global states for lighting.
		void setGlobalLighting(View &_view);
		// Set per material lighting states.
		void setMaterialLighting( const GFX_MATERIAL* _gfxMat);
		// Set per material lighting states (for derivate classes).
		void setMaterialLightingInternal( const GFX_MATERIAL* _gfxMat, shaderDefineKey &vertexDefine, shaderDefineKey &pixelDefine, void * constPtr);

		// Init before a new rendering (one call per frame).
		void beginDisplay(f32 _dt);
		// Init before a new view rendering (one call per view).
		void beginViewDisplay();
		// End of the main 3d rendering (one call per view).
		void endDisplay3D();

		virtual shaderDefineKey getDisabledDefines();

		// Accessors for front/back color parameters.
		void setFrontLightClearColor(const Color & _clearColor)
		{
			m_frontLightClearColor = _clearColor;
		}
		void setBackLightClearColor(const Color & _clearColor)
		{
			m_backLightClearColor = _clearColor;
		}
		Color getFrontLightClearColor() const
		{
			return m_frontLightClearColor;
		}
		Color getBackLightClearColor() const
		{
			return m_backLightClearColor;
		}
		bbool getFrontLightUseClearConstant() const
		{
			return m_frontLightUseClearConstant;
		}
		bbool getBackLightUseClearConstant() const
		{
			return m_backLightUseClearConstant;
		}
		void setFrontLightUseClearConstant(bbool _useClearConst)
		{
			m_frontLightUseClearConstant = _useClearConst;
		}
		void setBackLightUseClearConstant(bbool _useClearConst)
		{
			m_backLightUseClearConstant = _useClearConst;
		}

		// Draw lighting pass (ro2 lighting).
		void drawLightingPass( RenderPassContext & _rdrCtxt, const MaskResolvePrimitive &_resolvePrimitive, f32 _Z, f32 _ZWorld, f32 _ZAbs);

		void setFrontLightBufferType(eLightTexBuffer _type) { m_currentFrontLightBufferType = _type;}
		void setBackLightBufferType(eLightTexBuffer _type) { m_currentBackLightBufferType = _type;}
		eLightTexBuffer getFrontLightBufferType() { return m_currentFrontLightBufferType;}
		eLightTexBuffer getBackLightBufferType() { return m_currentBackLightBufferType;}

	protected:
		// Current colors used to clear light buffers.
		Color m_frontLightClearColor;
		Color m_backLightClearColor;
		// For optimisation purpose, when a lighted object is rendered before the first light resolver,
		// the sampling in light buffer is replaced by a shader variation with the color in a constant shader register
		bbool m_frontLightUseClearConstant; // set to true at view start rendering, to false at the first light mask resolver
		bbool m_backLightUseClearConstant;

		RenderPassContext m_frontLightRenderPass;
		RenderPassContext m_backLightRenderPass;

		eLightTexBuffer m_currentFrontLightBufferType;
		eLightTexBuffer m_currentBackLightBufferType;
	};

	#if (GFX_USE_LIGHTINGMODEL == 1)
		typedef GFX_LightManager_Model1 GFX_LightManager_Implementation;
	#endif
} // namespace ITF

#endif // _ITF_LIGHTMANAGER_MODEL1_H_
