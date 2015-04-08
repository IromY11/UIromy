#include "precompiled_engine.h"

#include <algorithm>

#ifndef ITF_GFX_ZLIST_H_
#include "GFXZList.h"
#endif // ITF_GFX_ZLIST_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_LIGHTMANAGER_H_
#include "engine/display/lighting/LightManager.h"
#endif // _ITF_LIGHTMANAGER_H_

#ifndef _ITF_RENDERPARAMMANAGER_H_
#include "engine/display/RenderParamManager.h"
#endif //_ITF_RENDERPARAMMANAGER_H_

#ifndef _ITF_GRIDFLUID_H_
#include "engine/display/GridFluid/GridFluid.h"
#endif // _ITF_GRIDFLUID_H_

namespace ITF
{

    static bbool gs_zEqualDebugMode = bfalse;

    template<class P>
    GFX_Zlist<P>::GFX_Zlist()
    {
        m_passFlag = 0;
    }

    template<class P>
    void GFX_Zlist<P>::addNode(PrimType * _pPrimitive, f32 _zordering, const ObjectRef & _ref)
    {
        m_zListBufferNode.emplace_back();

        ZList_Node<PrimType> & newNode = m_zListBufferNode.back();

        newNode.m_depth = _zordering;
        newNode.m_primitive = _pPrimitive;
        newNode.setObjectRef(_ref);
        m_passFlag |= _pPrimitive->getPassFilterFlag();
    }

	// Try to check invalid primitives. David
	template<>
    void GFX_Zlist<GFXPrimitive>::addNode(PrimType * _pPrimitive, f32 _zordering, const ObjectRef & _ref)
    {
		//ITF_ASSERT( _pPrimitive->getType() <= 2 );

        m_zListBufferNode.emplace_back();

        ZList_Node<PrimType> & newNode = m_zListBufferNode.back();

        newNode.m_depth = _zordering;
        newNode.m_primitive = _pPrimitive;
        newNode.setObjectRef(_ref);
        m_passFlag |= _pPrimitive->getPassFilterFlag();
    }

    template<class P>
    void GFX_Zlist<P>::reset()
    {
        ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
        m_zListBufferNode.clear();
        m_passFlag = 0;
    }



    // comparison fcts for sorting
    template<class PrimLess>
    struct ZListNodeDepthAndAddressLess
    {
        // sort first per Z then per address
        template<class PrimType>
        inline bool operator()(const ZList_Node<PrimType> & _l, const ZList_Node<PrimType> & _r)
        {
            if (_l.m_depth == _r.m_depth)
                return PrimLess()(_l.m_primitive, _r.m_primitive);
            return _l.m_depth < _r.m_depth;
        }
    };

    struct PrimAddressLess
    {
        template<class PrimType>
        inline bool operator()(PrimType * _l, PrimType * _r)
        {
#ifndef ITF_FINAL
            // To get the order the less determinist we take the address in a 64K page
            return (uPtr(_l) & uPtr(0x0000ffffu)) < (uPtr(_r) & uPtr(0x0000ffffu)); 
#else
            return _l < _r;
#endif
        }
    };


    // Alternatively prim less used for sorting in debug mode (to get Z-Equal primitives blinking)
    struct PrimAddressAlternateLess
    {
        template<class PrimType>
        inline bool operator()(PrimType * _l, PrimType * _r)
        {
#ifndef ITF_FINAL
            // To get the order the less determinist we take the address in a 64K page
            return (uPtr(_r) & uPtr(0x0000ffffu)) < (uPtr(_l) & uPtr(0x0000ffffu)); 
#else
            return _r < _l;
#endif
        }
    };

    template<class P>
    void GFX_Zlist<P>::sort()
    {
        if (gs_zEqualDebugMode && ((CURRENTFRAME>>1u) & 1u)!=0) // inverted sort mode ?
        {
            std::sort(m_zListBufferNode.begin(), m_zListBufferNode.end(), ZListNodeDepthAndAddressLess<PrimAddressLess>());
        }
        else
        {
            std::sort(m_zListBufferNode.begin(), m_zListBufferNode.end(), ZListNodeDepthAndAddressLess<PrimAddressAlternateLess>());
        }
    }

