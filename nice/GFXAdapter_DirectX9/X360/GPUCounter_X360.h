#ifndef GPUCOUNTER_X360_H
#define GPUCOUNTER_X360_H

#ifdef _MSC_VER
#endif

#ifdef _XBOX

#ifdef _OPTIMIZED_
#   define _USE_GPU_PERFCOUNTER_    0
#else
#   define _USE_GPU_PERFCOUNTER_    1
#endif


//------------------------------------------------------------------------
namespace ITF
{

class GPUPerfCounter_X360
{
public:
                    GPUPerfCounter_X360(IDirect3DDevice9 * _pD3DDevice, ux _numCounters);
    virtual        ~GPUPerfCounter_X360();

    void            start          ();
    void            stop           ();
    void            computeResults ();
    float           getCommandProcessorCost() const;

protected:
    IDirect3DDevice9 *      mp_device;
    u32                     m_frameCounter;
    u8                      m_numCounters;
    u8                      m_curCounter;
    u16                     m_unused;

    float                   m_CPCost;

    D3DPERFCOUNTER_EVENTS * m_pEvents;
    D3DPERFCOUNTER_VALUES * m_pValues;

    enum
    {
        GPUFreq         = 500 * 1000 * 1000,
        GPUFreqDivider  = GPUFreq / 1000
    };

#if _USE_GPU_PERFCOUNTER_
    enum { PC_Count = 3 };  // number of buffers
    struct PerfCounter
    {
        D3DPerfCounters * begin[PC_Count];
        D3DPerfCounters * end[PC_Count];
    };
    PerfCounter * mp_Counters;
#endif
};


//------------------------------------------------------------------------
// Counter class
//------------------------------------------------------------------------
class GPUPerfCounterDrawTime : public GPUPerfCounter_X360
{
public:
            GPUPerfCounterDrawTime(IDirect3DDevice9 * _pD3DDevice);

    void    computeResults  ();
    ux      formatResults   (char * _pBuffer, ux _bufferSize) const;

protected:
    enum
    {
        DEFAULT = 0,
        NUM_COUNTER
    };
};

//------------------------------------------------------------------------
class GPUPerfCounterALU : public GPUPerfCounter_X360
{
public:
            GPUPerfCounterALU(IDirect3DDevice9 * _pD3DDevice);

    void    computeResults  ();
    ux      formatResults   (char * _pBuffer, ux _bufferSize) const;

protected:
    enum
    {
        SQ_VTX = 0,
        SQ_PIX,
        SQ_UNIT_BUSY,
        SQ_WATERFALL,
        NUM_COUNTER
    };

    float   m_vtxALUTime;
    float   m_vtxTEXTime;
    float   m_vtxCFTime;
    float   m_vtxBusy;
    float   m_pixALUTime;
    float   m_pixTEXTime;
    float   m_pixCFTime;
    float   m_pixBusy;
    float   m_waterfallTime;
};

//------------------------------------------------------------------------
class GPUPerfCounterTexVtx : public GPUPerfCounter_X360
{
public:
            GPUPerfCounterTexVtx(IDirect3DDevice9 * _pD3DDevice);

    void    computeResults  ();
    ux      formatResults   (char * _pBuffer, ux _bufferSize) const;

protected:
    enum
    {
        DEFAULT = 0,
        NUM_COUNTER
    };

    float   m_texFetchTime;
    float   m_texBilinPointTime;
    float   m_texFilterOtherTime;
    float   m_texStallTime;
    float   m_vtxFetchTime;
    float   m_vtxStallTime;
    ux      m_numVertices;
};

//------------------------------------------------------------------------
class GPUPerfCounterBandwidth: public GPUPerfCounter_X360
{
public:
            GPUPerfCounterBandwidth(IDirect3DDevice9 * _pD3DDevice);

    void    computeResults  ();
    ux      formatResults   (char * _pBuffer, ux _bufferSize) const;

protected:
    enum
    {
        DEFAULT = 0,
        NUM_COUNTER
    };

    float   m_totalBW;
    float   m_idxBW;
    float   m_vtxBW;
    float   m_texBW;
    float   m_resolveBW;
    ux      m_numQuads;
};


//------------------------------------------------------------------------
// PerCounterManager
//------------------------------------------------------------------------
class PerfCounterManager
{
public:
    PerfCounterManager(IDirect3DDevice9 * _pD3DDevice);

    void    start           ();
    void    stop            ();
    ux      formatResults   (char * _pBuffer, ux _bufferSize) const; // returns the number of lines in the buffer

    float   getGPUFrameTime () const;

protected:
    GPUPerfCounterDrawTime  m_drawTimeCounters;
//     GPUPerfCounterALU       m_ALUCounters;
//     GPUPerfCounterTexVtx    m_TexVtxCounters;
//     GPUPerfCounterBandwidth m_BWCounters;
};

}

#endif  // _XBOX
#endif  // GPUCOUNTER_X360_H
