#include "precompiled_engine.h"

#ifndef _ITF_INGAMECAMERAPARAMETERS_H_
#include "engine/actors/components/Camera/InGameCameraParameters.h"
#endif

namespace ITF
{
    BEGIN_SERIALIZATION(ConstraintExtended)        
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("offset", m_offset);
            SERIALIZE_MEMBER("timeToIncrease", m_timeToIncrease);
            SERIALIZE_MEMBER("timeToWaitBeforeDecrease", m_timeToWaitBeforeDecrease);
            SERIALIZE_MEMBER("timeToDecrease", m_timeToDecrease);            
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(CamModifier)  
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("modifierPriority", m_cameraModifierPriority);
            SERIALIZE_MEMBER("lookAtOffset", m_lookAt);
            SERIALIZE_MEMBER("lookAtOffsetMax", m_lookAtMax);
            SERIALIZE_MEMBER("blendingZoneStart", m_blendingZoneStart);
            SERIALIZE_MEMBER("blendingZoneStop", m_blendingZoneStop);                       
            SERIALIZE_MEMBER("zoneNeutral", m_zoneNeutral);
            SERIALIZE_MEMBER("useDecentering", m_useDecentering);             
            SERIALIZE_MEMBER("rotationAngle", m_rotationAngle);
            SERIALIZE_MEMBER("rotationSpeed", m_rotationSpeed);
			SERIALIZE_MEMBER("rotationUpDownAngle", m_UpDnAngle);
			SERIALIZE_MEMBER("transitionTime", m_transitionTime);
			SERIALIZE_MEMBER("moveInertia", m_moveInertia);
			SERIALIZE_MEMBER("horizontalVersusVertical", m_horizontalVersusVertical);
			SERIALIZE_MEMBER("offsetHVS", m_offsetHVS);
            
            SERIALIZE_ENUM_BEGIN("flipView", m_flipView);
                SERIALIZE_ENUM_VAR(CameraFlip_None);
                SERIALIZE_ENUM_VAR(CameraFlip_X);
                SERIALIZE_ENUM_VAR(CameraFlip_Y);
                SERIALIZE_ENUM_VAR(CameraFlip_Both);
            SERIALIZE_ENUM_END();

            SERIALIZE_MEMBER("constraintLeftIsActive", m_constraintLeftIsActive);
            SERIALIZE_MEMBER("constraintRightIsActive", m_constraintRightIsActive);
            SERIALIZE_MEMBER("constraintTopIsActive", m_constraintTopIsActive);
            SERIALIZE_MEMBER("constraintBottomIsActive", m_constraintBottomIsActive); 
            SERIALIZE_MEMBER("constraintMatchView", m_constraintMatchView);
            SERIALIZE_OBJECT("constraintExtendedLeft", m_constraintExtendedLeft);
            SERIALIZE_OBJECT("constraintExtendedRight", m_constraintExtendedRight);
            SERIALIZE_OBJECT("constraintExtendedTop", m_constraintExtendedTop);
            SERIALIZE_OBJECT("constraintExtendedBottom", m_constraintExtendedBottom);    
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(CamModifier_Template)
        SERIALIZE_MEMBER("cameraLookAtOffsetYUp", m_cameraLookAtOffsetYUp); 
        SERIALIZE_MEMBER("cameraLookAtOffsetYDown", m_cameraLookAtOffsetYDown);
        SERIALIZE_MEMBER("cameraLookAtOffset", m_cameraLookAtOffset); 
        SERIALIZE_MEMBER("cameraLookAtOffsetMaxInMulti", m_cameraLookAtOffsetMaxInMulti);

        SERIALIZE_MEMBER("focale", m_focale);
        SERIALIZE_MEMBER("ignoreZ", m_ignoreZ);                

        SERIALIZE_MEMBER("zoneScaleDepthMin", m_zoneScaleDepthMin);
        SERIALIZE_MEMBER("zoneScaleDepthMax", m_zoneScaleDepthMax);
        SERIALIZE_MEMBER("zoneScaleAtDepthMin", m_zoneScaleAtDepthMin);
        SERIALIZE_MEMBER("zoneScaleAtDepthMax", m_zoneScaleAtDepthMax);
        SERIALIZE_MEMBER("zoneScaleSpeed", m_zoneScaleSpeed);
        
