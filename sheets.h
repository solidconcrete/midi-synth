#include <iostream>
#include <Windows.h>
#include <cmath>
using namespace std;
HWND myconsole = GetConsoleWindow();
	HDC mydc = GetDC(myconsole);
void drawsheetnote (int x0, int y0, int radius);
void deletesheetnote (int x0, int y0, int radius);
class sheetnotes{
	private:
	int x0, y0, h, w, radius, keycoord[13], forecol[3], backcol[3];
	string clef;
	public:
	void setcoord (int, int);
	void setsize (int, int);
	void setcolors(int, int, int, int, int, int);
	void setkeycoord();
	void drawsheet();
	void drawkey(int, bool);
	void deletekey(int, int);
	void drawsharp(int, bool);
	void drawflat(int, bool);
	
};
void sheetnotes::setcoord(int x0, int y0)
{
	this->x0=x0;
	this->y0=y0;
}
void sheetnotes::setsize(int w, int h)
{
	this->h=h;
	radius=h/5;
	this->w=w;
}
void sheetnotes::setcolors(int rf, int gf, int bf, int rb, int gb, int bb)
{
	forecol[0]=rf;
	forecol[1]=gf;
	forecol[2]=bf;
	backcol[0]=rb;
	backcol[1]=gb;
	backcol[2]=bb;
}
void sheetnotes::drawsheet()
{
	int x, y;
	for(y=y0; y<y0+h; y+=radius)
	for(x=x0; x<x0+w; x++)
	SetPixel(mydc, x, y, RGB(forecol[0], forecol[1], forecol[2]));
}
void sheetnotes::setkeycoord()
{
	int twelve=10;
	for(int i=0; i<=12; i++)
	{
		keycoord[i]=y0+(twelve*radius/2);
		twelve--;
	}
}
void sheetnotes::drawkey(int n, bool action)
{
	int r, g, b;
	if(action == 1)
	{
		r = forecol[0];
		g = forecol[1];
		b = forecol[2];
	}
	else if(action == 0)
	{
		r = backcol[0];
		g = backcol[1];
		b = backcol[2];
	}
    int x = radius/2-radius/4;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius/3 << 1);

    while (x >= y)
    {
        SetPixel(mydc, x0+(n*radius/2) + x, keycoord[n] + y, RGB(r, g, b));
        SetPixel(mydc, x0+(n*radius/2) + y, keycoord[n] + x, RGB(r, g, b));
        SetPixel(mydc, x0+(n*radius/2) - y, keycoord[n] + x, RGB(r, g, b));
        SetPixel(mydc, x0+(n*radius/2) - x, keycoord[n] + y, RGB(r, g, b));
        SetPixel(mydc, x0+(n*radius/2) - x, keycoord[n] - y, RGB(r, g, b));
        SetPixel(mydc, x0+(n*radius/2) - y, keycoord[n] - x, RGB(r, g, b));
        SetPixel(mydc, x0+(n*radius/2) + y, keycoord[n] - x, RGB(r, g, b));
        SetPixel(mydc, x0+(n*radius/2) + x, keycoord[n] - y, RGB(r, g, b));

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius/3 << 1);
        }
    }
	if((n==0)||(n==12))
    for(x=radius/2; x>=0; x--)
    {
	    SetPixel(mydc, x0-x+(n*radius/2), keycoord[n], RGB(r, g, b));
	    SetPixel(mydc, x0+x+(n*radius/2), keycoord[n], RGB(r, g, b));
	}
	if(action == 0)
	drawsheet();
}

void sheetnotes::drawsharp(int n, bool action)
{
	
	int r, g, b;
	if(action == 1)
	{
		r = forecol[0];
		g = forecol[1];
		b = forecol[2];
	}
	else if(action == 0)
	{
		r = backcol[0];
		g = backcol[1];
		b = backcol[2];
	}
	
	int x, y=keycoord[n], basex=x0+((n-2)*radius/2), basey=keycoord[n];
	for(x = basex; x<basex+radius/2; x++)
	{
		SetPixel(mydc, x, basey-radius/8, RGB(r, g, b));
		SetPixel(mydc, x, basey+radius/8, RGB(r, g, b));
	}

	for(y=basey-radius/4; y<basey+radius/4; y++)
	{
		SetPixel(mydc, basex+radius/8, y, RGB(r, g, b));
		SetPixel(mydc, basex+radius/2-radius/8, y, RGB(r, g, b));
		
	}


}
void sheetnotes::drawflat(int n, bool action)
{
	
	int r, g, b;
	if(action == 1)
	{
		r = forecol[0];
		g = forecol[1];
		b = forecol[2];
	}
	else if(action == 0)
	{
		r = backcol[0];
		g = backcol[1];
		b = backcol[2];
	}
    int x = radius/7;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius/7 << 1);

    while (x >= y)
    {
        SetPixel(mydc, x0+((n-1)*radius/2) + x, keycoord[n] + y, RGB(r, g, b));
        SetPixel(mydc, x0+((n-1)*radius/2) + y, keycoord[n] + x, RGB(r, g, b));
        SetPixel(mydc, x0+((n-1)*radius/2) - y, keycoord[n] + x, RGB(r, g, b));
        SetPixel(mydc, x0+((n-1)*radius/2) - x, keycoord[n] + y, RGB(r, g, b));
        SetPixel(mydc, x0+((n-1)*radius/2) - x, keycoord[n] - y, RGB(r, g, b));
        SetPixel(mydc, x0+((n-1)*radius/2) - y, keycoord[n] - x, RGB(r, g, b));
        SetPixel(mydc, x0+((n-1)*radius/2) + y, keycoord[n] - x, RGB(r, g, b));
        SetPixel(mydc, x0+((n-1)*radius/2) + x, keycoord[n] - y, RGB(r, g, b));

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius/7 << 1);
        }
    }
    for(int y=keycoord[n]; y>keycoord[n]-radius/2; y--)
	{
	SetPixel(mydc, x0+((n-1)*radius/2)-radius/8, y, RGB(r, g, b));
	}
}
	

