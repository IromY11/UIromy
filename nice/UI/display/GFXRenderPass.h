#ifndef ITF_GFX_RENDER_PASS_H_
#define ITF_GFX_RENDER_PASS_H_

#ifndef ITF_GFX_CONTEXT_H_
#include "engine/display/GFXContext.h"
#endif //ITF_GFX_CONTEXT_H_

#ifndef ITF_FINAL
#define ITF_ZLIST_PASS_DEBUGMODE
#endif

namespace ITF
{

    class RenderPassAlgo
    {
    public:
        virtual void prepareDrawCallState() {};
        virtual void begin(class RenderPassContext & _rdrCtxt, bbool _restoreContext = bfalse);
        virtual void end(class RenderPassContext & _rdrCtxt, bbool _resolveTarget);
        virtual void unpause(class RenderPassContext & _rdrCtxt, bbool _restoreContext = bfalse);
        virtual void pause(class RenderPassContext & _rdrCtxt, bbool _resolveTarget);
        DrawCallStateCache m_drawCallStateCache;
    };

}

#endif // ITF_GFX_RENDER_PASS_H_
