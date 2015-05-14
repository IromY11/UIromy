 #include "precompiled_engine.h"

#ifndef _ITF_FONTTEXTAREA_H_
#include "FontTextArea.h"
#endif //_ITF_FONTTEXTAREA_H_

#ifndef ITF_CORE_UNICODE_TOOLS_H_
#include "core/UnicodeTools.h"
#endif //ITF_CORE_UNICODE_TOOLS_H_

#ifndef _ITF_FONTTEXT_H_
#include "engine/display/FontText.h"
#endif //_ITF_FONTTEXT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif // _ITF_GFX_ADAPTER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FONTTEMPLATE_H_
#include "engine/display/FontTemplate.h"
#endif //_ITF_FONTTEMPLATE_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif // _ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif // _ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_TEXTBOXCOMPONENT_H_
#include "gameplay/components/UI/TextBoxComponent.h"
#endif //_ITF_TEXTBOXCOMPONENT_H_

#ifndef _ITF_W1W_COMICSTEXTBOXCOMPONENT_H_
#include "W1W/HUD/W1W_ComicsTextBoxComponent.h"
#endif //_ITF_W1W_COMICSTEXTBOXCOMPONENT_H_

namespace ITF
{
    static int useInterligne = btrue;

    BEGIN_SERIALIZATION(FontTextArea)
        SERIALIZE_DESCRIPTION(FontSet)
        SERIALIZE_DESCRIPTION(Style)
    END_SERIALIZATION()


FontTextArea::FontTextArea():
    m_isPassword(bfalse),
    m_linesCount(-1),
    m_scrollStartLine(-1),
    m_scrollLineCount(0),
    m_scrollDelta(0.0f),
    m_scrollDrawOffset(0.0f),
    m_needUpdate(bfalse),
    m_needAlign(bfalse),
    m_needParsing(bfalse),
    m_needTransform(bfalse),
    m_updateDate(0.0f),
    m_parsingDate(0.0f),
    m_isVisible(btrue),
    m_isActive(btrue),
    m_is2D(bfalse),
    m_font(NULL),
    m_size(-1),
    m_color(Color::white()),
    m_alpha(1.0),
    m_area(Vec2d(-1.0f, -1.0f)),
    m_hAlignment(FONT_ALIGN_LEFT),
    m_vAlignment(FONT_ALIGN_TOP),
    m_position(Vec3d::Zero),
    m_localPivot(Vec2d::Zero),
    m_localScale(Vec2d(1, 1)),
    m_localAngle(0.0),
    m_shadowOffset(Vec2d::Zero),
    m_shadowColor(Color::black()),
    m_anchor(AREA_ANCHOR_TOP_LEFT),
    m_constAdvanceChar_Size(0),
    m_constAdvanceChar_Min(0),
    m_constAdvanceChar_Max(0),
    m_displayedCharCount(-1),
    m_viewMask(U32_INVALID),
    m_lineSpacing(0.0f),
    m_paragraphSpacing(0.0f),
    m_charSpacing(0.0f),
    m_rubySpacing(0.0f),
    m_maxWidth(-1.0f),
    m_scrollXSize(0.0f),
    m_scrollXOffset(0.0f),
    m_scrollXTime(0.0f),
    m_scrollXSpeed(1.0f),
    m_scrollXWaitTime(0.0f),
    m_scrollXLoop(bfalse),
    m_scrollXLoopGap(1),
    m_owner(NULL),
    m_globalActorScaleFactor(1.f),
    m_viewportVisibility(0x0000FFFF)
{
}

FontTextArea::~FontTextArea()
{
    clear();
}

void FontTextArea::setText(const String8& _text) 
{
    if(m_text!=_text)
    {
        m_text = _text; 
        m_displayText = (m_scrollXLoop ? m_text + m_scrollXLoopGapString + m_text + m_scrollXLoopGapString : m_text);
        m_needParsing = btrue;
    }
}

void FontTextArea::setAutoScrollXLoopGap(u32 _gap)
{
    if (m_scrollXLoopGap != _gap)
    {
        m_scrollXLoopGap = _gap;
        m_scrollXLoopGapString.clear();
        m_scrollXLoopGapString.reserve(_gap);
        for (u32 i = 0; i < _gap; ++i)
            m_scrollXLoopGapString += ' ';
        if (m_scrollXLoop)
        {
            m_displayText = m_text + m_scrollXLoopGapString + m_text + m_scrollXLoopGapString;
            m_needParsing = true;
        }
    }
}

void FontTextArea::setAutoScrollXLoop(bbool _loop)
{
    if (m_scrollXLoop != _loop)
    {
        m_scrollXLoop = _loop;
        m_displayText = m_scrollXLoop ? m_text + m_scrollXLoopGapString + m_text + m_scrollXLoopGapString : m_text;
        m_needParsing = true;
    }
}

//---------------------------------------------------------------------------------------------------------------------
// String manipulation : insert _textToInsert in m_text, insertion point is determined by _marker value
//
void FontTextArea::insertTextAtMarker( u32 _marker, const String8 &_textToInsert)
{
    insertText( TEXT_MARKER | _marker, _textToInsert );
}

//---------------------------------------------------------------------------------------------------------------------
// static version of previous function
//
void FontTextArea::insertTextAtMarker( String8 &_text, u32 _marker, const String8 &_textToInsert)
{
    insertText( _text, TEXT_MARKER | _marker, _textToInsert );
}

//---------------------------------------------------------------------------------------------------------------------
// String manipulation : insert _textToInsert into m_text, insertion point is determined by _where value
// _where & TEXT_MARKER => marker index
// _where == -1 => at the end of string
// _where between 0 and string length => index of char
//
void FontTextArea::insertText( u32 _where, const String8 &_textToInsert)
{
    insertText( m_text, _where, _textToInsert);
    if (m_scrollXLoop)
    {
        m_displayText = m_text + m_scrollXLoopGapString + m_text + m_scrollXLoopGapString;
    }
    else
    {
        insertText( m_displayText, _where, _textToInsert);
    }
    m_needParsing = btrue;
}

//---------------------------------------------------------------------------------------------------------------------
// static version of previous function
//
void FontTextArea::insertText( String8 &_text, u32 _where, const String8 &_textToInsert)
{
    if (_where & TEXT_MARKER)
    {
        _where = (u32) getMarkIndex(_text, _where - TEXT_MARKER);
        if (_where == (u32) -1)
            return;
    }

    u32 len = _text.getLen();
    if (_where == U32_INVALID || _where > len)
        _text = _text + _textToInsert;
    else
        _text = _text.substr(0, _where) + _textToInsert + _text.substr(_where);
}

void FontTextArea::setFont(const FontTemplate* _font) 
{
    if(m_font!=_font)
    {
        m_font = _font; 
        m_needParsing = btrue;
    }
}

void FontTextArea::setStyle(const Style& _style)
{
    setFont(_style.m_fontTemplate);
    setSize(_style.m_fontSize);
    setColor(_style.m_color);        
    setHAlignment(_style.m_hAlignment);
    setVAlignment(_style.m_vAlignment);
    setShadow(_style.m_shadowOffset, _style.m_shadowColor);
    setAnchor(_style.m_anchor);
    setLineSpacing(useInterligne ? _style.m_lineSpacing : 0.0f);
    setParagraphSpacing(useInterligne ? _style.m_paragraphSpacing : 0.0f);
    setCharSpacing(_style.m_charSpacing);
    setGradient(_style.m_useGradient, _style.m_gradientSize, _style.m_gradientColor, _style.m_gradientOffset);

    m_rubySpacing = 0.0f;
    { //*************** TWEAK MODE ON  ***************//
        // Auto-adjust ruby spacing in Japanese
        ITF_LANGUAGE curlang = LOCALISATIONMANAGER->getCurrentLanguage();
        if ((curlang == ITF_LANGUAGE_JAPANESE) && !_style.m_fontSet.m_japanese.isEmpty())
            m_rubySpacing = (0.1f * _style.m_fontSize);
    } //*************** TWEAK MODE OFF ***************//
}

void FontTextArea::setColor( const Color _color)
{
    m_color = _color;
}

void FontTextArea::setSize(const f32 _size) 
{
    if(m_size!=_size)
    {
        m_size = _size;
        m_needParsing = btrue;
    }
}

f32 FontTextArea::getSize() const
{
    return m_size;
}

void FontTextArea::setHAlignment(const FONT_ALIGNMENT _hAlignment) 
{
    if(m_hAlignment!=_hAlignment)
    {
        m_hAlignment = _hAlignment; 
        m_needUpdate = btrue;
    }
}

void FontTextArea::setVAlignment(const FONT_ALIGNMENT _vAlignment) 
{
    if(m_vAlignment!=_vAlignment)
    {
        m_vAlignment = _vAlignment; 
        m_needUpdate = btrue;
    }
}

void FontTextArea::setText(const String8 & _text, FontTemplate* _font, const f32 _size, const Color _color) 
{
    setText(_text);
    setFont(_font);
    setSize(_size);
    setColor(_color);
}

void FontTextArea::setAlignment(const FONT_ALIGNMENT _hAlignment, const FONT_ALIGNMENT _vAlignment) 
{
    setHAlignment(_hAlignment); 
    setVAlignment(_vAlignment);
}

void FontTextArea::setConstAdvanceChar(const char _constAdvanceChar_Size, const char _constAdvanceChar_Min, const char _constAdvanceChar_Max) 
{
    if(m_constAdvanceChar_Size!=_constAdvanceChar_Size || m_constAdvanceChar_Min!=_constAdvanceChar_Min || m_constAdvanceChar_Max!=_constAdvanceChar_Max)
    {
        m_constAdvanceChar_Size = _constAdvanceChar_Size; 
        m_constAdvanceChar_Min = _constAdvanceChar_Min;
        m_constAdvanceChar_Max = _constAdvanceChar_Max;

        m_needUpdate = btrue;
    }
}

void FontTextArea::setArea(const Vec2d& _area) 
{
    if(m_area!=_area)
    {
        m_area = _area; 
        m_needParsing = btrue;
    }
}

void FontTextArea::setOwner(Actor* _owner) 
{
    if(m_owner!=_owner)
    {
        m_owner = _owner; 
        m_needUpdate = btrue;
    }
}

void FontTextArea::setLocalPivot(const Vec2d& _localPivot) 
{
    if(m_localPivot!=_localPivot)
    {
        m_localPivot = _localPivot;
        m_needTransform = btrue;
    }
}

void FontTextArea::setLocalScale(const Vec2d& _localScale) 
{
    if(m_localScale!=_localScale)
    {
        m_localScale = _localScale;
        m_needTransform = btrue;
    }
}
void FontTextArea::setLocalAngle(const f32 _localAngle) 
{
    if(m_localAngle!=_localAngle)
    {
        m_localAngle = _localAngle;
        m_needTransform = btrue;
    }
}

void FontTextArea::setPosition(const Vec3d& _position) 
{
    if(m_position!=_position) 
    {
        m_position = _position; 
        m_needTransform = btrue;
    }
}

void FontTextArea::setMaxWidth(f32 _maxWidth)
{
    if(m_maxWidth != _maxWidth)
    {
        m_maxWidth = _maxWidth;
        m_needAlign = btrue;
    }
}


//-------------------------------------------------------------------------------------------------
// update font text area
// VL : add deltaTime parameters for effect update
//
void FontTextArea::Update(f32 _deltaTime)
{
    ///////////////////////////
    // VL temp for test, keep it a little ...
    /*if (m_text.substr(0,12) == "Checkpoint 1")
    {
        static bbool sclear = bfalse;
        static i32 sadd = 0;
        static i32 start = -1;
        static i32 end = -1;
        
        if (sclear)
            clearFontEffect();
        else if (sadd == 1)
            addFontEffect( UI_TEXTMANAGER->getFontEffect( "scale" ), start, end);
        else if (sadd == 2)
            addFontEffect( UI_TEXTMANAGER->getFontEffect( "shiftY" ), start, end);
        else if (sadd == 3)
            addFontEffect( UI_TEXTMANAGER->getFontEffect( "chill" ), start, end);
        else if (sadd == 4)
            addFontEffect( UI_TEXTMANAGER->getFontEffect( "scaleTest" ), start, end);
        else if (sadd == 4)
            addFontEffect( UI_TEXTMANAGER->getFontEffect( "zoomAlpha" ), start, end);
    }
    */

    //----- update effects ------------------------------------------------------------------------
    if (m_dynamicFontEffectsToAdd.size())
    {
        for (u32 _index = 0; _index < m_dynamicFontEffectsToAdd.size(); _index++)
            m_dynamicFontEffects.push_back(m_dynamicFontEffectsToAdd[0]);
        m_dynamicFontEffectsToAdd.clear();
        m_needAlign = btrue;
    }

    for ( ITF_VECTOR<FontEffect>::iterator fontEffect = m_fontEffects.begin(); fontEffect!= m_fontEffects.end();++fontEffect)
        fontEffect->update(_deltaTime);
    for ( ITF_VECTOR<FontEffect>::iterator fontEffect = m_dynamicFontEffects.begin(); fontEffect!= m_dynamicFontEffects.end();++fontEffect)
        fontEffect->update(_deltaTime);

    updateScroll(_deltaTime);

    //----- update spawned actor ------------------------------------------------------------------
    if(m_owner && m_owner->getScene())
    {
        for ( ITF_VECTOR<BlockActorData>::iterator actorSpawner = m_spawnedActors.begin(); actorSpawner!= m_spawnedActors.end();++actorSpawner)
            actorSpawner->m_spawner.update(m_owner->getScene());
    }

    //----- refresh data if needed  ---------------------------------------------------------------
    if (m_font)
    {
        if(m_needParsing)
        {
            clearParsedActors();
            m_fontTexts.clear();
            m_textBlocks.clear();
            m_linesBounds.clear();
            m_linesCount = 0;

            computeBlockParsing();
            clearSpawnedActors();

            m_parsingDate = ELAPSEDTIME;
            m_needParsing=bfalse;
            m_needUpdate = btrue;
        }

        if(m_needUpdate)
        {
            computeBlockSize();

            m_needUpdate = bfalse;
            m_needAlign = btrue;
        }

        if(m_needAlign && areActorsReady())
        {
            computeBlockActor();
            computeBlockLine();
            computeBlockAlignment();
            computeBlockText();
            computeFontEffectChain();
            computeDisplayAreaLines();

            m_needAlign = bfalse;
            m_needTransform = btrue;
        }

        if(m_needTransform)
        {
            computeTransform();
            computeScroll();

            m_needTransform = bfalse;
            m_updateDate = ELAPSEDTIME;
        }

        computeTransformActors();
    }
}

void FontTextArea::clear()
{
    clearActors();
    m_fontTexts.clear();
    m_textBlocks.clear();
    m_fontEffects.clear();
    m_dynamicFontEffects.clear();
    m_fontEffectChain.clear();
    m_linesBounds.clear();
    m_linesCount = 0;
}

void FontTextArea::Draw(const ITF_VECTOR <class View*>& _views, f32 _depthOffset,  const ConstObjectRef & _objectRef)
{
    if(m_isVisible && m_isActive)
    {
        i32 lastBlockIndex = -1;
        for ( ITF_VECTOR<FontTextBlock>::iterator block = m_textBlocks.begin(); block!= m_textBlocks.end();++block)
        {
            //AABB blockAABB = computeBlockAABB(*block);
            //Vec2d bottomRight(blockAABB.getMax().x(), blockAABB.getMin().y());
            //Vec2d topLeft(blockAABB.getMin().x(), blockAABB.getMax().y());
            //if(m_is2D)
            //{
            //    GFX_ADAPTER->drawDBG2dLine(blockAABB.getMin(), topLeft, 1, Color::cyan().getAsU32());
            //    GFX_ADAPTER->drawDBG2dLine(topLeft, blockAABB.getMax(), 1, Color::cyan().getAsU32());
            //    GFX_ADAPTER->drawDBG2dLine(blockAABB.getMax(), bottomRight, 1, Color::cyan().getAsU32());
            //    GFX_ADAPTER->drawDBG2dLine(bottomRight, blockAABB.getMin(), 1, Color::cyan().getAsU32());
            //}
            //else
            //{
            //    DebugDraw::line2D(blockAABB.getMin(), topLeft, m_position.z(), Color::cyan().getAsU32());
            //    DebugDraw::line2D(topLeft, blockAABB.getMax(), m_position.z(), Color::cyan().getAsU32());
            //    DebugDraw::line2D(blockAABB.getMax(), bottomRight, m_position.z(), Color::cyan().getAsU32());
            //    DebugDraw::line2D(bottomRight, blockAABB.getMin(), m_position.z(), Color::cyan().getAsU32());
            //}

            // if line is valid
                // if scrolling is enabled, or block is inside of scroll-viewport
            if(block->m_line != -1 && (m_scrollStartLine == -1 || (block->m_line >= m_scrollStartLine && block->m_line < m_scrollStartLine + m_scrollLineCount))) 
            {
                if(block->m_type == TEXT_BLOCK_WORD && block->m_index!=-1 && block->m_index!=lastBlockIndex)
                {
                    Color fontColor(block->m_color != 0 ? block->m_color : m_color);
                    Color shadowColor(m_shadowColor);
                    Color gradiantColor(m_gradientColor);

                    fontColor.setAlpha(fontColor.getAlpha() * m_alpha);
                    shadowColor.setAlpha(shadowColor.getAlpha() * m_alpha);
                    gradiantColor.setAlpha(gradiantColor.getAlpha() * m_alpha);

                    FontText& fontText = m_fontTexts[block->m_index];
                    fontText.setColor(fontColor);
                    fontText.setSize(block->m_size);
                    fontText.setShadowColor(shadowColor);
                    fontText.setShadowOffset(m_shadowOffset);
                    fontText.setUseGradient(m_useGradient);
                    fontText.setGradientScale(m_gradientSize);
                    fontText.setGradientOffset(m_gradientOffset);
                    fontText.setGradientColor(gradiantColor);
                    fontText.setArea(getTextAreaBound());
                    fontText.setOffset(Vec3d(-m_scrollXOffset, m_scrollDrawOffset, 0.0f));
                    fontText.setViewportVisibility(m_viewportVisibility);

                    if(m_maxWidth > 0.0f)
                    {
                        fontText.setClippingRect(getAABB());
                    }
                    
                    GFX_ADAPTER->drawFontText(_views, &fontText, _depthOffset, _objectRef);

                    lastBlockIndex = block->m_index;
                }
                else if(block->m_type == TEXT_BLOCK_ACTOR && block->m_actorIndex != U32_INVALID)
                {
                    if (Actor* actor = m_spawnedActors[block->m_actorIndex].m_ref.getActor())
                    {
                        if(!actor->isAsyncLoading())
                        {
                            if(WorldUpdateElement* element = actor->getWorldUpdateElement())
                            {
                                element->setViewMask( m_viewMask );
                            }

                            if(GraphicComponent* graphicComponent = actor->GetComponent<GraphicComponent>())
                            {
                                graphicComponent->setDepthOffset(_depthOffset);
                            }

                            if(TextBoxComponent* textBoxComponent = actor->GetComponent<TextBoxComponent>())
                            {
                                textBoxComponent->setDepthOffset(_depthOffset);
							}

							if(W1W_ComicsTextBoxComponent* textBoxComponent = actor->GetComponent<W1W_ComicsTextBoxComponent>())
							{
								textBoxComponent->setDepthOffset(_depthOffset);
							}
                        }

                        // JR: display the actors manually to avoid lost frames
                        if(!actor->isActive())
                        {
                            AnimLightComponent* anim = actor->GetComponent<AnimLightComponent>();
                            if (!anim || anim->isLoaded())
                            {
                                if(actor->getIs2D())
                                    actor->batchPrimitives2D(_views);
                                else
                                    actor->batchPrimitives(_views);
                            }
                        }
                    }
                }
            }
        }
    }
}

void FontTextArea::computePivotMulMatrixOffset(Matrix44& _finalMatrix, bbool _useRotation) const
{
    Matrix44 offsetMatrix;
    Vec3d    topLeft = getLocalTopLeftOffset(getTextAreaBound(), Vec2d::Zero, !m_is2D).to3d();
    offsetMatrix.setIdentity34();
    offsetMatrix.setTranslation(topLeft);

    Matrix44 localMatrix;
    localMatrix.setIdentity34();
    if (_useRotation)
        localMatrix.setRotationZ(m_is2D ? -m_localAngle : m_localAngle);

    localMatrix.mulScale(m_localScale.to3d(1.f));
    localMatrix.setTranslation(Vec3d(m_localPivot.x(), m_localPivot.y(), 0));

    localMatrix.mul44(offsetMatrix, localMatrix);

    Matrix44 transformMatrix;
    transformMatrix.setIdentity34();
    transformMatrix.setTranslation(m_position);

    _finalMatrix.mul44(localMatrix, transformMatrix);
}


void FontTextArea::computeGlobalPosition(Vec3d& _globalPos, const Vec3d& _localPosition, const Matrix44& _anchorMulMatrix) const
{
    Vec3d localAnchorPos = _localPosition - Vec3d(m_localPivot.x(), m_localPivot.y(), 0.0);

    _anchorMulMatrix.transformVector(_globalPos, localAnchorPos);
    Vec3d translation;
    storeXYZ((float *)&translation,_anchorMulMatrix.T());
    _globalPos += translation;
}

void FontTextArea::computeGlobalScale(Vec2d& _globalScale, const Vec2d& _localScale, const Matrix44& _mulMatrix) const
{
    Vec3d finalScale;
    _mulMatrix.transformVector(finalScale, _localScale.to3d(1.f));

    _globalScale = finalScale.truncateTo2D();
}

AABB FontTextArea::computeBlockAABB(const FontTextBlock& _block) const
{
    Vec2d textAreaBound = _block.m_bound;
    Vec3d textAreaPos = _block.m_position.to3d();


    Matrix44 pivotMulMatrix;
    computePivotMulMatrixOffset(pivotMulMatrix, btrue);

    Vec3d cornerA, cornerB, cornerC, origin;
    if(m_is2D)
    {
        computeGlobalPosition(origin, textAreaPos, pivotMulMatrix);
        computeGlobalPosition(cornerA, Vec3d(textAreaBound.x(), 0.0, 0.0) + textAreaPos, pivotMulMatrix);
        computeGlobalPosition(cornerB, Vec3d(0.0, textAreaBound.y() * -1.0f, 0.0) + textAreaPos, pivotMulMatrix);
        computeGlobalPosition(cornerC, Vec3d(textAreaBound.x(), textAreaBound.y() * -1.0f, 0.0) + textAreaPos, pivotMulMatrix);
    }
    else
    {
        computeGlobalPosition(origin, textAreaPos, pivotMulMatrix);
        computeGlobalPosition(cornerA, Vec3d(textAreaBound.x(), 0.0, 0.0) + textAreaPos, pivotMulMatrix);
        computeGlobalPosition(cornerB, Vec3d(0.0, textAreaBound.y(), 0.0) + textAreaPos, pivotMulMatrix);
        computeGlobalPosition(cornerC, Vec3d(textAreaBound.x(), textAreaBound.y(), 0.0) + textAreaPos, pivotMulMatrix);
    }

    Vec3d translation;
    storeXYZ((float *)&translation,pivotMulMatrix.T());
    AABB globalAABB(origin);
    globalAABB.grow(cornerA);
    globalAABB.grow(cornerB);
    globalAABB.grow(cornerC);

    return globalAABB;
}



FontTextArea::BlockActorData * FontTextArea::getBlockWithRef(const ActorRef & _ref)
{
    for ( ITF_VECTOR<BlockActorData>::iterator actorData = m_spawnedActors.begin(); 
        actorData!= m_spawnedActors.end(); ++actorData)
    {
        if(actorData->m_ref == _ref )
        {
            return &(*actorData);
        }
    }
    return NULL;
}

void FontTextArea::updateScroll(f32 _dt)
{
    m_scrollXOffset = 0.0f;
    if(m_scrollXSize > 0.0f)
    {
        if(m_scrollXSpeed != 0.0f)
        {
            m_scrollXTime += _dt;
            if(m_scrollXSpeed > 0.0f)
            {
                if (m_scrollXLoop)
                {
                    m_scrollXOffset = m_scrollXTime * m_scrollXSpeed;
                }
                else
                {
                    m_scrollXOffset = Min(m_scrollXSize, m_scrollXTime * m_scrollXSpeed);
                }
            }
            else
            {
                if (m_scrollXLoop)
                {
                    m_scrollXOffset = m_scrollXSize + m_scrollXTime * m_scrollXSpeed;
                }
                else
                {
                    m_scrollXOffset = Max(0.0f, m_scrollXSize + m_scrollXTime * m_scrollXSpeed);
                }
            }

            if((m_scrollXTime - (m_scrollXLoop ? 0 : m_scrollXWaitTime)) * fabs(m_scrollXSpeed) >= m_scrollXSize)
            {
                m_scrollXTime = 0.0f;
                if (!m_scrollXLoop)
                    m_scrollXSpeed = -m_scrollXSpeed;
            }
        }

        if(m_hAlignment == FONT_ALIGN_RIGHT)
            m_scrollXOffset = -m_scrollXOffset;
        else if(m_hAlignment == FONT_ALIGN_CENTER)
            m_scrollXOffset -= m_scrollXSize * 0.5f;
    }
}

void FontTextArea::setIsVisible(const bbool _isVisible)
{
    if (m_isVisible !=_isVisible)
    {
        if(!m_needAlign)
        {
            for ( ITF_VECTOR<BlockActorData>::iterator actorData = m_spawnedActors.begin(); 
                actorData!= m_spawnedActors.end(); ++actorData)
            {
                if(Actor* actor = actorData->m_ref.getActor())
                {
                    if (m_isActive && _isVisible && m_parsedActors.find(actorData->m_ref)!=-1) 
                        actor->enable();
                    else
                        actor->disable();
                }
            }
        }

        m_isVisible = _isVisible;
    }
}

void FontTextArea::setIsActive(const bbool _isActive)
{
    if (m_isActive ==_isActive)
        return;

    for ( ITF_VECTOR<ActorRef>::iterator actorRef = m_parsedActors.begin(); 
        actorRef!= m_parsedActors.end(); ++actorRef)
    {
        if (Actor* actor = actorRef->getActor())
        {
            if (m_isVisible && _isActive) 
                actor->enable();
            else
                actor->disable();
        }
    }

    m_isActive = _isActive;
}

u32 FontTextArea::createActor(const Path& _path, bbool _isPreSpawnedActor, f32 _fontSizeBase)
{
    u32         blockIndex      = U32_INVALID;
    ActorRef    spawnedActor    = ActorRef::InvalidRef;

    if(!_isPreSpawnedActor)
    {
        for ( ITF_VECTOR<BlockActorData>::iterator actorData = m_spawnedActors.begin(); 
            actorData!= m_spawnedActors.end(); ++actorData)
        {
            if(Actor* actor = actorData->m_ref.getActor())
            {
                if((actor->getInstanceDataFile() == _path || (actor->getInstanceDataFile().isEmpty() && actor->getTemplatePath()==_path)) && m_parsedActors.find(actorData->m_ref)==-1)
                {
                    spawnedActor    = actorData->m_ref;
                    blockIndex      = uSize(actorData - m_spawnedActors.begin());
                    actorData->m_fontSizeBase = _fontSizeBase;
                    break;
                }
            }
        }
    }

    if(!spawnedActor.isValid())
    {
        m_spawnedActors.emplace_back();
        m_spawnedActors.back().m_spawner.registerInPool(ObjectRef::InvalidRef, NULL, _path, 1, 1);
        m_spawnedActors.back().m_spawner.setCheckClearOnDestroy(bfalse);
        m_spawnedActors.back().m_fontSizeBase = _fontSizeBase;

        ActorSpawner::Spawn& spawn = m_spawnedActors.back().m_spawner.spawnActor(m_position);

        if(spawn.m_actor)
        {
            spawnedActor = spawn.m_actor->getRef();
            blockIndex   = m_spawnedActors.size() - 1;

            m_spawnedActors.back().m_ref = spawnedActor;
            m_spawnedActors.back().m_ready = bfalse;
            m_spawnedActors.back().m_isPreSpawnedActor = _isPreSpawnedActor;
        }
    }
    
    if(!_isPreSpawnedActor)
    {
        if(spawnedActor.isValid())
        {
            m_parsedActors.push_back(spawnedActor);
        }
    }

    return blockIndex;
}

void FontTextArea::addPreSpawnedActor(const Path& _path)
{
    createActor(_path, btrue);
}

void FontTextArea::removePreSpawnedActor(const Path& _path)
{
    for ( ITF_VECTOR<BlockActorData>::iterator actorData = m_spawnedActors.begin(); actorData!= m_spawnedActors.end(); ++actorData)
    {
        if(actorData->m_isPreSpawnedActor && actorData->m_spawner.getPath()==_path)
        {
            actorData->m_isPreSpawnedActor = bfalse;
            clearSpawnedActors();
            break;
        }
    }
}

void FontTextArea::clearActors()
{
    for ( ITF_VECTOR<BlockActorData>::iterator actorData = m_spawnedActors.begin(); actorData!= m_spawnedActors.end(); ++actorData)
    {
        if(Actor* actor = actorData->m_ref.getActor())
        {
            actor->requestDestruction();
        }
        actorData->m_spawner.clear();
    }

    m_spawnedActors.clear();
    m_parsedActors.clear();  
}

void FontTextArea::clearSpawnedActors()
{
    for ( ITF_VECTOR<BlockActorData>::iterator actorData = m_spawnedActors.begin(); actorData!= m_spawnedActors.end();)
    {
        if( m_parsedActors.find(actorData->m_ref)==-1)
        {
            if(actorData->m_isPreSpawnedActor)
            {
                if(Actor* actor = actorData->m_ref.getActor())
                {
                    actor->disable();
                }
                ++actorData;
            }
            else
            {
                if(Actor* actor = actorData->m_ref.getActor())
                {
                    actor->requestDestruction();
                }
                actorData->m_spawner.clear();

                i32 idxToDelete = uSize(actorData - m_spawnedActors.begin());
                for (u32 i=0; i<m_textBlocks.size(); i++)
                {
                    FontTextBlock& block = m_textBlocks[i];
                    if (block.m_actorIndex != U32_INVALID && block.m_actorIndex > u32(idxToDelete))
                        block.m_actorIndex--;
                }
                actorData = m_spawnedActors.erase(actorData);
            }
        }
        else
        {
            ++actorData;
        }
    }
}

void FontTextArea::clearParsedActors()
{
    m_parsedActors.clear();
}


void FontTextArea::setViewportVisibility(u32 _value) 
{ 
	m_viewportVisibility = _value; 
	EventViewportVisibility evtViewportVisibility(_value);
	for ( ITF_VECTOR<BlockActorData>::iterator actorData = m_spawnedActors.begin(); 
		actorData!= m_spawnedActors.end(); ++actorData)
	{
		if(Actor* actor = actorData->m_ref.getActor())
		{
			if(!actor->isAsyncLoading() && actorData->m_ready)
			{
				actor->onEvent(&evtViewportVisibility);
			}
		}
	}
}

bbool FontTextArea::areActorsReady()
{
    bbool actorReady = btrue;
    for ( ITF_VECTOR<BlockActorData>::iterator actorData = m_spawnedActors.begin(); 
        actorData!= m_spawnedActors.end(); ++actorData)
    {
        if(Actor* actor = actorData->m_ref.getActor())
        {
            if(!actor->isAsyncLoading())
            {
                ITF_WARNING(NULL, m_is2D==actor->getIs2D(), "You try to add an actor in text area without the same 2d flag.");
                if(!actorData->m_ready && (!actor->isActive() || !actor->getAABB().isValid()))
				{
                    actor->setPos(m_position);
                    actor->setWorldInitialPos(m_position, bfalse, m_owner->getScene());

                    actor->enable();
                    EventShow hide(0.f, 0.0);
					actor->onEvent(&hide);
                    actorReady = bfalse;

                    AIUtils::bindChild(m_owner,actor->getRef());
                }
            }
            else
            {
                actorReady = bfalse;
            }
        }
    }

    if(actorReady)
    {
        for ( ITF_VECTOR<BlockActorData>::iterator actorData = m_spawnedActors.begin(); 
            actorData!= m_spawnedActors.end(); ++actorData)
        {
            
            if(Actor* actor = actorData->m_ref.getActor())
            {
                if(!actorData->m_ready)
                {
                    actorData->m_scale = actor->getWorldInitialScale();

                    actorData->m_aabb  = actor->getAABB();
                    actorData->m_aabb.Translate(-actor->get2DPos());
                    actorData->m_ready = btrue;

                    actorData->m_scale *= m_globalActorScaleFactor;
                    actorData->m_aabb.Scale(Vec2d::One*m_globalActorScaleFactor);

                    if (actor->getIs2D())
                    {

                        actorData->m_scale *= actor->currentResolutionToReferenceFactor();
                        actorData->m_aabb.Scale(actor->currentResolution2dPosToReference(Vec2d::One));
					}
					EventViewportVisibility evtViewportVisibility(m_viewportVisibility);
					actor->onEvent(&evtViewportVisibility);
                }

                EventShow show(1.f, 0.0);
                actor->onEvent(&show);
                if(!m_isVisible || m_parsedActors.find(actorData->m_ref)==-1)
                {
                    actor->disable();
                }
            }
        }
    }

    return actorReady;
}

f32 FontTextArea::getTextHeight(const i32 _lineStart, const i32 _lineCount) const
{
    f32 textHeight = 0.0f;

    u32 lineEnd = _lineStart + _lineCount;
    if(_lineCount<0 || lineEnd>m_linesBounds.size())
    {
        lineEnd = m_linesBounds.size();
    }

    for ( u32 n=_lineStart; n<lineEnd; n++)
    {
        textHeight += m_linesBounds[n].m_bounds.y();
    }

    return textHeight;
}

f32 FontTextArea::getTextWidth(const i32 _lineStart, const i32 _lineCount) const
{
    f32 textWidth = 0.0f;

    u32 lineEnd = _lineStart + _lineCount;
    if(_lineCount<0 || lineEnd>m_linesBounds.size())
    {
        lineEnd = m_linesBounds.size();
    }

    for ( u32 n=_lineStart; n<lineEnd; n++)
    {
        f32 width = m_linesBounds[n].m_bounds.x();

        if(width > textWidth)
        {
            textWidth = width;
        }
    }

    return textWidth;
}

Vec2d FontTextArea::getTextAreaBound() const
{
    Vec2d bound = m_area;

    if(bound.x()<0) 
    {
        bound.x() = getTextWidth();
    }

    if(m_maxWidth > 0.0f && bound.x() > m_maxWidth)
        bound.x() = m_maxWidth;

    if(bound.y()<0) 
    {
        bound.y() = getTextHeight();
    }

    return bound;
}

Vec2d FontTextArea::getLocalTopLeftOffset(const Vec2d& _bound, const Vec2d& _offset, bbool _inverseY) const
{
    Vec2d topLeftPosition = Vec2d::Zero;

    switch(m_anchor)
    {
    case AREA_ANCHOR_TOP_LEFT :
        topLeftPosition = Vec2d::Zero;
        break;
    case AREA_ANCHOR_MIDDLE_CENTER :
        topLeftPosition = -_bound*0.5f;
        break;
    case AREA_ANCHOR_MIDDLE_LEFT :
        topLeftPosition.y() = -_bound.y()*0.5f;
        break;
    case AREA_ANCHOR_MIDDLE_RIGHT :
        topLeftPosition.y() = -_bound.y()*0.5f;
        topLeftPosition.x() = -_bound.x();
        break;
    case AREA_ANCHOR_TOP_CENTER :
        topLeftPosition.x() = -_bound.x()*0.5f;
        break;
    case AREA_ANCHOR_TOP_RIGHT :
        topLeftPosition.x() = -_bound.x();
        break;
    case AREA_ANCHOR_BOTTOM_CENTER :
        topLeftPosition.x() = -_bound.x()*0.5f;
        topLeftPosition.y() = -_bound.y();
        break;
    case AREA_ANCHOR_BOTTOM_LEFT :
        topLeftPosition.y() = -_bound.y();
        break;
    case AREA_ANCHOR_BOTTOM_RIGHT :
        topLeftPosition = -_bound;
        break;
    }

    topLeftPosition += _offset;
    if(_inverseY) 
        topLeftPosition.y() *= -1;

    return topLeftPosition;
}

AABB FontTextArea::getAABB(bbool _useRotation) const
{
    Vec2d textAreaBound = getTextAreaBound();

    Matrix44 pivotMulMatrix;
    computePivotMulMatrixOffset(pivotMulMatrix, _useRotation);

    Vec3d cornerA, cornerB, cornerC;
    if(m_is2D)
    {
        computeGlobalPosition(cornerA, Vec3d(textAreaBound.x(), 0.0, 0.0), pivotMulMatrix);
        computeGlobalPosition(cornerB, Vec3d(0.0, textAreaBound.y(), 0.0), pivotMulMatrix);
        computeGlobalPosition(cornerC, Vec3d(textAreaBound.x(), textAreaBound.y(), 0.0), pivotMulMatrix);
    }
    else
    {
        computeGlobalPosition(cornerA, Vec3d(textAreaBound.x(), 0.0, 0.0), pivotMulMatrix);
        computeGlobalPosition(cornerB, Vec3d(0.0, textAreaBound.y() * -1.0f, 0.0), pivotMulMatrix);
        computeGlobalPosition(cornerC, Vec3d(textAreaBound.x(), textAreaBound.y() * -1.0f, 0.0), pivotMulMatrix);
    }

    Vec3d translation;
    storeXYZ((float *)&translation,pivotMulMatrix.T());
    AABB globalAABB(translation);
    globalAABB.grow(cornerA);
    globalAABB.grow(cornerB);
    globalAABB.grow(cornerC);

    return globalAABB;
}

AABB FontTextArea::getLocalAABB() const
{
    Vec2d textAreaBound = getTextAreaBound();

    Matrix44 pivotMulMatrix;
    pivotMulMatrix.setIdentity();

    Vec3d cornerA, cornerB, cornerC;
    if(m_is2D)
    {
        computeGlobalPosition(cornerA, Vec3d(textAreaBound.x(), 0.0, 0.0), pivotMulMatrix);
        computeGlobalPosition(cornerB, Vec3d(0.0, textAreaBound.y(), 0.0), pivotMulMatrix);
        computeGlobalPosition(cornerC, Vec3d(textAreaBound.x(), textAreaBound.y(), 0.0), pivotMulMatrix);
    }
    else
    {
        computeGlobalPosition(cornerA, Vec3d(textAreaBound.x(), 0.0, 0.0), pivotMulMatrix);
        computeGlobalPosition(cornerB, Vec3d(0.0, textAreaBound.y() * -1.0f, 0.0), pivotMulMatrix);
        computeGlobalPosition(cornerC, Vec3d(textAreaBound.x(), textAreaBound.y() * -1.0f, 0.0), pivotMulMatrix);
    }

    AABB globalAABB(Vec2d::Zero);
    globalAABB.grow(cornerA);
    globalAABB.grow(cornerB);
    globalAABB.grow(cornerC);

    return globalAABB;
}

AABB FontTextArea::getTextAreaAABBWithoutRotation()
{
    AABB globalAABB = getAABB(bfalse);
    AABB globalAABBUseRotation = getAABB(btrue);

    Vec2d halfSize = globalAABB.getSize() * 0.5f;
    Vec2d centerPos = globalAABBUseRotation.getCenter();
    globalAABB.setMin(centerPos - halfSize);
    globalAABB.setMax(centerPos + halfSize);

    return globalAABB;
}

//=================================================================================================
//
// Effects functions
//
//=================================================================================================


//-------------------------------------------------------------------------------------------------
//  add a dynamic font effect
//
void FontTextArea::addFontEffect( const FontEffect_Template *_fontEffect, u32 _markerStart, u32 _markerEnd )
{
    if (!_fontEffect)
        return;

    FontEffect fontEffect;
    fontEffect.setTemplate( _fontEffect );
    fontEffect.activate(btrue);
    fontEffect.setUser( (_markerStart & 0xFFFF) << 16 | (_markerEnd & 0xFFFF)) ;
    m_dynamicFontEffectsToAdd.push_back( fontEffect );
}

//-------------------------------------------------------------------------------------------------
//  remove font effect
//
void FontTextArea::removeFontEffect( FontEffect *_fontEffect, u32 _fontEffectIndex )
{
    for ( ITF_VECTOR<FontTextBlock>::iterator block = m_textBlocks.begin(); block!= m_textBlocks.end();++block)
    {
        if ((u32)block->m_effectIndex == _fontEffectIndex)
            block->m_effectIndex = -1;
    }

    //for ( ITF_VECTOR<FontText>::iterator fontText = m_fontTexts.begin(); fontText!= m_fontTexts.end();++fontText)
    //{
        //fontText->removeEffect( _fontEffect );
    //}
}

//-------------------------------------------------------------------------------------------------
// apply dynamic effect on blocks
//
void FontTextArea::computeFontEffectChain()
{
    if (m_fontTexts.size() == 0) return;

    m_fontEffectChain.clear();
    m_fontEffectChain.reserve( m_fontTexts.size() *(1+m_dynamicFontEffects.size()) );
    
    if (m_dynamicFontEffects.size() == 0)
    {
        i32 lastBlockIndex = -1;
        for ( ITF_VECTOR<FontTextBlock>::iterator block = m_textBlocks.begin(); block!= m_textBlocks.end();++block)
        {
            if (block->m_type != TEXT_BLOCK_WORD) continue;
            if (block->m_index == -1) continue;
            if (block->m_index == lastBlockIndex) continue;
            lastBlockIndex = block->m_index;
            if (block->m_effectIndex == -1) continue;

            FontEffectChain fontEffectChain;
            fontEffectChain.m_fontEffect = &m_fontEffects[block->m_effectIndex];
            fontEffectChain.m_next = NULL;
            m_fontEffectChain.push_back( fontEffectChain );

            FontText& fontText = m_fontTexts[block->m_index];
            fontText.setEffectChain(&m_fontEffectChain[m_fontEffectChain.size() - 1]);
        }
    }
    else
    {
        i32 lastBlockIndex = -1;
        u32 markerFlags = 0;

        for ( ITF_VECTOR<FontTextBlock>::iterator block = m_textBlocks.begin(); block!= m_textBlocks.end();++block)
        {
            if (block->m_type != TEXT_BLOCK_WORD) continue;
            if (block->m_index == -1) continue;
            
            if (block->m_marker != -1) markerFlags |= 1 << block->m_marker;

            if (block->m_index == lastBlockIndex) continue;
            lastBlockIndex = block->m_index;

            FontEffectChain *firstChain = NULL;
            FontEffectChain *lastChain = NULL;
            
            if (block->m_effectIndex != -1) 
            {
                FontEffectChain fontEffectChain;
                fontEffectChain.m_fontEffect = &m_fontEffects[block->m_effectIndex];
                fontEffectChain.m_next = NULL;
                m_fontEffectChain.push_back( fontEffectChain );
                firstChain = lastChain = &m_fontEffectChain[m_fontEffectChain.size() - 1];
            }

            for (u32 i = 0; i < m_dynamicFontEffects.size(); i++)
            {
                FontEffect *dynEffect = &m_dynamicFontEffects[i];
                u16 markerStart = (u16) ((dynEffect->getUser() >> 16) & 0xFFFF);
                u16 markerEnd = (u16) (dynEffect->getUser() & 0xFFFF);
                if (markerStart != (u16) -1 && !((1 << markerStart) & markerFlags)) continue;
                if (markerEnd != (u16) -1 && ((1 << markerEnd) & markerFlags)) continue;

                FontEffectChain fontEffectChain;
                fontEffectChain.m_fontEffect = dynEffect;
                fontEffectChain.m_next = NULL;
                m_fontEffectChain.push_back( fontEffectChain );

                if (firstChain == NULL)
                    firstChain = lastChain = &m_fontEffectChain[m_fontEffectChain.size() - 1];
                else
                {
                    lastChain->m_next = &m_fontEffectChain[m_fontEffectChain.size() - 1];
                    lastChain = lastChain->m_next;
                }
            }

            FontText& fontText = m_fontTexts[block->m_index];
            fontText.setEffectChain(firstChain);
        }
    }
}

//-------------------------------------------------------------------------------------------------
// force dynamic effect to go to end phase
//
void FontTextArea::terminateFontEffect()
{
    m_dynamicFontEffectsToAdd.clear();
    for ( ITF_VECTOR<FontEffect>::iterator fontEffect = m_dynamicFontEffects.begin(); fontEffect!= m_dynamicFontEffects.end();++fontEffect)
        fontEffect->forceFinish();
}


//=================================================================================================
//
// Compute blocks functions
//
//=================================================================================================

//-------------------------------------------------------------------------------------------------
// compute a tag from a string that began with tag starter ('[')
// in parameters:
//      _beginTag and _endTag are the tag delimiter [m: for marker tag for example
//      _text and _charCursor : give the starting point for analyze in text
// out parameters:
//      _path : the value of tag, what is between _beginTag and _endTag
// return:
//      char index in _text after tag (just after _endTag string)
//
i32 FontTextArea::computeTag(String8 & _path, const String8 & _beginTag, const String8 & _endTag, const i32 _charCursor, const String8 & _text)
{
    i32 indexTagBegin = -1, indexTagEnd = -1;

    _text.strstr(_beginTag.cStr(), bfalse, &indexTagBegin, _charCursor);

    if(indexTagBegin==0)
    {
        _text.strstr(_endTag.cStr(), bfalse, &indexTagEnd, _charCursor);

        if(indexTagEnd!=-1)
        {
            indexTagBegin += _charCursor + _beginTag.getLen();
            indexTagEnd += _charCursor;

            _path = _text.substr(indexTagBegin, indexTagEnd - indexTagBegin);

            return indexTagEnd + _endTag.getLen();
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------
// convert a char to it's hexadecimal value
//
i32 CharToHexa( char _c )
{
    if (_c >= '0' && _c <= '9') return _c - '0';
    if (_c >= 'a' && _c <= 'f') return _c - 'a' + 10;
    if (_c >= 'A' && _c <= 'F') return _c - 'A' + 10;
    return -1;
}

//-------------------------------------------------------------------------------------------------
// convert a hexa number tag to u32 value
//
bbool TagHexaToU32( const String8 &_tag, u32 &_value )
{
    _value = 0;
    for (u32 i = 0; i < _tag.getLen(); i++)
    {
        i32 val = CharToHexa( _tag.cStr()[i]);
        if (val < 0) return bfalse;
        _value = _value * 16 + val;
    }
    return btrue;
}

//-------------------------------------------------------------------------------------------------
// convert a tag to a color
//
bbool TagToColor( const String8 &_tag, Color &_color)
{
    u32 colorU32 = 0;
    if (!TagHexaToU32(_tag, colorU32)) return bfalse;
    _color.m_a = ITFCOLOR_U8_TO_F32((colorU32 >> 24) & 0xFF);
    _color.m_r = ITFCOLOR_U8_TO_F32((colorU32 >> 16) & 0xFF);
    _color.m_g = ITFCOLOR_U8_TO_F32((colorU32 >> 8) & 0xFF);
    _color.m_b = ITFCOLOR_U8_TO_F32(colorU32 & 0xFF);
    return btrue;
}

//-------------------------------------------------------------------------------------------------
// convert a tag to a float
//
bbool TagToFloat( const String8 &_tag, f32 &_value)
{
    _value = (f32) atof(_tag.cStr());
    return btrue;
}

//-------------------------------------------------------------------------------------------------
// convert a tag to an int
//
bbool TagToInt( const String8 &_tag, i32 &_value)
{
    _value = (i32) atoi(_tag.cStr());
    return btrue;
}

//-------------------------------------------------------------------------------------------------
// Tags
//
static const char       startTagDelimiter = '[';
static const String8    endTagDelimiter = "]";
static const String8    actorTag = "[actor:";
static const String8    iconTag = "[icon:";
static const String8    colorTag = "[C:";
static const String8    sizeAbsoluteTag = "[S:";
static const String8    sizeRelativeTag = "[s:";
static const String8    effectTag = "[fx:";
static const String8    markTag = "[m:";
static const String8    rubyTag = "[ruby:";
static const String8    eolTag = "[eol";

//-------------------------------------------------------------------------------------------------
// find char index just after a mark tag in text
//
i32 FontTextArea::getMarkIndex( const String8 & _text, u32 _marker )
{
    i32 charCursor = 0;
    i32 charCount = _text.getLen();
    const char * textStr = _text.cStr();
    String8          tagWorkingPath;

    while(charCursor<charCount)
    {
        char c = textStr[charCursor];
        charCursor++;

        if(c == startTagDelimiter && textStr[charCursor] == markTag.cStr()[1])
        {
            i32 charTagCursor = computeTag(tagWorkingPath, markTag, endTagDelimiter, charCursor-1, _text);
            if(charTagCursor > 0)
            {
                int marker;
                if ((tagWorkingPath != "") && TagToInt(tagWorkingPath, marker) && (marker == (int) _marker) )
                    return charTagCursor;
            }
        }
    }
    return -1;
}

//-------------------------------------------------------------------------------------------------
// find next block in text
//
i32 FontTextArea::computeNextBlock(FontTextBlock& _block, const i32 _charCursor, const String8 & _text, const FontTextBlock *_previousBlock)
{
    _block.m_text = "";
    _block.m_isEOL = bfalse;
    _block.m_actorIndex = U32_INVALID;
    _block.m_line = -1;
    _block.m_isButton = bfalse;
    _block.m_index = -1;
    _block.m_offset = Vec2d::Zero;
    _block.m_position = Vec2d::Zero;
    _block.m_bound = Vec2d::Zero;
    _block.m_scale = Vec2d::One;
    _block.m_flags = 0;   
    _block.m_color = 0;
    _block.m_effectIndex = -1;
    _block.m_marker = -1;
    _block.m_rubyTopIndex = U32_INVALID;

    if (_previousBlock)
    {
        _block.m_color = _previousBlock->m_color;
        _block.m_size = _previousBlock->m_size;
        _block.m_effectIndex = _previousBlock->m_effectIndex;
        _block.m_rubyTopIndex = _previousBlock->m_rubyTopIndex;
        if(_block.m_rubyTopIndex != U32_INVALID)
        {
            _block.m_flags |= TEXT_BLOCK_FLAG_RubyBottom;
            if(_previousBlock->m_flags & TEXT_BLOCK_FLAG_RubyTop)
                _block.m_size /= TEXT_RUBY_SCALE; // remove auto ruby scale
        }
    }
    else
    {
        _block.m_size = m_size;
    }

    i32 startCursor = _charCursor;
    i32 charCursor = _charCursor;
    i32 charCount = m_displayText.getLen();
    const char * textStr = m_displayText.cStr();

    while(charCursor<charCount)
    {
        char c = textStr[charCursor];
        charCursor++;
        
        if(c== startTagDelimiter && !m_isPassword)
        {
            // End current block when the first tag char is not the the first text char
            if(charCursor-1 != _charCursor)
            {
                charCursor--;
                break;
            }
            else
            {
                //-------- actor Tag --------------------------------------------------------------
                if (textStr[charCursor] == actorTag.cStr()[1])
                {
                    i32 charTagCursor = computeTag(m_workingTagPath, actorTag, endTagDelimiter, charCursor-1, _text);
                    if(charTagCursor > 0)
                    {
                        charCursor = charTagCursor;

                        Path actorPath(m_workingTagPath);

                        _block.m_type = TEXT_BLOCK_ACTOR;
                        _block.m_flags |= TEXT_BLOCK_FLAG_EndWorld;
                        _block.m_actorIndex = FILEMANAGER->fileExistsOnFat( FILESERVER->getPathNameByContext(actorPath) ) ? createActor(actorPath) : U32_INVALID;
                        ITF_WARNING_CATEGORY(GPP,NULL, _block.m_actorIndex!=U32_INVALID,"actor not found :%s",actorPath.toString8().cStr());

                        return charCursor;
                    }
                }
                //-------- icon Tag --------------------------------------------------------------
                if (textStr[charCursor] == iconTag.cStr()[1])
                {
                    i32 charTagCursor = computeTag(m_workingTagPath, iconTag, endTagDelimiter, charCursor-1, _text);
                    if(charTagCursor > 0)
                    {
                        charCursor = charTagCursor;

                        _block.m_type = TEXT_BLOCK_ACTOR;
                        _block.m_flags |= TEXT_BLOCK_FLAG_EndWorld;
                        if(const UITextManager_Template::ActorIcon* actorIcon = UI_TEXTMANAGER->getActorIcon(m_workingTagPath))
                        {
                            _block.m_actorIndex = FILEMANAGER->fileExistsOnFat( FILESERVER->getPathNameByContext(actorIcon->m_iconPath) ) ? createActor(actorIcon->m_iconPath, bfalse, actorIcon->m_fontSize) : U32_INVALID;
                            ITF_WARNING_CATEGORY(GPP,NULL, _block.m_actorIndex!=U32_INVALID,"actor not found :%s",actorIcon->m_iconPath.toString8().cStr());
                        }
                        else
                        {
                            _block.m_actorIndex = U32_INVALID;
                            ITF_WARNING_CATEGORY(GPP,NULL, _block.m_actorIndex!=U32_INVALID,"actor icon not found :%s",m_workingTagPath.cStr());
                        }

                        return charCursor;
                    }
                }
                //-------- color Tag --------------------------------------------------------------
                else if (textStr[charCursor] == colorTag.cStr()[1])
                {
                    i32 charTagCursor = computeTag(m_workingTagPath, colorTag, endTagDelimiter, charCursor-1, _text);
                    if(charTagCursor > 0)
                    {
                        if (m_workingTagPath == "")
                            _block.m_color = 0;
                        else if (!TagToColor(m_workingTagPath, _block.m_color))
                            _block.m_color = _previousBlock ? _previousBlock->m_color : m_color;

                        charCursor = charTagCursor;
                        startCursor = charTagCursor;
                    }
                }
                //-------- absolute size Tag ------------------------------------------------------
                else if (textStr[charCursor] == sizeAbsoluteTag.cStr()[1])
                {
                    i32 charTagCursor = computeTag(m_workingTagPath, sizeAbsoluteTag, endTagDelimiter, charCursor-1, _text);
                    if(charTagCursor > 0)
                    {
                        if (m_workingTagPath == "")
                            _block.m_size = m_size;
                        else if (!TagToFloat(m_workingTagPath, _block.m_size))
                            _block.m_size = _previousBlock ? _previousBlock->m_size : m_size;

                        charCursor = charTagCursor;
                        startCursor = charTagCursor;
                    }
                }
                //-------- relative size Tag ------------------------------------------------------
                else if (textStr[charCursor] == sizeRelativeTag.cStr()[1])
                {
                    i32 charTagCursor = computeTag(m_workingTagPath, sizeRelativeTag, endTagDelimiter, charCursor-1, _text);
                    if(charTagCursor > 0)
                    {
                        if (m_workingTagPath == "")
                            _block.m_size = m_size;
                        else if (!TagToFloat(m_workingTagPath, _block.m_size))
                            _block.m_size = _previousBlock ? _previousBlock->m_size : m_size;
                        else
                            _block.m_size *= m_size;

                        charCursor = charTagCursor;
                        startCursor = charTagCursor;
                    }
                }
                //-------- effect Tag -------------------------------------------------------------
                else if (textStr[charCursor] == effectTag.cStr()[1])
                {
                    i32 charTagCursor = computeTag(m_workingTagPath, effectTag, endTagDelimiter, charCursor-1, _text);
                    if(charTagCursor > 0)
                    {
                        //FontEffect fontEffect;
                        const FontEffect_Template *fontEffectTemplate = UI_TEXTMANAGER->getFontEffect( m_workingTagPath );
                        if (fontEffectTemplate)
                        {
                            FontEffect fontEffect;
                            fontEffect.setTemplate( fontEffectTemplate );
                            fontEffect.activate(btrue);
                            m_fontEffects.push_back( fontEffect );
                            _block.m_effectIndex = m_fontEffects.size() - 1;
                        }
                        else
                            _block.m_effectIndex = -1;

                        charCursor = charTagCursor;
                        startCursor = charTagCursor;
                    }
                }
                //-------- marker Tag --------------------------------------------------------------
                else if (textStr[charCursor] == markTag.cStr()[1])
                {
                    i32 charTagCursor = computeTag(m_workingTagPath, markTag, endTagDelimiter, charCursor-1, _text);
                    if(charTagCursor > 0)
                    {
                        if ((m_workingTagPath == "") || !TagToInt(m_workingTagPath, _block.m_marker))
                            _block.m_marker = -1;
                        
                        charCursor = charTagCursor;
                        startCursor = charTagCursor;
                    }
                }
                //-------- ruby Tag -------------------------------------------------------------
                else if (textStr[charCursor] == rubyTag.cStr()[1])
                {
                    i32 charTagCursor = computeTag(m_workingTagPath, rubyTag, endTagDelimiter, charCursor-1, _text);
                    if(charTagCursor > 0)
                    {
                        if(m_workingTagPath == "")
                        {
                            _block.m_rubyTopIndex = U32_INVALID;
                            _block.m_flags &= ~TEXT_BLOCK_FLAG_RubyBottom;
                        }
                        else
                        {
                            // Create a specific block
                            _block.m_rubyTopIndex = m_textBlocks.size() - 1;
                            _block.m_flags |= (TEXT_BLOCK_FLAG_RubyTop | TEXT_BLOCK_FLAG_EndWorld);
                            _block.m_type = TEXT_BLOCK_WORD;
                            _block.m_text = m_workingTagPath;
                            _block.m_size *= TEXT_RUBY_SCALE; // auto ruby scale
                            return charTagCursor;
                        }

                        charCursor = charTagCursor;
                        startCursor = charTagCursor;
                    }
                }
                //-------- eol Tag -------------------------------------------------------------
                else if (textStr[charCursor] == eolTag.cStr()[1])
                {
                    i32 charTagCursor = computeTag(m_workingTagPath, eolTag, endTagDelimiter, charCursor-1, _text);
                    if(charTagCursor > 0)
                    {
                        _block.m_flags |= TEXT_BLOCK_FLAG_EndWorld;
                        _block.m_isEOL = btrue;
                        _block.m_text = "";
                        return charTagCursor;
                    }
                }
            }
        }
        else if(c== ' ')
        {
            // do not end a world on space if after space is a ponctuation character : ! ? or ;
            if (charCursor < charCount)
            {
                char cNext = textStr[charCursor];
                if (cNext == ':' || cNext == ';' || cNext == '!' || cNext == '?')
                    continue;
            }

            _block.m_flags |= TEXT_BLOCK_FLAG_EndWorld;
            break;
        }   
        else if(c=='\n')
        {
            _block.m_flags |= TEXT_BLOCK_FLAG_EndWorld;
            _block.m_isEOL = btrue;
            break;
        }
    }

    if ( charCursor == charCount)
        _block.m_flags |= TEXT_BLOCK_FLAG_EndWorld;

    _block.m_type = TEXT_BLOCK_WORD;
    _block.m_text = _text.substr(startCursor, charCursor - startCursor);

    if(m_isPassword)
    {
        u32 count = getNbUnicodeChar(_block.m_text.cStr());
        _block.m_text.clear();
        for(u32 n=0; n<count; ++n)
            _block.m_text += "*";
    }

    return charCursor;
}

//-------------------------------------------------------------------------------------------------
// compute all blocks
//
void FontTextArea::computeBlockParsing()
{
    m_fontEffects.clear();
    m_textBlocks.clear();
    m_linesBounds.clear();
    m_linesCount = 0;

    i32 charCount = m_displayText.getLen();
    i32 charCursor = 0;

    while(charCursor < charCount)
    {
        m_textBlocks.emplace_back();
        const FontTextBlock *previousBlock = m_textBlocks.size() == 1 ? NULL : &m_textBlocks[m_textBlocks.size() - 2];
        charCursor = computeNextBlock(m_textBlocks.back(), charCursor, m_displayText, previousBlock);
    }
}

//=================================================================================================
//
// misc blocks functions
//
//=================================================================================================

//-------------------------------------------------------------------------------------------------
void FontTextArea::computeBlockSize()
{
    for ( ITF_VECTOR<FontTextBlock>::iterator iter = m_textBlocks.begin(); iter!= m_textBlocks.end();++iter)
    {
        iter->m_bound = m_font->getScaledTextSize(iter->m_text, iter->m_size, m_constAdvanceChar_Size, m_constAdvanceChar_Min, m_constAdvanceChar_Max, m_charSpacing);
    }
}

//-------------------------------------------------------------------------------------------------
void FontTextArea::computeBlockLine()
{
    m_scrollStartLine = -1;

    if(m_textBlocks.size()>0)
    {
        m_linesCount = 0;
        m_linesBounds.clear();

        f32     lineMinHeight = m_font->getScaledHeight(m_size);
        f32     lineWidth = 0.f;
        f32     lineHeight = lineMinHeight;
        f32     lineSpacing = 0.f; // Add spacing only after first line (n lines = (n-1) spacing)
        bbool   gotoLine = bfalse;
        u32     currentBlockCount = 0;

        for ( ITF_VECTOR<FontTextBlock>::iterator iter = m_textBlocks.begin(); iter!= m_textBlocks.end();++iter)
        {
            if(iter->m_flags & TEXT_BLOCK_FLAG_RubyTop)
                continue;

            f32 wordWidth;
            if (iter->m_flags & TEXT_BLOCK_FLAG_EndWorld)
            {
                wordWidth = iter->m_bound.x();
            }
            else
            {
                wordWidth = 0.f;
                for ( ITF_VECTOR<FontTextBlock>::iterator iterWord = iter; iterWord != m_textBlocks.end();++iterWord)
                {
                    wordWidth += iterWord->m_bound.x();
                    if ( iterWord->m_flags & TEXT_BLOCK_FLAG_EndWorld )
                        break;
                }
            }

            if(gotoLine || (m_area.x() > 0.0 && lineWidth + wordWidth > m_area.x() && lineWidth != 0) )
            {
                m_linesBounds.push_back(LineData(lineWidth, lineHeight+lineSpacing, currentBlockCount, gotoLine));
                gotoLine = bfalse;
                m_linesCount++;
                lineWidth = 0.f;
                lineHeight = lineMinHeight;
                lineSpacing = m_lineSpacing;
                currentBlockCount = 0;
            }

            iter->m_line = m_linesCount;

            ++currentBlockCount;
            lineWidth += iter->m_bound.x();

            f32 blockHeight = iter->m_bound.y();
            if(iter->m_flags & TEXT_BLOCK_FLAG_RubyBottom)
            {
                ITF_ASSERT_CRASH(iter->m_rubyTopIndex < m_textBlocks.size(), "Invalid block index !");
                m_textBlocks[iter->m_rubyTopIndex].m_line = iter->m_line;
                blockHeight += (m_textBlocks[iter->m_rubyTopIndex].m_bound.y()+m_rubySpacing);
            }
            if(blockHeight > lineHeight)
                lineHeight = blockHeight;

            if(iter->m_isEOL) gotoLine = btrue;
        }

        m_linesBounds.push_back(LineData(lineWidth, lineHeight+lineSpacing, currentBlockCount, btrue));
        m_linesCount++;
        currentBlockCount = 0;

        f32 height = getTextHeight();
        if(m_area.y() > 0.0 && height > m_area.y())
            m_scrollStartLine = 0;
    }
}

//-------------------------------------------------------------------------------------------------
void FontTextArea::computeBlockActor()
{
    for ( ITF_VECTOR<FontTextBlock>::iterator block = m_textBlocks.begin(); block!= m_textBlocks.end();++block)
    {
        if(block->m_type == TEXT_BLOCK_ACTOR && block->m_actorIndex != U32_INVALID)
        {
            BlockActorData & blockActor = m_spawnedActors[block->m_actorIndex];
            const AABB & aabb = blockActor.m_aabb;
            if ( aabb.getMin() != aabb.getMax() )
            {
                block->m_bound.x() = aabb.getWidth();
                block->m_bound.y() = aabb.getHeight();

                block->m_offset.x() = - aabb.getMin().x();

                if(m_is2D)
                {
                    block->m_offset.y() = -aabb.getMax().y();
                }
                else
                {
                    block->m_offset.y() = -aabb.getMin().y();
                }

                if(blockActor.m_fontSizeBase>0.0f)
                {
                    f32 fontScale = m_size / blockActor.m_fontSizeBase;
                    block->m_offset *= fontScale;
                    block->m_bound *= fontScale;
                    block->m_scale = Vec2d(fontScale, fontScale);
                }
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------
void FontTextArea::computeBlockAlignment()
{
    f32 currentX = 0.0;
    f32 offsetX = 0.0f;
    f32 currentY = 0.0;
    f32 transformY = 0.0;
    f32 offsetY = 0.0f;
    f32 currentLineHeight = 0.0f;
    i32 currentLine = -1;
    Vec2d bound = getTextAreaBound();
    Vec2d position;
    f32 additionalWordSpacing = 0;
    bbool firstBlockInLine = btrue;

    switch(m_vAlignment)
    {
    case FONT_ALIGN_TOP :       offsetY = 0.0f; break;
    case FONT_ALIGN_MIDDLE :    offsetY = (bound.y() - getTextHeight()) * 0.5f; break;
    case FONT_ALIGN_BOTTOM :    offsetY = bound.y() - getTextHeight(); break;
    }

    for (ITF_VECTOR<FontTextBlock>::iterator block = m_textBlocks.begin(); block!= m_textBlocks.end(); ++block)
    {
        i32 line = block->m_line;

        if(line == -1) continue;

        if(currentLine != line)
        {
            currentX = 0.0;
            switch(m_hAlignment)
            {
            case FONT_ALIGN_LEFT :
                offsetX = 0;
                break;
            case FONT_ALIGN_JUSTIFY:
                {
                    offsetX = 0;
                    // No justification for a line at the end of a paragraph
                    const u32 currentBlockCount = getLineBlockCount(line);
                    if (currentBlockCount > 1 && !isLastLineOfParagraph(line))
                    {
                        const f32 lineWidth = getLineWidth(line);
                        additionalWordSpacing = (bound.x() - lineWidth) / (f32)(currentBlockCount - 1);
                    }
                    else
                    {
                        additionalWordSpacing = 0.0f;
                    }
                }
                break;
            case FONT_ALIGN_CENTER :
                offsetX = (bound.x() - getLineWidth(line)) * 0.5f;
                break;
            case FONT_ALIGN_RIGHT :
                offsetX = bound.x() - getLineWidth(line);
                break;
            }

            if(m_is2D)
            {
                currentLineHeight = getLineHeight(line);
                currentY += currentLineHeight;
                transformY = offsetY + currentY;
            }
            else
            {
                currentLineHeight = getLineHeight(line);
                if (m_font->getSize() < 0)
                {
                    transformY  = -(offsetY + currentY);
                    currentY += currentLineHeight;
                }
                else
                {
                    currentY += currentLineHeight;
                    transformY  = -(offsetY + currentY);
                }
            }

            currentLine = line;
            firstBlockInLine = btrue;
        }

        if(block->m_flags & TEXT_BLOCK_FLAG_RubyTop)
        {
            // Default position before adjustment by bottom text
            block->m_position.x() = currentX + offsetX;
            block->m_position.y() = transformY;
        }
        else
        {
            // X
            if (firstBlockInLine)
            {
                block->m_position.x() = currentX + offsetX;
                currentX += block->m_bound.x();
            }
            else
            {
                block->m_position.x() = currentX + additionalWordSpacing + offsetX;
                currentX += block->m_bound.x() + additionalWordSpacing;
            }
            // Y
            block->m_position.y() = transformY;

            // Adjust ruby top text if any
            if(block->m_flags & TEXT_BLOCK_FLAG_RubyBottom)
            {
                ITF_ASSERT_CRASH(block->m_rubyTopIndex < m_textBlocks.size(), "Invalid block index !");
                FontTextBlock* topBlock = &m_textBlocks[block->m_rubyTopIndex];
                // X : center top text on bottom text = last bottom text is the rightmost one,
                // so default topBlock position is the left limit, and current block position is the right limit
                topBlock->m_offset.x() = (block->m_position.x() + block->m_bound.x() - topBlock->m_position.x() - topBlock->m_bound.x()) * 0.5f;
                // Y : put top text on top of bottom text = max height of all bottom texts
                if(m_is2D)
                    topBlock->m_offset.y() = - f32_Max(- topBlock->m_offset.y(), block->m_bound.y()+m_rubySpacing);
                else
                    topBlock->m_offset.y() = f32_Max(topBlock->m_offset.y(), block->m_bound.y()+m_rubySpacing);
            }
        }

        firstBlockInLine = bfalse;
    }
}

void FontTextArea::computeScroll()
{
    m_scrollXSize = m_maxWidth > 0.0f ? (m_scrollXLoop ? (getTextWidth()) / 2 : getTextWidth() - m_maxWidth) * m_localScale.x() : 0.0f;
}

void FontTextArea::computeBlockText()
{
    m_fontTexts.clear();

    FontText fontText;
    fontText.setFontTemplate(*m_font);
    fontText.setIs2D(m_is2D);
    fontText.setSize(m_size);
    fontText.setColor(m_color);
    fontText.setShadowOffset(m_shadowOffset);
    fontText.setShadowColor(m_shadowColor);
    fontText.setCharSpacing(m_charSpacing);
    fontText.setAdvanceCharSize(m_constAdvanceChar_Size);
    fontText.setAdvanceCharMin(m_constAdvanceChar_Min);
    fontText.setAdvanceCharMax(m_constAdvanceChar_Max);
    fontText.setUseGradient(m_useGradient);
    fontText.setGradientScale(m_gradientSize);
    fontText.setGradientOffset(m_gradientOffset);
    fontText.setGradientColor(m_gradientColor);

    u32 remainingDisplayedCharCount = 999999;
    if(m_displayedCharCount>=0)
    {
        remainingDisplayedCharCount = u32(m_displayedCharCount);
    }

    i32 lastBlockIndex = -1;
    i32 currentBlockIndex = 0;

    for ( ITF_VECTOR<FontTextBlock>::iterator block = m_textBlocks.begin(); block!= m_textBlocks.end();++block)
    {
        if(block->m_type == TEXT_BLOCK_WORD)
        {
            if(remainingDisplayedCharCount>0)
            {
                ux nbUnicodeChar = getNbUnicodeChar(block->m_text.cStr());
                if(nbUnicodeChar<=remainingDisplayedCharCount)
                {
                    fontText.setText(block->m_text);
                    remainingDisplayedCharCount -= nbUnicodeChar;
                }
                else
                {
                    const char * startText = block->m_text.cStr();
                    const char * endText = startText;
                    advanceNbUnicodeChar(endText, remainingDisplayedCharCount);
                    fontText.setText(String8(startText, uSize(endText - startText)));
                    remainingDisplayedCharCount = 0;
                }

                // Merge block if it is the same setting
                if(m_hAlignment != FONT_ALIGN_JUSTIFY && lastBlockIndex!=-1 && block->hasSameSettingsAs( m_textBlocks[lastBlockIndex] ))
                {
                    block->m_index = m_textBlocks[lastBlockIndex].m_index;

                    if(block->m_index!=-1)
                    {
                        m_fontTexts[block->m_index].appendText(fontText.getText());
                    }
                }
                else
                {
                    block->m_index = m_fontTexts.size();
                    m_fontTexts.push_back(fontText);
                    lastBlockIndex = currentBlockIndex;
                }
            }
            else
            {
                block->m_index = -1;
            }
        }
        else
        {
            if (block->m_type == TEXT_BLOCK_ACTOR && block->m_actorIndex != U32_INVALID)
            {
                if (Actor* actor = m_spawnedActors[block->m_actorIndex].m_ref.getActor())
                {
                    if (remainingDisplayedCharCount > 0 && !actor->isEnabled() && m_isVisible)
                    {
                        actor->enable();
                    } else if (remainingDisplayedCharCount <= 0 && actor->isEnabled())
                    {
                        actor->disable();
                    }
                }
            }
            
            if(remainingDisplayedCharCount>0)
            {
                remainingDisplayedCharCount--;
            }

            lastBlockIndex = -1;
        }

        currentBlockIndex++;
    }
}

void FontTextArea::computeTransform()
{
    Matrix44 pivotMulMatrix;
    computePivotMulMatrixOffset(pivotMulMatrix, btrue);

    Vec2d localScale;
    if (getIs2D())
    {
        Matrix44 scaleMatrix;
        computePivotMulMatrixOffset(scaleMatrix, bfalse);
        computeGlobalScale(localScale, Vec2d::One, scaleMatrix);
    }
    else
        localScale = m_localScale;

    i32 lastBlockIndex = -1;
    for ( ITF_VECTOR<FontTextBlock>::iterator block = m_textBlocks.begin(); 
          block!= m_textBlocks.end(); ++block)
    {
        if(block->m_type == TEXT_BLOCK_WORD && block->m_index!=-1 && block->m_index!=lastBlockIndex)
        {
            FontText& fontText = m_fontTexts[block->m_index];
            fontText.setLocalAngle(m_localAngle);
            fontText.setLocalPivot(m_localPivot/* - block->m_position*/);
            fontText.setLocalScale(localScale);

            Vec3d localPos = (block->m_position + block->m_offset).to3d(0);
            Vec3d globalPos;
            computeGlobalPosition(globalPos, localPos, pivotMulMatrix);

            fontText.setPosition(globalPos);
            lastBlockIndex = block->m_index;
        }
    }
}

void FontTextArea::computeTransformActors()
{
    if (m_needAlign)
        return;

    Matrix44 pivotMulMatrix;
    computePivotMulMatrixOffset(pivotMulMatrix, btrue);

    Matrix44 scaleMatrix;
    computePivotMulMatrixOffset(scaleMatrix, bfalse);


    for ( ITF_VECTOR<FontTextBlock>::iterator block = m_textBlocks.begin(); block!= m_textBlocks.end();++block)
    {
        if(block->m_type != TEXT_BLOCK_ACTOR || block->m_actorIndex == U32_INVALID)
            continue;

        BlockActorData blockActorData = m_spawnedActors[block->m_actorIndex];

        Actor* actor = blockActorData.m_ref.getActor();
        if (!actor)
            continue;

        Vec2d globalScale;
        computeGlobalScale(globalScale, block->m_scale, scaleMatrix);
        actor->setScale(blockActorData.m_scale * globalScale);

        Vec3d localPos = (block->m_position + block->m_offset).to3d(0);
        Vec3d globalPos;
        computeGlobalPosition(globalPos, localPos, pivotMulMatrix);
        actor->setPos(globalPos);

        actor->setAngle(m_localAngle);

        if(GraphicComponent* graphicComponent = actor->GetComponent<GraphicComponent>())
        {
            graphicComponent->setAlpha(m_alpha);
            graphicComponent->getGfxPrimitiveParam().m_useStaticFog = btrue;
        }
    }
}

void FontTextArea::computeDisplayAreaLines()
{
    // if scrolling isn't required, show all the lines
    if(m_scrollStartLine == -1)
    {
        m_scrollLineCount = m_linesCount;
        return;
    }

    if(m_textBlocks.size() > 0 || m_scrollLineCount != -1)
    {
        m_scrollLineCount = 0;

        // determine how many lines are invisible at the top of the area.
        i32 topLineVisible = 0;
        f32 curHeight = 0.0f;
        for(; topLineVisible < m_linesCount; ++topLineVisible)
        {
            curHeight = getTextHeight(0, topLineVisible + 1);

            // invisible at the top
            if(curHeight < 0.0f)
            {
                topLineVisible++;
                continue;
            }

            // invisible at bottom
            if(curHeight > m_area.y())
                break;

            m_scrollLineCount++;
        }
    }
}

void FontTextArea::setDisplayedCharCount(const i32 _count)
{
    if(_count!=m_displayedCharCount)
    {
        m_displayedCharCount = _count;
        computeBlockText();
        computeTransform();
    }
}

void FontTextArea::setScrollPx( const f32 _deltaScrollPx /*= 0*/ )
{
    // if scrolling is disabled, don't handle anything
    if(m_scrollStartLine == -1 || _deltaScrollPx == 0.0f)
        return;

    // get current "cursor" line height
    f32 lineHeight = getTextHeight(m_scrollStartLine, 1);

    if(_deltaScrollPx < 0.0f)
    {
        // can we scroll up
        if(m_scrollDrawOffset - _deltaScrollPx <= 0.0f && m_scrollStartLine >= 0)
        {
            m_scrollDelta -= _deltaScrollPx;
            m_scrollDrawOffset -= _deltaScrollPx;

            if(m_scrollDelta >= 0.0f && lineHeight != 0.0f)
            {
                m_scrollDelta -= lineHeight;
                m_scrollStartLine--;
            }
        }
    }
    else
    {
        // can we scroll down
        f32 totalHeight = getTextHeight(0, m_linesCount - m_scrollLineCount);
        if(m_scrollDrawOffset <= totalHeight + lineHeight && m_scrollStartLine <= m_linesCount - m_scrollLineCount)
        {
            m_scrollDelta -= _deltaScrollPx;
            m_scrollDrawOffset -= _deltaScrollPx;

            if(m_scrollDelta < -lineHeight)
            {
                m_scrollDelta += lineHeight;	// previous line? (if any)
                m_scrollStartLine++;
            }
        }
    }
}

bbool FontTextArea::getFirstCharPos(Vec3d& _pos, Vec2d& _scale, f32& _angle, const Vec2d& _offset) const
{
    _pos = m_position;
    _scale = m_localScale;
    _angle = m_localAngle;

    if(!m_textBlocks.empty())
    {
        Vec3d localPos3D = (m_textBlocks.front().m_position + _offset).to3d(0);
        if(m_maxWidth > 0.0f && m_hAlignment == FONT_ALIGN_RIGHT)
            localPos3D.x() = f32_Max(localPos3D.x(), localPos3D.x() + (getTextWidth() - m_maxWidth));
        

        Matrix44 scaleMatrix;
        computePivotMulMatrixOffset(scaleMatrix, bfalse);
        computeGlobalScale(_scale, Vec2d::One, scaleMatrix);

        Matrix44 pivotMulMatrix;
        computePivotMulMatrixOffset(pivotMulMatrix, btrue);
        computeGlobalPosition(_pos, localPos3D, pivotMulMatrix);
    }

    return getIsUpdated();
}

bbool FontTextArea::getAABBAnchorPos(TEXT_AREA_ANCHOR _anchor, Vec3d& _pos, Vec2d& _scale, f32& _angle, const Vec2d& _offset) const
{
    _pos = m_position;
    _scale = m_localScale;
    _angle = m_localAngle;

    if(!m_textBlocks.empty())
    {
        AABB localAABB = getLocalAABB();
        Vec2d localPos = localAABB.getCenter();

        switch(_anchor)
        {
        case AREA_ANCHOR_TOP_LEFT :
            localPos = localAABB.getMin();
            break;
        case AREA_ANCHOR_MIDDLE_CENTER :
            localPos = localAABB.getCenter();
            break;
        case AREA_ANCHOR_MIDDLE_LEFT :
            localPos.x() = localAABB.getMin().x();
            break;
        case AREA_ANCHOR_MIDDLE_RIGHT :
            localPos.x() = localAABB.getMax().x();
            break;
        case AREA_ANCHOR_TOP_CENTER :
            localPos.y() = localAABB.getMin().y();
            break;
        case AREA_ANCHOR_TOP_RIGHT :
            localPos.x() = localAABB.getMax().x();
            localPos.y() = localAABB.getMin().y();
            break;
        case AREA_ANCHOR_BOTTOM_CENTER :
            localPos.y() = localAABB.getMax().y();
            break;
        case AREA_ANCHOR_BOTTOM_LEFT :
            localPos = localAABB.getMin();
            break;
        case AREA_ANCHOR_BOTTOM_RIGHT :
            localPos.x() = localAABB.getMax().x();
            localPos.y() = localAABB.getMax().y();
            break;
        }

        Vec3d localPos3D = (localPos + _offset).to3d(0);

        Matrix44 scaleMatrix;
        computePivotMulMatrixOffset(scaleMatrix, bfalse);
        computeGlobalScale(_scale, Vec2d::One, scaleMatrix);

        Matrix44 pivotMulMatrix;
        computePivotMulMatrixOffset(pivotMulMatrix, btrue);
        computeGlobalPosition(_pos, localPos3D, pivotMulMatrix);
    }

    return getIsUpdated();
}

BEGIN_SERIALIZATION_SUBCLASS(FontTextArea, Style)
    BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
        SERIALIZE_MEMBER("fontPath",m_fontPath);
    END_CONDITION_BLOCK();
    SERIALIZE_OBJECT("fontSet", m_fontSet); 
    SERIALIZE_MEMBER("fontSize",m_fontSize); 
    SERIALIZE_MEMBER("color",m_color); 
    SERIALIZE_MEMBER("shadowOffset",m_shadowOffset);  
    SERIALIZE_MEMBER("shadowColor",m_shadowColor);
    SERIALIZE_MEMBER("lineSpacing",m_lineSpacing);
    SERIALIZE_MEMBER("paragraphSpacing",m_paragraphSpacing);
    SERIALIZE_ENUM_BEGIN("anchor",m_anchor);
        SERIALIZE_ENUM_VAR(AREA_ANCHOR_TOP_LEFT);
        SERIALIZE_ENUM_VAR(AREA_ANCHOR_MIDDLE_CENTER);
        SERIALIZE_ENUM_VAR(AREA_ANCHOR_MIDDLE_LEFT);
        SERIALIZE_ENUM_VAR(AREA_ANCHOR_MIDDLE_RIGHT);
        SERIALIZE_ENUM_VAR(AREA_ANCHOR_TOP_CENTER);
        SERIALIZE_ENUM_VAR(AREA_ANCHOR_TOP_RIGHT);
        SERIALIZE_ENUM_VAR(AREA_ANCHOR_BOTTOM_CENTER);
        SERIALIZE_ENUM_VAR(AREA_ANCHOR_BOTTOM_LEFT);
        SERIALIZE_ENUM_VAR(AREA_ANCHOR_BOTTOM_RIGHT);
    SERIALIZE_ENUM_END(); 
    SERIALIZE_ENUM_H_ALIGNMENT("hAlignment",m_hAlignment);
    SERIALIZE_ENUM_V_ALIGNMENT("vAlignment",m_vAlignment);
    SERIALIZE_MEMBER("useGradient",m_useGradient);
    SERIALIZE_MEMBER("gradientSize",m_gradientSize);
    SERIALIZE_MEMBER("gradientOffset",m_gradientOffset);
    SERIALIZE_MEMBER("gradientColor",m_gradientColor);
    SERIALIZE_MEMBER("charSpacing",m_charSpacing);
END_SERIALIZATION()

FontTextArea::Style::Style()
: m_fontPath(Path::EmptyPath)
, m_fontSize(-1)
, m_color(Color::white())
, m_hAlignment(FONT_ALIGN_LEFT)
, m_vAlignment(FONT_ALIGN_TOP)
, m_shadowOffset(Vec2d(3.4f,3.4f))
, m_shadowColor(COLOR_BLACK)
, m_anchor(AREA_ANCHOR_TOP_LEFT)
, m_fontTemplate(NULL)
, m_lineSpacing(0.0f)
, m_paragraphSpacing(0.0f)
, m_charSpacing(0.0f)
, m_useGradient(bfalse)
, m_gradientSize(1.0f)
, m_gradientOffset(0.5f)
, m_gradientColor(Color::black())
{
}

void FontTextArea::Style::onLoaded(class ResourceContainer * _parentContainer)
{
    ITF_LANGUAGE curlang = LOCALISATIONMANAGER->getCurrentLanguage();

    { //*************** TWEAK MODE ON  ***************//
        // Auto-adjust line spacing in Japanese
        if ((curlang == ITF_LANGUAGE_JAPANESE) && !m_fontSet.m_japanese.isEmpty())
            m_lineSpacing += (0.1f * m_fontSize);
    } //*************** TWEAK MODE OFF ***************//

    const Path& langFontPath = m_fontSet.getFontPath(curlang);

    if ( !langFontPath.isEmpty() )
    {
        m_fontTemplate = TEMPLATEDATABASE->requestTemplate<FontTemplate>(langFontPath);
    }
    else if ( !m_fontPath.isEmpty() )
    {
        m_fontTemplate = TEMPLATEDATABASE->requestTemplate<FontTemplate>(m_fontPath);
    }
    
    if(_parentContainer && m_fontTemplate)
    {
        //_parentContainer->addChild(m_fontTemplate->getResourceContainer());
    }
}

void FontTextArea::Style::onUnLoaded(class ResourceContainer * _parentContainer)
{
    if(m_fontTemplate)
    {
        //if(_parentContainer)
        //{
        //    _parentContainer->removeChild(m_fontTemplate->getResourceContainer());
        //}

        TEMPLATEDATABASE->releaseTemplate(m_fontTemplate->getFile());
        m_fontTemplate = NULL;
    }
}

// font set
BEGIN_SERIALIZATION_SUBCLASS(FontTextArea, FontSet)
    SERIALIZE_MEMBER("default", m_default)
    SERIALIZE_MEMBER("japanese", m_japanese)
    SERIALIZE_MEMBER("korean", m_korean)
    SERIALIZE_MEMBER("tradChinese", m_tradChinese)
    SERIALIZE_MEMBER("simpleChinese", m_simpleChinese)
    SERIALIZE_MEMBER("russian", m_russian)
END_SERIALIZATION()

const Path & FontTextArea::FontSet::getFontPath(ITF_LANGUAGE _language) const
{
    if (_language == ITF_LANGUAGE_JAPANESE && !m_japanese.isEmpty())
        return m_japanese;
    if (_language == ITF_LANGUAGE_KOREAN && !m_korean.isEmpty())
        return m_korean;
    if (_language == ITF_LANGUAGE_TRADITIONALCHINESE && !m_tradChinese.isEmpty())
        return m_tradChinese;
    if (_language == ITF_LANGUAGE_SIMPLIFIEDCHINESE && !m_simpleChinese.isEmpty())
        return m_simpleChinese;
    if (_language == ITF_LANGUAGE_RUSSIAN && !m_russian.isEmpty())
        return m_russian;
    return m_default;
}



}
