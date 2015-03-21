#include "precompiled_gameplay.h"

#ifndef _ITF_TEXTBOXCOMPONENT_H_
#include "gameplay/components/UI/TextBoxComponent.h"
#endif //_ITF_TEXTBOXCOMPONENT_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_FONTTEMPLATE_H_
#include "engine/display/FontTemplate.h"
#endif //_ITF_FONTTEMPLATE_H_

#define JAPANESE_SCALE 0.8f

namespace ITF
{
///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(TextBoxComponent)
BEGIN_SERIALIZATION_CHILD(TextBoxComponent)
    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
    SERIALIZE_MEMBER("style", m_defaultStyle);
    SERIALIZE_MEMBER("offset", m_offset);
    SERIALIZE_MEMBER("scale", m_scale);
    SERIALIZE_MEMBER("area", m_area); 
    SERIALIZE_MEMBER("maxWidth", m_maxWidth);
    SERIALIZE_MEMBER("rawText", m_text.m_defaultText); 
    SERIALIZE_MEMBER("locId", m_text.m_locId);
    SERIALIZE_MEMBER("scaleToMatchWithArea", m_scaleToMatchWithArea);
    SERIALIZE_MEMBER("autoScrollSpeed", m_autoScrollSpeed);
    SERIALIZE_MEMBER("autoScrollWaitTime", m_autoScrollWaitTime);
    SERIALIZE_MEMBER("overridingColor",m_color);
    SERIALIZE_ENUM_H_ALIGNMENT("overridingHAlignment",m_hAlignment);
    SERIALIZE_ENUM_V_ALIGNMENT("overridingVAlignment",m_vAlignment);
    SERIALIZE_MEMBER("depthOffset", m_depthOffset);
    END_CONDITION_BLOCK()
END_SERIALIZATION()

#ifdef ITF_SUPPORT_EDITOR
void TextBoxComponent::onPostPropertyChange()
{
    setStyle(m_defaultStyle);

    if(m_scaleToMatchWithArea)
    {
        m_textArea.setArea(Vec2d(-1.0f, -1.0f));
    }
    else
    {
        m_textArea.setArea(m_area);
    }
    

    if(m_color.getAsU32()!=0)
    {
        m_textArea.setColor(m_color);
    }

    if(m_hAlignment!=FONT_ALIGN_NONE)
    {
        m_textArea.setHAlignment(m_hAlignment);
    }

    if(m_vAlignment!=FONT_ALIGN_NONE)
    {
        m_textArea.setVAlignment(m_vAlignment);
    }

    m_textArea.setMaxWidth(m_maxWidth);
    m_textArea.setAutoScrollXSpeed(m_autoScrollSpeed);
    m_textArea.setAutoScrollXWaitTime(m_autoScrollWaitTime);

    m_textArea.setText(m_text.getText());
}
#endif //ITF_SUPPORT_EDITOR

///////////////////////////////////////////////////////////////////////////////////////////
TextBoxComponent::TextBoxComponent()
: m_offset(Vec2d::Zero)
, m_area(Vec2d(-1.0f, -1.0f))
, m_scale(Vec2d::One)
, m_defaultStyle(0)
, m_currentStyle(U32_INVALID)
, m_text("Text", LocalisationId::Invalid)
, m_color(0)
, m_hAlignment(FONT_ALIGN_NONE)
, m_vAlignment(FONT_ALIGN_NONE)
, m_mainScale(Vec2d::One)
, m_mainAngle(0.0f)
, m_mainOffset(Vec2d::Zero)
, m_currentAlpha(1.f)
, m_dstAlpha(1.f)
, m_srcAlpha(1.f)
, m_fadeTime(0.f)
, m_fadeTotal(0.f)
, m_scaleToMatchWithArea(bfalse)
, m_maxWidth(-1.0f)
, m_autoScrollSpeed(0.0f)
, m_autoScrollWaitTime(0.0f)
, m_depthOffset(0.f)
{
    m_textArea.setIsActive(bfalse);
}

///////////////////////////////////////////////////////////////////////////////////////////
TextBoxComponent::~TextBoxComponent()
{

}

void TextBoxComponent::onStartDestroy( bbool _hotReload )
{
    Super::onStartDestroy(_hotReload);

    m_textArea.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////
void TextBoxComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    Vec2d offset = m_offset;
    m_actor->transformLocalVectorToWorld(offset);

    m_textArea.setIs2D(bfalse);
    m_textArea.setPosition(m_actor->get2DPos() + offset);
    m_textArea.setOwner(m_actor);

    if(m_scaleToMatchWithArea)
    {
        m_textArea.setArea(Vec2d(-1.0f, -1.0f));
    }
    else
    {
        m_textArea.setArea(m_area);
    }

    m_textArea.setMaxWidth(m_maxWidth);
    m_textArea.setAutoScrollXSpeed(m_autoScrollSpeed);
    m_textArea.setAutoScrollXWaitTime(m_autoScrollWaitTime);

    m_textArea.setText(m_text.getText());

	ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventViewportVisibility_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventShow_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSetText_CRC,this);
 }

