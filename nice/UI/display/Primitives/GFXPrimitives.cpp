#include "precompiled_engine.h"

#ifndef ITF_GFX_PRIMITIVES_H_
#include "engine\display\Primitives\GFXPrimitives.h"
#endif

#ifndef ITF_GFX_RENDER_PASS_H_
#include "engine\display\GFXRenderPass.h"
#endif // ITF_GFX_RENDER_PASS_H_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif //SERIALIZEROBJECT_H

#ifndef ITF_STRIPPED_DATA
    #ifndef _ITF_SERIALIZED_OBJECT_CONTENT_H_
    #include "core/serializer/SerializedObjectContent.h"
    #endif //_ITF_SERIALIZED_OBJECT_CONTENT_H_
#endif // ITF_STRIPPED_DATA

namespace ITF
{
    BEGIN_SERIALIZATION(NormalLightingParam)
        SERIALIZE_MEMBER("LightBrightness",m_lightBrigthness);
        SERIALIZE_MEMBER("LightContrast",m_lightContrast);
        SERIALIZE_MEMBER("Rotation",m_rotation);
		SERIALIZE_MEMBER("UseNormalMapLighting",m_useNormalMapLighting);
		SERIALIZE_MEMBER("RimLightColor",m_rimLightColor);
		SERIALIZE_MEMBER("RimLightPower",m_rimPower);
		SERIALIZE_MEMBER("SpecIntensity",m_specIntensity);
		SERIALIZE_MEMBER("SpecSize",m_specSize);
    END_SERIALIZATION()

    // Common Param
    BEGIN_SERIALIZATION(GFXPrimitiveParam)
        SERIALIZE_MEMBER("colorFactor",m_colorFactor);
        SERIALIZE_MEMBER("FrontLightBrightness",m_frontLightBrigthness);
        SERIALIZE_MEMBER("FrontLightContrast",m_frontLightContrast);
        SERIALIZE_MEMBER("BackLightBrightness",m_backLightBrigthness);
        SERIALIZE_MEMBER("BackLightContrast",m_backLightContrast);
        SERIALIZE_MEMBER("colorFog",m_colorFog);
        SERIALIZE_MEMBER("DynamicFogFactor",m_fogFactor);
        SERIALIZE_BOOL("useStaticFog",m_useStaticFog);
        SERIALIZE_BOOL("UseGlobalLighting",m_useGlobalLighting);
        SERIALIZE_BOOL("RenderInReflections",m_renderInReflection);
		SERIALIZE_BOOL("UseZInject",m_useZInject);
		SERIALIZE_OBJECT("NormalLightParam", m_normalLightParam);
		SERIALIZE_MEMBER("OutlineColor",m_outlineColor);
		SERIALIZE_MEMBER("OutlineWidth",m_outlineWidth);
		SERIALIZE_MEMBER("OutlineGlow",m_outlineGlow);
		SERIALIZE_MEMBER("ViewportVisibility",m_viewportVisibility);
        SERIALIZE_ENUM_GFX_OCCLUDE_INFO("gfxOccludeInfo", m_gfxOccludeInfo);
        SERIALIZE_ENUM_GFX_MASK_INFO("gfxMaskInfo", m_gfxMaskInfo);
        SERIALIZE_MEMBER("colorForMask1", m_colorForMask[0]);
        SERIALIZE_MEMBER("colorForMask2", m_colorForMask[1]);
        SERIALIZE_MEMBER("colorForMask3", m_colorForMask[2]);
        SERIALIZE_MEMBER("saturation", m_saturation);
    END_SERIALIZATION()

    // GFX Primitive

    GFXPrimitiveParam GFXPrimitiveParam::ms_default;
    GFXPrimitiveParam GFXPrimitiveParam::ms_defaultNoFog(btrue);


    GFXPrimitive::GFXPrimitive(Type _primType)
        : m_primType(_primType)
        , m_filterFlag(GFX_ZLIST_PASS_ALL_FLAG & ~GFX_ZLIST_PASS_ALL_SPECIAL_FLAG) // render everywhere by default but not in specila pass
    {
    }

