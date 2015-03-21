#include "precompiled_gameplay.h"

#ifndef _ITF_MULTITEXTBOXCOMPONENT_H_
#include "gameplay/components/UI/MultiTextBoxComponent.h"
#endif //_ITF_MULTITEXTBOXCOMPONENT_H_

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

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

namespace ITF
{

///////////////////////////////////////////////////////////////////////////////////////////

BEGIN_SERIALIZATION_SUBCLASS(MultiTextBoxComponent, TextBox)
BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
SERIALIZE_OBJECT("text", m_text); 
SERIALIZE_MEMBER("area", m_area); 
SERIALIZE_MEMBER("offset", m_offset);
SERIALIZE_MEMBER("scale", m_scale);
SERIALIZE_MEMBER("style", m_defaultStyle);
SERIALIZE_BOOL("scaleToMatchWithArea", m_scaleToMatchWithArea);
SERIALIZE_MEMBER("maxWidth", m_maxWidth);
SERIALIZE_MEMBER("autoScrollSpeed", m_autoScrollSpeed);
SERIALIZE_MEMBER("autoScrollWaitTime", m_autoScrollWaitTime);
SERIALIZE_MEMBER("overridingColor",m_color);
SERIALIZE_ENUM_H_ALIGNMENT("overridingHAlignment",m_hAlignment);
SERIALIZE_ENUM_V_ALIGNMENT("overridingVAlignment",m_vAlignment);
SERIALIZE_ENUM_ANCHOR("overridingAnchor",m_anchor);
END_CONDITION_BLOCK()
END_SERIALIZATION()

MultiTextBoxComponent::TextBox::TextBox()
: m_offset(Vec3d::Zero)
, m_scale(Vec2d(1.0f, 1.0f))
, m_area(Vec2d(-1.0f, -1.0f))
, m_defaultStyle(0)
, m_text("Text", LocalisationId::Invalid)
, m_color(0)
, m_hAlignment(FONT_ALIGN_NONE)
, m_vAlignment(FONT_ALIGN_NONE)
, m_anchor(AREA_ANCHOR_NONE)
, m_scaleToMatchWithArea(bfalse)
, m_maxWidth(-1.0f)
, m_autoScrollSpeed(0.0f)
, m_autoScrollWaitTime(0.0f)
, m_currentStyle(U32_INVALID)
{

}

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(MultiTextBoxComponent)
BEGIN_SERIALIZATION_CHILD(MultiTextBoxComponent)
BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
SERIALIZE_CONTAINER_OBJECT("textBoxList", m_textBoxList);
END_CONDITION_BLOCK()
END_SERIALIZATION()


#ifdef ITF_SUPPORT_EDITOR
void MultiTextBoxComponent::onPostPropertyChange()
{
    i32 index = 0;
    for(ITF_VECTOR<MultiTextBoxComponent::TextBox>::iterator it = m_textBoxList.begin(); it!=m_textBoxList.end(); ++it)
    {
        it->m_textArea.setIsActive(m_actor->isActive());
        it->m_textArea.setIs2D(m_actor->getIs2D());
        it->m_textArea.setOwner(m_actor);
        it->m_textArea.setText(it->m_text.getText());

        if(it->m_scaleToMatchWithArea)
        {
            it->m_textArea.setArea(Vec2d(-1.0f, -1.0f));
        }
        else
        {
            it->m_textArea.setArea(it->m_area);
        }

        it->m_textArea.setMaxWidth(it->m_maxWidth);
        it->m_textArea.setAutoScrollXSpeed(it->m_autoScrollSpeed);
        it->m_textArea.setAutoScrollXWaitTime(it->m_autoScrollWaitTime);

        setStyle(index, it->m_defaultStyle);

        index++;
    }
}

#endif //ITF_SUPPORT_EDITOR

///////////////////////////////////////////////////////////////////////////////////////////
MultiTextBoxComponent::MultiTextBoxComponent()
: m_mainScale(1.0f)
, m_mainAlpha(1.0f)
, m_showAlpha(1.0f)
, m_textBoxList(1)
{

}

///////////////////////////////////////////////////////////////////////////////////////////
MultiTextBoxComponent::~MultiTextBoxComponent()
{

}

void MultiTextBoxComponent::onStartDestroy( bbool _hotReload )
{
    Super::onStartDestroy(_hotReload);

    for(ITF_VECTOR<MultiTextBoxComponent::TextBox>::iterator it = m_textBoxList.begin(); it!=m_textBoxList.end(); ++it)
    {
        it->m_textArea.clear();
    }

    clearActorIcon();
}

void MultiTextBoxComponent::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if (EventShow* eventShow = DYNAMIC_CAST(_event, EventShow))
    {
        m_showAlpha = eventShow->getAlpha();
	}
	else if( EventViewportVisibility* eventViewportVisibility = DYNAMIC_CAST(_event,EventViewportVisibility) )
	{
		for(ITF_VECTOR<MultiTextBoxComponent::TextBox>::iterator it = m_textBoxList.begin(); it!=m_textBoxList.end(); ++it)
		{
			it->m_textArea.setViewportVisibility(eventViewportVisibility->getViewportVisibility());
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
void MultiTextBoxComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    if(m_textBoxList.size()==0)
    {
        m_textBoxList.emplace_back();
    }

    i32 index = 0;
    for(ITF_VECTOR<MultiTextBoxComponent::TextBox>::iterator it = m_textBoxList.begin(); it!=m_textBoxList.end(); ++it)
    {
        it->m_textArea.setIsActive(bfalse);
        it->m_textArea.setIs2D(m_actor->getIs2D());
        it->m_textArea.setText(it->m_text.getText(m_actor));
        it->m_textArea.setOwner(m_actor);

        if(it->m_scaleToMatchWithArea)
        {
            it->m_textArea.setArea(Vec2d(-1.0f, -1.0f));
        }
        else
        {
            it->m_textArea.setArea(it->m_area);
        }

        it->m_textArea.setMaxWidth(it->m_maxWidth);
        it->m_textArea.setAutoScrollXSpeed(it->m_autoScrollSpeed);
        it->m_textArea.setAutoScrollXWaitTime(it->m_autoScrollWaitTime);

        setStyle(index, it->m_defaultStyle);

        index++;
    }

	ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventViewportVisibility_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor, EventShow_CRC, this);
 }

void MultiTextBoxComponent::onBecomeActive()
{
    Super::onBecomeActive();

    
    for(ITF_VECTOR<MultiTextBoxComponent::TextBox>::iterator it = m_textBoxList.begin(); it!=m_textBoxList.end(); ++it)
    {
        it->m_textArea.setIsActive(btrue);
        it->m_textArea.setOwner(m_actor);
    }
    
}

void MultiTextBoxComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    for(ITF_VECTOR<MultiTextBoxComponent::TextBox>::iterator it = m_textBoxList.begin(); it!=m_textBoxList.end(); ++it)
    {
        it->m_textArea.setIsActive(bfalse);
    }
}

