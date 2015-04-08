#ifndef ITF_GFX_PRIMITIVES_H_
#define ITF_GFX_PRIMITIVES_H_

#ifndef SERIALIZEROBJECTDEFINES_H
#include "core/serializer/ZSerializerObject_Defines.h"
#endif // SERIALIZEROBJECTDEFINES_H

#ifndef ITF_CORE_BITTWEAK_H_
#include "core/math/BitTweak.h"
#endif //ITF_CORE_BITTWEAK_H_
#include "core/Color.h"
#include "core/boundingvolume/AABB.h"

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

namespace ITF
{
    enum GFXOccludeInfo
    {
        GFX_OCCLUDE_INFO_DEFAULT,
        GFX_OCCLUDE_INFO_BIG_OPAQUE,            // Elements taking big opaque area on screen. (Good occluder)
        GFX_OCCLUDE_INFO_SMALL_OR_TRANSPARENT,  // Elements being small on screen OR being transparent (Bad occluder)
        GFX_OCCLUDE_INFO_ZPASS_ONLY,            // Occlude only, draw nothing
    };

    enum GFXMaskInfo
    {
        GFX_MASK_INFO_NONE,                     // Don't draw mask.
        GFX_MASK_INFO_HOLE,                     // Draw a hole in the mask layer.
        GFX_MASK_INFO_SILHOUETTE,               // Draw a silhouette in the mask layer.
    };
    #define SERIALIZE_ENUM_GFX_MASK_INFO(name, val) \
    SERIALIZE_ENUM_BEGIN(name, val); \
        SERIALIZE_ENUM_VAR(GFX_MASK_INFO_NONE); \
        SERIALIZE_ENUM_VAR(GFX_MASK_INFO_HOLE); \
        SERIALIZE_ENUM_VAR(GFX_MASK_INFO_SILHOUETTE); \
    SERIALIZE_ENUM_END()


    #define SERIALIZE_ENUM_GFX_OCCLUDE_INFO(name, val) \
    SERIALIZE_ENUM_BEGIN(name, val); \
        SERIALIZE_ENUM_VAR(GFX_OCCLUDE_INFO_DEFAULT); \
        SERIALIZE_ENUM_VAR(GFX_OCCLUDE_INFO_BIG_OPAQUE); \
        SERIALIZE_ENUM_VAR(GFX_OCCLUDE_INFO_SMALL_OR_TRANSPARENT); \
        SERIALIZE_ENUM_VAR(GFX_OCCLUDE_INFO_ZPASS_ONLY); \
    SERIALIZE_ENUM_END()

    struct NormalLightingParam
    {
        DECLARE_SERIALIZE()

        NormalLightingParam():
            m_lightBrigthness(0.0f),
            m_lightContrast(1.0f),
			m_rotation(0.0f,0.0f,0.0f),
			m_useNormalMapLighting(false),
			m_rimLightColor(1.0f,1.0f,1.0f,1.0f),
			m_rimPower(2.0f),
			m_specIntensity(0.5f),
			m_specSize(30.0f)
		{
		}

        const NormalLightingParam& operator = (const NormalLightingParam& src)
        {
            m_lightBrigthness		 = src.m_lightBrigthness;
            m_lightContrast			 = src.m_lightContrast;
            m_rotation				 = src.m_rotation;
            m_useNormalMapLighting   = src.m_useNormalMapLighting;
            m_rimLightColor          = src.m_rimLightColor;
            m_rimPower				 = src.m_rimPower;
            m_specIntensity			 = src.m_specIntensity;
            m_specSize				 = src.m_specSize;
            return *this;
        }

        bool operator == (const NormalLightingParam& _ref) const
        {
            if (m_lightBrigthness		!= _ref.m_lightBrigthness) return false;
            if (m_lightContrast			!= _ref.m_lightContrast) return false;
            if (m_rotation				!= _ref.m_rotation) return false;
            if (m_useNormalMapLighting	!= _ref.m_useNormalMapLighting) return false;
            if (m_rimLightColor			!= _ref.m_rimLightColor) return false;
            if (m_rimPower				!= _ref.m_rimPower) return false;
            if (m_specIntensity			!= _ref.m_specIntensity) return false;
            if (m_specSize				!= _ref.m_specSize) return false;
            return true;
        }

