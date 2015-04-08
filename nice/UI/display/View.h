#ifndef _ITF_VIEW_H_
#define _ITF_VIEW_H_

#ifndef _ITF_BV_AABB_H_
#include "core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef _ITF_FRUSTUM_H_
#include "engine/display/Frustum.h"
#endif //_ITF_FRUSTUM_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_   

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

namespace ITF
{
	//================================================================================================
	// structures

	//-------------------------------------------------------------------------------------------------
	// viewport definition
	//
    struct GFX_Viewport
    {
        GFX_Viewport() : m_x(0), m_y(0), m_width(0), m_height(0), m_minZ(0), m_maxZ(1) {}
        i32    m_x;
        i32    m_y;
        i32    m_width;
        i32    m_height;
        f32    m_minZ;
        f32    m_maxZ;
    };

	//-------------------------------------------------------------------------------------------------
	// viewport definition
	//
	struct GFX_ViewportDefinition
	{
        GFX_ViewportDefinition() : m_left(0), m_top(0), m_right(1), m_bottom(1), m_minZ(0), m_maxZ(1) {}
		f32    m_left;
		f32    m_top;
		f32    m_right;
		f32    m_bottom;
		f32    m_minZ;
		f32    m_maxZ;

        f32 getWidth() const {return m_right - m_left;}
        f32 getHeight() const {return m_bottom - m_top;}
	};

    struct reflectionViewParam
    {
        reflectionViewParam():
            m_reflectionPlaneY(0.0f),
            m_reflectionPlaneZ(0.0f),
            m_scissorLeft(0),
            m_scissorRight(0),
            m_scissorTop(0),
            m_scissorBottom(0),
            m_lastReflectionFrame(0)
        {}


        const reflectionViewParam& operator = (const reflectionViewParam& src)
        {
            m_reflectionPlaneY      = src.m_reflectionPlaneY;
            m_reflectionPlaneZ      = src.m_reflectionPlaneZ;
            m_scissorLeft           = src.m_scissorLeft;
            m_scissorRight          = src.m_scissorRight;
            m_scissorTop            = src.m_scissorTop;
            m_scissorBottom         = src.m_scissorBottom;
            m_lastReflectionFrame   = src.m_lastReflectionFrame;
            return *this;
        }

        f32 m_reflectionPlaneY;
        f32 m_reflectionPlaneZ;

        u32 m_scissorLeft;
        u32 m_scissorRight;
        u32 m_scissorTop;
        u32 m_scissorBottom;

        u32 m_lastReflectionFrame;
    };

class Camera;
class FrustumDesc;

#define CAMERA  View::getCurrentView()->getCamera()
#define CAMERA_REMOTE View::getRemoteView()->getCamera()


#define SERIALIZE_ENUM_EDITABLE_VIEW_IDS(name,val) \
    SERIALIZE_ENUM_BEGIN(name,val); \
        SERIALIZE_ENUM_VAR(View::None); \
        SERIALIZE_ENUM_VAR(View::Main); \
        SERIALIZE_ENUM_VAR(View::Remote); \
        SERIALIZE_ENUM_VAR(View::MainAndRemote); \
        SERIALIZE_ENUM_VAR(View::MainOnly); \
        SERIALIZE_ENUM_VAR(View::RemoteOnly); \
        SERIALIZE_ENUM_VAR(View::RemoteAsMainOnly); \
        SERIALIZE_ENUM_VAR(View::All); \
    SERIALIZE_ENUM_END();

enum GFXViewZPassOverride
{
    GFX_VIEW_ZPASS_DEFAULT,
    GFX_VIEW_ZPASS_FORCE_USE_ZPASS,
    GFX_VIEW_ZPASS_FORCE_NO_ZPASS,
};

#define SERIALIZE_ENUM_GFX_ZPASS_OVERRIDE(name, val) \
    SERIALIZE_ENUM_BEGIN(name, val); \
        SERIALIZE_ENUM_VAR(GFX_VIEW_ZPASS_DEFAULT); \
        SERIALIZE_ENUM_VAR(GFX_VIEW_ZPASS_FORCE_USE_ZPASS); \
        SERIALIZE_ENUM_VAR(GFX_VIEW_ZPASS_FORCE_NO_ZPASS); \
    SERIALIZE_ENUM_END()

class View
{
public:
    enum ViewType
    {
        viewType_Camera,
        viewType_Frustum,
        viewType_ViewportUI,
        viewType_Count,
        ENUM_FORCE_SIZE_32(0)
    };

