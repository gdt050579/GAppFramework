#include "GApp.h"

GApp::Utils::MapPoint::MapPoint()
{
	memset(this, 0, sizeof(GApp::Utils::MapPoint));

}
//----------------------------------------------------------------------------------------------------------
GApp::Utils::MapTranslator::MapTranslator()
{
	memset(this, 0, sizeof(GApp::Utils::MapTranslator));

}
bool GApp::Utils::MapTranslator::Create(int width, int height, int widthUnits, int heightUnits)
{
	CHECK(width > 0, false, "Width should be bigger than 0");
	CHECK(height > 0, false, "Height should be bigger than 0");
	CHECK(widthUnits > 0, false, "WidthUnits should be bigger than 0");
	CHECK(heightUnits > 0, false, "HeightUnits should be bigger than 0");
	CHECK((widthUnits & 1) == 1, false, "WidthUnits should be an odd number !");
	CHECK((heightUnits & 1) == 1, false, "heightUnits should be an odd number !");
	Width = width;
	Height = height;
	WidthUnits = widthUnits;
	HeightUnits = heightUnits;
	LeftMargin = WidthUnits / 2;
	TopMargin = HeightUnits / 2;
	RightMargin = Width*WidthUnits - (LeftMargin + 1);
	BottomMargin = Height*HeightUnits - (TopMargin + 1);
	return true;
}
bool GApp::Utils::MapTranslator::Create(int width, int height, int widthUnits, int heightUnits, int cellWidth, int cellHeight)
{
	CHECK(Create(width, height, widthUnits, heightUnits), false, "Unable to create map translator");
	CHECK(SetCellSize(cellWidth, cellHeight), false, "Unable to set cell size");
	return true;
}
bool GApp::Utils::MapTranslator::SetCellSize(int width, int height)
{
	CHECK(width > 0, false, "Width should be bigger than 0");
	CHECK(height > 0, false, "Height should be bigger than 0");
	CellWidth = width;
	CellHeight = height;
	return true;
}
bool GApp::Utils::MapTranslator::AlignGridToScreen(int gridX, int gridY, int screenX, int screenY)
{
	ScrollLeft = screenX - (((gridX / WidthUnits)*CellWidth) + (((2 * (gridX % WidthUnits) + 1)*CellWidth) / (WidthUnits*2)));
	ScrollTop = screenY - (((gridY / HeightUnits)*CellHeight) + (((2 * (gridY % HeightUnits) + 1)*CellHeight) / (HeightUnits * 2)));
	return true;
}
bool GApp::Utils::MapTranslator::AlignMapPointScreen(MapPoint *point, int screenX, int screenY)
{
	CHECK(point != NULL, false, "Invalid map (NULL) parameter !");
	return this->AlignGridToScreen(point->GridX, point->GridY, screenX, screenY);
}
bool GApp::Utils::MapTranslator::AlignMapToScreen(int screenX, int screenY, GAC_TYPE_ALIGNAMENT align)
{
	float x, y;
	ALIGN_COORD(x, y, screenX, screenY, Width*CellWidth, Height*CellHeight, align);
	ScrollLeft = (int)x;
	ScrollTop = (int)y;
	return true;
}
bool GApp::Utils::MapTranslator::ComputeVisibleCells(int viewLeft, int viewTop, int viewRight, int viewBottom, GApp::Graphics::Rect *visibleCells)
{
	CHECK(visibleCells != NULL, false, "Invalid (NULL) screen view object !");
	visibleCells->Left = (viewLeft - this->ScrollLeft) / CellWidth;
	visibleCells->Right = ((viewRight) - this->ScrollLeft) / CellWidth;
	visibleCells->Top = (viewTop - this->ScrollTop) / CellHeight;
	visibleCells->Bottom = ((viewBottom ) - this->ScrollTop) / CellHeight;
	visibleCells->Left = GAC_MAX(0, visibleCells->Left);
	visibleCells->Top = GAC_MAX(0, visibleCells->Top);
	visibleCells->Right = GAC_MIN(Width - 1, visibleCells->Right);
	visibleCells->Bottom = GAC_MIN(Height - 1, visibleCells->Bottom);
	return true;
}
bool GApp::Utils::MapTranslator::ComputeVisibleCells(GApp::Graphics::Rect *screenView, GApp::Graphics::Rect *visibleCells)
{
	CHECK(screenView != NULL, false, "Invalid (NULL) screen view object !");
	return ComputeVisibleCells(screenView->Left, screenView->Top, screenView->Right, screenView->Bottom, visibleCells);
}
bool GApp::Utils::MapTranslator::GetCellRect(int cellX, int cellY, GApp::Graphics::Rect *rect)
{
	CHECK(rect != NULL, false, "Invalid (NULL) screen view object !");
	CHECK((cellX >= 0) && (cellX < Width), false, "Invalid cell (X=%d) - expecting a value between 0 and %d", cellX, Width);
	CHECK((cellY >= 0) && (cellY < Height), false, "Invalid cell (Y=%d) - expecting a value between 0 and %d", cellY, Height);
	rect->Left = ScrollLeft + CellWidth*cellX;
	rect->Right = rect->Left + CellWidth-1;
	rect->Top = ScrollTop + CellHeight*cellY;
	rect->Bottom = rect->Top + CellHeight-1;
	return true;
}
bool GApp::Utils::MapTranslator::MoveMapPointBy(MapPoint *point, int addX, int addY)
{
	CHECK(point != NULL, false, "Invalid point (NULL) parameter !");
	point->GridX += addX;
	point->GridY += addY;
	if (point->GridX < this->LeftMargin)
		point->GridX = this->LeftMargin;
	if (point->GridX > this->RightMargin)
		point->GridX = this->RightMargin;
	if (point->GridY < this->TopMargin)
		point->GridY = this->TopMargin;
	if (point->GridY > this->BottomMargin)
		point->GridY = this->BottomMargin;
	point->CellX = point->GridX / this->WidthUnits;
	point->CellY = point->GridY / this->HeightUnits;
	return ((point->GridX % this->WidthUnits) == this->LeftMargin) && ((point->GridY % this->HeightUnits) == this->TopMargin);
}
bool GApp::Utils::MapTranslator::SetMapPointToCell(MapPoint *point, int X, int Y)
{
	CHECK(point != NULL, false, "Invalid point (NULL) parameter !");
	CHECK((X >= 0) && (X<this->Width), false, "Invalid value for X position (%d) - should be between 0 and %d", X, this->Width);
	CHECK((Y >= 0) && (Y<this->Height), false, "Invalid value for Y position (%d) - should be between 0 and %d", Y, this->Height);
	point->GridX = X*this->WidthUnits + this->LeftMargin;
	point->GridY = Y*this->HeightUnits + this->TopMargin;
	point->CellX = X;
	point->CellY = Y;
	return true;
}
bool GApp::Utils::MapTranslator::SetMapPointToGrid(MapPoint *point, int X, int Y)
{
	CHECK(point != NULL, false, "Invalid point (NULL) parameter !");
	CHECK((X >= 0) && (X<this->Width*this->WidthUnits), false, "Invalid value for X position (%d) - should be between 0 and %d", X, this->Width*this->WidthUnits);
	CHECK((Y >= 0) && (Y<this->Height*this->HeightUnits), false, "Invalid value for Y position (%d) - should be between 0 and %d", Y, this->Height*this->HeightUnits);
	point->GridX = X;
	point->GridY = Y;
	if (point->GridX < this->LeftMargin)
		point->GridX = this->LeftMargin;
	if (point->GridX > this->RightMargin)
		point->GridX = this->RightMargin;
	if (point->GridY < this->TopMargin)
		point->GridY = this->TopMargin;
	if (point->GridY > this->BottomMargin)
		point->GridY = this->BottomMargin;
	point->CellX = point->GridX / this->WidthUnits;
	point->CellY = point->GridY / this->HeightUnits;
	return true;
}

