#include "precompiled_engine.h"

#include "ShaderParameterDBDesc.h"

// class to manage the shader parameters (aka uniforms) provided by the engine.
// Use for shader validation purpose.
// On PS3 it's used to build the fragment shader DB (ShaderParameterDB)

namespace ITF
{

    ux ShaderParameterDBDesc::getNbRegForType(ParamType _type)
    {
        struct TypeNbReg
        {
            ShaderParameterDBDesc::ParamType paramType;
            ux nbReg;
        } 
        typeInfo[] =
        {
            {PARAM_TYPE_struct, 1}, // unknown -> array size has to be the total number of registers
            {PARAM_TYPE_float, 1},
            {PARAM_TYPE_float2, 1},
            {PARAM_TYPE_float3, 1},
            {PARAM_TYPE_float4, 1},
            {PARAM_TYPE_float4x4, 4},
            {PARAM_TYPE_BoneMatrix,3},
        };
        ITF_ASSERT(PARAM_TYPE_Count == sizeof(typeInfo)/sizeof(typeInfo[0]));
        
        for(ux i = 0; i<PARAM_TYPE_Count; ++i)
        {
            if (_type == typeInfo[i].paramType)
            {
                return typeInfo[i].nbReg;
            }
        }
        ITF_ASSERT(0);
        return 1;
    }


    void ShaderParameterDBDesc::registerParameter(ShaderParameterDBDesc::ParamType _type, const char * _name, ux _firstReg)
    {
        // if _type == PARAM_TYPE_struct, sizeof the struct is assumed the same as float4
        ux nbReg = getNbRegForType(_type);
        m_paramDesc.resize(m_paramDesc.size()+1);
        ParamDesc & paramDesc = m_paramDesc.back();
        paramDesc.name = _name;
        paramDesc.firstReg = _firstReg;
        paramDesc.nbReg = nbReg;
        paramDesc.type = _type;
        paramDesc.isArray = bfalse;
        paramDesc.isStruct = bfalse;
    }

    void ShaderParameterDBDesc::registerParameterStruct(const char * _name, ux _firstReg, ux _nbReg)
    {
        m_paramDesc.resize(m_paramDesc.size()+1);
        ParamDesc & paramDesc = m_paramDesc.back();
        paramDesc.name = _name;
        paramDesc.firstReg = _firstReg;
        paramDesc.nbReg = _nbReg;
        paramDesc.type = PARAM_TYPE_struct;
        paramDesc.isArray = bfalse;
        paramDesc.isStruct = btrue;
    }

    void ShaderParameterDBDesc::registerParameterArray(ShaderParameterDBDesc::ParamType _type, const char * _name, ux _firstReg, ux _arraySize) // handle any type
    {
        ux nbRegPerElement = getNbRegForType(_type);
        m_paramDesc.resize(m_paramDesc.size()+1);
        ParamDesc & paramDesc = m_paramDesc.back();
        paramDesc.name = _name;
        paramDesc.firstReg = _firstReg;
        paramDesc.nbReg = nbRegPerElement * _arraySize;
        paramDesc.type = _type;
        paramDesc.isArray = btrue;
        paramDesc.isStruct = bfalse;
    }

    ShaderParameterDBDesc::const_iterator ShaderParameterDBDesc::find(const char * _name) const
    {
        for(vector<ParamDesc>::const_iterator it = m_paramDesc.begin(), itEnd = m_paramDesc.end();
            it!=itEnd;
            ++it)
        {
            if (it->name == _name)
            {
                return it;
            }
        }
        return end();
    }
}