        void reset()
        {
            m_lightBrigthness  = 0.0f;
            m_lightContrast    = 1.0f;
            m_rotation.set(0.0f, 0.0f, 0.0f);
			m_useNormalMapLighting = false;
			m_rimLightColor.setRed(0.0f);
			m_rimLightColor.setGreen(0.0f);
			m_rimLightColor.setBlue(0.0f);
			m_rimLightColor.setAlpha(0.0f);
			m_rimPower = 2.0f;
			m_specIntensity = 1.0f;
			m_specSize = 30.0f;
        }

		f32         m_lightBrigthness;
        f32         m_lightContrast;
		Vec3d		m_rotation;
		bool		m_useNormalMapLighting;
		Color		m_rimLightColor;
		f32			m_rimPower;
		f32			m_specIntensity;
		f32			m_specSize;
    };


    class GFXPrimitiveParam
    {
        DECLARE_SERIALIZE()

    public:

        GFXPrimitiveParam():
            m_colorFactor(Color::white()),
            m_colorFog(Color::zero()),
            m_frontLightBrigthness(0.0f),
            m_frontLightContrast(1.0f),
            m_backLightBrigthness(0.0f),
            m_backLightContrast(1.0f),
            m_fogFactor(1.0f),
            m_useStaticFog(bfalse),
            m_useGlobalLighting(btrue),
            m_gfxOccludeInfo(GFX_OCCLUDE_INFO_DEFAULT),
            m_gfxMaskInfo(GFX_MASK_INFO_NONE),
            m_renderInReflection(btrue),
            m_BV(),
            m_outlineColor(0.0f, 1.0f, 1.0f, 1.0f),
			m_outlineWidth(0.0f),
			m_outlineGlow(0.0f),
			m_useZInject(false),
			m_normalLightParam(nullptr),
			m_viewportVisibility(0x0000ffff),
            m_saturation(0)
        {
			m_colorForMask[0] = COLOR_WHITE;    
			m_colorForMask[1] = COLOR_WHITE;    
			m_colorForMask[2] = COLOR_WHITE;    
		}

		~GFXPrimitiveParam()
		{
			if ( m_normalLightParam )
			{
				delete(m_normalLightParam);
				m_normalLightParam = nullptr;
			}
		}

        GFXPrimitiveParam(const GFXPrimitiveParam &src)
        {
			memcpy(this, &src, sizeof(GFXPrimitiveParam));
			if ( src.m_normalLightParam )
			{
				m_normalLightParam = newAlloc(mId_Frieze,NormalLightingParam);
				*m_normalLightParam = *src.m_normalLightParam;
			}
			else
				m_normalLightParam = nullptr;
		}

		const GFXPrimitiveParam& operator = (const GFXPrimitiveParam& src)
        {
            m_colorFactor				= src.m_colorFactor;
            m_colorFog					= src.m_colorFog;
            m_frontLightBrigthness		= src.m_frontLightBrigthness;
            m_frontLightContrast		= src.m_frontLightContrast;
            m_backLightBrigthness		= src.m_backLightBrigthness;
            m_backLightContrast			= src.m_backLightContrast;
            m_fogFactor					= src.m_fogFactor;
            m_useStaticFog				= src.m_useStaticFog;
            m_useGlobalLighting			= src.m_useGlobalLighting;
            m_gfxOccludeInfo			= src.m_gfxOccludeInfo;
            m_gfxMaskInfo			    = src.m_gfxMaskInfo;
            m_renderInReflection		= src.m_renderInReflection;
            m_BV						= src.m_BV;
            m_outlineColor				= src.m_outlineColor;
            m_outlineWidth				= src.m_outlineWidth;
            m_outlineGlow				= src.m_outlineGlow;
			m_useZInject                = src.m_useZInject;
            m_viewportVisibility		= src.m_viewportVisibility;
			m_colorForMask[0]			= src.m_colorForMask[0];    
			m_colorForMask[1]			= src.m_colorForMask[1];    
			m_colorForMask[2]			= src.m_colorForMask[2];    
            m_saturation                = src.m_saturation;
			if ( src.m_normalLightParam )
			{
				if (!m_normalLightParam)
					m_normalLightParam = newAlloc(mId_Frieze,NormalLightingParam);
				*m_normalLightParam = *src.m_normalLightParam;
			}
			else
			{
				if (m_normalLightParam)
				{
					delete(m_normalLightParam);
					m_normalLightParam = nullptr;
				}
			}
            return *this;
        }

