#ifndef ITF_GFX_MATERIAL_SHADER_H_
#define ITF_GFX_MATERIAL_SHADER_H_

#ifndef ITF_TEMPLATEDATABASE_TYPES_H
#include "engine/TemplateManager/TemplateDatabase_types.h"
#endif // ITF_TEMPLATEDATABASE_TYPES_H

#ifndef ITF_GFX_VECTOR4_H_
#include "engine/display/GFXVector4.h"
#endif // ITF_GFX_VECTOR4_H_

#ifndef _MTH_SIMD_MATRIX44_H_
#include "core/math/SIMD/SIMD_Matrix44.h"
#endif

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_COLORINTEGER_H_
#include "core/ColorInteger.h"
#endif //_ITF_COLORINTEGER_H_

#ifndef _ITF_BV_AABB_H_
#include "Core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef ITF_GFX_MATERIAL_PARAM_H_
#include "engine/display/material/GFXMaterialParam.h"
#endif //ITF_GFX_MATERIAL_PARAM_H_

namespace ITF
{
    enum GFX_MATERIAL_TYPE
    {
        GFX_MAT_DEFAULT = 0,
        GFX_MAT_REFRACTION,
        GFX_MAT_PATCH,
        GFX_MAT_FRIEZEANIM,
        GFX_MAT_GLOW,
        GFX_MAT_ALPHAFADE,
        GFX_MAT_FRIEZEOVERLAY,
        GFX_MAT_REFLECTION,
        GFX_MAT_FLUID_PARTICULE,
        GFX_MAT_FLUID,
        GFX_MAT_HIDDEN_MASK,
        GFX_MAT_DRAW_MASK,
        GFX_MAT_MASKED_TEX,
        GFX_MAT_INVALID = -1,

        ENUM_FORCE_SIZE_32(GFX_MATERIAL_TYPE)
    };

    enum GFX_MATERIAL_MODIFIER
    {
        GFX_MAT_MOD_DEFAULT = 0,
        GFX_MAT_MOD_FRIEZEANIM,
        GFX_MAT_MOD_STENCILTAG_MARK_HOLE,
        GFX_MAT_MOD_STENCILTAG_TEST_HOLE,
        GFX_MAT_MOD_STENCILTAG_FLUID,
        GFX_MAT_MOD_INVALID = -1,

        ENUM_FORCE_SIZE_32(GFX_MATERIAL_MODIFIER)
    };

    enum GFXMaterialLightingShader
    {
        GFX_MAT_SHADER_DEFAULT = 0,
        GFX_MAT_SHADER_LIGHT_LAYERED = 1,
        GFX_MAT_SHADER_LIGHT_LAYERED_DEFAULT_BACK_FLAG = 2,
        GFX_MAT_SHADER_LIGHT_LAYERED_DEFAULT_BACK = GFX_MAT_SHADER_LIGHT_LAYERED | GFX_MAT_SHADER_LIGHT_LAYERED_DEFAULT_BACK_FLAG,
        GFX_MAT_SHADER_LIGHT_LAYERED_DEFAULT_FRONT_FLAG = 4,
        GFX_MAT_SHADER_LIGHT_LAYERED_DEFAULT_FRONT = GFX_MAT_SHADER_LIGHT_LAYERED | GFX_MAT_SHADER_LIGHT_LAYERED_DEFAULT_FRONT_FLAG,
        GFX_MAT_SHADER_LIGHT_LAYERED_DEFAULT = GFX_MAT_SHADER_LIGHT_LAYERED | GFX_MAT_SHADER_LIGHT_LAYERED_DEFAULT_FRONT_FLAG | GFX_MAT_SHADER_LIGHT_LAYERED_DEFAULT_BACK_FLAG,
        GFX_MAT_SHADER_LIGHT_USE_TANGENT_LIGHT = 8,
        GFX_MAT_SHADER_LIGHT_LAYERED_TANGENTLIGHT = GFX_MAT_SHADER_LIGHT_LAYERED | GFX_MAT_SHADER_LIGHT_USE_TANGENT_LIGHT,
        GFX_MAT_SHADER_LIGHT_LAYERED_LAST = GFX_MAT_SHADER_LIGHT_LAYERED_TANGENTLIGHT,

        GFX_MAT_SHADER_INVALID = -1
    };

