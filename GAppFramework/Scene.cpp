#include "GApp.h"
#ifdef PLATFORM_DEVELOP
	#include "DevelopMode.h"
#endif

class MainSceneControl : public GApp::Controls::GenericControl
{
public:
	MainSceneControl(void * scene);
};
MainSceneControl::MainSceneControl(void *scene) : GApp::Controls::GenericControl(this, "x:0%,y:0%,w:100%,h:100%,a:lt")
{
	SET_BIT(Flags, GAC_CONTROL_FLAG_MAIN);
}
//=================================================================================
GApp::Utils::SceneTimer* GetSceneTImerWithID(GApp::Utils::SceneTimer* timers, unsigned int count, unsigned int ID)
{
	for (unsigned int tr = 0; tr < count; tr++,timers++)
	{
		if (timers->ID == ID)
			return timers;
	}
	return NULL;
}
void PaintControls(GApp::Controls::GenericControl *control)
{
	if ((control->Flags & (GAC_CONTROL_FLAG_VISIBLE|GAC_CONTROL_FLAG_OUTSIDE_PARENT_RECT))!=GAC_CONTROL_FLAG_VISIBLE)
		return;
	((GApp::UI::CoreSystem *)control->CoreContext)->Graphics.SetClip(control->Layout.ViewLeft,control->Layout.ViewTop,control->Layout.ViewRight,control->Layout.ViewBottom);
	//glScissor(control->Layout.ViewLeft,control->Layout.ViewTop,control->Layout.ViewRight-control->Layout.ViewLeft,control->Layout.ViewBottom-control->Layout.ViewTop);
	((GApp::UI::CoreSystem*)control->CoreContext)->Graphics.TranslateX = control->Layout.TranslateX;
	((GApp::UI::CoreSystem*)control->CoreContext)->Graphics.TranslateY = control->Layout.TranslateY;
		
	((GApp::UI::CoreSystem*)control->CoreContext)->Graphics.SetCoordinatesType(GAC_COORDINATES_PIXELS);
	control->OnPaint();
	if (control->Controls!=NULL)
	{		
		for (int tr = (int)(control->ControlsCount)-1; tr>=0; tr--)
			if (control->Controls[tr]!=NULL)
				PaintControls(control->Controls[tr]);
	}
}
GApp::Controls::GenericControl* GetControlWithID(GApp::Controls::GenericControl* parent,int ID)
{
	if (parent==NULL)
		return NULL;
	if (parent->ID==ID)
		return parent;
	if (parent->Controls!=NULL)
	{
		GApp::Controls::GenericControl* res;
		for (unsigned int tr=0;tr<parent->ControlsCount;tr++)
		{
			res = GetControlWithID(parent->Controls[tr],ID);
			if (res!=NULL)
				return res;
		}
	}
	return NULL;	
}
GApp::Controls::GenericControl* GetCheckedControl(GApp::Controls::GenericControl* parent, int groupID)
{
	if (parent == NULL)
		return NULL;
	if ((parent->GroupID == groupID) && (parent->IsChecked()))
		return parent;
	if (parent->Controls != NULL)
	{
		GApp::Controls::GenericControl* res;
		for (unsigned int tr = 0; tr<parent->ControlsCount; tr++)
		{
			res = GetCheckedControl(parent->Controls[tr], groupID);
			if (res != NULL)
				return res;
		}
	}
	return NULL;
}
//=================================================================================
GApp::UI::Scene::Scene()
{
	CoreContext = NULL;
	Initialized = false;
	MainControl = NULL;
	OnLoadWaitTime = GAC_SCENELOAD_NOPROGRESSBAR;
	TimersCount = 0;
}
bool GApp::UI::Scene::AddControl(Controls::GenericControl *control)
{
	//LOG_INFO("Add control to scene:%p -> Control=%p", this, control);
	CHECK(CoreContext!=NULL,false,"Context is NULL. Did you added this scene to the App class");
	CHECK(control!=NULL,false,"Control is invalid (NULL)");
	
	
	if (MainControl==NULL)
	{
		CHECK((MainControl = new MainSceneControl(this))!=NULL,false,"Failed to create the MainControl");	
	}
	CHECK(MainControl->AddControl(control),false,"Failed to add control to the MainControl");
	return true;
}
void GApp::UI::Scene::PaintControls()
{
	if (MainControl==NULL)
		return;
#ifdef PLATFORM_DEVELOP
	if (((GApp::DevelopMode::DevelopModeOSContext *)(((AppContextData *)(API.Context))->OS))->SpecificData.ShowControls == false)
		return;
#endif
	GAC_TYPE_COORDINATES coord = G.DefaultCoordinates;
	::PaintControls(MainControl);
	G.SetCoordinatesType(coord);
	G.ClearClip();
	G.ClearTranslate();
}
void GApp::UI::Scene::Enter()
{
	if (MainControl != NULL)
	{
		// toate modal frameurile sunt facute invizible
		for (unsigned int tr = 0; tr < MainControl->ControlsCount; tr++ )
		{
			if ((MainControl->Controls[tr]->Flags & GAC_CONTROL_FLAG_MODAL_FRAME) != 0)
				MainControl->Controls[tr]->SetVisible(false);
		}
	}
	OnEnter();
}
GApp::Controls::GenericControl* GApp::UI::Scene::GetControlWithID(int ID)
{
	GApp::Controls::GenericControl* ctrl;
	ctrl = ::GetControlWithID(MainControl,ID);
	CHECK(ctrl!=NULL,NULL,"There is no control with ID=%d",ID);
	return (ctrl);
}
GApp::Controls::GenericControl* GApp::UI::Scene::GetCheckedControl(int groupID)
{
	return ::GetCheckedControl(MainControl, groupID);
}
GApp::Controls::GenericControl* GApp::UI::Scene::GetRootControl()
{
	if (MainControl==NULL)
	{
		GApp::Controls::GenericControl *mc;
		CHECK((mc = new MainSceneControl(this)) != NULL, NULL, "Failed to create the MainControl");
		MainControl = mc;
	}
	return MainControl;
}
unsigned int GApp::UI::Scene::GetControlsCount()
{
	if (MainControl == NULL)
		return 0;
	return MainControl->GetControlsCount();
}
GApp::Controls::GenericControl* GApp::UI::Scene::GetControl(unsigned int index)
{
	if (MainControl == NULL)
		return NULL;
	unsigned int count = MainControl->GetControlsCount();
	if (index >= count)
		return NULL;
	return MainControl->GetControl(index);
}
bool GApp::UI::Scene::UseProfile(GApp::Resources::Profile *profile)
{
	CHECK(profile!=NULL,false,"Invalid (NULL) profile object");
	CHECK(profile->ProfileType!=GAC_PROFILE_GLOBAL,false,"Global profiles are always loaded and can not be loaded or unloaded dynamically !");
	CHECK(profile->ProfileType<GAC_PROFILETYPE_COUNT,false,"Unkwnow profile type with id = %d (expecting a number smaller than %d)",profile->ProfileType,GAC_PROFILETYPE_COUNT);
	Core.DelayLoadProfiles[profile->ProfileType] = profile;
	if (((AppContextData *)(API.Context))->CurrentScene->OnLoadWaitTime==GAC_SCENELOAD_NOTHINGTOLOAD)
		((AppContextData *)(API.Context))->CurrentScene->OnLoadWaitTime=GAC_SCENELOAD_NOPROGRESSBAR;	
	return true;
}
bool GApp::UI::Scene::EnableProfileLoadingProgressBar(unsigned int ms)
{
	CHECK(ms<GAC_LOAD_PROFILE_THREAD_REQUEST,false,"Invalid value for milliseconds (should be less than 0x08000000)");
	CHECK(((AppContextData *)(API.Context))->CurrentScene->OnLoadWaitTime>=GAC_SCENELOAD_NOPROGRESSBAR,false,"EnableProfileLoadingProgressBar can not be used twice (value=%d)",ms);
	((AppContextData *)(API.Context))->CurrentScene->OnLoadWaitTime = (ms|GAC_LOAD_PROFILE_THREAD_REQUEST);
	return true;
}
bool GApp::UI::Scene::CreateTimer(unsigned int id, unsigned int interval, bool enabled)
{
	GApp::Utils::SceneTimer* tm = GetSceneTImerWithID(Timers, TimersCount, id);
	CHECK(tm == NULL, false, "Timer with id: %u already exists !", id);
	CHECK(TimersCount < MAX_SCENE_TIMERS, false, "There were alread %d timers define in current scene !", MAX_SCENE_TIMERS);
	Timers[TimersCount].ID = id;
	Timers[TimersCount].Interval = interval;
	Timers[TimersCount].LastTime = API.GetSystemTickCount();
	Timers[TimersCount].Enabled = enabled;
	TimersCount++;
	return true;
}
bool GApp::UI::Scene::EnableTimer(unsigned int id)
{
	GApp::Utils::SceneTimer* tm = GetSceneTImerWithID(Timers, TimersCount, id);
	CHECK(tm != NULL, false, "Timer with id: %u doesn not exists !", id);
	if (tm->Enabled == false)
	{
		tm->Enabled = true;
		tm->LastTime = API.GetSystemTickCount();
	}
	return true;
}
bool GApp::UI::Scene::DisableTimer(unsigned int id)
{
	GApp::Utils::SceneTimer* tm = GetSceneTImerWithID(Timers, TimersCount, id);
	CHECK(tm != NULL, false, "Timer with id: %u doesn not exists !", id);
	tm->Enabled = false;
	return true;
}
bool GApp::UI::Scene::SetTimerInterval(unsigned int id, unsigned int newInterval)
{
	GApp::Utils::SceneTimer* tm = GetSceneTImerWithID(Timers, TimersCount, id);
	CHECK(tm != NULL, false, "Timer with id: %u doesn not exists !", id);
	tm->Interval = newInterval;
	return true;
}
bool GApp::UI::Scene::ResetTimer(unsigned int id)
{
	GApp::Utils::SceneTimer* tm = GetSceneTImerWithID(Timers, TimersCount, id);
	CHECK(tm != NULL, false, "Timer with id: %u doesn not exists !", id);
	tm->LastTime = API.GetSystemTickCount();
	return true;
}
bool GApp::UI::Scene::RemoveTimer(unsigned int id)
{
	GApp::Utils::SceneTimer* tm = GetSceneTImerWithID(Timers, TimersCount, id);
	CHECK(tm != NULL, false, "Timer with id: %u doesn not exists !", id);
	unsigned int index = (unsigned int)(tm - Timers);
	CHECK(index < TimersCount, false, "Invalid index (%u) for timer !", index);
	for (unsigned int tr = index + 1; tr < TimersCount; tr++)
		Timers[tr - 1] = Timers[tr];
	TimersCount--;
	return true;
}
void GApp::UI::Scene::ResetAllTimers()
{
	unsigned int cTime = API.GetSystemTickCount();
	for (unsigned int tr = 0; tr < TimersCount; tr++)
	{
		Timers[tr].LastTime = cTime;
	}
}
void GApp::UI::Scene::RemoveAllTimers()
{
	TimersCount = 0;
}
unsigned int GApp::UI::Scene::GetTimersCount()
{
	return TimersCount;
}

