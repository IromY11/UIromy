#ifndef _ITF_FOG_H_
#define _ITF_FOG_H_

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/resource.h"
#endif //_ITF_RESOURCE_H_

#ifndef _ITF_KEYARRAY_H_
#include "core/container/KeyArray.h"
#endif //_ITF_KEYARRAY_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

class GFX_Vector4;
namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Fog Parameter class
    // 
    ///////////////////////////////////////////////////////////////////////////////////////////////
	struct FogParam
	{
    public:

        ///// constructor ----------------------------------------------------------
        FogParam(){};

        FogParam( AABB _position, Vec2d _attenuation, f32 _near, Color _nearColor, f32 _far, Color _farColor, bbool _useNearOffset, f32 _nearOffset)
        {
            position        = _position;
            attenuation     = _attenuation;
            nearDist        = _near;
            nearColor       = _nearColor;
            farDist         = _far;
            farColor        = _farColor;
            useNearOffset   = _useNearOffset;
            nearOffset      = _nearOffset;

            updateInternalValue();
        }

        ///// operators ------------------------------------------------------------
        const FogParam& operator = (const FogParam& src)
        {
            position    = src.position;
            attenuation = src.attenuation;
            nearDist    = src.nearDist;
            nearColor   = src.nearColor;
            farDist     = src.farDist;
            farColor    = src.farColor;
            useNearOffset   = src.useNearOffset;
            nearOffset      = src.nearOffset;

            updateInternalValue();

            return *this;
        }

        void updateInternalValue()
        {
            const Vec2d cVec1 = Vec2d(1.f, 1.f);
            const Vec2d cVec0 = Vec2d(0.f, 0.f);

            vecAttenuation = Vec2d(position.getWidth()/2, position.getHeight()/2) - attenuation;
            invSizeAtt = cVec0;
            if( !attenuation.IsEqual( cVec0 ) )
                invSizeAtt = cVec1 / attenuation;        
            invFarNearDist = 1.0f / (farDist - nearDist);

            Vec2d vecScale;
            const Vec2d vecSize(position.getWidth(), position.getHeight());
            if(vecSize.x() != 0 && vecSize.y()!=0)
            {
                const Vec2d vecDist(attenuation.x(), attenuation.y());
                vecScale = (vecSize - vecDist * 2) / vecSize;
                vecScale = vecScale.abs();
                Vec2d::Min(&vecScale, &vecScale, &Vec2d::One);
                Vec2d::Max(&vecScale, &vecScale, &Vec2d::Zero);
            }
            else
            {
                vecScale = Vec2d::One;
            }
            attenuationAABB = position;
            attenuationAABB.ScaleFromCenter(vecScale);
        }

        const Vec2d & getAttenuation() const { return vecAttenuation;}
        const Vec2d & getInvSizeAttenuatin() const { return invSizeAtt;}
        const AABB &  getAttenuationAABB() const { return attenuationAABB;}
        f32           getInvFarNearDistance() const { return invFarNearDist;}

        ///// members --------------------------------------------------------------
    public:
        
        AABB    position;
        Vec2d   attenuation;

        f32     nearDist;
        Color   nearColor;

        f32     farDist;
        Color   farColor;

        bbool   useNearOffset;
        f32     nearOffset;

    private:
        // intern pre calculate value
        AABB  attenuationAABB;
        Vec2d vecAttenuation;
        Vec2d invSizeAtt;
        f32   invFarNearDist;
	};

    //must be the mirror of the VS_DynFogParam struct in renderPCT_Register.fx
    struct ShaderFogParamVS
    {
        GFX_Vector4	f4_BoxCenter;      //xy: 2D box center
        GFX_Vector4 f4_CamFarNearDist; //x:Cam Z, y: farDist, z:far-near Dist
        GFX_Vector4 f4_AlphaAtt;       //x:near Alpha Att, y:far Alpha Att
    };

    //must be the mirror of the PS_DynFogParam struct in renderPCT_Register.fx
    struct ShaderFogParamPS
    {
        GFX_Vector4	f4_BoxSizeAtt;     //xy: att, zw: 1/(att-size)
        GFX_Vector4	f4_Color;
    };

	///////////////////////////////////////////////////////////////////////////////////////////////
	// 
	// Fog class
	// 
	///////////////////////////////////////////////////////////////////////////////////////////////
	class FogManager
	{
	public:

		///// constructor/destructor --------------------------------------------------------------
		FogManager();
		~FogManager() {};


        ///// Accessor ----------------------------------------------------------------------------
        ITF_INLINE const FogParam & getDefaultFogParam() const { return m_defaultFogParam; }
        ITF_INLINE void             setDefaultFogParam( FogParam& _default) { m_defaultFogParam = _default; }
        
        ITF_INLINE bbool            isUseFog() const { return m_useFog;}
        ITF_INLINE void             setUseFog(bbool _useFog) { m_useFog = _useFog;}
        ITF_INLINE bbool            isUseFogBox() const { return (m_objectFogBox.size() != 0);}
        ITF_INLINE bbool            isFullOverlapFogBox( u32 _index) const { return m_objectFogBoxOverlap[_index];}

        ///// Manage ------------------------------------------------------------------------------------
        void                        addFogBox(FogParam &_param);
        void                        removeAllFogBox();
        void                        startFrame();

        ///// ------------------------------------------------------------------------------------
        void                        computeObjectFog(Color & _objectFog, FogParam &_param, f32 _objectCamAbsZ);
        void                        computeShaderFogConstant(ITF_VECTOR<ShaderFogParamVS>&_fogParamArrayVS, ITF_VECTOR<ShaderFogParamPS>&_fogParamArrayPS, ITF_VECTOR<u32>&_fullFog, f32 _fogFactor, f32 _currentObjectWorldZ, f32 _currentObjectCamAbsZ);
        void                        computeBVVisibility(const AABB& _aabb);
        void                        clearObjectFogBoxList();


	public:
	private:
        bbool               m_useFog;

        bbool               m_useDefaultFog;
        FogParam            m_defaultFogParam;

        SafeArray<FogParam> m_worldFogBox;  // All the fogBox in frustrum
        SafeArray<ux> m_objectFogBox; // The fogBox that intersect the object
        ITF_VECTOR<bbool>   m_objectFogBoxOverlap;  // say if the corresponding fogBox overlap the object
	};

} // namespace ITF

#endif // _ITF_FOG_H_
