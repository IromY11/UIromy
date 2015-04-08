#include "precompiled_GFXAdapter_OpenGLES2.h"

#ifndef _ITF_OPENGLES2_DRAWPRIM_H_
#include "adapters/GFXAdapter_OpenGLES2/GFXAdapter_OpenGLES2.h"
#endif //_ITF_OPENGLES2_DRAWPRIM_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_

namespace ITF
{

	// ------------------------------------------------------------------------------
	// VertexBuffer.
	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::createVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
	{
#ifdef ITF_GLES2
        _vertexBuffer->mp_VertexBuffer = NULL;
        _vertexBuffer->mp_LockedBuffer = NULL;
        _vertexBuffer->m_lockedsize = 0;

		if ( Synchronize::getCurrentThreadId() == ThreadInfo::getGraphicThreadId() )
		{
			createVertexBufferGL(_vertexBuffer);
		}
		else
        {
            // VBO creation is pending
            _vertexBuffer->m_gl_VBOState = VB_T_VBO_REQUESTED; 
            AddPendingCreateVertexBuffer(_vertexBuffer);
        }

#endif
	}

    void GFXAdapter_OpenGLES2::removePendingVertexBufferCreation(ITF_VertexBuffer* _vertxb)
    {
        csAutoLock cs(m_csVBPend);

        const u32 countVBC = (u32)m_pendingCreateVerterBuffer.size();
        for (ux i = 0; i < countVBC; ++i)
        {
            if( m_pendingCreateVerterBuffer[i] == _vertxb )
            {
                m_pendingCreateVerterBuffer[i] = NULL;
            }
        }
    }

	void GFXAdapter_OpenGLES2::createVertexBufferGL(ITF_VertexBuffer* _vertexBuffer)
	{
		GL_CHECK( glGenBuffers(1, (GLuint*)&_vertexBuffer->mp_VertexBuffer) );
		GLenum glUsage = _vertexBuffer->bIsDynamic() ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;  
        
//		char* pBuffer = newAlloc(mId_Temporary, char[_vertexBuffer->m_size]);
		char* pBuffer = (char *)Memory::alignedMalloc(_vertexBuffer->m_size, 16);

                              
		GL_CHECK( glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_vertexBuffer->mp_VertexBuffer) );
		GL_CHECK( glBufferData(GL_ARRAY_BUFFER, _vertexBuffer->m_size, pBuffer, glUsage) );
        Memory::alignedFree( pBuffer );
        
