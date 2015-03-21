#include "precompiled_gameplay.h"

#ifndef _ITF_TUTORIALTEXTCOMPONENT_H_
#include "gameplay/Components/UI/TutorialTextComponent.h"
#endif //_ITF_TUTORIALTEXTCOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/Components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#include "engine/actors/managers/UIMenuManager.h"

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(TutorialTextComponent)
BEGIN_SERIALIZATION_CHILD(TutorialTextComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("RELATIVEPOS",m_relativePos);
    END_CONDITION_BLOCK()

END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(TutorialTextComponent)

    VALIDATE_COMPONENT_PARAM("TextBoxComponent", m_textBoxComponent, "TextBoxComponent mandatory");
    VALIDATE_COMPONENT_PARAM("AnimatedComponent", m_animatedComponent, "AnimatedComponent mandatory");

END_VALIDATE_COMPONENT()

TutorialTextComponent::TutorialTextComponent()
: m_textBoxComponent(NULL)
, m_animatedComponent(NULL)
, m_relativePos(0.5f,0.5f)
, m_iconPos(Vec2d::Zero)
, m_locIdToShow(LocalisationId::Invalid)
, m_padToShow(InputAdapter::Pad_Invalid)
, m_currentLocId(LocalisationId::Invalid)
, m_currentPad(InputAdapter::Pad_Invalid)
, m_hiding(bfalse)
, m_receiveHide(bfalse)
, m_receiveShow(bfalse)
, m_receiveForceHide(bfalse)
, m_isShown(bfalse)
, m_firstUpdate(btrue)
{
}

TutorialTextComponent::~TutorialTextComponent()
{
}

void TutorialTextComponent::onActorClearComponents()
{
    Super::onActorClearComponents();

    if ( GAMEMANAGER->getTutorialMsgActor() == m_actor->getRef() )
    {
        GAMEMANAGER->setTutorialMsgActor(ITF_INVALID_OBJREF);
    }
}

void TutorialTextComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_textBoxComponent = m_actor->GetComponent<UITextBox>();
    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();

    GAMEMANAGER->setTutorialMsgActor(m_actor->getRef());

    updatePosition();
}

void TutorialTextComponent::onBecomeActive()
{
    Super::onBecomeActive();

    m_isShown = bfalse;
    m_firstUpdate = btrue;
    m_iconPos = Vec2d::Zero;
}

void TutorialTextComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    m_textBoxComponent->setLoc(LocalisationId::Invalid);
    m_isShown = bfalse;
    m_iconPos = Vec2d::Zero;

    if ( GAMEMANAGER->inTutorial() )
    {
        GAMEMANAGER->hideTutorial();
    }
}

void TutorialTextComponent::show( const LocalisationId& _id, InputAdapter::PadType _padType )
{
    if ( m_currentLocId != _id || m_currentPad != _padType )
    {
        m_receiveShow = btrue;
        m_locIdToShow = _id;
        m_padToShow = _padType;
    }

    m_receiveForceHide = bfalse;
    m_receiveHide = bfalse; 
}

bbool TutorialTextComponent::isFinishHiding() const
{
    return !m_actor->isEnabled() || ( !m_firstUpdate && !m_isShown && m_animatedComponent->isMainNodeFinished() );
}

void TutorialTextComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if ( m_receiveForceHide )
    {
        if ( m_isShown )
        {
            m_isShown = bfalse;
            m_currentLocId = LocalisationId::Invalid;
            m_currentPad = InputAdapter::Pad_Invalid;
            m_locIdToShow = LocalisationId::Invalid;
            m_padToShow = InputAdapter::Pad_Invalid;
            m_actor->disable();

            if ( GAMEMANAGER->inTutorial() )
            {
                GAMEMANAGER->hideTutorial();
            }
        }

        m_receiveForceHide = bfalse;
        return;
    }

    if ( m_animatedComponent->isMainNodeFinished() || m_firstUpdate )
    {
        if ( m_receiveShow )
        {
            m_currentLocId = m_locIdToShow;
            m_currentPad = m_padToShow;
            m_locIdToShow = LocalisationId::Invalid;
            m_padToShow = InputAdapter::Pad_Invalid;

            m_textBoxComponent->setLoc(m_currentLocId);

            if ( !m_isShown )
            {
                // show tutorial
                if ( !GAMEMANAGER->inTutorial() )
                {
                    GAMEMANAGER->showTutorial();
                }

                m_textBoxComponent->setIsDisplay(btrue);
                m_isShown = btrue;
            }

            m_receiveShow = bfalse;
        }
        else if ( m_receiveHide )
        {
            if ( m_isShown )
            {
                m_textBoxComponent->setIsDisplay(bfalse);
                m_isShown = bfalse;
                m_currentLocId = LocalisationId::Invalid;
            }
            
            m_receiveHide = bfalse;
        }
        else if ( m_locIdToShow != LocalisationId::Invalid )
        {
            m_textBoxComponent->setLoc(m_locIdToShow);
            m_currentLocId = m_locIdToShow;
            m_currentPad = m_padToShow;
            m_locIdToShow = LocalisationId::Invalid;
        }
        else if ( !m_isShown )
        {
            m_actor->disable();

            if ( GAMEMANAGER->inTutorial() )
            {
                GAMEMANAGER->hideTutorial();
            }
        }
    }

    updatePosition();
    updateIconPos();
    updateIconActor();

    m_firstUpdate = bfalse;
}

