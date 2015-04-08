#ifndef _ITF_SPRITE_H_
#define _ITF_SPRITE_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

namespace ITF
{

class SpriteAtomicObject
{


public:
    enum AtomicType
    {
        ATOMIC_BONE = 0,
        ATOMIC_LINK,
        ATOMIC_PATCH,
        ENUM_FORCE_SIZE_32(0)
    };

    AtomicType  m_type;
    String8      m_UID;
    String8      m_templateUID;

    Vec2d       m_position;
    Vec2d       m_positionUV;
    Vec2d       m_normale;
    Vec2d       m_normaleUV;
    
    Vec2d       m_localPosition;
    Vec2d       m_localNormale;

    f32         m_IKLenght;
    ITF_VECTOR<SpriteAtomicObject *>    m_refs;
    ITF_VECTOR<String8>                  m_refsUID;
};


class SpriteTemplate
{
public:
    String8                          m_UID;
    Texture                         *m_texture;
    ITF_VECTOR<SpriteAtomicObject>  m_objectList;
    bbool                           refsOk;

    bbool                   processRefs();
};

class SpriteWorld
{
public:
    ITF_MAP<String8, SpriteTemplate*>  m_templateMap;

    SpriteTemplate* getTemplateByUID(String8 _UID);
};

}



#endif //_ITF_SPRITE_H_