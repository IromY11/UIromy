#include "precompiled_GFXAdapter_OpenGLES.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////       implementation of various shapes drawing
///////////////////////////////////////////////////////////////////////////////

#include <math.h>

#ifndef _ITF_OGL_DRAWPRIM_H_
#include "adapters/GFXAdapter_OpenGLES/GFXAdapter_OpenGLES.h"
#endif //_ITF_OGL_DRAWPRIM_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_
#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_
#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif //_ITF_GFXADAPTER_RENDERTARGET_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_DDSFORMAT_H_
#include "engine/display/ddsformat.h"
#endif //_ITF_DDSFORMAT_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))


#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#define TRACK_FUNCTION_CALLS 0

#if TRACK_FUNCTION_CALLS
	#define TRACK_FUNCTION_CALL printf("%.3lf: GFXAdapter_OpenGLES::%s\n", CFAbsoluteTimeGetCurrent(), __FUNCTION__);
#else
	#define TRACK_FUNCTION_CALL
#endif

static void MakePerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	GLfloat top = zNear * tanf(fovy);
	GLfloat right = top * aspect;
	glFrustumf(-right, right, -top, top, zNear, zFar);
	
	glRotatef(-90.f, 0.f, 0.f, 1.f);
	glMatrixMode(GL_MODELVIEW);
}

static int glhProjectf(float objx, float objy, float objz, float *modelview, float *projection, int *viewport, float *windowCoordinate);
static int glhUnProjectf(float winx, float winy, float winz, float *modelview, float *projection, int *viewport, float *objectCoordinate);
static void MultiplyMatrices4by4OpenGL_FLOAT(float *result, float *matrix1, float *matrix2);
static void MultiplyMatrixByVector4by4OpenGL_FLOAT(float *resultvector, const float *matrix, const float *pvector);
static int glhInvertMatrixf2(float *m, float *out);

static int glhProjectf(float objx, float objy, float objz, float *modelview, float *projection, int *viewport, float *windowCoordinate)
{
	//Transformation vectors
	float fTempo[8];
	//Modelview transform
	fTempo[0]=modelview[0]*objx+modelview[4]*objy+modelview[8]*objz+modelview[12];  //w is always 1
	fTempo[1]=modelview[1]*objx+modelview[5]*objy+modelview[9]*objz+modelview[13];
	fTempo[2]=modelview[2]*objx+modelview[6]*objy+modelview[10]*objz+modelview[14];
	fTempo[3]=modelview[3]*objx+modelview[7]*objy+modelview[11]*objz+modelview[15];
	//Projection transform, the final row of projection matrix is always [0 0 -1 0]
	//so we optimize for that.
	fTempo[4]=projection[0]*fTempo[0]+projection[4]*fTempo[1]+projection[8]*fTempo[2]+projection[12]*fTempo[3];
	fTempo[5]=projection[1]*fTempo[0]+projection[5]*fTempo[1]+projection[9]*fTempo[2]+projection[13]*fTempo[3];
	fTempo[6]=projection[2]*fTempo[0]+projection[6]*fTempo[1]+projection[10]*fTempo[2]+projection[14]*fTempo[3];
	fTempo[7]=-fTempo[2];
	//The result normalizes between -1 and 1
	if(fTempo[7]==0.0)	//The w value
		return 0;
	fTempo[7]=1.0/fTempo[7];
	//Perspective division
	fTempo[4]*=fTempo[7];
	fTempo[5]*=fTempo[7];
	fTempo[6]*=fTempo[7];
	//Window coordinates
	//Map x, y to range 0-1
	windowCoordinate[0]=(fTempo[4]*0.5+0.5)*viewport[2]+viewport[0];
	windowCoordinate[1]=(fTempo[5]*0.5+0.5)*viewport[3]+viewport[1];
	//This is only correct when glDepthRange(0.0, 1.0)
	windowCoordinate[2]=(1.0+fTempo[6])*0.5;	//Between 0 and 1
	return 1;
}

static int glhUnProjectf(float winx, float winy, float winz, float *modelview, float *projection, int *viewport, float *objectCoordinate)
{
	//Transformation matrices
	float m[16], A[16];
	float in[4], out[4];
	//Calculation for inverting a matrix, compute projection x modelview
	//and store in A[16]
	MultiplyMatrices4by4OpenGL_FLOAT(A, projection, modelview);
	//Now compute the inverse of matrix A
	if(glhInvertMatrixf2(A, m)==0)
		return 0;
	//Transformation of normalized coordinates between -1 and 1
	in[0]=(winx-(float)viewport[0])/(float)viewport[2]*2.0-1.0;
	in[1]=(winy-(float)viewport[1])/(float)viewport[3]*2.0-1.0;
	in[2]=2.0*winz-1.0;
	in[3]=1.0;
	//Objects coordinates
	MultiplyMatrixByVector4by4OpenGL_FLOAT(out, m, in);
	if(out[3]==0.0)
		return 0;
	out[3]=1.0/out[3];
	objectCoordinate[0]=out[0]*out[3];
	objectCoordinate[1]=out[1]*out[3];
	objectCoordinate[2]=out[2]*out[3];
	return 1;
}

static void MultiplyMatrices4by4OpenGL_FLOAT(float *result, float *matrix1, float *matrix2)
{
    result[0]=matrix1[0]*matrix2[0]+
	matrix1[4]*matrix2[1]+
	matrix1[8]*matrix2[2]+
	matrix1[12]*matrix2[3];
    result[4]=matrix1[0]*matrix2[4]+
	matrix1[4]*matrix2[5]+
	matrix1[8]*matrix2[6]+
	matrix1[12]*matrix2[7];
    result[8]=matrix1[0]*matrix2[8]+
	matrix1[4]*matrix2[9]+
	matrix1[8]*matrix2[10]+
	matrix1[12]*matrix2[11];
    result[12]=matrix1[0]*matrix2[12]+
	matrix1[4]*matrix2[13]+
	matrix1[8]*matrix2[14]+
	matrix1[12]*matrix2[15];
    result[1]=matrix1[1]*matrix2[0]+
	matrix1[5]*matrix2[1]+
	matrix1[9]*matrix2[2]+
	matrix1[13]*matrix2[3];
    result[5]=matrix1[1]*matrix2[4]+
	matrix1[5]*matrix2[5]+
	matrix1[9]*matrix2[6]+
	matrix1[13]*matrix2[7];
    result[9]=matrix1[1]*matrix2[8]+
	matrix1[5]*matrix2[9]+
	matrix1[9]*matrix2[10]+
	matrix1[13]*matrix2[11];
    result[13]=matrix1[1]*matrix2[12]+
	matrix1[5]*matrix2[13]+
	matrix1[9]*matrix2[14]+
	matrix1[13]*matrix2[15];
    result[2]=matrix1[2]*matrix2[0]+
	matrix1[6]*matrix2[1]+
	matrix1[10]*matrix2[2]+
	matrix1[14]*matrix2[3];
    result[6]=matrix1[2]*matrix2[4]+
	matrix1[6]*matrix2[5]+
	matrix1[10]*matrix2[6]+
	matrix1[14]*matrix2[7];
    result[10]=matrix1[2]*matrix2[8]+
	matrix1[6]*matrix2[9]+
	matrix1[10]*matrix2[10]+
	matrix1[14]*matrix2[11];
    result[14]=matrix1[2]*matrix2[12]+
	matrix1[6]*matrix2[13]+
	matrix1[10]*matrix2[14]+
	matrix1[14]*matrix2[15];
    result[3]=matrix1[3]*matrix2[0]+
	matrix1[7]*matrix2[1]+
	matrix1[11]*matrix2[2]+
	matrix1[15]*matrix2[3];
    result[7]=matrix1[3]*matrix2[4]+
	matrix1[7]*matrix2[5]+
	matrix1[11]*matrix2[6]+
	matrix1[15]*matrix2[7];
    result[11]=matrix1[3]*matrix2[8]+
	matrix1[7]*matrix2[9]+
	matrix1[11]*matrix2[10]+
	matrix1[15]*matrix2[11];
    result[15]=matrix1[3]*matrix2[12]+
	matrix1[7]*matrix2[13]+
	matrix1[11]*matrix2[14]+
	matrix1[15]*matrix2[15];
}

