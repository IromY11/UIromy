#include "precompiled_engine.h"

#ifndef _ITF_FONTTEXT_H_
#include "engine/display/FontText.h"
#endif // _ITF_FONTTEXT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif // _ITF_GFX_ADAPTER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif // _ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FONTTEMPLATE_H_
#include "engine/display/FontTemplate.h"
#endif //_ITF_FONTTEMPLATE_H_

#ifndef ITF_CORE_UNICODE_TOOLS_H_
#include "core/UnicodeTools.h"
#endif //ITF_CORE_UNICODE_TOOLS_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_


namespace ITF
{
FontText::FontText():
    m_font(NULL),
    m_color(Color::white()),
    m_size(-1),
    m_is2D(btrue),
    m_position(Vec3d::Zero),
    m_localPivot(Vec2d::Zero),
    m_localScale(Vec2d(1, 1)),
    m_localAngle(0.0),
    m_shadowOffset(Vec2d::Zero),
    m_shadowColor(Color::black()),
    m_constAdvanceChar_Size(0),
    m_constAdvanceChar_Min(0),
    m_constAdvanceChar_Max(0),
    m_charSpacing(0),
    m_useGradient(bfalse),
    m_gradientScale(1.0f),
    m_gradientOffset(0.5f),
    m_gradientColor(Color::black()),
    m_bIsDirty(btrue),
    m_effectChain(NULL)
{
    m_clippingRect.invalidate();
}

FontText::~FontText()
{
    freeMesh();
}

void FontText::computeFinalMatrix(Matrix44& _finalMatrix) const
{
    Matrix44 localMatrix;
    localMatrix.setRotationZ(m_localAngle);
    localMatrix.mulScale(m_localScale.to3d(1.f));
    localMatrix.setTranslation(m_localPivot.to3d());

    Matrix44 transformMatrix;
    transformMatrix.setIdentity34();

    const Vec3d position = m_position + m_offset;
    if(m_is2D)
    {
        transformMatrix.setT(loadXYZW(position.x(), position.y(), 0.0f, 1.f));
    }
    else
    {
        transformMatrix.setTranslation(position);
    }

    _finalMatrix.mul44(localMatrix, transformMatrix);
}

void FontText::computePosition(f32& x, f32& y, f32& z) const
{
    if(m_is2D)
    {
        y = m_localPivot.y();
    }
    else
    {
        y = -m_localPivot.y();
    }

    x = -m_localPivot.x();
    z = 0.0f;
}

void FontText::setupScissor(GFXAdapter * _gfxDeviceAdapter, const Vec2d &_offset) const
{
    if (m_clippingRect.isValid())
    {
        Vec3d       projected;
        i32 px, py;
        GFX_RECT    rcClipping;

        GFX_Viewport viewport;
        _gfxDeviceAdapter->getViewport(viewport);

        const f32   yScale = m_is2D ? -1.0f : 1.0f;
        const f32   yOffset = m_is2D ? GFX_ADAPTER->getWindowHeight() : 0.0f;

        _gfxDeviceAdapter->compute3DTo2D(m_clippingRect.getMin().to3d(m_position.z()), projected);
        px                  = static_cast<i32>(projected.x() + _offset.x());
        py                  = static_cast<i32>(projected.y() * yScale + yOffset + _offset.y());
        rcClipping.left      = px;
        rcClipping.right     = px;
        rcClipping.top       = py;
        rcClipping.bottom    = py;

        _gfxDeviceAdapter->compute3DTo2D(m_clippingRect.getMax().to3d(m_position.z()), projected);
        px                  = static_cast<i32>(projected.x() + _offset.x());
        py                  = static_cast<i32>(projected.y() * yScale + yOffset + _offset.y());
        if (rcClipping.left  > px) rcClipping.left    = px;
        if (rcClipping.right < px) rcClipping.right   = px;
        if (rcClipping.top   > py) rcClipping.top     = py;
        if (rcClipping.bottom< py) rcClipping.bottom  = py;

        _gfxDeviceAdapter->compute3DTo2D(m_clippingRect.getMinXMaxY().to3d(m_position.z()), projected);
        px                  = static_cast<i32>(projected.x() + _offset.x());
        py                  = static_cast<i32>(projected.y() * yScale + yOffset + _offset.y());
        if (rcClipping.left  > px) rcClipping.left    = px;
        if (rcClipping.right < px) rcClipping.right   = px;
        if (rcClipping.top   > py) rcClipping.top     = py;
        if (rcClipping.bottom< py) rcClipping.bottom  = py;

        _gfxDeviceAdapter->compute3DTo2D(m_clippingRect.getMaxXMinY().to3d(m_position.z()), projected);
        px                  = static_cast<i32>(projected.x() + _offset.x());
        py                  = static_cast<i32>(projected.y() * yScale + yOffset + _offset.y());
        if (rcClipping.left  > px) rcClipping.left    = px;
        if (rcClipping.right < px) rcClipping.right   = px;
        if (rcClipping.top   > py) rcClipping.top     = py;
        if (rcClipping.bottom< py) rcClipping.bottom  = py;
        
        rcClipping.left   = Clamp(rcClipping.left,  i32(viewport.m_x), i32(viewport.m_x+viewport.m_width));
        rcClipping.right  = Clamp(rcClipping.right, i32(viewport.m_x), i32(viewport.m_x+viewport.m_width));
        rcClipping.top    = Clamp(rcClipping.top,   i32(viewport.m_y), i32(viewport.m_y+viewport.m_height));
        rcClipping.bottom = Clamp(rcClipping.bottom,i32(viewport.m_y), i32(viewport.m_y+viewport.m_height));

        _gfxDeviceAdapter->setScissorRect(&rcClipping);
    }
}

void FontText::resetScissor(GFXAdapter * _gfxDeviceAdapter) const
{
    if(m_clippingRect.isValid())
        _gfxDeviceAdapter->setScissorRect(NULL);
}

void FontText::computeMeshByPage()
{
    ITF_ASSERT_MSG(m_font!=NULL, "Font resource id badly intialized");

    // remove finished effects
    while( m_effectChain )
    {
        if (!m_effectChain->m_fontEffect->isFinished())
            break;
        m_effectChain = m_effectChain->m_next;
    }

    if(!m_bIsDirty && !m_effectChain)
        return;
    m_bIsDirty = bfalse;

    const char * textStr = m_text.cStr();

    const f32 scale = m_font->getScale(m_size);
    const i32 scaleW = m_font->getScaleW();
    const i32 scaleH = m_font->getScaleH();
    const u32 color = Color::white().getAsU32();

    freeMesh();

    // COMPUTE CHARS COUNT BY PAGE
    SafeArray<i32> charsCountByPage;
    charsCountByPage.resize(m_font->getPagesCount());

    for(u32 n =0; n<charsCountByPage.size(); n++)
    {
        charsCountByPage[n] = 0;
    }

    const char * currentTextStr = textStr;
    while(*currentTextStr != 0)
    {
        i32 charId = i32(getNextUnicodeChar(currentTextStr));

        const FontTemplate::Char* ch = m_font->getCharDesc(charId);

        if(ch)
        {
            charsCountByPage[ch->m_page]++;
        }
    }
    // ---------------------------


    // CREATE VERTEX BUFFER BY PAGE
    m_bufferByPage.resize(m_font->getPagesCount());
    m_meshByPage.resize(m_font->getPagesCount());

    for(u32 n = 0; n < m_meshByPage.size(); ++n)
    {
        ITF_Mesh* mesh = &m_meshByPage[n];
        i32 charsCount = charsCountByPage[n];
        if (charsCount  == 0)
        {
            mesh->removeVertexBuffer();
            continue;
        }
        
        mesh->createVertexBuffer(charsCount*6, VertexFormat_PCB2T, sizeof(VertexPCB2T), vbLockType_static, VB_T_FONT);
        mesh->LockVertexBuffer((void **)&m_bufferByPage[n]);
    }
    // --------------------------
    

    // COMPUTE CHAR VERTICES
    f32 x, y, z;
    computePosition(x, y, z);
    y = y + scale * ((m_font->getSize() + m_font->getHeight()) * 0.5f);

    f32 constAdvance = 0;
    if(m_constAdvanceChar_Size!=0)
    {
        const FontTemplate::Char* ch = m_font->getCharDesc(m_constAdvanceChar_Size);
        if(ch)
            constAdvance = scale * f32(ch->m_advanceX);
    }

    currentTextStr = textStr;
	int chnl=0;

    if (!m_effectChain)
    {
        while(*currentTextStr != 0)
        {
            i32 charId = i32(getNextUnicodeChar(currentTextStr));

            const FontTemplate::Char* ch = m_font->getCharDesc(charId);

            if( ch ) 
            {
                i32 page = ch->m_page;

                // Map the center of the texel to the corners
                // in order to get pixel perfect mapping
                f32 u = (f32(ch->m_x)+0.5f) / scaleW;
                f32 v = (f32(ch->m_y)+0.5f) / scaleH;
                f32 u2 = u + f32(ch->m_width) / scaleW;
                f32 v2 = v + f32(ch->m_height) / scaleH;

                f32 a = scale * f32(ch->m_advanceX);
                f32 w = scale * f32(ch->m_width);
                f32 h = scale * f32(ch->m_height);
                f32 ox = scale * f32(ch->m_offsetX);
                f32 oy = scale * f32(ch->m_offsetY);
            
                if(constAdvance!=0 && charId>=m_constAdvanceChar_Min && (m_constAdvanceChar_Max==0 || charId<=m_constAdvanceChar_Max))
                {
                    ox += Max(0.0f, (constAdvance - a) * 0.5f);
                    a = constAdvance;
                }

                VertexPCB2T * & vbElt = m_bufferByPage[page];

                vbElt->setData( Vec3d(x+ox, y-oy, z), Vec2d(u, v), Vec2d(0.0f, 0.0f),color,chnl);
                vbElt++;
                vbElt->setData( Vec3d(x+w+ox, y-oy, z),Vec2d(u2, v),Vec2d(1.0f, 0.0f),color,chnl);
                vbElt++;
                vbElt->setData( Vec3d(x+w+ox, y-h-oy, z),Vec2d(u2, v2),Vec2d(1.0f, 1.0f),color,chnl);
                vbElt++;
                vbElt->setData( Vec3d(x+ox, y-h-oy, z),Vec2d(u, v2),Vec2d(0.0f, 1.0f),color,chnl);
                vbElt++;
                vbElt->setData( Vec3d(x+ox, y-oy, z),Vec2d(u, v),Vec2d(0.0f, 0.0f),color,chnl);
                vbElt++;
                vbElt->setData( Vec3d(x+w+ox, y-h-oy, z),Vec2d(u2, v2),Vec2d(1.0f, 1.0f),color,chnl);
                vbElt++;

                x += a + (scale * m_font->getOutlineThickness()) + (scale * m_charSpacing);
            }
        }
    }
    else
    {
        Vec3d P0, P1, P2, P3;
        u32 curColor;
        FontEffectChain *curEffect;

        while(*currentTextStr != 0)
        {
            i32 charId = i32(getNextUnicodeChar(currentTextStr));

            const FontTemplate::Char* ch = m_font->getCharDesc(charId);

            if( ch ) 
            {
                i32 page = ch->m_page;

                // Map the center of the texel to the corners
                // in order to get pixel perfect mapping
                f32 u = (f32(ch->m_x)+0.5f) / scaleW;
                f32 v = (f32(ch->m_y)+0.5f) / scaleH;
                f32 u2 = u + f32(ch->m_width) / scaleW;
                f32 v2 = v + f32(ch->m_height) / scaleH;

                f32 a = scale * f32(ch->m_advanceX);
                f32 w = scale * f32(ch->m_width);
                f32 h = scale * f32(ch->m_height);
                f32 ox = scale * f32(ch->m_offsetX);
                f32 oy = scale * f32(ch->m_offsetY);

                if(constAdvance!=0 && charId>=m_constAdvanceChar_Min && (m_constAdvanceChar_Max==0 || charId<=m_constAdvanceChar_Max))
                {
                    ox += Max(0.0f, (constAdvance - a) * 0.5f);
                    a = constAdvance;
                }

                f32 seed = x + y + w + h;
                u32 uSeed = ( *(u32 *) ch | *(u32 *) &seed ) & 0xFF;
                seed = (float) uSeed / (float) 0xFF;

                P0.set( x+ox, y-oy, z);
                P1.set( x+w+ox, y-oy, z);
                P2.set( x+w+ox, y-h-oy, z);
                P3.set( x+ox, y-h-oy, z);
                curColor = color;

                curEffect = m_effectChain;
                while (curEffect)
                {
                    curEffect->m_fontEffect->apply( P0, P1, P2, P3, curColor, seed, m_area );
                    curEffect = curEffect->m_next;
                }

                VertexPCB2T * & vbElt = m_bufferByPage[page];

                vbElt->setData(P0, Vec2d(u, v), Vec2d(0.0f, 0.0f),curColor,chnl);
                vbElt++;
                vbElt->setData(P1,Vec2d(u2, v),Vec2d(1.0f, 0.0f),curColor,chnl);
                vbElt++;
                vbElt->setData(P2,Vec2d(u2, v2),Vec2d(1.0f, 1.0f),curColor,chnl);
                vbElt++;
                vbElt->setData(P3,Vec2d(u, v2),Vec2d(0.0f, 1.0f),curColor,chnl);
                vbElt++;
                vbElt->setData(P0,Vec2d(u, v),Vec2d(0.0f, 0.0f),curColor,chnl);
                vbElt++;
                vbElt->setData(P2,Vec2d(u2, v2),Vec2d(1.0f, 1.0f),curColor,chnl);
                vbElt++;

                x += a + (scale * m_font->getOutlineThickness()) + (scale * m_charSpacing);
            }
        }

        // remove all effects if no dynamic effect found
        curEffect = m_effectChain;
        while( curEffect )
        {
            if (!curEffect->m_fontEffect->getTemplate()->isStatic())
                break;
            curEffect = curEffect->m_next;
        }
        if (!curEffect) m_effectChain = NULL;
    }

    for(ITF_VECTOR<ITF_Mesh>::iterator it = m_meshByPage.begin(); it != m_meshByPage.end(); ++it)
    {
        if (it->getCurrentVB() == NULL) 
            continue;
        it->UnlockVertexBuffer();
    }
}

void FontText::drawMesh(PrimitiveContext & _primCtxt, GFXAdapter * _gfxDeviceAdapter) const
{
    Matrix44 finalMatrix;
    const f32 fontHeight = m_font->getHeight();
    const f32 scale = m_font->getScale(m_size);

    computeFinalMatrix(finalMatrix);

    DrawCallContext drawcallCtx = DrawCallContext(&_primCtxt);

    for(u32 n = 0; n < m_meshByPage.size(); ++n)
    {
        const ITF_Mesh &mesh = m_meshByPage[n];
        if (mesh.getCurrentVB() == NULL)
            continue;

        const ResourceID& resID = m_font->getPageResourceID(n);
        Texture* pTexture = (Texture*) resID.getResource();

        if (pTexture->isPhysicallyLoaded())
        {
            // transformation position
            if(m_is2D)
            {
                Float4 widthHeight = _primCtxt.getRenderPassContext()->getRenderTargetSize();
                Float4 translation = finalMatrix.T();
                // we need to invert y : (height - translation.y)
                Float4 subHeight = widthHeight - translation;
                Float4 trans2DYInverted = shuffle<0, 5, 2, 3>(translation, subHeight); // take Y on subHeight
                finalMatrix.setT(trans2DYInverted);
            }

            // prepare
            if (_gfxDeviceAdapter->setGfxMatDefault(drawcallCtx))
            {
                GFX_Vector4 fontParam;
                if(m_useGradient)
                {
                    fontParam.x() = 1.0f / m_gradientScale;
                    fontParam.y() = 1.0f / m_gradientOffset;
                    fontParam.z() = 1.0f / (1.0f - m_gradientOffset);
                }
                _gfxDeviceAdapter->SetTexture(0,pTexture);
                drawcallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ALPHA);

                // draw shadow
                if (!m_shadowOffset.IsEqual(Vec2d::Zero, MTH_EPSILON))
                { 
                    setupScissor(_gfxDeviceAdapter, m_shadowOffset);

                    f32 rap = fontHeight / FONT_INITIAL_SIZE;
                    Matrix44 shadowMatrix = finalMatrix;
                    float temp = scale * 3.f * rap;
                    Matrix44 translationMatrix;
                    translationMatrix.setIdentity();
                    translationMatrix.setTranslation(Vec3d(m_shadowOffset.x() * temp, -m_shadowOffset.y() * temp, 0));
                    shadowMatrix.mul(translationMatrix, shadowMatrix);
                    _gfxDeviceAdapter->setMatrixTransform(shadowMatrix);
                    _gfxDeviceAdapter->setShaderForFont(bfalse, bfalse, fontParam, m_gradientColor);
                    Color color = m_shadowColor;
                    color.setAlpha(m_shadowColor.getAlpha() * m_color.getAlpha());
                    _gfxDeviceAdapter->setGlobalColor(color);
                    _gfxDeviceAdapter->setVertexBuffer(mesh.getCurrentVB());
                    _gfxDeviceAdapter->DrawVertexBuffer(drawcallCtx, GFX_TRIANGLES, 0, mesh.getCurrentVB()->m_nVertex);
                }

                // draw
                setupScissor(_gfxDeviceAdapter);

                _gfxDeviceAdapter->setShaderForFont(bfalse, m_useGradient, fontParam, m_gradientColor);
                _gfxDeviceAdapter->setGlobalColor(m_color);
                _gfxDeviceAdapter->setMatrixTransform(finalMatrix);
                _gfxDeviceAdapter->setVertexBuffer(mesh.getCurrentVB());
                _gfxDeviceAdapter->DrawVertexBuffer(drawcallCtx, GFX_TRIANGLES, 0, mesh.getCurrentVB()->m_nVertex );
            }
        }
    }

