#include "precompiled_engine.h"

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef ITF_GFX_MATERIAL_H_
#include "GFXMaterial.h"
#endif //ITF_GFX_MATERIAL_H_

#ifndef ITF_GFX_MATERIAL_SHADER_MANAGER_H_
#include "GFXMaterialShaderManager.h"
#endif //ITF_GFX_MATERIAL_MANAGER_H_

namespace ITF
{
    //////////////////////////////////////////////////////////////////////////
    // GFX_MATERIAL
    //////////////////////////////////////////////////////////////////////////


    GFX_MATERIAL::GFX_MATERIAL()
        :   m_matShaderTpl(&GFXMaterialShader_Template::defaultTpl)
        ,   m_matParams(m_matShaderTpl->getMatParams())
        ,   m_blendMode(m_matShaderTpl->getBlendMode())
        ,   m_materialType(m_matShaderTpl->getMatType())
        ,   m_materialModifier(GFX_MAT_MOD_DEFAULT)
        ,   m_useStencilTest(bfalse)
        ,   m_skipDepthTest(bfalse)
		,   m_isTwoSided(btrue)
        ,   m_useAlphaTest((u32)-1) // not initialised parameter.
        ,   m_useAlphaRef((u32)-1) // not initialised parameter.
        ,   m_useLinearFiltering(btrue)
    {
    }

    GFX_MATERIAL::GFX_MATERIAL(const GFX_MATERIAL & _mat)
        :   m_texSet(_mat.m_texSet)
        ,   m_matShaderTpl(_mat.m_matShaderTpl)
        ,   m_matParams(_mat.m_matParams)
        ,   m_blendMode(_mat.m_blendMode)
        ,   m_materialType(_mat.m_materialType)
        ,   m_materialModifier(_mat.m_materialModifier)
        ,   m_useAlphaTest(_mat.m_useAlphaTest)
        ,   m_useAlphaRef(_mat.m_useAlphaRef)
        ,   m_useStencilTest(_mat.m_useStencilTest)
        ,   m_skipDepthTest(_mat.m_skipDepthTest)
		,   m_isTwoSided(_mat.m_isTwoSided)
        ,   m_useLinearFiltering(btrue)
    {
        if (m_matShaderTpl != &GFXMaterialShader_Template::defaultTpl)
            GFXMaterialShaderManager::getInstance()->registerForHotReload(this);
        
        m_uvAnim[0] = _mat.m_uvAnim[0];
        m_uvAnim[1] = _mat.m_uvAnim[1];


		m_templateMatParams = _mat.m_templateMatParams;
    }

    GFX_MATERIAL::~GFX_MATERIAL()
    {
        if (m_matShaderTpl != &GFXMaterialShader_Template::defaultTpl)
            GFXMaterialShaderManager::getInstance()->unregisterForHotReload(this);
 
    }

    GFX_MATERIAL & GFX_MATERIAL::operator = (const GFX_MATERIAL & _mat)
    {
        m_texSet = _mat.m_texSet;
        if (m_matShaderTpl != &GFXMaterialShader_Template::defaultTpl)
            GFXMaterialShaderManager::getInstance()->unregisterForHotReload(this);
        m_matShaderTpl = _mat.m_matShaderTpl;
        if (m_matShaderTpl != &GFXMaterialShader_Template::defaultTpl)
            GFXMaterialShaderManager::getInstance()->registerForHotReload(this);
        m_uvAnim[0] = _mat.m_uvAnim[0];
        m_uvAnim[1] = _mat.m_uvAnim[1];
        m_matParams = _mat.m_matParams;
        m_blendMode = _mat.m_blendMode;
        m_materialType = _mat.m_materialType;
        m_materialModifier = _mat.m_materialModifier;
        m_templateMatParams = _mat.m_templateMatParams;
        m_useAlphaTest = _mat.m_useAlphaTest;
        m_useAlphaRef = _mat.m_useAlphaRef;
        m_useStencilTest = _mat.m_useStencilTest;
        m_skipDepthTest = _mat.m_skipDepthTest;
		m_isTwoSided = _mat.m_isTwoSided;
        return *this;
    }

