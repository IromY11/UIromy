#ifndef _ITF_RENDERBOXCOMPONENT_H_
#define _ITF_RENDERBOXCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_ANIMATIONMESHVERTEX_H_
#include    "engine/animation/AnimationMeshVertex.h"
#endif // _ITF_ANIMATIONMESHVERTEX_H_

#define RenderBoxComponent_CRC ITF_GET_STRINGID_CRC(RenderBoxComponent,3943433679)
#define RenderBoxComponent_Template_CRC ITF_GET_STRINGID_CRC(RenderBoxComponent_Template,1444533686)


namespace ITF
{
    struct UsedDataAmv
    {
        UsedDataAmv()
            : m_index(U32_INVALID)
            , m_override(bfalse)
            , m_pos(Vec3d::Zero)
            , m_flip(bfalse)
        {
        }
        
        u32     m_index;
        bbool   m_override;
        Vec3d   m_pos;
        bbool   m_flip;
    };


    //---------------------------------------------------------------------------------------------------
    class  RenderSingleAnimData : public SingleAnimData
    {
        DECLARE_SERIALIZE()
        DECLARE_OBJECT_CHILD_RTTI(RenderSingleAnimData,SingleAnimData,ITF_GET_STRINGID_CRC(RenderSingleAnimData,2911136265))

    public:
        RenderSingleAnimData()
            : SingleAnimData()
            , m_xMin(0.f)
            , m_yMin(0.f)
            , m_state(U32_INVALID)
        {
        }

        f32 m_xMin;
        f32 m_yMin;
        u32 m_state;
    };
    typedef ITF_VECTOR<RenderSingleAnimData> RenderSingleAnimDataList;

    //---------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------