    class GFX_MaterialParams : public GFX_MaterialParamsT<4,2,5>
    {
    public:
        DECLARE_SERIALIZE()

        enum AlphaFadeType {AlphaFade_Distance=0, AlphaFade_ExpDistance, AlphaFade_ExpSquaredDistance, AlphaFade_Constant};

    };

    enum GFX_BLENDMODE 
    {
        GFX_BLEND_UNKNOWN = 0,
        GFX_BLEND_COPY = 1,
        GFX_BLEND_ALPHA = 2,
        GFX_BLEND_ALPHAPREMULT = 3,
        GFX_BLEND_ALPHADEST = 4,
        GFX_BLEND_ALPHADESTPREMULT = 5, 
        GFX_BLEND_ADD = 6,
        GFX_BLEND_ADDALPHA = 7,
        GFX_BLEND_SUBALPHA = 8,
        GFX_BLEND_SUB = 9,
        GFX_BLEND_MUL = 10,
        GFX_BLEND_ALPHAMUL = 11,
        GFX_BLEND_IALPHAMUL = 12,
        GFX_BLEND_IALPHA = 13,
        GFX_BLEND_IALPHAPREMULT = 14,
        GFX_BLEND_IALPHADEST = 15,
        GFX_BLEND_IALPHADESTPREMULT = 16,
        GFX_BLEND_MUL2X = 17,
        GFX_BLEND_ALPHATOCOLOR = 18,
        GFX_BLEND_IALPHATOCOLOR = 19,
        GFX_BLEND_SETTOCOLOR = 20,
        GFX_BLEND_SCREEN = 21,
        GFX_BLEND_ADDSMOOTH = 22,
        GFX_BLEND_NUMBER = 23,
        ENUM_FORCE_SIZE_32(GFX_BLENDMODE)
    };

#define SERIALIZE_GFX_BLENDMODE(name,val) \
    SERIALIZE_ENUM_BEGIN(name,val); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_COPY); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHA); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAPREMULT); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADEST); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADESTPREMULT); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_ADD); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_ADDALPHA); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_SUBALPHA); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_SUB); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_MUL); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAMUL); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAMUL); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHA); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAPREMULT); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADEST); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADESTPREMULT); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_MUL2X); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHATOCOLOR); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHATOCOLOR); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_SETTOCOLOR); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_SCREEN); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_ADDSMOOTH); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_NUMBER); \
    SERIALIZE_ENUM_END();

// Serialize only usefull blend modes.
#define SERIALIZE_GFX_BLENDMODE2(name,val) \
    SERIALIZE_ENUM_BEGIN(name,val); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_COPY); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHA); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_ADDALPHA); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_SUBALPHA); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_MUL); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_MUL2X); \
    SERIALIZE_ENUM_VAR(GFX_BLEND_ADDSMOOTH); \
    SERIALIZE_ENUM_END();

    struct GFX_UVANIM
    {
        enum 
        {
            UVANIM_F_TRANS =  0x00000001,
            UVANIM_F_ROTATE = 0x00000002,
            UVANIM_F_SCALE =  0x00000004,
            UVANIM_F_MANUAL = 0x00000008,
        };

        GFX_UVANIM()
            : m_lastAnimatedFrame(CURRENTFRAME)
            , m_uvFlagAnim(0)
            , m_speedRotate(0.f)
            , m_currentangle(0.f)
            , m_pos(Vec2d::Zero)
            , m_pivot(Vec2d::Zero)
            , m_speedTrans(Vec2d::Zero)
            , m_scale(Vec2d::One)
            , m_preTranslate(Vec2d::Zero)
            , m_preScale(Vec2d::One)
        {
        }

        bool operator == (const GFX_UVANIM & _rhUVAnim) const
        {
            return m_preTranslate == _rhUVAnim.m_preTranslate
                && m_preScale == _rhUVAnim.m_preScale
                && m_pivot == _rhUVAnim.m_pivot
                && m_uvFlagAnim == _rhUVAnim.m_uvFlagAnim
                && m_speedTrans == _rhUVAnim.m_speedTrans
                && m_speedRotate == _rhUVAnim.m_speedRotate
                && m_currentangle == _rhUVAnim.m_currentangle
                && m_scale == _rhUVAnim.m_scale;
        }

        bool operator != (const GFX_UVANIM & _rhUVAnim) const { return ! operator == (_rhUVAnim); }

        mutable u32     m_lastAnimatedFrame;
        mutable Vec2d   m_pos;
        Vec2d           m_pivot;
        Vec2d           m_preScale;
        Vec2d           m_preTranslate;
        u32             m_uvFlagAnim;
        Vec2d           m_speedTrans;
        f32             m_speedRotate;
        mutable f32     m_currentangle;
        Vec2d           m_scale;
    };

    class GFXMatAnimImpostor
    {
    public:
        DECLARE_SERIALIZE()

        GFXMatAnimImpostor();

        void onLoaded();

    private:
        // serialized members
        f32             m_AnimTranslationX;
        f32             m_AnimTranslationY;
        f32             m_AnimScaleX;
        f32             m_AnimScaleY;
        f32             m_AnimRot;
        i32             m_AnimIndex;
        i32             m_AnimTexSizeX;
        i32             m_AnimTexSizeY;
        ColorInteger    m_AnimBackGroundColor;
        AABB            m_animAABB;
        f32             m_animPhase;
    };


