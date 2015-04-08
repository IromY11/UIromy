#include "precompiled_engine.h"

#ifndef _ITF_TRAIL_H_
#include "engine/display/Trail.h"
#endif //_ITF_TRAIL_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_


namespace ITF
{
    BEGIN_SERIALIZATION(Trail)
    END_SERIALIZATION()


    Trail::Trail()
    : m_template(NULL)
    , m_hdiv(MESH_DEFAULT_HDIV)
    , m_vdiv(MESH_DEFAULT_VDIV)
    , m_color(Color::white())
    , m_thicknessScale(1)
    , m_is2D(bfalse)
    , m_alphaMultiplier(1)
    , m_trailFaidingValue(0.f)
    , m_currTrailLenght(0)
    , m_currTrailBegin(U32_INVALID)
    , m_useLife(bfalse)
    , m_useSetColor(bfalse)
    , m_setColor(Color::white())
	, m_active(btrue)
#ifdef TRAIL_DBG
    , m_dbgDrawFeedPoints(1)
#endif // TRAIL_DBG
    {
    }


    Trail::~Trail()
    {
    }

    void Trail::initData()
    {
        const u32 nbFrames = getTemplate()->getNBFrames();

        m_color = getTemplate()->getInitColor();
        m_trailPointTab.resize(nbFrames);
        for (u32 trailCount=0; trailCount < nbFrames; ++trailCount)
            m_trailPointTab[trailCount].m_life = 0;
    }

    void Trail::setColor( const Color& _color )
    {
        m_useSetColor = btrue;
        m_setColor = _color;
        m_color = m_setColor;
    }

    void Trail::resetSetColor()
    {
        m_useSetColor = bfalse;
        m_color = getTemplate()->getInitColor();
    }

    void Trail::multiplyColor( const Color& _color )
    {
        m_color = ( m_useSetColor ? m_setColor : getTemplate()->getInitColor() ) * _color;
    }

    void Trail::startFade() 
    { 
        m_trailFaidingValue = getTemplate()->getTrailFadingTime();
    } 

    void Trail::clear()
    {
        m_currTrailLenght = 0;
        u32 count = m_trailPointTab.size();
        for (u32 trailCount=0; trailCount < count; ++trailCount)
            m_trailPointTab[trailCount].m_life = 0;
        m_color = getTemplate()->getInitColor();
    }

    void Trail::addPoint(const Vec3d &_pos, f32 _angle)
    {
        const u32 nbFrames = getTemplate()->getNBFrames();
        if ( !nbFrames)
            return;
        
        if(   (getTemplate()->getTesselateMaxLen() != MTH_HUGE)
           && (getTemplate()->getTesselateMaxLen() > MTH_EPSILON)
           && (m_currTrailLenght>=3))
        {
            u32 id0, id1;
            if(m_currTrailBegin >= 1)
                id1 = (m_currTrailBegin-1) % nbFrames;
            else
                id1 = nbFrames-1;

            if(id1 >= 1)
                id0 = (id1-1) % nbFrames;
            else
                id0 = nbFrames-1;

            const Vec3d& p0 = m_trailPointTab[id0].m_pos;
            const Vec3d  p1 = m_trailPointTab[id1].m_pos;
            const Vec3d  p2 = m_trailPointTab[m_currTrailBegin].m_pos;
            const Vec3d& p3 = _pos;

            // Check length between p1 and p2
            Vec3d v = p2 - p1; 
            f32 d = v.norm();
            if(d > getTemplate()->getTesselateMaxLen())
            {
                // remove last point
                m_currTrailBegin = id1;
                m_currTrailLenght--;

                // add points
                d /= getTemplate()->getTesselateMaxLen();
                f32 tinc = f32_Inv(d+1.f);
                for(f32 t = 0.f + tinc; t < 1.f; t+=tinc)
                {
                    v = Spline::Eq(t, p0, p1, p2, p3);
                    addPointPriv(v, 0.f);
                }

                // replace removed point
                addPointPriv(p2, 0.f);
            }

            addPointPriv(_pos, _angle);
            return;
        }

        if(   (getTemplate()->getTesselateMinLen() > 0.f)
           && (m_currTrailLenght>=2))
        {
            u32 iprev;
            if(m_currTrailBegin >= 1)
                iprev = (m_currTrailBegin-1) % nbFrames;
            else
                iprev = nbFrames-1;
            const Vec3d&  prev = m_trailPointTab[iprev].m_pos;
            Vec3d v = prev - _pos; 
            f32 d = v.norm();
            if(d<getTemplate()->getTesselateMinLen())
            { // replace last point
                m_currTrailBegin = iprev;
                m_currTrailLenght--;
            }
        }

        addPointPriv(_pos, _angle);
    }