    class RenderBoxComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(RenderBoxComponent, GraphicComponent, RenderBoxComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        RenderBoxComponent();
        virtual ~RenderBoxComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const;
        virtual bbool       needsDraw2D() const;

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
		virtual void onStartDestroy(bbool _hotReload);
        virtual void        Update( f32 _dt );
        virtual void        batchPrimitives(const ITF_VECTOR <class View*>& _views);
        virtual void        batchPrimitives2D(const ITF_VECTOR <class View*>& _views);
        virtual void        onEvent( Event* _event);
        virtual void        onResourceReady();

        ITF_INLINE void     enableDraw(bbool _enable) { m_drawEnabled = _enable;}
        void                setSize(Vec2d _size);
        void                setOffset(const Vec2d & _offset);

        Vec2d               getSize() const;
        Vec2d               getOffset() const;

        void                computeAMV();
        void                computeAMVPositions();
        
        Vec3d               getGlobalPosFromAMVLocalPos(const Vec3d & _pos);
        bbool               getUnicRenderSingleAnimDataWithState(u32 _state, RenderSingleAnimData & _renderSingleAnimData);
        UsedDataAmv *       getUnicUsedDataWithState(u32 _state);
        void                setState(u32 _state);


        const Vec2d &       geUVtRatio() const { return m_uvRatio; }
        void                seUVtRatio(const Vec2d & val) { m_uvRatio = val; m_needRefreshUvMatrix = btrue; }

        const Vec2d &       geUVtPreTranslation() const { return m_uvPreTranslation; }
        void                seUVtPreTranslation(const Vec2d & val) { m_uvPreTranslation = val; m_needRefreshUvMatrix = btrue; }

        const Vec2d &       geUVtTranslation() const { return m_uvTranslation; }
        void                seUVtTranslation(const Vec2d & val) { m_uvTranslation = val; m_needRefreshUvMatrix = btrue; }

        Angle               geUVtRotation() const { return m_uvRotation; }
        void                seUVtRotation(Angle val) { m_uvRotation = val; m_needRefreshUvMatrix = btrue; }

        const Vec2d &       geUVtTranslationSpeed() const { return m_uvTranslationSpeed; }
        void                seUVtTranslationSpeed(const Vec2d & val) { m_uvTranslationSpeed = val; m_needRefreshUvMatrix = btrue; }

        Angle               geUVtRotationSpeed() const { return m_uvRotationSpeed; }
        void                seUVtRotationSpeed(Angle val) { m_uvRotationSpeed = val; m_needRefreshUvMatrix = btrue; }

        const Vec2d &       geUVtPivot() const { return m_uvPivot; }
        void                seUVtPivot(const Vec2d & val) { m_uvPivot = val; m_needRefreshUvMatrix = btrue; }

        bbool               getUseShadow() const {return m_useShadow;}
        const Vec3d &       getShadowOffset() const { return m_shadowOffset; }
        void                setShadowOffset(const Vec3d & val);

        f32                 getShadowAlpha() const { return m_shadowAlpha; }
        void                setShadowAlpha(f32 val);

        bbool               getUseHighlight() const {return m_useHighlight;}
        const Vec3d &       getHighlightOffset() const { return m_highlightOffset; }
        void                setHighlightOffset(const Vec3d & val);

        f32                 getHighlightAlpha() const { return m_highlightAlpha; }
        void                setHighlightAlpha(f32 val);

        const Vec2d &       getHighlightScale() const { return m_highlightScale; }
        void                setHighlightScale(const Vec2d & val);

        f32                 getDynamicAMVMinimunBorderSize() const;
		void                setColor(const Color & _color, f32 _time);

		void				setOverrideMaterial(GFX_MATERIAL _mat){m_overwrittenmaterial = _mat;}
		void				setMaterialIndex(u32 idx);

#ifdef ITF_SUPPORT_EDITOR
        virtual void        onPostPropertyChange();
#endif //ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class RenderBoxComponent_Template* getTemplate() const;
        Vec3d               getFinalShadowOffset();
        Vec3d               getFinalInvShadowOffset();

        void                computeBox();
        void                updateAABB();
        void                updateAnimMaterial(bbool _forceFull = bfalse);
        void                updateMeshInfo(bbool _is2D);
        void                processTile(Vec2d * uvTab, u32 * indexTab, u32 _x, u32 _y, f32 midWidth, f32 midHeight, bbool _onWidth, bbool _isRotate);
        void                processRotate(Vec2d * pt, i32 _rotate);
        void                computeShadowMesh();
        void                computeHighlightMesh();

        ITF_VECTOR<UsedDataAmv>     m_usedAmvList;

        bbool               m_drawEnabled;
        bbool               m_needRefreshUvMatrix;
        ITF_Mesh            m_boxMesh;
        ITF_Mesh            m_shadowMesh;
        ITF_Mesh            m_highlightMesh;
        Vec2d               m_trueSize;

		bbool				m_autoSize;
		Vec2d				m_autoSizeScale;

		Vec2d				m_originalTextureSize;
        Vec2d               m_size;
        Vec2d               m_offset;

        Vec2d               m_uvRatio;
        Vec2d               m_uvPreTranslation;
        Vec2d               m_uvTranslation;
        Angle               m_uvRotation;
        Vec2d               m_uvTranslationSpeed;
        Angle               m_uvRotationSpeed;
        Vec2d               m_uvPivot;

        ITF_IndexBuffer *   m_indexBuffer;
        ITF_IndexBuffer *   m_indexBuffer2;
        bbool               m_useFrontTexture;

        Vec3d               m_shadowOffset;
        f32                 m_shadowAlpha;
        bbool               m_useShadow;

        Vec3d               m_highlightOffset;
        f32                 m_highlightAlpha;
        Vec2d               m_highlightScale;
        bbool               m_useHighlight;

        u32                 m_state;

        Color               m_colorSrc;
        Color               m_colorDst;
        f32                 m_colorTimeLeft;
        f32                 m_colorTimeTotal;

		class AnimMeshVertexComponent * m_amvComponent;
		GFXMaterialSerializable			m_overwrittenmaterial_serialized;
		GFX_MATERIAL					m_overwrittenmaterial;

		u32								m_currentFrontIndex;

		const GFX_MATERIAL &			getSerializedOverrideMaterial() const { return m_overwrittenmaterial_serialized; }
    };


    //---------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------