    enum EditableViewIds            // editable view ids, !! PLEASE do not Change ID values !!
    {
        None = 0,
        Main = 1,
        Remote = 2,
        MainAndRemote = 3,
        MainOnly = 4,
        RemoteOnly = 5,
        RemoteAsMainOnly = 6,
        All = 0xFFFFFFFF,
        ENUM_FORCE_SIZE_32(EditableViewIds)
    };

    enum MaskIds
    {
        MASKID_NONE =       0,
        MASKID_REMOTE_ASMAIN =      0x01,
        MASKID_MAINONLY =           0x02,
        MASKID_REMOTEONLY =         0x04,

        MASKID_MAIN =               MASKID_MAINONLY | MASKID_REMOTE_ASMAIN,
        MASKID_REMOTE =             MASKID_REMOTEONLY | MASKID_REMOTE_ASMAIN,

        MASKID_ALL =        0xFFFFFFFF,
    };

    enum { ZlistID_invalid = 0xFFFFFFFFu };

    enum eFreezeDrawPassState
    {
        DPFreeze_NONE = 0,
        DPFreeze_COPY_BACKBUFFER = 1,
        DPFreeze_DRAW_IMPOSTOR = 2,
        DPFreeze_COUNT,
        ENUM_FORCE_SIZE_32(eFreezeDrawPassState)
    };

    //////////////////////////////////////////////////////////////////////////
    ///
    /// @return the view's type
    ViewType getViewType()const { return m_type; }
    void setViewType(ViewType _type) { m_type = _type; }

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Creates the camera for a view
    /// A view doesn't have a camera by default because some views can be created just to prefectch data loading for example
    /// This method will erase the existing camera if any
     void    createCamera();

    //////////////////////////////////////////////////////////////////////////
    ///
	/// Deletes the camera for a view
	/// Does nothing if the view has no camera
	void    deleteCamera();

    //////////////////////////////////////////////////////////////////////////
    ///
    /// @return the view's camera if any
	Camera* getCamera() const {return m_camera;}

    
     //////////////////////////////////////////////////////////////////////////
     ///
     /// Set the view's target frustum
     void    setTargetFrustum(const FrustumDesc& _frustum);

     //////////////////////////////////////////////////////////////////////////
     ///
     /// @return the view's target frustum
     const FrustumDesc&    getTargetFrustum()const { return m_frustum; }

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Assigns an objectRef to a view
     void    setLinkedObject(ObjectRef _or) {m_or = _or;}

    //////////////////////////////////////////////////////////////////////////
    ///
    /// @returns the objref assigned to the view
     ObjectRef getLinkedObject(ObjectRef _or) {return m_or;}

     //////////////////////////////////////////////////////////////////////////
    ///
	/// Defines the view as the main view
	void    setMainView();

	//////////////////////////////////////////////////////////////////////////
	///
    /// @return the main view
     static View*  getMainView() {return m_mainView;}

     //////////////////////////////////////////////////////////////////////////
    ///
	/// @return btrue if the view is the main view
	bbool   isMainView();

    //////////////////////////////////////////////////////////////////////////
    ///
	/// @return the gameplay view
	static View*  getGameplayView() {return m_views[0];}

#ifdef ITF_USE_REMOTEVIEW
    //////////////////////////////////////////////////////////////////////////
    ///
    /// Defines the view as the remote view
    void    setRemoteView();

