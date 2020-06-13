#include "GApp.h"

using namespace GApp::Animations::Transformations;
using namespace GApp::Animations;

#define ANIMATION_STATUS_STARTED		1
#define ANIMATION_STATUS_PAUSED			2
#define ANIMATION_POPUP_LOOP_WAIT       4

#define G_CONTEXT (((GApp::UI::CoreSystem *)(animObj->CoreContext))->Graphics)
#define SET_ELEMENT_FULL_SCREEN_LOCATION { this->LeftInPixels = this->TopInPixels = 0; }
//UpdateElementRect((float)animObj->CoreContext->Width, (float)animObj->CoreContext->Height);

void AnimationPaintImage(GApp::Animations::AnimationObject * animObj, GApp::Resources::Bitmap *Image, float x, float y, GAC_TYPE_ALIGNAMENT align, float scaleWidth, float scaleHeight, unsigned int blendColor)
{
	if (Image == NULL)
		return;
	if (blendColor == 0xFFFFFFFF)
	{
		if (scaleWidth == scaleHeight)
		{
			G_CONTEXT.DrawImageAligned(Image, x, y, align, scaleWidth, NULL, GAC_COORDINATES_PIXELS);
		}
		else
		{
			G_CONTEXT.DrawImageResizedAndAligned(Image, x, y, align, scaleWidth, scaleHeight, NULL, GAC_COORDINATES_PIXELS);
		}
	}
	else
	{
		if (scaleWidth == scaleHeight)
		{
			G_CONTEXT.DrawImageAlignedWithColorBlending(Image, x, y, align, scaleWidth, blendColor, GAC_COORDINATES_PIXELS);
		}
		else
		{
			G_CONTEXT.DrawImageResizedAndAlignedWithColorBlending(Image, x, y, align, scaleWidth, scaleHeight, blendColor, GAC_COORDINATES_PIXELS);

		}
	}
}


bool GApp::Animations::AnimationObject::Start()
{
	REMOVE_BIT(Flags, (ANIMATION_STATUS_STARTED | ANIMATION_STATUS_PAUSED));
	TouchCaptureElement = NULL;
	CHECK(OnStart(), false, "Failed to initialize the animation !");
	Flags |= (ANIMATION_STATUS_STARTED| ANIMATION_POPUP_LOOP_WAIT);
    AnimationEndEventID = -1;
	return true;
}
bool GApp::Animations::AnimationObject::Stop()
{
	CHECK((Flags & ANIMATION_STATUS_STARTED) != 0, false, "Animation is not started (can not be stopped)!");
	REMOVE_BIT(Flags, (ANIMATION_STATUS_STARTED | ANIMATION_STATUS_PAUSED));
	TouchCaptureElement = NULL;
    if (AnimationEndEventID >= 0)
        this->RaiseEvent(AnimationEndEventID);
    AnimationEndEventID = -1;
	return true;
}
bool GApp::Animations::AnimationObject::Pause()
{
	CHECK((Flags & (ANIMATION_STATUS_STARTED | ANIMATION_STATUS_PAUSED)) == ANIMATION_STATUS_STARTED, false, "Animation is not started or is already paused !");
	Flags |= ANIMATION_STATUS_PAUSED;
	TouchCaptureElement = NULL;
	return true;
}
bool GApp::Animations::AnimationObject::Resume()
{
	CHECK((Flags & (ANIMATION_STATUS_STARTED | ANIMATION_STATUS_PAUSED)) == (ANIMATION_STATUS_STARTED | ANIMATION_STATUS_PAUSED), false, "Animation is not started or is it is not paused !");
	REMOVE_BIT(Flags, ANIMATION_STATUS_PAUSED);
	TouchCaptureElement = NULL;
	return true;
}
bool GApp::Animations::AnimationObject::IsRunning()
{
	return ((Flags & ANIMATION_STATUS_STARTED) != 0);
}
bool GApp::Animations::AnimationObject::IsPaused()
{
	return ((Flags & ANIMATION_STATUS_PAUSED) != 0);
}
void GApp::Animations::AnimationObject::MoveWithOffset(float offsetX, float offsetY, GAC_TYPE_COORDINATES coord)
{
	unsigned int current_coord = Flags >> 16;

	if (coord == GAC_COORDINATES_PERCENTAGE)
	{
		// calcul cu procente
		if (current_coord == GAC_COORDINATES_PERCENTAGE)
		{
			this->OffsetX += offsetX;
			this->OffsetY += offsetY;
		}
		else 
		{
			// convertesc la pixeli
			this->OffsetX += offsetX * Width;
			this->OffsetY += offsetY * Height;
		}
	}
	else 
	{
		// calcul cu pixeli
		if (current_coord == GAC_COORDINATES_PERCENTAGE)
		{
			// convertesc la procent
			this->OffsetX += offsetX / Width;
			this->OffsetY += offsetY / Height;
		}
		else
		{
			this->OffsetX += offsetX;
			this->OffsetY += offsetY;
		}
	}

}
void GApp::Animations::AnimationObject::ResetMovementOffsets()
{
	this->OffsetX = 0;
	this->OffsetY = 0;
}
bool GApp::Animations::AnimationObject::ProcessTouchEvents(GApp::Controls::TouchEvent *te, Elements::Element* zOrder[], int zOrderElementsCount)
{
    if ((zOrder == NULL) || (zOrderElementsCount <= 0))
        return false;
	if (te->Type == GAC_TOUCHEVENTTYPE_DOWN)
	{
		if (TouchCaptureElement != NULL)
			TouchCaptureElement->CancelTouch(this);
		TouchCaptureElement = NULL;
		Elements::Element** it = (&zOrder[0]) + zOrderElementsCount;
		Elements::Element* elem;
		it--;
		while (it >= zOrder)
		{
			elem = *it;
			if ((elem) && (elem->CanProcessTouchEvents) && (elem->Visible))
			{
				if (elem->OnTouchEvent(this, te))
				{
					TouchCaptureElement = elem;
					break;
				}
			}
			it--;
		}
		return (TouchCaptureElement != NULL);
	}
	// daca nu e touch down
	if (TouchCaptureElement != NULL) 
	{
		if (TouchCaptureElement->OnTouchEvent(this, te) == false)
		{
			TouchCaptureElement = NULL;
		}
		// stiu sigur ca l-am procesat eu
		return true;
	}
	// nu am captura
	return false;
}
void GApp::Animations::AnimationObject::ExitPopupLoop()
{
    REMOVE_BIT(Flags, ANIMATION_POPUP_LOOP_WAIT);
}
void GApp::Animations::AnimationObject::PerformRadioBoxCheck(int groupID, GApp::Animations::Elements::Element * obj, Elements::Element* elements[], int elementsCount)
{
    if (elementsCount == 0)
        return;
    Elements::Element** start = &elements[0];
    Elements::Element** end = start + elementsCount;
    while (start < end)
    {
        (*start)->RadioBoxCheck(groupID, obj);
        start++;
    }
}
//================================================================================================================================================================================================================================================
GApp::Animations::Elements::Element::Element()
{
	Visible = false;
	CallVirtualFunction = false;
	CanProcessTouchEvents = false;
	ColorTransform = 0;
	Alignament = 0;
	X = Y = WidthInPixels = HeightInPixels = ScaleWidth = ScaleHeight = 0;
}
void GApp::Animations::Elements::Element::Paint(GApp::Animations::AnimationObject * animObj)
{
}
void GApp::Animations::Elements::Element::OnUpdatePosition(GApp::Animations::AnimationObject * animObj)
{
}
void GApp::Animations::Elements::Element::OnUpdateSize(GApp::Animations::AnimationObject * animObj)
{
}
void GApp::Animations::Elements::Element::OnUpdateBlendColor(GApp::Animations::AnimationObject * animObj)
{
}
void GApp::Animations::Elements::Element::OnUpdateScale(GApp::Animations::AnimationObject * animObj)
{
}
bool GApp::Animations::Elements::Element::OnTouchEvent(GApp::Animations::AnimationObject * animObj, GApp::Controls::TouchEvent *te)
{
	return false;
}
void GApp::Animations::Elements::Element::CancelTouch(GApp::Animations::AnimationObject * animObj)
{

}
float GApp::Animations::Elements::Element::GetX(GApp::Animations::AnimationObject * animObj, bool pixels)
{
	if (pixels)
	{
		if ((animObj->Flags>>16) == GAC_COORDINATES_PERCENTAGE)
		{
			return (this->X + animObj->OffsetX) * animObj->Width;
		}
		else {
			return this->X + animObj->OffsetX;
		}
	}
	else {
		if ((animObj->Flags >> 16) == GAC_COORDINATES_PERCENTAGE)
		{
			return this->X + animObj->OffsetX;			
		}
		else {
			return (this->X + animObj->OffsetX) / animObj->Width;
		}
	}
}
float GApp::Animations::Elements::Element::GetY(GApp::Animations::AnimationObject * animObj, bool pixels)
{
	if (pixels)
	{
		if ((animObj->Flags >> 16) == GAC_COORDINATES_PERCENTAGE)
		{
			return (this->Y + animObj->OffsetY) * animObj->Height;
		}
		else {
			return this->Y + animObj->OffsetY;
		}
	}
	else {
		if ((animObj->Flags >> 16) == GAC_COORDINATES_PERCENTAGE)
		{
			return this->Y + animObj->OffsetY;
		}
		else {
			return (this->Y + animObj->OffsetY) / animObj->Height;
		}
	}
}
void GApp::Animations::Elements::Element::ComputeElementRect(GApp::Animations::AnimationObject * animObj)
{
	float xx, yy;
	if (this->Parent == NULL)
	{
		xx = this->X * animObj->Width + animObj->OffsetX;
		yy = this->Y * animObj->Height + animObj->OffsetY;
	}
	else {
		xx = this->X * this->Parent->WidthInPixels * this->Parent->ScaleWidth + this->Parent->LeftInPixels;
		yy = this->Y * this->Parent->HeightInPixels * this->Parent->ScaleHeight + this->Parent->TopInPixels;
	}
	// se considera ca Width si Height sunt setate la valorile care trebuie
	ALIGN_COORD(LeftInPixels, TopInPixels, xx, yy, this->WidthInPixels * this->ScaleWidth, this->HeightInPixels * this->ScaleHeight, this->Alignament);

}
void GApp::Animations::Elements::Element::UpdateScreenRect(GApp::Animations::AnimationObject * animObj)
{
	ComputeElementRect(animObj);
}
void GApp::Animations::Elements::Element::RadioBoxCheck(int id, GApp::Animations::Elements::Element * obj) { }

