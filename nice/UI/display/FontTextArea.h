#ifndef _ITF_FONTTEXTAREA_H_
#define _ITF_FONTTEXTAREA_H_

#ifndef _ITF_FONTTEXT_H_
#include "engine/display/FontText.h"
#endif // _ITF_FONTTEXT_H_

#ifndef _ITF_ACTORSPAWNER_H_
#include "gameplay/AI/Utils/ActorSpawner.h"
#endif //_ITF_ACTORSPAWNER_H_

#ifndef ITF_ENGINE_QUAD2DPRIM_H_
#include "engine/display/Primitives/Quad2DPrim.h"
#endif //ITF_ENGINE_QUAD2DPRIM_H_

namespace ITF
{
    class FontTemplate;

    #define TEXT_BLOCK_FLAG_EndWorld    0x00000001
    #define TEXT_BLOCK_FLAG_RubyTop     0x00000002
    #define TEXT_BLOCK_FLAG_RubyBottom  0x00000004
    #define TEXT_MARKER                 0x1000

    #define TEXT_RUBY_SCALE             0.5f

    enum TEXT_BLOCK_TYPE
    {
        TEXT_BLOCK_NONE     = -1,
        TEXT_BLOCK_WORD     = 0,
        TEXT_BLOCK_ICON     = 1,
        TEXT_BLOCK_ACTOR    = 2,
        ENUM_FORCE_SIZE_32(TEXT_BLOCK_TYPE)
    };

    enum TEXT_AREA_ANCHOR
    {
        AREA_ANCHOR_NONE = -1,
        AREA_ANCHOR_TOP_LEFT = 0,
        AREA_ANCHOR_MIDDLE_CENTER = 1,
        AREA_ANCHOR_MIDDLE_LEFT = 2,
        AREA_ANCHOR_MIDDLE_RIGHT = 3,
        AREA_ANCHOR_TOP_CENTER = 4,
        AREA_ANCHOR_TOP_RIGHT = 5,
        AREA_ANCHOR_BOTTOM_CENTER = 6,
        AREA_ANCHOR_BOTTOM_LEFT = 7,
        AREA_ANCHOR_BOTTOM_RIGHT = 8,
        ENUM_FORCE_SIZE_32(TEXT_AREA_ANCHOR)
    };

#define SERIALIZE_ENUM_H_ALIGNMENT(name, member) \
    SERIALIZE_ENUM_BEGIN(name,member); \
    SERIALIZE_ENUM_VAR(FONT_ALIGN_NONE); \
    SERIALIZE_ENUM_VAR(FONT_ALIGN_LEFT); \
    SERIALIZE_ENUM_VAR(FONT_ALIGN_CENTER); \
    SERIALIZE_ENUM_VAR(FONT_ALIGN_RIGHT); \
    SERIALIZE_ENUM_VAR(FONT_ALIGN_JUSTIFY); \
    SERIALIZE_ENUM_END();

#define SERIALIZE_ENUM_V_ALIGNMENT(name, member) \
    SERIALIZE_ENUM_BEGIN(name,member); \
    SERIALIZE_ENUM_VAR(FONT_ALIGN_NONE); \
    SERIALIZE_ENUM_VAR(FONT_ALIGN_TOP); \
    SERIALIZE_ENUM_VAR(FONT_ALIGN_MIDDLE); \
    SERIALIZE_ENUM_VAR(FONT_ALIGN_BOTTOM); \
    SERIALIZE_ENUM_END(); 

#define SERIALIZE_ENUM_ANCHOR(name, member) \
    SERIALIZE_ENUM_BEGIN(name,member); \
    SERIALIZE_ENUM_VAR(AREA_ANCHOR_NONE); \
    SERIALIZE_ENUM_VAR(AREA_ANCHOR_TOP_LEFT); \
    SERIALIZE_ENUM_VAR(AREA_ANCHOR_MIDDLE_CENTER); \
    SERIALIZE_ENUM_VAR(AREA_ANCHOR_MIDDLE_LEFT); \
    SERIALIZE_ENUM_VAR(AREA_ANCHOR_MIDDLE_RIGHT); \
    SERIALIZE_ENUM_VAR(AREA_ANCHOR_TOP_CENTER); \
    SERIALIZE_ENUM_VAR(AREA_ANCHOR_TOP_RIGHT); \
    SERIALIZE_ENUM_VAR(AREA_ANCHOR_BOTTOM_CENTER); \
    SERIALIZE_ENUM_VAR(AREA_ANCHOR_BOTTOM_LEFT); \
    SERIALIZE_ENUM_VAR(AREA_ANCHOR_BOTTOM_RIGHT); \
    SERIALIZE_ENUM_END(); 