static void MultiplyMatrixByVector4by4OpenGL_FLOAT(float *resultvector, const float *matrix, const float *pvector)
{
    resultvector[0]=matrix[0]*pvector[0]+matrix[4]*pvector[1]+matrix[8]*pvector[2]+matrix[12]*pvector[3];
    resultvector[1]=matrix[1]*pvector[0]+matrix[5]*pvector[1]+matrix[9]*pvector[2]+matrix[13]*pvector[3];
    resultvector[2]=matrix[2]*pvector[0]+matrix[6]*pvector[1]+matrix[10]*pvector[2]+matrix[14]*pvector[3];
    resultvector[3]=matrix[3]*pvector[0]+matrix[7]*pvector[1]+matrix[11]*pvector[2]+matrix[15]*pvector[3];
}

#define SWAP_ROWS_DOUBLE(a, b) { double *_tmp = a; (a)=(b); (b)=_tmp; }
#define SWAP_ROWS_FLOAT(a, b) { float *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]

//This code comes directly from GLU except that it is for float
static int glhInvertMatrixf2(float *m, float *out)
{
	float wtmp[4][8];
	float m0, m1, m2, m3, s;
	float *r0, *r1, *r2, *r3;
	r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];
	r0[0] = MAT(m, 0, 0), r0[1] = MAT(m, 0, 1),
	r0[2] = MAT(m, 0, 2), r0[3] = MAT(m, 0, 3),
	r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,
	r1[0] = MAT(m, 1, 0), r1[1] = MAT(m, 1, 1),
	r1[2] = MAT(m, 1, 2), r1[3] = MAT(m, 1, 3),
	r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,
	r2[0] = MAT(m, 2, 0), r2[1] = MAT(m, 2, 1),
	r2[2] = MAT(m, 2, 2), r2[3] = MAT(m, 2, 3),
	r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,
	r3[0] = MAT(m, 3, 0), r3[1] = MAT(m, 3, 1),
	r3[2] = MAT(m, 3, 2), r3[3] = MAT(m, 3, 3),
	r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;
	/* choose pivot - or die */
	if (fabsf(r3[0]) > fabsf(r2[0]))
		SWAP_ROWS_FLOAT(r3, r2);
	if (fabsf(r2[0]) > fabsf(r1[0]))
		SWAP_ROWS_FLOAT(r2, r1);
	if (fabsf(r1[0]) > fabsf(r0[0]))
		SWAP_ROWS_FLOAT(r1, r0);
	if (0.0 == r0[0])
		return 0;
	/* eliminate first variable     */
	m1 = r1[0] / r0[0];
	m2 = r2[0] / r0[0];
	m3 = r3[0] / r0[0];
	s = r0[1];
	r1[1] -= m1 * s;
	r2[1] -= m2 * s;
	r3[1] -= m3 * s;
	s = r0[2];
	r1[2] -= m1 * s;
	r2[2] -= m2 * s;
	r3[2] -= m3 * s;
	s = r0[3];
	r1[3] -= m1 * s;
	r2[3] -= m2 * s;
	r3[3] -= m3 * s;
	s = r0[4];
	if (s != 0.0) {
		r1[4] -= m1 * s;
		r2[4] -= m2 * s;
		r3[4] -= m3 * s;
	}
	s = r0[5];
	if (s != 0.0) {
		r1[5] -= m1 * s;
		r2[5] -= m2 * s;
		r3[5] -= m3 * s;
	}
	s = r0[6];
	if (s != 0.0) {
		r1[6] -= m1 * s;
		r2[6] -= m2 * s;
		r3[6] -= m3 * s;
	}
	s = r0[7];
	if (s != 0.0) {
		r1[7] -= m1 * s;
		r2[7] -= m2 * s;
		r3[7] -= m3 * s;
	}
	/* choose pivot - or die */
	if (fabsf(r3[1]) > fabsf(r2[1]))
		SWAP_ROWS_FLOAT(r3, r2);
	if (fabsf(r2[1]) > fabsf(r1[1]))
		SWAP_ROWS_FLOAT(r2, r1);
	if (0.0 == r1[1])
		return 0;
	/* eliminate second variable */
	m2 = r2[1] / r1[1];
	m3 = r3[1] / r1[1];
	r2[2] -= m2 * r1[2];
	r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3];
	r3[3] -= m3 * r1[3];
	s = r1[4];
	if (0.0 != s) {
		r2[4] -= m2 * s;
		r3[4] -= m3 * s;
	}
	s = r1[5];
	if (0.0 != s) {
		r2[5] -= m2 * s;
		r3[5] -= m3 * s;
	}
	s = r1[6];
	if (0.0 != s) {
		r2[6] -= m2 * s;
		r3[6] -= m3 * s;
	}
	s = r1[7];
	if (0.0 != s) {
		r2[7] -= m2 * s;
		r3[7] -= m3 * s;
	}
	/* choose pivot - or die */
	if (fabsf(r3[2]) > fabsf(r2[2]))
		SWAP_ROWS_FLOAT(r3, r2);
	if (0.0 == r2[2])
		return 0;
	/* eliminate third variable */
	m3 = r3[2] / r2[2];
	r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
	r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];
	/* last check */
	if (0.0 == r3[3])
		return 0;
	s = 1.0 / r3[3];		/* now back substitute row 3 */
	r3[4] *= s;
	r3[5] *= s;
	r3[6] *= s;
	r3[7] *= s;
	m2 = r2[3];			/* now back substitute row 2 */
	s = 1.0 / r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
	r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
	m1 = r1[3];
	r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
	r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
	m0 = r0[3];
	r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
	r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;
	m1 = r1[2];			/* now back substitute row 1 */
	s = 1.0 / r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
	r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
	m0 = r0[2];
	r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
	r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;
	m0 = r0[1];			/* now back substitute row 0 */
	s = 1.0 / r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
	r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);
	MAT(out, 0, 0) = r0[4];
	MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6];
	MAT(out, 0, 3) = r0[7], MAT(out, 1, 0) = r1[4];
	MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6];
   MAT(out, 1, 3) = r1[7], MAT(out, 2, 0) = r2[4];
   MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6];
   MAT(out, 2, 3) = r2[7], MAT(out, 3, 0) = r3[4];
   MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6];
   MAT(out, 3, 3) = r3[7];
   return 1;
}

