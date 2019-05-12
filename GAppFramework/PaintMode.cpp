#include "GApp.h"

#define PAINTMODEFLAG_IDENTITY_MATRIX		(0x00000001)

// 0 1 2 | 3 4 5 | 6 7 8
#define IDENTITY_MATRIX(m) { m[0]=1;m[1]=0;m[2]=0;m[3]=0;m[4]=1;m[5]=0;m[6]=0;m[7]=0;m[8]=1; }
#define COPY_MATRIX(sursa,dest) {dest[0]=sursa[0];dest[1]=sursa[1];dest[2]=sursa[2];dest[3]=sursa[3];dest[4]=sursa[4];dest[5]=sursa[5];dest[6]=sursa[6];dest[7]=sursa[7];dest[8]=sursa[8]; }
#define MULTIPLY_MATRIX(m1,m2,result) { \
	result[0] = m1[0]*m2[0]+m1[1]*m2[3]+m1[2]*m2[6]; \
	result[1] = m1[0]*m2[1]+m1[1]*m2[4]+m1[2]*m2[7]; \
	result[2] = m1[0]*m2[2]+m1[1]*m2[5]+m1[2]*m2[8]; \
	\
	result[3] = m1[3]*m2[0]+m1[4]*m2[3]+m1[5]*m2[6]; \
	result[4] = m1[3]*m2[1]+m1[4]*m2[4]+m1[5]*m2[7]; \
	result[5] = m1[3]*m2[2]+m1[4]*m2[5]+m1[5]*m2[8]; \
	\
	result[6] = m1[6]*m2[0]+m1[7]*m2[3]+m1[8]*m2[6]; \
	result[7] = m1[6]*m2[1]+m1[7]*m2[4]+m1[8]*m2[7]; \
	result[8] = m1[6]*m2[2]+m1[7]*m2[5]+m1[8]*m2[8]; \
	}