bool GApp::UI::Scene::OnInit()
{
	return true;
}
void GApp::UI::Scene::OnEnter()
{
}
void GApp::UI::Scene::OnLeave()
{
}
void GApp::UI::Scene::OnUpdate()
{
}
void GApp::UI::Scene::OnPaint()
{
}
void GApp::UI::Scene::OnPause()
{
}
void GApp::UI::Scene::OnResume()
{
}
void GApp::UI::Scene::OnBeforeSaveState()
{
}
void GApp::UI::Scene::OnAfterRestoreState()
{
}
void GApp::UI::Scene::OnLoadProfiles(float percentage)
{
}
void GApp::UI::Scene::OnLoadProfilesCompleted()
{
}
void GApp::UI::Scene::OnTouchEvent(Controls::TouchEvent *touchEvent)
{
}
void GApp::UI::Scene::OnMouseEvent(Controls::TouchEvent *touchEvent)
{
}
void GApp::UI::Scene::OnControlEvent(Controls::GenericControl *control, GAC_TYPE_EVENTTYPE type)
{
}
void GApp::UI::Scene::OnKeyEvent(Controls::KeyboardEvent *keybEvent)
{
}
void GApp::UI::Scene::__internal__Serialize(bool firstCall, bool onlyPermanent)
{
}
void GApp::UI::Scene::__internal__Deserialize(bool firstCall, bool onlyPermanent)
{
}
bool GApp::UI::Scene::OnBillingEvent(const char* itemName, GAC_PURCHASE_STATUS_TYPE purchaseStatus)
{	
	return false;
}
void GApp::UI::Scene::OnTimer(unsigned int id, unsigned int timeDelta)
{

}
bool GApp::UI::Scene::OnEvent(GApp::UI::FrameworkObject *object, unsigned int eventID)
{
	return false;
}
bool GApp::UI::Scene::OnAdEvent(GApp::Platform::AdInterface *adObject, GAC_ADEVENT_TYPE eventID)
{
	return false;
}
bool GApp::UI::Scene::OnServiceEvent(GAC_SERVICE_TYPE sserviceID, GAC_SERVICES_EVENT_TYPE eventID)
{
	return false;
}