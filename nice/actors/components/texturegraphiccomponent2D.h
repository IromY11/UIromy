#ifndef _ITF_TEXTUREGRAPHICCOMPONENT2D_H_
#define _ITF_TEXTUREGRAPHICCOMPONENT2D_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

namespace ITF
{
    class TextureGraphicComponent2D : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TextureGraphicComponent2D,GraphicComponent,1712599772);

    public:
        DECLARE_SERIALIZE()

        TextureGraphicComponent2D();
        ~TextureGraphicComponent2D();

        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return bfalse; }
        virtual bbool needsDraw2D() const { return btrue; }
        virtual void  Update(f32 _deltaTime);        
        virtual void  onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void  onActorClearComponents();
#ifdef ITF_SUPPORT_EDITOR
        virtual void  onEditorCreated( class Actor* _original );
        virtual void  onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class TextureGraphicComponent2D_Template*  getTemplate() const;

        void                clear();
        void                setScreenPourcentValues();
        const Path&         getTextureFile() const;
        void                updateAABB();

        ResourceID          m_textureID;

        // Percentage of the screen
        u32                 m_depthRank;
        f32                 m_screenPourcentX, m_screenPourcentY;
        f32                 m_width, m_height;
        Vec2d               m_quadSize;
        bbool               m_isWaitingForTex;

        enum QuadAlign
        {
            align_free, 
            align_centerX,
            align_centerY,
            align_centerXY
        };
        u32                 m_align; 
        Path                m_instanceFile;
        Texture*            getTexture();
    };

    class TextureGraphicComponent2D_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TextureGraphicComponent2D_Template,GraphicComponent_Template,2729231470);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(TextureGraphicComponent2D);

    public:

        TextureGraphicComponent2D_Template();
        ~TextureGraphicComponent2D_Template() {}

        virtual bbool  onTemplateLoaded( bbool _hotReload );
        virtual void  onTemplateDelete( bbool _hotReload );

        const ResourceID &  getTextureID() const { return m_textureID; }
        const Path&         getTextureFile() const { return m_textureFile; }
        u32                 getDepthRank() const { return m_depthRank; }
    private:

        ResourceID          m_textureID;
        Path                m_textureFile;
        u32                 m_depthRank;
    };



    const TextureGraphicComponent2D_Template*  TextureGraphicComponent2D::getTemplate() const {return static_cast<const TextureGraphicComponent2D_Template*>(m_template);}
    ITF_INLINE const Path& TextureGraphicComponent2D::getTextureFile() const
    {
        return getTemplate()->getTextureFile();
    }
}
#endif // _ITF_TEXTUREGRAPHICCOMPONENT2D_H_

