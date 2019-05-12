#include "GApp.h"

void GApp::Graphics::TextImagePainter::RecomputeLayout()
{
	float spaceW,spaceH,l,t,r,b,iw,ih,tw,th,x,y,ml,mr,mt,mb;
	float res_l,res_t,res_r,res_b;

	x = 0;
	y = 0;
	
	Background.SetViewRect(ViewRect.Left,ViewRect.Top,ViewRect.Right,ViewRect.Bottom);
	l = Background.ViewLeft;
	t = Background.ViewTop;
	r = Background.ViewRight;
	b = Background.ViewBottom;
	
	TRANSLATE_COORD_TO_PIXELS(spaceW,spaceH,ImageTextSpace,ImageTextSpace,r-l,b-t,ImageTextSpaceCoordType);
	TRANSLATE_COORD_TO_PIXELS(ml,mt,MarginRect.Left,MarginRect.Top,r-l,b-t,MarginCoordType);
	TRANSLATE_COORD_TO_PIXELS(mr,mb,MarginRect.Right,MarginRect.Bottom,r-l,b-t,MarginCoordType);

	l+=ml;
	t+=mt;
	r-=mr;
	b-=mb;
	tw = r-l;
	th = b-t;

	if (Face.Image==NULL)
	{
		iw = ih = 0;
	} else {
		iw = Face.Width;
		ih = Face.Height;
		if ((iw==0) && (ih==0))
		{
			// nu a apucat sa faca nici un calcul
			TRANSLATE_COORD_TO_PIXELS(res_l,res_t,FaceViewRect.Left,FaceViewRect.Top,tw,th,FaceViewCoordType);
			TRANSLATE_COORD_TO_PIXELS(res_r,res_b,FaceViewRect.Right,FaceViewRect.Bottom,tw,th,FaceViewCoordType);
			Face.SetViewRect(res_l+l,res_t+t,res_r+l,res_b+t);
			iw = Face.Width;
			ih = Face.Height;
		}
	}

	switch (ImageTextRelation)
	{
		case GAC_IMAGETEXTRELATION_NONE:
			TRANSLATE_COORD_TO_PIXELS(res_l,res_t,FaceViewRect.Left,FaceViewRect.Top,tw,th,FaceViewCoordType);
			TRANSLATE_COORD_TO_PIXELS(res_r,res_b,FaceViewRect.Right,FaceViewRect.Bottom,tw,th,FaceViewCoordType);
			Face.SetViewRect(res_l+l,res_t+t,res_r+l,res_b+t);
			TRANSLATE_COORD_TO_PIXELS(res_l,res_t,TextViewRect.Left,TextViewRect.Top,tw,th,TextViewCoordType);
			TRANSLATE_COORD_TO_PIXELS(res_r,res_b,TextViewRect.Right,TextViewRect.Bottom,tw,th,TextViewCoordType);
			Text.SetViewRect(res_l+l,res_t+t,res_r+l,res_b+t);
			break;
		case GAC_IMAGETEXTRELATION_IMAGEONLEFTOFTHETEXT:			
			Text.SetViewRect(l+iw+spaceW,t,r,b);
			tw = iw+spaceW+Text.GetTextWidth();
			ALIGN_OBJECT_TO_RECT(x,y,l,t,r,b,tw,th,ImageTextAlignament);
			Face.SetViewRect(x,t,x+iw,b);
			Text.SetViewRect(x+iw+spaceW,t,x+iw+spaceW+Text.GetTextWidth(),b);
			break;
		case GAC_IMAGETEXTRELATION_IMAGEONRIGHTOFTHETEXT:			
			Text.SetViewRect(l+iw+spaceW,t,r,b);
			tw = iw+spaceW+Text.GetTextWidth();
			ALIGN_OBJECT_TO_RECT(x,y,l,t,r,b,tw,th,ImageTextAlignament);
			Text.SetViewRect(x,t,x+Text.GetTextWidth(),b);				
			Face.SetViewRect(x+Text.GetTextWidth()+spaceW,t,x+Text.GetTextWidth()+spaceW+iw,b);			
			break;
		case GAC_IMAGETEXTRELATION_IMAGEONTOPOFTHETEXT:			
			Text.SetViewRect(l,t+ih+spaceH,r,b);
			th = ih+spaceH+Text.GetTextHeight();
			ALIGN_OBJECT_TO_RECT(x,y,l,t,r,b,tw,th,ImageTextAlignament);
			Face.SetViewRect(l,y,r,y+ih);
			Text.SetViewRect(l,y+ih+spaceH,r,y+ih+spaceH+Text.GetTextHeight());
			break;
		case GAC_IMAGETEXTRELATION_IMAGEONBOTTOMOFTHETEXT:			
			Text.SetViewRect(l,t+ih+spaceH,r,b);
			th = ih+spaceH+Text.GetTextHeight();
			ALIGN_OBJECT_TO_RECT(x,y,l,t,r,b,tw,th,ImageTextAlignament);
			Text.SetViewRect(l,y,r,y+Text.GetTextHeight());
			Face.SetViewRect(l,y+Text.GetTextHeight()+spaceH,r,y+Text.GetTextHeight()+spaceH+ih);
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------
GApp::Graphics::TextImagePainter::TextImagePainter()
{
	ImageTextRelation = GAC_IMAGETEXTRELATION_NONE;
	ViewRect.Set(0,0,0,0);
	FaceViewRect.Set(0,0,1,1);
	TextViewRect.Set(0,0,1,1);
	MarginRect.Set(0,0,0,0);
	ImageTextAlignament = GAC_ALIGNAMENT_CENTER;
	ImageTextSpace = 0;
	MarginCoordType= GAC_COORDINATES_PERCENTAGE;
	FaceViewCoordType = GAC_COORDINATES_PERCENTAGE;
	TextViewCoordType = GAC_COORDINATES_PERCENTAGE;
	ImageTextSpaceCoordType = GAC_COORDINATES_PERCENTAGE;
}
void GApp::Graphics::TextImagePainter::SetViewRect(float left,float top,float right,float bottom)
{
	if ((left!=ViewRect.Left) || (top!=ViewRect.Top) || (right!=ViewRect.Right) || (bottom!=ViewRect.Bottom))
	{
		ViewRect.Set(left,top,right,bottom);
		RecomputeLayout();
	}
	
}
void GApp::Graphics::TextImagePainter::SetViewRectWH(float left, float top, float width, float height)
{
	SetViewRect(left, top, left + width, top + height);
}
bool GApp::Graphics::TextImagePainter::SetViewRectFromImage(GApp::Resources::Bitmap *backgroundImage)
{
	CHECK(backgroundImage!=NULL,false,"");
	if (backgroundImage!=Background.Image)
	{
		CHECK(Background.Create(backgroundImage,GAC_IMAGERESIZEMODE_NONE,GAC_ALIGNAMENT_TOPLEFT),false,"Failed to create Background ImagePainter object");
		ViewRect.Set(0,0,backgroundImage->Width,backgroundImage->Height);	
		RecomputeLayout();
	}
	return true;
}
void GApp::Graphics::TextImagePainter::SetShader(GApp::Resources::Shader *shader)
{
	Background.SetShader(shader);
	Face.SetShader(shader);
	Text.SetShader(shader);
}
void GApp::Graphics::TextImagePainter::SetAlphaBlending(float alpha)
{
	Background.SetAlphaBlending(alpha);
	Face.SetAlphaBlending(alpha);
	Text.SetAlphaBlending(alpha);
}
void GApp::Graphics::TextImagePainter::SetColorBlending(unsigned int color)
{
	Background.SetColorBlending(color);
	Face.SetColorBlending(color);
	Text.SetColorBlending(color);
}
void GApp::Graphics::TextImagePainter::SetInternalMargins(float marginLeft,float marginTop,float marginRight,float marginBottom,GAC_TYPE_COORDINATES coord)
{
	if ((marginLeft!=MarginRect.Left) || (marginTop!=MarginRect.Top) || (marginRight!=MarginRect.Right) || (marginBottom!=MarginRect.Bottom) || (coord!=MarginCoordType))
	{
		MarginRect.Set(marginLeft,marginTop,marginRight,marginBottom);
		MarginCoordType = coord;
		RecomputeLayout();
	}
}
void GApp::Graphics::TextImagePainter::SetSpaceBetweenImageAndText(float value,GAC_TYPE_COORDINATES coord)
{
	if ((coord!=ImageTextSpaceCoordType) || (value!=ImageTextSpace))
	{
		ImageTextSpace = value;
		ImageTextSpaceCoordType = coord;
		RecomputeLayout();
	}
}
void GApp::Graphics::TextImagePainter::SetImageTextRelation(GAC_TYPE_IMAGETEXTRELATION relation,GAC_TYPE_ALIGNAMENT align)
{
	if ((relation!=ImageTextRelation) || (align!=ImageTextAlignament))
	{
		ImageTextRelation = relation;
		ImageTextAlignament = align;
		RecomputeLayout();
	}
}
//----------------------------------- BACKGROUND --------------------------------------------------------------------------
void GApp::Graphics::TextImagePainter::SetBackgroundImage(GApp::Resources::Bitmap *image)
{
	if (image!=Background.Image)
	{
		Background.SetImage(image);
		RecomputeLayout();
	}
}
void GApp::Graphics::TextImagePainter::SetBackgroundShader(GApp::Resources::Shader *shader)
{
	Background.SetShader(shader);
}
void GApp::Graphics::TextImagePainter::SetBackgroundAlphaBlending(float alpha)
{
	Background.SetAlphaBlending(alpha);
}
void GApp::Graphics::TextImagePainter::SetBackgroundColorBlending(unsigned int color)
{
	Background.SetColorBlending(color);
}
void GApp::Graphics::TextImagePainter::SetBackgroundLayout(GAC_TYPE_IMAGERESIZEMODE mode,GAC_TYPE_ALIGNAMENT align)
{
	Background.SetLayout(mode,align);
	RecomputeLayout();
}
void GApp::Graphics::TextImagePainter::SetBackgroundPosition(float x,float y,GAC_TYPE_COORDINATES coord,GAC_TYPE_ALIGNAMENT align)
{
	Background.SetPosition(x,y,coord,align);
	RecomputeLayout();
}
void GApp::Graphics::TextImagePainter::SetBackgroundScale(float scaleWidth,float scaleHeigh)
{
	Background.SetScale(scaleWidth,scaleHeigh);
	RecomputeLayout();
}
void GApp::Graphics::TextImagePainter::SetBackgroundImageSize(float width,float height,GAC_TYPE_COORDINATES coord)
{
	Background.SetImageSize(width,height,coord);
	RecomputeLayout();
}
//----------------------------------- FACE --------------------------------------------------------------------------
void GApp::Graphics::TextImagePainter::SetFaceImage(GApp::Resources::Bitmap *image)
{
	if (image!=Face.Image)
	{
		Face.SetImage(image);
		RecomputeLayout();
	}
}
void GApp::Graphics::TextImagePainter::SetFaceShader(GApp::Resources::Shader *shader)
{
	Face.SetShader(shader);
}
void GApp::Graphics::TextImagePainter::SetFaceAlphaBlending(float alpha)
{
	Face.SetAlphaBlending(alpha);
}
void GApp::Graphics::TextImagePainter::SetFaceColorBlending(unsigned int color)
{
	Face.SetColorBlending(color);
}
void GApp::Graphics::TextImagePainter::SetFaceLayout(GAC_TYPE_IMAGERESIZEMODE mode,GAC_TYPE_ALIGNAMENT align)
{
	Face.SetLayout(mode,align);
	RecomputeLayout();
}
void GApp::Graphics::TextImagePainter::SetFacePosition(float x,float y,GAC_TYPE_COORDINATES coord,GAC_TYPE_ALIGNAMENT align)
{
	Face.SetPosition(x,y,coord,align);
	RecomputeLayout();
}
void GApp::Graphics::TextImagePainter::SetFaceScale(float scaleWidth,float scaleHeigh)
{
	Face.SetScale(scaleWidth,scaleHeigh);
	RecomputeLayout();
}
void GApp::Graphics::TextImagePainter::SetFaceImageSize(float width,float height,GAC_TYPE_COORDINATES coord)
{
	Face.SetImageSize(width,height,coord);
	RecomputeLayout();
}
void GApp::Graphics::TextImagePainter::SetFaceViewRect(float left,float top,float right,float bottom,GAC_TYPE_COORDINATES coord)
{
	if ((left!=FaceViewRect.Left) || (top!=FaceViewRect.Top) || (right!=FaceViewRect.Right) || (bottom!=FaceViewRect.Bottom) || (coord!=FaceViewCoordType))
	{
		ImageTextRelation = GAC_IMAGETEXTRELATION_NONE;
		FaceViewRect.Set(left,top,right,bottom);
		FaceViewCoordType = coord;
		RecomputeLayout();
	}	
}
//----------------------------------- TEXT ----------------------------------------------------------------------------
bool GApp::Graphics::TextImagePainter::SetTextFont(GApp::Resources::Font *fnt)
{
	if (Text.TextFont!=fnt)
	{
		Text.SetFont(fnt);
		RecomputeLayout();
	}
	return true;
}
void GApp::Graphics::TextImagePainter::SetTextLineSpace(float spacePercentage)
{
	if (Text.HeightPercentage!=(spacePercentage+1))
	{
		Text.SetLineSpace(spacePercentage);
		RecomputeLayout();
	}
}
bool GApp::Graphics::TextImagePainter::SetText(const char *text)
{
	if (Text.SetText(text)==false)
	{
		RecomputeLayout();
		return false;
	}
	RecomputeLayout();
	return true;
}
void GApp::Graphics::TextImagePainter::SetTextWordWrap(bool value)
{
	Text.SetWordWrap(value);
	RecomputeLayout();
}
void GApp::Graphics::TextImagePainter::SetTextJustify(bool value)
{
	Text.SetTextJustify(value);
	RecomputeLayout();
}
void GApp::Graphics::TextImagePainter::SetTextFontSize(GAC_TYPE_FONTSIZETYPE mode,float scale)
{
	Text.SetFontSize(mode,scale);
	RecomputeLayout();
}
void GApp::Graphics::TextImagePainter::SetTextPosition(float x,float y,GAC_TYPE_COORDINATES coord,GAC_TYPE_ALIGNAMENT align)
{
	Text.SetPosition(x,y,coord,align);
	RecomputeLayout();
}
void GApp::Graphics::TextImagePainter::SetTextDockPosition(GAC_TYPE_ALIGNAMENT dockAlignament)
{
	Text.SetDockPosition(dockAlignament);
	RecomputeLayout();
}
void GApp::Graphics::TextImagePainter::SetTextShader(GApp::Resources::Shader *shader)
{
	Text.SetShader(shader);
}
void GApp::Graphics::TextImagePainter::SetTextAlphaBlending(float alpha)
{
	Text.SetAlphaBlending(alpha);
}
void GApp::Graphics::TextImagePainter::SetTextColorBlending(unsigned int color)
{
	Text.SetColorBlending(color);
}
void GApp::Graphics::TextImagePainter::SetTextCharacterSpacing(float value)
{
	Text.SetCharacterSpacing(value);
	RecomputeLayout();
}
void GApp::Graphics::TextImagePainter::SetTextSpaceWidth(float value)
{
	Text.SetSpaceWidth(value);
	RecomputeLayout();
}
void GApp::Graphics::TextImagePainter::SetTextViewRect(float left,float top,float right,float bottom,GAC_TYPE_COORDINATES coord)
{
	if ((left!=TextViewRect.Left) || (top!=TextViewRect.Top) || (right!=TextViewRect.Right) || (bottom!=TextViewRect.Bottom) || (coord!=TextViewCoordType))
	{
		ImageTextRelation = GAC_IMAGETEXTRELATION_NONE;
		TextViewRect.Set(left,top,right,bottom);
		TextViewCoordType = coord;
		RecomputeLayout();
	}
}