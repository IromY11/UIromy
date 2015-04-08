#include "precompiled_engine.h"

#ifndef _ITF_FXBANKCOMPONENT_H_
#include "engine/actors/components/FxBankComponent.h"
#endif //_ITF_FXBANKCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "../actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef ITF_GFX_MATERIAL_SHADER_MANAGER_H_
#include "engine/display/material/GFXMaterialShaderManager.h"
#endif //ITF_GFX_MATERIAL_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif // _ITF_CAMERA_H_


#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_BEZIERBRANCH_H_
#include "engine/BezierTree/BezierBranch.h"
#endif //_ITF_BEZIERBRANCH_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifndef _ITF_PLUGIN_FXEDITOR_H_
#include "tools/plugins/FxEditorPlugin/Plugin_FxEditor.h"
#endif // _ITF_PLUGIN_FXEDITOR_H_

namespace ITF
{

    enum
    {
        FXBANKCOMPONENT_MAX_OF_MAX_NUM_FX_INSTANCES = 250, // should be kept below 32K because of the handles being limited
        FXBANKCOMPONENT_DEFAULT_MAX_NUM_FX_INSTANCES = 20,
    }; 

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(FxBankComponent_Template)
    BEGIN_SERIALIZATION_CHILD(FxBankComponent_Template)
        SERIALIZE_CONTAINER_OBJECT("Fx",m_FxList);
        SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
        SERIALIZE_MEMBER("visibilityTest",m_FxVisibilityTest);

    SERIALIZE_MEMBER("MaxActiveInstance", m_maxActiveInstance);
    END_SERIALIZATION()

    FxBankComponent_Template::FxBankComponent_Template()
        : m_FxVisibilityTest(bfalse)
        , m_maxActiveInstance(FXBANKCOMPONENT_DEFAULT_MAX_NUM_FX_INSTANCES)
    {
    }

    FxBankComponent_Template::~FxBankComponent_Template()
    {
    }

    bbool FxBankComponent_Template::onTemplateLoaded( bbool _hotReload )
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);

        m_maxActiveInstance = std::min(u32(FXBANKCOMPONENT_MAX_OF_MAX_NUM_FX_INSTANCES), m_maxActiveInstance); // limit

        // Load resources assigned on this actor
        for ( u32 i = 0 ; i < m_FxList.size() ; ++i )
        {
            ITF_WARNING_CATEGORY(FX,NULL,m_materialMap.find(m_FxList[i].GetName()) == m_materialMap.end(), "A FX descriptor with the same name (%s) has already been added. Make sure the template uses unique names", m_FxList[i].GetName().getDebugString() );
            m_FxList[i].getMaterial().onLoaded(m_actorTemplate->getResourceContainer());
#ifndef ITF_FINAL
            m_FxList[i].getGenPtr()->setFile(m_actorTemplate->getFile());
#endif // ITF_FINAL
            m_FxList[i].getGenPtr()->onLoaded(m_actorTemplate->getResourceContainer());
#ifdef ITF_SUPPORT_FXEDITOR
            ITF_ParticleGenerator::AddRef(m_FxList[i].getGenPtr(), m_FxList[i].GetName());
#endif // ITF_SUPPORT_FXEDITOR
            m_materialMap[m_FxList[i].GetName()] = &m_FxList[i].getMaterial();
        }

        // Load extra resources needed from feedback fx manager
        FXControllerComponent_Template * FXControllerCmp_Tmpl = m_actorTemplate->GetComponent<FXControllerComponent_Template>();
        if( FXControllerCmp_Tmpl )
        {
            ITF_VECTOR<FxDescriptor_Template* > additonnalDescs;
            FEEDBACKFX_MANAGER->acquireAdditionnalFXDescriptors(FXControllerCmp_Tmpl->getFeedbackTags(),additonnalDescs);

            for ( u32 i = 0 ; i < additonnalDescs.size() ; ++i )
            {
                GFXMaterialSerializable & mat = additonnalDescs[i]->getMaterial();

                MaterialMap::iterator it = m_materialMap.find(additonnalDescs[i]->GetName());

                if ( it == m_materialMap.end())
                {
                    ITF_ASSERT(m_actorTemplate->getResourceContainer());
                    mat.fillResContainer(*m_actorTemplate->getResourceContainer());
                    m_materialMap[additonnalDescs[i]->GetName()] = &mat;
                }
#ifdef ITF_SUPPORT_EDITOR
                else
                {
                    GFXMaterialSerializable* matFromMap = it->second;

                    ResourceContainer resA;
                    ResourceContainer resB;

                    RESOURCE_MANAGER->registerResourceContainer(&resA,"TestA");
                    RESOURCE_MANAGER->registerResourceContainer(&resB,"TestB");

                    mat.fillResContainer(resA);
                    matFromMap->fillResContainer(resB);

                    ITF_VECTOR <ResourceID> resList;

                    resA.getContainedResources(resList);

                    u32 numRes = resList.size();

                    for ( u32 resN = 0; resN < numRes; resN++ )
                    {
                        bbool isContained = resB.usesResource(resList[resN]);

                        if ( !isContained )
                        {
                            String8 templatePath;
                            m_actorTemplate->getFile().toString8(templatePath);
                            ITF_ASSERT_MSG(isContained,"Resource conflict in FXBankManager");
                            ITF_WARNING(NULL,0,"There is a resource conflict in FxBankComponent from %s in FXControl %s",templatePath.cStr(),additonnalDescs[i]->GetName().getDebugString());
                        }
                    }

                    resA.clear();
                    resB.clear();

                    RESOURCE_MANAGER->unregisterResourceContainer(&resA);
                    RESOURCE_MANAGER->unregisterResourceContainer(&resB);
                }
#endif
            }
        }

        return bOk;
    }

    void FxBankComponent_Template::onTemplateDelete( bbool _hotReload )
    {
        for ( u32 i = 0 ; i < m_FxList.size() ; ++i )
        {
            m_FxList[i].getMaterial().onUnLoaded(m_actorTemplate->getResourceContainer());
        }

        FXControllerComponent_Template * FXControllerCmp_Tmpl = m_actorTemplate->GetComponent<FXControllerComponent_Template>();
        if( FXControllerCmp_Tmpl )
        {
            FEEDBACKFX_MANAGER->releaseAdditionnalFXDescriptors(FXControllerCmp_Tmpl->getFeedbackTags());
        }

        m_materialMap.clear();

        Super::onTemplateDelete(_hotReload);
    }

    const GFX_MATERIAL * FxBankComponent_Template::getMaterial(const StringID& _descName) const
    {
        MaterialMap::const_iterator it = m_materialMap.find(_descName);
        if (it == m_materialMap.end())
        {
            return NULL;
        }
        else
        {
            ITF_ASSERT(it->second);
            return it->second;
        }
    }

    IMPLEMENT_OBJECT_RTTI(FxBankComponent)
    BEGIN_SERIALIZATION_CHILD(FxBankComponent)
        BEGIN_CONDITION_BLOCK(ESerialize_DataRaw)