    void GFXPrimitive::adjustZPassFilterFlag(GFXOccludeInfo _gfxOccludeInfo)
    {
        if (_gfxOccludeInfo == GFX_OCCLUDE_INFO_SMALL_OR_TRANSPARENT)
            removePassFilterFlag(GFX_ZLIST_PASS_ZPREPASS_FLAG);
        if (_gfxOccludeInfo == GFX_OCCLUDE_INFO_BIG_OPAQUE)
            addPassFilterFlag(GFX_ZLIST_PASS_ZPREPASS_FLAG);
		if (_gfxOccludeInfo == GFX_OCCLUDE_INFO_ZPASS_ONLY)
			m_filterFlag = GFX_ZLIST_PASS_ZPREPASS_FLAG;
    }
    
    void GFXPrimitive::addPassFilterFlag(const GFX_MATERIAL & _mat, GFXOccludeInfo _gfxOccludeInfo)
    {
        if (_mat.getRegularRender())
        {
            m_filterFlag |= GFX_ZLIST_PASS_REGULAR_FLAG 
                            | GFX_ZLIST_PASS_REFLECTION_FLAG
                            | GFX_ZLIST_PASS_2D_FLAG
                            | GFX_ZLIST_PASS_SCREENSHOT_2D_FLAG
                            | GFX_ZLIST_PASS_3D_FLAG; // 3D/2D/Screenshot2D pass are assumed as a regular one (from material point of view)
#ifdef ITF_VITA
            // Vita or any platform with expensive ZPass (PowerVR), put only tagged objects in the ZPass
            adjustZPassFilterFlag(_gfxOccludeInfo);
#else
            if (_gfxOccludeInfo == GFX_OCCLUDE_INFO_DEFAULT) // should be tweak later on a per platform basis
            {
                if (_mat.getTexture() && _mat.getTexture()->getOpaqueRatio()>0.2f)
                    m_filterFlag |= GFX_ZLIST_PASS_ZPREPASS_FLAG;
            }
            else
            {
                adjustZPassFilterFlag(_gfxOccludeInfo);
            }
#endif
         }

        if (_mat.getRenderInReflection())
        {
            m_filterFlag |= GFX_ZLIST_PASS_REFLECTION_FLAG;
        }

        if (_mat.getFrontLightRender())
        {
            m_filterFlag |= GFX_ZLIST_PASS_FRONT_LIGHT_FLAG;
        }

        if (_mat.getBackLightRender())
        {
            m_filterFlag |= GFX_ZLIST_PASS_BACK_LIGHT_FLAG;
        }

        if (_mat.getRefractionRender())
        {
            m_filterFlag |= GFX_ZLIST_PASS_REFRACTION_FLAG;
        }
        if (_mat.getReflectionRender())
        {
            m_filterFlag |= GFX_ZLIST_PASS_3D_FLAG | GFX_ZLIST_PASS_REGULAR_FLAG | GFX_ZLIST_PASS_REFLECTION_PREPASS_FLAG;
        }
        if (_mat.getFluidRender())
        {
            m_filterFlag |= GFX_ZLIST_PASS_REGULAR_FLAG;
        }
        if (_mat.getFluidParticuleRender())
        {
            m_filterFlag = GFX_ZLIST_PASS_FLUID_FLAG;
        }
        if (_mat.getHiddenMaskRender())
        {
            m_filterFlag = GFX_ZLIST_PASS_REGULAR_FLAG | GFX_ZLIST_PASS_3D_FLAG | GFX_ZLIST_PASS_REFLECTION_PREPASS_FLAG | GFX_ZLIST_PASS_ZPREPASS_FLAG;
        }
		if (_mat.getMaskRender())
        {
            m_filterFlag |= GFX_ZLIST_PASS_MASK_FLAG;
        }
	}

#ifndef ITF_STRIPPED_DATA

