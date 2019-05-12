#include "GApp.h"

#define STATE_PROCESS(cmd) \
	switch (state) { \
	case GAC_CHECKBOX_STATE_ALL: NormalAndChecked.cmd; NormalAndUnChecked.cmd; PressedAndChecked.cmd; PressedAndUnChecked.cmd; InactiveAndChecked.cmd; InactiveAndUnChecked.cmd; break; \
	case GAC_CHECKBOX_STATE_NORMAL_AND_CHECKED: NormalAndChecked.cmd; break; \
	case GAC_CHECKBOX_STATE_NORMAL_AND_UNCHECKED: NormalAndUnChecked.cmd;  break; \
	case GAC_CHECKBOX_STATE_PRESSED_AND_CHECKED: PressedAndChecked.cmd; break; \
	case GAC_CHECKBOX_STATE_PRESSED_AND_UNCHECKED: PressedAndUnChecked.cmd; break; \
	case GAC_CHECKBOX_STATE_INACTIVE_AND_CHECKED: InactiveAndChecked.cmd;  break; \
	case GAC_CHECKBOX_STATE_INACTIVE_AND_UNCHECKED: InactiveAndUnChecked.cmd;  break; \
	case GAC_CHECKBOX_STATE_CHECKED: NormalAndChecked.cmd;  PressedAndChecked.cmd;  InactiveAndChecked.cmd;  break; \
	case GAC_CHECKBOX_STATE_UNCHECKED: NormalAndUnChecked.cmd;  PressedAndUnChecked.cmd;  InactiveAndUnChecked.cmd;  break; \
	case GAC_CHECKBOX_STATE_CHECKED_NORMAL_PRESSED: NormalAndChecked.cmd;  PressedAndChecked.cmd; break; \
	case GAC_CHECKBOX_STATE_UNCHECKED_NORMAL_PRESSED: NormalAndUnChecked.cmd;  PressedAndUnChecked.cmd; break; \
	case GAC_CHECKBOX_STATE_NORMAL: NormalAndChecked.cmd; NormalAndUnChecked.cmd; break; \
	case GAC_CHECKBOX_STATE_PRESSED: PressedAndChecked.cmd; PressedAndUnChecked.cmd; break; \
	case GAC_CHECKBOX_STATE_INACTIVE: InactiveAndChecked.cmd; InactiveAndUnChecked.cmd; break; \
	case GAC_CHECKBOX_STATE_NORMAL_PRESSED: NormalAndChecked.cmd; NormalAndUnChecked.cmd; PressedAndChecked.cmd; PressedAndUnChecked.cmd; break; \
	default: RETURNERROR(false, "Unkown check box state: %d", state); \
	}

