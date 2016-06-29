/*----------------------------------------------------------------------
 *加载水杉模型（.mqo） 
 *翻译者RL
 *date 2010.04.16
 *Copyright (c) 2008 Sunao Hashimoto & Keisuke Konishi(kei)
 *----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
	1.关于纹理
		可加载bmp,jpeg,png格式纹理
		加载jpeg格式需要库：libjpeg.lib jpeglib.h
			宏定义 DEF_USE_LIBJPEG
		加载png格式需要库 ：libpng.lib zlib.lib png.h zlib.h
			宏定义 DEF_USE_LIBPNG
	2. mqo读取相关函数
		(1) 初始化，argInit()使用后
			mqoInit();
		(2) 加载模型
			MQO_MODEL model;
			model = mqoCreateModel( "mario.mqo", 1.0 );
		(3) 调用模型
			mqoCallModel( model );
		(4) 清除模型
			mqoDeleteModel( model );
		(5) 终止(退出前调用)
			mqoCleanup();
	3. mqo动画相关函数
		(1)初始化，同上
			mqoInit();
		(2)加载模型 例如mario0.mqo~mario9.mqo
			MQO_SEQUENCE seq;
			seq = mqoCreateSequence( "mario%d.mqo", 10, 1.0 );
		(3)调用指定的帧序号序列
			mqoCallSequence( seq, i );
		(4)清除释放
			mqoDeleteSequence( seq );
		(5) 同上
			mqoCleanup();
	4. 显示功能
		-支持水杉ver1.0/2.0~2.4
	   信息支持 
	    -材料 颜色（光） 纹理映射（非凹凸贴图/UV贴图）
	   支持对象
		-显示/隐藏切换
		-顶点信息
		-表面信息（非顶点颜色）			
------------------------------------------------------------------------*/



/*----------------------------------------------------------------------
 *用户设定
 *----------------------------------------------------------------------*/

#ifndef _MQOLOADER_H
#define _MQOLOADER_H

#define MAX_TEXTURE				100			// 最高纹理数
#define MAX_OBJECT				50			// 最高对象数目
#define SIZE_STR				256			// 字符串大小缓冲区
#define DEF_IS_LITTLE_ENDIAN	1			// 指定字节序（基于intel=1）
#define DEF_USE_LIBJPEG			0			// libjpeg 0/1 不使用/使用 
#define DEF_USE_LIBPNG			0			// libpng  0/1 不使用/使用



/*----------------------------------------------------------------------
 * 编译选项
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
 *使用jpeg 
 *----------------------------------------------------------------------*/

#if DEF_USE_LIBJPEG

	#define XMD_H //int16 和 int32 避免错误
	#ifdef FAR
		#undef FAR
	#endif

	#include "jpeglib.h"
	#pragma comment(lib,"libjpeg.lib")

#endif


/*----------------------------------------------------------------------
 *使用png
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

//TGA格式定义
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
结构面
------------------------------------------------------------------------*/
typedef struct {
	int			n;		// 
	int			m;		// 材料表面数目
	int			v[4];	// 顶点数组
	glPOINT2f	uv[4];	// UV坐标
} MQO_FACE;


/*------------------------------------------------------------------------
材料的信息结构
------------------------------------------------------------------------*/
typedef struct {
	glCOLOR4f	col;				// 色彩
	GLfloat		dif[4];				// 漫反射
	GLfloat		amb[4];				// 环境光
	GLfloat		emi[4];				// 自然超明
	GLfloat		spc[4];				// 反射
	GLfloat		power;				// 反射光强度
	int			useTex;				// 使用纹理
	char		texFile[SIZE_STR];	// 纹理文件
	char		alpFile[SIZE_STR];	// 
	GLuint		texName;			// 纹理名称
} MQO_MATDATA;


/*------------------------------------------------------------------------
对象结构定义
------------------------------------------------------------------------*/
typedef struct {
	char		objname[SIZE_STR];	// 对象名称
	int			visible;			// 可视
	int			shading;			// 底纹 0:遮光1:发光
	float		facet;				// 平滑角
	int			n_face;				// 面数
	int			n_vertex;			// 顶点数
	MQO_FACE	*F;					// 面
	glPOINT3f	*V;					// 顶点
} MQO_OBJDATA;


/*------------------------------------------------------------------------
 纹理库
------------------------------------------------------------------------*/
typedef struct {
	GLuint			texture_id;			// 纹理编号
	int				texsize;			// 纹理尺寸
	char			texfile[MAX_PATH];	// 纹理文件地址
	char			alpfile[MAX_PATH];	// 
	unsigned char	alpha;				// 
} TEXTURE_POOL;


