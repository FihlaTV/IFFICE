/*----------------------------------------------------------------------
 *����ˮɼģ�ͣ�.mqo�� 
 *������RL
 *date 2010.04.16
 *Copyright (c) 2008 Sunao Hashimoto & Keisuke Konishi(kei)
 *----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
	1.��������
		�ɼ���bmp,jpeg,png��ʽ����
		����jpeg��ʽ��Ҫ�⣺libjpeg.lib jpeglib.h
			�궨�� DEF_USE_LIBJPEG
		����png��ʽ��Ҫ�� ��libpng.lib zlib.lib png.h zlib.h
			�궨�� DEF_USE_LIBPNG
	2. mqo��ȡ��غ���
		(1) ��ʼ����argInit()ʹ�ú�
			mqoInit();
		(2) ����ģ��
			MQO_MODEL model;
			model = mqoCreateModel( "mario.mqo", 1.0 );
		(3) ����ģ��
			mqoCallModel( model );
		(4) ���ģ��
			mqoDeleteModel( model );
		(5) ��ֹ(�˳�ǰ����)
			mqoCleanup();
	3. mqo������غ���
		(1)��ʼ����ͬ��
			mqoInit();
		(2)����ģ�� ����mario0.mqo~mario9.mqo
			MQO_SEQUENCE seq;
			seq = mqoCreateSequence( "mario%d.mqo", 10, 1.0 );
		(3)����ָ����֡�������
			mqoCallSequence( seq, i );
		(4)����ͷ�
			mqoDeleteSequence( seq );
		(5) ͬ��
			mqoCleanup();
	4. ��ʾ����
		-֧��ˮɼver1.0/2.0~2.4
	   ��Ϣ֧�� 
	    -���� ��ɫ���⣩ ����ӳ�䣨�ǰ�͹��ͼ/UV��ͼ��
	   ֧�ֶ���
		-��ʾ/�����л�
		-������Ϣ
		-������Ϣ���Ƕ�����ɫ��			
------------------------------------------------------------------------*/



/*----------------------------------------------------------------------
 *�û��趨
 *----------------------------------------------------------------------*/

#ifndef _MQOLOADER_H
#define _MQOLOADER_H

#define MAX_TEXTURE				100			// ���������
#define MAX_OBJECT				50			// ��߶�����Ŀ
#define SIZE_STR				256			// �ַ�����С������
#define DEF_IS_LITTLE_ENDIAN	1			// ָ���ֽ��򣨻���intel=1��
#define DEF_USE_LIBJPEG			0			// libjpeg 0/1 ��ʹ��/ʹ�� 
#define DEF_USE_LIBPNG			0			// libpng  0/1 ��ʹ��/ʹ��



/*----------------------------------------------------------------------
 * ����ѡ��
 *----------------------------------------------------------------------*/

// JPEG
#ifdef D_JPEG
	#undef	DEF_USE_LIBJPEG
	#define	DEF_USE_LIBJPEG 1
#endif

// JPEG
#ifdef D_NO_JPEG
	#undef	DEF_USE_LIBJPEG
	#define	DEF_USE_LIBJPEG 0
#endif

// PNG
#ifdef D_PNG
	#undef	DEF_USE_LIBPNG
	#define	DEF_USE_LIBPNG 1
#endif

// PNG
#ifdef D_NO_PNG
	#undef	DEF_USE_LIBPNG
	#define	DEF_USE_LIBPNG 0
#endif

#ifdef WIN32
	#include <windows.h>
#else
	#ifndef MAX_PATH
		#define MAX_PATH    256
	#endif
	#ifndef TRUE
		#define TRUE    (1==1)
	#endif
	#ifndef FALSE
	    #define FALSE   (1!=1)
	#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <GLUT/glut.h>
	#include <OpenGL/glext.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
//	#include <gl/glext.h>
#endif


/*----------------------------------------------------------------------
 *ʹ��jpeg 
 *----------------------------------------------------------------------*/

#if DEF_USE_LIBJPEG

	#define XMD_H //int16 �� int32 �������
	#ifdef FAR
		#undef FAR
	#endif

	#include "jpeglib.h"
	#pragma comment(lib,"libjpeg.lib")

#endif


/*----------------------------------------------------------------------
 *ʹ��png
 * ---------------------------------------------------------------------*/