GApp::Controls::CheckBox::CheckBox(void *scene, int objectID, const char* layout, GApp::Resources::Bitmap *BackgroundNormal,
			 GApp::Resources::Bitmap *BackgroundPressed,
			 GApp::Resources::Bitmap *BackgroundInactive,
			 GApp::Resources::Bitmap *FaceChecked,
			 GApp::Resources::Bitmap *FaceUnChecked,
			 GApp::Resources::Bitmap *FaceInactive) : GApp::Controls::GenericControl(scene, layout)
{
				 //LOG_INFO("new Button(scene=%p,id=%d,layout=%s",scene,objectID,layout);
				 ID = objectID;
				 // totul e ok - incep sa setez datele despre button
				 if (BackgroundNormal != NULL)
				 {
					 GAC_TYPE_IMAGERESIZEMODE imgMode = GAC_IMAGERESIZEMODE_FILL;
					 if (IS_CONTROL_AUTOSIZED)
					 {
						 Resize((float)BackgroundNormal->Width, (float)BackgroundNormal->Height);
						 imgMode = GAC_IMAGERESIZEMODE_DOCK;
					 }
					 NormalAndChecked.SetBackgroundImage(BackgroundNormal);
					 NormalAndChecked.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
					 NormalAndUnChecked.SetBackgroundImage(BackgroundNormal);
					 NormalAndUnChecked.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
					 if (BackgroundPressed != NULL)
					 {
						 PressedAndChecked.SetBackgroundImage(BackgroundPressed);
						 PressedAndChecked.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
						 PressedAndUnChecked.SetBackgroundImage(BackgroundPressed);
						 PressedAndUnChecked.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
					 }
					 else {
						 PressedAndChecked.SetBackgroundImage(BackgroundNormal);
						 PressedAndChecked.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
						 PressedAndUnChecked.SetBackgroundImage(BackgroundNormal);
						 PressedAndUnChecked.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
					 }
					 if (BackgroundInactive != NULL)
					 {
						 InactiveAndChecked.SetBackgroundImage(BackgroundInactive);
						 InactiveAndChecked.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
						 InactiveAndUnChecked.SetBackgroundImage(BackgroundInactive);
						 InactiveAndUnChecked.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
					 }
					 else {
						 InactiveAndChecked.SetBackgroundImage(BackgroundNormal);
						 InactiveAndChecked.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
						 InactiveAndUnChecked.SetBackgroundImage(BackgroundNormal);
						 InactiveAndUnChecked.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
					 }

					 if (FaceChecked != NULL)
					 {
						 NormalAndChecked.SetFaceImage(FaceChecked);
						 NormalAndChecked.SetFaceLayout(GAC_IMAGERESIZEMODE_DOCK, GAC_ALIGNAMENT_CENTER);
						 PressedAndChecked.SetFaceImage(FaceChecked);
						 PressedAndChecked.SetFaceLayout(GAC_IMAGERESIZEMODE_DOCK, GAC_ALIGNAMENT_CENTER);
					 }
					 if (FaceUnChecked != NULL)
					 {
						 NormalAndUnChecked.SetFaceImage(FaceUnChecked);
						 NormalAndUnChecked.SetFaceLayout(GAC_IMAGERESIZEMODE_DOCK, GAC_ALIGNAMENT_CENTER);
						 PressedAndUnChecked.SetFaceImage(FaceUnChecked);
						 PressedAndUnChecked.SetFaceLayout(GAC_IMAGERESIZEMODE_DOCK, GAC_ALIGNAMENT_CENTER);
					 }
					 if (FaceInactive != NULL)
					 {
						 InactiveAndChecked.SetFaceImage(FaceInactive);
						 InactiveAndChecked.SetFaceLayout(GAC_IMAGERESIZEMODE_DOCK, GAC_ALIGNAMENT_CENTER);
						 InactiveAndUnChecked.SetFaceImage(FaceInactive);
						 InactiveAndUnChecked.SetFaceLayout(GAC_IMAGERESIZEMODE_DOCK, GAC_ALIGNAMENT_CENTER);
					 }
				 }
			 }
void GApp::Controls::CheckBox::OnPaint()
{
	bool en = IsEnabled();
	bool ch = IsChecked();
	bool ht = HasTouch();

	G.UseDerivedShaderVariable = false;
	if (en)	
	{
		if (ht)
		{
			if (ch) { G.Draw(&PressedAndChecked); }
			else { G.Draw(&PressedAndUnChecked); }
		}
		else
		{
			if (ch) { G.Draw(&NormalAndChecked); }
			else { G.Draw(&NormalAndUnChecked); }
		}
	}
	else {
		if (ch) { G.Draw(&InactiveAndChecked); }
		else { G.Draw(&InactiveAndUnChecked); }
	}
	G.UseDerivedShaderVariable = true;
}
bool GApp::Controls::CheckBox::OnLayout()
{
	NormalAndChecked.SetViewRect(0, 0, GetWidth(), GetHeight());
	NormalAndUnChecked.SetViewRect(0, 0, GetWidth(), GetHeight());
	PressedAndChecked.SetViewRect(0, 0, GetWidth(), GetHeight());
	PressedAndUnChecked.SetViewRect(0, 0, GetWidth(), GetHeight());
	InactiveAndChecked.SetViewRect(0, 0, GetWidth(), GetHeight());
	InactiveAndUnChecked.SetViewRect(0, 0, GetWidth(), GetHeight());
	return false;
}
bool GApp::Controls::CheckBox::OnTouchEvent(GApp::Controls::TouchEvent *te)
{
	if (te->Type == GAC_TOUCHEVENTTYPE_CLICK)
	{
		SetChecked(!IsChecked());
	}
	return ProcessTouchEventForClick(te);
}