    // only instantiate valid GFXZList 
    template class GFX_Zlist<GFXPrimitive>;
    template class GFX_Zlist<AFXPostProcess>;

    // ZListViewNode

    void ZListViewNode::reset()
    {
        m_renderZlist.reset();
        m_render3Dlist.reset();
        m_renderZlistRTarget.reset();
        m_afterFxZlist.reset();
        for(ux i = 0; i<ZlistPriorityCount2d; ++i)
        {
            m_render2D[i].reset();
        }
        m_screenshot.reset();
        m_light3DList.reset();
        m_renderParamList.reset();
        m_prerenderPrimList.reset();
        m_gFluidModifierPrimList.reset();
		m_renderMaskList.reset();
    }

    // ZLIST MANAGER 

    GFX_ZlistManager::GFX_ZlistManager()
    {
    }

    template<ZLIST_TYPE _listType>
    void GFX_ZlistManager::AddPrimitiveInZList( const ITF_VECTOR <class View*>& _views,
                                                typename ZListTypeInfo<_listType>::PrimType * _pPrimitive,
                                                f32 _depth,
                                                const ObjectRef& _ref)
    {
        for(ITF_VECTOR<class View*>::const_iterator it = _views.begin(), itEnd = _views.end();
            it!=itEnd;
            it++)
        {
            AddPrimitiveInZList<_listType>(**it, _pPrimitive, _depth, _ref);
        }
    }

    template<ZLIST_TYPE _listType>
    void GFX_ZlistManager::AddPrimitiveInZList( const View & _view,
                                                typename ZListTypeInfo<_listType>::PrimType * _pPrimitive,
                                                f32 _depth,
                                                const ObjectRef& _ref)
    {
        ITF_ASSERT(!isAddressProbablyOnStack(_pPrimitive));
        ITF_ASSERT(_listType<GFX_ZLIST_TYPE_COUNT);
		if ( _view.isObjectVisibleInViewport( _pPrimitive->getViewportVisibility() ) )
		{
			u32 zlistId = _view.getZlistID();
//			ITF_ASSERT(zlistId != View::ZlistID_invalid);

			if(zlistId != View::ZlistID_invalid)
			{
				getZlistAt<_listType>(zlistId).addNode(_pPrimitive, _depth, _ref);
			}
		}
    }

    // 2D specialization -> _depth is converted to integer priority, and list choosed according to it
    template<>
    void GFX_ZlistManager::AddPrimitiveInZList<GFX_ZLIST_2D>( const View & _view,
                                                              GFXPrimitive * _pPrimitive,
                                                              f32 _depth,
                                                              const ObjectRef& _ref)
    {
        ITF_ASSERT(!isAddressProbablyOnStack(_pPrimitive));
        if ( _view.isObjectVisibleInViewport( _pPrimitive->getViewportVisibility() ) )
        {
            i32 priority = i32(_depth); // LHS here
            u32 clampedPriority = u32(Clamp(priority, 0, i32(ZlistPriorityCount2d-1u)));
            u32 zlistId = _view.getZlistID();
//            ITF_ASSERT(zlistId != View::ZlistID_invalid);
            if(zlistId != View::ZlistID_invalid)
            {
                get2DZlistAt(zlistId, clampedPriority).addNode(_pPrimitive, 0.f, _ref);
            }
        }
    }

