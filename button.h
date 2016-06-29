#ifndef _BUTTON_H
#define _BUTTON_H

#define		BUTTON_POOL_MAXSIZE		30  //maxsize of button pool

#include <stdlib.h>

class BUTTON {
	char	name[256];			//name
	int		id;					//id
	double	size_X,size_Z;		//length of weight and height
	char	texture[3][256];	//path of texture file
	int		button_texture[3];	//texture id
	double	pos_x,pos_y,pos_z;	//world (x,y,z)
	int		state;				//0:unchecked 1:checked but inactive 2:active
	int		visible;			//0:invisible 1:visible
	void	(*callback)(void);	//callback function
	int		worked;				//0:wait to word 1:tired and donot want to work

public:
	BUTTON(char _name[]="NONE",int _id=-1,double _size_X=100,double _size_Z=10,char _texture[][256]=NULL,double _x=0,double _y=0,double _z=0,void (*_callback)(void)=NULL);
	~BUTTON();

	void	set_Name(char *_name);
	void	set_Id(int _id);
	void	set_Size(double _x,double _z);
	void	set_Texture(char _texture[][256]);
	void	set_Pos(double _x,double _y,double _z);
	void	set_State(int _state);
	void	set_Visible(int _visible);
	void	set_Callback(void (*_callback)(void));
	
	void	draw_Button(void);
	void	check_State(void);
	double  caldis(double pos[]);

	friend	class BUTTON_POOL;
};

class BUTTON_POOL {
	char			name[256];
	int				id;
	int				active_button_id;
	int				preactive_button_id;
	int				timecount;
	int				deadline;
	double			dis[BUTTON_POOL_MAXSIZE];
	BUTTON			*button[BUTTON_POOL_MAXSIZE];
	int				button_num;
	
public:
	BUTTON_POOL(char _name[]="NONE",int _id=-1,int dl=30);
	~BUTTON_POOL();
	
	BUTTON	*register_Button(char name[],double sx,double sy,char t[][256],double x,double y,double z,void (*_callback)(void));
	void	draw_ButtonPool(void);
	void	check_ButtonPool(double pos[]);
};

BUTTON *register_Button(char name[],double sx,double sy,char t[][256],double x,double y,double z,void (*_callback)(void));
void	draw_ButtonPool(void);
void	check_ButtonPool(double pos[]);

#endif