     //////////////////////////////////////////////////////////////////////////
    ///
    /// @return the remote view
     static View*  getRemoteView() {return m_remoteView;}

    //////////////////////////////////////////////////////////////////////////
    ///
    /// @return btrue if the view is the remote view
    bbool   isRemoteView();

#endif // ITF_USE_REMOTEVIEW

	static View*  getTouchView();

	//////////////////////////////////////////////////////////////////////////
	// disable view editor or cafe cheat
	//
#if defined(ITF_SUPPORT_EDITOR) || defined(CAFE_CHEAT_SUPPORT)
    void setDisableRendering(bbool _b) { m_disableRendering = _b; }
    bbool isDisabledForRendering()const { return m_disableRendering; }
#else
    bbool isDisabledForRendering()const { return bfalse; }
#endif

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Defines the current active view
    void    setCurrentView();

     //////////////////////////////////////////////////////////////////////////
    ///
    /// @return the current view
     static View*  getCurrentView() {return m_currentView;}

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Activates / desactivates a View
    void setActive(bbool _active);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// @return btrue if the view is active
    bbool isActive() const {return m_isActive;}

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Deletes the view (and its camera if any)
     static void    deleteView(View* _view);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Creates a new view
    /// @return the created view
    static View* createView(const char* _name, i32 _maskId, bbool _canBeMainView = btrue, u32 _zOrder = U32_INVALID, const char* _parentViewName = NULL);

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Deletes all the views
    static void    deleteAll();
    static void    deletePendingViews(); // Truly delete views which were deleted in non main threads

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Returns the all the views
     static const ITF_VECTOR<View*>&    getViews() {return m_views;}

     void setName(const String8& _name) {m_name = _name;}
     const String8& getName() const {return m_name;}
     void setMaskId(u32 _m) { m_maskId = _m; }
     u32 getMaskId() const { return m_maskId; }
     
     u64 getMaskView() const;

     static View::MaskIds           getMaskIdFromEditableViewId( View::EditableViewIds _editableViewId ); 
     static View::EditableViewIds   getEditableViewIdFromMaskId( View::MaskIds _maskId ); 

     u32 getZlistID()const { return m_ZlistID; }
     void setZlistID(u32 _zlistID) { m_ZlistID = _zlistID;}

     void lockAABB (bbool _lock) {m_lockAABB = _lock;}
     bbool isLocked() const {return m_lockAABB;}
     void updateScreenProjAABB(const AABB& _aabb) {m_lastScreenProj = _aabb;}
     const AABB& getScreenProjAABB() const {return m_lastScreenProj;}

     static void   lockAll();
     static void   unLockAll();
     static void   switchLockAll();

     /// Viewport
     void                             setViewportDefinition(const GFX_ViewportDefinition& _viewport);
     void                             setViewportDefinition(f32 _left, f32 _top, f32 _right, f32 _bottom, f32 _minZ = 0.0f, f32 _maxZ = 1.0f);
     const GFX_ViewportDefinition&    getViewportDefinition() const {return m_viewportDefinition;}
     const GFX_Viewport&              getViewport() const {return m_viewport;}
     const GFX_Viewport&              getWantedViewport() const {return m_wantedViewport;}
     const Vec3d*					  getWantedViewportPoints() const {return m_wantedViewportPoints;}
     void                             computeViewport();

     void                   compute2DTo3D(const Vec3d& _in, Vec3d& _out) const;
     void                   compute3DTo2D(const Vec3d& _in, Vec3d& _out) const;

     bool                   compute2DTo3D(const Vec2d & _in, f32 _z, Vec3d & _out) const; // return bfalse if the camera is parallel to the plane

     void                   apply();

     static View *          copyView(View * _view);
     static void            destroyCopy(View * _view);