    BEGIN_SERIALIZATION(GFXPrimitiveParamForced)
        BEGIN_CONDITION_BLOCK(ESerialize_ForcedValues)
            SERIALIZE_MEMBER("FORCED_FLAGS", m_flags);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    GFXPrimitiveParamForced::GFXPrimitiveParamForced()
    {
        reset();
    }

    void GFXPrimitiveParamForced::reset()
    {
        m_flags = 0;
    }
        
    void GFXPrimitiveParamForced::update( const SerializedObjectContent_Object* _pContent )
    {
        reset();

        if(_pContent)
        {
            if ( const SerializedObjectContent_Object* pContentGFXPrimitiveParam = _pContent->getObjectWithTag("GFXPrimitiveParam") )
            {
                m_flags |= BitTweak::BuildBitRange(pContentGFXPrimitiveParam->hasProperty("colorFactor") ? 1 : 0, ColorFactor, 1);
                m_flags |= BitTweak::BuildBitRange(pContentGFXPrimitiveParam->hasProperty("colorFog") ? 1 : 0, ColorFog, 1);
                m_flags |= BitTweak::BuildBitRange(pContentGFXPrimitiveParam->hasProperty("FrontLightBrightness") ? 1 : 0, FrontLightBrigthness, 1);
                m_flags |= BitTweak::BuildBitRange(pContentGFXPrimitiveParam->hasProperty("FrontLightContrast") ? 1 : 0, FrontLightContrast, 1);
                m_flags |= BitTweak::BuildBitRange(pContentGFXPrimitiveParam->hasProperty("BackLightBrightness") ? 1 : 0, BackLightBrigthness, 1);
                m_flags |= BitTweak::BuildBitRange(pContentGFXPrimitiveParam->hasProperty("BackLightContrast") ? 1 : 0, BackLightContrast, 1);
                m_flags |= BitTweak::BuildBitRange(pContentGFXPrimitiveParam->hasProperty("DynamicFogFactor") ? 1 : 0, FogFactor, 1);
                m_flags |= BitTweak::BuildBitRange(pContentGFXPrimitiveParam->hasProperty("useStaticFog") ? 1 : 0, UseStaticFog, 1);
                m_flags |= BitTweak::BuildBitRange(pContentGFXPrimitiveParam->hasProperty("gfxOccludeInfo") ? 1 : 0, GFXOccluderInfo, 1);
                m_flags |= BitTweak::BuildBitRange(pContentGFXPrimitiveParam->hasProperty("renderInReflections") ? 1 : 0, RenderInReflection, 1);
                m_flags |= BitTweak::BuildBitRange(pContentGFXPrimitiveParam->hasProperty("NormalMapLightBrigthness") ? 1 : 0, NormalMapLightBrigthness, 1);
                m_flags |= BitTweak::BuildBitRange(pContentGFXPrimitiveParam->hasProperty("NormalMapLightContrast") ? 1 : 0, NormalMapLightContrast, 1);
            }
        }
    }

