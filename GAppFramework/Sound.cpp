#include "GApp.h"

GApp::Resources::Sound::Sound()
{
	SoundID = -1;
	Flags = GAC_RESOURCEFLAG_SOUND;
}
bool GApp::Resources::Sound::Create(CoreReference core,int soundID,unsigned int position,unsigned int size)
{
	CHECK(soundID>=0,false,"Invalid sound ID - expecting a positive number , got %d",soundID);
	SoundID = soundID;
	CoreContext = core;
	Position = position;
	Size = size;
	return true;
}
bool GApp::Resources::Sound::Load()
{
	CHECK(CoreContext!=NULL,false,"Invalid (NULL) CoreContext");
	CHECK(((AppContextData*)Core.Api.Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	CHECK(((AppContextData*)Core.Api.Context)->OS->fnLoadSound(SoundID,Position,Size),false,"Fail to load sound !");
	return true;
}
bool GApp::Resources::Sound::Unload()
{
	if (IsLoaded()==false)
		return true;
	CHECK(CoreContext!=NULL,false,"Invalid (NULL) CoreContext");	
	CHECK(((AppContextData*)Core.Api.Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	((AppContextData*)Core.Api.Context)->OS->fnStopSound(SoundID);
	((AppContextData*)Core.Api.Context)->OS->fnUnLoadSound(SoundID);
	this->ProfileContext = NULL;
	return true;
}