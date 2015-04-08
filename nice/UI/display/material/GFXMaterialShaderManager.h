#ifndef ITF_GFX_MATERIAL_SHADER_MANAGER_H_
#define ITF_GFX_MATERIAL_SHADER_MANAGER_H_

#ifndef ITF_TEMPLATECLIENTHANDLER_H
#include "engine/TemplateManager/TemplateClientHandler.h"
#endif //ITF_TEMPLATECLIENTHANDLER_H

#ifndef _ITF_WATCHDIRECTORYLISTENER_H
#include "core/file/WatchDirectoryListener.h"
#endif  //_ITF_WATCHDIRECTORYLISTENER_H

namespace ITF
{
    
    class GFXMaterialShader_Template;

    class GFXMaterialShaderManager : public WatchDirectoryListener
    {
    public:

        static GFXMaterialShaderManager *   getInstance() { static GFXMaterialShaderManager s_materialShaderMgr; return &s_materialShaderMgr; }  

        // acquire/release (ref counted)
        const GFXMaterialShader_Template *  acquire(const Path & _path);
        void                                release(const GFXMaterialShader_Template * _gfxMaterialTpl);

#ifdef ITF_SUPPORT_HOTRELOAD_TEMPLATE
        virtual void onModifiedFile(const Path& _path);
        void reloadDependencies(const GFXMaterialShader_Template * _gfxMaterialTpl);
#else
        void reloadDependencies(const GFXMaterialShader_Template * _gfxMaterialTpl) {}
#endif

    private:

        friend class GFX_MATERIAL;
#ifdef ITF_SUPPORT_HOTRELOAD_TEMPLATE

        void                                registerForHotReload(GFX_MATERIAL * _materialInstance);
        void                                unregisterForHotReload(GFX_MATERIAL * _materialInstance);
        ITF_SET<GFX_MATERIAL *>             m_allMaterialInstance;
        Mutex                               m_hotReloadCS;
#else
        void                                registerForHotReload(GFX_MATERIAL * _materialInstance) {}
        void                                unregisterForHotReload(GFX_MATERIAL * _materialInstance) {}
#endif

        GFXMaterialShaderManager();
        ~GFXMaterialShaderManager();

        KeyArray <const GFXMaterialShader_Template*>     m_materialShaders;
    };


} // namespace ITF

#endif // ITF_GFX_MATERIAL_SHADER_MANAGER_H_
