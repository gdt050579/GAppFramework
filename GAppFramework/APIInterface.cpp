#include "GApp.h"
#include "stdlib.h"
#ifdef PLATFORM_DEVELOP
	#include "DevelopMode.h"
#endif


GApp::UI::APIInterface::APIInterface()
{
	Context = NULL;
}
int   GApp::UI::APIInterface::Random(int maxValue)
{
	if (maxValue>0)
		return rand()%(maxValue);
	return 0;
}
int   GApp::UI::APIInterface::RandomInclusive(int maxValue)
{
	if (maxValue>=0)
		return rand() % (maxValue+1);
	return 0;
}
int   GApp::UI::APIInterface::Random(int minValue,int maxValue)
{
	if (maxValue == minValue)
		return maxValue;
	if (maxValue > minValue)
		return rand() % ((maxValue + 1) - minValue) + minValue;
	else
		return rand() % ((minValue + 1) - maxValue) + maxValue;
}
float GApp::UI::APIInterface::LogFromBase(float value, float base)
{
	return log(value) / log(base);
}
float GApp::UI::APIInterface::NthRoot(float value, float n)
{
	return pow(value, 1.0f / n);
}
float GApp::UI::APIInterface::ComputeResolutionAspectRatio(float width, float height)
{
	CHECK(Context != NULL, 0.0f, "Context was not set (is NULL)");
	CHECK(width > 0, 0.0f, "Width parameter should be positive and bigger than 0 (Value is %f)", width);
	CHECK(height > 0, 0.0f, "Height parameter should be positive and bigger than 0 (Value is %f)", height);
	float rap1 = ((float)(((AppContextData *)Context)->CoreObject.Width)) / (width);
	float rap2 = ((float)(((AppContextData *)Context)->CoreObject.Height)) / (height);
	return GAC_MIN(rap1, rap2);
}

