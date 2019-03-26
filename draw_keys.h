#include <iostream>
#include <windows.h>
using namespace std;
HANDLE console=GetStdHandle(STD_OUTPUT_HANDLE);
class console_setattributes{
	private:
	int x, y;
	COORD coord;
	WORD color;
	public:
	void setcoord(short, short);
	void setcolors(short, short);
	friend class drawkeys;
	friend class textbox;
	friend class highlight;
	friend class rect;
};
void console_setattributes::setcoord(short x, short y)
{
	this->x=x;
	this->y=y;
	coord.X=x;
	coord.Y=y;
	SetConsoleCursorPosition(console, coord);
}
void console_setattributes::setcolors(short fore, short back)
{
	color=16*fore+back;
	SetConsoleTextAttribute(console, color);
}
class drawkeys : public console_setattributes{
	private:
		short w, h, type;
	public:
		void setsize(short, short);
		void settype(short);
		void draw(string);
		friend class rect;
		
};
void drawkeys::setsize(short w, short h)
{
	this->w=w;
	this->h=h;
}
void drawkeys::settype (short type)
{
	this->type=type;
}
void drawkeys::draw(string status)
{
	
	if (status == "OFF")
	{
		if (type == 0)
			setcolors(15, 2);
		else 
			setcolors(0, 2);
	}
	else
	if (status == "ON")
		setcolors (14, 2);
	char a=179;
	COORD tempcoord=coord;
	for(int i=0; i<h; i++)
	{
		for(int j=0; j<w; j++)
		{
			tempcoord.X=coord.X+j;
			tempcoord.Y=coord.Y+i;
			SetConsoleCursorPosition(console, tempcoord);
			cout<<" ";
		}
		if (type == 0)
		cout<<a;
	}
}
class textbox : public console_setattributes{
	private:
		int length;
	public:
		void setlength (int);
		void write (string);
};
void textbox::setlength(int length)
{
	this->length=length;
}
void textbox::write(string text)
{
	SetConsoleTextAttribute(console, color);
	SetConsoleCursorPosition(console, coord);
	for(int i=0; i<length; i++)
	cout<<" ";
	SetConsoleCursorPosition(console, coord);
	cout<<text;
	
}
class rect : public drawkeys{
	public:
	void drawrect();
};
void rect::drawrect()
{
	
	for(int i=0; i<h; i++)
	{
		for(int j=0; j<w; j++)
		cout<<" ";
		setcoord(x, y+1);
	}
}
