#include "draw_keys.h"
#include "sheets.h"
#include <iostream>
#include <Windows.h>
#include <mmsystem.h>
#include <iomanip>
UINT midi_out_info();
UINT midi_in_info();
void midi_in_test();
void midi_out_test(int status, int k, int vel);
void playwithkeyboard();
void choose(int i);
void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}
using namespace std;
HMIDIOUT hmidiout;
string keystatus[96]="OFF";
drawkeys key[96];
sheetnotes notescreen;
console_setattributes text, box;
textbox volume[2], program[2], keyname[12], menu[6], txt;
rect menuback, casing, screen;
string keynames[12]={"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"},
menuoptions[5]={"Free Play", "Guess the pitch", "Sheet reading", "Key names", "Exit"};
	sheetnotes task, exc;
struct midi_msg{
	long long tick;
	int status;
	int data[2];
};
#define MIDI_NOTE_OFF				0x80
#define MIDI_NOTE_ON				0x90
#define MIDI_POLY_KEY_PRESSURE		0xa0
#define MIDI_CC						0xb0
#define MIDI_PROGRAM_CHANGE			0xc0
#define MIDI_CHANNEL_PRESSURE		0xd0
#define MIDI_PITCH_BEND				0xe0

inline void check_return_code(MMRESULT rc, char *msg)
{
	if (rc != MMSYSERR_NOERROR)
	{
		cout<<"Error: "<<msg<<endl;
		return;
	}
}
void CALLBACK MidiInProc (HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance,
							DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	text.setcolors(0, 2);
	text.setcoord(0, 0);
	HMIDIOUT hmidiout;
	(void) (hMidiIn);
    (void) (dwInstance);
    midi_msg msg;
    if( wMsg == MIM_OPEN)
    	cout<<"MidiInProc: MIM_OPEN"<<endl;
    else if (wMsg == MIM_CLOSE)
    	cout<<"MidiInProc: MIM_CLOSE"<<endl;
    else if (wMsg == MIM_DATA)
    {
    	msg.tick=dwParam2;
    	msg.status=dwParam1 & 0xff;
    	msg.data[0]=(dwParam1 >> 8) & 0xff;
    	msg.data[1]=(dwParam1 >> 16) & 0xff;
    	midi_out_test(msg.status, msg.data[0], msg.data[1]);
    	cout<<"tick: "<<msg.tick;
    	if(msg.status  == MIDI_NOTE_OFF)
    	{
 
    		cout<<"status: "<<msg.status<<" Note off ch: "<<msg.status<<
    		"  Key: "<<msg.data[0]<<" Vel:"<<msg.data[1];
    	}
    		
    	else if(msg.status == MIDI_NOTE_ON)
    	{
    		cout<<"status: "<<msg.status<<" Note on ch: "<<msg.status<<
    		"  Key: "<<msg.data[0]<<" Vel:"<<msg.data[1];

    	}
    			
    	else if((msg.status & MIDI_CC) == MIDI_CC)
    	{
    		cout<<"Control change: "<<msg.status<<
    		"  Controller: "<<msg.data[0]<<" Value:"<<msg.data[1];
    	}
		else if(msg.status == MIDI_PITCH_BEND)
    	{
    		cout<<"MIDI_PITCH_BEND: "<<msg.status<<
    		"  Controller: "<<msg.data[0]<<" Value:"<<msg.data[1];
    	}   
		else if(msg.status == MIDI_PROGRAM_CHANGE)
    	{
    		cout<<"MIDI_PROGRAM_CHANGE: "<<msg.status
			<<"  Program nr:"<<msg.data[0]<<" "<<msg.data[1];
    	}   			
    		
	}
    else 
		cout<<endl<<wMsg<<endl;
}
void midi_in_test ()
{
	int i=0;
	MMRESULT ok;
	HMIDIIN hmidiin;
	ok=midiInOpen(&hmidiin, 0, (DWORD_PTR) MidiInProc, 0, CALLBACK_FUNCTION | MIDI_IO_STATUS);
	if (ok != MMSYSERR_NOERROR )
	{
		cout<<"midiInOpen failure"<<endl;
		return ;
	}
	
	ok = midiInStart (hmidiin);
		if (ok != MMSYSERR_NOERROR )
	{
		cout<<"midiInStart failure"<<endl;
		return ;
	}
	while(1)
	{

		if (GetAsyncKeyState(VK_ESCAPE))
		break;
			if (GetAsyncKeyState(VK_DOWN))
		{
			menu[i].setcolors(2, 7);
			menu[i].write(menuoptions[i]);
			if (i<4)	
				i++;
			else i=0;	
			menu[i].setcolors(3, 7);
			menu[i].write(menuoptions[i]);
			Sleep(100);
		}
			if (GetAsyncKeyState(VK_UP))
		{
			menu[i].setcolors(2, 7);
			menu[i].write(menuoptions[i]);
			if (i>0)	
				i--;
			else i=4;	
			menu[i].setcolors(3, 7);
			menu[i].write(menuoptions[i]);
			Sleep(300);
		}
		if (GetAsyncKeyState(VK_RETURN))
		{
			if(i != 4)
			choose(i);
			else return;
		}
	}
	ok = midiInStop (hmidiin);
	if (ok != MMSYSERR_NOERROR )
	{
		cout<<"midiInStop failure"<<endl;
		return ;
	}
}
void CALLBACK MidiOutProc(HMIDIOUT hmo, UINT wMsg, DWORD_PTR dwInstance,
						 DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
   (void) (dwParam1);
   (void) (dwParam2);
   if (wMsg == MOM_OPEN)
		cout<<"MidiOutProc() MOM_OPEN"<<endl;
   else if (wMsg == MOM_CLOSE)
   		cout<<"MidiOutProc() MOM_CLOSE"<<endl;
   	else
   		cout<<"MidiOutProc() Msg:"<<wMsg<<endl;

}
int main(int argc, char** argv)
{
	system ("cls");
	
	ShowConsoleCursor(0);
	UINT in_num, out_num;
	DWORD msg; 
	midiOutOpen(&hmidiout, 0, (DWORD_PTR)MidiOutProc, 0, CALLBACK_FUNCTION);   //open up the port for outputting messages
	
	int anymidi=midi_in_info();
	midi_out_info();
	
	
	casing.setcolors(8, 0);  //coloring
	casing.setcoord(4,1);
	casing.setsize(257, 50);
	casing.drawrect();
	
	
	casing.setcolors(0, 0); //coloring
	casing.setcoord(5,2);
	casing.setsize(255, 8);
	casing.drawrect();
	
	casing.setcolors(0, 0); //coloring
	casing.setcoord(99,9);
	casing.setsize(52, 11);
	casing.drawrect();
	
	screen.setcolors(2, 7);	  //draw info screen (below the keyboard)
	screen.setcoord(100, 10);
	screen.setsize(50, 9);
	screen.drawrect();
	
	short base_x=10, base_y=3, shift=0, i;
	for (i = 24; i < 108; i++) 			//draw all the white keys
	{
		if( (i % 12 == 0) || (i % 12 == 2) || (i % 12 == 4) || (i % 12 == 5) || (i % 12 == 7) || (i % 12 == 9) || (i % 12 == 11))
		{
			key[i-24].settype(0);
			key[i-24].setcoord(base_x+shift, base_y);
			key[i-24].setsize(4, 6);
			key[i-24].draw("OFF");
			shift+=5;
		}
	}
	
	shift=3;						//in some places draw black keys over them
	for (i = 25; i < 108; i++)
	{
		if( (i % 12 == 1) || (i % 12 == 3) || (i % 12 == 6) || (i % 12 == 8) || (i % 12 == 10) )
		{
			key[i-24].settype(1);
			key[i-24].setcoord(base_x+shift, base_y);
			key[i-24].setsize(3, 4);
			key[i-24].draw("OFF");
			shift-=5;
		}
		shift+=5;
	}

	volume[0].setcoord(101, 14);		//put volume indicator on info screen
	volume[0].setcolors(2, 7);
	volume[0].setlength(4);
	volume[0].write("VOL:");
	volume[1].setcoord(106, 14);
	volume[1].setcolors(2, 7);
	volume[1].setlength(9);
	
	program[0].setcoord(101, 16);		//put program indicator on info screen
	program[0].setcolors(2, 7);
	program[0].setlength(8);
	program[0].write("PROGRAM:");
	program[1].setcoord(109, 16);
	program[1].setcolors(2, 7);
	program[1].setlength(3);
	
	shift=0;
	for(i=0; i<12; i++)
	{
		keyname[i].setcoord(108+shift, 10);		//put key indicator on info screen
		keyname[i].setcolors(2, 7);
		keyname[i].setlength(2);
		keyname[i].write(keynames[i]);
		shift+=3;
	}
	menuback.setcolors(5, 7);			//create menu window
	menuback.setcoord(8, 20);
	menuback.setsize(54, 31);
	menuback.drawrect();
	
	menuback.setcolors(1, 7);			//create menu window
	menuback.setcoord(10, 21);
	menuback.setsize(50, 29);
	menuback.drawrect();
	
	menuback.setcolors(5, 7);			//create menu window
	menuback.setcoord(60, 20);
	menuback.setsize(55, 31);
	menuback.drawrect();
	
	menuback.setcolors(1, 7);			//create menu window
	menuback.setcoord(63, 21);
	menuback.setsize(50, 29);
	menuback.drawrect();
	
	text.setcolors(1, 7);
	text.setcoord(64, 21);
	cout<<"1-8: Piano";
	
	text.setcoord(64, 23);
	cout<<"9-16: Chromatic Percussion";
	
	text.setcoord(64, 25);
	cout<<"17-24: Orgas";
	
	text.setcoord(64, 27);
	cout<<"25-32: Guitar";
	
	text.setcoord(64, 29);
	cout<<"33-40 Bass";
	
	text.setcoord(64, 31);
	cout<<"41-48: Strings";
	
	text.setcoord(64, 33);
	cout<<"49-56: Ensemble";
	
	text.setcoord(64, 35);
	cout<<"57-64: Brass";
	
	text.setcoord(64, 37);
	cout<<"65-72: Reed";
	
	text.setcoord(64, 39);
	cout<<"73-80: Pipe";
	
	text.setcoord(64, 41);
	cout<<"81-88 Synth Lead";
	
	text.setcoord(64, 43);
	cout<<"89-96: Synth Pad";
	
	text.setcoord(64, 45);
	cout<<"97-104: Synth Effects";
	
	text.setcoord(64, 47);
	cout<<"105-112: Ethnic";
	
	text.setcoord(64, 49);
	cout<<"113-128: Percussive / Sound Effects";
	
	txt.setcoord(33, 22);       //menu header
	txt.setcolors(4, 2);
	txt.setlength(4);
	txt.write("MENU");
	
	menu[0].setcoord(30, 25);    //menu choices
	menu[0].setcolors(3, 7);
	menu[0].setlength(8);
	menu[0].write("Free Play");
	
	menu[1].setcoord(28, 27);
	menu[1].setcolors(2, 7);
	menu[1].setlength(8);
	menu[1].write("Guess the pitch");
	
	menu[2].setcoord(29, 29);
	menu[2].setcolors(2, 7);
	menu[2].setlength(8);
	menu[2].write("Sheet reading");
	
	menu[3].setcoord(30, 31);
	menu[3].setcolors(2, 7);
	menu[3].setlength(8);
	menu[3].write("Key names");
	
	menu[4].setcoord(32, 33);
	menu[4].setcolors(2, 7);
	menu[4].setlength(5);
	menu[4].write("Exit");

	task.setcoord(830, 180);    //put sheet notation on info screen
	task.setsize(100, 80);
	task.setcolors(224, 224, 224, 0, 130, 0);
	task.setkeycoord();
	task.drawsheet();
	

	if (anymidi > 0)		//if midi input device found - use it, if not - use qwerty
	midi_in_test();
	else playwithkeyboard();
	system("cls");
}

   

