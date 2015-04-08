#ifndef _ITF_UVATLAS_H_
#define _ITF_UVATLAS_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_PLATFORM_H_
#include "core/Platform.h"
#endif //_ITF_PLATFORM_H_

#ifndef _ITF_UVDATA_H_
#include "engine/display/UVData.h"
#endif //_ITF_UVDATA_H_


namespace ITF
{

    class Path;

    class UVAtlas
    {
    public:

        UVAtlas()
        {
            m_atlasName = "noname";
        }

        UVAtlas(const String8& _name)
        {
            m_atlasName = _name;
        }

        ~UVAtlas()
        {
            m_uvMap.clear();
            m_uvMapParameters.clear();
        }


        const UVparameters* getUvParameterByIndex(i32 _index) const;
        const UVdata&  getUVDatabyIndex(i32 _index) const;
        const UVdata&  getUVDataAt(u32 _n) const;
        bbool isIndexValid(i32 _index) const;
        void get4UVbyIndex(i32 _index, Vec2d *m_UV) const;
        void get4UVAt(i32 _n, Vec2d *m_UV) const ;

        void addUVdata(i32 _index, UVdata _data);
        void addUVparameters(i32 _index, UVparameters _parameters);
        void removeUVdataByIndex(i32 _index);
        void removeUVdataAt(i32 _n);
        void deleteAllUVdata();
        static bbool openFile(UVAtlas& _atlas, const Path& _path);

        ITF_INLINE void setName(String8 _name) {m_atlasName = _name;};
        ITF_INLINE const String8& getName() const { return m_atlasName;};
        ITF_INLINE const u32 getNumberUV() const {return m_uvMap.size();}

        bbool serialize(ArchiveMemory& _pArchive);

    private:
        String8                      m_atlasName;
        ITF_MAP<i32, UVdata>        m_uvMap;
        ITF_MAP<i32, UVparameters>  m_uvMapParameters;
    };


    //-----------------------------------------------------------------------------------------------------------------


    class UVAtlasManager:public TemplateSingleton<UVAtlasManager>
    {
    public:

        typedef ITF_MAP<StringID,UVAtlas> UVAtlasKey;
        typedef UVAtlas* (*func_uvAtlasCookeForTools) (const Path& _texturePath); 
        UVAtlasManager():m_bContainerAccess(bfalse)
#ifdef ITF_SUPPORT_COOKING
            ,m_funcAtlasCookerForTools(NULL) 
#endif //ITF_SUPPORT_COOKING
        {};
        virtual ~UVAtlasManager();

        void serialize(ArchiveMemory& _archive,UVAtlasKey& _atlasMap);

        void getCookedNamePlatform(const Path& _src,const Platform& _platform, Path& _atlName, Path& _cookedname);
        void loadDefault();
        void load(const Path& _filename, bbool _append = bfalse);
        const UVAtlas* getAtlas(const Path& _filesTexture);
        
#ifdef ITF_SUPPORT_COOKING
        const UVAtlas* getAtlasFromTextureFile(const Path& _texturePath);
        void removeCookedAtlas(const Path& _atlasPath);
        void registerCallbackCookForTools(func_uvAtlasCookeForTools _func) {m_funcAtlasCookerForTools=_func;}//used by tools if you want to get atlas outside of the application
#endif //ITF_SUPPORT_COOKING
        

#ifdef ITF_WINDOWS
        void build(const Path& _filename,const Platform& _platform,const ITF_VECTOR<Path>& _files);
        void save(const Path& _filenameCooked,UVAtlasKey& _atlasMap);
        void merge(const Path& _outputFilename, const ITF_VECTOR<Path>& _inputFilenames);
#endif //ITF_WINDOWS

    protected:

#ifdef ITF_SUPPORT_COOKING
        ITF_MAP<Path,const UVAtlas*> m_atlasCooked;
        func_uvAtlasCookeForTools m_funcAtlasCookerForTools;
#endif //ITF_SUPPORT_COOKING
       

        UVAtlasKey m_atlasKey;
        bbool m_bContainerAccess;
    };


#define UVATLASMANAGER UVAtlasManager::getptr()
}



#endif //_ITF_UVATLAS_H_