//================================================================================================================================================================================================================================================
void GApp::Animations::Elements::EntireSurfaceElement::Paint(AnimationObject * animObj)
{
	if (!Visible)
		return;
	G_CONTEXT.FillScreen(ColorTransform);
}
void GApp::Animations::Elements::EntireSurfaceElement::UpdateScreenRect(GApp::Animations::AnimationObject * animObj)
{
	SET_ELEMENT_FULL_SCREEN_LOCATION;
}
void GApp::Animations::Elements::SingleImageElement::Paint(AnimationObject * animObj)
{
	if (!Visible)
		return;
	// stiu ca dreptunghiul a fost calculat deja, deci poti sa aliniez fata de coltul de stanga sus
	AnimationPaintImage(animObj, this->Image,LeftInPixels,TopInPixels,GAC_ALIGNAMENT_TOPLEFT,this->ScaleWidth,this->ScaleHeight,this->ColorTransform);
}
void GApp::Animations::Elements::SingleImageElement::UpdateScreenRect(GApp::Animations::AnimationObject * animObj)
{
	if (this->Image != NULL)
	{
		this->WidthInPixels = this->Image->Width;
		this->HeightInPixels = this->Image->Height;
	}
	else {
		this->WidthInPixels = 0;
		this->HeightInPixels = 0;
	}
	ComputeElementRect(animObj);
}
//================================================================================================================================================================================================================================================
void GApp::Animations::Elements::TextElement::Paint(AnimationObject * animObj)
{
	if (!Visible)
		return;
	this->TP.SetViewRectWH(this->LeftInPixels, this->TopInPixels, GAC_ALIGNAMENT_TOPLEFT, this->WidthInPixels * this->ScaleWidth, this->HeightInPixels * this->ScaleHeight);
    if (this->ScaleWidth<this->ScaleHeight)
        this->TP.SetFontSize(GAC_FONTSIZETYPE_SCALE, fontSize * this->ScaleWidth);
    else
        this->TP.SetFontSize(GAC_FONTSIZETYPE_SCALE, fontSize * this->ScaleHeight);
	G_CONTEXT.DrawString(&TP, animObj->OffsetX, animObj->OffsetY);
}
void GApp::Animations::Elements::TextElement::OnUpdatePosition(GApp::Animations::AnimationObject * animObj)
{
}
void GApp::Animations::Elements::TextElement::OnUpdateSize(GApp::Animations::AnimationObject * animObj)
{
}
void GApp::Animations::Elements::TextElement::OnUpdateBlendColor(GApp::Animations::AnimationObject * animObj)
{
	TP.SetColorBlending(this->ColorTransform);
}
void GApp::Animations::Elements::TextElement::OnUpdateScale(GApp::Animations::AnimationObject * animObj)
{

}
void GApp::Animations::Elements::RectangleElement::Paint(AnimationObject * animObj)
{
	if (!Visible)
		return;
	//__COMPUTE_ELEMENT_RECT__(this->WidthInPixels, this->Height,this->ScaleWidth,this->ScaleHeight);
	//G_CONTEXT.DrawRectF(&r, 0, this->ColorTransform, 0, NULL, GAC_COORDINATES_PIXELS);
	G_CONTEXT.DrawRectWH(LeftInPixels,TopInPixels,WidthInPixels*ScaleWidth,HeightInPixels*ScaleHeight,0,ColorTransform,0, NULL, GAC_COORDINATES_PIXELS);
}
void GApp::Animations::Elements::ExclusionRectElement::Paint(AnimationObject * animObj)
{
	if (!Visible)
		return;
/*	__COMPUTE_ELEMENT_RECT__(this->WidthInPixels, this->Height, this->ScaleWidth, this->ScaleHeight);
	G_CONTEXT.DrawExclusionRect(r.Left, r.Top, r.Right, r.Bottom, this->ColorTransform, GAC_COORDINATES_PIXELS);*/		
	G_CONTEXT.DrawExclusionRect(LeftInPixels, TopInPixels, LeftInPixels+ WidthInPixels * ScaleWidth, TopInPixels+ HeightInPixels * ScaleHeight, this->ColorTransform, GAC_COORDINATES_PIXELS);
}
void GApp::Animations::Elements::ClippingElement::Paint(AnimationObject * animObj)
{
	if (!Visible)
		return;
	//__COMPUTE_ELEMENT_RECT__(this->WidthInPixels, this->Height, this->ScaleWidth, this->ScaleHeight);
	G_CONTEXT.SetClipRect(true, LeftInPixels, TopInPixels, LeftInPixels + WidthInPixels * ScaleWidth, TopInPixels + HeightInPixels * ScaleHeight, GAC_COORDINATES_PIXELS);
}
void GApp::Animations::Elements::DisableClippingElement::Paint(AnimationObject * animObj)
{
	if (!Visible)
		return;
	G_CONTEXT.ClearClip();
}
void GApp::Animations::Elements::DisableClippingElement::UpdateScreenRect(GApp::Animations::AnimationObject * animObj)
{
	SET_ELEMENT_FULL_SCREEN_LOCATION;
}
void GApp::Animations::Elements::SimpleButtonElement::UpdateScreenRect(GApp::Animations::AnimationObject * animObj)
{
	if (this->UseBackgoundImage)
	{
		SimpleButtonFaceContainer * face;
		if (Enabled == false)
			face = &this->Inactive;
		else {
			if (this->IsPressed)
				face = &this->Pressed;
			else
				face = &this->Normal;
		}
		if (face->Background != NULL)
		{
			this->WidthInPixels = face->Background->Width;
			this->HeightInPixels = face->Background->Height;
		}
		else {
			this->WidthInPixels = 0;
			this->HeightInPixels = 0;
		}
	}
	ComputeElementRect(animObj);
}
void GApp::Animations::Elements::SimpleButtonElement::Paint(AnimationObject * animObj)
{
	if (Visible == false)
		return;
	SimpleButtonFaceContainer * face;
	if (Enabled == false)
		face = &this->Inactive;
	else {
		if (this->IsPressed)
			face = &this->Pressed;
		else
			face = &this->Normal;
	}
	// dreptunghiul e deja calculat
	if (this->UseBackgoundImage)
		AnimationPaintImage(animObj, face->Background, this->LeftInPixels, this->TopInPixels, GAC_ALIGNAMENT_TOPLEFT, this->ScaleWidth, this->ScaleHeight, face->BackgroundColorBlending);
	else {
		G_CONTEXT.DrawRectWH(LeftInPixels, TopInPixels, WidthInPixels*ScaleWidth, HeightInPixels*ScaleHeight, 0,face->BackgroundColorBlending, 0, NULL, GAC_COORDINATES_PIXELS);
	}
	
	// calculez noile pozitii
	if (face->Symbol != NULL)
	{
		float xPixels = this->LeftInPixels + this->WidthInPixels*this->ScaleWidth*face->SymbolX;
		float yPixels = this->TopInPixels + this->HeightInPixels*this->ScaleHeight*face->SymbolY;
		AnimationPaintImage(animObj, face->Symbol, xPixels, yPixels, face->SymbolAlign, face->SymbolScaleWidth, face->SymbolScaleHeight, face->SymbolColorBlending);
	}

	// calculez si pentru Text
	if (face->TP.TextFont != NULL)
	{
		float xPixels = this->LeftInPixels + this->WidthInPixels*this->ScaleWidth*face->TextRectX;
		float yPixels = this->TopInPixels + this->HeightInPixels*this->ScaleHeight*face->TextRectY;
		face->TP.SetViewRectWH(xPixels, yPixels, face->TextRectAlign, face->TextRectWidth*this->ScaleWidth, face->TextRectHeight*this->ScaleHeight);
		face->TP.SetColorBlending(face->TextColorBlending);
		G_CONTEXT.DrawString(&face->TP);
	}
}
void GApp::Animations::Elements::SimpleButtonElement::CancelTouch(GApp::Animations::AnimationObject * animObj)
{
    this->IsPressed = false;
}
bool GApp::Animations::Elements::SimpleButtonElement::OnTouchEvent(GApp::Animations::AnimationObject * animObj, GApp::Controls::TouchEvent *te)
{
	if (this->Enabled == false)
		return false;
	GApp::Graphics::RectF r;
	//LOG_INFO("SimpleButtonElement::OnTouchEvent(Type=%d,EventID=%d)",te->Type,this->ClickEvent);
	switch (te->Type)
	{
		case GAC_TOUCHEVENTTYPE_DOWN:
			if ((te->X>=this->LeftInPixels) && (te->X<=(this->LeftInPixels+this->WidthInPixels*this->ScaleWidth)) &&
				(te->Y >= this->TopInPixels) && (te->Y <= (this->TopInPixels + this->HeightInPixels*this->ScaleHeight)))
				{
					this->IsPressed = true;
					return true;
				}
			return false;
		case GAC_TOUCHEVENTTYPE_UP:
		case GAC_TOUCHEVENTTYPE_CLICK:
			if (this->IsPressed) 
			{
				this->IsPressed = false;
                if (this->SendEventWhenAnimationEnds)
                {
                    animObj->ExitPopupLoop(); // termin popup loop daca exista
                    animObj->AnimationEndEventID = this->ClickEvent;
                }
                else {
                    animObj->RaiseEvent(this->ClickEvent);
                }
				if (this->ClickSound != NULL)
					(((GApp::UI::CoreSystem *)(animObj->CoreContext))->AudioInterface).Play(this->ClickSound);
			}
			return false; // nu mai vreau sa am eu touch-ul
		default:
			return true;
	}
}
void GApp::Animations::Elements::SimpleCheckBoxElement::UpdateScreenRect(GApp::Animations::AnimationObject * animObj)
{
    if (this->UseBackgoundImage)
    {
        SimpleButtonFaceContainer * face;
        if (Enabled == false)
        {
            if (IsChecked)
                face = &this->CheckedInactive;
            else
                face = &this->UncheckedInactive;
        }
        else {
            if (IsChecked)
                face = &this->Checked;
            else
                face = &this->Unchecked;
        }

        if (face->Background != NULL)
        {
            this->WidthInPixels = face->Background->Width;
            this->HeightInPixels = face->Background->Height;
        }
        else {
            this->WidthInPixels = 0;
            this->HeightInPixels = 0;
        }
    }
    ComputeElementRect(animObj);
}
void GApp::Animations::Elements::SimpleCheckBoxElement::Paint(AnimationObject * animObj)
{
    if (Visible == false)
        return;
    SimpleButtonFaceContainer * face;
    if (Enabled == false)
    {
        if (IsChecked)
            face = &this->CheckedInactive;
        else
            face = &this->UncheckedInactive;
    }
    else {
        if (IsChecked)
            face = &this->Checked;
        else
            face = &this->Unchecked;
    }
    // dreptunghiul e deja calculat
    if (this->UseBackgoundImage)
        AnimationPaintImage(animObj, face->Background, this->LeftInPixels, this->TopInPixels, GAC_ALIGNAMENT_TOPLEFT, this->ScaleWidth, this->ScaleHeight, face->BackgroundColorBlending);
    else {
        G_CONTEXT.DrawRectWH(LeftInPixels, TopInPixels, WidthInPixels*ScaleWidth, HeightInPixels*ScaleHeight, 0, face->BackgroundColorBlending, 0, NULL, GAC_COORDINATES_PIXELS);
    }

    // calculez noile pozitii
    if (face->Symbol != NULL)
    {
        float xPixels = this->LeftInPixels + this->WidthInPixels*this->ScaleWidth*face->SymbolX;
        float yPixels = this->TopInPixels + this->HeightInPixels*this->ScaleHeight*face->SymbolY;
        AnimationPaintImage(animObj, face->Symbol, xPixels, yPixels, face->SymbolAlign, face->SymbolScaleWidth, face->SymbolScaleHeight, face->SymbolColorBlending);
    }

    // calculez si pentru Text
    if (face->TP.TextFont != NULL)
    {
        float xPixels = this->LeftInPixels + this->WidthInPixels*this->ScaleWidth*face->TextRectX;
        float yPixels = this->TopInPixels + this->HeightInPixels*this->ScaleHeight*face->TextRectY;
        face->TP.SetViewRectWH(xPixels, yPixels, face->TextRectAlign, face->TextRectWidth*this->ScaleWidth, face->TextRectHeight*this->ScaleHeight);
        face->TP.SetColorBlending(face->TextColorBlending);
        G_CONTEXT.DrawString(&face->TP);
    }
}
bool GApp::Animations::Elements::SimpleCheckBoxElement::OnTouchEvent(GApp::Animations::AnimationObject * animObj, GApp::Controls::TouchEvent *te)
{
    if (this->Enabled == false)
        return false;
    GApp::Graphics::RectF r;
    //LOG_INFO("SimpleButtonElement::OnTouchEvent(Type=%d,EventID=%d)",te->Type,this->ClickEvent);
    switch (te->Type)
    {
    case GAC_TOUCHEVENTTYPE_DOWN:
        if ((te->X >= this->LeftInPixels) && (te->X <= (this->LeftInPixels + this->WidthInPixels*this->ScaleWidth)) &&
            (te->Y >= this->TopInPixels) && (te->Y <= (this->TopInPixels + this->HeightInPixels*this->ScaleHeight)))
            return true;
        return false;
    case GAC_TOUCHEVENTTYPE_UP:
    case GAC_TOUCHEVENTTYPE_CLICK:
        if ((te->X >= this->LeftInPixels) && (te->X <= (this->LeftInPixels + this->WidthInPixels*this->ScaleWidth)) &&
            (te->Y >= this->TopInPixels) && (te->Y <= (this->TopInPixels + this->HeightInPixels*this->ScaleHeight)))
            {
                this->IsChecked = !this->IsChecked;
                if (this->ClickEvent>=0)
                    animObj->RaiseEvent(this->ClickEvent);                
                if (this->ClickSound != NULL)
                    (((GApp::UI::CoreSystem *)(animObj->CoreContext))->AudioInterface).Play(this->ClickSound);
            }
        return false; // nu mai vreau sa am eu touch-ul
    default:
        return true;
    }
}
bool GApp::Animations::Elements::SimpleRadioBoxElement::OnTouchEvent(GApp::Animations::AnimationObject * animObj, GApp::Controls::TouchEvent *te)
{
    if (this->Enabled == false)
        return false;
    GApp::Graphics::RectF r;
    //LOG_INFO("SimpleButtonElement::OnTouchEvent(Type=%d,EventID=%d)",te->Type,this->ClickEvent);
    switch (te->Type)
    {
    case GAC_TOUCHEVENTTYPE_DOWN:
        if ((te->X >= this->LeftInPixels) && (te->X <= (this->LeftInPixels + this->WidthInPixels*this->ScaleWidth)) &&
            (te->Y >= this->TopInPixels) && (te->Y <= (this->TopInPixels + this->HeightInPixels*this->ScaleHeight)))
            return true;
        return false;
    case GAC_TOUCHEVENTTYPE_UP:
    case GAC_TOUCHEVENTTYPE_CLICK:
        if ((te->X >= this->LeftInPixels) && (te->X <= (this->LeftInPixels + this->WidthInPixels*this->ScaleWidth)) &&
            (te->Y >= this->TopInPixels) && (te->Y <= (this->TopInPixels + this->HeightInPixels*this->ScaleHeight)))
        {
            animObj->RadioBoxCheck(this->Group, this);
            if (this->ClickEvent>=0)
                animObj->RaiseEvent(this->ClickEvent);
            if (this->ClickSound != NULL)
                (((GApp::UI::CoreSystem *)(animObj->CoreContext))->AudioInterface).Play(this->ClickSound);
        }
        return false; // nu mai vreau sa am eu touch-ul
    default:
        return true;
    }
}
void GApp::Animations::Elements::SimpleRadioBoxElement::RadioBoxCheck(int id, GApp::Animations::Elements::Element * obj)
{
    if (id != Group)
        return;
    this->IsChecked = (obj == this);
}
//================================================================================================================================================================================================================================================
Transformation::Transformation()
{
	__Started = false;
}
void Transformation::Init(AnimationObject * animObj)
{
	OnInit(animObj);
	__Started = true;
}
bool Transformation::Update(AnimationObject * animObj)
{
	if (!__Started)
		return false;
	if (OnUpdate(animObj)) // continuu
		return true;
	__Started = false;
	return false;
}
//================================================================================================================================================================================================================================================
void Repeat::OnInit(AnimationObject * animObj)
{
	Counter = NumberOfTimes;
	if (Counter > 0)
		trans->Init(animObj);
}
bool Repeat::OnUpdate(AnimationObject * animObj)
{
	if (Counter <= 0)
		return false;
	if (trans->Update(animObj) == false)
	{
		Counter--;
		if (Counter <= 0)
			return false;
		trans->Init(animObj);
	}
	return true;
}
//================================================================================================================================================================================================================================================
void RepeatUntil::OnInit(AnimationObject * animObj)
{
	trans->Init(animObj);
}
bool RepeatUntil::OnUpdate(AnimationObject * animObj)
{
	if (trans->Update(animObj)) 
	{
		if ((ForceExit) && (ExitCondition))
		{
			ExitCondition = false; // resetez mutexul pt ca tura urmatoare sa astept inca o data
			return false;
		}
		return true;
	}
	if (ExitCondition) {
		ExitCondition = false; // resetez mutexul pt ca tura urmatoare sa astept inca o data
		return false;
	}
	trans->Init(animObj);
	return true;
}
//================================================================================================================================================================================================================================================
void DoOnceUntil::OnInit(AnimationObject * animObj)
{
	trans->Init(animObj);
}
bool DoOnceUntil::OnUpdate(AnimationObject * animObj)
{
	if (trans->Update(animObj))
	{
		if ((ForceExit) && (ExitCondition))
		{
			ExitCondition = false; // resetez mutexul pt ca tura urmatoare sa astept inca o data
			return false;
		}
		return true;
	}
	ExitCondition = false; // resetez mutexul pt ca tura urmatoare sa astept inca o data
	return false;
}
//================================================================================================================================================================================================================================================
void PopupLoop::OnInit(AnimationObject * animObj)
{
    trans->Init(animObj);
}
bool PopupLoop::OnUpdate(AnimationObject * animObj)
{
    if ((animObj->Flags & ANIMATION_POPUP_LOOP_WAIT) == 0)
    {
        if (ForceExit)
        {
            return false; // ies direct indiferent de unde era animatia
        }
        else {
            return trans->Update(animObj); // ies cand se termina animatia
        }
    }
    else {
        if (trans->Update(animObj)==false)
            trans->Init(animObj);
        return true;
    }
}
//================================================================================================================================================================================================================================================
void DoOncePopupLoop::OnInit(AnimationObject * animObj)
{
    trans->Init(animObj);
}
bool DoOncePopupLoop::OnUpdate(AnimationObject * animObj)
{
    if ((animObj->Flags & ANIMATION_POPUP_LOOP_WAIT) == 0)
    {
        if (ForceExit)
        {
            return false; // ies direct indiferent de unde era animatia
        }
        else {
            return trans->Update(animObj); // ies cand se termina animatia
        }
    }
    else {
        if (trans->Update(animObj) == false)
            return false; // s-a terminat animatia
        return true;
    }
}
//================================================================================================================================================================================================================================================
void WaitUntil::OnInit(AnimationObject * animObj)
{
}
bool WaitUntil::OnUpdate(AnimationObject * animObj)
{
	if (ExitCondition) {
		ExitCondition = false; // resetez mutexul pt ca tura urmatoare sa astept inca o data
		return false;
	}
	return true;
}
//================================================================================================================================================================================================================================================
void BranchBlock::OnInit(AnimationObject * animObj)
{
	if ((BranchIndex >= 0) && (BranchIndex < NumberOfTransformations))
	{
		trans = list[BranchIndex];
		trans->Init(animObj);
	}
	else {
		trans = NULL;
	}
}
bool BranchBlock::OnUpdate(AnimationObject * animObj)
{
	if (trans == NULL)
		return false;
	return trans->Update(animObj);
}
//================================================================================================================================================================================================================================================
void IfElseBlock::OnInit(AnimationObject * animObj)
{
	if (IfElseCondition)
	{
		if (then_branch)
			then_branch->Init(animObj);
	} else {
		if (else_branch)
			else_branch->Init(animObj);
	}
}
bool IfElseBlock::OnUpdate(AnimationObject * animObj)
{
	if (IfElseCondition)
	{
		if (then_branch)
			return then_branch->Update(animObj);
	}
	else {
		if (else_branch)
			return else_branch->Update(animObj);
	}
	return false;
}
//================================================================================================================================================================================================================================================
void Timer::OnInit(AnimationObject * animObj)
{
	Counter = NumberOfTimes;
}
bool Timer::OnUpdate(AnimationObject * animObj)
{
	if (Counter <= 0)
		return false;
	Counter--;
	return Counter > 0;
}
//================================================================================================================================================================================================================================================
void Event::OnInit(AnimationObject * animObj)
{
	animObj->RaiseEvent(EventID);
}
bool Event::OnUpdate(AnimationObject * animObj)
{
	return false;
}
//================================================================================================================================================================================================================================================
void AnimationEndEvent::OnInit(AnimationObject * animObj)
{    
}
bool AnimationEndEvent::OnUpdate(AnimationObject * animObj)
{
    LOG_INFO("Set default event for animation end to %d", EventID);
    animObj->AnimationEndEventID = EventID;
    return false;
}
//================================================================================================================================================================================================================================================
void TouchStatus::OnInit(AnimationObject * animObj)
{
	if (TouchEnabled)
		((GApp::UI::CoreSystem *)(animObj->CoreContext))->Api.ClearTouchBoundary();
	else
		((GApp::UI::CoreSystem *)(animObj->CoreContext))->Api.DisableTouch();
}
bool TouchStatus::OnUpdate(AnimationObject * animObj)
{
	return false;
}
//================================================================================================================================================================================================================================================
void TouchBoundary::OnInit(AnimationObject * animObj)
{
	// se calculeaza relativ la animation
	GApp::Graphics::RectF r;
	float xx, yy;
	xx = (this->X * animObj->Width) + animObj->OffsetX;
	yy = (this->Y * animObj->Height) + animObj->OffsetY;
	r.Set(xx, yy, this->Alignament, this->Width, this->Height);
	((GApp::UI::CoreSystem *)(animObj->CoreContext))->Api.SetTouchBoundary(r.Left, r.Top, r.Right, r.Bottom);
}
bool TouchBoundary::OnUpdate(AnimationObject * animObj)
{
	return false;
}
//================================================================================================================================================================================================================================================
void ZOrder::OnInit(AnimationObject * animObj)
{
	animObj->SetZOrder(ZOrderID);
}
bool ZOrder::OnUpdate(AnimationObject * animObj)
{
	return false;
}
//================================================================================================================================================================================================================================================
void SoundPlay::OnInit(AnimationObject * animObj)
{
	(((GApp::UI::CoreSystem *)(animObj->CoreContext))->AudioInterface).Play(sound);		
}
bool SoundPlay::OnUpdate(AnimationObject * animObj)
{
	return false;
}
//================================================================================================================================================================================================================================================
void Continous::OnInit(AnimationObject * animObj)
{
	trans->Init(animObj);
}
bool Continous::OnUpdate(AnimationObject * animObj)
{
	if (trans->Update(animObj) == false)
		trans->Init(animObj);	
	return true;
}
//================================================================================================================================================================================================================================================
void Stopper::OnInit(AnimationObject * animObj)
{
}
bool Stopper::OnUpdate(AnimationObject * animObj)
{
	return true;
}
//================================================================================================================================================================================================================================================
void Sequance::OnInit(AnimationObject * animObj)
{
	Counter = 0;
	this->list[Counter]->Init(animObj);
}
bool Sequance::OnUpdate(AnimationObject * animObj)
{
	if (this->list[Counter]->Update(animObj) == false)
	{
		Counter++;
		if (Counter >= NumberOfTransformations)
			return false;
		this->list[Counter]->Init(animObj);
	}
	return true;
}
//================================================================================================================================================================================================================================================
void Parallel::OnInit(AnimationObject * animObj)
{
	for (unsigned int tr = 0; tr < NumberOfTransformations; tr++)
		this->list[tr]->Init(animObj);
}
bool Parallel::OnUpdate(AnimationObject * animObj)
{
	unsigned int running = 0;
	Transformation** s = &list[0];
	Transformation** e = s + NumberOfTransformations;
	for (; s < e;s++)
	{
		if ((*s)->__Started == false)
			continue;
		if ((*s)->Update(animObj))
			running++;
	}
	return running>0;
}
//================================================================================================================================================================================================================================================
#define LINEAR_UPDATE_TRANSFORMATION(class_name, assign_value ) \
	void class_name::OnInit(AnimationObject * animObj) { Counter = Steps; CurrentValue = Start; Step = (End - Start) / Steps; { assign_value; } } \
	bool class_name::OnUpdate(AnimationObject * animObj) { Counter--; CurrentValue += Step; { assign_value; }; return Counter > 0; }

