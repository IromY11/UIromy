#ifndef _ITF_FONTTEXT_H_
#define _ITF_FONTTEXT_H_

#ifndef ITF_GFX_PRIMITIVES_H_
#include "engine/display/Primitives/GFXPrimitives.h"
#endif

#ifndef ITF_ENGINE_MESH_H_
#include "engine/display/Primitives/Mesh.h"
#endif
#include "FontTemplate.h"

namespace ITF
{
    //=================================================================================================
    //
    //  Font Effect template
    //      data for some effect on text characters...
    //
    //=================================================================================================
    class FontEffect_Template
    {
        DECLARE_SERIALIZE()

    public:
        //---------------------------------------------------------------------------------------------
        // effect flag enumeration
        //
        enum EffectFlag
        {
            none = 0,
            LanguageJapaneseExcept,
        };

        //---------------------------------------------------------------------------------------------
        // effect type enumeration
        //
        enum EffectType
        {
            shiftY = 0,
            scale,
            rotate,
            zoomAlpha,
            clampY,
            shiftYandRotate
        };

        //---------------------------------------------------------------------------------------------
        // constructor
        //
    public:
        FontEffect_Template() 
        {
            setTime(0,0,-1,-1);
            setSpeed(1,1);
            m_static = bfalse;
            m_staticSeed = 0;
            m_min = 0;
            m_max = 0;
            m_value = 0;
            m_limit = 0;
            m_type = shiftY;
            m_rotateCycle = 0.0f;
            m_rotateSeedFactor = 1.0f;
            m_flags = none;
        }
        ~FontEffect_Template() {}

        //---------------------------------------------------------------------------------------------
        // overriden functions
        //
    public:
        //virtual bbool   onTemplateLoaded( bbool _hotReload );
        //virtual void    onTemplateDelete( bbool _hotReload );

        //---------------------------------------------------------------------------------------------
        // main accessors
        //
    public:
        ITF_INLINE const bbool hasFlags(EffectFlag _flags) const{ return (m_flags & _flags) == _flags; }
        ITF_INLINE const EffectType &type() const{ return m_type; }
        ITF_INLINE const StringID &name() const{ return m_name; }
        ITF_INLINE const bbool isNamed(StringID _name ) const{ return m_name == _name; }
        ITF_INLINE const bbool &isStatic() const { return m_static; }
        ITF_INLINE const f32 &staticSeed()  const{ return m_staticSeed; }

        void setTime( f32 _inStart, f32 _inEnd, f32 _outStart, f32 _outEnd )
        {
            m_timeFadeInStart = _inStart;
            m_timeFadeInEnd = _inEnd;
            m_timeFadeOutStart = _outStart;
            m_timeFadeOutEnd = _outEnd;
        }

        void setStatic( bbool _static, f32 _seed = 0)
        {
            m_static = _static;
            m_staticSeed = _seed;
        }

        void setSpeed( f32 _min, f32 _max )
        {
            m_speedMin = _min;
            m_speedMax = _max;
        }


        //---------------------------------------------------------------------------------------------
        // time accessors
        //
    public:
        ITF_INLINE  f32 fadeinStart() const{ return m_timeFadeInStart; }
        ITF_INLINE  f32 fadeinEnd() const{ return m_timeFadeInEnd; }
        ITF_INLINE  f32 fadeoutStart() const{ return m_timeFadeOutStart; }
        ITF_INLINE  f32 fadeoutEnd() const{ return m_timeFadeOutEnd; }

        //---------------------------------------------------------------------------------------------
        // speed
        //
    public:
        ITF_INLINE  f32 speedMin() const{ return m_speedMin; }
        ITF_INLINE  f32 speedMax() const{ return m_speedMax; }

        //---------------------------------------------------------------------------------------------
        // shiftY effect accessors
        //
    public:
        ITF_INLINE  f32 shiftYMin() const{ return m_min; }
        ITF_INLINE  f32 shiftYMax() const{ return m_max; }
        ITF_INLINE  f32 shiftYLimit() const{ return m_limit; }

        //---------------------------------------------------------------------------------------------
        // scale effect accessors
        //
    public:
        ITF_INLINE  f32 scaleMin() const{ return m_min; }
        ITF_INLINE  f32 scaleMax() const{ return m_max; }