     void                           setReflectionPlaneReference(reflectionViewParam& _param) { m_reflectionParam = _param; }
     const reflectionViewParam&     getReflectionPlaneReference() const { return m_reflectionParam;}
     u32                            getUseReflectionPlane(u32 _currentFrame) { return m_reflectionParam.m_lastReflectionFrame >= _currentFrame;}
     
     eFreezeDrawPassState   getFreezeState() const {return m_freezeDrawPassCurrentState;}
     void                   setFreezeState(eFreezeDrawPassState _freezeState) {m_freezeDrawPassCurrentState = _freezeState;}

	 inline void			setFrame(const ResourceID _frameTextureRessource, i32 _frameShiftOutLeft, i32 _frameShiftOutTop, 
		                             i32 _frameShiftOutRight, i32 _frameShiftOutBottom, i32 _frameSizeLeft, i32 _frameSizeTop,
									 i32 _frameSizeRight, i32 _frameSizeBottom)

	 {
		m_frameTextureRessource = _frameTextureRessource;
		m_frameShiftOutLeft = _frameShiftOutLeft;
		m_frameShiftOutTop = _frameShiftOutTop;
		m_frameShiftOutRight = _frameShiftOutRight;
		m_frameShiftOutBottom = _frameShiftOutBottom;
		m_frameSizeLeft = _frameSizeLeft;
		m_frameSizeTop = _frameSizeTop;
		m_frameSizeRight = _frameSizeRight;
		m_frameSizeBottom = _frameSizeBottom;
	 }

	class Texture *		getFrameTexture();

	inline i32				getFrameShiftOutLeft()	 { return m_frameShiftOutLeft; }
	inline i32				getFrameShiftOutTop()	 { return m_frameShiftOutTop; }
	inline i32				getFrameShiftOutRight()	 { return m_frameShiftOutRight; }
	inline i32				getFrameShiftOutBottom() { return m_frameShiftOutBottom; }

	inline i32				getFrameSizeLeft()	 { return m_frameSizeLeft; }
	inline i32				getFrameSizeTop()	 { return m_frameSizeTop; }
	inline i32				getFrameSizeRight()	 { return m_frameSizeRight; }
	inline i32				getFrameSizeBottom() { return m_frameSizeBottom; }

	void					setVisibilityRequiredFlags(ux _flags)
	{
		m_visibilityRequiredFlags = _flags;
	}
    ux						getVisibilityRequiredFlags() const
	{
		return m_visibilityRequiredFlags;
	}
    void					setVisibilityRejectFlags(ux _flags)
	{
		m_visibilityRejectFlags = _flags;
	}
    ux						getVisibilityRejectFlags() const
	{
		return m_visibilityRejectFlags;
	}
	bbool					isObjectVisibleInViewport(ux _objFlags) const
	{
		return (ux(_objFlags) & m_visibilityRejectFlags) == 0 && (ux(_objFlags) & m_visibilityRequiredFlags) == m_visibilityRequiredFlags;
	}

    void					setZPassOverride(GFXViewZPassOverride _zpass)
	{
		m_ZPassOverride = _zpass;
	}
    GFXViewZPassOverride	getZPassOverride() const
	{
		return m_ZPassOverride;
	}
    void                    setClearViewport(bbool _clear)
    {
        m_clearViewport = _clear;
    }
    bbool                   getClearViewport() const
    {
        return m_clearViewport;
    }

    View *getParentView() const {return m_parentView;}

    f32 getRatio () const;

	const Matrix44& getPreViewportMatrix() const { return m_preViewportMatrix; }

	void setViewportRotation( f32 _rot )
	{
		m_viewportRotation = _rot;
	}
	f32 getViewportRotation( void )
	{
		return m_viewportRotation;
	}
	f32	getScreenSizeX() const
	{
		return m_screenSizeX;
	}
	f32	getScreenSizeY() const
	{
		return m_screenSizeY;
	}

	// Adjust camera and projection according wanted ratio and some preferences.
	void adjustCameraAndProj( Vec3d &_camPos, Vec3d &_camDir, f32 &_focale, f32 &_finalRatio );

