#ifndef _ITF_OGLES_DRAWPRIM_H_
#define _ITF_OGLES_DRAWPRIM_H_

#ifdef ITF_IPAD

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif

#ifndef _ITF_RASTERSMANAGER_H_
#include "engine/rasters/rastersmanager.h"
#endif //_ITF_RASTERSMANAGER_H_

#include "PVRTVector.h"

namespace ITF
{
class GFXAdapter_OpenGLES : public GFXAdapter
{
	public:
		float			m_x, m_y;
		void			init();
		void			reshape();
		void			project(const Vec3d& _in3d, Vec2d & _out2d);
		void			coord2DTo3D(const Vec2d& _in2d, float _zPlane, Vec2d& _out3d, bool noTranslation = btrue);
		void			compute3DTo2D(const Vec3d& _in, Vec3d& _out);
		void			compute2DTo3D(const Vec3d& _in, Vec3d& _out);
		void			drawSphere(float _x, float _y, float _z, float r, int lats, int longs, int _wireFrame, float _r=1, float _g=1, float _b=1);
		void			drawCircle(float _x, float _y, float r, float _r=1, float _g=1, float _b=1, float _z = 0.f, u32 _segsCount = 32);
		void			drawBox(const Vec2d& _pos, f32 _angle, const Vec2d& _extent, f32 _r=1, f32 _g=1, f32 _b=1, float _z = 0.f, bbool _fill = bfalse);
		void			drawLine(const Vec2d& _p1, const Vec2d& _p2, float _r=1, float _g=1, float _b=1, float _size=1.f, float _z = 0.f);
		void			draw2dLine(const Vec2d& _p1, const Vec2d& _p2, float _size=1.f, u32 _color = COLOR_WHITE);
		void			draw2dBox(const Vec2d& _p, f32 _width, f32 _height, u32 _color0 = COLOR_WHITE, u32 _color1 = COLOR_WHITE, u32 _color2 = COLOR_WHITE, u32 _color3 = COLOR_WHITE, i32 _rank = 0);
		void			drawTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r=1, float _g=1, float _b=1, float _a=1, float _z = 0.f);
		void			drawTriangle(const Vertex& _v1, const Vertex& _v2, const Vertex& _v3, const Texture* _tex, bbool _showWireFrame = 0);
		void			drawQuad2D(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color = COLOR_WHITE, const Texture* _tex = NULL, const Vec2d* _uvStart = NULL,  const Vec2d* _uvEnd = NULL);
		void			draw3DQuad(GFX_3DQUAD* _t_Quad); 
		bbool			loadTexture(Texture* _tex, const char* _path, u32 _alhpa = 0);
		bbool			loadTextureCooked(Texture* _t, const char* _path);
		void			drawScreenQuad(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, bbool _noTex = 0);

		void			setCamera(Camera* _cam);
		void			init2DRender();
		void			init2DGuiRender();
		void			setOrthoView(f32 _l, f32 _r, f32 _b, f32 _t);

		void			CalcCoord(const Vec2d& _in2d, float _zPlane, Vec2d& _out3d);

		void			clear(u32 _buffer, u32 _Col = 0);
		void			clear(u32 _buffer, float _r=0.f, float _g=0.f, float _b=0.f, float _a=0.f);
		void			setAlphaBlend(GFX_BLENDMODE _blend);
		void			setCullMode(GFX_CULLMODE _cull);
		void			depthMask(u32 _depthMask);
		void			depthFunc(u32 _func);
		void			depthTest(u32 _test);
		void			setScissorRect(GFX_RECT* _clipRect);
		void			SetTextureBind(int _Sampler, unsigned int _Bind, bbool _linearFiltering = btrue);
		void			cleanupTexture(Texture* _texture);

		void			DrawPrimitive(PRIMITIVETYPE _type, const void *_p_Vertex, u32 _NumberVertex);

		void			setMatrixTransform(MATRIXMODE _type, const GMatrix44 *_Matrix);
		void			setupViewport(GFX_RECT* _Viewport);
		void			unprojectPoint(const Vec2d& _in, float _viewDistance, const GMatrix44 *_MatrixModel, const GMatrix44 *_MatrixProj, const int *_View, Vec2d& _out);
		void			setLookAtMatrix(GMatrix44* _MatrixOut, double _midx, double _midy, float _viewDistance, float _aspect);
		void			getViewport(GFX_RECT *_RectOut);

		/// GfxMaterials.
		void		    setGfxMaterial(const GFX_MATERIAL& _gfxMat);
		void			setGfxMatDefault();
		void			setGfxMatRefraction(const GFX_MATERIAL& _gfxMat);

		/// Texture.
		void			generateTexture(Texture* _texture);
		void			cleanupBaseTexture(Texture* _texture);
		void			setTextureSize(Texture* _texture, const Size& _size);
		unsigned char*	grabTexture(Texture* _texture);
		u32				getMaxTextureSize();
		void			loadFromMemory(const void* _buffer, const Size& _texture_size, Texture* _texture, Texture::PixFormat _pixel_format,const u32 _buffer_size,bbool bCooked);
		void			SetTexture(int _Sampler, Texture* _Texture, bbool _linearFiltering = btrue);
		void			lockTexture(Texture* _tex, LOCKED_TEXTURE* _lockTex, u32 _flag);
		void			unlockTexture(Texture* _tex);
		void			saveToMemory(void* _buffer, Texture* _Tex);
		float			getTexelOffset();
        void            setSamplerState( u32 _samp, GFX_SAMPLERSTATETYPE _type, i32 _value );