        //---------------------------------------------------------------------------------------------
        // rotate effect accessors
        //
    public:
        ITF_INLINE  f32 rotateAngle() const{ return m_value; }
        ITF_INLINE  f32 rotateCycle() const{ return m_rotateCycle; }
        ITF_INLINE  f32 rotateSeedFactor() const{ return m_rotateSeedFactor; }

        //---------------------------------------------------------------------------------------------
        // zoom alpha accessors
        //
    public:
        ITF_INLINE  f32     zoomAlphaMin() const { return m_min; }
        ITF_INLINE  f32     zoomAlphaMax() const { return m_max; }
        ITF_INLINE  bbool   zoomIsReverse() const { return m_value == 1; }

        //---------------------------------------------------------------------------------------------
        // members
        //
    private:
        StringID    m_name;
        EffectType  m_type;
        EffectFlag  m_flags;

        f32         m_timeFadeInStart;      // fade in time => the effect begins 
        f32         m_timeFadeInEnd;        // fade in end time => the effects beginning is finished, after that effect is full
        f32         m_timeFadeOutStart;     // start exit effect
        f32         m_timeFadeOutEnd;       // end exit effect => no more effect after this time

        f32         m_speedMin;             // speed min and max. for shiftY, scale and rotate effect value reprensent the number of cycle per seconds
        f32         m_speedMax;

        bbool       m_static;               // effect is applied only at mesh construction, no animation.
        f32         m_staticSeed;           // seed to apply when effect is applied at mesh construction

        f32         m_min;                  // min parameter, used for shiftY / scale
        f32         m_max;                  // max parameter, used for shiftY / scale
        f32         m_limit;                // limit parameter, used for shiftY 
        f32         m_value;                // value parameter, used for rotate
        f32         m_rotateCycle;          // rotation cycle parameter, used for rotate
        f32         m_rotateSeedFactor;     // rotation seed factor, used for rotate
    };

    //=================================================================================================
    //
    // Font Effect : use to add small effect on position / size / rotation of text letters.
    //
    //=================================================================================================
    class FontEffect
    {
    public:
        //---------------------------------------------------------------------------------------------
        FontEffect();

        //---------------------------------------------------------------------------------------------
        void setTemplate( const class FontEffect_Template *_template ) { m_params = _template; }
        const FontEffect_Template *getTemplate() { return m_params; }

        //---------------------------------------------------------------------------------------------
        void setUser( u32 _value ) { m_user = _value; }
        u32 getUser() { return m_user; }

        //---------------------------------------------------------------------------------------------
        bbool isActive() { return m_active; }
        bbool isFinished() { return m_finished; }
        void finish();

        //---------------------------------------------------------------------------------------------
        void activate( bbool _active);
        void reset();

        void timeSet( f32 _startFadeIn, f32 _endFadeIn, f32 _startFadeOut, f32 _endFadeOut);
        void timeReset();

        //---------------------------------------------------------------------------------------------
        void shiftYApply( f32 _seed, Vec3d &_P0, Vec3d &_P1, Vec3d &_P2, Vec3d &_P3 );

        //---------------------------------------------------------------------------------------------
        void scaleApply( f32 _seed, Vec3d &_P0, Vec3d &_P1, Vec3d &_P2, Vec3d &_P3 );

        //---------------------------------------------------------------------------------------------
        void rotateApply( f32 _seed, Vec3d &_P0, Vec3d &_P1, Vec3d &_P2, Vec3d &_P3, const Vec2d &_area );

        //---------------------------------------------------------------------------------------------
        void zoomAlphaApply( f32 _seed, Vec3d &_P0, Vec3d &_P1, Vec3d &_P2, Vec3d &_P3, u32 &_color);

        //---------------------------------------------------------------------------------------------
        void update(f32 _deltaTime);
        void forceFinish();
    
        //---------------------------------------------------------------------------------------------
        void apply( Vec3d &_P0, Vec3d &_P1, Vec3d &_P2, Vec3d &_P3, u32 &_color, f32 _seed, const Vec2d &_area);

