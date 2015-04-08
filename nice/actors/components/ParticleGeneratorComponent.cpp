#include "precompiled_engine.h"

#ifndef _ITF_PARTICLEGENERATORCOMPONENT_H_
#include "engine/actors/components/ParticleGeneratorComponent.h"
#endif //_ITF_PARTICLEGENERATORCOMPONENT_H_

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(ParticleGeneratorComponent)

BEGIN_SERIALIZATION_CHILD(ParticleGeneratorComponent)
END_SERIALIZATION()

ParticleGeneratorComponent::ParticleGeneratorComponent()
: m_velocityNorm(1.0f)
, m_increment(0.0f)
, m_isPause(bfalse)
, m_maxFrequency(0.0f)
, m_isFlipped(bfalse)
{
}

ParticleGeneratorComponent::~ParticleGeneratorComponent()
{
}

void ParticleGeneratorComponent::start()
{
    //m_increment = m_maxFrequency * TARGETDT / m_startTime;  // force constant dt
    pause(bfalse);
}
void ParticleGeneratorComponent::stop()
{
    //m_increment = - m_maxFrequency * TARGETDT / m_startTime;  // force constant dt
    pause(btrue);
}

void ParticleGeneratorComponent::onBecomeActive()
{
    m_particleGenerator.getParameters()->setDepth(GetActor()->getDepth());
}

void ParticleGeneratorComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
{
    if (m_particleGenerator.getParameters()->getFrequency() > 0.0f)
    {
        m_particleGenerator.setAlpha(getAlpha());
        m_particleGenerator.setGFXPrimitiveParam(getGfxPrimitiveParam());
        m_particleGenerator.render(this, _views);
    }
}

void ParticleGeneratorComponent::updateAABB()
{
    //const Layer* layer = LAYER_MANAGER->findLayer(GetActor()->getLayerID());
    //m_particleGenerator.applyAmbiantColor(layer->getAmbientLight().m_color);
    if (m_particleGenerator.getTrueAABB())
    {
        GetActor()->growAABB(m_particleGenerator.getParameters()->getBoundingBox());
        return;
    }

    Vec2d minAABB = m_particleGenerator.getParameters()->getBoundingBox().getMin();
    Vec2d maxAABB = m_particleGenerator.getParameters()->getBoundingBox().getMax();
    minAABB *= GetActor()->getScale(); 
    maxAABB *= GetActor()->getScale();
    AABB aabb(minAABB);
    aabb.grow(maxAABB);
    aabb.Translate(m_actor->get2DPos());
    GetActor()->growAABB(aabb);
}

bbool ParticleGeneratorComponent::keepAlive()
{
    return m_particleGenerator.getActive();
}

void ParticleGeneratorComponent::Update(f32 _dt)
{
    ITF_WARNING(m_actor, m_particleGenerator.checkGeneratorValidity(), "generator: '%s'", m_particleGenerator.getValidityMessage().cStr());

    m_particleGenerator.setFlip(m_actor->getIsFlipped());

    f32 freq = f32_Inv(m_particleGenerator.getParameters()->getFrequency());

    if (!getTemplate()->getFrequencyInput().isValid())
    {
        freq += m_increment;
        m_particleGenerator.getParameters()->setFrequency(1.0f / Min(freq,m_maxFrequency));
    }
    else
    {
        freq = getTemplate()->getFrequencyInput().getValue(freq, _dt, m_inputList);
        f32 invFreq = freq ? f32_Inv(freq) : 0.f;
        m_particleGenerator.getParameters()->setFrequency(invFreq);
        //freq = 1.f; // just to pass the if (freq > 0.0f)
    }

    if (getTemplate()->getEmitCountInput().isValid())
    {
        f32 emitCount = (f32)m_particleGenerator.getParameters()->getNumToEmit();
        emitCount = getTemplate()->getEmitCountInput().getValue(emitCount, _dt, m_inputList);
        m_particleGenerator.getParameters()->setNumToEmit((u32)emitCount);
    }

    if (getTemplate()->getmaxParticlesInput().isValid())
    {
        f32 maxCount = (f32)m_particleGenerator.getMaxParticlesClamp();
        maxCount = getTemplate()->getmaxParticlesInput().getValue(maxCount, _dt, m_inputList);
        m_particleGenerator.setMaxParticlesClamp((u32)maxCount);
    }

    if (freq > 0.0f)
    {
        m_particleGenerator.getParameters()->setPosition(m_actor->getPos());
        m_particleGenerator.getParameters()->setAngle(Angle( bfalse, m_actor->getAngle()));

        Vec2d direction = Vec2d::Right.Rotate(m_actor->getAngle());
        if ( m_isFlipped )
            direction *= -1.f;
        m_particleGenerator.getParameters()->setVelocity( direction.to3d() );
        //GFX_ADAPTER->drawDBGArrow(m_actor->getPos(), m_actor->getPos() + direction.to3d());

        m_particleGenerator.update(_dt, m_isPause);
    }

    updateAABB();
}


void ParticleGeneratorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    Vec3d velocity;
    f32_CosSin(m_actor->getAngle(), &velocity.x(), &velocity.y());
    velocity.z() = 0.f;

    m_particleGenerator.init(&getTemplate()->getParticuleGenerator());
    m_particleGenerator.setMaterial(getTemplate()->getMaterial());
    m_maxFrequency      = f32_Inv(m_particleGenerator.getParameters()->getFrequency());
    m_particleGenerator.getParameters()->setVelocity(velocity);
    m_particleGenerator.setMaxParticles(m_particleGenerator.getParameters()->getMaxParticles());

    if (getTemplate()->getBeginStart())
    {
        start();
    }
    else
    {
        stop();
    }

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventTrigger_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventDie_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSequenceActorActivate_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSetFloatInput_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSetUintInput_CRC,this);

    m_inputList.resize(getTemplate()->getInputList().size());

    const u32 s = m_inputList.size();
    for ( u32 i = 0; i < s; i++ )
    {
        Input* input = &m_inputList[i];
        const InputDesc& inputDesc = getTemplate()->getInputList()[i];

        input->setId(inputDesc.getId());
        input->setType(inputDesc.getType());

        if ( inputDesc.getType() == InputType_F32 )
        {
            input->setValue(0.f);
        }
        else
        {
            input->setValue(static_cast<u32>(0));
        }
    }

    updateAABB();
}

void ParticleGeneratorComponent::onResourceLoaded()
{
    Super::onResourceLoaded();
    m_particleGenerator.computeAMVInfos();
}

void ParticleGeneratorComponent::onUnloadResources()
{
    m_particleGenerator.releaseAMVInfos();
    Super::onResourceLoaded();
}

void ParticleGeneratorComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if (EventTrigger * evt = DYNAMIC_CAST(_event,EventTrigger))
    {
        if (evt->getActivated())
            start();
        else
            stop();
    }
    else if (_event->IsClassCRC(EventDie::GetClassCRCStatic()))
    {
        stop();
    }
    else if (_event->IsClassCRC(EventSequenceActorActivate::GetClassCRCStatic()))
    {
        m_particleGenerator.reInit();
    }
    else if (EventSetFloatInput * onSetInput = DYNAMIC_CAST(_event,EventSetFloatInput))
    {
        setInput(onSetInput->getInputName(),onSetInput->getInputValue());
    }
    else if (EventSetUintInput * onSetInput = DYNAMIC_CAST(_event,EventSetUintInput))
    {
        setInput(onSetInput->getInputName(),onSetInput->getInputValue());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(ParticleGeneratorComponent_Template)

BEGIN_SERIALIZATION_CHILD(ParticleGeneratorComponent_Template)

    SERIALIZE_OBJECT("ParticleGeneratorParams",m_particleGenerator);
    SERIALIZE_MEMBER("startTime",m_startTime);
    SERIALIZE_MEMBER("stopTime",m_stopTime);
    BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
        SERIALIZE_MEMBER("texture", m_material.getTexturePathSet().getTexturePath());
    END_CONDITION_BLOCK();
    SERIALIZE_OBJECT("material",m_material);
    SERIALIZE_MEMBER("beginStart",m_beginStart);
    SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
    SERIALIZE_OBJECT("frequencyInput",m_frequencyInput);
    SERIALIZE_OBJECT("emitCountInput",m_emitCountInput);
    SERIALIZE_OBJECT("maxParticlesInput",m_maxParticlesInput);

END_SERIALIZATION()

ParticleGeneratorComponent_Template::ParticleGeneratorComponent_Template()
: m_startTime(1.0f)
, m_stopTime(1.0f)
, m_beginStart(btrue)
{
}

bbool ParticleGeneratorComponent_Template::onTemplateLoaded( bbool _hotReload )
{
    bbool bOk = Super::onTemplateLoaded(_hotReload);

    m_material.onLoaded(m_actorTemplate->getResourceContainer());
    m_particleGenerator.onLoaded(m_actorTemplate->getResourceContainer());

    return bOk;
}

void ParticleGeneratorComponent_Template::onTemplateDelete( bbool _hotReload )
{
    Super::onTemplateDelete(_hotReload);

    m_material.onUnLoaded(m_actorTemplate->getResourceContainer());
}

} // namespace ITF

