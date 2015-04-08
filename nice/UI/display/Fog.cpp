#include "precompiled_engine.h"


#ifndef _ITF_FOG_H_
#include    "engine/display/Fog.h"
#endif // _ITF_FOG_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_PREFETCH_FCT_H_
#include "engine/boot/PrefetchFct.h"
#endif  //_ITF_PREFETCH_FCT_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

namespace ITF
{

    const f32 cDefaultNearDistance = 0.f;
    const f32 cDefaultFarDistance = 1.f;
    const f32 cDefaultAttDist = 0.0f;
    const u32 cDefaultNearColor = 0x00000000;
    const u32 cDefaultFarColor = 0x00000000;
    const bbool cDefaultUseNearOffset = bfalse;
    const f32 cDefaultNearOffset = 0.0f;
    const u32 cMaxFogBoxByObject = 2;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// 
	// Fog class
	// 
	///////////////////////////////////////////////////////////////////////////////////////////////

    FogManager::FogManager()
    {
        m_useFog = btrue;
        m_useDefaultFog = btrue;

        //default Fog Param
        AABB pos(Vec2d(0.f, 0.f), Vec2d(1.f, 1.f));
        m_defaultFogParam = FogParam(pos, Vec2d(cDefaultAttDist, cDefaultAttDist), cDefaultNearDistance, Color(cDefaultNearColor),
                                        cDefaultFarDistance, Color(cDefaultFarColor),cDefaultUseNearOffset, cDefaultNearOffset);
    };

	///////////////////////////////////////////////////////////////////////////////////////////////
    void FogManager::addFogBox(FogParam &_param)
    {
        m_worldFogBox.push_back(_param);
    }

    void FogManager::removeAllFogBox()
    {
        m_worldFogBox.clear();
    }

    void FogManager::startFrame()
    {
        removeAllFogBox();
    }

	///////////////////////////////////////////////////////////////////////////////////////////////
    void FogManager::computeObjectFog(Color& _objectFog, FogParam &_param, f32 _objectCamAbsZ)
    {
        //if (GFX_FOG == FOG_T_LINEAR)
        {
            f32 factor = _param.farDist - _objectCamAbsZ;
            factor *= _param.getInvFarNearDistance();
            factor = Clamp(factor, 0.f, 1.f);
            
            _objectFog = Interpolate(_param.farColor, _param.nearColor, factor);
        }
    }

    void FogManager::computeBVVisibility(const AABB& _aabb)
    {
        m_objectFogBox.clear();
        m_objectFogBoxOverlap.clear();
        for(u32 i=0; i<m_worldFogBox.size(); i++)
        {
            if(_aabb.checkOverlap(m_worldFogBox[i].position))
            {
                m_objectFogBox.push_back(i);

                bbool isOverlap = m_worldFogBox[i].getAttenuationAABB().contains(_aabb);
                m_objectFogBoxOverlap.push_back(isOverlap);
            }
        }
    }

    void FogManager::clearObjectFogBoxList()
    {
        m_objectFogBox.clear();
        m_objectFogBoxOverlap.clear();
   }

    void FogManager::computeShaderFogConstant(ITF_VECTOR<ShaderFogParamVS>&_fogParamArrayVS, ITF_VECTOR<ShaderFogParamPS>&_fogParamArrayPS, ITF_VECTOR<u32>&_fullFog, f32 _fogFactor, f32 _currentObjectWorldZ, f32 _currentObjectCamAbsZ)
    {
        //Find principal Fog Box for the current object
        const u32 numFogBox = m_objectFogBox.size() > cMaxFogBoxByObject ? cMaxFogBoxByObject : m_objectFogBox.size();

        const f32 fCamDist = CAMERA->getPos().getZ() - CAMERA->getDeltaFogZ();
        const Vec2d cVec1 = Vec2d(1.f, 1.f);

       //prepare shader Constant
        _fogParamArrayVS.clear();
        _fogParamArrayPS.clear();
        _fullFog.clear();

        for(u32 i=0; i<numFogBox; i++)
        {
            FogParam & fogBox = m_worldFogBox[m_objectFogBox[i]];
            bbool excludeByNearOffset = fogBox.useNearOffset && (f32_Abs(_currentObjectWorldZ) < f32_Abs(fogBox.nearOffset));
            if(!excludeByNearOffset)
            {
                f32 nearAlphaAtt = f32_Clamp(fogBox.nearColor.getAlpha() * _fogFactor, 0.0f, 1.0f);
                f32 farAlphaAtt = f32_Clamp(fogBox.farColor.getAlpha() * _fogFactor, 0.0f, 1.0f);
                const Vec2d& attenuation = fogBox.getAttenuation();
                const Vec2d& invSize = fogBox.getInvSizeAttenuatin();

                Color colorFog;
                computeObjectFog(colorFog, fogBox, _currentObjectCamAbsZ);

                _fogParamArrayVS.emplace_back();
                ShaderFogParamVS & shaderParamVS = _fogParamArrayVS.back();
                _fogParamArrayPS.emplace_back();
                ShaderFogParamPS & shaderParamPS = _fogParamArrayPS.back();

                shaderParamPS.f4_BoxSizeAtt.set(invSize.x(), invSize.y(), attenuation.x(), attenuation.y());
                shaderParamPS.f4_Color.set(colorFog.getRed(), colorFog.getGreen(), colorFog.getBlue(), colorFog.getAlpha());
                shaderParamVS.f4_BoxCenter.set(fogBox.position.getCenter().x(), fogBox.position.getCenter().y(), 0.0f, 0.0f);
                shaderParamVS.f4_CamFarNearDist.set(fCamDist, fogBox.farDist, fogBox.getInvFarNearDistance(), 1.0f);
                shaderParamVS.f4_AlphaAtt.set(nearAlphaAtt, farAlphaAtt, 1.0f, 1.0f);
                _fullFog.push_back(i);
            }
        }
    }


} // namespace ITF