        SERIALIZE_MEMBER("cameraDecenteringOffsetAtSpeedMin", m_cameraDecenteringOffsetAtSpeedMin);
        SERIALIZE_MEMBER("cameraDecenteringOffsetAtSpeedMax", m_cameraDecenteringOffsetAtSpeedMax);
        SERIALIZE_MEMBER("cameraDecenteringSpeedMin", m_cameraDecenteringSpeedMin);
        SERIALIZE_MEMBER("cameraDecenteringSpeedMax", m_cameraDecenteringSpeedMax);
        SERIALIZE_MEMBER("cameraDecenteringBlendAtSpeedMin", m_cameraDecenteringBlendAtSpeedMin);
        SERIALIZE_MEMBER("cameraDecenteringBlendAtSpeedMax", m_cameraDecenteringBlendAtSpeedMax);
        SERIALIZE_MEMBER("cameraDecenteringInertieAtSpeedMin", m_cameraDecenteringInertieAtSpeedMin);
        SERIALIZE_MEMBER("cameraDecenteringInertieAtSpeedMax", m_cameraDecenteringInertieAtSpeedMax);
        SERIALIZE_MEMBER("cameraDecenteringSmooth", m_cameraDecenteringSmooth);
        SERIALIZE_MEMBER("cameraDecenteringSubjectLookDirWeight", m_cameraDecenteringSubjectLookDirWeight);        

        SERIALIZE_MEMBER("cameraDecenteringDepthMin", m_cameraDecenteringDepthMin);
        SERIALIZE_MEMBER("cameraDecenteringDepthMax", m_cameraDecenteringDepthMax);

        SERIALIZE_MEMBER("cameraMovingSpeedMin", m_cameraMovingSpeedMin);    
        SERIALIZE_MEMBER("cameraMovingSpeedMax", m_cameraMovingSpeedMax);    
        SERIALIZE_MEMBER("cameraMovingBlendAtSpeedMin", m_cameraMovingBlendAtSpeedMin);    
        SERIALIZE_MEMBER("cameraMovingBlendAtSpeedMax", m_cameraMovingBlendAtSpeedMax);    
        SERIALIZE_MEMBER("cameraMovingSmooth", m_cameraMovingSmooth);    
        SERIALIZE_MEMBER("cameraMovingBlendAccelerationJustSticked", m_cameraMovingBlendAccelerationJustSticked);

        SERIALIZE_MEMBER("rayCastScaleMax", m_rayCastScaleMax);    

        // lock position
        SERIALIZE_MEMBER("lockPositionBlendOnEnter", m_lockPositionBlendOnEnter);
        SERIALIZE_MEMBER("lockPositionBlendOnExit", m_lockPositionBlendOnExit);

        // screen
        SERIALIZE_MEMBER("screenLimitUpSpeedMax", m_screenLimitUpSpeedMax);
        SERIALIZE_MEMBER("screenLimitUpScale", m_screenLimitUpScale);

        // multi players                  
        SERIALIZE_MEMBER("subjectWeightUnregisterDelay", m_subjectWeightUnregisterDelay);    
        SERIALIZE_MEMBER("subjectWeightRegisterDelay", m_subjectWeightRegisterDelay);    
                
        SERIALIZE_MEMBER("subjectMainVisibilityHorizontal", m_subjectMainVisibilityHorizontal);
        SERIALIZE_MEMBER("subjectMainVisibilityVertical", m_subjectMainVisibilityVertical);
        SERIALIZE_MEMBER("cameraMovingSmoothCoeffForLeader", m_cameraMovingSmoothCoeffForLeader);
        SERIALIZE_MEMBER("subjectMainVisibilitySwitchAxeSpeed", m_subjectMainVisibilitySwitchAxeSpeed);
        
        SERIALIZE_MEMBER("zoomBlendInMultiplayer", m_zoomBlendInMultiplayer);   

        SERIALIZE_MEMBER("isMainSubject", m_isMainSubject); 
        SERIALIZE_MEMBER("isMainDRCPlayer", m_isMainDRCPlayer);
        SERIALIZE_MEMBER("isLockedAxe", m_isLockedAxe); 
        SERIALIZE_MEMBER("isFixed", m_isFixed); 
        
    END_SERIALIZATION()

    void CamModifierUpdate::init( const CamModifier_Template* _cmTemplate )
    {
        m_camModifierTemplate = _cmTemplate;

        if ( m_camModifierTemplate )
        {           
            m_cameraLookAtOffsetYUp = m_camModifierTemplate->m_cameraLookAtOffsetYUp;
            m_cameraLookAtOffsetYDown = m_camModifierTemplate->m_cameraLookAtOffsetYDown;
            m_ignoreZ = m_camModifierTemplate->m_ignoreZ;
            m_isMainSubject = m_camModifierTemplate->m_isMainSubject;
            m_cameraLookAtOffset = m_camModifierTemplate->m_cameraLookAtOffset;
            m_cameraLookAtOffsetMaxInMulti = m_camModifierTemplate->m_cameraLookAtOffsetMaxInMulti;

            if ( m_camModifierTemplate->m_focale.ToRadians() > 0.f )
            {
                m_focale = m_camModifierTemplate->m_focale.ToRadians();
                m_updateModifiersListTemplate.push_back(CamModifierUpdate::update_focale);
            }
        }
    }

