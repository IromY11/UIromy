#include "precompiled_engine.h"

#ifndef _ITF_FONTTEMPLATE_H_
#include "engine/display/FontTemplate.h"
#endif //_ITF_FONTTEMPLATE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef ITF_CORE_UNICODE_TOOLS_H_
#include "core/UnicodeTools.h"
#endif //ITF_CORE_UNICODE_TOOLS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(FontTemplate)

BEGIN_SERIALIZATION_SUBCLASS(FontTemplate,Info)
    SERIALIZE_MEMBER("face",m_face);
    SERIALIZE_MEMBER("size",m_size);
    SERIALIZE_MEMBER("bold",m_bold);
    SERIALIZE_MEMBER("italic",m_italic);
    SERIALIZE_MEMBER("charset",m_charset);
    SERIALIZE_MEMBER("unicode",m_unicode);
    SERIALIZE_MEMBER("stretchH",m_stretchH);
    SERIALIZE_MEMBER("smooth",m_smooth);
    SERIALIZE_MEMBER("aa",m_antiAlias);
    SERIALIZE_MEMBER("paddingLeft",m_paddingLeft);
    SERIALIZE_MEMBER("paddingRight",m_paddingRight);
    SERIALIZE_MEMBER("paddingTop",m_paddingTop);
    SERIALIZE_MEMBER("paddingBottom",m_paddingBottom);
    SERIALIZE_MEMBER("spacingLeft",m_spacingLeft);
    SERIALIZE_MEMBER("spacingTop",m_spacingTop);
    SERIALIZE_MEMBER("outline",m_outline);
END_SERIALIZATION()

BEGIN_SERIALIZATION_SUBCLASS(FontTemplate,Common)
    SERIALIZE_MEMBER("lineHeight",m_lineHeight);
    SERIALIZE_MEMBER("base",m_base);
    SERIALIZE_MEMBER("scaleW",m_scaleW);
    SERIALIZE_MEMBER("scaleH",m_scaleH);
    SERIALIZE_MEMBER("pages",m_pages);
    SERIALIZE_MEMBER("packed",m_packed);
    SERIALIZE_MEMBER("alphaChnl",m_alphaChannel);
    SERIALIZE_MEMBER("redChnl",m_redChannel);
    SERIALIZE_MEMBER("greenChnl",m_greenChannel);
    SERIALIZE_MEMBER("blueChnl",m_blueChannel);
END_SERIALIZATION()

BEGIN_SERIALIZATION_SUBCLASS(FontTemplate,Page)
    SERIALIZE_MEMBER("id",m_id);
    SERIALIZE_MEMBER("file",m_texture);
END_SERIALIZATION()

BEGIN_SERIALIZATION_SUBCLASS(FontTemplate,Char)
    SERIALIZE_MEMBER("id",m_id);
    SERIALIZE_MEMBER("x",m_x);
    SERIALIZE_MEMBER("y",m_y);
    SERIALIZE_MEMBER("width",m_width);
    SERIALIZE_MEMBER("height",m_height);
    SERIALIZE_MEMBER("xoffset",m_offsetX);
    SERIALIZE_MEMBER("yoffset",m_offsetY);
    SERIALIZE_MEMBER("xadvance",m_advanceX);
    SERIALIZE_MEMBER("page",m_page);
    SERIALIZE_MEMBER("chnl",m_channel);
END_SERIALIZATION()

    BEGIN_SERIALIZATION_CHILD(FontTemplate)
    SERIALIZE_PROPERTY_NAME("FontTemplateInfo"); 
    SERIALIZE_OBJECT("info",m_info); 

	SERIALIZE_PROPERTY_NAME("FontTemplateCommon"); 
    SERIALIZE_OBJECT("common",m_common);

    SERIALIZE_CONTAINER_OBJECT("pages",m_pages);
    SERIALIZE_CONTAINER_OBJECT("chars",m_chars);
END_SERIALIZATION()

bbool FontTemplate::onTemplateLoaded( bbool _hotReload )
{
    bbool bOk = Super::onTemplateLoaded(_hotReload);

    acquireResourceContainer(_hotReload);
    getResourceContainer()->loadResources();

    // build hash table mapping unicode ID to the index of the char in m_chars
    for(ITF_VECTOR<Char>::iterator it = m_chars.begin(); it!=m_chars.end(); ++it)
    {
        m_charsDesc.insert(it->m_id, uSize(std::distance(m_chars.begin(), it)));
    }

    for(ITF_VECTOR<Page>::iterator it = m_pages.begin(); it!=m_pages.end(); ++it)
    {
        Path globalPath = getFile().getDirectory();
        Path relatifPath = it->m_texture;

        //GG until version is deployed ,support both relative and local
        if (!relatifPath.getDepth())
        {
            it->m_texture = globalPath.copyAndAppendPath(relatifPath);
        }

        ResourceID res = addResource(Resource::ResourceType_Texture, it->m_texture);
        m_textures.push_back(res);
    }

    return bOk;
}

