#include "precompiled_engine.h"

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_BV_AABB_H_
#include "Core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef __ITF_PROJECTPARAMETERS_H__
#include "core/projects/ProjectParameters.h"
#endif //__ITF_PROJECTPARAMETERS_H__

namespace ITF
{

Camera::Camera() 
: m_focale(MTH_PIBY4)
{
	m_validyCount = 1;
    m_x = 0.0f;
    m_y = 0.0f;
    m_z = 150.f;
    m_lookAtDir = -Vec3d::ZAxis;
    m_upDir = Vec3d::YAxis;
    m_near = 1.0f;
    setNearFadeRange(0.f);
#ifdef ITF_CONSOLE
    //m_far = 100.f;
    m_far = 1000.f;
#else // ITF_CONSOLE
    m_far = 10000.f;
#endif // ITF_CONSOLE
    m_topLeft = Vec2d::Zero;
    m_bottomRight = Vec2d::One;
    m_rotateZAngle = 0.f;
    m_useRotateCam = bfalse;
    m_deltaFogZ = 0.f;
    m_flag = 0;
    m_updateFrustumEnlargeRange = Camera_DefaultUpdateFrustumEnlargeRange;
    m_loadingFrustumEnlargeRange = Camera_DefaultLoadingFrustumEnlargeRange;
	m_horizontalVersusVertical = 0.0f;
	m_fixedPoint.set( 0.5f, 0.5f );
	m_offsetHVS.set( 0.0f, 0.0f );
}


Camera::~Camera() 
{

}

//////////////////////////////////////////////////////////////////////////////
void    Camera::startFrame()
{
    // determine camera origin
    Vec2d cam(m_x, m_y);

    // create clipping vectors

    apply();
    m_rayOrigin = Vec3d(m_x, m_y, m_z);
    f32 x = m_x;
    f32 y = m_y;
    f32 z = m_z;

    const GFX_ViewportDefinition &viewport = View::getCurrentView()->getViewportDefinition();

    GFX_ADAPTER->coord2DTo3D(Vec2d(viewport.m_left, viewport.m_top + viewport.getHeight() * 0.5f), 0.f, m_Left);
    GFX_ADAPTER->coord2DTo3D(Vec2d(viewport.m_right, viewport.m_top + viewport.getHeight() * 0.5f), 0.f, m_Right);
    GFX_ADAPTER->coord2DTo3D(Vec2d(viewport.m_left + viewport.getWidth() * 0.5f, viewport.m_top), 0.f, m_Up);
    GFX_ADAPTER->coord2DTo3D(Vec2d(viewport.m_left + viewport.getWidth() * 0.5f, viewport.m_bottom), 0.f, m_Down);
    m_Left.x() = m_Left.x() * z + x;
    m_Left.y() = m_Left.y() * z +y;
    m_Right.x() = m_Right.x() * z + x;
    m_Right.y() = m_Right.y() * z +y;
    m_Up.x() = m_Up.x() * z + x;
    m_Up.y() = m_Up.y() * z +y;
    m_Down.x() = m_Down.x() * z + x;
    m_Down.y() = m_Down.y() * z +y;

    // create clipping vector for left side of the screen
    Vec3d rayDirection;
    rayDirection.x() = m_Left.x();
    rayDirection.y() = m_Left.y();
    rayDirection.z() = 0.0f;
    rayDirection -= m_rayOrigin;
    rayDirection.normalize();
    if (rayDirection.z() == 0.f)
    {
        rayDirection.z() = 0.0001f; // set an epsilon to avoid 0 divide
    }
    m_clippingInfo[0].x() = rayDirection.x();
    m_clippingInfo[0].y() = 1.f / rayDirection.z();

    // create clipping vector for right side of the screen
    rayDirection.x() = m_Right.x();
    rayDirection.y() = m_Right.y();
    rayDirection.z() = 0.0f;
    rayDirection -= m_rayOrigin;
    rayDirection.normalize();
    if (rayDirection.z() == 0.f)
    {
        rayDirection.z() = 0.0001f; // set an epsilon to avoid 0 divide
    }
    m_clippingInfo[1].x() = rayDirection.x();
    m_clippingInfo[1].y() = 1.f / rayDirection.z();

    // create clipping vector for up side of the screen
    rayDirection.x() = m_Up.x();
    rayDirection.y() = m_Up.y();
    rayDirection.z() = 0.0f;
    rayDirection -= m_rayOrigin;
    rayDirection.normalize();
    if (rayDirection.z() == 0.f)
    {
        rayDirection.z() = 0.0001f; // set an epsilon to avoid 0 divide
    }
    m_clippingInfo[2].x() = rayDirection.y(); // up ray -> take Y
    m_clippingInfo[2].y() = 1.f / rayDirection.z();


    // create clipping vector for down side of the screen
    rayDirection.x() = m_Down.x();
    rayDirection.y() = m_Down.y();
    rayDirection.z() = 0.0f;
    rayDirection -= m_rayOrigin;
    rayDirection.normalize();
    if (rayDirection.z() == 0.f)
    {
        rayDirection.z() = 0.0001f; // set an epsilon
    }
    m_clippingInfo[3].x() = rayDirection.y(); // bottom ray -> take Y
    m_clippingInfo[3].y() = 1.f / rayDirection.z();

    // create corner vectors
    Vec2d TopLeft, TopRight, BottomLeft, BottomRight;
    apply();
    GFX_ADAPTER->coord2DTo3D(Vec2d(viewport.m_left, viewport.m_bottom), 0.f, BottomLeft);
    GFX_ADAPTER->coord2DTo3D(Vec2d(viewport.m_right, viewport.m_bottom), 0.f, BottomRight);
    GFX_ADAPTER->coord2DTo3D(Vec2d(viewport.m_left, viewport.m_top), 0.f, TopLeft);
    GFX_ADAPTER->coord2DTo3D(Vec2d(viewport.m_right, viewport.m_top), 0.f, TopRight);

    TopLeft.x() = TopLeft.x() * z + x;
    TopLeft.y() = TopLeft.y() * z +y;
    TopRight.x() = TopRight.x() * z + x;
    TopRight.y() = TopRight.y() * z +y;
    BottomLeft.x() = BottomLeft.x() * z + x;
    BottomLeft.y() = BottomLeft.y() * z +y;
    BottomRight.x() = BottomRight.x() * z + x;
    BottomRight.y() = BottomRight.y() * z +y;

    m_screenCornersRays[0].x() = TopLeft.x();
    m_screenCornersRays[0].y() = TopLeft.y();
    m_screenCornersRays[0].z() = 0.0f;
    m_screenCornersRays[0] -= m_rayOrigin;
    m_screenCornersRays[0].normalize();

    m_screenCornersRays[1].x() = TopRight.x();
    m_screenCornersRays[1].y() = TopRight.y();
    m_screenCornersRays[1].z() = 0.0f;
    m_screenCornersRays[1] -= m_rayOrigin;
    m_screenCornersRays[1].normalize();

    m_screenCornersRays[2].x() = BottomLeft.x();
    m_screenCornersRays[2].y() = BottomLeft.y();
    m_screenCornersRays[2].z() = 0.0f;
    m_screenCornersRays[2] -= m_rayOrigin;
    m_screenCornersRays[2].normalize();

    m_screenCornersRays[3].x() = BottomRight.x();
    m_screenCornersRays[3].y() = BottomRight.y();
    m_screenCornersRays[3].z() = 0.0f;
    m_screenCornersRays[3] -= m_rayOrigin;
    m_screenCornersRays[3].normalize();

#ifdef ITF_SUPPORT_DEBUGFEATURE
    if (DEBUGINFO)
        DEBUGINFO->setCameraInfo(this);
#endif //!ITF_SUPPORT_DEBUGFEATURE

    computeDeltaXY();
}

void  Camera::computeDeltaXY()
{
    f32 halfFocaleX = getFocale();
    ITF_ASSERT(f32_Abs(halfFocaleX-MTH_PIBY2) > MTH_EPSILON);
    m_deltaX = f32_Abs(tan(halfFocaleX));

    f32 w = (f32)GFX_ADAPTER->getScreenWidth();
    f32 h = (f32)GFX_ADAPTER->getScreenHeight();
    
    f32 halfFocaleY = halfFocaleX * h / w;
    ITF_ASSERT(f32_Abs(halfFocaleY-MTH_PIBY2) > MTH_EPSILON);

    m_deltaY = f32_Abs(tan(halfFocaleY));


}

void Camera::copyTo(Camera& dst)
{
    dst = *this;
	dst.m_validyCount++;
}

void Camera::getScreenCornersAtZ (float _z, Vec2d* _corners) const
{
    if (m_screenCornersRays[0].z() != 0.0f)
    {
        float k = (_z - m_rayOrigin.z()) / m_screenCornersRays[0].z();
        _corners[0].x() = m_rayOrigin.x() + k * m_screenCornersRays[0].x();
        _corners[0].y() = m_rayOrigin.y() + k * m_screenCornersRays[0].y();
    }
    if (m_screenCornersRays[1].z() != 0.0f)
    {
        float k = (_z - m_rayOrigin.z()) / m_screenCornersRays[1].z();
        _corners[1].x() = m_rayOrigin.x() + k * m_screenCornersRays[1].x();
        _corners[1].y() = m_rayOrigin.y() + k * m_screenCornersRays[1].y();
    }
    if (m_screenCornersRays[2].z() != 0.0f)
    {
        float k = (_z - m_rayOrigin.z()) / m_screenCornersRays[2].z();
        _corners[2].x() = m_rayOrigin.x() + k * m_screenCornersRays[2].x();
        _corners[2].y() = m_rayOrigin.y() + k * m_screenCornersRays[2].y();
    }
    if (m_screenCornersRays[3].z() != 0.0f)
    {
        float k = (_z - m_rayOrigin.z()) / m_screenCornersRays[3].z();
        _corners[3].x() = m_rayOrigin.x() + k * m_screenCornersRays[3].x();
        _corners[3].y() = m_rayOrigin.y() + k * m_screenCornersRays[3].y();
    }
}

AABB Camera::getAABBatZ (float _z) const
{
    Vec2d min;  // bottom-left
    if (m_screenCornersRays[2].z() != 0.0f)
    {
        float k = (_z - m_rayOrigin.z()) / m_screenCornersRays[2].z();
        min.x() = m_rayOrigin.x() + k * m_screenCornersRays[2].x();
        min.y() = m_rayOrigin.y() + k * m_screenCornersRays[2].y();
    }

    Vec2d max;  // top-right
    if (m_screenCornersRays[1].z() != 0.0f)
    {
        float k = (_z - m_rayOrigin.z()) / m_screenCornersRays[1].z();
        max.x() = m_rayOrigin.x() + k * m_screenCornersRays[1].x();
        max.y() = m_rayOrigin.y() + k * m_screenCornersRays[1].y();
    }

    return AABB(min, max);
}

void Camera::setPosition(const Vec3d& _newPos)
{
    ITF_ASSERT_CRASH( _newPos.isValid(), "Invalid position");

    m_x = _newPos.x();
    m_y = _newPos.y();
    m_z = _newPos.z();
	m_validyCount++;
}

void Camera::setLookAtDir( const Vec3d& _newDir )
{
    m_lookAtDir = _newDir; m_lookAtDir.normalize();
	m_validyCount++;
}

void Camera::setUpDir( const Vec3d& _newUp )
{
    m_upDir = _newUp;
	m_validyCount++;
}

void Camera::update(float _dt)
{
    m_near = 1.0f;
    setNearFadeRange(0.f);
}

void Camera::apply()
{
    GFX_ADAPTER->setCamera(this);
}

void Camera::setDeltaFogZ( f32 _delta )
{
    m_deltaFogZ = _delta;
}

void Camera::computeFrustums(View *_view)
{
    FrustumDesc frustum;
    Vec3d cameraPos(m_x, m_y, m_z);
    FrustumDesc::buildFrustum(frustum, cameraPos, -Vec3d::ZAxis, getUpDir(), getFocale(), _view->getRatio(), getNearPlaneDist(), m_far);
    setViewFrustum(frustum);

    frustum.enlarge(getUpdateFrustumEnlargeRange());
    setUpdateFrustum(frustum);

    frustum.enlarge(getLoadingFrustumEnlargeRange() - getUpdateFrustumEnlargeRange());
    setLoadingFrustum(frustum);
}

} // namespace ITF
