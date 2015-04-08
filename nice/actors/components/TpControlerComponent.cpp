#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_TPCONTROLERCOMPONENT_H_
#include "engine/actors/components/TpControlerComponent.h"
#endif //_ITF_TPCONTROLERCOMPONENT_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(TpControlerComponent_Template)

    BEGIN_SERIALIZATION_CHILD(TpControlerComponent_Template)
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(TpControlerComponent)
    BEGIN_SERIALIZATION_CHILD(TpControlerComponent)

    END_SERIALIZATION()

    TpControlerComponent::TpControlerComponent()
    : Super()
    ,m_relativePosX(0.0f)
    ,m_relativePosY(0.0f)
    ,m_position2D(Vec2d::Zero)
    {
    }

    void TpControlerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_actor->setScale(Vec2d(90,90));
        m_actor->setLocalInitialPos(Vec3d(100, 100,-1));
        m_actor->set2DPos(Vec2d(100, 100));
    }

    void  TpControlerComponent::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);

        if ( INPUT_ADAPTER->IsTPJustPressed() )
        {
            GraphicComponent* _Graphic = (m_actor)->GetComponent<GraphicComponent>();
            if ( _Graphic )
            {
                EventShow evtShow(0.f, 1.f);
                _Graphic->onEvent(&evtShow);
            }
        }
        else if (INPUT_ADAPTER->IsTPPressed())
        {
            m_actor->set2DPos(Vec2d(INPUT_ADAPTER->GetTPPosX(), INPUT_ADAPTER->GetTPPosY()));
        }
        else if (INPUT_ADAPTER->IsTPJustReleased())
        {
            GraphicComponent* _Graphic = (m_actor)->GetComponent<GraphicComponent>();
            if ( _Graphic )
            {
                EventShow evtShow(1.f, 1.f);
                _Graphic->onEvent(&evtShow);
            }
        }
    }
}