#if DEF_USE_LIBPNG

	#include "png.h"
	#include "zlib.h"
	#pragma comment(lib,"libpng.lib")
	#pragma comment(lib,"zlib.lib")

#endif

#ifndef MAX
	#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

//TGA��ʽ����
#define DEF_TGA_COLOR_MAP_FLAG_VALID	1
#define DEF_TGA_TYPE_NON				0
#define DEF_TGA_TYPE_INDEX				1
#define DEF_TGA_TYPE_FULL				2
#define DEF_TGA_TYPE_MONO				3
#define DEF_TGA_TYPE_RLEINDEX			9
#define DEF_TGA_TYPE_RLEFULL			10
#define DEF_TGA_TYPE_RLEMONO			11
#define DEF_TGA_BIT_INFO_RIGHT_TO_LEFT	0x00
#define DEF_TGA_BIT_INFO_LEFT_TO_RIGHT	0x10
#define DEF_TGA_BIT_INFO_DOWN_TO_TOP	0x00
#define DEF_TGA_BIT_INFO_TOP_TO_DOWN	0x20

typedef struct {
	unsigned char	id;
	unsigned char	color_map_flag;
	unsigned char	type;
	unsigned short	color_map_entry;
	unsigned char	color_map_entry_size;
	unsigned short	x;
	unsigned short	y;
	unsigned short	width;
	unsigned short	height;
	unsigned char	depth;
	unsigned char	bit_info;
} STR_TGA_HEAD;

typedef struct {
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;
} glCOLOR4f;

typedef struct {
	GLfloat x;
	GLfloat y;
} glPOINT2f;

typedef struct tag_glPOINT3f {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} glPOINT3f;


/*------------------------------------------------------------------------
�ṹ��
------------------------------------------------------------------------*/
typedef struct {
	int			n;		// 
	int			m;		// ���ϱ�����Ŀ
	int			v[4];	// ��������
	glPOINT2f	uv[4];	// UV����
} MQO_FACE;


/*------------------------------------------------------------------------
���ϵ���Ϣ�ṹ
------------------------------------------------------------------------*/
typedef struct {
	glCOLOR4f	col;				// ɫ��
	GLfloat		dif[4];				// ������
	GLfloat		amb[4];				// ������
	GLfloat		emi[4];				// ��Ȼ����
	GLfloat		spc[4];				// ����
	GLfloat		power;				// �����ǿ��
	int			useTex;				// ʹ������
	char		texFile[SIZE_STR];	// �����ļ�
	char		alpFile[SIZE_STR];	// 
	GLuint		texName;			// ��������
} MQO_MATDATA;


/*------------------------------------------------------------------------
����ṹ����
------------------------------------------------------------------------*/
typedef struct {
	char		objname[SIZE_STR];	// ��������
	int			visible;			// ����
	int			shading;			// ���� 0:�ڹ�1:����
	float		facet;				// ƽ����
	int			n_face;				// ����
	int			n_vertex;			// ������
	MQO_FACE	*F;					// ��
	glPOINT3f	*V;					// ����
} MQO_OBJDATA;


/*------------------------------------------------------------------------
 �����
------------------------------------------------------------------------*/
typedef struct {
	GLuint			texture_id;			// ������
	int				texsize;			// ����ߴ�
	char			texfile[MAX_PATH];	// �����ļ���ַ
	char			alpfile[MAX_PATH];	// 
	unsigned char	alpha;				// 
} TEXTURE_POOL;


/*------------------------------------------------------------------------
����ṹ
------------------------------------------------------------------------*/
typedef struct {		
	GLfloat point[3];	//  (x, y, z)
	GLfloat normal[3];	//  (x, y, z)
	GLfloat uv[2];		// UV (u, v)
} VERTEX_TEXUSE;


/*------------------------------------------------------------------------
����ṹ����ʹ������
------------------------------------------------------------------------*/
typedef struct {
	GLfloat point[3];	//  (x, y, z)
	GLfloat normal[3];	//  (x, y, z)
} VERTEX_NOTEX;


