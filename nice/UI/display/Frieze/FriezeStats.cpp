#include "precompiled_engine.h"

#ifndef _ITF_FRIEZESTATS_H_
#include "engine/display/Frieze/FriezeStats.h"
#endif //_ITF_FRIEZESTATS_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_


namespace ITF
{
#ifdef ENABLE_FRIEZESTATS

FriezeStats* FriezeStats::m_singleton = NULL;


void FriezeStats::createSingleton()
{
    ITF_ASSERT(NULL == m_singleton);
    m_singleton = newAlloc(mId_Singleton, FriezeStats());
}

void FriezeStats::deleteSingleton()
{
    SF_DEL(m_singleton);
}

void FriezeStats::addFriezeInstance(ObjectRef _or)
{    
    const i32 index = m_friezeList.find(_or);
    ITF_ASSERT( index == -1 );

    if ( index == -1 )    
    {
        m_friezeList.push_back(_or);
    }
}

void FriezeStats::removeFriezeInstance(ObjectRef _or)
{
    const i32 index = m_friezeList.find(_or);
    ITF_ASSERT( index >= 0 );

    if ( index >= 0 )
    {
        m_friezeList.eraseNoOrder(index);
    }
}

void FriezeStats::addFriezeVB(void* _adrs, ObjectRef _or, i32 _size)
{
    ITF_ASSERT(m_friezeList.find(_or) >= 0);
    VB vb;
    vb.m_owner = _or;
    vb.m_vertexCount = _size;
    m_vbList[uPtr(_adrs)] = vb;
}

void FriezeStats::removeFriezeVB(void* _adrs)
{
    ITF_MAP<uPtr, VB>::iterator it = m_vbList.find(uPtr(_adrs));
    if (it != m_vbList.end())
        m_vbList.erase(it);
    else
    {
        ITF_ASSERT_MSG(bfalse, "unregistering unknown VB");
    }
}

void FriezeStats::dumpToFile()
{
#ifdef WIN32
    /*
    FILE* plof = fopen("c:\\frisedump.txt", "wt");
    i32 totalFriseCount = 0;
    i32 totalVtxCount = 0;
    for (u32 i = 0; i < m_friezeList.size(); i++)
    {
        ObjectRef or = m_friezeList[i];
        Frise* pFrise = (Frise*)GETOBJECT(or);
        ITF_ASSERT(pFrise);
        if (pFrise)
        {
            totalFriseCount++;
            String8 id;
            pFrise->getObjectId(id);
            fprintf(plof, "Frieze:\n");
            fprintf(plof, "\tname:%s\n", pFrise->getUserFriendly().cStr());
            fprintf(plof, "\tid:%s\n", id.cStr());
            fprintf(plof, "\tctrl pts:%d\n", pFrise->getPosCount());
            fprintf(plof, "\tVB data:\n");
            ITF_MAP<uPtr, VB>::iterator it = m_vbList.begin();
            ITF_MAP<uPtr, VB>::iterator end = m_vbList.end();
            i32 vbcount = 0;
            i32 vtxcount = 0;
            while (it != end)
            {
                if ((*it).second.m_owner == or)
                {
                    vbcount++;
                    vtxcount += (*it).second.m_vertexCount;
                    fprintf(plof, "\t\tVB #%d:%d vertex\n", vbcount, (*it).second.m_vertexCount);
                }
                ++it;
            }
            fprintf(plof, "\t\tvertex count:%d\n", vtxcount);
            totalVtxCount += vtxcount;
        }
    }
    fprintf(plof, "total frieze count:%d\n", totalFriseCount);
    fprintf(plof, "total vertex count:%d\n", totalVtxCount);
    fclose(plof);*/
#endif

}


#endif //ENABLE_FRIEZESTATS

} // namespace ITF
