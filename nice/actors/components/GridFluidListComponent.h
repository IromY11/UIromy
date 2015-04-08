#ifndef _ITF_GRIDFLUIDLISTCOMPONENT_H_
#define _ITF_GRIDFLUIDLISTCOMPONENT_H_

#define GridFluidListComponent_CRC ITF_GET_STRINGID_CRC(GridFluidListComponent,4242398396)
#define GridFluidListComponent_Template_CRC ITF_GET_STRINGID_CRC(GridFluidListComponent_Template,1542042529)

#ifndef _ITF_GRIDFLUID_H_
#include "engine/display/GridFluid/GridFluid.h"
#endif // _ITF_GRIDFLUID_H_

#ifdef GFX_USE_GRID_FLUIDS

#define MRK_ActivateGrid ITF_GET_STRINGID_CRC(MRK_ActivateGrid,246998718)
#define MRK_DeactivateGrid ITF_GET_STRINGID_CRC(MRK_DeactivateGrid,21337661)

//#define GRID_MAP

namespace ITF
{
    class GridFluidListComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(GridFluidListComponent, ActorComponent, GridFluidListComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:
        GridFluidListComponent();
        virtual ~GridFluidListComponent();

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

        void                setAcivateGrid( u32 _gridIndex, bbool _active, bbool _useAnim );
#ifdef ITF_SUPPORT_EDITOR
        virtual void  drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
        virtual void  onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        virtual void		setUpdateDisabled(bbool _val);
#ifdef GRID_MAP
        const ITF_MAP<StringID, GFX_GridFluid>& getGridList() const{return m_fluidList;};
#else
        const ITF_VECTOR<GFX_GridFluid>& getGridList() const{return m_fluidList;};
#endif

    private:
        ITF_INLINE const class GridFluidListComponent_Template* getTemplate() const;
        void                updateDeactivate(f32 _dt);
#ifdef GRID_MAP
        ITF_MAP<StringID, GFX_GridFluid> m_fluidList;
#else
        ITF_VECTOR<GFX_GridFluid> m_fluidList;
#endif
		
        ITF_MAP<u32, f32> m_pendingDeactivate;
		f32               m_delayDesactivate;

		struct DensityStructList
		{
			DensityStructList(ActorRef _actorToSendTo) {actorToSendTo = _actorToSendTo;}

			ActorRef actorToSendTo;
			ITF_VECTOR<u32> ids;
		};
		ITF_MAP<ActorRef, DensityStructList>	m_densityList;
    };


    //---------------------------------------------------------------------------------------------------

    class GridFluidListComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(GridFluidListComponent_Template, ActorComponent_Template, GridFluidListComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(GridFluidListComponent)
        DECLARE_SERIALIZE()

    public:

        GridFluidListComponent_Template();
        virtual ~GridFluidListComponent_Template();


    private:

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const GridFluidListComponent_Template* GridFluidListComponent::getTemplate() const
    {
        return static_cast<const GridFluidListComponent_Template*>(m_template);
    }
}

#endif // GFX_USE_GRID_FLUIDS

#endif // _ITF_GridFluidListComponent_H_
