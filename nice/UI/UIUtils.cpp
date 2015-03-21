#include "precompiled_gameplay.h"

#ifndef _ITF_UIUTILS_H_
#include "gameplay/components/UI/UIUtils.h"
#endif //_ITF_UIUTILS_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    //------------------------------------------------------------------------------------------------------------------
    //											        PULSE GENERATOR
    //------------------------------------------------------------------------------------------------------------------

    PulseGenerator::PulseGenerator()
        : m_currentCursor(0.0f)
        , m_currentMinValue(0.0f)
        , m_defaultValue(0.0f)
        , m_minValue(0.0f)
        , m_maxValue(0.0f)
        , m_periodDuration(0.0f)
        , m_blendSpeed(F32_INFINITY)
        , m_needUpdate(bfalse)
    {
    }

    void PulseGenerator::setup(f32 _defaultValue, f32 _minValue, f32 _maxValue, f32 _periodDuration, f32 _blendSpeed)
    {
        m_defaultValue = _defaultValue;
        m_minValue = _minValue;
        m_maxValue = _maxValue;
        m_periodDuration = _periodDuration;
        m_blendSpeed = _blendSpeed;
    }

    void PulseGenerator::reset()
    {
        m_currentCursor = 0.0f;
        m_currentMinValue = m_defaultValue;
        m_needUpdate = btrue;
    }

    bbool PulseGenerator::update(bbool _blendOut, f32 _dt)
    {
        if(!_blendOut || m_currentCursor > 0.0f || m_currentMinValue!=m_defaultValue)
        {
            f32 minValueDelta = F32_INFINITY;
            if(m_blendSpeed!=F32_INFINITY)
            {
                minValueDelta = m_blendSpeed * _dt;
            }

            f32 cursorDelta = F32_INFINITY;
            if(m_periodDuration != 0.0f)
            {
                cursorDelta = (_dt/m_periodDuration);
            }

            moveCursor(cursorDelta, minValueDelta, _blendOut);

            m_needUpdate = bfalse;
            return btrue;
        }

        if(m_needUpdate)
        {
            m_needUpdate = bfalse;
            return btrue;
        }

        return bfalse;
    }

    f32 PulseGenerator::getValue() const
    {
        f32 val = m_currentMinValue + (m_maxValue - m_currentMinValue) * (f32_Cos((m_currentCursor + 1)* MTH_PI) + 1.f) * 0.5f;

        return val;
    }

    f32 PulseGenerator::getCursor() const
    {
        return m_currentCursor;
    }

    void PulseGenerator::moveCursor(f32 _cursorDelta, f32 _minValueDelta, bbool _blendOut)
    {
        if(_minValueDelta!=0.0f)
        {
            f32 target = _blendOut ? m_defaultValue : m_minValue;
            f32 diff = target - m_currentMinValue;

            if(f32_Abs(diff)<=_minValueDelta)
            {
                m_currentMinValue = target;
            }
            else if(target<m_currentMinValue)
            {
                m_currentMinValue -= _minValueDelta;
            }
            else if(target>m_currentMinValue)
            {
                m_currentMinValue += _minValueDelta;
            }
        }

        
        if(!_blendOut)
        {
            if(_cursorDelta!=F32_INFINITY)
                m_currentCursor += _cursorDelta;
            else
                m_currentCursor = 0.5f;
        }
        else if(m_currentCursor > 0.0f)
        {
            if(_cursorDelta!=F32_INFINITY)
            {
                m_currentCursor = f32_Modulo(m_currentCursor, 2.0f);
                if (m_currentCursor > 1.f) 
                    m_currentCursor = 2.0f - m_currentCursor;
                m_currentCursor -= _cursorDelta * 2.0f;
                if (m_currentCursor < 0.0f)
                    m_currentCursor = 0.0f;
            }
            else
            {
                m_currentCursor = 0.0f;
            }
        }           
    }


    //------------------------------------------------------------------------------------------------------------------
    //											        SHAKE GENERATOR
    //------------------------------------------------------------------------------------------------------------------

    ShakeGenerator::ShakeGenerator()
        : m_currentCursor(0.0f)
        , m_defaultValue(0.0f)
        , m_maxValue(0.0f)
        , m_shakeDuration(0.0f)
        , m_reboundCount(0)
    {

    }

    void ShakeGenerator::setup(f32 _defaultValue, f32 _maxValue, f32 _shakeDuration, u32 _reboundCount)
    {
        m_defaultValue = _defaultValue;
        m_maxValue = _maxValue;
        m_shakeDuration = _shakeDuration;
        m_reboundCount = _reboundCount;
    }

    void ShakeGenerator::reset()
    {
        m_currentCursor = 0.0f;
    }

    bbool ShakeGenerator::update(f32 _dt)
    {
        static const f32 endToleranceCursor = 0.9f;

        f32 newCursor = 1.0f;
        if(m_shakeDuration!=0.0f)
        {
            newCursor = m_currentCursor + (_dt / m_shakeDuration);
        }

        bbool needContinue = btrue;
        if(newCursor >= endToleranceCursor)
        {
            newCursor = 1.0f;
            needContinue = bfalse;
        }

        m_currentCursor = newCursor;

        return needContinue;
    }

    f32 ShakeGenerator::getValue() const
    {
        f32 amplitude = AIUtils::computeDeterministReboundAmplitudeSin(m_currentCursor, m_shakeDuration, m_reboundCount);
        f32 val = m_defaultValue + (m_maxValue - m_defaultValue) * amplitude;
        
        return val;
    }

    f32 ShakeGenerator::getCursor() const
    {
        return m_currentCursor;
    }


    //------------------------------------------------------------------------------------------------------------------
    //											        DRAG SCROLLING
    //------------------------------------------------------------------------------------------------------------------

    DragScrolling::DragScrolling()
        : m_movingTotalDelta(0.0f)
        , m_movingCurrentDelta(0.0f)
        , m_movingDuration(0.0f)
        , m_movingStart(bfalse)
        , m_movingTransitionLast(MovingTransitionType_None)
        , m_movingTransitionType(MovingTransitionType_None)
        , m_movingTargetDist(0.0f)
        , m_movingTargetTime(0.0f)
        , m_movingTransitionCursor(-1.0f)
        , m_movingCurrentPos(0.0f)
        , m_movingSpeedAverager(0.0f)
        , m_dragStartNorm(0.0f)
        , m_momentumFriction(2.5f)
        , m_momentumDeceleration(1.2f)
        , m_dampingBezierValA(0.0f)
        , m_dampingBezierValB(0.0f)
        , m_dampingBezierValC(0.25f)
        , m_dampingBezierValD(1.0f)
        , m_dampingOutsideFactor(3.0f)
        , m_dragBounceNorm(0.0f)
        , m_dragBounceTime(1.0f)
        , m_dragSpeedAveragerCoef(0.75f)
    {

    }

    void DragScrolling::dragReset(f32 _pos)
    {
        m_movingSpeedAverager = 0.0f;
        m_movingCurrentPos = _pos;
        m_movingTotalDelta = 0.0f;
        m_movingCurrentDelta = 0.0f;
        m_movingDuration = 0.0f;
        m_movingStart = bfalse;
        m_movingTransitionLast = MovingTransitionType_None;
        m_movingTransitionType = MovingTransitionType_None;
        m_movingTransitionCursor = -1.0f;
        m_movingTargetDist = 0.0f;
        m_movingTargetTime = 0.0f;
    }

    void DragScrolling::transitionCancel()
    {
        m_movingTransitionLast = MovingTransitionType_None;
        m_movingTransitionType = MovingTransitionType_None;
        m_movingTransitionCursor = -1.0f;
        m_movingTargetDist = 0.0f;
        m_movingTargetTime = 0.0f;
    }

    bbool DragScrolling::dragMove(f32 _pos)
    {
        f32 move = _pos - m_movingCurrentPos;
        m_movingCurrentPos = _pos;
        m_movingTransitionType = MovingTransitionType_None;
        m_movingTransitionCursor = -1.0f;
        m_movingTargetDist = 0.0f;
        m_movingTargetTime = 0.0f;
        m_movingTotalDelta += move;

        if(m_movingStart)
        {
            m_movingCurrentDelta = move;
        }
        else if( fabs(m_movingTotalDelta)>m_dragStartNorm)
        {
            m_movingStart = btrue;
        }

        return m_movingStart;
    }

    bbool DragScrolling::dragEnd(f32 _pos)
    {
        f32 move = _pos - m_movingCurrentPos;
        m_movingCurrentPos = _pos;

        if(m_movingStart)
        {
            m_movingCurrentDelta = move;
            m_movingTotalDelta += move;
            m_movingStart = bfalse;

            return btrue;
        }

        m_movingSpeedAverager = 0.0f;
        m_movingCurrentDelta = 0.0f;
        m_movingTotalDelta = 0.0f;

        return bfalse;
    }

    bbool DragScrolling::dragIsStarted() const
    {
        return m_movingStart;
    }

    bbool DragScrolling::transitionIsJustEnd() const
    {
        bbool isJustEnd = m_movingTransitionLast!=MovingTransitionType_None && m_movingTransitionType==MovingTransitionType_None && m_movingTransitionCursor == 1.0f;

        return isJustEnd;
    }

    bbool DragScrolling::transitionIsProgressing() const
    {
        return m_movingTransitionType!=MovingTransitionType_None;
    }

    f32 DragScrolling::dragUpdate(f32 _currPos, f32 _minPos, f32 _maxPos, f32 _snapDist, f32 _dt)
    {
        f32 newPos = _currPos;
        MovingTransitionType transitionTypeLast = m_movingTransitionType;

        // Damping transition
        if(m_movingTransitionType != MovingTransitionType_None)
        {
            f32 lastDamping, newDamping, addDamping;
            damping(m_movingTransitionCursor, lastDamping);
            if(m_movingTargetTime!=0.0f )
            {
                m_movingTransitionCursor = m_movingTransitionCursor + (_dt / m_movingTargetTime);
            }
            else
            {
                m_movingTransitionCursor = 1.0f;
            }

            if(m_movingTransitionCursor>= 1.0f)
            {
                m_movingTransitionType = MovingTransitionType_None;
                m_movingTransitionCursor = 1.0f;
            }

            damping(m_movingTransitionCursor, newDamping);
            addDamping = fabs(newDamping - lastDamping) * m_movingTargetDist;

            newPos = _currPos + addDamping;

            if(m_movingTransitionCursor==1.0f)
            {
                newPos = f32_Round(newPos);
            }
        }
        // Launch
        else if(!m_movingStart && f32_Abs(m_movingSpeedAverager)>0.01f)
        {
            if(_currPos>_minPos && _currPos<_maxPos)
            {
                f32 maxDistLower = fabs(_minPos - _currPos) + m_dragBounceNorm;
                f32 maxDistUpper = fabs(_maxPos - _currPos) + m_dragBounceNorm;

                if(_snapDist==0.0f)
                {
                    momentumSpeed(m_movingSpeedAverager, maxDistLower, maxDistUpper, m_movingTargetDist, m_movingTargetTime);
                }
                else
                {
                    f32 snapTargetDist = 0;
                    if(m_movingSpeedAverager<0)
                    {
                        snapTargetDist = (f32_Floor(_currPos / _snapDist) * _snapDist) - _currPos;
                    }
                    else
                    {
                        snapTargetDist =  (f32_Ceil(_currPos / _snapDist) * _snapDist) - _currPos;
                    }

                    momentumDist(snapTargetDist, maxDistLower, maxDistUpper, m_movingTargetDist, m_movingTargetTime);
                }

                m_movingTransitionCursor = 0.0f;
                m_movingTransitionType = MovingTransitionType_LaunchTo;
            }

            m_movingSpeedAverager = 0.0f;
            m_movingCurrentDelta = 0.0f;
            m_movingTotalDelta = 0.0f;
        }
        // Bounce
        else if(!m_movingStart) 
        {
            MovingTransitionType transitionType = MovingTransitionType_InstantBack;
            f32 cursor = 0.0f;
            f32 factor = 1.0f;

            if(m_movingTransitionLast != MovingTransitionType_None)
            {
                transitionType = MovingTransitionType_BounceBack;
                cursor = -1.0f;
                factor = 0.5f;
            }

            if(_currPos<_minPos)
            {
                m_movingTransitionType = transitionType;
                m_movingTransitionCursor = cursor;
                m_movingTargetDist = (_minPos - _currPos) * factor;
                m_movingTargetTime = m_dragBounceTime * factor;
            }
            else if(_currPos>_maxPos)
            {
                m_movingTransitionType = transitionType;
                m_movingTransitionCursor = cursor;
                m_movingTargetDist = (_maxPos - _currPos) * factor;
                m_movingTargetTime = m_dragBounceTime * factor;
            }
            else if(_snapDist!=0)
            {
                f32 snapTargetDist = (f32_Round(_currPos / _snapDist) * _snapDist) - _currPos;
                if(f32_Abs(snapTargetDist)>0.01f)
                {
                    m_movingTransitionType = transitionType;
                    m_movingTransitionCursor = cursor;
                    m_movingTargetDist = snapTargetDist * factor;
                    m_movingTargetTime = m_dragBounceTime * factor;
                }
            }
        }
        // Drag
        else
        {
            if(_dt!=0.0f)
            {
                m_movingSpeedAverager = m_movingSpeedAverager * (1.0f - m_dragSpeedAveragerCoef) + (m_movingCurrentDelta / _dt) * m_dragSpeedAveragerCoef;
            }

            newPos = _currPos + m_movingCurrentDelta;

            if(m_dragBounceNorm!=0.0f)
            {
                if(newPos<_minPos)
                {
                    f32 dampingOffset = ((_currPos - _minPos) * m_dampingOutsideFactor + m_movingCurrentDelta) / m_dampingOutsideFactor;
                    newPos = _minPos + dampingOffset;
                }
                else if(newPos>_maxPos)
                {
                    f32 dampingOffset = ((_currPos - _maxPos) * m_dampingOutsideFactor + m_movingCurrentDelta) / m_dampingOutsideFactor;
                    newPos = _maxPos + dampingOffset;
                }
            }
        }

        m_movingTransitionLast = transitionTypeLast;
        m_movingDuration += _dt;

        return newPos;
    }

    void DragScrolling::dragTo(f32 _currPos, f32 _wantedPos)
    {
        f32 targetDist = _wantedPos - _currPos;
        f32 abstargetDist = f32_Abs(targetDist);

        momentumDist(targetDist, abstargetDist, abstargetDist, m_movingTargetDist, m_movingTargetTime);
        m_movingTransitionCursor = 0.0f;
        m_movingTransitionType = MovingTransitionType_DragTo;
    }

    void DragScrolling::momentumSpeed(f32 _speed, f32 _maxDistLower, f32 _maxDistUpper, f32& _newDist, f32& _newTime) const
    {
        f32 friction = m_momentumFriction;
        f32 deceleration = m_momentumDeceleration ;
        f32 speed = fabs(_speed);
        _newDist = speed * speed / friction / 1000;

        if (_speed > 0 && _newDist > _maxDistUpper) 
        {
            speed = speed * _maxDistUpper / _newDist;
            _newDist = _maxDistUpper;
        } 
        else if (_speed < 0 && _newDist > _maxDistLower) 
        {
            speed = speed * _maxDistLower / _newDist;
            _newDist = _maxDistLower;
        }

        _newDist = _newDist * (_speed < 0 ? -1 : 1);
        _newTime = speed / (deceleration * 1000);
    }

    void DragScrolling::momentumDist(f32 _dist, f32 _maxDistLower, f32 _maxDistUpper, f32& _newDist, f32& _newTime) const
    {
        f32 speed = f32_Sqrt(f32_Abs(_dist) * 1000 * m_momentumFriction) *  (_dist < 0 ? -1 : 1);

        momentumSpeed(speed, _maxDistLower, _maxDistUpper, _newDist, _newTime);
    }

    void DragScrolling::damping(f32 _cursor, f32& _res) const
    {
        if(_cursor<0.0f)
        {
            _cursor = 0.0f - _cursor;
        }
        _cursor = 1.0f - _cursor;
        _res = pow(1-_cursor, 3)*m_dampingBezierValA + 3*pow(1-_cursor, 2)*_cursor*m_dampingBezierValB + 3*(1-_cursor)*_cursor*_cursor*m_dampingBezierValC + pow(_cursor,3)*m_dampingBezierValD;
    }
}
