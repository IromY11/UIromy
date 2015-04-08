#include "precompiled_engine.h"

#ifndef ITF_GFX_RENDER_PASS_H_
#include "GFXRenderPass.h"
#endif

namespace ITF
{

void RenderPassAlgo::begin(class RenderPassContext & _rdrCtxt, bbool _restoreContext)
{
#ifndef ITF_FINAL
    GFX_ADAPTER->startRasterForPass(_rdrCtxt.getPass());
#endif // ITF_FINAL

    GFX_ADAPTER->setRenderTargetForPass(_rdrCtxt.getPass(), _restoreContext);
    if (_rdrCtxt.getClear() || _rdrCtxt.getClearZStencil())
    {
        const Color & c = _rdrCtxt.getClearColor();
        u32 flag = _rdrCtxt.getClear()?GFX_CLEAR_COLOR:0;
        flag |= _rdrCtxt.getClearZStencil()?GFX_CLEAR_ZBUFFER|GFX_CLEAR_STENCIL:0;
        GFX_ADAPTER->clear(flag, c.m_r, c.m_g, c.m_b, c.m_a);
    }
    _rdrCtxt.getStateCache() = m_drawCallStateCache;
    GFX_ADAPTER->SetDrawCallState(_rdrCtxt.getStateCache(), 0xFFffFFff, btrue);

}

void RenderPassAlgo::end(class RenderPassContext & _rdrCtxt, bbool _resolveTarget)
{
    if(_resolveTarget)
        GFX_ADAPTER->resolveRenderTargetForPass(_rdrCtxt.getPass());

#ifndef ITF_FINAL
    GFX_ADAPTER->endRasterForPass(_rdrCtxt.getPass());
#endif // ITF_FINAL
}

void RenderPassAlgo::unpause(class RenderPassContext & _rdrCtxt, bbool _restoreContext)
{
#ifndef ITF_FINAL
    GFX_ADAPTER->startRasterForPass(_rdrCtxt.getPass());
#endif // ITF_FINAL
    GFX_ADAPTER->setRenderTargetForPass(_rdrCtxt.getPass(), _restoreContext);
}

void RenderPassAlgo::pause(class RenderPassContext & _rdrCtxt, bbool _resolveTarget)
{
    if(_resolveTarget)
        GFX_ADAPTER->resolveRenderTargetForPass(_rdrCtxt.getPass());
#ifndef ITF_FINAL
    GFX_ADAPTER->endRasterForPass(_rdrCtxt.getPass());
#endif // ITF_FINAL
}

} // ITF