        // VBO is actually created 
#ifdef ITF_GLES2
        _vertexBuffer->m_gl_VBOState = VB_T_VBO_CREATED; 
#endif
    }

    
    // ------------------------------------------------------------------------------
    
	void GFXAdapter_OpenGLES2::releaseVertexBufferGL( ITF_VertexBufferStruct &_vertexBuffer )
    {
		ITF_ASSERT(_vertexBuffer.mp_VertexBuffer);
		GL_CHECK( glDeleteBuffers(1, (GLuint*)&_vertexBuffer.mp_VertexBuffer) );
		_vertexBuffer.mp_VertexBuffer = 0;
#ifdef ITF_GLES2
		if (_vertexBuffer.mp_LockedBuffer)
		{
			//delete[] _vertexBuffer->mp_LockedBuffer;
            Memory::alignedFree( _vertexBuffer.mp_LockedBuffer );
			_vertexBuffer.mp_LockedBuffer = 0;
		}
#endif
    }
    
	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::releaseVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
	{
#ifdef ITF_GLES2
		// Deferred if not main thread.
		if ( Synchronize::getCurrentThreadId() == ThreadInfo::getGraphicThreadId() )
		{
            ITF_VertexBufferStruct VB(_vertexBuffer->mp_VertexBuffer,_vertexBuffer->mp_LockedBuffer);
			releaseVertexBufferGL(VB);
            _vertexBuffer->m_gl_VBOState = VB_T_VBO_NONE; 
		}
		else
        {
            _vertexBuffer->m_gl_VBOState = VB_T_VBO_PENDING_DELETE; // watch out, we can't notify when the delete is actually happening, as VB will be gone most of the time at this point
			AddPendingReleaseVertexBuffer(_vertexBuffer);
        }
#endif
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::LockVertexBuffer(ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag)
	{
		ITF_ASSERT_MSG(_size, "requesting 0 size");
	
#ifdef ITF_GLES2
		//ITF_ASSERT(_vertexBuffer->mp_LockedBuffer == 0);
		if ( _vertexBuffer->mp_LockedBuffer == NULL )
		{
            _vertexBuffer->mp_LockedBuffer = (char *)Memory::alignedMalloc(_vertexBuffer->m_size, 16);
			_vertexBuffer->m_lockedsize = 0;
		}
		if ( _vertexBuffer->m_lockedsize == 0 )
		{
			_vertexBuffer->m_lockedOffset = _offset;
			_vertexBuffer->m_lockedsize = _size;
		}
		else
		{
			// Merge with previous lock.
			u32 prevOffset = _vertexBuffer->m_lockedOffset;
			_vertexBuffer->m_lockedOffset = Min( prevOffset, _offset );
			_vertexBuffer->m_lockedsize = Max( _offset + _size, prevOffset + _vertexBuffer->m_lockedsize ) - _vertexBuffer->m_lockedOffset;
		}
		*_data = (void*)(_vertexBuffer->mp_LockedBuffer + _offset);
#endif	
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::UnlockVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
	{
		// Defered if not main thread.
		if ( Synchronize::getCurrentThreadId() == ThreadInfo::getGraphicThreadId() )
		{
			unlockVertexBufferGL(_vertexBuffer);
		}
#ifdef ITF_GLES2
		else
			AddPendingCreateUnlockVertexBuffer(_vertexBuffer);
#endif	
	}

    // ------------------------------------------------------------------------------
    
	void GFXAdapter_OpenGLES2::unlockVertexBufferGL(ITF_VertexBuffer* _vertexBuffer)
	{
		// Defered if not main thread.
#ifdef ITF_GLES2
        {
    
			if ( _vertexBuffer->m_lockedsize )
			{
				GL_CHECK( glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_vertexBuffer->mp_VertexBuffer) );
				GL_CHECK( glBufferSubData(GL_ARRAY_BUFFER, _vertexBuffer->m_lockedOffset, _vertexBuffer->m_lockedsize, _vertexBuffer->mp_LockedBuffer + _vertexBuffer->m_lockedOffset ) );
                
				_vertexBuffer->m_lockedsize = 0;
                /*
				if (_vertexBuffer->mp_LockedBuffer)
				{
					Memory::alignedFree( _vertexBuffer->mp_LockedBuffer );
					_vertexBuffer->mp_LockedBuffer = NULL;
				}
                 */
			}
		}

#endif	
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::UnlockVertexBuffer(ITF_VertexBuffer* _vertexBuffer, u32 _offset, u32 _size)
	{
#ifdef GLES2
		ITF_ASSERT(_size);

		GL_CHECK( glBindBuffer(GL_ARRAY_BUFFER, (GLuint)_vertexBuffer->mp_VertexBuffer) );
		GL_CHECK( glBufferSubData(GL_ARRAY_BUFFER, 0, mux_lockedSize, m_pLockedBuffer + _vertexBuffer->m_lockedOffset) );

		_vertexBuffer->m_lockedsize = 0;
#endif	
	}

	// ------------------------------------------------------------------------------
	// IndexBuffer.
	// ------------------------------------------------------------------------------
	void GFXAdapter_OpenGLES2::createIndexBuffer(ITF_IndexBuffer* _indexBuffer)
	{
#ifdef ITF_GLES2
        _indexBuffer->mp_LockedBuffer = NULL;
        _indexBuffer->mp_IndexBuffer = NULL;
#endif
        
		if ( Synchronize::getCurrentThreadId() == ThreadInfo::getGraphicThreadId() )
		{
			createIndexBufferGL(_indexBuffer);
		}
		else
        {
#ifdef ITF_GLES2
            _indexBuffer->m_gl_IBOState = IB_T_IBO_REQUESTED;
#endif

            AddPendingCreateIndexBuffer(_indexBuffer);
        }
	}

	void GFXAdapter_OpenGLES2::createIndexBufferGL(ITF_IndexBuffer* _indexBuffer)
	{
        //LOG( "%08x - %u, createIndexBufferGL Index Buffer ", _indexBuffer, _indexBuffer->m_counter );

		GL_CHECK( glGenBuffers(1, (GLuint*)&_indexBuffer->mp_IndexBuffer) );
		GLenum glUsage = _indexBuffer->bIsDynamic() ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;  
        
		u16* pBuffer = newAlloc(mId_Temporary,u16[_indexBuffer->m_nIndices]);

		GL_CHECK( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)_indexBuffer->mp_IndexBuffer) );
		GL_CHECK( glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer->m_nIndices*sizeof(u16), pBuffer, glUsage) );
        
        ITF_ASSERT(_indexBuffer->mp_IndexBuffer); // must be non null
        
		delete[] pBuffer;
