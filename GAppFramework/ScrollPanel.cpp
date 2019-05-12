#include "GApp.h"

#define ALIGN_SCROLL(value,align)	((float)((int)(((int)((((float)(value))/((float)(align)))+0.5f))*((float)(align)))))
#define SLIDE_SCROLL(curent_value,final_value,step) {\
	if ((curent_value)<(final_value)) {\
		if (((curent_value)+(step))<(final_value)) { curent_value+=(step); } else {curent_value = (final_value); } \
	}\
	if ((curent_value)>(final_value)) {\
		if (((curent_value)-(step))>(final_value)) { curent_value-=(step); } else { curent_value = (final_value); }\
	}\
}

GApp::Controls::ScrollPanel::ScrollPanel(void* scene,const char *layout): GApp::Controls::GenericControl(scene,layout)
{
	GoToX = GoToY = -1;
	AlignWidth = AlignHeight = 1;
	SlideStep = 1;
	ScrollWidth = ScrollHeight = 0;
}
void GApp::Controls::ScrollPanel::MoveScrollTo(float X,float Y,bool slide)
{
	float w,h;
	w = GetWidth();
	h = GetHeight();

	if (X+w>ScrollWidth)
		X = ScrollWidth-w;
	if (Y+h>ScrollHeight)
		Y = ScrollHeight-h;

	if (X<0) 
		X = 0;
	if (Y<0)
		Y = 0;
	if ((X == Layout.ScrollXPixels) && (Y == Layout.ScrollYPixels))
		return;
	LOG_INFO("[SCR] Move Scroll tor = %f,%f ", X, Y);
	if (slide)
	{				
		GoToX = X;
		GoToY = Y;
	} else {
		//LOG_INFO("Moving to %f,%f from %f,%f",X,Y,Layout.ScrollXPixels,Layout.ScrollYPixels);
		GoToX = GoToY = -1;
		Layout.ScrollXPixels = X;
		Layout.ScrollYPixels = Y;
		RecomputeLayout();
		OnScrollChanged();
	}
}
bool GApp::Controls::ScrollPanel::MoveScrollToControl(GApp::Controls::GenericControl *control, bool slide)
{
	if (control == NULL)
		return false;
	GApp::Controls::GenericControl **copil = this->Controls;
	for (unsigned int tr = 0; tr < this->ControlsCount; tr++, copil++)
	{
		//LOG_INFO("[SCR] OBJ = %d [Left  = %f, Top = %f, Width = %f , Height = %f]  Relative : [Left=%f,Top=%f]", tr, (*copil)->GetLeft(), (*copil)->GetTop(), (*copil)->GetWidth(), (*copil)->GetHeight(), (*copil)->GetRelativeLeft(), (*copil)->GetRelativeTop());
		if ((*copil) == control)
		{
			float cx = control->GetRelativeLeft() + control->GetWidth() / 2 - this->GetWidth() / 2;
			float cy = control->GetRelativeTop() + control->GetHeight() / 2 - this->GetHeight() / 2;
			//LOG_INFO("[SCR] Top  = %f, Height = %f , This->Height = %f ", control->GetTop(), control->GetHeight(), this->GetHeight());
			//LOG_INFO("[SCR] Left = %f, Width  = %f , This->Width  = %f ", control->GetLeft(), control->GetWidth(), this->GetWidth());
			//LOG_INFO("[SCR] Scroll = %f,%f [Move to: %f,%f] - control->scroll = [%f,%f]", Layout.ScrollXPixels, Layout.ScrollYPixels,cx,cy,control->GetScrollX(),control->GetScrollY());
			MoveScrollTo(cx, cy, slide);
			//LOG_INFO("[SCR] Scroll After = %f,%f ", Layout.ScrollXPixels, Layout.ScrollYPixels);
			return true;
		}
	}
	return false;
}
bool GApp::Controls::ScrollPanel::SetScrollSize(float width,float height)
{
	CHECK(width>=0,false,"");
	CHECK(height>=0,false,"");
	if ((width!=ScrollWidth) || (height!=ScrollHeight))
	{
		ScrollWidth = width;
		ScrollHeight = height;
		MoveScrollTo(Layout.ScrollXPixels,Layout.ScrollYPixels,false);
	}
	return true;
}
bool GApp::Controls::ScrollPanel::SetSlideStep(float value)
{
	CHECK(value>=1,false,"Slide step shoul be bigger than 1 (%f)",value);
	SlideStep = value;
	return true;
}
bool GApp::Controls::ScrollPanel::SetScrollAlignamentSize(float cell_width,float cell_height)
{
	if ((cell_width!=AlignWidth) || (cell_height!=AlignHeight))
	{
		CHECK(cell_width>=1,false,"CellWidth shoud be bigger than 1 (cell_width=%f)",cell_width);
		CHECK(cell_height>=1,false,"CellHeight shoud be bigger than 1 (cell_height=%f)",cell_height);
		AlignWidth = cell_width;
		AlignHeight = cell_height;
		MoveScrollTo(ALIGN_SCROLL(Layout.ScrollXPixels,AlignWidth), ALIGN_SCROLL(Layout.ScrollYPixels,AlignHeight),false);
	}
	return true;
}
bool GApp::Controls::ScrollPanel::OnLayout()
{
	return false;
}
bool GApp::Controls::ScrollPanel::OnTouchEvent(GApp::Controls::TouchEvent *te)
{
	float sx,sy;
	switch (te->Type)
	{
		case GAC_TOUCHEVENTTYPE_DOWN:
			OriginalScrollX = Layout.ScrollXPixels;
			OriginalScrollY = Layout.ScrollYPixels;
			//LOG_INFO("[DOWN] OriginalScrollX = %f, OriginalScrollY = %f [OX=%f,X=%f,OY=%f,Y=%f]",OriginalScrollX,OriginalScrollY,te->OriginalX,te->X,te->OriginalY,te->Y);
			break;
		case GAC_TOUCHEVENTTYPE_MOVE:
			MoveScrollTo(OriginalScrollX-te->X+te->OriginalX,OriginalScrollY-te->Y+te->OriginalY,false);
			//LOG_INFO("[MOVE] ScrollX = %f, ScrollY = %f",Layout.ScrollXPixels,Layout.ScrollYPixels);
			break;
		case GAC_TOUCHEVENTTYPE_CLICK:
		case GAC_TOUCHEVENTTYPE_UP:
			sx = ALIGN_SCROLL(OriginalScrollX - te->X + te->OriginalX, AlignWidth);
			sy = ALIGN_SCROLL(OriginalScrollY - te->Y + te->OriginalY, AlignHeight);
			//LOG_INFO("[ UP ] OriginalScrollX=%f, te->X=%f, te->OriginalX=%f", OriginalScrollX, te->X, te->OriginalX);
			//LOG_INFO("[ UP ] OriginalScrollY=%f, te->Y=%f, te->OriginalY=%f", OriginalScrollY, te->Y, te->OriginalY);
			//LOG_INFO("[ UP ] ScrollX = %f, ScrollY = %f", OriginalScrollX - te->X + te->OriginalX, OriginalScrollY - te->Y + te->OriginalY);
			//LOG_INFO("[ UP ] AlignScrollX = %f, AlignScrollY = %f (AlignWidth = %f, AlignHeight = %f)",sx,sy,AlignWidth,AlignHeight);
			if (te->TimeDelta<500)
			{
				//LOG_INFO("Small Delta: X,Y = [%f,%f], Original X,y = [%f,%f]",te->X,te->Y,te->OriginalX,te->OriginalY);
				if ((te->X<te->OriginalX) && (sx==OriginalScrollX))
					sx+=AlignWidth;
				if ((te->X>te->OriginalX) && (sx==OriginalScrollX))
					sx-=AlignWidth;
				if ((te->Y<te->OriginalY) && (sy==OriginalScrollY))
					sy+=AlignHeight;
				if ((te->Y>te->OriginalY) && (sy==OriginalScrollY))
					sy-=AlignHeight;
			} 
			//LOG_INFO("Slide to %f,%f from %f,%f",sx,sy,Layout.ScrollXPixels-te->X+te->OriginalX,Layout.ScrollYPixels-te->Y+te->OriginalY);
			MoveScrollTo(sx,sy ,true);
			break;
	}

	return true;
}
void GApp::Controls::ScrollPanel::OnPaint()
{
	if ((GoToX>=0) && (GoToY>=0))
	{
		SLIDE_SCROLL(Layout.ScrollXPixels,GoToX,SlideStep);
		SLIDE_SCROLL(Layout.ScrollYPixels,GoToY,SlideStep);
		RecomputeLayout();
		OnScrollChanged();
		if ((Layout.ScrollXPixels==GoToX) && (Layout.ScrollYPixels==GoToY))
		{
			GoToX = GoToY = -1;
			OnScrollCompleted();
		}
	}
	//G.DrawRect(0,0,AlignWidth,AlignHeight,0xFFFFFF00);
}
void GApp::Controls::ScrollPanel::OnScrollCompleted()
{
}
void GApp::Controls::ScrollPanel::OnScrollChanged()
{
}
