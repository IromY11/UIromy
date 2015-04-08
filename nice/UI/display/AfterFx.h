#ifndef _ITF_AFTERFX_H_
#define _ITF_AFTERFX_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef SERIALIZEROBJECTDEFINES_H
#include "core/serializer/ZSerializerObject_Defines.h"
#endif // SERIALIZEROBJECTDEFINES_H

#ifndef ITF_GFX_MATERIAL_SHADER_H_
#include "engine/display/material/GFXMaterialShader.h"
#endif //ITF_GFX_MATERIAL_SHADER_H_

#ifndef ITF_GFX_PRIMITIVES_H_
#include "engine/display/Primitives/GFXPrimitives.h"
#endif

namespace ITF
{

enum AFTERFX_Type
{
    AFX_None = 0,
    AFX_Blur,
    AFX_Glow,
    AFX_Remanence,
    AFX_DOF,
    AFX_RayCenter,
    AFX_ColorSetting,
    AFX_ColorRemap,
    AFX_ColorLevels,
    AFX_Fade,
    AFX_Bright,
    AFX_ToneMap,
    AFX_AddSceneAndMul,
    AFX_objectsGlow,
    AFX_simpleBlend,
    AFX_zoomCenter,
    AFX_BorderBright,
    AFX_Refraction,
    ENUM_FORCE_SIZE_32(AFTERFX_Type)
};

class Texture;

class AFTERFX
{
    DECLARE_SERIALIZE()

public:

    AFTERFX()
    {
        m_type = AFX_None;
        m_lifeTime = 0.f;
        m_apply = 1;
        for (u32 i=0;i<8;i++)
            m_Paramf.push_back(1.f);
        for (u32 i=0;i<8;i++)
            m_Parami.push_back(0);
        for (u32 i=0;i<8;i++)
            m_Paramv.push_back(Vec3d::Zero);
        for (u32 i=0;i<2;i++)
            m_Paramc.push_back(Color::zero());

        m_customval = 0;
        m_colorTarget = Color::zero();

        m_renderintarget = 0;

        m_zStart = 0.f;
        m_zRender = 0.f;

		m_isPost2D = bfalse;
    };

    AFTERFX(const AFTERFX& _source) {copy(_source);}
 
    ~AFTERFX()
    {
        destroy();
    }

    ITF_INLINE void copy(const AFTERFX& _from)
    {
        m_type = _from.m_type;
        m_lifeTime = _from.m_lifeTime;
        m_apply = _from.m_apply;
        m_customval = _from.m_customval;

        m_Paramf = _from.m_Paramf;
        m_Parami = _from.m_Parami;
        m_Paramv = _from.m_Paramv;
    }

    static AFTERFX_Type getTypeByName(String8& _name);
    
    ITF_INLINE void setType( AFTERFX_Type _type ) { m_type = _type;};
    ITF_INLINE void setActiveFx( u32 _active ) { m_apply = _active;};
    ITF_INLINE void setLifeTime( f32 _lifeTime   ) { m_lifeTime = _lifeTime;};

    ITF_INLINE AFTERFX_Type getType( ) { return m_type;};
    
    ITF_INLINE void setParamfAt( u32 _n, f32 _fval   ) { m_Paramf[_n] = _fval;};
    ITF_INLINE f32 getParamfAt( u32 _n) { return m_Paramf[_n];};

    ITF_INLINE void setParamiAt( u32 _n, i32 _ival   ) { m_Parami[_n] = _ival;};
    ITF_INLINE i32 getParamiAt( u32 _n) { return m_Parami[_n];};

    ITF_INLINE void setParamvAt( u32 _n, Vec3d _vec   ) { m_Paramv[_n] = _vec;};
    ITF_INLINE Vec3d getParamvAt( u32 _n) { return m_Paramv[_n];};

    ITF_INLINE void setParamcAt( u32 _n, Color _col   ) { m_Paramc[_n] = _col;};
    ITF_INLINE Color getParamcAt( u32 _n) { return m_Paramc[_n];};

    ITF_INLINE void setCustomVal( i32 _val ) { m_customval = _val;};
    ITF_INLINE i32 getCustomVal() { return m_customval;}

    ITF_INLINE void setColorTarget( Color _color ) { m_colorTarget = _color;};
    ITF_INLINE Color getColorTarget() { return m_colorTarget;}

    ITF_INLINE void setZStart( f32 _z ) { m_zStart = _z;};
    ITF_INLINE f32 getZStart() { return m_zStart;}

