#include "precompiled_engine.h"


#ifndef _ITF_RENDERPARAMMANAGER_H_
#include    "engine/display/RenderParamManager.h"
#endif // _ITF_RENDERPARAMMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_LIGHTMANAGER_H_
#include "engine/display/lighting/LightManager.h"
#endif // _ITF_LIGHTMANAGER_H_

namespace ITF
{
	// Constructor.
	SubRenderParam::SubRenderParam()
	{
		m_type = RPType_Invalid;
		m_enable = false;
	}

    IMPLEMENT_OBJECT_RTTI(SubRenderParam)

    BEGIN_SERIALIZATION(SubRenderParam)
    SERIALIZE_MEMBER("Enable", m_enable);
    END_SERIALIZATION()

	IMPLEMENT_OBJECT_RTTI(SubRenderParam_ClearColor)

    BEGIN_SERIALIZATION_CHILD(SubRenderParam_ClearColor)
        SERIALIZE_MEMBER("ClearColor", m_clearColor);
        SERIALIZE_MEMBER("ClearFrontLightColor", m_clearFrontLightColor);
        SERIALIZE_MEMBER("ClearBackLightColor", m_clearBackLightColor);
    END_SERIALIZATION()

	// Constructor.
	SubRenderParam_ClearColor::SubRenderParam_ClearColor()
	{
		m_type = RPType_ClearColor;
        SubRenderParam_ClearColor::reset();
	}

	// Blend a sub render parameter.
	void SubRenderParam_ClearColor::blend(const SubRenderParam &_other, f32 _weightInit, f32 _weightNew)
	{
		const SubRenderParam_ClearColor &otherClearColor = (const SubRenderParam_ClearColor &)_other;
		m_clearColor *= _weightInit;
		m_clearFrontLightColor *= _weightInit;
		m_clearBackLightColor *= _weightInit;
		m_clearColor.maddItself(otherClearColor.m_clearColor, _weightNew);
		m_clearFrontLightColor.maddItself(otherClearColor.m_clearFrontLightColor, _weightNew);
		m_clearBackLightColor.maddItself(otherClearColor.m_clearBackLightColor, _weightNew);
	}
	// Reset sub render param to a weight default values. (weight=0 => zero values; weight=1 => default value)
	void SubRenderParam_ClearColor::reset(f32 _weight)
	{
		if (GFX_ADAPTER)
		{
			m_clearColor = GFX_ADAPTER->getDefaultClearColor();
			GFX_LightManager &lightManager = GFX_ADAPTER->getLightManager();
			m_clearFrontLightColor = lightManager.getDefaultFrontLightClearColor();
			m_clearBackLightColor = lightManager.getDefaultBackLightClearColor();
		}
		else
		{
			m_clearColor = COLOR_BLACK;
			m_clearFrontLightColor = COLOR_BLACK;
			m_clearBackLightColor = COLOR_BLACK;
		}
		m_clearColor *= _weight;
		m_clearFrontLightColor *= _weight;
		m_clearBackLightColor *= _weight;
	}
	// Finish blending and apply parameter in the adapter if needed.
	void SubRenderParam_ClearColor::apply()
	{
        GFX_ADAPTER->setClearColor(m_clearColor);
		GFX_LightManager &lightManager = GFX_ADAPTER->getLightManager();
        lightManager.setFrontLightClearColor(m_clearFrontLightColor);
        lightManager.setBackLightClearColor(m_clearBackLightColor);
	}

	IMPLEMENT_OBJECT_RTTI(SubRenderParam_Lighting)

    BEGIN_SERIALIZATION_CHILD(SubRenderParam_Lighting)
        SERIALIZE_MEMBER("GlobalColor", m_globalColor);
        SERIALIZE_MEMBER("GlobalStaticFog", m_globalStaticFog);
        SERIALIZE_MEMBER("GlobalFogOpacity", m_globalFogOpacity);
        SERIALIZE_MEMBER("GlobalBrightness", m_globalBrightness);
    END_SERIALIZATION()

