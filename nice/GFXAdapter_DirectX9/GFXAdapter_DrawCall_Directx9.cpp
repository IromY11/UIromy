#include "precompiled_GFXAdapter_Directx9.h"

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_


namespace ITF
{
    ///----------------------------------------------------------------------------//
    /// Draw Vertex/IndexBuffer.
    ///----------------------------------------------------------------------------//

    void GFXAdapter_Directx9::SetDrawVertexBufferCommand(PRIMITIVETYPE _type, u32 _vertexStart, u32 _vertexNumber)
    {
        ITF_ASSERT(_vertexNumber);

        D3DPRIMITIVETYPE type;
        u32 numberprimitive = 0;

        switch(_type)
        {
        default:
        case GFX_TRIANGLES: 
            type = D3DPT_TRIANGLELIST;
            numberprimitive = _vertexNumber / 3;
            break;
        case GFX_POINTS:
            type = D3DPT_POINTLIST;
            numberprimitive = _vertexNumber;
            break;
        case GFX_LINES:
            type = D3DPT_LINELIST;
            numberprimitive = _vertexNumber / 2;
            break;
        case GFX_LINE_STRIP:
            type = D3DPT_LINESTRIP;
            numberprimitive = _vertexNumber - 1;
            break;
        case GFX_TRIANGLE_STRIP:
            type = D3DPT_TRIANGLESTRIP;
            numberprimitive = _vertexNumber - 2;
            break;
        case GFX_TRIANGLE_FAN:
            type = D3DPT_TRIANGLEFAN;
            numberprimitive = _vertexNumber - 2;
            break;
        }
        m_pd3dDevice->DrawPrimitive(type, _vertexStart, numberprimitive );
    }

    void GFXAdapter_Directx9::SetDrawIndexedVertexBufferCommand(PRIMITIVETYPE _type, u32 _indexNumber, u32 _startVertex )
    {
        D3DPRIMITIVETYPE type;
        u32 numberprimitive = 0;

        switch(_type)
        {
        default:
        case GFX_TRIANGLES: 
            type = D3DPT_TRIANGLELIST;
            numberprimitive = _indexNumber/ 3;
            break;
        case GFX_POINTS:
            type = D3DPT_POINTLIST;
            numberprimitive = _indexNumber;
            break;
        case GFX_LINES:
            type = D3DPT_LINELIST;
            numberprimitive = _indexNumber / 2;
            break;
        case GFX_LINE_STRIP:
            type = D3DPT_LINESTRIP;
            numberprimitive = _indexNumber - 1;
            break;
        case GFX_TRIANGLE_STRIP:
            type = D3DPT_TRIANGLESTRIP;
            numberprimitive = _indexNumber - 2;
            break;
        case GFX_TRIANGLE_FAN:
            type = D3DPT_TRIANGLEFAN;
            numberprimitive = _indexNumber - 2;
            break;
        }

        ITF_ASSERT(numberprimitive>0);
        if (numberprimitive>0)
            m_pd3dDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)_type, _startVertex, 0, m_drawCallStates.nbVertices, 0, numberprimitive );
    }

    void GFXAdapter_Directx9::SetDrawPrimitiveCommand(PRIMITIVETYPE _type, const void* _p_Vertex, u32 _NumberVertex)
    {
        D3DPRIMITIVETYPE type;
        u32 numberprimitive = 0;

        switch(_type)
        {
        default:
        case GFX_TRIANGLES: 
            type = D3DPT_TRIANGLELIST;
            numberprimitive = _NumberVertex / 3;
            break;
        case GFX_POINTS:
            type = D3DPT_POINTLIST;
            numberprimitive = _NumberVertex;
            break;
        case GFX_LINES:
            type = D3DPT_LINELIST;
            numberprimitive = _NumberVertex / 2;
            break;
        case GFX_LINE_STRIP:
            type = D3DPT_LINESTRIP;
            numberprimitive = _NumberVertex - 1;
            break;
        case GFX_TRIANGLE_STRIP:
            type = D3DPT_TRIANGLESTRIP;
            numberprimitive = _NumberVertex - 2;
            break;
        }

#ifdef ITF_WINDOWS
        m_pd3dDevice->SetStreamSourceFreq(0, 1);
        m_pd3dDevice->SetStreamSourceFreq(1, 1);
#endif //ITF_WINDOWS

        ITF_ASSERT(numberprimitive>0u && numberprimitive<0x80000000u);
        m_pd3dDevice->DrawPrimitiveUP(type, numberprimitive, _p_Vertex, m_drawCallStates.vertexSize);
    }


    void GFXAdapter_Directx9::SetDrawInstancedVertexBufferCommand(PRIMITIVETYPE _type, u32 _indexNumber, u32 _startVertex )
    {
        D3DPRIMITIVETYPE type;
        u32 numberprimitive = 0;

        switch(_type)
        {
        default:
        case GFX_TRIANGLES: 
            type = D3DPT_TRIANGLELIST;
            numberprimitive = _indexNumber/ 3;
            break;
        case GFX_POINTS:
            type = D3DPT_POINTLIST;
            numberprimitive = _indexNumber;
            break;
        case GFX_LINES:
            type = D3DPT_LINELIST;
            numberprimitive = _indexNumber / 2;
            break;
        case GFX_LINE_STRIP:
            type = D3DPT_LINESTRIP;
            numberprimitive = _indexNumber - 1;
            break;
        case GFX_TRIANGLE_STRIP:
            type = D3DPT_TRIANGLESTRIP;
            numberprimitive = _indexNumber - 2;
            break;
        case GFX_TRIANGLE_FAN:
            type = D3DPT_TRIANGLEFAN;
            numberprimitive = _indexNumber - 2;
            break;
        }

#ifdef ITF_WINDOWS
        ITF_ASSERT(numberprimitive>0);
        if (numberprimitive>0)
            m_pd3dDevice->DrawIndexedPrimitive(type, _startVertex, 0, m_drawCallStates.nbVertices, 0, numberprimitive );

        m_pd3dDevice->SetStreamSourceFreq(0, 1);
        m_pd3dDevice->SetStreamSourceFreq(1, 1);
#else

        m_pd3dDevice->DrawVertices(type, _startVertex, m_drawCallStates.nbVertices * _indexNumber );

#endif
    }
} // namespace ITF