    void GFXPrimitiveParamForced::setPrimitiveParam( GFXPrimitiveParam& _paramTo, const GFXPrimitiveParam& _paramFrom ) const
    {
        if ( !getColorFactorIsForced() )
            _paramTo.m_colorFactor = _paramFrom.m_colorFactor;

        if ( !getColorFogIsForced() )
            _paramTo.m_colorFog = _paramFrom.m_colorFog;

        if ( !getFrontLightBrigthnessIsForced() )
            _paramTo.m_frontLightBrigthness = _paramFrom.m_frontLightBrigthness;

        if ( !getFrontLightContrastIsForced() )
            _paramTo.m_frontLightContrast = _paramFrom.m_frontLightContrast;

        if ( !getBackLightBrigthnessIsForced() )
            _paramTo.m_backLightBrigthness = _paramFrom.m_backLightBrigthness;

        if ( !getBackLightContrastIsForced() )
            _paramTo.m_backLightContrast = _paramFrom.m_backLightContrast;

        if ( !getNormalMapLightBrigthnessIsForced() )
		{
			if ( _paramFrom.m_normalLightParam && _paramTo.m_normalLightParam )
				_paramTo.m_normalLightParam->m_lightBrigthness = _paramFrom.m_normalLightParam->m_lightBrigthness;
		}

        if ( !getNormalMapLightContrastIsForced() )
		{
			if ( _paramFrom.m_normalLightParam && _paramTo.m_normalLightParam )
				_paramTo.m_normalLightParam->m_lightContrast = _paramFrom.m_normalLightParam->m_lightContrast;
		}

/*        if ( !getNormalMapRotationIsForced() )
            _paramTo.m_normalMapRotation = _paramFrom.m_normalMapRotation;*/

        if ( !getFogFactorIsForced() )
            _paramTo.m_fogFactor = _paramFrom.m_fogFactor;

        if ( !getUseStaticFogIsForced() )
            _paramTo.m_useStaticFog = _paramFrom.m_useStaticFog;

        if ( !getGFXOccludeInfoIsForced() )
            _paramTo.m_gfxOccludeInfo = _paramFrom.m_gfxOccludeInfo;

        if ( !getRenderInReflectionIsForced() )
            _paramTo.m_renderInReflection = _paramFrom.m_renderInReflection;

	    _paramTo.m_outlineColor = _paramFrom.m_outlineColor;
        _paramTo.m_outlineWidth = _paramFrom.m_outlineWidth;
        _paramTo.m_outlineGlow = _paramFrom.m_outlineGlow;
        _paramTo.m_saturation = _paramFrom.m_saturation;
}

    void GFXPrimitiveParamForced::setPrimitiveParamInvert( GFXPrimitiveParam& _paramTo, const GFXPrimitiveParam& _paramFrom ) const
    {
        if ( getColorFactorIsForced() )
            _paramTo.m_colorFactor = _paramFrom.m_colorFactor;

        if ( getColorFogIsForced() )
            _paramTo.m_colorFog = _paramFrom.m_colorFog;

        if ( getFrontLightBrigthnessIsForced() )
            _paramTo.m_frontLightBrigthness = _paramFrom.m_frontLightBrigthness;

        if ( getFrontLightContrastIsForced() )
            _paramTo.m_frontLightContrast = _paramFrom.m_frontLightContrast;

        if ( getBackLightBrigthnessIsForced() )
            _paramTo.m_backLightBrigthness = _paramFrom.m_backLightBrigthness;

        if ( getBackLightContrastIsForced() )
            _paramTo.m_backLightContrast = _paramFrom.m_backLightContrast;

        if ( getNormalMapLightBrigthnessIsForced() )
		{
			if ( _paramFrom.m_normalLightParam && _paramTo.m_normalLightParam )
				_paramTo.m_normalLightParam->m_lightBrigthness = _paramFrom.m_normalLightParam->m_lightBrigthness;
		}

        if ( getNormalMapLightContrastIsForced() )
		{
			if ( _paramFrom.m_normalLightParam && _paramTo.m_normalLightParam )
	            _paramTo.m_normalLightParam->m_lightContrast = _paramFrom.m_normalLightParam->m_lightContrast;
		}

/*        if ( getNormalMapRotationIsForced() )
            _paramTo.m_normalMapRotation = _paramFrom.m_normalMapRotation;*/

		if ( getFogFactorIsForced() )
            _paramTo.m_fogFactor = _paramFrom.m_fogFactor;

        if ( getUseStaticFogIsForced() )
            _paramTo.m_useStaticFog = _paramFrom.m_useStaticFog;

        if ( getGFXOccludeInfoIsForced() )
            _paramTo.m_gfxOccludeInfo = _paramFrom.m_gfxOccludeInfo;

        if ( getRenderInReflectionIsForced() )
            _paramTo.m_renderInReflection = _paramFrom.m_renderInReflection;
    }

    bbool GFXPrimitiveParamForced::allPropertiesForced() const
    {
        for(u32 i = 0; i < Count; ++i)
        {
            if(!BitTweak::GetBitRange(m_flags, i, 1))
            {
                return bfalse;
            }
        }
        return btrue;
    }

#endif // ITF_STRIPPED_DATA

} // ITF