	// Constructor.
	SubRenderParam_Lighting::SubRenderParam_Lighting()
	{
		m_type = RPType_Lighting;
        reset();
	}

	// Blend a sub render parameter.
	void SubRenderParam_Lighting::blend(const SubRenderParam &_other, f32 _weightInit, f32 _weightNew)
	{
		const SubRenderParam_Lighting &otherLighting = (const SubRenderParam_Lighting &)_other;
		m_globalColor *= _weightInit;
		m_globalStaticFog *= _weightInit;
		m_globalFogOpacity *= _weightInit;
		m_globalBrightness *= _weightInit;
		m_globalColor.maddItself(otherLighting.m_globalColor, _weightNew);
		m_globalStaticFog.maddItself(otherLighting.m_globalStaticFog, _weightNew);
		m_globalFogOpacity += otherLighting.m_globalFogOpacity * _weightNew;
		m_globalBrightness += otherLighting.m_globalBrightness * _weightNew;
	}
	// Reset sub render param to a weight default values. (weight=0 => zero values; weight=1 => default value)
	void SubRenderParam_Lighting::reset(f32 _weight)
	{
        m_globalColor = Color(0.0f, 0.0f, 0.0f, 0.0f);
        m_globalStaticFog = Color(0.0f, 0.0f, 0.0f, 0.0f);
        m_globalFogOpacity = 0.0f;
        m_globalBrightness = 0.0f;
		m_globalColor *= _weight;
		m_globalStaticFog *= _weight;
		m_globalFogOpacity *= _weight;
		m_globalBrightness *= _weight;
	}
	// Finish blending and apply parameter in the adapter if needed.
	void SubRenderParam_Lighting::apply()
	{
		GFX_ADAPTER->setGlobalColorBlend(m_globalColor);
		GFX_ADAPTER->setGlobalStaticFog(m_globalStaticFog);
		GFX_ADAPTER->setGlobalFogOpacity(m_globalFogOpacity);
		GFX_ADAPTER->setGlobalBrightness(m_globalBrightness);
	}

	IMPLEMENT_OBJECT_RTTI(SubRenderParam_Misc)

    BEGIN_SERIALIZATION_CHILD(SubRenderParam_Misc)
        SERIALIZE_ENUM_GFX_ZPASS_OVERRIDE("ZPassOverride", m_ZPassOverride);
    END_SERIALIZATION()

	// Constructor.
	SubRenderParam_Misc::SubRenderParam_Misc()
	{
		m_type = RPType_Misc;
        reset();
	}

	// Blend a sub render parameter.
	void SubRenderParam_Misc::blend(const SubRenderParam &_other, f32 _weightInit, f32 _weightNew)
	{
		const SubRenderParam_Misc &otherMisc = (const SubRenderParam_Misc &)_other;
        if ( _weightNew > 0.5f )
            m_ZPassOverride = otherMisc.m_ZPassOverride;
	}
	// Reset sub render param to a weight default values. (weight=0 => zero values; weight=1 => default value)
	void SubRenderParam_Misc::reset(f32 _weight)
	{
        m_ZPassOverride = GFX_VIEW_ZPASS_DEFAULT;
	}
	// Finish blending and apply parameter in the adapter if needed.
	void SubRenderParam_Misc::apply()
	{
	}

	IMPLEMENT_OBJECT_RTTI(SubRenderParam_Mask)

    BEGIN_SERIALIZATION_CHILD(SubRenderParam_Mask)
        SERIALIZE_MEMBER("SilhouetteColor", m_silhouetteColor);
    END_SERIALIZATION()

	// Constructor.
	SubRenderParam_Mask::SubRenderParam_Mask()
	{
		m_type = RPType_Mask;
        reset();
	}

