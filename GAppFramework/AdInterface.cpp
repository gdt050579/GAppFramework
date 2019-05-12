#include "GApp.h"

#define CHECK_IF_ADS_ARE_ALLOWED	CHECK((Core.AdsEnabled) || (adType == GAC_ADTYPE_REWARDABLE), false, "Ads are disabled - exiting !");

GApp::Platform::AdInterface::AdInterface()
{
	adID = -1;
	CoreContext = NULL;
	adType = GAC_ADTYPE_BANNER;
	loadOnStartup = false;
}
bool GApp::Platform::AdInterface::Init(CoreReference coreContext, int _adID, bool _loadOnStartup, unsigned int _adType)
{
	CHECK(coreContext != NULL, false, "");
	CHECK(_adID >= 0, false, "");
	CoreContext = coreContext;
	adID = _adID;
	adType = _adType;
	loadOnStartup = _loadOnStartup;
		
	return true;
}
bool	GApp::Platform::AdInterface::Show()
{
	CHECK(CoreContext != NULL, false, "Invalid (NULL) CoreContext");
	CHECK(((AppContextData*)Core.Api.Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");	
	CHECK_IF_ADS_ARE_ALLOWED;
	CHECK(adID >= 0, false, "Invalid adID (should be bigger than 0)");
	((AppContextData*)Core.Api.Context)->OS->fnSetAdVisibility(adID, true);
	return true;
}
bool	GApp::Platform::AdInterface::Hide()
{
	CHECK(CoreContext != NULL, false, "Invalid (NULL) CoreContext");
	CHECK(((AppContextData*)Core.Api.Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	//CHECK(Core.AdsEnabled, false, "Ads are disabled - exiting !"); - nu e necesara
	CHECK(adID >= 0, false, "Invalid adID (should be bigger than 0)");
	((AppContextData*)Core.Api.Context)->OS->fnSetAdVisibility(adID, false);
	return true;
}
bool	GApp::Platform::AdInterface::Load()
{
	CHECK(CoreContext != NULL, false, "Invalid (NULL) CoreContext");
	CHECK(((AppContextData*)Core.Api.Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	CHECK_IF_ADS_ARE_ALLOWED;
	CHECK(adID >= 0, false, "Invalid adID (should be bigger than 0)");
	LOG_INFO("Query new ad for adID = %d", adID);
	((AppContextData*)Core.Api.Context)->OS->fnQueryNewAd(adID);
	return true;
}
bool	GApp::Platform::AdInterface::IsVisible()
{
	CHECK(CoreContext != NULL, false, "Invalid (NULL) CoreContext");
	CHECK(((AppContextData*)Core.Api.Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	//CHECK(Core.AdsEnabled, false, "Ads are disabled - exiting !"); - nu e necesara
	CHECK(adID >= 0, false, "Invalid adID (should be bigger than 0)");
	return ((AppContextData*)Core.Api.Context)->OS->fnIsAdVisible(adID);
}
bool	GApp::Platform::AdInterface::IsLoaded()
{
	CHECK(CoreContext != NULL, false, "Invalid (NULL) CoreContext");
	CHECK(((AppContextData*)Core.Api.Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !");
	CHECK_IF_ADS_ARE_ALLOWED;
	CHECK(adID >= 0, false, "Invalid adID (should be bigger than 0)");
	return ((AppContextData*)Core.Api.Context)->OS->fnIsAdLoaded(adID);
}