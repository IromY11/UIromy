#include "precompiled_engine.h"

#ifndef ITF_GFX_MATERIAL_SHADER_MANAGER_H_
#include "engine/display/material/GFXMaterialShaderManager.h"
#endif //ITF_GFX_MATERIAL_SHADER_MANAGER_H_

#ifndef ITF_GFX_MATERIAL_SHADER_H_
#include "engine/display/material/GFXMaterialShader.h"
#endif //ITF_GFX_MATERIAL_SHADER_H_

namespace ITF
{
    BEGIN_SERIALIZATION(GFX_MaterialParams)
        SERIALIZE_MEMBER("matParams0F", m_fparams[0]);
        SERIALIZE_MEMBER("matParams1F", m_fparams[1]);
        SERIALIZE_MEMBER("matParams2F", m_fparams[2]);
        SERIALIZE_MEMBER("matParams3F", m_fparams[3]);
        SERIALIZE_MEMBER("matParams0I", m_iparams[0]);
        SERIALIZE_MEMBER("matParams0VX", m_vparams[0].x());
        SERIALIZE_MEMBER("matParams0VY", m_vparams[0].y());
        SERIALIZE_MEMBER("matParams0VZ", m_vparams[0].m_z);
        SERIALIZE_MEMBER("matParams0VW", m_vparams[0].m_w);

        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
        // alias
        // refraction
        SERIALIZE_MEMBER("Refract_texmul",      m_fparams[0]);
        SERIALIZE_MEMBER("Refract_normalmul",   m_fparams[1]);
        SERIALIZE_MEMBER("Refract_offsetx",     m_fparams[2]);
        SERIALIZE_MEMBER("Refract_offsety",     m_fparams[3]);
        SERIALIZE_MEMBER("Refract_downscale2x2", m_iparams[0]);

        // reflection
        SERIALIZE_MEMBER("Reflection_factor",   m_fparams[0]);

        // alpha fade
        SERIALIZE_MEMBER("alphafadeDistMin", m_fparams[0]);
        SERIALIZE_MEMBER("alphafadeDistMax", m_fparams[1]);
        SERIALIZE_MEMBER("alphafadeDensity", m_fparams[2]);
        SERIALIZE_MEMBER("alphafadetype", m_iparams[0]);

        END_CONDITION_BLOCK();

    END_SERIALIZATION()


    //////////////////////////////////////////////////////////////////////////
    // GFXMatAnimImpostor
    //////////////////////////////////////////////////////////////////////////
    BEGIN_SERIALIZATION(GFXMatAnimImpostor)
        SERIALIZE_MEMBER("animTranslationX", m_AnimTranslationX);
        SERIALIZE_MEMBER("animTranslationY", m_AnimTranslationY);
        SERIALIZE_MEMBER("animScaleX", m_AnimScaleX);
        SERIALIZE_MEMBER("animScaleY", m_AnimScaleY);
        SERIALIZE_MEMBER("animRot", m_AnimRot);
        SERIALIZE_MEMBER("animIndex", m_AnimIndex);
        SERIALIZE_MEMBER("animTexSizeX", m_AnimTexSizeX);
        SERIALIZE_MEMBER("animTexSizeY", m_AnimTexSizeY);
        SERIALIZE_MEMBER("animBackgroundColor", m_AnimBackGroundColor);
        SERIALIZE_OBJECT("animAABB", m_animAABB);
        SERIALIZE_MEMBER("animPhase", m_animPhase);
    END_SERIALIZATION()

    GFXMatAnimImpostor::GFXMatAnimImpostor()
        :   m_AnimTranslationX(0.f)
        ,   m_AnimTranslationY(0.f)
        ,   m_AnimScaleX(0.f)
        ,   m_AnimScaleY(0.f)
        ,   m_AnimRot(0.f)
        ,   m_AnimIndex(0)
        ,   m_AnimTexSizeX(0)
        ,   m_AnimTexSizeY(0)
        ,   m_AnimBackGroundColor(COLOR_ZERO)
        ,   m_animAABB(Vec2d::Zero, Vec2d::Zero)
        ,   m_animPhase(0)
    {
    }

    void GFXMatAnimImpostor::onLoaded()
    {
    }


    //////////////////////////////////////////////////////////////////////////
    // GFXMaterialShader_Template
    //////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(GFXMaterialShader_Template);

