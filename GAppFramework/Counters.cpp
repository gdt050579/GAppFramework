#include "GApp.h"

#define COUNTER_INIT(name,type) \
	void GApp::Utils::name::Init(unsigned int method, type _min, type _max, type _step,unsigned int flags) { \
	min = GAC_MIN(_min, _max); \
	max = GAC_MAX(_min, _max); \
	if (method!=GAC_COUNTER_KEEP_VALUE_CONST) \
		Value = min; \
	if (method == GAC_COUNTER_MIN) Value = max; \
	Flags = method; \
	step = GAC_MIN(max - min, GAC_ABS(_step)); \
	SetFlags(flags); \
}

#define COUNTER_SETFLAGS(name,type) \
	void GApp::Utils::name::SetFlags(unsigned int _flags) { \
	if (_flags & GAC_COUNTER_FLAG_ASCENDENT) step = GAC_ABS(step); \
	if (_flags & GAC_COUNTER_FLAG_DESCENDENT) step = -(GAC_ABS(step)); \
	if ((_flags & GAC_COUNTER_FLAG_START_FROM_MAX) && ((Flags & 0xFF)!=GAC_COUNTER_KEEP_VALUE_CONST)) Value = max; \
	if ((_flags & GAC_COUNTER_FLAG_START_FROM_MIN) && ((Flags & 0xFF) != GAC_COUNTER_KEEP_VALUE_CONST)) Value = min; \
	Flags = (_flags & 0xFFFFFF00) | (Flags & 0xFF); \
}

#define COUNTER_UPDATE(name,type) \
	bool GApp::Utils::name::Update() { \
	switch (Flags & 0xFF) \
	{ \
		case GAC_COUNTER_MIN: \
			Value = GAC_MAX(Value-step,min); \
			return ((Flags & GAC_COUNTER_FLAG_IGNORE_MIN_MARGIN_NOTIFICATION) == 0) && (Value == min); \
		case GAC_COUNTER_MAX: \
			Value = GAC_MIN(Value + step, max); \
			return ((Flags & GAC_COUNTER_FLAG_IGNORE_MAX_MARGIN_NOTIFICATION) == 0) && (Value == max); \
		case GAC_COUNTER_KEEP_VALUE_CONST: \
			min = GAC_MIN(min+step,max); \
			return ((Flags & GAC_COUNTER_FLAG_IGNORE_MAX_MARGIN_NOTIFICATION) == 0) && (max == min); \
		case GAC_COUNTER_CIRCULAR: \
			if (step > 0) { \
				if (Value<max) { \
					Value += step; \
					if (Value >= max) { Value = max; return ((Flags & GAC_COUNTER_FLAG_IGNORE_MAX_MARGIN_NOTIFICATION) == 0); } \
				} else { \
					Value = min; \
					return false; \
				} \
				return false; \
			} \
			if (step < 0) { \
				if (Value>min) { \
					Value += step; \
					if (Value <= min) { Value = min; return ((Flags & GAC_COUNTER_FLAG_IGNORE_MIN_MARGIN_NOTIFICATION) == 0); } \
				} else { \
					Value = max; \
					return false; \
					} \
			} \
			return false; \
		case GAC_COUNTER_MIN_MAX: \
			if (step > 0) { \
				Value += step; \
				if (Value >= max) { Value = max; step = -step; return ((Flags & GAC_COUNTER_FLAG_IGNORE_MAX_MARGIN_NOTIFICATION) == 0); } \
				return false; \
			} \
			if (step < 0) { \
				Value += step; \
				if (Value <= min) { Value = min; step = -step; return ((Flags & GAC_COUNTER_FLAG_IGNORE_MIN_MARGIN_NOTIFICATION) == 0); } \
				return false; \
			} \
			return false; \
	} \
	return false; \
}

#define COUNTER_GETTYPE(name) GAC_COUNTER_TYPE GApp::Utils::name::GetType() { return Flags & 0xFF; }
#define COUNTER_DISABLE(name) void GApp::Utils::name::Disable() { Flags = 0; }
#define COUNTER_ISENABLED(name) bool GApp::Utils::name::IsEnabled() { return (Flags & 0xFF)!=0 ; }
#define COUNTER_CONSTRUCTOR(name) GApp::Utils::name::name() { min = max = step = Value = 0; Flags = 0; }
//=====================================================================================
void GApp::Utils::CircularCounter::SetInterval(int _min, int _max)
{
	min = GAC_MIN(_min, _max);
	max = GAC_MAX(_min, _max);
	Value = min;
}
bool GApp::Utils::CircularCounter::Increment()
{
	Value++;
	if (Value > max)
	{
		Value = min;
		return true;
	}
	return false;
}
bool GApp::Utils::CircularCounter::Decrement()
{
	Value--;
	if (Value < min)
	{
		Value = max;
		return true;
	}
	return false;
}
void GApp::Utils::UpDownCounter::SetInterval(int _min, int _max)
{
	min = GAC_MIN(_min, _max);
	max = GAC_MAX(_min, _max);
	Value = min;
	add = 1;
}
bool GApp::Utils::UpDownCounter::Update()
{
	Value += add;
	if (Value >= max)
	{
		Value = max;
		add = -1;
		return true;
	}
	if (Value <= min)
	{
		Value = min;
		add = 1;
		return true;
	}
	return false;
}
void GApp::Utils::UpDownCounter::SetDirection(int dir)
{
	if (dir >= 0)
		add = 1;
	else
		add = -1;
	if (Value >= max)
	{
		Value = max;
		add = -1;
	}
	if (Value <= min)
	{
		Value = min;
		add = 1;
	}
}

//=====================================================================================
COUNTER_INIT(NumericCounter, int);
COUNTER_SETFLAGS(NumericCounter, int);
COUNTER_UPDATE(NumericCounter, int);
COUNTER_GETTYPE(NumericCounter);
COUNTER_DISABLE(NumericCounter);
COUNTER_ISENABLED(NumericCounter);
COUNTER_CONSTRUCTOR(NumericCounter);
//=====================================================================================
COUNTER_INIT(FloatCounter, float);
COUNTER_SETFLAGS(FloatCounter, float);
COUNTER_UPDATE(FloatCounter, float);
COUNTER_GETTYPE(FloatCounter);
COUNTER_DISABLE(FloatCounter);
COUNTER_ISENABLED(FloatCounter);
COUNTER_CONSTRUCTOR(FloatCounter);