#include "precompiled_engine.h"

#ifndef _ITF_FRUSTUM_H_
#include "engine/display/Frustum.h"
#endif //_ITF_FRUSTUM_H_

namespace ITF
{

#if defined(ASSERT_ENABLED) 
    static ITF_INLINE bbool isNormalized(const Vec3d & _v)
    {
        f32 sqrNorm = _v.sqrnorm();
        return F32_ALMOST_EQUALS(sqrNorm, 1.0f, MTH_EPSILON);
    }
#endif

    void FrustumDesc::buildFrustum( FrustumDesc& _frustum, const Vec3d& _p, const Vec3d& _dir, const Vec3d& _up, f32 _focale, f32 _ratio, f32 _near, f32 _far )
    {
        ITF_ASSERT(_focale<MTH_PI);

        const f32 zfar = _far;
        
        f32 nw, nh, fw, fh, tang;
        tang = tanf(_focale * 0.5f);
        nh = _near * tang;
        nw = nh * _ratio; 
        fh = zfar  * tang;
        fw = fh * _ratio;

        Vec3d nc,fc,P;
        Vec3d ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;

        nc = _p + _dir * _near;
        fc = _p + _dir * zfar;
        
        Vec3d right;
        _dir.crossProduct(_up, right);
        right.normalize();

        ntl = nc + _up * nh - right * nw;
        ntr = nc + _up * nh + right * nw;
        nbl = nc - _up * nh - right * nw;
        nbr = nc - _up * nh + right * nw;
        ftl = fc + _up * fh - right * fw;
        ftr = fc + _up * fh + right * fw;
        fbl = fc - _up * fh - right * fw;
        fbr = fc - _up * fh + right * fw;

        _frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Top].computeFromPoints(ntl, ftr, ftl);
        _frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Bottom].computeFromPoints(nbl, fbl, fbr);        
        _frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Left].computeFromPoints(ntl, ftl, fbl);
        _frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Right].computeFromPoints(ntr, fbr, ftr);        
        _frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Near].computeFromPoints(ntl, nbl, ntr);

        ITF_ASSERT(isNormalized(_frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Top].m_normal));
        ITF_ASSERT(isNormalized(_frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Bottom].m_normal));
        ITF_ASSERT(isNormalized(_frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Left].m_normal));
        ITF_ASSERT(isNormalized(_frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Right].m_normal));
        ITF_ASSERT(isNormalized(_frustum.m_frustumPlanes[FrustumDesc::CameraPlane_Near].m_normal));
	}

} // namespace ITF