/*------------------------------------------------------------------------
顶点结构
------------------------------------------------------------------------*/
typedef struct {		
	GLfloat point[3];	//  (x, y, z)
	GLfloat normal[3];	//  (x, y, z)
	GLfloat uv[2];		// UV (u, v)
} VERTEX_TEXUSE;


/*------------------------------------------------------------------------
顶点结构（不使用纹理）
------------------------------------------------------------------------*/
typedef struct {
	GLfloat point[3];	//  (x, y, z)
	GLfloat normal[3];	//  (x, y, z)
} VERTEX_NOTEX;


/*------------------------------------------------------------------------
材料
------------------------------------------------------------------------*/
typedef struct {
	int				isValidMaterialInfo;// 材料信息 启用/禁用
	int				isUseTexture;		// 有无纹理 USE_TEXTURE / NOUSE_TEXTURE
	GLuint			texture_id;			// 纹理id (OpenGL)
	GLuint			VBO_id;				// 顶点缓冲区编号
	int				datanum;			// 
	GLfloat			color[4];			// 颜色数组 (r, g, b, a)
	GLfloat			dif[4];				// 漫反射
	GLfloat			amb[4];				// 环境光
	GLfloat			emi[4];				// 自动光照
	GLfloat			spc[4];				// 反射
	GLfloat			power;				// 反射光强度
	VERTEX_NOTEX	*vertex_p;			// 顶点（无贴图）
	VERTEX_TEXUSE	*vertex_t;			// 顶点（有贴图）
} MQO_MATERIAL;


/*------------------------------------------------------------------------
内部对象
------------------------------------------------------------------------*/
typedef struct {
	char			objname[SIZE_STR];		// 对象名称
	int				isVisible;				// 是否可视
	int				isShadingFlat;			// 光滑阴影
	int				matnum;					// 材质数目
	MQO_MATERIAL	*mat;					// 材质阵列
} MQO_INNER_OBJECT;


/*------------------------------------------------------------------------
model对象实体
------------------------------------------------------------------------*/
typedef struct {
	unsigned char		alpha;				// 
	int					objnum;				// 内部对象数目
	MQO_INNER_OBJECT	obj[MAX_OBJECT];	// 内部对象阵列
} MQO_OBJECT;

typedef MQO_OBJECT * MQO_MODEL;		// MQO_MODEL


/*------------------------------------------------------------------------
 MQO序列 
------------------------------------------------------------------------*/
typedef struct {
	MQO_MODEL	model;		// 模型
	int			n_frame;	// 帧的数目
} MQO_SEQUENCE;


/*------------------------------------------------------------------------
VBO 扩展设计
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
定义宏
------------------------------------------------------------------------*/

#ifdef _MQOLOADER_C
	#define _MQOLOADER_C_EXTERN
#else
	#define _MQOLOADER_C_EXTERN extern
#endif

_MQOLOADER_C_EXTERN int g_isVBOSupported;	// 支持openGL的顶点缓冲区

#ifdef WIN32	
	// VBO Extension 
	__GLMETASEQ_C__EXTERN PFNGLGENBUFFERSARBPROC glGenBuffersARB;		// VBO 设名代
	__GLMETASEQ_C__EXTERN PFNGLBINDBUFFERARBPROC glBindBufferARB;		// VBO 设绑定
	__GLMETASEQ_C__EXTERN PFNGLBUFFERDATAARBPROC glBufferDataARB;		// VBO 数据加载
	__GLMETASEQ_C__EXTERN PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;	// VBO 删除
#endif

#undef __GLMETASEQ_C__EXTERN


/*------------------------------------------------------------------------
函数声明
------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif


// 初始化
void mqoInit(void);

// 清空
void mqoCleanup(void);

// 创建模型 （模型路径，比例）
MQO_MODEL	 mqoCreateModel(char *filename, double scale); 

// 创建动画 （格式，）
MQO_SEQUENCE mqoCreateSequence(const char *format, int n_file, double scale);

// 创建动画-扩展版本
MQO_SEQUENCE mqoCreateSequenceEx(const char *format, int n_file, double scale,
								 int fade_inout, unsigned char alpha);

// 调用模型
void mqoCallModel(MQO_MODEL model);

// 调用动画
void mqoCallSequence(MQO_SEQUENCE seq, int i);

// 删除模型
void mqoDeleteModel(MQO_MODEL model);

// 删除动画
void mqoDeleteSequence(MQO_SEQUENCE seq);


#ifdef __cplusplus
}
#endif




#endif	// -- end of header --终于翻译完了！！

