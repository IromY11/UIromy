#ifndef ITF_GFX_MATERIAL_H_
#define ITF_GFX_MATERIAL_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef ITF_GFX_VECTOR4_H_
#include "engine/display/GFXVector4.h"
#endif // ITF_GFX_VECTOR4_H_

#ifndef ITF_GFX_MATERIAL_SHADER_H_
#include "GFXMaterialShader.h"
#endif // ITF_GFX_MATERIAL_SHADER_H_

#ifndef ITF_GFX_MATERIAL_TEXTURE_SET_H_
#include "GFXMaterialTextureSet.h"
#endif //ITF_GFX_MATERIAL_TEXTURE_SET_H_

namespace ITF
{
    //////////////////////////////////////////////////////////////////////////
    // GFXMaterialSerializableParam
    //////////////////////////////////////////////////////////////////////////
    // use this class when the material is built from serialization
    class GFXMaterialSerializableParam : public GFX_MaterialParamsT<1,1,1>
    {
    public:
        DECLARE_SERIALIZE()
    };

    //////////////////////////////////////////////////////////////////////////
    // GFX_MATERIAL
    //////////////////////////////////////////////////////////////////////////

    // material flags.
    class GFX_MATERIAL
    {
    public:
        GFX_MATERIAL();
        GFX_MATERIAL(const GFX_MATERIAL & _mat);
        ~GFX_MATERIAL();

        GFX_MATERIAL & operator = (const GFX_MATERIAL & _mat);

        // get
        const class GFXMaterialShader_Template * getShaderTemplate() const { return m_matShaderTpl; }
        u32 getFlags() const { return m_matShaderTpl->getFlags();}

        // This is used in case an animation is rendered in a texture
        const GFXMatAnimImpostor & getAnimImpostor() const { return m_matShaderTpl->getAnimImpostor(); }
        ux getNbAnimImpostor() const { return m_matShaderTpl->getNbAnimImpostor(); }

        const GFXMaterialTextureSet & getTextureSet() const { return m_texSet; }
        ResourceID          getTextureResID(TextureSetSubID _idTex = TEXSET_ID_DIFFUSE) const { return m_texSet.getTextureResID(_idTex); }
        class Texture *     getTexture(TextureSetSubID _idTex = TEXSET_ID_DIFFUSE) const { return m_texSet.getTexture(_idTex); }
        void setTexture(TextureSetSubID _idTex, Texture *_texture)
		{
			m_texSet.setTexture(_idTex, _texture);
		}
        GFX_MATERIAL_TYPE   getMatType() const { return m_materialType; }
        GFX_MATERIAL_MODIFIER getMatModifier() const { return m_materialModifier; }
        const GFX_UVANIM &        getUVAnim(u32 _idx) const { return m_uvAnim[_idx]; }
        GFXMaterialLightingShader getMatLighting() const { return m_matShaderTpl->getMatLighting(); }
        const GFX_MaterialParams & getMatParams() const { return m_matParams; }
        const GFXMaterialSerializableParam & getTemplateMatParams() const { return m_templateMatParams; }
        GFX_BLENDMODE getBlendMode() const { return m_blendMode; }
        bbool getAlphaTest() const { return m_useAlphaTest==(u32)-1 ? m_matShaderTpl->getAlphaTest() : m_useAlphaTest!=0; }
        u32   getAlphaRef() const { return m_useAlphaRef==(u32)-1 ? m_matShaderTpl->getAlphaRef() : m_useAlphaRef; }
        bbool getStencilTest() const { return m_useStencilTest; }
        bbool getSkipDepthTest() const { return m_skipDepthTest; }
        bbool getIsTwoSided() const { return m_isTwoSided; }
        bbool useSeparateAlpha() const { return m_matShaderTpl->useSeparateAlpha(); }
        bbool useNormalMapLighting() const { return m_matShaderTpl->useNormalMapLighting(); }
        bbool useTextureBlend() const { return m_matShaderTpl->useTextureBlend(); }
        bbool getRegularRender() const { return m_matShaderTpl->getRegularRender(); }
        bbool getRenderInReflection() const { return m_matShaderTpl->getRenderInReflection(); }
        bbool getFrontLightRender() const { return m_matShaderTpl->getFrontLightRender(); }
        bbool getBackLightRender() const { return m_matShaderTpl->getBackLightRender(); }
        bbool getRefractionRender() const { return m_materialType == GFX_MAT_REFRACTION; }
        bbool getReflectionRender() const { return m_materialType == GFX_MAT_REFLECTION; }
		bbool getMaskRender() const { return m_materialType == GFX_MAT_DRAW_MASK; }
        bbool getFluidRender() const { return (m_materialType == GFX_MAT_FLUID); }
        bbool getFluidParticuleRender() const { return m_materialType == GFX_MAT_FLUID_PARTICULE; }
        bbool getHiddenMaskRender() const { return m_materialType == GFX_MAT_HIDDEN_MASK; }