		void			clearRenderTarget(renderTarget* _target, u32 _color);
		void			initialiseRenderTarget(renderTarget* _target);
		void			resizeRenderTarget(renderTarget* _target, const Size& _size);
		void			enableRenderTarget(renderTarget* _target);
		void			disableRenderTarget(renderTarget* _target);
		void			cleanupRenderTarget(renderTarget* _target);
		bbool			isRenderTargetInverted();
		void			doScreenCapture();
		void			startScreenCapture();
		void			stopScreenCapture();
		void			setShaderForFont(bbool bOutline);
		void			restoreDefaultShader();

		void			present();
		void			startRendering();
		void			endRendering();

		bbool			isDeviceLost();
		bbool			resetDevice(bbool _forceReset);

		void			useHDWLightning(u32 _activate);

		void			setVertexFormat(u32 _vformat);

		/// shaders.
		void			createShader(ITF_shader* _shader);
		void			releaseShader(ITF_shader* _shader);
		bbool			loadShader(ITF_shader* _shader);
		void			beginShader(ITF_shader* _shader);
		void			endShader(ITF_shader* _shader);

		// Patch shader
		void			prepareShaderPatch(GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv);        
		bbool			shaderDrawPatch32(Texture *texture, Vec2d *PointsNS, ColorInteger* _lighttab);
		void			endShaderPatch();
		void			shaderPatchFlush(u32 _hdiv, u32 _vdiv, u32 _pass);

        // Fluid Shader
        void			drawFluid(   GMatrix44* _matrix, Texture* _texture, const FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT> &_colorTab, const Vec2d _heightTab[], const u32 _heightTabSize, const FixedArray<Vec2d, 4> &_uvTab, u32 mode, f32 _delta1, f32 _delta2, f32 _stepCount  );
		/// VertexBuffer.
		void			createVertexBuffer( ITF_VertexBuffer* _vertexBuffer);
		void			releaseVertexBuffer(ITF_VertexBuffer* _vertexBuffer);
		void			setVertexBuffer(ITF_VertexBuffer* _vertexBuffer);
		void			LockVertexBuffer(ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag);
		void			UnlockVertexBuffer(ITF_VertexBuffer* _vertexBuffer);

		/// IndexBuffer.
		void			createIndexBuffer(ITF_IndexBuffer* _indexBuffer);
		void			releaseIndexBuffer(ITF_IndexBuffer* _indexBuffer);
		void			setIndexBuffer(ITF_IndexBuffer* _indexBuffer);
		void			LockIndexBuffer(ITF_IndexBuffer* _indexBuffer, void** _data);
		void			UnlockIndexBuffer(ITF_IndexBuffer* _indexBuffer);

		/// Vertex/Index Buffer Draw.
		void			DrawVertexBuffer(u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber);
		void			DrawIndexedVertexBuffer(u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _indexNumber);
		void			drawMesh(ITF_Mesh& _mesh, bbool _showWireFrame);
#ifndef ITF_CONSOLE_FINAL
		void			drawFlatMesh(const ITF_Mesh& _mesh);
#endif // ITF_CONSOLE_FINAL
		void			drawMeshPT(const ITF_Mesh& _mesh, bbool _showWireFrame);

		/// After FX.
		void			copyCurrentColorBuffer(u32 _rt);
		void			copyTexture(int *TexHan);
		void			validateTexture(int *TexHan,int Mode,int ScreenSize,int ScreenSize_V);
		void			AFTERFX_BigBlur(f32 _factor, u32 _MaxPass, bbool _clearRT, GFX_QUALITY _quality);
		void			AFTERFX_Glow(f32 _factor, f32 _glowfactor, f32 _addalpha, GFX_BLENDMODE _finalBlendMode = GFX_BLEND_COPY);
        void            AFTERFX_objectsGlow(f32 _factor);
		void			AFTERFX_Remanence(f32 _factor, f32 _glowfactor, f32 _addalpha);
		void			AFTERFX_RayCenter(f32 _v1, f32 _v2, f32 _v3, f32 _v4, f32 _v5, f32 _v6, f32 _v7);
		void			AFTERFX_ColorSetting(f32 _sat, f32 _contrast, f32 _contrastscale, f32 _bright);
		void			AFTERFX_ColorRemap(Texture* _tex);
        void            AFTERFX_ColorLevels(f32 _inBlack,  f32 _inGamma, f32 _inWhite, f32 _outBlack, f32 _outWhite);
		void			AFTERFX_addSceneAndMul(f32 _mul);
		void			AFTERFX_byTech(u32 _tech);
		void			AFTERFX_byTechParams(u32 _tech, f32 _p0, f32 _p1, f32 _p2, f32 _p3);
		void			AFTERFX_draw(AFX_Group* _group);
		void			AFTERFX_startRenderTarget(Color _color);
        void            AFTERFX_prepareObjectGlow();
        void            prepareAfterFx(bbool _nocopy);
        void            endAfterfx(bbool _nocopy, GFX_BLENDMODE _finalBlend);

		void			startImpostorRendering(renderTarget* _rt, f32 xmin, f32 ymin, f32 xmax, f32 ymax);
		void			stopImpostorRendering(renderTarget* _rt);

		/// Surfaces.
		f32				ReadSurfaceFloat(void* _surf, u32 _posX, u32 _posY);

		/// UV.
		void			setUVAnim(const GFX_UVANIM& _uvanim);

		/// Render Context.
		void			renderContext_Set();
		void			renderContext_SetDefault();

		void			setGlobalColor(const Color& _color);

		const GMatrix44*		getViewMatrix();

		const String	getRenderName();
};

} // namespace ITF

#endif //ITF_IPAD

#endif // _ITF_OGLES_DRAWPRIM_H_
