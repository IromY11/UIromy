#include "precompiled_engine.h"


#ifndef _ITF_LIGHTMANAGER_BASE_H_
#include "engine/display/lighting/LightManager_Base.h"
#endif // _ITF_LIGHTMANAGER_BASE_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef ITF_GFX_ZLIST_H_
#include "GFXZList.h"
#endif // ITF_GFX_ZLIST_H_

namespace ITF
{
	GFX_Light3D::GFX_Light3D()
	{
		m_type = LightType_Invalid;
		m_boxPos.set(0.0f,0.0f, 0.0f);
		m_boxSize.set(10.0f,10.0f);
	}

	///// constructor/destructor --------------------------------------------------------------
	GFX_LightManager_Base::GFX_LightManager_Base(GFXAdapter *_adapter)
	{
		m_adapter = _adapter;
	}
	GFX_LightManager_Base::~GFX_LightManager_Base()
	{

	}

	shaderDefineKey GFX_LightManager_Base::getDisabledDefines()
	{
		shaderDefineKey defines = 0;
		defines |= m_adapter->mc_define_Light;
		defines |= m_adapter->mc_define_Light3D;
		defines |= m_adapter->mc_define_UseBackLight;
		defines |= m_adapter->mc_define_NormalMap;
		return defines;
	}
} // namespace ITF