    class FontTextArea
    {
        DECLARE_SERIALIZE()

        class FontTextBlock
        {
        public:
            TEXT_BLOCK_TYPE m_type;
            String8 m_text;
            i32 m_index;
            bbool m_isButton;
            u32 m_actorIndex;

            Vec2d m_offset;
            Vec2d m_position;
            Vec2d m_bound;
            Vec2d m_scale;
            i32 m_line;
            bbool m_isEOL;

            Color m_color;
            f32 m_size;

            i32 m_effectIndex;
            i32 m_marker;

            u32 m_rubyTopIndex;

            u32 m_flags;

            bbool hasSameSettingsAs(const FontTextBlock &_compareTo )
            {
                if (m_marker != -1) return bfalse;
                if (m_line != _compareTo.m_line) return bfalse;
                if (m_color != _compareTo.m_color) return bfalse;
                if (m_size != _compareTo.m_size) return bfalse;
                if (m_effectIndex != _compareTo.m_effectIndex) return bfalse;
                if (m_rubyTopIndex != _compareTo.m_rubyTopIndex) return bfalse;
                if ((m_flags & TEXT_BLOCK_FLAG_RubyTop) || (_compareTo.m_flags & TEXT_BLOCK_FLAG_RubyTop)) return bfalse;
                if ((m_flags & TEXT_BLOCK_FLAG_RubyBottom) != (_compareTo.m_flags & TEXT_BLOCK_FLAG_RubyBottom)) return bfalse;
                return btrue;
            }
        };

        struct BlockActorData
        {
            ActorSpawner m_spawner;
            ActorRef    m_ref;
            AABB        m_aabb;
            Vec2d       m_scale;
            bbool       m_ready;
            bbool       m_isPreSpawnedActor;
            f32         m_fontSizeBase;
        };

        struct LineData
        {
            LineData()
                : m_blockCount(0)
                , m_paragraphEnd(bfalse)
            {
            }
            LineData(f32 _width, f32 _height, u32 _wordCount, bbool _paragraphEnd)
                : m_bounds(_width, _height)
                , m_blockCount(_wordCount)
                , m_paragraphEnd(_paragraphEnd)
            {
            }
            Vec2d   m_bounds;
            u32     m_blockCount;
            bbool   m_paragraphEnd;
        };

    public:

        struct FontSet
        {
            DECLARE_SERIALIZE()

            Path m_default;
            // languages that will probably asked for a specific font
            Path m_japanese;
            Path m_korean;
            Path m_tradChinese;
            Path m_simpleChinese;
            Path m_russian;

            const Path & getFontPath(ITF_LANGUAGE _language) const;
        };

        struct Style
        {
            DECLARE_SERIALIZE()

            Style();

            const FontTemplate*     m_fontTemplate;
            Path                    m_fontPath;
            FontSet                 m_fontSet;
            f32                     m_fontSize;
            Color                   m_color;
            FONT_ALIGNMENT          m_hAlignment;
            FONT_ALIGNMENT          m_vAlignment;
            TEXT_AREA_ANCHOR        m_anchor;
            Color                   m_shadowColor;
            Vec2d                   m_shadowOffset;
            f32                     m_lineSpacing;
            f32                     m_paragraphSpacing;
            f32                     m_charSpacing;
            bbool                   m_useGradient;
            f32                     m_gradientSize;
            f32                     m_gradientOffset;
            Color                   m_gradientColor;

