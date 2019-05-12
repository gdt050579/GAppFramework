#include "GApp.h"

// CTX.MediaState 
// 0-8	SoundVolume
// 8-16	Music Volume
// 17   SoundEnabled
// 18   MusicEnabled
// 19   VibrationEnabled
// 20   LoopMusic

#define MEDIA_SOUND_ENABLED			0x010000
#define MEDIA_MUSIC_ENABLED			0x020000
#define MEDIA_LOOP_MUSIC			0x040000
#define MEDIA_VIBRATION_ENABLED		0x080000

#define CTX	(*((AppContextData*)Core.Api.Context))
#define SOUND_VOLUME	((CTX.MediaState) & 0xFF)
#define MUSIC_VOLUME	(((CTX.MediaState)>>8) & 0xFF)



int AdjustVolumeValue(int value)
{
	if (value < 0)
		value = 0;
	if (value > 100)
		value = 100;
	return value;
}
GApp::Media::AudioPlayer::AudioPlayer()
{
	CoreContext = NULL;
}

int  GApp::Media::AudioPlayer::GetSoundVolume()
{
	if (CoreContext==NULL)
		return 0;
	return SOUND_VOLUME;
}
int	 GApp::Media::AudioPlayer::GetMusicVolume()
{
	if (CoreContext==NULL)
		return 0;
	return MUSIC_VOLUME;
}
void GApp::Media::AudioPlayer::SetMusicVolume(int value)
{
	if (CoreContext != NULL) {
		CTX.MediaState = ((CTX.MediaState) & 0xFFFF00FF) | (AdjustVolumeValue(value) << 8);
		LOG_INFO("AudioPlayer::SetMusicVolume(%d) => MediaState = %08X, resulted volume = %d, CurrentMusicID = %d", value, CTX.MediaState, (MUSIC_VOLUME), CTX.CurrentMusicID);
		if (CTX.CurrentMusicID >= 0)
			((AppContextData*)Core.Api.Context)->OS->fnSetSoundState(CTX.CurrentMusicID, MUSIC_VOLUME, (CTX.MediaState & MEDIA_LOOP_MUSIC) != 0);
	}
}
void GApp::Media::AudioPlayer::SetSoundVolume(int value)
{
	if (CoreContext != NULL) {
		CTX.MediaState = ((CTX.MediaState) & 0xFFFFFF00) | (AdjustVolumeValue(value));
		LOG_INFO("AudioPlayer::SetSoundVolume(%d) => MediaState = %08X, resulted volume = %d", value, CTX.MediaState, (SOUND_VOLUME));
	}
		//CTX.MediaState = AdjustVolumeValue(value) | (AdjustVolumeValue(MUSIC_VOLUME)<<8) | (CTX.MediaState & 0xFFFF0000);
}
void GApp::Media::AudioPlayer::SetMusicLoop(bool state)
{
	if (CoreContext!=NULL)
	{
		if (state)
		{	
			SET_BIT(CTX.MediaState,MEDIA_LOOP_MUSIC); 
		}
		else
		{ 
			REMOVE_BIT(CTX.MediaState,MEDIA_LOOP_MUSIC); 
		}
		if (CTX.CurrentMusicID>=0)
			((AppContextData*)Core.Api.Context)->OS->fnSetSoundState(CTX.CurrentMusicID,MUSIC_VOLUME,state);
	}
}
void GApp::Media::AudioPlayer::SetMusicEnabled(bool state)
{
	if (CoreContext!=NULL)
	{
		if (state)
		{	
			SET_BIT(CTX.MediaState,MEDIA_MUSIC_ENABLED); 
		}
		else
		{ 
			REMOVE_BIT(CTX.MediaState,MEDIA_MUSIC_ENABLED); 
		}
		if (state)
			PlayBackgroundMusic();
		else
			StopBackgroundMusic();
	}
}
void GApp::Media::AudioPlayer::SetSoundEnabled(bool state)
{
	if (CoreContext!=NULL)
	{
		if (state)
		{	
			SET_BIT(CTX.MediaState,MEDIA_SOUND_ENABLED); 
		}
		else
		{ 
			REMOVE_BIT(CTX.MediaState,MEDIA_SOUND_ENABLED); 
		}
	}
}
void GApp::Media::AudioPlayer::SetMediaEnabled(bool state)
{
	if (CoreContext!=NULL)
	{
		if (state)
		{	
			SET_BIT(CTX.MediaState,MEDIA_SOUND_ENABLED|MEDIA_MUSIC_ENABLED); 
		}
		else
		{ 
			REMOVE_BIT(CTX.MediaState,MEDIA_SOUND_ENABLED|MEDIA_MUSIC_ENABLED); 
		}
		if (state)
			PlayBackgroundMusic();
		else
			StopBackgroundMusic();
	}
}
bool GApp::Media::AudioPlayer::IsMusicLooped()
{
	if (CoreContext==NULL)
		return false;
	return ((CTX.MediaState & MEDIA_LOOP_MUSIC)!=0);
}
bool GApp::Media::AudioPlayer::IsMusicEnabled()
{	
	if (CoreContext==NULL)
		return false;
	return ((CTX.MediaState & MEDIA_MUSIC_ENABLED)!=0);
}
bool GApp::Media::AudioPlayer::IsSoundEnabled()
{
	if (CoreContext==NULL)
		return false;
	return ((CTX.MediaState & MEDIA_SOUND_ENABLED)!=0);
}
bool GApp::Media::AudioPlayer::IsMediaEnabled()
{
	if (CoreContext==NULL)
		return false;
	return ((CTX.MediaState & (MEDIA_SOUND_ENABLED|MEDIA_MUSIC_ENABLED))!=0);
}
bool GApp::Media::AudioPlayer::Play(GApp::Resources::Sound *snd)
{
	CHECK(snd!=NULL,false,"Invalid (NULL) sound object");
	CHECK(snd->IsLoaded(),false,"Sound resource not loaded !");
	CHECK(CoreContext!=NULL,false,"Invalid (NULL) core reference !");
	CHECK(((AppContextData*)Core.Api.Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
		
	if ((CTX.MediaState & MEDIA_SOUND_ENABLED)!=0)
		((AppContextData*)Core.Api.Context)->OS->fnPlaySound(snd->SoundID,SOUND_VOLUME,false);
	return true;
}
bool GApp::Media::AudioPlayer::Play(GApp::Resources::Sound *snd, bool loop)
{
	CHECK(snd != NULL, false, "Invalid (NULL) sound object");
	CHECK(snd->IsLoaded(), false, "Sound resource not loaded !");
	CHECK(CoreContext != NULL, false, "Invalid (NULL) core reference !");
	CHECK(((AppContextData*)Core.Api.Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");

	if ((CTX.MediaState & MEDIA_SOUND_ENABLED) != 0)
		((AppContextData*)Core.Api.Context)->OS->fnPlaySound( snd->SoundID, SOUND_VOLUME, loop);
	return true;
}
bool GApp::Media::AudioPlayer::Play(GApp::Resources::Sound *snd, int volume, bool loop)
{
	CHECK(snd != NULL, false, "Invalid (NULL) sound object");
	CHECK(snd->IsLoaded(), false, "Sound resource not loaded !");
	CHECK(CoreContext != NULL, false, "Invalid (NULL) core reference !");
	CHECK(((AppContextData*)Core.Api.Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");

	if ((CTX.MediaState & MEDIA_SOUND_ENABLED) != 0)
		((AppContextData*)Core.Api.Context)->OS->fnPlaySound(snd->SoundID, volume, loop);
	return true;
}
void GApp::Media::AudioPlayer::Stop(GApp::Resources::Sound *snd)
{
	while (true)
	{
		CHECKBK(snd != NULL,  "Invalid (NULL) sound object");
		CHECKBK(snd->IsLoaded(),  "Sound resource not loaded !");
		CHECKBK(CoreContext != NULL,  "Invalid (NULL) core reference !");
		CHECKBK(((AppContextData*)Core.Api.Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		((AppContextData*)Core.Api.Context)->OS->fnStopSound(snd->SoundID);
		return;
	}
}
bool GApp::Media::AudioPlayer::PlayBackgroundMusic(GApp::Resources::Sound *snd)
{
	int soundID = -1;	
	if (snd!=NULL)
	{	
		CHECK(snd->IsLoaded(),false,"Sound resource not loaded !");
		soundID = snd->SoundID;
	}
	CHECK(CoreContext!=NULL,false,"Invalid (NULL) core reference !");
	CHECK(((AppContextData*)Core.Api.Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	LOG_INFO("AudioPlayer::PlayBackgroundMusic(sndID = %d) - CurrentMusicID = %d [Volume=%d, Loop=%d]", soundID, CTX.CurrentMusicID, (MUSIC_VOLUME), ((CTX.MediaState & MEDIA_LOOP_MUSIC) != 0));

	if (soundID<0)
		soundID = CTX.CurrentMusicID;
	if (CTX.CurrentMusicID >= 0)
		((AppContextData*)Core.Api.Context)->OS->fnStopSound(CTX.CurrentMusicID);
	CTX.CurrentMusicID = soundID;

	if ((CTX.MediaState & MEDIA_MUSIC_ENABLED)!=0)
	{
		if (CTX.CurrentMusicID >= 0)
			((AppContextData*)Core.Api.Context)->OS->fnPlaySound(CTX.CurrentMusicID, MUSIC_VOLUME, ((CTX.MediaState & MEDIA_LOOP_MUSIC) != 0));
	}
	return true;
}
void GApp::Media::AudioPlayer::StopBackgroundMusic()
{
	while (true)
	{
		CHECKBK(CoreContext != NULL,  "Invalid (NULL) core reference !");
		CHECKBK(((AppContextData*)Core.Api.Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		LOG_INFO("AudioPlayer::StopBackgroundMusic() - CurrentMusicID = %d", CTX.CurrentMusicID);
		if (CTX.CurrentMusicID >= 0)
			((AppContextData*)Core.Api.Context)->OS->fnStopSound(CTX.CurrentMusicID);
		return;
	}
}
void GApp::Media::AudioPlayer::ChangeAudioSettings(GAC_TYPE_CHANGEAUDIOSETTINGSMETHOD method)
{
	//bool musicStatus = IsMusicEnabled();
	switch (method)
	{
		case GAC_CHANGEAUDIOSETTINGSMETHOD_ONOFF:
			SetMediaEnabled(!IsMediaEnabled());				
			break;
		case GAC_CHANGEAUDIOSETTINGSMETHOD_ONSFXOFF:
            if ((IsMusicEnabled()) && (IsSoundEnabled()))
                SetMusicEnabled(false);
            else if ((IsMusicEnabled() == false) && (IsSoundEnabled()))
                SetMediaEnabled(false);
            else
                SetMediaEnabled(true);
			break;
		case GAC_CHANGEAUDIOSETTINGSMETHOD_SFX_ONOFF:			
			SetSoundEnabled(!IsSoundEnabled());
			break;
		case GAC_CHANGEAUDIOSETTINGSMETHOD_MUSIC_ONOFF:
			SetMusicEnabled(!IsMusicEnabled());
			break;
	}
	//bool newMusicStatus = IsMusicEnabled();
}
/*
bool GApp::Media::AudioPlayer::Load(int soundID)
{
	if (CoreContext!=NULL)
		return ((AppContextData*)Core.Api.Context)->OS->LoadSound(((AppContextData*)Core.Api.Context)->OS,soundID);
	return false;
}
void GApp::Media::AudioPlayer::Dispose(int soundID)
{
	if (CoreContext!=NULL)
		((AppContextData*)Core.Api.Context)->OS->UnLoadSound(((AppContextData*)Core.Api.Context)->OS,soundID);
	
}
*/