        bool operator == (const GFXPrimitiveParam& _ref) const
        {
            if (m_colorFactor.getAsU32()    != _ref.m_colorFactor.getAsU32()) return false;
            if (m_colorFog.getAsU32()       != _ref.m_colorFog.getAsU32()) return false;
            if (m_frontLightBrigthness      != _ref.m_frontLightBrigthness) return false;
            if (m_frontLightContrast        != _ref.m_frontLightContrast) return false;
            if (m_backLightBrigthness       != _ref.m_backLightBrigthness) return false;
            if (m_backLightContrast         != _ref.m_backLightContrast) return false;
            if (m_fogFactor                 != _ref.m_fogFactor) return false;
            if (m_useStaticFog              != _ref.m_useStaticFog) return false;
            if (m_useGlobalLighting         != _ref.m_useGlobalLighting) return false;
            if (m_gfxOccludeInfo            != _ref.m_gfxOccludeInfo) return false;
            if (m_gfxMaskInfo               != _ref.m_gfxMaskInfo) return false;
            if (m_renderInReflection        != _ref.m_renderInReflection) return false;
            if (m_BV                        != _ref.m_BV) return false;
            if (m_outlineColor              != _ref.m_outlineColor) return false;
            if (m_outlineWidth              != _ref.m_outlineWidth) return false;
            if (m_outlineGlow				!= _ref.m_outlineGlow) return false;
			if (m_useZInject                != _ref.m_useZInject) return false;
            if (m_viewportVisibility		!= _ref.m_viewportVisibility) return false;
            if (m_colorForMask[0]           != _ref.m_colorForMask[0]) return false;
            if (m_colorForMask[1]           != _ref.m_colorForMask[1]) return false;
            if (m_colorForMask[2]           != _ref.m_colorForMask[2]) return false;
            if(m_saturation                 != _ref.m_saturation) return false;
			if ( (m_normalLightParam != nullptr) || (_ref.m_normalLightParam != nullptr) )
			{
				if ( (m_normalLightParam == nullptr) || (_ref.m_normalLightParam == nullptr) )
					return false;
				if ( !( *m_normalLightParam == *_ref.m_normalLightParam ) )
					return false;
			}
            return true;
        }

        void reset()
        {
            m_colorFactor				= Color::white();
            m_colorFog					= Color::zero();
            m_frontLightBrigthness		= 0.0f;
            m_frontLightContrast		= 1.0f;
            m_backLightBrigthness		= 0.0f;
            m_backLightContrast			= 1.0f;
            m_fogFactor					= 1.0f;
            m_useStaticFog				= bfalse;
			m_useGlobalLighting         = btrue;
            m_gfxOccludeInfo			= GFX_OCCLUDE_INFO_DEFAULT;
            m_gfxMaskInfo               = GFX_MASK_INFO_NONE;
            m_renderInReflection		= btrue;
            m_outlineColor.setAlpha(0.0f);
			m_outlineColor.setRed(1.0f);
			m_outlineColor.setGreen(1.0f);
			m_outlineColor.setBlue(1.0f);
            m_outlineWidth				= 0.0f;
			m_outlineGlow				= 0.0f;
			m_useZInject				= false;
            m_BV.invalidate();
			m_viewportVisibility		= 0xffff;
			m_colorForMask[0]           = COLOR_WHITE;    
			m_colorForMask[1]           = COLOR_WHITE;    
			m_colorForMask[2]           = COLOR_WHITE;
            m_saturation                = 0;
			if ( m_normalLightParam )
				m_normalLightParam->reset();
        }

		const Color& getColorForMask(ux _index) const {return m_colorForMask[_index];}
		void  setColorForMask(ux _index, const Color &_color) {m_colorForMask[_index] = _color;}

        Color       m_colorFactor;
        Color       m_colorFog;
        f32         m_frontLightBrigthness;
        f32         m_frontLightContrast;
        f32         m_backLightBrigthness;
        f32         m_backLightContrast;
        f32         m_fogFactor;
		Color       m_outlineColor;
		f32			m_outlineWidth;
		f32			m_outlineGlow;
		NormalLightingParam *m_normalLightParam;
		u32			m_viewportVisibility;
        f32         m_saturation;