void MultiTextBoxComponent::onFinalizeLoad()
{
    Super::onFinalizeLoad();

}

void MultiTextBoxComponent::setShadow(u32 _textBoxIndex, const Vec2d& _shadowOffset, const Color _shadowColor) 
{
    m_textBoxList[_textBoxIndex].m_textArea.setShadow(_shadowOffset, _shadowColor);
}

void MultiTextBoxComponent::setBox(u32 _textBoxIndex, const Vec2d& _area, const TEXT_AREA_ANCHOR _anchor)
{
    MultiTextBoxComponent::TextBox& textBox = m_textBoxList[_textBoxIndex];

    if(_area != Vec2d::Zero)
    {
        textBox.m_textArea.setArea(_area);
    }

    if(_anchor!=AREA_ANCHOR_NONE && textBox.m_anchor!=AREA_ANCHOR_NONE)
    {
        textBox.m_textArea.setAnchor(_anchor);
    }
}

void MultiTextBoxComponent::setSize(u32 _textBoxIndex, const f32 _fontSize)
{
    m_textBoxList[_textBoxIndex].m_textArea.setSize(_fontSize);
}

void MultiTextBoxComponent::setOffset(u32 _textBoxIndex, const Vec3d& _offset) 
{
    m_textBoxList[_textBoxIndex].m_offset = _offset;
}