int  GApp::Utils::MapTranslator::GetMapPointScreenX(MapPoint *point)
{
	CHECK(point != NULL, 0, "Invalid point (NULL) parameter !");
	return this->ScrollLeft + point->CellX*this->CellWidth + (((2 * (point->GridX % this->WidthUnits) + 1)*this->CellWidth) / (this->WidthUnits * 2));
}
int  GApp::Utils::MapTranslator::GetMapPointScreenY(MapPoint *point)
{
	CHECK(point != NULL, 0, "Invalid point (NULL) parameter !");
	return this->ScrollTop + point->CellY*this->CellHeight + (((2 * (point->GridY % this->HeightUnits) + 1)*this->CellHeight) / (this->HeightUnits * 2));
}
bool GApp::Utils::MapTranslator::ScreenToMapPoint(int screenX, int screenY, MapPoint *point)
{
	CHECK(point != NULL, false, "Invalid point (NULL) parameter !");
	CHECK(CellWidth != 0, false, "CellWidth has not been set ");
	CHECK(CellHeight != 0, false, "CellHeight has not been set ");
	int g_X = ((screenX - this->ScrollLeft)*this->WidthUnits) / CellWidth;
	int g_Y = ((screenY- this->ScrollTop)*this->HeightUnits) / CellHeight;
	// false daca sunt in afara hartii
	if ((g_X < 0) || (g_X >= this->WidthUnits*this->Width))
		return false;
	if ((g_Y < 0) || (g_Y >= this->HeightUnits*this->Height))
		return false;
	if (g_X < this->LeftMargin)
		g_X = this->LeftMargin;
	if (g_X > this->RightMargin)
		g_X = this->RightMargin;
	if (g_Y < this->TopMargin)
		g_Y = this->TopMargin;
	if (g_Y > this->BottomMargin)
		g_Y = this->BottomMargin;
	point->GridX = g_X;
	point->GridY = g_Y;
	point->CellX = g_X / this->WidthUnits;
	point->CellY = g_Y / this->HeightUnits;

	return true;
}
int	 GApp::Utils::MapTranslator::GetCellID(MapPoint *point)
{
	CHECK(point != NULL, -1, "Invalid point (NULL) parameter !");
	return GetCellID(point->CellX, point->CellY);
}
int  GApp::Utils::MapTranslator::GetCellID(int X, int Y)
{
	return Y*Width + X;
}
int	 GApp::Utils::MapTranslator::GetScrollX()
{
	return ScrollLeft;
}
int	 GApp::Utils::MapTranslator::GetScrollY()
{
	return ScrollTop;
}
void GApp::Utils::MapTranslator::SetScrollXY(int left, int top)
{
	ScrollLeft = left;
	ScrollTop = top;
}
int	 GApp::Utils::MapTranslator::ConvertCellXToGridPosition(int X)
{
	return X*this->WidthUnits + this->LeftMargin;
}
int	 GApp::Utils::MapTranslator::ConvertCellYToGridPosition(int Y)
{
	return Y*this->HeightUnits + this->TopMargin;
}
int	 GApp::Utils::MapTranslator::GetGridX(MapPoint *point)
{
	CHECK(point != NULL, -1, "Invalid point (NULL) parameter !");
	return point->GridX;
}
int	 GApp::Utils::MapTranslator::GetGridY(MapPoint *point)
{
	CHECK(point != NULL, -1, "Invalid point (NULL) parameter !");
	return point->GridY;
}
bool GApp::Utils::MapTranslator::MoveMapPointInLineToGridPosition(MapPoint *point, int gridX, int gridY,int Steps)
{
	CHECK(point != NULL, false, "Invalid point (NULL) parameter !");
	CHECK(Steps > 0, false, "You have to have at least one step for MoveMapPointInLineToGridPosition !");
	if ((gridX == point->GridX) && (gridY == point->GridY))
		return true;
	
	int x = point->GridX;
	int y = point->GridY;
	int x2 = gridX;
	int y2 = gridY;

	int w = x2 - x;
	int h = y2 - y;
	int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
	if (w<0) dx1 = -1; else if (w>0) dx1 = 1;
	if (h<0) dy1 = -1; else if (h>0) dy1 = 1;
	if (w<0) dx2 = -1; else if (w>0) dx2 = 1;
	int longest = GAC_ABS(w);
	int shortest = GAC_ABS(h);
	if (!(longest>shortest)) {
		longest = GAC_ABS(h);
		shortest = GAC_ABS(w);
		if (h<0) dy2 = -1; else if (h>0) dy2 = 1;
		dx2 = 0;
	}
	int numerator = longest >> 1;
	for (int i = 0; (i <= longest) && (Steps>0); i++,Steps--) {
		numerator += shortest;
		if (!(numerator<longest)) {
			numerator -= longest;
			x += dx1;
			y += dy1;
		}
		else {
			x += dx2;
			y += dy2;
		}
	}


	SetMapPointToGrid(point, x, y);
	return ((gridX == point->GridX) && (gridY == point->GridY));
}