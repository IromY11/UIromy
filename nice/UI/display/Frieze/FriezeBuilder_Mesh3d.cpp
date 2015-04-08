#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_MESH3DRESOURCE_H_
#include "engine/display/Mesh3DResource.h"
#endif //_ITF_MESH3DRESOURCE_H_

#ifndef _ITF_MESH3D_H_
#include    "engine/display/Mesh3D.h"
#endif // _ITF_MESH3D_H_


#ifdef ITF_SUPPORT_COOKING
#ifndef _ITF_MESH3DCOOKER_H_
#include    "tools/plugins/CookerPlugin/Mesh3DCooker.h"
#endif // _ITF_MESH3DCOOKER_H_
#endif

namespace ITF
{


        /*
        v1...v3
        .    .
        .    .
        v0...v2
        */
        

    void computeXStartEnd(f32 & _uvInTextureXStart, f32 & _uvInTextureXEnd, f32 tileSize, Vec2d * _uvTab)
    {
        if (_uvInTextureXStart > 1.f - MTH_EPSILON)
            _uvInTextureXStart -= 1.f;

        _uvInTextureXEnd = _uvInTextureXStart + tileSize;

        if (_uvInTextureXEnd < -MTH_EPSILON)
        {
            _uvInTextureXStart += 1.f;
            _uvInTextureXEnd   += 1.f;
        }

        _uvTab[0].x() = _uvTab[1].x() = _uvInTextureXStart;
        _uvTab[2].x() = _uvTab[3].x() = _uvInTextureXEnd;
    }

