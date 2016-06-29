
#define _MQOLOADER_C
#include "mqoloader.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


/*------------------------------------------------------------------------
全局静态变量
------------------------------------------------------------------------*/

static TEXTURE_POOL l_texPool[MAX_TEXTURE];		// 纹理池
static int			l_texPoolnum;				// 纹理数
static int			l_GLMetaseqInitialized = 0;	// 初始化标志


/*------------------------------------------------------------------------
函数声明
------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

void	endianConverter(void *addr,unsigned int size);
void	TGAHeaderEndianConverter(	STR_TGA_HEAD *tgah );
int		IsExtensionSupported( char* szTargetExtension );

GLuint		mqoSetTexturePool(char *texfile, char *alpfile, unsigned char alpha );
void		mqoClearTexturePool();
GLubyte*	mqoLoadTextureEx(char *texfile,char *alpfile,int *tex_size,unsigned char alpha);
int			mqoLoadFile(MQO_OBJECT *mqoobj,char *filename,double scale,unsigned char alpha);
MQO_OBJECT*	mqoCreateList(int num);
int			mqoCreateListObject( MQO_OBJECT *obj, int id, char *filename,double scale,unsigned char alpha);

void mqoCallListObject(MQO_OBJECT object[],int num);
void mqoClearObject(MQO_OBJECT object[],int from,int num);
void mqoDeleteObject(MQO_OBJECT * object,int num);
void mqoGetDirectory(const char *path_file, char *path_dir);
void mqoSnormal(glPOINT3f A, glPOINT3f B, glPOINT3f C, glPOINT3f *normal);
void mqoReadMaterial(FILE *fp, MQO_MATDATA M[]);
void mqoReadVertex(FILE *fp, glPOINT3f V[]);
int	 mqoReadBVertex(FILE *fp,glPOINT3f V[]);
void mqoReadFace(FILE *fp, MQO_FACE F[]);
void mqoReadObject(FILE *fp, MQO_OBJDATA *obj);

void mqoMakeArray(MQO_MATERIAL *mat, int matpos,MQO_FACE F[], int fnum,glPOINT3f V[],
				  glPOINT3f N[], double facet, glCOLOR4f *mcol, double scale, unsigned char alpha );

glPOINT3f *mqoVertexNormal(MQO_OBJDATA *obj);

void mqoMakePolygon(MQO_OBJDATA *readObj, MQO_OBJECT *mqoobj,
					glPOINT3f N[], MQO_MATDATA M[], int n_mat, double scale, unsigned char alpha);

void mqoMakeObjectsEx(MQO_OBJECT *mqoobj, MQO_OBJDATA obj[], int n_obj, MQO_MATDATA M[],int n_mat,
					  double scale,unsigned char alpha);

#ifdef __cplusplus
}
#endif


/*------------------------------------------------------------------------
[函数名]endianConverter
[功  能]字节序转换
[参  数]
		addr	地址
		size	大小

[返回值]无
------------------------------------------------------------------------*/

void endianConverter(void *addr,unsigned int size)
{
	unsigned int pos;
	char c;
	if ( size <= 1 ) return;
	for ( pos = 0; pos < size/2; pos++ ) {
		c = *(((char *)addr)+pos);
		*(((char *)addr)+pos) = *(((char *)addr)+(size-1 - pos));
		*(((char *)addr)+(size-1 - pos)) = c;
	}
}


/*------------------------------------------------------------------------
[函数名]TGAHeaderEndianConverter
[功  能]TGA 转换
[参  数]
		tgah	TGA

[返回值]无
------------------------------------------------------------------------*/

void TGAHeaderEndianConverter(	STR_TGA_HEAD *tgah )
{
	endianConverter(&tgah->color_map_entry,sizeof(tgah->color_map_entry));
	endianConverter(&tgah->x,sizeof(tgah->x));
	endianConverter(&tgah->y,sizeof(tgah->y));
	endianConverter(&tgah->width,sizeof(tgah->width));
	endianConverter(&tgah->height,sizeof(tgah->height));
}


/*------------------------------------------------------------------------
[函数名]IsExtensionSupported
[功  能]确定是否支持openGL扩展 
[参  数]
		szTargetExtension	

[返回值]1：支持  0：不支持
------------------------------------------------------------------------*/

