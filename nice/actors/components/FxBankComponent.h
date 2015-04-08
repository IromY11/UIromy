
#ifndef _ITF_FXBANKCOMPONENT_H_
#define _ITF_FXBANKCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "../actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SEEDER_H_
#include "core/Seeder.h"
#endif //_ITF_SEEDER_H_

#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_   

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

#ifndef _ITF_FXDESCRIPTOR_H_
#include "engine/display/FxDescriptor.h"
#endif //_ITF_FXDESCRIPTOR_H_

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{
    class ITF_ParticleGenerator;

    typedef u32 FXHandle;
    enum { InvalidFXHandle = U32_INVALID };

    struct FxInstance
    {
        FxInstance()
            : m_gen(NULL)
            , m_playMode(0)
            , m_generatorIndexInPool(-1)
            , m_material(NULL)
            , m_instanceHandle(u8(-1))
         {
             clear();
         }
        enum InstanceState
        {
            InstanceState_Unused,
            InstanceState_Playing,
            InstanceState_Stopping,
        };

        void clear();
        ITF_ParticleGenerator*  m_gen;
        FxDescriptor *          m_descriptor;
        InstanceState           m_state;
        bbool                   m_emitFromBase;
        bbool                   m_useActorSpeed;
        bbool                   m_useActorOrientation;
        bbool                   m_useActorAlpha;
        bbool                   m_recomputeColorFactor;
        f32                     m_angle;
        u32                     m_boneIndex;
        BOOL_COND               m_useBoneOrientation;
        Vec3d                   m_pos;
        f32                     m_timer;
        StringID                m_name;
        u32                     m_playMode;
        i32                     m_generatorIndexInPool;
        bbool                   m_isOwner;
        const GFX_MATERIAL  *   m_material;
        f32                     m_fxAlpha;
        FXHandle                m_instanceHandle;       
    };

#define FxBankComponent_CRC ITF_GET_STRINGID_CRC(FxBankComponent,2523615645)

    class FxBankComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(FxBankComponent,GraphicComponent,2523615645);

    public:
        DECLARE_SERIALIZE()

        FxBankComponent();
        ~FxBankComponent();
        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return !m_draw2D; }
        virtual bbool needsDraw2D() const { return m_draw2D; }
        virtual bbool keepAlive();

		virtual void    onBecomeInactive();

        virtual void batchPrimitives( const ITF_VECTOR <class View*>& _views );
        virtual void batchPrimitives2D( const ITF_VECTOR <class View*>& _views );
#ifdef ITF_SUPPORT_EDITOR
        virtual void drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 /*_flags*/  )  const;
#endif // ITF_SUPPORT_EDITOR

        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void Update( f32 _deltaTime );

		bbool				getInvisible()							{ return m_Invisible;	}
		void				setInvisible(const bbool _b)			{ m_Invisible = _b;	}
															  
        virtual void onEvent(Event * _event);

        struct playFxParams {
            playFxParams()
                :m_boneIndex(U32_INVALID)
                ,m_useBoneOrientation(BOOL_false)
                ,m_emitFromBase(bfalse)
                ,m_useActorSpeed(btrue)
                ,m_useActorOrientation(bfalse)
                ,m_useActorAlpha(btrue)
                ,m_lifeTimeMult(1.0f)
                ,m_FxDesc(NULL)
                ,m_isOwner(btrue)
				, m_pickColorFromFrieze(bfalse)
				, m_gfxPrimitiveParamNeededForBank(NULL)
            {}

            u32   m_boneIndex;
			bbool m_pickColorFromFrieze;
            BOOL_COND m_useBoneOrientation;
            bbool m_emitFromBase;
            bbool m_useActorSpeed;
            bbool m_useActorOrientation;
            bbool m_useActorAlpha;
            f32   m_lifeTimeMult;
            const FxDescriptor_Template * m_FxDesc;
            bbool m_isOwner;
			GFXPrimitiveParam * m_gfxPrimitiveParamNeededForBank;
        };

        FXHandle playFx(StringID _tag, const playFxParams& _params = playFxParams());
        void stopFx(FXHandle _hdl, bbool _stopOnlyLooped = bfalse);
        void stopFxImmediate(FXHandle _hdl);

        bbool isPlaying(FXHandle _hdl) const;

        template <typename T>
        void                        setInput(StringID _inputName, T _value );
        const InputContainer &      getInputs() const { return m_inputList; }
        void                        setUpdateFxInput( IUpdateFxInputCriteria* _update ) { m_fxInputUpdate = _update; }
        u32                         getNumInputs() const { return m_inputList.size();}
#ifdef ITF_DEBUG_STRINGID
        const char*                 getInputName( u32 _index ) const { return m_inputList[_index].getName(); }
