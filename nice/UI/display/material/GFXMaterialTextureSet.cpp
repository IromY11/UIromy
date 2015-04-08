#include "precompiled_engine.h"

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef ITF_GFX_MATERIAL_TEXTURE_SET_H_
#include "GFXMaterialTextureSet.h"
#endif //ITF_GFX_MATERIAL_TEXTURE_SET_H_

namespace ITF
{

    //////////////////////////////////////////////////////////////////////////
    // Tex Path Set
    //////////////////////////////////////////////////////////////////////////

    BEGIN_SERIALIZATION(GFXMaterialTexturePathSet)
        SERIALIZE_MEMBER("diffuse", m_texturePath[TEXSET_ID_DIFFUSE]);
        SERIALIZE_MEMBER("back_light", m_texturePath[TEXSET_ID_BACK_LIGHT]);
        SERIALIZE_MEMBER("normal", m_texturePath[TEXSET_ID_NORMAL]);
        SERIALIZE_MEMBER("separateAlpha",  m_texturePath[TEXSET_ID_SEPARATE_ALPHA]);
        SERIALIZE_MEMBER("diffuse_2",  m_texturePath[TEXSET_ID_DIFFUSE_2]);
        SERIALIZE_MEMBER("back_light_2",  m_texturePath[TEXSET_ID_BACK_LIGHT_2]);
        SERIALIZE_MEMBER("specular", m_texturePath[TEXSET_ID_SPECULAR]);
        SERIALIZE_MEMBER("colorMask", m_texturePath[TEXSET_ID_COLORMASK]);
        SERIALIZE_MEMBER("anim_impostor", m_animPath);
    END_SERIALIZATION();


    //////////////////////////////////////////////////////////////////////////
    // Tex Set
    //////////////////////////////////////////////////////////////////////////

    GFXMaterialTextureSet GFXMaterialTextureSet::defaultTextureSet;

    GFXMaterialTextureSet::GFXMaterialTextureSet()
    {
    }

    GFXMaterialTextureSet::GFXMaterialTextureSet(const GFXMaterialTextureSet & _texSet)
    {
        operator = (_texSet);
    }

    GFXMaterialTextureSet::GFXMaterialTextureSet(const GFXMaterialTexturePathSet & _pathSet)
    {
        buildFromPathSet(_pathSet);
    }

    GFXMaterialTextureSet::~GFXMaterialTextureSet()
    {
        clean();
    }

    GFXMaterialTextureSet & GFXMaterialTextureSet::operator = (const GFXMaterialTextureSet & _texSet)
    {
        clean();
        for(ux i = 0, n = TEXSET_ID_COUNT; i < n; ++i)
        {
            m_textureResID[i] = _texSet.m_textureResID[i];
            if (m_textureResID[i].isValid())
                RESOURCE_MANAGER->newResourceRequest(m_textureResID[i]);
        }
        m_animResID = _texSet.m_animResID;
        // TODO -> anim stuff
        return *this;
    }

    void GFXMaterialTextureSet::clean()
    {
        // clean the texture resources
        for(ux i = 0, n = TEXSET_ID_COUNT; i < n; ++i)
        {
            if (m_textureResID[i].isValid())
            {
                RESOURCE_MANAGER->releaseResource(m_textureResID[i]);
                m_textureResID[i] = ResourceID();
            }
        }
    }

    void GFXMaterialTextureSet::buildFromPathSet(const GFXMaterialTexturePathSet & _pathSet)
    {
        clean();

        for(ux i = 0, n = TEXSET_ID_COUNT; i < n; ++i)
        {
            const Path & texPath = _pathSet.getTexturePath(TextureSetSubID(i));
            if (!texPath.isEmpty())
            {
                m_textureResID[i] = RESOURCE_MANAGER->newResourceFromFile(Resource::ResourceType_Texture, texPath);
            }
        }

        // TODO -> anim stuff
    }

        void GFXMaterialTextureSet::setTexture(TextureSetSubID _idTex, Texture *_texture)
		{
            if(_idTex < TEXSET_ID_COUNT) 
			{
				if ( m_textureResID[_idTex].isValid() )
					RESOURCE_MANAGER->releaseResource(m_textureResID[_idTex]);
                m_textureResID[_idTex].setResource(_texture);
				RESOURCE_MANAGER->newResourceRequest(m_textureResID[_idTex]);
			}
		}

    bbool GFXMaterialTextureSet::isCoherentWithPathSet(const GFXMaterialTexturePathSet & _pathSet) const
    {
        for(ux i = 0, n = TEXSET_ID_COUNT; i < n; ++i)
        {
            if (m_textureResID[i].isValid())
            {
                Texture * tex = getTexture(TextureSetSubID(i));
                if (!tex || tex->getPath() != _pathSet.getTexturePath(TextureSetSubID(i)))
                {
                    return bfalse;
                }
            }
            else if (!_pathSet.getTexturePath(TextureSetSubID(i)).isEmpty())
            {
                return bfalse;
            }
        }
        return btrue;
    }

    void GFXMaterialTextureSet::setTextureResID(TextureSetSubID _idTex, const ResourceID & _resID)
    {
        ITF_ASSERT(_idTex < TEXSET_ID_COUNT);
        if (m_textureResID[_idTex].isValid())
        {
            RESOURCE_MANAGER->releaseResource(m_textureResID[_idTex]);
        }

        m_textureResID[_idTex] = _resID;
        if (m_textureResID[_idTex].isValid())
        {
            RESOURCE_MANAGER->newResourceRequest(m_textureResID[_idTex]);
        }
    }

    void GFXMaterialTextureSet::fillResContainer(ResourceContainer & _resContainer) const
    {
        for(ux i = 0, n = TEXSET_ID_COUNT; i < n; ++i)
        {
            _resContainer.addResourceFromResId(m_textureResID[i]);
        }
        // anim TODO
        // m_animResID = _texSet.m_animResID;
    }

    void GFXMaterialTextureSet::cleanResContainer(class ResourceContainer & _resContainer) const
    {
        for(ux i = 0, n = TEXSET_ID_COUNT; i < n; ++i)
        {
            _resContainer.removeResource(m_textureResID[i]);
        }
        // anim TODO
    }

    bbool GFXMaterialTextureSet::hasResourceLoaded() const
    {
        for(ux i = 0, n = TEXSET_ID_COUNT; i < n; ++i)
        {
            ResourceID resID = m_textureResID[i];
            if (resID.isValid())
            {
                Texture * pTex = static_cast<class Texture *>(resID.getResource());
                ITF_ASSERT(pTex);
                if (pTex && !pTex->isPhysicallyLoaded())
                    return bfalse;
            }
        }
        // anim TODO
        return btrue;
    }

    bbool GFXMaterialTextureSet::hasValidResource() const
    {
        for(ux i = 0, n = TEXSET_ID_COUNT; i < n; ++i)
        {
            ResourceID resID = m_textureResID[i];
            if (resID.isValid())
            {
                return btrue;
            }
        }
        // anim TODO
        return btrue;
    }

} // namespace ITF
