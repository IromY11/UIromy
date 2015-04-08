#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_


namespace ITF
{
    static const f32 stepOverlay = 2.f;

    void Frise::buildFrieze_InOverlay( ITF_VECTOR<edgeFrieze>& _edgeList )
    {
        const FriseConfig* config = getConfig();
        if ( !config || !m_pRecomputeData->m_anim )
            return;

        i32 idTexConfig = getFirstNoFillTextureIndex( config ); 
        if ( idTexConfig == -1 )
            return;

        const FriseTextureConfig& texConfig = config->m_textureConfigs[idTexConfig];
        if ( !texConfig.hasDiffuseTexture()) 
            return;

        copyEdgeFromPolyline(_edgeList);
        buildEdgeList( _edgeList);

        if ( m_pRecomputeData->m_edgeListCount  == 0 )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        buildVB_InOverlay( config, texConfig, _edgeList );  

        buildMesh_Overlay( (u32)idTexConfig );
    }

    void Frise::buildVB_InOverlay( const FriseConfig* _config, const FriseTextureConfig& _texConfig, const ITF_VECTOR<edgeFrieze>& _edgeList )
    {        
        if ( !_texConfig.hasDiffuseTexture() )
            return;

        Texture* tex = _texConfig.getGFXMaterial().getTexture();
        if ( !tex )
            return;

        const UVAtlas *atlas = tex->getUVAtlas();
        if ( !atlas )
        {
#ifndef ITF_FINAL
            String8 msg("Overlay's atlas file is missing for : ");
            msg += tex->getPath().toString8().cStr();
            ITF_WARNING_CATEGORY(Graph,this, 0, msg.cStr());
#endif
            return;
        }

        const u32 uvDataCount = atlas->getNumberUV();
        if ( uvDataCount == 0 )
        {
#ifndef ITF_FINAL
            String8 msg("Overlay's atlas file is empty for : ");
            msg += tex->getPath().toString8().cStr();
            ITF_WARNING_CATEGORY(Graph,this, 0, msg.cStr());                
#endif
            return;
        }
             
        const f32 density = 1.f -Clamp( _config->m_density, 0.f, 1.f);
        u32 indexAnim = 0;

        Vec2d pos[4];
        Vec2d uv[4];
        u32 color[4];
        setVtxColor( _texConfig, color[1], color[0]); 
        color[2] = color[0];
        color[3] = color[1];

        for (u32 idEdge=0; idEdge< m_pRecomputeData->m_edgeListCount; idEdge++)
        {
            const edgeFrieze& edge = _edgeList[idEdge];

            if ( edge.m_snap || isEdgeWithHoleVisual(edge))
                continue;

            Seeder seed( (u32)(f32_Abs(edge.m_points[0].x())*edge.m_norm) );

            // get uvData
            u32 uvDataId = seed.GetU32( 0, uvDataCount -1);
            UVdata uvData = atlas->getUVDatabyIndex(uvDataId);
            f32 longUvDataX = uvData.getUV1().x() - uvData.getUV0().x();
            f32 longUvDataY = uvData.getUV1().y() - uvData.getUV0().y();
            f32 longOverlay = longUvDataX *_config->m_scale;
            f32 heightOverlay = longUvDataY *_config->m_scale;
            f32 uvXStart = longUvDataX *(f32)seed.GetU32( 0, 100);
            f32 uvXsens = seed.GetFloat(-10.f, 10.f);
            f32 heightOffset = m_pRecomputeData->m_heightScale *f32_Modulo(seed.GetFloat( 0.f, 10.f), 1.f);
            f32 startAccumulate = seed.GetFloat( 0.f, density) *stepOverlay;


            while ( startAccumulate + longOverlay <= edge.m_norm )
            {
                const Vec2d normalHeightOffset = edge.m_normal *heightOffset;
                pos[0] = edge.m_points[0] +edge.m_sightNormalized *startAccumulate +normalHeightOffset;
                pos[1] = pos[0] +edge.m_normal *heightOverlay;
                pos[2] = pos[0] +edge.m_sightNormalized*longOverlay;
                pos[3] = pos[2] +edge.m_normal *heightOverlay;

                Vec2d uv0 = uvData.getUV0();
                Vec2d uv1 = uvData.getUV1();
                uv0.x() += uvXStart;
                uv1.x() += uvXStart;
                if ( uvXsens <= 0.f )
                {
                    const f32 temp = uv0.x();
                    uv0.x() = uv1.x();
                    uv1.x() = temp;
                }

                uv[0] = Vec2d(uv0.x(), uv1.y());
                uv[1] = uv0;
                uv[2] = uv1;
                uv[3] = Vec2d(uv1.x(), uv0.y());

                buildVB_Overlay_Quad( &pos[0], &uv[0], &color[0], 1 +indexAnim );
                indexAnim = ( indexAnim +1) %8;

                startAccumulate += Max( longOverlay*0.5f, seed.GetFloat( 0.f, density) *stepOverlay );

                uvDataId = seed.GetU32( 0, 1000)%uvDataCount;
                uvData = atlas->getUVDatabyIndex(uvDataId);
                longUvDataX = uvData.getUV1().x() - uvData.getUV0().x();
                longUvDataY = uvData.getUV1().y() - uvData.getUV0().y();
                longOverlay = longUvDataX *_config->m_scale *seed.GetFloat(0.8f,1.2f);
                heightOverlay = longUvDataY *_config->m_scale;
                uvXStart = longUvDataX *(f32)seed.GetU32( 0, 100);
                uvXsens = seed.GetFloat(-10.f, 10.f);
                heightOffset = m_pRecomputeData->m_heightScale *f32_Modulo(seed.GetFloat( 0.f, 10.f), 1.f);
            }               
        }
    }
} // namespace ITF

