#ifndef _AIT_H
#define _AIT_H

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

typedef struct MUSIC {
	char name[256];
	char path[256];
};

Mix_Chunk *load_sound(char *);
void music_Init(void);
void music_Play(MUSIC *music); //播放音乐 play music ,with looping
void music_Pause(void); //暂停音乐 pause music 
void music_Resume(void); //恢复播放 resume paused music
void music_Rewind(void); //重新播放 rewind music to beginning
void music_Quit(void);

#endif 