        bbool       m_useStaticFog;
        bbool       m_renderInReflection;
		bbool		m_useGlobalLighting;
		bbool       m_useZInject;
        GFXOccludeInfo m_gfxOccludeInfo;
        GFXMaskInfo m_gfxMaskInfo;
		Color       m_colorForMask[3];

        AABB        m_BV;

        static const GFXPrimitiveParam & getDefault() { return ms_default; }
        static const GFXPrimitiveParam & getDefaultNoFog() { return ms_defaultNoFog; }

    private:

        static GFXPrimitiveParam ms_default;
        static GFXPrimitiveParam ms_defaultNoFog;

        explicit GFXPrimitiveParam(bbool _useStaticFog) :
            m_colorFactor(Color::white()),
            m_colorFog(Color::zero()),
            m_frontLightBrigthness(0.0f),
            m_frontLightContrast(1.0f),
            m_backLightBrigthness(0.0f),
            m_backLightContrast(1.0f),
            m_fogFactor(1.0f),
            m_useStaticFog(_useStaticFog),
			m_useGlobalLighting(btrue),
            m_gfxOccludeInfo(GFX_OCCLUDE_INFO_DEFAULT),
            m_renderInReflection(btrue),
            m_BV(),
            m_outlineColor(0.0f, 1.0f, 1.0f, 1.0f),
			m_outlineWidth(0.0f),
			m_outlineGlow(0.0f),
			m_useZInject(false),
			m_normalLightParam(nullptr),
			m_viewportVisibility(0x0000ffff),
            m_saturation(0),
			m_gfxMaskInfo(GFX_MASK_INFO_NONE)
        {
			m_colorForMask[0] = COLOR_WHITE;    
			m_colorForMask[1] = COLOR_WHITE;    
			m_colorForMask[2] = COLOR_WHITE;    
		}
    };

#ifndef ITF_STRIPPED_DATA
    class SerializedObjectContent_Object;

    class GFXPrimitiveParamForced
    {
        DECLARE_SERIALIZE()

    public:
                GFXPrimitiveParamForced();

        void    update( const SerializedObjectContent_Object* _pContent );
        void    setPrimitiveParam( GFXPrimitiveParam& _paramTo, const GFXPrimitiveParam& _paramFrom )const;
        void    setPrimitiveParamInvert( GFXPrimitiveParam& _paramTo, const GFXPrimitiveParam& _paramFrom )const;

        bbool   allPropertiesForced()const;

        bbool   getColorFactorIsForced()const           { return BitTweak::GetBitRange(m_flags, ColorFactor, 1) != 0; }
        bbool   getColorFogIsForced()const              { return BitTweak::GetBitRange(m_flags, ColorFog, 1) != 0; }
        bbool   getFrontLightBrigthnessIsForced()const  { return BitTweak::GetBitRange(m_flags, FrontLightBrigthness, 1) != 0; }
        bbool   getFrontLightContrastIsForced()const    { return BitTweak::GetBitRange(m_flags, FrontLightContrast, 1) != 0; }
        bbool   getBackLightBrigthnessIsForced()const   { return BitTweak::GetBitRange(m_flags, BackLightBrigthness, 1) != 0; }
        bbool   getBackLightContrastIsForced()const     { return BitTweak::GetBitRange(m_flags, BackLightContrast, 1) != 0; }
        bbool   getNormalMapLightBrigthnessIsForced()const { return BitTweak::GetBitRange(m_flags, NormalMapLightBrigthness, 1) != 0; }
        bbool   getNormalMapLightContrastIsForced()const   { return BitTweak::GetBitRange(m_flags, NormalMapLightContrast, 1) != 0; }
        bbool   getNormalMapRotationIsForced()const     { return BitTweak::GetBitRange(m_flags, NormalMapRotation, 1) != 0; }
        bbool   getFogFactorIsForced()const             { return BitTweak::GetBitRange(m_flags, FogFactor, 1) != 0; }
        bbool   getUseStaticFogIsForced()const          { return BitTweak::GetBitRange(m_flags, UseStaticFog, 1) != 0; }
        bbool   getGFXOccludeInfoIsForced()const        { return BitTweak::GetBitRange(m_flags, GFXOccluderInfo, 1) != 0;  }
        bbool   getRenderInReflectionIsForced()const    { return BitTweak::GetBitRange(m_flags, RenderInReflection, 1) != 0; }