    ITF_INLINE void setZRender( f32 _z ) { m_zRender = _z;};
    ITF_INLINE f32 getZRender() { return m_zRender;}

	ITF_INLINE void setPost2D( bbool _v ) { m_isPost2D = _v;};
	ITF_INLINE bbool isPost2D() { return m_isPost2D;}

    void destroy();

    Texture* getTexture(u32 _n);
    void setTexture(u32 _n, ResourceID _texture);

    ITF_INLINE u32              isRenderInTarget(   )               {   return m_renderintarget;};
    ITF_INLINE void             SetRenderInTarget( u32 _v )       {   m_renderintarget = _v;  };

private:
    ResourceID          m_customTextureID[4];

    AFTERFX_Type        m_type;
    ITF_VECTOR<f32>     m_Paramf;
    ITF_VECTOR<i32>     m_Parami;
    ITF_VECTOR<Vec3d>   m_Paramv;
    ITF_VECTOR<Color>   m_Paramc;
    f32                 m_lifeTime;
    u32                 m_apply;
    i32                 m_customval;
    Color               m_colorTarget;
    u32                 m_renderintarget;

    f32                 m_zStart;
    f32                 m_zRender;

	bbool				m_isPost2D;
};



    /////////////////////////////////
    //  New AFX Post Process
    /////////////////////////////////


    static f32 f32_barycenter(f32 _val1, f32 _w1, f32 _val2, f32 _w2)
    {
        return (_val1 * _w1 + _val2 * _w2)/(_w1 + _w2);
    }

    static f32 f32_merge(f32 _val1, f32 _fade1, f32 _val2, f32 _fade2, f32 _neutral)
    {
        /*
        f32 valWeighted1 = f32_barycenter(_val1, _fade1, _neutral, 1.0f-_fade1);
        f32 valWeighted2 = f32_barycenter(_val2, _fade2, _neutral, 1.0f-_fade2);
        return f32_barycenter(valWeighted1, _fade1, valWeighted2, _fade2);
        */
        return f32_barycenter(_val1, _fade1, _val2, _fade2);
    }


class AFX_BlurParam
{
public:
    DECLARE_SERIALIZE()

    AFX_BlurParam()
    {
        Reset();
    }

    ITF_INLINE void Reset()
    {
        m_use = bfalse;
        m_fade = 0.0f;
        m_pixelSize = 8.0f;
        m_quality = 1;
    }


    ITF_INLINE void merge(AFX_BlurParam& _param)
    {
        if(_param.m_use && m_use)
        {
            m_pixelSize = f32_merge(m_pixelSize, m_fade, _param.m_pixelSize, _param.m_fade, 1.0f);
            m_quality = (u32)f32_merge((f32)m_quality, m_fade, (f32)_param.m_quality, _param.m_fade, 1.0f);
            m_fade = 1.0f;
        }
    }

    bbool           m_use;
    f32             m_fade;
    f32             m_pixelSize;
    u32             m_quality;
};

class AFX_GlowParam
{
public:
    DECLARE_SERIALIZE()

    AFX_GlowParam()
    {
        Reset();
    }

    ITF_INLINE void Reset()
    {
        m_use = bfalse;
        m_fade = 0.0f;
        m_factor = 1.0f;
        m_pixelSize = 8;
        m_quality = 1;
        m_useTonemap = bfalse;
        m_threshold = 0.0f;
        m_thresholdScale = 1.0f;
    }

    ITF_INLINE void merge(AFX_GlowParam& _param)
    {
        if(_param.m_use && m_use)
        {
            m_pixelSize = (u32)f32_merge((f32)m_pixelSize, m_fade, (f32)_param.m_pixelSize, _param.m_fade, 1.0f);
            m_quality = (u32)f32_merge((f32)m_quality, m_fade, (f32)_param.m_quality, _param.m_fade, 1.0f);   
            if(m_useTonemap)
            {
                m_threshold = f32_merge(m_threshold, m_fade, _param.m_threshold, _param.m_fade, 0.0f);
                m_thresholdScale = f32_merge(m_thresholdScale, m_fade, _param.m_thresholdScale, _param.m_fade, 1.0f);
            }
            m_fade = 1.0f;
        }
    }

    bbool           m_use;
    f32             m_fade;
    f32             m_factor;
    u32             m_pixelSize;
    u32             m_quality;
    bbool           m_useTonemap;
    f32             m_threshold;
    f32             m_thresholdScale;
};

class AFX_ColorSettingParam
{
public:
    DECLARE_SERIALIZE()