namespace ITF
{
	void GFXAdapter_OpenGLES::init()
	{
		TRACK_FUNCTION_CALL;		
		
		if (getScreenWidth() == 0 || getScreenHeight() == 0)
			LOG("GFXAdapter init error : resolution = 0, call setResolution(width,height) before init");
		
		glViewport(0, 0, getScreenHeight(), getScreenWidth());
		glMatrixMode (GL_PROJECTION); //set the matrix to projection
		glLoadIdentity ();
		MakePerspective (45.f, (GLfloat)((GLfloat)getScreenWidth()/(GLfloat)getScreenHeight()), 1.f, 10000.0f);
		
		glMatrixMode (GL_MODELVIEW); //set the matrix back to model
		glShadeModel(GL_SMOOTH);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		
		renderContext_SetDefault();
		
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);            // Really Nice Perspective Calculations
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		
		for (u32 i = 0; i < 10 ; i++)
			FBTexture[i] = 0;
	    CAMERA->setPosition(Vec3d(0.f, 0.f, 1.f));
		setCamera(CAMERA);
		
		createInternalBuffers();
		
		setGlobalColor(Color::white());
	}

	void GFXAdapter_OpenGLES::reshape()
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - reshape");
	}

	void GFXAdapter_OpenGLES::project(const Vec3d& _in3d, Vec2d & _out2d)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - project");
	}

	void GFXAdapter_OpenGLES::coord2DTo3D(const Vec2d& _in2d, float _zPlane, Vec2d& _out3d, bool noTranslation)
	{
		float	viewMatrix[16];
		float	projMatrix[16];
		int		viewPort[4];
		float	out[4];
		
		glGetIntegerv(GL_VIEWPORT, viewPort);
		glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
		glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);
		if(noTranslation)
		{
			viewMatrix[12] = 0.f;
			viewMatrix[13] = 0.f;
			viewMatrix[14] = 0.f;
		}
		
		glhUnProjectf(_in2d.m_x, _in2d.m_y, _zPlane, viewMatrix, projMatrix, viewPort, out);
		_out3d.m_x = out[0];
		_out3d.m_y = out[1];
	}
	
	void GFXAdapter_OpenGLES::compute3DTo2D(const Vec3d& _in, Vec3d& _out)
	{
		float	viewMatrix[16];
		float	projMatrix[16];
		int		viewPort[4];
		float	out[4];
		
		glGetIntegerv(GL_VIEWPORT, viewPort);
		glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
		glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);
		
		glhProjectf(_in.m_x, _in.m_y, _in.m_z, viewMatrix, projMatrix, viewPort, out);
		_out.m_x = out[0];
		_out.m_y = out[1];
		_out.m_z = out[2];
	}
	
	void GFXAdapter_OpenGLES::compute2DTo3D(const Vec3d& _in, Vec3d& _out)
	{
		float	viewMatrix[16];
		float	projMatrix[16];
		int		viewPort[4];
		float	out[4];
		
		glGetIntegerv(GL_VIEWPORT, viewPort);
		glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
		glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);
		
		glhUnProjectf(_in.m_x, _in.m_y, _in.m_z, viewMatrix, projMatrix, viewPort, out);
		_out.m_x = out[0];
		_out.m_y = out[1];
		_out.m_z = out[2];
	}

	void GFXAdapter_OpenGLES::drawSphere(float _x, float _y, float _z, float r, int lats, int longs, int _wireFrame, float _r, float _g, float _b)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - drawSphere");
	}

	void GFXAdapter_OpenGLES::drawCircle(float _x, float _y, float r, float _r, float _g, float _b, float _z, u32 _segsCount)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - drawCircle");
	}

	void GFXAdapter_OpenGLES::drawBox(const Vec2d& _pos, f32 _angle, const Vec2d& _extent, f32 _r, f32 _g, f32 _b, float _z, bbool _fill)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - drawBox");
	}

	void GFXAdapter_OpenGLES::drawLine(const Vec2d& _p1, const Vec2d& _p2, float _r, float _g, float _b, float _size, float _z)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - drawLine");
	}

	void GFXAdapter_OpenGLES::draw2dLine(const Vec2d& _p1, const Vec2d& _p2, float _size, u32 _color)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - draw2dLine");
	}

	void GFXAdapter_OpenGLES::draw2dBox(const Vec2d& _p, f32 _width, f32 _height, u32 _color0, u32 _color1, u32 _color2, u32 _color3, i32 _rank)
	{
        if (hasHideFlags(HideFlag_2DBox))
            return;
		ITF_ASSERT_MSG(0, "Not implemented on iPad - draw2dBox");
	}

	void GFXAdapter_OpenGLES::drawTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r, float _g, float _b, float _a, float _z)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - drawTriangle");
	}

	void GFXAdapter_OpenGLES::drawTriangle(const Vertex& _v1, const Vertex& _v2, const Vertex& _v3, const Texture* _tex, bbool _showWireFrame)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - drawTriangle");
	}

	void GFXAdapter_OpenGLES::drawQuad2D(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, const Texture* _tex, const Vec2d* _uvStart,  const Vec2d* _uvEnd)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - drawQuad2D");
	}

	void GFXAdapter_OpenGLES::draw3DQuad(GFX_3DQUAD* _t_Quad)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - draw3DQuad");
	}

	bbool GFXAdapter_OpenGLES::loadTexture(Texture* _texture, const char* _path, u32 _alhpa)
	{
		TRACK_FUNCTION_CALL;

		return loadTextureCooked(_texture,_path);
	}

	bbool GFXAdapter_OpenGLES::loadTextureCooked(Texture* _texture, const char* _path)
	{
		TRACK_FUNCTION_CALL;
		String filenameCooked = FILESERVER->getCookedName(_path);
		
		File* fileTexture = FILEMANAGER->openFile(filenameCooked,ITF_FILE_ATTR_READ);
		ITF_ASSERT(fileTexture);
		
		u32 sizeFile = (u32) fileTexture->getLength();
		u8 *rawBuf = newAlloc(mId_Temporary,u8[sizeFile]);
		u32 bytesRead = 0;
		ITF_VERIFY(fileTexture->read(rawBuf,sizeFile,&bytesRead));
		fileTexture = FILEMANAGER->closeFile(fileTexture);
		
		DDS_HEADER header;
		ITF_Memcpy(&header,rawBuf,sizeof(header));
		
		Size size((f32) header.dwWidth,(f32) header.dwHeight);
		setTextureSize(_texture, size);
		
		ITF::Texture::PixFormat PixFormat;
		
		switch (header.format.dwFourCC)
		{
			case MAKEFOURCC('D', 'X', 'T', '1'):
			case MAKEFOURCC('D', 'X', 'T', '2'):
			case MAKEFOURCC('D', 'X', 'T', '3'):
			case MAKEFOURCC('D', 'X', 'T', '4'):
			case MAKEFOURCC('D', 'X', 'T', '5'):
				ITF_ASSERT_MSG(0, "Texture format not supported on iPad");
				break;
			default:
				PixFormat = Texture::PF_RGBA;
				break;
		}
		
		loadFromMemory(rawBuf + sizeof(header), size,_texture, PixFormat ,sizeFile,btrue);
		
		SF_DEL_ARRAY(rawBuf);
		
#ifndef ITF_FINAL
		_texture->setMemoryUsage(sizeFile);
#endif //ITF_FINAL
		
		RESOURCE_MANAGER->addResourceAsLoaded(_texture);
		return btrue;
	}

	void GFXAdapter_OpenGLES::drawScreenQuad(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, bbool _noTex)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - drawScreenQuad");
	}
	
	void GFXAdapter_OpenGLES::setCamera(Camera* _cam)
	{
        GFXAdapter::setCamera(_cam);

		TRACK_FUNCTION_CALL;
		f32 nearPlane = 1.0f;
		f32 farPlane = 10000.f;
		
		CAMERA->m_near = nearPlane;
		
		GFX_RECT Viewport;
		Viewport.left = 0;
		Viewport.top = 0;
		Viewport.right = getScreenWidth();
		Viewport.bottom = getScreenHeight();
		setupViewport(&Viewport);
		
		PVRTMat4 mg_World;
		
		PVRTVec3 vEyePt( CAMERA->getX(), CAMERA->getY(), CAMERA->getZ() );
		PVRTVec3 vLookatPt = vEyePt;
		PVRTVec3 vUpVec( CAMERA->getUpDir().getX(), CAMERA->getUpDir().getY(), CAMERA->getUpDir().getZ() );
		
		PVRTVec3 lookatOffset(CAMERA->getLookAtDir().getX(), CAMERA->getLookAtDir().getY(), CAMERA->getLookAtDir().getZ());
		vLookatPt += lookatOffset;
		mg_World = PVRTMat4::LookAtRH(vEyePt, vLookatPt, vUpVec);
		
		MakePerspective(CAMERA->getFocale(), (float)getScreenHeight() / (float)getScreenWidth(), nearPlane, farPlane );
		glLoadIdentity();
		glMultMatrixf(mg_World.f);
	}

	void GFXAdapter_OpenGLES::init2DRender()
	{
		TRACK_FUNCTION_CALL;
		setOrthoView(0.f, (f32) getScreenWidth(), 0.f, (f32) getScreenHeight());
	}

	void GFXAdapter_OpenGLES::init2DGuiRender()
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - init2DGuiRender");
	}

	void GFXAdapter_OpenGLES::setOrthoView(f32 _l, f32 _r, f32 _b, f32 _t)
	{
		TRACK_FUNCTION_CALL;
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		glOrthof(_b, _t, _r, _l, -1,  1);
		glRotatef(-90.f, 0.f, 0.f, 1.f);
		glScalef(-1.f, 1.f, -1.f);
		
		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity ();
	}

	void GFXAdapter_OpenGLES::CalcCoord(const Vec2d& _in2d, float _zPlane, Vec2d& _out3d)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - CalcCoord");
	}

	void GFXAdapter_OpenGLES::clear(u32 _buffer, u32 _Col)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - clear");
	}

	void GFXAdapter_OpenGLES::clear(u32 _buffer, float _r, float _g, float _b, float _a)
	{
		TRACK_FUNCTION_CALL;
		u32 Buf = 0;
		
		if(_buffer & GFX_CLEAR_COLOR)
		{
			Buf |= GL_COLOR_BUFFER_BIT;
		}
		
		glClearColor(_r, _g, _b, _a);
		glClear(Buf);
	}

	void GFXAdapter_OpenGLES::setAlphaBlend(GFX_BLENDMODE _blend)
	{
		switch(_blend)
		{
			case GFX_BLEND_COPY:
				glDisable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ZERO);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_ALPHA:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_ALPHAPREMULT:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_ALPHADEST:
				glEnable(GL_BLEND);
				glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_ALPHADESTPREMULT:
				glEnable(GL_BLEND);
				glBlendFunc(GL_DST_ALPHA, GL_ONE);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_ADD:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_ADDALPHA:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_SUBALPHA:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				glBlendEquationOES(GL_FUNC_REVERSE_SUBTRACT_OES);
				break;
				
			case GFX_BLEND_SUB:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_MUL:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ZERO, GL_SRC_COLOR);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_ALPHAMUL:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ZERO, GL_SRC_ALPHA);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_IALPHAMUL:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_IALPHA:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_IALPHAPREMULT:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_SRC_ALPHA);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_IALPHADEST:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_IALPHADESTPREMULT:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
				glBlendEquationOES(GL_FUNC_ADD_OES);
				break;
				
			case GFX_BLEND_MUL2X:
				glEnable(GL_BLEND);
				glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
				glBlendEquationOES(GL_FUNC_ADD_OES);
		}
	}

	void GFXAdapter_OpenGLES::setCullMode(GFX_CULLMODE _cull)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - setCullMode");
	}

	void GFXAdapter_OpenGLES::depthMask(u32 _depthMask)
	{
		TRACK_FUNCTION_CALL;
		if (_depthMask)
		{
			glDepthMask(GL_TRUE);
		}
		else
		{
			glDepthMask(GL_FALSE);
		}
	}

	void GFXAdapter_OpenGLES::depthFunc(u32 _func)
	{
		TRACK_FUNCTION_CALL;
		switch(_func)
		{
			case GFX_CMP_NEVER:         
				glDepthFunc(GL_NEVER); 
				break;
			case GFX_CMP_LESS:           
				glDepthFunc(GL_LESS); 
				break;
			case GFX_CMP_EQUAL:          
				glDepthFunc(GL_EQUAL); 
				break;
			case GFX_CMP_LESSEQUAL:      
				glDepthFunc(GL_LEQUAL); 
				break;
			case GFX_CMP_GREATER:        
				glDepthFunc(GL_GREATER); 
				break;
			case GFX_CMP_NOTEQUAL:       
				glDepthFunc(GL_NOTEQUAL); 
				break;
			case GFX_CMP_GREATEREQUAL:   
				glDepthFunc(GL_GEQUAL); 
				break;
			case GFX_CMP_ALWAYS:         
				glDepthFunc(GL_ALWAYS); 
				break;
		}
	}

	void GFXAdapter_OpenGLES::depthTest(u32 _test)
	{
	}

	void GFXAdapter_OpenGLES::setScissorRect(GFX_RECT* _clipRect)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - setScissorRect");
	}

	void GFXAdapter_OpenGLES::SetTextureBind(int _Sampler, unsigned int _Bind, bbool _linearFiltering)
	{
		TRACK_FUNCTION_CALL;
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, (GLuint)_Bind);
	}

	void GFXAdapter_OpenGLES::cleanupTexture(Texture* _texture)
	{
		TRACK_FUNCTION_CALL;
		if (_texture)
		{
			glDeleteTextures(1, &  _texture->m_adapterimplementationData);
			_texture->m_adapterimplementationData = 0;
		}
	}

	void GFXAdapter_OpenGLES::DrawPrimitive(PRIMITIVETYPE _type, const void *_p_Vertex, u32 _NumberVertex)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - DrawPrimitive");
	}

	void GFXAdapter_OpenGLES::setMatrixTransform(MATRIXMODE _type, const GMatrix44 *_Matrix)
	{
		TRACK_FUNCTION_CALL;
		i32 Matrixtype;
		switch(_type)
		{
			default:
			case GFX_MWORLD:
				return;
				break;
			case GFX_MWORLDTOWIEW:
			case GFX_MVIEW:
				Matrixtype = GL_MODELVIEW;
				break;
			case GFX_PROJECTION:
				Matrixtype = GL_PROJECTION;
				break;
			case GFX_TEXTURE:
				Matrixtype = GL_TEXTURE;
				break;
		}
		
		glMatrixMode(Matrixtype);
		glLoadMatrixf((GLfloat*)_Matrix);
	}

	void GFXAdapter_OpenGLES::setupViewport(GFX_RECT* _Viewport)
	{
		TRACK_FUNCTION_CALL;
		glViewport(_Viewport->top, _Viewport->left, _Viewport->bottom - _Viewport->top, _Viewport->right - _Viewport->left);
	}

	void GFXAdapter_OpenGLES::unprojectPoint(const Vec2d& _in, float _viewDistance, const GMatrix44 *_MatrixModel, const GMatrix44 *_MatrixProj, const int *_View, Vec2d& _out)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - unprojectPoint");
	}

	void GFXAdapter_OpenGLES::setLookAtMatrix(GMatrix44* _MatrixOut, double _midx, double _midy, float _viewDistance, float _aspect)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - setLookAtMatrix");
	}

	void GFXAdapter_OpenGLES::getViewport(GFX_RECT *_RectOut)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - getViewport");
	}

	/// GfxMaterials.
	void GFXAdapter_OpenGLES::setGfxMaterial(const GFX_MATERIAL& _gfxMat)
	{
		/// set alpha blend. 
		setAlphaBlend(_gfxMat.m_blendMode);
		
		/// set Texture;
		if (_gfxMat.m_textureDiffuse)
			SetTextureBind(0, _gfxMat.m_textureDiffuse->m_adapterimplementationData);
    }

	void GFXAdapter_OpenGLES::setGfxMatDefault()
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - setGfxMatDefault");
	}

	void GFXAdapter_OpenGLES::setGfxMatRefraction(const GFX_MATERIAL& _gfxMat)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - setGfxMatRefraction");
	}

	/// Texture.
	void GFXAdapter_OpenGLES::generateTexture(Texture* _texture)
	{
		TRACK_FUNCTION_CALL;
		// save old texture binding
		GLuint old_tex;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&old_tex));
		
		GLuint _id = 0;
		
		glGenTextures(1, &_id);
		
		// set some parameters for this texture.
		glBindTexture(GL_TEXTURE_2D, _id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		_texture->m_adapterimplementationData = (u32)_id;
		
		// restore previous texture binding.
		glBindTexture(GL_TEXTURE_2D, old_tex);
	}

	void GFXAdapter_OpenGLES::cleanupBaseTexture(Texture* _texture)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - cleanupBaseTexture");
	}

	void GFXAdapter_OpenGLES::setTextureSize(Texture* _texture, const Size& _size)
	{
		TRACK_FUNCTION_CALL;
		_texture->m_sizeX = (u32)_size.d_width;
		_texture->m_sizeY = (u32)_size.d_height;
	}

	unsigned char*	GFXAdapter_OpenGLES::grabTexture(Texture* _texture)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - grabTexture");
		return NULL;
	}

	u32 GFXAdapter_OpenGLES::getMaxTextureSize()
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - getMaxTextureSize");
		return 1024;
	}

	void GFXAdapter_OpenGLES::loadFromMemory(const void* _buffer, const Size& _texture_size, Texture* _texture, Texture::PixFormat _pixel_format,const u32 _buffer_size,bbool bCooked)
	{
		TRACK_FUNCTION_CALL;
		GLint comps;
		GLenum format;
		switch (_pixel_format)
		{
			case Texture::PF_RGB:
				comps = 3;
				format = GL_RGB;
				break;
			case Texture::PF_RGBA:
				comps = 4;
				format = GL_RGBA;
				break;
			default:
				ITF_ERROR("[GFXAdapter_OpenGL::loadFromMemory] Unknow pixel format");
				return;
		}
		
		generateTexture(_texture);
		
		setTextureSize(_texture, _texture_size);
		_texture->m_datasizeX = (u32)_texture_size.d_width;
		_texture->m_datasizeY = (u32)_texture_size.d_height;
		updateCachedScaleValues(_texture);
		
		for(GLubyte* buffer = (GLubyte*)_buffer, *end = (GLubyte*)_buffer + (u32)_texture_size.d_width * (u32)_texture_size.d_height * 4; buffer != end; buffer += 4)
		{
			GLubyte red = *buffer;
			*buffer = *(buffer + 2);
			*(buffer + 2) = red; 
		}
		
		// save old texture binding
		GLuint old_tex;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&old_tex));
		
		// do the real work of getting the data into the texture
		glBindTexture(GL_TEXTURE_2D, _texture->m_adapterimplementationData);
		glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexImage2D(GL_TEXTURE_2D, 0, format, (u32)_texture_size.d_width, (u32)_texture_size.d_height, 0, format, GL_UNSIGNED_BYTE, _buffer);
		
		// restore previous texture binding.
		glBindTexture(GL_TEXTURE_2D, old_tex);
	}

	void GFXAdapter_OpenGLES::SetTexture(int _Sampler, Texture* _Texture, bbool _linearFiltering)
	{
		glBindTexture(GL_TEXTURE_2D, _Texture->m_adapterimplementationData);
	}

	void GFXAdapter_OpenGLES::lockTexture(Texture* _tex, LOCKED_TEXTURE* _lockTex, u32 _flag)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - lockTexture");
	}

	void GFXAdapter_OpenGLES::unlockTexture(Texture* _tex)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - unlockTexture");
	}

	void GFXAdapter_OpenGLES::saveToMemory(void* _buffer, Texture* _Tex)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - saveToMemory");
	}

    void GFXAdapter_OpenGLES::setSamplerState( u32 _samp, GFX_SAMPLERSTATETYPE _type, i32 _value );
    {
        ITF_ASSERT_MSG(0, "Not implemented on iPad - setSamplerState");
    }

	float GFXAdapter_OpenGLES::getTexelOffset()
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - getTexelOffset");
		return 0.f;
	}

	void GFXAdapter_OpenGLES::clearRenderTarget(renderTarget* _target, u32 _color)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - clearRenderTarget");
	}

	void GFXAdapter_OpenGLES::initialiseRenderTarget(renderTarget* _target)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - initialiseRenderTarget");
	}

	void GFXAdapter_OpenGLES::resizeRenderTarget(renderTarget* _target, const Size& _size)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - resizeRenderTarget");
	}

	void GFXAdapter_OpenGLES::enableRenderTarget(renderTarget* _target)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - enableRenderTarget");
	}

	void GFXAdapter_OpenGLES::disableRenderTarget(renderTarget* _target)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - disableRenderTarget");
	}

	void GFXAdapter_OpenGLES::cleanupRenderTarget(renderTarget* _target)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - cleanupRenderTarget");
	}

	bbool GFXAdapter_OpenGLES::isRenderTargetInverted()
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - isRenderTargetInverted");
		return bfalse;
	}

	void GFXAdapter_OpenGLES::doScreenCapture()
	{
		TRACK_FUNCTION_CALL;
	}

	void GFXAdapter_OpenGLES::startScreenCapture()
	{
		TRACK_FUNCTION_CALL;
	}

	void GFXAdapter_OpenGLES::stopScreenCapture()
	{
		TRACK_FUNCTION_CALL;
	}

	void GFXAdapter_OpenGLES::setShaderForFont(bbool bOutline)
	{
		//ITF_ASSERT_MSG(0, "Not implemented on iPad - setShaderForFont");
	}

	void GFXAdapter_OpenGLES::restoreDefaultShader()
	{
		//ITF_ASSERT_MSG(0, "Not implemented on iPad - restoreDefaultShader");
	}

	void GFXAdapter_OpenGLES::present()
	{
		TRACK_FUNCTION_CALL;
	}

	void GFXAdapter_OpenGLES::startRendering()
	{
		TRACK_FUNCTION_CALL;
	}

	void GFXAdapter_OpenGLES::endRendering()
	{
		TRACK_FUNCTION_CALL;
	}

	bbool GFXAdapter_OpenGLES::isDeviceLost()
	{
		return bfalse;
	}

	bbool GFXAdapter_OpenGLES::resetDevice(bbool _forceReset)
	{
		return bfalse;
	}

	void GFXAdapter_OpenGLES::useHDWLightning(u32 _activate)
	{
		TRACK_FUNCTION_CALL;
		if (_activate)
		{
			glEnable(GL_LIGHTING);
		}
		else
		{
			glDisable(GL_LIGHTING);
		}
	}

	void GFXAdapter_OpenGLES::setVertexFormat(u32 _vformat)
	{
		TRACK_FUNCTION_CALL;
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisable(GL_TEXTURE_2D);
		
		switch(_vformat)
		{
			case VertexFormat_P:
				glEnableClientState(GL_VERTEX_ARRAY);
				break;
			case VertexFormat_PC:
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_COLOR_ARRAY);
				break;
			case VertexFormat_PT:
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glEnable(GL_TEXTURE_2D);
				break;
			case VertexFormat_PCT:
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_COLOR_ARRAY);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glEnable(GL_TEXTURE_2D);
				break;
			case VertexFormat_PNCT:
			case VertexFormat_PCBT:
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_COLOR_ARRAY);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glEnableClientState(GL_NORMAL_ARRAY);
				glEnable(GL_TEXTURE_2D);
				break;
			default:
				ITF_ASSERT_MSG(0, "Invalid vertex format!");
				break;
				
		}
	}

	/// shaders.
	void GFXAdapter_OpenGLES::createShader(ITF_shader* _shader)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - createShader");
	}

	void GFXAdapter_OpenGLES::releaseShader(ITF_shader* _shader)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - releaseShader");
	}

	bbool GFXAdapter_OpenGLES::loadShader(ITF_shader* _shader)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - loadShader");
		return btrue;
	}

	void GFXAdapter_OpenGLES::beginShader(ITF_shader* _shader)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - beginShader");
	}

	void GFXAdapter_OpenGLES::endShader(ITF_shader* _shader)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - endShader");
	}

	// Patch shader
	void GFXAdapter_OpenGLES::prepareShaderPatch(GMatrix44* _matrix, f32 _z, u32 &_hdiv, u32 &_vdiv)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - prepareShaderPatch");
	}

	bbool GFXAdapter_OpenGLES::shaderDrawPatch32(Texture *texture, Vec2d *PointsNS, ColorInteger* _lighttab)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - shaderDrawPatch32");
		return btrue;
	}

	void GFXAdapter_OpenGLES::endShaderPatch()
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - endShaderPatch");
	}

	void GFXAdapter_OpenGLES::shaderPatchFlush(u32 _hdiv, u32 _vdiv, u32 _pass)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - shaderPatchFlush");
	}

    // Fluid Shader
    void GFXAdapter_OpenGLES::drawFluid(   GMatrix44* _matrix, Texture* _texture, const FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT> &_colorTab, const SafeArray<Vec2d> &_heightTab, const FixedArray<Vec2d, 4> &_uvTab, u32 mode, f32 _delta1, f32 _delta2, f32 _stepCount  )
    {
        ITF_ASSERT_MSG(0, "Not implemented on iPad - drawFluid");
    }

	/// VertexBuffer.
	void GFXAdapter_OpenGLES::createVertexBuffer( ITF_VertexBuffer* _vertexBuffer)
	{
		TRACK_FUNCTION_CALL;
		GLubyte* _data = new GLubyte[_vertexBuffer->m_nVertex * _vertexBuffer->m_structVertexSize];
		_vertexBuffer->mp_VertexBuffer = (u32)_data;
	}

	void GFXAdapter_OpenGLES::releaseVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
	{
		TRACK_FUNCTION_CALL;
		delete[] (GLubyte*)_vertexBuffer->mp_VertexBuffer;
		_vertexBuffer->mp_VertexBuffer = 0;
	}

	void GFXAdapter_OpenGLES::setVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
	{
		TRACK_FUNCTION_CALL;
		setVertexFormat(_vertexBuffer->m_vertexFormat);
		
		switch(_vertexBuffer->m_vertexFormat)
		{
			case VertexFormat_P:
				glVertexPointer(3, GL_FLOAT, _vertexBuffer->m_structVertexSize, &((VertexP*)_vertexBuffer->mp_VertexBuffer)->m_pos);
				break;
			case VertexFormat_PC:
				glVertexPointer(3, GL_FLOAT, _vertexBuffer->m_structVertexSize, &((VertexPC*)_vertexBuffer->mp_VertexBuffer)->m_pos);
				glColorPointer(4, GL_UNSIGNED_BYTE, _vertexBuffer->m_structVertexSize, &((VertexPC*)_vertexBuffer->mp_VertexBuffer)->m_color);
				break;
			case VertexFormat_PT:
				glVertexPointer(3, GL_FLOAT, _vertexBuffer->m_structVertexSize, &((VertexPT*)_vertexBuffer->mp_VertexBuffer)->m_pos);
				glTexCoordPointer(2, GL_FLOAT, _vertexBuffer->m_structVertexSize, &((VertexPT*)_vertexBuffer->mp_VertexBuffer)->m_uv);
				break;				
			case VertexFormat_PCT:
				glVertexPointer(3, GL_FLOAT, _vertexBuffer->m_structVertexSize, &((VertexPCT*)_vertexBuffer->mp_VertexBuffer)->m_pos);
				glColorPointer(4, GL_UNSIGNED_BYTE, _vertexBuffer->m_structVertexSize, &((VertexPCT*)_vertexBuffer->mp_VertexBuffer)->m_color);
				glTexCoordPointer(2, GL_FLOAT, _vertexBuffer->m_structVertexSize, &((VertexPCT*)_vertexBuffer->mp_VertexBuffer)->m_uv);
				break;
			case VertexFormat_PNCT:
				glVertexPointer(3, GL_FLOAT, _vertexBuffer->m_structVertexSize, &((VertexPNCT*)_vertexBuffer->mp_VertexBuffer)->m_pos);
				glColorPointer(4, GL_UNSIGNED_BYTE, _vertexBuffer->m_structVertexSize, &((VertexPNCT*)_vertexBuffer->mp_VertexBuffer)->m_color);
				glTexCoordPointer(2, GL_FLOAT, _vertexBuffer->m_structVertexSize, &((VertexPNCT*)_vertexBuffer->mp_VertexBuffer)->m_uv);
				glNormalPointer(GL_FLOAT, _vertexBuffer->m_structVertexSize, &((VertexPNCT*)_vertexBuffer->mp_VertexBuffer)->m_normal);
				break;				
			case VertexFormat_PCBT:
				glVertexPointer(3, GL_FLOAT, _vertexBuffer->m_structVertexSize, &((VertexPNCBT*)_vertexBuffer->mp_VertexBuffer)->m_pos);
				glColorPointer(4, GL_UNSIGNED_BYTE, _vertexBuffer->m_structVertexSize, &((VertexPNCBT*)_vertexBuffer->mp_VertexBuffer)->m_color);
				glTexCoordPointer(2, GL_FLOAT, _vertexBuffer->m_structVertexSize, &((VertexPNCBT*)_vertexBuffer->mp_VertexBuffer)->m_uv);
				glNormalPointer(GL_FLOAT, _vertexBuffer->m_structVertexSize, &((VertexPNCBT*)_vertexBuffer->mp_VertexBuffer)->m_normal);
				break;
			default:
				ITF_ASSERT_MSG(0, "Invalid vertex format!");
				break;
		}
	}

	void GFXAdapter_OpenGLES::LockVertexBuffer(ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag)
	{
		TRACK_FUNCTION_CALL;
		*_data = (void**)_vertexBuffer->mp_VertexBuffer;
	}

	void GFXAdapter_OpenGLES::UnlockVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
	{
	}

	/// IndexBuffer.
	void GFXAdapter_OpenGLES::createIndexBuffer(ITF_IndexBuffer* _indexBuffer)
	{
		TRACK_FUNCTION_CALL;
		u16* _data = new u16[_indexBuffer->m_nIndices];
		_indexBuffer->mp_IndexBuffer = (u32)_data;
	}

	void GFXAdapter_OpenGLES::releaseIndexBuffer(ITF_IndexBuffer* _indexBuffer)
	{
		TRACK_FUNCTION_CALL;
		delete[] (u16*)_indexBuffer->mp_IndexBuffer;
		_indexBuffer->mp_IndexBuffer = 0;
	}

	void GFXAdapter_OpenGLES::setIndexBuffer(ITF_IndexBuffer* _indexBuffer)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - setIndexBuffer");
	}

	void GFXAdapter_OpenGLES::LockIndexBuffer(ITF_IndexBuffer* _indexBuffer, void** _data)
	{
		TRACK_FUNCTION_CALL;
		*_data = (void**)_indexBuffer->mp_IndexBuffer;
	}

	void GFXAdapter_OpenGLES::UnlockIndexBuffer(ITF_IndexBuffer* _indexBuffer)
	{
	}

	/// Vertex/Index Buffer Draw.
	void GFXAdapter_OpenGLES::DrawVertexBuffer(u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber)
	{
		TRACK_FUNCTION_CALL;
		GLenum type;
		switch(_type)
		{
			default:
			case GFX_TRIANGLES: 
				type = GL_TRIANGLES;
				break;
			case GFX_POINTS:
				type = GL_POINTS;
				break;
			case GFX_LINES:
				type = GL_LINES;
				break;
			case GFX_LINE_STRIP:
				type = GL_LINE_STRIP;
				break;
			case GFX_TRIANGLE_STRIP:
				type = GL_TRIANGLE_STRIP;
				break;
			case GFX_TRIANGLE_FAN:
				type = GL_TRIANGLE_FAN;
				break;
		}    
		
		setVertexBuffer(_vertexBuffer);
		glDrawArrays(type, _vertexStart, _vertexNumber);
	}

	void GFXAdapter_OpenGLES::DrawIndexedVertexBuffer(u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _indexNumber)
	{
		i32 type = GFX_TRIANGLES;
		switch(_type)
		{
			default:
			case GFX_TRIANGLES: 
				type = GL_TRIANGLES;
				break;
			case GFX_POINTS:
				type = GL_POINTS;
				break;
			case GFX_LINES:
				type = GL_LINES;
				break;
			case GFX_LINE_STRIP:
				type = GL_LINE_STRIP;
				break;
			case GFX_TRIANGLE_STRIP:
				type = GL_TRIANGLE_STRIP;
				break;
			case GFX_TRIANGLE_FAN:
				type = GL_TRIANGLE_FAN;
				break;
		}    
		
		// draw the geometry
		setVertexBuffer(_vertexBuffer);
		glDrawElements(type, _indexNumber, GL_UNSIGNED_SHORT, (void*)_indexBuffer->mp_IndexBuffer);
	}

	void GFXAdapter_OpenGLES::drawMesh(ITF_Mesh& _mesh, bbool _showWireFrame)
	{
		TRACK_FUNCTION_CALL;
		glDisable(GL_CULL_FACE);    
		useHDWLightning(0);

		ITF_VertexBuffer* pVertexBuffer =  _mesh.m_vtxBuffer ?   _mesh.m_vtxBuffer : _mesh.m_vtxBufferBuffered[_mesh.m_userBuffer];
		
		for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
		{
			if (!_mesh.m_ElementList[i].m_material.m_textureDiffuse || _mesh.m_ElementList[i].m_count == 0)
				continue;
			
			setGfxMaterial(_mesh.m_ElementList[i].m_material);
			
			if (_mesh.m_ElementList[i].m_material.m_uvAnim.m_uvFlagAnim)
				setUVAnim(_mesh.m_ElementList[i].m_material.m_uvAnim);
			else
			{
                setMatrixTransform(GFX_TEXTURE, &GMatrixIdentity);
			} 
			if (!_mesh.m_ElementList[i].m_indexBuffer)
				DrawVertexBuffer(GFX_TRIANGLES, pVertexBuffer, _mesh.m_ElementList[i].m_startIndex, _mesh.m_ElementList[i].m_count);
			else
				DrawIndexedVertexBuffer(GFX_TRIANGLES, pVertexBuffer, _mesh.m_ElementList[i].m_indexBuffer, _mesh.m_ElementList[i].m_count);
		} 
		
		setMatrixTransform(GFX_TEXTURE, &GMatrixIdentity);
	}