    bbool Frise::buildVB_Static_EdgeRunMesh(const FriseConfig* config, ITF_VECTOR<edgeFrieze>& _edgeList, const edgeRun& _edgeRun, IndexList& _indexList, u32 _vtxColorUp, u32 _vtxColorDown, u32 _vtxColorUpExtremity, u32 _vtxColorDownExtremity, u16& _indexVtxUp, u16& _indexVtxDown, Vec2d& _uvUp, Vec2d& _uvDown, f32 _uvXsign)
    {
        if (!config->m_mesh3d.m_mesh3dList.size() || _edgeRun.m_edgeCount == 0)
            return bfalse;


        Vec2d prevUvUp     = _uvUp;
        Vec2d prevUvDown   = _uvDown;
        Vec2d posTab[4];
        Vec2d uvTab[4];

        f32 tileSize          = config->m_mesh3d.m_texureTileSize * _uvXsign;
        f32 uvInTextureXStart = _uvUp.x(); // must be "ok" 
        f32 uvInTextureXEnd;

        uvTab[0].y() = uvTab[2].y() = _uvDown.y();
        uvTab[1].y() = uvTab[3].y() = _uvUp.y();
        computeXStartEnd(uvInTextureXStart, uvInTextureXEnd, tileSize, uvTab);

        // set Default value to tab[2] and [3]
        posTab[0] = _edgeList[_edgeRun.m_idEdgeStart].m_points[0];
        posTab[1] = _edgeList[_edgeRun.m_idEdgeStart].m_points[1];
        

        f32     cutPos          = -1;
        u32     meshVertexStart = 0;

        const Mesh3dData * meshData = NULL;
        ITF_VECTOR<Mesh3dInfo> meshList;
        u32 index = 0;

        // compute nb Elements to render
        Vec2d uvEndUp     = _uvUp;
        Vec2d uvEndDown   = _uvDown;
        for (u32 edgeCount = 0; edgeCount < _edgeRun.m_edgeCount; edgeCount++ )
        {
            u32 idEdge   = ( _edgeRun.m_idEdgeStart + edgeCount ) % m_pRecomputeData->m_edgeListCount;
            updateUvs(_edgeRun, &_edgeList[idEdge], uvEndUp, uvEndDown, _uvXsign, 0.f, 0.f);
        }

        i32      remainingMeshes = i32((uvEndUp.x() - _uvUp.x()) / tileSize + 0.5f);
        StringID family;
        i32      familyIndex;

        for (u32 edgeCount = 0; edgeCount < _edgeRun.m_edgeCount; edgeCount++ )
        {
            u32             idEdge   = ( _edgeRun.m_idEdgeStart + edgeCount ) % m_pRecomputeData->m_edgeListCount;
            edgeFrieze *    currEdge = &_edgeList[idEdge];

            if (currEdge->m_snap)
            {
                if (!edgeCount)
                    continue;

                posTab[2] = currEdge->m_points[0];
                posTab[3] = currEdge->m_points[1];

                buildVB_Static_QuadMeshAlone(posTab, uvTab);

                posTab[0] = posTab[2];
                posTab[1] = posTab[3];
                
                continue;
            }
            
            Vec2d currentUvUp   = prevUvUp;
            Vec2d currentUvDown = prevUvDown;
            
            updateUvs(_edgeRun, currEdge, currentUvUp, currentUvDown, _uvXsign, 0.f, 0.f);

            f32 ceilUVPrevX = f32_Ceil(prevUvUp.x()/tileSize - MTH_EPSILON) * tileSize;

            // continue previous Mesh
            if (f32_Abs(ceilUVPrevX - prevUvUp.x()) > MTH_EPSILON)
            {
                if (ceilUVPrevX * _uvXsign > currentUvUp.x() * _uvXsign + MTH_EPSILON)
                {
                    if (getUVMeshes( (uvTab[0] + uvTab[3]) * 0.5f, meshData, meshList))
                    {
                        // use previous data
                        posTab[2] = currEdge->m_points[2];
                        posTab[3] = currEdge->m_points[3];

                        f32 newCutPos = cutPos + (currentUvUp.x() - prevUvUp.x())/tileSize;
                        if (newCutPos > cutPos)
                            fillMeshVertex(posTab, uvTab, meshData, meshList, index, meshVertexStart, cutPos, newCutPos);
                        else
                            fillMeshVertex(posTab, uvTab, meshData, meshList, index, meshVertexStart, newCutPos, cutPos);

                        posTab[0] = posTab[2];
                        posTab[1] = posTab[3];

                        prevUvUp      = currentUvUp;
                        prevUvDown    = currentUvDown;
                        cutPos        = newCutPos;
                    }
                    continue;
                } else
                {
                    if (getUVMeshes( (uvTab[0] + uvTab[3]) * 0.5f, meshData, meshList))
                    {
                        // use previous data
                        f32 startPercent = (ceilUVPrevX - prevUvUp.x()) / (currentUvUp.x() - prevUvUp.x());

                        posTab[2] = currEdge->m_points[0] * (1.f-startPercent) + currEdge->m_points[2] * startPercent;
                        posTab[3] = currEdge->m_points[1] * (1.f-startPercent) + currEdge->m_points[3] * startPercent;

                        fillMeshVertex(posTab, uvTab, meshData, meshList, index, meshVertexStart, cutPos*_uvXsign, cutPos*_uvXsign*-1);

                        posTab[0] = posTab[2];
                        posTab[1] = posTab[3];

                    }
                    uvInTextureXStart = uvInTextureXStart + tileSize;
                    computeXStartEnd(uvInTextureXStart, uvInTextureXEnd, tileSize, uvTab);
                }
            }

            f32 ceilUVCurrX = ceilUVPrevX + tileSize;
            while (ceilUVCurrX * _uvXsign < currentUvUp.x() * _uvXsign + MTH_EPSILON)
            {
                f32 endPercent   = (ceilUVCurrX - prevUvUp.x()) / (currentUvUp.x() - prevUvUp.x());

                posTab[2] = currEdge->m_points[0] * (1.f-endPercent) + currEdge->m_points[2] * endPercent;
                posTab[3] = currEdge->m_points[1] * (1.f-endPercent) + currEdge->m_points[3] * endPercent;

                buildVB_Static_QuadMesh(posTab, uvTab, family, familyIndex, remainingMeshes);

                posTab[0] = posTab[2];
                posTab[1] = posTab[3];

                ceilUVPrevX = ceilUVCurrX;
                ceilUVCurrX = ceilUVCurrX + tileSize;
                
                uvInTextureXStart = uvInTextureXStart + tileSize;
                computeXStartEnd(uvInTextureXStart, uvInTextureXEnd, tileSize, uvTab);
            }


            if (f32_Abs(currentUvUp.x() - ceilUVPrevX) > MTH_EPSILON)
            {
                if (getUVMeshes( (uvTab[0] + uvTab[3]) * 0.5f, meshData, meshList))
                {
                    meshVertexStart = m_pRecomputeData->m_anim ? m_meshBuildData->m_animVertexList.size() : m_meshBuildData->m_staticVertexList.size();
                    
                    index = getMeshIndex(meshData, family, familyIndex, remainingMeshes);
                    ITF_VECTOR<IndexList> & vectIndexList = m_pRecomputeData->m_anim ? m_meshBuildData->m_animIndexList : m_meshBuildData->m_staticIndexList;
                    u32 IBIndex = Min( meshData->m_meshList[index].m_textureIndex, vectIndexList.size() - 1);
                    IndexList& indexList = vectIndexList[IBIndex];
                    InitMeshData( indexList, meshList, index, meshVertexStart);

                    // use previous data
                    posTab[2] = currEdge->m_points[2];
                    posTab[3] = currEdge->m_points[3];

                    cutPos = (currentUvUp.x() - ceilUVPrevX)/tileSize;
                    fillMeshVertex(posTab, uvTab, meshData, meshList, index, meshVertexStart, cutPos*_uvXsign*-1.f, cutPos*_uvXsign);

                    posTab[0] = posTab[2];
                    posTab[1] = posTab[3];
                }
            }

            prevUvUp      = currentUvUp;
            prevUvDown    = currentUvDown;
        }

        return btrue;
    }

