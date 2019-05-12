#include "GApp.h"

GApp::Controls::Toast::Toast(void *scene, const char *layout,GApp::Resources::Bitmap *img,GAC_TYPE_ALIGNAMENT align , GAC_TYPE_IMAGERESIZEMODE resizeMode): ImageTextView(scene,layout,img,align,resizeMode)
{
	curentFrame = 0;
	framesCount = 100;
	uniformIndex = 0;
	shaderptr = NULL;
	Hide();
}
GApp::Controls::Toast::Toast(void *scene, const char *layout,GApp::Resources::Bitmap *img,const char *text,GApp::Resources::Font *font,GAC_TYPE_ALIGNAMENT align,GAC_TYPE_IMAGERESIZEMODE resizeMode):ImageTextView(scene,layout,img,text,font,align,resizeMode)
{
	curentFrame = 0;
	framesCount = 100;
	uniformIndex = 0;
	shaderptr = NULL;
	Hide();
}
GApp::Controls::Toast::Toast(void *scene, const char *layout,const char *text,GApp::Resources::Font *font): ImageTextView(scene,layout,text,font)
{
	curentFrame = 0;
	framesCount = 100;
	uniformIndex = 0;
	shaderptr = NULL;
	Hide();
}

void GApp::Controls::Toast::SetAlphaShader(GApp::Resources::Shader *shader,unsigned int uIndex)
{
	Content.SetShader(shader);
	shaderptr = shader;
	uniformIndex = uIndex;
}
void GApp::Controls::Toast::SetShowTime(unsigned int frames)
{
	if (frames>0)
		framesCount = frames;
}
void GApp::Controls::Toast::Show()
{
	curentFrame = 0;
	SetVisible(true);
}
void GApp::Controls::Toast::Hide()
{
	curentFrame = 0;
	SetVisible(false);
}
void GApp::Controls::Toast::OnPaint()
{
	if (curentFrame>=framesCount)
	{
		Hide();
		return;
	}
	float alpha = 1.0f;
	if (curentFrame<framesCount/4)
		alpha = (float)(curentFrame*4)/(float)framesCount;
	else if (curentFrame>=(framesCount*3/4))
		alpha = (float)((framesCount-curentFrame)*4)/(float)framesCount;
	
	if (shaderptr != NULL)
	{
		G.UseDerivedShaderVariable = false;
		shaderptr->SetVariableValue(uniformIndex, alpha, true);
		G.Draw(&Content);
		G.UseDerivedShaderVariable = true;		
	}
	else {
		Content.SetAlphaBlending(alpha);
		G.Draw(&Content);
	}
	curentFrame++;
}