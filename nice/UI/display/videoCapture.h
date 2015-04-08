#ifndef _ITF_VIDEOCAPTURE_H_
#define _ITF_VIDEOCAPTURE_H_

#ifdef ITF_SUPPORT_VIDEOCAPTURE

#ifndef _ITF_SYNCHRONIZE_H_
#include "engine/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

#ifndef _ITF_GAMEMANAGER_EVENT_INTERFACE_H_
#include "gameplay/Managers/GameManager_Events.h"
#endif //_ITF_GAMEMANAGER_EVENT_INTERFACE_H_

namespace ITF
{

class AudioCapture;

//=============================================================================================
class VideoCaptureBuffer
{
public:
    virtual void release() = 0;
    virtual void lock() = 0;
    virtual void unlock() = 0;

    VideoCaptureBuffer():m_captureOffscreenSurface(NULL),m_dataPtr(NULL),m_pitch(U32_INVALID),
                         m_width(0),m_height(0)
    {};

    virtual ~VideoCaptureBuffer() {};

    const bbool hasOffscreenBuffer()  const         {return (m_captureOffscreenSurface != NULL);}
    void setOffscreenBuffer(u32 _width,u32 _height,void* _offscreenBuffer,void* _stretchBuffer,void* _rendertarget);
    void* getOffscreenBuffer()                      {return m_captureOffscreenSurface;}
    void* getStretchScreenBuffer()                  {return m_captureStretchSurface;}

    const u32 getWidth()    const   {return m_width;}
    const u32 getHeight()   const   {return m_height;}
    const u32 getPitch()    const   {return m_pitch;}
    void*     getDataPtr()  const   {return m_dataPtr;}
    
protected:

    void*   m_captureOffscreenSurface;
    void*   m_captureStretchSurface;
    void*   m_captureRenderTarget;
    void*   m_dataPtr;
    u32     m_pitch;  
    u32     m_height;
    u32     m_width;

};

//=============================================================================================
class VideoCapture : public Interface_GameManagerEventListener
{
protected:
    //----------------------------------------------------------------------------------------------------------------------------------------------------------
    struct	images_Desc
    {
        void	*p_Palette;
        void	*p_Bitmap;
        u16		u16_FileFlags;
        u16		u16_Width;
        u16		u16_Height;
        u16		u16_PaletteLength;
        u8		u8_BPP;
        u8		u8_PaletteBPC;
        u8		u8_Format;
        u8		u8_Dummy2;
    } ;

    //----------------------------------------------------------------------------------------------------------------------------------------------------------
    //Screen Trailer Section
    struct FrameCaptureInfo
    {
        u32 m_uFrameCount;
        u8* m_pBuffer;
        u32 m_bufferSize; 
        u32 m_width;
        u32 m_quality;
        u32 m_height;
        bbool m_bLosslessCompression;
        bbool m_bBMPOutput;
        void compress(const Path& _path,u8* compressTargetBuffer);
    };
public:    
    //----------------------------------------------------------------------------------------------------------------------------------------------------------
    struct ThreadCaptureContainer
    {
    public:

        VideoCapture*           m_pVideoCapture;
        u8*                     memBufferTarget;
        u32                     dirIdx;
    };

    //----------------------------------------------------------------------------------------------------------------------------------------------------------
    struct ImageMemory
    {
        ImageMemory() { clear(); }

        void clear() 
        {
            pBuffer = NULL;
            size    = 0;
            frame   = 0;
        }

        u8* pBuffer;
        u32 size;
        u32 frame;
    };


public:

    VideoCapture(u32 _threadUsage,bbool _bStepByStep);
    virtual ~VideoCapture();

    static VideoCapture* getInstance() {return m_Instance;}

    void    start(u32 _width,u32 _height, const Path * _directory = NULL);
    void    stop();
    void    cancel();

    bbool               isEnable()  {return m_bEnableCapture;}
    void                enableCaptureDebug() {m_bSaveInMemory = btrue;m_bLowResolution = btrue;}
    bbool               videoDebugEnable() {return m_bLowResolution && m_bSaveInMemory;}
    bbool               useLowResolution() {return m_bLowResolution;}
    const bbool         isStepByStep() const {return m_bStepByStep;}

