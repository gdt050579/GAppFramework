#include "GApp.h"
#ifdef PLATFORM_DEVELOP
	#include "DevelopMode.h"
#endif

#define GROUP_COUNTER_DISABLED	0xFFFFFFFF

#define BEFORE_PAUSE_STATUS_NONE		0
#define BEFORE_PAUSE_STATUS_STOPPED		1
#define BEFORE_PAUSE_STATUS_ACTIVE		2



void GApp::UI::Counter::Init(unsigned int _index, unsigned int _interval, unsigned int _maxTimes, unsigned int _hash, unsigned int _priority, bool _enabled, const char *_name, const char * _group, unsigned int _gotoScene)
{
	this->Index = _index;
	this->Interval = _interval;
	this->MaxTimes = _maxTimes;
	this->Hash = _hash;
	this->Enabled = _enabled;
	this->Value = 0;
	this->Times = 0;
	this->Name = _name;
	this->Group = _group;
	this->Priority = _priority;
	this->ActionsCount = 0;
	this->GoToSceneID = _gotoScene;
	if (this->Interval > 0x7FF)
		this->Interval = 0x7FF;
	if (this->MaxTimes > 0x7FF)
		this->MaxTimes = 0x7FF;
	this->ConditionsCount = 0;
}
bool GApp::UI::Counter::AddAutoEnableStateCondition(unsigned int Condition, unsigned int Value, void* Ptr, bool applyAndOperator)
{
	CHECK(this->ConditionsCount < MAX_AUTO_ENABLE_STATE_CONDITIONS, false, "Too many conditions for %s counter (max allowed are %d) !", this->Name, MAX_AUTO_ENABLE_STATE_CONDITIONS);
	AutoEnableStateCondition *cnd = &this->Conditions[ConditionsCount];
	cnd->Condition = Condition;
	cnd->Value = Value;
	cnd->Ptr = Ptr;
	cnd->andOperation = applyAndOperator;
	ConditionsCount++;
	return true;
}
bool GApp::UI::Counter::Update()
{
	if (this->Enabled == false)
		return false;
	this->Value++;
	if (this->Value >= this->Interval)
	{
		this->Value = 0;
		this->Times++;
		if ((this->Times >= this->MaxTimes) && (this->MaxTimes != 0))
		{
			this->Times = this->MaxTimes;
			this->Enabled = false;
		}
#ifdef PLATFORM_DEVELOP
		PRINT_GLOBAL_COUNTER(this);
#endif
		return true;
	}
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER(this);
#endif
	return false;
}
bool GApp::UI::Counter::IsEnabled()
{
	return this->Enabled;
}
void GApp::UI::Counter::Enable()
{
	this->Enabled = true;
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER(this);
#endif
}
void GApp::UI::Counter::Disable()
{
	this->Enabled = false;
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER(this);
#endif
}
void GApp::UI::Counter::SetEnableState(bool value)
{
	if (value)
		Enable();
	else
		Disable();
}
void GApp::UI::Counter::UpdateEnableStateFromConditions(CoreReference CoreContext)
{
	if (ConditionsCount == 0)
		return;
	AutoEnableStateCondition *start = &this->Conditions[0];
	AutoEnableStateCondition *s = start;
	AutoEnableStateCondition *e = s + this->ConditionsCount;
	bool evaluationResult = false;
	bool applyAnd = false;
	bool localResult = false;
	for (; s < e;s++)
	{
		switch (s->Condition)
		{
		case AUTO_ENABLE_STATE_FB_APP_EXISTS:
			localResult = ((AppContextData*)Core.Api.Context)->OS->fnIsAppHandlerAvailable(GAC_APP_HANDLER_FACEBOOK_PAGE);
			break;
		case AUTO_ENABLE_STATE_ACTIONS_LESS:
			localResult = ((this->ActionsCount) < s->Value);
			break;
		case AUTO_ENABLE_STATE_ACTIONS_BIGGER:
			localResult = ((this->ActionsCount) > s->Value);
			break;
		case AUTO_ENABLE_STATE_AD_IS_LOADED:
			if (s->Ptr == NULL)
				localResult = false;
			else
				localResult = ((GApp::Platform::AdInterface*)s->Ptr)->IsLoaded();
			break;
		case AUTO_ENABLE_STATE_ADS_ARE_ENABLED:
			localResult = Core.AdsEnabled;
			break;
		case AUTO_ENABLE_STATE_ADS_ARE_DISABLED:
			localResult = !(Core.AdsEnabled);
			break;
		default:
			LOG_ERROR("Unknwon condition in Counter: %s (value = %d) -> as a result the counter will be evaluated to false !", this->Name, s->Condition);
			Disable();
			return;
		}
		if (s == start)
		{
			evaluationResult = localResult;			
		} else {
			if (applyAnd)
				evaluationResult &= localResult;
			else
				evaluationResult |= localResult;
		}
		applyAnd = s->andOperation;
	}
	LOG_INFO("Auto evaluation for Counter: %s => %d", this->Name, evaluationResult);
	if (evaluationResult)
		Enable();
	else
		Disable();
}
void GApp::UI::Counter::Reset()
{
	this->Value = 0;
	this->Enabled = true;
	this->Times = 0;
	this->ActionsCount = 0;
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER(this);
#endif
}
void GApp::UI::Counter::IncrementActionsCount()
{
	if (ActionsCount < 255)
		ActionsCount++;
	else
		ActionsCount = 255;
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER(this);
#endif
}
void			GApp::UI::Counter::ResetActionsCount()
{
	this->ActionsCount = 0;
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER(this);
#endif
}
unsigned int	GApp::UI::Counter::GetActionsCount()
{
	return this->ActionsCount;
}
unsigned int	GApp::UI::Counter::EncodeToPersistenValue()
{
	// value [cresc pana la interval] - 11 Biti (2048)
	// times [0 = infinit, ...]       - 11 Biti (2048)
	// actions [0 = niciodata,        - 8  Biti (256)
	unsigned int value;
	value = (this->Value & 0x7FF) | ((this->Times & 0x7FF) << 11) | ((this->ActionsCount & 0xFF) << 22);
	if (this->Enabled)
		value |= 0x80000000;
	return value;
}
void			GApp::UI::Counter::DecodeFromPersistentValue(unsigned int encoded_value)
{
	this->Enabled = ((encoded_value & 0x80000000) != 0);
	this->Value = encoded_value & 0x7FF;
	this->Times = (encoded_value >> 11) & 0x7FF;
	this->ActionsCount = (encoded_value >> 22) & 0xFF;
}

