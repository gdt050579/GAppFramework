#include "GApp.h"

GApp::Utils::ThreadSyncRequest::ThreadSyncRequest()
{
#if defined(PLATFORM_DEVELOP) || defined(PLATFORM_WINDOWS_DESKTOP)
	syncEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
#elif defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
	pthread_cond_init(&SignalCondition, NULL);
	pthread_mutex_init(&Mutex,NULL);
	SignalState = false;
#else
    LOG_INFO("isDone = false (constructor)");
	isDone = false;
#endif
	Clear();
}
void GApp::Utils::ThreadSyncRequest::Create()
{
	Clear();
	Command = 0;

}
void GApp::Utils::ThreadSyncRequest::Finish()
{
	LOG_INFO("Command %d is finish !", Command);
	Command = 0; // neaaparat imediat in acest punct
#if defined(PLATFORM_DEVELOP) || defined(PLATFORM_WINDOWS_DESKTOP)
	SetEvent(syncEvent);
#elif defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
	pthread_mutex_lock(&Mutex);
	SignalState = false;
	pthread_cond_signal(&SignalCondition);
	pthread_mutex_unlock(&Mutex);
#else
    LOG_INFO("isDone = true");
	isDone = true;
#endif
}
void GApp::Utils::ThreadSyncRequest::Clear()
{
	Command = 0;
	for (unsigned int tr = 0; tr < 8; tr++)
		Values[tr].UInt64Value = 0;
	Result.UInt64Value = 0;
}
void GApp::Utils::ThreadSyncRequest::SendCommand(unsigned int command)
{
	LOG_INFO("Setting thread command to %d", command);
#if defined(PLATFORM_DEVELOP) || defined(PLATFORM_WINDOWS_DESKTOP)
	ResetEvent(syncEvent);
#elif defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
    LOG_INFO("Enter the lock");
	pthread_mutex_lock(&Mutex);
    LOG_INFO("Signal state is true");
	SignalState = true;
	pthread_mutex_unlock(&Mutex);
    LOG_INFO("Exist the lock");
#else
    LOG_INFO("isDone = false");
	isDone = false;
#endif
	Command = command;
}
void GApp::Utils::ThreadSyncRequest::Wait()
{
	LOG_INFO("Waiting for the other thread to finish ...");
#if defined(PLATFORM_DEVELOP) || defined(PLATFORM_WINDOWS_DESKTOP)
	WaitForSingleObject(syncEvent,INFINITE);
#elif defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
	pthread_mutex_lock(&Mutex);
	while (SignalState)
		pthread_cond_wait(&SignalCondition,&Mutex);
	pthread_mutex_unlock(&Mutex);
#else
	while (!isDone) {}
#endif
	LOG_INFO("The other thread had finished !");
}
