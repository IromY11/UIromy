#ifndef ITF_GFX_ZLIST_H_
#define ITF_GFX_ZLIST_H_

namespace ITF
{
    class GFXPrimitive;
    class AFXPostProcess;
	class GFX_Light3D;
	class GFX_RenderParam;
	class GFX_GridFluidModifierList;

    template<class PrimType>
    struct ZList_Node
    {
        PrimType *      m_primitive;
        f32             m_depth;
        
        // m_ref only available for debugging purpose in not final
#ifndef ITF_FINAL
        ConstObjectRef  m_ref;
        void setObjectRef(const ObjectRef & _objRef) { m_ref = _objRef; }
        ConstObjectRef  getObjectRef() const { return m_ref; }
#else
        void setObjectRef(const ObjectRef & _objRef) { }
        ConstObjectRef  getObjectRef() const { return ConstObjectRef(); }
#endif
    };

    // Right now, GFX_Zlist is only available for GFXPrimitive and AFXPostProcess
    template<class P>
    class GFX_Zlist
    {
    public:
        typedef P PrimType;
        typedef ZList_Node<PrimType> ZListNode;

        GFX_Zlist();

        void            reset();

        void            addNode(PrimType * _pPrimitive, f32 _zordering, const ObjectRef & _ref);

        u32             getNumberEntry() const { return m_zListBufferNode.size(); }
        u32             getPassFlag() const { return m_passFlag; }

        void            sort();

        const ZListNode &  getNodeAt(u32 _index) const {return m_zListBufferNode[_index]; }
        ZListNode &        getNodeAt(u32 _index) {return m_zListBufferNode[_index]; }

    protected:
        ITF::vector<ZListNode> m_zListBufferNode;
        u32                    m_passFlag;
    };

    typedef ZList_Node<GFXPrimitive> ZList_Node_Prim;
    typedef ZList_Node<AFXPostProcess> ZList_Node_AfterFx;

    // ZLIST MANAGER 

    enum ZLIST_TYPE
    {
        GFX_ZLIST_MAIN,
        GFX_ZLIST_3D,
        GFX_ZLIST_RTARGET,
        GFX_ZLIST_AFTERFX,
        GFX_ZLIST_2D,
        GFX_ZLIST_SCREENSHOT_2D, // for rendering of primitives on screenshot
        GFX_ZLIST_LIGHT_3D,
        GFX_ZLIST_RENDER_PARAM,
        GFX_ZLIST_PRERENDER_PRIM,
#ifdef GFX_USE_GRID_FLUIDS
        GFX_ZLIST_GFLUID_MODIFIER,
#endif // GFX_USE_GRID_FLUIDS
        GFX_ZLIST_MASK,
        GFX_ZLIST_TYPE_COUNT,
    };

    // class to deduce the primitive type from the ZListType
    template <ZLIST_TYPE _zlistType>
    struct ZListTypeInfo
    {
    };

    // specialized for each ZListType
    template <>
    struct ZListTypeInfo<GFX_ZLIST_MAIN>
    {
        typedef GFXPrimitive PrimType;
    };

    template <>
    struct ZListTypeInfo<GFX_ZLIST_3D>
    {
        typedef GFXPrimitive PrimType;
    };

    template <>
    struct ZListTypeInfo<GFX_ZLIST_AFTERFX>
    {
        typedef AFXPostProcess PrimType;
    };

    template <>
    struct ZListTypeInfo<GFX_ZLIST_2D>
    {
        typedef GFXPrimitive PrimType;
    };

    template <>
    struct ZListTypeInfo<GFX_ZLIST_SCREENSHOT_2D>
    {
        typedef GFXPrimitive PrimType;
    };

    template <>
    struct ZListTypeInfo<GFX_ZLIST_LIGHT_3D>
    {
        typedef GFX_Light3D PrimType;
    };

    template <>
    struct ZListTypeInfo<GFX_ZLIST_RENDER_PARAM>
    {
        typedef GFX_RenderParam PrimType;
    };

    template <>
    struct ZListTypeInfo<GFX_ZLIST_PRERENDER_PRIM>
    {
        typedef GFXPrimitive PrimType;
    };

#ifdef GFX_USE_GRID_FLUIDS
    template <>
    struct ZListTypeInfo<GFX_ZLIST_GFLUID_MODIFIER>
    {
        typedef GFX_GridFluidModifierList PrimType;
    };
#endif // GFX_USE_GRID_FLUIDS

    template <>
    struct ZListTypeInfo<GFX_ZLIST_MASK>
    {
        typedef GFXPrimitive PrimType;
    };
    //

    enum { ZlistPriorityCount2d = 32u };


    class ZListViewNode
    {
    public:
        
		ZListViewNode()
		{
			m_used = false;
		}

        void reset();

        template<ZLIST_TYPE _zListType>
        inline GFX_Zlist<typename ZListTypeInfo<_zListType>::PrimType> & getList(); // specialized for each type

        GFX_Zlist<GFXPrimitive> & get2DList(u32 _priority)
        {
            return m_render2D[_priority];
        }