void TutorialTextComponent::updatePosition()
{
    Vec2d pos;
    f32 screenWidth = (f32)GFX_ADAPTER->getScreenWidth();
    f32 screenHeight = (f32)GFX_ADAPTER->getScreenHeight();

    pos.x() = screenWidth * m_relativePos.x();
    pos.y() = screenHeight * m_relativePos.y();

    m_actor->set2DPos(pos);

    Vec2d newScale = getTemplate()->getAnimSize();

    // TODO : screen reference in GFX_adapter
    Vec2d screenRatio = Vec2d(screenWidth/1280.0f, screenHeight/720.0f);

    newScale *= screenRatio;

    m_actor->setScale(newScale);
}

void TutorialTextComponent::updateIconPos()
{
    AnimPolyline* pPolyline = NULL;
    u32 pointIndex;
    const Vec2d* pointsBuffer = m_animatedComponent->getCurrentFirstPolylinePoint(getTemplate()->getIconPoint(), &pPolyline, &pointIndex);

    if (pointsBuffer)
    {
        Vec2d p = pointsBuffer[pointIndex];

        p.y() *= -1.f;
        
        m_iconPos = (p*m_actor->getScale()).Rotate(m_actor->getAngle()) + m_actor->get2DPos();
    }
    else
    {
        m_iconPos = Vec2d::Zero;
    }
}

void TutorialTextComponent::updateIconActor()
{
    ETutorialConfigIcon configIcon;

    switch(m_currentPad)
    {
    case InputAdapter::Pad_WiiSideWay:
        {
            configIcon = ConfigIcon_SidewayWii;
        }
        break;
    case InputAdapter::Pad_WiiNunchuk:
        {
            configIcon = ConfigIcon_NunchukWii;
        }
        break;
    case InputAdapter::Pad_WiiClassic:
        {
            configIcon = ConfigIcon_ClassicWii;
        }
        break;
    default:
        {
            configIcon = ConfigIcon_None;
        }
        break;
    }

    UIMenu* uiMenu = (UIMenu*) UI_MENUMANAGER->getUIMenu(GAMEMANAGER->getTutorialMenuId());

    if (!uiMenu)
    {
        return;
    }

    // show/hide Wii config icons
    static const String8 s_sidewayWiiIcon = "sidewayWiiIcon";
    static const String8 s_nunchukWiiIcon = "nunchukWiiIcon";
    static const String8 s_classicWiiIcon = "classicWiiIcon";
    UIComponent* sidewayWiiIcon = (UIComponent*) uiMenu->getChildComponent(s_sidewayWiiIcon);
    UIComponent* nunchukWiiIcon = (UIComponent*) uiMenu->getChildComponent(s_nunchukWiiIcon);
    UIComponent* classicWiiIcon = (UIComponent*) uiMenu->getChildComponent(s_classicWiiIcon);
    ITF_WARNING(
        m_actor,
        sidewayWiiIcon && nunchukWiiIcon && classicWiiIcon,
        "Missing Wii icon");
    if (sidewayWiiIcon && nunchukWiiIcon && classicWiiIcon)
    {
        Actor* currentIcon = NULL;
        bbool posValid = m_iconPos != Vec2d::Zero && m_actor->isEnabled();

        // sideway
        if (configIcon == ConfigIcon_SidewayWii && posValid)
        {
            currentIcon = sidewayWiiIcon->GetActor();
            sidewayWiiIcon->GetActor()->enable();
        }
        else
            sidewayWiiIcon->GetActor()->disable();

        // nunchuk
        if (configIcon == ConfigIcon_NunchukWii && posValid)
        {
            currentIcon = nunchukWiiIcon->GetActor();
            nunchukWiiIcon->GetActor()->enable();
        }
        else
            nunchukWiiIcon->GetActor()->disable();

        // classic
        if (configIcon == ConfigIcon_ClassicWii && posValid)
        {
            currentIcon = classicWiiIcon->GetActor();
            classicWiiIcon->GetActor()->enable();
        }
        else
            classicWiiIcon->GetActor()->disable();

        if ( currentIcon )
        {
            currentIcon->set2DPos(m_iconPos);
        }
    }
}

#ifdef ITF_SUPPORT_EDITOR
void TutorialTextComponent::onEditorCreated( class Actor* _original )
{
    Super::onEditorCreated(_original);

    // Calculate the relative position
    m_relativePos.x() = m_actor->getPos().x() / (f32)GFX_ADAPTER->getScreenWidth();
    m_relativePos.y() = m_actor->getPos().y() / (f32)GFX_ADAPTER->getScreenHeight();
}

void TutorialTextComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    // Calculate the relative position
    m_relativePos.x() = m_actor->getPos().x() / (f32)GFX_ADAPTER->getScreenWidth();
    m_relativePos.y() = m_actor->getPos().y() / (f32)GFX_ADAPTER->getScreenHeight();
}

#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(TutorialTextComponent_Template)
BEGIN_SERIALIZATION_CHILD(TutorialTextComponent_Template)

    SERIALIZE_MEMBER("animSize",m_animSize);
    SERIALIZE_MEMBER("iconPoint", m_iconPoint);

END_SERIALIZATION()

//------------------------------------------------------------------------------
TutorialTextComponent_Template::TutorialTextComponent_Template()
: m_animSize(300.f,300.f)
, m_iconPoint("NewFeature_Pos")
{
}

//------------------------------------------------------------------------------
TutorialTextComponent_Template::~TutorialTextComponent_Template()
{
}

} // namespace ITF