            void onLoaded(class ResourceContainer * _parentContainer = NULL); 
            void onUnLoaded(class ResourceContainer * _parentContainer = NULL);
        };

        FontTextArea();
        ~FontTextArea();

        void                setIsActive(const bbool _isActive);
        void                setIsVisible(const bbool _isVisible);
        void                setText(const String8 & _text);
        void                insertText( u32 _where, const String8 &_textToInsert);
        void                insertTextAtMarker( u32 _marker, const String8 &_textToInsert);
        static void         insertText( String8 &_text, u32 _where, const String8 &_textToInsert);
        static void         insertTextAtMarker( String8 &_text, u32 _marker, const String8 &_textToInsert);
        void                setFont(const FontTemplate* _font);
        void                setStyle(const Style& _style);
        void                setSize(const f32 _size);
        const String8 &     getText() const {return m_text;}
        const FontTemplate* getFont() const {return m_font;}
        f32                 getSize() const;

        void                setHAlignment(const FONT_ALIGNMENT _hAlignment);
        void                setVAlignment(const FONT_ALIGNMENT _vAlignment);
        void                setText(const String8 & _text, FontTemplate* _font, const f32 _size = -1.0, const Color _color = COLOR_WHITE);
        void                setAlignment(const FONT_ALIGNMENT _hAlignment, const FONT_ALIGNMENT _vAlignment);
        void                setConstAdvanceChar(const char _constAdvanceChar_Size, const char _constAdvanceChar_Min = '\0', const char _constAdvanceChar_Max = '\0') ;
        void                setArea(const Vec2d& _area);
        void                setOwner(Actor* _owner);
        void                setLocalPivot(const Vec2d& _localPivot);
        void                setLocalScale(const Vec2d& _localScale);
        void                setLocalAngle(const f32 _localAngle);
        void				setScrollPx(const f32 _deltaScrollPx = 0);
        ITF_INLINE void     setViewMask(u32 _mask) {m_viewMask = _mask;}
        ITF_INLINE void     setIs2D(const bbool _is2D) {m_is2D = _is2D;}
        void                setColor(const Color _color);
        ITF_INLINE void     setAlpha(const f32 _alpha) {m_alpha = _alpha;}
        void                setPosition(const Vec3d& _position);
        ITF_INLINE void     setPosition(const Vec2d& _position) {setPosition(Vec3d(_position.x(), _position.y(), m_position.z()));}
        ITF_INLINE void     setDepth(const i32 _depth) {m_position.z() = (f32) _depth;}
        ITF_INLINE void     setAnchor(const TEXT_AREA_ANCHOR _anchor) {m_anchor = _anchor;}
        ITF_INLINE void     setShadowOffset(const Vec2d& _shadowOffset) {m_shadowOffset = _shadowOffset;}
        ITF_INLINE void     setShadowColor(const Color _shadowColor) {m_shadowColor = _shadowColor;}
        ITF_INLINE void     setShadow(const Vec2d& _shadowOffset, const Color _shadowColor) {m_shadowOffset = _shadowOffset; m_shadowColor = _shadowColor;}
        ITF_INLINE void     setGradient(bbool _use, f32 _size, const Color _color, f32 _offset) {m_useGradient = _use; m_gradientSize = _size; m_gradientColor = _color; m_gradientOffset = _offset;}
        ITF_INLINE void		setIsPassword(bbool inIsPassword) { m_isPassword = inIsPassword; }
        ITF_INLINE void		setLineSpacing( f32 _lineSpacing ) { m_lineSpacing = _lineSpacing; }
        ITF_INLINE void		setParagraphSpacing(f32 _paragraphSpacing) { m_paragraphSpacing = _paragraphSpacing; }
        ITF_INLINE void		setCharSpacing(f32 _charSpacing) { m_charSpacing = _charSpacing; }
        ITF_INLINE void		setGlobalActorScaleFactor(f32 _value) { m_globalActorScaleFactor = _value; }
        void				setViewportVisibility(u32 _value);
        void                setMaxWidth(f32 _maxWidth);
        ITF_INLINE void     setAutoScrollXSpeed(f32 _speed) { m_scrollXSpeed = _speed; }
        ITF_INLINE void     setAutoScrollXWaitTime(f32 _time) { m_scrollXWaitTime = _time; }
        void     setAutoScrollXLoop(bbool _loop);
        void     setAutoScrollXLoopGap(u32 _gap);
        
