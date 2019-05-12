#include "GApp.h"

#define MATRIX_POS(ptr,w,x,y,h)	(&ptr[((((h)-(y))-1)*(w))+((x)<<2)])
#define PIXEL_POS(x,y)			MATRIX_POS(Data->Data,LineStrip,x,y,Height)

#define RASTER_IMAGE_MAGIC		0x474D4952

#define LANCSOZ_KERNEL_SIZE		3


double Lancsoz_Filter(double x, int a)
{
	if (x == 0)
		return 1.0;
	if ((x!=0) && (a>0) && (x<a))
		return a * sin(3.1415926 * x)*sin(3.1415926 * x / a) / (3.1415926 * 3.1415926 * x * x);
	return 0.0;
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
void Resample_Bilinear_Fast		(GApp::Graphics::Renderer *dest, GApp::Graphics::Renderer *source, int xPoz, int yPoz, int w, int h)
{
	if ((source->GetWidth() < 1) || (source->GetHeight() < 1) || (w < 1) || (h < 1))
	{
		LOG_ERROR("Invalid size for bilinear filter - size to small !");
		return;
	}
	if ((xPoz<0) || (yPoz<0) || (xPoz + w>dest->GetWidth()) || (yPoz + h>dest->GetHeight()))
	{
		LOG_ERROR("Invalid size for bilinear filter [xPoz=%d,yPoz=%d,w=%d,h=%d,DestWidth=%d,DestHeight=%d]",xPoz,yPoz,w,h,dest->GetWidth(),dest->GetHeight());
		return;
	}
	float xRap = (((float)source->GetWidth()) / ((float)w));
	float yRap = (((float)source->GetHeight()) / ((float)h));
	//LOG_INFO("Resample_Bilinear: Source: %dx%d -> %dx%d - xRap=%f, yRap=%f [xPoz=%d,yPoz=%d - Texture:%dx%d]",source.GetWidth(),source.GetHeight(),w,h,xRap,yRap,xPoz,yPoz,dest.GetWidth(),dest.GetHeight());
	int xx, yy;
	float x_diff, y_diff, m_xDiff, m_yDiff, v1, v2, v3, v4;
	//unsigned int A, B, C, D;
	unsigned char *A;
	unsigned char *B;
	unsigned char *C;
	unsigned char *D;


	unsigned char *destPixels = (unsigned char *)dest->GetPixelBuffer();
	unsigned int destStride = (dest->GetWidth() << 2);
	//destPixels += (destStride * (dest->GetHeight()-h)) + (xPoz << 2);
	destPixels += ((destStride * (dest->GetHeight() - (1 + yPoz))) + (xPoz << 2));
	unsigned char *cDest;

	unsigned int sourceStride = (source->GetWidth() << 2);
	int sourceW, sourceH;// , sourceW1, sourceH1;
	sourceW = source->GetWidth() - 1;
	sourceH = source->GetHeight() - 1;
	//sourceW1 = sourceW + 1;
	//sourceH1 = sourceH + 1;

	// xRap*x == source->GetWidth()-2
	//int wSafe = (source->GetWidth() - 1) / xRap;
	//int hSafe = (source->GetHeight() - 1) / yRap;

	unsigned char NoColor[4] = { 0, 0, 0, 0 };

	for (int y = 0; y<h; y++)
	{
		cDest = destPixels;

		for (int x = 0; x<w; x++)
		{
			xx = (int)(xRap * x);
			yy = (int)(yRap * y);
			x_diff = (xRap * x) - xx;
			y_diff = (yRap * y) - yy;
			m_xDiff = 1.0f - x_diff;
			m_yDiff = 1.0f - y_diff;
			v1 = m_xDiff * m_yDiff;
			v2 = x_diff * m_yDiff;
			v3 = m_xDiff * y_diff;
			v4 = x_diff * y_diff;

			if ((xx < sourceW) && (yy < sourceH))
			{
				A = source->GetUnSafePixelPointer(xx, yy);
				B = A + 4;
				C = A - sourceStride;
				D = C + 4;
			}
			else {
				A = source->GetSafePixelPointer(xx, yy, NoColor);
				B = source->GetSafePixelPointer(xx + 1, yy, NoColor);
				C = source->GetSafePixelPointer(xx, yy + 1, NoColor);
				D = source->GetSafePixelPointer(xx + 1, yy + 1, NoColor);
			}
			// Yb = Ab(1-w)(1-h) + Bb(w)(1-h) + Cb(h)(1-w) + Db(wh)
			// consider formatul ARGB

			(*cDest) = (unsigned char)(A[0] * v1 + B[0] * v2 + C[0] * v3 + D[0] * v4);
			cDest++;
			(*cDest) = (unsigned char)(A[1] * v1 + B[1] * v2 + C[1] * v3 + D[1] * v4);
			cDest++;
			(*cDest) = (unsigned char)(A[2] * v1 + B[2] * v2 + C[2] * v3 + D[2] * v4);
			cDest++;
			(*cDest) = (unsigned char)(A[3] * v1 + B[3] * v2 + C[3] * v3 + D[3] * v4);
			cDest++;
		}
		destPixels -= destStride;
	}
}
float Bicubic_GetCoef(float x)
{
	float f = x;
	if (f < 0.0)
	{
		f = -f;
	}
	if (f >= 0.0 && f <= 1.0)
	{
		return (2.0 / 3.0) + (0.5) * (f* f * f) - (f*f);
	}
	else
	{
		if (f > 1.0 && f <= 2.0)
		{
			f = 2.0f - f;
			return 1.0 / 6.0 * f*f*f;
		}
	}
	return 1.0;
}

void Resample_Bicubic(GApp::Graphics::Renderer *dest, GApp::Graphics::Renderer *source, int xPoz, int yPoz, int w, int h, GApp::Graphics::PaintMode *p)
{
	int i, j, m, n;
	int x, y;
	float dx, dy;
	float tx, ty;
	float Bmdx;
	float Bndy;
	unsigned int Acolor, Rcolor, Bcolor, Gcolor;
	unsigned int point;

	tx = (float)source->GetWidth() / w;
	ty = (float)source->GetHeight() / h;

	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			x = (int)(tx * j);
			y = (int)(ty * i);
			dx = tx*j - x;
			dy = ty*i - y;

			Acolor = 0;
			Rcolor = 0;
			Gcolor = 0;
			Bcolor = 0;

			for (m = -1; m <= 2; m++)
			{
				Bmdx = Bicubic_GetCoef(m - dx);
				for (n = -1; n <= 2; n++)
				{
					point = source->GetPixel(x + m, y + n);
					Bndy = Bicubic_GetCoef(dy - n) * Bmdx;
					Acolor += (GET_ALPHA(point))*Bndy;
					Rcolor += GET_RED(point)*Bndy;
					Gcolor += GET_GREEN(point)*Bndy;
					Bcolor += GET_BLUE(point)*Bndy;
				}
			}

			dest->SetPixel(j+xPoz, i+yPoz, COLOR_ARGB(Acolor, Rcolor, Gcolor, Bcolor), p);
		}
	}
}
void Resample_Gaussian			(GApp::Graphics::Renderer *dest,GApp::Graphics::Renderer *source,int xPoz,int yPoz,int w,int h, GApp::Graphics::PaintMode *p)
{
	//TO BE IMPLEMENTED
}
void Resample_Lanczos			(GApp::Graphics::Renderer *dest,GApp::Graphics::Renderer *source,int xPoz,int yPoz,int w,int h, GApp::Graphics::PaintMode *p)
{
	if ((source->GetWidth()<1) || (source->GetHeight()<1) || (w<1) || (h<1))
		return;
	float w_ratio = (((float)source->GetWidth()) / ((float)w));
	float h_ratio = (((float)source->GetHeight()) / ((float)h));
	double c_A, c_B, c_G, c_R;
	int s_W,s_H;

	//dimensiunea kernel-ului
	//pentru redimensionari mici indicat este 2
	//pentru redimensionari mari indicat este 3, 4 in cazuri extreme!
	//int a = 3;
	s_W = source->GetWidth();
	s_H = source->GetHeight();

	for (int c = 0; c < w; c++)
	{
		for (int r = 0; r < h; r++)
		{
			c_A = 0.0;
			c_B = 0.0;
			c_G = 0.0;
			c_R = 0.0;
			
			for (int i = (int)(c * w_ratio - LANCSOZ_KERNEL_SIZE + 1); i <= (int)(c * w_ratio + LANCSOZ_KERNEL_SIZE); i++)
			{
				if (i >= 0 && i < s_W)
				{
					for (int j = (int)(r * h_ratio - LANCSOZ_KERNEL_SIZE + 1); j <= (int)(r * h_ratio + LANCSOZ_KERNEL_SIZE); j++)
					{
						if (j >= 0 && j < s_H)
						{
							unsigned int point;
							point = source->GetPixel(i, j);

							c_A += (GET_ALPHA(point) * Lancsoz_Filter(c * w_ratio - i, LANCSOZ_KERNEL_SIZE) * Lancsoz_Filter(r * h_ratio - j, LANCSOZ_KERNEL_SIZE));
							c_B += (GET_BLUE(point) * Lancsoz_Filter(c * w_ratio - i, LANCSOZ_KERNEL_SIZE) * Lancsoz_Filter(r * h_ratio - j, LANCSOZ_KERNEL_SIZE));
							c_G += (GET_GREEN(point) * Lancsoz_Filter(c * w_ratio - i, LANCSOZ_KERNEL_SIZE) * Lancsoz_Filter(r * h_ratio - j, LANCSOZ_KERNEL_SIZE));
							c_R += (GET_RED(point) * Lancsoz_Filter(c * w_ratio - i, LANCSOZ_KERNEL_SIZE) * Lancsoz_Filter(r * h_ratio - j, LANCSOZ_KERNEL_SIZE));
						}
					}
				}
			}

			dest->SetPixel(c+xPoz, r+yPoz, COLOR_ARGB((unsigned int)c_A, (unsigned int)c_R, (unsigned int)c_G, (unsigned int)c_B), p);
		}
	}
}

