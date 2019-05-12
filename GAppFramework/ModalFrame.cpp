#include "GApp.h"

using namespace GApp::Controls;

ModalFrame::ModalFrame(void *scene) : GenericControl(scene, "x:0%,y:0%,a:tl,w:100%,h:100%")
{
	GenericControl::SetVisible(false);
	GenericControl::SetEnabled(true);
	SET_BIT(Flags, GAC_CONTROL_FLAG_MODAL_FRAME);
	if (scene != NULL)
	{
		((GApp::UI::Scene*)scene)->AddControl(this);
	}
}
void ModalFrame::Show()
{
	if (GenericControl::IsVisible() == false)
	{		
		GenericControl::SetID(-1);
		OnShow();
		GenericControl::SetVisible(true);
	}
}
void ModalFrame::Close(int controlID)
{	
	if (GenericControl::IsVisible() == true)
	{
		GenericControl::SetID(controlID);
		TriggerEvent(GAC_EVENTTYPE_MODALFRAME_BEFORE_CLOSE);
		OnClosing();
	}
}
void ModalFrame::Hide()
{
	if (GenericControl::IsVisible() == true)
	{
		GenericControl::SetVisible(false);
		TriggerEvent(GAC_EVENTTYPE_MODALFRAMECLOSED);
	}
}
bool ModalFrame::IsOpened()
{
	return GenericControl::IsVisible();
}
bool ModalFrame::AddControl(GenericControl *control)
{
	return GenericControl::AddControl(control);
}
unsigned int ModalFrame::GetControlsCount()
{
	return GenericControl::GetControlsCount();
}
GenericControl* ModalFrame::GetControl(unsigned int index)
{
	return GenericControl::GetControl(index);
}

void ModalFrame::OnPaint()
{
}
bool ModalFrame::OnTouchEvent(TouchEvent *evn)
{
	return false;
}
void ModalFrame::OnShow()
{
}
void ModalFrame::OnClosing()
{
	Hide();
}