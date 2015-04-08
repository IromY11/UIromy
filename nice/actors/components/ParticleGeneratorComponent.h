#ifndef _ITF_PARTICLEGENERATORCOMPONENT_H_
#define _ITF_PARTICLEGENERATORCOMPONENT_H_

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_


namespace ITF
{
    class Texture;

	class ParticleGeneratorComponent : public GraphicComponent
	{
        DECLARE_OBJECT_CHILD_RTTI(ParticleGeneratorComponent,GraphicComponent,1621341835);
	public:
        DECLARE_SERIALIZE()

        ParticleGeneratorComponent();
        ~ParticleGeneratorComponent();
        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return btrue; }
        virtual bbool needsDraw2D() const { return bfalse; }
        virtual bbool keepAlive();

        virtual void Update( f32 _deltaTime );
        virtual void batchPrimitives( const ITF_VECTOR <class View*>& _views );
        virtual void onEvent(Event * _event);
        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void onResourceLoaded();
        virtual void onUnloadResources();
        virtual void onBecomeActive();

        void    updateAABB();
        void    start();
        void    stop();
        void    pause(bbool _pause) {m_isPause = _pause;}

        template <typename T>
        void                        setInput(StringID _inputName, T _value );
        const InputContainer &      getInputs() const { return m_inputList; }
        void                        setUpdateParticleGeneratorInput( IUpdateFxInputCriteria* _update ) { m_particleGeneratorInputUpdate = _update; }

    private:

        ITF_INLINE const class ParticleGeneratorComponent_Template*  getTemplate() const;

        void processFlip(bbool _flip);

        f32     m_velocityNorm;
        ITF_ParticleGenerator m_particleGenerator;
        f32     m_maxFrequency;
        f32     m_increment;
        bbool   m_isPause;
        bbool   m_isFlipped;

        InputContainer      m_inputList;
        IUpdateFxInputCriteria* m_particleGeneratorInputUpdate;
    };

    template <typename T>
    ITF_INLINE void ParticleGeneratorComponent::setInput(StringID _inputName, T _value )
    {
        u32 numInputs = m_inputList.size();

        for ( u32 i = 0; i < numInputs; i++ )
        {
            Input& input = m_inputList[i];

            if (input.getId() == _inputName)
            {
                input.setValue(_value);
                break;
            }
        }
    }

    class ParticleGeneratorComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(ParticleGeneratorComponent_Template,GraphicComponent_Template,4010009333);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(ParticleGeneratorComponent);

    public:

        ParticleGeneratorComponent_Template();
        ~ParticleGeneratorComponent_Template() {}

        virtual bbool onTemplateLoaded( bbool _hotReload );
        virtual void onTemplateDelete( bbool _hotReload );

        const ITF_ParticleGenerator_Template&   getParticuleGenerator() const { return m_particleGenerator; }
        f32                                     getStartTime() const { return m_startTime; }
        f32                                     getStopTime() const { return m_stopTime; }
        bbool                                   getBeginStart() const { return m_beginStart; }

        const ProceduralInputData&  getFrequencyInput()     const       {return m_frequencyInput;}
        const ProceduralInputData&  getEmitCountInput()     const       {return m_emitCountInput;}
        const ProceduralInputData&  getmaxParticlesInput() const       {return m_maxParticlesInput;}
        const ITF_VECTOR<InputDesc>& getInputList()          const       { return m_inputList; }
        const GFX_MATERIAL & getMaterial() const { return m_material; }

    private:

        ITF_ParticleGenerator_Template  m_particleGenerator;
        GFXMaterialSerializable         m_material;
        f32                             m_startTime;
        f32                             m_stopTime;
        bbool                           m_beginStart;

        ProceduralInputData m_frequencyInput;
        ProceduralInputData m_emitCountInput;
        ProceduralInputData m_maxParticlesInput;
        ITF_VECTOR<InputDesc> m_inputList;
    };




    ITF_INLINE const ParticleGeneratorComponent_Template*  ParticleGeneratorComponent::getTemplate() const
    {return static_cast<const ParticleGeneratorComponent_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_PARTICLEGENERATORCOMPONENT_H_