#ifdef ITF_GLES2
        _indexBuffer->m_gl_IBOState = IB_T_IBO_CREATED;
#endif
	}

    
	// ------------------------------------------------------------------------------
	void GFXAdapter_OpenGLES2::releaseIndexBufferGL(ITF_IndexBufferStruct &_indexBuffer)
    {
		ITF_ASSERT(_indexBuffer.mp_IndexBuffer);
		GL_CHECK( glDeleteBuffers(1, (GLuint*)&_indexBuffer.mp_IndexBuffer) );
		_indexBuffer.mp_IndexBuffer = 0;
#ifdef ITF_GLES2
		if (_indexBuffer.mp_LockedBuffer)
		{
			delete[] _indexBuffer.mp_LockedBuffer;
			_indexBuffer.mp_LockedBuffer = NULL;
		}
#endif
    }
	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::releaseIndexBuffer(ITF_IndexBuffer* _indexBuffer)
	{
#ifdef ITF_GLES2
        if( _indexBuffer->m_gl_IBOState == IB_T_IBO_NONE )
        {
            // no need to release something not created (would crash)
            csAutoLock cs(m_csIBPend);

            const u32 countIBC = (u32)m_pendingCreateIndexBuffer.size();
            for (ux i = 0; i < countIBC; ++i)
            {
                if( m_pendingCreateIndexBuffer[i] == _indexBuffer )
                {
                    m_pendingCreateIndexBuffer[i] = NULL;

                }
            }
            return;
        }


		if ( Synchronize::getCurrentThreadId() == ThreadInfo::getGraphicThreadId() )
		{
            ITF_IndexBufferStruct IB(_indexBuffer->mp_IndexBuffer,_indexBuffer->mp_LockedBuffer);
			releaseIndexBufferGL(IB);
            _indexBuffer->m_gl_IBOState = IB_T_IBO_NONE;
		}
		else
        {
            _indexBuffer->m_gl_IBOState = IB_T_IBO_PENDING_DELETE;
			AddPendingReleaseIndexBuffer(_indexBuffer);
        }
#endif
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::LockIndexBuffer(ITF_IndexBuffer* _indexBuffer, void** _data)
	{
            
        //LOG( "%08x - %u, LOCK LOCK Index Buffer - count %u", _indexBuffer, _indexBuffer->m_counter, count );

        
        
#ifdef ITF_GLES2
		if ( _indexBuffer->mp_LockedBuffer == NULL )
		{
			_indexBuffer->mp_LockedBuffer = newAlloc(mId_Temporary, u16[_indexBuffer->m_nIndices]);
		}

		*_data = (void*)_indexBuffer->mp_LockedBuffer;
#endif	
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::UnlockIndexBuffer(ITF_IndexBuffer* _indexBuffer)
	{
        
		if ( Synchronize::getCurrentThreadId() == ThreadInfo::getGraphicThreadId() )
		{
			unlockIndexBufferGL(_indexBuffer);
		}
        else
		{
			AddPendingCreateUnlockIndexBuffer(_indexBuffer);
		}
	}

// ------------------------------------------------------------------------------

void GFXAdapter_OpenGLES2::unlockIndexBufferGL(ITF_IndexBuffer* _indexBuffer)
{
    //LOG( "%08x - %u : Unlock Index Buffer ", _indexBuffer, _indexBuffer->m_counter );

        ITF_ASSERT(_indexBuffer);

    
    
        ITF_ASSERT(_indexBuffer->mp_IndexBuffer);
#ifdef ITF_GLES2
        if ( _indexBuffer->mp_LockedBuffer )
        {
            //ITF_ASSERT(_indexBuffer->mp_LockedBuffer);
            GL_CHECK( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)_indexBuffer->mp_IndexBuffer) );
            
            u32 size = _indexBuffer->m_nIndices << 1;
            GL_CHECK( glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, _indexBuffer->mp_LockedBuffer) );
            
            delete [] _indexBuffer->mp_LockedBuffer;
            _indexBuffer->mp_LockedBuffer = NULL;
        }
#endif			
    

    }

} // namespace ITF
