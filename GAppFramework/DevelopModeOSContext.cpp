#include "DevelopMode.h"

using namespace GApp::DevelopMode;

DevelopModeOSContext::DevelopModeOSContext()
{
	this->SpecificData.FaceBookHandlerAvailable = true;
	this->SpecificData.YouTubeHandlerAvailable = true;
	this->SpecificData.InstagramHandlerAvailable = true;
	this->SpecificData.TwitterHandlerAvailable = true;
	this->SpecificData.EnableSounds = true;
	this->SpecificData.BillingServiceAvailable = false;
	this->SpecificData.InAppItems.Create(256);
	this->SpecificData.Currency.Create(256);
	this->SpecificData.mainApp = NULL;

	this->SpecificData.SettingsSnapshotID = SETTINGS_SNAPSHOT_ID_LAST;
	this->SpecificData.BillingServiceAvailable = true;
	this->SpecificData.GooglePlayServicesSignedIn = true;
	this->SpecificData.EnableSecondaryTouch = false;
	this->SpecificData.ShowBoudaryRectangle = true;
	this->SpecificData.SecondaryTouchX = 0.5f;
	this->SpecificData.SecondaryTouchY = 0.5f;
	this->SpecificData.SecondaryTouchPresed = false;
	this->SpecificData.SecondaryTouchColor = 0;
	this->SpecificData.SecondaryTouchAdd = 1;

	this->SpecificData.hDC = NULL;
	this->SpecificData.hRC = NULL;
	this->SpecificData.hWnd = NULL;

	this->SpecificData.MaxTextureSize = 0;
	this->SpecificData.ShowControls = true;
	this->SpecificData.TimeDelta = 0;

	this->Data.Lang = GAC_LANGUAGE_ENGLISH;
	this->Data.OSName = "Windows (Develop)";
	this->Data.OSVersion = 0;


	this->Data.SnapshotType = GAC_SNAPSHOT_TYPE_ONLYGLOBALDATA;
	this->Data.AnimatedSplashScreen = false;
	this->Data.AnimatedSplashScreenSpriteWidth = 128;
	this->Data.AnimatedSplashScreenSpriteHeight = 128;
	this->Data.AnimatedSplashScreenSpritesCount = 64;
	this->Data.WidthInInch = 0;
	this->Data.HeightInInch = 0;
}
void DevelopModeOSContext::ShowSecondaryTouch(GApp::Graphics::GraphicsContext &gContext, float x, float y, bool touch)
{
	unsigned int col1, col2;
	float delta = 0;


	this->SpecificData.SecondaryTouchColor += this->SpecificData.SecondaryTouchAdd * 3;
	if (this->SpecificData.SecondaryTouchColor >= 255)
	{
		this->SpecificData.SecondaryTouchColor = 255;
		this->SpecificData.SecondaryTouchAdd = -this->SpecificData.SecondaryTouchAdd;
	}
	if (this->SpecificData.SecondaryTouchColor <= 0)
	{
		this->SpecificData.SecondaryTouchColor = 0;
		this->SpecificData.SecondaryTouchAdd = -this->SpecificData.SecondaryTouchAdd;
	}


	if (touch) {
		col2 = COLOR_ARGB(255, 255 - this->SpecificData.SecondaryTouchColor, this->SpecificData.SecondaryTouchColor, 0);
		col1 = COLOR_ARGB(255, 0, 0, 0);
		delta = 3.0f;
	}
	else {
		col2 = COLOR_ARGB(this->SpecificData.SecondaryTouchColor, this->SpecificData.SecondaryTouchColor, this->SpecificData.SecondaryTouchColor, this->SpecificData.SecondaryTouchColor);
		col1 = COLOR_ARGB(this->SpecificData.SecondaryTouchColor, 0, 0, 0);
	}

	float pas = (GAC_MIN(gContext.Width, gContext.Height)) / 20.0f;
	float ps2 = pas * 0.85f;
	float pm1 = pas * 0.15f;
	float pm2 = pas * 0.25f;
	x = x * gContext.Width;
	y = y * gContext.Height;
	gContext.DrawLine(x - pas, y, x - pm1, y, col1, 6.0f + delta, NULL);
	gContext.DrawLine(x - ps2, y, x - pm2, y, col2, 2.0f + delta, NULL);
	gContext.DrawLine(x + pas, y, x + pm1, y, col1, 6.0f + delta, NULL);
	gContext.DrawLine(x + ps2, y, x + pm2, y, col2, 2.0f + delta, NULL);
	gContext.DrawLine(x, y - pas, x, y - pm1, col1, 6.0f + delta, NULL);
	gContext.DrawLine(x, y - ps2, x, y - pm2, col2, 2.0f + delta, NULL);
	gContext.DrawLine(x, y + pas, x, y + pm1, col1, 6.0f + delta, NULL);
	gContext.DrawLine(x, y + ps2, x, y + pm2, col2, 2.0f + delta, NULL);
}
void DevelopModeOSContext::ProcessSecondaryTouchKeys(unsigned int Key, bool down)
{
	float x = this->SpecificData.SecondaryTouchX * this->Data.Width;
	float y = this->SpecificData.SecondaryTouchY * this->Data.Height;
	float pas = (GAC_MIN(this->Data.Width, this->Data.Height)) / 30.0f;

	if (!down)
	{
		if (Key == VK_CONTROL) {
			this->SpecificData.SecondaryTouchPresed = false;
			this->SpecificData.mainApp->ProcessSystemEvent(SYSEVENT_TOUCH_UP, 1, GetTickCount(), x, y);
		}
	}
	else {
		switch (Key)
		{
		case VK_LEFT:
			x = GAC_MAX(0, x - pas);
			this->SpecificData.SecondaryTouchX = x / ((float)this->Data.Width);
			if (this->SpecificData.SecondaryTouchPresed)
				this->SpecificData.mainApp->ProcessSystemEvent(SYSEVENT_TOUCH_MOVE, 1, GetTickCount(), x, y);
			break;
		case VK_RIGHT:
			x = GAC_MIN(this->Data.Width, x + pas);
			this->SpecificData.SecondaryTouchX = x / ((float)this->Data.Width);
			if (this->SpecificData.SecondaryTouchPresed)
				this->SpecificData.mainApp->ProcessSystemEvent(SYSEVENT_TOUCH_MOVE, 1, GetTickCount(), x, y);
			break;
		case VK_UP:
			y = GAC_MAX(0, y - pas);
			this->SpecificData.SecondaryTouchY = y / ((float)this->Data.Height);
			if (this->SpecificData.SecondaryTouchPresed)
				this->SpecificData.mainApp->ProcessSystemEvent(SYSEVENT_TOUCH_MOVE, 1, GetTickCount(), x, y);
			break;
		case VK_DOWN:
			y = GAC_MIN(this->Data.Height, y + pas);
			this->SpecificData.SecondaryTouchY = y / ((float)this->Data.Height);
			if (this->SpecificData.SecondaryTouchPresed)
				this->SpecificData.mainApp->ProcessSystemEvent(SYSEVENT_TOUCH_MOVE, 1, GetTickCount(), x, y);
			break;
		case VK_CONTROL:
			if (this->SpecificData.SecondaryTouchPresed == false) {
				this->SpecificData.SecondaryTouchPresed = true;
				this->SpecificData.mainApp->ProcessSystemEvent(SYSEVENT_TOUCH_DOWN, 1, GetTickCount(), x, y);
			}
			break;
		}


	}

}
void DevelopModeOSContext::Paint(GApp::Graphics::GraphicsContext &gContext)
{
	// simulated ads
	this->SpecificData.Ads.Paint(gContext);
	this->SpecificData.Ads.Update();
	// simulated touch
	if (this->SpecificData.EnableSecondaryTouch)
		ShowSecondaryTouch(gContext, this->SpecificData.SecondaryTouchX, this->SpecificData.SecondaryTouchY, this->SpecificData.SecondaryTouchPresed);
	// boundery
	if (this->SpecificData.ShowBoudaryRectangle)
	{
		AppContextData *appContext = (AppContextData*)(this->SpecificData.mainApp->Context);
		if ((appContext != NULL) && (appContext->TouchBoundaryEnabled))
		{
			gContext.DrawExclusionRect(appContext->TouchBoundary.Left, appContext->TouchBoundary.Top, appContext->TouchBoundary.Right, appContext->TouchBoundary.Bottom, 0x80FF0000);
			gContext.DrawRectF(&appContext->TouchBoundary, 0xFFFFFFFF, 0, 4.0f);
			gContext.DrawRectF(&appContext->TouchBoundary, 0xFFFF0000, 0, 2.0f);
		}
	}
	//this->SpecificData.mainApp->
}
void DevelopModeOSContext::Init(InitializationData &initData)
{
	this->Data.Width = initData.Width;
	this->Data.Height = initData.Height;
	this->Data.DesignWidth = initData.DesignWidth;
	this->Data.DesignHeight = initData.DesignHeight;
	this->Data.Resources = initData.Resources;
	this->Data.GlobalData = initData.GlobalData;
	this->Data.AlarmCheckUpdateTicks = initData.AlarmCheckUpdateTicks;
	this->Data.Name = initData.Name;
	this->Data.AppVersion = initData.AppVersion;
	this->Data.SplashScreenResourceStart = initData.SplashScreenResourceStart;
	this->Data.SplashScreenResourceSize = initData.SplashScreenResourceSize;
	this->Data.SplashScreenLogoWidth = initData.SplashScreenLogoWidth;
	this->Data.SplashScreenLogoHeight = initData.SplashScreenLogoHeight;
	this->Data.SplashScreenViewWidth = initData.SplashScreenViewWidth;
	this->Data.SplashScreenViewHeight = initData.SplashScreenViewHeight;
	this->Data.SplashScreenMethod = initData.SplashScreenMethod;
	this->Data.SplashScreenMinimalWaitTime = initData.SplashScreenMinimalWaitTime;
	//this->Data.RateInterval = initData.RateInterval;
	//this->Data.RateMaxTimes = initData.RateMaxTimes;
	//this->Data.ComercialInterval = initData.ComercialInterval;
	//this->Data.ComercialMaxTimes = initData.ComercialMaxTimes;

	this->SpecificData.AvailableResolutions = initData.AvailableResolutions;
	this->SpecificData.AvailableResolutionsCount = initData.AvailableResolutionsCount;
}
int  DevelopModeOSContext::fnGetSystemTicksCount()
{
	return GetTickCount();
}
bool DevelopModeOSContext::fnGetSystemDateTime(GApp::Utils::DateTime *dt)
{
	CHECK(dt != NULL, false, "");	
	SYSTEMTIME tm;
	::GetLocalTime(&tm);
	if (SpecificData.TimeDelta != 0)
	{
		FILETIME ft;
		CHECK(::SystemTimeToFileTime(&tm, &ft), false, "Can not convert system time to file time !");
		ULARGE_INTEGER ul;
		ul.LowPart = ft.dwLowDateTime;
		ul.HighPart = ft.dwHighDateTime;
		//LOG_INFO("BEFORE: fnGetSystemDateTime (hi = %08X, lo = %08X) - %04d-%02d-%02d  %2d:%02d:%02d", ul.HighPart, ul.LowPart,tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond);
		ul.QuadPart += (long long)(((double)SpecificData.TimeDelta) / (100. * 1.E-9));
		ft.dwLowDateTime = ul.LowPart;
		ft.dwHighDateTime = ul.HighPart;
		CHECK(::FileTimeToSystemTime(&ft, &tm), false, "Can not convert from file time to system time");
		//LOG_INFO("AFTER : fnGetSystemDateTime (hi = %08X, lo = %08X) - %04d-%02d-%02d  %2d:%02d:%02d", ul.HighPart, ul.LowPart, tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);
	}
	dt->Year = tm.wYear;
	dt->Month = tm.wMonth;
	dt->Day = tm.wDay;
	dt->Hour = tm.wHour;
	dt->Minute = tm.wMinute;
	dt->Second = tm.wSecond;
	dt->DayOfWeek = tm.wDayOfWeek;
	return true;
}
void DevelopModeOSContext::fnClose()
{
	PostMessage(this->SpecificData.hWnd, WM_QUIT, 0, 0);
}
void DevelopModeOSContext::fnQueryNewAd( int adID)
{
	LOG_INFO("Query new ad for id: %d", adID);
}
void DevelopModeOSContext::fnSetAdVisibility( int adID, bool visible)
{
	LOG_INFO("SetAdVisibility: %d -> %d", adID, visible);
	this->SpecificData.Ads.SetVisible(adID, visible);
}
bool DevelopModeOSContext::fnIsAdVisible( int adID)
{
	return this->SpecificData.Ads.IsVisible(adID);
}
bool DevelopModeOSContext::fnIsAdLoaded( int adID)
{
	return this->SpecificData.Ads.IsLoaded(adID);
}
void DevelopModeOSContext::fnOpenAppHandler(unsigned int handlerID, const char *data)
{
	LOG_INFO("Open AppHandler with ID: %d => %s", handlerID, data);
}
bool DevelopModeOSContext::fnIsAppHandlerAvailable(unsigned int handlerID)
{
	LOG_INFO("Open AppHandler with ID: %d", handlerID);
	switch (handlerID)
	{
	case GAC_APP_HANDLER_FACEBOOK_PAGE: return this->SpecificData.FaceBookHandlerAvailable;
	case GAC_APP_HANDLER_INSTAGRAM_PAGE: return this->SpecificData.InstagramHandlerAvailable;
	case GAC_APP_HANDLER_TWITTER_PAGE: return this->SpecificData.TwitterHandlerAvailable;
	case GAC_APP_HANDLER_YOUTUBE_CHANNAL: return this->SpecificData.YouTubeHandlerAvailable;
	}

	RETURNERROR(false, "IsAppHandlerAvailable (handlerID=%d) is not implemented in develop mode !", handlerID);
}
void DevelopModeOSContext::fnOpenBrowser(const char *url)
{
	LOG_INFO("OpenBrowser(%s)", url);
}
void DevelopModeOSContext::fnOpenApplicationMarketPage()
{
	LOG_INFO("OpenApplicationMarketPage()");
}
void DevelopModeOSContext::fnRateApp()
{
	LOG_INFO("RateApp()");
}
void DevelopModeOSContext::fnOpenDeveloperMarketPage()
{
	LOG_INFO("OpenDeveloperMarketPage()");
}
bool DevelopModeOSContext::fnReadBufferFromResources(unsigned int pos, unsigned char *Buffer, unsigned int size)
{
	CHECK(this->SpecificData.ResourceAssetHandle != INVALID_HANDLE_VALUE, false, "");
	SetFilePointer(this->SpecificData.ResourceAssetHandle, pos, NULL, FILE_BEGIN);
	DWORD sz;
	CHECK(ReadFile(this->SpecificData.ResourceAssetHandle, Buffer, size, &sz, NULL), false, "");
	CHECK(size == sz, false, "");
	return true;
}
bool DevelopModeOSContext::fnSaveBufferToLocalStorage(const char* fileName, const void *Buffer, unsigned int BufferSize)
{
	CHECK(Buffer != NULL, false, "");
	CHECK(fileName != NULL, false, "");
	CHECK(BufferSize>0, false, "");

	char temp[2048];
	memset(temp, 0, 2048);
	GetModuleFileNameA(NULL, temp, 2000);
	int last = -1;
	for (int tr = 0; temp[tr] != 0; tr++)
	if (temp[tr] == '\\')
		last = tr;
	last += 1;
	memcpy(&temp[last], fileName, GApp::Utils::String::Len(fileName) + 1);
	LOG_INFO("SaveBufferToLocalStorage(%s,%d)", temp, BufferSize);

	HANDLE hFile = CreateFileA(temp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	CHECK(hFile != INVALID_HANDLE_VALUE, false, "");
	while (true)
	{
		DWORD nrWrite;
		CHECKBK(WriteFile(hFile, Buffer, BufferSize, &nrWrite, NULL) != FALSE, "");
		CHECKBK(nrWrite == BufferSize, "");
		CloseHandle(hFile);
		return true;
	}
	CloseHandle(hFile);
	return false;
}
bool DevelopModeOSContext::fnReadBufferFromLocalStorage(const char* fileName, GApp::Utils::Buffer *buffer)
{
	CHECK(buffer != NULL, false, "");
	CHECK(fileName != NULL, false, "");

	char temp[2048];
	memset(temp, 0, 2048);
	GetModuleFileNameA(NULL, temp, 2000);
	int last = -1;
	for (int tr = 0; temp[tr] != 0; tr++)
	if (temp[tr] == '\\')
		last = tr;
	last += 1;
	memcpy(&temp[last], fileName, GApp::Utils::String::Len(fileName) + 1);
	HANDLE hFile = CreateFileA(temp, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	while (true)
	{
		DWORD nrRead;
		DWORD size;
		size = GetFileSize(hFile, NULL);
		CHECKBK(size>0, "");
		CHECKBK(buffer->Resize(size), "");
		CHECKBK(ReadFile(hFile, buffer->GetBuffer(), size, &nrRead, NULL) != FALSE, "");
		CHECKBK(nrRead == size, "");
		CloseHandle(hFile);
		return true;
	}
	CloseHandle(hFile);
	return false;
}
void DevelopModeOSContext::fnPlaySound(int soundID, int volume, bool loop)
{
	LOG_INFO("PlaySound: [SoundID=%d, Volume=%d, loop=%d] (EnableSounds = %d)", soundID, volume, loop, this->SpecificData.EnableSounds);
	if (!this->SpecificData.EnableSounds) return;
	if ((soundID >= 0) && (soundID < MAX_SOUND_OBJECTS))
	{
		this->SpecificData.BassSound[soundID].SetLoop(loop);
		this->SpecificData.BassSound[soundID].SetVolume(((float)volume) / 100.0f);
		this->SpecificData.BassSound[soundID].Play();
	}
}
void DevelopModeOSContext::fnStopSound(int soundID)
{
	LOG_INFO("StopSound: [SoundID=%d]", soundID);
	if (!this->SpecificData.EnableSounds) return;
	if ((soundID >= 0) && (soundID < MAX_SOUND_OBJECTS))
		this->SpecificData.BassSound[soundID].Pause();
}
void DevelopModeOSContext::fnSetSoundState(int soundID, int volume, bool loop)
{
	LOG_INFO("SetSoundState: [SoundID=%d, Volume=%d, loop=%d]", soundID, volume, loop);
	if (!this->SpecificData.EnableSounds) return;
	if ((soundID >= 0) && (soundID < MAX_SOUND_OBJECTS))
	{
		this->SpecificData.BassSound[soundID].SetLoop(loop);
		this->SpecificData.BassSound[soundID].SetVolume(((float)volume) / 100.0f);
	}
}
bool DevelopModeOSContext::fnLoadSound(int soundID, unsigned int pos, unsigned int size)
{
	LOG_INFO("LoadSound: [SoundID=%d] -> Pos:%d, Size:%d", soundID, pos, size);
	if (!this->SpecificData.EnableSounds) 
		return true;
	CHECK((soundID >= 0) && (soundID < MAX_SOUND_OBJECTS), false, "");
	CHECK(size>0, false, "");
	unsigned char *mem = new unsigned char[size];
	CHECK(mem != NULL, false, "");
	while (true)
	{
		CHECKBK(this->fnReadBufferFromResources(pos, mem, size), "");
		CHECK(this->SpecificData.BassSound[soundID].Create(mem, size), false, "");
		//delete mem;
		return true;
	}
	delete mem;
	return false;
}
void DevelopModeOSContext::fnUnLoadSound(int soundID)
{
	LOG_INFO("UnLoadSound: [SoundID=%d]", soundID);
	if (!this->SpecificData.EnableSounds) return;
	if ((soundID >= 0) && (soundID < MAX_SOUND_OBJECTS))
		this->SpecificData.BassSound[soundID].Close();
}
void DevelopModeOSContext::fnThreadWait(unsigned int milliseconds)
{
	::Sleep(milliseconds);
}
void DevelopModeOSContext::fnExecuteOsCommand(const char* command, GApp::Utils::String *result)
{
	LOG_INFO("ExecuteOsCommand(%s)", command);
}
DWORD WINAPI WindowsThreadProc(LPVOID lpParameter)
{
	void(*RunFunction) (void* ThreadObject);
	void*					ThreadObject;
	GApp::UI::OSContext*	OS;
	HANDLE					hSync;

	RunFunction = ((ThreadExecutionContext*)lpParameter)->RunFunction;
	ThreadObject = ((ThreadExecutionContext*)lpParameter)->ThreadObject;
	hSync = ((ThreadExecutionContext*)lpParameter)->SyncEvent;
	OS = ((ThreadExecutionContext*)lpParameter)->OS;

	SetEvent(hSync);
	RunFunction(ThreadObject);

	// clean up thread data
	((DevelopModeOSContext*)OS)->SpecificData.hThread = NULL;
	return 0;
}
bool DevelopModeOSContext::fnThreadRun(void(*runFunction)(void *threadObject), void*  threadObject)
{
	CHECK(runFunction != NULL, false, "Thread run function should not be NULL!");
	ThreadExecutionContext tec;
	tec.RunFunction = runFunction;
	tec.ThreadObject = threadObject;
	tec.SyncEvent = NULL;
	tec.OS = this;
	CHECK((tec.SyncEvent = ::CreateEventA(NULL, TRUE, FALSE, NULL)) != NULL, false, "Unable to create synchronization event !");
	while (true)
	{
		this->SpecificData.hThread = ::CreateThread(NULL, 0, WindowsThreadProc, &tec, 0, NULL);
		CHECKBK(this->SpecificData.hThread != NULL, "Failed to create thread: Error Code = %08X", GetLastError());
		::WaitForSingleObject(tec.SyncEvent, INFINITE);
		CloseHandle(tec.SyncEvent);
		return true;
	}
	CloseHandle(tec.SyncEvent);
	return false;
}
bool DevelopModeOSContext::fnSendBuyRequestForItem(const char* item)
{
	this->Data.LastPurchaseInfo.Clear();
	CHECK(item != NULL, false, "Invalid (NULL) item !");
	CHECK(this->SpecificData.BillingServiceAvailable, false, "Billing service is unavailable !");

	this->Data.LastPurchaseInfo.Set("Market:Develop emulation\n");
	this->Data.LastPurchaseInfo.Add("ProductID:");
	this->Data.LastPurchaseInfo.Add(item);
	this->Data.LastPurchaseInfo.Add("\n");
	if (MessageBoxA(this->SpecificData.hWnd, item, "Buy ?", MB_YESNO) == IDYES)
	{

		if (this->SpecificData.AllowPurchases)
		{
			LOG_INFO("Billing system: Item purchased !");
			this->Data.LastPurchaseInfo.Add("Status:Completed\n");
			this->SpecificData.mainApp->ProcessSystemEvent(SYSEVENT_BILLING, 0, 0, 0, 0);	// Completed
		}
		else {
			LOG_ERROR("Billing system: Purchase error !");
			this->Data.LastPurchaseInfo.Add("Status:Error\n");
			this->SpecificData.mainApp->ProcessSystemEvent(SYSEVENT_BILLING, 2, 0, 0, 0);	// Error
		}
	}
	else {
		LOG_INFO("Billing system: Purchase canceled !");
		this->Data.LastPurchaseInfo.Add("Status:Canceled\n");
		this->SpecificData.mainApp->ProcessSystemEvent(SYSEVENT_BILLING, 1, 0, 0, 0);	// Canceled
	}
	return true;
}
bool DevelopModeOSContext::fnGetItemPrice(const char* item, GApp::Utils::String &result)
{
	result.Set("");
	CHECK(item != NULL, false, "Invalid (NULL) item !");
	CHECK(this->SpecificData.BillingServiceAvailable, false, "Billing service is unavailable !");
	CHECK(this->SpecificData.InAppItems.Len() > 0, false, "No in-app items defined !");
	int start = 0;
	int l = GApp::Utils::String::Len(item);
	char ch;
	do
	{
		start = this->SpecificData.InAppItems.Find(item, false, start);
		if (start < 0) break;
		ch = ' ';
		if (start > 0)
			ch = (char)this->SpecificData.InAppItems.GetChar(start - 1);
		if (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z')))
		{
			start++;
			continue;
		}
		if (this->SpecificData.InAppItems.GetChar(start + l) != ':')
		{
			start++;
			continue;
		}
		const char *tmp = &this->SpecificData.InAppItems.GetText()[start + l + 1];
		while (((*tmp) != 0) && ((*tmp) != ',')) {
			result.AddChar(*tmp);
			tmp++;
		}
		result.Strip();
		result.Add(" ");
		result.Add(&this->SpecificData.Currency);
		return true;
	} while (true);
	RETURNERROR(false, "Unable to find item '%s' in item list", item);
	//return true;
}
bool DevelopModeOSContext::fnIsBillingSystemAvailable()
{
	return this->SpecificData.BillingServiceAvailable;
}
void DevelopModeOSContext::fnSendEmail(const char *To, const char *Subject, const char *Content)
{
	LOG_INFO("Sending email to: '%s' with subject: '%s'", To, Subject);
	LOG_INFO("Email content:\n---------------------------------------------\n%s\n---------------------------------------------\n", Content);
}
bool DevelopModeOSContext::fnSendAnalyticsEvent(const char* eventName, const void *buffer, GAC_ANALYTICS_EVENT_TYPE type)
{
	CHECK(eventName != NULL, false, "Invalid event name (null) !");
	switch (type)
	{
	case GAC_ANALYTICS_EVENT_SIMPLE:
		DEBUGMSG("ANALYTICS|%s|1", eventName);
		break;
	case GAC_ANALYTICS_EVENT_BOOL:
		CHECK(buffer != NULL, false, "Invalid buffer (NULL) !");
		if ((*(bool *)buffer) == true) {
			DEBUGMSG("ANALYTICS|%s|1", eventName);
		}
		else {
			DEBUGMSG("ANALYTICS|%s|0", eventName);
		}
		break;
	case GAC_ANALYTICS_EVENT_DOUBLE:
		CHECK(buffer != NULL, false, "Invalid buffer (NULL) !");
		DEBUGMSG("ANALYTICS|%s|%.6lf", eventName, *(double *)buffer);
		break;
	case GAC_ANALYTICS_EVENT_INT32:
		CHECK(buffer != NULL, false, "Invalid buffer (NULL) !");
		DEBUGMSG("ANALYTICS|%s|%d", eventName, *(int*)buffer);
		break;
	case GAC_ANALYTICS_EVENT_INT64:
		CHECK(buffer != NULL, false, "Invalid buffer (NULL) !");
		DEBUGMSG("ANALYTICS|%s|%lld", eventName, *(__int64*)buffer);
		break;
	default:
		LOG_ERROR("Analytics: Unknwon event type for %s -> value: %d", eventName, type);
		return false;
	}
	return true;
}
void DevelopModeOSContext::fnShare(const char* text)
{
	LOG_INFO("Sharing text: %s", text);
}
void DevelopModeOSContext::fnSendSMS(const char* text)
{
	LOG_INFO("Sending SMS: %s", text);
}
bool DevelopModeOSContext::fnIsAppInstalled(const char* appOrPackageName)
{
	char temp[1024];
	GApp::Utils::String tmp;
	tmp.Create(temp, 1024, true);
	tmp.Format("Does the following app '%s' exists !", appOrPackageName);
	if (MessageBoxA(this->SpecificData.hWnd, temp, "App Exists ?", MB_YESNO) == IDYES)
		return true;
	return false;
}
void DevelopModeOSContext::fnInstallApp(const char* appOrPackageName)
{
	if (MessageBoxA(this->SpecificData.hWnd, appOrPackageName, "Install app ?", MB_YESNO) == IDYES)
	{
		// eveniment ca s-a instalat
	}
	else {
		// eveniment ca nu s-a instalat
	}
}
void DevelopModeOSContext::fnApplicationInitializeCompleted()
{
	LOG_INFO("Application has beed initialized succesifully !");
}
// Google play services specific
bool DevelopModeOSContext::fnGooglePlayServices_IsSignedIn()
{
	return this->SpecificData.GooglePlayServicesSignedIn;
}
void DevelopModeOSContext::fnGooglePlayServices_SignIn()
{
	this->SpecificData.GooglePlayServicesSignedIn = true;
}
void DevelopModeOSContext::fnGooglePlayServices_SignOut()
{
	this->SpecificData.GooglePlayServicesSignedIn = false;
}
void DevelopModeOSContext::fnGooglePlayServices_SubmitScore(int leaderboardID, int score)
{
	LOG_INFO("GooglePlayServices -> SubmitScore (ID:%d -> %d)", leaderboardID, score);
}
const char*	DevelopModeOSContext::fnGooglePlayServices_GetCurrentPlayerName()
{
	return "Simulated User";
}
void DevelopModeOSContext::fnGooglePlayServices_ShowLeaderBoard(int leaderboardID)
{
	LOG_INFO("GooglePlayServices -> Show leaderboard (%d)", leaderboardID);
}
void DevelopModeOSContext::fnGooglePlayServices_RequestCurrentPlayerScore(int leaderboardID)
{
	LOG_INFO("GooglePlayServices -> Request current player score");
}
int	 DevelopModeOSContext::fnGooglePlayServices_GetCurrentPlayerScore(int leaderboardID)
{
	LOG_INFO("GooglePlayServices -> GetCurrentPlayerScore");
	return 100;
}
int	 DevelopModeOSContext::fnGooglePlayServices_GetCurrentPlayerRank(int leaderboardID)
{
	LOG_INFO("GooglePlayServices -> GetCurrentPlayerRank");
	return 2;
}