#define UNDEF_VALUE -1

    ///////////////////////////////////////////////////////////////////////
    // GFXMaterialShader_Template
    ///////////////////////////////////////////////////////////////////////
    class GFXMaterialShader_Template : public TemplateObj
    {
        DECLARE_OBJECT_CHILD_RTTI(GFXMaterialShader_Template, TemplateObj,3869849057);
        DECLARE_SERIALIZE()

    public:
        static GFXMaterialShader_Template defaultTpl;

        GFXMaterialShader_Template();

        virtual ~GFXMaterialShader_Template();

        u32 getFlags() const { return m_flags;}

        // This is used in case an animation is rendered in a texture then used in the 
        const GFXMatAnimImpostor & getAnimImpostor(ux numAnim = 0) const { ITF_ASSERT(numAnim < getNbAnimImpostor()); return m_animImpostor[numAnim]; }
        ux getNbAnimImpostor() const { return m_animImpostor.size(); }

        GFX_MATERIAL_TYPE  getMatType() const { return m_matType; }
        GFXMaterialLightingShader getMatLighting() const { return m_lightingShader; }
        const GFX_MaterialParams & getMatParams() const { return m_matParams; }
        GFX_MaterialParams & getMatParams() { return m_matParams; }
        GFX_BLENDMODE getBlendMode() const { return m_blendMode; }

        bbool getRegularRender() const { return m_renderRegular; }
        bbool getFrontLightRender() const { return m_renderFrontLight; }
        bbool getBackLightRender() const { return m_renderBackLight; }
        bbool getRenderInReflection() const { return (m_renderInReflection == UNDEF_VALUE) ? m_renderRegular : m_renderInReflection == 1; }

        bbool getAlphaTest() const { return m_useAlphaTest; }
        u32   getAlphaRef() const { return m_alphaRef; }
        bbool  useSeparateAlpha() const { return m_useSeparateAlpha; }
		bbool  useNormalMapLighting() const { return m_useNormalMapLighting; }
        bbool  useTextureBlend() const { return m_useTextureBlend; }

        virtual bbool onTemplateLoaded(bbool _hotReload);
        virtual void onTemplateDelete(bbool _hotReload);

    protected:

        // serialized members
        u32 m_flags;

        ITF_VECTOR<GFXMatAnimImpostor> m_animImpostor;

        GFX_BLENDMODE                                           m_blendMode;

        GFX_MaterialParams  m_matParams; // params for material (mapped on shader uniforms)

        GFX_MATERIAL_TYPE   m_matType;
        GFXMaterialLightingShader   m_lightingShader;
        // The vertex and fragment shader used are determined by mixing of m_matType and m_lightingShader

        bbool m_renderRegular;
        bbool m_renderFrontLight;
        bbool m_renderBackLight;
        int   m_renderInReflection;

        bbool m_useAlphaTest;
        u32   m_alphaRef;

        bbool m_useSeparateAlpha;
        bbool m_useTextureBlend;
		bbool m_useNormalMapLighting;
    };

} // namespace ITF

#endif // ITF_GFX_MATERIAL_SHADER_H_