bool GApp::Controls::CheckBox::SetBackgroundImage(GAC_CHECKBOX_STATE_TYPE state, GApp::Resources::Bitmap *Image, bool autoResize)
{
	STATE_PROCESS(SetBackgroundImage(Image));
	if ((autoResize) && (Image != NULL))
		Resize((float)Image->Width, (float)Image->Height);
	return true;
}
bool GApp::Controls::CheckBox::SetBackgroundShader(GAC_CHECKBOX_STATE_TYPE state, GApp::Resources::Shader *shader)
{
	STATE_PROCESS(SetBackgroundShader(shader));
	return true;
}
bool GApp::Controls::CheckBox::SetBackgroundShaderParams(GAC_CHECKBOX_STATE_TYPE state, const char *VariableName, unsigned int countParams, float p1, float p2, float p3, float p4)
{
	STATE_PROCESS(Background.SetShaderUniformValue(VariableName, countParams, p1, p2, p3, p4));
	return true;
}
bool GApp::Controls::CheckBox::SetBackgroundShaderColor(GAC_CHECKBOX_STATE_TYPE state, const char *VariableName, unsigned int color)
{
	STATE_PROCESS(Background.SetShaderUniformColor(VariableName, color));
	return true;
}
bool GApp::Controls::CheckBox::SetBackgroundLayout(GAC_CHECKBOX_STATE_TYPE state, GAC_TYPE_IMAGERESIZEMODE mode, GAC_TYPE_ALIGNAMENT align)
{
	STATE_PROCESS(SetBackgroundLayout(mode, align));
	return true;
}

bool GApp::Controls::CheckBox::SetFaceImage(GAC_CHECKBOX_STATE_TYPE state, GApp::Resources::Bitmap *Image)
{
	STATE_PROCESS(SetFaceImage(Image));
	return true;
}
bool GApp::Controls::CheckBox::SetFaceShader(GAC_CHECKBOX_STATE_TYPE state, GApp::Resources::Shader *shader)
{
	STATE_PROCESS(SetFaceShader(shader));
	return true;
}
bool GApp::Controls::CheckBox::SetFaceShaderParams(GAC_CHECKBOX_STATE_TYPE state, const char *VariableName, unsigned int countParams, float p1, float p2, float p3, float p4)
{
	STATE_PROCESS(Face.SetShaderUniformValue(VariableName, countParams, p1, p2, p3, p4));
	return true;
}
bool GApp::Controls::CheckBox::SetFaceShaderColor(GAC_CHECKBOX_STATE_TYPE state, const char *VariableName, unsigned int color)
{
	STATE_PROCESS(Face.SetShaderUniformColor(VariableName, color));
	return true;
}
bool GApp::Controls::CheckBox::SetFaceLayout(GAC_CHECKBOX_STATE_TYPE state, GAC_TYPE_IMAGERESIZEMODE mode, GAC_TYPE_ALIGNAMENT align)
{
	STATE_PROCESS(SetFaceLayout(mode, align));
	return true;
}

bool GApp::Controls::CheckBox::SetFaceViewRect(GAC_CHECKBOX_STATE_TYPE state, float left, float top, float right, float bottom, GAC_TYPE_COORDINATES coord)
{
	STATE_PROCESS(SetFaceViewRect(left, top, right, bottom, coord));
	return true;
}

