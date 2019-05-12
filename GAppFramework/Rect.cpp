#include "GApp.h"

#define RECT_IMPLEMENTED_FUNCTION(className,type) \
GApp::Graphics::className::className() \
{ \
	Left = Top = Right = Bottom = 0; \
} \
GApp::Graphics::className::className(type x,type y,GAC_TYPE_ALIGNAMENT align,type Width,type Height) \
{ \
	Set(x,y,align,Width,Height); \
} \
GApp::Graphics::className::className(type left, type top, type right, type bottom) \
{ \
	Set(left,top,right,bottom); \
} \
bool	GApp::Graphics::className::Set(type x,type y,GAC_TYPE_ALIGNAMENT align,type width,type height) \
{ \
	if ((width<0) || (height<0)) \
		return false; \
	float l=0,t=0; \
	ALIGN_COORD(l,t,((float)x),((float)y),((float)width),((float)height),align); \
	Left = (type)l; \
	Top = (type)t; \
	Right = Left+width; \
	Bottom = Top+height; \
	return true; \
} \
bool	GApp::Graphics::className::SetWH(type left, type top, type width, type height) \
{ \
	if ((width<0) || (height<0)) \
		return false; \
	Left = left; \
	Top = top; \
	Right = Left+width; \
	Bottom = Top+height; \
	return true; \
} \
bool	GApp::Graphics::className::Set(type left, type top, type right, type bottom) \
{ \
	if ((right<left) || (bottom<top)) \
		return false; \
	Left = left; \
	Top = top; \
	Right = right; \
	Bottom = bottom; \
	return true; \
} \
bool	GApp::Graphics::className::Set(className *r) \
{ \
	CHECK(r!=NULL,false,""); \
	return Set(r->Left,r->Top,r->Right,r->Bottom); \
} \
void	GApp::Graphics::className::Intersect(type left,type top,type right,type bottom) \
{ \
	if (left > Left) \
		Left = left; \
	if (right < Right) \
		Right = right; \
	if (top > Top) \
		Top = top; \
	if (bottom < Bottom) \
		Bottom = bottom; \
} \
void	GApp::Graphics::className::Intersect(className *r) \
{ \
	if (r!=NULL) \
		Intersect(r->Left,r->Top,r->Right,r->Bottom); \
} \
bool	GApp::Graphics::className::OverlapWith(type left, type top, type right, type bottom) \
{ \
	if (Left>right) return false; \
	if (Right<left) return false; \
	if (Top>bottom) return false; \
	if (Bottom<top) return false; \
	return true; \
} \
bool	GApp::Graphics::className::OverlapWith(className *r) \
{ \
	if (r!=NULL) \
		return OverlapWith(r->Left,r->Top,r->Right,r->Bottom); \
	return false; \
} \
type		GApp::Graphics::className::GetCenterX() \
{ \
	return (Left+Right)/2; \
} \
type		GApp::Graphics::className::GetCenterY() \
{ \
	return (Top+Bottom)/2; \
} \
type		GApp::Graphics::className::GetWidth() \
{ \
	return (Right-Left+1); \
} \
type		GApp::Graphics::className::GetHeight() \
{ \
	return (Bottom-Top+1); \
} \
bool	GApp::Graphics::className::Contains(type x,type y) \
{ \
	return ((x >= Left) && (x <= Right) && (y >= Top) && (y <= Bottom)); \
} \
bool	GApp::Graphics::className::SetWidthFromLeft(type newValue) \
{ \
	if (newValue<0) \
		return false; \
	Right = Left + newValue; \
	return true; \
} \
bool	GApp::Graphics::className::SetWidthFromRight(type newValue) \
{ \
	if (newValue<0) \
		return false; \
	Left = Right - newValue; \
	return true; \
} \
bool	GApp::Graphics::className::SetHeightFromTop(type newValue) \
{ \
	if (newValue<0) \
		return false; \
	Bottom = Top + newValue; \
	return true; \
} \
bool	GApp::Graphics::className::SetHeightFromBottom(type newValue) \
{ \
	if (newValue<0) \
		return false; \
	Top = Bottom - newValue; \
	return true; \
} \
void	GApp::Graphics::className::Offset(type x, type y) \
{ \
	Top += y; \
	Left += x; \
	Bottom += y; \
	Right += x; \
} \
void	GApp::Graphics::className::MoveTo(type left, type top) \
{ \
	Right = (left + Right) - Left; \
	Bottom = (top + Bottom) - Top; \
	Left = left; \
	Top = top; \
} \
bool	GApp::Graphics::className::MoveTo(type x, type y, GAC_TYPE_ALIGNAMENT align) \
{ \
	return Set(x, y, align, Right - Left, Bottom - Top); \
}

RECT_IMPLEMENTED_FUNCTION(Rect,int);
RECT_IMPLEMENTED_FUNCTION(RectF,float);