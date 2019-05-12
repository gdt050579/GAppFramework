#include "GApp.h"

#define TIMER_STATUS_STOPPED	0
#define TIMER_STATUS_STARTED	1
#define TIMER_STATUS_PAUSED		2

#define UPDATE_VALUE {\
	unsigned int currentTime = API.GetSystemTickCount(); \
	if (currentTime >= timeStart) Value = (currentTime - timeStart) + extraTime; \
	else Value = ((0xFFFFFFFF - timeStart) + currentTime) + extraTime; \
}

GApp::Utils::Timer::Timer(GApp::UI::Scene *scene) 
{ 
	CoreContext = scene->CoreContext; 
	Value = extraTime = 0;
	Status = TIMER_STATUS_STOPPED;
}
GApp::Utils::Timer::Timer(GApp::UI::Application *app) 
{ 
	CoreContext = app->CoreContext; 
	Value = extraTime = 0;
	Status = TIMER_STATUS_STOPPED;
}
GApp::Utils::Timer::Timer(GApp::UI::FrameworkObject *obj) 
{ 
	CoreContext = obj->CoreContext;
	Value = extraTime = 0;
	Status = TIMER_STATUS_STOPPED;
}
void			GApp::Utils::Timer::Start()
{
	Value = extraTime = 0;
	Status = TIMER_STATUS_STARTED;
	timeStart = API.GetSystemTickCount();
}
void			GApp::Utils::Timer::Stop()
{
	if (Status != TIMER_STATUS_STOPPED)
	{
		UPDATE_VALUE;
		Status = TIMER_STATUS_STOPPED;
	}
}
void			GApp::Utils::Timer::Pause()
{
	if (Status == TIMER_STATUS_STARTED)
	{
		UPDATE_VALUE;
		Status = TIMER_STATUS_PAUSED;
	}
}
void			GApp::Utils::Timer::Resume()
{
	if (Status == TIMER_STATUS_PAUSED)
	{
		extraTime = Value;
		Status = TIMER_STATUS_STARTED;
		timeStart = API.GetSystemTickCount();
	}
}
unsigned int	GApp::Utils::Timer::GetValue()
{
	if (Status == TIMER_STATUS_STARTED)
	{
		UPDATE_VALUE;
		return Value;
	}
	return Value;
}
bool			GApp::Utils::Timer::IsStopped()
{
	return Status == TIMER_STATUS_STOPPED;
}
bool			GApp::Utils::Timer::IsStarted()
{
	return Status == TIMER_STATUS_STARTED;
}
bool			GApp::Utils::Timer::IsPaused()
{
	return Status == TIMER_STATUS_PAUSED;
}