#include "GApp.h"

using namespace GApp::Controls;

AnimationButton::AnimationButton(void *scene, int objectID, const char* layout, GApp::Animations::AnimationObject * _normal, GApp::Animations::AnimationObject * _pressed, GApp::Animations::AnimationObject * _inactive) : GApp::Controls::GenericControl(scene, layout)
{
	ID = objectID;
	normal = _normal;
	pressed = _pressed;
	inactive = _inactive;
	float w = -1;
	float h = -1;
	if (pressed != NULL) {
		w = pressed->Width;
		h = pressed->Height;
	}
	if (normal != NULL) {
		w = normal->Width;
		h = normal->Height;
	}
	if ((w > 0) && (h > 0)) {
		Resize(w, h);
	}
	else {
		LOG_ERROR("Unable to initialize animation button (no animation pressent !");
	}
	RestartAnimations();
}
AnimationButton::AnimationButton(void *scene, int objectID, const char* layout, GApp::Animations::AnimationObject * anim) : GApp::Controls::GenericControl(scene, layout)
{
	ID = objectID;
	normal = anim;
	pressed = anim;
	inactive = anim;
	float w = anim->Width;
	float h = anim->Height;
	if ((w > 0) && (h > 0)) {
		Resize(w, h);
	}
	else {
		LOG_ERROR("Unable to initialize animation button (no animation pressent !");
	}
	RestartAnimations();
}
void AnimationButton::OnPaint()
{
	bool v_i = false, v_n = false, v_p = false;
	if ((normal == pressed) && (normal == inactive) && (normal!=NULL))
	{
		// daca e aceeasi animatie - o rulez o data
		normal->ButtonPaint(true);
		return;
	}
	if (IsEnabled() == false)
	{
		if (inactive != NULL)
			v_i = true;
	}
	else {
		if (HasTouch())
		{
			if (pressed != NULL)
				v_p = true;
		}
		else {
			if (normal != NULL)
				v_n = true;
		}
	}	
	if (normal != NULL)
		normal->ButtonPaint(v_n);
	if (pressed != NULL)
		pressed->ButtonPaint(v_p);
	if (inactive != NULL)
		inactive->ButtonPaint(v_i);
}
void AnimationButton::RestartAnimations()
{
	if ((normal == pressed) && (normal == inactive) && (normal != NULL))
	{
		normal->Stop(); 
		normal->Start();
		return;
	}
	if (normal != NULL) { normal->Stop(); normal->Start(); }
	if (pressed != NULL) { pressed->Stop(); pressed->Start(); }
	if (inactive != NULL) { inactive->Stop(); inactive->Start(); }
}
bool AnimationButton::OnTouchEvent(GApp::Controls::TouchEvent *te)
{
	return ProcessTouchEventForClick(te);
}
GApp::Animations::AnimationObject * AnimationButton::GetAnimationForNormalState() { return normal; }
GApp::Animations::AnimationObject * AnimationButton::GetAnimationForPressedState() { return pressed; }
GApp::Animations::AnimationObject * AnimationButton::GetAnimationForInactiveState() { return inactive; }
