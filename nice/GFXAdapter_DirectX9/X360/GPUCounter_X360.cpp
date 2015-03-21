#include "precompiled_GFXAdapter_Directx9.h"

#ifdef ITF_X360


#ifndef GPUCOUNTER_X360_H
#include "adapters\GFXAdapter_DirectX9\X360\GPUCounter_X360.h"
#endif // GPUCOUNTER_X360_H


// Counters are described in this file,  GPUPERFEVENT_*
#include <d3d9gpu.h>

namespace ITF
{

//------------------------------------------------------------------------
GPUPerfCounter_X360::GPUPerfCounter_X360(IDirect3DDevice9 * _pD3DDevice, ux _numCounters):
    mp_device(_pD3DDevice),
    m_frameCounter(0xFFffFFff),
    m_numCounters((u8)_numCounters),
    m_curCounter(255),  // +1 at first start => 0
    m_unused(0),
    m_CPCost(0.0f)
{
    m_pValues = newAlloc(mId_GfxAdapter, D3DPERFCOUNTER_VALUES[_numCounters]);
    m_pEvents = newAlloc(mId_GfxAdapter, D3DPERFCOUNTER_EVENTS[_numCounters]);

    for ( ux i = 0; i < _numCounters; ++i )
    {
        ZeroMemory(&m_pValues[i], sizeof(D3DPERFCOUNTER_VALUES));
        ZeroMemory(&m_pEvents[i], sizeof(D3DPERFCOUNTER_EVENTS));

        // All counters define CP_CYCLES to get GPU frame time
        m_pEvents[i].CP[0] = GPUPE_CP_COUNT;
    }

#if _USE_GPU_PERFCOUNTER_
    mp_Counters = newAlloc(mId_GfxAdapter,PerfCounter[_numCounters]);

    // Use perf counters to measure GPU time
    for ( ux i = 0; i < _numCounters; ++i )
    {
        PerfCounter & pc = mp_Counters[i];

        for ( ux j = 0; j < PC_Count; ++j )
        {
            mp_device->CreatePerfCounters(&pc.begin[j], 1);
            mp_device->CreatePerfCounters(&pc.end[j], 1);
        }
    }
#endif
}


//------------------------------------------------------------------------
GPUPerfCounter_X360::~GPUPerfCounter_X360()
{
#if _USE_GPU_PERFCOUNTER_
    for ( ux c = 0; c < m_numCounters; ++c )
    {
        PerfCounter & pc = mp_Counters[c];
        for ( ux i = 0; i < PC_Count; ++i )
        {
            if ( pc.begin[i] )
                pc.begin[i]->Release();
            if ( pc.end[i] )
                pc.end[i]->Release();
        }
    }
    if ( m_numCounters )
    {
        delete[] m_pEvents;
        delete[] m_pValues;
        delete[] mp_Counters;
    }
#endif
}


//------------------------------------------------------------------------
void GPUPerfCounter_X360::start()
{
#if _USE_GPU_PERFCOUNTER_
    ++m_frameCounter;
    ux curFrame = m_frameCounter % PC_Count;
    if ( curFrame == 0 )
    {
        // Change counter
        m_curCounter = (m_curCounter + 1) % m_numCounters;
    }
    mp_device->SetPerfCounterEvents(&m_pEvents[m_curCounter], 0 );
    PerfCounter & pc = mp_Counters[m_curCounter];
    mp_device->QueryPerfCounters(pc.begin[curFrame], D3DPERFQUERY_WAITGPUIDLE);
#endif
}


//------------------------------------------------------------------------
void GPUPerfCounter_X360::stop()
{
#if _USE_GPU_PERFCOUNTER_
    PerfCounter & pc = mp_Counters[m_curCounter];
    ux curFrame = m_frameCounter % PC_Count;
    mp_device->QueryPerfCounters(pc.end[curFrame], D3DPERFQUERY_WAITGPUIDLE);
#endif
}


//------------------------------------------------------------------------
float GPUPerfCounter_X360::getCommandProcessorCost() const
{
    return m_CPCost;
}


//------------------------------------------------------------------------
void GPUPerfCounter_X360::computeResults()
{
#if _USE_GPU_PERFCOUNTER_
    D3DPERFCOUNTER_VALUES startValues;
    ux count = m_numCounters;
    for ( ux i = 0; i < count; ++i )
    {
        PerfCounter & pc = mp_Counters[i];
        D3DPERFCOUNTER_VALUES & endValues = m_pValues[i];
        u32 curCounter = (m_frameCounter + 1) % PC_Count;
        pc.begin[curCounter]->GetValues(&startValues, 0, NULL);
        pc.end  [curCounter]->GetValues(&endValues,   0, NULL);

        // Subtract start values from end values
        UINT64* pstartValues = (UINT64 *)&startValues;
        UINT64* pEndValues   = (UINT64 *)&endValues;
        const ux dwCount  = sizeof(D3DPERFCOUNTER_VALUES) / sizeof(UINT64);
        for( ux val = 0; val < dwCount; ++val )
        {
            pEndValues[val] -= pstartValues[val];
        }
    }
    m_CPCost = (float)m_pValues[m_curCounter].CP[0].QuadPart / GPUFreqDivider;
#else
    m_CPCost = 0;
#endif
}



//------------------------------------------------------------------------
//
// PerCounter manager
//
//------------------------------------------------------------------------
PerfCounterManager::PerfCounterManager(IDirect3DDevice9 * _pD3DDevice):
    m_drawTimeCounters(_pD3DDevice)
{
}


//------------------------------------------------------------------------
void PerfCounterManager::start()
{
    m_drawTimeCounters.start();
}


//------------------------------------------------------------------------
void PerfCounterManager::stop()
{
    m_drawTimeCounters.stop();
    m_drawTimeCounters.computeResults();
}


//------------------------------------------------------------------------
ux PerfCounterManager::formatResults(char * _pBuffer, ux _bufferSize) const
{
    _pBuffer[0] = 0;
    char buffer[256];
    ux numLines;

    numLines = m_drawTimeCounters.formatResults(buffer, sizeof(buffer));
    strcat_s(_pBuffer, _bufferSize, buffer);

    return numLines;
}


//------------------------------------------------------------------------
float PerfCounterManager::getGPUFrameTime() const
{
    return m_drawTimeCounters.getCommandProcessorCost();
}



//------------------------------------------------------------------------
//
// Counter classes
//
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Draw time Counter
//------------------------------------------------------------------------
GPUPerfCounterDrawTime::GPUPerfCounterDrawTime(IDirect3DDevice9 * _pD3DDevice):
    GPUPerfCounter_X360(_pD3DDevice, NUM_COUNTER)
{

}

void GPUPerfCounterDrawTime::computeResults()
{
    D3DPERFCOUNTER_VALUES startValues; 
    PerfCounter & pc = mp_Counters[0]; 
    D3DPERFCOUNTER_VALUES & endValues = m_pValues[0]; 
    u32 curCounter = (m_frameCounter + 1) % PC_Count; 
    pc.begin[curCounter]->GetValues(&startValues, 0, NULL); 
    pc.end  [curCounter]->GetValues(&endValues,   0, NULL);   

    // Subtract start values from end values
    //UINT64* pstartValues = (UINT64 *)&startValues;
    //UINT64* pEndValues   = (UINT64 *)&endValues;
    //const ux dwCount  = sizeof(D3DPERFCOUNTER_VALUES) / sizeof(UINT64);
    //for( ux val = 0; val < dwCount; ++val )
    //{
    //    pEndValues[val] -= pstartValues[val];
    //}
    m_CPCost = (float)(endValues.CP[0].QuadPart - startValues.CP[0].QuadPart) / GPUFreqDivider;

    //(unsigned __int64 &)endValues.CP[0].QuadPart &= (unsigned __int64 &)startValues.CP[0].QuadPart; 
    //m_CPCost = (float)m_pValues[0].CP[0].QuadPart / GPUFreqDivider; 
}

ux GPUPerfCounterDrawTime::formatResults(char * _pBuffer, ux _bufferSize) const
{
    sprintf_s(_pBuffer, _bufferSize,
        "draw time: %.3f µs\n",
        m_CPCost * 1000.0f * 1000.0f
        );

    return 1;   // number of lines in result
}

//------------------------------------------------------------------------
// ALU Counter
//------------------------------------------------------------------------

GPUPerfCounterALU::GPUPerfCounterALU(IDirect3DDevice9 * _pD3DDevice):
    GPUPerfCounter_X360(_pD3DDevice, NUM_COUNTER)
{
    D3DPERFCOUNTER_EVENTS & vtxEvt = m_pEvents[SQ_VTX];
    vtxEvt.SQ[0] = GPUPE_SQ_ALU_VTX_INST_ISSUED;
    vtxEvt.SQ[1] = GPUPE_SQ_TEX_VTX_INST_ISSUED;
    vtxEvt.SQ[2] = GPUPE_SQ_CF_VTX_INST_ISSUED;
    vtxEvt.SQ[3] = GPUPE_VERTEX_THREAD_0_ACTIVE;
    vtxEvt.RBBM[0] = GPUPE_RBBM_NRT_BUSY;

    D3DPERFCOUNTER_EVENTS & pixEvt = m_pEvents[SQ_PIX];
    pixEvt.SQ[0] = GPUPE_SQ_ALU_PIX_INST_ISSUED;
    pixEvt.SQ[1] = GPUPE_SQ_TEX_PIX_INST_ISSUED;
    pixEvt.SQ[2] = GPUPE_SQ_CF_PIX_INST_ISSUED;
    pixEvt.SQ[3] = GPUPE_PIXEL_THREAD_0_ACTIVE;

    D3DPERFCOUNTER_EVENTS & wtfEvt = m_pEvents[SQ_WATERFALL];
    wtfEvt.SQ[0] = GPUPE_SQ_CONSTANTS_SENT_SP_SIMD0;
    wtfEvt.SQ[1] = GPUPE_SQ_CONSTANTS_USED_SIMD0;

    m_vtxALUTime    = 0.0f;
    m_vtxTEXTime    = 0.0f;
    m_vtxCFTime     = 0.0f;
    m_vtxBusy       = 0.0f;
    m_pixALUTime    = 0.0f;
    m_pixTEXTime    = 0.0f;
    m_pixCFTime     = 0.0f;
    m_pixBusy       = 0.0f;
    m_waterfallTime = 0.0f;
}


//------------------------------------------------------------------------
void GPUPerfCounterALU::computeResults()
{
    GPUPerfCounter_X360::computeResults();

    UINT64 vtxALUCycles = m_pValues[SQ_VTX].SQ[0].QuadPart * 4 / 3;
    UINT64 vtxTEXCycles = m_pValues[SQ_VTX].SQ[1].QuadPart;
    UINT64 vtxCFCycles  = m_pValues[SQ_VTX].SQ[2].QuadPart;
    UINT64 vtxBusyCycles= m_pValues[SQ_VTX].SQ[3].QuadPart;
    UINT64 pixALUCycles = m_pValues[SQ_PIX].SQ[0].QuadPart * 4 / 3;
    UINT64 pixTEXCycles = m_pValues[SQ_PIX].SQ[1].QuadPart;
    UINT64 pixCFCycles  = m_pValues[SQ_PIX].SQ[2].QuadPart;
    UINT64 pixBusyCycles= m_pValues[SQ_PIX].SQ[3].QuadPart;
    UINT64 wtfCycles    = (m_pValues[SQ_WATERFALL].SQ[0].QuadPart -
                           m_pValues[SQ_WATERFALL].SQ[1].QuadPart) * 2 * 4 / 3;
    
    double busyCycles = (double)m_pValues[SQ_VTX].RBBM[0].QuadPart;

    m_vtxALUTime    = (float)vtxALUCycles / GPUFreqDivider;
    m_vtxTEXTime    = (float)vtxTEXCycles / GPUFreqDivider;
    m_vtxCFTime     = (float)vtxCFCycles / GPUFreqDivider;
    m_vtxBusy       = (float)(vtxBusyCycles / busyCycles);
    m_pixALUTime    = (float)pixALUCycles / GPUFreqDivider;
    m_pixTEXTime    = (float)pixTEXCycles / GPUFreqDivider;
    m_pixCFTime     = (float)pixCFCycles / GPUFreqDivider;
    m_pixBusy       = (float)(pixBusyCycles / busyCycles);
    m_waterfallTime = (float)wtfCycles / GPUFreqDivider;
}


//------------------------------------------------------------------------
ux GPUPerfCounterALU::formatResults(char * _pBuffer, ux _bufferSize) const
{
    float vtxTime = m_vtxALUTime + m_vtxTEXTime + m_vtxCFTime;
    float pixTime = m_pixALUTime + m_pixTEXTime + m_pixCFTime;
    float total = vtxTime + pixTime + m_waterfallTime;
    sprintf_s(_pBuffer, _bufferSize,
        "VTX: %.3f ms - busy: %.2f%%\n"
        "PIX: %.3f ms - busy: %.2f%%\n"
        "WATERFALL: %.3f ms\n"
        "ALU TOTAL: %.3f ms\n",
        vtxTime, 100.0f * m_vtxBusy, pixTime, 100.0f * m_pixBusy, m_waterfallTime, total
        );

     return 4;   // number of lines in result
}


//------------------------------------------------------------------------
// TEXTURE Counter
//------------------------------------------------------------------------
GPUPerfCounterTexVtx::GPUPerfCounterTexVtx(IDirect3DDevice9 * _pD3DDevice):
    GPUPerfCounter_X360(_pD3DDevice, NUM_COUNTER)
{
    D3DPERFCOUNTER_EVENTS & evt = m_pEvents[DEFAULT];
    evt.TCF[0] = GPUPE_VALID_CYCLES;
    evt.TCF[1] = GPUPE_ANY_STALL;
    evt.TCF[2] = GPUPE_SINGLE_PHASES;
    evt.TCF[3] = GPUPE_MIP_VOL_ANISO_PHASES;
    evt.VC[0]  = GPUPE_RG_MEGAFETCH;
    evt.VC[1]  = GPUPE_CC_FREEZE;
    evt.VC[2]  = GPUPE_RG_VERTICES;

    m_texFetchTime = 0.0f;
    m_texBilinPointTime = 0.0f;
    m_texFilterOtherTime = 0.0f;
    m_texStallTime = 0.0f;
    m_vtxFetchTime = 0.0f;
    m_vtxStallTime = 0.0f;
    m_numVertices = 0;
}


//------------------------------------------------------------------------
void GPUPerfCounterTexVtx::computeResults()
{
    GPUPerfCounter_X360::computeResults();
    const D3DPERFCOUNTER_VALUES & val = m_pValues[DEFAULT];

    UINT64 texFetchCycles = val.TCF[0].QuadPart;
    UINT64 texStallCycles = val.TCF[1].QuadPart;
    UINT64 texBilinPointCycles      = val.TCF[2].QuadPart;
    UINT64 texFilterOtherTimeCycles = val.TCF[3].QuadPart;
    UINT64 vtxFetchCycles = val.VC[0].QuadPart / 2;
    UINT64 vtxStallCycles = val.VC[1].QuadPart;
    m_numVertices         = (ux)val.VC[2].QuadPart;

    // average time one 10 captures
    m_texFetchTime = (float)texFetchCycles / GPUFreqDivider;
    m_texBilinPointTime     = (float)texBilinPointCycles / GPUFreqDivider;
    m_texFilterOtherTime    = (float)texFilterOtherTimeCycles / GPUFreqDivider;
    m_texStallTime = (float)texStallCycles / GPUFreqDivider;
    m_vtxFetchTime = (float)vtxFetchCycles / GPUFreqDivider;
    m_vtxStallTime = (float)vtxStallCycles / GPUFreqDivider;
}


//------------------------------------------------------------------------
ux GPUPerfCounterTexVtx::formatResults(char * _pBuffer, ux _bufferSize) const
{
    float texTime = f32_Max(m_texFetchTime + m_texStallTime, m_texBilinPointTime + m_texFilterOtherTime);
    float vtxTime = m_vtxFetchTime + m_vtxStallTime;
    float total = texTime + vtxTime;
    sprintf_s(_pBuffer, _bufferSize,
        "TFETCH: %.3f ms\n"
        "TSTALL: %.3f ms\n"
        "BILINPOINT: %.3f ms\n"
        "Mip/aniso: %.3f ms\n"
        "VFETCH: %.3f ms\n"
        "VSTALL: %.3f ms\n"
        "TexVtx TOTAL: %.3f ms\n"
        "#VTX: %u\n",
        m_texFetchTime, m_texStallTime,
        m_texBilinPointTime, m_texFilterOtherTime,
        m_vtxFetchTime, m_vtxStallTime,
        total,
        m_numVertices
        );

    return 8;   // number of lines in result
}


//------------------------------------------------------------------------
// BANDWIDTH Counter
//------------------------------------------------------------------------
GPUPerfCounterBandwidth::GPUPerfCounterBandwidth(IDirect3DDevice9 * _pD3DDevice):
    GPUPerfCounter_X360(_pD3DDevice, NUM_COUNTER)
{
    D3DPERFCOUNTER_EVENTS & evt = m_pEvents[DEFAULT];

    evt.MC0[0]  = GPUPE_TOTAL_ACCESSES;     // total memory read
    evt.MH[0]   = GPUPE_VGT_READ_MEMORY;    // index memory read
    evt.MH[1]   = GPUPE_BC0_CP_WRITE;       // resolve
    evt.MH[2]   = GPUPE_BC1_CP_WRITE;
    evt.VC[0]   = GPUPE_MI_REQUESTS_TO_MH;  // vertex memory read
    evt.TCF[0]  = GPUPE_MEM_REQ_SENT;       // texture memory read
    evt.BC[0]   = GPUPE_BC_DRAW_COUNT;      // #quads sent to AZ

    m_totalBW   = 0.0f;
    m_idxBW     = 0.0f;
    m_vtxBW     = 0.0f;
    m_texBW     = 0.0f;
    m_resolveBW = 0.0f;
    m_numQuads  = 0;
}


//------------------------------------------------------------------------
void GPUPerfCounterBandwidth::computeResults()
{
    GPUPerfCounter_X360::computeResults();
    const D3DPERFCOUNTER_VALUES & val = m_pValues[DEFAULT];

    m_totalBW   = (float)val.MC0[0].QuadPart * 32 / (1024 * 1024);
    m_idxBW     = (float)val.MH[0].QuadPart  * 32 / (1024 * 1024);
    m_vtxBW     = (float)val.VC[0].QuadPart  * 32 / (1024 * 1024);
    m_texBW     = (float)val.TCF[0].QuadPart * 32 / (1024 * 1024);
    m_resolveBW = (float)(val.MH[1].QuadPart + val.MH[2].QuadPart) * 32 / (1024 * 1024);
    m_numQuads  = val.BC[0].LowPart;
}


//------------------------------------------------------------------------
ux GPUPerfCounterBandwidth::formatResults(char * _pBuffer, ux _bufferSize) const
{
    sprintf_s(_pBuffer, _bufferSize,
        "IDX READ BW: %.3f of 133 MB\n"
        "VTX READ BW: %.3f of 267 MB\n"
        "TEX READ BW: %.3f of 267 MB\n"
        "TOTAL READ BW: %.3f of 350 MB\n"
        "RESOLVE BW: %.3f of 267 MB\n"
        "#QUADS: %u\n",
        m_idxBW, m_vtxBW,
        m_texBW, m_totalBW,
        m_resolveBW,
        m_numQuads
        );

    return 6;   // number of lines in result
}


}