	void setWantedRatio( f32 _ratio )
	{
		m_wantedRatio = _ratio;
		m_dirtyMatrixInternal = true;
	}
	f32 getWantedRatio( void )
	{
		return m_wantedRatio;
	}

	void computeInternalMatrix();
	inline void updateInternalMatrix()
	{
		if ( m_dirtyMatrixInternal || m_curCamValidyCount != m_camera->getValidityCount() )
			computeInternalMatrix();
	}
	const Matrix44& getWorldToView()
	{
		return m_worldToView;
	}
	const Matrix44& getViewToProj()
	{
		return m_viewToProj;
	}
	const Matrix44& getWorldToProj()
	{
		return m_worldToProj;
	}
	const Matrix44& getProjToWorld()
	{
		return m_projToWorld;
	}

    bbool getWaitForPrefetchBeforeDisplay() {return m_waitForPrefetchBeforeDisplay;}
    void  setWaitForPrefetchBeforeDisplay(bbool _wait) {m_waitForPrefetchBeforeDisplay = _wait;}
	
    bbool isPhysicalReady() {return m_isPhysicalReady;}
    void setIsPhysicalReady(bbool _ready) {m_isPhysicalReady = _ready;}

private:
    View();     // only createView() is public
    ~View();    // only deleteView() is public

    static  ITF_VECTOR<View*>   m_views;
    static  ITF_VECTOR<View*>   m_pendingDeleteViews;
    static  View*               m_mainView;

#ifdef ITF_USE_REMOTEVIEW
    static  View*               m_remoteView;
#endif
    static  View*               m_currentView;

    Camera*                     m_camera;
    ObjectRef                   m_or;
    bbool                       m_isActive;
    bbool                       m_lockAABB;
    bbool                       m_clearViewport;

    AABB                        m_lastScreenProj;
    FrustumDesc                 m_frustum;
    ViewType                    m_type;
    String8                     m_name;
    View                        *m_parentView; // A view with a parent view will be rendered during its parent view render, it's faster 
                                               // but it's more limited (no custom AFX, no 2D ...).
    u32                         m_maskId;

    u32                         m_ZlistID;

    reflectionViewParam         m_reflectionParam;

    // Freeze DrawPass
    eFreezeDrawPassState        m_freezeDrawPassCurrentState;

#if defined(ITF_SUPPORT_EDITOR) || defined(CAFE_CHEAT_SUPPORT)
    bbool                       m_disableRendering;
#endif

	ResourceID					m_frameTextureRessource;
	i32							m_frameShiftOutLeft;
	i32							m_frameShiftOutTop;
	i32							m_frameShiftOutRight;
	i32							m_frameShiftOutBottom;
	i32							m_frameSizeLeft;        // in percent of the viewport size
	i32							m_frameSizeTop;
	i32							m_frameSizeRight;
	i32							m_frameSizeBottom;

    u32							m_visibilityRequiredFlags;
    u32							m_visibilityRejectFlags;
    u32                         m_zOrder;
    GFXViewZPassOverride		m_ZPassOverride;
    bbool                       m_waitForPrefetchBeforeDisplay;
    bbool                       m_isPhysicalReady;

private:
    GFX_Viewport                m_viewport;
	Vec3d						m_wantedViewportPoints[4];
    GFX_Viewport                m_wantedViewport;
    GFX_ViewportDefinition      m_viewportDefinition;
	Matrix44					m_preViewportMatrix;
    f32							m_wantedRatio;
    f32							m_viewportRotation;
	f32							m_screenSizeX, m_screenSizeY;

	bbool						m_dirtyMatrixInternal;
	u32							m_curCamValidyCount;
	Vec3d						m_cameraEyePt;
	Matrix44					m_worldToView;
	Matrix44					m_viewToProj;
	Matrix44					m_worldToProj;
	Matrix44					m_projToWorld;
};

} // namespace ITF



#endif //_ITF_VIEW_H_
