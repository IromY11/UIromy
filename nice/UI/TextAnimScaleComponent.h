#ifndef _ITF_TEXTANIMSCALECOMPONENT_H_
#define _ITF_TEXTANIMSCALECOMPONENT_H_

#define TextAnimScaleComponent_CRC ITF_GET_STRINGID_CRC(TextAnimScaleComponent, 2846382420)
#define TextAnimScaleComponent_Template_CRC ITF_GET_STRINGID_CRC(TextAnimScaleComponent_Template, 4259388681)

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

namespace ITF
{
    class FontTemplate;

    class TextAnimScaleComponent : public ActorComponent, IUpdateAnimInputCriteria
    {

#define SERIALIZE_ENUM_SCALETYPE(name, member) \
    SERIALIZE_ENUM_BEGIN(name, member); \
    SERIALIZE_ENUM_VAR(ScaleType_None); \
    SERIALIZE_ENUM_VAR(ScaleType_Horizontale); \
    SERIALIZE_ENUM_VAR(ScaleType_Verticale); \
    SERIALIZE_ENUM_VAR(ScaleType_All); \
    SERIALIZE_ENUM_END();

        DECLARE_OBJECT_CHILD_RTTI(TextAnimScaleComponent, ActorComponent, TextAnimScaleComponent_CRC);

    public:

        enum ScaleType
        {
            ScaleType_None,
            ScaleType_Horizontale = (1 << 0),
            ScaleType_Verticale = (1 << 1),
            ScaleType_All = (ScaleType_Horizontale | ScaleType_Verticale),
            ENUM_FORCE_SIZE_32(ScaleType)
        };

        DECLARE_SERIALIZE()   
        DECLARE_VALIDATE_COMPONENT()

        TextAnimScaleComponent();
        ~TextAnimScaleComponent();
        
        virtual     bbool       needsUpdate         (       ) const             { return btrue; }
        virtual     bbool       needsDraw           (       ) const             { return bfalse; }
        virtual     bbool       needsDraw2D         (       ) const             { return bfalse; }
		virtual		bbool		needsDraw2DNoScreenRatio(	) const				{ return bfalse; }

		virtual void onStartDestroy(bbool _hotReload);
        virtual     void        onActorLoaded       ( Pickable::HotReloadType /*_hotReload*/ );
        virtual     void        onFinalizeLoad      ();
        virtual		void		onBecomeActive	    ();
        virtual		void		onBecomeInactive	();
        virtual     void        Update              ( f32 _deltaTime );

        virtual     void        updateAnimInput     ();

                    void        setScaleType        (ScaleType _scaleType);
                    void        setIsActive         (bbool _value) {m_isActive = _value;}
                    void        setIsLocked         (bbool _value) {m_isLocked = _value;}
                    bbool       getIsReady() const {return m_isReady;}

        const f32               getMarginTop() const { return m_margingTop; }
        const f32               getMarginBottom() const { return m_margingBottom; }
        const f32               getMarginLeft() const { return m_margingLeft; }
        const f32               getMarginRight() const { return m_margingRight; }
        void                    setMarginTop( f32 _top) { if(m_margingTop != _top) {m_margingTop = _top; m_needUpdate = btrue;}}
        void                    setMarginLeft( f32 _left ) { if(m_margingLeft != _left) {m_margingLeft = _left; m_needUpdate = btrue;}}
        void                    setMarginBottom( f32 _bottom ) { if(m_margingBottom != _bottom) {m_margingBottom = _bottom; m_needUpdate = btrue;}}
        void                    setMarginRight( f32 _right) { if(m_margingRight != _right) {m_margingRight = _right;  m_needUpdate = btrue;}}


        void                    needUpdate() { m_needUpdate = btrue;}

        const AABB              getAABB() const { return m_aabb; }

#ifdef ITF_SUPPORT_EDITOR
        virtual	void		    onPostPropertyChange	();
        const   AABB            getWantedAABB() const { return m_wantedAABB; }
        void                    setWantedAABB( const AABB &_aabb );                       
#endif

    protected:

        ITF_INLINE const class TextAnimScaleComponent_Template*  getTemplate() const;

        bbool                   getNeedUpdate(bbool& _needUpdate, bbool& _isTextUpdating);
        AABB                    computeWantedAABB   ();
        void                    applyWantedAABB(const AABB& _wantedAABB);
		void					fillContainersDefault();
        Vec2d                   getIntersection(const Vec2d& _lineStart, const Vec2d& _lineStop, const Vec2d& _topLeft, const Vec2d& _bottomRight);
        void                    setIsReady(bbool _isReady);
        void                    repositionActor(Actor *_actor, const Vec2d &_pivotOffset, const Vec2d &_sizeOffset) const;

        ScaleType m_scaleType;
        bbool m_isActive;
        Vec2d m_minimumSize;
        f32 m_margingTop;
        f32 m_margingLeft;
        f32 m_margingRight;
        f32 m_margingBottom;
        
        Vec2d m_wantedCursor;
        class AnimLightComponent* m_animLightComponent;
        class AnimatedComponent* m_animatedComponent;
        class RenderBoxComponent* m_renderBoxComponent;

        f32 m_updateDate;
        bbool m_isLocked;
        bbool m_needUpdate;
        bbool m_forceUpdate;
        bbool m_isReady;

		bbool m_autoFillContained;
		ITF_VECTOR<ObjectRef> m_containedActors;
		ITF_VECTOR<ObjectPath> m_containedObjectPaths;

		bbool m_autoFillRepositioned;
		ITF_VECTOR<ObjectRef> m_repositionActors;
		ITF_VECTOR<ObjectPath> m_repositionedObjectPaths;
        ITF_VECTOR<ObjectRef> m_relRepositionActors;
        ITF_VECTOR<ObjectPath> m_relRepositionedObjectPaths;
		
		Vec2d m_wantedSize;
		Vec2d m_wantedOffset;
        AABB m_aabb;
#ifdef ITF_SUPPORT_EDITOR
        AABB m_wantedAABB;
#endif
    };

    //---------------------------------------------------------------------------------------------------

    class TextAnimScaleComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TextAnimScaleComponent_Template,ActorComponent_Template,TextAnimScaleComponent_Template_CRC);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(TextAnimScaleComponent);

    public:

        TextAnimScaleComponent_Template();
        ~TextAnimScaleComponent_Template() {}

        virtual bbool onTemplateLoaded(bbool _hotReload) {m_baseAnchor = m_baseSize*0.5f; return Super::onTemplateLoaded(_hotReload);}

        Vec2d m_baseAnchor;
        Vec2d m_baseSize;
        Vec2d m_baseSizeMax;
        Vec2d m_margeSize;
        StringID m_animInputX;
        StringID m_animInputY;
    };


    ITF_INLINE const TextAnimScaleComponent_Template*  TextAnimScaleComponent::getTemplate() const {return static_cast<const TextAnimScaleComponent_Template*>(m_template);}
}
#endif // _ITF_TEXTANIMSCALECOMPONENT_H_
