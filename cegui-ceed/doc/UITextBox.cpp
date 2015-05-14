
#include "precompiled_gameplay.h"

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

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
IMPLEMENT_OBJECT_RTTI(UITextBox)
BEGIN_SERIALIZATION_CHILD(UITextBox)
    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
    SERIALIZE_MEMBER("style", m_defaultStyle);
    SERIALIZE_MEMBER("offset", m_offset);
    SERIALIZE_MEMBER("depth", m_depth);
    SERIALIZE_MEMBER("scale", m_scale);
    SERIALIZE_MEMBER("area", m_area); 
    SERIALIZE_MEMBER("maxWidth", m_maxWidth);
    SERIALIZE_MEMBER("rawText", m_text.m_defaultText); 
    SERIALIZE_MEMBER("locId", m_text.m_locId); 
    SERIALIZE_MEMBER("overridingColor", m_color);
    BEGIN_CONDITION_BLOCK(ESerialize_Deprecate)
        bbool scaleOneLine = bfalse;
        SERIALIZE_MEMBER("scaleToMatchWithArea", scaleOneLine);
        if (scaleOneLine)
        {
            m_scaleToMatchWithArea = ScaleMatchOneLine;
        }
    END_CONDITION_BLOCK()
    SERIALIZE_ENUM_BEGIN("scaleToMatchWithArea", m_scaleToMatchWithArea);
        SERIALIZE_ENUM_VAR(NoScaleMatch);
        SERIALIZE_ENUM_VAR(ScaleMatchOneLine);
        SERIALIZE_ENUM_VAR(ScaleMatchMultiLine);
    SERIALIZE_ENUM_END();
    SERIALIZE_MEMBER("autoScrollSpeed", m_autoScrollSpeed);
    SERIALIZE_MEMBER("autoScrollWaitTime", m_autoScrollWaitTime);
    SERIALIZE_MEMBER("autoScrollLoop", m_autoScrollLoop);
    SERIALIZE_MEMBER("autoScrollLoopGap", m_autoScrollLoopGap);
    SERIALIZE_ENUM_H_ALIGNMENT("overridingHAlignment",m_hAlignment);
    SERIALIZE_ENUM_V_ALIGNMENT("overridingVAlignment",m_vAlignment);
    SERIALIZE_ENUM_ANCHOR("overridingAnchor",m_anchor);
    END_CONDITION_BLOCK()
END_SERIALIZATION()

#ifdef ITF_SUPPORT_EDITOR
void UITextBox::onPostPropertyChange()
{
    setStyle(m_defaultStyle);

    if(m_scaleToMatchWithArea == ScaleMatchOneLine)
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
    m_textArea.setAutoScrollXLoop(m_autoScrollLoop);
    m_textArea.setAutoScrollXLoopGap(m_autoScrollLoopGap);

    m_textArea.setText(m_text.getText());
    m_refreshScaleToMatchMultiLine = btrue;
}
#endif //ITF_SUPPORT_EDITOR

///////////////////////////////////////////////////////////////////////////////////////////
UITextBox::UITextBox()
: m_offset(Vec2d::Zero)
, m_scale(Vec2d::One)
, m_area(Vec2d(-1.0f, -1.0f))
, m_depth(0.0f)
, m_defaultStyle(0)
, m_text("Text", LocalisationId::Invalid)
, m_color(0)
, m_hAlignment(FONT_ALIGN_NONE)
, m_vAlignment(FONT_ALIGN_NONE)
, m_anchor(AREA_ANCHOR_NONE)
, m_mainScale(Vec2d::One)
, m_mainOffset(Vec2d::Zero)
, m_mainAlpha(1.0f)
, m_showAlpha(1.0f)
, m_dstAlpha(1.f)
, m_srcAlpha(1.f)
, m_fadeTime(0.f)
, m_fadeTotal(0.f)
, m_scaleToMatchWithArea(NoScaleMatch)
, m_maxWidth(-1.0f)
, m_autoScrollSpeed(0.0f)
, m_autoScrollWaitTime(0.0f)
, m_autoScrollLoop(bfalse)
, m_autoScrollLoopGap(1)
, m_currentStyle(U32_INVALID)
, m_refreshScaleToMatchMultiLine(btrue)
, m_baseFontSize(0.0f)
{
    m_textArea.setIsActive(bfalse);
}

///////////////////////////////////////////////////////////////////////////////////////////
UITextBox::~UITextBox()
{

}


