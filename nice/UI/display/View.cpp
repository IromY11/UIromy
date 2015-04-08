#include "precompiled_engine.h"

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef _ITF_GRIDFLUID_H_
#include "engine/display/GridFluid/GridFluid.h"
#endif // _ITF_GRIDFLUID_H_

namespace ITF
{

#if (defined ITF_PROJECT_POP && defined ITF_WINDOWS) || defined ITF_PC
#define ITF_FORCE_SIZE_VIEW
#endif

ITF_VECTOR<View*>   View::m_views;
ITF_VECTOR<View*>   View::m_pendingDeleteViews;
View*               View::m_mainView = NULL;
View*               View::m_currentView = NULL;

#ifdef ITF_USE_REMOTEVIEW
View*               View::m_remoteView = NULL;
#endif

void    View::createCamera()
{
    deleteCamera();
    m_camera = newAlloc(mId_Singleton, Camera);
    m_type = viewType_Camera;
	m_dirtyMatrixInternal = true;
}

void    View::deleteCamera()
{
    if (m_camera)
    {
        delete m_camera;
        m_camera = NULL;
    }
}

void View::setTargetFrustum( const FrustumDesc& _frustum )
{
    m_frustum = _frustum;
    m_type = viewType_Frustum;
}

void    View::setMainView()
{
    m_mainView = this;
    setActive(btrue);
}


bbool    View::isMainView()
{
    return (this == m_mainView);
}

#ifdef ITF_USE_REMOTEVIEW
void    View::setRemoteView()
{
    m_remoteView = this;
    setActive(btrue);
}

bbool    View::isRemoteView()
{
    return (this == m_remoteView);
}
#endif //ITF_USE_REMOTEVIEW

View*  View::getTouchView()
{
	View *TouchView = getCurrentView();

#ifdef ITF_USE_REMOTEVIEW
#ifndef ITF_WINDOWS
	TouchView = getRemoteView();
#endif // ITF_WINDOWS
#endif // ITF_USE_REMOTEVIEW

	return TouchView;
}

void    View::setCurrentView()
{
    m_currentView = this;
    setActive(btrue);
}

void    View::deleteView(View* _view)
{
    if(ThreadSettings::getCurrentThreadId() != eThreadId_mainThread)
    {
        _view->setActive(bfalse);
        m_pendingDeleteViews.push_back(_view);
        return;
    }

    delete _view;
    i32 index = m_views.find(_view);
    ITF_ASSERT(index >= 0);
    if (index < 0)
        return;
    m_views.removeAt(index);
    if (_view == m_mainView)
    {
        if (m_views.size())
            m_mainView = m_views[0];
        else
            m_mainView = NULL;
    }
}

void View::setActive(bbool _active)
{
	if ( m_isActive != _active )
	{
		m_isActive = _active;
		if ( getZlistID() != View::ZlistID_invalid )
		{
			GFX_ADAPTER->getZListManager().reset(getZlistID());
			GFX_ADAPTER->getGridFluidManager().clearFluidPrimitiveList();
		}
	}
}


View*    View::createView(const char* _name, i32 _maskId, bbool _canBeMainView, u32 _zOrder, const char* _parentViewName)
{
    View* ret = newAlloc(mId_System, View());
    ret->m_name = _name;
    ret->m_maskId = static_cast<u32>(_maskId);
    ret->m_zOrder = _zOrder;
    ITF_VECTOR<View*>::iterator viewIt = m_views.begin();
    for (; viewIt != m_views.end(); ++viewIt)
    {
        if ((*viewIt)->m_zOrder > _zOrder)
            break;
    }
    m_views.insert(viewIt, ret);
    if (_canBeMainView && NULL == m_mainView)
        m_mainView = ret;

    // Set parent view
    ret->m_parentView = NULL;
    const ITF_VECTOR<View*>& views = View::getViews();
    for(u32 iView = 0; iView < views.size(); ++iView)
    {
        View* pCurrentView = views[iView];
        if(pCurrentView->getName() == _parentViewName)
        {
            ret->m_parentView = pCurrentView;
            break;
        }
    }

    return ret;
}

void    View::deleteAll()
{
    for(;;)
    {
        if (0 == m_views.size())
            break;
        deleteView(m_views[0]);
    }
}

void    View::deletePendingViews()
{
    ITF_ASSERT(ThreadSettings::getCurrentThreadId() == eThreadId_mainThread);

    for (u32 i = 0; i <  m_pendingDeleteViews.size(); i++)
    {
        deleteView(m_pendingDeleteViews[i]);
    }
    m_pendingDeleteViews.clear();
}


View::View()
{
    m_camera = NULL;
    m_or = ITF_INVALID_OBJREF;
    m_isActive = bfalse;
    m_type = viewType_Count;
    m_lockAABB = bfalse;
    m_ZlistID = u32(ZlistID_invalid);
#if defined(ITF_SUPPORT_EDITOR) || defined(CAFE_CHEAT_SUPPORT)
    m_disableRendering = bfalse;
#endif
    m_freezeDrawPassCurrentState = DPFreeze_NONE;

	m_dirtyMatrixInternal = true;
    m_wantedRatio = 16.f/9.f;//default

	m_frameShiftOutLeft = 0;
	m_frameShiftOutTop = 0;
	m_frameShiftOutRight = 0;
	m_frameShiftOutBottom = 0;
	m_frameSizeLeft = 50;
	m_frameSizeTop = 50;
	m_frameSizeRight = 50;
	m_frameSizeBottom = 50;
	m_viewportRotation = 0;

    m_visibilityRequiredFlags = 0; // no bit required => everything is visible
    m_visibilityRejectFlags = 0x00000000; // UI view
    m_zOrder = U32_INVALID;
	m_ZPassOverride = GFX_VIEW_ZPASS_DEFAULT;
    m_isPhysicalReady = bfalse;
    m_waitForPrefetchBeforeDisplay = btrue;
    m_clearViewport = false;
}

View::~View()
{
    deleteCamera();
}


void View::lockAll()
{
    for (u32 i = 0; i < m_views.size(); i++)
    {
        m_views[i]->lockAABB(btrue);
    }
}

void View::unLockAll()
{
    for (u32 i = 0; i < m_views.size(); i++)
    {
        m_views[i]->lockAABB(bfalse);
    }
}

void View::switchLockAll()
{
    for (u32 i = 0; i < m_views.size(); i++)
    {
        m_views[i]->lockAABB(!m_views[i]->isLocked());
    }
}

void View::setViewportDefinition(const GFX_ViewportDefinition& _viewport)
{
    ITF_Memcpy(&m_viewportDefinition, &_viewport, sizeof(GFX_ViewportDefinition));

    computeViewport();
}

void View::setViewportDefinition(f32 _left, f32 _top, f32 _right, f32 _bottom, f32 _minZ, f32 _maxZ)
{
    m_viewportDefinition.m_left   = _left;
    m_viewportDefinition.m_top    = _top;
    m_viewportDefinition.m_right  = _right;
    m_viewportDefinition.m_bottom = _bottom;
    m_viewportDefinition.m_minZ   = _minZ;
    m_viewportDefinition.m_maxZ   = _maxZ;

    computeViewport();
}

void View::computeViewport()
{
    m_screenSizeX = f32(GFX_ADAPTER->getWindowWidth());
    m_screenSizeY = f32(GFX_ADAPTER->getWindowHeight());

    // DO NOT CHANGE the viewport size on console / mobile
#ifdef ITF_FORCE_SIZE_VIEW
    // Forces the viewport to be 16/9...
    // Pop tools request

    if(m_screenSizeY * m_wantedRatio < m_screenSizeX)
        m_screenSizeX  = f32_Floor(m_screenSizeY * m_wantedRatio);
    else
        m_screenSizeY = f32_Floor(m_screenSizeX / m_wantedRatio);
#endif

	m_wantedViewportPoints[0].set(m_viewportDefinition.m_left * m_screenSizeX, m_viewportDefinition.m_top * m_screenSizeY, 0.0f);
	m_wantedViewportPoints[1].set(m_viewportDefinition.m_right * m_screenSizeX, m_viewportDefinition.m_top * m_screenSizeY, 0.0f);
	m_wantedViewportPoints[2].set(m_viewportDefinition.m_right * m_screenSizeX, m_viewportDefinition.m_bottom * m_screenSizeY, 0.0f);
	m_wantedViewportPoints[3].set(m_viewportDefinition.m_left * m_screenSizeX, m_viewportDefinition.m_bottom * m_screenSizeY, 0.0f);
	if ( m_viewportRotation != 0.0f )
	{
		Matrix44 rotMat;
		Vec3d center;
		center = (m_wantedViewportPoints[0] + m_wantedViewportPoints[2]) * 0.5f;
		rotMat.setRotationZ(m_viewportRotation * MTH_PI / 180.0f);
		for ( ux i = 0; i < 4; i++)
		{
			m_wantedViewportPoints[i] -= center;
			rotMat.transformVector(m_wantedViewportPoints[i], m_wantedViewportPoints[i]);
			m_wantedViewportPoints[i] += center;
		}
	}
	f32 wantedLeft, wantedTop, wantedRight, wantedBottom;
	wantedLeft = ITF::Min(m_wantedViewportPoints[0].x(),m_wantedViewportPoints[1].x());
	wantedLeft = ITF::Min(wantedLeft,m_wantedViewportPoints[2].x());
	wantedLeft = ITF::Min(wantedLeft,m_wantedViewportPoints[3].x());

	wantedRight = ITF::Max(m_wantedViewportPoints[0].x(),m_wantedViewportPoints[1].x());
	wantedRight = ITF::Max(wantedRight,m_wantedViewportPoints[2].x());
	wantedRight = ITF::Max(wantedRight,m_wantedViewportPoints[3].x());

	wantedTop = ITF::Min(m_wantedViewportPoints[0].y(),m_wantedViewportPoints[1].y());
	wantedTop = ITF::Min(wantedTop,m_wantedViewportPoints[2].y());
	wantedTop = ITF::Min(wantedTop,m_wantedViewportPoints[3].y());

	wantedBottom = ITF::Max(m_wantedViewportPoints[0].y(),m_wantedViewportPoints[1].y());
	wantedBottom = ITF::Max(wantedBottom,m_wantedViewportPoints[2].y());
	wantedBottom = ITF::Max(wantedBottom,m_wantedViewportPoints[3].y());

	//***
	// Handle viewport outside screen.
	f32 left, top, right, bottom;
	left = ITF::Clamp(wantedLeft, 0.0f, m_screenSizeX);
	top = ITF::Clamp(wantedTop, 0.0f, m_screenSizeY);
	right = ITF::Clamp(wantedRight, 0.0f, m_screenSizeX);
	bottom = ITF::Clamp(wantedBottom, 0.0f, m_screenSizeY);
	if ( ( left != wantedLeft ) || ( right != wantedRight ) ||
		 ( top != wantedTop ) || ( bottom != wantedBottom ) )
	{
		f32 Xaw = (wantedRight - wantedLeft) * 0.5f;
		f32 Xbw = wantedLeft + Xaw;
		f32 Yaw = (wantedBottom - wantedTop) * 0.5f;
		f32 Ybw = wantedTop + Yaw;

		f32 Xai = (right - left) * 0.5f;
		f32 Xbi = left + Xai;
		f32 Yai = (bottom - top) * 0.5f;
		f32 Ybi = top + Yai;

		if ( ( Xai != 0.0f ) && ( Yai != 0.0f ) )
		{
			f32 Xaf = Xaw / Xai;
			f32 Xbf = (Xbw - Xbi) / Xai;
			f32 Yaf = Yaw / Yai;
			f32 Ybf = (Ybw - Ybi) / Yai;
			m_preViewportMatrix.setFromFloat(Xaf,	0.0f,	0.0f,	0.0f,
											 0.0f,	Yaf,	0.0f,	0.0f,
											 0.0f,	0.0f,	1.0f,	0.0f,
											 Xbf,	-Ybf,	0.0f,	1.0f );
		}
		else
			m_preViewportMatrix.setIdentity();
	}
	else
		m_preViewportMatrix.setIdentity();

    m_viewport.m_x = (u32) (left);
    m_viewport.m_width = (u32) ((right-left));
    m_viewport.m_y = (u32) (top);
    m_viewport.m_height = (u32) ((bottom-top));
    m_viewport.m_maxZ = m_viewportDefinition.m_maxZ;
    m_viewport.m_minZ = m_viewportDefinition.m_minZ;

    m_wantedViewport.m_x = (u32) (wantedLeft);
    m_wantedViewport.m_width = (u32) ((wantedRight - wantedLeft));
    m_wantedViewport.m_y = (u32) (wantedTop);
    m_wantedViewport.m_height = (u32) ((wantedBottom - wantedTop));
    m_wantedViewport.m_maxZ = m_viewportDefinition.m_maxZ;
    m_wantedViewport.m_minZ = m_viewportDefinition.m_minZ;

	m_dirtyMatrixInternal = true;
}

void View::compute2DTo3D(const Vec3d& _in, Vec3d& _out) const
{
	((View*)this)->updateInternalMatrix();

    GFX_ADAPTER->vec3dUnProject(&_out, &_in, &getViewport(), &m_projToWorld);
}

bool View::compute2DTo3D(const Vec2d & _in, f32 _zPlane, Vec3d & _out) const
{
	((View*)this)->updateInternalMatrix();

    // 1 - we first compute the 3D point for a reference Z = 1
    Vec3d point2DAtZOne(_in.x(), _in.y(), 1.);
    Vec3d point3DAtZOne;
    GFX_ADAPTER->vec3dUnProject(&point3DAtZOne, &point2DAtZOne, &getViewport(), &m_projToWorld);

    // 2 - There we compute a direction vector from the camera position
    Vec3d vEyePt = m_cameraEyePt; // ( m_camera->getX(), m_camera->getY(), m_camera->getZ());
    Vec3d dirToTarget = point3DAtZOne - vEyePt;

    // 3 - We get the intersection of the ray with the Z plane
    // Point On Ray = vEyePt + t * dirToTarget
    // We want the point on the ray with a specific Z
    f32 denominator = dirToTarget.z();
    if (denominator==0.f)
        return bfalse;

    f32 t = (_zPlane - vEyePt.z())/denominator; // t is the parametric value of the ray equation
    _out.x() = vEyePt.x() + t * dirToTarget.x();
    _out.y() = vEyePt.y() + t * dirToTarget.y();
    _out.z() = _zPlane;

    return btrue;
}

void View::compute3DTo2D(const Vec3d& _in, Vec3d& _out) const
{
	((View*)this)->updateInternalMatrix();

    GFX_ADAPTER->vec3dProject(&_out, &_in, &getViewport(), &m_worldToProj);
}

void View::apply()
{
    computeViewport();
    GFX_ADAPTER->forceScreenSize(getViewport().m_width, getViewport().m_height);
    m_camera->apply();
}

View * View::copyView(View * _view)
{
    View* ret = newAlloc(mId_Debug, View());
    *ret = *_view;

    ret->m_camera = NULL;
    if (_view->getCamera())
    {
        ret->m_camera = newAlloc(mId_Debug,Camera());
        _view->getCamera()->copyTo(*ret->m_camera);
    }
    return ret;
}

void View::destroyCopy(View * _view)
{
    SF_DEL(_view);
}

u64 View::getMaskView() const
{
    ITF_ASSERT_MSG(m_views.size() < 128, "too many views in scene !!");
    u32 i = 0;
    for (ITF_VECTOR<View*>::const_iterator viewIter = m_views.begin();
        viewIter != m_views.end(); viewIter++, i++)
    {
        if ((*viewIter) == this)
            return u64(1)<<i;
    }
    return 0;
}


View::MaskIds View::getMaskIdFromEditableViewId( View::EditableViewIds _editableViewId )
{
    switch ( _editableViewId )
    {
        case View::None :         return View::MASKID_NONE;
        case View::Main :         return View::MASKID_MAIN;
        case View::Remote :       return View::MASKID_REMOTE;
        case View::MainAndRemote : return (View::MaskIds)(View::MASKID_MAIN | View::MASKID_REMOTE);
        case View::MainOnly :     return View::MASKID_MAINONLY;
        case View::RemoteOnly :   return View::MASKID_REMOTEONLY;
    }
    return View::MASKID_ALL;
}

View::EditableViewIds View::getEditableViewIdFromMaskId( View::MaskIds _maskId )
{
    switch ( _maskId )
    {
        case View::MASKID_NONE :    return View::None;
        case View::MASKID_MAIN :    return View::Main;
        case View::MASKID_REMOTE :  return View::Remote;
        case View::MASKID_MAINONLY :  return View::MainOnly;
        case View::MASKID_REMOTEONLY :return View::RemoteOnly;
        case View::MASKID_REMOTE_ASMAIN : return View::RemoteAsMainOnly;
        default :
        {
            if ( _maskId == (View::MASKID_MAIN | View::MASKID_REMOTE) )
            {
                return View::MainAndRemote;
            }
        }        
    }
    return View::All;
}

Texture *View::getFrameTexture()
{
	if (m_frameTextureRessource.isValid())
	{
		Texture* texture = (Texture*)m_frameTextureRessource.getResource();

		if ( texture && texture->isPhysicallyLoaded())
			return texture;
	}
	return NULL;
}

f32 View::getRatio () const
{
	if ( m_wantedViewport.m_height != 0.0f )
	    return (f32) m_wantedViewport.m_width / (f32) m_wantedViewport.m_height;
	else
		return 1.0f;
}

// Adjust camera and projection according wanted ratio and some preferences.
void View::adjustCameraAndProj( Vec3d &_camPos, Vec3d &_camDir, f32 &_focale, f32 &_finalRatio )
{
	Camera* cam = m_camera;
	f32 horizontalVersusVertical = cam->getHorizontalVersusVertical();
	if ( horizontalVersusVertical <= 0.0f )
	{
		_camPos.set( cam->getX(), cam->getY(), cam->getZ());
		_camDir = cam->getLookAtDir();
		_focale = cam->getFocale();
		_finalRatio = getRatio();
	}
	else
	{
		f32 realRatio = getRatio();
		f32 focaleVert = cam->getFocale();
		f32 wantedHalfHeight = tanf(focaleVert * 0.5f);
		f32 wantedHalfWidth = wantedHalfHeight * m_wantedRatio;
		f32 realHalfHeight0 = wantedHalfHeight;
		f32 realHalfWidth0 = realHalfHeight0 * realRatio;
		f32 realHalfHeight1 = realHalfHeight0 * (wantedHalfWidth / realHalfWidth0);
		f32 realHalfWidth1 = wantedHalfWidth;

		f32 halfWidth = f32_Lerp(realHalfWidth0, realHalfWidth1, horizontalVersusVertical);
		f32 halfHeight = f32_Lerp(realHalfHeight0, realHalfHeight1, horizontalVersusVertical);

		_focale = atanf(halfHeight) * 2.0f;
		_finalRatio = realRatio;

		_camPos.set( cam->getX(), cam->getY(), cam->getZ());
		_camDir = cam->getLookAtDir();

		const Vec2d &fixedPointInit = cam->getFixedPoint();
		if ( ( fixedPointInit.x() != 0.5f ) || ( fixedPointInit.y() != 0.5f ) )
		{
			Vec2d fixedPoint(fixedPointInit.x() * 2.0f - 1.0f, fixedPointInit.y() * -2.0f + 1.0f);
			Vec2d srcFixedPoint, dstFixedPoint, shiftFixedPoint;
			srcFixedPoint.set(fixedPoint.x() * realHalfWidth0, fixedPoint.y() * realHalfHeight0);
			dstFixedPoint.set(fixedPoint.x() * halfWidth, fixedPoint.y() * halfHeight);
			shiftFixedPoint = dstFixedPoint - srcFixedPoint;
			Vec3d viewDir0, viewDir1, worldDir0, worldDir1;

			viewDir0.set(halfWidth - shiftFixedPoint.x(), halfHeight - shiftFixedPoint.y(), -1.0f);
			viewDir1.set(-halfWidth - shiftFixedPoint.x(), -halfHeight - shiftFixedPoint.y(), -1.0f);

			Vec3d vLookatPt = _camPos + _camDir;
			Vec3d vUpVec( cam->getUpDir().getX(), cam->getUpDir().getY(), cam->getUpDir().getZ() );
			Matrix44 worldToView, viewToWorld;
			computeViewLookAtRH(worldToView, _camPos, vLookatPt, vUpVec);
			viewToWorld.inverseOrthogonal(worldToView);

			viewToWorld.transformVector(worldDir0, viewDir0);
			viewToWorld.transformVector(worldDir1, viewDir1);
			worldDir0.normalize();
			worldDir1.normalize();
			_camDir = worldDir0 + worldDir1;
			_camDir.normalize();
		}

		const Vec2d &offsetHVS = cam->getOffsetHVS();
		if ( !offsetHVS.isNullEpsilon() )
		{
			_camPos.x() += (halfWidth - realHalfWidth0) * offsetHVS.x() * 5.0f;
			_camPos.y() += (halfHeight - realHalfHeight0) * offsetHVS.y() * 5.0f;
		}
	}
}

void View::computeInternalMatrix()
{
	Camera *camera = m_camera;
	m_dirtyMatrixInternal = false;
	m_curCamValidyCount = camera->getValidityCount();

    f32 nearPlane   = 1.f;
    f32 farPlane    = 1000.f;

    nearPlane   = camera->getNearPlaneDist();
    farPlane    = camera->getFarPlaneDist();

	Vec3d vEyePt, eyeDir;
	f32 usedFocale, usedRatio;
	adjustCameraAndProj( vEyePt, eyeDir, usedFocale, usedRatio );

    Vec3d vLookatPt = vEyePt + eyeDir;
    Vec3d vUpVec( camera->getUpDir().getX(), camera->getUpDir().getY(), camera->getUpDir().getZ() );
	f32 focale_factor = ((getWantedViewport().m_height / getScreenSizeY()) / getViewportDefinition().getHeight());
	if ( focale_factor != 1.0f )
		usedFocale = atanf( tanf(usedFocale * 0.5f) * focale_factor ) * 2.0f;

	m_cameraEyePt = vEyePt;
	computeViewLookAtRH( m_worldToView, (const Vec3d &)vEyePt, (const Vec3d &)vLookatPt, (const Vec3d &)vUpVec);
	computeProjPerspectiveRH( m_viewToProj, usedFocale, usedRatio, nearPlane, farPlane );

    //Flip if needed
    if(camera->getFlag() & CAMERAFLAG_MIRROR_X)
    {
        const Float4 invX = loadXYZW(-1.0f, 1.0f, 1.0f, 1.0f);
        Float4 vtrans = m_worldToView.T();
        vtrans = vtrans * invX;
        Float4 vI = m_worldToView.I();
        vI = neg(vI);
        m_worldToView.setI(vI);
        m_worldToView.setT(vtrans);
    }
    if(camera->getFlag() & CAMERAFLAG_MIRROR_Y)
    {
        const Float4 invY = loadXYZW(1.0f, -1.0f, 1.0f, 1.0f);
        Float4 vtrans = m_worldToView.T();
        vtrans = vtrans * invY;
        Float4 vJ = m_worldToView.J();
        vJ = neg(vJ);
        m_worldToView.setJ(vJ);
        m_worldToView.setT(vtrans);
    }

    m_worldToProj.mul44(m_worldToView,m_viewToProj);
    m_projToWorld.inverse44(m_worldToProj);
}

} // namespace ITF
