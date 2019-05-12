#include "GApp.h"

GApp::Resources::GenericResource::GenericResource()
{
	CoreContext = NULL;
	ProfileContext = NULL;
	Position = 0;
	Size = 0;
	Flags = GAC_RESOURCEFLAG_NONE;
#if defined(ENABLE_INFO_LOGGING) || defined(ENABLE_ERROR_LOGGING)
	Name=NULL;
#endif
}
bool GApp::Resources::GenericResource::IsLoaded()
{
	return (ProfileContext!=NULL);
}
bool GApp::Resources::GenericResource::OnCreate()
{
	return true;
}
bool GApp::Resources::GenericResource::OnUpdate()
{
	return true;
}