///////////////////////////////////////////////////////////////////////////////////////////
void UITextBox::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    Vec2d offset = m_offset;
    m_actor->transformLocalVectorToWorld(offset);

    m_textArea.setIs2D(btrue);
    m_textArea.setPosition(getAbsolutePosition() + offset);
    m_textArea.setOwner(m_actor);

    if(m_scaleToMatchWithArea == ScaleMatchOneLine)
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
    m_textArea.setAutoScrollXLoop(m_autoScrollLoop);
    m_textArea.setAutoScrollXLoopGap(m_autoScrollLoopGap);

	ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventViewportVisibility_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventShow_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventSetText_CRC,this);

    m_textArea.setText(m_text.getText());
}

void UITextBox::onFinalizeLoad()
{
    Super::onFinalizeLoad();

    for(ITF_VECTOR<Path>::const_iterator it = getTemplate()->m_preSpawnedActorPaths.begin(); it!=getTemplate()->m_preSpawnedActorPaths.end(); ++it)
    {
        m_textArea.addPreSpawnedActor(*it);
    }

    setStyle(m_defaultStyle);
}

void UITextBox::onStartDestroy( bbool _hotReload )
{
    Super::onStartDestroy(_hotReload);

    m_textArea.clear();

    clearActorIcon();
}


void UITextBox::setShadow(const Vec2d& _shadowOffset, const Color _shadowColor) 
{
    m_textArea.setShadow(_shadowOffset, _shadowColor);
    m_refreshScaleToMatchMultiLine = btrue;
}

void UITextBox::setBox(const Vec2d& _area, const TEXT_AREA_ANCHOR _anchor)
{
    if(_area != Vec2d::Zero)
    {
        m_textArea.setArea(_area);
        m_refreshScaleToMatchMultiLine = btrue;
    }

    if(_anchor!=AREA_ANCHOR_NONE && m_anchor!=AREA_ANCHOR_NONE)
    {
        m_textArea.setAnchor(_anchor);
        m_refreshScaleToMatchMultiLine = btrue;
    }
}