    bbool Trail::removeFirstPoint()
    {
        const u32 nbFrames = getTemplate()->getNBFrames();
        if ( !nbFrames)
            return bfalse;

        if(!m_currTrailLenght)
            return bfalse;

        m_currTrailLenght--;
        return btrue;
    }

    bbool Trail::removeLastPoint()
    {
        const u32 nbFrames = getTemplate()->getNBFrames();
        if ( !nbFrames)
            return bfalse;

        if(!m_currTrailLenght)
            return bfalse;

        u32 prev = (m_currTrailBegin+nbFrames-1) % nbFrames;

        m_currTrailBegin = prev;
        m_currTrailLenght--;

        return btrue;
    }

    void Trail::addPointPriv(const Vec3d &_pos, f32 _angle)
    {
        const u32 nbFrames = getTemplate()->getNBFrames();
        if ( !nbFrames)
            return;
       
        f32 screenHeight = (f32)GFX_ADAPTER->getScreenHeight();
        f32 scale = m_is2D?screenHeight:1;
        scale *= m_thicknessScale;

        TrailPoint *pTrailPoint;
        Vec2d   dir;
        u32 prevTrail3DBegin;

        prevTrail3DBegin    = m_currTrailBegin;
        m_currTrailBegin    = (m_currTrailBegin + 1) % nbFrames;
        if (m_currTrailLenght < nbFrames)
            m_currTrailLenght++;

        pTrailPoint = &m_trailPointTab[m_currTrailBegin];
        pTrailPoint->m_pos         = _pos;
        pTrailPoint->m_life        = nbFrames;
        if (m_currTrailLenght == 1)
        {
            Vec2d::CosSin(&dir, _angle);
            pTrailPoint->m_norm = dir.getPerpendicular();
        } else
        {
            dir                 = (pTrailPoint->m_pos - m_trailPointTab[prevTrail3DBegin].m_pos).truncateTo2D();
            pTrailPoint->m_norm = dir.getPerpendicular();
            if (pTrailPoint->m_norm.sqrnorm() < MTH_BIG_EPSILON)
            {
                //pTrailPoint->m_norm = m_trailPointTab[prevTrail3DBegin].m_norm;
                removeLastPoint(); // skip this unnecessary point
                //removeFirstPoint(); // remove first in point.
                return;
            }
            else
                pTrailPoint->m_norm.normalize();
            f32 t = f32_Clamp(getTemplate()->getTrailBlending(), 0.f, 1.f);
            Vec2d::Lerp(&pTrailPoint->m_norm, &pTrailPoint->m_norm, &m_trailPointTab[prevTrail3DBegin].m_norm, t);
            if (pTrailPoint->m_norm.sqrnorm() < MTH_EPSILON)
                pTrailPoint->m_norm= m_trailPointTab[prevTrail3DBegin].m_norm;
            if (pTrailPoint->m_norm.dot(m_trailPointTab[prevTrail3DBegin].m_norm) < 0)
            {
                pTrailPoint->m_norm *= -1.f;
                pTrailPoint->m_norm.normalize();
                m_trailPointTab[prevTrail3DBegin].m_norm = pTrailPoint->m_norm;
            }
        }
        pTrailPoint->m_norm.normalize();

        pTrailPoint->m_thickness   = scale * getTemplate()->getThicknessBegin();
        pTrailPoint->m_alpha       = getTemplate()->getAlphaBegin();

        pTrailPoint->m_uv1         = Vec2d::Zero;
        pTrailPoint->m_uv2         = Vec2d(0.f, 1.f);
    }

