#include "precompiled_engine.h"

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_MESH3D_H_
#include    "engine/display/Mesh3D.h"
#endif // _ITF_MESH3D_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_PREFETCH_FCT_H_
#include "engine/boot/PrefetchFct.h"
#endif  //_ITF_PREFETCH_FCT_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifdef ITF_SUPPORT_COOKING
#ifndef _ITF_MESH3DCOOKER_H_
#include    "tools/plugins/CookerPlugin/Mesh3DCooker.h"
#endif // _ITF_MESH3DCOOKER_H_
#endif

namespace ITF
 // namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // transform skinning data from list of vertex touch by a bones to list of bones used by a vertex
    //
    void Mesh3D::MeshComputeSkinning_PerElementToPerVertex( ITF_VECTOR<SkinVertex> &_skinVertex )
    {
        _skinVertex.resize(m_ListUniqueVertex.size());
        for (u32 i = 0; i < _skinVertex.size(); i++)
            _skinVertex[i].Index = i;

        for (u32 i = 0; i < m_SkinElements.size(); i++)
        {
            Mesh3D::IndexWeight current;

            //current.m_Index = m_SkinElements[i].m_MatrixIndex;
            current.m_Index = i;

            ITF_VECTOR<IndexWeight>::const_iterator it = m_SkinElements[i].m_Point.begin();
            while(it != m_SkinElements[i].m_Point.end())
            {
                current.m_Weight = it->m_Weight;
                _skinVertex[it->m_Index].m_SkinElements.push_back( current );
                ++it;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // remove bones influence if more than 4 bones
    //
    void Mesh3D::MeshComputeSkinning_3BonesMax( ITF_VECTOR<SkinVertex> &_skinVertex )
    {
        // remove more than four matrices
        u32 count = 0;
        for (u32 i = 0; i < _skinVertex.size(); i++)
        {
            while (_skinVertex[i].m_SkinElements.size() >= 4) 
            {
                f32 fbest = _skinVertex[i].m_SkinElements[0].m_Weight;
                i32 ibest = 0;
                for (u32 j = 1; j < _skinVertex[i].m_SkinElements.size(); j++)
                {
                    if (_skinVertex[i].m_SkinElements[j].m_Weight >= fbest) continue;
                    fbest = _skinVertex[i].m_SkinElements[j].m_Weight;
                    ibest = j;
                }
                _skinVertex[i].m_SkinElements.removeAtUnordered( ibest );
                count++;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // remove bones with too low influence
    //
    void Mesh3D::MeshComputeSkinning_RemoveLowWeight( ITF_VECTOR<SkinVertex> &_skinVertex, f32 _thresh )
    {
        u32 count = 0;
        for (u32 i = 0; i < _skinVertex.size(); i++)
        {
            for (u32 j = 0; j < _skinVertex[i].m_SkinElements.size();)
            {
                if (_skinVertex[i].m_SkinElements[j].m_Weight > _thresh)
                    j++;
                else
                {
                    _skinVertex[i].m_SkinElements.removeAtUnordered(j);
                    count++;
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // renormalize weight ( so sum of weight is equal to one )
    //
    void Mesh3D::MeshComputeSkinning_Normalize( ITF_VECTOR<SkinVertex> &_skinVertex )
    {
        for (u32 i = 0; i < _skinVertex.size(); i++)
        {
            f32 weight = 0;
            for (u32 j = 0; j < _skinVertex[i].m_SkinElements.size();j++)
                weight += _skinVertex[i].m_SkinElements[j].m_Weight;
            for (u32 j = 0; j < _skinVertex[i].m_SkinElements.size();j++)
                _skinVertex[i].m_SkinElements[j].m_Weight /= weight;
        }
    }



    ///////////////////////////////////////////////////////////////////////////////////////////////
    // reduce number of bones
    //
    bbool Mesh3D::MeshComputeSkinning_ReduceBones(ITF_VECTOR<SkinVertex> &_skinVertex, u32 _maxBones, u32 _loop )
    {
        u32 i, j, num;

        if (m_SkinElements.size() < _maxBones)
            return bfalse;

        // first init
        if (!_loop)
        {
            // create data to store neighborhood computing
            for (i = 0; i < m_SkinElements.size(); i++)
            {
                m_SkinElements[i].m_Flags = (u32) -1;
                // create neighborhood
                m_SkinElements[i].m_Neighbor.resize( m_SkinElements.size() );
                for (j = 0; j < m_SkinElements.size(); j++)
                {
                    m_SkinElements[i].m_Neighbor[j].m_index = j;
                    m_SkinElements[i].m_Neighbor[j].m_numPoint = 0;
                }
            }

            // get neighborhood
            for (i = 0; i < _skinVertex.size(); i++ )
            {
                if (_skinVertex[i].m_SkinElements.size() < 2) continue;
                IndexWeight *iw = &_skinVertex[i].m_SkinElements[0];
                IndexWeight *iwlast = iw + _skinVertex[i].m_SkinElements.size();

                for (; iw < iwlast; iw++ )
                {
                    IndexWeight *iw0 = &_skinVertex[i].m_SkinElements[0];
                    for (; iw0 < iwlast; iw0++)
                    {
                        if (iw == iw0) continue;
                        m_SkinElements[ iw->m_Index ].m_Neighbor[iw0->m_Index].m_numPoint++;
                        m_SkinElements[ iw0->m_Index ].m_Neighbor[iw->m_Index].m_numPoint++;
                    }
                }
            }

            // compress neighborhood
            for (i = 0; i < m_SkinElements.size(); i++)
            {
                for (j = 0; j < m_SkinElements[i].m_Neighbor.size(); )
                {
                    if (m_SkinElements[i].m_Neighbor[j].m_numPoint == 0)
                        m_SkinElements[i].m_Neighbor.removeAt( j );
                    else 
                        j++;
                }
            }
        }

        // add while num is less than max
        num = 0;

        // add all bones with no neighbor
        for (i = 0; i < m_SkinElements.size() && num < _maxBones; i++)
        {
            if ( m_SkinElements[i].m_Flags != (u32) -1) continue;
            if (m_SkinElements[i].m_Neighbor.size()) continue;
            m_SkinElements[i].m_Flags = _loop;
            num++;
        }

        while (num < _maxBones)
        {
            // get a bone with only one neighbor
            for (i = 0; i < m_SkinElements.size(); i++)
            {
                if ( m_SkinElements[i].m_Flags != (u32) -1) continue;
                if (m_SkinElements[i].m_Neighbor.size() == 1) break;
            }

            // none found, take first not treated
            if (i == m_SkinElements.size())
            {
                for (i = 0; i < m_SkinElements.size() && num < _maxBones; i++)
                {
                    if ( m_SkinElements[i].m_Flags != (u32) -1) continue;
                    break;
                }
            }

            // none found => no more bones, all added, should not happend
            if (i == m_SkinElements.size())
                return bfalse;

            ITF_VECTOR<u32> toTreat;
            toTreat.push_back( i );
            while (toTreat.size() && num < _maxBones)
            {
                SkinElement &skinElement = m_SkinElements[ toTreat[0] ];
                skinElement.m_Flags = _loop;
                num++;
                toTreat.removeAt( 0 );

                for (i = 0; i < skinElement.m_Neighbor.size(); i++)
                {
                    if (m_SkinElements[skinElement.m_Neighbor[i].m_index].m_Flags == _loop) continue;
                    for (j = 0; j< toTreat.size(); j++)
                        if (toTreat[j] == skinElement.m_Neighbor[i].m_index)
                            break;
                    if (j == toTreat.size())
                        toTreat.push_back( skinElement.m_Neighbor[i].m_index);
                }
            }
        }

        // mark all vertices to be removed from separate mesh
        ITF_VECTOR<u32> vertexState;
        u32 newIndex = 0;
        vertexState.resize( m_ListUniqueVertex.size() );
        for (i = 0; i < vertexState.size(); i++)
            vertexState[i] = (u32) -1;

        for (i = 0; i < _skinVertex.size(); i++)
        {
            for (j = 0; j < _skinVertex[i].m_SkinElements.size(); j++)
            {
                if (m_SkinElements[ _skinVertex[i].m_SkinElements[j].m_Index ].m_Flags == _loop) continue;
                break;
            }
            if (j == _skinVertex[i].m_SkinElements.size())
                vertexState[i] = newIndex++;
        }

        // separate element
        u32 countElem = m_ListElement.size();
        for (i = 0; i < countElem; i++)
        {
            Element &cur = m_ListElement[i], newElem;

            newElem.m_MaterialID = cur.m_MaterialID;
            newElem.m_Material = cur.m_Material;

            for (j = 0; j < cur.m_ListTriangles.size(); j++)
            {
                const Triangle &tri = cur.m_ListTriangles[j];
                if (vertexState[tri.VBIndex[0]] == (u32) -1) continue;
                if (vertexState[tri.VBIndex[1]] == (u32) -1) continue;
                if (vertexState[tri.VBIndex[2]] == (u32) -1) continue;

                newElem.m_ListTriangles.push_back( tri );
                cur.m_ListTriangles.removeAt( j );
                j--;
            }

            if (newElem.m_ListTriangles.size())
                m_ListElement.push_back( newElem );

        }

        return btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // store computed skinning data in mesh buffer
    //
    void Mesh3D::MeshComputeSkinning_Store(ITF_VECTOR<SkinVertex> &_skinVertex)
    {
        // count with only one point
        m_SkinVertexOneMatrix.clear();
        m_SkinVertex.clear();
        
        for (u32 i = 0; i < _skinVertex.size(); i++)
        {
#ifndef HARDWARESKINNING
            if (_skinVertex[i].m_SkinElements.size() == 0) continue;
            if (_skinVertex[i].m_SkinElements.size() == 1)
            {
                SkinVertexOneMatrix data;
                data.Index = i;
                data.IndexMatrix = _skinVertex[i].m_SkinElements[0].m_Index;
                m_SkinVertexOneMatrix.push_back( data );
            }
            else
#endif
            {
                SkinVertexFourMatrices data;
                data.Index = i;
                data.Number = _skinVertex[i].m_SkinElements.size();
                if (data.Number > 4) data.Number = 4;
                for (u32 j = 0; j < data.Number ; j++ )
                {
                    data.Skin[j].m_Index = _skinVertex[i].m_SkinElements[j].m_Index;
                    data.Skin[j].m_Weight = _skinVertex[i].m_SkinElements[j].m_Weight;
                }
                m_SkinVertex.push_back( data );
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // compute skinning data
    //
    void Mesh3D::MeshComputeSkinning()
    {
        ITF_VECTOR<SkinVertex>          skinVertex;

        m_SkinMaxMatrixIndex = 0;
        for (u32 i = 0; i < m_SkinElements.size(); i++)
            if (m_SkinElements[i].m_MatrixIndex + 1 > m_SkinMaxMatrixIndex) m_SkinMaxMatrixIndex = m_SkinElements[i].m_MatrixIndex + 1;

        m_SkinToGizmo.resize( m_SkinElements.size() );
        for (u32 i = 0; i < m_SkinElements.size(); i++)
            m_SkinToGizmo[i] = m_SkinElements[i].m_MatrixIndex;

        MeshComputeSkinning_PerElementToPerVertex( skinVertex );
        MeshComputeSkinning_3BonesMax(skinVertex);
        MeshComputeSkinning_RemoveLowWeight(skinVertex, 0.01f );
        MeshComputeSkinning_Normalize(skinVertex);

        /*
        u32 loop = 0;
        while (1)
        {
            if (!MeshComputeSkinning_ReduceBones( skinVertex, 30, loop))
                break;
            loop++;
        }
        */

        MeshComputeSkinning_Store(skinVertex);
    }
    
}