    AFX_ColorSettingParam()
    {
        Reset();
    }

    ITF_INLINE void Reset()
    {
        m_use = bfalse;
        m_fade = 0.0f;
        m_saturation = 1.0f;
        m_contrast = 0.0f;
        m_contrastScale = 1.0f;
        m_bright = 0.0f;
        m_colorCorrection = Color::white();
    }

    ITF_INLINE void merge(AFX_ColorSettingParam& _param)
    {
        if(_param.m_use && m_use)
        {
			m_saturation = f32_merge(m_saturation, m_fade, _param.m_saturation, _param.m_fade, 1.0f);
            m_contrast = f32_merge(m_contrast, m_fade, _param.m_contrast, _param.m_fade, 0.0f);
            m_contrastScale = f32_merge(m_contrastScale, m_fade, _param.m_contrastScale, _param.m_fade, 1.0f);
            m_bright = f32_merge(m_bright, m_fade, _param.m_bright, _param.m_fade, 0.0f);
            m_colorCorrection = ((m_colorCorrection * m_fade) + (_param.m_colorCorrection * _param.m_fade)) * (f32)(1.0/(m_fade+_param.m_fade));
            m_fade = 1.0f;
        }
    }

    ITF_INLINE void removeFade()
    {
        m_saturation = f32_Lerp(1.0f, m_saturation, m_fade);
        m_contrast = f32_Lerp(0.0f, m_contrast, m_fade);
        m_contrastScale = f32_Lerp(1.0f, m_contrastScale, m_fade);
        m_bright = f32_Lerp(0.0f, m_bright, m_fade);
        m_colorCorrection = Color::Interpolate( COLOR_WHITE, m_colorCorrection, m_fade);
        m_fade = 1.0f;
    }

    bbool           m_use;
    f32             m_fade;
    f32             m_saturation;
    f32             m_contrast;
    f32             m_contrastScale;
    f32             m_bright;
    Color           m_colorCorrection;
};

class AFX_RefractionParam
{
public:
    DECLARE_SERIALIZE()

    AFX_RefractionParam()
    {
        Reset();
    }

    ITF_INLINE void Reset()
    {
        m_use = bfalse;
        m_fade = 0.0f;
    }

    ITF_INLINE void merge(AFX_RefractionParam& _param)
    {
        if(_param.m_use && m_use)
        {
            m_fade = (m_fade+_param.m_fade) / 2.0f;
        }
    }

    bbool           m_use;
    f32             m_fade;
};

class AFX_TileParam
{
public:
    DECLARE_SERIALIZE()

    AFX_TileParam()
    {
        Reset();
    }

    ITF_INLINE void Reset()
    {
        m_use = bfalse;
        m_fade = 0.0f;
        m_repetition = 2.0f;
    }

    ITF_INLINE void merge(AFX_TileParam& _param)
    {
        if(_param.m_use && m_use)
        {
            m_repetition = f32_merge(m_repetition, m_fade, _param.m_repetition, _param.m_fade, 1.0f);
            m_fade = 1.0f;
        }
    }

    bbool           m_use;
    f32             m_fade;
    f32             m_repetition;
};

class AFX_MosaicParam
{
public:
    DECLARE_SERIALIZE()

    AFX_MosaicParam()
    {
        Reset();
    }

    ITF_INLINE void Reset()
    {
        m_use = bfalse;
        m_fade = 0.0f;
        m_pixelSize = 2.0f;
    }

    ITF_INLINE void merge(AFX_MosaicParam& _param)
    {
        if(_param.m_use && m_use)
        {
            m_pixelSize = f32_merge(m_pixelSize, m_fade, _param.m_pixelSize, _param.m_fade, 1.0f);
            m_fade = 1.0f;
        }
    }

    bbool           m_use;
    f32             m_fade;
    f32             m_pixelSize;
};

class AFX_NegatifParam
{
public:
    DECLARE_SERIALIZE()

    AFX_NegatifParam()
    {
        Reset();
    }

    ITF_INLINE void Reset()
    {
        m_use = bfalse;
        m_fade = 0.0f;
    }

    ITF_INLINE void merge(AFX_NegatifParam& _param)
    {
        if(_param.m_use && m_use)
        {
            m_fade = (m_fade+_param.m_fade) / 2.0f;
        }
    }

    bbool           m_use;
    f32             m_fade;
};

class AFX_KaleiParam
{
public:
    DECLARE_SERIALIZE()

