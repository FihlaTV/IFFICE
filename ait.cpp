#include "ait.h"
#include "config.h"

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

Mix_Music *mymusic=NULL;
Mix_Chunk *myring;
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

void music_Done(){
	return;
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
	Mix_HookMusicFinished(music_Done);
	music_init_flag=true;
	}
	return ;
}	

void music_Play(MUSIC *music) {
	if (Mix_PlayingMusic() || Mix_PausedMusic()){
		Mix_HaltMusic();
		Mix_FreeMusic(mymusic);
		mymusic=NULL;
	}
	mymusic=Mix_LoadMUS(music->path);
	Mix_PlayMusic(mymusic,0);
	return ;
}

void music_Pause() {
	if (mymusic!=NULL && !Mix_PausedMusic())
		//pause music playback
		Mix_PauseMusic();
	return ;
}

void music_Resume() {
	if (mymusic!=NULL && Mix_PausedMusic())
		//resume music playback
		Mix_ResumeMusic();
	return ;
}

void music_Rewind() {
	//rewind music playback to the start
	Mix_RewindMusic();
}
void music_Quit() {
	if (mymusic!=NULL) {
		Mix_HaltMusic();
		Mix_FreeMusic(mymusic);
		mymusic=NULL;
	}
//	Mix_CloseAudio();
//	SDL_Quit();
	return ;
}
/*
int main(int argc,char *argv[])
{
	music=Mix_LoadMUS(argv[1]);
	//ring=load_sound(argv[2]);
	Mix_PlayMusic(music,0);
	while(i!=0){
		sleep(1);
		i--;
		if (i==10) {
			Mix_HaltMusic();
			Mix_FreeMusic(music);
			music=Mix_LoadMUS(argv[2]);
			Mix_PlayMusic(music,0);
			flag=1;
		}
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

*/
