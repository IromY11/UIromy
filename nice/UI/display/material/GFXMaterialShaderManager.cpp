#include "precompiled_engine.h"

#ifndef ITF_GFX_MATERIAL_SHADER_MANAGER_H_
#include "GFXMaterialShaderManager.h"
#endif //ITF_GFX_MATERIAL_MANAGER_H_

#ifndef ITF_GFX_MATERIAL_H_
#include "GFXMaterial.h"
#endif //ITF_GFX_MATERIAL_H_

#ifndef _ITF_WARNINGMANAGER_H_
#include "core/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

namespace ITF
{

    GFXMaterialShaderManager::GFXMaterialShaderManager()
    {
    }

    GFXMaterialShaderManager::~GFXMaterialShaderManager()
    {
    }

#ifdef ITF_SUPPORT_HOTRELOAD_TEMPLATE
    void GFXMaterialShaderManager::registerForHotReload(GFX_MATERIAL * _materialInstance)
    {
        ScopeLock<Mutex> scopeLock(m_hotReloadCS);
        m_allMaterialInstance.insert(_materialInstance);
    }

    void GFXMaterialShaderManager::unregisterForHotReload(GFX_MATERIAL * _materialInstance)
    {
        ScopeLock<Mutex> scopeLock(m_hotReloadCS);
        m_allMaterialInstance.erase(_materialInstance);
    }

    void GFXMaterialShaderManager::reloadDependencies(const GFXMaterialShader_Template * _gfxMaterialShaderTpl)
    {
        ScopeLock<Mutex> scopeLock(m_hotReloadCS);
        for(ITF_SET<GFX_MATERIAL*>::const_iterator it = m_allMaterialInstance.begin(), itEnd = m_allMaterialInstance.end();
            it!=itEnd;
            ++it)
        {
            GFX_MATERIAL * mat = (*it);
            if (mat->getShaderTemplate() == _gfxMaterialShaderTpl)
                mat->resetFromShaderTemplate();
        }
    }
#endif

    const GFXMaterialShader_Template * GFXMaterialShaderManager::acquire(const Path & _path)
    {
        if (_path.isEmpty()) return NULL;
        const GFXMaterialShader_Template * gfxMaterialShaderTemplate = TEMPLATEDATABASE->requestTemplate<GFXMaterialShader_Template>(_path);
        if(gfxMaterialShaderTemplate == NULL)
        {           
            ITF_WARNING(NULL,0,"Unable to read file : %s", _path.toString8().cStr());
        }
        
        return gfxMaterialShaderTemplate;
    }

    void GFXMaterialShaderManager::release(const GFXMaterialShader_Template * _gfxMatShaderTpl)
    {
        if (_gfxMatShaderTpl
            && _gfxMatShaderTpl!=&GFXMaterialShader_Template::defaultTpl) // releasing the default Tpl is just ignored
        {
            TEMPLATEDATABASE->releaseTemplate(_gfxMatShaderTpl->getFile());
        }
    }

#ifdef ITF_SUPPORT_HOTRELOAD_TEMPLATE
    void GFXMaterialShaderManager::onModifiedFile(const Path& _path)
    {
        String8 extension = _path.getExtension();
        ITF_VECTOR<Path> mshFiles;

        if (extension == "msh")
        {
            const GFXMaterialShader_Template* reloadTemplate = TEMPLATEDATABASE->reloadTemplate<GFXMaterialShader_Template>(_path);
            reloadDependencies(reloadTemplate);
        }
#ifdef ITF_SUPPORT_LOGICDATABASE
        else if (extension == "ilu")
        {
            const ITF_MAP<Path, LogicDataTableDependencies*> & dependenciesTables = LOGICDATABASE->getDependenciesTables();

            for (ITF_MAP<Path, LogicDataTableDependencies*>::const_iterator it = dependenciesTables.begin();
                it != dependenciesTables.end();
                ++it)
            {
                if (it->second->hasDependency(_path))
                {
                    mshFiles.push_back(it->first);
                    continue;
                }
            }

            for (ITF_VECTOR<Path>::const_iterator it = mshFiles.begin(); it != mshFiles.end(); ++it)
            {
                const GFXMaterialShader_Template* reloadTemplate = TEMPLATEDATABASE->reloadTemplate<GFXMaterialShader_Template>(*it);
                reloadDependencies(reloadTemplate);
            }
        }
#endif //ITF_SUPPORT_LOGICDATABASE
    }
#endif // ITF_SUPPORT_HOTRELOAD_TEMPLATE

} // namespace ITF