        ITF_INLINE void     setFontEffect( const FontEffect_Template *_fontEffect, bbool _smooth = false ) { clearFontEffect(_smooth); addFontEffect(_fontEffect); }
                   void     terminateFontEffect();
        ITF_INLINE void     clearFontEffect(bbool _smooth = bfalse) { if (_smooth) terminateFontEffect(); else m_dynamicFontEffects.clear(); m_needAlign = btrue; }
                   void     addFontEffect( const FontEffect_Template *_fontEffect, u32 _markerStart = (u32) -1, u32 _markerEnd = (u32) -1 );
                   void     removeFontEffect( FontEffect *_fontEffect, u32 _fontEffectIndex );

        ITF_INLINE bbool    getIs2D() const {return m_is2D;}
        ITF_INLINE const    Vec3d& getPosition() const {return m_position;}
        ITF_INLINE const    Vec2d& getLocalScale() const {return m_localScale;}
        ITF_INLINE FONT_ALIGNMENT getHAlignment() const { return m_hAlignment; }

        ITF_INLINE const Color &getColor() const { return m_color; }
        ITF_INLINE i32      getLineCount() const {return m_linesCount;}
        ITF_INLINE i32		getScrollStart() const {return m_scrollStartLine;}
        ITF_INLINE f32      getLineWidth(const i32 _line) const {return m_linesBounds[_line].m_bounds.x(); }
        ITF_INLINE f32      getLineHeight(const i32 _line) const {return m_linesBounds[_line].m_bounds.y(); }
        ITF_INLINE u32      getLineBlockCount(const i32 _line) const {return m_linesBounds[_line].m_blockCount; }
        ITF_INLINE bbool    isLastLineOfParagraph(const i32 _line) const {return m_linesBounds[_line].m_paragraphEnd; }
        ITF_INLINE f32      getMaxWidth() const { return m_maxWidth; }

        ITF_INLINE const ITF_VECTOR<ActorRef>&    getParsedActors() const { return m_parsedActors; }

        f32                 getTextWidth(const i32 _lineStart = 0, const i32 _lineCount = -1) const;
        f32                 getTextHeight(const i32 _lineStart = 0, const i32 _lineCount = -1) const;
        Vec2d               getTextAreaBound() const;
        AABB                getAABB(bbool _useRotation = bfalse) const;
        AABB                getLocalAABB() const;
        AABB                getTextAreaAABBWithoutRotation();
        Vec2d               getLocalTopLeftOffset(const Vec2d& _bound, const Vec2d& _offset, bbool _inverseY) const;
        bbool               getAABBAnchorPos(TEXT_AREA_ANCHOR _anchor, Vec3d& _pos, Vec2d& _scale, f32& _angle, const Vec2d& _offset = Vec2d::Zero) const;
        bbool               getFirstCharPos(Vec3d& _pos, Vec2d& _scale, f32& _angle, const Vec2d& _offset = Vec2d::Zero) const;
        
        bbool               getIsUpdated() const {return !m_needParsing && !m_needUpdate && !m_needAlign;}
        f32                 getUpdateDate() const {return m_updateDate;}
        f32                 getParsingDate() const {return m_parsingDate;}
        void                Update(f32 _deltaTime);
        void                Draw(const ITF_VECTOR <class View*>& _viewsGraphicComponent_Template, f32 _depthOffset, const ConstObjectRef & _objectRef);

        void                addPreSpawnedActor(const Path& _path);
        void                removePreSpawnedActor(const Path& _path);

        void                setDisplayedCharCount(const i32 _count);
        void                clear();

    private:

        static i32          getMarkIndex( const String8 & _text, u32 _marker );
        static i32          computeTag(String8 & _path, const String8 & _beginTag, const String8 & _endTag, const i32 _charCursor, const String8 & _text);

