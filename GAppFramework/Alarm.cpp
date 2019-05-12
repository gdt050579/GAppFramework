#include "GApp.h"


#define ALARM_TYPE_EXACT_DATE			0	// Exact date (Year / Month / Day / Hour / Minute)
#define ALARM_TYPE_SPECIFIC_DATE		1	// On a specific date (Month / Day / Hour / Minute )
#define ALARM_TYPE_SPECIFIC_MONTH_DAY	2	// On a specific day of a month
#define ALARM_TYPE_SPECIFIC_WEEK_DAY	3	// On a specific day of a week
#define ALARM_TYPE_SPECIFIC_HOUR_MINUTE	4	// Every day at a specific time (Hour / Minute)
#define ALARM_TYPE_EVERY_X_MINUTES		5	// Every x minutes
#define ALARM_TYPE_EVERY_X_HOURS		6	// Every x hours
#define ALARM_TYPE_EVERY_X_DAYS			7	// Every x days


static unsigned int __days__[4][12] =
{
	{ 0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335 },
	{ 366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700 },
	{ 731, 762, 790, 821, 851, 882, 912, 943, 974,1004,1035,1065 },
	{ 1096,1127,1155,1186,1216,1247,1277,1308,1339,1369,1400,1430 },
};

#define DATE_TO_SECONDS_FROM_2000(a,l,z,o,m,s) (((((a) / 4 * (1461) + __days__[(a) % 4][(l)] + ((z) - 1)) * 24 + (o)) * 60 + (m)) * 60 + (s))

#define UPDATE_END_ALARM(startTime) \
	if (Duration <= 0) EndOfCurrentAlarm = 0xFFFFFFFF; else EndOfCurrentAlarm = (startTime) + Duration; \
	if (EndOfCurrentAlarm < (startTime)) EndOfCurrentAlarm = 0xFFFFFFFF;


bool GApp::UI::Alarm::DateTimeToSecondsFrom2000(GApp::Utils::DateTime *dt, unsigned int &result)
{
	CHECK((dt->Year >= 2000) && (dt->Year <= 2099), false, "Expecting an year between 2000 and 2099 - received: %d", dt->Year);
	unsigned int year = dt->Year - 2000;
	result = DATE_TO_SECONDS_FROM_2000(year, dt->Month, dt->Day, dt->Hour, dt->Minute, dt->Second);
	return true;
}


