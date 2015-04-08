#include "precompiled_engine.h"

#ifndef ITF_REFLECTIONCOMPONENT_H_
#include "engine/actors/components/reflectionComponent.h"
#endif //ITF_REFLECTIONCOMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(ReflectionComponent_Template)

    BEGIN_SERIALIZATION_CHILD(ReflectionComponent_Template)
    END_SERIALIZATION()

    ReflectionComponent_Template::ReflectionComponent_Template()
        : Super()
    {
    }

    ReflectionComponent_Template::~ReflectionComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(ReflectionComponent)

    BEGIN_SERIALIZATION_CHILD(ReflectionComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(ReflectionComponent)
    END_VALIDATE_COMPONENT()

    ReflectionComponent::ReflectionComponent()
    {
    }

    ReflectionComponent::~ReflectionComponent()
    {
    }


    void ReflectionComponent::Update(f32 _deltaTime)
    {
        computeUnitScaledAABB();
    }

    void ReflectionComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);
        computeUnitScaledAABB();
    }

    void ReflectionComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
        Super::batchPrimitives(_views);

        f32 yComponentPos = GetActor()->getPos().getY();
        for(ITF_VECTOR<View*>::const_iterator it = _views.begin(); it!=_views.end(); ++it)
        {
            View * pView = *it;
            if (pView && pView->getCamera())
            {
                reflectionViewParam reflectionParam;
                reflectionParam.m_reflectionPlaneY = yComponentPos;
                reflectionParam.m_reflectionPlaneZ = GetActor()->getDepth();
                reflectionParam.m_lastReflectionFrame = CURRENTFRAME;

                pView->setReflectionPlaneReference(reflectionParam);
            }
        }

    }

    void ReflectionComponent::computeUnitScaledAABB()
    {
        Actor * actor = GetActor();
        AABB aabb(Vec2d(-0.5, -0.5), Vec2d(0.5, 0.5));
        aabb.Scale(actor->getScale());
        aabb.Translate(actor->get2DPos());
        actor->growAABB(aabb);
    }
}
