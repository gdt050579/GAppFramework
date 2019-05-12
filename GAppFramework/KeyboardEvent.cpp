#include "GApp.h"

bool GApp::Controls::KeyboardEvent::IsBackPressed()
{
	return ((KeyCode & (GAC_KEYBOARD_FLAG_DOWN|GAC_KEYBOARD_CODE_BACK))==GAC_KEYBOARD_CODE_BACK);
}