    void CamModifierUpdate::init( const CamModifier* _cm, const CamModifier_Template* _cmTemplate )
    {
        m_updateModifiersListTemplate.clear();
        m_updateModifiersListInstance.clear();

        init( _cmTemplate );

        m_camModifier = _cm;
        m_cameraModifierDirectionNormalized = Vec2d::Right;

        if ( m_camModifier )
        {
            m_constraintLeftIsActive = m_camModifier->m_constraintLeftIsActive;
            m_constraintRightIsActive = m_camModifier->m_constraintRightIsActive;
            m_constraintTopIsActive = m_camModifier->m_constraintTopIsActive;
            m_constraintBottomIsActive = m_camModifier->m_constraintBottomIsActive;
            m_constraintMatchView = m_camModifier->m_constraintMatchView;   

            m_cameraLookAtOffset = m_camModifier->m_lookAt;
            m_cameraLookAtOffsetMaxInMulti = m_camModifier->m_lookAtMax;

            if ( m_constraintLeftIsActive || m_constraintRightIsActive || m_constraintTopIsActive || m_constraintBottomIsActive )
            {
                m_updateModifiersListInstance.push_back(CamModifierUpdate::update_constraintLeftIsActive);
                m_updateModifiersListInstance.push_back(CamModifierUpdate::update_constraintRightIsActive);
                m_updateModifiersListInstance.push_back(CamModifierUpdate::update_constraintTopIsActive);
                m_updateModifiersListInstance.push_back(CamModifierUpdate::update_constraintBottomIsActive);
                m_updateModifiersListInstance.push_back(CamModifierUpdate::update_constraintMatchView);
            }
        }
        else
        {
            m_constraintLeftIsActive = bfalse;
            m_constraintRightIsActive = bfalse;
            m_constraintTopIsActive = bfalse;
            m_constraintBottomIsActive = bfalse;
            m_constraintMatchView = bfalse;
        }
    }

    void CamModifierUpdate::getModifierList(CamModifierUpdate& _to ) const
    {                
        _to.m_cameraModifierDirectionNormalized = m_cameraModifierDirectionNormalized;
        _to.m_isMainSubject = m_isMainSubject;
        _to.m_isLockedAxeX = m_isLockedAxeX;
        _to.m_isLockedAxeY = m_isLockedAxeY;
        _to.m_ignoreZ = m_ignoreZ;

        if ( m_camModifierTemplate )
        {
            ITF_VECTOR<ingameCameraParameterTemplateFncDecl>::const_iterator it =  m_updateModifiersListTemplate.begin();
            ITF_VECTOR<ingameCameraParameterTemplateFncDecl>::const_iterator itend =  m_updateModifiersListTemplate.end();
            for(; it != itend; ++it)
            {
                (*it)(*m_camModifierTemplate, _to);
            }
        }

        if ( m_camModifier )
        {
            _to.m_cameraLookAtOffset = m_camModifier->m_lookAt;
            _to.m_cameraLookAtOffsetMaxInMulti = m_camModifier->m_lookAtMax;
            //_to.m_constraintLeftIsActive = m_camModifier->m_constraintLeftIsActive;
            //_to.m_constraintRightIsActive = m_camModifier->m_constraintRightIsActive;
            //_to.m_constraintTopIsActive = m_camModifier->m_constraintTopIsActive;
            //_to.m_constraintBottomIsActive = m_camModifier->m_constraintBottomIsActive;

            ITF_VECTOR<ingameCameraParameterInstanceFncDecl>::const_iterator itInstance =  m_updateModifiersListInstance.begin();
            ITF_VECTOR<ingameCameraParameterInstanceFncDecl>::const_iterator itInstanceend =  m_updateModifiersListInstance.end();
            for(; itInstance != itInstanceend; ++itInstance)
            {
                (*itInstance)(*m_camModifier, _to);
            }
        }
    }

    void CamModifierUpdate::clear()
    {
        m_updateModifiersListTemplate.clear();
        m_updateModifiersListInstance.clear();
    }
}