    bool familySort(const pair<StringID, u32> & _a, const pair<StringID, u32> & _b)
    {
        return _a.second > _b.second;
    }

    i32 Mesh3dData::getMeshIndex( const StringID & _family, i32 _familyIndex ) const
    {
        for (u32 index = 0; index < m_meshList.size(); index++)
        {
            const Mesh3dDataElement & elmt = m_meshList[index];
            if (elmt.m_family == _family && elmt.m_familyIndex == _familyIndex)
            {
                return index;
            }
        }
        return -1;
    }
    
    u32 Mesh3dData::getNbElmentsInFamily(const StringID & _family) const
    {
        u32 nbElemnts = 0;
        for (u32 index = 0; index < m_meshList.size(); index++)
        {
            const Mesh3dDataElement & elmt = m_meshList[index];
            if (elmt.m_family == _family)
                nbElemnts++;
        }
        return nbElemnts;
    }



    u32 Frise::getMeshIndex( const Mesh3dData * _meshData, StringID & _family, i32 & _lastIndexInFamily, i32 & _remainingMeshes)
    {
        u32 nbMeshes = (u32)_remainingMeshes;
        _remainingMeshes--;

        i32 manualPosIndex = m_pRecomputeData->m_meshPosition < m_meshManualySet.size() ?  m_meshManualySet[m_pRecomputeData->m_meshPosition] : -1;
        m_pRecomputeData->m_meshPosition++;

        if (manualPosIndex >= 0 &&
            (u32)manualPosIndex < _meshData->m_meshList.size())
        {
            const Mesh3dDataElement & mesh3dData = _meshData->m_meshList[manualPosIndex];
            if (!mesh3dData.m_family.isValid())
            {
                _family.invalidate();
                return manualPosIndex;
            }

            // select element must be first element of a family
            if (mesh3dData.m_familyIndex == 0)
            {
                u32 nbElements = 0;
                for (u32 index = 0; index < _meshData->m_meshList.size(); index++)
                {
                    const Mesh3dDataElement & elmt = _meshData->m_meshList[index];
                    if (elmt.m_family == mesh3dData.m_family)
                        nbElements++;
                }
                if (nbElements <= nbMeshes)
                {
                    _family            = mesh3dData.m_family;
                    _lastIndexInFamily = 0;
                    return manualPosIndex; 
                }
            }
        }

        if (_meshData->m_meshList.size() <= 1)
        {
            _family.invalidate();
            return 0;
        }

        if (_family.isValid())
        {
            i32 newFamilyIndex = _lastIndexInFamily + 1;
            i32 index = _meshData->getMeshIndex( _family, newFamilyIndex );
            if (index >= 0)
            {
                _lastIndexInFamily = newFamilyIndex;
                return (u32)index;
            }
        }

        // need new family
        ITF_MAP<StringID, u32> familyNb;
        ITF_VECTOR<u32>        orphelineIndex;

        for (u32 index = 0; index < _meshData->m_meshList.size(); index++)
        {
            const Mesh3dDataElement & elmt = _meshData->m_meshList[index];
            if (elmt.m_family.isValid() )
            {
                ITF_MAP<StringID, u32>::iterator it = familyNb.find(elmt.m_family);
                if (it == familyNb.end())
                    familyNb[elmt.m_family] = 1;
                else
                    it->second++;
            } else
                orphelineIndex.push_back(index);
        }

        if (familyNb.empty())
        {
            _family.invalidate();
            return m_seeder.GetU32(_meshData->m_meshList.size() * 1024) / 1024;
        }

        ITF_VECTOR<pair<StringID, u32> > familyList;
        familyList.reserve(familyNb.size());
        for (ITF_MAP<StringID, u32>::iterator it = familyNb.begin();
            it != familyNb.end(); it++)
        {
            familyList.push_back(pair<StringID, u32>(it->first, it->second));
        }
        std::sort(familyList.begin(), familyList.end(), familySort);
        ITF_VECTOR<pair<StringID, u32> >::iterator familyBegin = familyList.end();
        ITF_VECTOR<pair<StringID, u32> >::iterator familyEnd   = familyList.end();

        i32 nbElmts = -1;
        for (ITF_VECTOR<pair<StringID, u32> >::iterator it = familyList.begin();
            it != familyList.end(); it++)
        {
            if (nbElmts < 0)
            {
                if (it->second <= nbMeshes)
                {
                    familyBegin = it;
                    nbElmts     = it->second;
                }
            } else
            {
                if (it->second < (u32)nbElmts)
                {
                    familyEnd   = it;
                    break;
                }
            }
        }

        if (familyBegin < familyEnd)
        {
            _family             = (familyBegin + m_seeder.GetU32(u32(familyEnd - familyBegin) * 1024) / 1024)->first;
            _lastIndexInFamily  = 0;
            i32 index = _meshData->getMeshIndex( _family, _lastIndexInFamily );
            ITF_ASSERT(index >= 0);
            if (index >= 0)
                return (u32)index;
        }

        _family.invalidate();
        if (orphelineIndex.size())
            return orphelineIndex[m_seeder.GetU32(orphelineIndex.size() * 1024) / 1024];

        return 0;
    }


