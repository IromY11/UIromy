#include "precompiled_engine.h"

#if 0

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "gameplay/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
/*
//  BIT MAP
m_bitMapUvs[0] = Vec2d(0.01f, 0.76f);
m_bitMapUvs[1] = Vec2d(0.26f, 0.76f);
m_bitMapUvs[2] = Vec2d(0.51f, 0.76f);
m_bitMapUvs[3] = Vec2d(0.76f, 0.76f);
m_bitMapUvs[4] = Vec2d(0.01f, 0.51f);
m_bitMapUvs[5] = Vec2d(0.26f, 0.51f);
m_bitMapUvs[6] = Vec2d(0.51f, 0.51f);
m_bitMapUvs[7] = Vec2d(0.76f, 0.51f);
m_bitMapUvs[8] = Vec2d(0.01f, 0.26f);
m_bitMapUvs[9] = Vec2d(0.26f, 0.26f);
m_bitMapUvs[10] = Vec2d(0.51f, 0.26f);
m_bitMapUvs[11] = Vec2d(0.76f, 0.26f);
m_bitMapUvs[12] = Vec2d(0.01f, 0.01f);
*/
    typedef struct BitMap
    {
        int             m_vertexIndexDeb;
        int             m_vertexNb;
    }BitMap;

    

    struct BitMapList
    {
        BitMapList(Frise* _owner)
        {
            m_FriseRef = _owner;
            m_bitMap = NULL;
            m_bitMapSizeX = 10;
            m_bitMapSizeY = 10;

        }

        u32         m_bitMapSizeX;
        u32         m_bitMapSizeY;
        
        void    builsStripInBitMap          (   int _indexUv, Vec3d& _posBitMap,  float _x =0, float _y =0, int _indexVertexToOffSet =0, Vec2d _offSet = Vec2d(0.f,0.f));        
        Vec3d   addOffSetBitMap                   (   float _x, float _y);
        void    bitMapRandom                (   );

        ITF_VECTOR<BitMap>  m_bitMapStrip;
        Frise   *m_FriseRef;
        u32     *m_bitMap;
        Vec2d   m_bitMapUvs[13];
    };

    void Frise::buildFriezeInBitMap()
    {

        BitMapList list(this);

#define ARRAY(_arr, _x, _y) _arr[_x + list.m_bitMapSizeX * (_y)]

        //  move
        //    list.bitMapRandom();

        //  parsing du tableau de bitmap 
        for (u32 x=1; x<list.m_bitMapSizeX -1; x++)
        {
            for (u32 y=1; y<list.m_bitMapSizeY -1; y++)
            {
                u32 indexBitMap = x + y*list.m_bitMapSizeX;

                //  si la case est non nulle
                if ( list.m_bitMap[indexBitMap] )
                {
                    u32     res;
                    Vec3d   posBitMap;
                    float   thickness     = m_txt_height;
                    Vec3d   pos;

                    //  Haut Gauche
                    res = ARRAY(list.m_bitMap, x-1, y) +ARRAY(list.m_bitMap, x-1, y-1)*2 +ARRAY(list.m_bitMap, x, y-1)*4;
                    pos =Vec3d((float)(x*2)*thickness, -(float)(y*2)*thickness, 0.f);

                    if (res>7)
                        res  = 0;

                    switch(res)
                    {
                    case 0:
                    case 2:
                        list.builsStripInBitMap(0, pos, float(x*2), float(y*2));
                        break;
                    case 4:
                    case 6:
                        list.builsStripInBitMap(11, pos, float(x*2), float(y*2));
                        break;
                    case 5:
                        list.builsStripInBitMap(5, pos, float(x*2), float(y*2));
                        break;
                    case 1:
                    case 3:
                        list.builsStripInBitMap(8, pos, float(x*2), float(y*2));
                        break;
                    case 7:
                        list.builsStripInBitMap(12, pos, float(x*2), float(y*2));
                        break;
                    }

                    //  Haut Droite
                    res   = ARRAY(list.m_bitMap, x, y-1) +ARRAY(list.m_bitMap, x+1, y-1)*2 +ARRAY(list.m_bitMap, x+1, y)*4;
                    pos =Vec3d((float)(x*2+1)*thickness, -(float)(y*2)*thickness, 0.f);

                    if (res>7)
                        res  = 0;

                    switch(res)
                    {
                    case 0:
                    case 2:
                        list.builsStripInBitMap(1, pos, float(x*2+1), float(y*2));
                        break;
                    case 1:
                    case 3:
                        list.builsStripInBitMap(9, pos, float(x*2+1), float(y*2));
                        break;
                    case 5:
                        list.builsStripInBitMap(4, pos, float(x*2+1), float(y*2));
                        break;
                    case 4:
                    case 6:
                        list.builsStripInBitMap(8, pos, float(x*2+1), float(y*2));
                        break;
                    case 7:
                        list.builsStripInBitMap(12, pos, float(x*2+1), float(y*2));
                        break;
                    }

                    //  Bas Droite
                    res   = ARRAY(list.m_bitMap, x+1, y) +ARRAY(list.m_bitMap, x+1, y+1)*2 +ARRAY(list.m_bitMap, x, y+1)*4;                 
                    pos =Vec3d((float)(x*2+1)*thickness, -(float)(y*2+1)*thickness, 0.f);

                    if (res>7)
                        res  = 0;

                    switch(res)
                    {
                    case 0:
                    case 2:
                        list.builsStripInBitMap(2, pos, float(x*2+1), float(y*2+1));
                        break;
                    case 4:
                    case 6:
                        list.builsStripInBitMap(9, pos, float(x*2+1), float(y*2+1));
                        break;
                    case 5:
                        list.builsStripInBitMap(6, pos, float(x*2+1), float(y*2+1));
                        break;
                    case 1:
                    case 3:
                        list.builsStripInBitMap(10, pos, float(x*2+1), float(y*2+1));
                        break;
                    case 7:
                        list.builsStripInBitMap(12, pos, float(x*2+1), float(y*2+1));
                        break;
                    }

                    //  Bas gauche
                    res   = ARRAY(list.m_bitMap, x, y+1) +ARRAY(list.m_bitMap, x-1, y+1)*2 +ARRAY(list.m_bitMap, x-1, y)*4;
                    pos =Vec3d((float)(x*2)*thickness, -(float)(y*2+1)*thickness, 0.f);

                    if (res>7)
                        res  = 0;

                    switch(res)
                    {
                    case 0:
                    case 2:
                        list.builsStripInBitMap(3, pos, float(x*2), float(y*2+1));
                        break;
                    case 1:
                    case 3:
                        list.builsStripInBitMap(11, pos, float(x*2), float(y*2+1));
                        break;
                    case 5:
                        list.builsStripInBitMap(7, pos, float(x*2), float(y*2+1));
                        break;
                    case 4:
                    case 6:
                        list.builsStripInBitMap(10, pos, float(x*2), float(y*2+1));
                        break;
                    case 7:
                        list.builsStripInBitMap(12, pos, float(x*2), float(y*2+1));
                        break;
                    }
                }
            }
        }
#undef ARRAY


    }


    Vec3d BitMapList::addOffSetBitMap(float _x, float _y)
    {
        return Vec3d(0.f,0.f,0.f);

        //     float thickness     = pConfig->m_textureConfigs[0].m_thicknessFinal;
        //     return Vec3d( cos(_x+m_cpt) *0.25f *thickness, sin(_x+m_cpt) *0.25f *thickness, 0.f );
    }

    void BitMapList::builsStripInBitMap(int _indexUv, Vec3d& _posBitMap, float _x, float _y, int _indexOffSet, Vec2d _offSet)
    {
        FriseConfig* pConfig = m_FriseRef->getConfig();
        ITF_ASSERT(pConfig);
        if (!pConfig)
            return;

        BitMap strip;
        strip.m_vertexIndexDeb  = m_FriseRef->m_vertexs.size();
        strip.m_vertexNb        = 4;

        const PolyLineEdge&  edge0 = m_FriseRef->getEdgeAt(0);
        float thickness     = m_FriseRef->m_txt_height;
        Vec3d pivot         = Vec3d(edge0.getPos().x(), edge0.getPos().y(), m_FriseRef->getDepth());
        u32   vertexColor   = pConfig->m_textureConfigs[0].m_color;

        Vertex  v;
        Vec2d   posUv       = m_bitMapUvs[_indexUv];
        Vec3d   posVertex(0.f,0.f,0.f);


        //  0
        posVertex       = _posBitMap +addOffSetBitMap(_x, _y+1.f);
        posVertex.y()  -= thickness;
        v.setData(pivot+posVertex, posUv, vertexColor);
        m_FriseRef->m_vertexs.push_back(v);
        //  1
        posVertex       = _posBitMap +addOffSetBitMap(_x, _y);
        v.setData(pivot+posVertex, posUv +Vec2d(0.f,0.23f), vertexColor);
        m_FriseRef->m_vertexs.push_back(v);
        //  2
        posVertex       = _posBitMap +addOffSetBitMap(_x+1.f, _y+1.f);
        posVertex.x()  += thickness;
        posVertex.y()  -= thickness;
        v.setData(pivot+posVertex, posUv +Vec2d(0.23f,0.f), vertexColor);
        m_FriseRef->m_vertexs.push_back(v);
        //  3
        posVertex       = _posBitMap +addOffSetBitMap(_x+1.f, _y);
        posVertex.x()  += thickness;
        v.setData(pivot+posVertex, posUv +Vec2d(0.23f,0.23f), vertexColor);
        m_FriseRef->m_vertexs.push_back(v);

        //  add
        m_bitMapStrip.push_back(strip);
    }


    void BitMapList::bitMapRandom()
    {
        static float stepChange = 0.f;
        static float cpt = 0.f;

        cpt += 0.01f;
        if ( cpt>stepChange )
        {
            stepChange += 0.000001f;

            int index_X  = rand()%(m_bitMapSizeX-4) +2;
            int index_Y  = rand()%(m_bitMapSizeY-4) +2;

            m_bitMap[ index_X + index_Y*m_bitMapSizeX ]  = 1;//rand()%2;
        }
    }

    void Frise::renderBitMap()const
    {
//         FriseConfig* pConfig = getConfig();
//         ITF_ASSERT(pConfig);
//         if (!pConfig)
//             return;
//         for ( u32 i=0; i<m_bitMapStrip.size(); i++)
//         {
//             const BitMap& strip = m_bitMapStrip[i]; 
//             GFX_ADAPTER->addPrimitive(m_prioAffichageFinal, (Texture*)pConfig->m_textureConfigs[0].m_textureData.getResource(), GFX_TRIANGLE_STRIP, &m_vertexs, strip.m_vertexIndexDeb, strip.m_vertexNb);
//         }
    }

} // namespace ITF

#endif