int IsExtensionSupported( char* szTargetExtension )
{
	const unsigned char *pszExtensions = NULL;
	const unsigned char *pszStart;
	unsigned char *pszWhere, *pszTerminator;

	// Extension 偺柤慜偑惓偟偄偐挷傋傞(NULL傗嬻敀偼NG乯
	pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
	if ( pszWhere || *szTargetExtension == (char)NULL )
		return 0;

	// Extension 偺暥帤楍傪強摼偡傞
	pszExtensions = glGetString( GL_EXTENSIONS );

	// 暥帤楍偺拞偵昁梫側 extension 偑偁傞偐挷傋傞
	pszStart = pszExtensions;
	for (;;)
	{
		pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
		if ( !pszWhere )
			break;
		pszTerminator = pszWhere + strlen( szTargetExtension );
		if ( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
		if ( *pszTerminator == ' ' || *pszTerminator == (char)NULL )
			return 1;
		pszStart = pszTerminator;
	}
	return 0;
}


/*------------------------------------------------------------------------
[函数名]mqoInit
[功  能]初始化
[参  数]无
[返回值]无
------------------------------------------------------------------------*/

void mqoInit(void)
{
	// 初始化纹理池
	memset(l_texPool,0,sizeof(l_texPool));
	l_texPoolnum = 0;

	// 顶点缓冲支持
	g_isVBOSupported = IsExtensionSupported("GL_ARB_vertex_buffer_object");
//	g_isVBOSupported = 0;

#ifdef WIN32
	glGenBuffersARB = NULL;
	glBindBufferARB = NULL;
	glBufferDataARB = NULL;
	glDeleteBuffersARB = NULL;

	if ( g_isVBOSupported ) {
		// printf("OpenGL : 捀揰僶僢僼傽傪僒億乕僩偟偰偄傞偺偱巊梡偟傑偡\n");
		// GL 娭悢偺億僀儞僞傪強摼偡傞
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");
	}
#endif

	// 弶婜壔僼儔僌
	l_GLMetaseqInitialized = 1;
}


/*------------------------------------------------------------------------
[函数名]mqoCleanup
[功  能]释放
[参  数]无
[返回值]无
------------------------------------------------------------------------*/

void mqoCleanup(void)
{
	mqoClearTexturePool();	
}


/*------------------------------------------------------------------------
[函数名]mqoSetTexturePool
[功  能]加载纹理
[参  数]
		texfile		纹理文件名
		alpfile		
		alpha		

[返回值]纹理ID
[备  注]纹理如未加载，加载之，否则返回
------------------------------------------------------------------------*/

GLuint mqoSetTexturePool(char *texfile, char *alpfile, unsigned char alpha ) 
{
	int pos;
	GLubyte *image;

	for ( pos = 0; pos < l_texPoolnum; pos++ ) {
		if ( alpha != l_texPool[pos].alpha ) {
			continue;
		}
		if ( texfile != NULL ) {
			if ( strcmp(texfile,l_texPool[pos].texfile) != 0 ) {
				continue;
			}
		}
		if ( alpfile != NULL ) {
			if ( strcmp(alpfile,l_texPool[pos].alpfile) != 0 ) {
				continue;
			}
		}
		break;
	}
	if ( pos < l_texPoolnum ) { //偡偱偵撉傒崬傒嵪傒
		return  l_texPool[pos].texture_id;
	}
	if ( MAX_TEXTURE <= pos ) {
		printf("%s:mqoSetTexturePool 僥僋僗僠儍撉傒崬傒椞堟晄懌\n",__FILE__);
		return -1;
	}
	image = mqoLoadTextureEx(texfile,alpfile,&l_texPool[pos].texsize,alpha);
	if ( image == NULL ) {
		return -1;
	}

	if ( texfile != NULL ) strncpy(l_texPool[pos].texfile,texfile,MAX_PATH);
	if ( alpfile != NULL ) strncpy(l_texPool[pos].alpfile,alpfile,MAX_PATH);
	l_texPool[pos].alpha = alpha;

	glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	glPixelStorei(GL_PACK_ALIGNMENT,4);
	glGenTextures(1,&l_texPool[pos].texture_id);			// 僥僋僗僠儍傪惗惉
	glBindTexture(GL_TEXTURE_2D,l_texPool[pos].texture_id);	// 僥僋僗僠儍偺妱傝摉偰

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, l_texPool[pos].texsize, l_texPool[pos].texsize,
					0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	l_texPoolnum = pos+1;

	//搊榐偡傟偽丄撉傒崬傫偩僶僢僼傽偼晄梫
	free(image);
	glBindTexture(GL_TEXTURE_2D,0);	// 僨僼僅儖僩僥僋僗僠儍偺妱傝摉偰

	return l_texPool[pos].texture_id;
}


/*------------------------------------------------------------------------
[函数名]mqoClearTexturePool()
[功  能]清空纹理池
[参  数]无
[返回值]无
------------------------------------------------------------------------*/

void mqoClearTexturePool()
{
	int pos;
	for ( pos = 0; pos < l_texPoolnum; pos++ ) {
		glDeleteTextures(1, &l_texPool[pos].texture_id);	// 僥僋僗僠儍忣曬傪嶍彍
	}

	memset(l_texPool,0,sizeof(l_texPool));
	l_texPoolnum = 0;
}


/*------------------------------------------------------------------------
[函数名]mqoLoadTextureEx
[功  能]从文件中创建一个纹理
[参  数]
		texfile		纹理地址
		alpfile		
		tex_size	纹理大小
[返回值]24位图
------------------------------------------------------------------------*/

GLubyte* mqoLoadTextureEx(char *texfile,char *alpfile,int *tex_size,unsigned char alpha)
{
	FILE *fp;
	size_t namelen;
	char ext[4];
	char wbuf[3];
	int isTGA;
	int isPNG;
	int isJPEG;
	int other;
	int	y,x,size;
	int fl;
	char *filename[2];
	int width[2];
	int sts;
	STR_TGA_HEAD tgah;
	GLubyte	*pImage, *pRead;

#if DEF_USE_LIBJPEG
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY jpegimage;
#endif
#if DEF_USE_LIBPNG
	unsigned char **pngimage;
	unsigned long   pngwidth, pngheight;
	int				pngdepth;
	int             color_type;
#endif

	filename[0] = texfile;
	filename[1] = alpfile;
	width[0] = -1;
	width[1] = -1;
	pImage = NULL;
	fp = NULL;
	sts = 0;
#if DEF_USE_LIBJPEG
	jpegimage = NULL;
#endif
#if DEF_USE_LIBPNG
	pngimage = NULL;
#endif
	size = - 1;
	for ( fl = 0; fl < 2; fl++ ) {//僥僋僗僠儍亖fl=0    傾儖僼傽亖fl=1
		if ( filename[fl] == NULL ) continue;
		namelen = strlen(filename[fl]);
		ext[0] = tolower(filename[fl][namelen-3]);
		ext[1] = tolower(filename[fl][namelen-2]);
		ext[2] = tolower(filename[fl][namelen-1]);
		ext[3] = 0x00;
		isTGA = (strcmp(ext,"tga")==0)?1:0;
		isPNG = (strcmp(ext,"png")==0)?1:0;
		isJPEG = (strcmp(ext,"jpg")==0)?1:0;
		/* */
		if ( (! isTGA) && (! isPNG) &&(! isJPEG) ) {
			filename[fl][namelen-3] = 'b';
			filename[fl][namelen-2] = 'm';
			filename[fl][namelen-1] = 'p';
		}
		/* */
		if ( fl == 1 ) { //傾儖僼傽偺撉傒崬傒偼俿俧俙or俹俶俧
			if ( ! (isTGA || isPNG) ) {
				printf("傾儖僼傽偺僼傽僀儖偵懳墳偱偒側偄仺%s\n",filename[fl]);
				break;
			}
		}
		if ( fp != NULL ) fclose(fp);
		if ( (fp=fopen(filename[fl],"rb"))==NULL ) {
			printf("%s:cannot open bmp[%s]\n",__FILE__,filename[fl]);
			continue;
		}
		// 僿僢僟偺儘乕僪
		if ( isTGA ) {
			fread(&tgah,sizeof(STR_TGA_HEAD),1,fp);
#if DEF_IS_LITTLE_ENDIAN
#else
			TGAHeaderEndianConverter(&tgah);
#endif
			size = width[fl] = tgah.width;
		}
		if ( isJPEG ) {
#if DEF_USE_LIBJPEG
			unsigned int i;
			cinfo.err = jpeg_std_error( &jerr );
			jpeg_create_decompress( &cinfo );	//夝搥梡忣曬嶌惉
			jpeg_stdio_src( &cinfo, fp );		//撉傒崬傒僼傽僀儖巜掕
			jpeg_read_header( &cinfo, TRUE );	//jpeg僿僢僟撉傒崬傒
			jpeg_start_decompress( &cinfo );	//夝搥奐巒

			if ( cinfo.out_color_components == 3 && cinfo.out_color_space == JCS_RGB ) {
				if ( jpegimage != NULL ) {
					for (i = 0; i < cinfo.output_height; i++) free(jpegimage[i]);            // 埲壓俀峴偼俀師尦攝楍傪夝曻偟傑偡
					free(jpegimage);
				}
				//撉傒崬傒僨乕僞攝楍偺嶌惉
				jpegimage = (JSAMPARRAY)malloc( sizeof( JSAMPROW ) * cinfo.output_height );
				for ( i = 0; i < cinfo.output_height; i++ ) {
					jpegimage[i] = (JSAMPROW)malloc( sizeof( JSAMPLE ) * cinfo.out_color_components * cinfo.output_width );
				}
				//夝搥僨乕僞撉傒崬傒
				while( cinfo.output_scanline < cinfo.output_height ) {
					jpeg_read_scanlines( &cinfo,
						jpegimage + cinfo.output_scanline,
						cinfo.output_height - cinfo.output_scanline
					);
				}
				size = width[fl] = cinfo.output_width;
			}

			jpeg_finish_decompress( &cinfo );	//夝搥廔椆
			jpeg_destroy_decompress( &cinfo );	//夝搥梡忣曬夝曻
			if ( !(cinfo.out_color_components == 3 && cinfo.out_color_space == JCS_RGB) ) {
				printf("JPEG 懳墳偱偒側偄僼僅乕儅僢僩仺%s\n",filename[fl]);
			}
#else
			printf("偙偺僥僋僗僠儍偼懳墳偱偒側偄僼僅乕儅僢僩仺%s\n",filename[fl]);
			continue;
#endif
		}
		if ( isPNG ) {
#if DEF_USE_LIBPNG
			png_structp     png_ptr;
			png_infop       info_ptr;
			int             bit_depth, interlace_type;
			unsigned int             i;
			int j,k;
			png_ptr = png_create_read_struct(                       // png_ptr峔憿懱傪妋曐丒弶婜壔偟傑偡
							PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			info_ptr = png_create_info_struct(png_ptr);             // info_ptr峔憿懱傪妋曐丒弶婜壔偟傑偡
			png_init_io(png_ptr, fp);                               // libpng偵fp傪抦傜偣傑偡
			png_read_info(png_ptr, info_ptr);                       // PNG僼傽僀儖偺僿僢僟傪撉傒崬傒傑偡
			png_get_IHDR(png_ptr, info_ptr, &pngwidth, &pngheight,        // IHDR僠儍儞僋忣曬傪庢摼偟傑偡
							&bit_depth, &color_type, &interlace_type,
							&j,&k);
			if ( pngimage != NULL ) {
				for (i = 0; i < pngheight; i++) free(pngimage[i]);            // 埲壓俀峴偼俀師尦攝楍傪夝曻偟傑偡
				free(pngimage);
			}
			pngimage = (png_bytepp)malloc(pngheight * sizeof(png_bytep)); // 埲壓俁峴偼俀師尦攝楍傪妋曐偟傑偡
			i = png_get_rowbytes(png_ptr, info_ptr);
			pngdepth = i / pngwidth;
			for (i = 0; i < pngheight; i++)
					pngimage[i] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
			png_read_image(png_ptr, pngimage);                         // 夋憸僨乕僞傪撉傒崬傒傑偡

			png_destroy_read_struct(                                // 俀偮偺峔憿懱偺儊儌儕傪夝曻偟傑偡
	        &png_ptr, &info_ptr, (png_infopp)NULL);
			size = width[fl] = pngwidth;
#else
			printf("偙偺僥僋僗僠儍偼懳墳偱偒側偄僼僅乕儅僢僩仺%s\n",filename[fl]);
			continue;
#endif
		}
		if ( width[fl] == -1 ) {//僐僐傑偱偒偰僒僀僘偑巜掕偝傟偰偄側偄丂亖丂價僢僩儅僢僾
			fseek(fp,14+4,SEEK_SET);		// 夋憸暆偑奿擺偝傟偰偄傞埵抲傑偱僔乕僋
			fread(&size,sizeof(int),1,fp);	// BiWidth偺忣曬偩偗庢摼
			fseek(fp,14+40,SEEK_SET);		// 夋慺僨乕僞偑奿擺偝傟偰偄傞埵抲傑偱僔乕僋
#if DEF_IS_LITTLE_ENDIAN
#else
			endianConverter(&size,sizeof(int));
#endif
			width[fl] = size;

		}
		if ( width[0] != -1 && width[1] != -1 ) {
			if ( width[0] != width[1] ) {
				sts = -1;
				break;
			}
		}
		if ( fl == 1 && isTGA ) { //傾儖僼傽偺撉傒崬傒偼俿俧俙偺俉價僢僩儌僲僋儘倧倰俁俀價僢僩僼儖
			if ( !(
				(tgah.depth == 8 && tgah.type == DEF_TGA_TYPE_MONO) ||
				(tgah.depth == 32 && tgah.type == DEF_TGA_TYPE_FULL) 
				) ) {
				break;
			}
		}
		if ( fl == 1 && isPNG ) { //傾儖僼傽偺撉傒崬傒偼俹俶俧偺僩僁儖乕僇儔乕亄傾儖僼傽倧倰僌儗乕僗働乕儖亄傾儖僼傽
#if DEF_USE_LIBPNG
			if ( !(
				(color_type== 6 ) ||
				(color_type== 4 ) 
				) ) {
				break;
			}
#endif
		}

		// 儊儌儕偺妋曐
		if ( pImage == NULL ) {
			pImage = (GLubyte*)malloc(sizeof(unsigned char)*size*size*4);
		}
		if (pImage==NULL) return NULL;
		for (y=0; y<size; y++){
			pRead = pImage + (size-1-y)*4*size;
			for (x=0; x<size; x++) {
				other = 1;
				if ( fl == 0 ) {
					if ( isJPEG ) {
#if DEF_USE_LIBJPEG
						pRead[0]= jpegimage[size-1-y][x*3];
						pRead[1]= jpegimage[size-1-y][x*3+1];
						pRead[2]= jpegimage[size-1-y][x*3+2];
						pRead[3] = alpha;				// A
						other = 0;
#endif
					}
					if ( isPNG ) {
#if DEF_USE_LIBPNG
						if ( color_type == 2 || color_type==6 ) { 
							pRead[0]= pngimage[size-1-y][x*pngdepth];
							pRead[1]= pngimage[size-1-y][x*pngdepth+1];
							pRead[2]= pngimage[size-1-y][x*pngdepth+2];
							pRead[3] = alpha;				// A
							if ( color_type == 6 ) {
								pRead[3]= pngimage[size-1-y][x*pngdepth+3];
							}
						}
						other = 0;
#endif
					}
					if ( other )  {
						fread(&pRead[2],1,1,fp);	// B
						fread(&pRead[1],1,1,fp);	// G	
						fread(&pRead[0],1,1,fp);	// R
						pRead[3] = alpha;				// A
						if ( isTGA && tgah.depth == 32 ) {
							fread(&pRead[3],1,1,fp);	// A
							if ( alpha < pRead[3] ) pRead[3] = alpha;
						}
					}
				}
				else {
					if ( isPNG ) {
#if DEF_USE_LIBPNG
						if ( color_type == 6 ) { //僩僁儖乕僇儔乕亄傾儖僼傽
							pRead[3]= pngimage[size-1-y][x*pngdepth+3];
						}
						if ( color_type == 4 ) { //僌儗乕僗働乕儖亄傾儖僼傽
							pRead[3]= pngimage[size-1-y][x*pngdepth+1];
						}
						if ( alpha < pRead[3] ) pRead[3] = alpha;
#endif
					}
					if ( isTGA ) {
						if ( tgah.depth == 32 ) { //偄傜側偄僨乕僞傪撉傒旘偽偡
							fread(wbuf,3,1,fp);	// BGR
						}
						fread(&pRead[3],1,1,fp);	// A
						if ( alpha < pRead[3] ) pRead[3] = alpha;
					}
				}
				pRead+=4;
			}
		}
		fclose(fp);
		fp = NULL;
	}
	if ( sts != 0 ) {
		if ( pImage != NULL ) free(pImage);
		if ( fp != NULL ) fclose(fp);
	}
#if DEF_USE_LIBPNG
	if ( pngimage != NULL ) {
		unsigned int uy;
		for (uy = 0; uy < pngheight; uy++) free(pngimage[uy]);            // 埲壓俀峴偼俀師尦攝楍傪夝曻偟傑偡
		free(pngimage);
	}
#endif
#if DEF_USE_LIBJPEG
	if ( jpegimage != NULL ) {
		unsigned int uy;
		for (uy = 0; uy < cinfo.output_height; uy++) free(jpegimage[uy]);            // 埲壓俀峴偼俀師尦攝楍傪夝曻偟傑偡
		free(jpegimage);
	}
#endif
	if ( size < 0 ) {
		if ( pImage != NULL ) free(pImage);
		pImage = NULL;
	}
	*tex_size = size;

	return pImage;
}


/*------------------------------------------------------------------------
[函数名]mqoLoadFile
[功  能]加载mqo文件
[参  数]
		mqoobj		MQO对象
		filename	文件路径
		scale		放大比例
		alpha		

[返回值]1/0 成功/失败
------------------------------------------------------------------------*/

int mqoLoadFile( MQO_OBJECT *mqoobj, char *filename, double scale, unsigned char alpha)
{
	FILE			*fp;
	MQO_OBJDATA		obj[MAX_OBJECT];
	MQO_MATDATA		*M = NULL;

	char	buf[SIZE_STR];		// 暥帤楍撉傒崬傒僶僢僼傽
	char	path_dir[SIZE_STR];	// 僨傿儗僋僩儕偺僷僗
	char	path_tex[SIZE_STR];	// 僥僋僗僠儍僼傽僀儖偺僷僗
	char	path_alp[SIZE_STR];	// 傾儖僼傽僥僋僗僠儍僼傽僀儖偺僷僗
	int		n_mat = 0;			// 儅僥儕傾儖悢
	int		n_obj = 0;			// 僆僽僕僃僋僩悢
	int		i;

	// Material偲Object偺撉傒崬傒
	fp = fopen(filename,"rb");
	if (fp==NULL) return 0;

	mqoobj->alpha = alpha;
	memset(obj,0,sizeof(obj));

	i = 0;
	while ( !feof(fp) ) {
		fgets(buf,SIZE_STR,fp);

		// Material
		if (strstr(buf,"Material")) {
			sscanf(buf,"Material %d", &n_mat);
			M = (MQO_MATDATA*) calloc( n_mat, sizeof(MQO_MATDATA) );
			mqoReadMaterial(fp,M);
		}

		// Object
		if (strstr(buf,"Object")) {
			sscanf(buf,"Object %s", obj[i].objname);
			mqoReadObject(fp, &obj[i]);
			i++;
		}
	}
	n_obj = i;
	fclose(fp);
	// 僷僗偺庢摼
	mqoGetDirectory(filename, path_dir);

	// 僥僋僗僠儍偺搊榐
	for (i=0; i<n_mat; i++) {
		if (M[i].useTex) {

			if (strstr(M[i].texFile,".")) {
				strcpy(path_tex, M[i].texFile);	// 愨懳僷僗偺応崌
			} else {
				sprintf(path_tex,"%s%s",path_dir,M[i].texFile);	// 憡懳僷僗偺応崌
			}

			if ( M[i].alpFile[0] != (char)0 ) {
				if (strstr(M[i].texFile,".")) {
					strcpy(path_alp, M[i].alpFile);	// 愨懳僷僗偺応崌
				} else {
					sprintf(path_alp,"%s%s",path_dir,M[i].alpFile);	// 憡懳僷僗偺応崌
				}
				M[i].texName = mqoSetTexturePool(path_tex,path_alp,alpha);
			}
			else {
				M[i].texName = mqoSetTexturePool(path_tex,NULL,alpha);
			}
		}
	}

	mqoMakeObjectsEx( mqoobj, obj, n_obj, M, n_mat, scale, alpha );

	// 僆僽僕僃僋僩偺僨乕僞偺奐曻
	for (i=0; i<n_obj; i++) {
		free(obj[i].V);
		free(obj[i].F);
	}

	// 儅僥儕傾儖偺奐曻
	free(M);

	return 1;
}


/*------------------------------------------------------------------------
[函数名]mqoCreateList
[功  能]MQO对象列表
[参  数]num		MQO文件数目
[返回值]MQO对象指针
------------------------------------------------------------------------*/

MQO_OBJECT* mqoCreateList(int num)
{
	MQO_OBJECT *obj;

	// 弶婜壔偝傟偰側偐偭偨傜弶婜壔
	if ( ! l_GLMetaseqInitialized ) mqoInit();

	// 椞堟妋曐偲弶婜壔
	obj = (MQO_OBJECT *)malloc(sizeof(MQO_OBJECT)*num);
	memset(obj, 0, sizeof(MQO_OBJECT)*num);

	return obj;
}


/*------------------------------------------------------------------------
[函数名]mqoCreateListObject
[功  能]创建mqo数组

[参  数]mqoobj		MQO对象
		i			
		filename	文件路径
		scale		放缩比例
		alpha		（0-255）

[返回值]-1/0 异常/正常
------------------------------------------------------------------------*/

int mqoCreateListObject(MQO_OBJECT *mqoobj, int i, char *filename, double scale, unsigned char alpha )
{
	int ret;
	ret = 0;
	if ( mqoobj == (MQO_OBJECT *)NULL ) return -1;
	if (! mqoLoadFile(&mqoobj[i], filename, scale, alpha)) ret = -1; 
	return ret;
}


/*------------------------------------------------------------------------
[函数名]mqoCallListObject
[功  能]调用mqo对象
[参  数]
		mqoobj		MQO对象
		num			编号

[返回值]无
------------------------------------------------------------------------*/

void mqoCallListObject(MQO_OBJECT mqoobj[],int num)
{

	MQO_INNER_OBJECT	*obj;
	MQO_MATERIAL		*mat;
	GLfloat				matenv[4];
	GLint				bindGL_TEXTURE_2D	= 0;
	GLboolean			isGL_TEXTURE_2D		= GL_FALSE;
	GLboolean			isGL_BLEND			= GL_FALSE;
	GLint				blendGL_SRC_ALPHA	= 0;
	GLint				intFrontFace;

	int		o, m, offset;
	double	dalpha;
	char	*base;

	if ( mqoobj == NULL) return;

	glPushMatrix();
		//儊僞僙僐偼捀揰偺暲傃偑昞柺偐傜傒偰塃夞傝
		glGetIntegerv(GL_FRONT_FACE,&intFrontFace);
		glFrontFace(GL_CW);
		dalpha = (double)mqoobj[num].alpha/(double)255;

		for ( o=0; o<mqoobj[num].objnum; o++ ) {	// 撪晹僆僽僕僃僋僩儖乕僾

			obj = &mqoobj[num].obj[o];
			if ( ! obj->isVisible ) continue;
			glShadeModel(((obj->isShadingFlat))?GL_FLAT:GL_SMOOTH);

			for ( m = 0; m < obj->matnum; m++ ) {	//儅僥儕傾儖儖乕僾

				mat = &obj->mat[m];
				if ( mat->datanum == 0 ) continue;

				if ( mat->isValidMaterialInfo ) {	// 儅僥儕傾儖偺忣曬愝掕
					memcpy(matenv,mat->dif,sizeof(matenv));
					matenv[3] *= dalpha;
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matenv);
					memcpy(matenv,mat->amb,sizeof(matenv));
					matenv[3] *= dalpha;
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matenv);
					memcpy(matenv,mat->spc,sizeof(matenv));
					matenv[3] *= dalpha;
					glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matenv);
					memcpy(matenv,mat->emi,sizeof(matenv));
					matenv[3] *= dalpha;
					glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matenv);
					glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat->power);
				}

				if ( mat->isUseTexture) {	// 僥僋僗僠儍偑偁傞応崌
					glEnableClientState( GL_VERTEX_ARRAY );
					glEnableClientState( GL_NORMAL_ARRAY );
					glEnableClientState( GL_TEXTURE_COORD_ARRAY );

					isGL_TEXTURE_2D = glIsEnabled(GL_TEXTURE_2D);
					isGL_BLEND = glIsEnabled(GL_BLEND);
					glGetIntegerv(GL_TEXTURE_BINDING_2D,&bindGL_TEXTURE_2D);
//					glGetIntegerv(GL_BLEND_SRC_ALPHA,&blendGL_SRC_ALPHA);

					glEnable(GL_TEXTURE_2D);
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

					glBindTexture(GL_TEXTURE_2D,mat->texture_id);

					if ( g_isVBOSupported ) {	// 捀揰僶僢僼傽巊梡
						base = (char *)NULL;	// 傾僪儗僗偼NULL偑愭摢
						glBindBufferARB( GL_ARRAY_BUFFER_ARB, mat->VBO_id ); // 捀揰僶僢僼傽傪寢傃偮偗傞
					}
					else {
						// 捀揰攝楍偺帪偼丄傾僪儗僗傪偦偺傑傑擖傟傞
						base = (char *)mat->vertex_t[0].point;
					}

					// 捀揰攝楍傪愝掕
					offset = (int)( (char *)mat->vertex_t[0].point - (char *)mat->vertex_t[0].point );
					glVertexPointer( 3, GL_FLOAT, sizeof(VERTEX_TEXUSE) , base + offset );

					// 僥僋僗僠儍嵗昗攝楍傪愝掕
					offset = (int)((char *)mat->vertex_t[0].uv-(char *)mat->vertex_t[0].point);
					glTexCoordPointer( 2, GL_FLOAT, sizeof(VERTEX_TEXUSE) , base + offset );

					// 朄慄攝楍傪愝掕
					offset = (int)((char *)mat->vertex_t[0].normal-(char *)mat->vertex_t[0].point);
					glNormalPointer( GL_FLOAT, sizeof(VERTEX_TEXUSE) , base+offset );

					// 怓愝掕
					glColor4f(mat->color[0],mat->color[1],mat->color[2],mat->color[3]);

					// 昤夋幚峴
					glDrawArrays( GL_TRIANGLES, 0, mat->datanum );

					glBindTexture(GL_TEXTURE_2D,bindGL_TEXTURE_2D);
					if( isGL_BLEND == GL_FALSE ) glDisable(GL_BLEND);
					if( isGL_TEXTURE_2D == GL_FALSE ) glDisable(GL_TEXTURE_2D);

					if ( g_isVBOSupported ) {						// 捀揰僶僢僼傽巊梡
						glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );	// 捀揰僶僢僼傽傪僨僼僅儖僩傊
					}

					glDisableClientState( GL_VERTEX_ARRAY );
					glDisableClientState( GL_NORMAL_ARRAY );
					glDisableClientState( GL_TEXTURE_COORD_ARRAY );
				}
				else {	// 僥僋僗僠儍偑側偄応崌

					glEnableClientState( GL_VERTEX_ARRAY );
					glEnableClientState( GL_NORMAL_ARRAY );
				//	glEnableClientState( GL_COLOR_ARRAY );

					isGL_BLEND = glIsEnabled(GL_BLEND);
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

					if ( g_isVBOSupported ) {	// 捀揰僶僢僼傽巊梡
						base = (char *)NULL;
						glBindBufferARB( GL_ARRAY_BUFFER_ARB, mat->VBO_id );
					}
					else {
						base = (char *)mat->vertex_p[0].point;
					}

					// 捀揰攝楍傪愝掕
					offset = (int)((char *)mat->vertex_p[0].point-(char *)mat->vertex_p[0].point);
					glVertexPointer( 3, GL_FLOAT, sizeof(VERTEX_NOTEX) , base+offset );

					// 朄慄攝楍傪愝掕
					offset = (int)((char *)mat->vertex_p[0].normal-(char *)mat->vertex_p[0].point);
					glNormalPointer( GL_FLOAT, sizeof(VERTEX_NOTEX) , base+offset );

					// 怓愝掕
					glColor4f(mat->color[0],mat->color[1],mat->color[2],mat->color[3]);
				//	offset = (int)((char *)mat->vertex_p[0].color-(char *)mat->vertex_p[0].point);
				//	glColorPointer(4,GL_FLOAT,sizeof(VERTEX_NOTEX),base+offset);

					// 昤夋幚峴
					glDrawArrays( GL_TRIANGLES, 0, mat->datanum );

					if( isGL_BLEND == GL_FALSE ) glDisable(GL_BLEND);
					if ( g_isVBOSupported ) {						// 捀揰僶僢僼傽巊梡
						glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );	// 捀揰僶僢僼傽傪僨僼僅儖僩傊
					}

				//	glDisableClientState( GL_COLOR_ARRAY );
					glDisableClientState( GL_VERTEX_ARRAY );
					glDisableClientState( GL_NORMAL_ARRAY );

				}
			}
		}

		//儊僞僙僐偼捀揰偺暲傃偑昞柺偐傜傒偰塃夞傝乮尦偺愝掕偵傕偳偡乯
		glFrontFace(intFrontFace);
	glPopMatrix();
}


