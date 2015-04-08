#ifndef ITF_SHADERPARAMETERDBDESC_H_
#define ITF_SHADERPARAMETERDBDESC_H_

// class to manage the shader parameters (aka uniforms) provided by the engine.
// Use for shader validation purpose.
// On PS3 it's used to build the fragment shader DB (ShaderParameterDB)

namespace ITF
{
    class ShaderParameterDBDesc
    {
    public:

        enum ParamType
        {
            PARAM_TYPE_struct,
            PARAM_TYPE_float,
            PARAM_TYPE_float2,
            PARAM_TYPE_float3,
            PARAM_TYPE_float4,
            PARAM_TYPE_float4x4,
            PARAM_TYPE_BoneMatrix,
            PARAM_TYPE_Count,
        };

        void registerParameter(ParamType _type, const char * _name, ux _firstReg); // if _type == PARAM_TYPE_struct, sizeof the struct is assumed the same as float4
        void registerParameterStruct(const char * _name, ux _firstReg, ux _nbReg);
        void registerParameterArray(ParamType _type, const char * _name, ux _firstReg, ux _arraySize); // handle any type

        // access to information for shader parameter checks
        struct ParamDesc
        {
            String8         name;
            ux              firstReg;
            ux              nbReg;
            ParamType       type;
            bbool           isArray;
            bbool           isStruct;
        };

        typedef vector<ParamDesc>::const_iterator const_iterator;

        const_iterator begin() const { return m_paramDesc.begin(); }
        const_iterator end() const { return m_paramDesc.end(); }

        const_iterator find(const char * _name) const;

    private:
        ux getNbRegForType(ParamType _type);

        vector<ParamDesc> m_paramDesc;
    };
}

#endif // ITF_SHADERPARAMETERDBDESC_H_
