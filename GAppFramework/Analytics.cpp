#include "GApp.h"

using namespace GApp::Platform;

#define ANALYTICS_CALL(valoare,tip) \
	CHECK(CoreContext != NULL, false, "Invalid (NULL) CoreContext"); \
	CHECK(((AppContextData*)Core.Api.Context)->OS->Data.ApplicationIsClosing == false, false, "Application is closing - API access is denied !"); \
	CHECK(((AppContextData*)Core.Api.Context)->OS->fnSendAnalyticsEvent(name, valoare, tip), false, "Failed to call SendAnalyticsEvent function !"); \
	return true;

AnalyticsInterface::AnalyticsInterface()
{
	this->CoreContext = NULL;
}
bool AnalyticsInterface::SendEvent(const char* name)
{
	ANALYTICS_CALL(NULL, GAC_ANALYTICS_EVENT_SIMPLE);
}
bool AnalyticsInterface::SendDoubleValue(const char* name, double value)
{
	ANALYTICS_CALL(&value, GAC_ANALYTICS_EVENT_DOUBLE);
}
bool AnalyticsInterface::SendInt64Value(const char* name, Int64 value)
{
	ANALYTICS_CALL(&value, GAC_ANALYTICS_EVENT_INT64);
}
bool AnalyticsInterface::SendInt32Value(const char* name, Int32 value)
{
	ANALYTICS_CALL(&value, GAC_ANALYTICS_EVENT_INT32);
}
bool AnalyticsInterface::SendBoolValue(const char* name, bool value)
{
	ANALYTICS_CALL(&value, GAC_ANALYTICS_EVENT_BOOL);
}
bool AnalyticsInterface::SendStandardEvent(GAC_ANALYTICS_STD_EVENT_TYPE type)
{
	switch (type)
	{
		case GAC_ANALYTICS_STD_EVENT_FB_BUTTON				: return SendEvent("facebook_button");
		case GAC_ANALYTICS_STD_EVENT_FB_REWARD_YES			: return SendEvent("facebook_reward_yes");
		case GAC_ANALYTICS_STD_EVENT_FB_REWARD_NO			: return SendEvent("facebook_reward_no");
		case GAC_ANALYTICS_STD_EVENT_FB_POPUP_YES			: return SendEvent("facebook_popup_yes");
		case GAC_ANALYTICS_STD_EVENT_FB_POPUP_NO			: return SendEvent("facebook_popup_no");
		case GAC_ANALYTICS_STD_EVENT_SHARE_BUTTON			: return SendEvent("share_button");
		case GAC_ANALYTICS_STD_EVENT_SHARE_REWARD_YES		: return SendEvent("share_reward_yes");
		case GAC_ANALYTICS_STD_EVENT_SHARE_REWARD_NO		: return SendEvent("share_reward_no");
		case GAC_ANALYTICS_STD_EVENT_SHARE_POPUP_YES		: return SendEvent("share_popup_yes");
		case GAC_ANALYTICS_STD_EVENT_SHARE_POPUP_NO			: return SendEvent("share_popup_no");
		case GAC_ANALYTICS_STD_EVENT_RATE_BUTTON			: return SendEvent("rate_button");
		case GAC_ANALYTICS_STD_EVENT_RATE_REWARD_YES		: return SendEvent("rate_reward_yes");
		case GAC_ANALYTICS_STD_EVENT_RATE_REWARD_NO			: return SendEvent("rate_reward_no");
		case GAC_ANALYTICS_STD_EVENT_RATE_POPUP_YES			: return SendEvent("rate_popup_yes");
		case GAC_ANALYTICS_STD_EVENT_RATE_POPUP_NO			: return SendEvent("rate_popup_no");
	};
	RETURNERROR(false, "Unknwon standard event type: %d", type);
}
bool AnalyticsInterface::SendLevelReachEvent(unsigned int level)
{
	char temp[128];
	GApp::Utils::String tmp;
	tmp.Create(temp, 128, true);
	tmp.SetFormat("Level_%d", level);
	return SendEvent(temp);
}
bool AnalyticsInterface::SendSeasonReachEvent(unsigned int season)
{
	char temp[128];
	GApp::Utils::String tmp;
	tmp.Create(temp, 128, true);
	tmp.SetFormat("Season_%d", season);
	return SendEvent(temp);
}
bool AnalyticsInterface::SendLevelReachEvent(unsigned int season, unsigned int level)
{
	char temp[128];
	GApp::Utils::String tmp;
	tmp.Create(temp, 128, true);
	tmp.SetFormat("Season_%d__Level_%d", season, level);
	return SendEvent(temp);
}
bool AnalyticsInterface::SendRewardAvailableEvent(const char *rewardName)
{
	char temp[128];
	GApp::Utils::String tmp;
	tmp.Create(temp, 128, true);
	tmp.SetFormat("Reward_available_%s", rewardName);
	return SendEvent(temp);
}
bool AnalyticsInterface::SendRewardRequestedEvent(const char *rewardName)
{
	char temp[128];
	GApp::Utils::String tmp;
	tmp.Create(temp, 128, true);
	tmp.SetFormat("Reward_requested_%s", rewardName);
	return SendEvent(temp);
}
bool AnalyticsInterface::SendRewardReceivedEvent(const char *rewardName)
{
	char temp[128];
	GApp::Utils::String tmp;
	tmp.Create(temp, 128, true);
	tmp.SetFormat("Reward_received_%s", rewardName);
	return SendEvent(temp);
}
bool AnalyticsInterface::SendUserProperty(const char * propertyName, const char * propertyValue)
{
	CHECK(propertyName != NULL, false, "");
	CHECK(propertyValue != NULL, false, "");

	char temp[256];
	GApp::Utils::String tmp;
	CHECK(tmp.Create(temp, 256, true), false, "");
	CHECK(tmp.Add("user_property::"), false, "");
	CHECK(tmp.Add(propertyName), false, "");
	CHECK(tmp.Add("::"), false, "");
	CHECK(tmp.Add(propertyValue), false, "");

	return SendEvent(temp);
}
bool AnalyticsInterface::SendCurrentScreen(const char * screenName)
{
	CHECK(screenName != NULL, false, "");

	char temp[256];
	GApp::Utils::String tmp;
	CHECK(tmp.Create(temp, 256, true), false, "");
	CHECK(tmp.Add("screen::"), false, "");
	CHECK(tmp.Add(screenName), false, "");


	return SendEvent(temp);
}
