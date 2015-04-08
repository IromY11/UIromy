#ifndef _ITF_GRIDFLUIDCOMPONENT_H_
#define _ITF_GRIDFLUIDCOMPONENT_H_

#define GridFluidComponent_CRC ITF_GET_STRINGID_CRC(GridFluidComponent,1633526776)
#define GridFluidComponent_Template_CRC ITF_GET_STRINGID_CRC(GridFluidComponent_Template,3308160340)

#ifndef _ITF_GRIDFLUID_H_
#include "engine/display/GridFluid/GridFluid.h"
#endif // _ITF_GRIDFLUID_H_

#ifdef GFX_USE_GRID_FLUIDS

namespace ITF
{
    class GridFluidComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(GridFluidComponent, ActorComponent, GridFluidComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        GridFluidComponent();
        virtual ~GridFluidComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }
		virtual bbool       needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
		virtual void onStartDestroy(bbool _hotReload);
        virtual void        Update( f32 _dt );
        virtual void        batchPrimitives( const ITF_VECTOR <class View*>& _views );
        virtual void        onEvent( Event* _event);
		void				updateAABB();

#ifdef ITF_SUPPORT_EDITOR
        virtual void  drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
        virtual void  onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        ITF_INLINE virtual void		setUpdateDisabled(bbool _val) { Super::setUpdateDisabled(_val); m_fluid.m_active = !_val;}

    private:
        ITF_INLINE const class GridFluidComponent_Template* getTemplate() const;

		GFX_GridFluid			m_fluid;

		struct DensityStruct
		{
			DensityStruct(u32 _id, ActorRef _actorToSendTo) {id = _id; actorToSendTo = _actorToSendTo;}
			u32 id;
			ActorRef actorToSendTo;
		};
		ITF_MAP<ActorRef, DensityStruct>	m_densityList;
    };


    //---------------------------------------------------------------------------------------------------

    class GridFluidComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(GridFluidComponent_Template, ActorComponent_Template, GridFluidComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(GridFluidComponent)
        DECLARE_SERIALIZE()

    public:

        GridFluidComponent_Template();
        virtual ~GridFluidComponent_Template();


    private:

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const GridFluidComponent_Template* GridFluidComponent::getTemplate() const
    {
        return static_cast<const GridFluidComponent_Template*>(m_template);
    }
}

#endif // GFX_USE_GRID_FLUIDS

#endif // _ITF_GRIDFLUIDCOMPONENT_H_