void Resample_CustomFast(GApp::Graphics::Renderer *dest, GApp::Graphics::Renderer *source, int xPoz, int yPoz, int w, int h, GApp::Graphics::PaintMode *p)
{
	float xRap = ((float)source->GetWidth() / (float)w);
	float yRap = ((float)source->GetHeight() / (float)h);

	int x, y;
	float x1, x2, y1, y2;
	int xDown, xUp, yDown, yUp;
	int i, j;
	unsigned int Acolor, Rcolor, Bcolor, Gcolor;
	unsigned int point;

	if (xRap > 1 && yRap > 1)
	{
		int contor;

		for (x = 0; x < w; x++)
		{
			for (y = 0; y < h; y++)
			{
				Acolor = 0; Rcolor = 0;
				Bcolor = 0; Gcolor = 0;

				x1 = x * xRap; x2 = (x + 1) * xRap;
				y1 = y * yRap; y2 = (y + 1) * yRap;

				if ((floor(x1 + 1) - x1) >(x1 - floor(x1))) xDown = floor(x1);
				else xDown = floor(x1 + 1);
				if ((floor(x2 + 1) - x2) > (x2 - floor(x2))) xUp = floor(x2);
				else xUp = floor(x2 + 1);
				if ((floor(y1 + 1) - y1) > (y1 - floor(y1))) yDown = floor(y1);
				else yDown = floor(y1 + 1);
				if ((floor(y2 + 1) - y2) > (y2 - floor(y2))) yUp = floor(y2);
				else yUp = floor(y2 + 1);

				for (i = xDown; i <= xUp; i++)
				{
					for (j = yDown; j <= yUp; j++)
					{
						point = source->GetPixel(i, j);
						Acolor += GET_ALPHA(point);
						Rcolor += GET_RED(point);
						Bcolor += GET_BLUE(point);
						Gcolor += GET_GREEN(point);
					}
				}

				contor = (xUp + 1 - xDown) * (yUp + 1 - yDown);
				Acolor /= contor;
				Rcolor /= contor;
				Bcolor /= contor;
				Gcolor /= contor;

				dest->SetPixel(x+xPoz, y+yPoz, COLOR_ARGB(Acolor, Rcolor, Gcolor, Bcolor), p);
			}
		}
	}

	if (xRap < 1 && yRap < 1)
	{
		xRap = ((float)w / (float)(source->GetWidth()));
		yRap = ((float)h / (float)(source->GetHeight()));

		unsigned int color;

		for (x = 0; x < source->GetWidth(); x++)
		{
			for (y = 0; y < source->GetHeight(); y++)
			{
				point = source->GetPixel(x, y);
				Acolor = GET_ALPHA(point);
				Rcolor = GET_RED(point);
				Gcolor = GET_GREEN(point);
				Bcolor = GET_BLUE(point);

				color = COLOR_ARGB(Acolor, Rcolor, Gcolor, Bcolor);

				x1 = x * xRap; x2 = (x + 1) * xRap;
				y1 = y * yRap; y2 = (y + 1) * yRap;

				if ((floor(x1 + 1) - x1) >(x1 - floor(x1))) xDown = floor(x1);
				else xDown = floor(x1 + 1);
				if ((floor(x2 + 1) - x2) > (x2 - floor(x2))) xUp = floor(x2);
				else xUp = floor(x2 + 1);
				if ((floor(y1 + 1) - y1) > (y1 - floor(y1))) yDown = floor(y1);
				else yDown = floor(y1 + 1);
				if ((floor(y2 + 1) - y2) > (y2 - floor(y2))) yUp = floor(y2);
				else yUp = floor(y2 + 1);

				for (i = xDown; i <= xUp; i++)
				{
					for (j = yDown; j <= yUp; j++)
					{
						dest->SetPixel(i+xPoz, j+yPoz, color, p);
					}
				}
			}
		}
	}
}
void Resample_CustomSlow(GApp::Graphics::Renderer *dest, GApp::Graphics::Renderer *source, int xPoz, int yPoz, int w, int h, GApp::Graphics::PaintMode *p)
{
	float xRap = ((float)source->GetWidth() / (float)w);
	float yRap = ((float)source->GetHeight() / (float)h);

	int x, y;
	float x1, x2, y1, y2;
	int xDown, xUp, yDown, yUp;
	int i, j;
	unsigned int Acolor, Rcolor, Bcolor, Gcolor;
	unsigned int point;

	if (xRap > 1 && yRap > 1)
	{
		float contor;

		for (x = 0; x < w; x++)
		{
			for (y = 0; y < h; y++)
			{
				Acolor = 0; Rcolor = 0;
				Bcolor = 0; Gcolor = 0;

				x1 = x * xRap; x2 = (x + 1) * xRap;
				y1 = y * yRap; y2 = (y + 1) * yRap;

				if (floor(x1) != x1) xDown = floor(x1) + 1;
				else xDown = x1;
				if (floor(x2) != x2) xUp = floor(x2);
				else xUp = x2;
				if (floor(y1) != y1) yDown = floor(y1) + 1;
				else yDown = y1;
				if (floor(y2) != y2) yUp = floor(y2);
				else yUp = y2;

				for (i = xDown; i <= xUp; i++)
				{
					for (j = yDown; j <= yUp; j++)
					{
						point = source->GetPixel(i, j);
						Acolor += GET_ALPHA(point);
						Rcolor += GET_RED(point);
						Bcolor += GET_BLUE(point);
						Gcolor += GET_GREEN(point);
					}
				}
				contor = (xUp + 1 - xDown) * (yUp + 1 - yDown);
				float aux;

				if (floor(x1) != x1)
				{
					for (i = yDown; i <= yUp; i++)
					{
						aux = 1 - (x1 - floor(x1));
						point = source->GetPixel(floor(x1), i);
						Acolor += GET_ALPHA(point) * aux;
						Rcolor += GET_RED(point) * aux;
						Bcolor += GET_BLUE(point) * aux;
						Gcolor += GET_GREEN(point) * aux;
						contor += aux;
					}
				}
				if (floor(x2) != x2)
				{
					for (i = yDown; i <= yUp; i++)
					{
						aux = x1 - floor(x1);
						point = source->GetPixel(floor(x1) + 1, i);
						Acolor += GET_ALPHA(point) * aux;
						Rcolor += GET_RED(point) * aux;
						Bcolor += GET_BLUE(point) * aux;
						Gcolor += GET_GREEN(point) * aux;
						contor += aux;
					}
				}
				if (floor(y1) != y1)
				{
					for (i = xDown; i <= xUp; i++)
					{
						aux = 1 - (y1 - floor(y1));
						point = source->GetPixel(i, floor(y1));
						Acolor += GET_ALPHA(point) * aux;
						Rcolor += GET_RED(point) * aux;
						Bcolor += GET_BLUE(point) * aux;
						Gcolor += GET_GREEN(point) * aux;
						contor += aux;
					}
				}
				if (floor(y2) != y2)
				{
					for (i = xDown; i <= xUp; i++)
					{
						aux = y1 - floor(y1);
						point = source->GetPixel(i, floor(y1) + 1);
						Acolor += GET_ALPHA(point) * aux;
						Rcolor += GET_RED(point) * aux;
						Bcolor += GET_BLUE(point) * aux;
						Gcolor += GET_GREEN(point) * aux;
						contor += aux;
					}
				}

				Acolor /= contor;
				Rcolor /= contor;
				Bcolor /= contor;
				Gcolor /= contor;

				dest->SetPixel(x+xPoz, y+yPoz, COLOR_ARGB(Acolor, Rcolor, Gcolor, Bcolor), p);
			}
		}
	}

	if (xRap < 1 && yRap < 1)
	{
		xRap = ((float)w / (float)(source->GetWidth()));
		yRap = ((float)h / (float)(source->GetHeight()));

		unsigned int color;

		for (y = 0; y < source->GetHeight(); y++)
		{
			for (x = 0; x < source->GetWidth(); x++)
			{
				point = source->GetPixel(x, y);
				Acolor = GET_ALPHA(point);
				Rcolor = GET_RED(point);
				Gcolor = GET_GREEN(point);
				Bcolor = GET_BLUE(point);

				color = COLOR_ARGB(Acolor, Rcolor, Gcolor, Bcolor);

				x1 = x * xRap; x2 = (x + 1) * xRap;
				y1 = y * yRap; y2 = (y + 1) * yRap;

				if (floor(x1) != x1) xDown = floor(x1) + 2;
				else xDown = x1;
				if (floor(x2) != x2) xUp = floor(x2);
				else xUp = x2;
				if (floor(y1) != y1) yDown = floor(y1) + 2;
				else yDown = y1;
				if (floor(y2) != y2) yUp = floor(y2);
				else yUp = y2;

				for (i = xDown; i <= xUp; i++)
				{
					for (j = yDown; j <= yUp; j++)
					{
						dest->SetPixel(i, j, color, p);
					}
				}

				unsigned int pointAux;
				unsigned int Aaux, Raux, Gaux, Baux;

				if (floor(x1) != x1)
				{
					for (i = yDown; i <= yUp; i++)
					{
						pointAux = dest->GetPixel(floor(x1) + 1, i);
						Aaux = (GET_ALPHA(pointAux)) * (x1 - floor(x1)) + Acolor * (1 - (x1 - floor(x1)));
						Raux = (GET_RED(pointAux)) * (x1 - floor(x1)) + Rcolor * (1 - (x1 - floor(x1)));
						Gaux = (GET_GREEN(pointAux)) * (x1 - floor(x1)) + Gcolor * (1 - (x1 - floor(x1)));
						Baux = (GET_BLUE(pointAux)) * (x1 - floor(x1)) + Bcolor * (1 - (x1 - floor(x1)));

						dest->SetPixel(floor(x1) + 1, i, COLOR_ARGB(Aaux, Raux, Gaux, Baux), p);
					}
				}
				if (floor(y1) != y1)
				{
					for (i = xDown; i <= xUp; i++)
					{
						pointAux = dest->GetPixel(i, floor(y1) + 1);
						Aaux = (GET_ALPHA(pointAux)) * (y1 - floor(y1)) + Acolor * (1 - (y1 - floor(y1)));
						Raux = (GET_RED(pointAux)) * (y1 - floor(y1)) + Rcolor * (1 - (y1 - floor(y1)));
						Gaux = (GET_GREEN(pointAux)) * (y1 - floor(y1)) + Gcolor * (1 - (y1 - floor(y1)));
						Baux = (GET_BLUE(pointAux)) * (y1 - floor(y1)) + Bcolor * (1 - (y1 - floor(y1)));

						dest->SetPixel(i, floor(y1) + 1, COLOR_ARGB(Aaux, Raux, Gaux, Baux), p);
					}
				}
				if (floor(x1) != x1 && floor(y1) != y1)
				{
					pointAux = dest->GetPixel(floor(x1) + 1, floor(y1) + 1);

					float raport;
					raport = (1 - (x1 - floor(x1))) * (1 - (y1 - floor(y1)));

					Aaux = (GET_ALPHA(pointAux)) + Acolor * raport;
					Raux = (GET_RED(pointAux)) + Rcolor * raport;
					Gaux = (GET_GREEN(pointAux)) + Gcolor * raport;
					Baux = (GET_BLUE(pointAux)) + Bcolor * raport;

					dest->SetPixel(floor(x1) + 1, floor(y1) + 1, COLOR_ARGB(Aaux, Raux, Gaux, Baux), p);
				}
				if (floor(x2) != x2 && floor(y1) != y1)
				{
					pointAux = dest->GetPixel(floor(x2) + 1, floor(y1) + 1);

					float raport;
					raport = (x2 - floor(x2)) * (1 - (y1 - floor(y1)));


					Aaux = (GET_ALPHA(pointAux)) + Acolor * raport;
					Raux = (GET_RED(pointAux)) + Rcolor * raport;
					Gaux = (GET_GREEN(pointAux)) + Gcolor * raport;
					Baux = (GET_BLUE(pointAux)) + Bcolor * raport;

					dest->SetPixel(floor(x2) + 1, floor(y1) + 1, COLOR_ARGB(Aaux, Raux, Gaux, Baux), p);
				}

				if (floor(x2) != x2)
				{
					for (i = yDown; i <= yUp; i++)
					{
						dest->SetPixel(floor(x2) + 1, i, color, p);
					}
				}
				if (floor(y2) != y2)
				{
					for (i = xDown; i <= xUp; i++)
					{
						dest->SetPixel(i, floor(y2) + 1, color, p);
					}
				}
				if (floor(x2) != x2 && floor(y2) != y2)
				{
					float raport;
					raport = (x2 - floor(x2))*(y2 - floor(y2));
					Aaux = (GET_ALPHA(point)) * raport;
					Raux = (GET_RED(point)) * raport;
					Gaux = (GET_GREEN(point)) * raport;
					Baux = (GET_BLUE(point)) * raport;

					dest->SetPixel(floor(x2) + 1, floor(y2) + 1, COLOR_ARGB(Aaux, Raux, Gaux, Baux), p);
				}
				if (floor(x1) != x1 && floor(y2) != y2)
				{
					pointAux = dest->GetPixel(floor(x1) + 1, floor(y2) + 1);

					float raport;
					raport = (1 - (x1 - floor(x1))) * (y2 - floor(y2));

					Aaux = (GET_ALPHA(pointAux)) + Acolor * raport;
					Raux = (GET_RED(pointAux)) + Rcolor * raport;
					Gaux = (GET_GREEN(pointAux)) + Gcolor * raport;
					Baux = (GET_BLUE(pointAux)) + Bcolor * raport;

					dest->SetPixel(floor(x1) + 1+xPoz, floor(y2) + 1+yPoz, COLOR_ARGB(Aaux, Raux, Gaux, Baux), p);
				}
			}
		}
	}
}
// --------------------------------------------------------------------------------------------------------------------------
GApp::Graphics::Renderer::Renderer()
{
	Data = NULL;
	Width = Height = LineStrip = 0;
}
bool GApp::Graphics::Renderer::Create(GApp::Utils::Buffer *b,unsigned int w, unsigned int h)
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
void GApp::Graphics::Renderer::Dispose()
{
	Data = NULL;
	Width = Height = LineStrip = 0;
}
bool GApp::Graphics::Renderer::Resize(unsigned int w,unsigned int h)
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
bool GApp::Graphics::Renderer::Clear(unsigned int color)
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
unsigned int GApp::Graphics::Renderer::GetWidth()
{
	return Width;
}
unsigned int GApp::Graphics::Renderer::GetHeight()
{
	return Height;
}
unsigned char* GApp::Graphics::Renderer::GetPixelBuffer()
{
	if (Data==NULL)
		return NULL;
	return Data->GetBuffer();
}
unsigned char* GApp::Graphics::Renderer::GetSafePixelPointer(int x, int y, unsigned char *ptrIfInvalidLocation)
{
	if ((x >= (int)Width) || (y >= (int)Height) || (x<0) || (y<0))
		return ptrIfInvalidLocation;
	return PIXEL_POS(x, y);
}
unsigned char* GApp::Graphics::Renderer::GetUnSafePixelPointer(int x, int y)
{
	return PIXEL_POS(x, y);
}
unsigned int GApp::Graphics::Renderer::GetPixel(int x,int y,unsigned int colorIfInvalidPosition)
{
	if ((x>=(int)Width) || (y>=(int)Height) || (x<0) || (y<0))
		return colorIfInvalidPosition;
	return *((unsigned int *)(PIXEL_POS(x,y)));
}