void midi_out_test(int status, int k, int data)    //excecute midi message and provide gui changes
{
	DWORD msg;
	
	if((status == MIDI_NOTE_ON) && (data<50))    
		data=50;
		
	msg = (status) | (0 + k << 8) | (data << 16);     
	midiOutShortMsg(hmidiout, msg);
	
	
	if ((status == MIDI_NOTE_ON) || (status == MIDI_NOTE_OFF))
	{
		if(status == MIDI_NOTE_ON)
			keyname[k%12].setcolors(2, 0);
		else
			keyname[k%12].setcolors(2, 7);
		keyname[k%12].write(keynames[k%12]);
		if (k>=24)			
			k-=24;
		if (status == MIDI_NOTE_ON)
			keystatus[k]="ON";
		if (status == MIDI_NOTE_OFF)
			keystatus[k]="OFF";	
		switch (k%12)						//depending if the message was ON or OFF, changes the sheet notation on info screen
			{
				case 0:
					key[k].draw(keystatus[k]);
					key[k+1].draw(keystatus[k+1]);
					if(status == MIDI_NOTE_ON)
					task.drawkey(0, 1);
					else task.drawkey(0, 0);
					break;
					
				case 1:
					key[k].draw(keystatus[k]);
					if(status == MIDI_NOTE_ON)
					{
						task.drawkey(0, 1);
						task.drawsharp(0,1);
					}
					else 
					{
						task.drawkey(0, 0);
						task.drawsharp(0,0);
					}
					break;
					
				case 2:
					key[k].draw(keystatus[k]);
					key[k-1].draw(keystatus[k-1]);
					key[k+1].draw(keystatus[k+1]);
					
					if(status == MIDI_NOTE_ON)
					task.drawkey(1, 1);
					else task.drawkey(1, 0);
					break;
					
				case 3:
					key[k].draw(keystatus[k]);
					if(status == MIDI_NOTE_ON)
					{
						task.drawkey(1, 1);
						task.drawsharp(1,1);
					}
					else 
					{
						task.drawkey(1, 0);
						task.drawsharp(1,0);
					}
					break;
				case 4:
					key[k].draw(keystatus[k]);
					key[k-1].draw(keystatus[k-1]);
					if(status == MIDI_NOTE_ON)
					task.drawkey(2, 1);
					else task.drawkey(2, 0);
					break;
				case 5:
					key[k].draw(keystatus[k]);
					key[k+1].draw(keystatus[k+1]);
					if(status == MIDI_NOTE_ON)
					task.drawkey(3, 1);
					else task.drawkey(3, 0);
					break;
				case 6:
					key[k].draw(keystatus[k]);
					if(status == MIDI_NOTE_ON)
					{
						task.drawkey(3, 1);
						task.drawsharp(3,1);
					}
					else 
					{
						task.drawkey(3, 0);
						task.drawsharp(3,0);
					}
					break;
					
				case 7:
					key[k].draw(keystatus[k]);
					key[k-1].draw(keystatus[k-1]);
					key[k+1].draw(keystatus[k+1]);
					
					if(status == MIDI_NOTE_ON)
					task.drawkey(4, 1);
					else task.drawkey(4, 0);
					break;
					
				case 8:
					key[k].draw(keystatus[k]);
					key[k].draw(keystatus[k]);
					if(status == MIDI_NOTE_ON)
					{
						task.drawkey(4, 1);
						task.drawsharp(4,1);
					}
					else 
					{
						task.drawkey(4, 0);
						task.drawsharp(4,0);
					}
					break;
					
				case 9:
					key[k].draw(keystatus[k]);
					key[k-1].draw(keystatus[k-1]);
					key[k+1].draw(keystatus[k+1]);
					
					if(status == MIDI_NOTE_ON)
					task.drawkey(5, 1);
					else task.drawkey(5, 0);
					break;
					
				case 10:
					key[k].draw(keystatus[k]);
					
					if(status == MIDI_NOTE_ON)
					{
						task.drawkey(5, 1);
						task.drawsharp(5,1);
					}
					else 
					{
						task.drawkey(5, 0);
						task.drawsharp(5,0);
					}
					break;
				case 11:
					key[k].draw(keystatus[k]);
					key[k-1].draw(keystatus[k-1]);
					
					if(status == MIDI_NOTE_ON)
					task.drawkey(6, 1);
					else task.drawkey(6, 0);
					break;
			}
		}
	if((status== MIDI_CC) && (k==7))   //if volume was changed, changes it on screen
	{
		if(data == 127)
			volume[1].write("100");
		else
		if(data == 0)
			volume[1].write("0");
		else
			volume[1].write(to_string(data/1.27));
	}
	if(status == MIDI_PROGRAM_CHANGE)		//if program was changed, changes it on screen
	{
		program[1].write(to_string(k));
	}
	text.setcoord(0, 0);
	text.setcolors(0, 2);
	
}

