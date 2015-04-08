#include "precompiled_engine.h"


#ifndef _ITF_LIGHTMANAGER_H_
#include "engine/display/lighting/LightManager.h"
#endif // _ITF_LIGHTMANAGER_H_

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
	///// constructor/destructor --------------------------------------------------------------
	GFX_LightManager::GFX_LightManager(GFXAdapter *_adapter) : GFX_LightManager_Implementation(_adapter)
	{

	}
	GFX_LightManager::~GFX_LightManager()
	{

	}

} // namespace ITF

