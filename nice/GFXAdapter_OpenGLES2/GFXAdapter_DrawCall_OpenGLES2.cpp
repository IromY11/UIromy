#include "precompiled_GFXAdapter_OpenGLES2.h"
#ifdef ITF_GLES2

#ifndef _ITF_OPENGLES2_DRAWPRIM_H_
#include "adapters/GFXAdapter_OpenGLES2/GFXAdapter_OpenGLES2.h"
#endif //_ITF_OPENGLES2_DRAWPRIM_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

// This file is limited to drawcall relative methods

namespace ITF
{
	// ------------------------------------------------------------------------------
	// A full draw should calls, in any order:
    // * setShaderGroup(group)
    // * setVertexShaderVariants() -> optionnal if m_drawCallState has been resetted
    // * setPixelShaderVariants() -> optionnal if m_drawCallState has been resetted
    // * setIndexBuffer -> if using index buffer
    // * setVertexBuffer or setVertexFormat -> setVertexFormat is when calling DrawPrimitive
    // * setGfxMaterial
    // Followed by any of the 3 following drawcall methods :
    //      DrawIndexedVertexBuffer
    //      DrawVertexBuffer
    //      DrawPrimitive
    // These methods should contain:
    //      beginDrawcall
    //      GFX plateform specific API drawcall command
    //      endDrawCall
	// ------------------------------------------------------------------------------

	bool GFXAdapter_OpenGLES2::beginDrawCall(DrawCallContext &_drawCallCtx)
    {
		bool renderOK = Super::beginDrawCall(_drawCallCtx);
		if ( !renderOK )
			return false;

        
#ifdef _HARDCODED_SHADER
			float afIdentity[] = {
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1
			};

        flushSetTextures();

		glUseProgram(gProgram[m_curProg]);
		int i32Location = glGetUniformLocation(gProgram[m_curProg], "myPMVMatrix");

		// Then passes the matrix to that variable
		f32* matrix = (f32*)&m_worldViewProj.getWorldViewProj();
		//glUniformMatrix4fv(i32Location, 1, GL_FALSE, /*matrix*/afIdentity);
		glUniformMatrix4fv(i32Location, 1, GL_FALSE, matrix);
#else
        ITF_ASSERT(m_CurrentVertexShader);
        ITF_ASSERT(m_CurrentPixelShader);
        
		GLES2Program *program = getProgramFromShaders(m_CurrentVertexShader, m_CurrentPixelShader);
		program->flushStates(this);
#endif


#ifdef CHECK_RUNTIME_SHADER
        CheckRuntimeShader();
#endif // CHECK_RUNTIME_SHADER

		return true;
	}

 	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::SetDrawVertexBufferCommand(PRIMITIVETYPE _type, u32 _vertexStart, u32 _vertexNumber)
    {
		ITF_ASSERT(_vertexNumber);

		glDrawArrays
        (
            PrimitiveToGLES2((PRIMITIVETYPE)_type),
            _vertexStart,
           _vertexNumber
        );
    }

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::SetDrawIndexedVertexBufferCommand(PRIMITIVETYPE _type, u32 _indexNumber, u32 _startVertex )
    {
		glDrawElements
		(
			PrimitiveToGLES2((PRIMITIVETYPE)_type),
			_indexNumber,
			GL_UNSIGNED_SHORT,
			(const GLvoid*)(sizeof(u16)*_startVertex)
		);
	}

	// ------------------------------------------------------------------------------
    // void GFXAdapter_Cafe::SetDrawPrimitiveCommand(PRIMITIVETYPE _type, const void* _p_Vertex, u32 _NumberVertex);
    // DrawPrimitive has been overloaded from the GFXAdapter Common, so this method should not be called, and so on needed at linked
    // But it is requested to be declared in the class to compile.
	// ------------------------------------------------------------------------------

    void GFXAdapter_OpenGLES2::DrawPrimitive(DrawCallContext &_drawCallCtx, PRIMITIVETYPE _type, const void* _p_Vertex, u32 _NumberVertex)
    {
		if(_NumberVertex)
        {
            ITF_VertexBuffer localVBfr;
            localVBfr.initVertexBuffer(_NumberVertex, m_drawCallStates.vertexFormat, m_drawCallStates.vertexSize, btrue);
            localVBfr.m_debugType = VB_T_INTERNAL;
/*
#ifndef ITF_FINAL
            localVBfr.m_bUseRingBuffer = btrue;
#endif

			void *data;
            getVertexBufferManager().LockVertexBuffer(&localVBfr, &data);
            ITF_ASSERT(data);
            ITF_Memcpy(data, _p_Vertex, localVBfr.m_size);
            getVertexBufferManager().UnlockVertexBuffer(&localVBfr);
*/
			localVBfr.m_offset = (uPtr)_p_Vertex;
			localVBfr.mp_VertexBuffer = 0;
			setVertexBuffer(&localVBfr);
			
            beginDrawCall(_drawCallCtx);
			glDrawArrays(PrimitiveToGLES2(_type), 0, _NumberVertex);

            endDrawCall(_drawCallCtx);

            // avoid vb destruction by object destructor.
            localVBfr.mp_VertexBuffer = NULL;
        }
    }

    void GFXAdapter_OpenGLES2::SetDrawInstancedVertexBufferCommand(PRIMITIVETYPE _type, u32 _indexNumber, u32 _startVertex)
    {
        u32 nbInstance =  m_drawCallStates.vertexBuffer->m_nbInstance;

		glDrawElements
        (
            PrimitiveToGLES2((PRIMITIVETYPE)_type),
            _indexNumber * nbInstance,
            GL_UNSIGNED_SHORT,
            (const GLvoid*)(sizeof(u16)*_startVertex)
        );
    }
} // namespace ITF
#endif // ITF_GLES2