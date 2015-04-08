
#ifndef _ITF_FXDESCRIPTOR_H_
#define _ITF_FXDESCRIPTOR_H_

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_



namespace ITF
{
	class ITF_ParticleGenerator;

	class FxDescriptor_Template
	{
	public:

		DECLARE_SERIALIZE()

		FxDescriptor_Template();
        ~FxDescriptor_Template() {}

		const StringID& GetName() const { return m_name; }
		ITF_INLINE ITF_ParticleGenerator_Template * getGenPtr() { return &m_gen; }
		ITF_INLINE const ITF_ParticleGenerator_Template & getGen() const { return m_gen; }
		const GFXMaterialSerializable & getMaterial() const { return m_material; }
        GFXMaterialSerializable & getMaterial() { return m_material; }
		ITF_INLINE const Angle& getAngleOffset() const { return m_angleOffset; }
		ITF_INLINE f32 getMinDelay() const { return m_minDelay; }
		ITF_INLINE f32 getMaxDelay() const { return m_maxDelay; }

        const ProceduralInputData&  getFrequencyInput()     const       {return m_frequencyInput;}
        const ProceduralInputData&  getEmitCountInput()     const       {return m_emitCountInput;}
        const ProceduralInputData&  getmaxParticlesInput() const        {return m_maxParticlesInput;}
        const ProceduralInputData&  getVelocityInput()      const       {return m_velocityInput;}
        const ProceduralInputData&  getVelocityDeltaInput() const       {return m_velocityDeltaInput;}
        const ProceduralInputData&  getAngularSpeedInput()  const       {return m_angularSpeedInput;}
        const ProceduralInputData&  getAngularSpeedDeltaInput()  const  {return m_angularSpeedDeltaInput;}
        const ProceduralInputData&  getDefaultAlphaInput()  const       {return m_defaultAlphaInput;}

        bbool getDraw2D() const {return m_draw2D;}

	private:

		StringID                            m_name;
		ITF_ParticleGenerator_Template      m_gen;
        GFXMaterialSerializable             m_material;
		Angle                               m_angleOffset;
		f32                                 m_minDelay;
		f32                                 m_maxDelay;
        bbool                               m_draw2D;

        ProceduralInputData                 m_frequencyInput;
        ProceduralInputData                 m_emitCountInput;
        ProceduralInputData                 m_maxParticlesInput;
        ProceduralInputData                 m_velocityInput;
        ProceduralInputData                 m_velocityDeltaInput;
        ProceduralInputData                 m_angularSpeedInput;
        ProceduralInputData                 m_angularSpeedDeltaInput;
        ProceduralInputData                 m_defaultAlphaInput;
	};

	class FxDescriptor
	{
	public:

		DECLARE_SERIALIZE()

		FxDescriptor();
		~FxDescriptor();

		void clear();
		void    onActorLoaded(const FxDescriptor_Template* _template );
		ITF_INLINE const ITF_ParticleGenerator_Template & getGen() const { return m_template->getGen(); }
		ITF_INLINE const StringID& GetName() const { return m_template->GetName(); }
		ITF_INLINE f32 getMinDelay() const { return m_template->getMinDelay(); }
		ITF_INLINE f32 getMaxDelay() const { return m_template->getMaxDelay(); }
		ITF_INLINE const Angle& getAngleOffset() const { return m_template->getAngleOffset(); }
        const FxDescriptor_Template* getTemplate() const {return m_template;}

        const ProceduralInputData&  getFrequencyInput()     const       {return m_template->getFrequencyInput();}
        const ProceduralInputData&  getEmitCountInput()     const       {return m_template->getEmitCountInput();}
        const ProceduralInputData&  getmaxParticlesInput() const        {return m_template->getmaxParticlesInput();}
        const ProceduralInputData&  getVelocityInput()      const       {return m_template->getVelocityInput();}
        const ProceduralInputData&  getVelocityDeltaInput() const       {return m_template->getVelocityDeltaInput();}
        const ProceduralInputData&  getAngularSpeedInput()  const       {return m_template->getAngularSpeedInput();}
        const ProceduralInputData&  getAngularSpeedDeltaInput()  const  {return m_template->getAngularSpeedDeltaInput();}
        const ProceduralInputData&  getDefaultAlphaInput()  const       {return m_template->getDefaultAlphaInput();}

	private:
		const FxDescriptor_Template*    m_template;
	};
}

#endif //_ITF_FXDESCRIPTOR_H_