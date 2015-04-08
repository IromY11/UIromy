#include "precompiled_engine.h"

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef ITF_FINAL
# ifndef _ITF_FILE_MANAGER_LOGGER_ITF_H_
#  include "adapters/FileManager_ITF/FileManagerLogger_ITF.h"
# endif //_ITF_FILE_MANAGER_LOGGER_ITF_H_
#endif

#ifndef _ITF_PATH_H_
#include "core/file/Path.h"
#endif //_ITF_PATH_H_

#ifndef _ITF_COOKERCONTEXT_H_
    #include "engine/cooker/cookerContext.h"
#endif //_ITF_COOKERCONTEXT_H_

#ifndef _ITF_FILE_H_
# include "core/file/File.h"
#endif //_ITF_FILE_H_

namespace ITF
{
    const UVparameters* UVAtlas::getUvParameterByIndex(i32 _index) const
    {
        ITF_MAP<i32, UVparameters>::const_iterator iter = m_uvMapParameters.find(_index);

        if( iter == m_uvMapParameters.end() )
            return NULL;

        return &iter->second;
    }

    const UVdata& UVAtlas::getUVDatabyIndex(i32 _index) const
    {
        ITF_MAP<i32, UVdata>::const_iterator iter = m_uvMap.find(_index);
        
        ITF_ASSERT(iter != m_uvMap.end());
        
        if(iter != m_uvMap.end())
            return iter->second;
        else 
            return m_uvMap.begin()->second;// only for not crashing
    }

    const UVdata& UVAtlas::getUVDataAt(u32 _n) const
    {
        return m_uvMap.find(_n)->second;
    }

    bbool UVAtlas::isIndexValid(i32 _index) const
    {
        return m_uvMap.find(_index) != m_uvMap.end();
    }

    /// 0 -- 3
    /// |    |
    /// 1 -- 2
    void UVAtlas::get4UVbyIndex(i32 _index, Vec2d *m_UV) const
    {
        UVdata data;
        ITF_MAP<i32, UVdata>::const_iterator iter = m_uvMap.find(_index);
        if (iter != m_uvMap.end())
        {
            data = iter->second;
        }

        if (data.getUVs().size() == 4)
        {
            m_UV[0] = data.getUVs()[0];
            m_UV[1] = data.getUVs()[3];
            m_UV[2] = data.getUVs()[2];
            m_UV[3] = data.getUVs()[1];
        } else
        {
            m_UV[0] = data.getUV0();
            m_UV[1] = Vec2d(data.getUV0().x(), data.getUV1().y());
            m_UV[2] = data.getUV1();
            m_UV[3] = Vec2d(data.getUV1().x(), data.getUV0().y());
        }
    }

    /// 0 -- 3
    /// |    |
    /// 1 -- 2
    void UVAtlas::get4UVAt(i32 _n, Vec2d *m_UV) const
    {
        const UVdata& data = m_uvMap.find(_n)->second;
        if (data.getUVs().size() == 4)
        {
            m_UV[0] = data.getUVs()[0];
            m_UV[1] = data.getUVs()[3];
            m_UV[2] = data.getUVs()[2];
            m_UV[3] = data.getUVs()[1];
        } else
        {
            m_UV[0] = data.getUV0();
            m_UV[1] = Vec2d(data.getUV0().x(), data.getUV1().y());
            m_UV[2] = data.getUV1();
            m_UV[3] = Vec2d(data.getUV1().x(), data.getUV0().y());
        }
    }

    void UVAtlas::addUVdata(i32 _index, UVdata _data)
    {
        m_uvMap[_index] = _data;
    }

    void UVAtlas::addUVparameters(i32 _index, UVparameters _parameters)
    {
        m_uvMapParameters[_index] = _parameters;
    }

    void UVAtlas::removeUVdataAt(i32 _n)
    {
        ITF_MAP<i32, UVdata>::iterator iter = m_uvMap.find(_n);
        if (iter != m_uvMap.end())
        {
            iter->second;
        }
        m_uvMap.erase(_n);
    }

    void UVAtlas::removeUVdataByIndex(i32 _index)
    {
        ITF_MAP<i32, UVdata>::iterator itmap = m_uvMap.find(_index);
        ITF_ASSERT(itmap != m_uvMap.end());
        m_uvMap.erase(itmap);

        ITF_MAP<i32, UVparameters>::iterator itparameters = m_uvMapParameters.find(_index);
        if (itparameters!= m_uvMapParameters.end())
            m_uvMapParameters.erase(itparameters);
    }

    void UVAtlas::deleteAllUVdata()
    {
        m_uvMap.clear();
        m_uvMapParameters.clear();
    }

    bbool UVAtlas::serialize(ArchiveMemory& _pArchive)
    {
        ITF_VERIFY(Versioning::serializeVersion(_pArchive,Versioning::atlasVer()));
        m_uvMap.serialize(_pArchive);
        m_uvMapParameters.serialize(_pArchive);
        return (m_uvMap.size() || m_uvMapParameters.size());
    }