        i32                 computeNextBlock(FontTextBlock& _block, const i32 _charCursor, const String8 & _text, const FontTextBlock *_previousBlock);
        void                computeBlockParsing();
        void                computeBlockSize();
        void                computeBlockActor();
        void                computeBlockLine();
        void                computeBlockAlignment();
        void                computeBlockText();
        void                computeTransform();
        void                computeTransformActors();
        void				computeDisplayAreaLines();
        void                computeFontEffectChain();
        void                computeScroll();

        u32                 createActor(const Path& _path, bbool _isPreSpawnedActor = bfalse, f32 _fontSizeBase = 0.0f);
        void                clearActors();
        void                clearSpawnedActors();
        void                clearParsedActors();
        bbool               areActorsReady();

        void                computePivotMulMatrixOffset(Matrix44& _anchorMulMatrix, bbool _useRotation) const;
        void                computeGlobalPosition(Vec3d& _globalPos, const Vec3d& _localPosition, const Matrix44& _mulMatrix) const;
        void                computeGlobalScale(Vec2d& _globalScale, const Vec2d& _localScale, const Matrix44& _mulMatrix) const;
        AABB                computeBlockAABB(const FontTextBlock& _block) const;

        BlockActorData *    getBlockWithRef(const ActorRef & _ref);

        void                updateScroll(f32 _dt);
        
    private :

        bbool m_isPassword;
        String8 m_text;
        String8 m_displayText;
        const FontTemplate* m_font;
        f32 m_size;
        Color m_color;
        f32 m_alpha;
        FONT_ALIGNMENT m_hAlignment;
        FONT_ALIGNMENT m_vAlignment;

        char m_constAdvanceChar_Size;
        char m_constAdvanceChar_Min;
        char m_constAdvanceChar_Max;

        Vec2d m_shadowOffset;
        Color m_shadowColor;

        f32 m_lineSpacing;
        f32 m_paragraphSpacing;
        f32 m_charSpacing;
        f32 m_rubySpacing;

        bbool m_useGradient;
        f32   m_gradientSize;
        f32   m_gradientOffset;
        Color m_gradientColor;
        u32 m_viewportVisibility;

        i32 m_displayedCharCount;

        bbool m_is2D;
        Actor* m_owner;
        u32 m_viewMask;
        Vec3d m_position;
        Vec2d m_localPivot;
        Vec2d m_localScale;
        f32 m_localAngle;
        Vec2d m_area;
        bbool m_isVisible;
        bbool m_isActive;
        TEXT_AREA_ANCHOR m_anchor;

        ITF_VECTOR<FontTextBlock> m_textBlocks;
        ITF_VECTOR<LineData> m_linesBounds;
        i32 m_linesCount;		
        i32 m_scrollStartLine;		// start line (top)
        i32 m_lastScrollStart;
        i32 m_scrollLineCount;	// num lines visible
        f32 m_scrollDelta;
        f32 m_scrollDrawOffset;

        f32 m_maxWidth;
        f32 m_scrollXSize;
        f32 m_scrollXOffset;
        f32 m_scrollXTime;
        f32 m_scrollXSpeed;
        f32 m_scrollXWaitTime;
        bbool m_scrollXLoop;
        u32 m_scrollXLoopGap;
        String8 m_scrollXLoopGapString;

        ITF_VECTOR<BlockActorData> m_spawnedActors;
        ITF_VECTOR<ActorRef> m_parsedActors;
        ITF_VECTOR<FontText> m_fontTexts;
        ITF_VECTOR<FontEffect> m_fontEffects;
        ITF_VECTOR<FontEffect> m_dynamicFontEffects;
        ITF_VECTOR<FontEffect> m_dynamicFontEffectsToAdd;
        ITF_VECTOR<FontEffectChain> m_fontEffectChain;

        bbool m_needParsing;
        bbool m_needUpdate;
        bbool m_needAlign;
        bbool m_needTransform;
        f32 m_updateDate;
        f32 m_parsingDate;
        f32 m_globalActorScaleFactor;

        String8 m_workingTagPath;
    };

}




#endif // _ITF_FONTTEXTAREA_H_