    resetScissor(_gfxDeviceAdapter);
}

void FontText::freeMesh()
{
    for(ITF_VECTOR<ITF_Mesh>::iterator it = m_meshByPage.begin(); it != m_meshByPage.end(); ++it)
        it->removeVertexBuffer(1);

    m_meshByPage.clear();
}

void FontText::directDraw(const RenderPassContext & _rdrPassCtxt, class GFXAdapter * _gfxDeviceAdapter, f32 _Z, f32 _ZWorld, f32 _ZAbs)
{
    PRF_M_SCOPE(drawText);

    PrimitiveContext primCtx;
    primCtx.setRenderPassContext((RenderPassContext*)&_rdrPassCtxt)
        .setPrimitiveParam(getCommonParam())
        .setCamZ(_Z,_ZWorld, _ZAbs);

    computeMeshByPage();

    drawMesh(primCtx, _gfxDeviceAdapter);
}

void FontText::setEffectChain(FontEffectChain *_effectChain )
{
    m_effectChain = _effectChain;
}

void FontText::setText(const String8 &_text)
{
    if(_text != m_text)
    {
        m_text = _text;
        m_bIsDirty = btrue;
    }
}

void FontText::appendText(const String8 &_text)
{
    if(!_text.isEmpty())
    {
        m_text += _text;
        m_bIsDirty = btrue;
    }
}

