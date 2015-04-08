#ifndef _ITF_RENDERPARAMMANAGER_H_
#define _ITF_RENDERPARAMMANAGER_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

namespace ITF
{
	enum RenderParamType
	{
		RPType_ClearColor,
		RPType_Lighting,
		RPType_Misc,
		RPType_Mask,
		RPType_Count,
		RPType_Invalid = 0xff,
	};

	// A sub render param class contains some global parameters for rendering (global parameters = parameters apply to the entire screen or on all objets).
	class SubRenderParam : public IRTTIObject
	{
	public:
		DECLARE_OBJECT_CHILD_RTTI(SubRenderParam,IRTTIObject,3733934874)
        DECLARE_SERIALIZE_VIRTUAL()

		// Constructor.
		SubRenderParam();
		// Constructor.
		virtual ~SubRenderParam() {}

		// Blend a sub render parameter.
		virtual void blend(const SubRenderParam &_other, f32 _weightInit, f32 _weightNew) {}
		// Reset sub render param to a weight default values. (weight=0 => zero values; weight=1 => default value)
		virtual void reset(f32 _weight = 1.0f) {}
		// Finish blending and apply parameter in the adapter if needed.
		virtual void apply() {}

		// Get sub render param type.
		inline RenderParamType getType() const
		{
			return m_type;
		}
	protected:
		// Type of render param.
		RenderParamType m_type;
	public:
		// Enable/disable ?
		bbool	m_enable;
	};

	class SubRenderParam_ClearColor : public SubRenderParam
	{
	public:
		DECLARE_OBJECT_CHILD_RTTI(SubRenderParam_ClearColor,SubRenderParam,1989967594)
        DECLARE_SERIALIZE()

		// Constructor.
		SubRenderParam_ClearColor();

		// Blend a sub render parameter.
		virtual void blend(const SubRenderParam &_other, f32 _weightInit, f32 _weightNew);
		// Reset sub render param to a weight default values. (weight=0 => zero values; weight=1 => default value)
		virtual void reset(f32 _weight = 1.0f);
		// Finish blending and apply parameter in the adapter if needed.
		virtual void apply();

	public:
		Color m_clearColor;
        Color m_clearFrontLightColor;
        Color m_clearBackLightColor;
	};

	class SubRenderParam_Misc : public SubRenderParam
	{
	public:
		DECLARE_OBJECT_CHILD_RTTI(SubRenderParam_Misc,SubRenderParam,459897321)
        DECLARE_SERIALIZE()

		// Constructor.
		SubRenderParam_Misc();

		// Blend a sub render parameter.
		virtual void blend(const SubRenderParam &_other, f32 _weightInit, f32 _weightNew);
		// Reset sub render param to a weight default values. (weight=0 => zero values; weight=1 => default value)
		virtual void reset(f32 _weight = 1.0f);
		// Finish blending and apply parameter in the adapter if needed.
		virtual void apply();

	public:
		// Allow force or disable Z pass.
    	GFXViewZPassOverride m_ZPassOverride;
	};

	class SubRenderParam_Mask : public SubRenderParam
	{
	public:
		DECLARE_OBJECT_CHILD_RTTI(SubRenderParam_Mask,SubRenderParam,459897321)
        DECLARE_SERIALIZE()

		// Constructor.
		SubRenderParam_Mask();

		// Blend a sub render parameter.
		virtual void blend(const SubRenderParam &_other, f32 _weightInit, f32 _weightNew);
		// Reset sub render param to a weight default values. (weight=0 => zero values; weight=1 => default value)
		virtual void reset(f32 _weight = 1.0f);
		// Finish blending and apply parameter in the adapter if needed.
		virtual void apply();

	public:
		// Allow force or disable Z pass.
    	Color m_silhouetteColor;
	};

	class SubRenderParam_Lighting : public SubRenderParam
	{
	public:
		DECLARE_OBJECT_CHILD_RTTI(SubRenderParam_Lighting,SubRenderParam,459897321)
        DECLARE_SERIALIZE()

		// Constructor.
		SubRenderParam_Lighting();

		// Blend a sub render parameter.
		virtual void blend(const SubRenderParam &_other, f32 _weightInit, f32 _weightNew);
		// Reset sub render param to a weight default values. (weight=0 => zero values; weight=1 => default value)
		virtual void reset(f32 _weight = 1.0f);
		// Finish blending and apply parameter in the adapter if needed.
		virtual void apply();

	public:
		// Global color blended with the instance color.
		Color m_globalColor;
		// Global fog blended with instance static fog.
        Color m_globalStaticFog;
		// Fog opacity added to the instance static fog opacity.
		f32 m_globalFogOpacity;
		// Global brightness added to the instance brightness.
		f32 m_globalBrightness;
	};

	// A render param is a list of many sub render param with a weight, which will be blend together to compute final global render parameters.
	class GFX_RenderParam : public GFXObject
	{
		friend class RenderParamManager;
    public:
		GFX_RenderParam();
		~GFX_RenderParam();

		// Push a new sub render param. It's just a link, caller keep the sub render param ownership.
		void pushSubRenderParam(const SubRenderParam& _sub);
		// Remove a sub render param.
		void removeSubRenderParam(const SubRenderParam& _sub);

	protected:
		// List of sub render parameters.
		ITF::vector<const SubRenderParam*> m_subRenderParamList;

	public:
		// Weight value of the render param.
		f32	m_weight;
		// Priority. First render param with same priority are blended together with weight renormalization if needed.
		// Resulting render param per priority are blended after in priority order.
		u32 m_priority;
	};

	// Render parameters manager class
	// Allow to register render param class and blend them to compute final render param to use.
	class RenderParamManager
	{
	public:

		///// constructor/destructor --------------------------------------------------------------
		RenderParamManager();
		~RenderParamManager();

		// Sort render param.
		void                        sortRenderParam();
		// Blend render param for one priority. Return index of the next priority.
		ux                          blendRenderParam(ux _indexFirst, SubRenderParam** _subRenderParamTable, f32 *_weightTotalTable);
		// Compute current parameters from registered ones (called at the begin of the rendering).
		void                        processRenderParam(View &_view);

		// Access to a current final sub render param.
		inline const SubRenderParam& getCurrentSubRenderParam(RenderParamType _type) const
		{
			ITF_ASSERT(_type < RPType_Count);
			return *m_currentSubRenderParamTable[_type];
		}

	protected:
		// Registered render param list.
		ITF::vector<const GFX_RenderParam*> m_renderParamList;

		// Current final state of each sub render param.
		SubRenderParam*	m_currentSubRenderParamTable[RPType_Count];
		// Temporary state of each sub render param.
		SubRenderParam*	m_tempSubRenderParamTable[RPType_Count];
	};

} // namespace ITF

#endif // _ITF_RENDERPARAMMANAGER_H_
