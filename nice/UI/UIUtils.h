#ifndef _ITF_UIUTILS_H_
#define _ITF_UIUTILS_H_


namespace ITF
{


    //------------------------------------------------------------------------------------------------------------------
    //											        PULSE GENERATOR
    //------------------------------------------------------------------------------------------------------------------

    class PulseGenerator
    {
    public:
        PulseGenerator();

        void setup(f32 _defaultValue, f32 _minValue, f32 _maxValue, f32 _periodDuration, f32 _blendSpeed);
        void reset();
        bbool update(bbool _blendOut, f32 _dt);

        f32 getValue() const;
        f32 getCursor() const;

    private:

        void moveCursor(f32 _cursorDelta, f32 _minValueDelta, bbool _blendOut);

        f32 m_currentCursor;
        f32 m_currentMinValue;
        bbool m_needUpdate;

        f32 m_defaultValue;
        f32 m_minValue;
        f32 m_maxValue;
        f32 m_periodDuration;
        f32 m_blendSpeed;
    };


    //------------------------------------------------------------------------------------------------------------------
    //											        SHAKE GENERATOR
    //------------------------------------------------------------------------------------------------------------------

    class ShakeGenerator
    {
    public:
        ShakeGenerator();

        void setup(f32 _defaultValue, f32 _maxValue, f32 _shakeDuration, u32 _reboundCount);
        void reset();
        bbool update(f32 _dt);

        f32 getValue() const;
        f32 getCursor() const;

    private:

        f32 m_currentCursor;

        f32 m_defaultValue;
        f32 m_maxValue;
        f32 m_shakeDuration;
        u32 m_reboundCount;
    };


    //------------------------------------------------------------------------------------------------------------------
    //											        DRAG SCROLLING
    //------------------------------------------------------------------------------------------------------------------

    class DragScrolling
    {
    public :
        DragScrolling();

        void    dragReset(f32 _pos);
        bbool   dragMove(f32 _pos);
        bbool   dragEnd(f32 _pos);
        void    dragTo(f32 _currPos, f32 _wantedPos);
        f32     dragUpdate(f32 _currPos, f32 _minPos, f32 _maxPos, f32 _snapDist, f32 _dt);
        bbool   dragIsStarted() const;
        bbool   transitionIsJustEnd() const;
        bbool   transitionIsProgressing() const;
        void    transitionCancel();

        void    setupDragBounceTime(f32 _time) {m_dragBounceTime = _time;}
        void    setupDragBounceNorm(f32 _norm) {m_dragBounceNorm = _norm;}
        void    setupDragStartNorm(f32 _norm) {m_dragStartNorm = _norm;}
        void    setupMomentumFriction(f32 _friction) {m_momentumFriction = _friction;}
        void    setupMomentumDeceleration(f32 _deceleration) {m_momentumDeceleration = _deceleration;}
        void    setupMomentum(f32 _friction, f32 _deceleration) {m_momentumFriction = _friction; m_momentumDeceleration=_deceleration;}
        void    setupDampingBezierVal(f32 _valA, f32 _valB, f32 _valC, f32 _valD) {m_dampingBezierValA = _valA; m_dampingBezierValB=_valB; m_dampingBezierValC= _valC; m_dampingBezierValD=_valD;}

    private :
        void    momentumSpeed(f32 _speed, f32 _maxDistUpper, f32 _maxDistLower, f32& _newDist, f32& _newTime) const;
        void    momentumDist(f32 _dist, f32 _maxDistUpper, f32 _maxDistLower, f32& _newDist, f32& _newTime) const;
        void    damping(f32 _cursor, f32& _res) const;

        enum MovingTransitionType
        {
            MovingTransitionType_None = -1,
            MovingTransitionType_InstantBack,
            MovingTransitionType_BounceBack,
            MovingTransitionType_LaunchTo,
            MovingTransitionType_DragTo,
        };

        f32     m_movingSpeedAverager;
        f32     m_movingCurrentPos;
        f32     m_movingTotalDelta;
        f32     m_movingCurrentDelta;
        f32     m_movingDuration;
        bbool   m_movingStart;
        MovingTransitionType   m_movingTransitionLast;
        MovingTransitionType   m_movingTransitionType;
        f32     m_movingTransitionCursor;
        f32     m_movingTargetDist;
        f32     m_movingTargetTime;


        f32     m_dragBounceTime;
        f32     m_dragBounceNorm;
        f32     m_dragStartNorm;
        f32     m_dragSpeedAveragerCoef;
        f32     m_momentumFriction;
        f32     m_momentumDeceleration;
        f32     m_dampingBezierValA, m_dampingBezierValB, m_dampingBezierValC, m_dampingBezierValD;
        f32     m_dampingOutsideFactor;
    };
}
#endif // _ITF_UIUTILS_H_
