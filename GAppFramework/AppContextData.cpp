#include "GApp.h"
#ifdef PLATFORM_DEVELOP
	#include "DevelopMode.h"
#endif

#define EVENT_TOUCH_DOWN			1
#define EVENT_TOUCH_UP				2
#define EVENT_TOUCH_MOVE			3
#define EVENT_TOUCH_KEY				4
#define EVENT_ADVERTISING			5
#define EVENT_SERVICE				6

#define UPDATE_TOUCHEVENT_FOR_CONTROL(etp,control) {\
	etp->Event.X = etp->XPixel-control->Layout.LeftPixels;\
	etp->Event.Y = etp->YPixel-control->Layout.TopPixels;\
	etp->Event.OriginalX = etp->OriginalXPixels-control->Layout.LeftPixels;\
	etp->Event.OriginalY = etp->OriginalYPixels-control->Layout.TopPixels;\
	etp->Event.LastX = etp->LastXPixels-control->Layout.LeftPixels;\
	etp->Event.LastY = etp->LastYPixels-control->Layout.TopPixels;\
}

#define UPDATE_TOUCHEVENT_FOR_SCENE(etp) {\
	etp->Event.X = etp->XPixel;\
	etp->Event.Y = etp->YPixel;\
	etp->Event.OriginalX = etp->OriginalXPixels;\
	etp->Event.OriginalY = etp->OriginalYPixels;\
	etp->Event.LastX = etp->LastXPixels;\
	etp->Event.LastY = etp->LastYPixels;\
}

#define CHECK_TOUCH_BOUNDARY \
if (this->TouchBoundaryEnabled) { \
	CHECK(this->TouchBoundary.Contains(x, y), false, "Touch location (%.2f,%.2f) outside its boudaries (%.2f,%.2f) - (%.2f,%.2f)", x, y, this->TouchBoundary.Left, this->TouchBoundary.Top, this->TouchBoundary.Right, this->TouchBoundary.Bottom); \
}

