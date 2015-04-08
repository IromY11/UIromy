#ifndef _ITF_LIGHTMANAGER_BASE_H_
#define _ITF_LIGHTMANAGER_BASE_H_

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/resource.h"
#endif //_ITF_RESOURCE_H_

#ifndef _ITF_KEYARRAY_H_
#include "core/container/KeyArray.h"
#endif //_ITF_KEYARRAY_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#define float4 ITF::GFX_Vector4
#include "../bin/Shaders_dev/Unified/ShaderParameters.fxh" 
#undef float4

class GFX_Vector4;
namespace ITF
{
	enum GFX_LightType
	{
		LightType_Environement,
		LightType_OmniSpot,
		LightType_Invalid = 0xff,
	};

	class GFX_Light3D : public GFXObject
	{
    public:

        ///// constructor ----------------------------------------------------------
        GFX_Light3D();

        ///// members --------------------------------------------------------------
    public:
        u32		m_type : 8;
		Vec3d	m_boxPos;
		Vec2d	m_boxSize;

    private:
	};


	// Light manager base class.
	class GFX_LightManager_Base
	{
	public:

		///// constructor/destructor --------------------------------------------------------------
		GFX_LightManager_Base(GFXAdapter *_adapter);
		virtual ~GFX_LightManager_Base();

		// Set global states for lighting.
		/*virtual*/ void setGlobalLighting(View &_view) {}
		// Set per material lighting states.
		/*virtual*/ void setMaterialLighting( const GFX_MATERIAL* _gfxMat) {}
		// Init before a new rendering (one call per frame).
		/*virtual*/ void beginDisplay(f32 _dt) {}
		// Init before a new view rendering (one call per view).
		/*virtual*/ void beginViewDisplay() {}
		// End of the main 3d rendering (one call per view).
		/*virtual*/ void endDisplay3D() {}

		virtual shaderDefineKey getDisabledDefines();

		// Draw lighting pass (ro2 lighting).
		/*virtual*/ void drawLightingPass( RenderPassContext & _rdrCtxt, const MaskResolvePrimitive &_resolvePrimitive, f32 _Z, f32 _ZWorld, f32 _ZAbs){}

		// Accessors for front/back color parameters.
		/*virtual*/ Color getDefaultFrontLightClearColor() const { return Color(1.f, 0.5f, 0.5f, 0.5f); }
		/*virtual*/ Color getDefaultBackLightClearColor() const { return Color(1.f, 0.0f, 0.0f, 0.0f); }
		/*virtual*/ void setFrontLightClearColor(const Color & _clearColor) {}
		/*virtual*/ void setBackLightClearColor(const Color & _clearColor) {}
		/*virtual*/ Color getFrontLightClearColor() const { return COLOR_BLACK; }
		/*virtual*/ Color getBackLightClearColor() const { return COLOR_BLACK; }
		/*virtual*/ bbool getFrontLightUseClearConstant() const { return true; }
		/*virtual*/ bbool getBackLightUseClearConstant() const { return true; }
		/*virtual*/ void setFrontLightUseClearConstant(bbool _useClearConst) {}
		/*virtual*/ void setBackLightUseClearConstant(bbool _useClearConst) {}
		/*virtual*/ eLightTexBuffer getFrontLightBufferType() { return LIGHT_BFR_FULL;}
		/*virtual*/ eLightTexBuffer getBackLightBufferType() { return LIGHT_BFR_FULL;}
	protected:
		GFXAdapter *m_adapter;
	};
} // namespace ITF

#endif // _ITF_LIGHTMANAGER_BASE_H_