void FontTemplate::onTemplateDelete( bbool _hotReload )
{
    Super::onTemplateDelete(_hotReload);

    getResourceContainer()->unloadResources();
    releaseResourceContainer(_hotReload);
}

Vec2d FontTemplate::getTextSize(const String8 & _text, const char _constAdvanceChar_Size, const char _constAdvanceChar_Min, const char _constAdvanceChar_Max, const f32 _charSpacing) const
{
    f32 width = 0.f;
    f32 height = 0.f;

    const char * textStr = _text.cStr();

    f32 constAdvance = 0.f;
    if(_constAdvanceChar_Size!=0)
    {
        const Char* ch = getCharDesc(_constAdvanceChar_Size);

        if(ch)
        {
            constAdvance = f32(ch->m_advanceX);
        }
    }

    while(*textStr != 0)
    {
        i32 uniChar = i32(getNextUnicodeChar(textStr));

        const Char *charDesc = getCharDesc(uniChar);

        if( charDesc != 0x0 )
        {
            if(constAdvance!=0.f && uniChar>=_constAdvanceChar_Min && (_constAdvanceChar_Max==0 || uniChar<=_constAdvanceChar_Max))
            {
                width += (constAdvance + getOutlineThickness());
            }
            else
            {
                width += (charDesc->m_advanceX + getOutlineThickness());
            }
            width += _charSpacing;

            height = f32_Max(height, charDesc->m_height + charDesc->m_offsetY + getOutlineThickness());
        }
    }

    return Vec2d(width, height);
}

f32 FontTemplate::getTextWidth(const String8 & _text, const char _constAdvanceChar_Size, const char _constAdvanceChar_Min, const char _constAdvanceChar_Max, const f32 _charSpacing ) const
{
    f32 width = 0.0f;

    const char * textStr = _text.cStr();

    f32 constAdvance = 0;
    if(_constAdvanceChar_Size!=0)
    {
        const Char* ch = getCharDesc(_constAdvanceChar_Size);

        if(ch)
        {
            constAdvance = f32(ch->m_advanceX);
        }
    }

    while(*textStr != 0)
    {
        i32 uniChar = i32(getNextUnicodeChar(textStr));

        const Char *charDesc = getCharDesc(uniChar);

        if( charDesc != 0 )
        {
            if(constAdvance!=0 && uniChar>=_constAdvanceChar_Min && (_constAdvanceChar_Max==0 || uniChar<=_constAdvanceChar_Max))
            {
                width += (constAdvance + getOutlineThickness());
            }
            else
            {
                width += (charDesc->m_advanceX + getOutlineThickness());
            }
            width += _charSpacing;
        }
    }

    return width;
}

Vec2d FontTemplate::getScaledTextSize(const String8 & _text, const f32 _size, const char _constAdvanceChar_Size, const char _constAdvanceChar_Min, const char _constAdvanceChar_Max, const f32 _charSpacing) const
{
    return getTextSize(_text, _constAdvanceChar_Size, _constAdvanceChar_Min, _constAdvanceChar_Max, _charSpacing) * getScale(_size);
}

f32 FontTemplate::getScaledTextWidth(const String8 & _text, const f32 _size, const char _constAdvanceChar_Size, const char _constAdvanceChar_Min, const char _constAdvanceChar_Max, const f32 _charSpacing) const
{
    f32 width = getTextWidth(_text, _constAdvanceChar_Size, _constAdvanceChar_Min, _constAdvanceChar_Max, _charSpacing);
    f32 scale = getScale(_size);

    return width * scale;
}

f32 FontTemplate::getScale(f32 _size) const
{
    if( _size < 0)
    {
        return 1.0f;
    }

    return _size / m_common.m_lineHeight;
}

f32 FontTemplate::getScaledHeight(f32 _size) const
{
    return getScale(_size) * m_common.m_lineHeight;
}

i32 FontTemplate::getReplacementCharacter(i32 _uniCharId) const
{
    if (_uniCharId < 32) // do not replace control characters
        return _uniCharId;
    // Yen display fix
    if(_uniCharId == 65509)
        return 165;
    
    return i32(0x003F); // '?'
    //return i32(0xFFFD); // Replacement character
}

void  FontTemplate::truncate(String8 &_text, f32 _maxWidth, const String8 &_truncatedSymbol) const
{
    bbool truncated = bfalse;
    while(!_text.isEmpty() && getTextWidth(_text) > _maxWidth)
    {
        _text.truncate(_text.getLen() - 1);
        truncated = btrue;
    }

    if(truncated)
        _text += _truncatedSymbol;
}


const FontTemplate::Char* FontTemplate::getCharDesc(i32 _id) const
{
    ux * charIdx = m_charsDesc.find(_id);

    if (charIdx) 
    {
        return &m_chars[*charIdx];
    }
    else
    {
        charIdx = m_charsDesc.find(getReplacementCharacter(_id));
        return charIdx ? (&m_chars[*charIdx]) : (nullptr);
    }
}

} // namespace ITF