void AppContextData::SetFocusControl(GApp::Controls::GenericControl* control)
{
	if (FocusesControl!=control)
	{
		if (FocusesControl!=NULL)
		{
			REMOVE_BIT(FocusesControl->Flags,GAC_CONTROL_FLAG_FOCUSED);
			FocusesControl->OnLoseFocus();
		}
		FocusesControl = control;
		if (FocusesControl!=NULL)
		{
			SET_BIT(FocusesControl->Flags,GAC_CONTROL_FLAG_FOCUSED);
			FocusesControl->OnReceiveFocus();
		}
	}
}
GApp::Controls::GenericControl* AppContextData::TouchPositionToControl(GApp::Controls::GenericControl* control,float x,float y)
{
	if (control==NULL)
		return NULL;
	if ((control->Flags & (GAC_CONTROL_FLAG_VISIBLE|GAC_CONTROL_FLAG_OUTSIDE_PARENT_RECT|GAC_CONTROL_FLAG_ENABLED|GAC_CONTROL_FLAG_NO_TOUCH))!=(GAC_CONTROL_FLAG_VISIBLE|GAC_CONTROL_FLAG_ENABLED))
		return NULL;
	if ((x>=control->Layout.LeftPixels) && (x<=control->Layout.LeftPixels+control->Layout.WidthInPixels) && (y>=control->Layout.TopPixels) && (y<=control->Layout.TopPixels+control->Layout.HeighInPixels))
	{
		GApp::Controls::GenericControl* res;
		if (control->Controls!=NULL)
		{
			for (unsigned int tr=0;tr<control->ControlsCount;tr++)
			{
				res = TouchPositionToControl(control->Controls[tr],x,y);
				if (res!=NULL)
					return res;
			}
		}
		// daca nici unul din copii meu nu vrea
		
		return control;
	}
	return NULL;
}
bool AppContextData::InitControlsOnSceneActivate(GApp::Controls::GenericControl *control,GApp::UI::Scene *Scene, CoreReference coreContext)
{
	CHECK(control!=NULL,false,"Control is NULL");
	
	bool result = true;
	control->HandlerScene = Scene;
	control->CoreContext = coreContext;
	// alte resetari
	REMOVE_BIT(control->Flags,GAC_CONTROL_FLAG_TOUCHED);

	if (control->Controls!=NULL)
	{
		for (unsigned int tr=0;tr<control->ControlsCount;tr++)
			result = result & InitControlsOnSceneActivate(control->Controls[tr],Scene,coreContext);
	}
	return result;
}
bool AppContextData::InitCurrentScene()
{
	unsigned int tr;
	// initializare clasa
	if (CurrentScene->MainControl!=NULL)
	{
		InitControlsOnSceneActivate(CurrentScene->MainControl,CurrentScene,&this->CoreObject);
		CurrentScene->MainControl->RecomputeLayout();
		for (tr=0;tr<MAX_TOUCH_POINTERS;tr++)
		{
			TouchPointers[tr].Event.Type = GAC_TOUCHEVENTTYPE_NONE;
			TouchPointers[tr].Control = NULL;
		}
		ZoomEvent.Event.Type = GAC_TOUCHEVENTTYPE_NONE;
		ZoomEvent.Pointer1 = -1;
		ZoomEvent.Pointer2 = -1;
	}
	SetFocusControl(NULL);
	return true;
}
void AppContextData::ProcessEventTouchDownForScene(ExtendedTouchEvent *etp)
{
	GApp::Controls::GenericControl* res;
	if (CurrentScene->MainControl!=NULL)
	{	
		res = TouchPositionToControl(CurrentScene->MainControl,etp->XPixel,etp->YPixel);
		if (res!=NULL)
		{
			while (res!=NULL)
			{
				UPDATE_TOUCHEVENT_FOR_CONTROL(etp,res);
				if (res->OnTouchEvent(&etp->Event))
					break;
				res = res->Parent;
			}
			if (res!=NULL)
			{
				LOG_EVENT("TouchDownEvent assign to control: %p",res);
				SetFocusControl(res);
				SET_BIT(res->Flags,GAC_CONTROL_FLAG_TOUCHED);
				etp->Control = res;
				return;
			}
		} else {
			LOG_ERROR("No touch event assign - missing MainControl ???");
		}		
	};
	LOG_EVENT("TouchDownEvent assign to Scene");
	// alfel il asignez scenei
	SetFocusControl(NULL);
	etp->Control = NULL;
	// update touch event position for scene
	UPDATE_TOUCHEVENT_FOR_SCENE(etp);
	CurrentScene->OnTouchEvent(&etp->Event);
}
void AppContextData::ProcessNormalTouchEvent(ExtendedTouchEvent *etp)
{
	GAC_TYPE_TOUCHEVENTTYPE ttype;
	//GApp::Controls::GenericControl* res;
	
	
	if (etp->Control!=NULL)
	{
		// prima data incerc sa ii dau lui event-ul sa vad daca il proceseaza
		UPDATE_TOUCHEVENT_FOR_CONTROL(etp,etp->Control);
		if (etp->Control->OnTouchEvent(&etp->Event)==true)
			return; 
		// daca nu l-a procesat - mai intai ii trimit lui un UP
		LOG_EVENT("Current control can't process the event - move to parent");
		ttype = etp->Event.Type;
		etp->Event.Type = GAC_TOUCHEVENTTYPE_UP;
		UPDATE_TOUCHEVENT_FOR_CONTROL(etp,etp->Control);
		etp->Control->OnTouchEvent(&etp->Event);
		REMOVE_BIT(etp->Control->Flags,GAC_CONTROL_FLAG_TOUCHED);		
		// caut alt control care ar putea sa il proceseze
		etp->Control = etp->Control->Parent;
		etp->Event.Type = GAC_TOUCHEVENTTYPE_DOWN;
		while (etp->Control!=NULL)
		{
			UPDATE_TOUCHEVENT_FOR_CONTROL(etp,etp->Control);
			if (etp->Control->OnTouchEvent(&etp->Event)==true)
			{
				etp->Event.Type = ttype;
				SetFocusControl(etp->Control);
				SET_BIT(etp->Control->Flags,GAC_CONTROL_FLAG_TOUCHED);
				ProcessNormalTouchEvent(etp);
				return;
			}
			etp->Control = etp->Control->Parent;		
		}
		// nici un control nu a preluat req - dau la scena
		etp->Control = NULL;
		SetFocusControl(NULL);
		etp->Event.Type = GAC_TOUCHEVENTTYPE_DOWN;
		UPDATE_TOUCHEVENT_FOR_SCENE(etp);
		CurrentScene->OnTouchEvent(&etp->Event);
		etp->Event.Type = ttype;
	}
	/*
	if (etp->Control!=NULL)
	{
		res = etp->Control;
		while (res!=NULL)
		{
			UPDATE_TOUCHEVENT_FOR_CONTROL(etp,res);
			if (res->OnTouchEvent(&etp->Event))
				break;
			LOG_INFO("Tre sa merg mai jos");
			res = res->Parent;
		}
		if (res!=etp->Control)
		{
			ttype = etp->Event.Type;
			etp->Event.Type = GAC_TOUCHEVENTTYPE_UP;
			UPDATE_TOUCHEVENT_FOR_CONTROL(etp,etp->Control);
			etp->Control->OnTouchEvent(&etp->Event);
			REMOVE_BIT(etp->Control->Flags,GAC_CONTROL_FLAG_TOUCHED);
			SetFocusControl(res);
			if (res!=NULL)
			{
				SET_BIT(res->Flags,GAC_CONTROL_FLAG_TOUCHED);
				etp->Control = res;
				etp->Event.Type = GAC_TOUCHEVENTTYPE_DOWN;
				etp->OriginalXPixels = etp->LastXPixels = etp->Event.OriginalX = etp->Event.LastX = etp->Event.X = etp->XPixel;
				etp->OriginalYPixels = etp->LastYPixels = etp->Event.OriginalY = etp->Event.LastY = etp->Event.Y = etp->YPixel;
				UPDATE_TOUCHEVENT_FOR_CONTROL(etp,etp->Control);
				etp->Control->OnTouchEvent(&etp->Event);
				etp->Event.Type = ttype;
				UPDATE_TOUCHEVENT_FOR_CONTROL(etp,etp->Control);
				etp->Control->OnTouchEvent(&etp->Event);
				return;
			} else {
				etp->Control = NULL;
				etp->Event.Type = GAC_TOUCHEVENTTYPE_DOWN;
				UPDATE_TOUCHEVENT_FOR_SCENE(etp);
				etp->Event.Type = ttype;
			}
		} else {
			UPDATE_TOUCHEVENT_FOR_CONTROL(etp,etp->Control);
			etp->Control->OnTouchEvent(&etp->Event);
			return;
		}
	}
	*/
	// trimit la scena
	UPDATE_TOUCHEVENT_FOR_SCENE(etp);
	CurrentScene->OnTouchEvent(&etp->Event);
}
bool AppContextData::ProcessEventForScene(EventQue *e)
{
	unsigned int count,tr;
	GApp::Controls::KeyboardEvent keybEvent;
	GApp::Platform::AdInterface *ad;
	// initial setez touch pointerii
	LOG_EVENT("AppContextData::ProcessEventForScene(Type:%d,UintValue:%d,Float:[%f,%f,%f],Time:%d)",e->EventType,e->UintValue,e->FloatValues[0],e->FloatValues[1],e->FloatValues[2],e->Time); 
#ifdef PLATFORM_DEVELOP
	switch (e->EventType)
	{
		case EVENT_TOUCH_DOWN:
			if (GApp::DevelopMode::Execution::OnTouchEvent(DEVMODE_EVENT_TOUCH_DOWN, e->FloatValues[0], e->FloatValues[1]) == true)
				return true;
			break;
		case EVENT_TOUCH_MOVE:
			if (GApp::DevelopMode::Execution::OnTouchEvent(DEVMODE_EVENT_TOUCH_MOVE, e->FloatValues[0], e->FloatValues[1]) == true)
				return true;
			break;
		case EVENT_TOUCH_UP:
			if (GApp::DevelopMode::Execution::OnTouchEvent(DEVMODE_EVENT_TOUCH_UP, e->FloatValues[0], e->FloatValues[1]) == true)
				return true;
			break;
	}
#endif


	ExtendedTouchEvent *etp;
	switch (e->EventType)
	{
		case EVENT_TOUCH_DOWN:
			CHECK(e->UintValue<MAX_TOUCH_POINTERS,false,"Invalid touch pointer id (%d) - Max allowed is %d",e->UintValue,MAX_TOUCH_POINTERS-1);
			etp = &TouchPointers[e->UintValue];
			etp->XPixel = etp->OriginalXPixels = etp->LastXPixels = e->FloatValues[0];
			etp->YPixel = etp->OriginalYPixels = etp->LastYPixels = e->FloatValues[1];
			etp->Event.IsClick = true;
			etp->Event.Time = e->Time;
			etp->Event.TimeDelta = 0;
			etp->Event.Type = GAC_TOUCHEVENTTYPE_DOWN;
			etp->Event.IsPartOfZoom = false;
			if ((this->TouchFlags & TOUCH_FLAG_ENABLE_ZOOM_EVENT) != 0)
			{				
				float x = 0;
				float y = 0;
				for (tr = 0, count = 0; tr < MAX_TOUCH_POINTERS; tr++)
				{
					if (TouchPointers[tr].Event.Type != GAC_TOUCHEVENTTYPE_NONE) {
						count++;
						x += TouchPointers[tr].XPixel;
						y += TouchPointers[tr].YPixel;
					}
				}
				LOG_EVENT("TouchFlags = %08X, count = %d", TouchFlags, count);
				if (count == 2)
				{
					ZoomEvent.Event.Type = GAC_TOUCHEVENTTYPE_STARTZOOM;
					ZoomEvent.Pointer1 = ZoomEvent.Pointer2 = -1;
					for (tr = 0, count = 0; tr < MAX_TOUCH_POINTERS; tr++)
					{
						if (TouchPointers[tr].Event.Type != GAC_TOUCHEVENTTYPE_NONE)
						{
							if (ZoomEvent.Pointer1 == -1)
							{
								ZoomEvent.Pointer1 = tr;
								TouchPointers[tr].Event.IsPartOfZoom = true;
							}
							else
							{
								ZoomEvent.Pointer2 = tr;
								TouchPointers[tr].Event.IsPartOfZoom = true;
							}
						}
					}
					// daca vreunul din pointeri e asignat unui control il deasignez
					if ((TouchPointers[ZoomEvent.Pointer1].Control != NULL) || (TouchPointers[ZoomEvent.Pointer2].Control != NULL))
					{
						ZoomEvent.Event.Type = GAC_TOUCHEVENTTYPE_NONE;
						ZoomEvent.Pointer1 = ZoomEvent.Pointer2 = -1;
					}
					else {
						// avem zoom
						ZoomEvent.Event.OriginalDistance = SQUAREDDIST(TouchPointers[ZoomEvent.Pointer1].XPixel, TouchPointers[ZoomEvent.Pointer1].YPixel, TouchPointers[ZoomEvent.Pointer2].XPixel, TouchPointers[ZoomEvent.Pointer2].YPixel);
						ZoomEvent.Event.LastDistance = ZoomEvent.Event.OriginalDistance;
						ZoomEvent.Event.Time = 0;
						ZoomEvent.Event.TimeDelta = 0;
						ZoomEvent.Event.X = x / 2.0f;
						ZoomEvent.Event.Y = y / 2.0f;
						ZoomEvent.Event.PointerID = -1;
						ZoomEvent.Event.IsPartOfZoom = true;
						LOG_EVENT("Starting zoom event");
						CurrentScene->OnTouchEvent(&ZoomEvent.Event);
						ZoomEvent.Event.Type = GAC_TOUCHEVENTTYPE_ZOOM;
					}
				}
				else {
					ZoomEvent.Event.Type = GAC_TOUCHEVENTTYPE_NONE;
					ZoomEvent.Pointer1 = ZoomEvent.Pointer2 = -1;
				}
			}
			ProcessEventTouchDownForScene(etp);
			break;
		case EVENT_TOUCH_UP:
			CHECK(e->UintValue<MAX_TOUCH_POINTERS,false,"Invalid touch pointer id (%d) - Max allowed is %d",e->UintValue,MAX_TOUCH_POINTERS-1);
			etp = &TouchPointers[e->UintValue];
			CHECK(etp->Event.Type != GAC_TOUCHEVENTTYPE_NONE,false,"Event is of type None");
			etp->LastXPixels = etp->XPixel;
			etp->XPixel = e->FloatValues[0];
			etp->LastYPixels = etp->YPixel;
			etp->YPixel = e->FloatValues[1];
			etp->Event.TimeDelta = e->Time-etp->Event.Time;
			etp->Event.Type = GAC_TOUCHEVENTTYPE_UP;
			// verific exact ce anume dau
			// dau event UP
			// dau event click daca e cazul
			if (etp->Control!=NULL)
			{
				switch (TouchFlags & TOUCH_FLAG_CONTROL_SENDEVENT_MASK)
				{
					case TOUCH_FLAG_CONTROL_SENDEVENT_UP_AND_CLICK:
						etp->Control->OnTouchEvent(&etp->Event);
						if (etp->Event.IsClick) { etp->Event.Type = GAC_TOUCHEVENTTYPE_CLICK; etp->Control->OnTouchEvent(&etp->Event); }
						break;
					case TOUCH_FLAG_CONTROL_SENDEVENT_CLICK_AND_UP:
						etp->Event.Type = GAC_TOUCHEVENTTYPE_CLICK;
						etp->Control->OnTouchEvent(&etp->Event);
						if (etp->Event.IsClick) { etp->Event.Type = GAC_TOUCHEVENTTYPE_UP; etp->Control->OnTouchEvent(&etp->Event); }
						break;
					case TOUCH_FLAG_CONTROL_SENDEVENT_ONLY_UP:
						etp->Control->OnTouchEvent(&etp->Event);
						break;
					case TOUCH_FLAG_CONTROL_SENDEVENT_UP_OR_CLICK:
						if (etp->Event.IsClick) etp->Event.Type = GAC_TOUCHEVENTTYPE_CLICK;
						etp->Control->OnTouchEvent(&etp->Event);
						break;
				}			
			} else {
				// updatez pozitie
				switch (TouchFlags & TOUCH_FLAG_SCENE_SENDEVENT_MASK)
				{
					case TOUCH_FLAG_SCENE_SENDEVENT_UP_AND_CLICK:
						CurrentScene->OnTouchEvent(&etp->Event);
						if (etp->Event.IsClick) { etp->Event.Type = GAC_TOUCHEVENTTYPE_CLICK; CurrentScene->OnTouchEvent(&etp->Event);}
						break;
					case TOUCH_FLAG_SCENE_SENDEVENT_CLICK_AND_UP:
						etp->Event.Type = GAC_TOUCHEVENTTYPE_CLICK;
						CurrentScene->OnTouchEvent(&etp->Event);
						if (etp->Event.IsClick) { etp->Event.Type = GAC_TOUCHEVENTTYPE_UP; CurrentScene->OnTouchEvent(&etp->Event); }
						break;
					case TOUCH_FLAG_SCENE_SENDEVENT_ONLY_UP:
						CurrentScene->OnTouchEvent(&etp->Event);
						break;
					case TOUCH_FLAG_SCENE_SENDEVENT_UP_OR_CLICK:
						if (etp->Event.IsClick) etp->Event.Type = GAC_TOUCHEVENTTYPE_CLICK;
						CurrentScene->OnTouchEvent(&etp->Event);
						break;
				}
			
			}						
			// curat datele
			if (etp->Control!=NULL)
				REMOVE_BIT(etp->Control->Flags,GAC_CONTROL_FLAG_TOUCHED);
			etp->Control = NULL;
			etp->Event.Type = GAC_TOUCHEVENTTYPE_NONE;	
			if (((this->TouchFlags & TOUCH_FLAG_ENABLE_ZOOM_EVENT) != 0) && (ZoomEvent.Event.Type == GAC_TOUCHEVENTTYPE_ZOOM))
			{
				LOG_EVENT("Enging zoom event");
				ZoomEvent.Event.Type = GAC_TOUCHEVENTTYPE_ENDZOOM;
				CurrentScene->OnTouchEvent(&ZoomEvent.Event);
				ZoomEvent.Event.Type = GAC_TOUCHEVENTTYPE_NONE;
				ZoomEvent.Pointer1 = ZoomEvent.Pointer2 = -1;
			}
			break;
		case EVENT_TOUCH_MOVE:
			CHECK(e->UintValue<MAX_TOUCH_POINTERS,false,"Invalid touch pointer id (%d) - Max allowed is %d",e->UintValue,MAX_TOUCH_POINTERS-1);
			etp = &TouchPointers[e->UintValue];
			CHECK(etp->Event.Type != GAC_TOUCHEVENTTYPE_NONE,false,"Event is of type None");
			etp->LastXPixels = etp->XPixel;
			etp->XPixel = e->FloatValues[0];
			etp->LastYPixels = etp->YPixel;
			etp->YPixel = e->FloatValues[1];
			etp->Event.TimeDelta = e->Time-etp->Event.Time;
			etp->Event.Type = GAC_TOUCHEVENTTYPE_MOVE;
			if (etp->Event.IsClick)
			{
				etp->Event.IsClick = (SQUAREDDIST(etp->OriginalXPixels,etp->OriginalYPixels,etp->XPixel,etp->YPixel)<MinDistForClickSquared);
			}
			if (((this->TouchFlags & TOUCH_FLAG_ENABLE_ZOOM_EVENT) != 0) && (ZoomEvent.Event.Type == GAC_TOUCHEVENTTYPE_ZOOM))
			{
				TouchPointers[ZoomEvent.Pointer1].Event.IsClick = false;
				TouchPointers[ZoomEvent.Pointer2].Event.IsClick = false;
				// fac update la zoom
				ZoomEvent.Event.LastDistance = ZoomEvent.Event.Distance;
				ZoomEvent.Event.Distance = SQUAREDDIST(TouchPointers[ZoomEvent.Pointer1].XPixel, TouchPointers[ZoomEvent.Pointer1].YPixel, TouchPointers[ZoomEvent.Pointer2].XPixel, TouchPointers[ZoomEvent.Pointer2].YPixel);				
				ZoomEvent.Event.ZoomValue = sqrt(ZoomEvent.Event.Distance / ZoomEvent.Event.OriginalDistance);
				LOG_EVENT("ZoomEvent(%f)", ZoomEvent.Event.ZoomValue);
				CurrentScene->OnTouchEvent(&ZoomEvent.Event);
			}
			else {
				ProcessNormalTouchEvent(etp);
			}
			break;
		case EVENT_TOUCH_KEY:
			keybEvent.KeyCode = e->UintValue;
			keybEvent.Flags = 0;
			CurrentScene->OnKeyEvent(&keybEvent);
			break;
		case EVENT_ADVERTISING:
			ad = this->OS->Data.Resources->GetAd(e->UintValue >> 16);
			LOG_EVENT("Calling Scene::OnAdEvent(%p,%d)", ad, e->UintValue & 0xFFFF);
			if (ad != NULL) {				
				if (CurrentScene->OnAdEvent(ad, e->UintValue & 0xFFFF) == false)
				{
					// fallback-ul din application
					this->App->OnAdEvent(ad, e->UintValue & 0xFFFF);
				}
			}
			else {
				LOG_ERROR("Invalid Ad (null) for ID=%d", e->UintValue >> 16);
			}
			break;
		case EVENT_SERVICE:
			LOG_EVENT("Calling Scene::OnServiceEvent(%d,%d)", e->UintValue >> 16, e->UintValue & 0xFFFF);
			if (CurrentScene->OnServiceEvent(e->UintValue >> 16, e->UintValue & 0xFFFF) == false)
			{
				// fallback-ul din application
				this->App->OnServiceEvent(e->UintValue >> 16, e->UintValue & 0xFFFF);
			}
			break;
	}
	return true;
}

