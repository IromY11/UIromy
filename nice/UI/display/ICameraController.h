#ifndef _ITF_ICAMERACONTROLLER_H_
#define _ITF_ICAMERACONTROLLER_H_

namespace ITF
{
    class CameraControllerData;
    class CameraModifierComponent;

    class CameraData
    {
    public:
        CameraData()
            : m_pos(Vec3d::Zero)
            , m_lookAtDir(-Vec3d::ZAxis)
            , m_upDir(Vec3d::YAxis)
            , m_focale(MTH_PIBY4)
            , m_depth(13.f)
            , m_deltaFogZ(0.f)
            , m_flagMirror(0)
			, m_UpDnAngle(Angle::Zero)
			, m_horizontalVersusVertical(0.0f)
			, m_offsetHVS(0.0f, 0.0f)
        {
        }

        Vec3d m_pos;
        Vec3d m_lookAtDir;
        Vec3d m_upDir;
        f32 m_focale;
        f32 m_depth;
        f32 m_deltaFogZ;
        u32 m_flagMirror;
		Angle m_UpDnAngle;
		f32		m_horizontalVersusVertical;
		Vec2d	m_offsetHVS;
    };

    class ICameraController
    {
    public:
        ICameraController()
        {}

        virtual void        getCameraData( CameraData& _data ) const = 0;
        virtual void        teleport( const Vec3d& _newPos ) {}
        virtual Actor*      getControllerActor() const { return NULL; }
        virtual void        onCameraEvent( Event* _event ) {}
        virtual void        onManagerUpdate( f32 _dt ) {}
        virtual bbool       isAlwaysActive() const { return bfalse; }
        virtual CameraModifierComponent *getCameraModifier(bbool _useConstraint =btrue) const { return NULL; }
        virtual bbool       isInGameCamera() const { return bfalse; }
        virtual Actor*      getMainSubject() const { return NULL; }

#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void        drawDebug( const CameraControllerData& _data ) {}
#endif //ITF_SUPPORT_DEBUGFEATURE
    };
}

#endif // _ITF_ICAMERACONTROLLER_H_