    AFX_KaleiParam()
    {
        Reset();
    }

    ITF_INLINE void Reset()
    {
        m_use = bfalse;
        m_fade = 0.0f;
        m_size = 1.0f;
        m_uv1 = Vec2d(0.0f, 1.0f);
        m_uv2 = Vec2d(1.0f, 1.0f);
        m_uv3 = Vec2d(1.0f, 0.0f);
    }

    ITF_INLINE void merge(AFX_KaleiParam& _param)
    {
        if(_param.m_use && m_use)
        {
            m_size = f32_merge(m_size, m_fade, _param.m_size, _param.m_fade, 1.0f);
            m_fade = 1.0f;
        }
    }

    bbool           m_use;
    f32             m_fade;
    f32             m_size;
    Vec2d           m_uv1;
    Vec2d           m_uv2;
    Vec2d           m_uv3;
};

class AFX_EyeFishParam
{
public:
    DECLARE_SERIALIZE()

    AFX_EyeFishParam()
    {
        Reset();
    }

    ITF_INLINE void Reset()
    {
        m_use = bfalse;
        m_fade = 0.0f;
        m_height = 2.0f;
        m_scale = 1.0f;
    }

    ITF_INLINE void merge(AFX_EyeFishParam& _param)
    {
        if(_param.m_use && m_use)
        {
            m_height = f32_merge(m_height, m_fade, _param.m_height, _param.m_fade, 1.1f);
            m_scale = f32_merge(m_scale, m_fade, _param.m_scale, _param.m_fade, 0.5f);
            m_fade = f32_Max(m_fade,_param.m_fade);
        }
    }


    bbool           m_use;
    f32             m_fade;
    f32             m_height;
    f32             m_scale;
};

class AFX_MirrorParam
{
public:
    DECLARE_SERIALIZE()

    AFX_MirrorParam()
    {
        Reset();
    }

    ITF_INLINE void Reset()
    {
        m_use = bfalse;
        m_fade = 0.0f;
        m_offsetX = 0.0f;
        m_offsetY = 0.0f;
    }

    ITF_INLINE void merge(AFX_MirrorParam& _param)
    {
        if(_param.m_use && m_use)
        {
            m_offsetX = f32_merge(m_offsetX, m_fade, _param.m_offsetX, _param.m_fade, 0.0f);
            m_offsetY = f32_merge(m_offsetY, m_fade, _param.m_offsetY, _param.m_fade, 0.0f);
            m_fade = 1.0f;
        }
    }

    bbool           m_use;
    f32             m_fade;
    f32             m_offsetX;
    f32             m_offsetY;
};

class AFX_OldTVParam
{
public:
    DECLARE_SERIALIZE()

    AFX_OldTVParam()
    {
        Reset();
    }

    ITF_INLINE void Reset()
    {
        m_use = bfalse;
        m_fade = 0.0f;
        m_lineFade = 0.5f;
        m_useScanLine = bfalse;
        m_scanLineFade = 0.5f;
        m_scanLineSpeed = 2.0f;
        m_customTexture = 0;
        m_scanLineSize = 0.2f;
    }

    ITF_INLINE void merge(AFX_OldTVParam& _param)
    {
        if(_param.m_use && m_use)
        {
            m_lineFade = f32_merge(m_lineFade, m_fade, _param.m_lineFade, _param.m_fade, 0.0f);
            m_scanLineFade = f32_merge(m_scanLineFade, m_fade, _param.m_scanLineFade, _param.m_fade, 0.0f);
            m_scanLineSpeed = f32_merge(m_scanLineSpeed, m_fade, _param.m_scanLineSpeed, _param.m_fade, 0.0f);
            m_scanLineSize = f32_merge(m_scanLineSize, m_fade, _param.m_scanLineSize, _param.m_fade, 0.0f);
            m_fade = 1.0f;
        }
    }

    Texture* getTexture()
    {
        if (m_customTexture.isValid())
        {
            Texture* texture = (Texture*)m_customTexture.getResource();

            if ( texture && texture->isPhysicallyLoaded())
                return texture;
        }
        return NULL;
    }

    bbool           m_use;
    f32             m_fade;
    f32             m_lineFade;
    bbool           m_useScanLine;
    f32             m_scanLineFade;
    f32             m_scanLineSpeed;
    f32             m_scanLineSize;

    ResourceID      m_customTexture;
};

class AFX_NoiseParam
{
public:
    DECLARE_SERIALIZE()

