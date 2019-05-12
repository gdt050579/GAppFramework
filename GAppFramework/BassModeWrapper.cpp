#include "DevelopMode.h"

using namespace GApp::DevelopMode;

bool BassModeWrapper::Init()
{
	if (BASS_Init(-1, 44100, 0, NULL, NULL))
		return true;
	return false;
}
void BassModeWrapper::Dispose()
{
	BASS_Free();
}
BassModeWrapper::BassModeWrapper()
{
	hSound = NULL;
	Buffer = NULL;
}
BassModeWrapper::~BassModeWrapper()
{
	Close();
}
bool BassModeWrapper::Create(const void* _Buffer, unsigned int size)
{
	Close();
	if ((_Buffer == NULL) || (size == 0))
		return false;
	hSound = BASS_StreamCreateFile(true, _Buffer, 0, size, 0);
	CHECK(hSound != NULL, false, "Unable to load music stream: %p [Size=%d] -> Error Code: %d", _Buffer, size, BASS_ErrorGetCode());
	Length = BASS_ChannelGetLength(hSound, BASS_POS_BYTE);
	Buffer = (unsigned char*)_Buffer;
	return true;
}
bool BassModeWrapper::SetLoop(bool loop)
{
	if (hSound == NULL)
		return false;
	if (loop)
		BASS_ChannelFlags(hSound, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
	else
		BASS_ChannelFlags(hSound, 0, BASS_SAMPLE_LOOP);
	return true;
}
bool BassModeWrapper::SetVolume(float vol)
{
	if (hSound == NULL)
		return false;
	if (vol < 0)
		vol = 0;
	if (vol > 1.0f)
		vol = 1.0f;
	CHECK(BASS_ChannelSetAttribute(hSound, BASS_ATTRIB_VOL, vol), false, "Unable to set volume to stream: %d at %2.3f", hSound, vol);
	return true;

}
bool BassModeWrapper::Play()
{
	if (hSound == NULL)
		return false;
	DWORD status = BASS_ChannelIsActive(hSound);
	if (status == BASS_ACTIVE_PLAYING)
		return true;
	if (status == BASS_ACTIVE_PAUSED)
	{
		CHECK(BASS_ChannelPlay(hSound, false), false, "Unable to resume stream: %d", hSound);
		return true;
	}
	CHECK(BASS_ChannelPlay(hSound, false), true, "Unable to play stream: %d", hSound);
	return true;
}
bool BassModeWrapper::Pause()
{
	if (hSound == NULL)
		return false;
	CHECK(BASS_ChannelPause(hSound), false, "Unable to pause stream: %d", hSound);
	return true;
}
void BassModeWrapper::Close()
{
	if (hSound != NULL)
	{
		BASS_StreamFree(hSound);
		hSound = NULL;
	}
	if (Buffer != NULL)
	{
		delete Buffer;
		Buffer = NULL;
	}
}

