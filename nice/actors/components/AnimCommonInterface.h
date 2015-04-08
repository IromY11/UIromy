
#ifndef _ITF_ANIMCOMMONINTERFACE_H_
#define _ITF_ANIMCOMMONINTERFACE_H_

namespace ITF
{

class IAnimCommonInterface
{
public:

    virtual void        IAnimsetAnim( const StringID& _id ) = 0;
    virtual bbool       IAnimisAnimFinished() const = 0;
    virtual bbool       IAnimisAnimLooped() const = 0;
    virtual u32         IAnimgetNumAnimsPlaying() const = 0;
    virtual void        IAnimdbgGetCurAnim( u32 _index, String8& _out ) const = 0;
};

}

#endif // _ITF_ANIMCOMMONINTERFACE_H_

