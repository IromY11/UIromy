#ifndef ITF_GFX_MATERIAL_TEXTURE_SET_H_
#define ITF_GFX_MATERIAL_TEXTURE_SET_H_

#include "engine/display/Texture.h"

#ifndef SERIALIZEROBJECTDEFINES_H
#include "core/serializer/ZSerializerObject_defines.h"
#endif // SERIALIZEROBJECTDEFINES_H

namespace ITF
{
    enum TextureSetSubID
    {
        TEXSET_ID_DIFFUSE = 0,
        TEXSET_ID_BACK_LIGHT,
        TEXSET_ID_NORMAL,
        TEXSET_ID_SEPARATE_ALPHA,
        TEXSET_ID_DIFFUSE_2,
        TEXSET_ID_BACK_LIGHT_2,
        TEXSET_ID_SPECULAR,
        TEXSET_ID_COLORMASK,
        TEXSET_ID_COUNT,
    };

    class GFXMaterialTexturePathSet
    {
        DECLARE_SERIALIZE()

    public:
        static GFXMaterialTexturePathSet defaultTextureSet;

        GFXMaterialTexturePathSet() {}
        ~GFXMaterialTexturePathSet() {}

        const Path & getTexturePath(TextureSetSubID idTex = TEXSET_ID_DIFFUSE) const { ITF_ASSERT(idTex < TEXSET_ID_COUNT); return m_texturePath[idTex]; }
        // This is used in case an animation is rendered in a texture
        const Path & getAnimPath() const { return m_animPath; }

        Path & getTexturePath(TextureSetSubID idTex = TEXSET_ID_DIFFUSE) { ITF_ASSERT(idTex < TEXSET_ID_COUNT); return m_texturePath[idTex]; }
        void setTexturePath(TextureSetSubID idTex, const Path & _path) { ITF_ASSERT(idTex < TEXSET_ID_COUNT); m_texturePath[idTex] = _path; }
        void setTexturePath(const Path & _path) { setTexturePath(TEXSET_ID_DIFFUSE, _path); }
        void setAnimPath(const Path & _path) { m_animPath = _path; }

    protected:

        Path m_texturePath[TEXSET_ID_COUNT];
        Path m_animPath;
    };

    class GFXMaterialTextureSet
    {
    public:
        static GFXMaterialTextureSet defaultTextureSet;

        GFXMaterialTextureSet();
        GFXMaterialTextureSet(const GFXMaterialTextureSet & _texSet);
        explicit GFXMaterialTextureSet(const GFXMaterialTexturePathSet & _pathSet);
        ~GFXMaterialTextureSet();

        GFXMaterialTextureSet & operator = (const GFXMaterialTextureSet & _texSet);

        void buildFromPathSet(const GFXMaterialTexturePathSet & _pathSet);
        void clean(); // revert back to an empty tex set

        bbool isCoherentWithPathSet(const GFXMaterialTexturePathSet & _pathSet) const;

        ResourceID getTextureResID(TextureSetSubID _idTex = TEXSET_ID_DIFFUSE) const 
        { 
            if(_idTex < TEXSET_ID_COUNT) 
                return m_textureResID[_idTex]; 
            return ResourceID::Invalid;
        }
        Texture * getTexture(TextureSetSubID _idTex = TEXSET_ID_DIFFUSE) const { return static_cast<Texture *>(getTextureResID(_idTex).getResource()); }
        void setTexture(TextureSetSubID _idTex, Texture *_texture);

        void setTextureResID(TextureSetSubID _idTex, const ResourceID & _resID);
        void setTextureResID(const ResourceID & _resID) { setTextureResID(TEXSET_ID_DIFFUSE, _resID); }

        // This is used in case an animation is rendered in a texture
        ResourceID getAnimResID() const { return m_animResID; }

        void fillResContainer(class ResourceContainer & _resContainer) const;
        void cleanResContainer(class ResourceContainer & _resContainer) const;

        bbool hasResourceLoaded() const;
        bbool hasValidResource() const;

        bool operator == (const GFXMaterialTextureSet & _rhTexSet) const
        {
            return ITF::equal(m_textureResID, m_textureResID+TEXSET_ID_COUNT, _rhTexSet.m_textureResID)
                && m_animResID == _rhTexSet.m_animResID;
        }
        bool operator != (const GFXMaterialTextureSet & _rhTexSet) const { return ! operator == (_rhTexSet); }

    protected:

        ResourceID m_textureResID[TEXSET_ID_COUNT];
        ResourceID m_animResID;
    };

} // namespace ITF

#endif // ITF_GFX_MATERIAL_TEXTURE_SET_H_