LINEAR_UPDATE_TRANSFORMATION(ScaleLinear, { this->Element->ScaleWidth = this->Element->ScaleHeight = CurrentValue; if (this->Element->CallVirtualFunction) this->Element->OnUpdateScale(animObj); });
LINEAR_UPDATE_TRANSFORMATION(ScaleWidthLinear, { this->Element->ScaleWidth = CurrentValue; if (this->Element->CallVirtualFunction) this->Element->OnUpdateScale(animObj); });
LINEAR_UPDATE_TRANSFORMATION(ScaleHeightLinear, { this->Element->ScaleHeight = CurrentValue; if (this->Element->CallVirtualFunction) this->Element->OnUpdateScale(animObj); });
LINEAR_UPDATE_TRANSFORMATION(AlphaBlendingLinear, { Element->ColorTransform = COLOR_ALPHA(((unsigned int)(CurrentValue*255.0f)), Element->ColorTransform); if (this->Element->CallVirtualFunction) this->Element->OnUpdateBlendColor(animObj); });


#define FORWARD_AND_BACK_UPDATE_TRANSFORMATION(class_name, assign_value ) \
	void class_name::OnInit(AnimationObject * animObj) { Counter = Steps * 2; CurrentValue = Start; Step = (End - Start) / Steps; { assign_value; } } \
	bool class_name::OnUpdate(AnimationObject * animObj) { if (Counter > Steps) CurrentValue += Step; else CurrentValue -= Step;  Counter--;  { assign_value; }; return Counter > 0; }

