#include "precompiled_engine.h"

#ifndef _ITF_METAFRIEZE_H_
#include "engine/display/MetaFrieze/MetaFrieze.h"
#endif //_ITF_METAFRIEZE_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_PREFETCH_FCT_H_
#include "engine/boot/PrefetchFct.h"
#endif //_ITF_PREFETCH_FCT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef SERIALIZERTOOLXML_H
#include "core/serializer/ZSerializerToolXML.h"
#endif // SERIALIZERTOOLXML_H

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifdef ITF_SUPPORT_EDITOR
    #ifndef _ITF_PLUGINGATEWAY_H_
    #include "PluginGateway/PluginGateway.h"
    #endif //_ITF_PLUGINGATEWAY_H_
#endif // ITF_SUPPORT_EDITOR

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(MetaFrieze)

    BEGIN_SERIALIZATION_CHILD(MetaFrieze)
        BEGIN_CONDITION_BLOCK_NOT(ESerialize_Instance)  
    BEGIN_CONDITION_BLOCK(ESerializeGroup_Data|ESerialize_Editor)            
    SERIALIZE_OBJECT("PointsList", m_pointsList);
    END_CONDITION_BLOCK()

            BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
                SERIALIZE_CONTAINER_OBJECT("FriezePath", m_friezePathList);
                SERIALIZE_CONTAINER_OBJECT("PrimitiveParameters", m_friezePrimitiveParameters);
                BEGIN_CONDITION_BLOCK_NOT(ESerialize_Editor)                      
                    SERIALIZE_MEMBER("ConfigCRC", m_configCRC);
                END_CONDITION_BLOCK_NOT()                
            END_CONDITION_BLOCK()
        END_CONDITION_BLOCK_NOT()

        SERIALIZE_MEMBER("ConfigName", m_templatePath);
        SERIALIZE_MEMBER("SwitchExtremityStart", m_switchExtremityStart);
        SERIALIZE_MEMBER("SwitchExtremityStop", m_switchExtremityStop);
        SERIALIZE_MEMBER("SwitchTexturePipeExtremity", m_switchTexturePipeExtremity);
        SERIALIZE_MEMBER("IsFriendlyNameValid", m_isFriendlyNameValid);
    END_SERIALIZATION()
      
    BEGIN_SERIALIZATION(MetaFriezeParams)
        SERIALIZE_MEMBER("path", m_path);
        SERIALIZE_MEMBER("worldOffset", m_worldOffset);        
        SERIALIZE_MEMBER("localOffset", m_localOffset);
        SERIALIZE_MEMBER("scale", m_scale);
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(MetaFriezeConfig)
    BEGIN_SERIALIZATION_CHILD(MetaFriezeConfig)
        SERIALIZE_CONTAINER_OBJECT("params",m_paramList);
    END_SERIALIZATION()

    bool PrefetchMcg( const Path& path, PrefetchFactory::Params& params )
    {
        return btrue;
    }

    MetaFrieze::MetaFrieze()
        : Pickable()        
        , m_configCRC(0)
        , m_switchExtremityStart(bfalse)
        , m_switchExtremityStop(bfalse)
        , m_isFriendlyNameValid(bfalse)
        , m_switchTexturePipeExtremity(U32_INVALID)
    {
#ifdef ITF_SUPPORT_EDITOR
        m_isCenterPivot = btrue;
        m_processUpdateTransformInProgress = bfalse;
#endif // ITF_SUPPORT_EDITOR

        setObjectType(BaseObject::eMetaFrieze);
    }     

    MetaFrieze::~MetaFrieze()
    {
        
    }

    u32 MetaFrieze::getConfigCRC()
    {        
        u32 crc = 0;

#ifdef ITF_SUPPORT_XML
        ITF_ASSERT(getMetaConfig());
        if ( getMetaConfig())
        {
            MetaFriezeConfig config = *getMetaConfig();

            SerializerToolXML serializer;
            uSize size = 0;
            char* buffer = NULL;

            serializer.OpenToWrite(&buffer,&size);
            config.Serialize(&serializer,ESerialize_Data_Save);
            serializer.Close();

            crc = CRC::compute32((unsigned char *) buffer,size);

            if (size)
            {
                SF_DEL_ARRAY(buffer);
            }
        }
#endif

        return crc;
    }

    void MetaFrieze::requestTemplateType()
    {
        m_templatePickable = (MetaFriezeConfig*)requestTemplateType(m_templatePath);
    }

    const TemplatePickable* MetaFrieze::requestTemplateType(const Path& _path) const
    {
        return TEMPLATEDATABASE->requestTemplate<MetaFriezeConfig>(_path, getRef());
    }

    void MetaFrieze::onLoaded(HotReloadType _hotReload)
    {
        Super::onLoaded(_hotReload);

        setUpdateGroup(WorldUpdate::UpdateGroup_Environment);
    }

    void MetaFrieze::onFinalizeLoad( bbool _hotReload )
    {
        Super::onFinalizeLoad( _hotReload );

#ifdef ITF_SUPPORT_EDITOR

        updateSwitchTexturePipeExtremity();

        if ( _hotReload )
        {
            updateConfigCRC();
            spawnFriezeList();
        }
        else
        {
            if ( m_configCRC != 0 && (m_configCRC != getConfigCRC() || !m_isFriendlyNameValid) ) // If it's not the default value and the config changed
            {                
                updateConfigCRC();
                deleteFriezeList(bfalse);
                spawnFriezeList();
                setDirty(btrue);
            }
        }

        if (!m_isFriendlyNameValid)
        {
            setFriendlyName(getScene());
        }

        // m_configCRC == 0 means metafrieze has been copied,
        // in this case, friezePathList isn't empty, so do not affect these friezes contained by another metafrieze
        if ( m_configCRC != 0 )
        {
            setPointList();
            setPrimitiveParamsFriezesList();
            setOwnerFriezesList();
        }

        updateAABB();
#endif // ITF_SUPPORT_EDITOR        
    }

    void MetaFrieze::onDestroy( bbool _hotReload )
    {
#ifdef ITF_SUPPORT_EDITOR
        if(_hotReload)
        {
            deleteFriezeList(_hotReload);
        }
#endif // ITF_SUPPORT_EDITOR    

        Super::onDestroy(_hotReload);
    }
 
