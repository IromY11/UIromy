#ifndef _ITF_TRAIL_H_
#define _ITF_TRAIL_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_BONEID_H_
#include "engine/animation/BoneID.h"
#endif //_ITF_BONEID_H_

#ifndef ITF_GFX_PRIMITIVES_H_
#include "engine/display/Primitives/GFXPrimitives.h"
#endif

namespace ITF
{
    //#define TRAIL_DBG

    class TrailPoint
    {
    public:
        Vec3d   m_pos;
        Vec2d   m_norm;
        f32     m_thickness;
        Vec2d   m_uv1;
        Vec2d   m_uv2;

        f32     m_alpha;
        i32     m_life;
    };

    class Trail : public GFXPrimitive
    {
    private:

        const class Trail_Template* m_template;
    public:
        DECLARE_SERIALIZE()

        Trail();
        ~Trail();

        virtual void directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z = 0.0f, f32 _ZWorld = 0.0f, f32 _ZAbs = 0.0f);

        ITF_INLINE const GFXMaterialSerializable &  getMaterial() const;
        const StringID &getAttachedBone() { return m_attachBone; }

        void            addPoint(const Vec3d &_pos, f32 _angle);
        void            update();
        void            startFade();
        bbool           fade(f32 _deltaTime);

        void            updateAABB(AABB &_aabb);
        void            multiplyColor( const Color& _color );
        void            clear();

        void            setTemplate(const Trail_Template* _template)  {m_template = _template;}

        void            setThicknessScale(f32 _scale) {m_thicknessScale = _scale;}
        void            setIs2D(bbool _is2D) {m_is2D = _is2D;}
        void            setAlphaMultiplier(f32 _alphaMultiplier) {m_alphaMultiplier = _alphaMultiplier;}

        void            setColor( const Color& _color );
        void            resetSetColor();

        void            initData();//bbool _setSpecificNb = bfalse, u32 _specificNb = 0);
        void            setUseLife(bbool _useLife) { m_useLife = _useLife;}

        ITF_INLINE      SafeArray<TrailPoint>& getTrailPointTab()    { return m_trailPointTab; }
        
		ITF_INLINE      void setActive(bbool _active)    { m_active = _active; }
		ITF_INLINE      bbool getActive()    { return m_active; }

        ITF_INLINE      void setTrailPointTabLenght(u32 _length)    { m_currTrailLenght = _length; }
        ITF_INLINE      void setTrailPointTabBegin(u32 _begin)      { m_currTrailBegin = _begin; }

    private:


        ITF_INLINE const class Trail_Template*  getTemplate() const;
        void            addPointPriv(const Vec3d &_pos, f32 _angle);
        bbool           removeFirstPoint();
        bbool           removeLastPoint();

        #ifdef TRAIL_DBG
        unsigned int m_dbgDrawFeedPoints : 1;
        #endif // TRAIL_DBG

        u32                         m_hdiv;
        u32                         m_vdiv;

        StringID                    m_attachBone;

        Color                       m_color;
        f32                         m_thicknessScale;
        bbool                       m_is2D;
        f32                         m_alphaMultiplier;
        

        f32                         m_trailFaidingValue;

        u32                         m_currTrailLenght;
        u32                         m_currTrailBegin;
        SafeArray<TrailPoint>       m_trailPointTab;

        SafeArray<Vec3d>            m_shaderPointBuffer;
        bbool                       m_useLife;

        bbool                       m_useSetColor;
        Color                       m_setColor;

		bbool						m_active;
    };


    class Trail_Template
    {
        DECLARE_SERIALIZE()

    public:

        Trail_Template();
        ~Trail_Template() {}

        const GFXMaterialSerializable& getMaterial() const { return m_material; }
        GFXMaterialSerializable&       getMaterialnc() { return m_material; }
        u32                     getNBFrames() const { return m_nbFrames; }
        u32                     getFixTrailLenght() const { return m_fixTrailLenght; }
        f32                     getTrailFadingTime() const { return m_trailFadingTime; }
        f32                     getThicknessBegin() const { return m_thicknessBegin; }
        f32                     getThicknessEnd() const { return m_thicknessEnd; }
        f32                     getAlphaBegin() const { return m_alphaBegin; }
        f32                     getAlphaEnd() const { return m_alphaEnd; }
        f32                     getTrailBlending() const { return m_trailBlending; }
        f32                     getFadeLength() const { return m_fadeLength; }
        f32                     getTesselateMaxLen() const { return m_tesselateMaxLen; }
        f32                     getTesselateMinLen() const { return m_tesselateMinLen; }

        const Color&            getInitColor() const { return m_initColor; }
        const StringID&         getAttachBone() const { return m_attachBone; }

    private:

        GFXMaterialSerializable m_material;
        u32                     m_nbFrames;
        f32                     m_trailFadingTime;
        f32                     m_thicknessBegin;
        f32                     m_thicknessEnd;
        f32                     m_alphaBegin;
        f32                     m_alphaEnd;
        f32                     m_trailBlending;
        f32                     m_fadeLength;
        f32                     m_tesselateMaxLen;
        f32                     m_tesselateMinLen;
        u32                     m_fixTrailLenght;

        Color                   m_initColor;
        StringID                m_attachBone;
    };



    ITF_INLINE const Trail_Template*  Trail::getTemplate() const {return static_cast<const Trail_Template*>(m_template);}

    ITF_INLINE const GFXMaterialSerializable &  Trail::getMaterial() const { return getTemplate()->getMaterial(); }
}

#endif // _ITF_TRAIL_H_