FORWARD_AND_BACK_UPDATE_TRANSFORMATION(ScaleForwardAndBack, { this->Element->ScaleWidth = this->Element->ScaleHeight = CurrentValue; if (this->Element->CallVirtualFunction) this->Element->OnUpdateScale(animObj); });
FORWARD_AND_BACK_UPDATE_TRANSFORMATION(ScaleWidthForwardAndBack, { this->Element->ScaleWidth = CurrentValue; if (this->Element->CallVirtualFunction) this->Element->OnUpdateScale(animObj); });
FORWARD_AND_BACK_UPDATE_TRANSFORMATION(ScaleHeightForwardAndBack, { this->Element->ScaleHeight = CurrentValue; if (this->Element->CallVirtualFunction) this->Element->OnUpdateScale(animObj); });
FORWARD_AND_BACK_UPDATE_TRANSFORMATION(AlphaBlendingForwardAndBack, { Element->ColorTransform = COLOR_ALPHA(((unsigned int)(CurrentValue*255.0f)), Element->ColorTransform); if (this->Element->CallVirtualFunction) this->Element->OnUpdateBlendColor(animObj); });

//================================================================================================================================================================================================================================================
unsigned char __Get_Resulted_Channel(unsigned char start, unsigned char end, int steps, int index)
{
	return (unsigned char)((((((float)end) - ((float)start)) / ((float)steps)) * ((float)index)) + ((float)start));
}
void ColorBlendLinear::OnInit(AnimationObject * animObj) 
{ 
	Counter = 0; 
	Element->ColorTransform = Start; 
	if (this->Element->CallVirtualFunction) this->Element->OnUpdateBlendColor(animObj);
}
bool ColorBlendLinear::OnUpdate(AnimationObject * animObj) 
{
	Counter++;
	Element->ColorTransform = COLOR_ARGB(__Get_Resulted_Channel(GET_ALPHA(Start), GET_ALPHA(End), Steps, Counter), __Get_Resulted_Channel(GET_RED(Start), GET_RED(End), Steps, Counter), __Get_Resulted_Channel(GET_GREEN(Start), GET_GREEN(End), Steps, Counter), __Get_Resulted_Channel(GET_BLUE(Start), GET_BLUE(End), Steps, Counter));
	if (this->Element->CallVirtualFunction) this->Element->OnUpdateBlendColor(animObj);
	return Counter < Steps;
}
void ColorBlendForwardAndBack::OnInit(AnimationObject * animObj)
{
	Counter = 0;
	Element->ColorTransform = Start;
	if (this->Element->CallVirtualFunction) this->Element->OnUpdateBlendColor(animObj);
}
bool ColorBlendForwardAndBack::OnUpdate(AnimationObject * animObj) 
{
	Counter++;
	int index;
	if (Counter>Steps) index = (Steps << 1) - Counter; else index = Counter;
	Element->ColorTransform = COLOR_ARGB(__Get_Resulted_Channel(GET_ALPHA(Start), GET_ALPHA(End), Steps, index), __Get_Resulted_Channel(GET_RED(Start), GET_RED(End), Steps, index), __Get_Resulted_Channel(GET_GREEN(Start), GET_GREEN(End), Steps, index), __Get_Resulted_Channel(GET_BLUE(Start), GET_BLUE(End), Steps, index));
	if (this->Element->CallVirtualFunction) this->Element->OnUpdateBlendColor(animObj);
	return Counter < (Steps << 1);
}
//================================================================================================================================================================================================================================================
void MoveRelativeLinear::OnInit(AnimationObject * animObj) 
{ 
	Counter = Steps; 
}
bool MoveRelativeLinear::OnUpdate(AnimationObject * animObj) 
{ 
	Counter--;  
	Element->X += (OffsetX / (float)Steps); 
	Element->Y += (OffsetY / (float)Steps); 
	if (this->Element->CallVirtualFunction) Element->OnUpdatePosition(animObj);
	return Counter > 0; 
}
void MoveAbsoluteLinear::OnInit(AnimationObject * animObj)
{
	float dist;
	if ((animObj->Flags >> 16) == GAC_COORDINATES_PERCENTAGE)
	{
		float dw = ((GApp::UI::CoreSystem *)(animObj->CoreContext))->Width;
		float dh = ((GApp::UI::CoreSystem *)(animObj->CoreContext))->Height;
		dist = (float)sqrt(((X - Element->X) * dw) * ((X - Element->X) * dw) + ((Y - Element->Y) *dh) * ((Y - Element->Y)* dh));
	} else {
		dist = (float)sqrt((X - Element->X) * (X - Element->X) + (Y - Element->Y) * (Y - Element->Y));
	}
	float f_steps_count = dist / (StepSize * (((GApp::UI::CoreSystem *)(animObj->CoreContext))->ResolutionAspectRatio));
	addX = (X - Element->X) / f_steps_count;
	addY = (Y - Element->Y) / f_steps_count;
	Counter = (int)f_steps_count;
}
bool MoveAbsoluteLinear::OnUpdate(AnimationObject * animObj)
{
	Counter--;
	if (Counter <= 0)
	{
		Element->X = X;
		Element->Y = Y;
	}
	else
	{
		Element->X += addX;
		Element->Y += addY;
	}
	if (this->Element->CallVirtualFunction) Element->OnUpdatePosition(animObj);
	return Counter>0;
}
void QuadraticBezier::OnInit(AnimationObject * animObj)
{
	Counter = 0;
	Element->X = StartPointX;
	Element->Y = StartPointY;
	if (this->Element->CallVirtualFunction)
		Element->OnUpdatePosition(animObj);
}
bool QuadraticBezier::OnUpdate(AnimationObject * animObj)
{
	Counter++;
	float t = ((float)Counter) / ((float)Steps);
	Element->X = (1 - t) * (1 - t) * StartPointX + 2 * (1 - t) * t * ControlPointX + t * t * EndPointX;
	Element->Y = (1 - t) * (1 - t) * StartPointY + 2 * (1 - t) * t * ControlPointY + t * t * EndPointY;
	if (this->Element->CallVirtualFunction) 
		Element->OnUpdatePosition(animObj);
	return Counter < Steps;
}
void SetNewRelativePosition::OnInit(AnimationObject * animObj)
{
	Element->X += OffsetX;
	Element->Y += OffsetY;
	if (this->Element->CallVirtualFunction) Element->OnUpdatePosition(animObj);
}
bool SetNewRelativePosition::OnUpdate(AnimationObject * animObj)
{
	return false;
}
void SetNewAbsolutePosition::OnInit(AnimationObject * animObj)
{
	Element->X = X;
	Element->Y = Y;
	if (this->Element->CallVirtualFunction) Element->OnUpdatePosition(animObj);
}
bool SetNewAbsolutePosition::OnUpdate(AnimationObject * animObj)
{
	return false;
}
//================================================================================================================================================================================================================================================
void VisibleState::OnInit(AnimationObject * animObj) { Element->Visible = Visible; }
bool VisibleState::OnUpdate(AnimationObject * animObj) { return false; }
//================================================================================================================================================================================================================================================
void ButtonEnable::OnInit(AnimationObject * animObj) { ((GApp::Animations::Elements::SimpleButtonElement*)Element)->Enabled = Enabled; ((GApp::Animations::Elements::SimpleButtonElement*)Element)->IsPressed = false; }
bool ButtonEnable::OnUpdate(AnimationObject * animObj) { return false; }
//================================================================================================================================================================================================================================================
void ColorBlendState::OnInit(AnimationObject * animObj) { Element->ColorTransform = this->Color; if (this->Element->CallVirtualFunction) this->Element->OnUpdateBlendColor(animObj); }
bool ColorBlendState::OnUpdate(AnimationObject * animObj) { return false; }
//================================================================================================================================================================================================================================================
void AlphaBlendingState::OnInit(AnimationObject * animObj) { unsigned int a = (unsigned int)(Alpha * 255.0f); Element->ColorTransform = COLOR_ALPHA(a, Element->ColorTransform); if (this->Element->CallVirtualFunction) this->Element->OnUpdateBlendColor(animObj); }
bool AlphaBlendingState::OnUpdate(AnimationObject * animObj) { return false; }
//================================================================================================================================================================================================================================================
void SetNewText::OnInit(AnimationObject * animObj) { ((Elements::TextElement*)this->Element)->TP.SetText(this->Text); }
bool SetNewText::OnUpdate(AnimationObject * animObj) { return false; }
//================================================================================================================================================================================================================================================
void SetNewImage::OnInit(AnimationObject * animObj) { ((Elements::SingleImageElement*)this->Element)->Image = this->Image; }
bool SetNewImage::OnUpdate(AnimationObject * animObj) { return false; }
//================================================================================================================================================================================================================================================
#define SET_IMAGE_INDEX(idx) {\
	Elements::MultipleImageElement * img = (Elements::MultipleImageElement *)this->Element; \
	if ((idx >= 0) && (idx < img->Count)) { \
		img->Image = img->ImageList[idx]; \
		img->ImageIndex = idx; \
	} \
}
void SetImageIndex::OnInit(AnimationObject * animObj) 
{
	SET_IMAGE_INDEX(this->ImageIndex);
}
bool SetImageIndex::OnUpdate(AnimationObject * animObj) { return false; }
//================================================================================================================================================================================================================================================
void ImageIndexLinear::OnInit(AnimationObject * animObj)
{
	this->CurrentIndex = this->StartIndex;
	this->Timer = this->FramesInterval;
	SET_IMAGE_INDEX(this->CurrentIndex);
}
bool ImageIndexLinear::OnUpdate(AnimationObject * animObj) {
	this->Timer--;
	if (this->Timer > 0)
		return true;
	this->Timer = this->FramesInterval;
	if (this->CurrentIndex == EndIndex)
		return false;
	if (this->CurrentIndex < EndIndex)
	{
		this->CurrentIndex += Step;
		if (this->CurrentIndex > EndIndex)
			this->CurrentIndex = EndIndex;
	}
	else
	{
		this->CurrentIndex -= Step;
		if (this->CurrentIndex < EndIndex)
			this->CurrentIndex = EndIndex;
	}
	SET_IMAGE_INDEX(this->CurrentIndex);
	return true;
}
//================================================================================================================================================================================================================================================
void TextFlow::OnInit(AnimationObject * animObj) 
{ 
	Counter = FramesUpdate;
	Poz = 0;
	MaxChars = ((Elements::TextElement*)this->Element)->TP.GetCharactersCount();
	((Elements::TextElement*)this->Element)->TP.SetCharactesVisibility(0, 0xFFFFFFFF, false);
}
bool TextFlow::OnUpdate(AnimationObject * animObj) 
{ 
	Counter--;
	if (Counter <= 0)
	{
		Counter = FramesUpdate;
		((Elements::TextElement*)this->Element)->TP.SetCharactesVisibility(Poz, Poz + 1, true);
		Poz++;		
		return Poz < MaxChars;
	}
	return true;
}
//================================================================================================================================================================================================================================================
void TextCenterFlow::OnInit(AnimationObject * animObj)
{
	Counter = FramesUpdate;
	MaxChars = ((Elements::TextElement*)this->Element)->TP.GetCharactersCount();
	Poz = MaxChars / 2;
	((Elements::TextElement*)this->Element)->TP.SetCharactesVisibility(0, 0xFFFFFFFF, false);
}
bool TextCenterFlow::OnUpdate(AnimationObject * animObj)
{
	Counter--;
	if (Counter <= 0)
	{
		Counter = FramesUpdate;
		((Elements::TextElement*)this->Element)->TP.SetCharactesVisibility(Poz, MaxChars-Poz, true);
		if (Poz > 0)
		{
			Poz--;
		}
		else 
		{
			((Elements::TextElement*)this->Element)->TP.SetCharactesVisibility(0, 0xFFFFFFFF, true);
			return false;
		}
		return true;
	}
	return true;
}
//================================================================================================================================================================================================================================================
void FontSize::OnInit(AnimationObject * animObj)
{
	((Elements::TextElement*)this->Element)->TP.SetFontSize(GAC_FONTSIZETYPE_SCALE, this->Size);
}
bool FontSize::OnUpdate(AnimationObject * animObj)
{
	return false;
}
//================================================================================================================================================================================================================================================
void TextCharacterVisibility::OnInit(AnimationObject * animObj)
{
	GApp::Graphics::TextPainter *tp = &(((Elements::TextElement*)this->Element)->TP);
	int _end = End;
	if (_end < 0)
		_end = End + ((int)tp->GetCharactersCount());
	for (int tr = Start; tr <= _end; tr += Step)
		tp->SetCharactesVisibility((unsigned int)tr, (unsigned int)(tr + 1), Visibility);
}
bool TextCharacterVisibility::OnUpdate(AnimationObject * animObj)
{
	return false;
}
//================================================================================================================================================================================================================================================
#define NUMERIC_FORMAT_Integer						0
#define NUMERIC_FORMAT_IntegerWithDigitGrouping		1
#define NUMERIC_FORMAT_FloatWith2DecimalPlaces		2
#define NUMERIC_FORMAT_FloatWith3DecimalPlaces		3
#define NUMERIC_FORMAT_Percentage					4
#define NUMERIC_FORMAT_Percentage2DecimalPlaces		5

