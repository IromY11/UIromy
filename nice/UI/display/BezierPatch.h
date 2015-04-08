#ifndef _ITF_BEZIERPATCH_H_
#define _ITF_BEZIERPATCH_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

namespace ITF
{
    enum BezierDivMode
    {
    	BezierDivMode_Fix82 = 1,
    	BezierDivMode_Fix22 = 2,
    	BezierDivMode_Fix44 = 4,
        BezierDivMode_Adaptive1, // 1*128 mesh, with adaptive tessellation
        BezierDivMode_Adaptive2, // 2*128 mesh, with adaptive tessellation
        BezierDivMode_Adaptive4, // 4*128 mesh, with adaptive tessellation (default)
        ENUM_FORCE_SIZE_32(BezierDivMode)
    };

    #define SERIALIZE_BezierDivMode(name, val) \
    SERIALIZE_ENUM_BEGIN(name, val); \
        SERIALIZE_ENUM_VAR(BezierDivMode_Adaptive1); \
        SERIALIZE_ENUM_VAR(BezierDivMode_Adaptive2); \
        SERIALIZE_ENUM_VAR(BezierDivMode_Adaptive4); \
        SERIALIZE_ENUM_VAR(BezierDivMode_Fix82); \
        SERIALIZE_ENUM_VAR(BezierDivMode_Fix22); \
        SERIALIZE_ENUM_VAR(BezierDivMode_Fix44); \
    SERIALIZE_ENUM_END();

    //---------------------------------------------------------------------------------------------------

    class Texture;

    struct BezierPatchParams
    {
        BezierPatchParams()
            : m_p0(Vec2d::Zero)
            , m_p1(Vec2d::Zero)
            , m_p2(Vec2d::Zero)
            , m_p3(Vec2d::Zero)
            , m_uvX0(0.f)
            , m_uvX1(0.25f)
            , m_uvX2(0.75f)
            , m_uvX3(1.f)
            , m_width0(1.f)
            , m_width1(1.f)
            , m_width2(1.f)
            , m_width3(1.f)
            , m_startColor(Color::white())
            , m_endColor(Color::white())
            , m_uvY(0.5f)
            , m_uvWidth(1.f)
            , m_vdivToDraw(U32_INVALID)
        {
        }

        Vec2d m_p0;
        f32   m_uvX0;
        f32   m_width0;

        Vec2d m_p1;
        f32   m_uvX1;
        f32   m_width1;

        Vec2d m_p2;
        f32   m_uvX2;
        f32   m_width2;

        Vec2d m_p3;
        f32   m_uvX3;
        f32   m_width3;

        Color m_startColor;
        Color m_endColor;
        
        f32   m_uvY;
        f32   m_uvWidth;

        u32   m_vdivToDraw;

        bbool computeMidUvWidthValues();
        void setUVs( const Vec2d& _startUV, const Vec2d& _endUV, const bbool _rotate );
    };

    struct BezierPatchFadeParams : BezierPatchParams
    {
        BezierPatchFadeParams()
            : BezierPatchParams()
            , m_startWidth(1.f)
            , m_endWidth(1.f)
            , m_startUV(Vec2d::Zero)
            , m_endUV(Vec2d::One)
            , m_limitA(0.f)
            , m_alphaA(1.f)
            , m_widthA(1.f)
            , m_limitB(1.f)
            , m_alphaB(1.f)
            , m_widthB(1.f)
        {
        }

        f32 m_startWidth;
        f32 m_endWidth;

        Vec2d m_startUV;
        Vec2d m_endUV;

        f32 m_limitA;
        f32 m_alphaA;
        f32 m_widthA;

        f32 m_limitB;
        f32 m_alphaB;
        f32 m_widthB;
    };

    class BezierPatchContainer : public GFXPrimitive
    {
    public:
        BezierPatchContainer()
            : m_material(NULL)
            , m_z(0.f)
            , m_hdiv(MESH_DEFAULT_HDIV)
            , m_vdiv(MESH_DEFAULT_VDIV)
            , m_rotate(bfalse)
			, m_depthOffset(0.0f)
        {
        }

        void                        setDivMode( const BezierDivMode _mode );
        const u32                   getHDiv()       const { return m_hdiv; }
        void                        setHDiv( const u32 _div) { m_hdiv = _div; }

        const u32                   getVDiv()       const { return m_vdiv; }
        void                        setVDiv( const u32 _div) { m_vdiv = _div; }

        f32                         getZ()              const { return m_z; }
        void                        setZ(f32 _z)                                { m_z = _z; }

		ITF_INLINE f32 getDepthOffset() const { return m_depthOffset; }
		ITF_INLINE void setDepthOffset( const f32 _value ) { m_depthOffset = _value; }

        bbool                       getRotate()     const { return m_rotate; }
        void                        setRotate(bbool _rotate) { m_rotate = _rotate; }


        const class GFX_MATERIAL*   getMaterial()       const { return m_material; }
        void                        setMaterial( const class GFX_MATERIAL* val) { m_material = val; if (m_material) setPassFilterFlag(*m_material, getCommonParam()->m_gfxOccludeInfo); }

        void                        clearList();
        void                        addBezierPatchToList(const BezierPatchParams & _bezierParams);
        void                        addBezierPatchFadeToList(BezierPatchFadeParams & _bezierParams);
        
        void                        DrawShaderList(PrimitiveContext&  _primCtx);

        void                        computeAABB( AABB& _aabb ) const;
        void                        batchPrimitives( const ITF_VECTOR <class View*>& _views, const ConstObjectRef & _objectRef);

        void                        onResourceLoaded();
        void                        onUnloadResources();

    private:
        virtual void                directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z = 0.0f, f32 _ZWorld = 0.0f, f32 _ZAbs = 0.0f);

        void                        computeShaderAABB( AABB& _aabb, u32 _shaderIndex, u32 _elementInShader ) const;
        void                        getPosAndNormalExtruded( const Bezier4<Vec2d>& _bezier, const f32 _t, const f32 _halfWidth, Vec2d& _pos, Vec2d& _normal ) const;

        u32                             m_hdiv;
        u32                             m_vdiv;
        f32                             m_z;
        const class GFX_MATERIAL*       m_material;
        bbool                           m_rotate;
        SafeArray<u32>                  m_vdivToDrawList;
		f32								m_depthOffset;

        ITF_VECTOR<GFXAdapter_shaderBuffer>  m_shaderList;
    };
}

#endif // _ITF_BEZIERPATCH_H_