	// Blend a sub render parameter.
	void SubRenderParam_Mask::blend(const SubRenderParam &_other, f32 _weightInit, f32 _weightNew)
	{
		const SubRenderParam_Mask &otherMask = (const SubRenderParam_Mask &)_other;
		m_silhouetteColor *= _weightInit;
		m_silhouetteColor.maddItself(otherMask.m_silhouetteColor, _weightNew);
	}
	// Reset sub render param to a weight default values. (weight=0 => zero values; weight=1 => default value)
	void SubRenderParam_Mask::reset(f32 _weight)
	{
        m_silhouetteColor = COLOR_BLACK;
	}
	// Finish blending and apply parameter in the adapter if needed.
	void SubRenderParam_Mask::apply()
	{
	}

	GFX_RenderParam::GFX_RenderParam()
	{
		m_weight = 1.0f;
		m_priority = 10;
	}

	GFX_RenderParam::~GFX_RenderParam()
	{
	}

	// Push a new sub render param. It's just a link, caller keep the sub render param ownership.
	void GFX_RenderParam::pushSubRenderParam(const SubRenderParam& _sub)
	{
		m_subRenderParamList.push_back(&_sub);
	}
	// Remove a sub render param.
	void GFX_RenderParam::removeSubRenderParam(const SubRenderParam& _sub)
	{
		i32 pos = m_subRenderParamList.find(&_sub);
		if ( pos >= 0 )
			m_subRenderParamList.removeAtUnordered(pos);
	}

	///// constructor/destructor --------------------------------------------------------------
	RenderParamManager::RenderParamManager()
	{
		m_currentSubRenderParamTable[RPType_ClearColor] = newAlloc(mId_GfxAdapter, SubRenderParam_ClearColor());
		m_currentSubRenderParamTable[RPType_Lighting] = newAlloc(mId_GfxAdapter, SubRenderParam_Lighting());
		m_currentSubRenderParamTable[RPType_Misc] = newAlloc(mId_GfxAdapter, SubRenderParam_Misc());
		m_currentSubRenderParamTable[RPType_Mask] = newAlloc(mId_GfxAdapter, SubRenderParam_Mask());
		m_tempSubRenderParamTable[RPType_ClearColor] = newAlloc(mId_GfxAdapter, SubRenderParam_ClearColor());
		m_tempSubRenderParamTable[RPType_Lighting] = newAlloc(mId_GfxAdapter, SubRenderParam_Lighting());
		m_tempSubRenderParamTable[RPType_Misc] = newAlloc(mId_GfxAdapter, SubRenderParam_Misc());
		m_tempSubRenderParamTable[RPType_Mask] = newAlloc(mId_GfxAdapter, SubRenderParam_Mask());
	}
	RenderParamManager::~RenderParamManager()
	{
		for ( ux i = 0; i < RPType_Count; i++ )
		{
			delete( m_currentSubRenderParamTable[i] );
			m_currentSubRenderParamTable[i] = nullptr;
			delete( m_tempSubRenderParamTable[i] );
			m_tempSubRenderParamTable[i] = nullptr;
		}
	}

	// Sort render param.
	void RenderParamManager::sortRenderParam()
	{
		u32 rpCount = m_renderParamList.size();
		if (!rpCount)
			return;
		bbool needSort = true; // Also used as bubble sort loop condition.
		while ( needSort )
		{
			needSort = false;
			for( ux i = 0; i + 1 < rpCount; ++i )
			{
				const GFX_RenderParam *renderParam0 = m_renderParamList[i];
				const GFX_RenderParam *renderParam1 = m_renderParamList[i+1];
				if ( renderParam0->m_priority > renderParam1->m_priority )
				{
					m_renderParamList[i] = renderParam1;
					m_renderParamList[i+1] = renderParam0;
					needSort = true;
				}
			}
		}
	}

