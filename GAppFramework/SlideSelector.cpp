#include "GApp.h"

GApp::Controls::SlideSelector::SlideSelector(void* scene, const char *layout, unsigned int columns, unsigned int rows, unsigned int cellWidth, unsigned int cellHeight): GApp::Controls::ScrollPanel(scene, layout)
{
	Rows = Columns = 0;
	rows = GAC_MAX(rows, 1);
	columns = GAC_MAX(columns, 1);
	cellWidth = GAC_MAX(cellWidth, 1);
	cellHeight = GAC_MAX(cellHeight, 1);
	for (unsigned int y = 0; y<rows; y++)
	{
		for (unsigned int x = 0; x<columns; x++)
		{
			GenericControl *ctrl = new GenericControl(scene, "");
			if (ctrl != NULL)
			{
				ctrl->SetLayout(x*cellWidth, y*cellHeight, GAC_COORDINATES_PIXELS, cellHeight, cellHeight, GAC_COORDINATES_PIXELS, GAC_ALIGNAMENT_TOPLEFT);
				AddControl(ctrl);
			}
		}
	}
	CellWidth = cellWidth;
	CellHeight = cellHeight;
	Rows = rows;
	Columns = columns;
}
bool GApp::Controls::SlideSelector::OnLayout()
{
	SetScrollAlignamentSize(CellWidth, CellHeight);
	SetSlideStep(GAC_MAX(CellWidth, CellHeight) / 30 + 1);
	int s_x = (int)((GetWidth() - CellWidth) / 2);
	int s_y = (int)((GetHeight() - CellHeight) / 2);
	if (s_x < 0)
	{
		LOG_ERROR("SlideSelector:: CellWidth should be smaller than control width (CellWidth = %d, ControlWidth = %d)", CellWidth, (int)GetWidth());
	}
	if (s_y < 0)
	{
		LOG_ERROR("SlideSelector:: CellHeight should be smaller than control width (CellHeight = %d, ControlHeight = %d)", CellHeight, (int)GetHeight());
	}
	SetScrollSize(Columns*CellWidth+s_x*2, Rows*CellHeight+s_y*2);
	unsigned int index = 0;
	for (unsigned int y = 0; y<Rows; y++)
	{
		for (unsigned int x = 0; x<Columns; x++,index++)
		{
			GenericControl *ctrl = this->Controls[index];
			ctrl->SetLayout(x*CellWidth + s_x, y*CellHeight + s_y, GAC_COORDINATES_PIXELS, CellWidth, CellHeight, GAC_COORDINATES_PIXELS, GAC_ALIGNAMENT_TOPLEFT);
		}
	}
	return false;
}
GApp::Controls::GenericControl* GApp::Controls::SlideSelector::GetCell(unsigned int index)
{
	if (index<ControlsCount)
		return Controls[index];
	return NULL;
}
GApp::Controls::GenericControl* GApp::Controls::SlideSelector::GetCell(unsigned int x, unsigned int y)
{
	if ((x<Columns) && (y<Rows))
		return GetCell(y*Columns + x);
	return NULL;
}

unsigned int GApp::Controls::SlideSelector::GetCellsCount()
{
	return ControlsCount;
}
unsigned int GApp::Controls::SlideSelector::GetCellWidth()
{
	return CellWidth;
}
unsigned int GApp::Controls::SlideSelector::GetCellHeight()
{
	return CellHeight;
}
unsigned int GApp::Controls::SlideSelector::GetSelectedCellIndex()
{
	unsigned int ccontrol = 0, index = 0;
	float d, dist = 0;
	//int s_x = (GetWidth() - CellWidth) / 2;
	//int s_y = (GetHeight() - CellHeight) / 2;

	for (unsigned int y = 0; y<Rows; y++)
	{
		for (unsigned int x = 0; x<Columns; x++, ccontrol++)
		{
			d = SQUAREDDIST(Layout.ScrollXPixels, Layout.ScrollYPixels, x*CellWidth, y*CellHeight);
			if (ccontrol == 0)
				dist = d;
			if (d<dist)
			{
				index = ccontrol;
				dist = d;
			}
		}
	}
	LOG_INFO("SlideSelector::Index = %d", index);
	return index;
}
float GApp::Controls::SlideSelector::ComputeCellDistanceToCenter(unsigned int cellIndex)
{
	unsigned int x, y;
	if (Columns > 0)
	{
		x = cellIndex % Columns;
		y = cellIndex / Columns;
		return SQUAREDDIST(Layout.ScrollXPixels/CellWidth, Layout.ScrollYPixels/CellHeight, x, y);
	}
	RETURNERROR(0.0f, "Number of columns from slider control is set to 0 !");
}
bool GApp::Controls::SlideSelector::SelectCell(unsigned int index, bool slide)
{
	CHECK(index<ControlsCount, false, "Invalid item index (%d) - should be smaller than %d", index, ControlsCount);
	CHECK(Columns>0, false, "");

	unsigned int x, y;
	// y * Columns + x = index;
	x = index % Columns;
	y = index / Columns;
	MoveScrollTo(x*CellWidth, y*CellHeight, slide);
	return true;
}
bool GApp::Controls::SlideSelector::GoToNextCell(bool slide)
{
	unsigned int index = GetSelectedCellIndex();
	if ((index + 1)<ControlsCount)
		return SelectCell(index + 1, slide);
	return true;
}
bool GApp::Controls::SlideSelector::GoToPreviousCell(bool slide)
{
	unsigned int index = GetSelectedCellIndex();
	if (index>0)
		return SelectCell(index - 1, slide);
	return true;
}
bool GApp::Controls::SlideSelector::GoToFirstCell(bool slide)
{
	return SelectCell(0, slide);
}
bool GApp::Controls::SlideSelector::GoToLastCell(bool slide)
{
	if (ControlsCount>0)
		return SelectCell(ControlsCount - 1, slide);
	return true;
}
void GApp::Controls::SlideSelector::OnScrollCompleted()
{
	TriggerEvent(GAC_EVENTTYPE_SLIDERCELLCHANGED);
}
void GApp::Controls::SlideSelector::OnScrollChanged()
{
	TriggerEvent(GAC_EVENTTYPE_SLIDERSCROLLUPDATED);
}