#include "GApp.h"

#define STATE_PROCESS(cmd) \
	switch (state) { \
		case GAC_BUTTON_STATE_NORMAL: Normal.cmd; break; \
		case GAC_BUTTON_STATE_PRESSED: Pressed.cmd; break; \
		case GAC_BUTTON_STATE_INACTIVE: Inactive.cmd; break; \
		case GAC_BUTTON_STATE_ALL: Normal.cmd; Pressed.cmd; Inactive.cmd; break; \
		case GAC_BUTTON_STATE_NORMAL_PRESSED: Normal.cmd; Pressed.cmd; break; \
		default: RETURNERROR(false, "Unkown button state: %d", state); \
	}

void GApp::Controls::Button::_Create(int objectID, GApp::Resources::Bitmap *BackgroundNormal, GApp::Resources::Bitmap *BackgroundPressed, GApp::Resources::Bitmap *BackgroundInactive, GApp::Resources::Bitmap *FaceNormal, GApp::Resources::Bitmap *FacePressed, GApp::Resources::Bitmap *FaceInactive)
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
		Normal.SetBackgroundImage(BackgroundNormal);
		Normal.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
		if (BackgroundPressed != NULL)
		{
			Pressed.SetBackgroundImage(BackgroundPressed);
			Pressed.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
		}
		else {
			Pressed.SetBackgroundImage(BackgroundNormal);
			Pressed.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
		}
		if (BackgroundInactive != NULL)
		{
			Inactive.SetBackgroundImage(BackgroundInactive);
			Inactive.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
		}
		else {
			Inactive.SetBackgroundImage(BackgroundNormal);
			Inactive.SetBackgroundLayout(imgMode, GAC_ALIGNAMENT_TOPLEFT);
		}

		if (FaceNormal != NULL)
		{
			Normal.SetFaceImage(FaceNormal);
			Normal.SetFaceLayout(GAC_IMAGERESIZEMODE_DOCK, GAC_ALIGNAMENT_CENTER);
		}
		if (FacePressed != NULL)
		{
			Pressed.SetFaceImage(FacePressed);
			Pressed.SetFaceLayout(GAC_IMAGERESIZEMODE_DOCK, GAC_ALIGNAMENT_CENTER);
		}
		if (FaceInactive != NULL)
		{
			Inactive.SetFaceImage(FaceInactive);
			Inactive.SetFaceLayout(GAC_IMAGERESIZEMODE_DOCK, GAC_ALIGNAMENT_CENTER);
		}
	}
}

GApp::Controls::Button::Button(void *scene,int objectID,const char* layout,GApp::Resources::Bitmap *BackgroundNormal,
									GApp::Resources::Bitmap *BackgroundPressed,
									GApp::Resources::Bitmap *BackgroundInactive,
									GApp::Resources::Bitmap *FaceNormal,
									GApp::Resources::Bitmap *FacePressed,
									GApp::Resources::Bitmap *FaceInactive): GApp::Controls::GenericControl(scene,layout)
{	
	_Create(objectID, BackgroundNormal, BackgroundPressed, BackgroundInactive, FaceNormal, FacePressed, FaceInactive);
}
GApp::Controls::Button::Button(void *scene, int ID, const char* layout, 
								GApp::Resources::Bitmap *Background, 
								GApp::Resources::Bitmap *Face, 
								unsigned int blendColorNormal, 
								unsigned int blendColorPressed, 
								unsigned int blendColorInactive) : GApp::Controls::GenericControl(scene, layout)
{
	_Create(ID, Background, Background, Background, Face, Face, Face);
	SetBackgroundColorBlending(GAC_BUTTON_STATE_NORMAL, blendColorNormal);
	SetBackgroundColorBlending(GAC_BUTTON_STATE_PRESSED, blendColorPressed);
	SetBackgroundColorBlending(GAC_BUTTON_STATE_INACTIVE, blendColorInactive);
	SetFaceColorBlending(GAC_BUTTON_STATE_NORMAL, blendColorNormal);
	SetFaceColorBlending(GAC_BUTTON_STATE_PRESSED, blendColorPressed);
	SetFaceColorBlending(GAC_BUTTON_STATE_INACTIVE, blendColorInactive);
}
void GApp::Controls::Button::OnPaint()
{
	if (IsEnabled()==false)
	{
		G.UseDerivedShaderVariable = false;
		G.Draw(&Inactive);
		G.UseDerivedShaderVariable = true;
		return;
	}
	if (HasTouch())
	{
		G.UseDerivedShaderVariable = false;
		G.Draw(&Pressed);
		G.UseDerivedShaderVariable = true;
		return;
	}
	G.UseDerivedShaderVariable = false;
	G.Draw(&Normal);
	G.UseDerivedShaderVariable = true;
}
bool GApp::Controls::Button::OnLayout()
{
	Normal.SetViewRect(0,0,GetWidth(),GetHeight());
	Pressed.SetViewRect(0,0,GetWidth(),GetHeight());
	Inactive.SetViewRect(0,0,GetWidth(),GetHeight());
	return false;
}
bool GApp::Controls::Button::OnTouchEvent(GApp::Controls::TouchEvent *te)
{
	return ProcessTouchEventForClick(te);
}