void __Set_Number_(GApp::Graphics::TextPainter *tp, int formatType, int value)
{
	char temp[32];
	GApp::Utils::String tmpStr;
	tmpStr.Create(temp, 32, true);
	switch (formatType)
	{
		case NUMERIC_FORMAT_Integer: tmpStr.AddFormatedEx("%{int}", value); break;
		case NUMERIC_FORMAT_IntegerWithDigitGrouping: tmpStr.AddFormatedEx("%{int,G3}", value); break;
		case NUMERIC_FORMAT_FloatWith2DecimalPlaces: tmpStr.Format("%.2f", *((float *)&value)); break;
		case NUMERIC_FORMAT_FloatWith3DecimalPlaces: tmpStr.Format("%.3f", *((float *)&value)); break;
		case NUMERIC_FORMAT_Percentage: tmpStr.Format("%d%%", (int)((*((float *)&value))*100.0f)); break;
		case NUMERIC_FORMAT_Percentage2DecimalPlaces: tmpStr.Format("%.2f%%", ((*((float *)&value))*100.0f)); break;
	}
	tp->SetText(tmpStr);
}

void NumericFormatter::OnInit(AnimationObject * animObj)
{
	__Set_Number_(&(((Elements::TextElement*)this->Element)->TP), this->FormatType, intValue);
}
bool NumericFormatter::OnUpdate(AnimationObject * animObj)
{
	return false;
}
void NumberIncrease::OnInit(AnimationObject * animObj)
{
	__Set_Number_(&(((Elements::TextElement*)this->Element)->TP), this->FormatType, intStart);
	step = 0;
}
bool NumberIncrease::OnUpdate(AnimationObject * animObj)
{
	step++;
	if ((FormatType == NUMERIC_FORMAT_Integer) || (FormatType == NUMERIC_FORMAT_IntegerWithDigitGrouping))
	{
		int int_cValue = intStart + (((intEnd - intStart)*step) / Steps);
		if (step >= Steps)
			int_cValue = intEnd;
		__Set_Number_(&(((Elements::TextElement*)this->Element)->TP), this->FormatType, int_cValue);
	}
	else {
		float float_cValue = floatStart + (((floatEnd - floatStart)*step) / Steps);
		if (step >= Steps)
			float_cValue = floatEnd;
		__Set_Number_(&(((Elements::TextElement*)this->Element)->TP), this->FormatType, *(int*)(&float_cValue));
	}
	return step < Steps;
}

