#include "precompiled_engine.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_AFTERFX_H_
#include "AfterFx.h"
#endif //_ITF_AFTERFX_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif // SERIALIZEROBJECT_H

namespace ITF
{
    BEGIN_SERIALIZATION(AFTERFX)
    SERIALIZE_ENUM_BEGIN("Type",m_type);
        SERIALIZE_ENUM_VAR(AFX_None);
        SERIALIZE_ENUM_VAR(AFX_Blur);
        SERIALIZE_ENUM_VAR(AFX_Glow);
        SERIALIZE_ENUM_VAR(AFX_Remanence);
        SERIALIZE_ENUM_VAR(AFX_DOF);
        SERIALIZE_ENUM_VAR(AFX_RayCenter);
        SERIALIZE_ENUM_VAR(AFX_ColorSetting);
        SERIALIZE_ENUM_VAR(AFX_ColorRemap);
        SERIALIZE_ENUM_VAR(AFX_ColorLevels);
        SERIALIZE_ENUM_VAR(AFX_Fade);
        SERIALIZE_ENUM_VAR(AFX_Bright);
        SERIALIZE_ENUM_VAR(AFX_ToneMap);
        SERIALIZE_ENUM_VAR(AFX_AddSceneAndMul);
        SERIALIZE_ENUM_VAR(AFX_objectsGlow);
        SERIALIZE_ENUM_VAR(AFX_simpleBlend);
    SERIALIZE_ENUM_END();
    SERIALIZE_CONTAINER("paramF",m_Paramf);
    SERIALIZE_CONTAINER("paramI",m_Parami);
    SERIALIZE_CONTAINER("paramV",m_Paramv);
    SERIALIZE_CONTAINER("paramC",m_Paramc);
    SERIALIZE_MEMBER("lifeTime",m_lifeTime);
    SERIALIZE_MEMBER("colorTarget",m_colorTarget);
    END_SERIALIZATION()


///----------------------------------------------------------------------------//
/// AfterFx.
///----------------------------------------------------------------------------//

    AFTERFX_Type AFTERFX::getTypeByName(String8& _name)
    {
        String8 name = _name;
        name.toLower();

        if (name == "blur") return AFX_Blur;
        else if (name == "glow") return AFX_Glow;
        else if (name == "remanence") return AFX_Remanence;
        else if (name == "dof") return AFX_DOF;
        else if (name == "godray") return AFX_RayCenter;
        else if (name == "colorsetting") return AFX_ColorSetting;
        else if (name == "colorremap") return AFX_ColorRemap;
        else if (name == "colorlevels") return AFX_ColorLevels;
        else if (name == "fade") return AFX_Fade;
        else if (name == "brightpass") return AFX_Bright;
        else if (name == "tonemap") return AFX_ToneMap;
        else if (name == "addsceneandmul") return AFX_AddSceneAndMul;
        else if (name == "simpleblend") return AFX_simpleBlend;
        return AFX_None;
    }

    void AFTERFX::destroy()
    {
    }

    Texture* AFTERFX::getTexture(u32 _n)
    {
        if (m_customTextureID[_n].isValid() && _n < 4)
        {
            Texture* texture = (Texture*)m_customTextureID[_n].getResource();

            if ( texture && texture->isPhysicallyLoaded())
                return texture;
        }
    
        return NULL;
    }

    void AFTERFX::setTexture(u32 _n, ResourceID _texture)
    {
        ITF_ASSERT(_n < 4);
        m_customTextureID[_n] = _texture;
    }


    /////////////////////////////////
    //  New AFX Post Process
    /////////////////////////////////


    //Blur
    BEGIN_SERIALIZATION(AFX_BlurParam)
        SERIALIZE_MEMBER("use",m_use);
        SERIALIZE_MEMBER("pixelSize",m_pixelSize);
        SERIALIZE_MEMBER("quality",m_quality);
    END_SERIALIZATION();