    bbool UVAtlas::openFile(UVAtlas& _atlas, const Path& _path)
    {
        File* atlasFile = FILEMANAGER->openFile(_path,ITF_FILE_ATTR_READ);

        if (atlasFile)
        {
            u32 fileSize = (u32)atlasFile->getLength();
            u8 * readBuffer = newAlloc(mId_Temporary,u8[fileSize+1]);
            u32 sizeRead = 0;
            bbool rdValid = atlasFile->read(readBuffer,fileSize,&sizeRead);

            if (rdValid)
            {
                ITF_ASSERT(fileSize == sizeRead);
                readBuffer[fileSize] = '\0';
                FILEMANAGER->closeFile(atlasFile);

                ArchiveMemory rd(readBuffer, fileSize);
                _atlas.serialize(rd);
            }
            else
            {
                SF_DEL_ARRAY(readBuffer);
                return 0;
            }

            SF_DEL_ARRAY(readBuffer);
        }
        else
        {
            return 0;
        }


        _atlas.setName(_path.getBasenameWithoutExtension());

        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UVAtlasManager::~UVAtlasManager()
    {

#ifdef ITF_SUPPORT_COOKING
        for (ITF_MAP<Path,const UVAtlas*>::iterator iter  = m_atlasCooked.begin();iter!=m_atlasCooked.end();++iter)
        {
            SF_DEL((*iter).second);   
        }
#endif //ITF_SUPPORT_COOKING
    }

    void UVAtlasManager::getCookedNamePlatform(const Path& _src, const Platform& _platform, Path& _atlName, Path& _cookedname)
    {
        //support naming convention error from the texture path
#ifdef ITF_SUPPORT_EDITOR
        if (_src.isFlagSet(Path::FlagBasenameTooLong | Path::FlagHasRelativePathRoot))
            return;
#endif //ITF_SUPPORT_EDITOR

        _atlName = _src.copyAndChangeBasename(_src.getBasenameWithoutExtension() + ".atl");
        _cookedname = FILESERVER->getCookedNamePlatform(_atlName,_platform,btrue);
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    void UVAtlasManager::loadDefault()
    {
#ifdef ITF_CONSOLE
        m_bContainerAccess = !FILEMANAGER->isRemote();
#else
        m_bContainerAccess = FILEMANAGER->isBundleSupported();
#endif //ITF_CONSOLE

        
        if (m_bContainerAccess)
        {
            load(Path("atlascontainer"));
        }
#ifndef ITF_FINAL
        else
        {
            // even if the atlascontainer is not used, do a dummy opened log to have a reference
            Path CookedName(FILESERVER->getCookedName(Path("atlascontainer")));
            FILEMANAGER->getLogger().LogOpenedFile(CookedName);
        }
#endif //ITF_FINAL
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_COOKING
    void UVAtlasManager::removeCookedAtlas(const Path& _atlasPath)
    {
        ITF_MAP<Path,const UVAtlas*>::iterator iter = m_atlasCooked.find(_atlasPath);
        if (iter!=m_atlasCooked.end())
        {
            SF_DEL((*iter).second);   
            m_atlasCooked.erase(_atlasPath);
        }
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------
    const UVAtlas* UVAtlasManager::getAtlasFromTextureFile(const Path& _texturePath)
    {
        Path atlasPath = _texturePath.copyAndChangeBasename(_texturePath.getBasenameWithoutExtension() + ".atl");

        ITF_MAP<Path,const UVAtlas*>::const_iterator iter = m_atlasCooked.find(atlasPath);
        if (iter!=m_atlasCooked.end())
            return (*iter).second; 
        
        COOKERCONTEXT_SETPATH(atlasPath);
        if (FILEMANAGER->fileExists(atlasPath))
        {
            const UVAtlas* uvAtlas = m_funcAtlasCookerForTools(atlasPath);
            if (uvAtlas)
                m_atlasCooked[atlasPath]= uvAtlas;

            return uvAtlas;
        }
        
        COOKERCONTEXT_ERROR("Binary frieze cooking expected an atlas but was missing");     
        return NULL;
    }
#endif //ITF_SUPPORT_COOKING
    //------------------------------------------------------------------------------------------------------------------------------------------------

    const UVAtlas* UVAtlasManager::getAtlas(const Path& filename)
    {

        if (m_bContainerAccess)
        {
            UVAtlasKey::iterator iter =  m_atlasKey.find(filename.getStringID());
            if (iter!=m_atlasKey.end())
                return  &(*iter).second;

            return NULL;

        }
        else
        {
            Path atlName;
            Path cookedFile;

            getCookedNamePlatform(filename,FILESERVER->getPlatform(),atlName,cookedFile);

            if (FILEMANAGER->fileExists(atlName))
            {
                bbool cookNeeded = RESOURCE_MANAGER->cookNeeded(atlName);
                if (cookNeeded)
                {
                    bbool isDelayed;
                    if(RESOURCE_MANAGER->requestCooking(atlName,isDelayed) == bfalse)
                        return NULL;
                }

                bbool exists = FILEMANAGER->fileExists(cookedFile);
                if (exists)
                {
                    UVAtlas atlas;
                    if (UVAtlas::openFile(atlas,cookedFile))
                    {
                        UVAtlasKey::iterator iterFound = m_atlasKey.find(filename.getStringID());

                        if (iterFound != m_atlasKey.end())
                        {
                            m_atlasKey.erase(iterFound);
                        }

                        ITF_PAIR<UVAtlasKey::iterator,bool> ret = m_atlasKey.insert(filename.getStringID(),atlas);
                        UVAtlasKey::iterator iter = ret.first;
                        m_atlasKey[filename.getStringID()] = atlas;
                        return &(*iter).second;

                    }

                    return NULL;
                }
            }            
        }


        return NULL;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    void UVAtlasManager::serialize(ArchiveMemory& _archive,UVAtlasKey& _atlasMap)
    { 
        u32 version = 0;

        _archive.serialize(version);
        _atlasMap.serialize(_archive);
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    void UVAtlasManager::load(const Path& filename, bbool _append)
    {
        if(!m_bContainerAccess)
            return;

        Path cookedName(FILESERVER->getCookedName(filename));
        File* pFile = FILEMANAGER->openFile(cookedName,ITF_FILE_ATTR_READ);
        ITF_ASSERT_MSG(pFile,"the UVAtlasManager file is missing %s",filename.toString8().cStr());
        if (pFile)
        {
            u32 lenght = (u32) pFile->getLength();
            u8* pBuffer = newAlloc(mId_Temporary,u8[lenght]);

            pFile->read(pBuffer,lenght);
            FILEMANAGER->closeFile(pFile);
            pFile = NULL;

            ArchiveMemory rd(pBuffer,lenght);
            SF_DEL_ARRAY(pBuffer);

            if(_append)
            {
                UVAtlasKey newKeys;
                serialize(rd, newKeys);

                for(UVAtlasKey::const_iterator it = newKeys.begin(); it != newKeys.end(); ++it)
                    m_atlasKey[it->first] = it->second;
            }
            else
                serialize(rd, m_atlasKey);
        }

    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef ITF_WINDOWS
    void UVAtlasManager::save(const Path& _filenameCooked,UVAtlasKey& _atlasMap)
    {
        ArchiveMemory arch;

        serialize(arch,_atlasMap);

        File* writeFile = FILEMANAGER->openFile(_filenameCooked,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

        if (writeFile)
        {
            writeFile->write(arch.getData(),arch.getSize());
            writeFile->flush();
            FILEMANAGER->closeFile(writeFile);
            writeFile = NULL;
        }

    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    void UVAtlasManager::build(const Path& _filename,const Platform& _platform,const ITF_VECTOR<Path>& _filesTexture)
    {
        UVAtlasKey localAtlasKey;

        Path atlName;
        Path cookedFile;

        for (ITF_VECTOR<Path> ::const_iterator iter= _filesTexture.begin();iter!=_filesTexture.end();iter++)
        {
            UVAtlas atlas;

            getCookedNamePlatform(*iter,_platform,atlName,cookedFile);
            if (FILEMANAGER->fileExists(atlName) && UVAtlas::openFile(atlas,cookedFile))
            {
                //m_atlasKey.insert()
                localAtlasKey[iter->getStringID()] = atlas;
            }

        }

        save(_filename,localAtlasKey);
    }

    void UVAtlasManager::merge(const Path& _outputFilename, const ITF_VECTOR<Path>& _inputFilenames)
    {
        UVAtlasKey globalAtlasKey;

        for (ITF_VECTOR<Path>::const_iterator iter = _inputFilenames.begin(); iter != _inputFilenames.end(); ++iter)
        {
            File* pFile = FILEMANAGER->openFile(*iter, ITF_FILE_ATTR_READ);
            if (pFile)
            {
                u32 length = (u32) pFile->getLength();
                u8* pBuffer = newAlloc(mId_Temporary, u8[length]);

                pFile->read(pBuffer, length);
                FILEMANAGER->closeFile(pFile);

                ArchiveMemory rd(pBuffer, length);
                SF_DEL_ARRAY(pBuffer);

                UVAtlasKey localAtlasKey;
                serialize(rd, localAtlasKey);

                for(UVAtlasKey::const_iterator atlasIt = localAtlasKey.begin(); atlasIt != localAtlasKey.end(); ++atlasIt)
                    globalAtlasKey[atlasIt->first] = atlasIt->second;
            }
        }

        save(_outputFilename, globalAtlasKey);
    }
#endif //ITF_WINDOWS


    //------------------------------------------------------------------------------------------------------------------------------------------------

}