    template<>
    void GFX_ZlistManager::AddPrimitiveInZList<GFX_ZLIST_MAIN>( const View & _view,
                                                GFXPrimitive * _pPrimitive,
                                                f32 _depth,
                                                const ObjectRef& _ref)
    {
        ITF_ASSERT(!isAddressProbablyOnStack(_pPrimitive));
		if ( _view.isObjectVisibleInViewport( _pPrimitive->getViewportVisibility() ) )
		{
			u32 zlistId = _view.getZlistID();

			if(zlistId != View::ZlistID_invalid)
			{
                if ( _pPrimitive->getCommonParam()->m_gfxMaskInfo != GFX_MASK_INFO_NONE )
                {
					getZlistAt<GFX_ZLIST_MASK>(zlistId).addNode(_pPrimitive, _depth, _ref);
					if ( ( _pPrimitive->getPassFilterFlag() & (GFX_ZLIST_PASS_REGULAR_FLAG|GFX_ZLIST_PASS_ZPREPASS_FLAG) ) &&
                         ( _pPrimitive->getCommonParam()->m_gfxMaskInfo != GFX_MASK_INFO_HOLE ) )
						getZlistAt<GFX_ZLIST_MAIN>(zlistId).addNode(_pPrimitive, _depth, _ref);
                }
				else if ( _pPrimitive->getPassFilterFlag() & GFX_ZLIST_PASS_MASK_FLAG )
				{
					getZlistAt<GFX_ZLIST_MASK>(zlistId).addNode(_pPrimitive, _depth, _ref);
					if ( _pPrimitive->getPassFilterFlag() & (GFX_ZLIST_PASS_REGULAR_FLAG|GFX_ZLIST_PASS_ZPREPASS_FLAG) )
						getZlistAt<GFX_ZLIST_MAIN>(zlistId).addNode(_pPrimitive, _depth, _ref);
				}
				else
					getZlistAt<GFX_ZLIST_MAIN>(zlistId).addNode(_pPrimitive, _depth, _ref);
			}
		}
    }

/*    template void GFX_ZlistManager::AddPrimitiveInZList<GFX_ZLIST_MAIN>( const View & _view,
                                                ZListTypeInfo<GFX_ZLIST_MAIN>::PrimType * _pPrimitive,
                                                f32 _depth,
                                                const ObjectRef& _ref);*/


    // as the templated methods are defined in cpp, we need to instantiate all the valid variants to get link success
    template void GFX_ZlistManager::AddPrimitiveInZList<GFX_ZLIST_MAIN>(const ITF_VECTOR <View*>& _views,
                                                                        ZListTypeInfo<GFX_ZLIST_MAIN>::PrimType * _pPrimitive,
                                                                        f32 _depth,
                                                                        const ObjectRef& _ref);



    template void GFX_ZlistManager::AddPrimitiveInZList<GFX_ZLIST_3D>(const ITF_VECTOR <View*>& _views,
                                                                        ZListTypeInfo<GFX_ZLIST_3D>::PrimType * _pPrimitive,
                                                                        f32 _depth,
                                                                        const ObjectRef& _ref);

    template void GFX_ZlistManager::AddPrimitiveInZList<GFX_ZLIST_AFTERFX>(const ITF_VECTOR <View*>& _views,
                                                                        ZListTypeInfo<GFX_ZLIST_AFTERFX>::PrimType * _pPrimitive,
                                                                        f32 _depth,
                                                                        const ObjectRef& _ref);

    template void GFX_ZlistManager::AddPrimitiveInZList<GFX_ZLIST_2D>(const ITF_VECTOR <View*>& _views,
                                                                        ZListTypeInfo<GFX_ZLIST_2D>::PrimType * _pPrimitive,
                                                                        f32 _depth,
                                                                        const ObjectRef& _ref);
    
    template void GFX_ZlistManager::AddPrimitiveInZList<GFX_ZLIST_SCREENSHOT_2D>(const ITF_VECTOR <View*>& _views,
                                                                        ZListTypeInfo<GFX_ZLIST_SCREENSHOT_2D>::PrimType * _pPrimitive,
                                                                        f32 _depth,
                                                                        const ObjectRef& _ref);

    template void GFX_ZlistManager::AddPrimitiveInZList<GFX_ZLIST_LIGHT_3D>(const ITF_VECTOR <View*>& _views,
                                                                        ZListTypeInfo<GFX_ZLIST_LIGHT_3D>::PrimType * _pPrimitive,
                                                                        f32 _depth,
                                                                        const ObjectRef& _ref);