#ifndef ITF_CONSOLE_FINAL
	void GFXAdapter_OpenGLES::drawFlatMesh(const ITF_Mesh& _mesh)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - drawFlatMesh");
	}
#endif // ITF_CONSOLE_FINAL

	void GFXAdapter_OpenGLES::drawMeshPT(const ITF_Mesh& _mesh, bbool _showWireFrame)
	{
		TRACK_FUNCTION_CALL;
		glDisable(GL_CULL_FACE);    
		useHDWLightning(0);
		
		glPushMatrix();
		glMultMatrixf(&_mesh.getMatrix().mat._11);
		
		setVertexFormat(VertexFormat_PT);
		
		ITF_VertexBuffer* pVertexBuffer =  _mesh.m_vtxBuffer ?   _mesh.m_vtxBuffer : _mesh.m_vtxBufferBuffered[_mesh.m_userBuffer];
		
		for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
		{
			if (!_mesh.m_ElementList[i].m_material.m_textureDiffuse || _mesh.m_ElementList[i].m_count == 0)
				continue;
			
			setGfxMaterial(_mesh.m_ElementList[i].m_material);
			
			if (_mesh.m_ElementList[i].m_material.m_uvAnim.m_uvFlagAnim)
				setUVAnim(_mesh.m_ElementList[i].m_material.m_uvAnim);
			else
			{
                setMatrixTransform(GFX_TEXTURE, &GMatrixIdentity());
			} 
			if (!_mesh.m_ElementList[i].m_indexBuffer)
				DrawVertexBuffer(GFX_TRIANGLES, pVertexBuffer, _mesh.m_ElementList[i].m_startIndex, _mesh.m_ElementList[i].m_count);
			else
				DrawIndexedVertexBuffer(GFX_TRIANGLES, pVertexBuffer, _mesh.m_ElementList[i].m_indexBuffer, _mesh.m_ElementList[i].m_count);
		} 
		
        setMatrixTransform(GFX_TEXTURE, &GMatrixIdentity);
		glPopMatrix();
	}

	/// After FX.
	void GFXAdapter_OpenGLES::copyCurrentColorBuffer(u32 _rt)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - copyCurrentColorBuffer");
	}

	void GFXAdapter_OpenGLES::copyTexture(int *TexHan)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - copyTexture");
	}

	void GFXAdapter_OpenGLES::validateTexture(int *TexHan,int Mode,int ScreenSize,int ScreenSize_V)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - validateTexture");
	}

	void GFXAdapter_OpenGLES::AFTERFX_BigBlur(f32 _factor, u32 _MaxPass, bbool _clearRT, GFX_QUALITY _quality)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - AFTERFX_BigBlur");
	}

	void GFXAdapter_OpenGLES::AFTERFX_Glow(f32 _factor, f32 _glowfactor, f32 _addalpha, GFX_BLENDMODE _finalBlendMode)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - AFTERFX_Glow");
	}

    void GFXAdapter_OpenGLES::AFTERFX_objectsGlow(f32 _factor)
    {
        ITF_ASSERT_MSG(0, "Not implemented on iPad - AFTERFX_objectsGlow");
    }

    void GFXAdapter_OpenGLES::AFTERFX_prepareObjectGlow()
    {
        ITF_ASSERT_MSG(0, "Not implemented on iPad - AFTERFX_prepareObjectGlow");
    }

	void GFXAdapter_OpenGLES::AFTERFX_Remanence(f32 _factor, f32 _glowfactor, f32 _addalpha)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - AFTERFX_Remanence");
	}

	void GFXAdapter_OpenGLES::AFTERFX_RayCenter(f32 _v1, f32 _v2, f32 _v3, f32 _v4, f32 _v5, f32 _v6, f32 _v7)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - AFTERFX_RayCenter");
	}

	void GFXAdapter_OpenGLES::AFTERFX_ColorSetting(f32 _sat, f32 _contrast, f32 _contrastscale, f32 _bright)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - AFTERFX_ColorSetting");
	}

	void GFXAdapter_OpenGLES::AFTERFX_ColorRemap(Texture* _tex)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - AFTERFX_ColorRemap");
	}

    void GFXAdapter_OpenGLES::AFTERFX_ColorLevels(f32 _inBlack,  f32 _inGamma, f32 _inWhite, f32 _outBlack, f32 _outWhite)
    {
		ITF_ASSERT_MSG(0, "Not implemented on iPad - AFTERFX_ColorLevels");
    }

	void GFXAdapter_OpenGLES::AFTERFX_addSceneAndMul(f32 _mul)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - AFTERFX_addSceneAndMul");
	}

	void GFXAdapter_OpenGLES::AFTERFX_byTech(u32 _tech)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - AFTERFX_byTech");
	}

	void GFXAdapter_OpenGLES::AFTERFX_byTechParams(u32 _tech, f32 _p0, f32 _p1, f32 _p2, f32 _p3)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - AFTERFX_byTechParams");
	}

    void GFXAdapter_OpenGLES::prepareAfterFx(bbool _nocopy)
    {
        ITF_ASSERT_MSG(0, "Not implemented on iPad - prepareAfterFx");
    }

    void GFXAdapter_OpenGLES::endAfterfx(bbool _nocopy, GFX_BLENDMODE _finalBlend)
    {
        TF_ASSERT_MSG(0, "Not implemented on iPad - endAfterfx");
    }

	void GFXAdapter_OpenGLES::AFTERFX_draw(AFX_Group* _group)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - AFTERFX_draw");
	}

	void GFXAdapter_OpenGLES::AFTERFX_startRenderTarget(Color _color)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - AFTERFX_startRenderTarget");
	}

	void GFXAdapter_OpenGLES::startImpostorRendering(renderTarget* _rt, f32 xmin, f32 ymin, f32 xmax, f32 ymax, bbool _mustClear)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - startImpostorRendering");
	}

	void GFXAdapter_OpenGLES::stopImpostorRendering(renderTarget* _rt)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - stopImpostorRendering");
	}

	/// Surfaces.
	f32 GFXAdapter_OpenGLES::ReadSurfaceFloat(void* _surf, u32 _posX, u32 _posY)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - ReadSurfaceFloat");
		return 0.f;
	}

	/// UV.
	void GFXAdapter_OpenGLES::setUVAnim(const GFX_UVANIM& _uvanim)
	{
		setMatrixTransform(GFX_TEXTURE, &_uvanim.m_UVMat);
		
		/// back to view.
		glMatrixMode (GL_MODELVIEW);
	}

	/// Render Context.
	void GFXAdapter_OpenGLES::renderContext_Set()
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - renderContext_Set");
	}

	void GFXAdapter_OpenGLES::renderContext_SetDefault()
	{
		TRACK_FUNCTION_CALL;
		depthMask(0);
		depthTest(0);
		depthFunc(GFX_CMP_LESSEQUAL);
	}

	void GFXAdapter_OpenGLES::setGlobalColor(const Color& _color)
	{
		TRACK_FUNCTION_CALL;
		setGFXGlobalColor(_color);
	}

	const GMatrix44* GFXAdapter_OpenGLES::getViewMatrix()
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad - getViewMatrix");
		return NULL;
	}

	const String GFXAdapter_OpenGLES::getRenderName()
	{
		return String("OpenGLES");
	}
} // namespace ITF
