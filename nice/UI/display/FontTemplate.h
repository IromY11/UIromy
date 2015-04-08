#ifndef _ITF_FONTTEMPLATE_H_
#define _ITF_FONTTEMPLATE_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

namespace ITF
{

#define FONT_INITIAL_SIZE 150.0f

class FontTemplate : public TemplateObj
{
    DECLARE_OBJECT_CHILD_RTTI(FontTemplate, TemplateObj,1127877782)
    DECLARE_SERIALIZE()

    struct Char
    {
        DECLARE_SERIALIZE()

        Char()
            : m_id(-1)
            , m_x(-1)
            , m_y(-1)
            , m_width(-1)
            , m_height(-1)
            , m_offsetX(-1)
            , m_offsetY(-1)
            , m_advanceX(-1)
            , m_page(-1)
            , m_channel(-1)
        {
        }

        i32                         m_id;
        i32                         m_x;
        i32                         m_y;
        i32                         m_width;
        i32                         m_height;
        i32                         m_offsetX;
        i32                         m_offsetY;
        i32                         m_advanceX;
        i32                         m_page;
        i32                         m_channel;
    };

    struct Info
    {
        DECLARE_SERIALIZE()

        Info()
            : m_size(-1)
            , m_bold(bfalse)
            , m_italic(bfalse)
            , m_unicode(bfalse)
            , m_stretchH(-1)
            , m_smooth(bfalse)
            , m_antiAlias(bfalse)
            , m_paddingLeft(U32_INVALID)
            , m_paddingRight(U32_INVALID)
            , m_paddingTop(U32_INVALID)
            , m_paddingBottom(U32_INVALID)
            , m_spacingLeft(U32_INVALID)
            , m_spacingTop(U32_INVALID)
            , m_outline(U32_INVALID)
        {
        }

        String8                     m_face;
        i32                         m_size;
        bbool                       m_bold;
        bbool                       m_italic;
        String8                     m_charset;
        bbool                       m_unicode;
        i32                         m_stretchH;
        bbool                       m_smooth;
        bbool                       m_antiAlias;
        u32                         m_paddingLeft;
        u32                         m_paddingRight;
        u32                         m_paddingTop;
        u32                         m_paddingBottom;
        u32                         m_spacingLeft;
        u32                         m_spacingTop;
        u32                         m_outline;
    };

    struct Common
    {
        DECLARE_SERIALIZE()

        Common()
            : m_lineHeight(-1)
            , m_base(-1)
            , m_scaleW(-1)
            , m_scaleH(-1)
            , m_pages(-1)
            , m_packed(0)
            , m_alphaChannel(-1)
            , m_redChannel(-1)
            , m_greenChannel(-1)
            , m_blueChannel(-1)
        {
        }

        i32                         m_lineHeight;
        i32                         m_base;
        i32                         m_scaleW;
        i32                         m_scaleH;
        i32                         m_pages;
        bbool                       m_packed;
        i32                         m_alphaChannel;
        i32                         m_redChannel;
        i32                         m_greenChannel;
        i32                         m_blueChannel;
    };

    struct Page
    {
        DECLARE_SERIALIZE()

        Page()
            : m_id(-1)
        {
        }

        i32                         m_id;
        Path                        m_texture;
    };

public:

    FontTemplate() {}
    ~FontTemplate() {}

    virtual bbool                   onTemplateLoaded( bbool _hotReload );
    virtual void                    onTemplateDelete( bbool _hotReload );

    ITF_INLINE f32                  getHeight() const {return f32(m_common.m_lineHeight);}
    ITF_INLINE f32                  getBottomOffset() const	{return f32(m_common.m_base - m_common.m_lineHeight);}
    ITF_INLINE f32                  getTopOffset() const {return f32(m_common.m_base);}
    ITF_INLINE f32                  getOutlineThickness() const {return f32(m_info.m_outline);}
    ITF_INLINE f32                  getSize() const {return f32(m_info.m_size);}
    
    ITF_INLINE i32                  getScaleW() const {return m_common.m_scaleW; }
    ITF_INLINE i32                  getScaleH() const {return m_common.m_scaleH; }
    ITF_INLINE i32                  getPagesCount() const {return m_textures.size();}
    ITF_INLINE ResourceID           getPageResourceID(i32 _page) const {return m_textures[_page];}

    const Char*                     getCharDesc(i32 _id) const;
    Vec2d                           getTextSize(const String8 & text, const char _constAdvanceChar_Size = '\0', const char _constAdvanceChar_Min = '\0', const char _constAdvanceChar_Max = '\0', const f32 _charSpacing = 0.0f) const;
    f32                             getTextWidth(const String8 & text, const char _constAdvanceChar_Size = '\0', const char _constAdvanceChar_Min = '\0', const char _constAdvanceChar_Max = '\0', const f32 _charSpacing = 0.0f) const;

    f32                             getScale(f32 _size) const;
    Vec2d                           getScaledTextSize(const String8 & _text, const f32 _size, const char _constAdvanceChar_Size = '\0', const char _constAdvanceChar_Min = '\0', const char _constAdvanceChar_Max = '\0', const f32 _charSpacing = 0.0f) const;
    f32                             getScaledTextWidth(const String8 & _text, const f32 _size, const char _constAdvanceChar_Size = '\0', const char _constAdvanceChar_Min = '\0', const char _constAdvanceChar_Max = '\0', const f32 _charSpacing = 0.0f) const;
    f32                             getScaledHeight(f32 _size) const;

    void                            truncate(String8 &_text, f32 _maxWidth, const String8 &_truncatedSymbol) const;

private:

    i32                             getReplacementCharacter(i32 _uniCharId) const;

    Info                            m_info;
    Common                          m_common;
    ITF_VECTOR <Page>               m_pages;
    ITF_VECTOR <Char>               m_chars;
    ITF_VECTOR <ResourceID>         m_textures;
    ITF_HASHMAP<i32, ux>            m_charsDesc; // map unicode char to m_chars id
};

} // namespace ITF

#endif // _ITF_FONTTEMPLATE_H_