void MultiTextBoxComponent::setColor(u32 _textBoxIndex, const Color _fontColor)
{
    m_textBoxList[_textBoxIndex].m_textArea.setColor(_fontColor);
}


void MultiTextBoxComponent::setFont(u32 _textBoxIndex, const Color _fontColor, const f32 _fontSize, const FONT_ALIGNMENT _hAlignment, const FONT_ALIGNMENT _vAlignment)
{
    MultiTextBoxComponent::TextBox& textBox = m_textBoxList[_textBoxIndex];

    if(textBox.m_color.getAsU32()==0)
    {
        textBox.m_textArea.setColor(_fontColor);
    }
    
    if(_fontSize!=0.0)
    {
        textBox.m_textArea.setSize(_fontSize);
    }
    
    if(_hAlignment!=FONT_ALIGN_NONE && textBox.m_hAlignment!=FONT_ALIGN_NONE)
    {
        textBox.m_textArea.setHAlignment(_hAlignment);
    }

    if(_vAlignment!=FONT_ALIGN_NONE && textBox.m_vAlignment!=FONT_ALIGN_NONE)
    {
        textBox.m_textArea.setVAlignment(_vAlignment);
    }
}

void MultiTextBoxComponent::setStyle(u32 _textBoxIndex, u32 _styleIndex)
{
    MultiTextBoxComponent::TextBox& textBox = m_textBoxList[_textBoxIndex];

    if(_styleIndex == U32_INVALID)
    {
        _styleIndex = textBox.m_defaultStyle;
    }

    if(_styleIndex < getTemplate()->m_styles.size())
    {
        const FontTextArea::Style& style = getTemplate()->m_styles[_styleIndex];

        textBox.m_textArea.setStyle(style);

        if(textBox.m_color.getAsU32()!=0)
        {
            textBox.m_textArea.setColor(textBox.m_color);  
        }

        if(textBox.m_hAlignment!=FONT_ALIGN_NONE)
        {
            textBox.m_textArea.setHAlignment(textBox.m_hAlignment);
        }

        if(textBox.m_vAlignment!=FONT_ALIGN_NONE)
        {
            textBox.m_textArea.setVAlignment(textBox.m_vAlignment);
        }

        if(textBox.m_anchor!=AREA_ANCHOR_NONE)
        {
            textBox.m_textArea.setAnchor(textBox.m_anchor);
        }
    }

    textBox.m_currentStyle = _styleIndex;
}

void MultiTextBoxComponent::setIsVisible(u32 _textBoxIndex, bbool _isVisible)
{
    m_textBoxList[_textBoxIndex].m_textArea.setIsVisible(_isVisible);
}

void MultiTextBoxComponent::setText(u32 _textBoxIndex, const String8& _text)
{
    m_textBoxList[_textBoxIndex].m_textArea.setText(_text);
}

void MultiTextBoxComponent::setMaxWidth(u32 _textBoxIndex, f32 _width)
{
    m_textBoxList[_textBoxIndex].m_textArea.setMaxWidth(_width);
}

void MultiTextBoxComponent::insertTextClear(u32 _textBoxIndex)
{
    m_textBoxList[_textBoxIndex].m_textArea.setText(m_textBoxList[_textBoxIndex].m_text.getText());
}

void MultiTextBoxComponent::insertTextAtMarker(u32 _textBoxIndex, u32 _marker, const String8 &_textToInsert)
{
    m_textBoxList[_textBoxIndex].m_textArea.insertTextAtMarker(_marker, _textToInsert);
}