/*------------------------------------------------------------------------
[函数名]mqoGetDirectory
[功  能]提取到文件路径
[参  数]
		*path_file	
		*path_dir	

[返回值]无
[备  注]
		"C:/data/file.bmp" 提取 "C:/data/"
		"data/file.mqo"    提取 "data/"
------------------------------------------------------------------------*/

void mqoGetDirectory(const char *path_file, char *path_dir)
{
	char *pStr;
	int len;

	pStr = MAX( strrchr(path_file,'\\'), strrchr(path_file,'/') );
	len = MAX((int)(pStr-path_file)+1,0);
	strncpy(path_dir,path_file,len);
	path_dir[len] = (char)0;
}


/*------------------------------------------------------------------------
[函数名]mqoSnormal
[功  能]貌似是求平面的单位法向量
[参  数]
		A		平面3点坐标
		B		
		C		
		*normal	单位法向量指针

[返回值]无
亂巇條亃儊僞僙僐僀傾偵偍偄偰柺傪峔惉偡傞捀揰偺斣崋偼丆昞帵柺偐傜尒偰
		帪寁夞傝偵婰弎偟偰偁傞丏偮傑傝丆捀揰A,B,C 偑偁偭偨偲偒丆
		媮傔傞傋偒朄慄偼BA偲BC偺奜愊偵傛偭偰媮傔傜傟傞
------------------------------------------------------------------------*/