    BEGIN_SERIALIZATION_CHILD(GFXMaterialShader_Template)
        SERIALIZE_MEMBER("flags", m_flags);
        SERIALIZE_MEMBER("renderRegular", m_renderRegular);
        SERIALIZE_MEMBER("renderFrontLight", m_renderFrontLight);
        SERIALIZE_MEMBER("renderBackLight", m_renderBackLight);
        SERIALIZE_MEMBER("renderInReflection", m_renderInReflection);
        SERIALIZE_MEMBER("useAlphaTest", m_useAlphaTest);
        SERIALIZE_MEMBER("alphaRef", m_alphaRef);
        SERIALIZE_MEMBER("separateAlpha", m_useSeparateAlpha);
        SERIALIZE_MEMBER("normalMapLighting", m_useNormalMapLighting);
        SERIALIZE_MEMBER("textureBlend", m_useTextureBlend);
        SERIALIZE_CONTAINER_OBJECT("animInTex", m_animImpostor);
        SERIALIZE_ENUM_BEGIN("materialtype", m_matType);
            SERIALIZE_ENUM_VAR(GFX_MAT_DEFAULT);
            SERIALIZE_ENUM_VAR(GFX_MAT_REFRACTION);
            SERIALIZE_ENUM_VAR(GFX_MAT_PATCH);
            SERIALIZE_ENUM_VAR(GFX_MAT_FRIEZEANIM);
            SERIALIZE_ENUM_VAR(GFX_MAT_GLOW);
            SERIALIZE_ENUM_VAR(GFX_MAT_ALPHAFADE);
            SERIALIZE_ENUM_VAR(GFX_MAT_FRIEZEOVERLAY);
            SERIALIZE_ENUM_VAR(GFX_MAT_REFLECTION);
			SERIALIZE_ENUM_VAR(GFX_MAT_DRAW_MASK);
			SERIALIZE_ENUM_VAR(GFX_MAT_MASKED_TEX);
        SERIALIZE_ENUM_END();
        SERIALIZE_ENUM_BEGIN("lightingType", m_lightingShader);
            SERIALIZE_ENUM_VAR(GFX_MAT_SHADER_DEFAULT);
            SERIALIZE_ENUM_VAR(GFX_MAT_SHADER_LIGHT_LAYERED);
        SERIALIZE_ENUM_END();
        SERIALIZE_OBJECT("matParams", m_matParams);
        SERIALIZE_ENUM_BEGIN("blendmode", m_blendMode);
            SERIALIZE_ENUM_VAR(GFX_BLEND_UNKNOWN);
            SERIALIZE_ENUM_VAR(GFX_BLEND_COPY);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHA);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAPREMULT);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADEST);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADESTPREMULT);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ADD);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ADDALPHA);
            SERIALIZE_ENUM_VAR(GFX_BLEND_SUBALPHA);
            SERIALIZE_ENUM_VAR(GFX_BLEND_SUB);
            SERIALIZE_ENUM_VAR(GFX_BLEND_MUL);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAMUL);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAMUL);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHA);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAPREMULT);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADEST);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADESTPREMULT);
            SERIALIZE_ENUM_VAR(GFX_BLEND_MUL2X);
            SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHATOCOLOR);
            SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHATOCOLOR);
            SERIALIZE_ENUM_VAR(GFX_BLEND_SCREEN);
        SERIALIZE_ENUM_END();
    END_SERIALIZATION()

    GFXMaterialShader_Template::GFXMaterialShader_Template()
        :   Super()
        ,   m_flags(0)
        ,   m_blendMode(GFX_BLEND_ALPHA)
        ,   m_matParams()
        ,   m_matType(GFX_MAT_DEFAULT)
        ,   m_lightingShader(GFX_MAT_SHADER_DEFAULT)
        ,   m_renderRegular(btrue)
        ,   m_renderFrontLight(bfalse)
        ,   m_renderBackLight(bfalse)
        ,   m_renderInReflection(UNDEF_VALUE)
        ,   m_useAlphaTest(bfalse)
        ,   m_alphaRef(128)
        ,   m_useSeparateAlpha(bfalse)
        ,   m_useNormalMapLighting(bfalse)
        ,   m_useTextureBlend(bfalse)
    {
    }

    GFXMaterialShader_Template::~GFXMaterialShader_Template()
    {
    }

    bbool GFXMaterialShader_Template::onTemplateLoaded(bbool _hotReload)
    {
        return Super::onTemplateLoaded(_hotReload);
    }

    void GFXMaterialShader_Template::onTemplateDelete(bbool _hotReload)
    {
        Super::onTemplateDelete(_hotReload);
    }

    GFXMaterialShader_Template GFXMaterialShader_Template::defaultTpl; // default constructor is OK

} // namespace ITF