bool GApp::UI::CountersGroup::Init(CoreReference context, unsigned int index, const char * name, unsigned int _updateMethod, unsigned int _count, unsigned int _minTimeLimit, int _startTimeMethod, unsigned int _afterUpdateTimerStatus, unsigned int _gotoScene, bool _UseEnableConditionProperty)
{
	LastResult = NULL;
	Count = Capacity = UpdateMethod = 0;
	CHECK(_count > 0, false, "");
	Counters = new Counter*[_count];
	CHECK(Counters != NULL, false, "");
	Count = 0;
	Capacity = _count;
	UpdateMethod = _updateMethod;
	MinTimeLimit = _minTimeLimit;
	CumulativeTime = 0;
	StartTime = 0;
	CountTillEnabled = 0;
	CoreContext = context;
	TimerBefausePauseStatus = BEFORE_PAUSE_STATUS_NONE;
	Index = index;
	Name = name;
	StartTimeScene = _gotoScene;
	StartTimeMethod = _startTimeMethod;
	AfterUpdateTimerStatus = _afterUpdateTimerStatus;
	UseEnableConditionProperty = _UseEnableConditionProperty;
	
	if ((StartTimeMethod == COUNTER_GROUP_STARTTIME_METHOD_ON_APP_STARTS) && (MinTimeLimit>0))
		StartTimer();
	if (MinTimeLimit == 0)
		StartTimeMethod = COUNTER_GROUP_STARTTIME_METHOD_NONE;
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
	return true;
}
bool GApp::UI::CountersGroup::Add(Counter *element)
{
	CHECK(element != NULL, false, "");
	CHECK(Count < Capacity, false, "");
	Counters[Count] = element;
	Count++;
	return true;
}
void GApp::UI::CountersGroup::EnableCountersUsingEnableConditionProperty()
{
	LOG_INFO("CountersGroup - EnableCountersUsingEnableConditionProperty for %s", Name);
	for (unsigned int tr = 0; tr < this->Count; tr++)
	{
		if (Counters[tr]->ConditionsCount>0)
			Counters[tr]->UpdateEnableStateFromConditions(this->CoreContext);
	}
}
GApp::UI::Counter* GApp::UI::CountersGroup::Update()
{
	//LOG_INFO("GLOBALCOUNTERGROUP -> starting update !");
	if (CountTillEnabled == GROUP_COUNTER_DISABLED)
	{
#ifdef PLATFORM_DEVELOP
		PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
		return NULL;
	}
	if (CountTillEnabled>0)
	{
		CountTillEnabled--;
#ifdef PLATFORM_DEVELOP
		PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
		return NULL;
	}
	LastResult = NULL;
	bool timerIsStopped = false;
	if (this->MinTimeLimit > 0)
	{
		timerIsStopped = (StartTime == 0);
		if (!timerIsStopped)
			StopTimer();
		if (CumulativeTime<this->MinTimeLimit)
		{
			if (!timerIsStopped)
				StartTimer();
#ifdef PLATFORM_DEVELOP
			PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
			return NULL;
		}
	}
	if (this->UseEnableConditionProperty)
		EnableCountersUsingEnableConditionProperty();
	switch (UpdateMethod)
	{
		case 0: LastResult = UpdateAllCounters(); break;
		case 1: LastResult = UpdateBestCounter(); break;
	}
	if (this->MinTimeLimit > 0)
	{
		CumulativeTime = 0;
		//LOG_INFO("GLOBALCOUNTERGROUP -> After update status !");
		switch (AfterUpdateTimerStatus)
		{
			case 0: StopTimer(); break;  // ramane oprit
			case 1: StartTimer(); break; // se porneste tot timpul
			case 2: if (timerIsStopped) StopTimer(); else StartTimer(); break; // pastrez exact ce aveam inainte de update
		}
	}
	//LOG_INFO("GLOBALCOUNTERGROUP -> final steps !");
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
	return LastResult;
}
GApp::UI::Counter*	GApp::UI::CountersGroup::UpdateAllCounters()
{
	// se updateaza toate - dar se returneaza cel cu prioritatea cea mai mica
	Counter* ret = NULL;
	for (unsigned int tr = 0; tr < Count; tr++)
	{
		Counter* gc = Counters[tr];
		if (gc->Enabled == false)
			continue;
		if (gc->Value < gc->Interval)
			gc->Value++;
		if (gc->Value >= gc->Interval)
		{
			if (ret == NULL)
				ret = gc;
			else {
				if (gc->Priority < ret->Priority)
					ret = gc;
			}
		}
	}
	if (ret != NULL)
	{
		ret->Value = 0;
		ret->Times++;
		ret->Priority += 100;
		if ((ret->Times >= ret->MaxTimes) && (ret->MaxTimes != 0))
			ret->Enabled = false;
	}
#ifdef PLATFORM_DEVELOP
	for (unsigned int tr = 0; tr < Count; tr++)
	{
		Counter* gc = Counters[tr];
		PRINT_GLOBAL_COUNTER(gc);
	}
#endif
	return ret;
}
GApp::UI::Counter*	GApp::UI::CountersGroup::UpdateBestCounter()
{
	Counter* ret = NULL;
	for (unsigned int tr = 0; tr < Count; tr++)
	{
		Counter* gc = Counters[tr];
		if (gc->Enabled == false)
			continue;
		if (ret == NULL)
			ret = gc;
		else {
			if (gc->Priority < ret->Priority)
				ret = gc;
		}
	}
	if (ret != NULL)
	{
		if (ret->Update())
		{
			ret->Priority += 100;
			return ret;
		}
	}
	return NULL;
}
bool GApp::UI::CountersGroup::IsEnabled()
{
	return (CountTillEnabled == 0);
}
void GApp::UI::CountersGroup::Enable()
{
	CountTillEnabled = 0;
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
}
void GApp::UI::CountersGroup::Disable()
{
	CountTillEnabled = GROUP_COUNTER_DISABLED;	
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
}