/*------------------------------------------------------------------------
����
------------------------------------------------------------------------*/
typedef struct {
	int				isValidMaterialInfo;// ������Ϣ ����/����
	int				isUseTexture;		// �������� USE_TEXTURE / NOUSE_TEXTURE
	GLuint			texture_id;			// ����id (OpenGL)
	GLuint			VBO_id;				// ���㻺�������
	int				datanum;			// 
	GLfloat			color[4];			// ��ɫ���� (r, g, b, a)
	GLfloat			dif[4];				// ������
	GLfloat			amb[4];				// ������
	GLfloat			emi[4];				// �Զ�����
	GLfloat			spc[4];				// ����
	GLfloat			power;				// �����ǿ��
	VERTEX_NOTEX	*vertex_p;			// ���㣨����ͼ��
	VERTEX_TEXUSE	*vertex_t;			// ���㣨����ͼ��
} MQO_MATERIAL;


/*------------------------------------------------------------------------
�ڲ�����
------------------------------------------------------------------------*/
typedef struct {
	char			objname[SIZE_STR];		// ��������
	int				isVisible;				// �Ƿ����
	int				isShadingFlat;			// �⻬��Ӱ
	int				matnum;					// ������Ŀ
	MQO_MATERIAL	*mat;					// ��������
} MQO_INNER_OBJECT;


/*------------------------------------------------------------------------
model����ʵ��
------------------------------------------------------------------------*/
typedef struct {
	unsigned char		alpha;				// 
	int					objnum;				// �ڲ�������Ŀ
	MQO_INNER_OBJECT	obj[MAX_OBJECT];	// �ڲ���������
} MQO_OBJECT;

typedef MQO_OBJECT * MQO_MODEL;		// MQO_MODEL


/*------------------------------------------------------------------------
 MQO���� 
------------------------------------------------------------------------*/
typedef struct {
	MQO_MODEL	model;		// ģ��
	int			n_frame;	// ֡����Ŀ
} MQO_SEQUENCE;


/*------------------------------------------------------------------------
VBO ��չ���
------------------------------------------------------------------------*/
#ifdef WIN32
	#define GL_ARRAY_BUFFER_ARB	0x8892
	#define GL_STATIC_DRAW_ARB	0x88E4
	typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC)    (GLenum target, GLuint buffer);
	typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
	typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC)    (GLsizei n, GLuint *buffers);
	typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC)    (GLenum target, int size, const GLvoid *data, GLenum usage);
#endif


/*------------------------------------------------------------------------
�����
------------------------------------------------------------------------*/

#ifdef _MQOLOADER_C
	#define _MQOLOADER_C_EXTERN
#else
	#define _MQOLOADER_C_EXTERN extern
#endif

_MQOLOADER_C_EXTERN int g_isVBOSupported;	// ֧��openGL�Ķ��㻺����

#ifdef WIN32	
	// VBO Extension 
	__GLMETASEQ_C__EXTERN PFNGLGENBUFFERSARBPROC glGenBuffersARB;		// VBO ������
	__GLMETASEQ_C__EXTERN PFNGLBINDBUFFERARBPROC glBindBufferARB;		// VBO ���
	__GLMETASEQ_C__EXTERN PFNGLBUFFERDATAARBPROC glBufferDataARB;		// VBO ���ݼ���
	__GLMETASEQ_C__EXTERN PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;	// VBO ɾ��
#endif

#undef __GLMETASEQ_C__EXTERN


/*------------------------------------------------------------------------
��������
------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif


// ��ʼ��
void mqoInit(void);

// ���
void mqoCleanup(void);

// ����ģ�� ��ģ��·����������
MQO_MODEL	 mqoCreateModel(char *filename, double scale); 

// �������� ����ʽ����
MQO_SEQUENCE mqoCreateSequence(const char *format, int n_file, double scale);

// ��������-��չ�汾
MQO_SEQUENCE mqoCreateSequenceEx(const char *format, int n_file, double scale,
								 int fade_inout, unsigned char alpha);

// ����ģ��
void mqoCallModel(MQO_MODEL model);

// ���ö���
void mqoCallSequence(MQO_SEQUENCE seq, int i);

// ɾ��ģ��
void mqoDeleteModel(MQO_MODEL model);

// ɾ������
void mqoDeleteSequence(MQO_SEQUENCE seq);


#ifdef __cplusplus
}
#endif




#endif	// -- end of header --���ڷ������ˣ���

