#include "precompiled_engine.h"


#ifndef _ITF_SPRITE_H_
#include "engine/display/Sprite.h"
#endif //_ITF_SPRITE_H_



namespace ITF
{


bbool SpriteTemplate::processRefs()
{
    // had method ....
    if (refsOk)
        return btrue;

    ITF_MAP<String8, SpriteAtomicObject*> objMap;
    for (ITF_VECTOR<SpriteAtomicObject>::iterator  iter = m_objectList.begin();
         iter != m_objectList.end(); ++iter)
    {
        objMap[(*iter).m_UID] = &(*iter);
    }

    for (ITF_VECTOR<SpriteAtomicObject>::iterator  iter = m_objectList.begin();
         iter != m_objectList.end(); ++iter)
    {
        (*iter).m_refs.clear();
        (*iter).m_refs.reserve((*iter).m_refsUID.size());
        for (ITF_VECTOR<String8>::iterator  iterIn = (*iter).m_refsUID.begin();
                 iterIn != (*iter).m_refsUID.end(); ++iterIn)
        {
            (*iter).m_refs.push_back(objMap[*iterIn]);
        }
    }
    refsOk = btrue;
    return btrue;
}


SpriteTemplate* SpriteWorld::getTemplateByUID(String8 _UID)
{
    return m_templateMap[_UID];
/*    for ( ITF_VECTOR<SpriteTemplate*>::iterator iter = m_templateList.begin(); iter != m_templateList.end(); ++iter)
    {
        if ((*iter)->m_UID == _UID)
            return *iter;
    }
    return NULL;
*/
}

}