void GApp::UI::Alarm::Init(unsigned int _index, unsigned int _uniqueID,bool _enabled, bool _oneTimeOnly, unsigned int _duration, const char * pushNotificationMessage, int _alarmType, unsigned int _year, unsigned int _month, unsigned int _day, unsigned int _hour, unsigned int _minute)
{
	this->Index = _index;
	this->UniqueID = _uniqueID;
	this->Year = _year;
	this->Month = _month;
	this->Day = _day;
	this->Hour = _hour;
	this->Minute = _minute;
	this->AlarmType = _alarmType;
	this->OneTimeOnly = _oneTimeOnly;
	this->Duration = _duration * 60; // _duration e in minute, convertesc la secunde
	this->PushNotificationString = pushNotificationMessage;
	this->Enabled = _enabled;
	this->LastTriggered = GAC_ALARM_NOT_TRIGGERED; // cac sa poata 
	// ajustari ale valorilor
	switch (AlarmType)
	{
		case ALARM_TYPE_EVERY_X_MINUTES:
			this->Minute *= 60; // conversc in secunde
			break;
		case ALARM_TYPE_EVERY_X_HOURS:
			this->Hour *= 3600; // conversc in secunde
			break;
		case ALARM_TYPE_EVERY_X_DAYS:
			this->Day *= 86400; // conversc in secunde
			break;
	}
}
void GApp::UI::Alarm::Start()
{

}
void GApp::UI::Alarm::Stop()
{

}
bool GApp::UI::Alarm::IsEnabled()
{
	return Enabled;
}
GAC_TYPE_ALARMSTATE GApp::UI::Alarm::ShouldTrigger_ExactDate(GApp::Utils::DateTime *dt, unsigned int secondsFrom2000, unsigned int &missed)
{
	unsigned int EndOfCurrentAlarm;
	unsigned int cTime;

	cTime = DATE_TO_SECONDS_FROM_2000(Year, Month, Day, Hour, Minute, 0);
	UPDATE_END_ALARM(cTime);

	if (LastTriggered == GAC_ALARM_NOT_TRIGGERED) {
		if ((secondsFrom2000 >= cTime) && (secondsFrom2000 < EndOfCurrentAlarm))
		{
			missed = 0;
			LastTriggered = EndOfCurrentAlarm;
			return GAC_ALARMSTATE_TRIGGERED;
		}
	}
	else {
		if (secondsFrom2000 >= EndOfCurrentAlarm)
		{
			missed = 0;
			Enabled = false; // nu mai are sens sa o activez pt. ca e one-tiime
			LastTriggered = GAC_ALARM_NOT_TRIGGERED;
			return GAC_ALARMSTATE_CLOSED;
		}
	}
	return GAC_ALARMSTATE_NONE;
}
GAC_TYPE_ALARMSTATE GApp::UI::Alarm::ShouldTrigger_SpecificDate(GApp::Utils::DateTime *dt, unsigned int secondsFrom2000, unsigned int &missed)
{
	unsigned int EndOfCurrentAlarm;
	unsigned int cTime;

	cTime = DATE_TO_SECONDS_FROM_2000(dt->Year - 2000, Month, Day, Hour, Minute, 0);
	UPDATE_END_ALARM(cTime);

	if (LastTriggered == GAC_ALARM_NOT_TRIGGERED)
	{
		if ((secondsFrom2000 >= cTime) && (secondsFrom2000 < EndOfCurrentAlarm))
		{
			missed = 0;
			LastTriggered = EndOfCurrentAlarm;
			return GAC_ALARMSTATE_TRIGGERED;
		}
		return GAC_ALARMSTATE_NONE;
	}
	else {
		if (cTime >= LastTriggered) {
			if ((secondsFrom2000 >= cTime) && (secondsFrom2000 < EndOfCurrentAlarm))
			{
				missed = 0;
				LastTriggered = EndOfCurrentAlarm;
				return GAC_ALARMSTATE_CLOSED_AND_TRIGGER;
			}
			else {
				LastTriggered = GAC_ALARM_NOT_TRIGGERED;
				return GAC_ALARMSTATE_CLOSED;
			}
		}
	}
	return GAC_ALARMSTATE_NONE;
}
GAC_TYPE_ALARMSTATE GApp::UI::Alarm::ShouldTrigger_MonthDay(GApp::Utils::DateTime *dt)
{
	if (dt->Day == this->Day)
	{
		if (LastTriggered == GAC_ALARM_NOT_TRIGGERED)
		{
			LastTriggered = 86400 + (DATE_TO_SECONDS_FROM_2000(dt->Year - 2000, dt->Month, dt->Day, 0, 0, 0));
			return GAC_ALARMSTATE_TRIGGERED;
		}
		else {
			LastTriggered = 86400 + (DATE_TO_SECONDS_FROM_2000(dt->Year - 2000, dt->Month, dt->Day, 0, 0, 0));
			return GAC_ALARMSTATE_CLOSED_AND_TRIGGER;
		}
	}
	else {
		if (LastTriggered != GAC_ALARM_NOT_TRIGGERED)
		{
			LastTriggered = GAC_ALARM_NOT_TRIGGERED;
			return GAC_ALARMSTATE_CLOSED;
		}
		return GAC_ALARMSTATE_NONE;
	}
}
GAC_TYPE_ALARMSTATE GApp::UI::Alarm::ShouldTrigger_WeekDay(GApp::Utils::DateTime *dt, unsigned int &missed)
{
	unsigned int cTime;
	if (dt->DayOfWeek == this->Day)
	{
		if (LastTriggered == GAC_ALARM_NOT_TRIGGERED)
		{
			missed = 0;
			LastTriggered = 86400 + (DATE_TO_SECONDS_FROM_2000(dt->Year - 2000, dt->Month, dt->Day, 0, 0, 0));
			return GAC_ALARMSTATE_TRIGGERED;
		}
		else {
			cTime = 86400 + (DATE_TO_SECONDS_FROM_2000(dt->Year - 2000, dt->Month, dt->Day, 0, 0, 0));
			if (cTime > LastTriggered)
				missed = (cTime - LastTriggered) / (86400 * 7); // o saptamana
			else
				missed = 0;
			LastTriggered = cTime;
			return GAC_ALARMSTATE_CLOSED_AND_TRIGGER;
		}
	}
	else {
		if (LastTriggered != GAC_ALARM_NOT_TRIGGERED)
		{
			LastTriggered = GAC_ALARM_NOT_TRIGGERED;
			return GAC_ALARMSTATE_CLOSED;
		}
		return GAC_ALARMSTATE_NONE;
	}
}
GAC_TYPE_ALARMSTATE GApp::UI::Alarm::ShouldTrigger_SpecificHourMinute(GApp::Utils::DateTime *dt, unsigned int secondsFrom2000, unsigned int &missed)
{
	unsigned int cTime, EndOfCurrentAlarm;

	cTime = DATE_TO_SECONDS_FROM_2000(dt->Year - 2000, dt->Month, dt->Day, Hour, Minute, 0);
	UPDATE_END_ALARM(cTime);

	if ((secondsFrom2000 >= cTime) && (secondsFrom2000 < EndOfCurrentAlarm)) // ar trebui sa fac triggered la o alarma noua
	{
		if (LastTriggered == GAC_ALARM_NOT_TRIGGERED)
		{
			missed = 0;
			LastTriggered = EndOfCurrentAlarm;
			return GAC_ALARMSTATE_TRIGGERED;
		}
		else {
			if (EndOfCurrentAlarm > LastTriggered)
				missed = (EndOfCurrentAlarm - LastTriggered) / (86400); // o zi
			else
				missed = 0;
			LastTriggered = EndOfCurrentAlarm;
			return GAC_ALARMSTATE_CLOSED_AND_TRIGGER;
		}
	}
	else  // nu trebuie sa fac triggered
	{
		if (LastTriggered != GAC_ALARM_NOT_TRIGGERED)
		{
			LastTriggered = GAC_ALARM_NOT_TRIGGERED;
			return GAC_ALARMSTATE_CLOSED;
		}
		return GAC_ALARMSTATE_NONE;
	}
}
GAC_TYPE_ALARMSTATE	GApp::UI::Alarm::ShouldTrigger_EveryXTime(unsigned int intervalInSeconds, unsigned int secondsFrom2000, unsigned int &missed)
{
	unsigned int EndOfCurrentAlarm, dif, new_StartTime, result;
	
	if (LastTriggered == GAC_ALARM_NOT_TRIGGERED)
	{
		// din acest moment measurement
		this->Year = secondsFrom2000;
		this->Month = 0; // NOT_TRIGGERED
		LastTriggered = secondsFrom2000 + intervalInSeconds;
		return GAC_ALARMSTATE_NONE;
	}
	if (secondsFrom2000 < this->Year)
		return GAC_ALARMSTATE_NONE;
	dif = (secondsFrom2000 - this->Year) / intervalInSeconds;
	if (dif == 0)
	{
		LastTriggered = this->Year + intervalInSeconds;
		missed = 0;
		if (this->Month == 1)
		{
			this->Month = 0; // fac trigger
			return GAC_ALARMSTATE_CLOSED;
		}
		return GAC_ALARMSTATE_NONE;
	}
	// altfel va treebui sa fac trigger
	// recalculez starting point
	new_StartTime = this->Year + dif * intervalInSeconds;
	UPDATE_END_ALARM(new_StartTime);
	if ((secondsFrom2000 >= new_StartTime) && (secondsFrom2000 < EndOfCurrentAlarm))
	{
		// face trigger
		if (this->Month == 1)
			result = GAC_ALARMSTATE_CLOSED;
		else
			result = GAC_ALARMSTATE_NONE;
		this->Year = new_StartTime;
		this->Month = 1; // o sa fac trigger din nou
		missed = dif - 1;
		LastTriggered = EndOfCurrentAlarm;
		return result | GAC_ALARMSTATE_TRIGGERED;
	}
	// altfel nu sunt in interval, calculez noul LastTriggered
	missed = 0;
	LastTriggered = this->Year + intervalInSeconds;
	if (this->Month == 1) {
		this->Month = 0;
		return GAC_ALARMSTATE_CLOSED;
	}
	else
		return GAC_ALARMSTATE_NONE;
}
GAC_TYPE_ALARMSTATE GApp::UI::Alarm::ShouldTrigger(GApp::Utils::DateTime *dt, unsigned int secondsFrom2000, unsigned int &missed)
{
	unsigned int result;

	switch (AlarmType)
	{
		case ALARM_TYPE_EXACT_DATE:	
			result = ShouldTrigger_ExactDate(dt, secondsFrom2000, missed); 
			break;
		case ALARM_TYPE_SPECIFIC_DATE: 
			result = ShouldTrigger_SpecificDate(dt, secondsFrom2000, missed);
			break;
		case ALARM_TYPE_SPECIFIC_MONTH_DAY: 
			missed = 0; 
			result = ShouldTrigger_MonthDay(dt);
			break;
		case ALARM_TYPE_SPECIFIC_WEEK_DAY: 
			result = ShouldTrigger_WeekDay(dt, missed); 
			break;
		case ALARM_TYPE_SPECIFIC_HOUR_MINUTE:
			result = ShouldTrigger_SpecificHourMinute(dt, secondsFrom2000, missed);
			break;

		case ALARM_TYPE_EVERY_X_MINUTES:
			result = ShouldTrigger_EveryXTime(this->Minute, secondsFrom2000, missed);
			break;
		case ALARM_TYPE_EVERY_X_HOURS:
			result = ShouldTrigger_EveryXTime(this->Hour, secondsFrom2000, missed);
			break;
		case ALARM_TYPE_EVERY_X_DAYS:
			result = ShouldTrigger_EveryXTime(this->Day, secondsFrom2000, missed);
			break;

		default:
			result = GAC_ALARMSTATE_NONE;
			break;
	}

	// OneTime check
	if ((result & GAC_ALARMSTATE_CLOSED))
	{
		if (OneTimeOnly)
		{
			Enabled = false;
			LastTriggered = GAC_ALARM_NOT_TRIGGERED;
			result = GAC_ALARMSTATE_CLOSED; // daca era si bit-ul de triggered setat il sterg ca sa nu se mai activeze inca o data alarma
		}
	}
	return result;
}