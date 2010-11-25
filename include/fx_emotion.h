#ifndef FX_EMOTION_H
#define FX_EMOTION_H

struct emotion{

	const char* name;
	const char* symbol;
	int id;

};


extern FxEmotion *fx_emotion_new(FxChat *fxchat);

extern void fx_emotion_initialize(FxEmotion *fxemotion , int x , int y);

#endif
