
#ifndef _ITF_ATLASANIMATIONCOMPONENT_H_
#define _ITF_ATLASANIMATIONCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_

namespace ITF
{

class AtlasAnimationComponent : public GraphicComponent
{
    DECLARE_OBJECT_CHILD_RTTI(AtlasAnimationComponent,GraphicComponent,240098274);

public:
    DECLARE_SERIALIZE()

    AtlasAnimationComponent();
    ~AtlasAnimationComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return btrue; }
    virtual void  batchPrimitives( const ITF_VECTOR <class View*>& _views );
    virtual void  onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void  Update( f32 _deltaTime );

    ITF_INLINE const class AtlasAnimationComponent_Template* getTemplate() const;

private:
    void updateAABB();

    void updateAnimation( f32 _deltaTime );

    void updatePosition();

    AtlasDynamicObject m_atlasObject;
    Vec2d m_p1;
    Vec2d m_p2;
    Vec2d m_p3;
    Vec2d m_p4;
    u32 m_currentIndex;
    u32 m_numIndices;
    f32 m_timer;

};


//-------------------------------------------------------------------------------------
class AtlasAnimationComponent_Template : public GraphicComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(AtlasAnimationComponent_Template,GraphicComponent_Template,2427973027);
    DECLARE_SERIALIZE()
    DECLARE_ACTORCOMPONENT_TEMPLATE(AtlasAnimationComponent);

public:

    AtlasAnimationComponent_Template();
    ~AtlasAnimationComponent_Template();

    const GFX_MATERIAL & getMaterial()   const {return m_material;}
    f32 getWidth() const { return m_width; }
    f32 getHeight() const { return m_height; }
    f32 getPlayRate() const { return m_playRate; }

private:
    virtual bbool           onTemplateLoaded(bbool _hotReload);
    virtual void            onTemplateDelete(bbool _hotReload);

    GFXMaterialSerializable m_material;
    f32                     m_width;
    f32                     m_height;
    f32                     m_playRate;
};

const AtlasAnimationComponent_Template*  AtlasAnimationComponent::getTemplate() const {return static_cast<const AtlasAnimationComponent_Template*>(m_template);}

}

#endif // _ITF_ATLASANIMATIONCOMPONENT_H_