    private:
        //---------------------------------------------------------------------------------------------
        // global parameters
        //
        const class FontEffect_Template *m_params;

        bbool   m_active;
        bbool   m_finished;
        f32     m_time;
        f32     m_ratio; 

        bbool   m_forceFinish;
        u32     m_user;
    };

    //=================================================================================================
    //
    // FontEffectChain: font effect chain
    //
    //=================================================================================================

    struct FontEffectChain
    {
        class   FontEffect *m_fontEffect;
        struct  FontEffectChain *m_next;
    };


    //=================================================================================================
    //
    // FontText: primitive to display text
    //
    //=================================================================================================
    class FontText : public GFXPrimitive
    {
    public:
        FontText();
        ~FontText();

        virtual void directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z = 0.0f, f32 _ZWorld = 0.0f, f32 _ZAbs = 0.0f);

        const String8 & getText() const { return m_text; }
        const Vec3d & getPosition() const { return m_position; }
        bbool is2D() const { return m_is2D; }

        void setEffectChain(FontEffectChain *_fontEffectChain );
        void setText(const String8 &_text);
        void appendText(const String8 &_text);
        void setFontTemplate(const FontTemplate &_font);
        void setSize(f32 _size) { m_size = _size; }
        void setColor(const Color &_color) { m_color = _color; }
        void setShadowColor(const Color &_color) { m_shadowColor = _color; }
        void setShadowOffset(const Vec2d &_offset) { m_shadowOffset = _offset; }

        void setIs2D(bbool _is2D) { m_is2D = _is2D; }
        void setPosition(const Vec3d &_position) { m_position = _position; }
        void setLocalPivot(const Vec2d &_localPivot);
        void setLocalScale(const Vec2d &_localScale) { m_localScale = _localScale; }
        void setLocalAngle(f32 _localAngle) { m_localAngle = _localAngle; }
        void setArea(const Vec2d& _area) { m_area = _area; }
        void setOffset(const Vec3d& _offset) { m_offset = _offset; }

        void setCharSpacing( f32 _charSpacing );
        void setAdvanceCharSize(char _constAdvanceChar_Size);
        void setAdvanceCharMin(char _constAdvanceChar_Min);
        void setAdvanceCharMax(char _constAdvanceChar_Max);

        void setUseGradient(bbool _useGradient) { m_useGradient = _useGradient; }
        void setGradientScale(f32 _scale) { m_gradientScale = _scale; }
        void setGradientOffset(f32 _offset) { m_gradientOffset = _offset; }
        void setGradientColor(const Color &_color) { m_gradientColor = _color; }

        void setClippingRect(const AABB &_rect) { m_clippingRect = _rect; }

    protected:
        void computeMeshByPage();
        void computeFinalMatrix(Matrix44& _finalMatrix) const;
        void computePosition(f32& x, f32& y, f32& z) const;
        void setupScissor(GFXAdapter * _gfxDeviceAdapter, const Vec2d &_offset = Vec2d::Zero) const;
        void resetScissor(GFXAdapter * _gfxDeviceAdapter) const;
        void drawMesh(class PrimitiveContext & _rdrPassCtxt, GFXAdapter * _gfxDeviceAdapter) const;
        void freeMesh();

    private:
        String8 m_text;
        const FontTemplate* m_font;
        f32 m_size;
        Color m_color;

        Vec2d m_shadowOffset;
        Color m_shadowColor;
 
        bbool m_is2D;
        Vec3d m_position;
        Vec2d m_localPivot;
        Vec2d m_localScale;
        Vec3d m_offset;
        f32 m_localAngle;

        char m_constAdvanceChar_Size;
        char m_constAdvanceChar_Min;
        char m_constAdvanceChar_Max;
        float m_charSpacing;

        bbool m_useGradient;
        f32   m_gradientScale;
        f32   m_gradientOffset;
        Color m_gradientColor;

        FontEffectChain *m_effectChain;
        Vec2d m_area;
        bbool m_bIsDirty;

        AABB m_clippingRect;

        ITF_VECTOR<ITF_Mesh> m_meshByPage;
        ITF_VECTOR<class VertexPCB2T*> m_bufferByPage;
    };



}

#endif // _ITF_FONTTEXT_H_
