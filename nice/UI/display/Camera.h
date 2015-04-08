#ifndef _ITF_CAMERA_H_
#define _ITF_CAMERA_H_

#ifndef _ITF_FRUSTUM_H_
#include "engine/display/Frustum.h"
#endif //_ITF_FRUSTUM_H_

#ifndef _ITF_BV_AABB_H_
#include "Core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

namespace ITF
{

    enum Camera_Flag
    {
        CAMERAFLAG_MIRROR_X = 1,
        CAMERAFLAG_MIRROR_Y = 2,
        ENUM_FORCE_SIZE_32(Camera_Flag)
    };

//////////////////////////////////////////////////////////////////////////////////////////////////
///The camera itself
class Camera
{

public:
    Camera();
    ~Camera();

    AABB    getAABBatZ (float _z) const; // bottom-left to top-right
    void    getScreenCornersAtZ (float _z, Vec2d* _corners) const; // top-left, top-right, bottom-left, bottom-right

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// calculate clipping and picking vectors
    void    startFrame(); 
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// check visibility of a rectangle
    /// @param _aabb the rectangle
    /// @param _z the depth of the rectangle
    bbool   isRectVisible(const AABB& _aabb, float _z) const { return m_viewFrustum.isInFrustum(_aabb.getMin(), _aabb.getMax(), _z); }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// clone camera
    /// @param dst
    void copyTo(Camera& dst);
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// apply the current parameters to the graphics subsystem
    void apply();

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// accessor to X (X component of unbiased position)
    f32  getX()   const       {return m_x;}
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// accessor to Y (Y component of unbiased position)
    f32  getY()   const       {return m_y;}
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// accessor to Z (Z component of unbiased position)
    f32  getZ()   const       {return m_z;}
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// accessor to z (set)
    /// @param _z
    void setZ(f32 _z )        {m_z = _z;m_validyCount++;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// accessors to frustum
    const FrustumDesc&  getViewFrustum              () const                        {return m_viewFrustum;}
    void                setViewFrustum              (const FrustumDesc& _f)         { m_viewFrustum = _f;}
    
    const FrustumDesc&  getUpdateFrustum            () const                        {return m_updateFrustum;}
    void                setUpdateFrustum            (const FrustumDesc& _f)         { m_updateFrustum = _f;}
    
    const FrustumDesc&  getLoadingFrustum           () const                        {return m_loadingFrustum;}
    void                setLoadingFrustum           (const FrustumDesc& _f)         { m_loadingFrustum = _f;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// accessor to m_rotateZAngle (set)
    /// @param _z
    void            setRotateZAngle         (f32 _a )                       {m_rotateZAngle = _a;}

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// update the camera position depending on mode. Doesn't handle bias
    /// @param _dt frame duration
    void            update                  (   float _dt);
    
    Vec3d           getPos                  (   )const                      { return Vec3d(m_x, m_y, m_z); }    
    const Vec3d&    getLookAtDir            (   )const                      { return m_lookAtDir; }
    const Vec3d&    getUpDir                (   )const                      { return m_upDir; }
    f32             getFocale               (   )const                      { return m_focale; }
    f32             getDeltaFogZ            (   )const                      { return m_deltaFogZ; }
    
    void            setPosition             (   const Vec3d& _newPos    );  // from the gameplay, use teleport() instead    
    void            setLookAtDir            (   const Vec3d& _newDir    );
    void            setUpDir                (   const Vec3d& _newUp     );
    void            setDeltaFogZ            (   f32 _delta              );
    const void      setFocale               (   f32 _focale ) { m_focale = _focale; }

    void            computeDeltaXY();
    void            computeFrustums(class View *_view);

    ITF_INLINE  f32 getDeltaX() {return m_deltaX;} //slope of line which transforms world AABB to screen AABB
    ITF_INLINE  f32 getDeltaY() {return m_deltaY;}

    f32 getNearPlaneDist() const { return m_near; }
    void setNearPlaneDist(f32 _near) { m_near = _near; }

    f32 getFarPlaneDist() const { return m_far; }
    void setFarPlaneDist(f32 _far) { m_far = _far; }

    f32 getNearFadeRange() const { return m_nearFadeRange; }
    void setNearFadeRange(f32 _nearFadeRange) { m_nearFadeRange = _nearFadeRange; m_nearFadeInvertRange = (m_nearFadeRange==0.f)?0.f:(1.f/m_nearFadeRange); }
    f32 getNearFadeInvertRange() const { return m_nearFadeInvertRange; }

    u32 getFlag() const { return m_flag; }
    void setFlag(u32 _flag) { m_flag = _flag; }

    f32 getUpdateFrustumEnlargeRange()const { return m_updateFrustumEnlargeRange; }
    void setUpdateFrustumEnlargeRange(f32 _v) { m_updateFrustumEnlargeRange = _v; }

    f32 getLoadingFrustumEnlargeRange()const { return m_loadingFrustumEnlargeRange; }
    void setLoadingFrustumEnlargeRange(f32 _v) { m_loadingFrustumEnlargeRange = _v; }

	u32 getValidityCount() const {return m_validyCount;}

	void setHorizontalVersusVertical( f32 _hvv )
	{
		m_horizontalVersusVertical = _hvv;
		m_validyCount++;
	}
	f32 getHorizontalVersusVertical( void )
	{
		return m_horizontalVersusVertical;
	}
	void setFixedPoint( const Vec2d& _fp )
	{
		m_fixedPoint = _fp;
		m_validyCount++;
	}
	const Vec2d& getFixedPoint( void ) const
	{
		return m_fixedPoint;
	}
	void setOffsetHVS( const Vec2d& _offset )
	{
		m_offsetHVS = _offset;
		m_validyCount++;
	}
	const Vec2d& getOffsetHVS( void ) const
	{
		return m_offsetHVS;
	}

    ///////// START VIEWPORT
private:
        float   m_near, m_far;        // near & far planes
        f32   m_nearFadeRange;        // near alpha fade range
        f32   m_nearFadeInvertRange;  // 1.f/m_nearFadeRange or 0
public:
        Vec2d   m_topLeft;            // unit : screen size ratio (0..1)
        Vec2d   m_bottomRight;        // unit : screen size ratio (0..1)
    ///////// END  VIEWPORT


    Vec2d m_Left, m_Right, m_Up, m_Down;
    Vec3d m_rayOrigin;
    Vec2d m_clippingInfo[4]; // in the order left, right, up, bottom
                             // m_clippingInfo is precomputed from the ray starting at the camera and going along the clipping plane
                             // As the camera roll is 0, the y (for left/right) or x (up/bottom) of the ray direction is not interesting for clipping.
                             // So the member m_x of m_clippingInfo is the x or y of the parametric equation.
                             // The member m_z is the precomputed inverse of the ray z component
    Vec3d m_screenCornersRays[4]; // in the order left, right, up, bottom

private:

    f32         m_deltaX;
    f32         m_deltaY;
    f32         m_x, m_y, m_z;
    f32         m_rotateZAngle;
    f32         m_focale;
    bbool       m_useRotateCam;
    Vec3d       m_lookAtDir;
    Vec3d       m_upDir;
    f32         m_deltaFogZ;
    u32         m_flag;
	f32							m_horizontalVersusVertical;
	Vec2d						m_fixedPoint;
	Vec2d						m_offsetHVS;

    FrustumDesc m_viewFrustum;
    FrustumDesc m_updateFrustum;
    FrustumDesc m_loadingFrustum;


    f32         m_updateFrustumEnlargeRange;
    f32         m_loadingFrustumEnlargeRange;
	u32			m_validyCount;
};

} // namespace ITF



#endif //_ITF_CAMERA_H_