//================================================================================================================================================================================================================================================
#define COUNT_DOWN_FORMAT_AUTO						0
#define COUNT_DOWN_FORMAT_SECONDS					1
#define COUNT_DOWN_FORMAT_MIN_SEC					2
#define COUNT_DOWN_FORMAT_HOUR_MIN_SEC				3
#define COUNT_DOWN_FORMAT_DAYS_HOUR_MIN_SEC			4

void __Set_CountDownNumber_(GApp::Graphics::TextPainter *tp, int formatType, int value)
{
	char temp[32];
	GApp::Utils::String tmpStr;
	tmpStr.Create(temp, 32, true);
	int h, d, m, s;
	
	if (value >= 0)
	{
		d = value / (3600 * 24); value = value % (3600 * 24);
		h = value / 3600; value = value % 3600;
		m = value / 60; s = value % 60;

		if (formatType == COUNT_DOWN_FORMAT_AUTO)
		{
			if (d > 0)
				formatType = COUNT_DOWN_FORMAT_DAYS_HOUR_MIN_SEC;
			else {
				if (h > 0)
					formatType = COUNT_DOWN_FORMAT_HOUR_MIN_SEC;
				else {
					if (m > 0)
						formatType = COUNT_DOWN_FORMAT_MIN_SEC;
					else
						formatType = COUNT_DOWN_FORMAT_SECONDS;
				}
			}
		}

		switch (formatType)
		{
			case COUNT_DOWN_FORMAT_SECONDS:				tmpStr.SetFormat(":%02d", s); break;
			case COUNT_DOWN_FORMAT_MIN_SEC:				tmpStr.SetFormat("%02d:%02d", m,s); break;
			case COUNT_DOWN_FORMAT_HOUR_MIN_SEC:		tmpStr.SetFormat("%02d:%02d:%02d", h,m,s); break;
			case COUNT_DOWN_FORMAT_DAYS_HOUR_MIN_SEC:	tmpStr.SetFormat("%dd %02d:%02d:%02d", d,h,m,s); break;
		}
	}
	tp->SetText(tmpStr);
}
void CountDown::OnInit(AnimationObject * animObj)
{	
	Counter = 0;
	CurrentValue = SecondsLeft;
	__Set_CountDownNumber_(&(((Elements::TextElement*)this->Element)->TP), this->FormatType, CurrentValue);
}
bool CountDown::OnUpdate(AnimationObject * animObj)
{
	if (CurrentValue <= 0)
		return false;
	Counter++;
	if (Counter >= 60)
	{
		Counter = 0;
		CurrentValue--;
		__Set_CountDownNumber_(&(((Elements::TextElement*)this->Element)->TP), this->FormatType, CurrentValue);
	}
	return true;
}