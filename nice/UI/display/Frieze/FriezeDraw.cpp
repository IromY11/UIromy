#include "precompiled_engine.h"

#ifndef _ITF_FRIEZE_DRAW_H_
#include "engine/display/Frieze/FriezeDraw.h"
#endif //_ITF_FRIEZE_DRAW_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FLUIDSHAPER_H_
#include "engine/display/Frieze/FluidShaper.h"
#endif //_ITF_FLUIDSHAPER_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{
    BEGIN_SERIALIZATION(EdgeProcessData)
        SERIALIZE_MEMBER("Id", m_id);
        SERIALIZE_MEMBER("IndexStart", m_indexStart);
        SERIALIZE_MEMBER("IndexEnd", m_indexEnd);
        SERIALIZE_MEMBER("PosStart", m_posStart);
        SERIALIZE_MEMBER("PosEnd", m_posEnd);
        SERIALIZE_MEMBER("PosOffset", m_posOffset);
        SERIALIZE_MEMBER("Count", m_count);
        SERIALIZE_MEMBER("Normal", m_normal);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(EdgeData)
        SERIALIZE_MEMBER("Level", m_level);
        SERIALIZE_MEMBER("Pos", m_pos);
        SERIALIZE_MEMBER("Way", m_way);
        SERIALIZE_MEMBER("CutUV", m_cutUV);
        SERIALIZE_MEMBER("HeightScale", m_heightScale);
        SERIALIZE_MEMBER("HeightScaleTotal", m_heightScaleTotal);
        SERIALIZE_MEMBER("HeightPos", m_heightPos);
        SERIALIZE_MEMBER("PosZ", m_posZ);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(EdgeFluid)
        SERIALIZE_CONTAINER("PosList", m_pos);
        SERIALIZE_CONTAINER("UVList", m_uv);
        SERIALIZE_CONTAINER("Colors", m_colors);   
        SERIALIZE_MEMBER("UvAnimTrans", m_uvAnimTrans);
        SERIALIZE_MEMBER("MeshLevel", m_meshLevel);
        SERIALIZE_MEMBER("Depth", m_depth);
        SERIALIZE_MEMBER("LastIndex", m_lastIndex);
        SERIALIZE_MEMBER("LocalNorm", m_localNorm);
        SERIALIZE_MEMBER("WorldNorm", m_worldNorm);
        SERIALIZE_MEMBER("WorldHeight", m_worldHeight);
        SERIALIZE_MEMBER("WorldAngle", m_worldAngle);
        SERIALIZE_MEMBER("Delta", m_delta);
        SERIALIZE_MEMBER("DeltaUV", m_deltaUV);
        SERIALIZE_CONTAINER_OBJECT("CollisionProcesses", m_collisionProcesses);
        SERIALIZE_OBJECT("VisualProcess", m_visualProcess);
        SERIALIZE_OBJECT("LocalAABB", m_localAABB);
        SERIALIZE_OBJECT("WorldAABB", m_worldAABB);
    END_SERIALIZATION()
  
    BEGIN_SERIALIZATION(EdgeFluidLevel)
        SERIALIZE_MEMBER("IdEdgeFluid", m_idEdgeFluid);
        SERIALIZE_OBJECT("Data", m_data);
        SERIALIZE_OBJECT("Xf", m_xf);
        SERIALIZE_MEMBER("Scale", m_scale);
        SERIALIZE_CONTAINER_OBJECT("LayerInfos", m_layerInfos);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(DataFluid)
        BEGIN_CONDITION_BLOCK(/*ESerializeGroup_Data*/ESerialize_DataBin)
            SERIALIZE_CONTAINER_OBJECT("EdgeFluidList", m_edgeFluidList);
            SERIALIZE_CONTAINER_OBJECT("EdgeFluidListLevels", m_edgeFluidListLevels);
            SERIALIZE_MEMBER("IsCushion", m_isCushion);
            SERIALIZE_MEMBER("WeightMultiplier", m_weightMultiplier);
            SERIALIZE_CONTAINER_OBJECT("LayerInfos", m_layerInfos);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    static Seeder randomWater(548);



        /*
                1__3 
                |\ | 
                |_\| 
                0  2 
        */

    ITF_INLINE void FluidPropage(const FriseConfig *config, Vec2d &curHeight, Vec2d &nextHeight, f32 way, f32 _targetHeight)
    {
        curHeight.x() =  curHeight.x()*config->m_fluid.m_elasticity + 
            (nextHeight.y() - curHeight.y()) +
            (_targetHeight - curHeight.y())*config->m_fluid.m_viscosity;

        curHeight.y()  += curHeight.x() * config->m_fluid.m_velocity;
        nextHeight.x() += (curHeight.x() * config->m_fluid.m_amplification)*way; 
    }

    void  EdgeFluid::FillFluidToDraw(const EdgeData *_data, const ITF_VECTOR<FluidFriseLayer> &_layerInfos, const Transform2d& _xf, f32 _scale, FluidToDraw & _dstFluidToDraw)
    {
        if (!getHeightLevelBlended(0).size())
            return;

        const u32   steps = m_lastIndex; //NB_FLUID_LEVELS - 1;

        const Vec2d & p0_local = m_pos[0];
        const Vec2d & p1_local = m_pos[1];
        const Vec2d & p2_local = m_pos[2];
        const Vec2d & p3_local = m_pos[3];
        
        Vec2d p0_world = _xf.transformPos( p0_local );
        Vec2d p1_world = _xf.transformPos( p1_local );
        Vec2d p2_world = _xf.transformPos( p2_local );
        Vec2d p3_world = _xf.transformPos( p3_local );

        Vec2d::Lerp(&p1_world, &p0_world, &p1_world, _data->m_heightPos);
        Vec2d::Lerp(&p3_world, &p2_world, &p3_world, _data->m_heightPos);

        static Vec2d localArray[NB_FLUID_LEVELS + 1];

        const Vec2d baseVect  = p2_local - p0_local;
        const Vec2d leftVect  = p1_local - p0_local;
        const Vec2d rightVect = p3_local - p2_local;

        const Vec2d baseVectWorld  = p2_world - p0_world;
        const Vec2d leftVectWorld  = p1_world - p0_world;

        const f32 deltaBaseCoeff = 1.f /(m_localNorm*m_localNorm);
        const f32 deltaCoeff = 1.f /MESH_DEFAULT_VDIV;

        const f32 deltaBase1 = (leftVect ).dot(baseVect) *deltaBaseCoeff;
        const f32 deltaBase2 = (rightVect).dot(baseVect) *deltaBaseCoeff;
        f32 delta1 = deltaBase1 *deltaCoeff;
        f32 delta2 = deltaBase2 *deltaCoeff;

        f32 leftBorderHeight = 0, rightBorderHeight = 0;
        const u32 layerCount = _layerInfos.size();

        for (u32 levIndex=0; levIndex<layerCount; levIndex++)
        {
            const FluidHeightMap& heightMap = getHeightLevelBlended(levIndex);

            leftBorderHeight += heightMap[0].y();
            rightBorderHeight += heightMap[steps].y();
        }

        delta1 *= 1.f + leftBorderHeight;
        delta2 *= 1.f + rightBorderHeight;

        f32 yFactor =  _data->m_level;
        if (_data->m_way == 1)
            yFactor *= -1.f;

        const FluidColorMap &colorMap = m_fluidColorMap;
        ITF_ASSERT_CRASH(steps < NB_FLUID_LEVELS, "Too much fluid steps");

        const FluidHeightMap& heightMap0 = getHeightLevelBlended(0);
        for (u32 i=0; i<=steps; i++)
        {
            Vec2d& localArrayCur = localArray[i];

            localArrayCur.x() = colorMap[i];
            const f32 height = heightMap0[i].y();
            localArrayCur.y() = height*yFactor;
        }        

        for (u32 levIndex=1; levIndex<layerCount; levIndex++)
        {
            const FluidHeightMap &heightMap = getHeightLevelBlended(levIndex);

            for (u32 i=0; i<=steps; i++)
            {
                const f32 height = heightMap[i].y();
                localArray[i].y() += height*yFactor;
            }
        }
        FixedArray<Vec2d, 4> uv = m_uv;
        Matrix44 matrix, matrixTmp;

        if (_data->m_way == 0) // partie verticale
        {
            matrix.setRotationZ(m_worldAngle - MTH_PIBY2);
            matrix.mulScale(Vec3d( m_worldHeight*_data->m_heightScale, m_worldNorm * MESH_DEFAULT_VDIV / steps, 1.f) );
            matrix.setTranslation( (p0_world + Vec2d::Up.Rotate(m_worldAngle)*m_worldHeight*_data->m_heightScale).to3d(m_depth) + getShift(_data->m_pos) );

            const Vec2d deltaUV = _data->m_pos*m_deltaUV;
            for (u32 uvIdx=0; uvIdx<4; uvIdx++)
                uv[uvIdx] += deltaUV;
            
            if (_data->m_cutUV)
            {
                uv[1].y() = uv[1].y()*0.25f + uv[0].y()*0.75f;
                uv[3].y() = uv[3].y()*0.25f + uv[2].y()*0.75f;
            }
        } else // parties horizontales : front et back
        {
            matrix.setRotationZ(- MTH_PIBY2 );
            matrixTmp.setRotationX(-MTH_PIBY2*_data->m_way);
            matrix.mul44(matrix,matrixTmp);
            matrixTmp.setRotationZ(m_worldAngle );
            matrix.mul44(matrix,matrixTmp);
            
            if(steps)
                matrix.mulScale( Vec3d( _data->m_heightScale * _scale, m_worldNorm * MESH_DEFAULT_VDIV / steps, m_worldHeight) ); // perpedicular use m_height on Z too
            else
                matrix.mulScale( Vec3d( _data->m_heightScale * _scale, m_worldNorm , m_worldHeight) ); // perpedicular use m_height on Z too

            const f32     depth   = m_depth + _data->m_posZ;
            const Vec2d   pos     = p1_world - baseVectWorld * ((leftVectWorld ).dot(baseVectWorld)/(m_worldNorm*m_worldNorm));

            matrix.setTranslation( pos.to3d(depth));

            if (_data->m_cutUV)
            {
                FixedArray<Vec2d, 4> uvFinal = uv;
                
                if (_data->m_way == -1) // back
                {
                    const f32 ratio = 0.75f *(_data->m_heightScaleTotal - _data->m_heightScale)/_data->m_heightScaleTotal +0.25f;

                    uvFinal[0].y() = uv[1].y()*ratio + uv[0].y()*(1.f-ratio);
                    uvFinal[2].y() = uv[3].y()*ratio + uv[2].y()*(1.f-ratio);

                    uv[0] = uvFinal[1];
                    uv[1] = uvFinal[0];
                    uv[2] = uvFinal[3];
                    uv[3] = uvFinal[2];
                } else // front
                {
                    const f32 ratio = 0.75f *_data->m_heightScale/_data->m_heightScaleTotal +0.25f;

                    uvFinal[0].y() = uv[1].y()*0.25f + uv[0].y()*0.75f;
                    uvFinal[2].y() = uv[3].y()*0.25f + uv[2].y()*0.75f;
                    uvFinal[1].y() = uv[1].y()*ratio + uv[0].y()*(1.f-ratio);
                    uvFinal[3].y() = uv[3].y()*ratio + uv[2].y()*(1.f-ratio);

                    uv = uvFinal;
                }
            }
        }

        // FILL STRUCTURE

        _dstFluidToDraw.setMatrix(matrix);

        const u32 colorCount = m_colors.size();
        GFX_Vector4 * colorVect = _dstFluidToDraw.getColorVect();
        ITF_ASSERT_CRASH(colorCount<=GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT, "TOO MANY COLORS");

        for (u32 i=0; i<colorCount; i++)
        {
            Color           color(m_colors[i].getAsU32());
            GFX_Vector4     &colorVectElmt = colorVect[i];
            colorVectElmt.x() = color.getRed(); 
            colorVectElmt.y() = color.getGreen();
            colorVectElmt.m_z = color.getBlue();
            colorVectElmt.m_w = color.getAlpha();
        }

        GFX_Vector4 * bufferVect = _dstFluidToDraw.getBufferVect();
        int hdivCount = 0;
        int heightTabSize = steps+1;
        const int rest = (heightTabSize % 2);

        if (rest)
            heightTabSize--;

        for (int i=0; i<(int)heightTabSize;hdivCount++)
        {
            GFX_Vector4& bufferVectCur = bufferVect[hdivCount];

            bufferVectCur.x() = localArray[i].x();
            bufferVectCur.y() = localArray[i++].y();
            bufferVectCur.m_z = localArray[i].x();
            bufferVectCur.m_w = localArray[i++].y();
        }

        if (rest)
        {
            GFX_Vector4& bufferVectCur = bufferVect[hdivCount];

            bufferVectCur.x() = localArray[heightTabSize].x();
            bufferVectCur.y() = localArray[heightTabSize].y();
            bufferVectCur.m_z = 0.f;
            bufferVectCur.m_w = 0.f;

            heightTabSize++;
            hdivCount++;
        }

        _dstFluidToDraw.setHeighTabSize(heightTabSize);
        _dstFluidToDraw.setHDivCount(hdivCount);
        _dstFluidToDraw.setDelta1(delta1);
        _dstFluidToDraw.setDelta2(delta2);
        _dstFluidToDraw.setStepCount((f32)steps);

        GFX_Vector4   & uv1 = _dstFluidToDraw.getUV1();
        GFX_Vector4   & uv2 = _dstFluidToDraw.getUV2();

        uv1.x() = uv[0].x();
        uv1.y() = uv[0].y();
        uv1.m_z = uv[1].x();
        uv1.m_w = uv[1].y();
        uv2.x() = uv[2].x();
        uv2.y() = uv[2].y();
        uv2.m_z = uv[3].x();
        uv2.m_w = uv[3].y();
    }

    void  EdgeFluid::updateData( const FriseConfig *config, u32 _mask)
    {
        if (_mask & EDGEFLUID_MAIN_DATA)
        {
            Vec2d sight = m_pos[2] - m_pos[0];
            m_localNorm = sight.norm();

            m_lastIndex = (u32)ceil(m_localNorm/config->m_fluid.m_unityWidth);
            if (m_lastIndex >= NB_FLUID_LEVELS)
                m_lastIndex = NB_FLUID_LEVELS-1;
            if (m_lastIndex < 1)
                m_lastIndex = 1;

            m_meshLevel = 8;
            while (m_meshLevel < m_lastIndex)
                m_meshLevel *= 2;
        }

        if (_mask & EDGEFLUID_VISUAL)
        {
            m_visualProcess.m_posStart  = m_pos[0] + ( m_pos[1] - m_pos[0] ) * config->m_visualOffset;
            m_visualProcess.m_posEnd    = m_pos[2] + ( m_pos[3] - m_pos[2] ) * config->m_visualOffset;

            m_visualProcess.m_count     = m_lastIndex / config->m_fluid.m_polylineUnityMult;
            if (m_visualProcess.m_count == 0) m_visualProcess.m_count = 1;
            m_visualProcess.m_posOffset = (m_visualProcess.m_posEnd - m_visualProcess.m_posStart) / (f32)m_visualProcess.m_count;
            m_visualProcess.m_normal    = m_visualProcess.m_posOffset.getPerpendicular();
            m_visualProcess.m_normal.normalize();
        }


        if (_mask & EDGEFLUID_COLLISION)
        {
            const u32 sideCount = (config->m_methode == Frise::InString)? 2:1;

            for (u32 sideIndex=0; sideIndex<sideCount; sideIndex++)
            {
                EdgeProcessData &collisionProcess = m_collisionProcesses[sideIndex];

                const f32 collisionOffset = config->m_collisionFrieze.m_offset;
                if (sideIndex==0)
                {
                    Vec2d::Lerp(&collisionProcess.m_posStart, &m_pos[0], &m_pos[1], collisionOffset);
                    Vec2d::Lerp(&collisionProcess.m_posEnd,   &m_pos[2], &m_pos[3], collisionOffset);
                }
                else
                {
                    Vec2d::Lerp(&collisionProcess.m_posEnd,   &m_pos[0], &m_pos[1], collisionOffset);
                    Vec2d::Lerp(&collisionProcess.m_posStart, &m_pos[2], &m_pos[3], collisionOffset);
                }

                collisionProcess.m_count     = m_lastIndex / config->m_fluid.m_polylineUnityMult;
                if (collisionProcess.m_count == 0)
                    collisionProcess.m_count = 1;
                collisionProcess.m_posOffset = (collisionProcess.m_posEnd - collisionProcess.m_posStart) / (f32)collisionProcess.m_count;
                collisionProcess.m_normal    = collisionProcess.m_posOffset.getPerpendicular();
                collisionProcess.m_normal.normalize();
            }
        }
    }

    void EdgeFluid::fillPolyline( PolyPointList& _polypoint, u32 _mask, u32 _subIndex)
    {
        EdgeProcessData *procData = NULL;
        if (_mask & EDGEFLUID_VISUAL)
        {
            procData = &m_visualProcess;
            ITF_ASSERT(_subIndex==0);
        } else if (_mask & EDGEFLUID_COLLISION)
        {
            ITF_ASSERT(_subIndex<maxCollisionProcessCount);
            procData = &m_collisionProcesses[_subIndex];
        } else
            return;
        
        int count = procData->m_count - 1;
        if (count < 1) count = 1;
        Vec2d posCur = procData->m_posStart;
        for ( i32 i = 0; i < count; i++ )
        {
            _polypoint.addPoint( posCur );
            posCur += procData->m_posOffset;
        }
        if (_mask & EDGEFLUID_COLLISION)
            _polypoint.addPoint( procData->m_posEnd );
    }

    void DataFluid::setFrieze(Frise * _frieze)
    {
        ITF_ASSERT(_frieze);
        if (_frieze)
        {
            m_frieze = _frieze->getRef();
        }
    }

    Frise * DataFluid::getFrieze()
    {
        return (Frise *)GETOBJECT(m_frieze);
    }

    DataFluid::~DataFluid()
    {
        deleteFluidShapers();
    }

    void DataFluid::deleteFluidShapers()
    {
        for (u32 layerIndex=0; layerIndex<MAX_FLUID_LAYER_COUNT; layerIndex++)
        {
            SafeArray<FluidShaper*> &layer = m_virtualPerturbers[layerIndex];
            for (u32 i=0; i<layer.size(); i++)
            {
                layer[i]->decrementRefCount();
            }
            layer.clear();
        }        
    }

    void DataFluid::addStringWaveGenerator(ObjectRef _actorRef)
    {
        if (m_stringWaveGenerators.find(_actorRef)<0)
        {
            m_stringWaveGenerators.push_back(_actorRef);
        }
    }

    void DataFluid::removeStringWaveGenerator(ObjectRef _actorRef)
    {
        i32 index = m_stringWaveGenerators.find(_actorRef);
        if (index>=0)
        {
            m_stringWaveGenerators.eraseNoOrder(index);
        }
    }

    void DataFluid::addActor(ObjectRef _actorRef, f32 _speedNorm, f32 _radius, f32 _waterMultiplier, f32 _weight, bbool _queryPosition, bbool _noFX)
    {
        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        const FriseConfig * config = frieze->getConfig();
        if (!config)
            return;

        Actor *actor = (Actor *)GETOBJECT(_actorRef);
        if (actor)
        {
            for (u32 i=0; i<m_contactActorsInfo.size(); i++)
            {
                if (m_contactActorsInfo[i].m_ref == _actorRef)
                {
                    ITF_ASSERT_MSG(0, "Actor %s is already in the frieze\nplease verify swimingOn/swimingOff", actor->getUserFriendly().cStr());
                    return;
                }
            }


            ActorInfo actorInfo;
            m_contactActorsInfo.push_back(actorInfo);

            ActorInfo *pActorInfo = &m_contactActorsInfo[m_contactActorsInfo.size() - 1];
            pActorInfo->m_ref             = _actorRef;
            // remove speed to process speed
            pActorInfo->m_prevPos         = actor->get2DPos();
            pActorInfo->m_radius          = _radius;
            pActorInfo->m_waterMultiplier = _waterMultiplier;
            pActorInfo->m_mass            = _weight;
            pActorInfo->m_queryPosition   = _queryPosition;

            // special influence on contact !
            pActorInfo->m_prevInfluence   = config->m_fluid.m_enterMult;

            if (!m_isCushion)
            {
                u32 edgeIdx ,pointIdx;
                computeInfluence(actor->get2DPos(), _speedNorm*_waterMultiplier,
                                 _radius, &pActorInfo->m_prevInfluence,
                                 1, bfalse,
                                 _weight, edgeIdx ,pointIdx);

                if ( !_noFX )
                {
                    processFx(edgeIdx ,pointIdx, FluidFxType_Enter);
                }
            }
        }
    }

    void DataFluid::removeActor(ObjectRef _actorRef, f32 _speedNorm, bbool _noFX )
    {
        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        const FriseConfig * config = frieze->getConfig();
        if (!config)
            return;

        u32 count = m_contactActorsInfo.size();
        for (u32 i=0; i<count; ++i)
        {
            ActorInfo &actorinfo = m_contactActorsInfo[i];
            if (actorinfo.m_ref == _actorRef)
            {
                Actor *actor = (Actor *)GETOBJECT(_actorRef);
                if (actor && !m_isCushion)
                {
                    actorinfo.m_prevInfluence   = config->m_fluid.m_leaveMult;

                    u32 edgeIdx ,pointIdx;
                    computeInfluence(actor->get2DPos(), _speedNorm*actorinfo.m_waterMultiplier,
                                     actorinfo.m_radius, &actorinfo.m_prevInfluence, -1.f, bfalse,actorinfo.m_mass, edgeIdx ,pointIdx);

                    if ( !_noFX )
                    {
                        processFx(edgeIdx ,pointIdx, FluidFxType_Leave);
                    }
                }
                m_contactActorsInfo.eraseNoOrder(i);
                return;
            }
        }
    }

    void DataFluid::addFluidShaper(u32 _layerIndex, FluidShaper *_shaper)
    {
        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        if (_layerIndex<m_layerInfos.size())
        {
            SafeArray<FluidShaper*> &perturbers = m_virtualPerturbers[_layerIndex];
            if (perturbers.find(_shaper)<0)
            {
                perturbers.push_back(_shaper);
                ITF_ASSERT(perturbers.size()<=10);
                _shaper->incrementRefCount();
            }
        }
    }

    void DataFluid::removeFluidShaper(u32 _layerIndex, FluidShaper *_shaper)
    {
        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        if (_layerIndex<m_layerInfos.size())
        {
            i32 foundIndex = m_virtualPerturbers[_layerIndex].find(_shaper);
            if (foundIndex>=0)
            {
                m_virtualPerturbers[_layerIndex].eraseKeepOrder((u32)foundIndex);
                _shaper->decrementRefCount();
            }
        }
    }


    void DataFluid::sendPerturbation(const Vec2d& _pos, f32 _alteration, f32 _radius, bbool radiusRandom, f32 _mass)
    {
        f32 prevInfluence = 1.f;
        u32 edgeIdx ,pointIdx;
        computeInfluence(_pos, _alteration, _radius, &prevInfluence, 1.f, radiusRandom, _mass, edgeIdx ,pointIdx);
        processFx(edgeIdx ,pointIdx, FluidFxType_Plouf);
    }



    void DataFluid::applyLocalInfluence(Frise *_frieze, const FriseConfig *_config, f32 _deltaHeight, i32 _edgeIndex, i32 _referencePointIndex, i32 _startIndex, i32 _endIndex, i32 _indexStep, bbool _radiusRandom, i32 _influenceRadiusIndices)
    {
        for (i32 k = _startIndex; k !=_endIndex; k+=_indexStep )
        {
            int curIdx  = _edgeIndex;
            i32 kIn     = k;

            while (kIn < 0)
            {
                curIdx--;
                if (curIdx < 0)
                {
                    if (!_frieze->isLooping())
                    {
                        return;
                    }
                    curIdx = m_edgeFluidList.size()-1;
                }
                kIn += m_edgeFluidList[curIdx].m_lastIndex + 1;
            }

            while (kIn > (int)m_edgeFluidList[curIdx].m_lastIndex)
            {
                kIn -= m_edgeFluidList[curIdx].m_lastIndex + 1;
                curIdx++;
                if (curIdx >= (int)m_edgeFluidList.size())
                {
                    if (!_frieze->isLooping())
                    {
                        return;
                    }
                    curIdx = 0;
                }
            }

            if (kIn<(i32)m_edgeFluidList[curIdx].getHeightLevelSet(0).size())
            {
                Vec2d& heightMapLevelCur = m_edgeFluidList[curIdx].getHeightLevelSet(0)[kIn];

                if (_radiusRandom)
                    heightMapLevelCur.y() += randomWater.GetFloat(_deltaHeight);
                else
                {
                    f32 percentOfDistanceSQ = (k - _referencePointIndex)/(f32)_influenceRadiusIndices;
                    percentOfDistanceSQ *= percentOfDistanceSQ;
                    heightMapLevelCur.y() += _deltaHeight*(1-percentOfDistanceSQ);
                }

                if ( heightMapLevelCur.y() > _config->m_fluid.m_targetMaxHeight )
                    heightMapLevelCur.y() = _config->m_fluid.m_targetMaxHeight;

                if ( heightMapLevelCur.y() < _config->m_fluid.m_targetMinHeight )
                    heightMapLevelCur.y() = _config->m_fluid.m_targetMinHeight;
            }
        }
    }

    void DataFluid::computeInfluence(Vec2d _pos, f32 speedNorm, f32 _raidus, f32 * prevInfluence, f32 way, bbool _radiusRandom, f32 _mass, u32 & _edgeIdx ,u32 & _pointIdx)
    {
        Frise* frieze   = getFrieze();
        _edgeIdx        = U32_INVALID;
        _pointIdx       = U32_INVALID;

        if (!frieze)
            return;

        const FriseConfig * config = frieze->getConfig();
        if (!config)
            return;

        _pos -= frieze->get2DPos();

        int delta = 0;
        if (_raidus > 0.f)
            delta = (int)ceil(_raidus*config->m_fluid.m_dstInfluenceMultiplier / config->m_fluid.m_unityWidth);
        if (delta == 0)
            delta = 1;

        // search "true" contact point between frize and actor -> min distance
        f32 minDst = MTH_HUGE;

        //Detecting horizontal cushion
        bbool isHorizontalCushion = bfalse;
        if (m_isCushion)
        {
            const i32 edgeCount = (i32)frieze->getPosCount()-1;
            isHorizontalCushion = btrue;

            for (i32 i=0; i<edgeCount; i++)
            {
                const PolyLineEdge &edge = frieze->getEdgeAt((u32)i);
                if (fabsf(edge.m_normalizedVector.x())<0.995f)
                    isHorizontalCushion = bfalse;
            }
        }

        f32 horizontalCushion_EdgeRelative = 0;
        if (isHorizontalCushion)
        {
            //horizontal cushion for more accurate test
            for (u32 j=0; j<m_edgeFluidList.size(); j++)
            {
                const EdgeFluid &edgeFluid = m_edgeFluidList[j];

                const f32 edgeStart = edgeFluid.m_pos[1].x();
                const f32 edgeWidth = edgeFluid.m_pos[3].x() - edgeStart;

                f32 edgeRelative = _pos.x()-edgeStart;
                if (edgeWidth!=0)
                {
                    edgeRelative /= edgeWidth;
                    if (edgeRelative>=-0.001f && edgeRelative<=1.001f)
                    {
                        f32 dy = _pos.y()-edgeFluid.m_pos[1].y();

                        f32 dist = f32_Abs(dy);
                        if (dist<minDst)
                        {
                            minDst      = dist;
                            _edgeIdx    = j;
                            horizontalCushion_EdgeRelative = Min(Max(edgeRelative,0.f),1.f);
                        }
                    }

                }

            }
        }
        else
        {
            for (u32 j=0; j<m_edgeFluidList.size(); j++)
            {
                const EdgeFluid& edgeFluidCur = m_edgeFluidList[j];

                Vec2d edgeVectNorm  = edgeFluidCur.m_pos[2] - edgeFluidCur.m_pos[0];
                edgeVectNorm  = edgeVectNorm.normalize();

                const Vec2d actorToEdge   = edgeFluidCur.m_pos[2] - _pos;
                const Vec2d contactPoint =  edgeFluidCur.m_pos[2] - edgeVectNorm *(actorToEdge.dot(edgeVectNorm ));

                if (
                    (
                    ((contactPoint - edgeFluidCur.m_pos[0]).dot(edgeVectNorm ) >= 0.f) && 
                    ((contactPoint - edgeFluidCur.m_pos[2]).dot(edgeVectNorm ) <= 0.f)
                    )
                    )

                {
                    const Vec2d actorToEdge2  = edgeFluidCur.m_pos[3] - _pos;
                    Vec2d edgeVectNorm2 = edgeFluidCur.m_pos[3] - edgeFluidCur.m_pos[1];
                    edgeVectNorm2 = edgeVectNorm2.normalize();

                    f32 dst;
                    const Vec2d v1 = (actorToEdge  - edgeVectNorm*(actorToEdge.dot(edgeVectNorm)));
                    const Vec2d v2 = (actorToEdge2 - edgeVectNorm2*(actorToEdge2.dot(edgeVectNorm2)));

                    if (m_isCushion)
                    {
                        Vec2d upVec=edgeVectNorm2.getPerpendicular();
                        dst = actorToEdge2.dot(upVec);
                        if (dst<0)
                            dst = 0;
                    }
                    else
                    {
                        if (v1.dot(v2) < 0) // actor inside edge
                            dst = 0.f;
                        else
                            dst = Min(v1.norm(), v2.norm());
                    }

                    if (dst < minDst)
                    {
                        _edgeIdx    = j;
                        minDst      = dst;
                    }
                }
            }
        }

        //compute influence from 
        const f32 newInfluence = (config->m_fluid.m_maxDstInfluence - minDst)/config->m_fluid.m_maxDstInfluence;
        const f32 dstInfluence = Max(newInfluence, *prevInfluence);
        *prevInfluence = newInfluence;

        if (_edgeIdx != U32_INVALID && dstInfluence > 0.f)
        {
            //compute index of point in edge
            EdgeFluid & edgeFluid = m_edgeFluidList[_edgeIdx];
            if (isHorizontalCushion)
            {
                _pointIdx = (u32)(horizontalCushion_EdgeRelative*edgeFluid.m_lastIndex);
            }
            else
            {
                const Vec2d edgeVect = edgeFluid.m_pos[2] - edgeFluid.m_pos[0];
                Vec2d edgeVectNorm  = edgeVect;
                edgeVectNorm  = edgeVectNorm.normalize();
                const Vec2d actorToEdge   = edgeFluid.m_pos[2] - _pos;

                const Vec2d   contactPoint  =  edgeFluid.m_pos[2] - edgeVectNorm *(actorToEdge.dot(edgeVectNorm ));
                _pointIdx = (u32)((contactPoint - edgeFluid.m_pos[0]).norm()/edgeVect.norm()*edgeFluid.m_lastIndex);
            }


            // compute deformation
            f32 val;
            if (m_isCushion)
            {
                float konst=-_mass*m_weightMultiplier;
                val = konst*dstInfluence;
            } else
            {
                val = config->m_fluid.m_targetAddHeight * speedNorm * dstInfluence * way;
            }

            const f32 influenceLimit = Max(config->m_fluid.m_influenceLimit,0.f);
            if (val<-influenceLimit)
            {
                val = -influenceLimit;
            }
            else if (val>influenceLimit)
            {
                val = influenceLimit;
            }
            
            //apply
            delta = Max(delta,2);
            applyLocalInfluence(frieze, config, val, _edgeIdx, _pointIdx, _pointIdx,_pointIdx+delta, 1, _radiusRandom, delta);
            applyLocalInfluence(frieze, config, val, _edgeIdx, _pointIdx, _pointIdx-1,_pointIdx-delta, -1, _radiusRandom, delta);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void DataFluid::setLayerCollisionHeightMultiplier(u32 _layerIndex, f32 _multiplier)
    {
        ITF_ASSERT(_layerIndex<m_layerInfos.size());
        if (_layerIndex<m_layerInfos.size())
        {
            m_layerInfos[_layerIndex].m_collisionHeightMultiplier = _multiplier;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void DataFluid::resetAllLayerSettings()
    {
        for (u32 layerIndex=0; layerIndex<m_layerInfos.size(); layerIndex++)
        {
            setLayerCollisionHeightMultiplier(layerIndex, 1.f);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void DataFluid::processVirtualPerturbers(Frise *frieze, const FriseConfig *_config)
    {
        //reinitialize colors
        for (u32 edgeIndex=0; edgeIndex<m_edgeFluidList.size(); edgeIndex++)
        {
            EdgeFluid &edgeFluid = m_edgeFluidList[edgeIndex];
            FluidColorMap &colorMap = edgeFluid.m_fluidColorMap;
            const FluidHeightMap &heightMap = edgeFluid.getHeightLevelSet(0);
            FluidGameMaterialMap &gameMaterialMap = edgeFluid.m_fluidGameMaterialMap;

            if (heightMap.size()==0)
                continue;                

            const u32 maxSize = edgeFluid.m_lastIndex;
            for (u32 i=0; i<=maxSize; i++)
            {
                colorMap[i]=0;
            }

            for (u32 i=0; i<=maxSize; i++)
            {
                gameMaterialMap[i]=0;
            }

        }

        //
        for (u8 layerIndex=0; layerIndex<_config->m_fluid.m_layerCount; layerIndex++)
        {
            SafeArray<FluidShaper*> &objectsInlayer = m_virtualPerturbers[layerIndex];

            for (u32 edgeIndex=0; edgeIndex<m_edgeFluidList.size(); edgeIndex++)
            {
                EdgeFluid &edgeFluid = m_edgeFluidList[edgeIndex];
                FluidHeightMap &heightMap = edgeFluid.getHeightLevelSet(layerIndex);
                FluidColorMap &colorMap = edgeFluid.m_fluidColorMap;
                FluidGameMaterialMap &gameMaterialMap = edgeFluid.m_fluidGameMaterialMap;

                if (heightMap.size()==0)
                    continue;                

                const u32 maxSize = edgeFluid.m_lastIndex;
                const Vec2d posScale = (edgeFluid.m_pos[2]-edgeFluid.m_pos[0])/(float)maxSize;
                const Vec2d pos0 = edgeFluid.m_pos[0]+frieze->get2DPos();
                Vec2d posX = pos0;

                for (u32 i=0; i<=maxSize; (i++), (posX += posScale))
                {                    
                    f32 y=0;
                    const u32 objectCount = objectsInlayer.size();
                    bbool foundAny = bfalse;
                    bbool modifyColor = bfalse;
                    bbool modifyGameMaterial = bfalse;
                    u8 presentLayerIndex=0;
                    f32 waveWeight = 0;

                    for (u32 perturbIndex = 0; perturbIndex<objectCount;perturbIndex++)
                    {
                        const FluidShaper *perturber = objectsInlayer[perturbIndex];
                        bbool foundOne = bfalse;
                        f32 localWeight = 0;
                        bbool hasColor = bfalse;
                        bbool hasGMT = bfalse;
                        y += perturber->getHeight_2D(posX, foundOne,localWeight, hasColor, hasGMT);

                        if (foundOne)
                        {
                            foundAny = btrue;
                            modifyColor = modifyColor | hasColor;
                            modifyGameMaterial = modifyGameMaterial | hasGMT;
                            presentLayerIndex = layerIndex;
                            waveWeight = Min(Max(waveWeight, localWeight*2.f),1.f);

                        }
                    }

                    if (foundAny)
                    {
                        heightMap[i].y() = heightMap[i].y()*(1-waveWeight)+y*waveWeight;
                    }

                    if (modifyColor)
                    {
                        u8 &color = colorMap[i];
                        color = presentLayerIndex; //NB : WAS STD::MAX
                        if (modifyGameMaterial) //We can modify game material only if we modify color
                        {
                            gameMaterialMap[i] = presentLayerIndex;
                        }
                    }
                }

            }
        }
    }

    void DataFluid::ProcessEdgeFluidList_InString(f32 _elapsed)
    {
        if (m_edgeFluidList.size() == 0)
            return;

        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        const FriseConfig * config = frieze->getConfig();
        if (!config)
            return;

        const Transform2d xf( frieze->get2DPos(), frieze->getAngle(), frieze->getScale(), frieze->getIsFlipped() );

        updateUV(_elapsed);

        processVirtualPerturbers(frieze, config);

        checkElevationMapInitialization(config);
        checkActorInfluence();
        handleAbsorptionAtEdgeStartEnd_String(config);
        handlePropagation(frieze, config);

        copyFromSetToBlend(config);

        updateCollisionPolyLine_InString(frieze, config, xf);
        updateVisualPolyLine(frieze, config, xf);
    }

    void DataFluid::checkElevationMapInitialization(const FriseConfig *config)
    {
        const u32 steps = NB_FLUID_LEVELS - 1;
        EdgeFluid *curEdgeFluid;

        for (u8 layerIndex=0; layerIndex<config->m_fluid.m_layerCount; layerIndex++)
        {
            if (!m_edgeFluidList[0].getHeightLevelSet(layerIndex).size()) // verify if init ok
            {
                f32 targetHeight = (layerIndex==0)? config->m_fluid.m_targetHeight:0.f;

                for (ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
                    edgeIter != m_edgeFluidList.end(); ++edgeIter)
                {
                    curEdgeFluid = &(*edgeIter);
                    FluidHeightMap &curEdgeFluidHeightMap = curEdgeFluid->m_heightLevelSet[layerIndex];
                    FluidColorMap &curEdgeFluidColorMap = curEdgeFluid->m_fluidColorMap;
                    FluidGameMaterialMap &curEdgeFluidGMTMap = curEdgeFluid->m_fluidGameMaterialMap;
                    if (!curEdgeFluidHeightMap.size())
                    {
                        curEdgeFluidHeightMap.resize(steps+1);
                        curEdgeFluidColorMap.resize(steps+1);
                        curEdgeFluidGMTMap.resize(steps+1);

                        for (u32 i=0; i<=steps; i++)
                        {
                            curEdgeFluidHeightMap[i].x() = 0;
                            curEdgeFluidHeightMap[i].y() = targetHeight;
                            curEdgeFluidColorMap[i] = 0;
                            curEdgeFluidGMTMap[i] = 0;
                        }

                        if (config->m_fluid.m_blendFactor)
                        {
                            curEdgeFluid->m_useBlend = btrue;

                            FluidHeightMap &curEdgeFluidHeightMapBlended = curEdgeFluid->m_heightLevelBlended[layerIndex];
                            curEdgeFluidHeightMapBlended.resize(steps+1);
                            for (u32 i=0; i<=steps; i++)
                            {
                                curEdgeFluidHeightMapBlended[i].x() = 0;
                                curEdgeFluidHeightMapBlended[i].y() = targetHeight;
                            }
                        } else
                        {
                            curEdgeFluid->m_useBlend = bfalse;
                        }
                    }
                }
            }
        }
    }

    void DataFluid::checkActorInfluence()
    {
        for (u32 i = 0; i < m_contactActorsInfo.size(); i++)
        {
            ActorInfo &actorInfo = m_contactActorsInfo[i];
            Actor * actor = (Actor *)GETOBJECT(actorInfo.m_ref);
            if (!actor)
                continue;

            f32 speedNorm = 0.0f;
            Vec2d pos;
            if (actorInfo.m_queryPosition)
            {
                EventQueryWaterInfluence query;
                query.setSender(m_frieze);
                actor->onEvent(&query);
                pos = query.getPos();

            }
            else
            {
                pos = actor->get2DPos();
            }

            speedNorm = (pos - actorInfo.m_prevPos).norm()/LOGICDT;
            actorInfo.m_prevPos = pos;

            if (speedNorm < MTH_EPSILON && !m_isCushion)
                continue;

            u32 edgeIdx ,pointIdx;
            computeInfluence(pos, speedNorm*actorInfo.m_waterMultiplier,
                actorInfo.m_radius,  &actorInfo.m_prevInfluence,
                1.f,
                bfalse,
                actorInfo.m_mass,
                edgeIdx ,pointIdx);
            processFx(edgeIdx ,pointIdx, FluidFxType_Swim);
        }
    }

    void DataFluid::handleAbsorptionAtEdgeStartEnd_String(const FriseConfig *config)
    {
        if ((config->m_fluid.m_absorptionAtEdgeStart==0 && config->m_fluid.m_absorptionAtEdgeEnd==0) || (config->m_fluid.m_absorptionAtEdge_Length<0) )
        {
            return;
        }

        EdgeFluid *curEdgeFluid=NULL;

        for (u32 layerIndex=0; layerIndex<config->m_fluid.m_layerCount; layerIndex++)
        {
            const f32 targetHeight = (layerIndex==0)? config->m_fluid.m_targetHeight:0.f;

            EdgeFluid *prevEdge = NULL;

            u32 nextEdgeIndex;
            const u32 edgeCount = m_edgeFluidList.size();
            nextEdgeIndex=1;

            for (ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
                edgeIter < m_edgeFluidList.end(); ++edgeIter, ++nextEdgeIndex, prevEdge = curEdgeFluid)
            {
                curEdgeFluid = &(*edgeIter);
                EdgeFluid *nextEdge = NULL;
                if (nextEdgeIndex<edgeCount)
                {
                    nextEdge = &m_edgeFluidList[nextEdgeIndex];
                }

                FluidHeightMap &curEdgeHeightMap = curEdgeFluid->getHeightLevelSet(layerIndex);
                if (curEdgeHeightMap.size()==0)
                {
                    continue;
                }

                const f32 edgeLen = (curEdgeFluid->m_pos[2]-curEdgeFluid->m_pos[0]).norm();
                if (edgeLen<MTH_EPSILON)
                {
                    continue;
                }
                
                const f32 absorptionLenPercent =  f32_Min(config->m_fluid.m_absorptionAtEdge_Length/edgeLen ,1.f);                
                const u32 absorptionLength = (u32)(curEdgeFluid->m_lastIndex*absorptionLenPercent);
                const bbool hasPreviousEdge = (prevEdge!=NULL && curEdgeFluid->m_pos[0].IsEqual(prevEdge->m_pos[2], MTH_EPSILON));

                //absorption at start
                if (!hasPreviousEdge)
                {
                    const f32 absorptionAtStart = config->m_fluid.m_absorptionAtEdgeStart;
                    f32 currentAbsorption = absorptionAtStart;
                    const f32 absorptionStep = currentAbsorption/(f32)(absorptionLength+1);

                    for (u32 i=0; i<=absorptionLength; i++, currentAbsorption-=absorptionStep)
                {
                    f32 &height = curEdgeHeightMap[i].y();

                        height = height-(height-targetHeight)*currentAbsorption;
                    }
                }

                //absorption at end
                const bbool hasNextEdge = (nextEdge!=NULL && curEdgeFluid->m_pos[2].IsEqual(nextEdge->m_pos[0], MTH_EPSILON));
                if (!hasNextEdge)
                {
                    const u32 lastIndex = curEdgeFluid->m_lastIndex;
                    const f32 absorptionAtEnd = config->m_fluid.m_absorptionAtEdgeEnd;
                    f32 currentAbsorption = 0;
                    const f32 absorptionStep = absorptionAtEnd/(f32)(absorptionLength+1);

                    for (u32 i=lastIndex-absorptionLength; i<=lastIndex; i++, currentAbsorption += absorptionStep)
                    {
                        f32 &height = curEdgeHeightMap[i].y();

                        height = height-(height-targetHeight)*currentAbsorption;
                    }
                }
            }
        }
    }

    void DataFluid::handlePropagation(Frise *frieze, const FriseConfig *config)
    {
        const bbool isLooping = frieze->isLooping();

        EdgeFluid *curEdgeFluid;
        EdgeFluid *prevEdgeFluid;
        EdgeFluid *nextEdgeFluid;

        for (u32 layerIndex=0; layerIndex<config->m_fluid.m_layerCount; layerIndex++)
        {
            const f32 targetHeight = (layerIndex==0)? config->m_fluid.m_targetHeight:0.f;

            for (ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
                edgeIter < m_edgeFluidList.end(); ++edgeIter)
            {
                curEdgeFluid = &(*edgeIter);
                if (edgeIter != (m_edgeFluidList.end() - 1))
                    nextEdgeFluid = &(*(edgeIter+1));
                else if (isLooping)
                    nextEdgeFluid = &(*m_edgeFluidList.begin());
                else
                    nextEdgeFluid = NULL;

                if (nextEdgeFluid && !curEdgeFluid->m_pos[2].IsEqual(nextEdgeFluid->m_pos[0], MTH_EPSILON))
                    nextEdgeFluid = NULL;

                FluidHeightMap &curEdgeHeightMap = curEdgeFluid->getHeightLevelSet(layerIndex);

                for (u32 i=0; i < curEdgeFluid->m_lastIndex;i++) 
                { 
                    FluidPropage(config, curEdgeHeightMap[i],curEdgeHeightMap[i+1], 1.f, targetHeight);
                }
                if (nextEdgeFluid)
                {
                    FluidHeightMap &nextEdgeHeightMap = nextEdgeFluid->getHeightLevelSet(layerIndex);
                    FluidPropage(config, curEdgeHeightMap[curEdgeFluid->m_lastIndex],
                        nextEdgeHeightMap[1], 1.f,
                        targetHeight);
                    nextEdgeHeightMap[0] = curEdgeHeightMap[curEdgeFluid->m_lastIndex];
                } else
                {
                    FluidPropage(config, curEdgeHeightMap[curEdgeFluid->m_lastIndex],
                        curEdgeHeightMap[curEdgeFluid->m_lastIndex-1], -1.f, 
                        targetHeight);
                }
            }
        }

        for (u32 layerIndex=0; layerIndex<config->m_fluid.m_layerCount; layerIndex++)
        {
            const f32 targetHeight = (layerIndex==0)? config->m_fluid.m_targetHeight:0.f;
            const i32 last = (i32)m_edgeFluidList.size() - 1;

            for (i32 i=last; i >= 0; --i)
            {
                curEdgeFluid = &m_edgeFluidList[i];
                if (i > 0)
                    prevEdgeFluid = &m_edgeFluidList[i-1];
                else if (isLooping)
                    prevEdgeFluid = &m_edgeFluidList[last];
                else
                    prevEdgeFluid = NULL;            

                if (prevEdgeFluid && !curEdgeFluid->m_pos[0].IsEqual(prevEdgeFluid->m_pos[2], MTH_EPSILON))
                    prevEdgeFluid = NULL;            

                FluidHeightMap &curEdgeHeightMap = curEdgeFluid->getHeightLevelSet(layerIndex);

                for (i32 i=(i32)curEdgeFluid->m_lastIndex-1; i > 0; i--) 
                { 
                    FluidPropage(config, curEdgeHeightMap[i],
                        curEdgeHeightMap[i-1], 1.f,
                        targetHeight);
                }
                if (prevEdgeFluid)
                {
                    FluidHeightMap &prevEdgeHeightMap = prevEdgeFluid->getHeightLevelSet(layerIndex);
                    FluidPropage(config, curEdgeHeightMap[0],
                        prevEdgeHeightMap[prevEdgeFluid->m_lastIndex-1], 1.f,
                        targetHeight);
                    prevEdgeHeightMap[prevEdgeFluid->m_lastIndex] = curEdgeHeightMap[0];
                } else
                {
                    FluidPropage(config, curEdgeHeightMap[0],
                        curEdgeHeightMap[1], -1.f,
                        targetHeight);
                }
            }
        }

    }

    u8 DataFluid::getColorIndexAt_InString(const EdgeFluid *_edge, u32 _edgeIndex, i32 _posIndex) const
    {
        while (_posIndex<0)
        {
            if (_edgeIndex==0)
            {
                return 0;
            }
            _edgeIndex--;
            _edge = &m_edgeFluidList[_edgeIndex];
            _posIndex += (i32)_edge->m_lastIndex+1;
        }
        while (_posIndex>(i32)_edge->m_lastIndex)
        {
            u32 edgeCount = m_edgeFluidList.size();
            _posIndex -= (i32)_edge->m_lastIndex+1;
            _edgeIndex++;
            if (_edgeIndex>=edgeCount)
            {
                return 0;
            }
            _edge = &m_edgeFluidList[_edgeIndex];
        }
        return _edge->m_fluidColorMap[_posIndex];
    }

    u8 DataFluid::getGameMaterialIndexAt_InString(const EdgeFluid *_edge, u32 _edgeIndex, i32 _posIndex) const
    {
        while (_posIndex<0)
        {
            if (_edgeIndex==0)
            {
                return 0;
            }
            _edgeIndex--;
            _edge = &m_edgeFluidList[_edgeIndex];
            _posIndex += (i32)_edge->m_lastIndex+1;
        }
        while (_posIndex>(i32)_edge->m_lastIndex)
        {
            u32 edgeCount = m_edgeFluidList.size();
            _posIndex -= (i32)_edge->m_lastIndex+1;
            _edgeIndex++;

            if (_edgeIndex>=edgeCount)
            {
                return 0;
            }

            _edge = &m_edgeFluidList[_edgeIndex];
        }
        return _edge->m_fluidGameMaterialMap[_posIndex];
    }

    f32 DataFluid::computeFluidHeight_InString(const FriseConfig *_config, EdgeFluid &_edge, u32 _edgeIndex, i32 _indexForCollision, u32 _sideIndex, StringID::StringIdValueType &_gameMaterialID)
    {
        f32 totalHeight = (_sideIndex==0)?0.f:0.2f;
        const u32 layerCount = m_layerInfos.size();

        _gameMaterialID = _config->m_gameMaterial.getStringID().GetValue();

        for (u8 layerIndex=0; layerIndex<layerCount; layerIndex++)
        {
            f32 height = _edge.getHeightLevelSet(layerIndex)[_indexForCollision].y();
            const u8 col = getGameMaterialIndexAt_InString(&_edge, _edgeIndex, _indexForCollision);
            const FluidFriseLayer &layerInfos = m_layerInfos[layerIndex];

            if (col==layerIndex)
            {
                bbool isTooEroded = bfalse;
                bbool isABitEroded = bfalse;

                //Check erosion at this point
                if (layerInfos.m_erosion)
                {
                    const i32 erosionStep = (i32)_config->m_fluid.m_polylineUnityMult;
                    const i32 erosion = (i32)layerInfos.m_erosion*erosionStep+(erosionStep>>1);

                    u8 otherCol = getGameMaterialIndexAt_InString(&_edge, _edgeIndex, _indexForCollision-erosion);
                    if (otherCol!=col)
                    {
                        isABitEroded = btrue;
                    }
                    else
                    {
                        u8 otherCol = getGameMaterialIndexAt_InString(&_edge, _edgeIndex, _indexForCollision+erosion);
                        if (otherCol!=col)
                        {
                            isABitEroded = btrue;
                        }
                    }


                    {
                        for (i32 erosionDistance=-erosion+erosionStep; erosionDistance<0; erosionDistance+=erosionStep) 
                        {
                            u8 otherCol = getGameMaterialIndexAt_InString(&_edge, _edgeIndex, _indexForCollision+erosionDistance);
                            if (otherCol!=col)
                            {
                                isTooEroded = btrue;
                                break;
                            }
                        }
                        if (!isTooEroded)
                        {
                            for (i32 erosionDistance=erosionStep; erosionDistance<=(erosion-erosionStep); erosionDistance+=erosionStep) 
                            {
                                u8 otherCol = getGameMaterialIndexAt_InString(&_edge, _edgeIndex, _indexForCollision+erosionDistance);
                                if (otherCol!=col)
                                {
                                    isTooEroded = btrue;
                                    break;
                                }
                            }
                        }
                    }
                }
                
                //compute collision height and game material
                if (!(isABitEroded || isTooEroded))
                {
                    if (layerInfos.m_collisionHeight!=FluidFriseLayer::InvalidCollisionHeight) //special case for "Red" layer : it generates a collision height
                    {
                        height = layerInfos.m_collisionHeight * layerInfos.m_collisionHeightMultiplier;
                    }
                }
                else
                {
                    if (layerInfos.m_hasForcedHeightWhenNotColored)
                    {                    
                        height = layerInfos.m_forcedHeightWhenNotColored;
                    }
                }

                if (!isTooEroded)
                {
                    if (height>=layerInfos.m_threshold)
                    {
                        const StringID &layerGameMaterial = m_layerInfos[layerIndex].m_gameMaterialIds[_sideIndex];
                        if (layerGameMaterial.isValid())
                        {
                            _gameMaterialID = layerGameMaterial.GetValue();
                        }
                    }
                }
            }
            else
            {
                if (layerInfos.m_hasForcedHeightWhenNotColored)
                {                    
                    height = layerInfos.m_forcedHeightWhenNotColored;
                }
            }

            if (_sideIndex==1)
            {
                if (layerInfos.m_hasBacksideInversion) ///////TODO : REMOVE HARDCODED BEHAVIOR
                {
                    if (height>=0)
                    {
                        height=-height;
                    }
                    else
                    {
                        height = f32_Abs(height);
                    }
                }
                else
                {
                    height = f32_Abs(height);
                }
            }

            totalHeight += height;
        }
        return totalHeight;
    }
    

    void DataFluid::updateCollisionPolyLine_InString(Frise *frieze, const FriseConfig *config, const Transform2d& _xf)
    {
        // update polyline collision
        const u32 edgeCount = m_edgeFluidList.size();
        for (u32 edgeIndex=0; edgeIndex<edgeCount; edgeIndex++)
        {
            EdgeFluid &edge = m_edgeFluidList[edgeIndex];
            const u32 collisionProcessCount = config->m_fluid.m_sideCount;

            for (u32 sideIndex = 0; sideIndex<collisionProcessCount; sideIndex++)
            {
                EdgeProcessData *procData = &edge.m_collisionProcesses[sideIndex];
                const ITF_VECTOR<PolyLine*>* pCollisionData = frieze->getCollisionData_WorldSpace();

                if ( procData->m_id >= 0 && pCollisionData && (*pCollisionData)[procData->m_id] )
                {
                    PolyLine        *poly       = (*pCollisionData)[procData->m_id];
                    i32             indexForCollision, stepForCollisionIndex;
                    bbool           somethingChanged = btrue; /////////////

                    u32 lastPointIndex = poly->getPosCount() - 1;

                    u32 pointIndex;                    

                    Vec2d posCur = procData->m_posStart;
                    if (sideIndex==1) //back side
                    {
                        indexForCollision = ((i32)lastPointIndex-1)*config->m_fluid.m_polylineUnityMult;
                        stepForCollisionIndex = -(i32)config->m_fluid.m_polylineUnityMult;
                        posCur = procData->m_posEnd - ((f32)lastPointIndex-1)*procData->m_posOffset;
                        pointIndex = 1;
                    }
                    else
                    {
                        indexForCollision = 0;
                        pointIndex = 0;
                        stepForCollisionIndex = config->m_fluid.m_polylineUnityMult;
                    }

                    for (u32 i=0; i<lastPointIndex; i++, (posCur+=procData->m_posOffset), (indexForCollision+= stepForCollisionIndex), (pointIndex++))
                    {
                        Vec2d Pos = posCur;
                        
                        StringID::StringIdValueType gameMaterialID;
                        f32 totalHeight = computeFluidHeight_InString(config, edge, edgeIndex, indexForCollision, sideIndex, gameMaterialID);

                        Pos += procData->m_normal*totalHeight;

                        const Vec2d oldPos = poly->getPosAt(pointIndex);
                        const Vec2d newPos = _xf.transformPos( Pos );
                        poly->setPosAt( newPos, pointIndex );

                        if (!oldPos.IsEqual( newPos, 0.001f))
                        {
                            somethingChanged = btrue;
                        }
                        poly->setGameMaterialAt(gameMaterialID,pointIndex);
                    }

                    StringID::StringIdValueType finalGameMaterialID;
                    const f32 finalHeight = computeFluidHeight_InString(config, edge, edgeIndex, edge.m_lastIndex, sideIndex, finalGameMaterialID);                    

                    if (somethingChanged)
                    {
                        const u32 boundActorCount = m_contactActorsInfo.size();
                        for (u32 boundActorIndex = 0; boundActorIndex<boundActorCount; boundActorIndex++)
                        {
                            ObjectRef objectRef(m_contactActorsInfo[boundActorIndex].m_ref);
                            BaseObject *object = objectRef.getObject();
                            if (object)
                            {
                                Actor *actor = DYNAMIC_CAST(object,Actor);
                                if (actor)
                                {
                                    EventFluidCollisionPolylineChanged fluidEvent;
                                    fluidEvent.setSender(m_frieze);
                                    actor->onEvent(&fluidEvent);
                                }
                            }
                        }
                    }

                    if (sideIndex==0)
                    {
                        const Vec2d Pos = procData->m_posEnd + procData->m_normal*finalHeight;
                        const u32 finalPointIndex = lastPointIndex;
                        poly->setPosAt( _xf.transformPos( Pos ), finalPointIndex );
                    }
                    else
                    {
                        const Vec2d Pos = procData->m_posStart + procData->m_normal*finalHeight;
                        u32 finalPointIndex = 0;
                        poly->setPosAt( _xf.transformPos( Pos ), finalPointIndex );
                    }

                    PolyLine *previous = poly->getPrevious();
                    if (previous)
                    {
                        if (previous->getPosCount() && poly->getPosCount())
                        {
                            const Vec2d &endOfPrevious = previous->getPosAt(previous->getPosCount()-1);
                            poly->setPosAt(endOfPrevious,0);
                        }
                    }

                    poly->forceRecomputeData();
                }
            }
        }
    }

    void DataFluid::updateCollisionPolyLine_InFluid(Frise *frieze, const FriseConfig *config, const Transform2d& _xf)
    {
        const ITF_VECTOR<PolyLine*>* pCollisionData = frieze->getCollisionData_WorldSpace();
        if ( !pCollisionData || m_layerInfos.size() == 0 || !config->m_fluid.m_polylineReaction )
            return;        

        // update polyline collision
        for (ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
            edgeIter < m_edgeFluidList.end(); ++edgeIter)
        {
            EdgeFluid &edge = *edgeIter;
            EdgeProcessData *procData   = &(edge.m_collisionProcesses[0]);

            if  ( procData->m_id >= 0 && (*pCollisionData)[procData->m_id] )
            {         
                PolyLine        *poly       = (*pCollisionData)[procData->m_id];
                const u32 polyCount         = poly->getPosCount() - 1;

                const u32 layerCount = config->m_fluid.m_layerCount;
                u32 indexForCollision = 0;

                Vec2d posCurLocal;
                Vec2d posDown = edge.m_pos[0];
                Vec2d posUp = edge.m_pos[1];

                const f32 ratio = 1.f / (f32)polyCount;
                const Vec2d posOffsetDown  = (edge.m_pos[2] - posDown) * ratio;
                const Vec2d posOffsetUp  = (edge.m_pos[3] - posUp) * ratio;

                for (u32 i=0; i<polyCount; i++, posDown += posOffsetDown, posUp += posOffsetUp, indexForCollision += config->m_fluid.m_polylineUnityMult )
                {                    
                    StringID::StringIdValueType gameMaterialID = config->m_gameMaterial.getStringID().GetValue();
                    f32 finalHeight = config->m_collisionFrieze.m_offset;

                    for (u32 layerIndex=0; layerIndex<layerCount; layerIndex++)
                    {
                        const Vec2d &height = edge.getHeightLevelBlended(layerIndex)[indexForCollision];
                        finalHeight += height.y();

                        const f32 col = edge.m_fluidGameMaterialMap[indexForCollision];
                        const FluidFriseLayer &layerInfos = m_layerInfos[layerIndex];

                        if ((col>=(f32)layerIndex) && (height.y()>=layerInfos.m_threshold))
                        {
                            const StringID &layerGameMaterial = layerInfos.m_gameMaterialIds[0];
                            if (layerGameMaterial.isValid())
                            {
                                gameMaterialID = layerGameMaterial.GetValue();
                            }
                        }
                    }

                    Vec2d::Lerp(&posCurLocal,  &posDown, &posUp, finalHeight);
                    poly->setPosAt( _xf.transformPos( posCurLocal ), i );
                    poly->setGameMaterialAt(gameMaterialID,i);
                }

                // last point
                f32 finalHeight = config->m_collisionFrieze.m_offset;
                for (u32 layerIndex=0; layerIndex<layerCount; layerIndex++)
                {
                    finalHeight += edge.getHeightLevelBlended(layerIndex)[edge.m_lastIndex].y();
                }

                Vec2d::Lerp(&posCurLocal, &edge.m_pos[2], &edge.m_pos[3], finalHeight);
                poly->setPosAt( _xf.transformPos( posCurLocal ), polyCount );

                const PolyLine *previous = poly->getPrevious();
                if (previous)
                {
                    if (previous->getPosCount() && poly->getPosCount())
                    {
                        const Vec2d &endOfPrevious = previous->getPosAt(previous->getPosCount()-1);
                        poly->setPosAt(endOfPrevious,0);
                    }
                }

                poly->forceRecomputeData();
            }
        }
    }

    void DataFluid::updateVisualPolyLine(Frise *frieze, const FriseConfig *config, const Transform2d& _xf )
    {
        // update polyline visual
        PolyPointList* visualPolyLocal = &frieze->getVisualPolyPointListLocal();
        if ( visualPolyLocal->getPosCount() > 0 )
        {
            for (ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
                edgeIter < m_edgeFluidList.end(); ++edgeIter)
            {
                EdgeProcessData *procData   = &((*edgeIter).m_visualProcess);
                Vec2d           posCur      = procData->m_posStart;
                u32 layerCount = config->m_fluid.m_layerCount;

                for (i32 i=procData->m_indexStart; i<=procData->m_indexEnd; i++, posCur += procData->m_posOffset)
                {
                    f32 finalHeight = 0;
                    for (u32 layerIndex=0; layerIndex<layerCount; layerIndex++)
                    {
                        finalHeight += (*edgeIter).getHeightLevelBlended(layerIndex)[(i-procData->m_indexStart)*config->m_fluid.m_polylineUnityMult].y();
                    }
                    Vec2d Pos = posCur + procData->m_normal*finalHeight;
                    visualPolyLocal->setPosAt( Pos, i );
                }
            }

            frieze->updateVisualPolyline( _xf );
        }
    }

    void DataFluid::updateUV(f32 _elapsed)
    {
        if (fabs(_elapsed) < MTH_EPSILON)
            return;

        EdgeFluid *curEdgeFluid;
        for (ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
            edgeIter != m_edgeFluidList.end(); ++edgeIter)
        {
            curEdgeFluid = &(*edgeIter);
            if (!curEdgeFluid->m_uvAnimTrans.IsEqual(Vec2d::Zero))
            {
                for (u32 i=0; i<4; i++)
                {
                    curEdgeFluid->m_uv[i] += curEdgeFluid->m_uvAnimTrans * _elapsed;
                }
            }
        }
    }

    void DataFluid::processFx(u32 _edgeIdx ,u32 _pointIdx, FluidFxType _fxType)
    {
        if (!m_fxActor || _edgeIdx == U32_INVALID || _pointIdx == U32_INVALID)
            return;

        Frise * frise = getFrieze();
        if (!frise)
            return;

        // search position
        EdgeFluid & edgeFluid = m_edgeFluidList[_edgeIdx];
        EdgeFluidLevel & edgeFluidLevel = m_edgeFluidListLevels[_edgeIdx];

        Vec2d p0_world = edgeFluidLevel.m_xf.transformPos( edgeFluid.m_pos[0] );
        Vec2d p1_world = edgeFluidLevel.m_xf.transformPos( edgeFluid.m_pos[1] );
        Vec2d p2_world = edgeFluidLevel.m_xf.transformPos( edgeFluid.m_pos[2] );
        Vec2d p3_world = edgeFluidLevel.m_xf.transformPos( edgeFluid.m_pos[3] );
        f32   ratio    = f32(_pointIdx) / f32(edgeFluid.m_lastIndex);

        Vec2d pBase = p0_world + (p2_world - p0_world) * ratio;
        Vec2d pUp   = p1_world + (p3_world - p1_world) * ratio;
        Vec2d pos   = pBase + (pUp - pBase) * (edgeFluid.getHeightLevelBlended(0)[_pointIdx].y() + edgeFluidLevel.m_data.m_heightPos);

        EventPlayFX eventfx;
        eventfx.setPos(pos.to3d(frise->getDepth()));
        switch (_fxType)
        {
        case FluidFxType_Enter:
            eventfx.setFXName(StringID("onEnter"));
            break;
        case FluidFxType_Leave:
            eventfx.setFXName(StringID("onLeave"));
            break;
        case FluidFxType_Swim:
            eventfx.setFXName(StringID("onSwim"));
            break;
        case FluidFxType_Plouf:
            eventfx.setFXName(StringID("onPlouf"));
            break;
        default:
            return;
        }

        m_fxActor->onEvent(&eventfx);
    }


    void DataFluid::copyFromSetToBlend( const FriseConfig *config )
    {
        if (!config->m_fluid.m_blendFactor)
            return;

        f32 blendFactor = config->m_fluid.m_blendFactor;
        f32 blendInv    = 1.f - blendFactor;
        EdgeFluid *curEdgeFluid;

        for (u8 layerIndex=0; layerIndex<config->m_fluid.m_layerCount; layerIndex++)
        {
            for (ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();
                edgeIter != m_edgeFluidList.end(); ++edgeIter)
            {
                curEdgeFluid = &(*edgeIter);
                const u32   steps = curEdgeFluid->m_lastIndex;
                FluidHeightMap &curEdgeFluidHeightMap = curEdgeFluid->m_heightLevelSet[layerIndex];
                FluidHeightMap &curEdgeFluidHeightMapBlended = curEdgeFluid->m_heightLevelBlended[layerIndex];
                if (curEdgeFluidHeightMap.size())
                {
                    for (u32 i=0; i<=steps; i++)
                    {
                        curEdgeFluidHeightMapBlended[i] = blendFactor * curEdgeFluidHeightMapBlended[i] + blendInv * curEdgeFluidHeightMap[i];
                    }
                }
            }
        }

    }


    void DataFluid::ProcessEdgeFluidList_InFluid(f32 _elapsed)
    {
        // This part will be done externally
        // to Add
        if (m_edgeFluidList.size() == 0)
            return;

        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        const FriseConfig * config = frieze->getConfig();
        if (!config)
            return;

        const Transform2d xf( frieze->get2DPos(), frieze->getAngle(), frieze->getScale(), frieze->getIsFlipped() );

        updateUV(_elapsed);

        checkElevationMapInitialization(config);
        checkActorInfluence();
        handlePropagation(frieze, config);

        copyFromSetToBlend(config);

        updateCollisionPolyLine_InFluid(frieze, config, xf);
        updateVisualPolyLine(frieze, config, xf);
    }

    void DataFluid::computeEdgeFluidLevels(u32 _levelsFront, u32 _levelBack, bbool _cutUV)
    {
        ITF_ASSERT(_levelsFront + _levelBack < 31);
        f32 levelTab[32];
        u32 fullSize = (_levelsFront+_levelBack+1);
        u32 i=0;
        if (_levelBack != 0)
        {
            f32 step    = 1.f/(_levelBack+1);
            f32 val     = 1.f - step*_levelBack;
            for (; i<_levelBack; i++, val+=step)
            {
                levelTab[i] = val;
            }
        }
        levelTab[i++] = 1.f;
        if (_levelsFront != 0)
        {
            f32 step    = 1.f/(_levelsFront+1);
            f32 val     = 1.f - step;
            for (; i<fullSize; i++, val-=step)
            {
                levelTab[i] = val;
            }
        }
         
        m_edgeFluidListLevels.clear();
        m_edgeFluidListLevels.reserve(m_edgeFluidList.size()*fullSize);
        
        const u32 edgeFluidCount = m_edgeFluidList.size();
        for ( u32 id = 0; id < edgeFluidCount; id++)
        {         
            EdgeFluid& edgeFluid = m_edgeFluidList[id];

            for (u32 i=0; i<fullSize; i++)
            {
                EdgeFluidLevel edgeFluidLevel;
                edgeFluidLevel.m_edgeFluid      = &edgeFluid;
                edgeFluidLevel.m_idEdgeFluid    = id;
                edgeFluidLevel.m_data.m_level   = levelTab[i];
                edgeFluidLevel.m_data.m_pos     = f32((i32)i-(i32)_levelBack)/fullSize;
                edgeFluidLevel.m_data.m_cutUV   = _cutUV;
                edgeFluidLevel.m_layerInfos     = m_layerInfos;

                m_edgeFluidListLevels.push_back(edgeFluidLevel);
            }
        }
    }

    void DataFluid::addEdgeFluidLevelPerpendicular(i32 _way, f32 _heightScale, f32 _heightScaleTotal, f32 _heightPos, f32 _posZ, f32 _z)
    {
        ITF_ASSERT(_way == 1 || _way == -1);

        const u32 edgeFluidCount = m_edgeFluidList.size();
        for ( u32 id=0; id<edgeFluidCount; id++)
        {         
            EdgeFluid& edgeFluid = m_edgeFluidList[id];

            EdgeFluidLevel edgeFluidLevel;
            edgeFluidLevel.m_edgeFluid                  = &edgeFluid;
            edgeFluidLevel.m_idEdgeFluid                = id;
            edgeFluidLevel.m_data.m_pos                 = _z;
            edgeFluidLevel.m_data.m_cutUV               = btrue;
            edgeFluidLevel.m_data.m_way                 = _way;
            edgeFluidLevel.m_data.m_heightScale         = _heightScale;
            edgeFluidLevel.m_data.m_heightScaleTotal    = _heightScaleTotal;
            edgeFluidLevel.m_data.m_heightPos           = _heightPos;
            edgeFluidLevel.m_data.m_posZ                = _posZ;
            edgeFluidLevel.m_layerInfos                 = m_layerInfos;

            m_edgeFluidListLevels.push_back(edgeFluidLevel);
        }
    }

    void DataFluid::computeAABB( AABB& _aabb )
    {
        Frise* frieze = getFrieze();
        if (!frieze)
            return;

        const FriseConfig * config = frieze->getConfig();
        if (!config)
            return;

//         f32 extrudeCoeffX = config->m_fluid.m_perpendicularBackScale *0.25f;
//         f32 extrudeCoeffY = config->m_fluid.m_perpendicularBackScale *0.16f; 

        if ( m_edgeFluidList.size() )
        {
            // init mesh fluid aabb            
            _aabb.setMinAndMax( m_edgeFluidList[0].m_pos[0] );            

            ITF_VECTOR<EdgeFluid>::iterator edgeIter = m_edgeFluidList.begin();

            for (; edgeIter != m_edgeFluidList.end();edgeIter++)
            {
                EdgeFluid &edgeFluid = *edgeIter;
                edgeFluid.m_localAABB = AABB(edgeFluid.m_pos[0]);
                for (u32 i=1; i<4; i++)
                    edgeFluid.m_localAABB.grow(edgeFluid.m_pos[i]);

                _aabb.grow(edgeFluid.m_localAABB);   

                // add wave marge
                Vec2d dir = edgeFluid.m_pos[1]-edgeFluid.m_pos[0];
                edgeFluid.m_localAABB.grow(edgeFluid.m_pos[0] + dir*config->m_fluid.m_targetMinHeight);
                edgeFluid.m_localAABB.grow(edgeFluid.m_pos[1] + dir*config->m_fluid.m_targetMaxHeight);
                      dir = edgeFluid.m_pos[3]-edgeFluid.m_pos[2];
                edgeFluid.m_localAABB.grow(edgeFluid.m_pos[2] + dir*config->m_fluid.m_targetMinHeight);
                edgeFluid.m_localAABB.grow(edgeFluid.m_pos[3] + dir*config->m_fluid.m_targetMaxHeight);                     
            }
        }
    }

    void DataFluid::spawnFxActor(const Path & _path)
    {
        Frise * frieze = getFrieze();
        if (!frieze || _path.isEmpty())
            return;

        Scene * pScene = frieze->getScene();
        SpawnActorInfo info;
        info.setStartPos(frieze->getPos());
        info.setSceneRef(pScene ? pScene->getRef() : ITF_INVALID_OBJREF);
        info.setPath(_path);
        info.setDoInit(btrue);
        info.setParentResourceContainer(frieze->getResourceContainer());

        m_fxActor       = WORLD_MANAGER->spawnActor(info);
        m_fxBindDone    = bfalse;
    }

    void DataFluid::bindFxActor()
    {
        if (m_fxBindDone)
            return;

        Frise * frieze = getFrieze();
        if (!frieze || !m_fxActor)
        {
            m_fxBindDone = btrue;
            return;
        }

        if (m_fxActor->getWorldUpdateElement())
        {
            WORLD_MANAGER->getWorldUpdate().bindElement(m_fxActor->getWorldUpdateElement(), frieze->getWorldUpdateElement());
            m_fxBindDone = btrue;
        }
    }

    void DataFluid::clear()
    { 
        m_edgeFluidList.clear();
        m_contactActorsInfo.clear();
        m_edgeFluidListLevels.clear();

        // also destroy spawned actor if exist
        if (m_fxActor)
        {
            m_fxActor->requestDestruction();
            m_fxActor = NULL;
        }
    }



    void FullFluidToDraw::directDraw(const RenderPassContext & _rdrPassCtxt, GFXAdapter * _gfxDeviceAdapter, f32 _Z, f32 _ZWorld, f32 _ZAbs) 
    { 
        if (!m_fluidList.size())
            return;

        PRF_M_SCOPE(drawFullFluid);

        _gfxDeviceAdapter->setGFXPrimitiveParameters(getCommonParam(), _rdrPassCtxt);
        PrimitiveContext primCtx;
        primCtx.setRenderPassContext((RenderPassContext*)&_rdrPassCtxt)
            .setPrimitiveParam(getCommonParam())
            .setCamZ(_Z,_ZWorld, _ZAbs);

        u32 hdiv = MESH_DEFAULT_HDIV;
        u32 vdiv = MESH_DEFAULT_VDIV;

        ITF_IndexBuffer *  indexBuffer  = _gfxDeviceAdapter->getPatchGridIndexBuffer(hdiv, vdiv);
        ITF_VertexBuffer * vertexBuffer = _gfxDeviceAdapter->getPatchGridVertexBuffer(hdiv, vdiv);
        ITF_ASSERT(indexBuffer && vertexBuffer);

        // set Vertex format
        _gfxDeviceAdapter->setIndexBuffer(indexBuffer);
        _gfxDeviceAdapter->setVertexBuffer(vertexBuffer);
        ITF_ASSERT(vertexBuffer->m_vertexFormat == VertexFormat_PT);

        _gfxDeviceAdapter->drawFluid(primCtx, m_fluidList, m_mode);
    }

} // namespace ITF