void mqoSnormal(glPOINT3f A, glPOINT3f B, glPOINT3f C, glPOINT3f *normal)
{
	double norm;
	glPOINT3f vec0,vec1;

	// 儀僋僩儖BA
	vec0.x = A.x - B.x; 
	vec0.y = A.y - B.y;
	vec0.z = A.z - B.z;

	// 儀僋僩儖BC
	vec1.x = C.x - B.x;
	vec1.y = C.y - B.y;
	vec1.z = C.z - B.z;

	// 朄慄儀僋僩儖
	normal->x = vec0.y * vec1.z - vec0.z * vec1.y;
	normal->y = vec0.z * vec1.x - vec0.x * vec1.z;
	normal->z = vec0.x * vec1.y - vec0.y * vec1.x;

	// 惓婯壔
	norm = normal->x * normal->x + normal->y * normal->y + normal->z * normal->z;
	norm = sqrt ( norm );

	normal->x /= norm;
	normal->y /= norm;
	normal->z /= norm;
}


/*------------------------------------------------------------------------
[函数名]mqoReadMaterial
[功  能]阅读材料信息
[参  数]
		fp		文件指针
		M		阵列

[返回值]无
mqoCreateModel(), mqoCreateSequence() 子功能
------------------------------------------------------------------------*/