bool GApp::UI::APIInterface::GoToNextScene()
{
	CHECK(Context!=NULL,false,"Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	return ((AppContextData*)Context)->GoToNextScene();	
}
bool GApp::UI::APIInterface::SetNextScene(unsigned int sceneID)
{
	CHECK(Context!=NULL,false,"Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	return ((AppContextData*)Context)->SetNextScene(sceneID);
}
bool GApp::UI::APIInterface::ChangeScene(unsigned int sceneID)
{
	CHECK(Context!=NULL,false,"Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	return ((AppContextData*)Context)->ChangeScene(sceneID);
}
bool GApp::UI::APIInterface::ReloadCurrentScene()
{
	CHECK(Context!=NULL,false,"Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	return ((AppContextData*)Context)->ReloadCurrentScene();
}
bool GApp::UI::APIInterface::ChangeSceneUsingCountersGroup(unsigned int sceneID, GApp::UI::CountersGroup *grp, unsigned int gotoSceneID)
{
	CHECK(Context != NULL, false, "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	return ((AppContextData*)Context)->ChangeSceneUsingCountersGroup(sceneID,grp,gotoSceneID);
}
bool GApp::UI::APIInterface::ReloadCurrentSceneUsingCountersGroup(GApp::UI::CountersGroup *grp, unsigned int gotoSceneID)
{
	CHECK(Context != NULL, false, "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	return ((AppContextData*)Context)->ReloadCurrentSceneUsingCountersGroup(grp,gotoSceneID);
}
bool GApp::UI::APIInterface::ChangeSceneUsingCountersGroup(unsigned int sceneID, GApp::UI::CountersGroup *grp)
{
	CHECK(Context != NULL, false, "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	return ((AppContextData*)Context)->ChangeSceneUsingCountersGroup(sceneID, grp);
}
bool GApp::UI::APIInterface::ReloadCurrentSceneUsingCountersGroup(GApp::UI::CountersGroup *grp)
{
	CHECK(Context != NULL, false, "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	return ((AppContextData*)Context)->ReloadCurrentSceneUsingCountersGroup(grp);
}
GApp::UI::Scene* GApp::UI::APIInterface::GetCurrentScene()
{
	CHECK(Context != NULL, NULL, "Context was not set (is NULL)");
	return ((AppContextData *)Context)->CurrentScene;
}
bool GApp::UI::APIInterface::ReadResource(unsigned int pos,unsigned char *Buffer,unsigned int size,GAC_TYPE_ENCRYPTION encryptionMode,void *Key)
{
	CHECK(Context != NULL, false, "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	return ((AppContextData *)Context)->OS->fnReadBufferFromResources(pos, Buffer, size);
}
bool GApp::UI::APIInterface::ReadResource(unsigned int pos,GApp::Utils::Buffer *b,unsigned int size,GAC_TYPE_ENCRYPTION encryptionMode,void *Key)
{
	CHECK(b!=NULL,false,"Invalid (NULL) Buffer (variable b)");
	CHECK(b->Resize(size),false,"Unable to resize buffer to %d bytes",size);
	return ReadResource(pos,b->GetBuffer(),size,encryptionMode,Key);
}
unsigned int GApp::UI::APIInterface::GetSystemTickCount()
{
	// nu il validez ca Context nu e NULL pentru ca vrem sa fie cat mai rapid
	return ((AppContextData *)Context)->OS->fnGetSystemTicksCount();
}
bool GApp::UI::APIInterface::GetSystemDateTime(GApp::Utils::DateTime *dateTime)
{
	CHECK(dateTime != NULL, false, "");
	return ((AppContextData *)Context)->OS->fnGetSystemDateTime(dateTime);
}
void GApp::UI::APIInterface::Close()
{
	while (true)
	{
		CHECKBK(Context != NULL,  "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		((AppContextData *)Context)->OS->fnClose();
		return;
	}
}

void GApp::UI::APIInterface::OpenBrowser (const char *url)
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		((AppContextData *)Context)->OS->fnOpenBrowser(url);
		return;
	}
}
void GApp::UI::APIInterface::OpenDeveloperWebPage()
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");		
		((AppContextData *)Context)->OS->fnOpenBrowser(((AppContextData *)Context)->OS->Data.DeveloperHomePageURL);
		return;
	}
}
bool GApp::UI::APIInterface::OpenAppHandler(unsigned int handlerID)
{
	char temp[256];
	GApp::Utils::String str;
	
	CHECK(Context != NULL, false, "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	CHECK(str.Create(temp, 256, true), false, "Unable to create working string !");
	
	OSContext *OS = ((AppContextData *)Context)->OS;

	switch (handlerID)
	{
	case GAC_APP_HANDLER_FACEBOOK_PAGE:
		CHECK((OS->Data.FacebookURL != NULL) && (OS->Data.FacebookURL[0]!=0), false, "Missing Facebook acount !");
		CHECK(str.Add(OS->Data.FacebookURL), false, "");
		CHECK(str.Add("::https://www.facebook.com/"), false, "");
		CHECK(str.Add(OS->Data.FacebookURL), false, "");
		break;
	default:
		RETURNERROR(false, "Handler code for handler id: %d was not defined !", handlerID);
	}

	((AppContextData *)Context)->OS->fnOpenAppHandler(handlerID, str.GetText());	
	return true;
}
bool GApp::UI::APIInterface::IsAppHandlerAvailable(unsigned int handlerID)
{

	CHECK(Context != NULL, false, "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	return ((AppContextData *)Context)->OS->fnIsAppHandlerAvailable(handlerID);
}
void GApp::UI::APIInterface::OpenFacebookPage()
{
	OpenAppHandler(GAC_APP_HANDLER_FACEBOOK_PAGE);
}
void GApp::UI::APIInterface::OpenYouTubePage()
{
	OpenAppHandler(GAC_APP_HANDLER_YOUTUBE_CHANNAL);
}
void GApp::UI::APIInterface::OpenTwitterPage()
{
	OpenAppHandler(GAC_APP_HANDLER_TWITTER_PAGE);
}
void GApp::UI::APIInterface::OpenInstagramPage()
{
	OpenAppHandler(GAC_APP_HANDLER_INSTAGRAM_PAGE);
}
bool GApp::UI::APIInterface::IsFaceBookHandlerAvailable()
{
	return IsAppHandlerAvailable(GAC_APP_HANDLER_FACEBOOK_PAGE);
}
bool GApp::UI::APIInterface::IsYouTubeHandlerAvailable()
{
	return IsAppHandlerAvailable(GAC_APP_HANDLER_YOUTUBE_CHANNAL);
}
bool GApp::UI::APIInterface::IsTwitterHandlerAvailable()
{
	return IsAppHandlerAvailable(GAC_APP_HANDLER_TWITTER_PAGE);
}
bool GApp::UI::APIInterface::IsInstagramHandlerAvailable()
{
	return IsAppHandlerAvailable(GAC_APP_HANDLER_INSTAGRAM_PAGE);
}

void GApp::UI::APIInterface::OpenApplicationMarketPage ()
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		((AppContextData *)Context)->OS->fnOpenApplicationMarketPage();
		return;
	}
}
void GApp::UI::APIInterface::OpenDeveloperMarketPage ()
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		((AppContextData *)Context)->OS->fnOpenDeveloperMarketPage();
		return;
	}
}
void GApp::UI::APIInterface::Share(const char * text)
{
	Share(text, "Share");
}
void GApp::UI::APIInterface::Share(const char * text,const char *title)
{
	char temp[512];
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		CHECKBK(text != NULL, "text was not set (is NULL)");
		CHECKBK(title != NULL, "title was not set (is NULL)");
		GApp::Utils::String s;
		CHECKBK(s.Create(temp, 512, true), "Unable to create local string !");
		CHECKBK(s.Set(title), "Failed to add title");
		CHECKBK(s.Add("::"), "Failed to add separator !");
		CHECKBK(s.Add(text), "Failed to add text !");
		((AppContextData *)Context)->OS->fnShare(temp);
		return;
	}
}
void GApp::UI::APIInterface::SendSMS(const char * message)
{
	SendSMS(message, "");
}
void GApp::UI::APIInterface::SendSMS(const char * message, const char *phoneNumber)
{
	char temp[512];
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		CHECKBK(message != NULL, "message was not set (is NULL)");
		CHECKBK(phoneNumber != NULL, "phoneNumber was not set (is NULL)");
		GApp::Utils::String s;
		CHECKBK(s.Create(temp, 512, true), "Unable to create local string !");
		CHECKBK(s.Set(phoneNumber), "Failed to add phoneNumber");
		CHECKBK(s.Add("::"), "Failed to add separator !");
		CHECKBK(s.Add(message), "Failed to add message !");
		((AppContextData *)Context)->OS->fnSendSMS(temp);
		return;
	}
}
void GApp::UI::APIInterface::RateApplication(/*bool disableCounterForRateScene*/)
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		//if (disableCounterForRateScene) {
		//	DisableRate();
		//}
		((AppContextData *)Context)->OS->fnRateApp();
		return;
	}
}
void GApp::UI::APIInterface::DisableAds()
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		// dau Hide la toti
		for (unsigned int tr = 0;;tr++)
		{
			GApp::Platform::AdInterface* ad = ((AppContextData*)Context)->CoreObject.Resources->GetAd(tr);
			if (ad == NULL)
				break;
			LOG_INFO("Hiding ad: %d", tr);
			ad->Hide();
		}		
		((AppContextData *)Context)->CoreObject.AdsEnabled = false;
		LOG_INFO("Ads have been disabled !");
		return;
	}
}
bool GApp::UI::APIInterface::GetAdsEnableStatus()
{
	CHECK(Context != NULL, false, "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	return ((AppContextData *)Context)->CoreObject.AdsEnabled;
}

void GApp::UI::APIInterface::SetTouchFlags(unsigned int add, unsigned int remove)
{	
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		REMOVE_BIT(remove, TOUCH_FLAG_CONTROL_SENDEVENT_MASK);
		REMOVE_BIT(remove, TOUCH_FLAG_SCENE_SENDEVENT_MASK);

		if ((add & TOUCH_FLAG_CONTROL_SENDEVENT_MASK) != 0)
			remove |= TOUCH_FLAG_CONTROL_SENDEVENT_MASK;
		if ((add & TOUCH_FLAG_SCENE_SENDEVENT_MASK) != 0)
			remove |= TOUCH_FLAG_SCENE_SENDEVENT_MASK;

		REMOVE_BIT(((AppContextData *)Context)->TouchFlags, remove);
		SET_BIT(((AppContextData *)Context)->TouchFlags, add);
		return;
	}
}
void GApp::UI::APIInterface::SetTouchBoundary(float left, float top, float right, float bottom)
{
	float tmp;
	if (left > right) {
		tmp = left;
		left = right;
		right = tmp;
	}
	if (top > bottom)
	{
		tmp = top;
		top = bottom;
		bottom = tmp;
	}
	((AppContextData *)Context)->TouchBoundary.Set(left, top, right, bottom);
	((AppContextData *)Context)->TouchBoundaryEnabled = true;
}
void GApp::UI::APIInterface::ClearTouchBoundary()
{
	((AppContextData *)Context)->TouchBoundaryEnabled = false;
}
void GApp::UI::APIInterface::DisableTouch()
{
	((AppContextData *)Context)->TouchBoundary.Set(-1, -1, -1, -1);
	((AppContextData *)Context)->TouchBoundaryEnabled = true;
}
void GApp::UI::APIInterface::Sleep(unsigned int milliseconds)
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		((AppContextData *)Context)->OS->fnThreadWait(milliseconds);
		return;
	}
}
bool GApp::UI::APIInterface::ThreadRun(void (*runFunction)(void *threadObject),void*  threadObject)
{
	CHECK(Context != NULL, false, "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	CHECK(runFunction!=NULL,false,"Run function should not be NULL");
	CHECK(((AppContextData *)Context)->OS->fnThreadRun(runFunction,threadObject),false,"Thread function failed !");
	return true;
}
const char * GApp::UI::APIInterface::ExecuteOsCommand(const char * command)
{
	CHECK(Context != NULL, NULL, "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, NULL, "Application is closing - API access is denied !");
	CHECK(command != NULL, "", "Invalid (NULL) command for ExecuteOsCommand function !");
	CHECK(((AppContextData *)Context)->CoreObject.OSCommandString.Set(""), "", "Unable to create storage buffer");
	((AppContextData *)Context)->OS->fnExecuteOsCommand(command, &(((AppContextData *)Context)->CoreObject.OSCommandString));
	return ((AppContextData *)Context)->CoreObject.OSCommandString.GetText();
}
bool GApp::UI::APIInterface::SendBuyRequestForItem(const char * item)
{
	CHECK(Context != NULL, false, "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	CHECK(item != NULL, false, "Item should be a valid string !");
	CHECK(item[0] != 0, false, "Item should contain a word !");
	while (true)
	{
		LOG_INFO("SendBuyRequestForItem(%s)", item);
		CHECKBK(((AppContextData *)Context)->CoreObject.InAppBillingItem.Len() == 0, "There is already a transaction in place for item: %s", ((AppContextData *)Context)->CoreObject.InAppBillingItem.GetText());
		CHECKBK(((AppContextData *)Context)->CoreObject.InAppBillingItem.Set(item), "Failed to set new in-app item !");
		CHECKBK(((AppContextData *)Context)->OS->fnSendBuyRequestForItem(item), "SendBuyRequestForItem function failed !");
		return true;
	}
	((AppContextData *)Context)->CoreObject.InAppBillingItem.Clear();
	return false;
}
const char* GApp::UI::APIInterface::GetItemPrice(const char* item)
{
	CHECK(Context != NULL, "", "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "", "Application is closing - API access is denied !");
	CHECK(item != NULL, "", "Item should be a valid string !");
	CHECK(item[0] != 0, "", "Item should contain a word !");
	GApp::Utils::String str;
	CHECK(str.Create(((AppContextData *)Context)->OS->Data.RequestedItemPrice, 128, true), "", "Unable to create working string variable !");
	CHECK(((AppContextData *)Context)->OS->fnGetItemPrice(item, str), "", "Failed to read result from GetItemPrice API !");
	return ((AppContextData *)Context)->OS->Data.RequestedItemPrice;
}
bool GApp::UI::APIInterface::IsBillingSystemAvailable()
{
	CHECK(Context != NULL, false, "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	CHECK(((AppContextData *)Context)->OS->fnIsBillingSystemAvailable(), false, "IsBillingSystemAvailable function failed !");
	return true;
}
void GApp::UI::APIInterface::SendEmail(const char *To, const char *Subject, const char *Content)
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		CHECKBK(To != NULL, "Invalid address (NULL) !");
		CHECKBK(Subject != NULL, "Invalid Subject (NULL) !");
		CHECKBK(Content != NULL, "Invalid Content (NULL) !");
		((AppContextData *)Context)->OS->fnSendEmail(To, Subject, Content);
		return;
	}
	LOG_ERROR("SendEmail not call due to invalid parameters or improper setup!");
}
void GApp::UI::APIInterface::ApplicationInitializeCompleted()
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		((AppContextData *)Context)->OS->fnApplicationInitializeCompleted();
		return;
	}
}
const char* GApp::UI::APIInterface::GetLastPurchaseInformation()
{	
	CHECK(Context != NULL, "", "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "", "Application is closing - API access is denied !");
	if (((AppContextData *)Context)->OS->Data.LastPurchaseInfo.Len() == 0)
		return "";
	return ((AppContextData *)Context)->OS->Data.LastPurchaseInfo.GetText();
}
unsigned int GApp::UI::APIInterface::GetAppVersion()
{
	// nu e prefixat ca sa fie rapid
	return ((AppContextData *)Context)->CoreObject.AppVersion;
}
bool GApp::UI::APIInterface::RaiseEvent(FrameworkObject* object, unsigned int eventID)
{
	Scene* scn = ((AppContextData*)Context)->CurrentScene;
	// poate sa fie si NULL daca e trimis de App
	//CHECK(object != NULL, false, "Invalid (NULL) framework object for RaiseEvent scene !");
	CHECK(scn != NULL, false, "Expecting a valid scene !");
	return scn->OnEvent(object, eventID);
}
bool GApp::UI::APIInterface::IsAppInstalled(const char * appOrPackageName)
{
	CHECK(Context != NULL, false, "Context was not set (is NULL)");
	CHECK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	CHECK(appOrPackageName != NULL, false, "Invalid 'appOrPackageName' field (must not be NULL) !");
	return ((AppContextData *)Context)->OS->fnIsAppInstalled(appOrPackageName);
}
void GApp::UI::APIInterface::InstallApp(const char * appOrPackageName)
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		CHECKBK(appOrPackageName != NULL, "Invalid 'appOrPackageName' field (must not be NULL) !");
		((AppContextData *)Context)->OS->fnInstallApp(appOrPackageName);
		return;
	}
	LOG_ERROR("InstallApp not call due to invalid parameters or improper setup!");
}
bool GApp::UI::APIInterface::GooglePlayServices_IsSignedIn()
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		return ((AppContextData *)Context)->OS->fnGooglePlayServices_IsSignedIn();
	}
	RETURNERROR(false,"Fail to connect to Google Play Services");
}
void GApp::UI::APIInterface::GooglePlayServices_SignIn()
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		((AppContextData *)Context)->OS->fnGooglePlayServices_SignIn();
		return;
	}
	LOG_ERROR("Fail to sign in to Google Play Services");
}
void GApp::UI::APIInterface::GooglePlayServices_SignOut()
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		((AppContextData *)Context)->OS->fnGooglePlayServices_SignOut();
		return;
	}
	LOG_ERROR("Fail to sign out to Google Play Services");
}
void GApp::UI::APIInterface::GooglePlayServices_SubmitScore(int leaderBoardID, int score)
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		((AppContextData *)Context)->OS->fnGooglePlayServices_SubmitScore(leaderBoardID, score);
		return;
	}
	LOG_ERROR("Fail to submit score to Google Play Services");
}
const char*	GApp::UI::APIInterface::GooglePlayServices_GetCurrentPlayerName()
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		return ((AppContextData *)Context)->OS->fnGooglePlayServices_GetCurrentPlayerName();
	}
	RETURNERROR("", "Fail to get player name from to Google Play Services");
}
void GApp::UI::APIInterface::GooglePlayServices_ShowLeaderBoard(int leaderBoardID)
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		((AppContextData *)Context)->OS->fnGooglePlayServices_ShowLeaderBoard(leaderBoardID);
		return;
	}
	LOG_ERROR("Fail to show a leaderboard from Google Play Services");
}
void GApp::UI::APIInterface::GooglePlayServices_RequestCurrentPlayerScore(int leaderBoardID)
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		((AppContextData *)Context)->OS->fnGooglePlayServices_RequestCurrentPlayerScore(leaderBoardID);
		return;
	}
	LOG_ERROR("Fail to request current player score from Google Play Services");
}
int GApp::UI::APIInterface::GooglePlayServices_GetCurrentPlayerScore(int leaderBoardID)
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		return ((AppContextData *)Context)->OS->fnGooglePlayServices_GetCurrentPlayerScore(leaderBoardID);
	}
	RETURNERROR(-1,"Fail to request current player score from Google Play Services");
}
int GApp::UI::APIInterface::GooglePlayServices_GetCurrentPlayerRank(int leaderBoardID)
{
	while (true)
	{
		CHECKBK(Context != NULL, "Context was not set (is NULL)");
		CHECKBK(((AppContextData*)Context)->OS->Data.ApplicationIsClosing == false, "Application is closing - API access is denied !");
		return ((AppContextData *)Context)->OS->fnGooglePlayServices_GetCurrentPlayerRank(leaderBoardID);
	}
	RETURNERROR(-1,"Fail to request current player rank from Google Play Services");
}