#ifdef ITF_SUPPORT_EDITOR
    void MetaFrieze::getFriezeList( SafeArray<Frise*>& _friezesList ) const
    {
        const u32 friezeCount = m_friezePathList.size();

        for ( u32 i = 0; i < friezeCount; i++ )
        {
            if ( Frise* pFrieze = getFriezeAt(i) )
            {
                _friezesList.push_back(pFrieze);
            }
        }
    }

    Vec2d MetaFrieze::getPosOffsetAt( const Transform2d& _xf, const MetaFriezeParams& _param ) const
    {
        return _xf.transformVector( _param.m_worldOffset.truncateTo2D());
    }

    void MetaFrieze::updateAABB()
    {
        AABB aabb(get2DPos());

        const u32 posCount = m_pointsList.getPosCount();
        for (u32 i = 0; i<posCount ; i++)
        {
            Vec2d pos( m_pointsList.getPosAt(i) );
            transformLocalPosToWorld( pos );

            aabb.grow( pos );
        }

        const u32 friezeCount = m_friezePathList.size();
        for ( u32 i=0; i<friezeCount; i++)
        {
            if ( Frise* pFrieze = getFriezeAt(i) )
                aabb.grow(pFrieze->getAABB());
        }

        setAABB(aabb);
    }

    void MetaFrieze::processUpdateTransform()
    {
        const MetaFriezeConfig* pConfig = getMetaConfig();

        if ( !pConfig )
            return;        

        if ( !isOwnerFriezesList() )
            return;

        m_processUpdateTransformInProgress = btrue;

        const Transform2d xf( get2DPos(), getAngle(), getScale(), getIsFlipped() );
        const u32 friezeCount = m_friezePathList.size();

        ITF_ASSERT_CRASH( friezeCount <= pConfig->m_paramList.size(), "Mismatch meta config" );

        for ( u32 i=0; i<friezeCount; i++)
        {
            Frise* pFrieze = getFriezeAt(i);
            if ( !pFrieze )
                continue;

            const MetaFriezeParams& param = pConfig->m_paramList[i];

            processTransform( param, xf, pFrieze );           

            pFrieze->processUpdateTransform();
        }

        updateAABB();

        m_processUpdateTransformInProgress = bfalse;
    }

    void MetaFrieze::processTransform( const MetaFriezeParams& _param, const Transform2d& _xf, Frise* _pFrieze )
    {
        // set 2D pos
        _pFrieze->set2DPos( get2DPos() + getPosOffsetAt( _xf, _param) );         

        // set depth : getWorldInitialZ() gives the real depth in subscene
        const f32 depth = _pFrieze->isDepthForced() ? _pFrieze->getWorldInitialZ() : getDepth() + _param.m_worldOffset.z();
        _pFrieze->setDepth( depth );

        // set scale
        if ( !_pFrieze->isScaleForced() )
        {
            ITF_ASSERT_MSG( _param.m_scale > 0.f, "Invalid scale in meta frieze config: %s", getTemplatePath().toString8().cStr());
            if ( _param.m_scale > 0.f )
            {
                _pFrieze->setScale( getScale() * _param.m_scale );                
            }
        }
        else
        {
            _pFrieze->setScale( _pFrieze->getWorldInitialScale() ); // to get the real scale in subscene
        }

        // set angle
        _pFrieze->setAngle( getAngle() );

        // set flip
        _pFrieze->setIsFlipped( getIsFlipped() );
    }

    f32 MetaFrieze::getEdgeHeight( const MetaFriezeParams& _param, const PolyLineEdge& _edge )
    {
        return _param.m_localOffset * _edge.getScale();         
    }

    void MetaFrieze::setEdgePosFromVisualOffset( const MetaFriezeParams& _param, const Vec2d& _normalizedVector, PolyPointList* _pPointList, u32 _idPoint )
    {
        const PolyLineEdge& edge = _pPointList->getEdgeAt( _idPoint );

        _pPointList->setPosAt( edge.getPos() + _normalizedVector.getPerpendicular() * getEdgeHeight( _param, edge ), _idPoint );
    }

    void MetaFrieze::setEdgePosFromVisualOffset( const MetaFriezeParams& _param, PolyPointList* _pPointList, u32 _previousIdPoint, u32 _currentIdPoint )
    {
        const PolyLineEdge& previousEdge   = _pPointList->getEdgeAt( _previousIdPoint ); 
        const PolyLineEdge& currentEdge    = _pPointList->getEdgeAt( _currentIdPoint );

        // edges snapped
        if ( !previousEdge.m_normalizedVector.isNormalized() || !previousEdge.m_normalizedVector.isNormalized() )
            return;

        const f32 dotSight = previousEdge.m_normalizedVector.dot( currentEdge.m_normalizedVector );
        if ( dotSight - MTH_EPSILON <= -1.f) // edges angle = 360 degrees
            return;

        if ( dotSight + MTH_EPSILON >= 1.f ) // edges angle = 0 degree
        {
            setEdgePosFromVisualOffset( _param, currentEdge.m_normalizedVector, _pPointList, _currentIdPoint );
            return;
        }

        const Vec2d cornerNormal = previousEdge.m_normalizedVector.getPerpendicular() + currentEdge.m_normalizedVector.getPerpendicular();
        const Vec2d posOffset =  currentEdge.getPos() +currentEdge.m_normalizedVector.getPerpendicular() * getEdgeHeight( _param, currentEdge );

        Vec2d interPos;
        intersectionLineLine( currentEdge.getPos(), currentEdge.getPos() +cornerNormal, 
            posOffset, posOffset + currentEdge.m_normalizedVector, interPos );

        _pPointList->setPosAt( interPos, _currentIdPoint );
    }

    void MetaFrieze::applyVisualOffset( const MetaFriezeParams& _param, Frise* _pFrieze )
    {
        if ( _param.m_localOffset == 0.f )
            return;

        PolyPointList* pPointList = _pFrieze->getPolyPointList();
        pPointList->checkLoop();
        pPointList->executeRecomputeData();     

        const u32 posCount = pPointList->getPosCount();
        if ( posCount <= 1 )
            return;             

        // first and last points
        if ( pPointList->isLooping() )
        {
            setEdgePosFromVisualOffset( _param, pPointList, posCount -1, 0 );
        }
        else
        {
            setEdgePosFromVisualOffset( _param, pPointList->getEdgeAt( 0 ).m_normalizedVector, pPointList, 0 );
            setEdgePosFromVisualOffset( _param, pPointList->getEdgeAt( posCount -2 ).m_normalizedVector, pPointList, posCount -1 );         
        }

        for ( u32 i = 1; i < posCount -1; i++ )
        {
            setEdgePosFromVisualOffset( _param, pPointList, i -1, i );
        }        
    }

    void MetaFrieze::recomputeData()
    {            
        m_pointsList.checkLoop();
        m_pointsList.executeRecomputeData();

        const MetaFriezeConfig* pConfig = getMetaConfig();

        ITF_ASSERT(pConfig);
        if ( !pConfig )
            return;        

        const Transform2d xf( get2DPos(), getAngle(), getScale(), getIsFlipped() );
        const u32 friezeCount = m_friezePathList.size();

        ITF_ASSERT_CRASH( friezeCount <= pConfig->m_paramList.size(), "Mismatch meta config" );

        for ( u32 i=0; i<friezeCount; i++)
        {
            Frise* pFrieze = getFriezeAt(i);
            if ( !pFrieze )
                continue;

            const MetaFriezeParams& param = pConfig->m_paramList[i];

            // set point list
            PolyPointList* pFriezePointList = pFrieze->getPolyPointList();
            *pFriezePointList = m_pointsList; 

            // set loop
            if ( pFrieze->isLooping() && !pFrieze->hasLoopingMethode() )
                pFrieze->setLoop( bfalse);       

            // set switch extremity
            pFrieze->onEditorSetExtremityStart( m_switchExtremityStart );
            pFrieze->onEditorSetExtremityStop( m_switchExtremityStop );

            // set switch texture pipe extremity
            pFrieze->setSwitchTexturePipeExtremity(m_switchTexturePipeExtremity);

            applyVisualOffset( param, pFrieze );

            processTransform( param, xf, pFrieze );

            pFrieze->onEditorRecomputeData();
        }

        updateAABB();
    }

    Frise* MetaFrieze::getFriezeAt( u32 _index ) const
    {
        return static_cast<Frise*>( SceneObjectPathUtils::getObjectFromRelativePath( this, m_friezePathList[_index]) );
    }

    bbool MetaFrieze::hasLoopingMethode() const
    {        
        const u32 friezeCount = m_friezePathList.size();
        for ( u32 i=0; i<friezeCount; i++)
        {
            const Frise* pFrieze = getFriezeAt(i);
            if ( pFrieze && pFrieze->hasLoopingMethode() )
                return btrue;
        }

        return bfalse;
    }

    bbool MetaFrieze::hasSwitchExtremityMethode() const
    {        
        const u32 friezeCount = m_friezePathList.size();
        for ( u32 i=0; i<friezeCount; i++)
        {
            const Frise* pFrieze = getFriezeAt(i);
            if ( pFrieze && pFrieze->hasSwitchExtremityMethode() )
                return btrue;
        }

        return bfalse;
    }

    void MetaFrieze::onEditorCreated( Pickable* _original )
    {
        Super::onEditorCreated( _original );

        updateConfigCRC();
        spawnFriezeList();
    }

    void MetaFrieze::onEditorTranslatePivot( const Vec2d& _newPos )
    {
        Vec2d deltaPos( _newPos);
        transformWorldPosToLocal( deltaPos );

        if ( !deltaPos.IsEqual( Vec2d::Zero, 0.01f ))
        {
            const u32 posCount = m_pointsList.getPosCount();
            for ( u32 i=0; i<posCount; i++)
            {
                m_pointsList.setPosAt( m_pointsList.getPosAt(i) -deltaPos, i);
            }

            const Vec3d newPos = _newPos.to3d(getDepth());
            setWorldInitialPos(newPos, btrue);
            setPos(newPos);
        }
                
        const MetaFriezeConfig* pConfig = getMetaConfig();

        ITF_ASSERT(pConfig);
        if ( !pConfig )
            return;

        Transform2d xf( get2DPos(), getAngle(), getScale(), getIsFlipped() );

        // translate pivot to frieze list
        const u32 friezeCount = m_friezePathList.size();
        ITF_ASSERT_CRASH( friezeCount <= pConfig->m_paramList.size(), "Mismatch meta config" );

        for ( u32 i=0; i<friezeCount; i++)
        {
            const MetaFriezeParams& param = pConfig->m_paramList[i];

            if ( Frise* pFrieze = getFriezeAt(i) )
                pFrieze->onEditorTranslatePivot( _newPos + getPosOffsetAt( xf, param) );
        }
    }

    void MetaFrieze::onEditorCenterPivot()
    {        
        u32 posCount = m_pointsList.getPosCount();
        if ( m_pointsList.isLooping() && posCount >2 )
            posCount --;

        if ( posCount >0 )
        {
            Vec2d center(m_pointsList.getPosAt(0));
            for ( u32 i=1; i<posCount; i++)
            {
                center += m_pointsList.getPosAt(i);              
            }

            center /= (f32)posCount;
            transformLocalPosToWorld(center);

            onEditorTranslatePivot( center );
        }
    }

    //=============================================================================================
    //
    // point list transformations
    //
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    // invert order of points
    //
    void MetaFrieze::invertPoints(bbool _undo)
    {
        const ITF_VECTOR<PolyLineEdge> edgesListCopy     = m_pointsList.getMutableEdgeData();
        ITF_VECTOR<PolyLineEdge>& edgesList              = m_pointsList.getMutableEdgeData();

        u32 lastIndex = m_pointsList.getPosCount() -1;

        if ( m_pointsList.isLooping() )
        {
            lastIndex -= 1;
            for ( u32 index = 0; index < lastIndex; index++)
            {
                PolyLineEdge& edge = edgesList[index];   
                const PolyLineEdge& edgeCopy = edgesListCopy[lastIndex-index];
                edge = edgesListCopy[lastIndex-index-1]; // copy index -1 to get edge's datas
                edge.setPos( edgeCopy.getPos() );
                edge.setScale( edgeCopy.getScale() );
            }
            edgesList[lastIndex] = edgesListCopy[0];
            edgesList[lastIndex + 1] = edgesList[0];
            edgesList[0].setScale( edgesListCopy[0].getScale() );
        }
        else
        {
            for ( u32 index = 0; index < lastIndex; index++)
            {
                PolyLineEdge& edge = edgesList[index];   
                const PolyLineEdge& edgeCopy = edgesListCopy[lastIndex-index];
                edge = edgesListCopy[lastIndex-index-1]; // copy index -1 to get edge's datas
                edge.setPos( edgeCopy.getPos() );
                edge.setScale( edgeCopy.getScale() );
            }
            edgesList[lastIndex] = edgesListCopy[0];
        }

        recomputeData();
    }

    //---------------------------------------------------------------------------------------------
    // mirror points
    //
    void MetaFrieze::mirror( bbool _xAxis )
    {
        invertPoints(bfalse);
        const u32 posCount = m_pointsList.getPosCount();

        if (_xAxis)
        {
            for (u32 i = 0; i < posCount; i++)
            {
                Vec2d newPos( m_pointsList.getPosAt(i));
                newPos.x() = -newPos.x();
                m_pointsList.setPosAt( newPos, i);
            }    
        }
        else
        {
            for (u32 i = 0; i < posCount; i++)
            {
                Vec2d newPos( m_pointsList.getPosAt(i));
                newPos.y() = -newPos.y();
                m_pointsList.setPosAt( newPos, i);
            }    
        }

        recomputeData();
    }

    void MetaFrieze::resetTransformation( bbool _resetAngle, bbool _resetScale, bbool _resetFillingUV )
    {
        const u32 pointsCount = m_pointsList.getPosCount();

        // store world points
        Transform2d previousXf(get2DPos(), getAngle(), getScale(), getIsFlipped() );
        PolyPointList worldPointsList;
        for ( u32 i=0; i<pointsCount; i++)
            worldPointsList.addPoint( previousXf.transformPos( m_pointsList.getPosAt(i)) );

        Vec3d   parentPos;
        f32     parentAngle;
        bbool   parentFlip;
        Vec2d   parentScale;
        getParentWorldTransform( parentPos, parentAngle, parentFlip, parentScale);

        // scale
        Vec2d targetScale(Vec2d::One);
        if ( _resetScale )
        {
            targetScale = getScale();
            if ( parentScale.x() != 0.f ) targetScale.x() /= parentScale.x();
            if ( parentScale.y() != 0.f ) targetScale.y() /= parentScale.y();
            setLocalInitialScale( Vec2d::One );
        }

        // angle
        f32 targetAngle = 0.f;
        if ( _resetAngle )
        {
            targetAngle = getAngle() - parentAngle;
            setLocalInitialRot(0.f);
        }

        // flip
        setLocalInitialFlip( bfalse ); 

        // apply changes
        resetTransformationToInitial();

        // restore points
        Transform2d currentXf(get2DPos(), getAngle(), getScale(), getIsFlipped() );
        for ( u32 i =0; i<pointsCount; i++)
            m_pointsList.setPosAt( currentXf.inverseTransformPos( worldPointsList.getPosAt(i)), i);

        // restore fillings uvs
        if ( _resetFillingUV )
        {
            SafeArray<Frise*> friezesList;
            getFriezeList(friezesList);

            for (u32 i=0; i<friezesList.size(); ++i)
            {
                Frise* pFrieze = friezesList[i];
                pFrieze->onEditorSetFillUVOrigin(get2DPos());
            }
        }

        recomputeData();
    }

    void MetaFrieze::spawnFriezeList()
    {
        m_friezePathList.clear();

        const MetaFriezeConfig* pConfig = getMetaConfig();
        Scene* pScene = getScene();

        ITF_ASSERT(pConfig && pScene);
        if ( !pConfig || !pScene )
            return;

        const u32 paramCount = pConfig->m_paramList.size();
        for ( u32 i=0; i<paramCount; i++)
        {
            const MetaFriezeParams& param = pConfig->m_paramList[i];

            if ( param.m_path.isEmpty())
                continue;

            const String8 ext = param.m_path.getExtension();
            if ( ext != "fcg" && ext != "frz" )
                continue;

            Frise* pFrise = new Frise();

            if ( ext == "fcg")
            {
                pFrise->setTemplatePath(param.m_path);
            }
            else
            {
                pFrise->setInstanceDataFile(param.m_path);
                pFrise->loadInstanceData(bfalse);
            }
            
            pFrise->onCreate();    
            pFrise->setMetaFriezeOwner( getRef() );
            pFrise->generateUniqueName( pScene, param.m_path.getBasenameWithoutExtension() );
            pScene->registerPickable(pFrise);  

            ObjectPath objPath;
            SceneObjectPathUtils::getRelativePathFromObject( this, pFrise, objPath);

            m_friezePathList.push_back(objPath);                
        }
        
        setPointList();
        setPrimitiveParamsFriezesList();
        recomputeData();

        const u32 friezeCount = m_friezePathList.size();
        for ( u32 i=0; i<friezeCount; ++i)
        {
            Frise* pFrieze = getFriezeAt(i);
            if ( !pFrieze )
                continue;

            pFrieze->storeCurrentTransformationToInitial();
        }
    }

    void MetaFrieze::deleteFriezeList(bbool _hotReload)
    {
        Scene* scene = getScene(); 
        ITF_ASSERT(scene);
        if ( scene )
        {
            const u32 friezeCount = m_friezePathList.size();
            for ( u32 i=0; i<friezeCount; i++)
            {
                if ( Frise* frieze = getFriezeAt(i) )
                {
                    ITF_ASSERT(frieze->getScene() == scene);

                    scene->unregisterPickable( frieze );                
                }
            }

            m_friezePathList.clear();
            
            if(_hotReload)
            {
                scene->flushPending(bfalse, btrue);
            }
        }
    }

    void MetaFrieze::setHiddenForEditorFlag( bbool _hidden )
    {
        Super::setHiddenForEditorFlag( _hidden );

        const u32 friezeCount = m_friezePathList.size();
        for ( u32 i=0; i<friezeCount; i++)
        {
            if ( Frise* pFrieze = getFriezeAt(i) )
            {
                pFrieze->setHiddenForEditorFlag( _hidden );
            }
        }
    }

    void MetaFrieze::forceReload(HotReloadType _hotReload)
    {
        Super::forceReload( _hotReload );

        if ( _hotReload == Pickable::HotReloadType_DataChanged )
        {
            deleteFriezeList(_hotReload != HotReloadType_None);
        }
    }

    void MetaFrieze::updateDataError()
    {        
        if ( hasDataError() )        
        {
            resetDataError();
        }

        String8 str("Invalid friezes:");
        bbool hasError = bfalse;

        const u32 friezeCount = m_friezePathList.size();
        for ( u32 i=0; i<friezeCount; i++)
        {
            if ( Frise* pFrieze = getFriezeAt(i) )
            {
                if ( pFrieze->hasDataError() )
                {
                    if ( hasError )
                    {
                        str += ",";
                    }

                    str += " ";
                    str += pFrieze->getUserFriendly().cStr();
                    
                    hasError = btrue;
                }
            }
        }

        if ( hasError )
        {                       
            setDataError(str);
        }
    }

    //------------------------------------------------------------------    ---------------------------
    // update metafrieze point list parameters
    //
    void MetaFrieze::updatePointList()
    {    
        for (u32 i = 0; i < m_friezePathList.size(); i++)
        {
            Frise *frise = getFriezeAt(i);
            if (!frise) continue;

            PolyPointList *frisePointList = frise->getPolyPointList();

            if (frisePointList->getPosCount() != m_pointsList.getPosCount()) 
                continue;

            for (u32 j = 0; j < frisePointList->getPosCount(); j++)
            {
                const PolyLineEdge& edgeSrc = m_pointsList.getEdgeAt(j);
                const PolyLineEdge& edgeDst = frisePointList ->getEdgeAt(j);

                if (edgeDst.m_switchTexture != edgeSrc.m_switchTexture)
                    m_pointsList.switchTextureAt(j);
            }
            break;
        }
    }

    //---------------------------------------------------------------------------------------------
    // set frieze point list parameters from metafrieze point list parameters
    //
    void MetaFrieze::setPointList()
    {    
        for (u32 i = 0; i < m_friezePathList.size(); i++)
        {
            Frise *frise = getFriezeAt(i);
            if (!frise) continue;

            PolyPointList *frisePointList = frise->getPolyPointList();

            if (frisePointList->getPosCount() != m_pointsList.getPosCount()) 
                continue;

            for (u32 j = 0; j < frisePointList->getPosCount(); j++)
            {
                const PolyLineEdge& edgeSrc = m_pointsList.getEdgeAt(j);
                const PolyLineEdge& edgeDst = frisePointList ->getEdgeAt(j);

                if (edgeDst.m_switchTexture != edgeSrc.m_switchTexture)
                    frisePointList->switchTextureAt(j);
            }
        }
    }

    //---------------------------------------------------------------------------------------------
    // Set frieze primitive params from metafrieze data
    //
    void MetaFrieze::setPrimitiveParamsFriezesList()
    {    
        const u32 friezeCount = m_friezePathList.size();

        // if not enough frieze primitive parameters rebuild table
        while( m_friezePrimitiveParameters.size() < friezeCount )
        {
            Frise* frieze = getFriezeAt(m_friezePrimitiveParameters.size());
            if (!frieze)
                m_friezePrimitiveParameters.push_back( GFXPrimitiveParam::getDefault() );
            else
            {
                GFXPrimitiveParam primitiveParameters = frieze->getGfxPrimitiveParamSerialized();
                m_friezePrimitiveParameters.push_back( primitiveParameters );
            }
        }

        // affect frieze param
        for ( u32 i=0; i<friezeCount; i++)
        {
            Frise* frieze = getFriezeAt(i);
            if (!frieze) continue;
            frieze->setGFXPrimitiveParamSerialized( m_friezePrimitiveParameters[i]);
            frieze->resetGFXPrimitiveParamRunTime();
        }
    }

    void MetaFrieze::setOwnerFriezesList()
    {    
        const u32 friezeCount = m_friezePathList.size();
        for ( u32 i=0; i<friezeCount; i++)
        {
            if ( Frise* pFrieze = getFriezeAt(i) )
            {
                pFrieze->setMetaFriezeOwner( getRef() );
                pFrieze->setGFXPrimitiveParamSerialized( m_friezePrimitiveParameters[i]);
                pFrieze->resetGFXPrimitiveParamRunTime();
            }
        }
    }

    void MetaFrieze::setFriendlyName(Scene* pScene )
    {
        if ( !pScene )
            return;

        const Path path = !getInstanceDataFile().isEmpty() ? getInstanceDataFile() : getTemplatePath();
        generateUniqueName( pScene, path.getBasename() ); // keep extension

        m_isFriendlyNameValid = btrue;
    }

    bbool MetaFrieze::isOwnerFriezesList() const
    {        
        const u32 friezeCount = m_friezePathList.size();
        for ( u32 i=0; i<friezeCount; i++)
        {
            const Frise* pFrieze = getFriezeAt(i);
            if ( pFrieze )                
                return pFrieze->getMetaFriezeOwner() == getRef();                        
        }

        return btrue;
    }

    void MetaFrieze::updateSwitchTexturePipeExtremity(bbool _forceCompute /*= bfalse*/)
    {
        if (m_switchTexturePipeExtremity != U32_INVALID && !_forceCompute)
            return;

        const u32 friezeCount = m_friezePathList.size();

        for ( u32 i=0; i<friezeCount; i++)
        {
            Frise* pFrieze = getFriezeAt(i);
            if ( !pFrieze )
                continue;

            m_switchTexturePipeExtremity = pFrieze->getSwitchTexturePipeExtremity();

            return;
        }

        m_switchTexturePipeExtremity = 0;
    }
#endif // ITF_SUPPORT_EDITOR


    //////////////////////////////////////////////////////////////////////////
    // Config
    //////////////////////////////////////////////////////////////////////////
    MetaFriezeConfig::MetaFriezeConfig() 
    {
    }

    MetaFriezeConfig::~MetaFriezeConfig()
    {
    }
} // namespace ITF