///////////////////////////////////////////////////////////////////////////////////////////
u32 MultiTextBoxComponent:: getTextBoxCount() const
{
    return m_textBoxList.size();
}

const FontTextArea & MultiTextBoxComponent::getTextArea(u32 _textBoxIndex) const
{
    return m_textBoxList[_textBoxIndex].m_textArea;
}

FontTextArea & MultiTextBoxComponent::getTextArea(u32 _textBoxIndex)
{
    return m_textBoxList[_textBoxIndex].m_textArea;
}

Color MultiTextBoxComponent:: getInitColor(u32 _textBoxIndex) const
{
    const TextBox& textbox = m_textBoxList[_textBoxIndex];

    if(textbox.m_color.getAsU32()!=0)
    {
        return textbox.m_color;  
    }

    if(textbox.m_currentStyle < getTemplate()->m_styles.size())
    {
        return getTemplate()->m_styles[textbox.m_currentStyle].m_color;
    }

    return 0;
}

f32 MultiTextBoxComponent::getInitMaxWidth(u32 _textBoxIndex) const
{
    return m_textBoxList[_textBoxIndex].m_maxWidth;
}

const Vec3d & MultiTextBoxComponent::getOffset(u32 _textBoxIndex) const
{
    return m_textBoxList[_textBoxIndex].m_offset;
}

///////////////////////////////////////////////////////////////
void MultiTextBoxComponent::Update( f32 _deltaTime )
{
    Super::Update(_deltaTime);

    updateText(_deltaTime);
    updateActorIcon(_deltaTime);
}

///////////////////////////////////////////////////////////////////////////////////////////

void MultiTextBoxComponent::onResourceLoaded()
{
    updateText(0);
}


//////////////////////////////////////////////////////////////////////////////////////////

