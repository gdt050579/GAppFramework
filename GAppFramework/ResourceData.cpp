#include "GApp.h"

bool GApp::Resources::ResourceData::Init(CoreReference context)
{
	RETURNERROR(false,"GApp::Resources::ResourceData::Init() - not implemented !!!");
}
GApp::Resources::Profile* GApp::Resources::ResourceData::GetProfilesList()
{
	RETURNERROR(NULL,"GApp::Resources::ResourceData::GetProfilesList() - not implemented !!!");
}
unsigned int GApp::Resources::ResourceData::GetProfilesCount()
{
	RETURNERROR(0,"GApp::Resources::ResourceData::GetProfilesCount() - not implemented !!!");
}
GApp::Resources::Sound* GApp::Resources::ResourceData::GetSoundsList()
{
	RETURNERROR(NULL, "GApp::Resources::ResourceData::GetSoundsList() - not implemented !!!");
}
unsigned int GApp::Resources::ResourceData::GetSoundsCount()
{
	RETURNERROR(0, "GApp::Resources::ResourceData::GetSoundsCount() - not implemented !!!");
}
GApp::Platform::AdInterface* GApp::Resources::ResourceData::GetAd(unsigned int index)
{
	RETURNERROR(NULL, "GApp::Resources::ResourceData::GetAd() - not implemented !!!");
}
GApp::UI::Counter*		GApp::Resources::ResourceData::GetCountersList()
{
	RETURNERROR(NULL, "GApp::Resources::ResourceData::GetCountersList() - not implemented !!!");
}
unsigned int			GApp::Resources::ResourceData::GetCountersCount()
{
	RETURNERROR(0, "GApp::Resources::ResourceData::GetCountersCount() - not implemented !!!");
}
GApp::UI::CountersGroup* GApp::Resources::ResourceData::GetCountersGroupList()
{
	RETURNERROR(NULL, "GApp::Resources::ResourceData::GetCountersGroupList() - not implemented !!!");
}
unsigned int			GApp::Resources::ResourceData::GetCountersGroupCount()
{
	RETURNERROR(0, "GApp::Resources::ResourceData::GetCountersGroupCount() - not implemented !!!");
}
GApp::UI::Alarm*		GApp::Resources::ResourceData::GetAlarmsList()
{
	RETURNERROR(NULL, "GApp::Resources::ResourceData::GetAlarmsList() - not implemented !!!");
}
unsigned int			GApp::Resources::ResourceData::GetAlarmsCount()
{
	RETURNERROR(0, "GApp::Resources::ResourceData::GetAlarmsCount() - not implemented !!!");
}