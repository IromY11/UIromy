#ifndef _ITF_GRIDFLUIDMODIFIERCOMPONENT_H_
#define _ITF_GRIDFLUIDMODIFIERCOMPONENT_H_

#define GridFluidModifierComponent_CRC ITF_GET_STRINGID_CRC(GridFluidModifierComponent,3101228556)
#define GridFluidModifierComponent_Template_CRC ITF_GET_STRINGID_CRC(GridFluidModifierComponent_Template,2982692995)

#ifndef _ITF_GRIDFLUID_H_
#include "engine/display/GridFluid/GridFluid.h"
#endif // _ITF_GRIDFLUID_H_

#ifdef GFX_USE_GRID_FLUIDS

namespace ITF
{
    class GridFluidModifierComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(GridFluidModifierComponent, ActorComponent, GridFluidModifierComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        GridFluidModifierComponent();
        virtual ~GridFluidModifierComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }
		virtual bbool       needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        Update( f32 _dt );
        virtual void        batchPrimitives( const ITF_VECTOR <class View*>& _views );
        virtual void        onEvent( Event* _event);
		void				updateAABB();

        u32                 getModifierSize();
        virtual void		setActiveAll(bbool _active);
		virtual	void		setActive(u16 _idx, bbool _active); 
		virtual void		setFluidPosition(u32 idx, Vec3d _pos);

#ifdef ITF_SUPPORT_EDITOR
        virtual void  drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
        virtual void  onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR
        ITF_INLINE virtual void		setUpdateDisabled(bbool _val);

    private:
        ITF_INLINE const class GridFluidModifierComponent_Template* getTemplate() const;
		GFX_GridFluidModifierList m_modifierList;
		bbool					  m_reinit;
    };


    //---------------------------------------------------------------------------------------------------

    class GridFluidModifierComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(GridFluidModifierComponent_Template, ActorComponent_Template, GridFluidModifierComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(GridFluidModifierComponent)
        DECLARE_SERIALIZE()

    public:

        GridFluidModifierComponent_Template();
        virtual ~GridFluidModifierComponent_Template();


    private:

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const GridFluidModifierComponent_Template* GridFluidModifierComponent::getTemplate() const
    {
        return static_cast<const GridFluidModifierComponent_Template*>(m_template);
    }
}

#endif // GFX_USE_GRID_FLUIDS

#endif // _ITF_GridFluidModifierComponent_H_