void TextBoxComponent::onBecomeActive()
{
    Super::onBecomeActive();

    m_textArea.setIsActive(btrue);
    m_textArea.setOwner(m_actor);
}

void TextBoxComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    m_textArea.setIsActive(bfalse);
}

void TextBoxComponent::onFinalizeLoad()
{
    Super::onFinalizeLoad();

    setStyle(m_defaultStyle);
}

void TextBoxComponent::setShadow(const Vec2d& _shadowOffset, const Color _shadowColor) 
{
    m_textArea.setShadow(_shadowOffset, _shadowColor);
}

void TextBoxComponent::setBox(const Vec2d& _area, const TEXT_AREA_ANCHOR _anchor)
{
    if(_area != Vec2d::Zero)
    {
        m_textArea.setArea(_area);
    }

    if(_anchor!=AREA_ANCHOR_NONE)
    {
        m_textArea.setAnchor(_anchor);
    }
}

void TextBoxComponent::setSize(const f32 _fontSize)
{
    if(_fontSize!=0.0)
    {
        m_textArea.setSize(_fontSize);
    }
}

void TextBoxComponent::setColor(const Color _fontColor)
{
    m_textArea.setColor(_fontColor);
}


void TextBoxComponent::setFont(const Color _fontColor, const f32 _fontSize, const FONT_ALIGNMENT _hAlignment, const FONT_ALIGNMENT _vAlignment)
{
    if(m_color.getAsU32()==0)
    {
        m_textArea.setColor(_fontColor);
    }
    
    if(_fontSize!=0.0)
    {
        m_textArea.setSize(_fontSize);
    }
    
    if(_hAlignment!=FONT_ALIGN_NONE && m_hAlignment!=FONT_ALIGN_NONE)
    {
        m_textArea.setHAlignment(_hAlignment);
    }

    if(_vAlignment!=FONT_ALIGN_NONE && m_vAlignment!=FONT_ALIGN_NONE)
    {
        m_textArea.setVAlignment(_vAlignment);
    }
}

void TextBoxComponent::setStyle(u32 _styleIndex)
{
    if(_styleIndex == U32_INVALID)
    {
        _styleIndex = m_defaultStyle;
    }

    if(_styleIndex < getTemplate()->m_styles.size())
    {
        const FontTextArea::Style& style = getTemplate()->m_styles[_styleIndex];

        m_textArea.setStyle(style);

        if(m_color.getAsU32()!=0)
        {
            m_textArea.setColor(m_color);  
        }

        if(m_hAlignment!=FONT_ALIGN_NONE)
        {
            m_textArea.setHAlignment(m_hAlignment);
        }

        if(m_vAlignment!=FONT_ALIGN_NONE)
        {
            m_textArea.setVAlignment(m_vAlignment);
        }
    }

    m_currentStyle = _styleIndex;
}

void TextBoxComponent::setConstAdvanceChar(const char _constAdvanceChar_Size, const char _constAdvanceChar_Min, const char _constAdvanceChar_Max)
{
    m_textArea.setConstAdvanceChar(_constAdvanceChar_Size, _constAdvanceChar_Min, _constAdvanceChar_Max);
}

Color TextBoxComponent::getInitColor() const
{
    if(m_color.getAsU32()!=0)
    {
        return m_color;  
    }

    if(m_currentStyle < getTemplate()->m_styles.size())
    {
        return getTemplate()->m_styles[m_currentStyle].m_color;
    }

    return 0;
}