    private:
        enum    ParamName
        {
                ColorFactor,
                ColorFog,
                FrontLightBrigthness,
                FrontLightContrast,
                BackLightBrigthness,
                BackLightContrast,
                FogFactor,
                UseStaticFog,
                GFXOccluderInfo,
                RenderInReflection,
                NormalMapLightBrigthness,
                NormalMapLightContrast,
				NormalMapRotation,
                Count
        };

        void    reset();

        u32     m_flags;
    };
#endif // ITF_STRIPPED_DATA

	// Common interface to all registered graphic object.
    class GFXObject
    {
    public:
		GFXObject()
		{
			m_viewportVisibility = 0x0000ffff;
		};

		~GFXObject()
		{
		};

		inline u32 getViewportVisibility() const
		{
			return m_viewportVisibility;
		}
		inline void setViewportVisibility(ux _flags)
		{
			m_viewportVisibility = _flags;
		}

		u32 getPassFilterFlag() const { return 0; }

		u32 m_viewportVisibility;
	};

    // Z ordered primitives lists
    class GFXPrimitive
    {
    public:
        enum Type
        {
            TYPE_REGULAR,
            TYPE_MASKRESOLVER,
        };

        GFXPrimitive(Type _primType = TYPE_REGULAR);

        // types used for downcasts
        Type getType() const { return m_primType; }

        void setPassFilterFlag(u32 _filterFlag) { m_filterFlag = _filterFlag; } // cf. GFXRenderPass.h for valid flag values
        // adjustZPassFilterFlag -> according to occluder info set the ZPass or not
        void adjustZPassFilterFlag(GFXOccludeInfo _gfxOccludeInfo);
        u32 getPassFilterFlag() const { return m_filterFlag; }

        const GFXPrimitiveParam* getCommonParam() const { return &m_gfxParam;}
              GFXPrimitiveParam* getCommonParam() { return &m_gfxParam;}
        void                     setCommonParam( const GFXPrimitiveParam& _param) { m_gfxParam = _param; adjustZPassFilterFlag(m_gfxParam.m_gfxOccludeInfo); }

        virtual void directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z = 0.0f, f32 _ZWorld = 0.0f, f32 _ZAbs = 0.0f) = 0;
        // note on _gfxDeviceAdapter parameter -> right now it's the GFXAdapter class, but it should be the gfx device

		virtual void prerendering(class GFXAdapter * _gfxDeviceAdapter, f32 _dt, u32 _zListViewID) {}

        void cleanPassFilterFlag() { m_filterFlag = 0; } // do not draw in any pass...
        void addPassFilterFlag(u32 _flagToAdd) { m_filterFlag = m_filterFlag | _flagToAdd; }
        void removePassFilterFlag(u32 _flagToRemove) { m_filterFlag = m_filterFlag & ~_flagToRemove; }

        void setPassFilterFlag(const class GFX_MATERIAL & _mat) { cleanPassFilterFlag(); addPassFilterFlag(_mat, m_gfxParam.m_gfxOccludeInfo); }
        void addPassFilterFlag(const class GFX_MATERIAL & _mat) { addPassFilterFlag(_mat, m_gfxParam.m_gfxOccludeInfo); }

        void setPassFilterFlag(const class GFX_MATERIAL & _mat, GFXOccludeInfo _gfxOccludeInfo) { cleanPassFilterFlag(); addPassFilterFlag(_mat, _gfxOccludeInfo); }
        void addPassFilterFlag(const class GFX_MATERIAL & _mat, GFXOccludeInfo _gfxOccludeInfo); // add the flag according to a material

		inline u32 getViewportVisibility() const
		{
			return m_gfxParam.m_viewportVisibility;
		}
		inline void setViewportVisibility(ux _flags)
		{
			m_gfxParam.m_viewportVisibility = _flags;
		}

    protected:
        Type m_primType;
        u32 m_filterFlag;

        GFXPrimitiveParam m_gfxParam;
    };

} // ITF

#endif // ITF_GFX_PRIMITIVES_H_