    template void GFX_ZlistManager::AddPrimitiveInZList<GFX_ZLIST_RENDER_PARAM>(const ITF_VECTOR <View*>& _views,
                                                                        ZListTypeInfo<GFX_ZLIST_RENDER_PARAM>::PrimType * _pPrimitive,
                                                                        f32 _depth,
                                                                        const ObjectRef& _ref);

    template void GFX_ZlistManager::AddPrimitiveInZList<GFX_ZLIST_PRERENDER_PRIM>( const View & _view,
                                                                        ZListTypeInfo<GFX_ZLIST_PRERENDER_PRIM>::PrimType * _pPrimitive,
                                                                        f32 _depth,
                                                                        const ObjectRef& _ref);

    template void GFX_ZlistManager::AddPrimitiveInZList<GFX_ZLIST_PRERENDER_PRIM>(const ITF_VECTOR <View*>& _views,
                                                                        ZListTypeInfo<GFX_ZLIST_PRERENDER_PRIM>::PrimType * _pPrimitive,
                                                                        f32 _depth,
                                                                        const ObjectRef& _ref);

#ifdef GFX_USE_GRID_FLUIDS
    template void GFX_ZlistManager::AddPrimitiveInZList<GFX_ZLIST_GFLUID_MODIFIER>(const ITF_VECTOR <View*>& _views,
                                                                        ZListTypeInfo<GFX_ZLIST_GFLUID_MODIFIER>::PrimType * _pPrimitive,
                                                                        f32 _depth,
                                                                        const ObjectRef& _ref);
#endif

    u32 GFX_ZlistManager::createZListView()
    {
        ZListViewNode *newZlistView = newAlloc(mId_GfxAdapter, ZListViewNode());
		newZlistView->m_used = false;
        for(u32 i = 0; i < m_zListViewNode.size(); i++)
        {
            if(m_zListViewNode[i] == NULL)
            {
                m_zListViewNode[i] = newZlistView;
                return i;
            }
        }

        m_zListViewNode.push_back(newZlistView);
        return m_zListViewNode.size() - 1u;
    }

	u32	GFX_ZlistManager::getUnusedZListView()
	{
        for(u32 i = 0; i < m_zListViewNode.size(); i++)
        {
            if(m_zListViewNode[i] != NULL)
            {
                if ( m_zListViewNode[i]->m_used == false )
				{
					m_zListViewNode[i]->m_used = true;
					return i;
				}
            }
        }
		u32 id = createZListView();
		m_zListViewNode[id]->m_used = true;
		return id;
	}


    void GFX_ZlistManager::destroy(ux _zlistID)
    {
        delete m_zListViewNode[_zlistID];
        m_zListViewNode[_zlistID] = NULL;
    }

    void GFX_ZlistManager::destroyAll()
    {
        ux nbElement = m_zListViewNode.size();
        for(ux i = 0; i<nbElement; i++)
        {
            delete m_zListViewNode[i];
        }
        m_zListViewNode.clear();
    }

    void GFX_ZlistManager::reset(u32 _ZlistID)
    {
        if ( (_ZlistID != View::ZlistID_invalid) && (_ZlistID < m_zListViewNode.size() ) )
		{
            m_zListViewNode[_ZlistID]->reset();
			m_zListViewNode[_ZlistID]->m_used = false;
		}
   }

    void GFX_ZlistManager::resetAll()
    {
        for(u32 i = 0; i < m_zListViewNode.size(); i++)
        {
            m_zListViewNode[i]->reset();
            m_zListViewNode[i]->m_used = false;
        }
    }

    void     GFX_ZlistManager::setZEqualDebugMode(bbool _bOn)
    {
        gs_zEqualDebugMode = _bOn;
    }

    bbool    GFX_ZlistManager::getZEqualDebugMode()
    {
        return gs_zEqualDebugMode;
    }

}