		bbool					m_used;
    private:
        GFX_Zlist<GFXPrimitive> m_renderZlist;
        GFX_Zlist<GFXPrimitive> m_render3Dlist;
        GFX_Zlist<GFXPrimitive> m_renderZlistRTarget;
        GFX_Zlist<AFXPostProcess> m_afterFxZlist;
        GFX_Zlist<GFXPrimitive> m_render2D[ZlistPriorityCount2d];
        GFX_Zlist<GFXPrimitive>				m_screenshot; // rendering of primitives on screenshot
        GFX_Zlist<GFX_Light3D>				m_light3DList;
		GFX_Zlist<GFX_RenderParam>			m_renderParamList;
		GFX_Zlist<GFXPrimitive>				m_prerenderPrimList;
		GFX_Zlist<GFX_GridFluidModifierList>	m_gFluidModifierPrimList;
        GFX_Zlist<GFXPrimitive> m_renderMaskList;
    };

    // dispatch on each zlist via specialisation
    template<>
    inline GFX_Zlist<GFXPrimitive> & ZListViewNode::getList<GFX_ZLIST_MAIN>()
    {
        return m_renderZlist;
    }

    template<>
    inline GFX_Zlist<GFXPrimitive> & ZListViewNode::getList<GFX_ZLIST_3D>()
    {
        return m_render3Dlist;
    }

    template<>
    inline GFX_Zlist<AFXPostProcess> & ZListViewNode::getList<GFX_ZLIST_AFTERFX>()
    {
        return m_afterFxZlist;
    }

    template<>
    inline GFX_Zlist<GFXPrimitive> & ZListViewNode::getList<GFX_ZLIST_SCREENSHOT_2D>()
    {
        return m_screenshot;
    }    

    template<>
    inline GFX_Zlist<GFX_Light3D> & ZListViewNode::getList<GFX_ZLIST_LIGHT_3D>()
    {
        return m_light3DList;
    }    

    template<>
    inline GFX_Zlist<GFX_RenderParam> & ZListViewNode::getList<GFX_ZLIST_RENDER_PARAM>()
    {
        return m_renderParamList;
    }    

    template<>
    inline GFX_Zlist<GFXPrimitive> & ZListViewNode::getList<GFX_ZLIST_PRERENDER_PRIM>()
    {
        return m_prerenderPrimList;
    }    

#ifdef GFX_USE_GRID_FLUIDS
    template<>
    inline GFX_Zlist<GFX_GridFluidModifierList> & ZListViewNode::getList<GFX_ZLIST_GFLUID_MODIFIER>()
    {
        return m_gFluidModifierPrimList;
    }    
#endif // GFX_USE_GRID_FLUIDS

    template<>
    inline GFX_Zlist<GFXPrimitive> & ZListViewNode::getList<GFX_ZLIST_MASK>()
    {
        return m_renderMaskList;
    }    
    // other types are not valid


    class GFX_ZlistManager
    {
    public:
        GFX_ZlistManager();

        u32     createZListView();
        void    destroyAll();
        void    destroy(ux _zlistID);

        void    resetAll();
        void    reset(ux _zlistID);
		u32		getUnusedZListView();

        template<ZLIST_TYPE _listType>
        void AddPrimitiveInZList(const ITF_VECTOR <class View*>& _views, typename ZListTypeInfo<_listType>::PrimType * _pPrimitive, f32 _depth, const ObjectRef& _ref);
        template<ZLIST_TYPE _listType>
        void AddPrimitiveInZList(const class View & _view, typename ZListTypeInfo<_listType>::PrimType * _pPrimitive, f32 _depth, const ObjectRef& _ref);

        template<ZLIST_TYPE _listType>
        inline GFX_Zlist<typename ZListTypeInfo<_listType>::PrimType> & getZlistAt(u32 _zlistID);
        inline GFX_Zlist<GFXPrimitive> & get2DZlistAt(ux _zlistID, ux _listPrio);

        static void     setZEqualDebugMode(bbool _bOn);
        static bbool    getZEqualDebugMode();

    protected:
        ITF_VECTOR<ZListViewNode*> m_zListViewNode;
    };

    // 2D add primitive in ZList is specialized
    template<>
    void GFX_ZlistManager::AddPrimitiveInZList<GFX_ZLIST_2D>( const View & _view, GFXPrimitive * _pPrimitive, f32 _depth, const ObjectRef& _ref);

    template<ZLIST_TYPE _listType>
    inline GFX_Zlist<typename ZListTypeInfo<_listType>::PrimType>& GFX_ZlistManager::getZlistAt(u32 _zlistID)
    {
        return m_zListViewNode[_zlistID]->getList<_listType>();
    }

    inline GFX_Zlist<GFXPrimitive> & GFX_ZlistManager::get2DZlistAt(ux _zlistID, ux _listPrio)
    {
        return m_zListViewNode[_zlistID]->get2DList(_listPrio);
    }
}

#endif // ITF_GFX_ZLIST_H_