//-------------------------------------------------------------------------------------------------------------------------------------
GApp::Graphics::PaintMode::PaintMode()
{
	Reset();
}
void GApp::Graphics::PaintMode::Reset()
{
	ResampleMode = GAC_RESAMPLEMETHOD_NEARESTNEIGHBOR;
	ColorBlendMode = GAC_COLORBLEND_ALPHA;
	TransparentColor = 0;
	Flags=0;
	PositionCallBack = NULL;
	ColorBlendCallBack = NULL;
	CallBackContext = NULL;
	StrokeColor = COLOR_Transparent;
	FillColor = COLOR_Transparent;
	StrokeWidth = 1;
	AntiAliasing = true;
	ResetTransformationMatrix();
}
void GApp::Graphics::PaintMode::ResetTransformationMatrix()
{
	IDENTITY_MATRIX(Matrix);
	SET_BIT(Flags,PAINTMODEFLAG_IDENTITY_MATRIX);
}
void GApp::Graphics::PaintMode::SetResampleMode(GAC_TYPE_RESAMPLEMETHOD newMode)
{
	ResampleMode = newMode;
}
GAC_TYPE_RESAMPLEMETHOD	GApp::Graphics::PaintMode::GetResampleMode()
{
	return ResampleMode;
}
void GApp::Graphics::PaintMode::SetColorBlendMode(GAC_TYPE_COLORBLENDTYPE newMode)
{
	ColorBlendMode = newMode;
}
GAC_TYPE_COLORBLENDTYPE GApp::Graphics::PaintMode::GetColorBlendMode()
{
	return ColorBlendMode;
}
void GApp::Graphics::PaintMode::SetTransparentColor(unsigned int color)
{
	TransparentColor = color;
	SetColorBlendMode(GAC_COLORBLEND_TRANSPARENTCOLOR);
}
unsigned int GApp::Graphics::PaintMode::GetTransparentColor()
{
	return TransparentColor;
}
void GApp::Graphics::PaintMode::SetTranslate(float xOffset,float yOffset)
{
	GAC_TYPE_MATRIXFLOAT m[9];
	GAC_TYPE_MATRIXFLOAT result[9];

	m[0]=1;			m[1]=0;			m[2]=0;
	m[3]=0;			m[4]=1;			m[5]=0;
	m[6]=xOffset;	m[7]=yOffset;	m[8]=1;

	REMOVE_BIT(Flags,PAINTMODEFLAG_IDENTITY_MATRIX);
	MULTIPLY_MATRIX(m,Matrix,result);
	COPY_MATRIX(result,Matrix);
}
void GApp::Graphics::PaintMode::SetRotate(float angle)
{
	GAC_TYPE_MATRIXFLOAT m[9];
	GAC_TYPE_MATRIXFLOAT result[9];

	float radiansAngle = angle * 3.141592653589f / 180.0f;
	float c = cos(radiansAngle);
	float s = sin(radiansAngle);

	m[0]=c;			m[1]=s;			m[2]=0;
	m[3]=-s;		m[4]=c;			m[5]=0;
	m[6]=0;			m[7]=0;			m[8]=1;

	REMOVE_BIT(Flags,PAINTMODEFLAG_IDENTITY_MATRIX);
	MULTIPLY_MATRIX(m,Matrix,result);
	COPY_MATRIX(result,Matrix);
}
void GApp::Graphics::PaintMode::SetRotate(float angle,float x,float y)
{
	SetTranslate(x,y);
	SetRotate(angle);
}
bool GApp::Graphics::PaintMode::SetColorBlendCallBack(PixelColorBlendCallback fnc,void* Context)
{
	CHECK(fnc!=NULL,false,"Callback function should not be NULL");
	ColorBlendCallBack = fnc;
	CallBackContext = Context;

	return true;
}
bool GApp::Graphics::PaintMode::SetPositionCallBack(PixelPositionCallback fnc,void *Context)
{
	CHECK(fnc!=NULL,false,"Pixel position function should not be NULL");
	PositionCallBack = fnc;
	CallBackContext = Context;
	SetColorBlendMode(GAC_COLORBLEND_CALLBACK);
	return true;
}
void GApp::Graphics::PaintMode::SetCallBackContext(void *Context)
{
	CallBackContext = Context;
}
void GApp::Graphics::PaintMode::ComputePixelPosition(int x,int y,int &resultX,int &resultY)
{
	if (PositionCallBack!=NULL)
	{
		PositionCallBack(x,y,resultX,resultY,CallBackContext);
	} else {
		if ((Flags & PAINTMODEFLAG_IDENTITY_MATRIX)!=0)
		{
			resultX = x;
			resultY = y;
		} else {
			GAC_TYPE_MATRIXFLOAT fx = (GAC_TYPE_MATRIXFLOAT)x;
			GAC_TYPE_MATRIXFLOAT fy = (GAC_TYPE_MATRIXFLOAT)y;
			resultX = (int)(fx*Matrix[0]+fy*Matrix[3]+Matrix[6]);	
			resultY = (int)(fx*Matrix[1]+fy*Matrix[4]+Matrix[7]);
		}
	}
}
unsigned int GApp::Graphics::PaintMode::ComputePixelColor(int x,int y,unsigned int currentColor,unsigned int appliedColor)
{
	unsigned int a,inv_a;
	switch (ColorBlendMode)
	{
		case GAC_COLORBLEND_XOR:
			return COLOR_RGB(GET_RED(currentColor)^GET_RED(appliedColor),GET_GREEN(currentColor)^GET_GREEN(appliedColor),GET_BLUE(currentColor)^GET_BLUE(appliedColor));
		case GAC_COLORBLEND_ALPHA:
			a = GET_ALPHA(appliedColor);
			inv_a = 255-a;
			return COLOR_RGB(GET_RED(appliedColor)*a+GET_RED(currentColor)*inv_a,GET_GREEN(appliedColor)*a+GET_GREEN(currentColor)*inv_a,GET_BLUE(appliedColor)*a+GET_BLUE(currentColor)*inv_a);
		case GAC_COLORBLEND_TRANSPARENTCOLOR:
			if (appliedColor==TransparentColor)
				return currentColor;
			return appliedColor;
		case GAC_COLORBLEND_CALLBACK:
			CHECK(ColorBlendCallBack!=NULL,currentColor,"ColorBlend callback is not set !");
			return ColorBlendCallBack(x,y,currentColor,appliedColor,CallBackContext);
		case GAC_COLORBLEND_GRAYSCALE:
			a = 0xFF & ((GET_RED(appliedColor)+GET_GREEN(appliedColor)+GET_BLUE(appliedColor))/3);
			return COLOR_RGB(a,a,a);
	}
	return appliedColor;
}
void GApp::Graphics::PaintMode::SetStrokeColor(unsigned int color)
{
	StrokeColor = color;
}
void GApp::Graphics::PaintMode::SetFillColor(unsigned int color)
{
	FillColor = color;
}
void GApp::Graphics::PaintMode::SetStrokeWidth(float penWidth)
{	
	StrokeWidth = penWidth;
	if (StrokeWidth<0)
		StrokeWidth = 0;
}
bool GApp::Graphics::PaintMode::HasStrokeColor()
{
	return (GET_ALPHA(StrokeColor)!=0) && (StrokeWidth>0);
}
bool GApp::Graphics::PaintMode::HasFillColor()
{
	return GET_ALPHA(FillColor)!=0;
}
float GApp::Graphics::PaintMode::GetStrokeWidth()
{
	return StrokeWidth;
}
void GApp::Graphics::PaintMode::SetAntiAlias(bool value)
{
	AntiAliasing = value;
}
bool GApp::Graphics::PaintMode::IsAntiAliasEnabled()
{
	return AntiAliasing;
}
unsigned int GApp::Graphics::PaintMode::GetStrokePixelColor(int x,int y)
{
	return StrokeColor;
}
unsigned int GApp::Graphics::PaintMode::GetFillPixelColor(int x,int y)
{
	return FillColor;
}