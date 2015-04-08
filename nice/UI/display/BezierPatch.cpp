#include "precompiled_engine.h"

#ifndef _ITF_BEZIERPATCH_H_
#include "engine/display/BezierPatch.h"
#endif //_ITF_BEZIERPATCH_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef ITF_GFX_MATERIAL_H_
#include "engine/display/material/GFXMaterial.h"
#endif //ITF_GFX_MATERIAL_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    #define MAX_BEZIER_PATCH_BY_SHADER 24

    //*****************************************************************************

    void BezierPatchContainer::clearList()
    {
        m_vdivToDrawList.clear();
        m_shaderList.clear();
    }

    void BezierPatchContainer::addBezierPatchToList( const BezierPatchParams & _bezierParams )
    {
        if (m_vdivToDrawList.size() >= m_shaderList.size() * MAX_BEZIER_PATCH_BY_SHADER)
        {
            m_shaderList.push_back(GFXAdapter_shaderBuffer());
            m_shaderList.back().clear();
        }
        ITF_ASSERT_MSG(_bezierParams.m_vdivToDraw <= MESH_DEFAULT_VDIV, " Too many subdivisions for bezier " );
        m_vdivToDrawList.push_back(_bezierParams.m_vdivToDraw);
        GFXAdapter_shaderBuffer & currentShader = m_shaderList.back();

        /// The stride of float Buffer is:
        /// Vector2 PointsNS for patch Vertex, Vector2 PointsNS + 8 that is for UV.
        /// this is for Vector4 shader: xy = vertex zw = uv.
        // Compute UVs

        #define ADD_BEZIER_PARAM(idx, val) \
            currentShader.setBufferFloatAt(idx, currentShader.getCurBuffFloatPos(idx), val); \
            currentShader.addCurBuffFloatPos(idx, 1);

        ADD_BEZIER_PARAM(0, _bezierParams.m_p0.x());    // 0 - 0
        ADD_BEZIER_PARAM(0, _bezierParams.m_p0.y());    // 0 - 1
        ADD_BEZIER_PARAM(0, _bezierParams.m_uvX0);      // 0 - 2
        ADD_BEZIER_PARAM(0, _bezierParams.m_width0);    // 0 - 3

        ADD_BEZIER_PARAM(0, _bezierParams.m_p1.x());    // 0 - 4
        ADD_BEZIER_PARAM(0, _bezierParams.m_p1.y());    // 0 - 5
        ADD_BEZIER_PARAM(0, _bezierParams.m_uvX1);      // 0 - 6
        ADD_BEZIER_PARAM(0, _bezierParams.m_width1);    // 0 - 7

        ADD_BEZIER_PARAM(0, _bezierParams.m_p2.x());    // 0 - 8
        ADD_BEZIER_PARAM(0, _bezierParams.m_p2.y());    // 0 - 9
        ADD_BEZIER_PARAM(0, _bezierParams.m_uvX2);      // 0 - 10
        ADD_BEZIER_PARAM(0, _bezierParams.m_width2);    // 0 - 11

        ADD_BEZIER_PARAM(0, _bezierParams.m_p3.x());    // 0 - 12
        ADD_BEZIER_PARAM(0, _bezierParams.m_p3.y());    // 0 - 13
        ADD_BEZIER_PARAM(0, _bezierParams.m_uvX3);      // 0 - 14
        ADD_BEZIER_PARAM(0, _bezierParams.m_width3);    // 0 - 15
        /// number constant for control points is: f32 to vector4 -> 32 float / 4
        currentShader.addVector4ConstToFlush(0, 4);

        /// Widths.
        ADD_BEZIER_PARAM(0, _bezierParams.m_uvY);       // 1 - 8
        ADD_BEZIER_PARAM(0, _bezierParams.m_uvWidth);   // 1 - 9
        ADD_BEZIER_PARAM(0, 0.f);                       // 1 - 10
        ADD_BEZIER_PARAM(0, 0.f);                       // 1 - 11
        currentShader.addVector4ConstToFlush(0, 1);

        /// color.
        ADD_BEZIER_PARAM(1, _bezierParams.m_startColor.getRed());      // 1 - 0
        ADD_BEZIER_PARAM(1, _bezierParams.m_startColor.getGreen());    // 1 - 1
        ADD_BEZIER_PARAM(1, _bezierParams.m_startColor.getBlue());     // 1 - 2
        ADD_BEZIER_PARAM(1, _bezierParams.m_startColor.getAlpha());    // 1 - 3
        
        ADD_BEZIER_PARAM(1, _bezierParams.m_endColor.getRed());        // 1 - 4
        ADD_BEZIER_PARAM(1, _bezierParams.m_endColor.getGreen());      // 1 - 5
        ADD_BEZIER_PARAM(1, _bezierParams.m_endColor.getBlue());       // 1 - 6
        ADD_BEZIER_PARAM(1, _bezierParams.m_endColor.getAlpha());      // 1 - 7
        currentShader.addVector4ConstToFlush(1, 2);
    }

    bbool BezierPatchParams::computeMidUvWidthValues()
    {
        // cheap bezier length integration
        const f32 n0 = (m_p1 - m_p0).norm();
        const f32 n1 = (m_p2 - m_p1).norm();
        const f32 n2 = (m_p3 - m_p2).norm();
        if (n0 + n1 + n2 < MTH_EPSILON)
        {
            return bfalse;
        }

        const f32 invNorm = f32_Inv(n0 + n1 + n2);

        const f32 uvRatio = (m_uvX3 - m_uvX0) * invNorm;
        m_uvX1 = m_uvX0 + n0 * uvRatio;
        m_uvX2 = m_uvX3 - n2 * uvRatio;

        const f32 widthRatio = (m_width3 - m_width0) * invNorm;
        m_width1 = m_width0 + n0 * widthRatio;
        m_width2 = m_width3 - n2 * widthRatio;

        return btrue;
    }

    void BezierPatchParams::setUVs( const Vec2d& _startUV, const Vec2d& _endUV, const bbool _rotate )
    {
        if (_rotate)
        {
            m_uvX0    = _endUV.y();
            m_uvX3    = _startUV.y();

            m_uvY     = (_startUV.x() + _endUV.x()) * 0.5f;
            m_uvWidth = _endUV.x() - _startUV.x();
        }
        else
        {
            m_uvX0    = _startUV.x();
            m_uvX3    = _endUV.x();

            m_uvY     = (_startUV.y() + _endUV.y()) * 0.5f;
            m_uvWidth = _endUV.y() - _startUV.y();
        }
    }

    void BezierPatchContainer::addBezierPatchFadeToList( BezierPatchFadeParams & _bezierParams )
    {
        _bezierParams.setUVs(_bezierParams.m_startUV, _bezierParams.m_endUV, m_rotate);

        _bezierParams.m_width0    = _bezierParams.m_startWidth;
        _bezierParams.m_width3    = _bezierParams.m_endWidth;

        if ( !_bezierParams.computeMidUvWidthValues() )
        {
            return;
        }


        if ( f32_Abs(_bezierParams.m_limitA - 0.f) < MTH_EPSILON &&
             f32_Abs(_bezierParams.m_limitB - 1.f) < MTH_EPSILON )
        {
            addBezierPatchToList(_bezierParams);
            return;
        }

        // Early exit -> must be changed !!
        if (_bezierParams.m_limitA > _bezierParams.m_limitB)
        {
            ITF_WARNING(NULL, 0, "BezierPatch: wrong alpha limits");
            _bezierParams.m_startColor  = Color::red();
            _bezierParams.m_endColor    = Color::red();
            addBezierPatchToList( _bezierParams );
            return;
        }

        // need to create one struct per part
        BezierPatchParams bTmp;
        BezierPatchParams bMiddle = _bezierParams;
        f32 limitA = _bezierParams.m_limitA;
        f32 limitB = _bezierParams.m_limitB;

        if (limitA > MTH_EPSILON)
        {
            Bezier4<Vec2d> bezierP( bMiddle.m_p0,
                                    bMiddle.m_p1,
                                    bMiddle.m_p2,
                                    bMiddle.m_p3);
            Bezier4<f32>   bezierUv(bMiddle.m_uvX0,
                                    bMiddle.m_uvX1,
                                    bMiddle.m_uvX2,
                                    bMiddle.m_uvX3);
            Bezier4<f32>   bezierW( bMiddle.m_width0,
                                    bMiddle.m_width1,
                                    bMiddle.m_width2,
                                    bMiddle.m_width3);

            Bezier4<Vec2d>  bezierPLeft , bezierPRight;
            Bezier4<f32>    bezierUvLeft, bezierUvRight;
            Bezier4<f32>    bezierWLeft , bezierWRight;
            
            bezierP.split (limitA , bezierPLeft, bezierPRight);
            bezierUv.split(limitA, bezierUvLeft, bezierUvRight);
            bezierW.split (limitA, bezierWLeft, bezierWRight);

            bTmp.m_p0       = bezierPLeft.m_p0;
            bTmp.m_uvX0     = bezierUvLeft.m_p0;
            bTmp.m_width0   = _bezierParams.m_widthA;

            bTmp.m_p1       = bezierPLeft.m_p1;
            bTmp.m_uvX1     = bezierUvLeft.m_p1;
            bTmp.m_width1   = (_bezierParams.m_widthA + bMiddle.m_width0)*0.5f;

            bTmp.m_p2       = bezierPLeft.m_p2;
            bTmp.m_uvX2     = bezierUvLeft.m_p2;
            bTmp.m_width2   = (_bezierParams.m_widthA + bMiddle.m_width0)*0.5f;

            bTmp.m_p3       = bezierPLeft.m_p3;
            bTmp.m_uvX3     = bezierUvLeft.m_p3;
            bTmp.m_width3   = bMiddle.m_width0;

            bTmp.m_startColor       = bMiddle.m_startColor;
            bTmp.m_startColor.m_a   = _bezierParams.m_alphaA;
            bTmp.m_endColor         = bMiddle.m_startColor;

            bTmp.m_uvWidth    = bMiddle.m_uvWidth;
            bTmp.m_uvY        = bMiddle.m_uvY;

            if (bMiddle.m_vdivToDraw != U32_INVALID)
                bTmp.m_vdivToDraw = (u32)(limitA * bMiddle.m_vdivToDraw) + 1;
            else
                bTmp.m_vdivToDraw = U32_INVALID;
            addBezierPatchToList( bTmp );


            bMiddle.m_p0       = bezierPRight.m_p0;
            bMiddle.m_uvX0     = bezierUvRight.m_p0;

            bMiddle.m_p1       = bezierPRight.m_p1;
            bMiddle.m_uvX1     = bezierUvRight.m_p1;

            bMiddle.m_p2       = bezierPRight.m_p2;
            bMiddle.m_uvX2     = bezierUvRight.m_p2;

            bMiddle.m_p3       = bezierPRight.m_p3;
            bMiddle.m_uvX3     = bezierUvRight.m_p3;

            if (bMiddle.m_vdivToDraw != U32_INVALID)
                bMiddle.m_vdivToDraw = (u32)((1.f-limitA) * bMiddle.m_vdivToDraw) + 1;

            limitB = (limitB - limitA) / (1.f - limitA);
        }

        if (limitB < 1.f - MTH_EPSILON)
        {
            Bezier4<Vec2d> bezierP( bMiddle.m_p0,
                                    bMiddle.m_p1,
                                    bMiddle.m_p2,
                                    bMiddle.m_p3);
            Bezier4<f32>   bezierUv(bMiddle.m_uvX0,
                                    bMiddle.m_uvX1,
                                    bMiddle.m_uvX2,
                                    bMiddle.m_uvX3);
            Bezier4<f32>   bezierW( bMiddle.m_width0,
                                    bMiddle.m_width1,
                                    bMiddle.m_width2,
                                    bMiddle.m_width3);

            Bezier4<Vec2d>  bezierPLeft , bezierPRight;
            Bezier4<f32>    bezierUvLeft, bezierUvRight;
            Bezier4<f32>    bezierWLeft , bezierWRight;

            bezierP.split (limitB, bezierPLeft, bezierPRight);
            bezierUv.split(limitB, bezierUvLeft, bezierUvRight);
            bezierW.split (limitB, bezierWLeft, bezierWRight);

            bTmp.m_p0       = bezierPRight.m_p0;
            bTmp.m_uvX0     = bezierUvRight.m_p0;
            bTmp.m_width0   = bMiddle.m_width3;

            bTmp.m_p1       = bezierPRight.m_p1;
            bTmp.m_uvX1     = bezierUvRight.m_p1;
            bTmp.m_width1   = (bMiddle.m_width3 + _bezierParams.m_widthB)*0.5f;

            bTmp.m_p2       = bezierPRight.m_p2;
            bTmp.m_uvX2     = bezierUvRight.m_p2;
            bTmp.m_width2   = (bMiddle.m_width3 + _bezierParams.m_widthB)*0.5f;

            bTmp.m_p3       = bezierPRight.m_p3;
            bTmp.m_uvX3     = bezierUvRight.m_p3;
            bTmp.m_width3   = _bezierParams.m_widthB;


            bTmp.m_startColor       = bMiddle.m_endColor;
            bTmp.m_endColor         = bMiddle.m_endColor;
            bTmp.m_endColor.m_a     = _bezierParams.m_alphaB;

            if (bMiddle.m_vdivToDraw != U32_INVALID)
                bTmp.m_vdivToDraw = (u32)((1.f-limitB) * bMiddle.m_vdivToDraw) + 1;
            else
                bTmp.m_vdivToDraw = U32_INVALID;
            addBezierPatchToList( bTmp );


            bMiddle.m_p0       = bezierPLeft.m_p0;
            bMiddle.m_uvX0     = bezierUvLeft.m_p0;

            bMiddle.m_p1       = bezierPLeft.m_p1;
            bMiddle.m_uvX1     = bezierUvLeft.m_p1;

            bMiddle.m_p2       = bezierPLeft.m_p2;
            bMiddle.m_uvX2     = bezierUvLeft.m_p2;

            bMiddle.m_p3       = bezierPLeft.m_p3;
            bMiddle.m_uvX3     = bezierUvLeft.m_p3;

            if (bMiddle.m_vdivToDraw != U32_INVALID)
                bMiddle.m_vdivToDraw = (u32)(limitB * bMiddle.m_vdivToDraw) + 1;
        }

        addBezierPatchToList( bMiddle );
    }

    void BezierPatchContainer::DrawShaderList(PrimitiveContext&  _primCtx)
    {
        u32 shaderListSize = m_shaderList.size();
        if (shaderListSize == 0 || m_vdivToDrawList.size() == 0)
            return;

        DrawCallContext drawcallCtx = DrawCallContext(&_primCtx);

        GFX_ADAPTER->setGfxMaterial(drawcallCtx, *m_material);

        Matrix44 identity;
        identity.setIdentity();

        GFX_ADAPTER->prepareShaderBezierPatch(_primCtx, &identity, m_z, m_hdiv, m_vdiv);
        
        //DrawCallContext drawcallCtx = DrawCallContext(&_primCtx);

        //GFX_ADAPTER->setGfxMaterial(drawcallCtx, *m_material);

        if (m_vdiv == MESH_DEFAULT_VDIV)
        {
            for (u32 i=0; i<shaderListSize; i++)
            {
                GFX_ADAPTER->shaderBezierPatchSendBuffer(drawcallCtx, &(m_shaderList[i]), *m_material, m_hdiv, m_vdiv, m_rotate, &(m_vdivToDrawList[0]) + i*MAX_BEZIER_PATCH_BY_SHADER);
            }
        } else
        {
            for (u32 i=0; i<shaderListSize; i++)
            {
                GFX_ADAPTER->shaderBezierPatchSendBuffer(drawcallCtx, &(m_shaderList[i]), *m_material, m_hdiv, m_vdiv, m_rotate, NULL);
            }
        }
    }

    //*****************************************************************************
    //*****************************************************************************

    void BezierPatchContainer::getPosAndNormalExtruded(
        const Bezier4<Vec2d>& _bezier, const f32 _t, const f32 _halfWidth,
        Vec2d& _pos, Vec2d& _normal ) const
    {
        _pos = _bezier.getPos(_t);

        const Vec2d tan = _bezier.getTangent(_t);
        _normal = tan.getPerpendicular().normalize() * _halfWidth;
    }

    void BezierPatchContainer::computeShaderAABB( AABB& _aabb, u32 _shaderIndex, u32 _elementInShader ) const
    {
        const GFXAdapter_shaderBuffer & shader = m_shaderList[_shaderIndex];
        const f32 * startBuffer0 = shader.getBufferFloatAt(0, _elementInShader*4*4);
        const f32 * startBuffer1 = shader.getBufferFloatAt(1, _elementInShader*5*4);

        const f32 startHalfWidth = /*m_widths[0]*/startBuffer1[8] * 0.5f;
        const f32 endHalfWidth   = /*m_widths[1]*/startBuffer1[9] * 0.5f;
        // for the 2 mid points, getting the max should be cheaper than interpolating...
        const f32 maxHalfWidth = Max(/*m_widths[0] */startBuffer1[8],
                                 Max(/*m_widths2[2]*/startBuffer1[18], 
                                 Max(/*m_widths2[3]*/startBuffer1[19], 
                                     /*m_widths[1] */startBuffer1[9]))) * 0.5f;


        // we sample at 0, 0.333, 0.667, and 1
        Bezier4<Vec2d> bezier(  /*m_pointsAndUVs[0]*/Vec2d(startBuffer0[0],  startBuffer0[1]),
                                /*m_pointsAndUVs[1]*/Vec2d(startBuffer0[4],  startBuffer0[5]),
                                /*m_pointsAndUVs[2]*/Vec2d(startBuffer0[8],  startBuffer0[9]),
                                /*m_pointsAndUVs[3]*/Vec2d(startBuffer0[12], startBuffer0[13]));
        Vec2d pos, normal;

        getPosAndNormalExtruded(bezier, 0.f, startHalfWidth, pos, normal);
        _aabb = AABB(pos + normal);
        _aabb.grow(pos - normal);

        getPosAndNormalExtruded(bezier, 0.333f, maxHalfWidth, pos, normal);
        _aabb.grow(pos + normal);
        _aabb.grow(pos - normal);

        getPosAndNormalExtruded(bezier, 0.667f, maxHalfWidth, pos, normal);
        _aabb.grow(pos + normal);
        _aabb.grow(pos - normal);

        getPosAndNormalExtruded(bezier, 1.f, endHalfWidth, pos, normal);
        _aabb.grow(pos + normal);
        _aabb.grow(pos - normal);
    }


    void BezierPatchContainer::computeAABB( AABB& _aabb ) const
    {
        u32 nbPatchs = m_vdivToDrawList.size();
        if (m_shaderList.size() == 0 || nbPatchs == 0)
            return;

        computeShaderAABB(_aabb, 0, 0);
        u32 currentShader   = 0;
        u32 currentPos      = 1;
        for (u32 i=1; i<nbPatchs; i++, currentPos++)
        {
            if (currentPos >= MAX_BEZIER_PATCH_BY_SHADER)
            {
                currentShader++;
                currentPos = 0;
            }
            AABB aabb;
            computeShaderAABB(aabb, currentShader, currentPos);
            _aabb.grow(aabb);
        }
    }


    //*****************************************************************************
    //*****************************************************************************

    void BezierPatchContainer::batchPrimitives( const ITF_VECTOR <class View*>& _views, const ConstObjectRef & _objectRef)
    {
        // Push the primitive even if it's empty, because it could be filled during a job.
        // We check the size in DrawShaderList anyway.

        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, this, m_z + m_depthOffset, _objectRef);
    }

    void BezierPatchContainer::directDraw(const class RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z, f32 _ZWorld, f32 _ZAbs)
    {
        PRF_M_SCOPE(drawBezierPatch);

        PrimitiveContext primCtx;
        primCtx.setRenderPassContext((RenderPassContext*)&_rdrPassCtxt)
            .setPrimitiveParam(getCommonParam())
            .setCamZ(_Z,_ZWorld, _ZAbs);

        _gfxDeviceAdapter->setGFXPrimitiveParameters(getCommonParam(), _rdrPassCtxt);
        DrawShaderList(primCtx);
    }

    void BezierPatchContainer::onResourceLoaded()
    {
    }

    void BezierPatchContainer::onUnloadResources()
    {
        // GFXAdapter_shaderBuffer is rather big so save some memory here
        m_shaderList.free();
    }

    void BezierPatchContainer::setDivMode( const BezierDivMode _mode )
    {
        switch (_mode)
        {
        case BezierDivMode_Adaptive4:
            {
                m_hdiv = MESH_DEFAULT_HDIV;
                m_vdiv = MESH_DEFAULT_VDIV;
            }
            break;

        case BezierDivMode_Adaptive2:
            {
                m_hdiv = 2;
                m_vdiv = MESH_DEFAULT_VDIV;
            }
            break;

        case BezierDivMode_Adaptive1:
            {
                m_hdiv = 1;
                m_vdiv = MESH_DEFAULT_VDIV;
            }
            break;

        case BezierDivMode_Fix82:
            {
                m_hdiv = 2;
                m_vdiv = 8;
            }
            break;

        case BezierDivMode_Fix22:
        case BezierDivMode_Fix44:
            {
                m_hdiv = (u32)_mode;
                m_vdiv = (u32)_mode;
            }
            break;
        }
    }

}
