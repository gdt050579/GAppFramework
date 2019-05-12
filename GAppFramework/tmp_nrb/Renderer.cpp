#include "GApp.h"

#define MATRIX_POS(ptr,w,x,y,h)	(&ptr[((((h)-(y))-1)*(w))+((x)<<2)])
#define PIXEL_POS(x,y)			MATRIX_POS(Data->Data,LineStrip,x,y,Height)

#define RASTER_IMAGE_MAGIC		0x474D4952

double Lancsoz_Filter(double x, int a)
{
	if (x == 0)
	{
		return 1;
	}
	if (abs(x) > 0 && abs(x) < a)
	{
		return a * sin(3.1415926 * x)*sin(3.1415926 * x / a) / (3.1415926 * 3.1415926 * x * x);
	}
	return 0;
}

void Resample_NearestNeighbor	(GApp::Graphics::Renderer *dest,GApp::Graphics::Renderer *source,int xPoz,int yPoz,int w,int h, GApp::Graphics::PaintMode *p)
{
	if ((source->GetWidth()<1) || (source->GetHeight()<1) || (w<1) || (h<1))
		return;
	float xRap = (((float)source->GetWidth())/((float)w));
	float yRap = (((float)source->GetHeight())/((float)h));
	//LOG_INFO("Resample_NearestNeighbor: Source: %dx%d -> %dx%d - xRap=%f, yRap=%f [xPoz=%d,yPoz=%d - Texture:%dx%d]",source.GetWidth(),source.GetHeight(),w,h,xRap,yRap,xPoz,yPoz,dest.GetWidth(),dest.GetHeight());
	for (int x=0;x<w;x++)
	{
		for (int y=0;y<h;y++)
		{
			dest->SetPixel(x+xPoz,y+yPoz,source->GetPixel((int)((float)x*xRap),(int)((float)y*yRap)),p);
		}
	}
}
void Resample_Bilinear			(GApp::Graphics::Renderer *dest,GApp::Graphics::Renderer *source,int xPoz,int yPoz,int w,int h, GApp::Graphics::PaintMode *p)
{
	if ((source->GetWidth()<1) || (source->GetHeight()<1) || (w<1) || (h<1))
		return;
	float xRap = (((float)source->GetWidth())/((float)w));
	float yRap = (((float)source->GetHeight())/((float)h));
	//LOG_INFO("Resample_Bilinear: Source: %dx%d -> %dx%d - xRap=%f, yRap=%f [xPoz=%d,yPoz=%d - Texture:%dx%d]",source.GetWidth(),source.GetHeight(),w,h,xRap,yRap,xPoz,yPoz,dest.GetWidth(),dest.GetHeight());
	int xx,yy;
	float x_diff,y_diff;
	unsigned int A,B,C,D;
	unsigned int c_B,c_R,c_G,c_A;
	for (int x=0;x<w;x++)
	{
		for (int y=0;y<h;y++)
		{
			xx = (int)(xRap * x);
			yy = (int)(yRap * y);
			x_diff = (xRap * x) - xx;
			y_diff = (yRap * y) - yy;

			A = source->GetPixel(xx,yy);
			B = source->GetPixel(xx+1,yy);
			C = source->GetPixel(xx,yy+1);
			D = source->GetPixel(xx+1,yy+1);

			// Yb = Ab(1-w)(1-h) + Bb(w)(1-h) + Cb(h)(1-w) + Db(wh)
			c_B = (unsigned int)(GET_BLUE(A)*(1-x_diff)*(1-y_diff)+GET_BLUE(B)*x_diff*(1-y_diff)+GET_BLUE(C)*(1-x_diff)*y_diff+GET_BLUE(D)*x_diff*y_diff);
			c_R = (unsigned int)(GET_RED(A)*(1-x_diff)*(1-y_diff)+GET_RED(B)*x_diff*(1-y_diff)+GET_RED(C)*(1-x_diff)*y_diff+GET_RED(D)*x_diff*y_diff);
			c_G = (unsigned int)(GET_GREEN(A)*(1-x_diff)*(1-y_diff)+GET_GREEN(B)*x_diff*(1-y_diff)+GET_GREEN(C)*(1-x_diff)*y_diff+GET_GREEN(D)*x_diff*y_diff);
			c_A = (unsigned int)(GET_ALPHA(A)*(1-x_diff)*(1-y_diff)+GET_ALPHA(B)*x_diff*(1-y_diff)+GET_ALPHA(C)*(1-x_diff)*y_diff+GET_ALPHA(D)*x_diff*y_diff);

			dest->SetPixel(x+xPoz,y+yPoz,COLOR_ARGB(c_A,c_R,c_G,c_B),p);
		}
	}
}
void Resample_Bicubic			(GApp::Graphics::Renderer *dest,GApp::Graphics::Renderer *source,int xPoz,int yPoz,int w,int h, GApp::Graphics::PaintMode *p)
{
	//TO BE IMPLEMENTED
}
void Resample_Gaussian			(GApp::Graphics::Renderer *dest,GApp::Graphics::Renderer *source,int xPoz,int yPoz,int w,int h, GApp::Graphics::PaintMode *p)
{
	//TO BE IMPLEMENTED
}
void Resample_Lanczos			(GApp::Graphics::Renderer *dest,GApp::Graphics::Renderer *source,int xPoz,int yPoz,int w,int h, GApp::Graphics::PaintMode *p)
{
	float w_ratio = (((float)source->GetWidth()) / ((float)w));
	float h_ratio = (((float)source->GetHeight()) / ((float)h));
	unsigned int c_A, c_B, c_G, c_R;

	//dimensiunea kernel-ului
	//pentru redimensionari mici indicat este 2
	//pentru redimensionari mari indicat este 3, 4 in cazuri extreme!
	int a = 3;

	for (int c = 0; c < w; c++)
	{
		for (int r = 0; r < h; r++)
		{
			c_A = 0.0;
			c_B = 0.0;
			c_G = 0.0;
			c_R = 0.0;
			
			for (int i = (int)(c * w_ratio - a + 1); i <= (int)(c * w_ratio + a); i++)
			{
				if (i >= 0 && i < source->GetWidth())
				{
					for (int j = (int)(r * h_ratio - a + 1); j <= (int)(r * h_ratio + a); j++)
					{
						if (j >= 0 && j < source->GetHeight())
						{
							unsigned int point;
							point = source->GetPixel(i, j);

							c_A += (unsigned int)(GET_ALPHA(point) * Lancsoz_Filter(c * w_ratio - i, a) * Lancsoz_Filter(r * h_ratio - j, a));
							c_B += (unsigned int)(GET_BLUE(point) * Lancsoz_Filter(c * w_ratio - i, a) * Lancsoz_Filter(r * h_ratio - j, a));
							c_G += (unsigned int)(GET_GREEN(point) * Lancsoz_Filter(c * w_ratio - i, a) * Lancsoz_Filter(r * h_ratio - j, a));
							c_R += (unsigned int)(GET_RED(point) * Lancsoz_Filter(c * w_ratio - i, a) * Lancsoz_Filter(r * h_ratio - j, a));
						}
					}
				}
			}

			dest->SetPixel(c, r, COLOR_ARGB(c_A, c_R, c_G, c_B), p);
		}
	}

}

