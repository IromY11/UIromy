#include "precompiled_engine.h"

#ifndef _ITF_INGAMECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/InGameCameraComponent.h"
#endif //_ITF_INGAMECAMERACOMPONENT_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

namespace ITF
{
    static const f32 deltaContinuity = 0.25f;

    void InGameCameraComponent::resetConstraintContinuity()
    {
        m_leftConstraintContinuity    = 0.f;
        m_rightConstraintContinuity   = 0.f;
        m_topConstraintContinuity     = 0.f;
        m_bottomConstraintContinuity  = 0.f;
        m_isConstraintContinuityAxeX  = bfalse;
        m_isConstraintContinuityAxeY  = bfalse;
    }

    void InGameCameraComponent::setConstraintContinuity()
    {
        if ( !m_constraintModifier )
        {
            resetConstraintContinuity();
            return;
        }
                
        m_isConstraintContinuityAxeX  = bfalse;
        m_isConstraintContinuityAxeY    = bfalse;

        const AABB aabbOld = m_constraintModifier->getModifierAABBCur();
        const CamModifierUpdate& cmOld = m_constraintModifier->getUpdateData();

        const AABB aabbNew = m_cameraModifier->getModifierAABBCur();
        const CamModifierUpdate& cmNew = m_cameraModifier->getUpdateData();

        if ( cmOld.m_constraintLeftIsActive && cmNew.m_constraintLeftIsActive && cmOld.m_constraintRightIsActive && cmNew.m_constraintRightIsActive )
        {
            if ( cmOld.m_isLockedAxeX == cmNew.m_isLockedAxeX )
            {
                const f32 deltaLeft = aabbOld.getMin().x() - aabbNew.getMin().x();
                const f32 deltaRight = aabbOld.getMax().x() - aabbNew.getMax().x();

                if ( f32_Abs(deltaLeft) < deltaContinuity && f32_Abs(deltaRight) < deltaContinuity )
                {
                    m_isConstraintContinuityAxeX = btrue;
                    m_leftConstraintContinuity += deltaLeft;
                    m_rightConstraintContinuity += deltaRight;
                }
            }
        }

        if ( cmOld.m_constraintBottomIsActive && cmNew.m_constraintBottomIsActive && cmOld.m_constraintTopIsActive && cmNew.m_constraintTopIsActive )
        {
            if ( cmOld.m_isLockedAxeY == cmNew.m_isLockedAxeY )
            {
                const f32 deltaBottom = aabbOld.getMin().y() - aabbNew.getMin().y();
                const f32 deltaTop = aabbOld.getMax().y() - aabbNew.getMax().y();

                if ( f32_Abs(deltaBottom) < deltaContinuity && f32_Abs(deltaTop) < deltaContinuity )
                {
                    m_isConstraintContinuityAxeY = btrue;
                    m_bottomConstraintContinuity += deltaBottom;
                    m_topConstraintContinuity += deltaTop;
                }
            }
        }

        if ( !m_isConstraintContinuityAxeX )
        {
            m_leftConstraintContinuity = 0.f;
            m_rightConstraintContinuity = 0.f;
        }

        if ( !m_isConstraintContinuityAxeY )
        {
            m_bottomConstraintContinuity = 0.f;
            m_topConstraintContinuity = 0.f;
        }        
    }

    void InGameCameraComponent::setConstraintOnEnter(CameraModifierComponent* _modifier)
    {
        setConstraintContinuity();
        
        m_constraintModifier = _modifier;        
        m_constraintModifierOnEnter = btrue;   

        setConstraintTimeOnEnter();   
    }

    void InGameCameraComponent::setConstraintOnExit()
    {
        if ( m_constraintModifier )
            m_constraintModifierOnExit = btrue;

        m_constraintModifier = NULL;

        resetConstraintContinuity();
    }