bool AppContextData::QueTouchDownEvent(unsigned int pointer, float x, float y,unsigned int time)
{
	EventQue *e = &Events[EventsQueWrite];
	CHECK(e->EventIsFree,false,"No free event slots available!");
	CHECK_TOUCH_BOUNDARY;
	LOG_EVENT("Add TouchDown event (X=%d,y=%d) to Pointer %d [Que:%d] - Time:%d",(int)x,(int)y,pointer,EventsQueWrite,time);
	e->UintValue = pointer;
	e->FloatValues[0] = x;
	e->FloatValues[1] = y;
	e->EventType = EVENT_TOUCH_DOWN;
	e->EventIsFree = false;
	e->Time = time;
	EventsQueWrite = (EventsQueWrite+1)%(MAX_EVENTS_IN_QUE);
	return true;
}
bool AppContextData::QueTouchUpEvent(unsigned int pointer, float x, float y,unsigned int time)
{
	EventQue *e = &Events[EventsQueWrite];
	CHECK(e->EventIsFree,false,"No free event slots available!");
	CHECK_TOUCH_BOUNDARY;
	LOG_EVENT("Add TouchUp event (X=%d,y=%d) to Pointer %d [Que:%d] - Time:%d",(int)x,(int)y,pointer,EventsQueWrite,time);
	e->UintValue = pointer;
	e->FloatValues[0] = x;
	e->FloatValues[1] = y;
	e->EventType = EVENT_TOUCH_UP;
	e->EventIsFree = false;
	e->Time = time;
	EventsQueWrite = (EventsQueWrite+1)%(MAX_EVENTS_IN_QUE);
	return true;
}
bool AppContextData::QueTouchMoveEvent(unsigned int pointer, float x, float y,unsigned int time)
{
	EventQue *e = &Events[EventsQueWrite];
	CHECK(e->EventIsFree,false,"No free event slots available!");
	CHECK_TOUCH_BOUNDARY;
	LOG_EVENT("Add TouchMove event (X=%d,y=%d) to Pointer %d [Que:%d] - Time:%d",(int)x,(int)y,pointer,EventsQueWrite,time);
	e->UintValue = pointer;
	e->FloatValues[0] = x;
	e->FloatValues[1] = y;
	e->EventType = EVENT_TOUCH_MOVE;
	e->EventIsFree = false;
	e->Time = time;
	EventsQueWrite = (EventsQueWrite+1)%(MAX_EVENTS_IN_QUE);
	return true;
}
bool AppContextData::QueKeyEvent(unsigned int keyState)
{
	EventQue *e = &Events[EventsQueWrite];
	CHECK(e->EventIsFree,false,"No free event slots available!");
	LOG_EVENT("Add Keyboard event %08X",keyState);
	e->UintValue = keyState;
	e->EventType = EVENT_TOUCH_KEY;
	e->EventIsFree = false;
	EventsQueWrite = (EventsQueWrite+1)%(MAX_EVENTS_IN_QUE);
	return true;
}
bool AppContextData::QueAdEvent(int AdID,int EventID)
{
	//LOG_INFO("--------QueAdEvent(AdID=%d,EventID=%d,EventsQueWrite=%d)--------", AdID, EventID, EventsQueWrite);
	EventQue *e = &Events[EventsQueWrite];
	CHECK(e->EventIsFree, false, "No free event slots available!");
	CHECK((AdID >= 0) && (AdID < 0x7FFF), false, "Expecting an AdID bigger than 0 and smaller than 0x7FFF and received: %d", AdID);
	CHECK((EventID >= 0) && (EventID < 0x7FFF), false, "Expecting an EventID bigger than 0 and smaller than 0x7FFF and received: %d", EventID);
	LOG_EVENT("Add AdEvent event [AdID:%d, EventdID:%d]", AdID,EventID);
	e->UintValue = (AdID<<16)|EventID;
	e->EventType = EVENT_ADVERTISING;
	e->EventIsFree = false;
	EventsQueWrite = (EventsQueWrite + 1) % (MAX_EVENTS_IN_QUE);
	return true;
}