#ifdef ITF_SUPPORT_DEBUGFEATURE
            SERIALIZE_MEMBER("drawDebug",m_DBGdraw);
            SERIALIZE_MEMBER("drawDebugTextOffset",m_DBGTextOffset);
#endif // ITF_SUPPORT_DEBUGFEATURE
        END_CONDITION_BLOCK()
    END_SERIALIZATION()
    
    FxBankComponent::FxBankComponent()
    : Super()
    , m_fxInputUpdate(NULL)
    , m_draw2D(bfalse)
		, m_Invisible(bfalse)
    , m_uidHandleGenerator(0)
        , m_animComponent(nullptr)
        , m_physComponent(nullptr)
    //, m_DBGActive(0)
    //, m_DBGRejected(0)
    //, m_DBGMaxReached(0)
    {
#ifdef ITF_SUPPORT_DEBUGFEATURE
        m_DBGTextOffset = Vec2d::Zero;
        m_DBGdraw = bfalse;
#endif // ITF_SUPPORT_DEBUGFEATURE
    }


    FxBankComponent::~FxBankComponent()
    {
        clear();
    }

    void FxBankComponent::computeDraw2D()
    {
#if !defined(ITF_DISABLE_WARNING)
        bbool has3D = bfalse;
#endif // !defined(ITF_DISABLE_WARNING)
        bbool has2D = bfalse;

        for (u32 i=0; i<m_FxList.size(); i++)
        {
            FxDescriptor &descriptor = m_FxList[i];
            if (descriptor.getTemplate()->getDraw2D())
                has2D = btrue;
#if !defined(ITF_DISABLE_WARNING)
            else
                has3D = btrue;
#endif // !defined(ITF_DISABLE_WARNING)
        }
        ITF_WARNING_CATEGORY(GPP,m_actor, !(has2D && has3D), "FX bank component has both 2D & 3D descriptors!");
        m_draw2D = has2D;
    }

    void FxBankComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = GetActor()->GetComponent<AnimLightComponent>();
        m_physComponent = GetActor()->GetComponent<PhysComponent>();

        // Inputs
        m_inputList.resize(getTemplate()->getInputList().size());

        for ( u32 i = 0; i < m_inputList.size(); i++ )
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

        //FX

        u32 i = 0;
        m_FxList.resize(getTemplate()->getFxList().size());

        FxBankComponent_Template::FxList::const_iterator itTemplate = getTemplate()->getFxList().begin();
        for (; itTemplate != getTemplate()->getFxList().end(); itTemplate++ )
        {
            FxDescriptor* newDescriptor = &m_FxList[i];
            newDescriptor->onActorLoaded(&(*itTemplate));

            FxMap::const_iterator itmap = m_Fx.find( (*itTemplate).GetName());
            if (itmap != m_Fx.end())
            {
                ITF_WARNING(NULL, NULL, "fx %s already defined in FXBankComponent (%s)",(*itTemplate).GetName().getDebugString(), m_actor->getUserFriendly().cStr());
            }
            else
            {
                m_Fx[(*itTemplate).GetName()] = i;
            }
            ++i;
        }

        //
        computeDraw2D();

        //Register this component for events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSetFloatInput_CRC,this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSetUintInput_CRC,this);
    }

    bbool FxBankComponent::keepAlive()
    {
        u32 size = m_activeInstances.size();
        for (u32 i=0; i<size; i++)
        {
            ITF_ASSERT(m_activeInstances[i].m_gen);
            if (m_activeInstances[i].m_gen)
                return btrue;
        }
        return bfalse;
    }

    void FxBankComponent::onBecomeInactive()
    {
        u32 size = m_activeInstances.size();
        for (u32 i = 0; i < size; i++)
        {
            FxInstance& pFxInstance = m_activeInstances[i];
            ITF_ASSERT(pFxInstance.m_gen);
            if (pFxInstance.m_gen != NULL)
            {
                stopFx(pFxInstance);
                clearInstance(pFxInstance);
            }
        }
        m_activeInstances.clear();

        FEEDBACKFX_MANAGER->releaseFxDescriptors(m_actor);
        m_feedbackFxs.clear();
    }

    void FxBankComponent::clearInstance(FxInstance & _instance)
    {
        ITF_WARNING_CATEGORY(GPP, NULL, _instance.m_isOwner == (_instance.m_material==NULL), "Please check coherency of the owner flag and existence of the resource container in this FXInstance");
        if ( !_instance.m_isOwner && _instance.m_material )
        {
            _instance.m_material->cleanResContainer(*m_actor->getResourceContainer());
        }
        releaseFXHandle(_instance.m_instanceHandle);
        _instance.clear();
    }

    void FxBankComponent::Update( f32 _deltaTime )
    {
        PRF_M_SCOPE(updateFxBankComponent)

        Super::Update( _deltaTime );
         
        if ( m_fxInputUpdate )
        {
            m_fxInputUpdate->updateFxInput();
        }

        setInput(ITF_GET_STRINGID_CRC(GameTime,794481207), (f32)SYSTEM_ADAPTER->getTime());

        STATS_FXBANKCOMPONENTINSTANCE_INCSIZE(m_activeInstances.size());

        for (VecFxInstance::iterator it = m_activeInstances.begin(); it!=m_activeInstances.end(); /* increment or not depending on deletion */ )
        {
            FxInstance & pFxInstance = *it;
            ITF_ASSERT(pFxInstance.m_gen != NULL);
            if (pFxInstance.m_gen != NULL)
            {
#ifndef ITF_FINAL
               STATS_FXBANKCOMPONENTINSTANCE_HIT()
                bbool generatorValidity = pFxInstance.m_gen->checkGeneratorValidity();
                ITF_WARNING(m_actor, generatorValidity, "generator: %s : '%s'", pFxInstance.m_name.getDebugString(), pFxInstance.m_gen->getValidityMessage().cStr());
                if ( !generatorValidity )
                {
                    const ResourceContainer* resContainer = m_actor->getResourceContainer();

                    ITF_ASSERT_CRASH(resContainer->usesResource(pFxInstance.m_gen->getParameters()->getMaterial().getTextureResID()),"Resource not in resource container");
                }
#endif

                //pFxInstance.m_gen->applyAmbiantColor(layer->getAmbientLight().m_color);

                // Stop FX when 
                // - we are in manual mode and we explicitly asked to stop the FX through stopFX function
                // - we are not in manual mode and the generator is not playing anymore (no more particles to spawn, no more alive particles)
                // TODO : we shouldn't do that at all. A generator should only be started and stopped. the stop process should deal with particles
                // lifetime itself.
                
                if ( (pFxInstance.m_gen->getParameters()->getGeneratorMode() == PARGEN_MODE_MANUAL && pFxInstance.m_state == FxInstance::InstanceState_Stopping)
                    || (pFxInstance.m_gen->getParameters()->getGeneratorMode() != PARGEN_MODE_MANUAL && !isPlaying(pFxInstance)) )
                {
                    clearInstance(pFxInstance);
                    it = m_activeInstances.unordered_erase(it);
                    if (it != m_activeInstances.end())
                        relocateFXHandleEntry(it->m_instanceHandle, ux(std::distance(m_activeInstances.begin(), it)));
                    continue;
                }

                updateInstance(pFxInstance,_deltaTime);

                ++it;
            }
        }

        //String8 tmp;
        //tmp.setStringFormat("%ls : FX pool -> %d/%d",);
        //GFX_ADAPTER->drawDBGText(tmp,20,20,1.0f,0.0f,0.0f);
    }

    void FxBankComponent::setLifeTimeMultiplier(FXHandle _hdl, f32 _multiplier)
    {
        u32 idxInstance = getActiveInstanceFromHandle(_hdl);
        if (idxInstance != U32_INVALID)
        {
            FxInstance & instance = m_activeInstances[idxInstance];
            ITF_ASSERT(instance.m_gen != NULL);
            if (instance.m_gen != NULL)
            {
                instance.m_gen->getParameters()->setLifeTimeMultiplier(_multiplier);
            }
        }
    }

    void FxBankComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
		if (getInvisible())
			return;

        u32 size = m_activeInstances.size();
        ITF::Color defaultColor; // don't overhide particles defaultColor;
        for (u32 i = 0; i < size; i++)
        {
            ITF_ParticleGenerator *pGen = m_activeInstances[i].m_gen;
            ITF_ASSERT(pGen);
            if (pGen != NULL)
            {
                if(getTemplate()->getVisibilityTest())
                {
                    bbool bInView = pGen->m_activeParticlesCount > 0;
                    for(u32 j = 0; (j < _views.size()) && !bInView; ++j)
                        bInView |= _views[j]->getCamera()->isRectVisible( pGen->getParameters()->getBoundingBox(), GetActor()->getDepth() );
                    if(!bInView)
                        continue;
                }

                defaultColor = pGen->getGfxPrimitiveParam().m_colorFactor;
                pGen->getParameters()->setDisableLight(getDisableLight() == 1);
                if(m_activeInstances[i].m_useActorAlpha)
                    pGen->setAlpha(getAlpha()*m_activeInstances[i].m_fxAlpha);
                pGen->setRenderInTarget(m_renderInTarget);
                if(pGen->getTemplateParameters()->getForceNoDynamicFog())
                    pGen->getGfxPrimitiveParam().m_useStaticFog = btrue;
                pGen->render(this, _views);
            }
        }
    }

    void FxBankComponent::batchPrimitives2D( const ITF_VECTOR <class View*>& _views )
    {
		if (getInvisible())
			return;

        u32 size = m_activeInstances.size();
        ITF::Color defaultColor; // don't overhide particles defaultColor;
        bbool useStaticFog; // don't overhide staticfog
        for (u32 i = 0; i < size; i++)
        {
            FxInstance & instance = m_activeInstances[i];
            if (instance.m_gen != NULL)
            {
                defaultColor = instance.m_gen->getGfxPrimitiveParam().m_colorFactor;
                useStaticFog = instance.m_gen->getGfxPrimitiveParam().m_useStaticFog;
                instance.m_gen->setGFXPrimitiveParam(getGfxPrimitiveParam());
                instance.m_gen->getGfxPrimitiveParam().m_colorFactor = defaultColor;
                instance.m_gen->getGfxPrimitiveParam().m_useStaticFog = useStaticFog;
                if(instance.m_useActorAlpha)
                    instance.m_gen->setAlpha(getAlpha()*instance.m_fxAlpha);
                instance.m_gen->setColorFactor(getGfxPrimitiveParam().m_colorFactor);
                instance.m_gen->setRenderInTarget(m_renderInTarget);
                instance.m_gen->render(this, _views);
            }
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void FxBankComponent::drawEdit( ActorDrawEditInterface* drawInterface, u32 _flags)  const
    {
        if(m_DBGdraw == bfalse)
        {
#ifdef ITF_SUPPORT_FXEDITOR
            bbool doDrawText = bfalse;
            
            for(ITF_VECTOR<FxDescriptor_Template>::const_iterator it = getTemplate()->getFxList().begin() ; it != getTemplate()->getFxList().end() ; ++it)
            {
                const FxDescriptor_Template &fxTemplate = (*it);
                if(fxTemplate.getGen().getDrawDebugShape())
                {
                    fxTemplate.getGen().drawDebugShape( GetActor()->getPos(), GetActor()->getScale() );
                }
            }

            for (u32 i = 0; i < m_activeInstances.size(); i++)
            {
                const FxInstance & instance = m_activeInstances[i];
                if (instance.m_gen != NULL)
                {
                    if(instance.m_gen->m_bDrawText)
                    {
                        doDrawText = btrue;
                    }
                    else if(instance.m_gen->getTemplate()->getDrawDebugShape())
                    {
                        instance.m_gen->drawEdit( drawInterface, _flags);
                    }
                }
            }

            if(!doDrawText)
                return;
#else
            return;
#endif // ITF_SUPPORT_FXEDITOR
        }

        String8 str, tmp;

        u32 size = m_activeInstances.size();
        u32 genCount = 0;
        u32 ativePartCount = 0;
        for (u32 i = 0; i < size; i++)
        {
            const FxInstance & instance = m_activeInstances[i];
            if (instance.m_gen != NULL)
            {
                instance.m_gen->drawEdit( drawInterface, _flags);
                genCount++;
                ativePartCount += instance.m_gen->m_activeParticlesCount;
            }
        }
        str.setTextFormat("FX info: Gen %u/%u Particles %u\n", genCount, size, ativePartCount);

        // input debug info
        for (u32 i = 0; i < size; i++)
        {
            String8 strInputsInst;

            const FxInstance& pFxInstance = m_activeInstances[i];
            str += "      ";
            str += pFxInstance.m_name.getDebugString();
            str += "\n";

            if (pFxInstance.m_gen != NULL)
            {
                tmp.setTextFormat("Particles active = %u",pFxInstance.m_gen->m_activeParticlesCount);
                strInputsInst += tmp;

                tmp.setTextFormat(" Max = %u\n",pFxInstance.m_gen->getParameters()->getMaxParticles());
                if(pFxInstance.m_descriptor->getmaxParticlesInput().isValid())
                    strInputsInst += "(i)";
                strInputsInst += tmp;
                
                if(pFxInstance.m_gen->getBezierBranch())
                {
                    tmp.setTextFormat("Bezier len = %f\n",pFxInstance.m_gen->getBezierBranch()->getCurrentLength());
                    strInputsInst += tmp;
                }

                tmp.setTextFormat("Frequency = %f\n",pFxInstance.m_gen->getParameters()->getFrequency());
                if(pFxInstance.m_descriptor->getFrequencyInput().isValid())
                    strInputsInst += "(i)";
                strInputsInst += tmp;
                
                tmp.setTextFormat("EmitCount = %d\n", (int)pFxInstance.m_gen->getParameters()->getNumToEmit());
                if(pFxInstance.m_descriptor->getEmitCountInput().isValid())
                    strInputsInst += "(i)";
                strInputsInst += tmp;

                tmp.setTextFormat("Velocity = %.4f",pFxInstance.m_gen->getParameters()->getVelocityNorm());
                if(pFxInstance.m_descriptor->getVelocityInput().isValid())
                    strInputsInst += "(i)";
                strInputsInst += tmp;

                tmp.setTextFormat(" Var = %.4f\n",pFxInstance.m_gen->getParameters()->getVelocityVar());
                if(pFxInstance.m_descriptor->getVelocityDeltaInput().isValid())
                    strInputsInst += "(i)";
                strInputsInst += tmp;

                tmp.setTextFormat("AngularSpeed = %.2f",pFxInstance.m_gen->getParameters()->getAngularSpeed().ToDegrees());
                if(pFxInstance.m_descriptor->getAngularSpeedInput().isValid())
                    strInputsInst += "(i)";
                strInputsInst += tmp;

                tmp.setTextFormat(" Delta = %.2f\n",pFxInstance.m_gen->getParameters()->getAngularSpeedDelta().ToDegrees());
                if(pFxInstance.m_descriptor->getAngularSpeedDeltaInput().isValid())
                    strInputsInst += "(i)";
                strInputsInst += tmp;

                tmp.setTextFormat("fxAlpha = %.3f\n", pFxInstance.m_fxAlpha);
                if(pFxInstance.m_descriptor->getDefaultAlphaInput().isValid())
                    strInputsInst += "(i)";
                strInputsInst += tmp;
            }
            if(strInputsInst.cStr())
                str += strInputsInst;
        }

        if(str.cStr())
        {
            Vec3d pos2d;
            View::getCurrentView()->compute3DTo2D(m_actor->getPos(), pos2d);
            const char *p = str.cStr();
            u32 lineCount, lineLen, u32tmp; lineCount = lineLen = u32tmp = 0;
            while(*p) {u32tmp++; if(*p++ == '\n'){ lineCount++;if(u32tmp>lineLen)lineLen=u32tmp; u32tmp = 0;}}

            const f32 textSize = f32(GFX_ADAPTER->getDebugFontSize());
            pos2d.x() -= (lineLen+5) * textSize / 2.5f;
            pos2d.y() -= lineCount * textSize / 2;
            pos2d.x() += m_DBGTextOffset.x();
            pos2d.y() += m_DBGTextOffset.y();
            DebugDraw::text(pos2d.x(), pos2d.y(), Color::white(), str.cStr());

            u32 uColor = Color::grey().getAsU32(); uColor = ITFCOLOR_SET_A(uColor, (u8)200);
            u32 uColor2 = ITFCOLOR_SET_A(uColor, (u8)64);
            GFX_ADAPTER->drawDBG2dBox(Vec2d(pos2d.x() - 5, pos2d.y()), (lineLen * textSize / 2.5f) + 5.f, lineCount * textSize, uColor, uColor, uColor2, uColor);
        }
    }
#endif // ITF_SUPPORT_EDITOR

    void FxBankComponent::clear()
    {
        for (FxList::iterator it = m_FxList.begin(); it != m_FxList.end(); ++it)
        {
            it->clear();
        }

        m_FxList.clear();
        m_Fx.clear();

        u32 size = m_activeInstances.size();

        for (u32 i = 0; i < size; ++i)
        {
            clearInstance(m_activeInstances[i]);
        }
        m_activeInstances.clear();
        
        FEEDBACKFX_MANAGER->releaseFxDescriptors(m_actor);
    }

    u32 FxBankComponent::getEntryFromHdl(FXHandle _handle)
    {
        return BitTweak::GetBitRange< 0, 16>(_handle);
    }

    u32 FxBankComponent::getUIDFromHdl(FXHandle _handle)
    {
        return BitTweak::GetBitRange< 16, 16>(_handle);
    }

    FXHandle FxBankComponent::buildHandle(u32 _hdlEntry, u32 _uid)
    {
        return BitTweak::BuildBitRange< 16, 16>(_uid) | BitTweak::BuildBitRange< 0, 16>(_hdlEntry);
    }

    u32 FxBankComponent::getActiveInstanceFromHandle(FXHandle _handle) const
    {
        u32 entry = getEntryFromHdl(_handle);
        if (entry < m_handles.size())
        {
            if (m_handles[entry].m_uid == getUIDFromHdl(_handle))
            {
                u32 idxInstance = m_handles[entry].m_activeInstanceIdx;
                ITF_ASSERT(idxInstance < m_activeInstances.size() || idxInstance == U16_INVALID);
                if (idxInstance < m_activeInstances.size())
                    return idxInstance;
            }
        }
        return U32_INVALID;
    }

    FXHandle FxBankComponent::acquireFXHandle(u32 _activeInstanceIdx)
    {
        ITF_ASSERT(_activeInstanceIdx < m_activeInstances.size());
        u32 newEntryIdx = 0;
        if (    !m_handles.empty()
            &&  m_handles.size() > 2 * m_activeInstances.size()) // if the handles array is getting too big, prefer a linear search for invalid entry
        {
            ux idxHdl = 0;
            while(idxHdl<m_handles.size() && m_handles[idxHdl].m_activeInstanceIdx != U16_INVALID)
            {
                idxHdl++;
            }
            ITF_ASSERT(newEntryIdx<m_handles.size());
            if (newEntryIdx<m_handles.size())
                newEntryIdx = idxHdl;
        }
        else
        {
            m_handles.emplace_back();
            newEntryIdx = m_handles.size() - 1u;
        }
        HandleEntry & entry = m_handles[newEntryIdx];
        entry.m_activeInstanceIdx = u16(_activeInstanceIdx);
        entry.m_uid = m_uidHandleGenerator;
        m_uidHandleGenerator++; // get another uid
        return buildHandle(newEntryIdx, entry.m_uid);
    }

    void FxBankComponent::releaseFXHandle(FXHandle & _handle)
    {
        u32 entry = getEntryFromHdl(_handle);
        if (entry < m_handles.size())
        {
            if (getUIDFromHdl(_handle) == m_handles[entry].m_uid)
            {
                m_handles[entry].m_activeInstanceIdx = U16_INVALID;
                // shorten the array
                ux newSize = m_handles.size();
                while( (newSize > 0u) && (m_handles[newSize-1u].m_activeInstanceIdx == U16_INVALID) )
                {
                    newSize--;
                }
                m_handles.resize(newSize);
            }
        }
        _handle = (FXHandle)InvalidFXHandle;
    }

    void FxBankComponent::relocateFXHandleEntry(FXHandle _handle, u32 _newActiveInstanceIdx)
    {
        u32 entry = getEntryFromHdl(_handle);
        ITF_ASSERT(_newActiveInstanceIdx < m_activeInstances.size());
        if (entry < m_handles.size())
        {
            if (getUIDFromHdl(_handle) == m_handles[entry].m_uid)
            {
                 m_handles[entry].m_activeInstanceIdx = u16(_newActiveInstanceIdx);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    FXHandle FxBankComponent::playFx( StringID _name, const playFxParams& _params)
    {
        PRF_M_SCOPE(playFx)

        FxDescriptor * desc = NULL;

        // A FX descriptor has been specified, then check feedback list first to get descriptor
        if ( _params.m_FxDesc )
        {
            ITF_MAP<StringID,FxDescriptor *>::const_iterator feedback_it = m_feedbackFxs.find(_name);

            // A slot has already been taken from FeedbackFXManager ?
            if ( feedback_it != m_feedbackFxs.end() )
            {
                desc = feedback_it->second;
            }
            // Ask a free sound descriptor slot to FeedbackFXManager, and get SoundDescriptor
            else
            {
                desc = FEEDBACKFX_MANAGER->getNewFxDescriptor( m_actor, _params.m_FxDesc );
                if ( desc )
                {
                    m_feedbackFxs[_name] = desc;
                }
            }
        }
        else
        {
            ITF_ASSERT(_params.m_isOwner);
            FxMap::const_iterator it = m_Fx.find(_name);
            desc = it != m_Fx.end() ? &m_FxList[it->second] : NULL;
        }

        if (desc)
        {
            if (m_activeInstances.size() <= getTemplate()->getMaxActiveInstance())
            {
                i32 idxInPool = 0;
                ITF_ParticleGenerator * gen = ITF_ParticleGenerator::getGenerator(idxInPool);
                if (!gen)
                {
                    return (FXHandle)InvalidFXHandle;
                }

				gen->init(&desc->getGen());
                gen->setMaterial(desc->getTemplate()->getMaterial());

 				m_activeInstances.emplace_back();
                FxInstance & instance = m_activeInstances.back();

                // Set instance play mode
                if ( _params.m_boneIndex != U32_INVALID )
                {
                    instance.m_playMode = FXPlayMode_AttachedToBone;
                }
                else if ( _params.m_emitFromBase )
                {
                   instance.m_playMode = FXPlayMode_ActorBase;
                }
                else
                {
                    instance.m_playMode = FXPlayMode_Actor;
                }


                if (!_params.m_isOwner)
                {
                    ITF_ASSERT(m_actor->getResourceContainer());
                    desc->getTemplate()->getMaterial().fillResContainer(*m_actor->getResourceContainer());
                }
				instance.m_generatorIndexInPool = idxInPool;
                instance.m_isOwner = _params.m_isOwner;
                instance.m_material = _params.m_isOwner?NULL:&desc->getTemplate()->getMaterial();
                instance.m_descriptor = desc;
                instance.m_gen = gen;
			    instance.m_state = FxInstance::InstanceState_Playing;
                instance.m_emitFromBase = _params.m_emitFromBase;
                instance.m_useActorSpeed = _params.m_useActorSpeed;
                instance.m_useActorOrientation = _params.m_useActorOrientation;
                instance.m_useActorAlpha = _params.m_useActorAlpha;
                instance.m_gen->getParameters()->setLifeTimeMultiplier(_params.m_lifeTimeMult);

				Color defaultColor = instance.m_gen->getGfxPrimitiveParam().m_colorFactor;
				instance.m_gen->setGFXPrimitiveParam(getGfxPrimitiveParam());
				instance.m_gen->getGfxPrimitiveParam().m_colorFactor = defaultColor;
				if(_params.m_pickColorFromFrieze && _params.m_gfxPrimitiveParamNeededForBank)
				{
					instance.m_gen->setColorFactor(_params.m_gfxPrimitiveParamNeededForBank->m_colorFactor);
                    instance.m_recomputeColorFactor = bfalse;
				}
				else
				{
					instance.m_gen->setColorFactor(getGfxPrimitiveParam().m_colorFactor);
                    instance.m_recomputeColorFactor = btrue;
				}

                instance.m_boneIndex = _params.m_boneIndex;
                instance.m_useBoneOrientation = _params.m_useBoneOrientation;
                instance.m_timer = Seeder::getSharedSeeder().GetFloat(desc->getMinDelay(),desc->getMaxDelay());
                instance.m_name = _name;
                FXHandle handle = acquireFXHandle(m_activeInstances.size()-1u);
                instance.m_instanceHandle = handle;

                setPosAndAngle(instance);

                return handle;
            }
        }
        return U32_INVALID;
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    void FxBankComponent::stopFx(FxInstance & _instance, bbool _stopOnlyLooped)
    {
        if( _instance.m_gen != NULL 
            && _instance.m_state != FxInstance::InstanceState_Unused)
        {
            if ( !_stopOnlyLooped || _instance.m_gen->getTemplate()->getIsGeneratorLoop() )
            {
                _instance.m_state = FxInstance::InstanceState_Stopping;
                _instance.m_gen->stopGeneration();
            }
        }
    }


    void FxBankComponent::stopFx(FXHandle _hdl, bbool _stopOnlyLooped)
    {
        u32 idxInstance = getActiveInstanceFromHandle(_hdl);
        if (idxInstance != U32_INVALID)
        {
            stopFx(m_activeInstances[idxInstance], _stopOnlyLooped);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void FxBankComponent::stopFxImmediate(FXHandle _hdl)
    {
        u32 idxInstance = getActiveInstanceFromHandle(_hdl);
        if (idxInstance != U32_INVALID)
        {
            FxInstance & instance = m_activeInstances[idxInstance];

            if (instance.m_gen != NULL
                && instance.m_state != FxInstance::InstanceState_Unused)
            {
                instance.m_state = FxInstance::InstanceState_Stopping;
                instance.m_gen->stop();
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    ITF_ParticleGenerator* FxBankComponent::getParticleGenerator(FXHandle _hdl)
    {
        u32 idxInstance = getActiveInstanceFromHandle(_hdl);
        if (idxInstance != U32_INVALID)
        {
            FxInstance & instance = m_activeInstances[idxInstance];
            if (instance.m_state != FxInstance::InstanceState_Unused)
            {
                return instance.m_gen;
            }
        }
        return NULL;
    }

    const StringID& FxBankComponent::getInstanceName(FXHandle _hdl)
    {
        u32 idxInstance = getActiveInstanceFromHandle(_hdl);
        if (idxInstance != U32_INVALID)
        {
            FxInstance & instance = m_activeInstances[idxInstance];
            if (instance.m_gen != NULL && instance.m_state != FxInstance::InstanceState_Unused)
            {
                return instance.m_name;
            }
        }

        return StringID::Invalid;
    }

    void FxBankComponent::setFXPos(FXHandle _hdl, const Vec3d& _pos, FXPlayMode _mode)
    {
        u32 idxInstance = getActiveInstanceFromHandle(_hdl);
        if (idxInstance != U32_INVALID)
        {
            FxInstance & instance = m_activeInstances[idxInstance];

            if (instance.m_gen != NULL && instance.m_state != FxInstance::InstanceState_Unused)
            {
                instance.m_pos = _pos;
                instance.m_playMode = _mode;
            }
        }
    }

    void FxBankComponent::setFXAngle(FXHandle _hdl, f32 _angle)
    {
        u32 idxInstance = getActiveInstanceFromHandle(_hdl);
        if (idxInstance != U32_INVALID)
        {
            FxInstance & instance = m_activeInstances[idxInstance];

            if (instance.m_gen != NULL && instance.m_state != FxInstance::InstanceState_Unused)
            {
                instance.m_angle = _angle;
            }
        }
    }

    void FxBankComponent::attachToBone(FXHandle _hdl, u32 _boneIndex)
    {
        u32 idxInstance = getActiveInstanceFromHandle(_hdl);
        if (idxInstance != U32_INVALID)
        {
            FxInstance & instance = m_activeInstances[idxInstance];
            if (instance.m_gen != NULL && instance.m_state != FxInstance::InstanceState_Unused)
            {
                instance.m_boneIndex = _boneIndex;
                instance.m_playMode = FXPlayMode_AttachedToBone;
            }
        }
    }


    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    bbool FxBankComponent::isPlaying(const FxInstance &_instance)
    {
        return _instance.m_gen != NULL
            && ((_instance.m_gen->getNumGeneratedParticles() == 0) && (_instance.m_gen->isGenerationStarted())
            || _instance.m_gen->getActive() || _instance.m_timer != 0.0f);
    }

    bbool FxBankComponent::isPlaying(FXHandle _hdl) const
    {
        u32 idxInstance = getActiveInstanceFromHandle(_hdl);
        if (idxInstance != U32_INVALID)
        {
            return isPlaying(m_activeInstances[idxInstance]);
        }
        return bfalse;
    }


    void FxBankComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);
        if (_event->IsClassCRC(EventDie::GetClassCRCStatic()))
        {
            for (u32 i = 0, size = m_activeInstances.size(); i<size; i++)
            {
                stopFx(m_activeInstances[i], btrue);
            }
        }
        // Reset ?
        else if ( _event->IsClassCRC(EventReset::GetClassCRCStatic()) )
        {
            // Make sure all spawned particles are correctly removed
            for (u32 i = 0, size = m_activeInstances.size(); i<size; i++)
            {
                clearInstance(m_activeInstances[i]);
            }
            m_activeInstances.clear();
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

    void FxBankComponent::updateInstance( FxInstance& _instance, f32 _deltaTime )
    {
        if (_instance.m_timer == 0.0f)
        {
            setInput(ITF_GET_STRINGID_CRC(EmitterTime,2223213470), (f32)_instance.m_gen->getCurrentTime());

            setPosAndAngle(_instance);

            setInputs(_instance, _deltaTime);

            //UPDATE PARTICLE
            _instance.m_gen->update(_deltaTime, _instance.m_state == FxInstance::InstanceState_Stopping);

            if(_instance.m_recomputeColorFactor)
            {
                _instance.m_gen->getGfxPrimitiveParam().m_colorFactor = getGfxPrimitiveParam().m_colorFactor;
                _instance.m_gen->setColorFactor(getGfxPrimitiveParam().m_colorFactor);
            }

            // DEBUG
            //DebugDraw::circle(_instance.m_pos,1.0,Color::red(),0.0f,"FX");
        }


        updateAABB(_instance);

        _instance.m_timer = Max(_instance.m_timer-_deltaTime,0.0f);
    }

    void FxBankComponent::setPosAndAngle( FxInstance &_instance )
    {
        f32 angle = m_actor->getAngle();

        bbool fliped =  m_actor->getIsFlipped();

        angle += _instance.m_descriptor->getAngleOffset().ToRadians();

        Vec3d pos;
        if ((_instance.m_playMode == FXPlayMode_AttachedToBone) && (_instance.m_boneIndex != U32_INVALID))
        {
            if (m_animComponent)
            {
                pos = m_actor->getPos();
                if (m_animComponent->getBonePos(_instance.m_boneIndex, pos))
                {
                    if ( _instance.m_gen->bCheckBoolCond(_instance.m_useBoneOrientation) )
                    {
                        f32 offsetAngle = _instance.m_descriptor->getAngleOffset().ToRadians();
                        f32 boneAngle = 0.f; //world angle

                        m_animComponent->getBoneAngle(_instance.m_boneIndex, boneAngle);

                        if (fliped && _instance.m_gen->getTemplateParameters()->canFlipAngleOffset())
                            boneAngle += offsetAngle;
                        else
                            boneAngle -= offsetAngle;

                        _instance.m_gen->setLocalRotationZ(boneAngle);

                        angle = boneAngle;
                    }
                    else
                    {
                        if(_instance.m_useBoneOrientation == BOOL_cond)
                            angle = _instance.m_gen->getLocalRotationZ();
                    }

                    f32 alpha;
                    m_animComponent->getBoneAlpha(_instance.m_boneIndex, alpha);
                    setInput(ITF_GET_STRINGID_CRC(boneAlpha,579962163), alpha);
                }
            }
        }
        else if ( _instance.m_playMode == FXPlayMode_ActorBase)
        {
            pos = m_actor->getBase();
        }
        else if ( (_instance.m_playMode == FXPlayMode_Position ) || (_instance.m_playMode == FXPlayMode_PositionXYZ ))
        {
            pos = _instance.m_pos;
        }
        else
        {
            pos = m_actor->getPos();
        }

        if (_instance.m_playMode != FXPlayMode_PositionXYZ )
        {
            pos.z() = m_actor->getDepth();
        }

        if (_instance.m_useActorSpeed)
        {
            if (m_physComponent)
            {
                _instance.m_gen->getParameters()->setVelocityNorm(_instance.m_descriptor->getGen().getParameters().getVelocityNorm() * m_physComponent->getSpeed().norm());
            }
        }

        // Apply angle from instance
        if ( _instance.m_angle != 0.0f )
        {
            angle += _instance.m_angle;
            if (!_instance.m_gen->isUseMatrix()) // Do not change generator angle, it will be changed automatically by the actor rotation. 
                _instance.m_gen->getParameters()->setAngle(Angle(bfalse,angle));
        }
        // Apply angle from actor
        else if ( _instance.m_useActorOrientation ) 
        {
            if (!_instance.m_gen->isUseMatrix()) // Do not change generator angle, it will be changed automatically by the actor rotation. 
                _instance.m_gen->getParameters()->setAngle(Angle(bfalse,angle));
        }

        Vec3d velocity = Vec3d::XAxis;
        if(angle)
            f32_CosSinOpt(angle, &velocity.x(), &velocity.y());

        if (_instance.m_gen->isUseMatrix())
        {
            _instance.m_gen->setLocalRotationZ(angle);
            _instance.m_gen->setLocalScale(m_actor->getScale());
            angle = 0.f;
            _instance.m_gen->getParameters()->setVelocity(Vec3d::XAxis);

            if (!_instance.m_gen->isScaleGenBox() && !m_actor->getScale().IsEqual(Vec2d::One, MTH_EPSILON))
                _instance.m_gen->setGenBoxLocalScale(Vec2d::One / m_actor->getScale());
        }
        else
        {
            if(_instance.m_gen->getParameters()->m_useMatrix == BOOL_false)
                _instance.m_gen->getParameters()->setVelocity(velocity);

            if (_instance.m_gen->isScaleGenBox())
                _instance.m_gen->setGenBoxLocalScale(m_actor->getScale());
        }

        _instance.m_gen->setFlip(fliped);
        _instance.m_gen->getParameters()->setIs2D(m_draw2D);

        if(_instance.m_gen->getParameters()->m_useMatrix)
        {
            Vec3d posOffset(// apply angle rotation
                (   _instance.m_gen->getParameters()->getPositionOffset().x() * velocity.x()
                  - _instance.m_gen->getParameters()->getPositionOffset().y() * velocity.y())
                * m_actor->getScale().x(),

                (   _instance.m_gen->getParameters()->getPositionOffset().x() * velocity.y()
                  + _instance.m_gen->getParameters()->getPositionOffset().y() * velocity.x())
                * m_actor->getScale().y(),

                _instance.m_gen->getParameters()->getPositionOffset().z());

            _instance.m_gen->getParameters()->setPosition(pos + posOffset);
        }
        else
        {
            Vec3d posOffset = _instance.m_gen->getParameters()->getPositionOffset();
            _instance.m_gen->getParameters()->setPosition(pos + posOffset);
        }
    }

    void FxBankComponent::setInputs( FxInstance &_instance, f32 _deltaTime )
    {
        // Inputs
        //-------------------------------------------------------------

        if (_instance.m_descriptor->getFrequencyInput().isValid())
        {
            f32 freq = f32_Inv(_instance.m_gen->getParameters()->getFrequency());
            freq = _instance.m_descriptor->getFrequencyInput().getValue(freq, _deltaTime, m_inputList);
            f32 invFreq = freq ? f32_Inv(freq) : 0.f;
            _instance.m_gen->getParameters()->setFrequency(invFreq);
        }

        if (_instance.m_descriptor->getEmitCountInput().isValid() )
        {
            f32 emitCount = (f32)_instance.m_gen->getParameters()->getNumToEmit();
            emitCount = _instance.m_descriptor->getEmitCountInput().getValue(emitCount, _deltaTime, m_inputList);
            _instance.m_gen->getParameters()->setNumToEmit((u32)emitCount);
        }

        if (_instance.m_descriptor->getmaxParticlesInput().isValid() )
        {
            f32 maxCount = (f32)_instance.m_gen->getMaxParticlesClamp();
            maxCount = _instance.m_descriptor->getmaxParticlesInput().getValue(maxCount, _deltaTime, m_inputList);
            _instance.m_gen->setMaxParticlesClamp((u32)maxCount);
        }

        if ( _instance.m_descriptor->getVelocityInput().isValid() )
        {
            f32 velocity = _instance.m_gen->getParameters()->getVelocityNorm();
            velocity = _instance.m_descriptor->getVelocityInput().getValue(velocity, _deltaTime, m_inputList);
            _instance.m_gen->getParameters()->setVelocityNorm(velocity);
        }

        if ( _instance.m_descriptor->getVelocityDeltaInput().isValid() )
        {
            f32 velocityVar = _instance.m_gen->getParameters()->getVelocityVar();
            velocityVar = _instance.m_descriptor->getVelocityDeltaInput().getValue(velocityVar, _deltaTime, m_inputList);
            _instance.m_gen->getParameters()->setVelocityVar(velocityVar);
        }

        if ( _instance.m_descriptor->getAngularSpeedInput().isValid() )
        {
            Angle angularSpeed = _instance.m_gen->getParameters()->getAngularSpeed();
            angularSpeed.SetDegrees(_instance.m_descriptor->getAngularSpeedInput().getValue(angularSpeed.ToDegrees(), _deltaTime, m_inputList));
            _instance.m_gen->getParameters()->setAngularSpeed(angularSpeed);
        }

        if ( _instance.m_descriptor->getAngularSpeedDeltaInput().isValid() )
        {
            Angle angularSpeedDelta = _instance.m_gen->getParameters()->getAngularSpeedDelta();
            angularSpeedDelta.SetDegrees(_instance.m_descriptor->getAngularSpeedDeltaInput().getValue(angularSpeedDelta.ToDegrees(), _deltaTime, m_inputList));
            _instance.m_gen->getParameters()->setAngularSpeedDelta(angularSpeedDelta);
        }

        if ( _instance.m_descriptor->getDefaultAlphaInput().isValid() )
        {
            _instance.m_fxAlpha = _instance.m_descriptor->getDefaultAlphaInput().getValue(_instance.m_fxAlpha, _deltaTime, m_inputList);
            _instance.m_gen->setAlpha(_instance.m_fxAlpha);
        }
        else
            _instance.m_fxAlpha = 1.f;
    }

    void FxBankComponent::updateAABB( FxInstance &_instance )
    {
        const AABB& fxAABB = _instance.m_gen->getParameters()->getBoundingBox();

        if (_instance.m_gen->getTrueAABB())
        {
            m_actor->growAABB(fxAABB);
        }
        else
        {
            AABB aabb(fxAABB.getMin() * m_actor->getScale());
            aabb.grow(fxAABB.getMax() * m_actor->getScale());
            //aabb.Translate(m_actor->get2DPos());
            aabb.Translate(_instance.m_gen->getParameters()->getPosition().truncateTo2D());
            m_actor->growAABB(aabb);
        }
    }

	void FxInstance::clear()
    {
        if (m_generatorIndexInPool  >= 0)
        {
            ITF_ParticleGenerator::freeGenerator(m_gen, m_generatorIndexInPool);
            m_gen = NULL;
            m_generatorIndexInPool = -1;
        }
        else
        {
            SF_DEL(m_gen);
        }
        m_descriptor            = NULL;
        m_state                 = InstanceState_Unused;
        m_emitFromBase          = bfalse;
        m_useActorSpeed         = btrue;
        m_useActorOrientation   = bfalse;
        m_useActorAlpha         = btrue;
        m_recomputeColorFactor  = bfalse;
        m_angle                 = 0.0f;
        m_boneIndex             = U32_INVALID;
        m_useBoneOrientation    = BOOL_false;
        m_pos                   = Vec3d::Zero;
        m_timer                 = 0.0f;
        m_name                  = StringID::Invalid;
        m_playMode              = 0;
        m_isOwner               = btrue;
        m_material              = NULL;
        m_fxAlpha            = 1.f;
    }

}

