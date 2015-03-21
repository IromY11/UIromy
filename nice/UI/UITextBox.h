#ifndef _ITF_UITEXTBOX_H_
#define _ITF_UITEXTBOX_H_


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

    #define UITextBox_CRC ITF_GET_STRINGID_CRC(UITextBox, 3507273453)

    class UITextBox : public UIComponent
    {
    
        DECLARE_OBJECT_CHILD_RTTI(UITextBox,UIComponent,3507273453);

    public:
        DECLARE_SERIALIZE()   

        UITextBox();
        virtual ~UITextBox();

        enum ScaleMatchType
        {
            NoScaleMatch,
            ScaleMatchOneLine,
            ScaleMatchMultiLine,
        };
        
        void                    setDepth(const i32 _depth) {m_textArea.setDepth(_depth);}
        void                    setBox(const Vec2d& _area = Vec2d::Zero, const TEXT_AREA_ANCHOR _anchor = AREA_ANCHOR_NONE);
        void                    setFont(const Color _fontColor, const f32 _fontSize = 0.0f, const FONT_ALIGNMENT _hAlignment = FONT_ALIGN_NONE, const FONT_ALIGNMENT _vAlignment = FONT_ALIGN_NONE);
        void                    setShadow(const Vec2d& _shadowOffset, const Color _shadowColor);
        void                    setStyle(u32 _styleIndex);
        void                    setConstAdvanceChar(const char _constAdvanceChar_Size, const char _constAdvanceChar_Min = '\0', const char _constAdvanceChar_Max = '\0');
        void                    setOffset(const Vec2d& _offset) { m_offset = _offset; }
        void                    setAlpha(f32 _alpha);
        void                    setColor(const Color _fontColor);
        void                    setMainScale(const f32 _scale) {m_mainScale = Vec2d(_scale, _scale);}
        void                    setMainScale(const Vec2d& _scale) {m_mainScale = _scale;}
        void                    setMainOffset( const Vec2d& _offset ) { m_mainOffset = _offset; }
        void                    setMaxWidth(const f32 _width);
        void                    setEffect(StringID _name, bool _smooth = bfalse);
        Color                   getInitColor() const;
        String8                 getInitText() const;
        f32                     getInitSize() const;
        u32                     getInitStyle() const;

        u32                     getAutoScrollLoopGap() const { return m_autoScrollLoopGap; }

        void                    insertTextClear();
        void                    insertTextAtMarker(u32 _marker, const String8 &_textToInsert);
        const String8&          getText() const;
        void                    setText(const String8 & _text);
        void                    setLoc(const LocalisationId& _locId);
        void                    setActorScaleFactor(f32 _factor);
        void                    setViewportVisibility(u32 _viewportVisibility);

        FontTextArea&           getTextArea() {return m_textArea;}
        Vec2d                   getOffset() const { return m_offset; }

        virtual     void        onActorLoaded       ( Pickable::HotReloadType /*_hotReload*/ );
        virtual     void        onFinalizeLoad      ();
        virtual void onStartDestroy(bbool _hotReload);
        virtual     void        onEvent(Event * _event);
        virtual     void        Update              ( f32 _deltaTime );
        virtual     bbool       needsDraw2D         () const             { return btrue; }
        virtual     void        batchPrimitives2D   ( const ITF_VECTOR <class View*>& _views );

        void                    setIsVisible(bbool _isVisible);

        void                    setSize(const f32 _fontSize);
        f32                     getSize() const;

        struct ActorIcon
        {
            ActorIcon() : m_usedCount(0), m_iconFontSize(0), m_iconOffset(Vec2d::Zero) {}
            StringID m_iconName;
            ActorRef m_iconActor;
            f32      m_iconFontSize;
            u32      m_usedCount;
            Vec2d    m_iconOffset;
        };

        void                    setActorIcon(StringID _iconName);
        ActorIcon*              useActorIcon(StringID _iconName, bbool _use);
        const ActorIcon*        getActorIcon(StringID _iconName) const;
        void                    clearActorIcon();

        virtual     void        onBecomeActive();
        virtual     void        onBecomeInactive();

#ifdef ITF_SUPPORT_EDITOR
        virtual     void        onPostPropertyChange();
#endif //ITF_SUPPORT_EDITOR

    protected:
        ITF_INLINE const class UITextBox_Template*  getTemplate() const;

        void                    updateAABB();
        void                    updateAlpha( f32 _dt );

		FontTextArea            m_textArea;
        u32                     m_currentStyle;


        u32                     m_defaultStyle;
        Vec2d                   m_scale;
        Vec2d                   m_offset;
        Vec2d                   m_area;
        f32                     m_depth;
        SmartLocId              m_text;
        Color                   m_color;
        FONT_ALIGNMENT          m_hAlignment;
        FONT_ALIGNMENT          m_vAlignment;
        TEXT_AREA_ANCHOR        m_anchor;
        Vec2d                   m_mainScale;
        Vec2d                   m_mainOffset;
        f32                     m_mainAlpha;
        f32                     m_showAlpha;
        ScaleMatchType          m_scaleToMatchWithArea;
        f32                     m_maxWidth;
        f32                     m_autoScrollSpeed;
        f32                     m_autoScrollWaitTime;
        bbool                   m_autoScrollLoop;
        u32                     m_autoScrollLoopGap;

        f32                     m_dstAlpha;
        f32                     m_srcAlpha;
        f32                     m_fadeTime;
        f32                     m_fadeTotal;

        f32                     m_baseFontSize;
        bbool                   m_refreshScaleToMatchMultiLine;


        void                    updateActorIcon(f32 _dt);
        void                    setActorIconEnabled(bool _isEnabled);
        ITF_VECTOR<ActorIcon> m_iconActors;
        StringID m_currentActorIcon;
        StringID m_wantedActorIcon;
		
#ifdef ITF_SUPPORT_EDITOR
    public:
        virtual void            drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags ) const ;
        void                    drawBox(const f32 _border = 1.0f, const Color _color = COLOR_RED) const;
#endif //ITF_SUPPORT_EDITOR
    };

    //---------------------------------------------------------------------------------------------------

    class UITextBox_Template : public UIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UITextBox_Template,UIComponent_Template,2138714152);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(UITextBox);

    public:

        UITextBox_Template();
        ~UITextBox_Template() {}

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

        f32 getdepthOffset() const { return m_depthOffset; }

        ITF_VECTOR<FontTextArea::Style>         m_styles;
        ITF_VECTOR<Path>                        m_preSpawnedActorPaths;
        f32                                     m_depthOffset;   
    };


    ITF_INLINE const UITextBox_Template*  UITextBox::getTemplate() const {return static_cast<const UITextBox_Template*>(m_template);}
}
#endif // _ITF_UITEXTBOX_H_