    AFX_NoiseParam()
    {
        Reset();
    }

    ITF_INLINE void Reset()
    {
        m_use = bfalse;
        m_fade = 0.0f;
        m_blend = 0.5f;
        m_size = 1.0f;
    }

    ITF_INLINE void merge(AFX_NoiseParam& _param)
    {
        if(_param.m_use && m_use)
        {
            m_blend = f32_merge(m_blend, m_fade, _param.m_blend, _param.m_fade, 0.0f);
            m_size = f32_merge(m_size, m_fade, _param.m_size, _param.m_fade, 1.0f);
            m_fade = 1.0f;
        }
    }

    bbool           m_use;
    f32             m_fade;
    f32             m_blend;
    f32             m_size;
};

class AFX_RadialParam
{
public:
    DECLARE_SERIALIZE()

    AFX_RadialParam()
    {
        Reset();
    }

    ITF_INLINE void Reset()
    {
        m_use = bfalse;
        m_fade = 0.0f;
        m_centerOffset = Vec2d(0.0f, 0.0f);
        m_size = 10.0f;
        m_strength = 1.0f;
    }

    ITF_INLINE void merge(AFX_RadialParam& _param)
    {
        if(_param.m_use && m_use)
        {
            m_centerOffset.x() = f32_merge(m_centerOffset.x(), m_fade, _param.m_centerOffset.x(), _param.m_fade, 1.0f);
            m_centerOffset.y() = f32_merge(m_centerOffset.y(), m_fade, _param.m_centerOffset.y(), _param.m_fade, 1.0f);
            m_size = f32_merge(m_size, m_fade, _param.m_size, _param.m_fade, 1.0f);
            m_strength = f32_merge(m_strength, m_fade, _param.m_strength, _param.m_fade, 1.0f);
            m_fade = 1.0f;
        }
    }

    bbool           m_use;
    f32             m_fade;
    Vec2d           m_centerOffset;
    f32             m_size;
    f32             m_strength;
};


class AFXPostProcess : public GFXObject
{
public:
    AFXPostProcess()
    {
        reset();
    };

    ~AFXPostProcess()
    {
    };

    ITF_INLINE void reset()
    {
        m_blur.Reset();
        m_glow.Reset();
        m_colorSetting.Reset();
        m_refraction.Reset();
        m_tile.Reset();
        m_mosaic.Reset();
        m_negatif.Reset();
        m_kalei.Reset();
        m_eyeFish.Reset();
        m_mirror.Reset();
        m_oldTV.Reset();
        m_noise.Reset();
        m_fadeFactor = 0.0f;
    };

    void setFadeFactor(f32 _fadeFactor);
    void setBlurParam(AFX_BlurParam _parameters);
    void setGlowParam(AFX_GlowParam _parameters);
    void setColorSettingParam(AFX_ColorSettingParam _parameters);
    void setRefraction(AFX_RefractionParam _parameters);
    void setTileParam(AFX_TileParam _parameters);
    void setMosaicParam(AFX_MosaicParam _parameters);
    void setNegatifParam(AFX_NegatifParam _parameters);
    void setKaleiParam(AFX_KaleiParam _parameters);
    void setEyeFishParam(AFX_EyeFishParam _parameters);
    void setMirrorParam(AFX_MirrorParam _parameters);
    void setOldTVParam(AFX_OldTVParam _parameters);
    void setNoiseParam(AFX_NoiseParam _parameters);
    void setRadialParam(AFX_RadialParam _parameters);

    bbool isActived() const;

    void mergePostProcess(AFXPostProcess &_afx);

    bbool b_needBackBufferCopy();

    Texture* getTexture(u32 _n);
    void setTexture(u32 _n, ResourceID _texture);

	ResourceID              m_customTextureID[2];

    AFX_BlurParam           m_blur;
    AFX_GlowParam           m_glow;
    AFX_ColorSettingParam   m_colorSetting;
    AFX_RefractionParam     m_refraction;
    AFX_TileParam           m_tile;
    AFX_MosaicParam         m_mosaic;
    AFX_NegatifParam        m_negatif;
    AFX_KaleiParam          m_kalei;
    AFX_EyeFishParam        m_eyeFish;
    AFX_MirrorParam         m_mirror;
    AFX_OldTVParam          m_oldTV;
    AFX_NoiseParam          m_noise;
    AFX_RadialParam         m_radial;

    f32                     m_fadeFactor;
};

} // namespace ITF

#endif // _ITF_AFTERFX_H_