///////////////////////////////////////////////////////////////
void TextBoxComponent::Update( f32 _deltaTime )
{
    PRF_M_SCOPE(updateTextBoxComponent)

    Super::Update(_deltaTime);
    updateText(_deltaTime);
    updateAlpha(_deltaTime);
}

void TextBoxComponent::updateAlpha( f32 _dt )
{
    if ( m_fadeTime <= 0.f|| m_fadeTotal <= 0.f) 
        return;

    m_fadeTime -= _dt;

    if ( m_fadeTime <= 0.f)
    {
        m_currentAlpha = m_dstAlpha;
    }
    else
    {
        f32 t = 1.f - ( m_fadeTime / m_fadeTotal );
        m_currentAlpha = Interpolate(m_srcAlpha,m_dstAlpha,t);
    }

    m_textArea.setAlpha(m_currentAlpha);
}

void TextBoxComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( EventShow* eventShow = DYNAMIC_CAST(_event,EventShow) )
    {
        m_srcAlpha = m_currentAlpha;
        m_dstAlpha = eventShow->getAlpha();
        m_fadeTotal = eventShow->getTransitionTime();
        
        if ( m_fadeTotal <= 0.f)
        {
            setAlpha(m_dstAlpha);
        }
        else
        {
            m_fadeTime = m_fadeTotal;
        }
	}
	else if( EventViewportVisibility* eventViewportVisibility = DYNAMIC_CAST(_event,EventViewportVisibility) )
	{
		m_textArea.setViewportVisibility(eventViewportVisibility->getViewportVisibility());
	}
    else if( EventSetText* eventSetText = DYNAMIC_CAST(_event,EventSetText) )
    {
        m_textArea.setText(eventSetText->getText());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

void TextBoxComponent::onResourceLoaded()
{
    updateText(0);
}


//////////////////////////////////////////////////////////////////////////////////////////

void    TextBoxComponent::onScaleChanged( const Vec2d& _oldScale, const Vec2d& _newScale )
{
    Super::onScaleChanged(_oldScale,_newScale);

    Vec2d localScale = m_actor->getScale() * m_scale * m_mainScale;
    if(m_scaleToMatchWithArea)
    {
        Vec2d aabbSize = m_textArea.getLocalAABB().getSize();

        f32 scale =1.0f;
        if(aabbSize.x()>m_area.x() && m_area.x()>=0.0f)
        {
            scale = Min(scale, m_area.x() / aabbSize.x());
        }

        if(aabbSize.y()>m_area.y() && m_area.y()>=0.0f)
        {
            scale = Min(scale, m_area.y() / aabbSize.y());
        }

        localScale *= scale;
    }

    m_textArea.setLocalScale(localScale);
    m_textArea.Update(0);

    updateAABB();
}

///////////////////////////////////////////////////////////////////////////////////////////


#ifdef ITF_SUPPORT_EDITOR
    ///////////////////////////////////////////////////////////////////////////////////////////
    void TextBoxComponent::drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags ) const 
    {
        Super::drawEdit(drawInterface, _flags);

        drawBox(3.0f);
    }

    ////////////////////////////////////////////////////////////////////////////
    void TextBoxComponent::drawBox(const f32 _border, const Color _color) const
    {
        AABB aabb = m_textArea.getAABB();
        Vec2d boxMinPosition = aabb.getMin();
        Vec2d boxMaxPosition = aabb.getMax();
        if( m_textArea.getIs2D() )
        {
            //drawBox2D(boxMinPosition, boxMaxPosition, _border, _color);
        }
        else
        {
            //drawBox3D(boxMinPosition, boxMaxPosition, _border, _color);
        }
    }

    void TextBoxComponent::onEditorMove( bbool _modifyInitialPos /*= btrue*/ )
    {
        updateText(0);
        setStyle(m_defaultStyle);
    }

#endif //ITF_SUPPORT_EDITOR

////////////////////////////////////////////////////////////////////////////
void TextBoxComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
{
    m_textArea.Draw(_views, getDepthOffset(),  GetActor()->getRef());
}

///////////////////////////////////////////////////////////////////////////////////////////

const String8 & TextBoxComponent::getText() const
{
    return m_textArea.getText();
}

void TextBoxComponent::setText(const String8& _text)
{
    m_textArea.setText(_text);
}

void TextBoxComponent::setDefaultText( const String8& _text)
{
    m_text.setDefaultText(_text);
}