bool GApp::Controls::Button::SetBackgroundImage(GAC_BUTTON_STATE_TYPE state, GApp::Resources::Bitmap *Image, bool autoResize)
{
	STATE_PROCESS(SetBackgroundImage(Image));
	if ((autoResize) && (Image != NULL))
		Resize((float)Image->Width, (float)Image->Height);
	return true;
}
bool GApp::Controls::Button::SetBackgroundShader(GAC_BUTTON_STATE_TYPE state, GApp::Resources::Shader *shader)
{
	STATE_PROCESS(SetBackgroundShader(shader));
	return true;
}
bool GApp::Controls::Button::SetBackgroundShaderParams(GAC_BUTTON_STATE_TYPE state, const char *VariableName, unsigned int countParams, float p1, float p2 , float p3, float p4)
{
	STATE_PROCESS(Background.SetShaderUniformValue(VariableName, countParams, p1, p2, p3, p4));
	return true;
}
bool GApp::Controls::Button::SetBackgroundShaderColor(GAC_BUTTON_STATE_TYPE state, const char *VariableName, unsigned int color)
{
	STATE_PROCESS(Background.SetShaderUniformColor(VariableName, color));
	return true;
}
bool GApp::Controls::Button::SetBackgroundLayout(GAC_BUTTON_STATE_TYPE state, GAC_TYPE_IMAGERESIZEMODE mode, GAC_TYPE_ALIGNAMENT align)
{
	STATE_PROCESS(SetBackgroundLayout(mode, align));
	return true;
}

bool GApp::Controls::Button::SetFaceImage(GAC_BUTTON_STATE_TYPE state, GApp::Resources::Bitmap *Image)
{
	STATE_PROCESS(SetFaceImage(Image));
	return true;
}
bool GApp::Controls::Button::SetFaceShader(GAC_BUTTON_STATE_TYPE state, GApp::Resources::Shader *shader)
{
	STATE_PROCESS(SetFaceShader(shader));
	return true;
}
bool GApp::Controls::Button::SetFaceShaderParams(GAC_BUTTON_STATE_TYPE state, const char *VariableName, unsigned int countParams, float p1, float p2, float p3, float p4)
{
	STATE_PROCESS(Face.SetShaderUniformValue(VariableName, countParams, p1, p2, p3, p4));
	return true;
}
bool GApp::Controls::Button::SetFaceShaderColor(GAC_BUTTON_STATE_TYPE state, const char *VariableName, unsigned int color)
{
	STATE_PROCESS(Face.SetShaderUniformColor(VariableName, color));
	return true;
}
bool GApp::Controls::Button::SetFaceLayout(GAC_BUTTON_STATE_TYPE state, GAC_TYPE_IMAGERESIZEMODE mode, GAC_TYPE_ALIGNAMENT align)
{
	STATE_PROCESS(SetFaceLayout(mode, align));
	return true;
}
bool GApp::Controls::Button::SetFacePosition(GAC_BUTTON_STATE_TYPE state, float x, float y, GAC_TYPE_COORDINATES coord, GAC_TYPE_ALIGNAMENT align)
{
	STATE_PROCESS(SetFacePosition(x, y, coord, align));
	return true;
}
bool GApp::Controls::Button::SetFaceViewRect(GAC_BUTTON_STATE_TYPE state, float left, float top, float right, float bottom, GAC_TYPE_COORDINATES coord)
{
	STATE_PROCESS(SetFaceViewRect(left, top, right, bottom, coord));
	return true;
}