void UITextBox::setFont(const Color _fontColor, const f32 _fontSize, const FONT_ALIGNMENT _hAlignment, const FONT_ALIGNMENT _vAlignment)
{
    if(m_color.getAsU32()==0)
    {
        m_textArea.setColor(_fontColor);
        m_refreshScaleToMatchMultiLine = btrue;
    }

    if(_fontSize!=0.0)
    {
        m_textArea.setSize(_fontSize);
        m_baseFontSize = m_textArea.getSize();
        m_refreshScaleToMatchMultiLine = btrue;
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

void UITextBox::setColor(const Color _fontColor)
{
    if(m_color.getAsU32()==0)
    {
        m_textArea.setColor(_fontColor);
    }
}

void UITextBox::setAlpha(f32 _alpha)
{
    m_textArea.setAlpha(_alpha * m_showAlpha);
    m_mainAlpha = _alpha;
}

void UITextBox::setStyle(u32 _styleIndex)
{
    if(_styleIndex == U32_INVALID)
    {
        _styleIndex = m_defaultStyle;
    }

    if(_styleIndex < getTemplate()->m_styles.size())
    {
        const FontTextArea::Style& style = getTemplate()->m_styles[_styleIndex];

        m_textArea.setStyle(style);
        m_refreshScaleToMatchMultiLine = btrue;
        m_baseFontSize = m_textArea.getSize();

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

        if(m_anchor!=AREA_ANCHOR_NONE)
        {
            m_textArea.setAnchor(m_anchor);  
        }
    }

    m_currentStyle = _styleIndex;
}

void UITextBox::setConstAdvanceChar(const char _constAdvanceChar_Size, const char _constAdvanceChar_Min, const char _constAdvanceChar_Max)
{
    m_textArea.setConstAdvanceChar(_constAdvanceChar_Size, _constAdvanceChar_Min, _constAdvanceChar_Max);
    m_refreshScaleToMatchMultiLine = btrue;
}

void  UITextBox::setEffect(StringID _name, bool _smooth )
{
    if(const FontEffect_Template* effect = UI_TEXTMANAGER->getFontEffect(_name))
    {
        m_textArea.setFontEffect(effect, _smooth);
    }
    else
    {
        m_textArea.clearFontEffect(_smooth);
    }
    m_refreshScaleToMatchMultiLine = btrue;
    
}

void UITextBox::setMaxWidth(f32 _width)
{
    m_textArea.setMaxWidth(_width);
    m_refreshScaleToMatchMultiLine = btrue;
}

Color UITextBox::getInitColor() const
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

String8 UITextBox::getInitText() const
{
    return m_text.getText();
}

f32 UITextBox::getInitSize() const
{
    if(m_currentStyle < getTemplate()->m_styles.size())
    {
        return getTemplate()->m_styles[m_currentStyle].m_fontSize;
    }

    return 0.0f;
}

u32 UITextBox::getInitStyle() const
{
    return m_defaultStyle;
}

void UITextBox::onBecomeActive()
{
    Super::onBecomeActive();

    m_textArea.setIsActive(btrue);
    m_textArea.setOwner(m_actor);
    setActorIconEnabled(btrue);
}

void UITextBox::onBecomeInactive()
{
    Super::onBecomeInactive();

    setActorIconEnabled(bfalse);
    m_textArea.setIsActive(bfalse);
}

///////////////////////////////////////////////////////////////
void UITextBox::Update( f32 _deltaTime )
{
    Super::Update(_deltaTime);

    updateAlpha(_deltaTime);

    Vec2d offset = m_offset * m_mainScale + m_mainOffset;
    m_actor->transformLocalVectorToWorld(offset);

    Vec2d localScale = m_actor->getScale() * m_scale * m_mainScale;

    m_textArea.setPosition(m_actor->getPos() + offset.to3d(m_depth));
    m_textArea.setLocalScale(localScale);
    m_textArea.setLocalAngle(m_actor->getAngle());
    m_textArea.setViewMask(m_actor->getWorldUpdateElement()->getViewMask());
    m_textArea.Update(_deltaTime);
     
    if(m_scaleToMatchWithArea == ScaleMatchOneLine)
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
    if (m_scaleToMatchWithArea == ScaleMatchMultiLine && m_refreshScaleToMatchMultiLine && m_area.y() >= 0.0f && m_area.x() >= 0.0f && m_textArea.getIsUpdated())
    {
        if (m_baseFontSize == 0.0f)
            m_baseFontSize = getSize();
        m_textArea.setSize(m_baseFontSize);
        m_textArea.Update(0);
        f32 height = m_textArea.getTextHeight();
        if (height > m_area.y())
        {
            f32 maxValid = 0.0f;
            f32 minInvalid = m_baseFontSize;
            for (ux i = 0; i < 8; ++i)
            {
                f32 pivot = (minInvalid + maxValid) / 2;
                m_textArea.setSize(pivot);
                m_textArea.Update(0);
                if (m_textArea.getTextHeight() > m_area.y())
                {
                    minInvalid = pivot;
                }
                else
                {
                    maxValid = pivot;
                }
            }
            m_textArea.setSize(maxValid);
            m_textArea.Update(0);
        }
        m_refreshScaleToMatchMultiLine = bfalse;
    }

    updateActorIcon(_deltaTime);
    updateAABB();
} 

void UITextBox::updateAlpha( f32 _dt )
{
    if ( m_fadeTime <= 0.f|| m_fadeTotal <= 0.f) 
        return;

    m_fadeTime -= _dt;

    if ( m_fadeTime <= 0.f)
    {
        m_showAlpha = m_dstAlpha;
    }
    else
    {
        f32 t = 1.f - ( m_fadeTime / m_fadeTotal );
        m_showAlpha = Interpolate(m_srcAlpha,m_dstAlpha,t);
    }

    m_textArea.setAlpha(m_mainAlpha * m_showAlpha);
}

///////////////////////////////////////////////////////////////////////////////////////////

void UITextBox::setIsVisible(bbool _isVisible)
{
    m_textArea.setIsVisible(_isVisible);
    m_refreshScaleToMatchMultiLine = btrue;
}

///////////////////////////////////////////////////////////////////////////////////////////

void UITextBox::setSize(const f32 _fontSize)
{
    if(_fontSize!=0.0)
    {
        m_textArea.setSize(_fontSize);
        m_refreshScaleToMatchMultiLine = btrue;
        m_baseFontSize = m_textArea.getSize();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

f32 UITextBox::getSize() const
{
    return m_textArea.getSize();
}

#ifdef ITF_SUPPORT_EDITOR
///////////////////////////////////////////////////////////////////////////////////////////
void UITextBox::drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags ) const 
{
    Super::drawEdit(drawInterface, _flags);

    Vec2d area = Vec2d(max(0.0f, m_area.x()), max(0.0f, m_area.y()));
    AABB areaAABB(Vec2d::Zero, area);
    areaAABB.Translate(m_textArea.getLocalTopLeftOffset(area, m_offset, bfalse));
    areaAABB.transformLocalToGlobal(m_actor->get2DPos(), m_actor->getScale() * m_mainScale * m_scale, m_actor->getAngle());
    drawBox2D(areaAABB.getMin(), areaAABB.getMax(), 3.0f, Color::pink());

    drawBox(3.0f);
}

////////////////////////////////////////////////////////////////////////////
void UITextBox::drawBox(const f32 _border, const Color _color) const
{
    AABB aabb = m_textArea.getAABB();
    Vec2d boxMinPosition = aabb.getMin();
    Vec2d boxMaxPosition = aabb.getMax();
    if( m_textArea.getIs2D() )
    {
        drawBox2D(boxMinPosition, boxMaxPosition, _border, _color);
    }
    else
    {
        drawBox3D(boxMinPosition, boxMaxPosition, _border, _color);
    }
}


#endif //ITF_SUPPORT_EDITOR

////////////////////////////////////////////////////////////////////////////
void UITextBox::batchPrimitives2D( const ITF_VECTOR <class View*>& _views )
{
    m_textArea.Draw(_views, getTemplate()->getdepthOffset(), GetActor()->getRef());
}

///////////////////////////////////////////////////////////////////////////////////////////

const String8 & UITextBox::getText() const
{
    return m_textArea.getText();
}

void UITextBox::setText(const String8 & _text)
{
    m_textArea.setText(_text);
    m_refreshScaleToMatchMultiLine = btrue;
}

void UITextBox::setLoc(const LocalisationId& _locId)
{
    m_textArea.setText(LOCALISATIONMANAGER->getText(_locId));
    m_refreshScaleToMatchMultiLine = btrue;
}

void UITextBox::insertTextClear()
{
    m_textArea.setText(m_text.getText());
    m_refreshScaleToMatchMultiLine = btrue;
}

void UITextBox::insertTextAtMarker(u32 _marker, const String8 &_textToInsert)
{
    m_textArea.insertTextAtMarker(_marker, _textToInsert);
    m_refreshScaleToMatchMultiLine = btrue;
}

void UITextBox::setActorScaleFactor(f32 _factor)
{
    m_textArea.setGlobalActorScaleFactor(_factor);
    m_refreshScaleToMatchMultiLine = btrue;
}

void UITextBox::setViewportVisibility(u32 _viewportVisibility)
{
    m_textArea.setViewportVisibility(_viewportVisibility);
    m_refreshScaleToMatchMultiLine = btrue;
}

void UITextBox::updateAABB()
{
    GetActor()->growAABB(m_textArea.getAABB());
}

void UITextBox::onEvent( Event * _event )
{
    Super::onEvent(_event);

    if ( EventShow* eventShow = DYNAMIC_CAST(_event,EventShow) )
    {   
        m_srcAlpha = m_showAlpha;
        m_dstAlpha = eventShow->getAlpha();
        m_fadeTotal = eventShow->getTransitionTime();

        if ( m_fadeTotal <= 0.f)
        {
            m_textArea.setAlpha(m_mainAlpha * eventShow->getAlpha());
            m_showAlpha = eventShow->getAlpha();
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


void UITextBox::updateActorIcon(f32 _dt)
{
    if(m_wantedActorIcon!=m_currentActorIcon)
    {
        if(const UITextBox::ActorIcon* icon = getActorIcon(m_currentActorIcon))
        {
            if(Actor* actor = icon->m_iconActor.getActor())
            {
                actor->disable();
            }
        }

        if(const UITextBox::ActorIcon* icon = getActorIcon(m_wantedActorIcon))
        {
            if(Actor* actor = icon->m_iconActor.getActor())
            {
                actor->enable();
            }
        }

        m_currentActorIcon = m_wantedActorIcon;
    }

    if(const UITextBox::ActorIcon* icon = getActorIcon(m_currentActorIcon))
    {
        if(Actor* actor = icon->m_iconActor.getActor())
        {
            if(!actor->isAsyncLoading())
            {
                if(!actor->getScene())
                {
                    m_actor->getScene()->registerPickable(actor);
                    AIUtils::bindChild(m_actor, actor);
                }

                Vec3d pos = m_actor->getPos();
                Vec2d scale = m_actor->getScale();
                f32 angle = m_actor->getAngle();


                f32 fontScale = 1.0f;
                if(icon->m_iconFontSize!=0.0f)
                {
                    fontScale =  m_textArea.getSize() / icon->m_iconFontSize;
                }
                Vec2d offset(icon->m_iconFontSize * fontScale * -0.5f, 0.0f /*icon->m_iconFontSize * fontScale * 0.5f*/);
                m_textArea.getFirstCharPos(pos, scale, angle, offset + icon->m_iconOffset);
                scale *= fontScale;

                actor->setPos(pos);
                actor->setScale(scale);
                actor->setAngle(angle);

                EventShow showEv(m_mainAlpha * m_showAlpha, 0.0f);
                actor->onEvent(&showEv);
            }
        }
    }
}

void UITextBox::setActorIcon(StringID _iconName)
{
    m_wantedActorIcon = _iconName;
}

void UITextBox::setActorIconEnabled(bool _isEnabled)
{
    if(const UITextBox::ActorIcon* icon = getActorIcon(m_currentActorIcon))
    {
        if(Actor* actor = icon->m_iconActor.getActor())
        {
            _isEnabled ? actor->enable() : actor->disable();
        }
    }
}

const UITextBox::ActorIcon* UITextBox::getActorIcon(StringID _iconName) const
{
    if(_iconName.isValid())
    {
        for(u32 n=0; n<m_iconActors.size(); ++n)
        {
            const UITextBox::ActorIcon& ActorIcon = m_iconActors[n];
            if(ActorIcon.m_iconName == _iconName)
            {
                return &ActorIcon;
            }
        }
    }

    return NULL;
}

UITextBox::ActorIcon* UITextBox::useActorIcon(StringID _iconName, bbool _use)
{
    u32 index = U32_INVALID;
    for(u32 n=0; n<m_iconActors.size(); ++n)
    {
        ActorIcon& ActorIcon = m_iconActors[n];
        if(ActorIcon.m_iconName == _iconName)
        {
            index = n;
            break;
        }
    }

    if(_use)
    {
        if(index==U32_INVALID)
        {
            ActorIcon icon;
            if(const UITextManager_Template::ActorIcon* ActorIcon = UI_TEXTMANAGER->getActorIcon(_iconName))
            {
                ACTORSMANAGER->getSpawnPoolManager().registerForRequest(m_actor->getRef(), m_actor->getResourceContainer(), ActorIcon->m_iconPath, 1, 1);
                if(Actor* actor = ACTORSMANAGER->getSpawnPoolManager().requestSpawn( ActorIcon->m_iconPath))
                {
                    icon.m_iconActor = actor->getRef();
                }

                icon.m_iconFontSize = ActorIcon->m_fontSize;
            }

            icon.m_iconName = _iconName;
            icon.m_usedCount++;
            m_iconActors.push_back(icon);
            index = m_iconActors.size() - 1;
        }
        else
        {
            m_iconActors[index].m_usedCount++;
        }
    }
    else
    {
        if(index!=U32_INVALID)
        {
            m_iconActors[index].m_usedCount--;
            if(m_iconActors[index].m_usedCount == 0)
            {
                if(Actor* actor = m_iconActors[index].m_iconActor.getActor())
                {
                    actor->requestDestruction();
                }

                if(const UITextManager_Template::ActorIcon* ActorIcon = UI_TEXTMANAGER->getActorIcon(_iconName))
                {
                    ACTORSMANAGER->getSpawnPoolManager().unregisterForRequest(m_actor->getRef(), ActorIcon->m_iconPath);
                }

                m_iconActors.removeAtUnordered(index);
                index = U32_INVALID;
            }
        }
    }

    if(index!=U32_INVALID)
    {
        return &m_iconActors[index];
    }

    return NULL;
}

void UITextBox::clearActorIcon()
{
    for(u32 n=m_iconActors.size(); n>0; --n)
    {
        u32 index= n - 1;
        if(Actor* actor = m_iconActors[index].m_iconActor.getActor())
        {
            actor->requestDestruction();
        }

        if(const UITextManager_Template::ActorIcon* ActorIcon = UI_TEXTMANAGER->getActorIcon(m_iconActors[index].m_iconName))
        {
            ACTORSMANAGER->getSpawnPoolManager().unregisterForRequest(m_actor->getRef(), ActorIcon->m_iconPath);
        }

        m_iconActors.removeAtUnordered(index);
    }
}

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(UITextBox_Template)
BEGIN_SERIALIZATION_CHILD(UITextBox_Template)
    SERIALIZE_CONTAINER_OBJECT("styles", m_styles);
    SERIALIZE_CONTAINER("preSpawnedActorPaths", m_preSpawnedActorPaths);
    SERIALIZE_MEMBER("depthOffset", m_depthOffset);
END_SERIALIZATION()

UITextBox_Template::UITextBox_Template() 
: m_styles(1)
, m_depthOffset(0)
{
}
}
