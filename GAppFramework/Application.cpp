#include "GApp.h"
#ifdef PLATFORM_DEVELOP
	#include "DevelopMode.h"
#endif


#define SETTINGS_MARKER		0x4B4D4447

#define APPLICATION_STATE_INIT						0
#define APPLICATION_STATE_APP_LOADING				1
#define APPLICATION_STATE_APP_LOADING_COMPLETE		2
#define APPLICATION_STATE_SCENE_LOADING				3
#define APPLICATION_STATE_SCENE_LOADING_COMPLETE	4
#define APPLICATION_STATE_RUN						5
#define APPLICATION_STATE_ERROR						6
#define APPLICATION_STATE_QUERY_PAUSE				7
#define APPLICATION_STATE_PAUSED					8
#define APPLICATION_STATE_RESUME					9
#define APPLICATION_STATE_RESUME_RELOAD_CONTEXT		10

#define CTX							((AppContextData*)Context)

//----------------------------------------------------------------------------------------------------------------------------------
// format settings file
// MARKER
// count elemente
// elemente (indexi)
// Buffer date

bool LoadGlobalProfiles(GApp::Resources::Profile* profileList,unsigned int count,bool RunsOnUIThread)
{
	bool foundOne = false;
	CHECK(profileList!=NULL,false,"");
	CHECK(count>0,false,"");
	for (unsigned int tr=0;tr<count;tr++)
	{
		LOG_INFO("Analyzing profile: %d -> Type=%d NamePtr=%p", tr, profileList[tr].ProfileType, profileList[tr].Name);
		if (profileList[tr].ProfileType == GAC_PROFILE_GLOBAL)
		{
			CHECK(profileList[tr].Load(RunsOnUIThread),false,"Unable to load profile %s",profileList[tr].Name);
			foundOne = true;
		}
	}
	CHECK(foundOne,false,"No global profiles found !");
	return true;
}
bool LoadDelayProfiles(GApp::Resources::Profile* profileList, unsigned int count, bool RunsOnUIThread)
{
	CHECK(profileList != NULL, false, "");
	CHECK(count>0, false, "");
	
	for (unsigned int tr = 0; tr<count; tr++)
	{
		CHECK(profileList[tr].Load(RunsOnUIThread, true), false, "Unable to load profile: %s", profileList[tr].Name);
	}
	return true;
}
bool UnloadProfiles(GApp::Resources::Profile* profileList, unsigned int count, unsigned int ProfileType, bool RunsOnUIThread, bool markForDelayLoad)
{
	CHECK(profileList!=NULL,false,"");
	CHECK(count>0,false,"");
	for (unsigned int tr=0;tr<count;tr++)
		if (profileList[tr].ProfileType>=ProfileType)
		{
			CHECK(profileList[tr].Unload(RunsOnUIThread,markForDelayLoad),false,"Unable to unload profile: %s",profileList[tr].Name);
		}
	return true;
}
bool MarkLoadedProfilesForDelayLoad(GApp::Resources::Profile* profileList, unsigned int count)
{
	CHECK(profileList != NULL, false, "");
	CHECK(count>0, false, "");
	for (unsigned int tr = 0; tr<count; tr++)
		profileList[tr].MarkForDelayLoad();
	return true;
}
bool UpdateProfileState(GApp::Resources::Profile* profileList,unsigned int count,GApp::UI::CoreSystem *C,bool computeProgressBar,bool RunsOnUIThread)
{
	CHECK(profileList!=NULL,false,"");
	CHECK(count>0,false,"");
	CHECK(C->DelayLoadProfiles[GAC_PROFILE_GLOBAL]==NULL,false,"Internal error - Global Profile should never be set !");
	
	// mai intai dau jos toate profilele care sunt sub nivelul meu
	unsigned int minimType = GAC_PROFILE_GLOBAL;
	for (unsigned int tr=1;tr<GAC_PROFILETYPE_COUNT;tr++)
	{
		if ((C->DelayLoadProfiles[tr]!=NULL) && (C->DelayLoadProfiles[tr]->IsLoaded() == false))
		{
			minimType = tr;
			break;
		}
	}
	if (computeProgressBar)
	{
		C->ProgressMax = 0;
		C->ProgressValue = 0;
		// calculez progress bar-ul
		for (unsigned int tr=minimType;tr<GAC_PROFILETYPE_COUNT;tr++)
			if (C->DelayLoadProfiles[tr]!=NULL)
			{
				C->ProgressMax+=C->DelayLoadProfiles[tr]->ResourcesCount;
			}
		return true;
	}
	// daca nu gasesc nimik sau toate sunt deja incarcate - nu mai fac nimik
	if (minimType == GAC_PROFILE_GLOBAL)
		return true;

	// dau jos toate profilele de la nivelul la care sunt
	CHECK(UnloadProfiles(profileList,count,minimType,RunsOnUIThread,false),false,"Unable to unload profiles (minimType=%d)",minimType);

	// incarc celelalte profile
	for (unsigned int tr=minimType;tr<GAC_PROFILETYPE_COUNT;tr++)
		if (C->DelayLoadProfiles[tr]!=NULL)
		{
			CHECK(C->DelayLoadProfiles[tr]->Load(RunsOnUIThread),false,"Unable to load profile !");
		}

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void StartApplicationInitThread(void *app)
{	
	AppContextData* acd = (AppContextData*)(((GApp::UI::Application *)app)->Context);
	unsigned int startTime = acd->CoreObject.Api.GetSystemTickCount();
	if (((GApp::UI::Application *)app)->StartApplicationInitThread())
	{
		if (acd->OS->Data.SplashScreenMinimalWaitTime>0)
		{
			unsigned int dif = acd->CoreObject.Api.GetSystemTickCount() - startTime;
			if (dif<(acd->OS->Data.SplashScreenMinimalWaitTime*1000))
			{
				acd->CoreObject.Api.Sleep(acd->OS->Data.SplashScreenMinimalWaitTime*1000-dif);
			}
		}
		LOG_INFO("Application was loaded !");
		// actualizez informatia si in codul main
		acd->CoreObject.ThreadRequest.Create();
		acd->CoreObject.ThreadRequest.SendCommand(GAC_THREADCMD_APP_NOTIFY_APP_INITED);
		acd->CoreObject.ThreadRequest.Wait();
		acd->ApplicationInitedSuccesifully = true;
		
		acd->ApplicationState = APPLICATION_STATE_APP_LOADING_COMPLETE;
	} else {
		acd->ApplicationState = APPLICATION_STATE_ERROR;
	}
}
void StartApplicationResumeThread(void *app)
{
	AppContextData* acd = (AppContextData*)(((GApp::UI::Application *)app)->Context);
	GApp::Resources::Profile* profileList;
	unsigned int count,tr;
	profileList = acd->CoreObject.Resources->GetProfilesList();
	count = acd->CoreObject.Resources->GetProfilesCount();
	acd->CoreObject.ProgressMax = 0;
	acd->CoreObject.ProgressValue = 0;

	while (true)
	{
		for (tr = 0; tr < count; tr++)
		{
			if (profileList[tr].IsDelayedProfile())
			{
				acd->CoreObject.ProgressMax += profileList[tr].ResourcesCount;
			}
		}
		for (tr = 0; tr < count; tr++)
		{
			if (profileList[tr].IsDelayedProfile())
			{
				CHECKBK(profileList[tr].Load(false, true), "Unable to load profile: %s", profileList[tr].Name);
			}
		}
		if (tr < count)
			break;
		LOG_INFO("All profiles restored !");
		// incerc sa pornesc si sunetul
		acd->CoreObject.ThreadRequest.Create();
		acd->CoreObject.ThreadRequest.SendCommand(GAC_THREADCMD_APP_PLAYBACKGROUNDMUSIC);
		acd->CoreObject.ThreadRequest.Wait();
		CHECKBK(acd->CoreObject.ThreadRequest.Result.BoolValue, "Unable to start background music !");
		acd->ApplicationState = APPLICATION_STATE_APP_LOADING_COMPLETE;
		LOG_INFO("Application state resumed");
		return;
	}
	acd->ApplicationState = APPLICATION_STATE_ERROR;
}
bool CreateSplashScreenProfile(AppContextData *acd)
{
	// incarc logo-ul si creez profilul
	CHECK(acd->LogoBitmap.Create(&acd->CoreObject, acd->OS->Data.SplashScreenLogoWidth, acd->OS->Data.SplashScreenLogoHeight, acd->OS->Data.Width, acd->OS->Data.Height, acd->OS->Data.Width, acd->OS->Data.Height, 0, acd->OS->Data.SplashScreenResourceStart, acd->OS->Data.SplashScreenResourceSize), false, "Unable to create logo");
	CHECK(acd->Logo.Create(&acd->LogoBitmap), false, "Unable to create logo object");
	if (acd->OS->Data.AnimatedSplashScreen)
	{
		if ((acd->MaxTextureSize >= (int)acd->LogoBitmap.Width) && (acd->MaxTextureSize >= (int)acd->LogoBitmap.Height))
		{
			float rapX = (float)acd->OS->Data.Width / (float)acd->OS->Data.AnimatedSplashScreenSpriteWidth;
			float rapY = (float)acd->OS->Data.Height / (float)acd->OS->Data.AnimatedSplashScreenSpriteHeight;
			if (rapY < rapX)
				rapX = rapY;
			rapX = rapX * 0.75f;
			if (rapX < 1.0f)
				rapX = 1.0f;
			//rapX = (int)rapX;
			rapX = 1.0f;
			float w = (float)acd->OS->Data.AnimatedSplashScreenSpriteWidth * rapX;
			float h = (float)acd->OS->Data.AnimatedSplashScreenSpriteHeight * rapX;
			float v_left = ((float)acd->OS->Data.Width - w) / 2.0f;
			float v_top = ((float)acd->OS->Data.Height - h) / 2.0f;
			acd->Logo.SetViewRect(v_left, v_top, v_left + w, v_top + h);
			acd->Logo.SetLayout(GAC_IMAGERESIZEMODE_DOCK, GAC_ALIGNAMENT_CENTER);
		}
		else {
			LOG_ERROR("NOT enough memory for animated splash screen !");
			acd->OS->Data.AnimatedSplashScreen = false;
			// fallback pe ultima poza
			//acd->Logo.
		}
	}
	else {
		float w = acd->CoreObject.Width * acd->OS->Data.SplashScreenViewWidth;
		float h = acd->CoreObject.Height * acd->OS->Data.SplashScreenViewHeight;
		float v_left = ((float)acd->OS->Data.Width - w) / 2.0f;
		float v_top = ((float)acd->OS->Data.Height - h) / 2.0f;
		acd->Logo.SetViewRect(v_left, v_top, v_left + w, v_top + h);
		acd->Logo.SetLayout(GAC_IMAGERESIZEMODE_DOCK, GAC_ALIGNAMENT_CENTER);
	}

#if defined(ENABLE_INFO_LOGGING) || defined(ENABLE_ERROR_LOGGING)
	acd->LogoProfile.Name = "SplashScreenProfile";
#endif
	CHECK(acd->LogoProfile.Create(&acd->CoreObject, GAC_PROFILE_LOCAL, 1), false, "Unable to create logo profile object");
	CHECK(acd->LogoProfile.SetResource(0, &acd->LogoBitmap), false, "");
	CHECK(acd->LogoProfile.Load(true), false, "Unable to load profile logo!");

	return true;
}
void StartSceneLoadingProfilesThread(void *app)
{
	AppContextData* acd = (AppContextData*)(((GApp::UI::Application *)app)->Context);
	unsigned int startTime = acd->CoreObject.Api.GetSystemTickCount();
	if (((GApp::UI::Application *)app)->StartSceneLoadingProfilesThread())
	{
		if (acd->CurrentScene->OnLoadWaitTime<GAC_LOAD_PROFILE_THREAD_REQUEST)
		{
			unsigned int dif = acd->CoreObject.Api.GetSystemTickCount() - startTime;
			if (dif<acd->CurrentScene->OnLoadWaitTime)
			{
				acd->CoreObject.Api.Sleep(acd->CurrentScene->OnLoadWaitTime-dif);
			}
		}
		acd->CurrentScene->OnLoadWaitTime = GAC_SCENELOAD_NOTHINGTOLOAD;
		acd->ApplicationState =  APPLICATION_STATE_SCENE_LOADING_COMPLETE;
	} else {
		acd->ApplicationState = APPLICATION_STATE_ERROR;
	}
}
bool LoadSettingsFromBuffer(GApp::Utils::KeyDB *db,GApp::Utils::Buffer *buf)
{
	CHECK(db!=NULL,false,"");
	CHECK(buf!=NULL,false,"");
	CHECK(buf->GetUInt32(0)==SETTINGS_MARKER,false,"Invalid marker for settings file");
	unsigned int count = buf->GetUInt32(4);
	CHECK((count>0) && (count<0x100000),false,"Invalid (too big) number of records: %d",count);
	CHECK(db->Indexes.Create(sizeof(UInt32)*count),false,"Unable to create record index list for %d records",count);
	CHECK(db->Indexes.Resize(count*sizeof(UInt32)),false,"Unable to resize record index list for %d records",count);
	CHECK(buf->CopyData(8,db->Indexes.Data,count*sizeof(UInt32)),false,"");
	CHECK(8+sizeof(UInt32)*count<buf->GetSize(),false,"");
	CHECK(db->Data.Resize(buf->GetSize()-(8+sizeof(UInt32)*count)),false,"");
	CHECK(db->Data.SetData(0,buf->GetBuffer()+8+sizeof(UInt32)*count,buf->GetSize()-(8+sizeof(UInt32)*count)),false,"");
	CHECK(db->CheckIntegrity(),false,"Check Integrity failed !");

	return true;
}
//=====================================================================================================================================
void GetCounterHashName(char counter_name[32], unsigned int hash)
{
	*(unsigned int*)&counter_name[0] = 0x756F635F;
	*(unsigned int*)&counter_name[4] = 0x7265746E;
	counter_name[8] = '_';
	char* p = &counter_name[9];
	while (hash > 0)
	{
		(*p) = 48 + hash % 10;
		hash = hash / 10;
		p++;
	}
	(*p) = 0;
}
bool SaveFrameworkState(GApp::UI::Application &app, AppContextData &acd)
{
	CHECK(acd.ApplicationInitedSuccesifully, false, "SaveFrameworkState failed - application was not completed loaded !");
	// versiunea curenta
	CHECK(acd.CoreObject.SettingsDB.SetUInt32("__app_version__", acd.OS->Data.AppVersion),false,"Unable to save app versions !");
	// sound status
	CHECK(acd.CoreObject.SettingsDB.SetUInt32("__media_flags__", acd.MediaState), false, "Unable to save sound status !");
	// ads status
	CHECK(acd.CoreObject.SettingsDB.SetBool("__ads_enabled__", acd.CoreObject.AdsEnabled), false, "Unable to save ads enable status");


	// rate scene counter
	//if (acd.CoreObject.AlreadyVoted)
	//{
	//	CHECK(acd.CoreObject.SettingsDB.SetBool("__already_voted__", acd.CoreObject.AlreadyVoted), false, "Unable to save rate state (already voted) !");
	//}
	// salvare countere
	GApp::UI::Counter *gc = acd.CoreObject.Resources->GetCountersList();
	char counter_name[32];
	unsigned int counters_count = acd.CoreObject.Resources->GetCountersCount();
	for (unsigned int tr = 0; tr < counters_count; tr++, gc++)
	{
		if (gc->Hash == 0) // daca nu e persistent
			continue;
		GetCounterHashName(counter_name, gc->Hash);
		CHECK(acd.CoreObject.SettingsDB.SetUInt32(counter_name, gc->EncodeToPersistenValue()), false, "Unable to save counter '%s' status (HashName: %s) !", gc->Name, counter_name);
	}

	return true;
}
void SaveGlobalPersistentVars(GApp::UI::Application &app, AppContextData &acd)
{
	app.OnBeforeSaveState();
	if (acd.CoreObject.GlobalData != NULL)
		acd.CoreObject.GlobalData->__internal__Serialize(true, true);
}
bool SaveScenesSerializableVars(GApp::UI::Application &app, AppContextData &acd)
{
	bool scinit[MAX_SCENES];
	for (int tr = 0; tr < MAX_SCENES; tr++)
	{
		scinit[tr] = false;
		if (acd.Scenes[tr] != NULL)
			scinit[tr] = acd.Scenes[tr]->Initialized;
		if ((acd.Scenes[tr] != NULL) && (acd.Scenes[tr]->Initialized))
		{
			acd.Scenes[tr]->OnBeforeSaveState();
			acd.Scenes[tr]->__internal__Serialize(true, false);
		}
	}
	CHECK(acd.CoreObject.SettingsDB.SetBoolVector("__inited_scenes__", scinit, MAX_SCENES), false, "");
	CHECK(acd.CoreObject.SettingsDB.SetUInt32("__current_scene__", acd.CurrentSceneID), false, "");
	CHECK(acd.CoreObject.SettingsDB.SetUInt32("__next_scene__", acd.NextSceneID), false, "");
	CHECK(acd.CoreObject.SettingsDB.SetBool("__restore_from_snapshot__", true), false, "");
	return true;
}

bool SaveSnapshotState(GApp::UI::Application &app, AppContextData &acd, bool appIsTerminating, const char* fileName)
{
	LOG_INFO("SaveSettings(fileName=%s,appIsTerminating=%d", fileName, appIsTerminating);
	CHECK(acd.ApplicationInitedSuccesifully, false, "SaveSnapshotState failed - application was not completed loaded !");
	
	// apelez OnTerminate din aplicatie
	if (appIsTerminating)
		app.OnTerminate();

	// curat bufferul
	acd.CoreObject.SettingsDB.Clear();
	acd.CoreObject.SettingsDB.WriteEnabled = true;
	while (true)
	{
		// salvez valorrile persistente din Global
		SaveGlobalPersistentVars(app, acd);
	
		// media flags / app framework / etc
		CHECKBK(SaveFrameworkState(app, acd), "Unable to save framework state !");

		// scenele au doar chestii serializabile - daca aplicatia se termina nu e nevoie sa le salvez
		if ((acd.OS->Data.SnapshotType == GAC_SNAPSHOT_TYPE_FULL) && (!appIsTerminating))
		{
			CHECKBK(SaveScenesSerializableVars(app, acd), "Faile to create snapshot state (Full mode)");
		}
		
		acd.CoreObject.SettingsDB.WriteEnabled = false;
		acd.CoreObject.SnapshotSaved = true;
		// all is good salvez in fisier
		LOG_INFO("Saving settings ...");

		CHECKBK(acd.CoreObject.TempBufferBig.Resize(0), "Failed to clean up settings buffer !");
		CHECKBK(acd.CoreObject.TempBufferBig.PushUInt32(SETTINGS_MARKER), "Failed to add settings marker");
		CHECKBK(acd.CoreObject.TempBufferBig.PushUInt32(acd.CoreObject.SettingsDB.GetSize()), "Failed to add settings size");
		CHECKBK(acd.CoreObject.TempBufferBig.PushBuffer(&acd.CoreObject.SettingsDB.Indexes), "Failed to add settings indexes");
		CHECKBK(acd.CoreObject.TempBufferBig.PushBuffer(&acd.CoreObject.SettingsDB.Data), "Failed to create seetings content");

		if (fileName == NULL)
		{
			CHECKBK(acd.OS->fnSaveBufferToLocalStorage("settings.dat", acd.CoreObject.TempBufferBig.GetBuffer(), acd.CoreObject.TempBufferBig.GetSize()), "Failed to create 'Settings.da' file");
		}
		else {
			CHECKBK(acd.OS->fnSaveBufferToLocalStorage(fileName, acd.CoreObject.TempBufferBig.GetBuffer(), acd.CoreObject.TempBufferBig.GetSize()), "Failed to create '%s' file",fileName);
		}
		LOG_INFO("Settings saved succesifully !");
		return true;
	}
	acd.CoreObject.SettingsDB.WriteEnabled = false;
	RETURNERROR(false, "Failed to create a snapshot state !");
}
//=====================================================================================================================================
bool LoadSettings(AppContextData &acd)
{
#ifdef PLATFORM_DEVELOP
	GApp::DevelopMode::DevelopModeOSContext *devOS = (GApp::DevelopMode::DevelopModeOSContext *)acd.OS;
	if (devOS->SpecificData.SettingsSnapshotID <= SETTINGS_SNAPSHOT_ID_NONE)
	{
		LOG_INFO("Current config is set up to NOT load any settings.");
		acd.CoreObject.SettingsDB.Clear();
		return true;
	}
	char settingFileName[32];
	GApp::Utils::String str;
	CHECK(str.Create(settingFileName, 32, true), false, "");

	if (devOS->SpecificData.SettingsSnapshotID == SETTINGS_SNAPSHOT_ID_LAST) {
		CHECK(str.Set("settings.dat"), false, "");
	} else {
		CHECK(str.SetFormat("%08X.snapshot", devOS->SpecificData.SettingsSnapshotID), false, "");
	}
	LOG_INFO("Loading snapshot file: %s", settingFileName);
	if (acd.OS->fnReadBufferFromLocalStorage(str.GetText(), &acd.CoreObject.TempBufferBig))
	{
		if (LoadSettingsFromBuffer(&acd.CoreObject.SettingsDB, &acd.CoreObject.TempBufferBig))
		{
			LOG_INFO("Settings imported OK !");
		}
		else {
			LOG_ERROR("Invalid format or integrity problems for '%s' file!. Reseting the settings ...",str.GetText());
			acd.CoreObject.SettingsDB.Clear();
		}
	}
	else {
		LOG_ERROR("Unable to read 'settings.dat' file from local storeage");
	}
#else
	if (acd.OS->fnReadBufferFromLocalStorage("settings.dat", &acd.CoreObject.TempBufferBig))
	{
		if (LoadSettingsFromBuffer(&acd.CoreObject.SettingsDB, &acd.CoreObject.TempBufferBig))
		{
			LOG_INFO("Settings imported OK !");
		}
		else {
			LOG_ERROR("Invalid format or integrity problems for 'settings.dat' file!. Reseting the settings ...");
			acd.CoreObject.SettingsDB.Clear();
		}
	}
	else {
		LOG_ERROR("Unable to read 'settings.dat' file from local storeage");
	}
#endif
	// listez cheile
	// listez setingurile
#ifdef ENABLE_INFO_LOGGING
	unsigned int k_count = acd.CoreObject.SettingsDB.Len();
	LOG_INFO("Listing Settings: %d keys", k_count);
	LOG_INFO("-----------------------------------------------------------------");
	for (unsigned int k_index = 0; k_index < k_count; k_index++)
	{
		acd.CoreObject.TempString.SetFormat("%4d: ",k_index+1);
		if (acd.CoreObject.SettingsDB.GetKeyInfo(k_index, &(acd.CoreObject.TempString)) == false)
		{
			LOG_ERROR(" % 4d: Unable to read key !", k_index + 1);
		}
		else {
			LOG_INFO("%s", acd.CoreObject.TempString.GetText());
		}
	}
	LOG_INFO("-----------------------------------------------------------------");
#endif
	return true;
}
bool RestoreSnapshotState(GApp::UI::Application &app,AppContextData &acd,unsigned int &StartSceneID,unsigned int &NextSceneID)
{
	LOG_INFO("-------------------------- RESTORING SNAPSHOT --------------------------");

	bool scinit[MAX_SCENES];
	unsigned int curent_scene_id = INVALID_SCENE_ID;
	unsigned int next_scene_id = INVALID_SCENE_ID;
	for (int tr=0;tr<MAX_SCENES;tr++)
		scinit[tr]=false;
	CHECK(acd.CoreObject.SettingsDB.CopyBoolVector("__inited_scenes__",scinit,MAX_SCENES),false,"");
	CHECK(acd.CoreObject.SettingsDB.CopyUInt32("__current_scene__",curent_scene_id),false,"");
	CHECK(acd.CoreObject.SettingsDB.CopyUInt32("__next_scene__", next_scene_id), false, "");
	CHECK(curent_scene_id<MAX_SCENES,false,"");
	if (next_scene_id < MAX_SCENES)
	{
		CHECK(scinit[next_scene_id] == true, false, "Scene '%s' was not inited (NextScene)!", ReflectionSceneNames[next_scene_id]);
	}
	else {
		next_scene_id = INVALID_SCENE_ID;
	}
	CHECK(scinit[curent_scene_id]==true,false,"Scene '%s' was not inited !",ReflectionSceneNames[curent_scene_id]);
	CHECK(acd.Scenes[curent_scene_id] != NULL, false, "Scene '%s' was not defined/created !", ReflectionSceneNames[curent_scene_id]);
	// activez pe rand fiecare scena
	for (unsigned int tr=0;tr<MAX_SCENES;tr++)
	{
		if ((scinit[tr]) && (tr!=curent_scene_id))
		{
			CHECK(acd.Scenes[tr]!=NULL,false,"Scene '%s' was not created/defined !",ReflectionSceneNames[tr]);
			acd.CoreObject.ThreadRequest.Create();
			acd.CoreObject.ThreadRequest.Values[0].PtrValue = acd.Scenes[tr];
			acd.CoreObject.ThreadRequest.SendCommand(GAC_THREADCMD_SCENE_ONINIT);
			acd.CoreObject.ThreadRequest.Wait();
			CHECK(acd.CoreObject.ThreadRequest.Result.BoolValue, false, "Scene '%s' OnInit method failed !", ReflectionSceneNames[tr]);

			//CHECK(acd.Scenes[tr]->OnInit(), false, "Scene '%s' OnInit method failed !", ReflectionSceneNames[tr]);// trebuie rulat pe alt fir
			DEBUGMSG("InitScene|%d|%s", tr, ReflectionSceneNames[tr]);
			LOG_INFO("Initting scene : %s", ReflectionSceneNames[tr]);
			acd.Scenes[tr]->__internal__Deserialize(true,false);
			LOG_INFO("Calling OnRestoreState for scene: %s", ReflectionSceneNames[tr]);
			acd.CoreObject.ThreadRequest.Create();
			acd.CoreObject.ThreadRequest.Values[0].PtrValue = acd.Scenes[tr];
			acd.CoreObject.ThreadRequest.SendCommand(GAC_THREADCMD_SCENE_ONAFTERRESTORESTATE);
			acd.CoreObject.ThreadRequest.Wait();			
			acd.Scenes[tr]->Initialized = true;
		}
	}
	StartSceneID = curent_scene_id;
	NextSceneID = next_scene_id;

#ifdef ENABLE_INFO_LOGGING
	const char *n_s_n = NULL;
	if (NextSceneID < MAX_SCENES)
		n_s_n = ReflectionSceneNames[NextSceneID];
	LOG_INFO("Restoring to scene: %d [%s] (NextSceneID = %d [%s])", StartSceneID, ReflectionSceneNames[StartSceneID], NextSceneID, n_s_n);
#endif
	return true;
}
bool CheckIfNewVersion(GApp::UI::Application *app,AppContextData &acd,unsigned int &StartSceneID)
{
	unsigned int currentVersion = acd.CoreObject.SettingsDB.GetUInt32("__app_version__", 0);
	unsigned int newVersion = acd.OS->Data.AppVersion;
	LOG_INFO("Version -> Current:[%d.%d.%05d] , New:[%d.%d.%05d]", GAC_VERSION_GET_MAJOR(currentVersion), GAC_VERSION_GET_MINOR(currentVersion), GAC_VERSION_GET_BUILD(currentVersion), GAC_VERSION_GET_MAJOR(newVersion), GAC_VERSION_GET_MINOR(newVersion), GAC_VERSION_GET_BUILD(newVersion));
	if (newVersion != currentVersion)
	{
		acd.CoreObject.ThreadRequest.Create();
		acd.CoreObject.ThreadRequest.Values[0].PtrValue = app;
		acd.CoreObject.ThreadRequest.Values[1].UInt32Value = currentVersion;
		acd.CoreObject.ThreadRequest.Values[2].UInt32Value = newVersion;
		acd.CoreObject.ThreadRequest.SendCommand(GAC_THREADCMD_APP_ONNEWUPDATE);
		acd.CoreObject.ThreadRequest.Wait();
		if (currentVersion == 0)
		{
			// prima data cand rulez aplicatia
			if (acd.Scenes[GAC_SCENES_FIRSTTIMERUN] != NULL) {
				StartSceneID = GAC_SCENES_FIRSTTIMERUN;
				LOG_INFO("Switch to FirstTimeRun scene !")
			}
		}
		else {
			// am o versiune noua
			if (acd.Scenes[GAC_SCENES_WHATSNEW] != NULL) {
				StartSceneID = GAC_SCENES_WHATSNEW;
				LOG_INFO("Switch to WhatsNew scene !")
			}
		}
		return true; // avem o versiune noua 
	}
	// nu avem o versiune noue
	return false;
}
void ExecuteThreadLoadingCommand(AppContextData &acd)
{
	switch (acd.CoreObject.ThreadRequest.Command)
	{
		case GAC_THREADCMD_PROFILE_LOADTEXTURES:
			LOG_INFO("Load texture from another thread !");
			((GApp::Resources::Profile *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->LoadTextureToGraphicCard(acd.CoreObject.ThreadRequest);
			break;
		case GAC_THREADCMD_PROFILE_LOADSHADER:
			LOG_INFO("Load shader from another thread: %p => %p (treq = %p), Context=%p,Position=%d,Size=%d", acd.CoreObject.ThreadRequest.Values[0].PtrValue, ((GApp::Resources::Shader *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue)), &acd.CoreObject.ThreadRequest, ((GApp::Resources::Shader *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->CoreContext, ((GApp::Resources::Shader *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->Position, ((GApp::Resources::Shader *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->Size);
			acd.CoreObject.ThreadRequest.Result.BoolValue = ((GApp::Resources::Shader *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->Load();
			acd.CoreObject.ThreadRequest.Finish();
			break;
		case GAC_THREADCMD_PROFILE_UNLOADSHADER:
			LOG_INFO("Unload shader from another thread !");
			acd.CoreObject.ThreadRequest.Result.BoolValue = ((GApp::Resources::Shader *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->Unload();
			acd.CoreObject.ThreadRequest.Finish();
			break;
		case GAC_THREADCMD_PROFILE_UNLOADTEXTURES:
			LOG_INFO("Unload texture from another thread !");
			glDeleteTextures(1, &acd.CoreObject.ThreadRequest.Values[0].UInt32Value);
			acd.CoreObject.ThreadRequest.Result.BoolValue = true;
			acd.CoreObject.ThreadRequest.Finish();
			break;
		case GAC_THREADCMD_PROFILE_LOADSOUND:
			LOG_INFO("Load sound from another thread !");
			acd.CoreObject.ThreadRequest.Result.BoolValue = ((GApp::Resources::Sound *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->Load();
			acd.CoreObject.ThreadRequest.Finish();
			break;
		case GAC_THREADCMD_PROFILE_UNLOADSSOUND:
			LOG_INFO("Unload sound from another thread !");
			acd.CoreObject.ThreadRequest.Result.BoolValue = ((GApp::Resources::Sound *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->Unload();
			acd.CoreObject.ThreadRequest.Finish();
			break;
		case GAC_THREADCMD_APP_ONINIT:
			LOG_EVENT("App::OnInit()");
			if (((GApp::UI::Application *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->OnCreateScenes() == false)
				acd.CoreObject.ThreadRequest.Result.BoolValue = false;
			else
				acd.CoreObject.ThreadRequest.Result.BoolValue = ((GApp::UI::Application *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->OnInit();
			acd.CoreObject.ThreadRequest.Finish();
			break;
		case GAC_THREADCMD_APP_ONACTIVATE:
			LOG_EVENT("App::OnActivate()");
			acd.CoreObject.ThreadRequest.Result.BoolValue = ((GApp::UI::Application *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->OnActivate();
			acd.CoreObject.ThreadRequest.Finish();
			break;
		case GAC_THREADCMD_APP_ONNEWUPDATE:
			LOG_EVENT("App::OnNewUpdate()");
			((GApp::UI::Application *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->OnNewUpdate(acd.CoreObject.ThreadRequest.Values[1].UInt32Value, acd.CoreObject.ThreadRequest.Values[2].UInt32Value);
			acd.CoreObject.ThreadRequest.Finish();
			break;
		case GAC_THREADCMD_APP_PLAYBACKGROUNDMUSIC:
			LOG_INFO("Play background music from another thread !");
			acd.CoreObject.ThreadRequest.Result.BoolValue = acd.CoreObject.AudioInterface.PlayBackgroundMusic();
			acd.CoreObject.ThreadRequest.Finish();
			break;
		case GAC_THREADCMD_APP_NOTIFY_APP_INITED:
			LOG_INFO("Notify that application has been initialized succesifully !");
			acd.CoreObject.Api.ApplicationInitializeCompleted();
			acd.CoreObject.ThreadRequest.Result.BoolValue = true;
			acd.CoreObject.ThreadRequest.Finish();
			break;
		case GAC_THREADCMD_APP_ONAFTERRESTORESTATE:
			LOG_EVENT("App::OnAfterRestoreState()");
			((GApp::UI::Application *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->OnAfterRestoreState();
			acd.CoreObject.ThreadRequest.Finish();
			break;
		case GAC_THREADCMD_SCENE_ONINIT:
			LOG_EVENT("Scene::OnInit() -> called from restore snapshot point");
			acd.CoreObject.ThreadRequest.Result.BoolValue = ((GApp::UI::Scene *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->OnInit();
			acd.CoreObject.ThreadRequest.Finish();
			break;
		case GAC_THREADCMD_SCENE_ONAFTERRESTORESTATE:
			LOG_EVENT("Scene::OnAfterRestoreState() -> called from restore snapshot point");
			((GApp::UI::Scene *)(acd.CoreObject.ThreadRequest.Values[0].PtrValue))->OnAfterRestoreState();
			acd.CoreObject.ThreadRequest.Finish();
			break;
		case GAC_THREADCMD_LOAD_AD:
			LOG_INFO("Loading ad on startup: %p", acd.CoreObject.ThreadRequest.Values[0].PtrValue);
			((GApp::Platform::AdInterface*)acd.CoreObject.ThreadRequest.Values[0].PtrValue)->Load();
			acd.CoreObject.ThreadRequest.Finish();
			break;
	}
	//LOG_INFO("ExecuteThreadLoadingCommand is done (result is %d)", acd.CoreObject.ThreadRequest.Result.BoolValue);
}
void ShowStartupLogo(GApp::UI::Application &app,AppContextData &acd)
{
	//LOG_INFO("ShowStartupLogo(App=%p,Context=%p)", &app, &acd);
	// prepare paint context
	
	acd.CoreObject.Graphics.Prepare();
	if (acd.OS->Data.AnimatedSplashScreen == false) 
	{
		acd.CoreObject.Graphics.Draw(&acd.Logo);
	} else {
		unsigned int w = acd.LogoBitmap.Width / acd.OS->Data.AnimatedSplashScreenSpriteWidth;
		unsigned int x = acd.LogoAnimatedIndex % w;
		unsigned int y = acd.LogoAnimatedIndex / w;
		acd.CoreObject.Graphics.DrawImageResized(&acd.LogoBitmap, 
				(float)(x*acd.OS->Data.AnimatedSplashScreenSpriteWidth), 
				(float)(y*acd.OS->Data.AnimatedSplashScreenSpriteHeight), 
				(float)acd.OS->Data.AnimatedSplashScreenSpriteWidth, 
				(float)acd.OS->Data.AnimatedSplashScreenSpriteHeight, 
				acd.Logo.ViewLeft,
				acd.Logo.ViewTop,
				acd.Logo.ViewRight-acd.Logo.ViewLeft,
				acd.Logo.ViewBottom - acd.Logo.ViewTop,
				NULL);
		acd.LogoAnimatedIndex++;
		if (acd.LogoAnimatedIndex >= acd.OS->Data.AnimatedSplashScreenSpritesCount)
			acd.LogoAnimatedIndex--;
	}
	float left,top,right,bottom;
	left = (acd.CoreObject.Width*0.25f);
	right = (acd.CoreObject.Width*0.75f);
	top = (acd.CoreObject.Height*0.90f);
	bottom = top+10;
	if (acd.CoreObject.ProgressMax>0)
	{
		float rap = GAC_MIN(((float)acd.CoreObject.ProgressValue) / ((float)acd.CoreObject.ProgressMax), 1.0f);
		float progress = acd.CoreObject.Width*(0.25f+0.5f*rap);
		acd.CoreObject.Graphics.DrawRect(left,top,progress,bottom,0xFF0000FF,0xFF0000FF);
		acd.CoreObject.Graphics.DrawRect(left,top,right,bottom,0xFFFFFFFF);
	}
	ExecuteThreadLoadingCommand(acd);
}
void ShowSceneLoading(GApp::UI::Application &app,AppContextData &acd)
{
	// prepare paint context
	acd.CoreObject.Graphics.Prepare();
	float progress = 0.0f;
	if (acd.CoreObject.ProgressMax>0)
	{
		progress = ((float)acd.CoreObject.ProgressValue)/((float)acd.CoreObject.ProgressMax);
		progress = GAC_MAX(0.0f, progress);
		progress = GAC_MIN(1.0f, progress);
	}
	acd.CurrentScene->OnLoadProfiles(progress);
	ExecuteThreadLoadingCommand(acd);
}
void StopAllMusic(AppContextData &acd)
{
	LOG_INFO("Stopping all sounds !");
	acd.CoreObject.AudioInterface.StopBackgroundMusic();
	unsigned int count = acd.CoreObject.Resources->GetSoundsCount();
	GApp::Resources::Sound* sndList = acd.CoreObject.Resources->GetSoundsList();
	if ((sndList == NULL) || (count == 0))
	{
		LOG_INFO("No sounds to stop !");
	}
	else 
	{
		for (unsigned int tr = 0; tr < count; tr++, sndList++)
		{
			if ((*sndList).IsLoaded() == false)
				continue;
			LOG_INFO("Stopping sound with id: %d", (*sndList).SoundID);
			acd.OS->fnStopSound((*sndList).SoundID);
		}
	}
	LOG_INFO("All sounds stopped !");
}
void StopAllGroupCounters(AppContextData &acd)
{
	unsigned int cnt = acd.CoreObject.Resources->GetCountersGroupCount();
	if (cnt == 0)
		return;
	LOG_INFO("Pausing all group counters !");
	GApp::UI::CountersGroup *cg = acd.CoreObject.Resources->GetCountersGroupList();
	for (unsigned int tr = 0; tr < cnt; tr++, cg++)
		cg->PauseTimer();
	LOG_INFO("All group counters paused !");
}
void RestoreAllGroupCounters(AppContextData &acd)
{
	unsigned int cnt = acd.CoreObject.Resources->GetCountersGroupCount();
	if (cnt == 0)
		return;
	LOG_INFO("Resuming all group counters !");
	GApp::UI::CountersGroup *cg = acd.CoreObject.Resources->GetCountersGroupList();
	for (unsigned int tr = 0; tr < cnt; tr++, cg++)
		cg->ResumeTimer();
	LOG_INFO("All group counters resumed !");
}
void StartTimerForAllGroupCounters(AppContextData &acd, unsigned int sceneID)
{
	unsigned int cnt = acd.CoreObject.Resources->GetCountersGroupCount();
	if (cnt == 0) 
		return;
	LOG_INFO("Analyzing timers for %d counter groups (Scene: %s)",cnt,ReflectionSceneNames[sceneID]);
	GApp::UI::CountersGroup *cg = acd.CoreObject.Resources->GetCountersGroupList();
	for (unsigned int tr = 0; tr < cnt; tr++, cg++)
	{
		if (cg->StartTimeMethod == COUNTER_GROUP_STARTTIME_METHOD_ON_EVERY_SCENE)
		{
			LOG_INFO("Starting timer for counter: %s (reason: ON_EVERY_SCENE)", cg->Name);
			cg->StartTimer();
			continue;
		}
		if ((cg->StartTimeMethod == COUNTER_GROUP_STARTTIME_METHOD_ON_SCENE) && (sceneID == cg->StartTimeScene))
		{
			LOG_INFO("Starting timer for counter: %s (reason: SPECIFIC_SCENE)", cg->Name);
			cg->StartTimer();
			continue;
		}
	}
	LOG_INFO("All group counters timers started !");
}
//=====================================================================================================================================
GApp::UI::Application::Application()
{
	Context = NULL;
}
bool GApp::UI::Application::SetScene(unsigned int ID,Scene *scene)
{
	CHECK(scene != NULL,false,"");
	CHECK(ID<MAX_SCENES,false,"");
	CHECK(CTX->Scenes[ID]==NULL,false,"");

	CTX->Scenes[ID] = scene;
	scene->CoreContext = &CTX->CoreObject;
	return true;
}
bool GApp::UI::Application::StartSceneLoadingProfilesThread()
{
	CHECK(UpdateProfileState(CTX->CoreObject.Resources->GetProfilesList(),CTX->CoreObject.Resources->GetProfilesCount(),&CTX->CoreObject,true,false),false,"Unable to update to requested profile !");
	CHECK(UpdateProfileState(CTX->CoreObject.Resources->GetProfilesList(),CTX->CoreObject.Resources->GetProfilesCount(),&CTX->CoreObject,false,false),false,"Unable to update to requested profile !");
	return true;
}
bool GApp::UI::Application::StartApplicationInitThread()
{
	//unsigned int ChangeSceneFlags = 0;
	unsigned int StartSceneID;
	unsigned int NextSceneID;
	unsigned int tr, index;
	char counter_name[64];
	unsigned int counter_value;


	// initializez setarile
	LOG_INFO("Loading settings ....");
	CHECK(LoadSettings(*CTX), false, "Failed to load settings");
	// Restabilesc media state
	CTX->MediaState = CTX->CoreObject.SettingsDB.GetUInt32("__media_flags__", 0x036464);
	LOG_INFO("Media Flags = %d (Sound Volume=%d, Music Volume=%d), SoundEnabled=%d, MusicEnabled=%d, LoopMusic=%d", CTX->MediaState, (CTX->MediaState & 0xFF), ((CTX->MediaState >> 8) & 0xFF), (CTX->MediaState & 0x010000) != 0, (CTX->MediaState & 0x020000) != 0, (CTX->MediaState & 0x040000) != 0);
	CTX->CoreObject.AdsEnabled = CTX->CoreObject.SettingsDB.GetBool("__ads_enabled__", true);
	LOG_INFO("Ads enabled: %d", CTX->CoreObject.AdsEnabled);

	// NU SE POATE SCRIE nimik in settinfgs
	CTX->CoreObject.SettingsDB.WriteEnabled = false;


	LOG_INFO("Initializing resources ...");
	// initializez resursele
	CHECK(CTX->CoreObject.Resources->Init(&CTX->CoreObject), false, "");
	GApp::Resources::Profile *p = CTX->CoreObject.Resources->GetProfilesList();
	unsigned int pCount = CTX->CoreObject.Resources->GetProfilesCount();
	for (tr = 0; tr < pCount; tr++)
	if (p[tr].ProfileType == GAC_PROFILE_GLOBAL)
		CTX->CoreObject.ProgressMax += p[tr].ResourcesCount;

	LOG_INFO("Loading Global Profiles ...");
	// calculez cat e valoarea maxima, incarc profilele generale
	CHECK(LoadGlobalProfiles(CTX->CoreObject.Resources->GetProfilesList(), CTX->CoreObject.Resources->GetProfilesCount(), false), false, "Unable to load global profiles !");
	
	// incarc ad-urile 
	for (tr = 0;; tr++)
	{
		GApp::Platform::AdInterface *ad = CTX->CoreObject.Resources->GetAd(tr);
		if (ad == NULL)
			break;
		if (ad->ShouldLoadOnStartup() == false)
			break;
		CTX->CoreObject.ThreadRequest.Create();
		CTX->CoreObject.ThreadRequest.Values[0].PtrValue = ad;
		CTX->CoreObject.ThreadRequest.SendCommand(GAC_THREADCMD_LOAD_AD);
		CTX->CoreObject.ThreadRequest.Wait();
	}


	// Apelez App::OnInit()
	CTX->CoreObject.ThreadRequest.Create();
	CTX->CoreObject.ThreadRequest.Values[0].PtrValue = this;
	CTX->CoreObject.ThreadRequest.SendCommand(GAC_THREADCMD_APP_ONINIT);
	CTX->CoreObject.ThreadRequest.Wait();
	CHECK(CTX->CoreObject.ThreadRequest.Result.BoolValue, false, "App::OnInit() failed !");

#ifdef PLATFORM_DEVELOP
	// mesajele de debug
	for (tr = 0; tr < MAX_SCENES; tr++)
	{
		if (CTX->Scenes[tr] != NULL)
		{
			DEBUGMSG("NewScene|%d|%s", tr, ReflectionSceneNames[tr]);
		}
	}
#endif


	// initializez counterele
	//CTX->SceneCounters[GAC_SCENE_COUNTER_RATE].Init(CTX->OS->Data.RateInterval, CTX->OS->Data.RateMaxTimes, CTX->Scenes, GAC_SCENES_RATE);
	//CTX->CoreObject.AlreadyVoted = CTX->CoreObject.SettingsDB.GetBool("__already_voted__", false);
	//if ((CTX->SceneCounters[GAC_SCENE_COUNTER_RATE].Enabled) && (CTX->CoreObject.AlreadyVoted))
	//{
	//	CTX->SceneCounters[GAC_SCENE_COUNTER_RATE].Enabled = false;
	//	LOG_INFO("Rate was already called - disabling counter");
	//}
	//CTX->SceneCounters[GAC_SCENE_COUNTER_COMERCIAL].Init(CTX->OS->Data.ComercialInterval, CTX->OS->Data.ComercialMaxTimes, CTX->Scenes, GAC_SCENES_COMERCIAL);
	//LOG_INFO("Rate Counter      (Enabled=%d, Count=%2d, Times=%2d, Interval=%2d, MaxTimes=%2d, Scene=%2d)", CTX->SceneCounters[GAC_SCENE_COUNTER_RATE].Enabled, CTX->SceneCounters[GAC_SCENE_COUNTER_RATE].Count, CTX->SceneCounters[GAC_SCENE_COUNTER_RATE].Times, CTX->SceneCounters[GAC_SCENE_COUNTER_RATE].Interval, CTX->SceneCounters[GAC_SCENE_COUNTER_RATE].MaxTimes, CTX->SceneCounters[GAC_SCENE_COUNTER_RATE].SceneID);
	//LOG_INFO("Comercial Counter (Enabled=%d, Count=%2d, Times=%2d, Interval=%2d, MaxTimes=%2d, Scene=%2d)", CTX->SceneCounters[GAC_SCENE_COUNTER_COMERCIAL].Enabled, CTX->SceneCounters[GAC_SCENE_COUNTER_COMERCIAL].Count, CTX->SceneCounters[GAC_SCENE_COUNTER_COMERCIAL].Times, CTX->SceneCounters[GAC_SCENE_COUNTER_COMERCIAL].Interval, CTX->SceneCounters[GAC_SCENE_COUNTER_COMERCIAL].MaxTimes, CTX->SceneCounters[GAC_SCENE_COUNTER_COMERCIAL].SceneID);

	// incarc counterele
	GApp::UI::Counter *gc = CTX->CoreObject.Resources->GetCountersList();
	unsigned int counters_count = CTX->CoreObject.Resources->GetCountersCount();
	for (index = 0; index < counters_count; index++, gc++)
	{		
		if (gc->Hash != 0) // este persistent
		{
			GetCounterHashName(counter_name, gc->Hash);
			counter_value = CTX->CoreObject.SettingsDB.GetUInt32(counter_name, 0xFFFFFFFF);
			if (counter_value != 0xFFFFFFFF)
				gc->DecodeFromPersistentValue(counter_value);
		}
#ifdef PLATFORM_DEVELOP
		PRINT_GLOBAL_COUNTER(gc);
#endif
	}

	// incarc alarmele
	CTX->Alarms = CTX->CoreObject.Resources->GetAlarmsList();
	CTX->AlarmsCount = CTX->CoreObject.Resources->GetAlarmsCount();
	GApp::UI::Alarm *al = CTX->Alarms;
	GApp::Utils::DateTime dt;
	unsigned int secondsFrom2000;
	if ((CTX->OS->fnGetSystemDateTime(&dt)) && (GApp::UI::Alarm::DateTimeToSecondsFrom2000(&dt, secondsFrom2000)))
	{
		for (index = 0; index < CTX->AlarmsCount; index++, al++)
		{
			//if (gc->Hash != 0) // este persistent
			//{
			//	GetCounterHashName(counter_name, gc->Hash);
			//	counter_value = CTX->CoreObject.SettingsDB.GetUInt32(counter_name, 0xFFFFFFFF);
			//	if (counter_value != 0xFFFFFFFF)
			//		gc->DecodeFromPersistentValue(counter_value);
			//}
#ifdef PLATFORM_DEVELOP
			// index,Enabled
			DEBUGMSG("ALARM|%d|%d|%d", al->Index, al->UniqueID, al->Enabled);
#endif
		}
	}
	else {
		LOG_ERROR("Unable to read date/time correctly. Deactivating alarms !");
		for (index = 0; index < CTX->AlarmsCount; index++, al++)
		{
			al->Enabled = false;
		}
	}


	// App::update persistent data
	if (CTX->CoreObject.GlobalData != NULL)
		CTX->CoreObject.GlobalData->__internal__Deserialize(true,true);

	// App::OnAfterRestoreState()
	CTX->CoreObject.ThreadRequest.Create();
	CTX->CoreObject.ThreadRequest.Values[0].PtrValue = this;
	CTX->CoreObject.ThreadRequest.SendCommand(GAC_THREADCMD_APP_ONAFTERRESTORESTATE);
	CTX->CoreObject.ThreadRequest.Wait();

	// App::OnActivate()
	CTX->CoreObject.ThreadRequest.Create();
	CTX->CoreObject.ThreadRequest.Values[0].PtrValue = this;
	CTX->CoreObject.ThreadRequest.SendCommand(GAC_THREADCMD_APP_ONACTIVATE);
	CTX->CoreObject.ThreadRequest.Wait();
	CHECK(CTX->CoreObject.ThreadRequest.Result.BoolValue, false, "App::OnActivate() failed !");

	// verific ca scena principala sa existe
	StartSceneID = GAC_SCENES_MAIN;

	// verific versiunea
	// daca am un restore dar sunt fac o trecere catre o noua versiune, renunt la restore
	if (CheckIfNewVersion(this, *CTX, StartSceneID) == false)
	{
		// daca sunt dintr-un restore_snapshot
		// doar daca sunt pe modul GAC_SNAPSHOT_TYPE_FULL
		if ((CTX->OS->Data.SnapshotType == GAC_SNAPSHOT_TYPE_FULL) && (CTX->CoreObject.SettingsDB.GetBool("__restore_from_snapshot__")))
		{
			if (RestoreSnapshotState(*this, *CTX, StartSceneID, NextSceneID) == false)
			{
				LOG_ERROR("Error restoring state !");
				StartSceneID = GAC_SCENES_MAIN;
				NextSceneID = INVALID_SCENE_ID;
			}
			else {
				CTX->RestoreApplicationSettings = true;
				//SET_BIT(ChangeSceneFlags, SCENE_FLAGS_RESTORE_STATE);
				LOG_INFO("Previous state restored OK - switching to scene '%s' -> NextSceneID = %d!", ReflectionSceneNames[StartSceneID], NextSceneID);
				if (NextSceneID < MAX_SCENES)
				{
					if (CTX->SetNextScene(NextSceneID) == false)
					{
						LOG_ERROR("Failed to set NextScene to %s", ReflectionSceneNames[NextSceneID]);
						StartSceneID = GAC_SCENES_MAIN;
					}
				}
			}
		}
	}


	// pornesc
	LOG_INFO("Starting scene: %s (%d)", ReflectionSceneNames[StartSceneID], StartSceneID);
	CHECK(CTX->Scenes[StartSceneID]!=NULL,false,"");
	//CHECK(CTX->ChangeScene(StartSceneID,ChangeSceneFlags),false,"");
	CHECK(CTX->ChangeScene(StartSceneID), false, "");
	
	LOG_INFO("Startup application completed !");
	return true;
}
bool GApp::UI::Application::Create(OSContext &initData)
{
	int tr,LastError;
	
	if (Context==NULL)
	{
		CHECK((Context = new AppContextData())!=NULL,false,"");
	}

	CTX->OS = &initData;
	CTX->App = this;
	CTX->ApplicationState = APPLICATION_STATE_INIT;
	CTX->ApplicationInitedSuccesifully = false;
	initData.Data.ApplicationIsClosing = false;

	CTX->TouchFlags = TOUCH_FLAG_CONTROL_SENDEVENT_UP_AND_CLICK|TOUCH_FLAG_SCENE_SENDEVENT_UP_AND_CLICK;
	CTX->TouchBoundaryEnabled = false;
	CTX->CoreObject.Lang = CTX->OS->Data.Lang;
	CTX->CoreObject.AppName = CTX->OS->Data.Name;
	CTX->CoreObject.OSName = CTX->OS->Data.OSName;
	CTX->CoreObject.OSVersion = CTX->OS->Data.OSVersion;
	CTX->CoreObject.AppVersion = CTX->OS->Data.AppVersion;

	CTX->CoreObject.SnapshotSaved = false;
	CTX->CoreObject.AdsEnabled = true;
	CTX->BillingEventCode = -1;
	CTX->LogoAnimatedIndex = 0;
	CTX->AlarmsCount = 0;
	CTX->Alarms = NULL;
	CTX->AlarmCheckUpdateTicks = CTX->OS->Data.AlarmCheckUpdateTicks; // 2 secundde
	CTX->AlarmCheckTimer = CTX->AlarmCheckUpdateTicks; 

	CTX->RestoreApplicationSettings = false;

	CHECK((CTX->OS->Data.Width>10) && (CTX->OS->Data.Width<10000), false, "Invalid Width (%d) - should be between 10 and 10000", CTX->OS->Data.Width);
	CHECK((CTX->OS->Data.Height>10) && (CTX->OS->Data.Height<10000), false, "Invalid Height (%d) - should be between 10 and 10000", CTX->OS->Data.Height);
	CHECK((CTX->OS->Data.DesignWidth>10) && (CTX->OS->Data.DesignWidth<10000), false, "Invalid Design Width (%d) - should be between 10 and 10000", CTX->OS->Data.DesignWidth);
	CHECK((CTX->OS->Data.DesignHeight>10) && (CTX->OS->Data.DesignHeight<10000), false, "Invalid Design Height (%d) - should be between 10 and 10000", CTX->OS->Data.DesignHeight);
	CHECK(CTX->AlarmCheckUpdateTicks > 59, false, "Invalid value for AlarmCheckUpdateTicks - should be bigger than 60 - current value is: %d ", CTX->AlarmCheckUpdateTicks);

	CoreContext = &CTX->CoreObject;
	CTX->MediaState = 0;	
	CTX->CurrentMusicID = -1;
	CTX->CoreObject.AudioInterface.CoreContext = &CTX->CoreObject;
	CTX->CoreObject.AnalyticsObject.CoreContext = &CTX->CoreObject;

	CTX->CoreObject.Width = CTX->OS->Data.Width;
	CTX->CoreObject.Height = CTX->OS->Data.Height;
	CTX->MinDistForClickSquared = (float)((GAC_MIN(CTX->CoreObject.Width, CTX->CoreObject.Height) / 20)*(GAC_MIN(CTX->CoreObject.Width, CTX->CoreObject.Height) / 20));
	CTX->CoreObject.WidthInInch = CTX->OS->Data.WidthInInch;
	CTX->CoreObject.HeightInInch = CTX->OS->Data.HeightInInch;
	CTX->CoreObject.DiagonalInInch = (float)(sqrt((CTX->OS->Data.WidthInInch)*(CTX->OS->Data.WidthInInch) + (CTX->OS->Data.HeightInInch)*(CTX->OS->Data.HeightInInch)));
	if ((CTX->OS->Data.WidthInInch > 0) && (CTX->OS->Data.HeightInInch > 0))
	{
		// consideram de 1.5 CM = 0.590551181 inch
		CTX->CoreObject.FingerSize = (unsigned int)(((CTX->CoreObject.Width) * 0.590551181) / (CTX->CoreObject.WidthInInch));
		unsigned int tmp_r1 = (unsigned int)(((CTX->CoreObject.Height) * 0.590551181) / (CTX->CoreObject.HeightInInch));
		if (tmp_r1 > (CTX->CoreObject.FingerSize))
			CTX->CoreObject.FingerSize = tmp_r1;
		// sanity check
		if (CTX->CoreObject.FingerSize < 0)
			CTX->CoreObject.FingerSize = 0;
	}
	else {
		CTX->CoreObject.FingerSize = 0;
	}
	float _rap1 = ((float)(CTX->OS->Data.Width)) / ((float)(CTX->OS->Data.DesignWidth));
	float _rap2 = ((float)(CTX->OS->Data.Height)) / ((float)(CTX->OS->Data.DesignHeight));
	if (_rap1 < _rap2)
		CTX->CoreObject.ResolutionAspectRatio = _rap1;
	else
		CTX->CoreObject.ResolutionAspectRatio = _rap2;
	

	CHECK(CTX->OS->Data.Resources != NULL, false, "");
	CTX->CoreObject.Resources = CTX->OS->Data.Resources;

	CHECK(CTX->OS->Data.GlobalData != NULL, false, "");
	CTX->CoreObject.GlobalData = CTX->OS->Data.GlobalData;
	CTX->CoreObject.GlobalData->CoreContext = CoreContext;

	// curat lista de scene
	for (tr=0;tr<MAX_SCENES;tr++)
	{
		CTX->Scenes[tr] = NULL;
	}
	CTX->CurrentScene = NULL;
	CTX->CurrentSceneID = CTX->NewSceneID = CTX->NextSceneID = INVALID_SCENE_ID;

	// clean up delay load profile list
	for (tr=0;tr<GAC_PROFILETYPE_COUNT;tr++)
		CTX->CoreObject.DelayLoadProfiles[tr]=NULL;

	// initializez coada de evenimente	
	for (tr=0;tr<MAX_EVENTS_IN_QUE;tr++)
		CTX->Events[tr].EventIsFree = true;
	CTX->EventsQueRead = CTX->EventsQueWrite = 0;

	// initializez touch-urile
	for (tr=0;tr<MAX_TOUCH_POINTERS;tr++)
	{
		CTX->TouchPointers[tr].Event.PointerID = tr;
		CTX->TouchPointers[tr].Event.Type = GAC_TOUCHEVENTTYPE_NONE;
		CTX->TouchPointers[tr].Control = NULL;
	}
	CTX->FocusesControl = NULL;

	CTX->CoreObject.Api.Context = Context;

	CHECK(CTX->OS->Data.SnapshotType != GAC_SNAPSHOT_TYPE_NONE, false, "");

	// initializez constante OpenGL
	CHECK_GLERROR(glGetIntegerv(GL_MAX_TEXTURE_SIZE,&CTX->MaxTextureSize));
	
	LOG_INFO("Application::Create(MaxTextureSize=%d)",CTX->MaxTextureSize);

	// initializez video
	CHECK(CTX->CoreObject.Graphics.Init(CTX->CoreObject.Width,CTX->CoreObject.Height),false,"");
	// LOGO
	CHECK(CreateSplashScreenProfile(CTX), false, "Unable to create splash screen profile !");
	
#ifdef PLATFORM_DEVELOP
	GApp::DevelopMode::Execution::OnAppInit();
#endif
#ifdef PLATFORM_WINDOWS_DESKTOP
	srand(GetTickCount());
#endif

	// pornesc firul de LOGO
	CTX->CoreObject.ProgressValue = 0;
	CTX->CoreObject.ProgressMax = 0;
	CTX->ApplicationState = APPLICATION_STATE_APP_LOADING;
	CHECK(Core.Api.ThreadRun(::StartApplicationInitThread,this),false,"Unable to start application init thread");

	return true;	
}
bool GApp::UI::Application::OnInit()
{
	return true;
}
bool GApp::UI::Application::OnActivate()
{
	return true;
}
void GApp::UI::Application::OnBeforeSaveState()
{
}
void GApp::UI::Application::OnAfterRestoreState()
{
}
void GApp::UI::Application::OnTerminate()
{
	StopAllMusic(*(CTX));
}
bool GApp::UI::Application::OnCreateScenes()
{
	RETURNERROR(false, "Please define at least one scene for the application to work !");
}
void GApp::UI::Application::OnChangeScene(unsigned int oldSceneID,unsigned int newSceneID)
{
}
void GApp::UI::Application::OnNewUpdate(unsigned int oldVersion, unsigned int newVestion)
{
}
bool GApp::UI::Application::CheckAlarms()
{
	GApp::UI::Alarm *a = CTX->Alarms;
	GApp::UI::Alarm *a_end = a + CTX->AlarmsCount;
	GApp::Utils::DateTime dt;
	unsigned int secondsFrom2000, missed, res;
	CHECK(CTX->OS->fnGetSystemDateTime(&dt), false, "Unable to read DateTime");
	CHECK(GApp::UI::Alarm::DateTimeToSecondsFrom2000(&dt, secondsFrom2000), false, "Unable to convert time to seconds from 2000");
	// loghez verificarea
#ifdef PLATFORM_DEVELOP
	// index,Enabled
	DEBUGMSG("ALARMTEST|%04d-%02d-%02d  %2d:%02d:%02d", dt.Year, dt.Month, dt.Day, dt.Hour, dt.Minute, dt.Second);
#endif
	// iterez print alarme
	for (; a < a_end; a++)
	{
		if ((a->Enabled == false) || (secondsFrom2000<a->LastTriggered))
			continue;
		res = a->ShouldTrigger(&dt, secondsFrom2000, missed);
		if (res == GAC_ALARMSTATE_NONE)
			continue;
		if (res & GAC_ALARMSTATE_CLOSED)
			this->OnAlarm(a, 0, GAC_ALARMSTATE_CLOSED);
		if (res & GAC_ALARMSTATE_TRIGGERED)
			this->OnAlarm(a, 1, GAC_ALARMSTATE_TRIGGERED);
#ifdef PLATFORM_DEVELOP
		// index,Enabled
		DEBUGMSG("ALARMEVENT|%04d-%02d-%02d  %2d:%02d:%02d|%d|%d|%d", dt.Year, dt.Month, dt.Day, dt.Hour, dt.Minute, dt.Second,a->Index,missed,res);
#endif
	}
	// all good
	return true;
}
bool GApp::UI::Application::ExecuteIteration()
{
	register AppContextData* AppCTX = CTX;
	// verific daca trebuie sa schimb scena
	if (AppCTX->NewSceneID!=INVALID_SCENE_ID)
	{
		// verific flagurile mai intai
		LOG_INFO("----------------- Switch to scene: %s -----------------", ReflectionSceneNames[AppCTX->NewSceneID]);
		if ((AppCTX->NewSceneID<MAX_SCENES) && (AppCTX->Scenes[AppCTX->NewSceneID]!=NULL))
		{
			// fac copii la scene pentru cazul in care se schimba din OnActivate/OnDeactivate
			unsigned int contextCurrentSceneID = AppCTX->CurrentSceneID;
			unsigned int contextNewSceneID = AppCTX->NewSceneID;
			// dezactivez new scene ID
			AppCTX->NewSceneID = INVALID_SCENE_ID;

			if (AppCTX->CurrentScene!=NULL)
			{
				AppCTX->CurrentScene->OnLeave();
				DEBUGMSG("DeactivateScene|%d|%s", contextCurrentSceneID, ReflectionSceneNames[contextCurrentSceneID]);
				CHECK(UnloadProfiles(AppCTX->CoreObject.Resources->GetProfilesList(), AppCTX->CoreObject.Resources->GetProfilesCount(), GAC_PROFILE_SCENE, true, false), false, "Unable to unload scenes specific profiles for scene: %s", ReflectionSceneNames[contextCurrentSceneID]);
			}
			OnChangeScene(contextCurrentSceneID, contextNewSceneID);

			// trec pe scena curenta
			contextCurrentSceneID = contextNewSceneID;
			AppCTX->CurrentSceneID = contextCurrentSceneID;
			AppCTX->CurrentScene = (AppCTX->Scenes[contextCurrentSceneID]);
			AppCTX->TouchBoundaryEnabled = false; // dezactivez baundery pentrru touch-uri
			// initializez daca e prima data
			if (AppCTX->CurrentScene->Initialized == false)
			{
				LOG_INFO("Initialize scene '%s'", ReflectionSceneNames[contextCurrentSceneID]);
				CHECK(AppCTX->CurrentScene->OnInit(), false, "OnInit() failed in scene: '%s'", ReflectionSceneNames[contextCurrentSceneID]);
				AppCTX->CurrentScene->Initialized = true;
				DEBUGMSG("InitScene|%d|%s", contextCurrentSceneID, ReflectionSceneNames[contextCurrentSceneID]);
			}
			CHECK(AppCTX->InitCurrentScene(), false, "Failed to init parameters for scene: '%s'", ReflectionSceneNames[contextCurrentSceneID]);
			AppCTX->CurrentScene->OnLoadWaitTime = GAC_SCENELOAD_NOTHINGTOLOAD;
			StartTimerForAllGroupCounters(*(AppCTX), AppCTX->CurrentSceneID);
			LOG_INFO("Activate scene '%s'", ReflectionSceneNames[contextCurrentSceneID]);			
			AppCTX->CurrentScene->Enter();
			DEBUGMSG("ActivateScene|%d|%s", contextCurrentSceneID, ReflectionSceneNames[contextCurrentSceneID]);
			//if ((AppCTX->ChangeSceneFlags & SCENE_FLAGS_RESTORE_STATE)!=0) // daca trebuei sa fac restore
			if (AppCTX->RestoreApplicationSettings)
			{
				LOG_INFO("Scene '%s' will be restore from snapshot !", ReflectionSceneNames[contextCurrentSceneID]);
				//REMOVE_BIT(AppCTX->ChangeSceneFlags,SCENE_FLAGS_RESTORE_STATE);
				AppCTX->RestoreApplicationSettings = false;
				AppCTX->CurrentScene->__internal__Deserialize(true,false);
				AppCTX->CurrentScene->OnAfterRestoreState();
			}
		} else {			
			AppCTX->NewSceneID = INVALID_SCENE_ID;
			RETURNERROR(false, "LoopExecute(): Query to change to an invalid scene ID = %d", AppCTX->NewSceneID);
		}
		
	}
	// executia in scena curenta
	if (AppCTX->CurrentScene!=NULL)
	{
		if (AppCTX->CurrentScene->OnLoadWaitTime != GAC_SCENELOAD_NOTHINGTOLOAD)
		{
			if (AppCTX->CurrentScene->OnLoadWaitTime == GAC_SCENELOAD_NOPROGRESSBAR)
			{
				// incarc stuff
				CHECK(UpdateProfileState(AppCTX->CoreObject.Resources->GetProfilesList(),AppCTX->CoreObject.Resources->GetProfilesCount(),&AppCTX->CoreObject,false,true),false,"Unable to update to requested profile !");
				AppCTX->CurrentScene->OnLoadWaitTime = GAC_SCENELOAD_NOTHINGTOLOAD;
			} else {
				// incarc pe un alt fir de executie
				if ((AppCTX->CurrentScene->OnLoadWaitTime & GAC_LOAD_PROFILE_THREAD_REQUEST)!=0)
				{
					REMOVE_BIT(AppCTX->CurrentScene->OnLoadWaitTime,GAC_LOAD_PROFILE_THREAD_REQUEST);
					AppCTX->ApplicationState = APPLICATION_STATE_SCENE_LOADING;
					CHECK(Core.Api.ThreadRun(::StartSceneLoadingProfilesThread,this),false,"Unable to start scene loading thread");
				}
			}						
		} else {
			EventQue tmpEvent;
			// procesez eventuri - daca sunt
			for (unsigned int tr=0;tr<64;tr++)
			{
				EventQue *e = &AppCTX->Events[AppCTX->EventsQueRead];
				if (e->EventIsFree==false)
				{
					// am un event - fac o copie a lui
					memcpy(&tmpEvent, e, sizeof(EventQue));
					e->EventIsFree = true;
					AppCTX->EventsQueRead = (AppCTX->EventsQueRead + 1) % (MAX_EVENTS_IN_QUE);
					AppCTX->ProcessEventForScene(&tmpEvent);
				} else {
					break;
				}
			}
			// procesez billing
			if (AppCTX->BillingEventCode != (-1))
			{
				LOG_INFO("New billing event ! (%d)", AppCTX->BillingEventCode);
				if (AppCTX->CoreObject.InAppBillingItem.Len() == 0)
				{
					LOG_ERROR("Internal error - InAppBillingItem was not set !");
				}
				else {
					if (AppCTX->CurrentScene->OnBillingEvent(AppCTX->CoreObject.InAppBillingItem.GetText(), (GAC_PURCHASE_STATUS_TYPE)AppCTX->BillingEventCode) == false)
					{
						// nu a fost tratat de scena
						this->OnBillingEvent(AppCTX->CoreObject.InAppBillingItem.GetText(), (GAC_PURCHASE_STATUS_TYPE)AppCTX->BillingEventCode);
					}
				}
				AppCTX->CoreObject.InAppBillingItem.Clear();
				AppCTX->BillingEventCode = -1;
			}
			// procesez timerii
			if (AppCTX->CurrentScene->TimersCount > 0)
			{
				unsigned int currentTime = API.GetSystemTickCount();
				unsigned int diff;
				GApp::Utils::SceneTimer *t = AppCTX->CurrentScene->Timers;
				for (unsigned int tr = 0; tr < AppCTX->CurrentScene->TimersCount; tr++, t++)
				{
					if (t->Enabled)
					{
						diff = currentTime - t->LastTime;
						if (diff>=t->Interval)
						{
							t->LastTime = currentTime;
							AppCTX->CurrentScene->OnTimer(t->ID, diff);
						}
					}
				}
			}
			// update
			AppCTX->CurrentScene->OnUpdate();
			// prepare paint context
			AppCTX->CoreObject.Graphics.Prepare();	
			// paint
			AppCTX->CurrentScene->OnPaint();
			// curat clip-urile
			AppCTX->CoreObject.Graphics.Finish();
#ifdef PLATFORM_DEVELOP
			GApp::DevelopMode::Execution::Paint(G, AppCTX);
#endif
		}		
	}
	// alarmele
	if (AppCTX->AlarmsCount != 0)
	{
		AppCTX->AlarmCheckTimer--;
		if (AppCTX->AlarmCheckTimer <= 0)
		{
			AppCTX->AlarmCheckTimer = AppCTX->AlarmCheckUpdateTicks;
			this->CheckAlarms();
		}
	}
	return true;
}
void GApp::UI::Application::LoopExecute()
{
	if (Context == NULL) return;
	register AppContextData* AppCTX = CTX;
	// testez starile de complete
	switch (AppCTX->ApplicationState)
	{
	case  APPLICATION_STATE_SCENE_LOADING_COMPLETE:
		AppCTX->CurrentScene->OnLoadProfilesCompleted();
		AppCTX->ApplicationState = APPLICATION_STATE_RUN;
		break;
	case APPLICATION_STATE_APP_LOADING_COMPLETE:
		// dau jos profilul initial
		if (AppCTX->LogoProfile.Unload(true) == false)
		{
			LOG_ERROR("Unable to unload logo profile !!!");
			AppCTX->ApplicationState = APPLICATION_STATE_ERROR;
		}
		else {
			AppCTX->ApplicationState = APPLICATION_STATE_RUN;
		}
		break;
	case APPLICATION_STATE_QUERY_PAUSE:
		if ((AppCTX->CurrentScene != NULL) && (AppCTX->CurrentScene->Initialized))
			AppCTX->CurrentScene->OnPause();
		StopAllMusic(*(AppCTX));
		StopAllGroupCounters(*(AppCTX));
#ifdef PLATFORM_ANDROID		
		MarkLoadedProfilesForDelayLoad(AppCTX->CoreObject.Resources->GetProfilesList(), AppCTX->CoreObject.Resources->GetProfilesCount());
		//UnloadProfiles(AppCTX->CoreObject.Resources->GetProfilesList(), AppCTX->CoreObject.Resources->GetProfilesCount(),GAC_PROFILE_GLOBAL,true,true);
		//MarkDelayProfiles(AppCTX->CoreObject.Resources->GetProfilesList(), AppCTX->CoreObject.Resources->GetProfilesCount());
		AppCTX->ApplicationState = APPLICATION_STATE_PAUSED;
#endif
#ifdef PLATFORM_DEVELOP
		AppCTX->ApplicationState = APPLICATION_STATE_RUN;
#endif
#ifdef PLATFORM_WINDOWS_DESKTOP
		AppCTX->ApplicationState = APPLICATION_STATE_RUN;
#endif
		break;
	case APPLICATION_STATE_RESUME:
	case APPLICATION_STATE_RESUME_RELOAD_CONTEXT:
#ifdef PLATFORM_ANDROID
		if (AppCTX->ApplicationState == APPLICATION_STATE_RESUME_RELOAD_CONTEXT)
		{
			AppCTX->ApplicationState = APPLICATION_STATE_RUN;
			LOG_INFO("OpenGL context was lost - reload it !");
			if (AppCTX->CoreObject.Graphics.Init(CTX->CoreObject.Graphics.Width,CTX->CoreObject.Graphics.Height)==false)
			{
				LOG_ERROR("Failed to reinitialize the OpenGL Context !");
				AppCTX->ApplicationState = APPLICATION_STATE_ERROR;
			} else {
				if (AppCTX->LogoProfile.Load(true)==false)
				{
					LOG_ERROR("Failed to load splash screen !");
					AppCTX->ApplicationState = APPLICATION_STATE_ERROR;
				} else {
					AppCTX->CoreObject.ProgressValue = 0;
					AppCTX->CoreObject.ProgressMax = 0;
					AppCTX->ApplicationState = APPLICATION_STATE_APP_LOADING;
					if (Core.Api.ThreadRun(::StartApplicationResumeThread,this)==false)
					{
						LOG_ERROR("Failed to start resume thread !");
						AppCTX->ApplicationState = APPLICATION_STATE_ERROR;
					}
				}
			}
		}
		else {
			AppCTX->ApplicationState = APPLICATION_STATE_RUN;
			LOG_INFO("OpenGL context is alive - resuming sound !");
			AppCTX->CoreObject.AudioInterface.PlayBackgroundMusic();
		}
#endif
#ifdef PLATFORM_DEVELOP
		AppCTX->CoreObject.AudioInterface.PlayBackgroundMusic();
		AppCTX->ApplicationState = APPLICATION_STATE_RUN;
#endif
#ifdef PLATFORM_WINDOWS_DESKTOP
		AppCTX->CoreObject.AudioInterface.PlayBackgroundMusic();
		AppCTX->ApplicationState = APPLICATION_STATE_RUN;
#endif
		if ((AppCTX->CurrentScene != NULL) && (AppCTX->CurrentScene->Initialized))
			AppCTX->CurrentScene->OnResume();
		// daca s-a cerut resume - e clar ca va trebui sa resalvez snapshot-ul.
		// este necesar pentru ad-urile interstitiale de la google admob. In acel caz se apeleaza savesnapshot, apoi onResume si cand se termina applicatia nu se mai slaveaza starea.
		AppCTX->CoreObject.SnapshotSaved = false;
		RestoreAllGroupCounters(*(AppCTX));
		break;
	}
	switch (AppCTX->ApplicationState)
	{
		case APPLICATION_STATE_APP_LOADING:
			ShowStartupLogo(*this,*AppCTX);
			return;
		case APPLICATION_STATE_SCENE_LOADING:
			ShowSceneLoading(*this,*AppCTX);
			return;
		case APPLICATION_STATE_RUN:
			if (ExecuteIteration()==false)
			{
				LOG_ERROR("ExecuteIteration failed !");
				CTX->ApplicationState = APPLICATION_STATE_ERROR;
			}
			return;
		case APPLICATION_STATE_ERROR:
			// curat contextul
			AppCTX->CoreObject.Graphics.Prepare();
			break;
		case APPLICATION_STATE_PAUSED:
			break;
		default:
			LOG_ERROR("Internal error - this state (%d) should not have been reached at this point !",CTX->ApplicationState);
			AppCTX->ApplicationState = APPLICATION_STATE_ERROR;
			return;
	};
}
void GApp::UI::Application::ProcessSystemEvent(unsigned int SysEventCode,unsigned int i1,unsigned int i2,float f1,float f2)
{
	//LOG_INFO("--------ProcessSystemEvent(Context=%p,SysEventCode=%d,i1=%d,i2=%d,f1=%f,f2=%f)--------", Context, SysEventCode, i1,i2,f1,f2);

	if (Context == NULL) return;
	switch (SysEventCode)
	{
		case SYSEVENT_PAUSE:
			LOG_EVENT("Pause called ! (Current application state is %d)", CTX->ApplicationState);
			CTX->ApplicationState = APPLICATION_STATE_QUERY_PAUSE;
#ifdef PLATFORM_ANDROID
			while (CTX->ApplicationState == APPLICATION_STATE_QUERY_PAUSE) {}		
#endif
			break;
		case SYSEVENT_RESUME:
			
			if (i1 == 0)
			{
				LOG_EVENT("Resume called ! (Status=%d) -> context not lost", i1);
				CTX->ApplicationState = APPLICATION_STATE_RESUME;
			}
			else
			{
				LOG_EVENT("Resume called ! (Status=%d) -> context was lost (it will be rebuild)", i1);
				CTX->ApplicationState = APPLICATION_STATE_RESUME_RELOAD_CONTEXT;
			}
			break;	
		case SYSEVENT_SAVE_STATE:
			LOG_EVENT("SaveState");
			if (SaveSnapshotState(*this,*CTX,false,NULL)==false)
			{
				LOG_ERROR("Unable to save state !");
			} 
			CTX->CoreObject.SettingsDB.WriteEnabled = false;
			break;
		case SYSEVENT_TERMINATE:			
			LOG_EVENT("Terminate");
			// daca s-a salvat deja snapshot-ul de la SaveState - nu mai apelez nimic din OnTerminate
			if (CTX->CoreObject.SnapshotSaved == false)
			{
				if (SaveSnapshotState(*this, *CTX, true,NULL) == false)
				{
					LOG_ERROR("Unable to save state before app termination !");
				}
				CTX->CoreObject.SettingsDB.WriteEnabled = false;
			}
			else {
				LOG_INFO("SaveSnapshot called already - Terminate will not be called !");
			}
			// distrug o serie de obiecte care nu mai sunt necesare
			//CTX->OS->CloseOSHandles(CTX->OS);
			CTX->OS->Data.ApplicationIsClosing = true;
			CTX->CoreObject.ThreadRequest.Finish();			
			//LOG_INFO("Context is destroyed !!!");
			Context = NULL;
			break;
		case SYSEVENT_TOUCH_UP:
			CTX->QueTouchUpEvent(i1,f1,f2,i2);
			break;
		case SYSEVENT_TOUCH_DOWN:
			CTX->QueTouchDownEvent(i1,f1,f2,i2);
			break;
		case SYSEVENT_TOUCH_MOVE:
			CTX->QueTouchMoveEvent(i1,f1,f2,i2);
			break;
		case SYSEVENT_KEY_DOWN:
			CTX->QueKeyEvent(i1 | GAC_KEYBOARD_FLAG_DOWN);
			break;
		case SYSEVENT_KEY_UP:
			CTX->QueKeyEvent(i1);
			break;
		case SYSEVENT_BILLING:
			CTX->BillingEventCode = i1;
			break;
		case SYSEVENT_ADVERTISING:
			CTX->QueAdEvent(i1, i2);
			break;
		case SYSEVENT_SERVICES:
			CTX->QueServiceEvent(i1, i2);
			break;
	}
}
