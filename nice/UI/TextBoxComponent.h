#ifndef _ITF_TEXTBOXCOMPONENT_H_
#define _ITF_TEXTBOXCOMPONENT_H_

#define TextBoxComponent_CRC ITF_GET_STRINGID_CRC(TextBoxComponent,334090504)
#define TextBoxComponent_Template_CRC ITF_GET_STRINGID_CRC(TextBoxComponent_Template,1702546553)

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif // _LOCALISATION_ID_H_

#ifndef _ITF_FONTTEXTAREA_H_
#include "engine/display/FontTextArea.h"
#endif

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_SMARTLOCID_H_
#include "engine/localisation/SmartLocId.h"
#endif //_ITF_SMARTLOCID_H_

namespace ITF
{
    class FontTemplate;

    class TextBoxComponent : public UIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TextBoxComponent,UIComponent,TextBoxComponent_CRC);

    public:
        DECLARE_SERIALIZE()   

        TextBoxComponent();
        virtual ~TextBoxComponent();
        
        void                    setDepth(const i32 _depth) {m_textArea.setDepth(_depth);}
        void                    setBox(const Vec2d& _area = Vec2d::Zero, const TEXT_AREA_ANCHOR _anchor = AREA_ANCHOR_NONE);
        void                    setFont(const Color _fontColor, const f32 _fontSize = 0.0f, const FONT_ALIGNMENT _hAlignment = FONT_ALIGN_NONE, const FONT_ALIGNMENT _vAlignment = FONT_ALIGN_NONE);
        void                    setShadow(const Vec2d& _shadowOffset, const Color _shadowColor);
        void                    setStyle(u32 _styleIndex);
        void                    setOffset(const Vec2d& _offset) {m_offset = _offset;}
        void                    setSize(const f32 _fontSize);
        void                    setConstAdvanceChar(const char _constAdvanceChar_Size, const char _constAdvanceChar_Min = '\0', const char _constAdvanceChar_Max = '\0');
        
        void                    setColor(const Color _fontColor);
        const Color&            getColor() { return (m_color == 0) ? m_textArea.getColor(): m_color; }
        void                    setOverridenColor( const Color _color) { m_color = _color; }
        Color                   getInitColor() const;

        void                    setAlpha(const f32 _alpha) {m_textArea.setAlpha(_alpha); m_currentAlpha = m_dstAlpha = m_srcAlpha = _alpha; m_fadeTime = 0.f; }
        void                    setMainScale(const Vec2d& _mainScale) { m_mainScale = _mainScale; }
        void                    setMainAngle(const f32 _mainAngle) { m_mainAngle = _mainAngle; }
        void                    setMainOffset( const Vec2d& _mainOffset ) { m_mainOffset = _mainOffset; }

        void                    insertTextClear();
        void                    insertTextAtMarker(u32 _marker, const String8 &_textToInsert);
        const String8&          getText() const;
        void                    setText(const String8& _text);
        void                    setDefaultText( const String8& _text);
        void                    setLoc(const LocalisationId& _locId);
        void                    setActorScaleFactor(f32 _atorScaleFactor);

        FontTextArea&           getTextArea() {return m_textArea;}

        virtual void            onScaleChanged( const Vec2d& _oldScale, const Vec2d& _newScale );
        
        virtual     void        onResourceLoaded();

        virtual     bbool       needsUpdate         (       ) const             { return btrue; }
        virtual     bbool       needsDraw           (       ) const             { return btrue; }
        virtual     bbool       needsDraw2D         (       ) const             { return bfalse; }
        virtual void onStartDestroy(bbool _hotReload);
        virtual     void        onActorLoaded       ( Pickable::HotReloadType /*_hotReload*/ );
        virtual     void        onFinalizeLoad      ();
        virtual     void        onBecomeInactive();
        virtual     void        onBecomeActive();
        virtual     void        onEvent( Event * _event);
        f32                     getDepthOffset() const;
        void                    setDepthOffset(f32 _depth) { m_depthOffset = _depth; }


        virtual     void        Update              ( f32 _deltaTime );
        virtual     void        batchPrimitives( const ITF_VECTOR <class View*>& _views );

                    void        setIsVisible(bbool _isVisible);

                    AABB        getLocalAABB() const;
                    void        setLocalAABB( const AABB &_aabb );

                    void        setViewportVisibility( u32 _viewportVisibility );
#ifdef ITF_SUPPORT_EDITOR
        virtual void            onPostPropertyChange();
#endif //ITF_SUPPORT_EDITOR

    protected:
        ITF_INLINE const class TextBoxComponent_Template*  getTemplate() const;

        void                    updateText(f32 _deltaTime);
        void                    updateAlpha( f32 _dt );
        void                    updateAABB();


        FontTextArea            m_textArea;
        Color                   m_color;
        FONT_ALIGNMENT          m_hAlignment;
        FONT_ALIGNMENT          m_vAlignment;

        u32                     m_currentStyle;
        u32                     m_defaultStyle;
        Vec2d                   m_offset;
        Vec2d                   m_area;
        Vec2d                   m_scale;
        SmartLocId              m_text;
        Vec2d                   m_mainScale;
        f32                     m_mainAngle;
        Vec2d                   m_mainOffset;
        f32                     m_currentAlpha;
        f32                     m_dstAlpha;
        f32                     m_srcAlpha;
        f32                     m_fadeTime;
        f32                     m_fadeTotal;
        f32                     m_depthOffset;

        bbool                   m_scaleToMatchWithArea;

        f32                     m_maxWidth;
        f32                     m_autoScrollSpeed;
        f32                     m_autoScrollWaitTime;

#ifdef ITF_SUPPORT_EDITOR
    public:
        virtual void    drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags ) const ;
        void            drawBox(const f32 _border = 1.0f, const Color _color = COLOR_RED) const;
        virtual void    onEditorMove(bbool _modifyInitialPos = btrue);
#endif //ITF_SUPPORT_EDITOR
    };

    //---------------------------------------------------------------------------------------------------

    class TextBoxComponent_Template : public UIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TextBoxComponent_Template,UIComponent_Template,TextBoxComponent_Template_CRC);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(TextBoxComponent);

    public:

        TextBoxComponent_Template();
        ~TextBoxComponent_Template() {}

        const f32	            getDepthOffset() const { return m_depthOffset; }


        virtual bbool onTemplateLoaded( bbool _hotReload )
        {
            bbool ok = Super::onTemplateLoaded(_hotReload);

            for(ITF_VECTOR<FontTextArea::Style>::iterator it = m_styles.begin(); it!=m_styles.end(); ++it)
            {
                it->onLoaded(m_actorTemplate->getResourceContainer());
            }

            return ok;
        }

        virtual void onTemplateDelete( bbool _hotReload )
        {
            Super::onTemplateDelete(_hotReload);

            for(ITF_VECTOR<FontTextArea::Style>::iterator it = m_styles.begin(); it!=m_styles.end(); ++it)
            {
                it->onUnLoaded(m_actorTemplate->getResourceContainer());
            }
        }

        ITF_VECTOR<FontTextArea::Style>           m_styles;
        f32                                       m_depthOffset;

    };


    ITF_INLINE const TextBoxComponent_Template*  TextBoxComponent::getTemplate() const {return static_cast<const TextBoxComponent_Template*>(m_template);}
}
#endif // _ITF_TEXTBOXCOMPONENT_H_