// --------------------------------------------------------------------------------------------------------------------------

GApp::Graphics::Renderer::Renderer()
{
	Data = NULL;
	Width = Height = LineStrip = 0;
}
bool			GApp::Graphics::Renderer::Create(GApp::Utils::Buffer *b,unsigned int w, unsigned int h)
{
	Dispose();
	
	CHECK(b!=NULL,false,"");
	CHECK((w>0) && (w<0xFFFFF),false,"");
	CHECK((h>0) && (h<0xFFFFF),false,"");
	CHECK(b->Resize(w*h*4),false,"");
	
	Data = b;
	Width = w;
	Height = h;
	LineStrip = Width*4;

	Clear();

	return true;
}
void			GApp::Graphics::Renderer::Dispose()
{
	Data = NULL;
	Width = Height = LineStrip = 0;
}
bool			GApp::Graphics::Renderer::Resize(unsigned int w,unsigned int h)
{
	while (true)
	{
		CHECKBK(Data!=NULL,"");
		CHECKBK((w>0) && (w<0xFFFFF),"");
		CHECKBK((h>0) && (h<0xFFFFF),"");
		CHECKBK(Data->Resize(w*h*4),"");
		Width = w;
		Height = h;
		LineStrip = Width*4;	
		Clear();
		return true;
	}
	Dispose();
	return false;
}
bool			GApp::Graphics::Renderer::Clear(unsigned int color)
{
	CHECK(Data!=NULL,false,"");
	unsigned int *p = (unsigned int *)(Data->Data);
	unsigned int *e = p + Width*Height;
	while (p<e)
	{
		(*p) = color;
		p++;
	}
	return true;
}
unsigned int	GApp::Graphics::Renderer::GetWidth()
{
	return Width;
}
unsigned int	GApp::Graphics::Renderer::GetHeight()
{
	return Height;
}
unsigned char*	GApp::Graphics::Renderer::GetPixelBuffer()
{
	if (Data==NULL)
		return NULL;
	return Data->GetBuffer();
}
unsigned int	GApp::Graphics::Renderer::GetPixel(int x,int y,unsigned int colorIfInvalidPosition)
{
	if ((x>=(int)Width) || (y>=(int)Height) || (x<0) || (y<0))
		return colorIfInvalidPosition;
	return *((unsigned int *)(PIXEL_POS(x,y)));
}
bool			GApp::Graphics::Renderer::SetPixel(int x,int y,unsigned int color,PaintMode *p)
{
	if (p!=NULL)
		p->ComputePixelPosition(x,y,x,y);
	if ((x>=(int)Width) || (y>=(int)Height) || (x<0) || (y<0))
	{
		//LOG_INFO("Invalid SetPixel to %d,%d [Width=%d,Height=%d]",x,y,Width,Height);
		return false;	
	}
	unsigned int *poz = ((unsigned int *)(PIXEL_POS(x,y)));
	if (p==NULL)
		(*poz) = color;
	else
		(*poz) = p->ComputePixelColor(x,y,*poz,color);
	return true;
}

