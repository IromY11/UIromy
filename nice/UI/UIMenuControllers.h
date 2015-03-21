#ifndef _ITF_UIMENUCONTROLLERS_H_
#define _ITF_UIMENUCONTROLLERS_H_

#ifndef _ITF_UIMENUBASIC_H_
#include "gameplay/components/UI/UIMenuBasic.h"
#endif //_ITF_UIMENUBASIC_H_

#ifndef _ITF_ACTORSPAWNER_H_
#include "gameplay/AI/Utils/ActorSpawner.h"
#endif //_ITF_ACTORSPAWNER_H_

#include "engine/AdaptersInterfaces/InputAdapter.h"

#define UIMenuControllers_CRC           ITF_GET_STRINGID_CRC(UIMenuControllers, 3606580086)
#define UIMenuControllers_Template_CRC  ITF_GET_STRINGID_CRC(UIMenuControllers_Template, 2913536740)

namespace ITF
{
    class UIMenuControllers : public UIMenuBasic
    {
    DECLARE_OBJECT_CHILD_RTTI(UIMenuControllers, UIMenuBasic, UIMenuControllers_CRC);

    public:
        DECLARE_SERIALIZE()  

        UIMenuControllers  ();
        virtual ~UIMenuControllers ();

        virtual void Update( f32 _deltaTime );

    protected:
        const class UIMenuControllers_Template* getTemplate() const;

        virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void onFinalizeLoad();
        virtual void onStartDestroy(bbool _hotReload);


        virtual void onBecomeActive();
        virtual void onBecomeInactive();
                void positionControllers();

                void checkForControllers();
                void spawnControllerActors();
                void despawnControllerActors();

				u32  getInternalControllerIdFromControllerId(u32 _controllerId, InputAdapter::PadType _padType = InputAdapter::Pad_Invalid);

        struct Controller
        {
            Controller() : m_padType(InputAdapter::Pad_Invalid), m_Connected(0), m_ActorRef(ActorRef::InvalidRef), m_isLoading(bfalse) {}
            ~Controller() {}
                
            u32						m_Connected;
            ActorRef                m_ActorRef;
			bbool					m_isLoading;
			InputAdapter::PadType	m_padType;
        };

		struct PlayerStatus
		{
			PlayerStatus() : m_Connected(bfalse), m_padType(InputAdapter::Pad_Invalid), m_padId(U32_INVALID) {}
			~PlayerStatus() {}

			bbool                   m_Connected;
			InputAdapter::PadType   m_padType;
			u32						m_padId;
		};

        
        ITF_VECTOR<Controller>  m_controllers;
		ITF_VECTOR<PlayerStatus> m_players;
        //ITF_VECTOR<Controller>  m_controllerTypes;

        u32                     m_connectedControllers;
        u32                     m_lastNumControllers;

        //ITF_VECTOR<ActorSpawner> m_actorSpawner;
        //ActorSpawner		    m_actorSpawner[InputAdapter::PadType_Count];

        bbool                   m_isActive;
		bbool					m_isControllersArrayFilled;
        bbool                   m_typeChanged;
    };


    class UIMenuControllers_Template : public UIMenuBasic_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIMenuControllers_Template, UIMenuBasic_Template, UIMenuControllers_Template_CRC);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIMenuControllers);

    public:

        UIMenuControllers_Template();
        ~UIMenuControllers_Template();

        virtual bbool           onTemplateLoaded( bbool _hotReload );

		ITF_INLINE u32			getControllerVectorSize() const { return m_ControllerActorPaths.size(); }
        ITF_INLINE const Path&  getControllerPath(u32 _pathIndex) const { return m_ControllerActorPaths[_pathIndex]; }

		ITF_INLINE u32			getControllerCount() const { return m_ControllerActorNames.size();	 }
		ITF_INLINE const StringID & getControllerName(u32 _index) const { return m_ControllerActorNames[_index]; }
		

        struct ControllerLayout
        {
            DECLARE_SERIALIZE()

            u32                 m_numControllers;
            ITF_VECTOR<Vec2d>   m_position;
            ITF_VECTOR<Vec2d>   m_scale; 
        };

        // per num controllers 1-5
        ITF_VECTOR<ControllerLayout>    m_ControllerLayouts;
        ITF_VECTOR<Path>                m_ControllerActorPaths;
        ITF_VECTOR<StringID>            m_ControllerActorNames;
    };

    ITF_INLINE const UIMenuControllers_Template* UIMenuControllers::getTemplate() const
    {
        return static_cast<const UIMenuControllers_Template*>(m_template);
    }
}
#endif // _ITF_UIMENUITEMTEXT_H_