#include "GApp.h"

GApp::Controls::ImageTextView::ImageTextView(void *scene,const char *layout,GApp::Resources::Bitmap *img,GAC_TYPE_ALIGNAMENT align,GAC_TYPE_IMAGERESIZEMODE resizeMode): GApp::Controls::GenericControl(scene,layout)
{
	if (IS_CONTROL_AUTOSIZED)
	{
		if (img!=NULL)
			Resize((float)img->Width,(float)img->Height);
	}
	Content.SetBackgroundImage(img);
	Content.SetBackgroundLayout(resizeMode,align);	
}
GApp::Controls::ImageTextView::ImageTextView(void *scene,const char *layout,GApp::Resources::Bitmap *img,const char *text,GApp::Resources::Font *font, GAC_TYPE_ALIGNAMENT align, GAC_TYPE_IMAGERESIZEMODE resizeMode): GApp::Controls::GenericControl(scene,layout)
{
	if (IS_CONTROL_AUTOSIZED)
	{
		if (img!=NULL)
			Resize((float)img->Width,(float)img->Height);
	}
	Content.SetBackgroundImage(img);
	Content.SetBackgroundLayout(resizeMode,align);	
	Content.SetText(text);
	Content.SetTextFont(font);
	Content.SetTextDockPosition(GAC_ALIGNAMENT_CENTER);
}
GApp::Controls::ImageTextView::ImageTextView(void *scene,const char *layout,const char *text,GApp::Resources::Font *font): GApp::Controls::GenericControl(scene,layout)
{	
	Content.SetText(text);
	Content.SetTextFont(font);
	Content.SetTextDockPosition(GAC_ALIGNAMENT_CENTER);
}
void GApp::Controls::ImageTextView::OnPaint()
{
	G.UseDerivedShaderVariable = false;
	G.Draw(&Content);
	G.UseDerivedShaderVariable = true;
}
bool GApp::Controls::ImageTextView::OnLayout()
{
	//LOG_INFO("Setez dimensiune la %f x %f",GetWidth(),GetHeight());
	Content.SetViewRect(0,0,GetWidth(),GetHeight());
	return false;
}
void GApp::Controls::ImageTextView::SetFaceImage(GApp::Resources::Bitmap *FaceImage)
{
	Content.SetFaceImage(FaceImage);
}
void GApp::Controls::ImageTextView::SetText(const char *text)
{
	Content.SetText(text);
}
void GApp::Controls::ImageTextView::SetTextFont(GApp::Resources::Font *font)
{
	Content.SetTextFont(font);
}
void GApp::Controls::ImageTextView::SetTextFontSize(GAC_TYPE_FONTSIZETYPE mode,float size)
{
	Content.SetTextFontSize(mode,size);
}
void GApp::Controls::ImageTextView::SetTextShader(GApp::Resources::Shader *shader)
{
	Content.SetTextShader(shader);
}
bool GApp::Controls::ImageTextView::SetTextShaderParams(const char *VariableName, unsigned int countParams, float p1, float p2, float p3, float p4)
{
	return Content.Text.SetShaderUniformValue(VariableName, countParams, p1, p2, p3, p4);
}
bool GApp::Controls::ImageTextView::SetTextShaderColor(const char *VariableName, unsigned int color)
{
	return Content.Text.SetShaderUniformColor(VariableName, color);
}
void GApp::Controls::ImageTextView::SetTextCharacterSpacing(float value)
{
	Content.SetTextCharacterSpacing(value);
}
void GApp::Controls::ImageTextView::SetTextSpaceWidth(float value)
{
	Content.SetTextSpaceWidth(value);
}
void GApp::Controls::ImageTextView::SetTextLineSpace(float value)
{
	Content.SetTextLineSpace(value);
}
void GApp::Controls::ImageTextView::SetTextDockPosition(GAC_TYPE_ALIGNAMENT value)
{
	Content.SetTextDockPosition(value);
}
void GApp::Controls::ImageTextView::SetTextWordWrap(bool value)
{
	Content.SetTextWordWrap(value);
}
void GApp::Controls::ImageTextView::SetTextJustify(bool value)
{
	Content.SetTextJustify(value);
}
void GApp::Controls::ImageTextView::SetTextPosition(float x,float y,GAC_TYPE_COORDINATES coord,GAC_TYPE_ALIGNAMENT align)
{
	Content.SetTextPosition(x, y, coord, align);
}
void GApp::Controls::ImageTextView::SetFaceImageSize(float width,float height,GAC_TYPE_COORDINATES coord)
{
	Content.SetFaceImageSize(width,height,coord);	
}
void GApp::Controls::ImageTextView::SetFaceScale(float scaleWidth,float scaleHeight)
{
	Content.SetFaceScale(scaleWidth,scaleHeight);
}
void GApp::Controls::ImageTextView::SetFacePosition(float x,float y,GAC_TYPE_COORDINATES coord,GAC_TYPE_ALIGNAMENT align)
{
	Content.SetFacePosition(x, y, coord, align);
}
void GApp::Controls::ImageTextView::SetFaceLayout(GAC_TYPE_IMAGERESIZEMODE resizeMode, GAC_TYPE_ALIGNAMENT align)
{
	Content.SetFaceLayout(resizeMode,align);
}
void GApp::Controls::ImageTextView::SetFaceShader(GApp::Resources::Shader *shader)
{
	Content.SetFaceShader(shader);
}
bool GApp::Controls::ImageTextView::SetFaceShaderParams(const char *VariableName, unsigned int countParams, float p1, float p2, float p3, float p4)
{
	return Content.Face.SetShaderUniformValue(VariableName, countParams, p1, p2, p3, p4);
}
bool GApp::Controls::ImageTextView::SetFaceShaderColor(const char *VariableName, unsigned int color)
{
	return Content.Face.SetShaderUniformColor(VariableName, color);
}
void GApp::Controls::ImageTextView::SetFaceViewRect(float left,float top,float right,float bottom,GAC_TYPE_COORDINATES coord)
{
	Content.SetFaceViewRect(left,right,top,bottom,coord);
}
void GApp::Controls::ImageTextView::SetTextViewRect(float left,float top,float right,float bottom,GAC_TYPE_COORDINATES coord)
{
	Content.SetTextViewRect(left,right,top,bottom,coord);
}
void GApp::Controls::ImageTextView::SetInternalMargins(float marginLeft,float marginTop,float marginRight,float marginBottom,GAC_TYPE_COORDINATES coord)
{
	Content.SetInternalMargins(marginLeft,marginTop,marginRight,marginBottom,coord);
}
void GApp::Controls::ImageTextView::SetSpaceBetweenImageAndText(float value,GAC_TYPE_COORDINATES coord)
{
	Content.SetSpaceBetweenImageAndText(value,coord);
}
void GApp::Controls::ImageTextView::SetImageTextRelation(GAC_TYPE_IMAGETEXTRELATION relation,GAC_TYPE_ALIGNAMENT align)
{
	Content.SetImageTextRelation(relation,align);
}
void GApp::Controls::ImageTextView::SetBackgroundImage(GApp::Resources::Bitmap *image)
{
	Content.SetBackgroundImage(image);
}
void GApp::Controls::ImageTextView::SetBackgroundShader(GApp::Resources::Shader *shader)
{
	Content.SetBackgroundShader(shader);
}
bool GApp::Controls::ImageTextView::SetBackgroundShaderParams(const char *VariableName, unsigned int countParams, float p1, float p2, float p3, float p4)
{
	return Content.Background.SetShaderUniformValue(VariableName, countParams, p1, p2, p3, p4);
}
bool GApp::Controls::ImageTextView::SetBackgroundShaderColor(const char *VariableName, unsigned int color)
{
	return Content.Background.SetShaderUniformColor(VariableName, color);
}
void GApp::Controls::ImageTextView::SetBackgroundLayout(GAC_TYPE_IMAGERESIZEMODE mode,GAC_TYPE_ALIGNAMENT align)
{
	Content.SetBackgroundLayout(mode,align);
}
void GApp::Controls::ImageTextView::SetBackgroundPosition(float x,float y,GAC_TYPE_COORDINATES coord,GAC_TYPE_ALIGNAMENT align)
{
	Content.SetBackgroundPosition(x, y, coord, align);
}
void GApp::Controls::ImageTextView::SetBackgroundScale(float scaleWidth,float scaleHeight)
{
	Content.SetBackgroundScale(scaleWidth,scaleHeight);
}
void GApp::Controls::ImageTextView::SetBackgroundImageSize(float width,float height,GAC_TYPE_COORDINATES coord)
{
	Content.SetBackgroundImageSize(width,height,coord);
}
void GApp::Controls::ImageTextView::SetBackgroundAlphaBlending(float alpha)
{
	Content.SetBackgroundAlphaBlending(alpha);
}
void GApp::Controls::ImageTextView::SetBackgroundColorBlending(unsigned int color)
{
	Content.SetBackgroundColorBlending(color);
}
void GApp::Controls::ImageTextView::SetFaceAlphaBlending(float alpha)
{
	Content.SetFaceAlphaBlending(alpha);
}
void GApp::Controls::ImageTextView::SetFaceColorBlending(unsigned int color)
{
	Content.SetFaceColorBlending(color);
}
void GApp::Controls::ImageTextView::SetTextAlphaBlending(float alpha)
{
	Content.SetTextAlphaBlending(alpha);
}
void GApp::Controls::ImageTextView::SetTextColorBlending(unsigned int color)
{
	Content.SetTextColorBlending(color);
}