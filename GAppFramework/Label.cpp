#include "GApp.h"

GApp::Controls::Label::Label(void *scene, const char *layout, const char *text, GApp::Resources::Font *fnt, GAC_TYPE_ALIGNAMENT dock, unsigned int colorBlend) : GApp::Controls::GenericControl(scene, layout)
{	
	Content.SetText(text);
	Content.SetFont(fnt);
	Content.SetDockPosition(dock);	
	Content.SetColorBlending(colorBlend);
}
GApp::Controls::Label::Label(void *scene, const char *layout, const char *text, GApp::Resources::Font *fnt, GAC_TYPE_ALIGNAMENT dock, unsigned int colorBlend, float size, bool wordWrapz) : GApp::Controls::GenericControl(scene, layout)
{
	Content.SetText(text);
	Content.SetFont(fnt);
	Content.SetDockPosition(dock);
	Content.SetColorBlending(colorBlend);
	Content.SetWordWrap(wordWrapz);
	Content.SetFontSize(GAC_FONTSIZETYPE_SCALE, size);
}
void GApp::Controls::Label::OnPaint()
{
	G.UseDerivedShaderVariable = false;
	G.DrawString(&Content);
	G.UseDerivedShaderVariable = true;
}
bool GApp::Controls::Label::OnLayout()
{
	Content.SetViewRect(0,0,GetWidth(),GetHeight());
	return false;
}
void GApp::Controls::Label::SetText(const char *text)
{
	Content.SetText(text);
}
void GApp::Controls::Label::SetTextFont(GApp::Resources::Font *font)
{
	Content.SetFont(font);
}
void GApp::Controls::Label::SetTextFontSize(GAC_TYPE_FONTSIZETYPE mode,float size)
{
	Content.SetFontSize(mode,size);
}
void GApp::Controls::Label::SetTextShader(GApp::Resources::Shader *shader)
{
	Content.SetShader(shader);
}
bool GApp::Controls::Label::SetTextShaderColor(const char *VariableName, unsigned int color)
{
	Content.SetShaderUniformColor(VariableName, color);
	return true;
}
void GApp::Controls::Label::SetTextCharacterSpacing(float value)
{
	Content.SetCharacterSpacing(value);
}
void GApp::Controls::Label::SetTextSpaceWidth(float value)
{
	Content.SetSpaceWidth(value);
}
void GApp::Controls::Label::SetTextLineSpace(float value)
{
	Content.SetLineSpace(value);
}
void GApp::Controls::Label::SetTextDockPosition(GAC_TYPE_ALIGNAMENT value)
{
	Content.SetDockPosition(value);
}
void GApp::Controls::Label::SetTextWordWrap(bool value)
{
	Content.SetWordWrap(value);
}
void GApp::Controls::Label::SetTextJustify(bool value)
{
	Content.SetTextJustify(value);
}
void GApp::Controls::Label::SetTextPosition(float x,float y,GAC_TYPE_COORDINATES coord,GAC_TYPE_ALIGNAMENT align)
{
	Content.SetPosition(x, y, coord, align);
}
void GApp::Controls::Label::SetTextColorBlending(unsigned int color)
{
	Content.SetColorBlending(color);
}
void GApp::Controls::Label::SetTextAlphaBlending(float alpha)
{
	Content.SetAlphaBlending(alpha);
}