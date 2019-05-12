#include "GApp.h"

#define CLEAR_LOCAL_VARIABLES	{ VariableValueSize[0] = VariableValueSize[1] = VariableValueSize[2] = VariableValueSize[3] = 0; }

#define SHADER_MAGIC	0x44485347

unsigned int ComputeHash(const char *buffer)
{
	unsigned char	*str = (unsigned char *)buffer;
	unsigned int	Value = 0;
	unsigned int	a = 63689;
	for (; (*str)!=0; str++)
	{
		Value = Value * a + (*str);
		a *= 378551;
	}
	return Value;
}


GApp::Resources::Shader::Shader()
{
	int tr;

	ShadersIDs[0] = ShadersIDs[1] = 0;
	ProgramID = 0;
	LastError = 0;
	for (tr=0;tr<4;tr++)
	{
		Attributes[tr] = -1;
		Uniforms[tr] = -1;
		VariableValue[tr][0] = 0;
		VariableValue[tr][1] = 0;
		VariableValue[tr][2] = 0;
		VariableValue[tr][3] = 0;
		VariableValueSize[tr] = 0;
		UniformHash[tr] = 0;
	}

	Flags = GAC_RESOURCEFLAG_SHADER;
}
bool GApp::Resources::Shader::LoadShader(unsigned int ID, GLenum shaderType, const char *SourceCode)
{
	CHECK((ShadersIDs[ID] = glCreateShader(shaderType))!=0,false,"");
	while (true)
	{
		CHECK_GLERROR_BK(glShaderSource(ShadersIDs[ID], 1, (const char **)&SourceCode, NULL));
		CHECK_GLERROR_BK(glCompileShader(ShadersIDs[ID]));
		GLint compiled = 0;
		CHECK_GLERROR_BK(glGetShaderiv(ShadersIDs[ID], GL_COMPILE_STATUS, &compiled));
		if (!compiled)
		{
		#ifdef ENABLE_ERROR_LOGGING
			GLint infoLen = 0;
			glGetShaderiv(ShadersIDs[ID], GL_INFO_LOG_LENGTH, &infoLen);
			if ((infoLen>0) && (infoLen<1023))
			{
				char temp[1024];
				glGetShaderInfoLog(ShadersIDs[ID], infoLen, NULL, temp);
				LOG_ERROR("Shader Compile Error: %s",temp);
			} else {
				LOG_ERROR("Shader Compile Error: %d",LastError);
			}
			LOG_ERROR("Source Code = \n------------------------------------------------\n%s\n------------------------------------------\n",SourceCode);
		#endif
			break;
		}
		CHECK_GLERROR_BK(glAttachShader(ProgramID, ShadersIDs[ID]));
		return true;
	}
	if (ShadersIDs[ID]>0)
		glDeleteShader(ShadersIDs[ID]);
	ShadersIDs[ID] = 0;
    return false;
}
bool GApp::Resources::Shader::Create(CoreReference coreContext,unsigned int pos,unsigned int size)
{
	CLEAR_LOCAL_VARIABLES;
	Position = pos;
	Size = size;
	CoreContext = coreContext;	
	return true;
}
bool GApp::Resources::Shader::Load()
{
	LOG_INFO("Loading shader: %p (CoreContext=%p,Position=%d,Size=%d)", this, CoreContext, Position, Size);
	CHECK(CoreContext!=NULL,false,"Failed to load shader: %p",this);
	CHECK(Size>0,false,"");
	CHECK(API.ReadResource(Position,&Core.TempBufferBig,Size,GAC_ENCRYPTION_NONE,NULL),false,"");
	CHECK(Core.TempBufferBig.GetSize()>=17,false,"");
	unsigned int sz;
	unsigned int poz = 5;

	CHECK(Core.TempBufferBig.GetUInt32(0,0)==SHADER_MAGIC,false,"");
	//unsigned char *p = Core.TempBufferBig.GetBuffer();

	CHECK((sz = Core.TempBufferBig.GetUInt16(poz,0xFFFF))!=0xFFFF,false,"");
	poz+=2;
	CHECK((sz+poz)<Core.TempBufferBig.GetSize(),false,"");
	CHECK(Core.TempBufferBig.GetUInt8(poz+sz,0xFF)==0,false,"");
	char *v = (char *)(Core.TempBufferBig.GetBuffer()+poz);
	poz+=(sz+1);
	
	CHECK((sz = Core.TempBufferBig.GetUInt16(poz,0xFFFF))!=0xFFFF,false,"");
	poz+=2;
	CHECK((sz+poz)<Core.TempBufferBig.GetSize(),false,"");
	CHECK(Core.TempBufferBig.GetUInt8(poz+sz,0xFF)==0,false,"");
	char *f = (char *)(Core.TempBufferBig.GetBuffer()+poz);
	poz+=(sz+1);

	CHECK((sz = Core.TempBufferBig.GetUInt16(poz,0xFFFF))!=0xFFFF,false,"");
	poz+=2;
	CHECK((sz+poz)<Core.TempBufferBig.GetSize(),false,"");
	CHECK(Core.TempBufferBig.GetUInt8(poz+sz,0xFF)==0,false,"");
	char *a = (char *)(Core.TempBufferBig.GetBuffer()+poz);
	poz+=(sz+1);

	CHECK((sz = Core.TempBufferBig.GetUInt16(poz,0xFFFF))!=0xFFFF,false,"");
	poz+=2;
	CHECK((sz+poz)<Core.TempBufferBig.GetSize(),false,"");
	CHECK(Core.TempBufferBig.GetUInt8(poz+sz,0xFF)==0,false,"");
	char *u = (char *)(Core.TempBufferBig.GetBuffer()+poz);
	
	if (a[0]==0)
		a = NULL;
	if (u[0]==0)
		u = NULL;

	CHECK(Create(v,f,a,u),false,"");
	return true;
}
bool GApp::Resources::Shader::Create(const char *vertexShader,const char *fragmentShader,const char *atributeList,const char *uniformList)
{
	CLEAR_LOCAL_VARIABLES;
	CHECK((ProgramID = glCreateProgram())>0,false,"");
	while (true)
	{
		CHECKBK(LoadShader(0, GL_VERTEX_SHADER, vertexShader),"");
		CHECKBK(LoadShader(1, GL_FRAGMENT_SHADER, fragmentShader),"");
		CHECK_GLERROR_BK(glLinkProgram(ProgramID));
		GLint linkStatus = GL_FALSE;
		CHECK_GLERROR_BK(glGetProgramiv(ProgramID, GL_LINK_STATUS, &linkStatus));
		if (linkStatus!=GL_TRUE)
		{
		#ifdef ENABLE_ERROR_LOGGING
			GLint infoLen = 0;
			glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &infoLen);
			if ((infoLen>0) && (infoLen<2047))
			{
				char temp[2048];
				glGetProgramInfoLog(ProgramID, infoLen, NULL, temp);
				LOG_ERROR("Program Compile Error: %s",temp);
			} else {
				LOG_ERROR("Program Compile Error: %d\n",LastError);
			}
		#endif
			break;
		}

		// procesez listele de atribute si uniform
		char tempName[64];
		int index,poz;
		const char *curentChar;
		bool parsing_ok;

		// atributele
		if (atributeList!=NULL)
		{
			poz=0;
			index=0;
			curentChar = atributeList;
			parsing_ok = false;
			while (true)
			{
				if (((*curentChar)==',') || ((*curentChar)==0))
				{
					CHECKBK(index<4,"");
					tempName[poz]=0;
					LOG_INFO("Linking attribute: %s",tempName);
					if (index==0)
					{
						// primul trebuie sa fie pozitia 
						CHECKBK((tempName[0]=='p') && (tempName[1]=='o') && (tempName[2]=='s') && (tempName[3]==0),"");
					}
					poz = 0;
					CHECK_GLERROR_BK(Attributes[index] = glGetAttribLocation(ProgramID, tempName));
					CHECKBK(Attributes[index]>=0,"");
					LOG_INFO("Attribute %s linked OK (index=%d, OpenGL-ID=%d)",tempName,index,Attributes[index]);
					index++;
					if ((*curentChar)==0)
					{
						parsing_ok = true;
						break;
					}
				} else {
					tempName[poz++] = (*curentChar);
					CHECKBK(poz<60,"");
				}
				curentChar++;
			}
			CHECKBK(parsing_ok,"");
		}

		// uniforms
		if (uniformList)
		{
			poz=0;
			index=0;
			curentChar = uniformList;
			parsing_ok = false;
			while (true)
			{
				if (((*curentChar)==',') || ((*curentChar)==0))
				{
					CHECKBK(index<4,"");
					tempName[poz]=0;
					LOG_INFO("Linking uniform: %s",tempName);
					poz = 0;
					CHECK_GLERROR_BK(Uniforms[index] = glGetUniformLocation(ProgramID, tempName));
					CHECKBK(Uniforms[index]>=0,"");
					LOG_INFO("Uniform %s linked OK (index=%d, OpenGL-ID=%d)",tempName,index,Uniforms[index]);
					UniformHash[index] = ComputeHash(tempName);
					index++;
					if ((*curentChar)==0)
					{
						parsing_ok = true;
						break;
					}
				} else {
					tempName[poz++] = (*curentChar);
					CHECKBK(poz<60,"");
				}
				curentChar++;
			}
			CHECKBK(parsing_ok,"");
		}
		CHECK(OnCreate(), false, "Failed in OnCreate ()");
		return true;
	}
	Unload();
	return false;
}
bool GApp::Resources::Shader::Unload()
{
	int tr;
	if (ShadersIDs[0]>0)
		glDeleteShader(ShadersIDs[0]);
	if (ShadersIDs[1]>0)
		glDeleteShader(ShadersIDs[1]);
	ShadersIDs[0] = ShadersIDs[1] = 0;
	if (ProgramID>0)
		glDeleteProgram(ProgramID);
	for (tr=0;tr<4;tr++)
	{
		Attributes[tr] = -1;
		Uniforms[tr] = -1;
		VariableValue[tr][0] = 0;
		VariableValue[tr][1] = 0;
		VariableValue[tr][2] = 0;
		VariableValue[tr][3] = 0;
		VariableValueSize[tr] = 0;
	}
	ProgramID = 0;	
	return true;
}
bool GApp::Resources::Shader::Use()
{
	CHECK(ProgramID>0,false,"");
	CHECK_GLERROR(glUseProgram(ProgramID));
	return true;
}
bool GApp::Resources::Shader::SetPosition(float *dataPtr)
{
	CHECK(Attributes[0]>=0,false,"");
	CHECK_GLERROR(glVertexAttribPointer(Attributes[0], 2, GL_FLOAT, GL_FALSE, 0, dataPtr));	
	CHECK_GLERROR(glEnableVertexAttribArray(Attributes[0]));
	return true;
}
bool GApp::Resources::Shader::SetAttributeData(unsigned int index,float *dataPtr,int unitSize)
{
	CHECK(Attributes[index]>=0,false,"");
	CHECK_GLERROR(glVertexAttribPointer(Attributes[index], unitSize, GL_FLOAT, GL_FALSE, 0, dataPtr));
	CHECK_GLERROR(glEnableVertexAttribArray(Attributes[index]));
	return true;
}
bool GApp::Resources::Shader::SetUniformData(unsigned int index,GLfloat v1)
{
	CHECK(Uniforms[index]>=0,false,"");
	CHECK_GLERROR(glUniform1f(Uniforms[index],v1));
	return true;
}
bool GApp::Resources::Shader::SetUniformData(unsigned int index,GLfloat v1,GLfloat v2)
{
	CHECK(Uniforms[index]>=0,false,"");
	CHECK_GLERROR(glUniform2f(Uniforms[index],v1,v2));
	return true;
}
bool GApp::Resources::Shader::SetUniformData(unsigned int index,GLfloat v1,GLfloat v2,GLfloat v3)
{
	CHECK(Uniforms[index]>=0,false,"");
	CHECK_GLERROR(glUniform3f(Uniforms[index],v1,v2,v3));
	return true;
}
bool GApp::Resources::Shader::SetUniformData(unsigned int index,GLfloat v1,GLfloat v2,GLfloat v3,GLfloat v4)
{
	CHECK(Uniforms[index]>=0,false,"");
	CHECK_GLERROR(glUniform4f(Uniforms[index],v1,v2,v3,v4));
	return true;
}
bool GApp::Resources::Shader::SetVariableValue(unsigned int index,GLfloat v1,bool clearAfterUsage)
{
	CHECK((index<4) && (Uniforms[index]>=0),false,"");
	VariableValue[index][0] = v1;
	VariableValueSize[index] = 1;
	if (clearAfterUsage)
		VariableValueSize[index] |= 128;
	return true;
}
bool GApp::Resources::Shader::SetVariableValue(unsigned int index,GLfloat v1,GLfloat v2,bool clearAfterUsage)
{
	CHECK((index<4) && (Uniforms[index]>=0),false,"");
	VariableValue[index][0] = v1;
	VariableValue[index][1] = v2;
	VariableValueSize[index] = 2;
	if (clearAfterUsage)
		VariableValueSize[index] |= 128;
	return true;
}
bool GApp::Resources::Shader::SetVariableValue(unsigned int index,GLfloat v1,GLfloat v2,GLfloat v3,bool clearAfterUsage)
{
	CHECK((index<4) && (Uniforms[index]>=0),false,"");
	VariableValue[index][0] = v1;
	VariableValue[index][1] = v2;
	VariableValue[index][2] = v3;
	VariableValueSize[index] = 3;
	if (clearAfterUsage)
		VariableValueSize[index] |= 128;
	return true;
}
bool GApp::Resources::Shader::SetVariableValue(unsigned int index,GLfloat v1,GLfloat v2,GLfloat v3,GLfloat v4,bool clearAfterUsage)
{
	CHECK((index<4) && (Uniforms[index]>=0),false,"");
	VariableValue[index][0] = v1;
	VariableValue[index][1] = v2;
	VariableValue[index][2] = v3;
	VariableValue[index][3] = v4;
	VariableValueSize[index] = 4;
	if (clearAfterUsage)
		VariableValueSize[index] |= 128;
	return true;
}
bool GApp::Resources::Shader::SetVariableColor(unsigned int index,unsigned int color,bool clearAfterUsage)
{
	return SetVariableValue(index,RED(color),GREEN(color),BLUE(color),ALPHA(color),clearAfterUsage);
}
bool GApp::Resources::Shader::UpdateVaribleValues(bool UseDerivedShaderVariable)
{
	unsigned char sz;
	if (UseDerivedShaderVariable)
	{
		CHECK(OnUpdate(), false, "Failed to call on OnUpdate for Shader: %s", Name);
	}
	for (int tr=0;tr<4;tr++)
	{
		sz = VariableValueSize[tr] & 0x7F; 
		if (sz==0)
			continue;
		if (Uniforms[tr]<0)
			continue;
		switch (sz)
		{
			case 1: CHECK_GLERROR(glUniform1f(Uniforms[tr],VariableValue[tr][0])); break;
			case 2: CHECK_GLERROR(glUniform2f(Uniforms[tr],VariableValue[tr][0],VariableValue[tr][1])); break;
			case 3: CHECK_GLERROR(glUniform3f(Uniforms[tr],VariableValue[tr][0],VariableValue[tr][1],VariableValue[tr][2])); break;
			case 4: CHECK_GLERROR(glUniform4f(Uniforms[tr],VariableValue[tr][0],VariableValue[tr][1],VariableValue[tr][2],VariableValue[tr][3])); break;
		}
		if ((VariableValueSize[tr] & 128)!=0)
			VariableValueSize[tr] = 0;
	}
	return true;
}
int	GApp::Resources::Shader::GetUniformIndex(const char *name)
{
	unsigned int hash = ComputeHash(name);
	if (hash == 0)
		return -1;
	for (int tr = 0; tr < 4; tr++)
	{
		if ((UniformHash[tr] == hash) && (Uniforms[tr]>=0))
			return tr;
	}
	return -1;
}