bool			GApp::Graphics::Renderer::DrawImage(Renderer *r,int x,int y,PaintMode *p)
{
	CHECK(Data!=NULL,false,"");
	CHECK(r!=NULL,false,"");
	CHECK(r->Data!=NULL,false,"");

	if (p==NULL)
	{
		// nu am nici o operatie pe pixel de facut
		unsigned int *sursa;
		unsigned int *dest;
		unsigned int py,px;
		int yy,xx;
		for (py=0,yy=y;(py<r->Height) && (yy<(int)Height);py++,yy++)
		{
			if (yy<0)
				continue;
			sursa = (unsigned int *)MATRIX_POS(r->Data->Data,r->LineStrip,0,py,r->Height);
			for (px=0,xx=x;(px<r->Width) && (xx<(int)Width);px++,xx++,sursa++)
			{
				if (xx<0)
					continue;
				dest = (unsigned int *)PIXEL_POS(xx,yy);
				*dest = (*sursa);
			}
		}
	} else {
		// am operatii pe pixeli de facut
		for (int pY=0;pY<(int)r->Height;pY++)
		{
			for (int pX=0;pX<(int)r->Width;pX++)
			{
				SetPixel(x+pX,y+pY,r->GetPixel(pX,pY),p);
			}
		}
	}

	return true;
}
bool			GApp::Graphics::Renderer::DrawImage(Renderer *r,int x,int y,int newWidth,int newHeight,PaintMode *p)
{
	CHECK(Data!=NULL,false,"");
	CHECK(r!=NULL,false,"");

	if ((newWidth == r->Width) && (newHeight == r->Height))
		return DrawImage(r,x,y,p);
	if (p==NULL)
	{
		Resample_NearestNeighbor(this,r,x,y,newWidth,newHeight,p);
		return true;
	}
	GAC_TYPE_RESAMPLEMETHOD ResampleMode = p->GetResampleMode();
	switch (ResampleMode)
	{
		case GAC_RESAMPLEMETHOD_NEARESTNEIGHBOR:
			Resample_NearestNeighbor(this,r,x,y,newWidth,newHeight,p);
			return true;
		case GAC_RESAMPLEMETHOD_BILINEAR:
			Resample_Bilinear(this,r,x,y,newWidth,newHeight,p);
			return true;
		case GAC_RESAMPLEMETHOD_BICUBIC:
			Resample_Bicubic(this,r,x,y,newWidth,newHeight,p);
			return true;
		case GAC_RESAMPLEMETHOD_GAUSSIAN:
			Resample_Gaussian(this,r,x,y,newWidth,newHeight,p);
			return true;
		case GAC_RESAMPLEMETHOD_LANCZOS:
			Resample_Lanczos(this,r,x,y,newWidth,newHeight,p);
			return true;
	};
	RETURNERROR(false,"Unknwon resample algorithm : %d in GApp::Graphics::Renderer::DrawImage ",ResampleMode);	
}