    //Glow
    BEGIN_SERIALIZATION(AFX_GlowParam)
        SERIALIZE_MEMBER("use",m_use);
        SERIALIZE_MEMBER("factor",m_factor);
        SERIALIZE_MEMBER("pixelSize",m_pixelSize);
        SERIALIZE_MEMBER("quality",m_quality);
        SERIALIZE_MEMBER("useToneMap",m_useTonemap);
        SERIALIZE_MEMBER("threshold",m_threshold);
        SERIALIZE_MEMBER("thresholdScale",m_thresholdScale);
    END_SERIALIZATION();

    //Color Setting
    BEGIN_SERIALIZATION(AFX_ColorSettingParam)
        SERIALIZE_MEMBER("use",m_use);
        SERIALIZE_MEMBER("saturation",m_saturation);
        SERIALIZE_MEMBER("contrast",m_contrast);
        SERIALIZE_MEMBER("contrastScale",m_contrastScale);
        SERIALIZE_MEMBER("bright",m_bright);
        SERIALIZE_MEMBER("colorCorrection",m_colorCorrection);
    END_SERIALIZATION();

    //Refraction Setting
    BEGIN_SERIALIZATION(AFX_RefractionParam)
        SERIALIZE_MEMBER("use",m_use);
    END_SERIALIZATION();

    //Tile Setting
    BEGIN_SERIALIZATION(AFX_TileParam)
        SERIALIZE_MEMBER("use",m_use);
        SERIALIZE_MEMBER("repetition",m_repetition);
    END_SERIALIZATION();

    //Mosaic Setting
    BEGIN_SERIALIZATION(AFX_MosaicParam)
        SERIALIZE_MEMBER("use",m_use);
        SERIALIZE_MEMBER("pixelSize",m_pixelSize);
    END_SERIALIZATION();

    //negatif Setting
    BEGIN_SERIALIZATION(AFX_NegatifParam)
        SERIALIZE_MEMBER("use",m_use);
    END_SERIALIZATION();

    //Kalei Setting
    BEGIN_SERIALIZATION(AFX_KaleiParam)
        SERIALIZE_MEMBER("use",m_use);
        SERIALIZE_MEMBER("pixelSize",m_size);
        SERIALIZE_MEMBER("UV1",m_uv1);
        SERIALIZE_MEMBER("UV2",m_uv2);
        SERIALIZE_MEMBER("UV3",m_uv3);
    END_SERIALIZATION();

    //eyeFish Setting
    BEGIN_SERIALIZATION(AFX_EyeFishParam)
        SERIALIZE_MEMBER("use",m_use);
        SERIALIZE_MEMBER("height",m_height);
        SERIALIZE_MEMBER("scale",m_scale);
    END_SERIALIZATION();

    //mirror Setting
    BEGIN_SERIALIZATION(AFX_MirrorParam)
        SERIALIZE_MEMBER("use",m_use);
        SERIALIZE_MEMBER("offsetX",m_offsetX);
        SERIALIZE_MEMBER("offsetY",m_offsetY);
    END_SERIALIZATION();

    //oldTV Setting
    BEGIN_SERIALIZATION(AFX_OldTVParam)
        SERIALIZE_MEMBER("use",m_use);
        SERIALIZE_MEMBER("lineFade",m_lineFade);
        SERIALIZE_MEMBER("useScanLine",m_useScanLine);
        SERIALIZE_MEMBER("scanLineFade",m_scanLineFade);
        SERIALIZE_MEMBER("scanLineSpeed",m_scanLineSpeed);
        SERIALIZE_MEMBER("scanLineSize",m_scanLineSize);
    END_SERIALIZATION();

    //noise Setting
    BEGIN_SERIALIZATION(AFX_NoiseParam)
        SERIALIZE_MEMBER("use",m_use);
        SERIALIZE_MEMBER("blendFactor",m_blend);
        SERIALIZE_MEMBER("size",m_size);
    END_SERIALIZATION();

    //radial Setting
    BEGIN_SERIALIZATION(AFX_RadialParam)
        SERIALIZE_MEMBER("use",m_use);
        SERIALIZE_MEMBER("centerOffset",m_centerOffset);
        SERIALIZE_MEMBER("strength",m_strength);
        SERIALIZE_MEMBER("size",m_size);
    END_SERIALIZATION();

