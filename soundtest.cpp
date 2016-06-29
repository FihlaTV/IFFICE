#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

Mix_Music *music=NULL;
Mix_Chunk *ring;
bool	music_init_flag=false;

Mix_Chunk *load_sound(char *file){
	Mix_Chunk *sound;
	sound=Mix_LoadWAV(file);
	if (!sound){
		printf("ERROR:%s\n",SDL_GetError());
		exit(2);
	}
	return sound;
}

void music_Init(){
	if (!music_init_flag){
	if (SDL_Init(SDL_INIT_AUDIO)<0) {
		printf("AUDIO_INIT error!\n");
		exit(1);
	}
	if (Mix_OpenAudio(22050,AUDIO_S16,2,4096)<0){
		printf("ERROR:%s\n",SDL_GetError());
		exit(1);
	}
//	Mix_HookMusicFinished(music_Done);
	music_init_flag=true;
	}
	return ;
}	


int main(int argc,char *argv[])
{
	int i=20;
	music_Init();
	music=Mix_LoadMUS(argv[1]);
	//ring=load_sound(argv[2]);
	Mix_PlayMusic(music,0);
	while(i!=0){
		sleep(1);
		i--;
		//Mix_PlayChannel(-1,ring,0);
		printf("%d\n",i);
	};
	//Mix_HaltChannel(-1);
	Mix_HaltMusic();
	Mix_FreeMusic(music);
	music=NULL;
	Mix_CloseAudio();
	SDL_Quit();
	return 0;
}