#endif
        InputType                   getInputType( u32 _index ) const { return m_inputList[_index].getType(); }

        template <typename T>
        void                        getInputValue( u32 _index, const T& _val ) const
        {
            const Input& input = m_inputList[_index];
            input.getValue(_val);
        }

        ITF_ParticleGenerator*      getParticleGenerator(FXHandle _hdl);
        const StringID&             getInstanceName(FXHandle _hdl);
        void                        setFXPos(FXHandle _hdl, const Vec3d& _pos, FXPlayMode _mode = FXPlayMode_Position);
        void                        setFXAngle(FXHandle _hdl, f32 _angle);
        void                        attachToBone(FXHandle _hdl, u32 _boneIndex);
        void                        setLifeTimeMultiplier(FXHandle _hdl, f32 _multiplier);

    private:
    
		bbool m_Invisible;

		u16 m_uidHandleGenerator;

		u32 getActiveInstanceFromHandle(FXHandle _hdl) const;

        FXHandle acquireFXHandle(u32 _activeInstanceIdx);
        void releaseFXHandle(FXHandle & _handle);
        void relocateFXHandleEntry(FXHandle _handle, u32 _newActiveInstanceIdx);

        static u32 getEntryFromHdl(FXHandle _handle);
        static u32 getUIDFromHdl(FXHandle _handle);
        static FXHandle buildHandle(u32 _hdlEntry, u32 _magic);

		ITF_INLINE const class FxBankComponent_Template*  getTemplate() const;

        void clear();
        void computeDraw2D();

        void updateInstance( FxInstance &pFxInstance, f32 _deltaTime );

        void updateAABB( FxInstance &_instance );

        void setInputs( FxInstance &_instance, f32 _deltaTime );

        void setPosAndAngle( FxInstance &_instance );

        static bbool isPlaying(const FxInstance &_instance );
        static void stopFx(FxInstance &_instance, bbool _stopOnlyLooped = bfalse);
        void clearInstance(FxInstance & _instance);

        typedef ITF_MAP<StringID,u32>             FxMap;
        typedef ITF_VECTOR<FxDescriptor>          FxList;
        
        typedef vector<FxInstance> VecFxInstance;
        VecFxInstance               m_activeInstances;

        class AnimLightComponent *  m_animComponent;
        class PhysComponent *       m_physComponent;

        struct HandleEntry
        {
            u16 m_uid;
            u16 m_activeInstanceIdx;
        };
        vector<HandleEntry>     m_handles;

        FxMap                       m_Fx;
        FxList                      m_FxList;
        bbool                       m_draw2D;

        IUpdateFxInputCriteria*     m_fxInputUpdate;
        InputContainer              m_inputList;

		// Used to lock slots on feedbackFXManager
		ITF_MAP<StringID,FxDescriptor *> m_feedbackFxs;

        #ifdef ITF_SUPPORT_DEBUGFEATURE
		bbool m_DBGdraw;
        Vec2d m_DBGTextOffset;
        #endif // ITF_SUPPORT_DEBUGFEATURE
    };

    template <typename T>
    ITF_INLINE void FxBankComponent::setInput(StringID _inputName, T _value )
    {
        u32 numInputs = m_inputList.size();

        for ( u32 i = 0; i < numInputs; i++ )
        {
            Input& input = m_inputList[i];

            if ( input.getId() == _inputName )
            {
                input.setValue(_value);
                break;
            }
        }
    }

    class FxBankComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(FxBankComponent_Template,GraphicComponent_Template,12983557);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(FxBankComponent);

    public:

        FxBankComponent_Template();
        ~FxBankComponent_Template();

        typedef ITF_VECTOR<FxDescriptor_Template>   FxList;

		virtual bbool   onTemplateLoaded( bbool _hotReload );
        virtual void    onTemplateDelete( bbool _hotReload );

        u32 getMaxActiveInstance() const { return m_maxActiveInstance; }

        ITF_INLINE FxList&		                getFxList() { return m_FxList; }
        ITF_INLINE const FxList&		        getFxList() const { return m_FxList; }
        ITF_INLINE const ITF_VECTOR<InputDesc>& getInputList() const { return m_inputList; }

        const GFX_MATERIAL * getMaterial(const StringID& _descName) const;
        bbool getVisibilityTest() const {return m_FxVisibilityTest;}

    private:
        u32                             m_maxActiveInstance;
        FxList                          m_FxList;
        ITF_VECTOR <InputDesc>          m_inputList;
        bbool                           m_FxVisibilityTest;
        typedef ITF_MAP<StringID, GFXMaterialSerializable *> MaterialMap;
        MaterialMap m_materialMap;
    };

    ITF_INLINE const FxBankComponent_Template* FxBankComponent::getTemplate() const
    {
        return static_cast<const FxBankComponent_Template*>(m_template);
    }
}

#endif // _ITF_FXBANKCOMPONENT_H_