    //AFX PostProcess
    bbool AFXPostProcess::b_needBackBufferCopy()
    {
        return m_blur.m_use || m_glow.m_use || m_colorSetting.m_use;
    }

    void AFXPostProcess::setFadeFactor(f32 _fadeFactor)
    {
        m_fadeFactor = _fadeFactor;
    }

    void AFXPostProcess::setBlurParam(AFX_BlurParam _parameters)
    {
        m_blur = _parameters;
    }

    void AFXPostProcess::setGlowParam(AFX_GlowParam _parameters)
    {
        m_glow = _parameters;
    }

    void AFXPostProcess::setColorSettingParam(AFX_ColorSettingParam _parameters)
    {
        m_colorSetting = _parameters;
    }

    void AFXPostProcess::setRefraction(AFX_RefractionParam _parameters)
    {
        m_refraction = _parameters;
    }

    void AFXPostProcess::setTileParam(AFX_TileParam _parameters)
    {
        m_tile = _parameters;
    }

    void AFXPostProcess::setMosaicParam(AFX_MosaicParam _parameters)
    {
        m_mosaic = _parameters;
    }


    void AFXPostProcess::setNegatifParam(AFX_NegatifParam _parameters)
    {
        m_negatif = _parameters;
    }

    void AFXPostProcess::setKaleiParam(AFX_KaleiParam _parameters)
    {
        m_kalei = _parameters;
    }

    void AFXPostProcess::setEyeFishParam(AFX_EyeFishParam _parameters)
    {
        m_eyeFish = _parameters;
    }
    void AFXPostProcess::setMirrorParam(AFX_MirrorParam _parameters)
    {
        m_mirror = _parameters;
    }

    void AFXPostProcess::setOldTVParam(AFX_OldTVParam _parameters)
    {
        m_oldTV = _parameters;
    }

    void AFXPostProcess::setNoiseParam(AFX_NoiseParam _parameters)
    {
        m_noise = _parameters;
    }

    void AFXPostProcess::setRadialParam(AFX_RadialParam _parameters)
    {
        m_radial = _parameters;
    }

    bbool AFXPostProcess::isActived() const
    {
        return (m_fadeFactor!=0.0f) && (m_blur.m_use
            || m_glow.m_use
            || m_colorSetting.m_use
            || m_refraction.m_use
            || m_tile.m_use
            || m_mosaic.m_use
            || m_negatif.m_use
            || m_kalei.m_use
            || m_eyeFish.m_use
            || m_mirror.m_use
            || m_oldTV.m_use
            || m_noise.m_use
            || m_radial.m_use);
    }

