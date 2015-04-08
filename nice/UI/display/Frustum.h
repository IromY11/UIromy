#ifndef _ITF_FRUSTUM_H_
#define _ITF_FRUSTUM_H_

#ifndef _ITF_PLANE_H_
#include "core/math/plane.h"
#endif //_ITF_PLANE_H_

#ifndef _ITF_AABB3D_H_
#include "core/boundingvolume/AABB3d.h"
#endif //_ITF_AABB3D_H_


namespace ITF
{

//////////////////////////////////////////////////////////////////////////////////////////////////
///Descriptor for frustum area
class FrustumDesc
{
public:
    enum CameraPlane
    {
        CameraPlane_Left=0,
        CameraPlane_Right,
        CameraPlane_Top,
        CameraPlane_Bottom,
        CameraPlane_Near,
        CameraPlane_Count,
        ENUM_FORCE_SIZE_32(1)
    };


    Plane m_frustumPlanes[CameraPlane_Count];

private:

// On ITF_CAFE) (and probably others out of order processors), it's better to do the test for each plane
#if defined(ITF_PS3) || defined(ITF_X360)
#define FULL_MATHEMATICAL_AABB_VISIBILITY

    static inline f32 signedDistToPlane(f32 _pnx, f32 _pny, f32 _pnz, f32 _pcst, f32 _bx, f32 _by, f32 _bz)
    {
        return _pcst - (_pnx * _bx + _pny * _by + _pnz * _bz);
    }

    static inline f32 condAABBSideOfPlane(const Plane & _plane, f32 _minX, f32 _minY, f32 _maxX, f32 _maxY, f32 _z)
    {
        f32 pnx = _plane.m_normal.x();
        f32 pny = _plane.m_normal.y();
        f32 pnz = _plane.m_normal.z();
        f32 cst = _plane.m_constant;
        f32 bx = f32_Sel(pnx, _minX, _maxX);
        f32 by = f32_Sel(pny, _minY, _maxY);
        f32 dist = signedDistToPlane(pnx, pny, pnz, cst, bx, by, _z);
        return f32_Sel( dist, 1.f, 0.f);
    }

    static inline f32 condAABBSideOfPlane(const Plane & _plane, f32 _minX, f32 _minY, f32 _minZ, f32 _maxX, f32 _maxY, f32 _maxZ)
    {
        f32 pnx = _plane.m_normal.x();
        f32 pny = _plane.m_normal.y();
        f32 pnz = _plane.m_normal.z();
        f32 cst = _plane.m_constant;
        f32 bx = f32_Sel(pnx, _minX, _maxX);
        f32 by = f32_Sel(pny, _minY, _maxY);
        f32 bz = f32_Sel(pnz, _minZ, _maxZ);
        f32 dist = signedDistToPlane(pnx, pny, pnz, cst, bx, by, bz);
        return f32_Sel( dist, 1.f, 0.f);
    }

    static ITF_FORCE_INLINE bbool is2DAABBVisible_FullMath(const Plane _planes[CameraPlane_Count], f32 _minX, f32 _minY, f32 _maxX, f32 _maxY, f32 _z)
    {
        f32 cond = condAABBSideOfPlane(_planes[CameraPlane_Left], _minX, _minY, _maxX, _maxY, _z);
        cond    *= condAABBSideOfPlane(_planes[CameraPlane_Right], _minX, _minY, _maxX, _maxY, _z);
        cond    *= condAABBSideOfPlane(_planes[CameraPlane_Top], _minX, _minY, _maxX, _maxY, _z);
        cond    *= condAABBSideOfPlane(_planes[CameraPlane_Bottom], _minX, _minY, _maxX, _maxY, _z);
        cond    *= condAABBSideOfPlane(_planes[CameraPlane_Near], _minX, _minY, _maxX, _maxY, _z);
        return cond!= 0;
    }

    static ITF_FORCE_INLINE bbool is3DAABBVisible_FullMath(const Plane _planes[CameraPlane_Count], f32 _minX, f32 _minY, f32 _minZ, f32 _maxX, f32 _maxY, f32 _maxZ)
    {
        f32 cond = condAABBSideOfPlane(_planes[CameraPlane_Left],   _minX, _minY, _minZ, _maxX, _maxY, _maxZ);
        cond    *= condAABBSideOfPlane(_planes[CameraPlane_Right],  _minX, _minY, _minZ, _maxX, _maxY, _maxZ);
        cond    *= condAABBSideOfPlane(_planes[CameraPlane_Top],    _minX, _minY, _minZ, _maxX, _maxY, _maxZ);
        cond    *= condAABBSideOfPlane(_planes[CameraPlane_Bottom], _minX, _minY, _minZ, _maxX, _maxY, _maxZ);
        cond    *= condAABBSideOfPlane(_planes[CameraPlane_Near],   _minX, _minY, _minZ, _maxX, _maxY, _maxZ);
        return cond!= 0;
    }

#endif

public:

