#include "GApp.h"

#define ANIM_POPUP_TYPE_ONE_ANIMATION_FORCE_CLOSE		0
#define ANIM_POPUP_TYPE_ONE_ANIMATION_WAIT_TO_FINISH	1
#define ANIM_POPUP_TYPE_TWO_ANIMATIONS_WAIT_TO_FINISH	2

using namespace GApp::Controls;

AnimationPopup::AnimationPopup(void* scene, GApp::Animations::AnimationObject *backgroundAnimation, bool autoCloseWhenAnimationEnds) : ModalFrame(scene)
{
	GenericControl::AddControl(new Panel(scene, "x:0%,y:0%,a:tl,w:100%,h:100%"));
	back = backgroundAnimation;
	if (autoCloseWhenAnimationEnds)
		Type = ANIM_POPUP_TYPE_ONE_ANIMATION_WAIT_TO_FINISH;
	else
		Type = ANIM_POPUP_TYPE_ONE_ANIMATION_FORCE_CLOSE;
	end = NULL;
	Closing = false;
}
AnimationPopup::AnimationPopup(void* scene, GApp::Animations::AnimationObject *backgroundAnimation, GApp::Animations::AnimationObject *endingAnimation) : ModalFrame(scene)
{
	GenericControl::AddControl(new Panel(scene, "x:0%,y:0%,a:tl,w:100%,h:100%"));
	back = backgroundAnimation;
	end = endingAnimation;
	Closing = false;
	if (end == NULL)
		Type = ANIM_POPUP_TYPE_ONE_ANIMATION_FORCE_CLOSE;
	else
		Type = ANIM_POPUP_TYPE_TWO_ANIMATIONS_WAIT_TO_FINISH;
}
void AnimationPopup::OnPaint()
{
	switch (Type)
	{
		case ANIM_POPUP_TYPE_ONE_ANIMATION_FORCE_CLOSE:
			if (back != NULL)
				back->Paint();
			break;
		case ANIM_POPUP_TYPE_ONE_ANIMATION_WAIT_TO_FINISH:
			if (back != NULL) {
				back->Paint();
				if (back->IsRunning() == false)
					Hide();
			}
			break;
		case ANIM_POPUP_TYPE_TWO_ANIMATIONS_WAIT_TO_FINISH:
			if (Closing) {
				end->Paint();
				if (end->IsRunning() == false)
					Hide();
			}
			else {
				if (back != NULL)
					back->Paint();
			}
			break;
	}
}
void AnimationPopup::OnShow()
{
	Closing = false;
	if (back != NULL)
		back->Start();
}
void AnimationPopup::OnClosing()
{
	Closing = true;
	if (Type == ANIM_POPUP_TYPE_ONE_ANIMATION_FORCE_CLOSE)
		Hide();
	if (Type == ANIM_POPUP_TYPE_TWO_ANIMATIONS_WAIT_TO_FINISH)
		end->Start();
}
bool AnimationPopup::AddControl(GenericControl *control)
{
	// adaug din prima pe label
	return Controls[0]->AddControl(control);
}
bool AnimationPopup::OnTouchEvent(GApp::Controls::TouchEvent *te)
{
	if (te != NULL)
	{
		if (back->OnTouchEvent(te) == true)
			return true;
		if (te->Type == GAC_TOUCHEVENTTYPE_CLICK)
			TriggerEvent(GAC_EVENTTYPE_POPUPCLICK);
	}
	return true; // procesez totul eu
}