void FontText::setFontTemplate(const FontTemplate &_font)
{
    if(&_font != m_font)
    {
        m_font = &_font;
        m_bIsDirty = btrue;
    }
}

void FontText::setLocalPivot(const Vec2d &_localPivot)
{
    if(_localPivot != m_localPivot)
    {
        m_localPivot = _localPivot;
        m_bIsDirty = btrue;
    }
}

void FontText::setCharSpacing( f32 _charSpacing )
{
    if (m_charSpacing == _charSpacing) return;
    m_charSpacing = _charSpacing;
    m_bIsDirty = btrue;
}

void FontText::setAdvanceCharSize(char _constAdvanceChar_Size)
{
    if(m_constAdvanceChar_Size !=  _constAdvanceChar_Size)
    {
        m_constAdvanceChar_Size = _constAdvanceChar_Size;
        m_bIsDirty = btrue;
    }
}

void FontText::setAdvanceCharMin(char _constAdvanceChar_Min)
{
    if(m_constAdvanceChar_Min != _constAdvanceChar_Min)
    {
        m_constAdvanceChar_Min = _constAdvanceChar_Min;
        m_bIsDirty = btrue;
    }
}

void FontText::setAdvanceCharMax(char _constAdvanceChar_Max)
{
    if(m_constAdvanceChar_Max != _constAdvanceChar_Max)
    {
        m_constAdvanceChar_Max = _constAdvanceChar_Max;
        m_bIsDirty = btrue;
    }
}

    //=================================================================================================
    //
    // Font Effect : use to add small effect on position / size / rotation of text letters.
    //
    //=================================================================================================

    //---------------------------------------------------------------------------------------------
    FontEffect::FontEffect()
    {
        m_active = bfalse;
        m_finished = bfalse;
        m_forceFinish = bfalse;
        m_time = 0;
        m_ratio = 0;
        m_user = 0;
        m_params = NULL;
    }

    //---------------------------------------------------------------------------------------------
    // activate/deactivate effect
    //
    void FontEffect::activate( bbool _active)
    {
        m_active = _active;
    }

    //---------------------------------------------------------------------------------------------
    // reset all activation flags and time counter
    //
    void FontEffect::reset()
    {
        timeReset();
        m_active = bfalse;
    }

    //---------------------------------------------------------------------------------------------
    // reset just time (and finished flag)
    //
    void FontEffect::timeReset()
    {
        m_finished = bfalse;
        m_time = 0;
    }

    //---------------------------------------------------------------------------------------------
    // apply shiftY effect
    //
    void FontEffect::shiftYApply( f32 _seed, Vec3d &_P0, Vec3d &_P1, Vec3d &_P2, Vec3d &_P3 )
    {
        // compute time
        f32 time = MTH_2PI * (_seed + m_time * (m_params->speedMin() + _seed * (m_params->speedMax() - m_params->speedMin())));
        // compute delta Y
        f32 deltaY = m_ratio * (m_params->shiftYMin() + 0.5f * (f32_Sin( time) + 1) *( m_params->shiftYMax()- m_params->shiftYMin()));

        _P0.y() += deltaY;
        _P1.y() += deltaY;
        _P2.y() += deltaY;
        _P3.y() += deltaY;
    
        // test for floor effect
        //if (m_params->shiftYLimit() != -1 && deltaY < m_params->shiftYLimit())
        //    _h = _h + deltaY + m_params->shiftYLimit();
    }

    //---------------------------------------------------------------------------------------------
    // apply scale effect
    //
    void FontEffect::scaleApply( f32 _seed, Vec3d &_P0, Vec3d &_P1, Vec3d &_P2, Vec3d &_P3 )
    {
        // compute time
        f32 time = MTH_2PI * ( _seed + m_time * (m_params->speedMin() + _seed * (m_params->speedMax() - m_params->speedMin())));
        // and new scale
        f32 scale = 1 + (m_ratio * (m_params->scaleMin() + 0.5f * (f32_Sin( time) + 1) *( m_params->scaleMax() - m_params->scaleMin())));

        Vec3d center = (_P0 + _P1 + _P2 + _P3) / 4;

        _P0 = center + scale * (_P0 - center);
        _P1 = center + scale * (_P1 - center);
        _P2 = center + scale * (_P2 - center);
        _P3 = center + scale * (_P3 - center);
    }

    //---------------------------------------------------------------------------------------------
    // apply rotate effect
    //
    void FontEffect::rotateApply( f32 _seed, Vec3d &_P0, Vec3d &_P1, Vec3d &_P2, Vec3d &_P3, const Vec2d &_area )
    {
        // compute time
        //f32 time = MTH_2PI * (_seed + m_time * (m_params->speedMin() + _seed * (m_params->speedMax() - m_params->speedMin())));
        f32 rotateCycle = 2 * _area.x();
        if (rotateCycle != 0) rotateCycle = f32_Inv(rotateCycle);
        rotateCycle *= m_params->rotateCycle();

        f32 time = MTH_2PI * ( rotateCycle * (_P0.x() + _P1.x()) + _seed * m_params->rotateSeedFactor() + m_time * (m_params->speedMin() + _seed * (m_params->speedMax() - m_params->speedMin())));

        f32 rotate = m_ratio * (f32_Sin( time ) * m_params->rotateAngle() );
        f32 cos = f32_Cos( rotate );
        f32 sin = f32_Sin( rotate );

        Vec3d center = (_P0 + _P1 + _P2 + _P3) / 4;
        Vec3d diff;
        f32 oldX;

        diff = _P0 - center;
        oldX = diff.x();
        diff.x() = cos * oldX - sin * diff.y();
        diff.y() = sin * oldX + cos * diff.y();
        _P0 = center + diff;

        diff = _P1 - center;
        oldX = diff.x();
        diff.x() = cos * oldX - sin * diff.y();
        diff.y() = sin * oldX + cos * diff.y();
        _P1 = center + diff;
    
        diff = _P2 - center;
        oldX = diff.x();
        diff.x() = cos * oldX - sin * diff.y();
        diff.y() = sin * oldX + cos * diff.y();
        _P2 = center + diff;
    
        diff = _P3 - center;
        oldX = diff.x();
        diff.x() = cos * oldX - sin * diff.y();
        diff.y() = sin * oldX + cos * diff.y();
        _P3 = center + diff;
    }

    //---------------------------------------------------------------------------------------------
    // apply zoom alpha effect
    //
    void FontEffect::zoomAlphaApply( f32 _seed, Vec3d &_P0, Vec3d &_P1, Vec3d &_P2, Vec3d &_P3, u32 &_color)
    {
        f32 totalTime = m_params->speedMin() + _seed * (m_params->speedMax() - m_params->speedMin());
        f32 time = m_time - m_params->fadeinStart(); 
        if (time < 0) return;
        if (time > totalTime) time = totalTime;
        if (getTemplate()->zoomIsReverse())
            time = totalTime - time;

        f32 tRef = (0.9f + _seed * 0.05f) * totalTime;
        f32 ratio, scale;

        if (time < tRef)
        {
            ratio = (tRef - time) / tRef;
            scale = 1.0f + ( ratio * ratio * (m_params->zoomAlphaMax() - m_params->zoomAlphaMin()) + m_params->zoomAlphaMin());
            ratio = 1 - (ratio * ratio);
        }
        else
        {
            ratio = (time - tRef) / (totalTime - tRef);
            scale = 1.0f + m_params->zoomAlphaMin() * ( 1 - ratio * ratio);
            ratio = 1;
        }
        

        Vec3d center = (_P0 + _P1 + _P2 + _P3) / 4;

        _P0 = center + scale * (_P0 - center);
        _P1 = center + scale * (_P1 - center);
        _P2 = center + scale * (_P2 - center);
        _P3 = center + scale * (_P3 - center);

        _color &= 0xFFFFFF;
        _color |= ((u32) (255 * ratio)) << 24;
        
    }

    //---------------------------------------------------------------------------------------------
    // update FontEffect timer and ratio
    // ratio is a percentage of the effect depending on time marker (fade in / out)
    void FontEffect::finish( )
    {
        m_ratio = 0;
        m_finished = btrue;
        m_active = bfalse;
    }

    //---------------------------------------------------------------------------------------------
    // update FontEffect timer and ratio
    // ratio is a percentage of the effect depending on time marker (fade in / out)
    //
