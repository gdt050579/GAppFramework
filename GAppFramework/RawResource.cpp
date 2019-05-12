#include "GApp.h"


bool GApp::Resources::RawResource::Create(CoreReference coreContext,unsigned int pos,unsigned int size,bool storeLocally)
{
	CHECK(coreContext!=NULL,false,"");
	CHECK(size!=0,false,"");
	CoreContext = coreContext;
	Position = pos;
	Size = size;
	Flags = GAC_RESOURCEFLAG_RAW;
	if (storeLocally)
		Flags |= GAC_RESOURCEFLAG_LOCALCOPY;
	return true;
}
bool GApp::Resources::RawResource::CopyRawDataBuffer(GApp::Utils::Buffer *buffer)
{
	CHECK(CoreContext!=NULL,false,"");
	CHECK(Size!=0,false,"");
	return API.ReadResource(Position,buffer,Size,GAC_ENCRYPTION_NONE,NULL);
}
GApp::Utils::Buffer* GApp::Resources::RawResource::GetRawDataBuffer()
{
	if ((Flags & GAC_RESOURCEFLAG_LOCALCOPY)!=0)
	{
		if (Data.Data!=NULL)
			return &Data;
		CHECK(CopyRawDataBuffer(&Data),NULL,"Unable to load raw data from resource file %s",Name);
		return &Data;
	} else {
		CHECK(CoreContext!=NULL,NULL,"Invalid (NULL) core context for %s",Name);
		CHECK(CopyRawDataBuffer(&(Core.TempBufferBig)),NULL,"Unable to load raw data from resource file (%s)",Name);		
		return &(Core.TempBufferBig);
	}
}
bool GApp::Resources::RawResource::Load()
{
	if ((Flags & GAC_RESOURCEFLAG_LOCALCOPY)==0)
		return true;
	if (Data.Data!=NULL)
		return true;
	CHECK(CopyRawDataBuffer(&Data),false,"Unable to load raw data from resource file %s",Name);
	return true;
}
bool GApp::Resources::RawResource::Unload()
{
	Data.Free();
	ProfileContext=NULL;
	return true;	
}