    bbool Frise::getUVMeshes( const Vec2d & _uv, const Mesh3dData * & _meshData, ITF_VECTOR<Mesh3dInfo> & _meshList)
    {
        const FriseConfig * config = getConfig();
        for (ITF_VECTOR<Mesh3dData>::const_iterator meshIter = config->m_mesh3d.m_mesh3dList.begin();
            meshIter != config->m_mesh3d.m_mesh3dList.end(); ++meshIter)
        {
            const Mesh3dData & meshData = *meshIter;
            if (meshData.m_uvMin.x() <= _uv.x() &&
                meshData.m_uvMin.y() <= _uv.y() &&
                meshData.m_uvMax.x() >= _uv.x() &&
                meshData.m_uvMax.y() >= _uv.y())
            {
                _meshData = &meshData;
                break;
            }
        }

        if (!_meshData)
            return bfalse;

        return getMeshes(_meshData, _meshList);
    }
    
    Mesh3D * Frise::getMeshCooked3d(const Path & _path)
    {
#ifdef ITF_SUPPORT_COOKING
        ITF_MAP<StringID, Mesh3D*>::iterator meshIter = m_pRecomputeData->m_meshCookMap.find(_path.getStringID());
        if (meshIter != m_pRecomputeData->m_meshCookMap.end())
        {
            return meshIter->second;
        } 
        
        Mesh3D * mesh = Mesh3DCooker::readMesh3D(_path, Platform(), bfalse);
        if (!mesh)
            return bfalse;

        m_pRecomputeData->m_meshCookMap[_path.getStringID()] = mesh;
        return mesh;
#else //ITF_SUPPORT_COOKING
        return NULL;
#endif //ITF_SUPPORT_COOKING
    }