    bbool Trail::fade( f32 _deltaTime )
    {
        m_trailFaidingValue -= _deltaTime;
        f32 alphaFaid        = m_trailFaidingValue/getTemplate()->getTrailFadingTime();

        if (alphaFaid < 0.f)
            return bfalse;

        multiplyColor(Color(alphaFaid,1.f,1.f,1.f));
        return btrue;
    }

    void Trail::update()
    {
        if (m_currTrailLenght)
        {
            const u32 nbFrames = getTemplate()->getNBFrames();
            TrailPoint *pTrailPoint;
            f32             ratio   = 1.f/(getTemplate()->getFixTrailLenght() ? (f32)getTemplate()->getFixTrailLenght() : (f32)m_currTrailLenght);
            f32 thicknessFrameRatio = (getTemplate()->getThicknessEnd() - getTemplate()->getThicknessBegin())*ratio;
            f32 alphaFrameRatio     = (getTemplate()->getAlphaEnd() - getTemplate()->getAlphaBegin())*ratio;

            f32 screenHeight = (f32)GFX_ADAPTER->getScreenHeight();
            f32 scale = m_is2D?screenHeight:1;
            scale *= m_thicknessScale;
            u32 trailPos        = (m_currTrailBegin + nbFrames - 1) % nbFrames;
            for (f32 trailCount=1.0f; trailCount < m_currTrailLenght; ++trailCount)
            {
                pTrailPoint = &m_trailPointTab[trailPos];
                if (pTrailPoint->m_life <= 0)
                    break;

                if (m_useLife)
                    pTrailPoint->m_life--;
                pTrailPoint->m_thickness = scale * getTemplate()->getThicknessBegin() + thicknessFrameRatio*trailCount;
                pTrailPoint->m_alpha     = getTemplate()->getAlphaBegin()     + alphaFrameRatio*trailCount;
                pTrailPoint->m_uv1.x()   = ratio*trailCount;
                pTrailPoint->m_uv2.x()   = ratio*trailCount;

                trailPos = (trailPos + nbFrames - 1) % nbFrames;
            }
        }
    }

    void Trail::updateAABB(AABB &_aabb)
    {
        const u32 nbFrames = getTemplate()->getNBFrames();

        u32 trailPos = m_currTrailBegin;
        for (u32 trailCount=0 ; trailCount<m_currTrailLenght ; ++trailCount)
        {
            TrailPoint *pTrailPoint = &m_trailPointTab[trailCount];
            if (pTrailPoint->m_life > 0)
            {
                Vec2d pos2D = pTrailPoint->m_pos.truncateTo2D();
                _aabb.grow(pos2D - pTrailPoint->m_norm*(pTrailPoint->m_thickness*0.5f));
                _aabb.grow(pos2D + pTrailPoint->m_norm*(pTrailPoint->m_thickness*0.5f));
            }

            trailPos = (trailPos + nbFrames - 1) % nbFrames;
        }
    }