    // All isInFrustum with aabb can wrongly report an aabb as in the frustum while it's not.
    //       -----
    //      |     |    AABB
    //       -----
    //         /\
    //        /  \     Corner of a frustum

    // from 2D aabb 
    ITF_INLINE bbool isInFrustum(const Vec2d & _min, const Vec2d & _max, float _z) const
    {
#ifdef FULL_MATHEMATICAL_AABB_VISIBILITY
        return is2DAABBVisible_FullMath(m_frustumPlanes, _min.x(), _min.y(), _max.x(), _max.y(), _z);
#else
        return isInFrustum(Vec3d(_min.x(), _min.y(), _z), Vec3d(_max.x(), _max.y(), _z));
#endif
    }

    //
    ITF_INLINE bbool isInFrustum(const Vec3d &boxMin, const Vec3d &boxMax) const
    {
#ifdef FULL_MATHEMATICAL_AABB_VISIBILITY
        return is3DAABBVisible_FullMath(m_frustumPlanes, boxMin.x(), boxMin.y(), boxMin.z(), boxMax.x(), boxMax.y(), boxMax.z());
#else
        Vec3d minExtremity;
        for (int i=0; i<CameraPlane_Count; i++)
        {
            const Plane &plane = m_frustumPlanes[i];
            Vec3d::Sel(&minExtremity, &plane.m_normal, &boxMin, &boxMax);

            if (minExtremity.dot(plane.m_normal) > plane.m_constant)
                return bfalse; //completely outside frustum
        }
        return btrue;
#endif
    }

    enum IntersectResult
    {
        INSIDE,
        OUTSIDE,
        MAY_INTERSECT,
    };

    // This method is not a 100% correct : It can wrongly report intersect for outside objects. Outside and Inside result are correct.
    ITF_INLINE IntersectResult getIntersect(const Vec3d &boxMin, const Vec3d &boxMax) const
    {
        IntersectResult result = INSIDE;

        Vec3d minExtremity;
        Vec3d maxExtremity;
        for (int i=0; i<CameraPlane_Count; i++)
        {
            const Plane &plane = m_frustumPlanes[i];

            Vec3d::Sel(&minExtremity, &plane.m_normal, &boxMin, &boxMax);
            if (minExtremity.dot(plane.m_normal) > plane.m_constant)
                return OUTSIDE; //completely outside frustum

            Vec3d::Sel(&maxExtremity, &plane.m_normal, &boxMax, &boxMin);
            if (maxExtremity.dot(plane.m_normal) >= plane.m_constant)
                result = MAY_INTERSECT;
        }
        return result;
    }

    ITF_INLINE IntersectResult getIntersect(const AABB3d &_aabb) const
    {
        return getIntersect(_aabb.getMin(), _aabb.getMax());
    }


    //
    ITF_INLINE bbool isInFrustum(const AABB3d &_aabb) const
    {
        const Vec3d &boxMin = _aabb.getMin();
        const Vec3d &boxMax = _aabb.getMax();
        return isInFrustum(boxMin, boxMax);
    }
    
    static void buildFrustum(FrustumDesc& _frustum, const Vec3d& _p, const Vec3d& _dir, const Vec3d& _up, f32 _focale, f32 _ratio, f32 _near, f32 _far = 1000.0f);

    // push planes (except near and far) by the given number of meters
    inline void enlarge(f32 _distanceToEnlarge)
    {
        m_frustumPlanes[FrustumDesc::CameraPlane_Top].m_constant += _distanceToEnlarge;
        m_frustumPlanes[FrustumDesc::CameraPlane_Bottom].m_constant += _distanceToEnlarge;
        m_frustumPlanes[FrustumDesc::CameraPlane_Left].m_constant += _distanceToEnlarge;
        m_frustumPlanes[FrustumDesc::CameraPlane_Right].m_constant += _distanceToEnlarge;
    }
};

} // namespace ITF



#endif //_ITF_FRUSTUM_H_