//Text
bool GApp::Controls::CheckBox::SetText(GAC_CHECKBOX_STATE_TYPE state, const char *text)
{
	STATE_PROCESS(SetText(text));
	return true;
}
bool GApp::Controls::CheckBox::SetTextFont(GAC_CHECKBOX_STATE_TYPE state, GApp::Resources::Font *font)
{
	STATE_PROCESS(SetTextFont(font));
	return true;
}
bool GApp::Controls::CheckBox::SetTextFontSize(GAC_CHECKBOX_STATE_TYPE state, GAC_TYPE_FONTSIZETYPE mode, float value)
{
	STATE_PROCESS(SetTextFontSize(mode, value));
	return true;
}
bool GApp::Controls::CheckBox::SetTextShader(GAC_CHECKBOX_STATE_TYPE state, GApp::Resources::Shader *shader)
{
	STATE_PROCESS(SetTextShader(shader));
	return true;
}
bool GApp::Controls::CheckBox::SetTextCharacterSpacing(GAC_CHECKBOX_STATE_TYPE state, float value)
{
	STATE_PROCESS(SetTextCharacterSpacing(value));
	return true;
}
bool GApp::Controls::CheckBox::SetTextSpaceWidth(GAC_CHECKBOX_STATE_TYPE state, float value)
{
	STATE_PROCESS(SetTextSpaceWidth(value));
	return true;
}
bool GApp::Controls::CheckBox::SetTextLineSpace(GAC_CHECKBOX_STATE_TYPE state, float value)
{
	STATE_PROCESS(SetTextLineSpace(value));
	return true;
}
bool GApp::Controls::CheckBox::SetTextDockPosition(GAC_CHECKBOX_STATE_TYPE state, GAC_TYPE_ALIGNAMENT value)
{
	STATE_PROCESS(SetTextDockPosition(value));
	return true;
}
bool GApp::Controls::CheckBox::SetTextWordWrap(GAC_CHECKBOX_STATE_TYPE state, bool value)
{
	STATE_PROCESS(SetTextWordWrap(value));
	return true;
}
bool GApp::Controls::CheckBox::SetTextViewRect(GAC_CHECKBOX_STATE_TYPE state, float left, float top, float right, float bottom, GAC_TYPE_COORDINATES coord)
{
	STATE_PROCESS(SetTextViewRect(left, top, right, bottom));
	return true;
}
bool GApp::Controls::CheckBox::SetTextShaderParams(GAC_CHECKBOX_STATE_TYPE state, const char *VariableName, unsigned int countParams, float p1, float p2, float p3, float p4)
{
	STATE_PROCESS(Text.SetShaderUniformValue(VariableName, countParams, p1, p2, p3, p4));
	return true;
}
bool GApp::Controls::CheckBox::SetTextShaderColor(GAC_CHECKBOX_STATE_TYPE state, const char *VariableName, unsigned int color)
{
	STATE_PROCESS(Text.SetShaderUniformColor(VariableName, color));
	return true;
}


bool GApp::Controls::CheckBox::SetInternalMargins(GAC_CHECKBOX_STATE_TYPE state, float marginLeft, float marginTop, float marginRight, float marginBottom, GAC_TYPE_COORDINATES coord)
{
	STATE_PROCESS(SetInternalMargins(marginLeft, marginTop, marginRight, marginBottom, coord));
	return true;
}
bool GApp::Controls::CheckBox::SetSpaceBetweenImageAndText(GAC_CHECKBOX_STATE_TYPE state, float value, GAC_TYPE_COORDINATES coord)
{
	STATE_PROCESS(SetSpaceBetweenImageAndText(value, coord));
	return true;
}
bool GApp::Controls::CheckBox::SetImageTextRelation(GAC_CHECKBOX_STATE_TYPE state, GAC_TYPE_IMAGETEXTRELATION relation, GAC_TYPE_ALIGNAMENT align)
{
	STATE_PROCESS(SetImageTextRelation(relation, align));
	return true;
}