    bbool Frise::getMeshes( const Mesh3dData * _meshData, ITF_VECTOR<Mesh3dInfo> & _meshList)
    {
        u32 meshListSize = _meshData->m_meshList.size();
        if (meshListSize == 0)
            return bfalse;

        _meshList.resize(_meshData->m_meshList.size());
        for (u32 i=0; i<meshListSize; i++)
        {
            Mesh3dInfo & meshInfo = _meshList[i];

            meshInfo.m_main   = getMeshCooked3d(_meshData->m_meshList[i].m_meshPath);
            if (!meshInfo.m_main)
                return bfalse;

            meshInfo.m_dummy = getMeshCooked3d(_meshData->m_meshList[i].m_dummyPath);
            if (!meshInfo.m_dummy)
                continue;

            if (meshInfo.m_dummy->m_ListUniqueVertex.size() != 4)
                return bfalse;
        }
        
        return btrue;
    }

    void Frise::InitMeshData( IndexList& _indexList, const ITF_VECTOR<Mesh3dInfo> & _meshList, u32 _index, u32 _start)
    {
        Mesh3D * mainMesh = _meshList[_index].m_main;
        Mesh3D::Element element = mainMesh->m_ListElement[0];
        for (ITF_VECTOR<Mesh3D::Triangle>::const_iterator triIter = element.m_ListTriangles.begin();
            triIter != element.m_ListTriangles.end(); ++triIter)
        {
            const Mesh3D::Triangle & triangle = *triIter;
            _indexList.push_back((u16)(triangle.VBIndex[0] + _start));
            _indexList.push_back((u16)(triangle.VBIndex[1] + _start));
            _indexList.push_back((u16)(triangle.VBIndex[2] + _start));
        }

        if (m_pRecomputeData->m_anim)
        {
            ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;
            u32 vtxBegin = vtxList.size();
            vtxList.resize(vtxBegin + mainMesh->m_ListUniqueVertex.size());
            u32 vtxEnd   = vtxList.size();

            for (u32 i=vtxBegin; i<vtxEnd; i++)
                vtxList[i].setData(Vec3d::Zero, U32_INVALID, Vec2d::Zero);
        } else
        {
            ITF_VECTOR<VertexPCT>& vtxList = m_meshBuildData->m_staticVertexList;
            u32 vtxBegin = vtxList.size();
            vtxList.resize(vtxBegin + mainMesh->m_ListUniqueVertex.size());
            u32 vtxEnd   = vtxList.size();

            for (u32 i=vtxBegin; i<vtxEnd; i++)
                vtxList[i].setData(Vec3d::Zero, Vec2d::Zero, U32_INVALID);
        }
    }

