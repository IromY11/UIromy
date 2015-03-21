#ifndef _ITF_TUTORIALTEXTCOMPONENT_H_
#define _ITF_TUTORIALTEXTCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif // _LOCALISATION_ID_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

namespace ITF {

//------------------------------------------------------------------------------
enum ETutorialConfigIcon
{
    ConfigIcon_None,
    ConfigIcon_SidewayWii,
    ConfigIcon_NunchukWii,
    ConfigIcon_ClassicWii,
};

class TutorialTextComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(TutorialTextComponent, ActorComponent,2460478030)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    TutorialTextComponent();
    virtual ~TutorialTextComponent();

    virtual bbool                               needsUpdate() const { return btrue; }
    virtual bbool                               needsDraw() const { return bfalse; }
    virtual bbool                               needsDraw2D() const { return bfalse; }
	virtual	bbool								needsDraw2DNoScreenRatio() const { return bfalse; }

    virtual void                                onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void                                onActorClearComponents();
    virtual void                                onBecomeActive();
    virtual void                                onBecomeInactive();
    virtual void                                Update( f32 _dt );

    void                                        show( const LocalisationId& _id, InputAdapter::PadType _padType );
    void                                        hide() { m_receiveHide = btrue; m_receiveShow = bfalse; m_locIdToShow = LocalisationId::Invalid; m_receiveForceHide = bfalse; }
    void                                        forceHide() { m_receiveForceHide = btrue; }
    bbool                                       isFinishHiding() const;
    const Vec2d&                                getIconPos() const { return m_iconPos; }

#ifdef ITF_SUPPORT_EDITOR
    virtual void                                onEditorCreated( class Actor* _original );
    virtual void                                onEditorMove( bbool _modifyInitialPos );
#endif

private:

    ITF_INLINE const class TutorialTextComponent_Template* getTemplate() const;

    void                                        updatePosition();
    void                                        updateIconPos();
    void                                        updateIconActor();

    class UITextBox*                            m_textBoxComponent;
    class AnimatedComponent*                    m_animatedComponent;

    Vec2d                                       m_relativePos;
    Vec2d                                       m_iconPos;
    LocalisationId                              m_locIdToShow;
    LocalisationId                              m_currentLocId;
    InputAdapter::PadType                       m_padToShow;
    InputAdapter::PadType                       m_currentPad;
    bbool                                       m_hiding;
    bbool                                       m_receiveHide;
    bbool                                       m_receiveShow;
    bbool                                       m_receiveForceHide;
    bbool                                       m_isShown;
    bbool                                       m_firstUpdate;
};

//------------------------------------------------------------------------------
class TutorialTextComponent_Template : public ActorComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(TutorialTextComponent_Template, ActorComponent_Template,386828231)
    DECLARE_ACTORCOMPONENT_TEMPLATE(TutorialTextComponent);
    DECLARE_SERIALIZE()

public:
    TutorialTextComponent_Template();
    virtual ~TutorialTextComponent_Template();

    const Vec2d&                                getAnimSize() const { return m_animSize; }
    const StringID&                             getIconPoint() const { return m_iconPoint; }

private:

    Vec2d                                       m_animSize;
    StringID                                    m_iconPoint;
};

ITF_INLINE const TutorialTextComponent_Template* TutorialTextComponent::getTemplate() const { return static_cast<const TutorialTextComponent_Template*>(m_template); }

} // namespace ITF

#endif //_ITF_TUTORIALTEXTCOMPONENT_H_