void mqoReadMaterial(FILE *fp, MQO_MATDATA M[])
{
	GLfloat		dif, amb, emi, spc;
	glCOLOR4f	c;
	char		buf[SIZE_STR];
	char		*pStrEnd, *pStr;
	int			len;
	int			i = 0;

	while (1) {
		fgets(buf,SIZE_STR,fp);	// 峴撉傒崬傒
		if (strstr(buf,"}")) break;

		pStr = strstr(buf,"col(");	// 嵽幙柤撉傒旘偽偟
		sscanf( pStr,
				"col(%f %f %f %f) dif (%f) amb(%f) emi(%f) spc(%f) power(%f)",
				&c.r, &c.g, &c.b, &c.a, &dif, &amb, &emi, &spc, &M[i].power );

		// 捀揰僇儔乕
		M[i].col = c;

		// 奼嶶岝
		M[i].dif[0] = dif * c.r;
		M[i].dif[1] = dif * c.g;
		M[i].dif[2] = dif * c.b;
		M[i].dif[3] = c.a;

		// 廃埻岝
		M[i].amb[0] = amb * c.r;
		M[i].amb[1] = amb * c.g;
		M[i].amb[2] = amb * c.b;
		M[i].amb[3] = c.a;

		// 帺屓徠柧
		M[i].emi[0] = emi * c.r;
		M[i].emi[1] = emi * c.g;
		M[i].emi[2] = emi * c.b;
		M[i].emi[3] = c.a;

		// 斀幩岝
		M[i].spc[0] = spc * c.r;
		M[i].spc[1] = spc * c.g;
		M[i].spc[2] = spc * c.b;
		M[i].spc[3] = c.a;
		
		// tex丗柾條儅僢僺儞僌柤
		if ( (pStr = strstr(buf,"tex(")) != NULL ) {
			M[i].useTex = TRUE;

			pStrEnd = strstr(pStr,")")-1;
			len = pStrEnd - (pStr+5);
			strncpy(M[i].texFile,pStr+5,len);
			M[i].texFile[len] = (char)0;
			if ( (pStr = strstr(buf,"aplane(")) != NULL ) {
				pStrEnd = strstr(pStr,")")-1;
				len = pStrEnd - (pStr+8);
				strncpy(M[i].alpFile,pStr+8,len);
				M[i].alpFile[len] = (char)0;
			}
			else {
				M[i].alpFile[0] = (char)0;
			}

		} else {
			M[i].useTex = FALSE;
			M[i].texFile[0] = (char)0;
			M[i].alpFile[0] = (char)0;
		}

		i++;
	}

}


/*------------------------------------------------------------------------
[函数名]mqoReadVertex
[功  能]读取顶点信息
[参  数]
		fp		文件指针
		V		顶点阵列
		
[返回值]无
mqoReadObject()子函数
------------------------------------------------------------------------*/

void mqoReadVertex(FILE *fp, glPOINT3f V[])
{
	char buf[SIZE_STR];
	int  i=0;

	while (1) {
		fgets(buf,SIZE_STR,fp);
		if (strstr(buf,"}")) break;
		sscanf(buf,"%f %f %f",&V[i].x,&V[i].y,&V[i].z);
		i++;
	}
}


/*------------------------------------------------------------------------
[函数名]mqoReadBVertex
[功  能]二进制格式顶点信息
[参  数]
		fp		文件指针
		V		顶点数组

[返回值]无
mqoReadObject()子函数
------------------------------------------------------------------------*/

int mqoReadBVertex(FILE *fp, glPOINT3f V[])
{
	int n_vertex,i;
	float *wf;
	int size;
	char cw[256];
	char *pStr;

	fgets(cw,sizeof(cw),fp);
	if ( (pStr = strstr(cw,"Vector")) != NULL ) {
		sscanf(pStr,"Vector %d [%d]",&n_vertex,&size);	// 捀揰悢丄僨乕僞僒僀僘傪撉傒崬傓
	}
	else {
		return -1;
	}
	//MQO僼傽僀儖偺僶僀僫儕捀揰僨乕僞偼intel宍幃乮儕僩儖僄僨傿傾儞乯
	wf = (float *)malloc(size);
	fread(wf,size,1,fp);
	for ( i = 0; i < n_vertex; i++ ) {
		V[i].x = wf[i*3+0];
		V[i].y = wf[i*3+1];
		V[i].z = wf[i*3+2];
#if DEF_IS_LITTLE_ENDIAN
#else
		endianConverter((void *)&V[i].x,sizeof(V[i].x));
		endianConverter(&V[i].y,sizeof(V[i].y));
		endianConverter(&V[i].z,sizeof(V[i].z));
#endif
	}
	free(wf);

	// "}"傑偱撉傒旘偽偟
	{
		char buf[SIZE_STR];
		while (1) {
			fgets(buf,SIZE_STR,fp);
			if (strstr(buf,"}")) break;
		}
	}

	return n_vertex;
}


/*------------------------------------------------------------------------
[函数名]mqoReadFace
[功  能]读取面信息
[参  数]
		fp		文件指针
		F		面阵列
		
[返回值]无
mqoReadObject()子函数
------------------------------------------------------------------------*/

void mqoReadFace(FILE *fp, MQO_FACE F[])
{
	char buf[SIZE_STR];
	char *pStr;
	int  i=0;

	while (1) {
		fgets(buf,SIZE_STR,fp);
		if (strstr(buf,"}")) break;

		// 柺傪峔惉偡傞捀揰悢
		sscanf(buf,"%d",&F[i].n);

		// 捀揰(V)偺撉傒崬傒
		if ( (pStr = strstr(buf,"V(")) != NULL ) {
			switch (F[i].n) {
				case 3:
//儊僞僙僐偼捀揰偺暲傃偑昞柺偐傜傒偰塃夞傝
//撉傒崬傒帪偵暲傋懼偊傞曽朄傕偁傞丅偗偳丄昞柺偺愝掕傪
//glFrontFace偱曄偊傞傎偆偑僗儅乕僩丠
					sscanf(pStr,"V(%d %d %d)",&F[i].v[0],&F[i].v[1],&F[i].v[2]);
//					sscanf(pStr,"V(%d %d %d)",&F[i].v[2],&F[i].v[1],&F[i].v[0]);
					break;
				case 4:
					sscanf(pStr,"V(%d %d %d %d)",&F[i].v[0],&F[i].v[1],&F[i].v[2],&F[i].v[3]);
//					sscanf(pStr,"V(%d %d %d %d)",&F[i].v[3],&F[i].v[2],&F[i].v[1],&F[i].v[0]);
					break;
				default:
					break;
			}		
		}

		// 儅僥儕傾儖(M)偺撉傒崬傒
		F[i].m = 0;
		if ( (pStr = strstr(buf,"M(")) != NULL ) {
			sscanf(pStr,"M(%d)",&F[i].m);
		}
		else { // 儅僥儕傾儖偑愝掕偝傟偰偄側偄柺
			F[i].m = -1;
		}

		// UV儅僢僾(UV)偺撉傒崬傒
		if ( (pStr = strstr(buf,"UV(")) != NULL ) {
			switch (F[i].n) {
				case 3:	// 捀揰悢3
					sscanf(pStr,"UV(%f %f %f %f %f %f)",
									&F[i].uv[0].x, &F[i].uv[0].y,
									&F[i].uv[1].x, &F[i].uv[1].y,
									&F[i].uv[2].x, &F[i].uv[2].y
									);
					break;

				case 4:	// 捀揰悢4
					sscanf(pStr,"UV(%f %f %f %f %f %f %f %f)",
									&F[i].uv[0].x, &F[i].uv[0].y,
									&F[i].uv[1].x, &F[i].uv[1].y,
									&F[i].uv[2].x, &F[i].uv[2].y,
									&F[i].uv[3].x, &F[i].uv[3].y
									);
					break;
				default:
					break;
			}		
		}

		i++;
	}

}