    void Frise::fillMeshVertex(const Vec2d* _pos, const Vec2d* _uv, const Mesh3dData * _meshData, const ITF_VECTOR<Mesh3dInfo> & _meshList, u32 meshIndex, u32 _start, f32 _useUVLimitLeft /*= -1.f*/, f32 _useUVLimitRight /*= -1.f*/)
    {
        ITF_VECTOR<VertexPCT>  & vtxListPCT   = m_meshBuildData->m_staticVertexList;
        ITF_VECTOR<VertexPNC3T>& vtxListPNC3T = m_meshBuildData->m_animVertexList;

#ifdef ITF_SUPPORT_EDITOR
        bbool needPush = btrue;
        if (m_quadTab.size())
        {
            Mesd3dQuadPoints & quadPoints = m_quadTab.back();
            if (quadPoints.m_start == _start)
            {
                quadPoints.m_pTab[2] = _pos[3];
                quadPoints.m_pTab[3] = _pos[2];
                needPush = bfalse;
            }
        }
        if (needPush)
            m_quadTab.push_back(Mesd3dQuadPoints(_pos, _meshData, meshIndex, _start));
#endif //ITF_SUPPORT_EDITOR
        
        Mesh3D * mainMesh  = _meshList[meshIndex].m_main;
        Mesh3D * dummyMesh = _meshList[meshIndex].m_dummy;
        const FriseConfig * pConfig = getConfig();

        if (!dummyMesh)
        {
            // if no dummy mesh do not use pos / uv data
            
            u32 idx = _start;
            for (ITF_VECTOR<Mesh3D::UniqueVertex>::const_iterator vIter = mainMesh->m_ListUniqueVertex.begin();
                vIter != mainMesh->m_ListUniqueVertex.end(); ++vIter, ++idx)
            {
                const Mesh3D::UniqueVertex & uVtx = *vIter;
                if (m_pRecomputeData->m_anim)
                {
                    VertexPNC3T& vtx = vtxListPNC3T[ idx ];
                    vtx.setData(uVtx.Position, U32_INVALID, uVtx.UV);
                    setVtxAnim( pConfig->m_vtxAnim, vtx);
                    
                    f32 factor = Color::getRedFromU32(uVtx.color);
                    vtx.m_uv3.x() *= factor;
                    vtx.m_uv3.y() *= factor;
                }
                else
                {
                    vtxListPCT[idx].setData(uVtx.Position, uVtx.UV, U32_INVALID);
                }
            }
            return;
        }

        Vec2d dummyPt[4];
        for (u32 i=0; i<4; i++)
            dummyPt[i] = dummyMesh->m_ListUniqueVertex[i].Position.truncateTo2D();

        // get min max
        AABB dummyAABB(dummyPt[0]);
        for (i32 i=1; i<4; i++)
        {
            dummyAABB.grow(dummyPt[i]);
        }

        Vec2d InvTransform = Vec2d( f32_Inv(dummyAABB.getWidth()),
            f32_Inv(dummyAABB.getHeight()));

        u32 idx = _start;
        u32 p0 = 0, p1= 0, p2 = 0, p3 = 0;
        AABB uvAABB(_meshData->m_uvMin, _meshData->m_uvMax);
        for (u32 i=0; i<4; i++)
        {
            if (_uv[i].IsEqual(_meshData->m_uvMin, MTH_EPSILON))
                p1 = i;
            else if (_uv[i].IsEqual(_meshData->m_uvMax, MTH_EPSILON))
                p2 = i;
            else if (_uv[i].IsEqual(uvAABB.getMaxXMinY(), MTH_EPSILON))
                p3 = i;
            else  if (_uv[i].IsEqual(uvAABB.getMinXMaxY(), MTH_EPSILON))
                p0 = i;
            else
                return;
        }

        for (ITF_VECTOR<Mesh3D::UniqueVertex>::const_iterator vIter = mainMesh->m_ListUniqueVertex.begin();
            vIter != mainMesh->m_ListUniqueVertex.end(); ++vIter, ++idx)
        {
            const Mesh3D::UniqueVertex & uVtx = *vIter;

            Vec2d posLocal = (uVtx.Position.truncateTo2D() - dummyAABB.getMin()) * InvTransform;

            if (_useUVLimitLeft  >= 0.f && _useUVLimitRight >= 0.f)
            {
                if (posLocal.x() < _useUVLimitLeft || posLocal.x() > _useUVLimitRight)
                    continue;
                else
                    posLocal.x() = (posLocal.x() - _useUVLimitLeft) / (_useUVLimitRight - _useUVLimitLeft);
            } else if (_useUVLimitLeft  >= 0.f)
            {
                if (posLocal.x() < _useUVLimitLeft)
                    continue;
                else
                    posLocal.x() = (posLocal.x() - _useUVLimitLeft) / (1.f - _useUVLimitLeft);
            } else if (_useUVLimitRight >= 0.f)
            {
                if (posLocal.x() > _useUVLimitRight)
                    continue;
                else
                    posLocal.x() = posLocal.x() / _useUVLimitRight;
            }

            Vec2d pos = _pos[p0] + (_pos[p2] - _pos[p0])*posLocal.x() + (_pos[p1] - _pos[p0])*posLocal.y() + 
                (_pos[p3] + _pos[p0] - _pos[p1] - _pos[p2])*posLocal.x()*posLocal.y();


            if (m_pRecomputeData->m_anim)
            {
                VertexPNC3T& vtx = vtxListPNC3T[ idx ];                

                vtx.setPos(pos.to3d(uVtx.Position.z()));
                vtx.setUv( uVtx.UV );     
                vtx.setColor( U32_INVALID );

                setVtxAnim( pConfig->m_vtxAnim, vtx);
                f32 factor = Color::getRedFromU32(uVtx.color);
                vtx.m_uv3.x() *= factor;
                vtx.m_uv3.y() *= factor;
            } else
            {
                VertexPCT & data = vtxListPCT[idx];

                data.setUV(uVtx.UV);
                data.setColor(U32_INVALID);
                data.setPosition(pos.to3d(uVtx.Position.z()));
            }
        }
    }

