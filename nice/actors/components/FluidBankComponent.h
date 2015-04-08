
#ifndef _ITF_FLUIDBANKCOMPONENT_H_
#define _ITF_FLUIDBANKCOMPONENT_H_

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

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_GRIDFLUIDMODIFIERCOMPONENT_H_
#include "engine/actors/components/GridFluidModifierComponent.h"
#endif //_ITF_GRIDFLUIDMODIFIERCOMPONENT_H_

namespace ITF
{
    typedef u32 FluidHandle;
	const u32 InvalidFluidHandle = U32_INVALID;

    struct playFluidParams {
        playFluidParams()
            :m_boneIndex(U32_INVALID)
            ,m_useBoneOrientation(false)
            ,m_emitFromBase(bfalse)
            ,m_useActorOrientation(bfalse)
			,m_attach(false)
            ,m_angle(F32_INFINITY)
            ,m_pos(Vec3d(F32_INFINITY,F32_INFINITY,F32_INFINITY))
        {}

        Vec3d m_pos;                            // Code Pos
        f32   m_angle;                          // Code Angle
        u32   m_boneIndex;
		u32   m_useBoneOrientation : 1;
        u32   m_emitFromBase : 1;
        u32	  m_useActorOrientation : 1;
        u32	  m_attach : 1;
    };

#ifdef GFX_USE_GRID_FLUIDS

	#define FluidBankComponent_CRC ITF_GET_STRINGID_CRC(FluidBankComponent,4148724578)
	#define FluidBankComponent_Template_CRC ITF_GET_STRINGID_CRC(FluidBankComponent_Template,1870766221)

    class FluidBankComponent : public GraphicComponent
    {
		friend class FluidBankComponent_Template;
        DECLARE_OBJECT_CHILD_RTTI(FluidBankComponent,GraphicComponent,FluidBankComponent_CRC);

    public:
        DECLARE_SERIALIZE()

        FluidBankComponent();
        ~FluidBankComponent();
        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return true; }
        virtual bbool needsDraw2D() const { return false; }

		virtual void    onBecomeInactive();

        virtual void batchPrimitives( const ITF_VECTOR <class View*>& _views );

        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void Update( f32 _deltaTime );

        FluidHandle playFluid(StringID _tag, const playFluidParams& _params = playFluidParams());
        void stopFluid(FluidHandle _hdl);

        bbool isPlaying(FluidHandle _hdl) const;

        void setFXPos( FluidHandle _hdl, Vec3d _pos);
        void setFXAngle( FluidHandle _hdl, f32 _angle);


    private:

		void updateInstanceMatrix( GFX_GridFluidModifierList &_modifierInstance, const playFluidParams &_params );
    
		ITF_INLINE const class FluidBankComponent_Template*  getTemplate() const;

        void clear();

        class AnimLightComponent *  m_animComponent;

		ITF_VECTOR<GFX_GridFluidModifierList>  m_modifierInstanceList;
        ITF_VECTOR<playFluidParams>			   m_paramInstanceList;
    };


	class FluidBankComponent_Template : public GraphicComponent_Template
    {
		friend class FluidBankComponent;
        DECLARE_OBJECT_CHILD_RTTI(FluidBankComponent_Template,GraphicComponent_Template,FluidBankComponent_Template_CRC);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(FluidBankComponent);

    public:

        FluidBankComponent_Template();
        ~FluidBankComponent_Template();

		virtual bbool   onTemplateLoaded( bbool _hotReload );
        virtual void    onTemplateDelete( bbool _hotReload );

        u32 getMaxActiveInstance() const { return m_maxActiveInstance; }

    private:
        u32									   m_maxActiveInstance;
        ITF_VECTOR<GFX_GridFluidModifierList>  m_modifierTemplateList;
    };

    ITF_INLINE const FluidBankComponent_Template* FluidBankComponent::getTemplate() const
    {
        return static_cast<const FluidBankComponent_Template*>(m_template);
    }

#else

	// Empty class
    class FluidBankComponent : public GraphicComponent
    {
	public:
        FluidHandle playFluid(StringID _tag, const playFluidParams& _params = playFluidParams()) { return InvalidFluidHandle; }
		void stopFluid(FluidHandle _hdl) {}
        void setFXPos( FluidHandle _hdl, Vec3d _pos) {}
        void setFXAngle( FluidHandle _hdl, f32 _angle) {}

	};
#endif // GFX_USE_GRID_FLUIDS
}

#endif // _ITF_FLUIDBANKCOMPONENT_H_