bool AppContextData::QueServiceEvent(int serviceID, int EventID)
{
	//LOG_INFO("--------QueAdEvent(AdID=%d,EventID=%d,EventsQueWrite=%d)--------", AdID, EventID, EventsQueWrite);
	EventQue *e = &Events[EventsQueWrite];
	CHECK(e->EventIsFree, false, "No free event slots available!");
	CHECK((serviceID >= 0) && (serviceID < 0x7FFF), false, "Expecting an serviceID bigger than 0 and smaller than 0x7FFF and received: %d", serviceID);
	CHECK((EventID >= 0) && (EventID < 0x7FFF), false, "Expecting an EventID bigger than 0 and smaller than 0x7FFF and received: %d", EventID);
	LOG_EVENT("Add ServiceEvent event [serviceID:%d, EventdID:%d]", serviceID, EventID);
	e->UintValue = (serviceID << 16) | EventID;
	e->EventType = EVENT_SERVICE;
	e->EventIsFree = false;
	EventsQueWrite = (EventsQueWrite + 1) % (MAX_EVENTS_IN_QUE);
	return true;
}
bool AppContextData::GoToNextScene()
{
	// daca s-a mai dat la un moment dat - dar inca nu s-a rulat comanda - astept
	// in mod normal daca s-a mai dat o data GoToNextScene avem
	// * NextSceneID = INVALID_SCENE_ID
	// * NewSceneID - puncteaza la o scena valabila
	if ((NextSceneID == INVALID_SCENE_ID) && (NewSceneID<MAX_SCENES) && (NewSceneID != INVALID_SCENE_ID) && (Scenes[NewSceneID] != NULL))
		return true;
	if ((NextSceneID >= MAX_SCENES) || (Scenes[NextSceneID] == NULL))
	{
		LOG_ERROR("Invalid NextSceneId=%d or non existing scene for that scene id", NextSceneID);
		CHECK(Scenes[GAC_SCENES_MAIN] != NULL, false, "Invalid NextScene ID: %d - and MainScene does not exists !", NextSceneID);
		// fallback pe main
		NextSceneID = GAC_SCENES_MAIN;
	}
	NewSceneID = NextSceneID;
	NextSceneID = INVALID_SCENE_ID;
	return true;
}
bool AppContextData::SetNextScene(unsigned int sceneID)
{
	CHECK((sceneID<MAX_SCENES) && (Scenes[sceneID]!=NULL),false,"Invalid sceneID=%d or non existing scene for that scene id",sceneID);
	NextSceneID = sceneID;
	return true;
}
bool AppContextData::ChangeScene(unsigned int sceneID)
{
	CHECK((sceneID<MAX_SCENES) && (Scenes[sceneID]!=NULL),false,"Invalid sceneID=%d or non existing scene for that scene id",sceneID);
	NewSceneID = sceneID;
	//ChangeSceneFlags = Flags;
	return true;
}
bool AppContextData::ReloadCurrentScene()
{
	CHECK((CurrentSceneID<MAX_SCENES) && (Scenes[CurrentSceneID]!=NULL),false,"Invalid CurrentSceneID=%d or non existing scene for that scene id",CurrentSceneID);
	NewSceneID = CurrentSceneID;
	//ChangeSceneFlags = Flags;
	return true;
}
bool AppContextData::ChangeSceneUsingCountersGroup(unsigned int sceneID, GApp::UI::CountersGroup *grp, unsigned int gotoSceneID)
{
	CHECK(grp != NULL, false, "Expecting a valid group counter !");
	if (grp->Update() == NULL) {
		CHECK(ChangeScene(sceneID), false, "Failed to change scene to id: %d", sceneID);
	}
	else {
		CHECK(SetNextScene(sceneID), false, "Failed to set next scene to id: %d", sceneID);
		CHECK(ChangeScene(gotoSceneID), false, "Failed to change scene to id: %d", gotoSceneID);
	}
	return true;
}
bool AppContextData::ReloadCurrentSceneUsingCountersGroup(GApp::UI::CountersGroup *grp, unsigned int gotoSceneID)
{
	CHECK(grp != NULL, false, "Expecting a valid group counter !");
	if (grp->Update() == NULL) {
		CHECK(ReloadCurrentScene(), false, "Failed to reload current scene !");
	}
	else {
		CHECK(SetNextScene(CurrentSceneID), false, "Failed to set next scene to id: %d", CurrentSceneID);
		CHECK(ChangeScene(gotoSceneID), false, "Failed to change scene to id: %d", gotoSceneID);
	}
	return true;
}
bool AppContextData::ChangeSceneUsingCountersGroup(unsigned int sceneID, GApp::UI::CountersGroup *grp)
{
	CHECK(grp != NULL, false, "Expecting a valid group counter !");
	GApp::UI::Counter *cnt = grp->Update();
	if (cnt == NULL) {
		CHECK(ChangeScene(sceneID), false, "Failed to change scene to id: %d", sceneID);
	}
	else {
		
		if ((cnt->GoToSceneID < MAX_SCENES) && (Scenes[cnt->GoToSceneID] != NULL))
		{
			CHECK(SetNextScene(sceneID), false, "Failed to set next scene to id: %d", sceneID);
			CHECK(ChangeScene(cnt->GoToSceneID), false, "Failed to change scene to id: %d", cnt->GoToSceneID);
		} else {
			CHECK(ChangeScene(sceneID), false, "Failed to change scene to id: %d", sceneID);
		}
	}
	return true;
}
bool AppContextData::ReloadCurrentSceneUsingCountersGroup(GApp::UI::CountersGroup *grp)
{
	CHECK(grp != NULL, false, "Expecting a valid group counter !");
	GApp::UI::Counter *cnt = grp->Update();
	if (cnt == NULL) {
		CHECK(ReloadCurrentScene(), false, "Failed to reload current scene !");
	}
	else {
		if ((cnt->GoToSceneID < MAX_SCENES) && (Scenes[cnt->GoToSceneID] != NULL))
		{
			CHECK(SetNextScene(CurrentSceneID), false, "Failed to set next scene to id: %d", CurrentSceneID);
			CHECK(ChangeScene(cnt->GoToSceneID), false, "Failed to change scene to id: %d", cnt->GoToSceneID);
		} else {
			CHECK(ReloadCurrentScene(), false, "Failed to reload current scene !");
		}
	}
	return true;
}