UINT midi_out_info()
{
	UINT n_midi_out, n_midi_in ,i, out_num;
	MMRESULT check;
	MIDIOUTCAPS caps;
	n_midi_out=midiOutGetNumDevs();
	cout<<n_midi_out<<" midi out device(s) found:"<<endl;   
	
   for(int i=0; i<n_midi_out; i++)
   {
   	check=midiOutGetDevCaps((UINT_PTR)i, &caps, sizeof (caps));
   	if(check != MMSYSERR_NOERROR)
   	{
   		cout<<"ERROR: "<<check;
	   break;
	}
   
   cout<<endl<<i<<") Output Device"<<endl;
   cout<<"Name: "<<caps.szPname<<endl;
   cout<<"Technology: ";
   if(caps.wTechnology == MOD_MIDIPORT) cout<<"MIDI hardware port"<<endl;
   else if(caps.wTechnology == MOD_SYNTH) cout<<"Synthesizer"<<endl;
   else if(caps.wTechnology == MOD_SQSYNTH) cout<<"Square wave synthesizer."<<endl;
   else if(caps.wTechnology == MOD_FMSYNTH) cout<<"FM synthesizer"<<endl;
   else if(caps.wTechnology == MOD_MAPPER) cout<<"Microsoft MIDI mapper"<<endl;
   else if(caps.wTechnology == MOD_WAVETABLE) cout<<"Hardware wavetable synthesizer"<<endl;
   else if(caps.wTechnology == MOD_SWSYNTH) cout<<"Software synthesizer"<<endl;   
   cout<<"Manufacturer ID: "<<caps.wMid<<endl;
   cout<<"Product ID: "<<caps.wPid<<endl;
   cout<<"Channel mask: "<<caps.wChannelMask;
   cout<<"Device Technology : "<<caps.wTechnology<<endl;
   cout<<"n of Voices: "<<caps.wVoices<<endl;
   cout<<"n of Notes: "<<caps.wNotes<<endl;
   cout<<"optional functionality:"<<endl;
   if(caps.dwSupport & MIDICAPS_CACHE) cout<<"MIDICAPS_CACHE"<<endl;
   if(caps.dwSupport & MIDICAPS_LRVOLUME) cout<<"MIDICAPS_LRVOLUME"<<endl;
   if(caps.dwSupport & MIDICAPS_STREAM) cout<<"MIDICAPS_STREAM"<<endl;
   if(caps.dwSupport & MIDICAPS_VOLUME) cout<<"MIDICAPS_VOLUME"<<endl;
   cout<<endl;
	}
	system("Pause");
	system("cls");
//	cout<<"Enter Desired Output Device Number...";
//	cin>>out_num;
//	system ("cls");
//	return out_num;
}
UINT midi_in_info()
{
	UINT n_midi_in, i, in_num;
	MMRESULT check;
	MIDIINCAPS caps;
	n_midi_in=midiInGetNumDevs();
	if(n_midi_in == 0)
	cout<<endl<<"0 MIDI INPUTS DETECTED. SWTITCHING TO QWERTY KEYBOARD";
	
	
   for(int i=0; i<n_midi_in; i++)
   {
   	check=midiInGetDevCaps((UINT_PTR)i, &caps, sizeof (caps));
   	if(check != MMSYSERR_NOERROR)
   	{
   		cout<<"ERROR: "<<check;
	   break;
	}
   
   cout<<endl<<i<<") device ID"<<endl;
   cout<<"Name: "<<caps.szPname<<endl;
   cout<<"Manufacturer ID: "<<caps.wMid<<endl;
   cout<<"Product ID: "<<caps.wPid<<endl<<endl<<endl;
	}
	return n_midi_in;
}
void choose(int i)
{
	srand(time(NULL));
	if(i==1)
	{
		DWORD msg;
		msg = (MIDI_NOTE_ON) | (rand()%12+60 << 8) | (127 << 16);
		midiOutShortMsg(hmidiout, msg);
	}
	if (i == 2)
	{
		int key=rand()%12, symbol=rand()%3;
		exc.setcoord(500, 160);
		exc.setsize(150, 100);
		exc.setcolors(0, 0, 0, 128, 128, 128);
		exc.setkeycoord();
		exc.drawsheet();
		exc.drawkey(key, 1);
		if(symbol == 1)
		exc.drawsharp(key, 1);
		if (symbol == 2)
		exc.drawflat(key, 1);
		Sleep(1000);
		exc.drawkey(key, 0);
		if(symbol == 1)
		exc.drawsharp(key, 0);
		if (symbol == 2)
		exc.drawflat(key, 0);	
	}
	if (i == 3)
	{
		txt.setcoord(33, 12);
		txt.setcolors(4, 2);
		txt.setlength(4);
		txt.write(keynames[rand()%12]);
	}

}
void playwithkeyboard()
{
	int i=0;
	text.setcoord(14, 40);
	cout<<"keyboard controls: z s x d c v g b h n j m";
	int instrument=0;
	while (1)
	{
			while(1)
	{

		if (GetAsyncKeyState(VK_ESCAPE))
		break;
		if (GetAsyncKeyState(VK_OEM_PLUS))
		{
			if(instrument<127)
			{
				instrument++;
				midi_out_test(MIDI_PROGRAM_CHANGE, instrument, 0);
			}
			Sleep(500);
		}
		if (GetAsyncKeyState(VK_OEM_MINUS))
		{
			if(instrument>0)
			{
				instrument--;
				midi_out_test(MIDI_PROGRAM_CHANGE, instrument, 0);
			}
			Sleep(500);
		}
			if (GetAsyncKeyState(VK_DOWN))
		{
			menu[i].setcolors(2, 7);
			menu[i].write(menuoptions[i]);
			if (i<4)	
				i++;
			else i=0;	
			menu[i].setcolors(3, 7);
			menu[i].write(menuoptions[i]);
			Sleep(500);
		}
			if (GetAsyncKeyState(VK_UP))
		{
			menu[i].setcolors(2, 7);
			menu[i].write(menuoptions[i]);
			if (i>0)	
				i--;
			else i=4;	
			menu[i].setcolors(3, 7);
			menu[i].write(menuoptions[i]);
			Sleep(500);
		}
		if (GetAsyncKeyState(VK_RETURN))
		{
			if(i != 4)
			choose(i);
			else return;
		}
		if (GetAsyncKeyState(0x5A))
		{
			midi_out_test(MIDI_NOTE_ON, 60, 127);
			Sleep(500);
			midi_out_test(MIDI_NOTE_OFF, 60, 127);
		}
		if (GetAsyncKeyState(0x53))
		{
			midi_out_test(MIDI_NOTE_ON, 61, 127);
			Sleep(500);
			midi_out_test(MIDI_NOTE_OFF, 61, 127);
		}
		if (GetAsyncKeyState(0x58))
		{
			midi_out_test(MIDI_NOTE_ON, 62, 127);
			Sleep(500);
			midi_out_test(MIDI_NOTE_OFF, 62, 127);
		}
		if (GetAsyncKeyState(0x44))
		{
			midi_out_test(MIDI_NOTE_ON, 63, 127);
			Sleep(500);
			midi_out_test(MIDI_NOTE_OFF, 63, 127);
		}
		if (GetAsyncKeyState(0x43))
		{
			midi_out_test(MIDI_NOTE_ON, 64, 127);
			Sleep(500);
			midi_out_test(MIDI_NOTE_OFF, 64, 127);
		}
		if (GetAsyncKeyState(0x56))
		{
			midi_out_test(MIDI_NOTE_ON, 65, 127);
			Sleep(500);
			midi_out_test(MIDI_NOTE_OFF, 65, 127);
		}
		if (GetAsyncKeyState(0x47))
		{
			midi_out_test(MIDI_NOTE_ON, 66, 127);
			Sleep(500);
			midi_out_test(MIDI_NOTE_OFF, 66, 127);
		}
		if (GetAsyncKeyState(0x42))
		{
			midi_out_test(MIDI_NOTE_ON, 67, 127);
			Sleep(500);
			midi_out_test(MIDI_NOTE_OFF, 67, 127);
		}
		if (GetAsyncKeyState(0x48))
		{
			midi_out_test(MIDI_NOTE_ON, 68, 127);
			Sleep(500);
			midi_out_test(MIDI_NOTE_OFF, 68, 127);
		}
		if (GetAsyncKeyState(0x4E))
		{
			midi_out_test(MIDI_NOTE_ON, 69, 127);
			Sleep(500);
			midi_out_test(MIDI_NOTE_OFF, 69, 127);
		}
		if (GetAsyncKeyState(0x4A))
		{
			midi_out_test(MIDI_NOTE_ON, 70, 127);
			Sleep(500);
			midi_out_test(MIDI_NOTE_OFF, 70, 127);
		}
		if (GetAsyncKeyState(0x4D))
		{
			midi_out_test(MIDI_NOTE_ON, 71, 127);
			Sleep(500);
			midi_out_test(MIDI_NOTE_OFF, 71, 127);
		}
		
	}
	}
}
