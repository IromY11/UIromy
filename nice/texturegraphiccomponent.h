
#ifndef _ITF_TEXTUREGRAPHICCOMPONENT_H_
#define _ITF_TEXTUREGRAPHICCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef ITF_ENGINE_QUAD2DPRIM_H_
#include "engine/display/Primitives/Quad2DPrim.h"
#endif //ITF_ENGINE_QUAD2DPRIM_H_

namespace ITF
{

class TextureGraphicComponent : public GraphicComponent
{
    DECLARE_OBJECT_CHILD_RTTI(TextureGraphicComponent,GraphicComponent,2068359598);

public:
    DECLARE_SERIALIZE()

    enum TEXTURE_ANCHOR
    {
        TEXTURE_ANCHOR_NONE = -1,
        TEXTURE_ANCHOR_TOP_LEFT = 0,
        TEXTURE_ANCHOR_MIDDLE_CENTER = 1,
        TEXTURE_ANCHOR_MIDDLE_LEFT = 2,
        TEXTURE_ANCHOR_MIDDLE_RIGHT = 3,
        TEXTURE_ANCHOR_TOP_CENTER = 4,
        TEXTURE_ANCHOR_TOP_RIGHT = 5,
        TEXTURE_ANCHOR_BOTTOM_CENTER = 6,
        TEXTURE_ANCHOR_BOTTOM_LEFT = 7,
        TEXTURE_ANCHOR_BOTTOM_RIGHT = 8,
        ENUM_FORCE_SIZE_32(TEXTURE_ANCHOR)
    };

    TextureGraphicComponent();
    ~TextureGraphicComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const;
    virtual bbool needsDraw2D() const;
	virtual bbool needsDraw2DNoScreenRatio() const;

    virtual void  onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void  onActorClearComponents();
    virtual void  onResourceLoaded();
    virtual void  Update( f32 _deltaTime );
    virtual void  batchPrimitives( const ITF_VECTOR <class View*>& _views );
    virtual void  batchPrimitives2D( const ITF_VECTOR <class View*>& _views );
#ifdef ITF_SUPPORT_EDITOR
    virtual void  onEditorMove(bbool _modifyInitialPos = btrue);
    virtual void  onEditorCreated( class Actor* _original );
#endif // ITF_SUPPORT_EDITOR
    void setTextureFile(const Path& _textureFile);
	void setOverideMaterialTexture(Texture* _texture);

	void getTextureFile(ITF::Path& _path) ;
    void setSwapMaterial(const GFX_MATERIAL* _material);
    bbool setNewSpriteIndex(u32 _idx);
    void resetQuad();
    void setIsDrawEnabled(bbool _isEnabled) {m_isDrawEnabled = _isEnabled;}
    void setOffset(const Vec2d& _offset) {m_offset = _offset;}
    void setScale(const Vec2d& _scale) {m_scale = _scale;}
    Vec2d getSize() const;

    ITF_INLINE const Color&         getDefaultColor() const;

    ITF_INLINE const class TextureGraphicComponent_Template* getTemplate() const;

    ITF_INLINE const GFX_MATERIAL * getMaterial() { return &m_material; }

    void                    setMaterial(const GFX_MATERIAL& _material);
private:

    Vec2d                   computeMiddleCenterOffset(const Vec2d& _size) const;
    void                    updateAABB();

    GFXMaterialSerializable m_materialSerializable;
    GFX_MATERIAL            m_material;
    u32                     m_spriteIndex;
    u32                     m_newSpriteIndex;
    Quad3DPrim              m_quad;
    Quad2DPrim              m_quad2DPrim;
    Vec3d                   m_curAngle;
    TEXTURE_ANCHOR          m_anchor;
    bbool                   m_isDrawEnabled;
    Vec2d                   m_offset;
    Vec2d                   m_scale;
};

//-------------------------------------------------------------------------------------
class TextureGraphicComponent_Template : public GraphicComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(TextureGraphicComponent_Template,GraphicComponent_Template,2628666149);
    DECLARE_SERIALIZE()
    DECLARE_ACTORCOMPONENT_TEMPLATE(TextureGraphicComponent);

public:

    TextureGraphicComponent_Template();
    ~TextureGraphicComponent_Template() {}

    virtual bbool  onTemplateLoaded( bbool _hotReload );
    virtual void  onTemplateDelete( bbool _hotReload );

    ITF_INLINE const GFX_MATERIAL & getMaterial() const {return m_material;}
    ITF_INLINE const Color&  getDefaultColor()  const {return m_defaultColor;}
    ITF_INLINE const Angle&  getAngleX()        const {return m_angleX;}
    ITF_INLINE const Angle&  getAngleY()        const {return m_angleY;}
    ITF_INLINE const Angle&  getAngleZ()        const {return m_angleZ;}
    ITF_INLINE f32           getSpeedRotX()     const {return m_speedRotX;}
    ITF_INLINE f32           getSpeedRotY()     const {return m_speedRotY;}
    ITF_INLINE f32           getSpeedRotZ()     const {return m_speedRotZ;}
    ITF_INLINE const Vec2d   getSize()          const {return m_size;}
    ITF_INLINE f32           getZOffset()       const {return m_zOffset;}
    ITF_INLINE bbool        getDraw2D()         const {return m_draw2D;}
	ITF_INLINE bbool        getDraw2DNoScreenRatio()         const {return m_draw2DNoScreenRatio;}
	
private:
    GFXMaterialSerializable m_material;
    Color                   m_defaultColor;
    Angle                   m_angleX;
    Angle                   m_angleY;
    Angle                   m_angleZ;
    f32                     m_speedRotX;
    f32                     m_speedRotY;
    f32                     m_speedRotZ;
    Vec2d                   m_size;
    f32                     m_zOffset;

    bbool                   m_draw2D;
	bbool					m_draw2DNoScreenRatio;
};


const TextureGraphicComponent_Template*  TextureGraphicComponent::getTemplate() const {return static_cast<const TextureGraphicComponent_Template*>(m_template);}
const Color&         TextureGraphicComponent::getDefaultColor() const { return getTemplate()->getDefaultColor(); }


}

#endif // _ITF_GRAPHICCOMPONENT_H_