//  Reference from X360 news group
// By far, the easiest thing to do would be take a PIX GPU Command Buffer
// capture, and look at the Dr. PIX Analysis Tab. It will give a detailed
// breakdown of the performance of each DrawPrim, or each PixNamedEvent
// hierarchy.
// 
// But, of course, taking a PIX capture is far from real-time, and analyzing
// the PIX capture can't even be done while your game is running.
// 
// Here are a few performance counters I'd recommend, for doing in-game
// analysis. (Most of these are the same ones PIX uses in the Analysis Tab):
// 
// These will give the number of ALU, Tfetch, Vfetch, and Control Flow
// instructions executed in your pixel shader & vertex shader.
// 
// GPUPE_SQ_ALU_VTX_INST_ISSUED
// GPUPE_SQ_TEX_VTX_INST_ISSUED
// GPUPE_SQ_VC_VTX_INST_ISSUED
// GPUPE_SQ_CF_VTX_INST_ISSUED
// GPUPE_SQ_ALU_PIX_INST_ISSUED
// GPUPE_SQ_TEX_PIX_INST_ISSUED
// //GPUPE_SQ_VC_PIX_INST_ISSUED (not useful, since nobody does vfetches in a
// pixel shader)
// GPUPE_SQ_CF_PIX_INST_ISSUED
// 
// Multiplying the _ALU_VTX_ one by 4/3 will yield the number of cycles spent
// on vertex shader ALU instructions, and multiplying _ALU_PIX_ by 4/3 will
// yield the cycles spent on pixel shader ALU instructions. So adding them
// together (and multiplying by 4/3) will give total ALU processing time,
// except that it won't take into account "constant waterfalling", which can
// sometimes be pretty severe.
// 
// You can get an approximate cycle count for constant waterfalling by taking
// (GPUPE_SQ_CONSTANTS_SENT_SP_SIMD0 - GPUPE_SQ_CONSTANTS_USED_SIMD0) * 2 *  4
// / 3. Note, this is only approximate - it may be too small by a factor of 2,
// or too large by a factor of 2.
// 
// For texture costs, GPUPE_VALID_CYCLES will give you the total number of
// cycles spent on filtering textures.
// GPUPE_ANY_STALL is the number of cycles beyond _VALID_CYCLES which the
// texture cache stalled. (This is typically caused by cache thrashing.). There
// are several other texture-related stalls which are not included in these two
// counters, but they're typically small in comparison.
// 
// For vertex costs, GPUPE_RG_VERTICES or GPUPE_RG_MEGAFETCH should yield
// useful results. Dividing the MegaFetch by 2 should yield the number of
// cycles spent on vertex fetches. GPUPE_CC_FREEZE is an approximate number of
// cycles spent in VC stalls.
// 
// For bandwidth:
// GPUPE_MC_TOTAL_ACCESSES * 32 is the total number of any memory read by
// anything during the frame
// GPUPE_MH_VGT_READ_MEMORY * 32 is the total number of Index-buffer bytes read
// (it may be slightly inaccurate though.)
// GPUPE_VC_MI_REQUESTS_TO_MH * 32 is the total number of vertex buffer bytes
// read from system memory
// GPUPE_TCF_MEM_REQ_SENT * 32 is the total number of texture bytes read from
// system memory
// GPUPE_MH_BC0_CP_WRITE * 64 is the total number of bytes written to main
// memory because of a Resolve
// GPUPE_MH_BC0_EX_WRITE * 64 is the total number of bytes written to main
// memory due to a memory export (memexport).
// 
// Actually, the Resolve and Memexport ones would likely be more accurate if
// you read from both GPUPE_MH_BC0_CP_WRITE and _BC1_, added those together &
// multiplied by 32.
// 
// Once you get the number of bytes read or written, then you can approximate
// the number of cycles spent on each thing:
// Texture, Vertex, Resolve, and Memexport each get 16GB/sec of bandwidth
// Index gets 8GB/sec
// RAM itself gets around 21GB/sec. (measured by the MC_TOTAL_ACCESSES
// counter)
// 
// This should get you all of the major bottlenecks. (There are a few others:
// clipping, 8x8 tiles, and fill-rate, to name a few), but they tend to not be
// the largest bottlenecks in the frame.
// 
// Remember that the GPU does just about everything in parallel, so all you
// have to do is find the max of ALU+Waterfall, Texture Cycles+Texture Stalls,
// VC Cycles + VC Stalls, and all of the bandwidth cycle costs, to see where
// the biggest bottleneck is.
// 
// Unfortunately, there are some things that just can't be measured using perf
// counters (GPR Stalls, for one thing) - this is an area where PIX's Analysis
// Tab & Advanced Texture Tab can help quickly identify any issues.
// 
// We (meaning the Xbox 360 Graphics Team) have included all documentation we
// have about the perf counters, into d3d9gpu.h.
// 
// 
// thanks
// -Jason
// 
// 
// "Mike Beach" <mbeach@snowblindstudios.com> wrote in message
// news:zBzaHM5EJHA.3828@TK2ATGFSA01.phx.gbl...
// >I am trying to measure the GPU cost of each DrawPrimitive() call
// >(specifically the shader cost if possible) using D3D perf counters.
// >
// > The docs seem vague on what each data member means.  I am wondering which
// > events I want to monitor.  Anyone have an idea of which one's I should be
// > interested in (and maybe a better description of each)?
// >
// > Thanks
// > --
// > Mike Beach
// > Snowblind Studios
// 

#endif  // ITF_X360