#define Time_FinishLength   0.2f


    void FontEffect::update( f32 _deltaTime)
    {
        if(!m_active || !m_params) return;

        // bof : add in emergency of a special case for infinite effect to force smooth disappear
        // infinite effect have no fade out data so time is set to fadeinEnd and decrease to fadeinStart and then disappear
        if (m_forceFinish)
        {
            m_time -= _deltaTime;
            if (m_time < 0) finish();
            else if (m_time < Time_FinishLength) m_ratio = m_time / Time_FinishLength;
            else m_ratio = 1;
            return;
        }
        
        if (m_params->isStatic()) 
        {
            m_ratio = 1;
            return;
        }

        m_time += _deltaTime;

        if (m_params->type() == FontEffect_Template::zoomAlpha)
        {
            if (m_time > m_params->speedMax() + m_params->fadeinStart() )
                finish();
            return;
        }

        if (m_time < m_params->fadeinStart()) m_ratio = 0;
        else if (m_time < m_params->fadeinEnd()) m_ratio = (m_time - m_params->fadeinStart()) / (m_params->fadeinEnd() - m_params->fadeinStart());
        else if( m_time < m_params->fadeoutStart()|| m_params->fadeoutStart() == -1) m_ratio = 1;
        else if( m_time < m_params->fadeoutEnd()) m_ratio = (m_time - m_params->fadeoutEnd()) / (m_params->fadeoutStart() - m_params->fadeoutEnd());
        else finish();
    }

    //---------------------------------------------------------------------------------------------
    // update FontEffect timer and ratio
    // ratio is a percentage of the effect depending on time marker (fade in / out)
    void FontEffect::forceFinish()
    {
        if (m_forceFinish) return;
        m_forceFinish = true;
        m_time = m_ratio * Time_FinishLength;
    }

    //---------------------------------------------------------------------------------------------
    void FontEffect::apply( Vec3d &_P0, Vec3d &_P1, Vec3d &_P2, Vec3d &_P3, u32 &_color, f32 _seed, const Vec2d &_area )
    {
        if (!m_params) return;
        if (m_params->isStatic())
            _seed = f32_Modulo(_seed + m_params->staticSeed(), 1.0f);
        
        switch (m_params->type())
        {
        case FontEffect_Template::shiftY:               shiftYApply(_seed, _P0, _P1, _P2, _P3); break;
        case FontEffect_Template::scale:                scaleApply(_seed, _P0, _P1, _P2, _P3); break;
        case FontEffect_Template::rotate:               rotateApply(_seed, _P0, _P1, _P2, _P3, _area); break;
        case FontEffect_Template::zoomAlpha:            zoomAlphaApply(_seed, _P0, _P1, _P2, _P3, _color); break;
        case FontEffect_Template::shiftYandRotate:    
            shiftYApply(_seed, _P0, _P1, _P2, _P3);
            rotateApply(_seed, _P0, _P1, _P2, _P3, _area); 
            break;
        }
    }

}
