#include "GApp.h"


/*
FLAGS:	0,1,2		3 biti - ImageResizeMode
		3,4,5,6		4 biti - Alignament
		7,8,9		3 biti - Coordonatele pe X,Y
		10,11,12	3 biti - size-ul (w x h)
*/

void RecomputeLayout(GApp::Graphics::ImagePainter *img)
{
	// curat falgul care valideaza
	img->Flags &= PAINT_OBJECT_MASK;

	// verific ca am date ok
	if ((img->ViewRight<=img->ViewLeft) || (img->ViewBottom<=img->ViewTop))
		return;
	if (img->Image==NULL)
		return;

	float iWidth,iHeight,iX,iY,rapX,rapY;	
	GAC_TYPE_COORDINATES ct;

	ct = (GAC_TYPE_COORDINATES)GET_BITFIELD(img->Flags,10,3);
	switch (ct)
	{
		case GAC_COORDINATES_PIXELS:
			if (img->ImageWidth>=0)
			{
				iWidth = img->ImageWidth;
				iHeight = img->ImageHeight;
			} else {
				iWidth = img->Image->Width * img->ScaleWidth;
				iHeight = img->Image->Height * img->ScaleHeight;
			}
			break;
		case GAC_COORDINATES_PERCENTAGE:
			if (img->ImageWidth>=0)
			{
				iWidth = (img->ViewRight - img->ViewLeft)*img->ImageWidth;
				iHeight = (img->ViewBottom - img->ViewTop)*img->ImageHeight;
			} else {
				iWidth = img->Image->Width * img->ScaleWidth;
				iHeight = img->Image->Height * img->ScaleHeight;
			}
			break;
		case GAC_COORDINATES_AUTO:			
			//iWidth = img->Image->Width * img->ScaleWidth;
			//iHeight = img->Image->Height * img->ScaleHeight;
			iWidth = iHeight = 0;
			break;
		default:
			LOG_ERROR("ImagePainter::RecomputeLayout() - invalid size coordinate type");
			return;
	}	
	ct = (GAC_TYPE_COORDINATES)GET_BITFIELD(img->Flags,7,3);
	switch (ct)
	{
		case GAC_COORDINATES_PIXELS:
			iX = img->X+img->ViewLeft;
			iY = img->Y+img->ViewTop;
			break;
		case GAC_COORDINATES_PERCENTAGE:
			iX = (img->ViewRight - img->ViewLeft)*img->X+img->ViewTop;
			iY = (img->ViewBottom - img->ViewTop)*img->Y+img->ViewLeft;
			break;
		case GAC_COORDINATES_AUTO:			
			iX = iY = 0;
			break;
		default:
			LOG_ERROR("ImagePainter::RecomputeLayout() - invalid size coordinate type");
			return;
	}

	GAC_TYPE_IMAGERESIZEMODE imgRM = (GAC_TYPE_IMAGERESIZEMODE)GET_BITFIELD(img->Flags,0,3);
	GAC_TYPE_ALIGNAMENT align = (GAC_TYPE_ALIGNAMENT)GET_BITFIELD(img->Flags,3,4);
	switch (imgRM)
	{
		case GAC_IMAGERESIZEMODE_NONE:
			ALIGN_COORD(img->Left,img->Top,iX,iY,iWidth,iHeight,align);
			img->Width = iWidth;
			img->Height = iHeight;
			break;
		case GAC_IMAGERESIZEMODE_DOCK:
			ALIGN_OBJECT_TO_RECT(img->Left,img->Top,img->ViewLeft,img->ViewTop,img->ViewRight,img->ViewBottom,iWidth,iHeight,align);
			img->Width = iWidth;
			img->Height = iHeight;
			break;
		case GAC_IMAGERESIZEMODE_FILL:
			img->Left = img->ViewLeft;
			img->Top = img->ViewTop;
			img->Width = img->ViewRight - img->ViewLeft;
			img->Height = img->ViewBottom - img->ViewTop;
			break;
		case GAC_IMAGERESIZEMODE_FIT:
			rapX = img->Image->Width/(img->ViewRight-img->ViewLeft);
			rapY = img->Image->Height/(img->ViewBottom-img->ViewTop);
			rapX = GAC_MAX(rapX,rapY);
			img->Width = img->Image->Width/rapX;
			img->Height = img->Image->Height/rapX;
			ALIGN_OBJECT_TO_RECT(img->Left,img->Top,img->ViewLeft,img->ViewTop,img->ViewRight,img->ViewBottom,img->Width,img->Height,align);
			break;
		case GAC_IMAGERESIZEMODE_SHRINK:
			rapX = img->Image->Width/(img->ViewRight-img->ViewLeft);
			rapY = img->Image->Height/(img->ViewBottom-img->ViewTop);
			rapX = GAC_MAX(rapX,rapY);
			if (rapX>1)
			{
				img->Width = img->Image->Width/rapX;
				img->Height = img->Image->Height/rapX;
			} else {
				img->Width = img->Image->Width;
				img->Height = img->Image->Height;
			}
			ALIGN_OBJECT_TO_RECT(img->Left,img->Top,img->ViewLeft,img->ViewTop,img->ViewRight,img->ViewBottom,img->Width,img->Height,align);
			break;
		default:
			LOG_ERROR("ImagePainter::RecomputeLayout() - invalid image resize type");
			return;
	}
	// verific si daca e nevoie de resize
	if ((img->Image->Width == img->Width) && (img->Image->Height == img->Height))
		img->Flags |= PAINT_OBJECT_NO_RESIZE;
	// totul e ok - activez si bit-ul
	img->Flags |= PAINT_OBJECT_VALID;
}
//============================================================================
GApp::Graphics::ImagePainter::ImagePainter()
{
	Clear();
}
void GApp::Graphics::ImagePainter::Clear()
{
	Image = NULL;
	S = NULL;
	Left = Top = Width = Height = 0;
	ViewLeft = ViewTop = ViewRight = ViewBottom = 0;
	X = Y = 0;
	ImageWidth = ImageHeight = -1;
	ScaleWidth = ScaleHeight = 1.0f;
	Flags = 0;
	ClearShaderParams();
	SET_BITFIELD(Flags,0,3,GAC_IMAGERESIZEMODE_DOCK);
	SET_BITFIELD(Flags,3,4,GAC_ALIGNAMENT_CENTER);
	SET_BITFIELD(Flags,7,3,GAC_COORDINATES_AUTO);
	SET_BITFIELD(Flags,10,3,GAC_COORDINATES_AUTO);
	Blend.Mode = BLEND_IMAGE_MODE_NONE;
}
void GApp::Graphics::ImagePainter::SetShader(GApp::Resources::Shader *s)
{
	S = s;
	Blend.Mode = BLEND_IMAGE_MODE_NONE;
	ClearShaderParams();
}
void GApp::Graphics::ImagePainter::SetAlphaBlending(float alpha)
{
	S = NULL;
	Blend.Alpha = alpha;
	Blend.Mode = BLEND_IMAGE_MODE_ALPHA;
}
void GApp::Graphics::ImagePainter::SetColorBlending(unsigned int color)
{
	S = NULL;
	Blend.Color = color;
	Blend.Mode = BLEND_IMAGE_MODE_COLOR;
}
bool GApp::Graphics::ImagePainter::SetImage(GApp::Resources::Bitmap *bmp)
{	
	if (bmp==NULL)
	{
		Flags &= PAINT_OBJECT_MASK;
		Image = NULL;
		return true;
	} else {
		Image = bmp;
		SetImageSize(-1,-1,GAC_COORDINATES_PIXELS);
		return true;
	}
}
bool GApp::Graphics::ImagePainter::Create(GApp::Resources::Bitmap *img,GAC_TYPE_IMAGERESIZEMODE mode,GAC_TYPE_ALIGNAMENT align)
{
	Clear();
	CHECK(img!=NULL,false,"");
	Image = img;
	SetImageSize(-1,-1,GAC_COORDINATES_PIXELS);
	SetLayout(mode,align);
	return true;
}
void GApp::Graphics::ImagePainter::SetViewRect(float left,float top,float right,float bottom)
{
	ViewLeft = left;
	ViewTop = top;
	ViewRight = right;
	ViewBottom = bottom;
	RecomputeLayout(this);
}
void GApp::Graphics::ImagePainter::SetViewRectWH(float left, float top, float width, float height)
{
	SetViewRect(left, top, left + width, top + height);
}
void GApp::Graphics::ImagePainter::SetLayout(GAC_TYPE_IMAGERESIZEMODE mode,GAC_TYPE_ALIGNAMENT align)
{
	SET_BITFIELD(Flags,0,3,mode);
	SET_BITFIELD(Flags,3,4,align);
	RecomputeLayout(this);
}
void GApp::Graphics::ImagePainter::SetPosition(float x,float y,GAC_TYPE_COORDINATES coord,GAC_TYPE_ALIGNAMENT align)
{
	SET_BITFIELD(Flags,0,3,GAC_IMAGERESIZEMODE_NONE);
	SET_BITFIELD(Flags,3,4,align);
	SET_BITFIELD(Flags,7,3,coord);
	X = x;
	Y = y;
	RecomputeLayout(this);
}
void GApp::Graphics::ImagePainter::SetScale(float scaleWidth,float scaleHeight)
{
	ScaleWidth = scaleWidth;
	ScaleHeight = scaleHeight;
	if (ScaleHeight<=0)
		ScaleHeight = ScaleWidth;
	ImageWidth = ImageHeight = -1;
	RecomputeLayout(this);
}
void GApp::Graphics::ImagePainter::SetImageSize(float width,float height,GAC_TYPE_COORDINATES coord)
{
	SET_BITFIELD(Flags,10,3,coord);
	ImageWidth = width;
	ImageHeight = height;
	RecomputeLayout(this);
}
void GApp::Graphics::ImagePainter::ClearShaderParams()
{
	ShaderParamInfo[0] = ShaderParamInfo[1] = ShaderParamInfo[2] = ShaderParamInfo[3] = 0;
}
bool GApp::Graphics::ImagePainter::SetShaderUniformValue(const char * VariableName, unsigned int count, float v1, float v2, float v3, float v4)
{
	CHECK(S != NULL, false, "");
	int index = S->GetUniformIndex(VariableName);
	CHECK(index >= 0, false, "");
	CHECK((count >= 1) && (count <= 4), false, "");
	ShaderParamInfo[index] = count;
	ShaderParams[index * 4] = v1;
	ShaderParams[index * 4 + 1] = v2;
	ShaderParams[index * 4 + 2] = v3;
	ShaderParams[index * 4 + 3] = v4;
	return true;
}
bool GApp::Graphics::ImagePainter::SetShaderUniformColor(const char * VariableName, unsigned int color)
{
	return SetShaderUniformValue(VariableName, 4, RED(color), GREEN(color), BLUE(color), ALPHA(color));
}
void GApp::Graphics::ImagePainter::UpdateShaderParams()
{
	if (S == NULL)
		return;
	unsigned char *spi = &ShaderParamInfo[0];
	float *params = &ShaderParams[0];
	for (int tr = 0; tr < 4; tr++,spi++,params+=4)
	{
		if ((*spi) != 0)
		{
			switch (*spi)
			{
				case 1: S->SetVariableValue(tr, params[0], false); break;
				case 2: S->SetVariableValue(tr, params[0], params[1], false); break;
				case 3: S->SetVariableValue(tr, params[0], params[1], params[2], false); break;
				case 4: S->SetVariableValue(tr, params[0], params[1], params[2], params[3], false); break;
			}
		}
	}
}