    void AFXPostProcess::mergePostProcess(AFXPostProcess &_afx)
    {
        if(_afx.m_fadeFactor > 0.0f)
        {
            if(_afx.m_blur.m_use)
            {
                if(!m_blur.m_use)
                {
                    m_blur = _afx.m_blur;
                    m_blur.m_fade = _afx.m_fadeFactor;
                }
                else
                {
                    _afx.m_blur.m_fade = _afx.m_fadeFactor;
                    m_blur.merge(_afx.m_blur);
                }
            }
            
            if(_afx.m_glow.m_use)
            {
                if(!m_glow.m_use)
                {
                    m_glow = _afx.m_glow;
                    m_glow.m_fade = _afx.m_fadeFactor;
                }
                else
                {
                    _afx.m_glow.m_fade = _afx.m_fadeFactor;
                   m_glow.merge(_afx.m_glow);
                }
            }

            if(_afx.m_colorSetting.m_use)
            {
                if(!m_colorSetting.m_use)
                {
                    m_colorSetting = _afx.m_colorSetting;
                    m_colorSetting.m_fade = _afx.m_fadeFactor;
                }
                else
                {
                    _afx.m_colorSetting.m_fade = _afx.m_fadeFactor;
                    m_colorSetting.merge(_afx.m_colorSetting);
                }
            }

            if(_afx.m_refraction.m_use)
            {
                if(!m_refraction.m_use)
                {
                    m_refraction = _afx.m_refraction;
                    m_refraction.m_fade = _afx.m_fadeFactor;
                }
                else
                {
                    _afx.m_refraction.m_fade = _afx.m_fadeFactor;
                    m_refraction.merge(_afx.m_refraction);
                }
            }

            if(_afx.m_tile.m_use)
            {
                if(!m_tile.m_use)
                {
                    m_tile = _afx.m_tile;
                    m_tile.m_fade = _afx.m_fadeFactor;
                }
                else
                {
                    _afx.m_tile.m_fade = _afx.m_fadeFactor;
                    m_tile.merge(_afx.m_tile);
                }
            }

            if(_afx.m_mosaic.m_use)
            {
                if(!m_mosaic.m_use)
                {
                    m_mosaic = _afx.m_mosaic;
                    m_mosaic.m_fade = _afx.m_fadeFactor;
                }
                else
                {
                    _afx.m_mosaic.m_fade = _afx.m_fadeFactor;
                    m_mosaic.merge(_afx.m_mosaic);
                }
            }

            if(_afx.m_negatif.m_use)
            {
                if(!m_negatif.m_use)
                {
                    m_negatif = _afx.m_negatif;
                    m_negatif.m_fade = _afx.m_fadeFactor;
                }
                else
                {
                    _afx.m_negatif.m_fade = _afx.m_fadeFactor;
                    m_negatif.merge(_afx.m_negatif);
                }
            }

            if(_afx.m_kalei.m_use)
            {
                if(!m_kalei.m_use)
                {
                    m_kalei = _afx.m_kalei;
                    m_kalei.m_fade = _afx.m_fadeFactor;
                }
                else
                {
                    _afx.m_kalei.m_fade = _afx.m_fadeFactor;
                    m_kalei.merge(_afx.m_kalei);
                }
            }

            if(_afx.m_eyeFish.m_use)
            {
                if(!m_eyeFish.m_use)
                {
                    m_eyeFish = _afx.m_eyeFish;
                    m_eyeFish.m_fade = _afx.m_fadeFactor;
                }
                else
                {
                    _afx.m_eyeFish.m_fade = _afx.m_fadeFactor;
                    m_eyeFish.merge(_afx.m_eyeFish);
                }
            }

            if(_afx.m_mirror.m_use)
            {
                if(!m_mirror.m_use)
                {
                    m_mirror = _afx.m_mirror;
                    m_mirror.m_fade = _afx.m_fadeFactor;
                }
                else
                {
                    _afx.m_mirror.m_fade = _afx.m_fadeFactor;
                    m_mirror.merge(_afx.m_mirror);
                }
            }

            if(_afx.m_oldTV.m_use)
            {
                if(!m_oldTV.m_use)
                {
                    m_oldTV = _afx.m_oldTV;
                    m_oldTV.m_fade = _afx.m_fadeFactor;
                }
                else
                {
                    _afx.m_oldTV.m_fade = _afx.m_fadeFactor;
                    m_oldTV.merge(_afx.m_oldTV);
                }
            }

            if(_afx.m_noise.m_use)
            {
                if(!m_noise.m_use)
                {
                    m_noise = _afx.m_noise;
                    m_noise.m_fade = _afx.m_fadeFactor;
                }
                else
                {
                    _afx.m_noise.m_fade = _afx.m_fadeFactor;
                    m_noise.merge(_afx.m_noise);
                }
            }

            if(_afx.m_radial.m_use)
            {
                if(!m_radial.m_use)
                {
                    m_radial = _afx.m_radial;
                    m_radial.m_fade = _afx.m_fadeFactor;
                }
                else
                {
                    _afx.m_radial.m_fade = _afx.m_fadeFactor;
                    m_radial.merge(_afx.m_radial);
                }
            }

            m_fadeFactor = (_afx.m_fadeFactor + m_fadeFactor) / 2.0f;
        }
    }
} // namespace ITF