//Text
bool GApp::Controls::Button::SetText(GAC_BUTTON_STATE_TYPE state, const char *text)
{
	STATE_PROCESS(SetText(text));
	return true;
}
bool GApp::Controls::Button::SetTextFont(GAC_BUTTON_STATE_TYPE state, GApp::Resources::Font *font)
{
	STATE_PROCESS(SetTextFont(font));
	return true;
}
bool GApp::Controls::Button::SetTextFontSize(GAC_BUTTON_STATE_TYPE state, GAC_TYPE_FONTSIZETYPE mode, float value)
{
	STATE_PROCESS(SetTextFontSize(mode,value));
	return true;
}
bool GApp::Controls::Button::SetTextShader(GAC_BUTTON_STATE_TYPE state, GApp::Resources::Shader *shader)
{
	STATE_PROCESS(SetTextShader(shader));
	return true;
}
bool GApp::Controls::Button::SetTextCharacterSpacing(GAC_BUTTON_STATE_TYPE state, float value)
{
	STATE_PROCESS(SetTextCharacterSpacing(value));
	return true;
}
bool GApp::Controls::Button::SetTextSpaceWidth(GAC_BUTTON_STATE_TYPE state, float value)
{
	STATE_PROCESS(SetTextSpaceWidth(value));
	return true;
}
bool GApp::Controls::Button::SetTextLineSpace(GAC_BUTTON_STATE_TYPE state, float value)
{
	STATE_PROCESS(SetTextLineSpace(value));
	return true;
}
bool GApp::Controls::Button::SetTextDockPosition(GAC_BUTTON_STATE_TYPE state, GAC_TYPE_ALIGNAMENT value)
{
	STATE_PROCESS(SetTextDockPosition(value));
	return true;
}
bool GApp::Controls::Button::SetTextWordWrap(GAC_BUTTON_STATE_TYPE state, bool value)
{
	STATE_PROCESS(SetTextWordWrap(value));
	return true;
}
bool GApp::Controls::Button::SetTextViewRect(GAC_BUTTON_STATE_TYPE state, float left, float top, float right, float bottom, GAC_TYPE_COORDINATES coord)
{
	STATE_PROCESS(SetTextViewRect(left,top,right,bottom));
	return true;
}
bool GApp::Controls::Button::SetTextShaderParams(GAC_BUTTON_STATE_TYPE state, const char *VariableName, unsigned int countParams, float p1, float p2, float p3, float p4)
{
	STATE_PROCESS(Text.SetShaderUniformValue(VariableName, countParams, p1, p2, p3, p4));
	return true;
}
bool GApp::Controls::Button::SetTextShaderColor(GAC_BUTTON_STATE_TYPE state, const char *VariableName, unsigned int color)
{
	STATE_PROCESS(Text.SetShaderUniformColor(VariableName, color));
	return true;
}


bool GApp::Controls::Button::SetInternalMargins(GAC_BUTTON_STATE_TYPE state,float marginLeft, float marginTop, float marginRight, float marginBottom, GAC_TYPE_COORDINATES coord)
{
	STATE_PROCESS(SetInternalMargins(marginLeft, marginTop, marginRight, marginBottom, coord));
	return true;
}
bool GApp::Controls::Button::SetSpaceBetweenImageAndText(GAC_BUTTON_STATE_TYPE state,float value, GAC_TYPE_COORDINATES coord)
{
	STATE_PROCESS(SetSpaceBetweenImageAndText(value, coord));
	return true;
}
bool GApp::Controls::Button::SetImageTextRelation(GAC_BUTTON_STATE_TYPE state,GAC_TYPE_IMAGETEXTRELATION relation, GAC_TYPE_ALIGNAMENT align)
{
	STATE_PROCESS(SetImageTextRelation(relation, align));
	return true;
}

bool GApp::Controls::Button::SetBackgroundAlphaBlending(GAC_BUTTON_STATE_TYPE state, float alpha)
{
	STATE_PROCESS(SetBackgroundAlphaBlending(alpha));
	return true;
}
bool GApp::Controls::Button::SetBackgroundColorBlending(GAC_BUTTON_STATE_TYPE state, unsigned int color)
{
	STATE_PROCESS(SetBackgroundColorBlending(color));
	return true;
}
bool GApp::Controls::Button::SetFaceAlphaBlending(GAC_BUTTON_STATE_TYPE state, float alpha)
{
	STATE_PROCESS(SetFaceAlphaBlending(alpha));
	return true;
}
bool GApp::Controls::Button::SetFaceColorBlending(GAC_BUTTON_STATE_TYPE state, unsigned int color)
{
	STATE_PROCESS(SetFaceColorBlending(color));
	return true;
}
bool GApp::Controls::Button::SetTextAlphaBlending(GAC_BUTTON_STATE_TYPE state, float alpha)
{
	STATE_PROCESS(SetTextAlphaBlending(alpha));
	return true;
}
bool GApp::Controls::Button::SetTextColorBlending(GAC_BUTTON_STATE_TYPE state, unsigned int color)
{
	STATE_PROCESS(SetTextColorBlending(color));
	return true;
}