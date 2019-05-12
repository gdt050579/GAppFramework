#include "GApp.h"

GApp::Controls::Panel::Panel(void* scene,const char *layout): GApp::Controls::GenericControl(scene,layout)
{
}
GApp::Controls::Panel::Panel(void* scene,const char *layout,GApp::Resources::Bitmap *img,GAC_TYPE_ALIGNAMENT align,GAC_TYPE_IMAGERESIZEMODE resizeMode): GApp::Controls::GenericControl(scene,layout)
{	
	if (IS_CONTROL_AUTOSIZED)
	{
		if (img!=NULL)
			Resize(img->Width,img->Height);
	}
	Image.Create(img,resizeMode,align);	
}
void GApp::Controls::Panel::OnPaint()
{
	G.Draw(&Image);
}
bool GApp::Controls::Panel::OnLayout()
{
	Image.SetViewRect(0,0,GetWidth(),GetHeight());
	return false;
}
void GApp::Controls::Panel::SetImage(GApp::Resources::Bitmap *image)
{
	Image.SetImage(image);
}
void GApp::Controls::Panel::SetImageSize(float width,float height,GAC_TYPE_COORDINATES coord)
{
	Image.SetImageSize(width,height,coord);
}
void GApp::Controls::Panel::SetScale(float scaleWidth,float scaleHeight)
{
	Image.SetScale(scaleWidth,scaleHeight);
}
void GApp::Controls::Panel::SetPosition(float x,float y,GAC_TYPE_COORDINATES coord,GAC_TYPE_ALIGNAMENT align)
{
	Image.SetPosition(x, y, coord, align);
}
void GApp::Controls::Panel::SetImageLayout(GAC_TYPE_IMAGERESIZEMODE resizeMode, GAC_TYPE_ALIGNAMENT align)
{
	Image.SetLayout(resizeMode,align);
}
void GApp::Controls::Panel::SetShader(GApp::Resources::Shader *shader)
{
	Image.SetShader(shader);
}