    void Trail::directDraw(const RenderPassContext & _rdrPassCtxt, GFXAdapter * _gfxDeviceAdapter, f32 _Z, f32 _ZWorld, f32 _ZAbs)
    {
        if (m_currTrailLenght < 2)
            return;

        PRF_M_SCOPE(drawTrail);

#ifdef TRAIL_DBG
        bbool allowDebugDraw = bfalse;
        switch(_rdrPassCtxt.getPass())
        {
            case GFX_ZLIST_PASS_REGULAR:
            case GFX_ZLIST_PASS_2D:
                allowDebugDraw = View::getCurrentView()->isMainView();
                break;
        }
#endif // TRAIL_DBG

        f32 screenHeight = getX(rotLeft(_rdrPassCtxt.getRenderTargetSize()));

        u32 trailCount = 0;
        m_shaderPointBuffer.reserve(m_currTrailLenght*2);
        m_shaderPointBuffer.resize(0);
        const u32 nbFrames = getTemplate()->getNBFrames();

        Vec3d p1,p2,n;
        n.z() = 0.f;
        for (u32 trailPos = m_currTrailBegin;
            trailCount < m_currTrailLenght;
            trailPos = (trailPos + nbFrames - 1) % nbFrames, trailCount++)
        {
#ifdef TRAIL_DBG
            char msgT[64];
            sprintf(msgT,"%u", trailCount);
            if(allowDebugDraw && m_dbgDrawFeedPoints)
            {
                DebugDraw::circle(*(Vec2d*)&m_trailPointTab[trailPos].m_pos, 0.f, 0.025f, (m_trailPointTab[trailPos].m_life <= 0) ? Color::red() : Color::green(), 0.f, msgT);
            }
#endif // TRAIL_DBG

            if (m_trailPointTab[trailPos].m_life <= 0)
                break;

            TrailPoint &trail = m_trailPointTab[trailPos];
            n.setFromVec2d(trail.m_norm*(trail.m_thickness*0.5f));
            p1 = trail.m_pos - n;
            p2 = trail.m_pos + n;

            if (m_is2D)
            {
                p1.y() = screenHeight-p1.y();
                p2.y() = screenHeight-p2.y();
            }

            m_shaderPointBuffer.push_back(p1);
            m_shaderPointBuffer.push_back(p2);
        }

        Color c(m_color);
        c.m_a *= m_alphaMultiplier;
        _gfxDeviceAdapter->setGFXPrimitiveParameters(getCommonParam(), _rdrPassCtxt);
        PrimitiveContext primCtx;
        primCtx.setRenderPassContext((RenderPassContext*)&_rdrPassCtxt)
            .setPrimitiveParam(getCommonParam())
            .setCamZ(_Z,_ZWorld, _ZAbs);

        _gfxDeviceAdapter->drawTrail3D(primCtx, &getMaterial(), c, m_shaderPointBuffer, getTemplate()->getAlphaBegin(), getTemplate()->getAlphaEnd(), getTemplate()->getFadeLength());
    }


    BEGIN_SERIALIZATION(Trail_Template)

        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
            SERIALIZE_MEMBER("texture", m_material.getTexturePathSet().getTexturePath());
        END_CONDITION_BLOCK();
        SERIALIZE_OBJECT("material",m_material);
        SERIALIZE_MEMBER("nbFrames", m_nbFrames);
        SERIALIZE_MEMBER("fixTrailLenght", m_fixTrailLenght);
        SERIALIZE_MEMBER("trailFaidingTime", m_trailFadingTime);

        SERIALIZE_MEMBER("thicknessBegin",  m_thicknessBegin);
        SERIALIZE_MEMBER("thicknessEnd",    m_thicknessEnd);

        SERIALIZE_MEMBER("alphaBegin",  m_alphaBegin);
        SERIALIZE_MEMBER("alphaEnd",    m_alphaEnd);
        SERIALIZE_MEMBER("trailBlending", m_trailBlending);
        SERIALIZE_MEMBER("fadeLength", m_fadeLength);
        SERIALIZE_MEMBER("tesselateMaxLength",m_tesselateMaxLen);
        SERIALIZE_MEMBER("tesselateMinLength",m_tesselateMinLen);
        SERIALIZE_MEMBER("color", m_initColor);
        SERIALIZE_MEMBER("attachBone", m_attachBone);
    END_SERIALIZATION()

    Trail_Template::Trail_Template()
    : m_nbFrames(1)
    , m_trailFadingTime(1.f)
    , m_thicknessBegin(1.0f)
    , m_thicknessEnd(0.0f)
    , m_alphaBegin(1.0f)
    , m_alphaEnd(1.0f)
    , m_trailBlending(0.f)
    , m_fadeLength(0.f)
    , m_initColor(Color::white())
    , m_tesselateMaxLen(MTH_HUGE)
    , m_tesselateMinLen(0.f)
    , m_fixTrailLenght(0)
    {

    }
}
