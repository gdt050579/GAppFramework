#include "GApp.h"

using namespace GApp::Controls;

Popup::Popup(void* scene, const char *layout, GApp::Resources::Bitmap *img, GAC_TYPE_ALIGNAMENT align, GAC_TYPE_IMAGERESIZEMODE resizeMode) : ModalFrame(scene)
{
	GenericControl::AddControl(new Panel(scene, layout, img, align, resizeMode));
	alphaMax = 160;
	alpha = 0;
	alphaStep = 10;
	this->backgroundColor = 0;
}
Popup::Popup (void* scene, const char *layout) : ModalFrame(scene)
{
	GenericControl::AddControl(new Panel(scene, layout));
	alphaMax = 160;
	alpha = 0;
	alphaStep = 10;
	this->backgroundColor = 0;
}

void Popup::OnPaint()
{
	if (alphaStep > 0) {
		if (alpha < alphaMax) {
			alpha += alphaStep;
			if (alpha > alphaMax)
				alpha = alphaMax;
		}
	}
	else if (alphaStep < 0)
	{
		if (alpha > 0) {
			alpha += alphaStep;
			if (alpha < 0)
				alpha = 0;
		}
		else {
			Hide();
		}
	}
	G.FillScreen(COLOR_ALPHA(alpha,backgroundColor));
}
void Popup::SetBackgroundAlphaEfect(int maxAlpha, int step, unsigned int backColor)
{
	if (maxAlpha < 1)
		maxAlpha = 1;
	if (maxAlpha>255)
		maxAlpha = 255;
	if (step < 1)
		step = 1;
	alphaMax = maxAlpha;
	if (alphaStep < 0)
		alphaStep = -step;
	else
		alphaStep = step;
	this->backgroundColor = backColor;
}
bool Popup::AddControl(GenericControl *control)
{
	// adaug din prima pe label
	return Controls[0]->AddControl(control);
}
void Popup::OnShow()
{
	alpha = 0;
	if (alphaStep < 0)
		alphaStep = -alphaStep;
}
void Popup::OnClosing()
{
	if (alphaStep > 0)
		alphaStep = -alphaStep;
}
bool Popup::OnTouchEvent(GApp::Controls::TouchEvent *te)
{
	if ((te != NULL) && (te->Type == GAC_TOUCHEVENTTYPE_CLICK))
		TriggerEvent(GAC_EVENTTYPE_POPUPCLICK);
	return true; // procesez totul eu
}