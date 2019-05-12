#include "GApp.h"

GApp::Utils::FPSCounter::FPSCounter(GApp::UI::Scene *scene)
{
	CoreContext = scene->CoreContext;
	StartTime = 0;
	Fps = 0;
	Counter = 0;
	SetComputeInterval(1000);
}
GApp::Utils::FPSCounter::FPSCounter(GApp::UI::Scene *scene, unsigned int computeInterval)
{
	CoreContext = scene->CoreContext;
	StartTime = 0;
	Fps = 0;
	Counter = 0;
	SetComputeInterval(1000);
	SetComputeInterval(computeInterval);
}
bool	GApp::Utils::FPSCounter::SetComputeInterval(unsigned int value)
{
	CHECK(value >= 1, false, "Commpute interval value should be bigger than 0");
	ComputeInterval = value;
	return true;
}
float	GApp::Utils::FPSCounter::GetFPS()
{
	return Fps;
}
void    GApp::Utils::FPSCounter::Reset()
{
	Fps = 0;
	Counter = 0;
}
void	GApp::Utils::FPSCounter::Start()
{
	StartTime = CoreContext->Api.GetSystemTickCount();
	Counter = 0;
}
bool	GApp::Utils::FPSCounter::Update()
{
	Counter++;
	unsigned int dif = CoreContext->Api.GetSystemTickCount() - StartTime;
	if (dif >= ComputeInterval)
	{
		Fps = (Counter * 1000.0f) / dif;
		return true;
	}
	return false;
}