    bbool Frise::buildVB_Static_QuadMeshAlone(const Vec2d* _pos, const Vec2d* _uv, i32 index)
    {
        StringID dummyFamily;
        i32      dummyFamilyIndex     = index;
        i32      dymmyRemainingMeshes = 1;

        return buildVB_Static_QuadMesh(_pos, _uv, dummyFamily, dummyFamilyIndex, dymmyRemainingMeshes);
    }

    bbool Frise::buildVB_Static_QuadMesh(const Vec2d* _pos, const Vec2d* _uv, StringID & _family, i32 & _familyindex, i32 & _remainingMeshes)
    {
        const FriseConfig *config = getConfig();
        if (!config->m_mesh3d.m_mesh3dList.size())
            return bfalse;

        ITF_VECTOR<Mesh3dInfo> meshList;
        const Mesh3dData * meshData = NULL;

        if (!getUVMeshes( (_uv[0] + _uv[3])*0.5f, meshData, meshList))
            return btrue;


        u32 start = m_pRecomputeData->m_anim ? m_meshBuildData->m_animVertexList.size() : m_meshBuildData->m_staticVertexList.size();
        i32 index = 0;

        if (!_family.isValid() && _familyindex >= 0)
        {
            index = Min(_familyindex, int(meshList.size()-1));
        } else
        {
            index = getMeshIndex(meshData, _family, _familyindex, _remainingMeshes);
        }


        ITF_VECTOR<IndexList> & vectIndexList = m_pRecomputeData->m_anim ? m_meshBuildData->m_animIndexList : m_meshBuildData->m_staticIndexList;
        u32 IBIndex = Min( meshData->m_meshList[index].m_textureIndex, vectIndexList.size() - 1);
        IndexList& indexList = vectIndexList[IBIndex];

        InitMeshData( indexList, meshList, index, start);
        fillMeshVertex(_pos, _uv, meshData, meshList, index, start);

        return btrue;
    }