    class RenderBoxComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(RenderBoxComponent_Template, GraphicComponent_Template, RenderBoxComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(RenderBoxComponent)
        DECLARE_SERIALIZE()

    public:

        RenderBoxComponent_Template();
        virtual ~RenderBoxComponent_Template();

        bbool onTemplateLoaded( bbool _hotReload );
        void  onTemplateDelete(bbool _hotReload);

		const GFX_MATERIAL &        getMaterial() const {return m_material;}
		const GFX_MATERIAL &        getMaterial(u32 idx) const;
		const GFX_MATERIAL &        getAdditionalMaterial(u32 idx) const { return m_additionalMaterials[idx]; }
		u32							getAdditionalMaterialSize() const {return m_additionalMaterials.size();}
        const GFX_MATERIAL &        getFrontMaterial() const { return m_frontMaterial; }
        bbool                       is2D() const { return m_is2D; }
        bbool                       getUseAnimMeshVertex() const { return m_useAnimMeshVertex; }

        f32                         getLeftWidth() const { return m_leftWidth; }
        f32                         getTopHeight() const { return m_topHeight; }
        f32                         getRightWidth() const { return m_rightWidth; }
        f32                         getBottomHeight() const { return m_bottomHeight; }

        u32                         getBackTopLeftCornerAtlasIndex() const { return m_backTopLeftCornerAtlasIndex; }
        u32                         getBackTopMiddleCornerAtlasIndex() const { return m_backTopMiddleCornerAtlasIndex; }
        u32                         getBackTopRightCornerAtlasIndex() const { return m_backTopRightCornerAtlasIndex; }
        u32                         getBackMiddleLeftCornerAtlasIndex() const { return m_backMiddleLeftCornerAtlasIndex; }
        u32                         getBackMiddleMiddleCornerAtlasIndex() const { return m_backMiddleMiddleCornerAtlasIndex; }
        u32                         getBackMiddleRightCornerAtlasIndex() const { return m_backMiddleRightCornerAtlasIndex; }
        u32                         getBackBottomLeftCornerAtlasIndex() const { return m_backBottomLeftCornerAtlasIndex; }
        u32                         getBackBottomMiddleCornerAtlasIndex() const { return m_backBottomMiddleCornerAtlasIndex; }
        u32                         getBackBottomRightCornerAtlasIndex() const { return m_backBottomRightCornerAtlasIndex; }

        i32                         getBackTopLeftCornerAtlasRotate() const { return getRotate(m_backTopLeftCornerAtlasRotate); }
        i32                         getBackTopMiddleCornerAtlasRotate() const { return getRotate(m_backTopMiddleCornerAtlasRotate); }
        i32                         getBackTopRightCornerAtlasRotate() const { return getRotate(m_backTopRightCornerAtlasRotate); }
        i32                         getBackMiddleLeftCornerAtlasRotate() const { return getRotate(m_backMiddleLeftCornerAtlasRotate); }
        i32                         getBackMiddleMiddleCornerAtlasRotate() const { return getRotate(m_backMiddleMiddleCornerAtlasRotate); }
        i32                         getBackMiddleRightCornerAtlasRotate() const { return getRotate(m_backMiddleRightCornerAtlasRotate); }
        i32                         getBackBottomLeftCornerAtlasRotate() const { return getRotate(m_backBottomLeftCornerAtlasRotate); }
        i32                         getBackBottomMiddleCornerAtlasRotate() const { return getRotate(m_backBottomMiddleCornerAtlasRotate); }
        i32                         getBackBottomRightCornerAtlasRotate() const { return getRotate(m_backBottomRightCornerAtlasRotate); }

        bbool                       getBackTopMiddleCornerAtlasTile() const { return m_backTopMiddleCornerAtlasTile; }
        bbool                       getBackMiddleLeftCornerAtlasTile() const { return m_backMiddleLeftCornerAtlasTile; }
        bbool                       getBackMiddleRightCornerAtlasTile() const { return m_backMiddleRightCornerAtlasTile; }
        bbool                       getBackBottomMiddleCornerAtlasTile() const { return m_backBottomMiddleCornerAtlasTile; }

        f32                         getXUVToDistance() const { return m_xUVToDistance; }
        f32                         getYUVToDistance() const { return m_yUVToDistance; }
        f32                         getZOffset() const { return m_zOffset; }
        f32                         getDynamicAMVMinimunBorderSize() const { return m_dynamicAMVMinimunBorderSize; }

        bbool                       addBordersOnSetSize() const { return m_addBorders; }
        
        const RenderSingleAnimDataList &getAMVList()  const { return m_amvList; }

        const Vec3d &               getShadowOffset() const { return m_shadowOffset; }
        f32                         getShadowAlpha() const { return m_shadowAlpha; }

        const Vec3d &               getHighlightOffset() const { return m_highlightOffset; }
        f32                         getHighlightAlpha() const { return m_highlightAlpha; }
        const Vec2d &               getHighlightScale() const { return m_highlightScale; }
        const GFX_MATERIAL &        getHighlightMaterial() const { return m_highlightMaterial; }

        bbool                       isPosShadowRelative() const { return m_posShadowRelative; }
        bbool                       shadowUseFrontTexture() const { return m_shadowUseFrontTexture; }

    private:
        RenderSingleAnimDataList    m_amvList;     

		GFXMaterialSerializable     m_material;
		ITF_VECTOR<GFXMaterialSerializable>     m_additionalMaterials;
        GFXMaterialSerializable     m_frontMaterial;
        i32                         getRotate(i32 _rotate) const;

        bbool                       m_is2D;
        bbool                       m_addBorders;
        bbool                       m_useAnimMeshVertex;

        f32                         m_leftWidth;
        f32                         m_rightWidth;
        f32                         m_topHeight;
        f32                         m_bottomHeight;

        u32                         m_backTopLeftCornerAtlasIndex;
        u32                         m_backTopMiddleCornerAtlasIndex;
        u32                         m_backTopRightCornerAtlasIndex;
        u32                         m_backMiddleLeftCornerAtlasIndex;
        u32                         m_backMiddleMiddleCornerAtlasIndex;
        u32                         m_backMiddleRightCornerAtlasIndex;
        u32                         m_backBottomLeftCornerAtlasIndex;
        u32                         m_backBottomMiddleCornerAtlasIndex;
        u32                         m_backBottomRightCornerAtlasIndex;

        i32                         m_backTopLeftCornerAtlasRotate;
        i32                         m_backTopMiddleCornerAtlasRotate;
        i32                         m_backTopRightCornerAtlasRotate;
        i32                         m_backMiddleLeftCornerAtlasRotate;
        i32                         m_backMiddleMiddleCornerAtlasRotate;
        i32                         m_backMiddleRightCornerAtlasRotate;
        i32                         m_backBottomLeftCornerAtlasRotate;
        i32                         m_backBottomMiddleCornerAtlasRotate;
        i32                         m_backBottomRightCornerAtlasRotate;

        bbool                       m_backTopMiddleCornerAtlasTile;
        bbool                       m_backMiddleLeftCornerAtlasTile;
        bbool                       m_backMiddleRightCornerAtlasTile;
        bbool                       m_backBottomMiddleCornerAtlasTile;

        f32                         m_xUVToDistance;
        f32                         m_yUVToDistance;
        f32                         m_zOffset;
        f32                         m_dynamicAMVMinimunBorderSize;

        Vec3d                       m_shadowOffset;
        f32                         m_shadowAlpha;
        bbool                       m_shadowUseFrontTexture;
        bbool                       m_posShadowRelative;


        Vec3d                       m_highlightOffset;
        f32                         m_highlightAlpha;
        Vec2d                       m_highlightScale;
        GFXMaterialSerializable     m_highlightMaterial;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const RenderBoxComponent_Template* RenderBoxComponent::getTemplate() const
    {
        return static_cast<const RenderBoxComponent_Template*>(m_template);
    }

    ITF_INLINE f32 RenderBoxComponent::getDynamicAMVMinimunBorderSize() const
    { 
        return getTemplate()->getDynamicAMVMinimunBorderSize(); 
    }

}

#endif // _ITF_RENDERBOXCOMPONENT_H_