bool GApp::Graphics::Renderer::SetPixel(int x,int y,unsigned int color,PaintMode *p)
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


bool GApp::Graphics::Renderer::LoadImage(GApp::Utils::Buffer *img)
{	
	CHECK(img!=NULL,false,"");
	CHECK(img->GetSize()>9,false,"");
	CHECK(img->GetUInt32(0)==RASTER_IMAGE_MAGIC,false,"Invalid magic for image (found: 0x%08X - expecting: 0x%08X)",img->GetUInt32(0),RASTER_IMAGE_MAGIC);
	
	unsigned int w,h;
	unsigned char type;
	w = img->GetUInt16(4);
	h = img->GetUInt16(6);
	type = img->GetUInt8(8);

	CHECK((w>0) && (h>0),false,"");
	CHECK(Resize(w,h),false,"");

	// cel mai simplu model - copii toti bitii	
	unsigned int *sursa = (unsigned int *)(img->GetBuffer()+9);
	unsigned int *end = sursa+(w*h);
	unsigned int y,x;
	y = 0;
	x = 0;
	while (sursa<end)
	{
		* (unsigned int *)(PIXEL_POS(x,y)) = (*sursa);
		sursa++;
		x++;
		if (x==Width)
		{
			x = 0;
			y++;
		}
		
	}

	return true;
}
bool GApp::Graphics::Renderer::DrawImage(Renderer *r,int x,int y,PaintMode *p)
{
	CHECK(Data!=NULL,false,"");
	CHECK(r!=NULL,false,"");
	CHECK(r->Data!=NULL,false,"");

	if (p==NULL)
	{
		// nu am nici o operatie pe pixel de facut

		// optimizari pentru buffer in interior
		if ((x >= 0) && (y >= 0) && ((x + r->Width) <= this->Width) && ((y + r->Height) <= this->Height))
		{
			//LOG_INFO("Fast BLT (%d x %d)", r->Width, r->Height);
			unsigned char *p_source = (unsigned char *)(MATRIX_POS(r->Data->Data, r->LineStrip, 0, 0, r->Height));
			unsigned char *p_dest = (unsigned char *)(PIXEL_POS(x, y));
			for (unsigned int tr = 0; tr < r->Height; tr++, p_source -= r->LineStrip, p_dest -= this->LineStrip)
			{
				memcpy(p_dest, p_source, r->LineStrip);
			}
		}
		else {
			unsigned int *sursa;
			unsigned int *dest;
			unsigned int py, px;
			int yy, xx;
			for (py = 0, yy = y; (py < r->Height) && (yy < (int)Height); py++, yy++)
			{
				if (yy < 0)
					continue;
				sursa = (unsigned int *)MATRIX_POS(r->Data->Data, r->LineStrip, 0, py, r->Height);
				for (px = 0, xx = x; (px < r->Width) && (xx < (int)Width); px++, xx++, sursa++)
				{
					if (xx < 0)
						continue;
					dest = (unsigned int *)PIXEL_POS(xx, yy);
					*dest = (*sursa);
				}
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
bool GApp::Graphics::Renderer::DrawImage(Renderer *r,int x,int y,int newWidth,int newHeight,PaintMode *p)
{
	CHECK(Data!=NULL,false,"");
	CHECK(r!=NULL,false,"");

	if ((newWidth == r->Width) && (newHeight == r->Height))
		return DrawImage(r,x,y,p);
	if (p==NULL)
	{
		//Resample_NearestNeighbor(this,r,x,y,newWidth,newHeight,p);
		//Resample_Bicubic(this, r, x, y, newWidth, newHeight, p);
		//Resample_CustomFast(this, r, x, y, newWidth, newHeight, p);
		//Resample_CustomSlow(this, r, x, y, newWidth, newHeight, p);
		Resample_Bilinear_Fast(this, r, x, y, newWidth, newHeight);
		//Resample_Bilinear(this, r, x, y, newWidth, newHeight, p);
		//Resample_Lanczos(this, r, x, y, newWidth, newHeight, p);
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
		case GAC_RESAMPLEMETHOD_AVERAGE_FAST:
			Resample_CustomFast(this, r, x, y, newWidth, newHeight, p);
			return true;
		case GAC_RESAMPLEMETHOD_AVERAGE_SLOW:
			Resample_CustomSlow(this, r, x, y, newWidth, newHeight, p);
			return true;
	};
	RETURNERROR(false,"Unknwon resample algorithm : %d in GApp::Graphics::Renderer::DrawImage ",ResampleMode);	
}
bool GApp::Graphics::Renderer::DrawLine(int x1,int y1,int x2,int y2,PaintMode *p)
{
	RETURNERROR(false,"Unimplemented method");
}
bool GApp::Graphics::Renderer::DrawRect(int x,int y,int rectWidth,int rectHeight,PaintMode *p)
{
	return DrawRect(x,y,rectWidth,rectHeight,0,0,p);
}
bool GApp::Graphics::Renderer::DrawRect(int x,int y,int rectWidth,int rectHeight,float rx,float ry,PaintMode *p)
{
	CHECK(p!=NULL,false,"");

	if (p->HasFillColor())
	{
		for (int xx=0;xx<rectWidth;xx++)
			for (int yy=0;yy<rectHeight;yy++)
				SetPixel(x+xx,y+yy,p->GetFillPixelColor(x+xx,y+yy),p);
	}
	if (p->HasStrokeColor())
	{
		for (int xx=0;xx<rectWidth;xx++)
		{
			SetPixel(x+xx,y,p->GetStrokePixelColor(x+xx,y),p);
			SetPixel(x+xx,y+rectHeight,p->GetStrokePixelColor(x+xx,y+rectHeight),p);
		}
		for (int yy=0;yy<rectHeight;yy++)
		{
			SetPixel(x,y+yy,p->GetStrokePixelColor(x,y+yy),p);
			SetPixel(x+rectWidth,y+yy,p->GetStrokePixelColor(x+rectWidth,y+yy),p);
		}
	}

	return true;
}
bool GApp::Graphics::Renderer::DrawEllipse(int cx,int cy,int rx,int ry,PaintMode *p)
{
	RETURNERROR(false,"Unimplemented method");
}
bool GApp::Graphics::Renderer::DrawQuadraticBezierCurve(int x1,int y1,int x2,int y2,int x3,int y3,PaintMode *p)
{
	RETURNERROR(false,"Unimplemented method");
}
bool GApp::Graphics::Renderer::DrawCubicBezierCurve(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4,PaintMode *p)
{
	RETURNERROR(false,"Unimplemented method");
}