void TextBoxComponent::setLoc(const LocalisationId& _locId)
{
    bbool found;
    m_textArea.setText(LOCALISATIONMANAGER->getText(_locId, &found, m_actor));
}

void TextBoxComponent::insertTextClear()
{
    m_textArea.setText(m_text.getText());
}

void TextBoxComponent::insertTextAtMarker(u32 _marker, const String8 &_textToInsert)
{
    m_textArea.insertTextAtMarker(_marker, _textToInsert);
}

void TextBoxComponent::setActorScaleFactor(f32 _atorScaleFactor)
{
    m_textArea.setGlobalActorScaleFactor(_atorScaleFactor);
}

void TextBoxComponent::updateAABB()
{
    GetActor()->growAABB(m_textArea.getAABB());
}

AABB TextBoxComponent::getLocalAABB() const
{
    if(!m_scaleToMatchWithArea)
    {
        AABB aabb = m_textArea.getLocalAABB();
        aabb.Translate( m_offset );
        return aabb;
    }

    AABB aabb(Vec2d::Zero, Vec2d(Max(0.0f, m_area.x()), Max(0.0f, m_area.y())));
    aabb.Translate( m_offset );
    return aabb;
}

void TextBoxComponent::setLocalAABB( const AABB &_aabb )
{
    m_offset.x() = _aabb.getMin().x();
    if( m_textArea.getIs2D() )
        m_offset.y() = _aabb.getMin().y();
    else
        m_offset.y() = _aabb.getMax().y();

    AABB aabb = m_textArea.getLocalAABB();
    
    float sizeX = _aabb.getSize().x();
    if ( m_area.x() >= 0 || f32_Abs(sizeX - aabb.getSize().x()) > MTH_BIG_EPSILON)
        m_area.x() = sizeX;

    float sizeY = _aabb.getSize().y();
    if ( m_area.y() >= 0 || f32_Abs(sizeY - aabb.getSize().y()) > MTH_BIG_EPSILON)
        m_area.y() = sizeY;

    if(!m_scaleToMatchWithArea)
    {
        m_textArea.setArea(m_area);
    }
}


void TextBoxComponent::setIsVisible(bbool _isVisible)
{
    m_textArea.setIsVisible(_isVisible);
}

void TextBoxComponent::updateText(f32 _deltaTime)
{    
    Vec2d offset = m_offset * m_mainScale + m_mainOffset;
    m_actor->transformLocalVectorToWorld(offset);

    Vec2d localScale = m_actor->getScale() * m_scale * m_mainScale;

    m_textArea.setPosition(m_actor->getPos() + offset.to3d(0.001f));
    m_textArea.setLocalScale(localScale);
    m_textArea.setLocalAngle(m_actor->getAngle() + m_mainAngle);
    m_textArea.setViewMask(m_actor->getWorldUpdateElement()->getViewMask());
    m_textArea.Update(_deltaTime);

    if(m_scaleToMatchWithArea)
    {
        f32 scale =1.0f;
        Vec2d aabbSize = m_textArea.getLocalAABB().getSize();

        if(aabbSize.x()>m_area.x() && m_area.x()>=0.0f)
        {
            scale = Min(scale, m_area.x() / aabbSize.x());
        }

        if(aabbSize.y()>m_area.y() && m_area.y()>=0.0f)
        {
            scale = Min(scale, m_area.y() / aabbSize.y());
        }

        if(scale!=1.0f)
        {
            m_textArea.setLocalScale(localScale * scale);
            m_textArea.Update(_deltaTime);
        }
    }

    updateAABB();
}

f32 TextBoxComponent::getDepthOffset() const
{
    return getTemplate()->getDepthOffset() + m_depthOffset;
}

void TextBoxComponent::setViewportVisibility( u32 _viewportVisibility )
{
    m_textArea.setViewportVisibility(_viewportVisibility);
}


//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(TextBoxComponent_Template)
BEGIN_SERIALIZATION_CHILD(TextBoxComponent_Template)
SERIALIZE_CONTAINER_OBJECT("styles", m_styles);
SERIALIZE_MEMBER("depthOffset", m_depthOffset);
END_SERIALIZATION()

TextBoxComponent_Template::TextBoxComponent_Template()
: Super()
, m_styles(1)
, m_depthOffset(0.f)
{
    m_is2D = bfalse;
}
}