    template<class T>
    bool VertexPtrPosLower( T * _a, T * _b)
    {
        f32 aSqrNorm = _a->m_pos.sqrnorm();
        f32 bSqrNorm = _b->m_pos.sqrnorm();

        return aSqrNorm < bSqrNorm;
    }
    
    template<class T>
    bool VertexPtrPosEqual( T * _a, T * _b)
    {
        return (_a->m_pos - _b->m_pos).sqrnorm() < MTH_EPSILON/**MTH_EPSILON*/;
            
    }

    template<class T>
    bool VertexPtrEqual( T * _a, T * _b)
    {
        return (_a->m_pos - _b->m_pos).sqrnorm() < MTH_EPSILON/**MTH_EPSILON*/ &&
               (_a->m_uv  - _b->m_uv ).sqrnorm() < MTH_EPSILON/**MTH_EPSILON*/;
    }

    template<class T, MemoryId::ITF_ALLOCATOR_IDS MEMORY_ID>
    void WeldMeshData( Frise::IndexList& _indexList, ITF_VECTOR<T, MEMORY_ID> & vtxList)
    {
        if (vtxList.size() == 0)
            return;

        ITF_VECTOR<T *> vtxListPtr;
        vtxListPtr.resize(vtxList.size());

        typename ITF_VECTOR<T>::iterator vtxIter    = vtxList.begin();
        typename ITF_VECTOR<T*>::iterator vtxPtrIter = vtxListPtr.begin();
        for (; vtxPtrIter != vtxListPtr.end(); ++vtxIter, ++vtxPtrIter)
            *vtxPtrIter = &(*vtxIter);

        std::sort(vtxListPtr.begin(), vtxListPtr.end(), VertexPtrPosLower<T>);

        ITF_VECTOR<pair<u16, u16> > indexToRemove;

        typename ITF_VECTOR<T*>::iterator prevVtxPtrIter = vtxListPtr.begin();
        vtxPtrIter     = prevVtxPtrIter + 1;
        for (; vtxPtrIter != vtxListPtr.end(); vtxPtrIter++)
        {
            if (VertexPtrEqual<T>(*vtxPtrIter, *prevVtxPtrIter))
            {
                indexToRemove.push_back(pair<u16, u16>((u16)(*prevVtxPtrIter - &(*vtxList.begin())), (u16)(*vtxPtrIter - &(*vtxList.begin()))));
            } else if (VertexPtrPosEqual<T>(*vtxPtrIter, *prevVtxPtrIter))
            {
                Vec2d vtxUv = (*vtxPtrIter)->m_uv;
                *(*vtxPtrIter) = *(*prevVtxPtrIter);
                (*vtxPtrIter)->m_uv = vtxUv;
            } else
            {
                prevVtxPtrIter = vtxPtrIter;
            }
        }

        if (indexToRemove.size() == 0)
            return;

        for (ITF_VECTOR<u16>::iterator indexIter = _indexList.m_list.begin();
            indexIter != _indexList.m_list.end(); ++indexIter)
        {
            for (ITF_VECTOR<pair<u16, u16> >::iterator indexRemoveIter = indexToRemove.begin();
                indexRemoveIter != indexToRemove.end(); ++indexRemoveIter)
            {
                if (indexRemoveIter->second == *indexIter)
                {
                    *indexIter = indexRemoveIter->first;
                    break;
                }
            }
        }
    }

    void Frise::WeldMeshDataStatic( IndexList& _indexList)
    {
        ITF_VECTOR< VertexPCT > & vtxList = m_meshBuildData->m_staticVertexList;
        WeldMeshData< VertexPCT >( _indexList, vtxList);
    }

    void Frise::WeldMeshDataAnim( IndexList& _indexList)
    {
        ITF_VECTOR<VertexPNC3T>& vtxList = m_meshBuildData->m_animVertexList;
        WeldMeshData<VertexPNC3T>( _indexList, vtxList);
    }


} // namespace ITF
