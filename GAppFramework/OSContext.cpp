#include "GApp.h"

GApp::UI::OSContext::OSContext()
{
	unsigned char *p = (unsigned char *)&this->Data;
	for (int tr=0;tr<sizeof(GApp::UI::OSData);tr++)
		p[tr] = 0;
	Data.ApplicationIsClosing = false;
	Data.SnapshotType = GAC_SNAPSHOT_TYPE_NONE;
}
