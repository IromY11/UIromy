#ifndef _ITF_METAFRIEZE_H_
#define _ITF_METAFRIEZE_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{
    class MetaFriezeParams
    {
        DECLARE_SERIALIZE()
        
        MetaFriezeParams()
            : m_worldOffset(Vec3d::Zero)
            , m_scale(1.f)
            , m_localOffset(0.f)
        {
        }

        Path    m_path;
        Vec3d   m_worldOffset;
        f32     m_scale;
        f32     m_localOffset;
    };

    #define  MetaFriezeConfig_CRC ITF_GET_STRINGID_CRC(MetaFriezeConfig,291660116)
    class MetaFriezeConfig : public TemplatePickable
    {
        DECLARE_OBJECT_CHILD_RTTI(MetaFriezeConfig, TemplatePickable,291660116)
        DECLARE_SERIALIZE()

    public:

        MetaFriezeConfig();
        ~MetaFriezeConfig();

        ITF_VECTOR<MetaFriezeParams> m_paramList;
    };

   
    #define  MetaFrieze_CRC ITF_GET_STRINGID_CRC(MetaFrieze,2732675635)
    class MetaFrieze : public Pickable
    {
    public:

        ITF_DECLARE_NEW_AND_DELETE_OPERATORS_CATEGORY(mId_MetaFrieze);
        DECLARE_OBJECT_CHILD_RTTI(MetaFrieze, Pickable,2732675635);
        DECLARE_SERIALIZE()

        MetaFrieze                          (   );
        ~MetaFrieze                         (   );

        virtual void                        onLoaded(HotReloadType _hotReload); 
        virtual void                        onFinalizeLoad( bbool _hotReload );        
        virtual void                        onDestroy( bbool _hotReload );        
      
#ifdef ITF_SUPPORT_EDITOR
        virtual void                        onFlipChanged       (   )                          { processUpdateTransform(); }
        virtual void                        onPosChanged        (   const Vec3d& _prevPos )    { processUpdateTransform(); }
        virtual void                        onDepthChanged      (   f32 _prevDepth )           { processUpdateTransform(); }
        virtual void                        onAngleChanged      (   f32 _prevAngle )           { processUpdateTransform(); }
        virtual void                        onScaleChanged      (   const Vec2d& _prevScale)   { processUpdateTransform(); }
        virtual void                        onAnglePosChanged(f32 _prevAngle, const Vec3d& _prevPos ) {processUpdateTransform();}

        virtual void                        onEditorCreated( Pickable* _original = NULL);
        void                                onEditorTranslatePivot( const Vec2d& _newPos );
        void                                onEditorCenterPivot();
        ITF_INLINE const bbool              OnEditorIsCenterPivot() const { return m_isCenterPivot; }
        const void                          onEditorSetCenterPivot( bbool _isCenterPivot ) { m_isCenterPivot = _isCenterPivot; }
        void                                deleteFriezeList(bbool _hotReload);
        bbool                               hasLoopingMethode() const;
        bbool                               hasSwitchExtremityMethode() const;         
        void                                recomputeData();
        void                                getFriezeList( SafeArray<Frise*>& _friezesList ) const;
        Vec2d                               getPosOffsetAt( const Transform2d& _xf, const MetaFriezeParams& _param ) const;

        bbool                               m_processUpdateTransformInProgress;
        void                                processUpdateTransform();          

        bbool                               isValid() const { return m_pointsList.isValid(); }
        const void                          onEditorSwitchExtremityStart() { m_switchExtremityStart = !m_switchExtremityStart; }
        const void                          onEditorSwitchExtremityStop() { m_switchExtremityStop = !m_switchExtremityStop; }
        virtual void                        setHiddenForEditorFlag(bbool _hidden);
        virtual void                        forceReload(HotReloadType _hotReload);
        void                                invertPoints(bbool _undo = btrue);
        void                                mirror(bbool _xAxis);
        void                                resetTransformation( bbool _resetAngle, bbool _resetScale, bbool _resetFillingUV );

        void                                updatePointList();
        void                                setPointList();
        void                                setPrimitiveParamsFriezesList();
        void                                setOwnerFriezesList();
        bbool                               isOwnerFriezesList() const;
        void                                updateDataError();
        void                                setFriendlyName(Scene* pScene );
        void                                updateSwitchTexturePipeExtremity(bbool _forceCompute = bfalse);
#endif // ITF_SUPPORT_EDITOR
                
        ITF_INLINE const MetaFriezeConfig*  getMetaConfig() const;
        PolyPointList*                      getPolyPointList() { return &m_pointsList; }
        const PolyPointList*                getPolyPointList() const { return &m_pointsList; }
        ITF_INLINE const PolyPointList&		getPointsData() const {return m_pointsList;}  

private:	        
        PolyPointList                       m_pointsList;                      
        void                                requestTemplateType();     
        virtual const TemplatePickable*     requestTemplateType(const Path& _path) const;                      
        u32                                 m_configCRC;
        u32                                 getConfigCRC();
        Frise*                              getFriezeAt(u32 _index) const;
        bbool                               m_switchExtremityStart;
        bbool                               m_switchExtremityStop;
        u32                                 m_switchTexturePipeExtremity;
        bbool                               m_isFriendlyNameValid;

#ifdef ITF_SUPPORT_EDITOR        
        bbool                               m_isCenterPivot;        
        f32                                 getEdgeHeight( const MetaFriezeParams& _param, const PolyLineEdge& _edge );
        void                                updateAABB();
        void                                processTransform( const MetaFriezeParams& _param, const Transform2d& _xf, Frise* _pFrieze );
        void                                applyVisualOffset( const MetaFriezeParams& _param, Frise* _pFrieze );        
        void                                setEdgePosFromVisualOffset( const MetaFriezeParams& _param, const Vec2d& _normalizedVector, PolyPointList* _pPointList, u32 _idPoint );        
        void                                setEdgePosFromVisualOffset( const MetaFriezeParams& _param, PolyPointList* _pPointList, u32 _previousIdPoint, u32 _currentIdPoint );
#endif // ITF_SUPPORT_EDITOR

        friend class MetaFriezeEditor;
        friend class ColorTweakerPlugin;

        ITF_VECTOR<ObjectPath>              m_friezePathList;
        ITF_VECTOR<GFXPrimitiveParam>       m_friezePrimitiveParameters;
        void                                spawnFriezeList();        
        void                                updateConfigCRC() { m_configCRC = getConfigCRC(); }        

    };

    const MetaFriezeConfig* MetaFrieze::getMetaConfig() const { return static_cast<const MetaFriezeConfig*>(Super::getTemplate()); }

} // namespace ITF

#endif  // _ITF_METAFRIEZE_H_