    void                enableDrawRecIndicator( bbool _canDraw ) { m_canDrawRecIndicator = _canDraw; }
    void                drawRecIndicator();

    void                assignAudioCapture(AudioCapture* _audioCapture) {m_AudioCapture = _audioCapture;}

    void                assignCaptureOffscreenBuffer(VideoCaptureBuffer* _pBufferOffscreenBuffer);
    void                resolveBufferScreen();
    bbool               hasCaptureBuffer()  {return (m_captureBuffer != NULL);}
    VideoCaptureBuffer* getCaptureBuffer() {return m_captureBuffer;}
    

    const Path&         getScreenCaptureDirectory()  {return m_ScreenCaptureDirectory;}
    void                getFileNameCapture(Path& _filename);

    u32                 getLowResCaptureWidth() {return m_LowResCaptureWidth;}
    u32                 getLowResCompression()  {return m_LowResCompression;}

    //----------------------------------------------------------------------------------------------------------------------------------------------------------
    // From Interface_GameManagerEventListener
    virtual void onMapEnabled(World* _pWorld);
    virtual void onMapDisabled(World* _pWorld) {}

    //----------------------------------------------------------------------------------------------------------------------------------------------------------
public:
    void        convertToJPG(u8* compressTargetBuffer,u32 dirIdx);
private:
    static void jpeg_save(const Path& _fileName, images_Desc *_pt_Desc,u8* pMemory,u32 quality,u32 frameCount);
    void        recordInMemory(u8* pBuffer,u32 size,u32 frameCount);

    void        cleanITFScreenDirectories();

protected:
    //----------------------------------------------------------------------------------------------------------------------------------------------------------

    u32                     m_LowResCaptureWidth;
    u32                     m_LowResCompression;


    bbool                   m_bEnableCapture;

    bbool                   m_bModeEntrelace;
    bbool                   m_bLossless;
    bbool                   m_bBitmapOutput;
    bbool                   m_CompressLibEnable;
    bbool                   m_bModeOddFrame;
    volatile bbool          m_bExitCapture;

    bbool                   m_canDrawRecIndicator;

    u32                     m_uScreenNum;
    u32                     m_CaptureFrameCount;
    u32                     m_quality;
    u32                     m_backbufferWidth;
    u32                     m_backbufferHeight;

    u32                     m_idCurrentArray;
    u32*                    m_StoreFrameBuffer;
  
    u32                     m_uMaxThreadUsed;
    u32                     m_CurrentCoreUsage;
    u64                     m_MemoryAvailableAtStartCapture;
    u64                     m_MemoryThreshold;
    u32                     m_FlushThresholdBytes;
    
    u32                     m_StartEntrelaceLine;
    u32                     m_uFrameCountEntrelace;
    
    VideoCaptureBuffer*     m_captureBuffer;
    Path                    m_ScreenCaptureDirectory;
    bbool                   m_bLowResolution;
    bbool                   m_bSaveInMemory;
    bbool                   m_bStepByStep;

    AudioCapture*           m_AudioCapture;
    

    struct LowResCapture
    {
        u32                 m_uMaxFrameToCapture;
        u32                 m_currentFrameArray;
        u32                 m_incFrameMemory;

        ITF_VECTOR<ImageMemory>    m_vImageMemory;
    };

    SafeArray<ThreadCaptureContainer*>  m_ThreadContainers;

    LowResCapture           m_LowResCapture;
    static VideoCapture*    m_Instance;


    ITF_THREAD_CRITICAL_SECTION     m_csScreenCaptureLowRes;
    ITF_THREAD_CRITICAL_SECTION*    m_csScreenCapture;
    ITF_VECTOR<FrameCaptureInfo>    *m_pendingFrameInfo;
};

}

#endif //ITF_SUPPORT_VIDEOCAPTURE

#endif // _ITF_VIDEOCAPTURE_H_
