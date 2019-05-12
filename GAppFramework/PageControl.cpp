#include "GApp.h"

GApp::Controls::PageControl::PageControl(void* scene,const char *layout,unsigned int columns,unsigned int rows): GApp::Controls::ScrollPanel(scene,layout)
{
	Rows = Columns = 0;
	for (unsigned int y=0;y<rows;y++)
	{
		for (unsigned int x=0;x<columns;x++)
		{
			GenericControl *ctrl = new GenericControl(scene,"");
			if (ctrl!=NULL)
			{
				ctrl->SetLayout((float)x,(float)y,GAC_COORDINATES_PERCENTAGE,1,1,GAC_COORDINATES_PERCENTAGE,GAC_ALIGNAMENT_TOPLEFT);
				AddControl(ctrl);
			}
		}
	}
	Rows = rows;
	Columns = columns;
}
GApp::Controls::GenericControl* GApp::Controls::PageControl::GetPage(unsigned int index)
{
	if (index<ControlsCount)
		return Controls[index];
	return NULL;
}
GApp::Controls::GenericControl* GApp::Controls::PageControl::GetPage(unsigned int x,unsigned int y)
{
	if ((x<Columns) && (y<Rows))
		return GetPage(y*Columns+x);
	return NULL;
}
GApp::Controls::GenericControl* GApp::Controls::PageControl::GetCurrentPage()
{
	return GetPage(GetCurrentPageIndex());
}
bool GApp::Controls::PageControl::OnLayout()
{
	SetScrollAlignamentSize(GetWidth(),GetHeight());
	SetSlideStep(GAC_MAX(GetWidth(),GetHeight())/30+1);
	SetScrollSize(Columns*GetWidth(),Rows*GetHeight());
	return false;
}
unsigned int GApp::Controls::PageControl::GetCurrentPageIndex()
{	
	unsigned int ccontrol = 0,index = 0;
	float d, dist = 0;
	float w = GetWidth();
	float h = GetHeight();

	for (unsigned int y=0;y<Rows;y++)
	{
		for (unsigned int x=0;x<Columns;x++,ccontrol++)
		{
			d = SQUAREDDIST(Layout.ScrollXPixels,Layout.ScrollYPixels,x*w,y*h);
			if (ccontrol==0)
				dist = d;
			if (d<dist)
			{
				index = ccontrol;
				dist = d;
			}
		}
	}
	return index;
}
unsigned int GApp::Controls::PageControl::GetPagesCount()
{
	return ControlsCount;
}
bool GApp::Controls::PageControl::SetCurrentPage(unsigned int index,bool slide)
{
	CHECK(index<ControlsCount,false,"Invalid page index (%d) - should be smaller than %d",index,ControlsCount);
	CHECK(Columns>0,false,"");

	unsigned int x,y;
	// y * Columns + x = index;
	x = index % Columns;
	y = index / Columns;
	MoveScrollTo(x*GetWidth(),y*GetHeight(),slide);

	return true;
}
bool GApp::Controls::PageControl::GoToNextPage(bool slide)
{
	unsigned int index = GetCurrentPageIndex();
	if ((index+1)<ControlsCount)
		return SetCurrentPage(index+1,slide);
	return true;
}
bool GApp::Controls::PageControl::GoToPreviousPage(bool slide)
{
	unsigned int index = GetCurrentPageIndex();
	if (index>0)
		return SetCurrentPage(index-1,slide);
	return true;
}
bool GApp::Controls::PageControl::GoToFirstPage(bool slide)
{
	return SetCurrentPage(0,slide);
}
bool GApp::Controls::PageControl::GoToLastPage(bool slide)
{
	if (ControlsCount>0)
		return SetCurrentPage(ControlsCount-1,slide);
	return true;
}
void GApp::Controls::PageControl::OnScrollCompleted()
{
	TriggerEvent(GAC_EVENTTYPE_PAGECHANGED);
}