void GApp::UI::CountersGroup::DisableForMultipleUpdates(unsigned int nrOfUpdate)
{
	CountTillEnabled = nrOfUpdate;
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
}
void GApp::UI::CountersGroup::DisableForNextUpdate()
{
	DisableForMultipleUpdates(1);
}
void GApp::UI::CountersGroup::StartTimer()
{
	if (this->MinTimeLimit == 0)
	{
#ifdef PLATFORM_DEVELOP
		PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
		return;
	}
	StartTime = (unsigned int)(((AppContextData*)Core.Api.Context)->OS->fnGetSystemTicksCount());
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
}
void GApp::UI::CountersGroup::StopTimer()
{
	if (this->MinTimeLimit == 0)
	{
#ifdef PLATFORM_DEVELOP
		PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
		return;
	}
	if (StartTime == 0)
	{
#ifdef PLATFORM_DEVELOP
		PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
		return;
	}
	unsigned int time = (unsigned int)(((AppContextData*)Core.Api.Context)->OS->fnGetSystemTicksCount());
	if (time > StartTime)
		CumulativeTime += (time - StartTime);
	else
		CumulativeTime += (0xFFFFFFFF - StartTime) + time;
	StartTime = 0; // oprit
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
}
void GApp::UI::CountersGroup::PauseTimer()
{
	if (this->MinTimeLimit == 0)
	{
#ifdef PLATFORM_DEVELOP
		PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
		return;
	}
	if (TimerBefausePauseStatus != BEFORE_PAUSE_STATUS_NONE)
	{
#ifdef PLATFORM_DEVELOP
		PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
		return;
	}	
	if (StartTime == 0)
		TimerBefausePauseStatus = BEFORE_PAUSE_STATUS_STOPPED; // e oprit
	else
		TimerBefausePauseStatus = BEFORE_PAUSE_STATUS_ACTIVE; // e pornit
	StopTimer();
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
}
void GApp::UI::CountersGroup::ResumeTimer()
{
	if (this->MinTimeLimit == 0)
	{
#ifdef PLATFORM_DEVELOP
		PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
		return;
	}
	if (TimerBefausePauseStatus == BEFORE_PAUSE_STATUS_ACTIVE)
		StartTimer();
	// altfel ramane oprit - nu s-a schimbat nimic
	TimerBefausePauseStatus = BEFORE_PAUSE_STATUS_NONE;
	// resetez statusul
#ifdef PLATFORM_DEVELOP
	PRINT_GLOBAL_COUNTER_GROUP(this);
#endif
}