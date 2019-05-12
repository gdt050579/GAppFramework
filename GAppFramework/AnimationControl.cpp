#include "GApp.h"

using namespace GApp::Controls;

AnimationControl::AnimationControl(void *scene, const char* layout, GApp::Animations::AnimationObject * _a, bool start, bool loop) : GApp::Controls::GenericControl(scene, layout)
{
	SetAnimation(_a, start, loop);
}
void AnimationControl::SetAnimation(GApp::Animations::AnimationObject * _anim, bool start, bool loop)
{
	anim = _anim;
	AboutToStart = false;
	if (anim != NULL)
		Resize(anim->Width, anim->Height);
	Loop = loop;
	if (start)
		Start();
}
void AnimationControl::Start()
{
	if (anim != NULL) {
		anim->Start(); 
		AboutToStart = true;		
	}
}
void AnimationControl::Stop()
{
	if ((anim != NULL) && (anim->IsRunning())) {
		anim->Stop();
		this->TriggerEvent(GAC_EVENTTYPE_ANIMATIONENDED);
		AboutToStart = false;
	}
}
void AnimationControl::Pause()
{
	if ((anim != NULL) && (anim->IsRunning()) && (anim->IsPaused() == false)) {
		anim->Pause();
		this->TriggerEvent(GAC_EVENTTYPE_ANIMATIONPAUSED);
	}
}
void AnimationControl::Resume()
{
	if ((anim != NULL) && (anim->IsRunning()) && (anim->IsPaused() == true)) {
		anim->Resume();
		this->TriggerEvent(GAC_EVENTTYPE_ANIMATIONRESUMED);
	}
}
bool AnimationControl::IsStarted()
{
	if (anim != NULL)
		return anim->IsRunning();
	return AboutToStart;
}
bool AnimationControl::IsPaused()
{
	if (anim != NULL)
		return anim->IsPaused();
	return false;
}
void AnimationControl::OnPaint()
{
	if (AboutToStart)
	{
		this->TriggerEvent(GAC_EVENTTYPE_ANIMATIONSTARTED);
		AboutToStart = false;
	}
	if (anim != NULL) {
		if (anim->ControlPaint(Loop) == false)
		{
			this->TriggerEvent(GAC_EVENTTYPE_ANIMATIONENDED);
			AboutToStart = false;
		}
	}
}
bool AnimationControl::OnTouchEvent(GApp::Controls::TouchEvent *te)
{
	if ((anim == NULL) || (anim->IsRunning() == false))
		return false;
	return anim->OnTouchEvent(te);
}
