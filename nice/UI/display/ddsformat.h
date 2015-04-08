#ifndef _ITF_DDSFORMAT_H_
#define _ITF_DDSFORMAT_H_

namespace ITF
{
    struct DDSCAPS2
    {
        DWORD dwCaps;
        DWORD dwCaps2;
        DWORD Reserved[2];

        void swapToBigEndian()
        {
            Endian::swapBigEndian(dwCaps,(u8*)&dwCaps);
            Endian::swapBigEndian(dwCaps2,(u8*)&dwCaps2);

        }
    };

    struct DDPIXELFORMAT
    {
        DWORD dwSize;
        DWORD dwFlags;
        DWORD dwFourCC;
        DWORD dwRGBBitCount;
        DWORD dwRBitMask;
        DWORD dwGBitMaskDWORD;
        DWORD dwBBitMask;
        DWORD dwRGBAlphaBitMask;


         void swapToBigEndian()
        {

            Endian::swapBigEndian(dwSize,(u8*)&dwSize);
            Endian::swapBigEndian(dwFlags,(u8*)&dwFlags);
            Endian::swapBigEndian(dwFourCC,(u8*)&dwFourCC);
            Endian::swapBigEndian(dwRGBBitCount,(u8*)&dwRGBBitCount);
            Endian::swapBigEndian(dwGBitMaskDWORD,(u8*)&dwGBitMaskDWORD);
            Endian::swapBigEndian(dwBBitMask,(u8*)&dwBBitMask);
            Endian::swapBigEndian(dwRGBAlphaBitMask,(u8*)&dwRGBAlphaBitMask);

        }
    };

    struct DDS_USER
    {
        DWORD dwLodBias;
        DWORD dwAlphaBias;
        DWORD dwbHasOneColor;
        DWORD dwAnisotropy;
        DWORD dwReserved[7];
    };

    struct DDS_HEADER
    {
        DWORD dwMagic;
        DWORD dwSize;
        DWORD dwFlags;
        DWORD dwHeight;
        DWORD dwWidth;
        DWORD dwPitchOrLinearSize;
        DWORD dwDepth;
        DWORD dwMipMapCount;

        union
        {
            DDS_USER    dwUser;
            DWORD       dwReserved1[11];
        };

        DDPIXELFORMAT format;
        DDSCAPS2 ddsCaps;
        DWORD dwReserved2;

        void swapToBigEndian()
        {
            Endian::swapBigEndian(dwMagic,(u8*)&dwMagic);
            Endian::swapBigEndian(dwSize,(u8*)&dwSize);
            Endian::swapBigEndian(dwFlags,(u8*)&dwFlags);
            Endian::swapBigEndian(dwHeight,(u8*)&dwHeight);
            Endian::swapBigEndian(dwWidth,(u8*)&dwWidth);
            Endian::swapBigEndian(dwPitchOrLinearSize,(u8*)&dwPitchOrLinearSize);
            Endian::swapBigEndian(dwDepth,(u8*)&dwDepth);
            Endian::swapBigEndian(dwMipMapCount,(u8*)&dwMipMapCount);

            for (u32 i=0;i<11;++i)
            {
                Endian::swapBigEndian(dwReserved1[i],(u8*) &dwReserved1[i]);          
            }

            format.swapToBigEndian();
            ddsCaps.swapToBigEndian();

            Endian::swapBigEndian(dwReserved2,(u8*)&dwReserved2);

        }
    };
} // namespace ITF

#endif // _ITF_DDSFORMAT_H_
