
#include "precompiled_engine.h"

#ifndef _ITF_FXDESCRIPTOR_H_
#include "engine/display/FxDescriptor.h"
#endif //_ITF_FXDESCRIPTOR_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

namespace ITF
{
	///////////////////////////////////////////////////////////////////////////////////////////
    BEGIN_SERIALIZATION(FxDescriptor_Template)
        SERIALIZE_MEMBER("name",m_name);
        SERIALIZE_OBJECT("gen",m_gen);
        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
            SERIALIZE_MEMBER("texture", m_material.getTexturePathSet().getTexturePath());
        END_CONDITION_BLOCK();
        SERIALIZE_OBJECT("material", m_material);
        SERIALIZE_MEMBER("angleOffset",m_angleOffset);
        SERIALIZE_MEMBER("minDelay",m_minDelay);
        SERIALIZE_MEMBER("maxDelay",m_maxDelay);
        SERIALIZE_OBJECT("frequencyInput",m_frequencyInput);
        SERIALIZE_OBJECT("emitCountInput",m_emitCountInput);
        SERIALIZE_OBJECT("maxParticlesInput",m_maxParticlesInput);
        SERIALIZE_OBJECT("velocityInput",m_velocityInput);
        SERIALIZE_OBJECT("velocityDeltaInput",m_velocityDeltaInput);
        SERIALIZE_OBJECT("angularSpeedInput",m_angularSpeedInput);
        SERIALIZE_OBJECT("angularSpeedDeltaInput",m_angularSpeedDeltaInput);
        SERIALIZE_OBJECT("defaultAlphaInput",m_defaultAlphaInput);
        SERIALIZE_MEMBER("draw2D", m_draw2D);
    END_SERIALIZATION()

    FxDescriptor_Template::FxDescriptor_Template()
	: m_angleOffset(Angle::Zero)
        , m_minDelay(0.0f)
	    , m_maxDelay(0.0f)
        , m_draw2D(bfalse)
    {
    }

	FxDescriptor::FxDescriptor()
		: m_template(NULL)
	{
	}

	void FxDescriptor::onActorLoaded( const FxDescriptor_Template* _template )
	{
		m_template = _template;
        
        ITF_ASSERT(m_template!=NULL);
	}

	FxDescriptor::~FxDescriptor()
	{
	}

	void FxDescriptor::clear()
	{
		m_template = NULL;
	}
}