void MultiTextBoxComponent::onScaleChanged( const Vec2d& _oldScale, const Vec2d& _newScale )
{
    Super::onScaleChanged(_oldScale,_newScale);

    if(m_actor)
    {
        updateText(0);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////


#ifdef ITF_SUPPORT_EDITOR
    ///////////////////////////////////////////////////////////////////////////////////////////
    void MultiTextBoxComponent::drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags ) const 
    {
        Super::drawEdit(drawInterface, _flags);

        for(ITF_VECTOR<MultiTextBoxComponent::TextBox>::const_iterator it = m_textBoxList.begin(); it!=m_textBoxList.end(); ++it)
        {
            const TextBox& textBox = *it;
            Vec2d area = Vec2d(max(0.0f, textBox.m_area.x()), max(0.0f, textBox.m_area.y()));
            AABB areaAABB(Vec2d::Zero, area);
            areaAABB.Translate(textBox.m_textArea.getLocalTopLeftOffset(area, textBox.m_offset.truncateTo2D(), bfalse));
            //areaAABB.transformLocalToGlobal(m_actor->get2DPos(), m_actor->getScale() * m_mainScale, m_actor->getAngle());

            drawBox(areaAABB.getMin(), areaAABB.getMax(), 3.0f, Color::pink());
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    void MultiTextBoxComponent::drawBox(Vec2d _minBound, Vec2d _maxBound, const f32 _border, const Color _color) const
    {
        Vec2d bottomRight(_maxBound.x(), _minBound.y());
        Vec2d topLeft(_minBound.x(), _maxBound.y());

        if(m_actor->getIs2D())
        {
            Vec2d actorPos = m_actor->get2DPos();
            _maxBound.y() *= -1.0f; m_actor->transformLocalVectorToWorld(_maxBound); _maxBound.y() *= -1.0f; _maxBound += actorPos;
            _minBound.y() *= -1.0f; m_actor->transformLocalVectorToWorld(_minBound); _minBound.y() *= -1.0f; _minBound += actorPos;
            bottomRight.y() *= -1.0f; m_actor->transformLocalVectorToWorld(bottomRight); bottomRight.y() *= -1.0f; bottomRight += actorPos;
            topLeft.y() *= -1.0f;m_actor->transformLocalVectorToWorld(topLeft); topLeft.y() *= -1.0f; topLeft += actorPos;

            GFX_ADAPTER->drawDBG2dLine(_minBound, topLeft, _border, _color.getAsU32());
            GFX_ADAPTER->drawDBG2dLine(topLeft, _maxBound, _border, _color.getAsU32());
            GFX_ADAPTER->drawDBG2dLine(_maxBound, bottomRight, _border, _color.getAsU32());
            GFX_ADAPTER->drawDBG2dLine(bottomRight, _minBound, _border, _color.getAsU32());
        }
        else
        {
            m_actor->transformLocalPosToWorld(_maxBound);
            m_actor->transformLocalPosToWorld(_minBound);
            m_actor->transformLocalPosToWorld(bottomRight);
            m_actor->transformLocalPosToWorld(_maxBound);

            DebugDraw::line2D(_minBound, topLeft, m_actor->getDepth(), _color, _border);
            DebugDraw::line2D(topLeft, _maxBound, m_actor->getDepth(), _color, _border);
            DebugDraw::line2D(_maxBound, bottomRight, m_actor->getDepth(), _color, _border);
            DebugDraw::line2D(bottomRight, _minBound, m_actor->getDepth(), _color, _border);
        }
    }

    void MultiTextBoxComponent::onEditorMove( bbool _modifyInitialPos /*= btrue*/ )
    {
        updateText(0);
    }

#endif //ITF_SUPPORT_EDITOR

////////////////////////////////////////////////////////////////////////////
void MultiTextBoxComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
{
    for(ITF_VECTOR<MultiTextBoxComponent::TextBox>::iterator it = m_textBoxList.begin(); it!=m_textBoxList.end(); ++it)
    {
        it->m_textArea.Draw(_views, 0.f, GetActor()->getRef());
    } 
}

void MultiTextBoxComponent::batchPrimitives2D( const ITF_VECTOR <class View*>& _views )
{
    for(ITF_VECTOR<MultiTextBoxComponent::TextBox>::iterator it = m_textBoxList.begin(); it!=m_textBoxList.end(); ++it)
    {
        it->m_textArea.Draw(_views, 0.f, GetActor()->getRef());
    } 
}


void MultiTextBoxComponent::updateAABB()
{
    for(ITF_VECTOR<MultiTextBoxComponent::TextBox>::iterator it = m_textBoxList.begin(); it!=m_textBoxList.end(); ++it)
    {
        m_actor->growAABB(it->m_textArea.getAABB());
    } 
}



void MultiTextBoxComponent::updateText(f32 _deltaTime )
{    
    for(ITF_VECTOR<MultiTextBoxComponent::TextBox>::iterator it = m_textBoxList.begin(); it!=m_textBoxList.end(); ++it)
    {
        TextBox& textBox = *it;
        Vec2d offset = textBox.m_offset.truncateTo2D() * m_mainScale;
        Vec2d localScale = m_actor->getScale() * textBox.m_scale * m_mainScale;

        if (m_actor->getIs2D())
        {
            offset.y() *= -1.0f;
            m_actor->transformLocalVectorToWorld(offset);
            offset.y() *= -1.0f;
        }
        else
        {
            m_actor->transformLocalVectorToWorld(offset);
        }

        textBox.m_textArea.setAlpha(m_mainAlpha * m_showAlpha);
        textBox.m_textArea.setPosition(m_actor->getPos() + offset.to3d(textBox.m_offset.z() + 0.001f));
        textBox.m_textArea.setLocalScale(localScale);
        textBox.m_textArea.setLocalAngle(m_actor->getAngle());
        textBox.m_textArea.setViewMask(m_actor->getWorldUpdateElement()->getViewMask());
        textBox.m_textArea.Update(_deltaTime);
        
        if(textBox.m_scaleToMatchWithArea)
        {
            f32 scale =1.0f;
            Vec2d aabbSize = textBox.m_textArea.getLocalAABB().getSize();

            if(aabbSize.x()>textBox.m_area.x() && textBox.m_area.x()>=0.0f)
            {
                scale = Min(scale, textBox.m_area.x() / aabbSize.x());
            }

            if(aabbSize.y()>textBox.m_area.y() && textBox.m_area.y()>=0.0f)
            {
                scale = Min(scale, textBox.m_area.y() / aabbSize.y());
            }

            if(scale!=1.0f)
            {
                textBox.m_textArea.setLocalScale(localScale * scale);
                textBox.m_textArea.Update(_deltaTime);
            }
        }
    } 


    updateAABB();
}

bbool MultiTextBoxComponent::isTextUpdated() const
{
    for(ITF_VECTOR<MultiTextBoxComponent::TextBox>::const_iterator it = m_textBoxList.begin(); it!=m_textBoxList.end(); ++it)
    {
        if(!it->m_textArea.getIsUpdated())
        {
            return bfalse;
        }
    }

    return btrue;
}

void MultiTextBoxComponent::updateActorIcon(f32 _dt)
{
    if(m_wantedActorIcon!=m_currentActorIcon)
    {
        if(const MultiTextBoxComponent::ActorIcon* icon = getActorIcon(m_currentActorIcon))
        {
            if(Actor* actor = icon->m_iconActor.getActor())
            {
                actor->disable();
            }
        }

        if(const MultiTextBoxComponent::ActorIcon* icon = getActorIcon(m_wantedActorIcon))
        {
            if(Actor* actor = icon->m_iconActor.getActor())
            {
                actor->enable();
            }
        }

        m_currentActorIcon = m_wantedActorIcon;
    }

    if(const MultiTextBoxComponent::ActorIcon* icon = getActorIcon(m_currentActorIcon))
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

                if(icon->m_textBoxIndex<m_textBoxList.size())
                {
                    const FontTextArea& textArea = m_textBoxList[icon->m_textBoxIndex].m_textArea;
                    f32 fontScale = 1.0f;
                    if(icon->m_iconFontSize!=0.0f)
                    {
                        fontScale =  textArea.getSize() / icon->m_iconFontSize;
                    }
                    Vec2d offset(icon->m_iconFontSize * fontScale * -0.5f, 0.0f /*icon->m_iconFontSize * fontScale * 0.5f*/);
                    textArea.getFirstCharPos(pos, scale, angle, offset + icon->m_iconOffset);
                    scale *= fontScale;
                }

                actor->setPos(pos);
                actor->setScale(scale);
                actor->setAngle(angle);

                EventShow showEv(m_mainAlpha * m_showAlpha, 0.0f);
                actor->onEvent(&showEv);
            }
        }
    }
}

void MultiTextBoxComponent::setActorIcon(StringID _iconName)
{
    m_wantedActorIcon = _iconName;
}

const MultiTextBoxComponent::ActorIcon* MultiTextBoxComponent::getActorIcon(StringID _iconName) const
{
    if(_iconName.isValid())
    {
        for(u32 n=0; n<m_iconActors.size(); ++n)
        {
            const MultiTextBoxComponent::ActorIcon& ActorIcon = m_iconActors[n];
            if(ActorIcon.m_iconName == _iconName)
            {
                return &ActorIcon;
            }
        }
    }

    return NULL;
}

MultiTextBoxComponent::ActorIcon* MultiTextBoxComponent::useActorIcon(StringID _iconName, bbool _use)
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

void MultiTextBoxComponent::clearActorIcon()
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
IMPLEMENT_OBJECT_RTTI(MultiTextBoxComponent_Template)
BEGIN_SERIALIZATION_CHILD(MultiTextBoxComponent_Template)
SERIALIZE_CONTAINER_OBJECT("styles", m_styles);
END_SERIALIZATION()

MultiTextBoxComponent_Template::MultiTextBoxComponent_Template() :
m_styles(1)
{
}
}
