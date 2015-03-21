#ifndef _ITF_MULTITEXTBOXCOMPONENT_H_
#define _ITF_MULTITEXTBOXCOMPONENT_H_

#define MultiTextBoxComponent_CRC ITF_GET_STRINGID_CRC(MultiTextBoxComponent,3351280898)
#define MultiTextBoxComponent_Template_CRC ITF_GET_STRINGID_CRC(MultiTextBoxComponent_Template,1932438902)

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif // _LOCALISATION_ID_H_

#ifndef _ITF_FONTTEXTAREA_H_
#include "engine/display/FontTextArea.h"
#endif

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SMARTLOCID_H_
#include "engine/localisation/SmartLocId.h"
#endif //_ITF_SMARTLOCID_H_

namespace ITF
{
    class FontTemplate;

    class MultiTextBoxComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(MultiTextBoxComponent,ActorComponent, MultiTextBoxComponent_CRC);

        class TextBox
        {
            DECLARE_SERIALIZE()   

            friend class MultiTextBoxComponent;

            TextBox();

            FontTextArea            m_textArea;
            u32                     m_currentStyle;

            SmartLocId              m_text;
            u32                     m_defaultStyle;
            Vec2d                   m_area;
            Vec3d                   m_offset;
            Vec2d                   m_scale;
            Color                   m_color;
            FONT_ALIGNMENT          m_hAlignment;
            FONT_ALIGNMENT          m_vAlignment;
            TEXT_AREA_ANCHOR        m_anchor;
            bbool                   m_scaleToMatchWithArea;
            f32                     m_maxWidth;
            f32                     m_autoScrollSpeed;
            f32                     m_autoScrollWaitTime;
        };

    public:
        DECLARE_SERIALIZE()   

        MultiTextBoxComponent();
        virtual ~MultiTextBoxComponent();
        

        void                    setBox(u32 _textBoxIndex, const Vec2d& _area = Vec2d::Zero, const TEXT_AREA_ANCHOR _anchor = AREA_ANCHOR_NONE);
        void                    setFont(u32 _textBoxIndex, const Color _fontColor, const f32 _fontSize = 0.0f, const FONT_ALIGNMENT _hAlignment = FONT_ALIGN_NONE, const FONT_ALIGNMENT _vAlignment = FONT_ALIGN_NONE);
        void                    setShadow(u32 _textBoxIndex, const Vec2d& _shadowOffset, const Color _shadowColor);
        void                    setStyle(u32 _textBoxIndex, u32 _styleIndex);
        void                    setOffset(u32 _textBoxIndex, const Vec3d& _offset);
        void                    setSize(u32 _textBoxIndex, const f32 _fontSize);
        void                    setColor(u32 _textBoxIndex, const Color _fontColor);
        void                    setIsVisible(u32 _textBoxIndex, bbool _isVisible);
        void                    setText(u32 _textBoxIndex, const String8& _text);
        void                    setMaxWidth(u32 _textBoxIndex, f32 _width);
        Color                   getInitColor( u32 _textBoxIndex) const;
        f32                     getInitMaxWidth(u32 _textBoxIndex) const;
        const Vec3d &           getOffset(u32 _textBoxIndex) const;
        void                    insertTextClear(u32 _textBoxIndex);
        void                    insertTextAtMarker(u32 _textBoxIndex, u32 _marker, const String8 &_textToInsert);

        u32                     getTextBoxCount() const;
        const FontTextArea &    getTextArea(u32 _textBoxIndex) const;
        FontTextArea &          getTextArea(u32 _textBoxIndex);

        void                    setMainAlpha(const f32 _mainAlpha) {m_mainAlpha = _mainAlpha;}
        f32                     getMainAlpha() const {return m_mainAlpha;}
        void                    setMainScale(const f32 _mainScale) {m_mainScale = _mainScale;}

        
        virtual     void        onScaleChanged( const Vec2d& _oldScale, const Vec2d& _newScale );
        virtual     void        onResourceLoaded();

        virtual     bbool       needsUpdate         (       ) const             { return btrue; }
        virtual     bbool       needsDraw           (       ) const             { return !m_actor->getIs2D(); }
        virtual     bbool       needsDraw2D         (       ) const             { return m_actor->getIs2D(); }
		virtual		bbool		needsDraw2DNoScreenRatio(	) const				{ return m_actor->getIs2DNoScreenRatio(); }

        virtual void onStartDestroy(bbool _hotReload);
        virtual     void        onEvent             (Event * _event);
        virtual     void        onActorLoaded       ( Pickable::HotReloadType /*_hotReload*/ );
        virtual     void        onFinalizeLoad      ();
        virtual     void        onBecomeInactive();
        virtual     void        onBecomeActive();

        virtual     void        Update              ( f32 _deltaTime );
        virtual     void        batchPrimitives( const ITF_VECTOR <class View*>& _views );
        virtual     void        batchPrimitives2D( const ITF_VECTOR <class View*>& _views );

        bbool                   isTextUpdated() const;            

        struct ActorIcon
        {
            ActorIcon() : m_usedCount(0), m_iconFontSize(0), m_iconOffset(Vec2d::Zero), m_textBoxIndex(U32_INVALID) {}
            StringID m_iconName;
            ActorRef m_iconActor;
            f32      m_iconFontSize;
            u32      m_usedCount;
            Vec2d    m_iconOffset;
            u32      m_textBoxIndex;
        };

        void                    setActorIcon(StringID _iconName);
        ActorIcon*              useActorIcon(StringID _iconName, bbool _use);
        const ActorIcon*        getActorIcon(StringID _iconName) const;

#ifdef ITF_SUPPORT_EDITOR
        virtual     void        onPostPropertyChange();
#endif //ITF_SUPPORT_EDITOR

    protected:
        ITF_INLINE const class MultiTextBoxComponent_Template*  getTemplate() const;

        void                    updateText( f32 _deltaTime );
        void                    refreshText();
        void                    updateAABB();


        f32                     m_mainScale;
        f32                     m_mainAlpha;
        f32                     m_showAlpha;

        ITF_VECTOR<TextBox>     m_textBoxList;

        void                    updateActorIcon(f32 _dt);
        void                    clearActorIcon();
        ITF_VECTOR<ActorIcon>   m_iconActors;
        StringID                m_currentActorIcon;
        StringID                m_wantedActorIcon;

#ifdef ITF_SUPPORT_EDITOR
    public:
        virtual void    drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags ) const ;
        void            drawBox(Vec2d _minBound, Vec2d _maxBound, const f32 _border = 1.0f, const Color _color = COLOR_RED) const;
        virtual void    onEditorMove(bbool _modifyInitialPos = btrue);
#endif //ITF_SUPPORT_EDITOR
    };

    //---------------------------------------------------------------------------------------------------

    class MultiTextBoxComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(MultiTextBoxComponent_Template,ActorComponent_Template,MultiTextBoxComponent_Template_CRC);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(MultiTextBoxComponent);

    public:

        MultiTextBoxComponent_Template();
        ~MultiTextBoxComponent_Template() {}

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
    };


    ITF_INLINE const MultiTextBoxComponent_Template*  MultiTextBoxComponent::getTemplate() const {return static_cast<const MultiTextBoxComponent_Template*>(m_template);}
}
#endif // _ITF_MULTITEXTBOXCOMPONENT_H_