/*------------------------------------------------------------------------
[函数名]mqoReadObject
[功  能]读取对象信息
[参  数]
		fp		文件指针
		obj		对象阵列

[返回值]无
亂巇條亃偙偺娭悢偱侾屄偺僆僽僕僃僋僩忣曬偑撉傒崬傑傟傞丏
------------------------------------------------------------------------*/

void mqoReadObject(FILE *fp, MQO_OBJDATA *obj)
{
	char buf[SIZE_STR];

	while (1) {
		fgets(buf,SIZE_STR,fp);
		if (strstr(buf,"}")) break;

		// visible
		if (strstr(buf,"visible ")) {
			sscanf(buf," visible %d", &obj->visible);
		}

		// shading
		if (strstr(buf,"shading ")) {
			sscanf(buf," shading %d", &obj->shading);
		}

		// facet
		if (strstr(buf,"facet ")) {
			sscanf(buf," facet %f", &obj->facet);
		}

		// vertex
		if (strstr(buf,"vertex ")) {
			sscanf(buf," vertex %d", &obj->n_vertex);
			obj->V = (glPOINT3f*) calloc( obj->n_vertex, sizeof(glPOINT3f) );
			mqoReadVertex(fp, obj->V);
		}
		// BVertex
		if (strstr(buf,"BVertex"))	{
			sscanf(buf," BVertex %d", &obj->n_vertex);
			obj->V = (glPOINT3f*) calloc( obj->n_vertex, sizeof(glPOINT3f) );
			mqoReadBVertex(fp,obj->V);
		}

		// face
		if (strstr(buf,"face ")) {
			sscanf(buf," face %d", &obj->n_face);
			obj->F = (MQO_FACE*) calloc( obj->n_face, sizeof(MQO_FACE) );
			mqoReadFace(fp, obj->F);
		}

	}

}


/*------------------------------------------------------------------------
[函数名]mqoMakeArray
[功  能]创建数组
[参  数]
		mat		材质
		matpos	
		F		面
		fnum	
		V		顶点
		N		
		facet	平滑面角
		mcol	
		scale	比例
		alpha	

[返回值]无
亂巇條亃捀揰攝楍偼偡傋偰嶰妏偵偡傞偺偱丄巐妏偼嶰妏倶俀偵暘妱
		  0  3      0    0  3
		   仩   仺丂仮丂丂仱
		  1  2     1  2   2  
------------------------------------------------------------------------*/