    void InGameCameraComponent::setModifierConstraint()
    {     
        if ( m_constraintModifier == m_cameraModifier )
            return;
        
        if ( m_constraintModifier && !m_cameraModifier->isConstraint() )
            if ( m_constraintModifier->getModifierAABBDest().contains(m_cameraModifier->getModifierAABBDest(), 1.f) )
                return;

        if ( m_cameraModifier->isConstraint() )
        {
            setConstraintOnEnter(m_cameraModifier);        
        }
        else
        {            
            if ( getCameraControllerManager()->isInGameCameraJustReseted() )
            {
                CameraModifierComponent* cameraModifierConstraint = (CameraModifierComponent*)getCameraControllerManager()->getCameraModifierComponent( m_subjectGlobalAABB, m_subjectGlobalDepthMin, btrue);
                if ( cameraModifierConstraint )
                {
                    cameraModifierConstraint->getUpdateData().getModifierList(m_CMDestFinal); // init m_CMDestFinal from constraint modifier
                    m_cameraModifier->getUpdateData().getModifierList(m_CMDestFinal); // set m_CMDestFinal from current modifier

                    setConstraintOnEnter(cameraModifierConstraint); 
                }
                else
                {
                    setConstraintOnExit();
                }
            }
            else
            {
                setConstraintOnExit();
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    /// Constraints   
    void InGameCameraComponent::resetConstraints()
    {
        m_constraint = Vec3d::Zero;
        m_constraintModifier = NULL;
        m_constraintDepthOffsetMax = F32_INFINITY;
        m_constraintSecretAreaZ = 0.f;
        m_constraintSecretAreaZOld = 0.f;
        
        resetConstraintContinuity();
    }

    f32 InGameCameraComponent::getConstraintAnticipationX()
    {
        return m_constraintAABB.getMax().x() -m_constraintAnticipAABB.getMax().x();
    }

    f32 InGameCameraComponent::getConstraintAnticipationY()
    {
        return m_constraintAABB.getMax().y() -m_constraintAnticipAABB.getMax().y();
    }

    void InGameCameraComponent::setConstraintX()
    {           
        f32 dest = 0.f;

        if ( m_CMDestFinal.m_constraintLeftIsActive && m_CMDestFinal.m_constraintRightIsActive 
            && ( m_screenConstraintAABB.getWidth() > m_constraintAABB.getWidth() || m_constraintMatchViewX || m_CMDestFinal.m_isLockedAxeX ) )
        {          
            dest = m_constraintAnticipAABB.getCenter().x() -m_screenConstraintAABB.getCenter().x();                         
        }
        else 
        {
            if ( m_CMDestFinal.m_constraintLeftIsActive && m_screenConstraintAABB.getMin().x() < m_constraintAnticipAABB.getMin().x() )
            {
                f32 delta = m_constraintAnticipAABB.getMin().x() -m_screenConstraintAABB.getMin().x();            
                f32 anticip = getConstraintAnticipationX();

                if ( delta <= anticip*2.f )                
                {
                    f32 coeff = Min( delta /(anticip*4.f), 0.5f);
                    dest = delta *coeff;                
                }
                else
                {
                    dest = m_constraintAABB.getMin().x() -m_screenConstraintAABB.getMin().x();
                }
            }
            else if ( m_CMDestFinal.m_constraintRightIsActive && m_screenConstraintAABB.getMax().x() > m_constraintAnticipAABB.getMax().x() )
            {
                f32 delta = m_screenConstraintAABB.getMax().x() -m_constraintAnticipAABB.getMax().x();
                f32 anticip = getConstraintAnticipationX();

                if ( delta <= anticip*2.f )
                {
                    f32 coeff = Min( delta /(anticip*4.f), 0.5f);
                    dest = -delta *coeff;
                }     
                else
                {                
                    dest = m_constraintAABB.getMax().x() -m_screenConstraintAABB.getMax().x();
                }
            }     
        }

        m_constraint.x() = dest;
    }

    void InGameCameraComponent::setConstraintY()
    {           
        f32 dest = 0.f;

        if ( m_CMDestFinal.m_constraintBottomIsActive && m_CMDestFinal.m_constraintTopIsActive 
            && ( m_screenConstraintAABB.getHeight() > m_constraintAABB.getHeight() || m_constraintMatchViewY || m_CMDestFinal.m_isLockedAxeY ) )
        {          
            dest = m_constraintAnticipAABB.getCenter().y() -m_screenConstraintAABB.getCenter().y();                         
        }
        else 
        {
            if ( m_CMDestFinal.m_constraintBottomIsActive && m_screenConstraintAABB.getMin().y() < m_constraintAnticipAABB.getMin().y() )
            {
                f32 delta = m_constraintAnticipAABB.getMin().y() -m_screenConstraintAABB.getMin().y();            
                f32 anticip = getConstraintAnticipationY();

                if ( delta <= anticip*2.f )
                {
                    f32 coeff = Min( delta /(anticip*4.f), 0.5f);
                    dest =  delta *coeff;
                }
                else
                {
                    dest = m_constraintAABB.getMin().y() -m_screenConstraintAABB.getMin().y();
                }
            }
            else if ( m_CMDestFinal.m_constraintTopIsActive && m_screenConstraintAABB.getMax().y() > m_constraintAnticipAABB.getMax().y() )
            {
                f32 delta = m_screenConstraintAABB.getMax().y() -m_constraintAnticipAABB.getMax().y();
                f32 anticip = getConstraintAnticipationY();

                if ( delta <= anticip*2.f )
                {
                    f32 coeff = Min( delta /(anticip*4.f), 0.5f);
                    dest = -delta *coeff;
                }     
                else
                {                
                    dest = m_constraintAABB.getMax().y() -m_screenConstraintAABB.getMax().y();
                }
            }     
        }

        m_constraint.y() = dest;
    }

    void InGameCameraComponent::setConstraintSecretAreaZ(f32 _screenRatio)
    {
        Vec2d extensionArea = m_constraintModifier->getExtendedOffsetMin() +m_constraintModifier->getExtendedOffsetMax();
        m_constraintSecretAreaZ = Max( extensionArea.x() *_screenRatio, extensionArea.y() *0.5f);
    }

    void InGameCameraComponent::setConstraintZ()
    {        
        f32 tanFocalBy2 = f32_Abs(tan(m_CMCur.m_focale*0.5f));
        if ( tanFocalBy2== 0.f )
        {
            resetConstraintZ();
            return;
        }

        AABB modifierAABB = m_constraintModifier->GetActor()->getAABB();
        addContinuityOffset(modifierAABB);

        f32 tanFocaleBy2Inv = 1.f /tanFocalBy2;
        f32 depthDest = F32_INFINITY;         
        f32 screenRatio = 0.5f;
        if( getCameraControllerManager()->m_screenRatio )
            screenRatio /= getCameraControllerManager()->m_screenRatio;

        if ( m_CMDestFinal.m_constraintLeftIsActive && m_CMDestFinal.m_constraintRightIsActive )
        {
            m_constraintDepthOffsetMax = modifierAABB.getWidth() *screenRatio;

            if ( m_screenTargetAverageAABB.getWidth() > m_constraintAABB.getWidth() || m_constraintMatchViewX )
            {
                depthDest = ( m_constraintAABB.getWidth() -m_screenTargetAverageAABB.getWidth() ) *screenRatio;
            }
        }

        if ( m_CMDestFinal.m_constraintTopIsActive && m_CMDestFinal.m_constraintBottomIsActive )
        { 
            m_constraintDepthOffsetMax = Min( m_constraintDepthOffsetMax, modifierAABB.getHeight() *0.5f);

            if ( m_screenTargetAverageAABB.getHeight() > m_constraintAABB.getHeight() || m_constraintMatchViewY )
            {     
                depthDest = Min( depthDest, ( m_constraintAABB.getHeight() -m_screenTargetAverageAABB.getHeight() ) *0.5f );                   
            }
        }        

        if ( depthDest != F32_INFINITY )
        {
            depthDest *= tanFocaleBy2Inv;

            m_constraint.z() = depthDest;
        }
        else
        {
            resetConstraintZ();
        }        

        // set depth offset max
        m_constraintDepthOffsetMax *= tanFocaleBy2Inv;

        // set secret area
        setConstraintSecretAreaZ(screenRatio);
        m_constraintSecretAreaZ *= tanFocaleBy2Inv;
    }

    void InGameCameraComponent::setConstraintMatchView()
    {
        m_constraintMatchViewX = bfalse;
        m_constraintMatchViewY = bfalse;

        if ( !m_CMDestFinal.m_constraintMatchView )
            return;

        const f32 constraintHeight = m_constraintAABB.getHeight();

        if ( constraintHeight <= 0.f )
            return;

        const f32 screenRatioModifier = m_constraintAABB.getWidth() /constraintHeight;
        const f32 deltaRatio = getCameraControllerManager()->m_screenRatio -screenRatioModifier;

        if ( deltaRatio > -MTH_EPSILON && deltaRatio < MTH_EPSILON )
        {
            m_constraintMatchViewX = btrue;
            m_constraintMatchViewY = btrue;
        }
        else if ( deltaRatio > 0.f )
        {
            m_constraintMatchViewX = btrue;
        }
        else
        {
            m_constraintMatchViewY = btrue;
        } 
    }

    void InGameCameraComponent::addContinuityOffset(AABB& _aabb) const
    {
        _aabb.setMinX( _aabb.getMin().x() + m_leftConstraintContinuity);
        _aabb.setMaxX( _aabb.getMax().x() + m_rightConstraintContinuity);
        _aabb.setMinY( _aabb.getMin().y() + m_bottomConstraintContinuity);
        _aabb.setMaxY( _aabb.getMax().y() + m_topConstraintContinuity);
    }

    void InGameCameraComponent::setConstraintAABB()
    {
        // cur constraint aabb
        m_constraintAABB = m_constraintModifier->getModifierAABBCur();
        addContinuityOffset(m_constraintAABB);

        // anticip constraint aabb
        Vec2d anticipOffset;
        setScreenAABBLongBy2( anticipOffset, m_CMCur.m_cameraLookAtOffset.z() );
        anticipOffset *= 0.5f;
//#ifdef ITF_PROJECT_RABBIDS
		anticipOffset = Vec2d::Zero;
//#endif //ITF_PROJECT_RABBIDS

        m_constraintAnticipAABB.setMin( m_constraintAABB.getMin() +anticipOffset);        
        m_constraintAnticipAABB.setMax( m_constraintAABB.getMax() -anticipOffset);

        // adjust width with screen AABB
        if ( m_CMDestFinal.m_constraintLeftIsActive && m_CMDestFinal.m_constraintRightIsActive )
        {
            if ( m_screenConstraintAABB.getWidth() > m_constraintAnticipAABB.getWidth() )
            {
                f32 deltaBy2 = m_screenConstraintAABB.getWidth() -m_constraintAnticipAABB.getWidth();
                deltaBy2 = Min( deltaBy2, m_constraintAABB.getWidth() -m_constraintAnticipAABB.getWidth());
                deltaBy2 *= 0.5f;

                m_constraintAnticipAABB.setMin( Vec2d( m_constraintAnticipAABB.getMin().x() -deltaBy2, m_constraintAnticipAABB.getMin().y()) );
                m_constraintAnticipAABB.setMax( Vec2d( m_constraintAnticipAABB.getMax().x() +deltaBy2, m_constraintAnticipAABB.getMax().y()) );
            }
        }

        // adjust height with screen AABB
        if ( m_CMDestFinal.m_constraintTopIsActive && m_CMDestFinal.m_constraintBottomIsActive )
        {
            if ( m_screenConstraintAABB.getHeight() > m_constraintAnticipAABB.getHeight() )
            {
                f32 deltaBy2 = m_screenConstraintAABB.getHeight() -m_constraintAnticipAABB.getHeight();
                deltaBy2 = Min( deltaBy2, m_constraintAABB.getHeight() -m_constraintAnticipAABB.getHeight());
                deltaBy2 *= 0.5f;

                m_constraintAnticipAABB.setMin( Vec2d( m_constraintAnticipAABB.getMin().x(), m_constraintAnticipAABB.getMin().y() -deltaBy2) );
                m_constraintAnticipAABB.setMax( Vec2d( m_constraintAnticipAABB.getMax().x(), m_constraintAnticipAABB.getMax().y() +deltaBy2) );
            }
        }

        setConstraintMatchView();
    }

    void InGameCameraComponent::setConstraintTimeOnEnter()
    {    
        setConstraintAABB();      
    }

    void InGameCameraComponent::resetConstraintX()
    {
        m_constraint.x() = 0.0f;
    }

    void InGameCameraComponent::resetConstraintY()
    {
        m_constraint.y() = 0.0f;
    }

    void InGameCameraComponent::resetConstraintZ()
    {
        m_constraint.z() = 0.0f;
    }

    void InGameCameraComponent::setScreenConstraintAABB()
    {
        Vec2d screenTargetPos = m_cameraTargetAveragePos.truncateTo2D();
        Vec2d offset( m_screenRealAABB.getWidth() *0.5f, m_screenRealAABB.getHeight() *0.5f);
        m_screenConstraintAABB.setMin( screenTargetPos -offset);
        m_screenConstraintAABB.setMax( screenTargetPos +offset);
    }

    void InGameCameraComponent::testConstraintExtendedAxeX()
    {
        // left
        f32 deltaLeft = 0.f;
        if ( m_CMDestFinal.m_constraintLeftIsActive && m_constraintModifier->isConstraintExtendedLeft() )
        {
            u32 controllersListCount = m_controllers.size();
            for ( u32 i=0; i<controllersListCount; i++)
            {
                const Controller& controller = m_controllers[i];

                if ( controller.isActive() || controller.m_isPaused )
                {
                    deltaLeft = Min( deltaLeft, controller.m_subjectAABB.getMin().x() -m_constraintModifier->GetActor()->getAABB().getMin().x());
                }
            }            
        }
        if ( deltaLeft <0.f)
        {
            m_constraintModifier->setConstraintExtendedLeft(-deltaLeft);

            if ( getCameraControllerManager()->isInGameCameraJustReseted() )
            {
                m_constraintModifier->setConstraintExtendedLeftToDest();
                setConstraintAABB();
            }
        }

        // right
        f32 deltaRight = 0.f;
        if ( m_CMDestFinal.m_constraintRightIsActive && m_constraintModifier->isConstraintExtendedRight() )
        {
            u32 controllersListCount = m_controllers.size();
            for ( u32 i=0; i<controllersListCount; i++)
            {
                const Controller& controller = m_controllers[i];

                if ( controller.isActive() || controller.m_isPaused )
                {
                    deltaRight = Max(deltaRight, controller.m_subjectAABB.getMax().x() -m_constraintModifier->GetActor()->getAABB().getMax().x());                 
                }
            }            
        }
        if ( deltaRight >0.f)
        {
            m_constraintModifier->setConstraintExtendedRight(deltaRight);

            if ( getCameraControllerManager()->isInGameCameraJustReseted() )
            {
                m_constraintModifier->setConstraintExtendedRightToDest();
                setConstraintAABB();
            }
        }
    }

    void InGameCameraComponent::testConstraintExtendedAxeY()
    {
        // bottom
        f32 deltaBottom = 0.f;
        if ( m_CMDestFinal.m_constraintBottomIsActive && m_constraintModifier->isConstraintExtendedBottom() )
        {
            u32 controllersListCount = m_controllers.size();
            for ( u32 i=0; i<controllersListCount; i++)
            {
                const Controller& controller = m_controllers[i];

                if ( controller.isActive() || controller.m_isPaused )
                {
                    deltaBottom = Min( deltaBottom, controller.m_subjectAABB.getMin().y() -m_constraintModifier->GetActor()->getAABB().getMin().y());
                }
            }           
        }
        if ( deltaBottom <0.f )
        {
            m_constraintModifier->setConstraintExtendedBottom(-deltaBottom); 

            if ( getCameraControllerManager()->isInGameCameraJustReseted() )
            {
                m_constraintModifier->setConstraintExtendedBottomToDest();
                setConstraintAABB();
            }
        }

        // top
        f32 deltaTop = 0.f;
        if ( m_CMDestFinal.m_constraintTopIsActive && m_constraintModifier->isConstraintExtendedTop() )
        {
            u32 controllersListCount = m_controllers.size();
            for ( u32 i=0; i<controllersListCount; i++)
            {
                const Controller& controller = m_controllers[i];

                if ( controller.isActive() || controller.m_isPaused )
                {
                    deltaTop = Max(deltaTop, controller.m_subjectAABB.getMax().y() -m_constraintModifier->GetActor()->getAABB().getMax().y());  
                }
            }            
        }
        if ( deltaTop >0.f)
        {
            m_constraintModifier->setConstraintExtendedTop(deltaTop); 

            if ( getCameraControllerManager()->isInGameCameraJustReseted() )
            {
                m_constraintModifier->setConstraintExtendedTopToDest();
                setConstraintAABB();
            }
        }
     }

    void InGameCameraComponent::initConstraintParams()
    {
        m_constraintDepthOffsetMax = F32_INFINITY;
        m_constraintSecretAreaZOld = m_constraintSecretAreaZ;
        m_constraintSecretAreaZ = 0.f;

        if ( m_constraintModifierOnEnter || m_constraintModifierOnExit )
        {
            m_constraintSecretAreaZOld = 0.f;
        }
    }

    void InGameCameraComponent::updateConstraints()
    {
        initConstraintParams();

        if( m_constraintModifier )
        {
            ITF_WARNING_CATEGORY(GPP, GetActor(), m_CMDestFinal.m_constraintLeftIsActive || m_CMDestFinal.m_constraintRightIsActive || m_CMDestFinal.m_constraintTopIsActive || m_CMDestFinal.m_constraintBottomIsActive, 
                "No camera constraints set on \"%s\" (%s)", 
                m_constraintModifier->GetActor()->getUserFriendly().cStr(), 
                m_constraintModifier->GetActor()->getInstanceDataFile().toString8().cStr());

            if(!m_CMDestFinal.m_constraintLeftIsActive && !m_CMDestFinal.m_constraintRightIsActive && !m_CMDestFinal.m_constraintTopIsActive && !m_CMDestFinal.m_constraintBottomIsActive)
                setConstraintOnExit();
        }

        if ( m_constraintModifier )
        {   
            // set screen to apply constraint
            setScreenConstraintAABB();

            // set constraint anticip AABB
            setConstraintAABB();
           
            // axe x
            if ( m_CMDestFinal.m_constraintLeftIsActive || m_CMDestFinal.m_constraintRightIsActive )
            {
                testConstraintExtendedAxeX();
                setConstraintX();
            }
            else
            {
                resetConstraintX();
            }

            // axe y
            if ( m_CMDestFinal.m_constraintTopIsActive || m_CMDestFinal.m_constraintBottomIsActive )
            {
                testConstraintExtendedAxeY();
                setConstraintY();
            }
            else
            {
                resetConstraintY();
            }

            setConstraintZ();
        }
        else
        {
            resetConstraintX();
            resetConstraintY();
            resetConstraintZ();
        }
    }

}// namespace ITF