    void GFX_MATERIAL::setShaderTemplate(const GFXMaterialShader_Template * matShaderTpl) // note that this is changing blend mode and mat params
    {
        if (matShaderTpl && matShaderTpl!=&GFXMaterialShader_Template::defaultTpl)
        {
            if (m_matShaderTpl == &GFXMaterialShader_Template::defaultTpl)
            {
                GFXMaterialShaderManager::getInstance()->registerForHotReload(this);
            }
            m_matShaderTpl = matShaderTpl;
        }
        else
        {
            if (m_matShaderTpl != &GFXMaterialShader_Template::defaultTpl)
            {
                GFXMaterialShaderManager::getInstance()->unregisterForHotReload(this);
            }
            m_matShaderTpl = &GFXMaterialShader_Template::defaultTpl;
        }
        resetFromShaderTemplate();
    }

    void GFX_MATERIAL::setTextureSet(const GFXMaterialTextureSet & _texSet)
    {
        m_texSet = _texSet;
    }

    void GFX_MATERIAL::resetFromShaderTemplate()
    {
        ITF_ASSERT(m_matShaderTpl);
        m_matParams = m_matShaderTpl->getMatParams();
        m_blendMode = m_matShaderTpl->getBlendMode();
        m_materialType = m_matShaderTpl->getMatType();
    }

    bool GFX_MATERIAL::operator == (const GFX_MATERIAL & _rhMat) const
    {
        return m_texSet == _rhMat.m_texSet
            && m_matShaderTpl == _rhMat.m_matShaderTpl
            && m_blendMode == _rhMat.m_blendMode
            && m_uvAnim[0] == _rhMat.m_uvAnim[0]
            && m_uvAnim[1] == _rhMat.m_uvAnim[1]
            && m_matParams == _rhMat.m_matParams
            && m_materialType == _rhMat.m_materialType
            && getStencilTest() == _rhMat.getStencilTest()
            && getSkipDepthTest() == _rhMat.getSkipDepthTest()
            && getAlphaTest() == _rhMat.getAlphaTest()
            && getAlphaRef() == _rhMat.getAlphaRef();
    }

    //////////////////////////////////////////////////////////////////////////
    // GFXMaterialSerializable
    //////////////////////////////////////////////////////////////////////////

    BEGIN_SERIALIZATION(GFXMaterialSerializable)
        SERIALIZE_OBJECT("textureSet", m_texPathSet);
        SERIALIZE_MEMBER("shaderPath", m_matShaderTplPath);
        SERIALIZE_OBJECT("materialParams", m_matParam);
        SERIALIZE_MEMBER("stencilTest", m_useStencilTest);
        SERIALIZE_MEMBER("skipDepthTest", m_skipDepthTest);
        SERIALIZE_MEMBER("isTwoSided", m_isTwoSided);
        SERIALIZE_MEMBER("alphaTest", m_useAlphaTest);
        SERIALIZE_MEMBER("alphaRef", m_useAlphaRef);
    END_SERIALIZATION();

    void GFXMaterialSerializable::onLoaded(ResourceContainer * _parentContainer)
    {
        getTextureSet().buildFromPathSet(m_texPathSet);
        setShaderTemplate(GFXMaterialShaderManager::getInstance()->acquire(m_matShaderTplPath));

        if (_parentContainer)
        {
            fillResContainer(*_parentContainer);
        }
        m_templateMatParams = m_matParam;
    }

    void GFXMaterialSerializable::onUnLoaded(class ResourceContainer * _parentContainer)
    {
        if (_parentContainer)
        {
            cleanResContainer(*_parentContainer);
        }
        getTextureSet().clean();
        const GFXMaterialShader_Template * shTpl = getShaderTemplate();
        setShaderTemplate(&GFXMaterialShader_Template::defaultTpl);
        GFXMaterialShaderManager::getInstance()->release(shTpl);
    }

    bbool GFXMaterialSerializable::hasPathCoherency() const // return false if path content is not coherent with resource ids
    {
        // shader tpl
        if (getShaderTemplate())
        {
            if (getShaderTemplate()->getFile() != m_matShaderTplPath)
            {
                return bfalse;
            }
        }
        else if (!m_matShaderTplPath.isEmpty())
        {
            return bfalse;
        }

        // texture set stuff
        return getTextureSet().isCoherentWithPathSet(m_texPathSet);
    }


    //////////////////////////////////////////////////////////////////////////
    // GFXMaterialSerializableParam
    //////////////////////////////////////////////////////////////////////////

    BEGIN_SERIALIZATION(GFXMaterialSerializableParam)
    // reflection
    SERIALIZE_MEMBER("Reflector_factor",   m_fparams[0]);
    // Hidden Mask
    SERIALIZE_MEMBER("HiddenMask_Z_Extrude", m_fparams[0]);

    END_SERIALIZATION()

} // namespace ITF