        // set and lvalue accessors 
        void setShaderTemplate(const GFXMaterialShader_Template * _matShaderTpl); // note that this is resetting blend mode and mat params from the template value
        void setTextureSet(const GFXMaterialTextureSet & _texSet);
        void setBlendMode(GFX_BLENDMODE _blendMode) { m_blendMode = _blendMode; }
        void setMatType(GFX_MATERIAL_TYPE _materialType) { m_materialType = _materialType; }
        void setMatModifier(GFX_MATERIAL_MODIFIER _materialMod) { m_materialModifier = _materialMod; }
        GFXMaterialTextureSet & getTextureSet() { return m_texSet; }
        GFX_MaterialParams & getMatParams() { return m_matParams; }
        GFXMaterialSerializableParam & getTemplateMatParams() { return m_templateMatParams; }
        GFX_UVANIM &        getUVAnim(u32 _idx) { return m_uvAnim[_idx]; }

        bool operator == (const GFX_MATERIAL & _rhMat) const;
        bool operator != (const GFX_MATERIAL & _rhMat) const { return ! operator == (_rhMat); }

        void resetFromShaderTemplate(); // same as this->setShaderTemplate(getTemplate());

        void fillResContainer(class ResourceContainer & _resContainer) const { m_texSet.fillResContainer(_resContainer); }
        void cleanResContainer(class ResourceContainer & _resContainer) const { m_texSet.cleanResContainer(_resContainer); }

        bbool hasResourceLoaded() const { return m_texSet.hasResourceLoaded(); }
        bbool hasValidResource() const { return m_texSet.hasValidResource(); }

        bbool useLinearFiltering () const { return m_useLinearFiltering; }
        void  setLinearFiltering (bbool _useLinearFiltering) { m_useLinearFiltering = _useLinearFiltering; }

    protected:
        GFXMaterialTextureSet m_texSet;
        const GFXMaterialShader_Template * m_matShaderTpl;

        GFX_UVANIM                      m_uvAnim[2];
        GFX_MaterialParams              m_matParams; // params for material (mapped on shader uniforms)
        GFXMaterialSerializableParam    m_templateMatParams; // params for material (mapped on shader uniforms)
        GFX_BLENDMODE                   m_blendMode;
        GFX_MATERIAL_TYPE               m_materialType;
        GFX_MATERIAL_MODIFIER           m_materialModifier;
        bbool                           m_useStencilTest;
        bbool                           m_skipDepthTest;
        bbool                           m_isTwoSided;
        u32                             m_useAlphaTest;
        u32                             m_useAlphaRef;
        bbool                           m_useLinearFiltering;
    };

    //////////////////////////////////////////////////////////////////////////
    // GFXMaterialSerializable
    //////////////////////////////////////////////////////////////////////////
    // use this class when the material is built from serialization
    // and just the base class GFX_MATERIAL when it's built at runtime

    class GFXMaterialSerializable : public GFX_MATERIAL
    {
    public:
        DECLARE_SERIALIZE()

        ~GFXMaterialSerializable() { ITF_ASSERT_MSG(m_matShaderTpl == &GFXMaterialShader_Template::defaultTpl, "material not unloaded, check if onUnloaded has been called"); }

        // Call when loaded/unloaded to build resource set.
        // Optionally, give a parent container to attach resources to it.
        void onLoaded(class ResourceContainer * _parentContainer = NULL); 
        void onUnLoaded(class ResourceContainer * _parentContainer = NULL);

        GFXMaterialTexturePathSet & getTexturePathSet() { return m_texPathSet; }
        const GFXMaterialTexturePathSet & getTexturePathSet() const { return m_texPathSet; }
        const GFXMaterialSerializableParam & getTemplateMaterialParams() const { return m_matParam; }

        // return false if path content is not coherent with resource ids
        // It's the case if onLoaded has not been called for instance
        bbool hasPathCoherency() const;

    private:
        GFXMaterialTexturePathSet m_texPathSet;
        Path m_matShaderTplPath;
        GFXMaterialSerializableParam m_matParam;
    };

} // namespace ITF

#endif // ITF_GFX_MATERIAL_H_