	// Blend render param for one priority. Return index of the next priority.
	ux RenderParamManager::blendRenderParam(ux _indexFirst, SubRenderParam** _subRenderParamTable, f32 *_weightTotalTable)
	{
		ITF_MemSet(_weightTotalTable, 0, sizeof(f32) * RPType_Count);

		u32 priority = m_renderParamList[_indexFirst]->m_priority;

		// Compute total weight per sub param type.
		ux i;
		for( i = _indexFirst; i < m_renderParamList.size(); ++i )
		{
			const GFX_RenderParam *renderParam = m_renderParamList[i];
			if ( renderParam->m_priority != priority )
				break;
			for ( ux subIndex = 0; subIndex < renderParam->m_subRenderParamList.size(); ++subIndex )
			{
				const SubRenderParam *sub = renderParam->m_subRenderParamList[subIndex];
				if ( sub && sub->m_enable )
					_weightTotalTable[sub->getType()] += renderParam->m_weight;
			}
		}
		ux nextPriorityIndex = i;

		// Init current sub param.
		for ( ux subIndex = 0; subIndex < RPType_Count; ++subIndex )
		{
			_subRenderParamTable[subIndex]->reset( 0.0f );
		}

		// Blend all sub params.
		for( ux i = _indexFirst; i < nextPriorityIndex; ++i )
		{
			const GFX_RenderParam *renderParam = m_renderParamList[i];
			f32 weight = renderParam->m_weight;
			if ( weight > 0.0f )
			{
				for ( ux subIndex = 0; subIndex < renderParam->m_subRenderParamList.size(); ++subIndex )
				{
					const SubRenderParam *sub = renderParam->m_subRenderParamList[subIndex];
					if ( sub && sub->m_enable )
					{
						ux type = sub->getType();
//						_subRenderParamTable[type]->blend( *sub, 1.0f, weight / ITF::Max(1.0f, _weightTotalTable[type]) );
						_subRenderParamTable[type]->blend( *sub, 1.0f, weight / _weightTotalTable[type] );
					}
				}
			}
		}

		return nextPriorityIndex;
	}

	// Compute current parameters from registered ones (called at the begin of the rendering).
	void RenderParamManager::processRenderParam(View &_view)
	{
		u32 zListViewID = _view.getZlistID();
		if(zListViewID == View::ZlistID_invalid)
			return;

		m_renderParamList.clear();
		GFX_Zlist<GFX_RenderParam> & renderParamList = GFX_ADAPTER->getZListManager().getZlistAt<GFX_ZLIST_RENDER_PARAM>(zListViewID);
        u32 renderParamNbr = renderParamList.getNumberEntry();
        if (renderParamNbr)
        {
            for (u32 i = 0; i < renderParamNbr; i++)
            {
				const GFX_RenderParam *renderParam = renderParamList.getNodeAt(i).m_primitive;
				m_renderParamList.push_back(renderParam);
			}
		}

		// Init current sub param.
		for ( ux subIndex = 0; subIndex < RPType_Count; ++subIndex )
		{
			m_currentSubRenderParamTable[subIndex]->reset( 1.0f );
		}

		u32 rpCount = m_renderParamList.size();
		if ( rpCount )
		{
			sortRenderParam();

			// Blend render param per priority.
			f32 weightTotalTable[RPType_Count];
			u32 priorityIndex = 0;
			while ( priorityIndex < rpCount )
			{
				priorityIndex = blendRenderParam(priorityIndex, m_tempSubRenderParamTable, weightTotalTable);

				for ( ux subIndex = 0; subIndex < RPType_Count; ++subIndex )
				{
					f32 alpha = ITF::Min(1.0f, weightTotalTable[subIndex]);
					if ( alpha > 0.0f )
						m_currentSubRenderParamTable[subIndex]->blend( *m_tempSubRenderParamTable[subIndex], 1.0f - alpha, alpha );
				}
			}
		}

		// Apply all sub render param.
		for ( ux subIndex = 0; subIndex < RPType_Count; ++subIndex )
		{
			m_currentSubRenderParamTable[subIndex]->apply();
		}
	}

} // namespace ITF