void mqoMakeArray( MQO_MATERIAL *mat, int matpos, MQO_FACE F[], int fnum,glPOINT3f V[],
				   glPOINT3f N[], double facet, glCOLOR4f *mcol, double scale, unsigned char alpha )
{
	int f;
	int i;
	int dpos;
	double s;
	glPOINT3f normal;	// 朄慄儀僋僩儖
	
	dpos = 0;
	mat->color[0] = mcol->r;
	mat->color[1] = mcol->g;
	mat->color[2] = mcol->b;
	mat->color[3] = mcol->a;
	if ( mat->isUseTexture ) {
		for ( f = 0; f < fnum; f++ ){
			if ( F[f].m != matpos ) continue;
			if ( F[f].n == 3 ) {
				mqoSnormal(V[F[f].v[0]],V[F[f].v[1]],V[F[f].v[2]],&normal);	// 朄慄儀僋僩儖傪寁嶼
				for ( i = 0; i < 3; i++ ) {
					mat->vertex_t[dpos].point[0] = V[F[f].v[i]].x*scale;
					mat->vertex_t[dpos].point[1] = V[F[f].v[i]].y*scale;
					mat->vertex_t[dpos].point[2] = V[F[f].v[i]].z*scale;
					mat->vertex_t[dpos].uv[0] = F[f].uv[i].x;
					mat->vertex_t[dpos].uv[1] = F[f].uv[i].y;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if ( facet < s ) {
						// 僗儉乕僕儞僌妏丂亙乮捀揰朄慄偲柺朄慄偺妏搙乯偺偲偒偼柺朄慄傪捀揰朄慄偲偡傞
						mat->vertex_t[dpos].normal[0] = normal.x;
						mat->vertex_t[dpos].normal[1] = normal.y;
						mat->vertex_t[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_t[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_t[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_t[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
			}
			//係捀揰乮巐妏乯偼俁捀揰乮嶰妏乯倶俀偵暘妱
			if ( F[f].n == 4 ) {
				mqoSnormal(V[F[f].v[0]],V[F[f].v[1]],V[F[f].v[2]],&normal);	// 朄慄儀僋僩儖傪寁嶼
				for ( i = 0; i < 4; i++ ) {
					if ( i == 3 ) continue;
					mat->vertex_t[dpos].point[0] = V[F[f].v[i]].x*scale;
					mat->vertex_t[dpos].point[1] = V[F[f].v[i]].y*scale;
					mat->vertex_t[dpos].point[2] = V[F[f].v[i]].z*scale;
					mat->vertex_t[dpos].uv[0] = F[f].uv[i].x;
					mat->vertex_t[dpos].uv[1] = F[f].uv[i].y;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if ( facet < s ) {
						mat->vertex_t[dpos].normal[0] = normal.x;
						mat->vertex_t[dpos].normal[1] = normal.y;
						mat->vertex_t[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_t[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_t[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_t[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
				mqoSnormal(V[F[f].v[0]],V[F[f].v[2]],V[F[f].v[3]],&normal);	// 朄慄儀僋僩儖傪寁嶼
				for ( i = 0; i < 4; i++ ) {
					if ( i == 1 ) continue;
					mat->vertex_t[dpos].point[0] = V[F[f].v[i]].x*scale;
					mat->vertex_t[dpos].point[1] = V[F[f].v[i]].y*scale;
					mat->vertex_t[dpos].point[2] = V[F[f].v[i]].z*scale;
					mat->vertex_t[dpos].uv[0] = F[f].uv[i].x;
					mat->vertex_t[dpos].uv[1] = F[f].uv[i].y;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if ( facet < s ) {
						mat->vertex_t[dpos].normal[0] = normal.x;
						mat->vertex_t[dpos].normal[1] = normal.y;
						mat->vertex_t[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_t[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_t[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_t[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
			}
		}
	}
	else {
		if ( alpha != 255 ) {
			mat->color[3] = (double)alpha/(double)255;
		}
		for ( f = 0; f < fnum; f++ ){
			if ( F[f].m != matpos ) continue;
			if ( F[f].n == 3 ) {
				mqoSnormal(V[F[f].v[0]],V[F[f].v[1]],V[F[f].v[2]],&normal);		// 朄慄儀僋僩儖傪寁嶼
				for ( i = 0; i < 3; i++ ) {
					mat->vertex_p[dpos].point[0] = V[F[f].v[i]].x*scale;
					mat->vertex_p[dpos].point[1] = V[F[f].v[i]].y*scale;
					mat->vertex_p[dpos].point[2] = V[F[f].v[i]].z*scale;
					mat->vertex_p[dpos].normal[0] = normal.x;
					mat->vertex_p[dpos].normal[1] = normal.y;
					mat->vertex_p[dpos].normal[2] = normal.z;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if ( facet < s ) {
						mat->vertex_p[dpos].normal[0] = normal.x;
						mat->vertex_p[dpos].normal[1] = normal.y;
						mat->vertex_p[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_p[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_p[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_p[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
			}
			//係捀揰乮巐妏乯偼俁捀揰乮嶰妏乯倶俀偵暘妱
			if ( F[f].n == 4 ) {
				mqoSnormal(V[F[f].v[0]],V[F[f].v[1]],V[F[f].v[2]],&normal);		// 朄慄儀僋僩儖傪寁嶼
				for ( i = 0; i < 4; i++ ) {
					if ( i == 3 ) continue;
					mat->vertex_p[dpos].point[0] = V[F[f].v[i]].x*scale;
					mat->vertex_p[dpos].point[1] = V[F[f].v[i]].y*scale;
					mat->vertex_p[dpos].point[2] = V[F[f].v[i]].z*scale;
					mat->vertex_p[dpos].normal[0] = normal.x;
					mat->vertex_p[dpos].normal[1] = normal.y;
					mat->vertex_p[dpos].normal[2] = normal.z;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if ( facet < s ) {
						mat->vertex_p[dpos].normal[0] = normal.x;
						mat->vertex_p[dpos].normal[1] = normal.y;
						mat->vertex_p[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_p[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_p[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_p[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
				mqoSnormal(V[F[f].v[0]],V[F[f].v[2]],V[F[f].v[3]],&normal);		// 朄慄儀僋僩儖傪寁嶼
				for ( i = 0; i < 4; i++ ) {
					if ( i == 1 ) continue;
					mat->vertex_p[dpos].point[0] = V[F[f].v[i]].x*scale;
					mat->vertex_p[dpos].point[1] = V[F[f].v[i]].y*scale;
					mat->vertex_p[dpos].point[2] = V[F[f].v[i]].z*scale;
					mat->vertex_p[dpos].normal[0] = normal.x;
					mat->vertex_p[dpos].normal[1] = normal.y;
					mat->vertex_p[dpos].normal[2] = normal.z;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if ( facet < s ) {
						mat->vertex_p[dpos].normal[0] = normal.x;
						mat->vertex_p[dpos].normal[1] = normal.y;
						mat->vertex_p[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_p[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_p[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_p[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
			}
		}
	}
}


/*------------------------------------------------------------------------
[函数名]mqoVertexNormal
[功  能]捀揰朄慄偺寁嶼
[参  数]
		obj			僆僽僕僃僋僩忣曬

[返回值]朄慄攝楍
亂巇條亃係捀揰偺柺偼嶰妏宍偵暘妱偟偰寁嶼
		栠傝抣偼昁偢屇傃弌偟尦偱夝曻乮free乯偡傞偙偲両
------------------------------------------------------------------------*/

glPOINT3f * mqoVertexNormal(MQO_OBJDATA *obj)
{
	int f;
	int v;
	int i;
	double len;
	glPOINT3f fnormal;	// 柺朄慄儀僋僩儖
	MQO_FACE *F;
	glPOINT3f *V;
	glPOINT3f *ret;
	F = obj->F;
	V = obj->V;
	ret = (glPOINT3f *)calloc(obj->n_vertex,sizeof(glPOINT3f));
	//柺偺朄慄傪捀揰偵懌偟崬傒
	for ( f = 0; f < obj->n_face; f++ ) {
		if ( obj->F[f].n == 3 ) {
			mqoSnormal(V[F[f].v[0]],V[F[f].v[1]],V[F[f].v[2]],&fnormal);
			for ( i = 0; i < 3; i++ ) {
				ret[F[f].v[i]].x += fnormal.x;
				ret[F[f].v[i]].y += fnormal.y;
				ret[F[f].v[i]].z += fnormal.z;
			}
		}
		if ( obj->F[f].n == 4 ) {
			mqoSnormal(V[F[f].v[0]],V[F[f].v[1]],V[F[f].v[2]],&fnormal);
			for ( i = 0; i < 4; i++ ) {
				if ( i == 3 ) continue;
				ret[F[f].v[i]].x += fnormal.x;
				ret[F[f].v[i]].y += fnormal.y;
				ret[F[f].v[i]].z += fnormal.z;
			}
			mqoSnormal(V[F[f].v[0]],V[F[f].v[2]],V[F[f].v[3]],&fnormal);
			for ( i = 0; i < 4; i++ ) {
				if ( i == 1 ) continue;
				ret[F[f].v[i]].x += fnormal.x;
				ret[F[f].v[i]].y += fnormal.y;
				ret[F[f].v[i]].z += fnormal.z;
			}
		}
	}
	//惓婯壔
	for ( v = 0; v < obj->n_vertex; v++ ) {
		if ( ret[v].x == 0 && ret[v].y == 0 && ret[v].z == 0 ) {
			//柺偵巊傢傟偰側偄揰
			continue;
		}
		len = sqrt(ret[v].x*ret[v].x + ret[v].y*ret[v].y + ret[v].z*ret[v].z);
		if ( len != 0 ) {
			ret[v].x = ret[v].x/len;
			ret[v].y = ret[v].y/len;
			ret[v].z = ret[v].z/len;
		}
	}

	return ret;
}

/*------------------------------------------------------------------------
[函数名]mqoMakePolygon
[功  能]生成多边形
[参  数]
		readObj		撉傒崬傫偩僆僽僕僃僋僩忣曬
		mqoobj		MQO僆僽僕僃僋僩 
		N[]			朄慄攝楍
		M[]			儅僥儕傾儖攝楍
		n_mat		儅僥儕傾儖悢
		scale		僗働乕儖
		alpha		傾儖僼傽

[返回值]側偟
------------------------------------------------------------------------*/

void mqoMakePolygon(MQO_OBJDATA *readObj, MQO_OBJECT *mqoobj,
					glPOINT3f N[], MQO_MATDATA M[], int n_mat, double scale, unsigned char alpha)
{

	MQO_INNER_OBJECT		*setObj;
	MQO_MATERIAL			*material;
	glCOLOR4f				defcol;
	glCOLOR4f				*pcol;
	int						f, m, *mat_vnum;
	int						fnum;
	MQO_FACE				*F;
	glPOINT3f				*V;
	double					facet;


	setObj = &mqoobj->obj[mqoobj->objnum];
	strcpy(setObj->objname,readObj->objname);
	setObj->isVisible = readObj->visible;
	setObj->isShadingFlat = (readObj->shading == 0);
	F = readObj->F;
	fnum = readObj->n_face;
	V = readObj->V;
	facet = readObj->facet;

	// face偺拞偱偺儅僥儕傾儖枅偺捀揰偺悢
	// M=NULL偺偲偒丄F[].m = 0 偑擖偭偰偔傞
	if ( M == NULL ) n_mat = 1;

	mat_vnum = (int *)malloc(sizeof(int)*n_mat);
	memset(mat_vnum,0,sizeof(int)*n_mat);

	for ( f = 0; f < fnum; f++ ){
		if( F[f].m < 0 ) continue; // 儅僥儕傾儖偑愝掕偝傟偰偄側偄柺
		if ( F[f].n == 3 ) {
			mat_vnum[F[f].m] += 3;
		}
		if ( F[f].n == 4 ) {
			//係捀揰乮巐妏乯偼俁捀揰乮嶰妏乯倶俀偵暘妱
			//  0  3      0    0  3
			//   仩   仺丂仮丂丂仱
			//  1  2     1  2   2
			// 係捀揰偺暯柺僨乕僞偼
			// 俁捀揰偺暯柺僨乕僞倶俀屄
			mat_vnum[F[f].m] += 3*2;
		}
		if ( setObj->matnum < F[f].m+1 ) setObj->matnum = F[f].m+1;
	}

	// 儅僥儕傾儖暿偵捀揰攝楍傪嶌惉偡傞
	setObj->mat = (MQO_MATERIAL *)malloc(sizeof(MQO_MATERIAL)*setObj->matnum);
	memset(setObj->mat,0,sizeof(MQO_MATERIAL)*setObj->matnum);

	for ( m = 0; m < setObj->matnum; m++ ) {
		material = &setObj->mat[m];
		material->datanum = mat_vnum[m];
		material->isValidMaterialInfo = (M != NULL);

		if ( mat_vnum[m] <= 0 ) continue;
		if ( material->isValidMaterialInfo ) {
			memcpy(material->dif,M[m].dif,sizeof(material->dif));
			memcpy(material->amb,M[m].amb,sizeof(material->amb));
			memcpy(material->spc,M[m].spc,sizeof(material->spc));
			memcpy(material->emi,M[m].emi,sizeof(material->emi));
			material->power = M[m].power;
			material->isUseTexture = M[m].useTex;
			pcol = &M[m].col;
		}
		else {
			defcol.r = 1.0;
			defcol.g = 1.0;
			defcol.b = 1.0;
			defcol.a = 1.0;
			material->isUseTexture = 0;
			pcol = &defcol;
		}
		if ( material->isUseTexture ) {
			material->vertex_t = (VERTEX_TEXUSE *)calloc(material->datanum,sizeof(VERTEX_TEXUSE));
			material->texture_id = M[m].texName;
		}
		else {
			material->vertex_p = (VERTEX_NOTEX *)calloc(material->datanum,sizeof(VERTEX_NOTEX));
		}
		mqoMakeArray(material,m,F,fnum,V,N,facet,pcol,scale,alpha);
		if (g_isVBOSupported) {
			if ( material->isUseTexture ) {
				glGenBuffersARB( 1, &material->VBO_id );
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, material->VBO_id  );
				glBufferDataARB( GL_ARRAY_BUFFER_ARB, material->datanum*sizeof(VERTEX_TEXUSE), material->vertex_t, GL_STATIC_DRAW_ARB );
			}
			else {
				glGenBuffersARB( 1, &material->VBO_id );
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, material->VBO_id  );
				glBufferDataARB( GL_ARRAY_BUFFER_ARB, material->datanum*sizeof(VERTEX_NOTEX), material->vertex_p, GL_STATIC_DRAW_ARB );
			}
		}
	}
	mqoobj->objnum++;
	if ( MAX_OBJECT <= mqoobj->objnum ) {
		printf("MQO僼傽僀儖撉傒崬傒丗丂嵟戝僆僽僕僃僋僩悢傪挻偊傑偟偨[%d]\n",mqoobj->objnum);
		mqoobj->objnum = MAX_OBJECT-1;
	}

	free(mat_vnum);

}


/*------------------------------------------------------------------------
[函数名]mqoMakeObjectsEx
[功  能]从数据创建多边形对象
[参  数]
		mqoobj	MQO僆僽僕僃僋僩
		obj		僆僽僕僃僋僩攝楍
		n_obj	僆僽僕僃僋僩偺屄悢
		M		儅僥儕傾儖攝楍
		n_mat	儅僥儕傾儖偺屄悢
		scale	奼戝棪
		alpha	傾儖僼傽

[返回值]无
------------------------------------------------------------------------*/

void mqoMakeObjectsEx( MQO_OBJECT *mqoobj, MQO_OBJDATA obj[], int n_obj, MQO_MATDATA M[],int n_mat,
					   double scale,unsigned char alpha)
{
	int i;
	glPOINT3f *N;
	for (i=0; i<n_obj; i++) {
		N = mqoVertexNormal(&obj[i]);
		mqoMakePolygon(
				&obj[i],
				mqoobj,
				N,
				M,n_mat,
				scale,
				alpha);
		free(N);
	}
}


/*------------------------------------------------------------------------
[函数名]mqoCreateModel
[功  能]MQO僼傽僀儖偐傜MQO儌僨儖傪嶌惉偡傞
[参  数]
		filename	MQO僼傽僀儖
		scale		奼戝棪乮1.0偱偦偺傑傑乯

[返回值]MQO_MODEL乮MQO儌僨儖乯
------------------------------------------------------------------------*/

MQO_MODEL mqoCreateModel(char *filename, double scale)
{
	MQO_MODEL ret;
	ret = mqoCreateList(1);
	if ( mqoCreateListObject(ret,1-1,filename,scale,(unsigned char)255) < 0 ) {
		mqoDeleteObject(ret,1);
		ret = NULL;
	}
	return ret;
}


/*------------------------------------------------------------------------
[函数名]mqoCreateSequenceEx
[功  能]楢斣偺MQO僼傽僀儖偐傜MQO僔乕働儞僗傪嶌惉偡傞
[参  数]
		format		僼傽僀儖柤偺彂幃
		n_file		僼傽僀儖悢
		scale		奼戝棪乮1.0偱偦偺傑傑乯
		fade_inout	0:偦偺傑傑丂惓丗僼僃乕僪僀儞丂晧丗僼僃乕僪傾僂僩
					愨懳抣偼岠壥傪偐偗傞僼儗乕儉悢
		alpha		傾儖僼傽

[返回值]MQO_SEQUENCE乮MQO僔乕働儞僗乯
[备  注]楢斣偼0偐傜奐巒
------------------------------------------------------------------------*/

MQO_SEQUENCE mqoCreateSequenceEx(const char *format, int n_file, double scale,
								 int fade_inout, unsigned char alpha)
{
	MQO_SEQUENCE retSeq;
	int iret;
	int seq;
	char filename[SIZE_STR];
	short setAlpha;
	short calAlpha;
	int frames;

	retSeq.n_frame = 0;
	if ( format == NULL ) {
		return retSeq;
	}
	calAlpha = alpha;
	frames = abs(fade_inout);
	frames = MAX(frames,n_file);
	setAlpha = (fade_inout<=0)?alpha:0;

	retSeq.model = mqoCreateList(n_file);
	for ( seq = 0; seq < frames; seq++ ) {
		if ( seq < n_file ) {
			sprintf(filename,format,seq);
		}
		if ( (fade_inout !=  0) && ((frames-1) == seq) ) {
			setAlpha = (fade_inout<0)?0:calAlpha;
		}
		iret = mqoCreateListObject(retSeq.model,seq,filename,scale,(unsigned char)setAlpha);
		if ( iret == - 1 ) {
			seq--;
			mqoClearObject(retSeq.model,seq,n_file-seq);
			break;
		}
		if ( fade_inout !=  0 ) {
			if ( fade_inout<0 ) {
				if ( (n_file-seq) <= (-1*fade_inout) ) {
					setAlpha -= (calAlpha/(-1*fade_inout));
					if ( setAlpha < 0 ) setAlpha = 0;
				}
			}
			else {
				setAlpha += (calAlpha/fade_inout);
				if ( calAlpha < setAlpha ) setAlpha = calAlpha;
			}
		}
	}
	retSeq.n_frame = seq;
	return retSeq;
}


/*------------------------------------------------------------------------
[函数名]mqoCreateSequence
[功  能]楢斣偺MQO僼傽僀儖偐傜MQO僔乕働儞僗傪嶌惉偡傞
[参  数]
		format		僼傽僀儖柤偺僼僅乕儅僢僩
		n_file		僼傽僀儖悢
		scale		奼戝棪乮1.0偱偦偺傑傑乯

[返回值]MQO_SEQUENCE乮MQO僔乕働儞僗乯
[备  注]楢斣偼0偐傜奐巒
------------------------------------------------------------------------*/

MQO_SEQUENCE mqoCreateSequence(const char *format, int n_file, double scale)
{
	return mqoCreateSequenceEx(format, n_file, scale, 0, (unsigned char)255);
}


/*------------------------------------------------------------------------
[函数名]mqoCallModel
[功  能]MQO儌僨儖傪OpenGL偺夋柺忋偵屇傃弌偡
[参  数]
		model		MQO儌僨儖

[返回值]側偟
------------------------------------------------------------------------*/

void mqoCallModel(MQO_MODEL model)
{
	mqoCallListObject(model, 0);
}


/*------------------------------------------------------------------------
[函数名]mqoCallSequence
[功  能]MQO僔乕働儞僗傪OpenGL偺夋柺偵屇傃弌偡
[参  数]
		seq		MQO僔乕働儞僗
		i		僼儗乕儉斣崋

[返回值]側偟
亂巇條亃MQO僔乕働儞僗偺拞偐傜巜掕偟偨僼儗乕儉斣崋偺儌僨儖傪屇傃弌偡
------------------------------------------------------------------------*/

void mqoCallSequence(MQO_SEQUENCE seq, int i)
{
	if ( i>=0 && i<seq.n_frame ) {
		mqoCallListObject(seq.model,i);
	}
}


/*------------------------------------------------------------------------
[函数名]mqoClearObject
[功  能]MQO僆僽僕僃僋僩偺僋儕傾
[参  数]
		object	MQO僆僽僕僃僋僩攝楍
		from	嶍彍奐巒斣崋乮0乣乯
		num		嶍彍偡傞屄悢

[返回值]側偟
------------------------------------------------------------------------*/

void mqoClearObject( MQO_OBJECT object[], int from, int num ) 
{
	int					loop, o, m;
	MQO_INNER_OBJECT	*obj;
	MQO_MATERIAL		*mat;

	if ( object == NULL ) return;

	for ( loop = from; loop < from + num; loop++ ) {
		for ( o = 0; o < (object+from)->objnum; o++ ) {

			obj = &(object+loop)->obj[o];
			for ( m = 0; m < obj->matnum; m++ ) {
				mat = &obj->mat[m];
				if ( mat->datanum <= 0 ) continue;
				if ( g_isVBOSupported ) {
					// 捀揰僶僢僼傽偺嶍彍
					glDeleteBuffersARB( 1, &mat->VBO_id );
				}

				// 捀揰攝楍偺嶍彍
				if ( mat->isUseTexture ) {
					if ( mat->vertex_t != NULL ) {
						free(mat->vertex_t);
						mat->vertex_t = NULL;
					}
				}
				else {
					if ( mat->vertex_p != NULL ) {
						free(mat->vertex_p);
						mat->vertex_p = NULL;
					}
				}
			}
			if ( obj->mat != NULL ) {
				free(obj->mat);
				obj->mat = NULL;
			}
			obj->matnum = 0;
		}
	}

}


/*------------------------------------------------------------------------
[函数名]mqoDeleteObject
[功  能]MQO僆僽僕僃僋僩傪嶍彍偡傞
[参  数]
		object	MQO僆僽僕僃僋僩攝楍
		num		嶍彍偡傞屄悢

[返回值]側偟
------------------------------------------------------------------------*/

void mqoDeleteObject(MQO_OBJECT object[], int num)
{
	mqoClearObject(object,0,num);
	free(object);
}


/*------------------------------------------------------------------------
[函数名]mqoDeleteModel
[功  能]MQO儌僨儖傪嶍彍偡傞
[参  数]
		model	MQO儌僨儖

[返回值]側偟
[备  注]嶍彍張棟傪峴偭偨曄悢傪嵞棙梡偡傞壜擻惈偑偁傞応崌偼
		偙偺娭悢偺幚峴屻偵NULL傪戙擖偟偰偍偔偙偲
------------------------------------------------------------------------*/

void mqoDeleteModel(MQO_MODEL model)
{
	mqoDeleteObject(model,1);
}


/*------------------------------------------------------------------------
[函数名]mqoDeleteSequence
[功  能]MQO僔乕働儞僗傪嶍彍偡傞
[参  数]
		seq		MQO僔乕働儞僗

[备  注]嶍彍張棟傪峴偭偨曄悢傪嵞棙梡偡傞壜擻惈偑偁傞応崌偼
		偙偺娭悢偺幚峴屻偵NULL傪戙擖偟偰偍偔偙偲
------------------------------------------------------------------------*/

void mqoDeleteSequence(MQO_SEQUENCE seq)
{
	mqoDeleteObject( seq.model, seq.n_frame );
}
