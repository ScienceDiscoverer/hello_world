#include <iostream>
#include <chrono>
#include <thread>
#include <windows.h>
#include <conio.h>

using namespace std;
typedef unsigned long long ull;

const ull mod_rng = 0x10000000000; //2^40 modulus for RNG engine!
const char exit_key = 'q';
const char rock_key = '1';
const char paper_key = '2';
const char scis_key = '3';

int score[2] = {0,0}; // PLAYER : COMPUTER

const char scis[7][12] = {
 
 "   _    _ ",
 "  (_)  / )",
 "    | (_/ ",
 "   _+/    ",
 "  //|\\   ",
 " // | )   ",
 "(/  |/    "

};

const char rock[6][12] = {
 
 "    ___    ",
 "  _/   \\_  ",
 " /  -  # \\ ",
 " |  / -  | ",
 " \\_ \\  \\_/ ",
 "   \\___/   "

};

const char paper[7][12] = {
 
 " _________ ",
 "|    __   |",
 "|  ______ |",
 "|  ______ |",
 "|  ______ |",
 "|     ___ |",
 "|_________|"

};

ull getSeed()
{
	chrono::system_clock::time_point cur_time = chrono::system_clock::now();
    chrono::system_clock::duration seed = cur_time.time_since_epoch();
    return seed.count()%100000000000000/100; //max seed 1*10^12 - 1
}

//Linear Congruential Pseudo Random Number Generator
ull rng(int seed) //seed must be 0 <= seed < mod!
{   //multiplyer also must be multi%8 = 5 !!! For best uniformal distribution!
	const ull multi = 0x4C8F53A795; //multiplier 2.|multi - 1 must be % = 0 for all mod's prime factors|
	const ull inc = 1; //increment 1.|inc and mod must be co-prime|
	const ull mod = mod_rng; //2^40 modulus 3.|multi must be %4 = 0 if mod%4 = 0|
	
	return (multi * seed + inc) % mod;
}

const ull seed = getSeed(); //get seed
ull rnd_num = seed;

//uniform random number distribution of range
int rngRange(int min, int max) //min,max must be non negative!
{
	const ull rngrg = mod_rng;//RNG range
	int range = max - min + 1;
	ull scale = rngrg/range; //big range part to 1 part of range
	rnd_num = rng(rnd_num);
	return rnd_num/scale + min; //downscale
}

void showCursor(bool showFlag) //delete the cursor, muahha!
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag;
	SetConsoleCursorInfo(out, &cursorInfo);
}

void draw(const char obj[][12])
{
	int row;
	row = obj == rock ? 6 : 7;
	for(int i = 0; i < row; ++i)
	{
		cout << "              ";
		if(obj == scis) cout << ' ';
		for(int j = 0; j < 12; ++j)
			cout << obj[i][j];
		
		cout << endl;
	}
}

void drawScore()
{
	cout << "                SCORE:" << endl;
	cout << "                " << score[0] << " : " << score[1] << endl << endl << endl;
}

void drawMenu()
{
	system("CLS");
	cout << "              WELLCOME TO" << endl;
	for(int i = 0; i < 7; ++i)
	{
		for(int j = 0; j < 12; ++j)
			cout << rock[i][j];

		cout << "  ";
		for(int j = 0; j < 12; ++j)
			cout << paper[i][j];
		
		cout << "  ";
		for(int j = 0; j < 12; ++j)
			cout << scis[i][j];
			
		cout << endl;
	}
	cout << endl;
	cout << "               THE GAME" << endl << endl;
	cout << "             Choose wisely:" << endl << endl;
	cout << "Press 1        Press 2        Press 3" << endl;
	cout << "for rock      for paper    for scissors" << endl << endl << endl;
	if(score[0] != 0 || score[1] != 0) //show score if its non-zero
		drawScore();
	cout << "               Press Q" << endl;
	cout << "      To make a tactical retreat" << endl;
}

void drawBattle(const char p_choice[][12], int& b_choice)
{
	system("CLS");
	draw(p_choice);
	cout << endl << endl;
	cout << "             ";
	for(int i = 0; i < 13; ++i)
	{
		cout << rngRange(0, 1);
		this_thread::sleep_for(chrono::milliseconds(150)); //bot thinking speed
	}
	cout << endl << endl;
	if(b_choice == 1)
		draw(rock);
	else if(b_choice == 2)
		draw(paper);
	else
		draw(scis);
}

void playerWin()
{
	score[0]++; // player +1 : bot +0
	cout << endl;
	cout << "           CONGRATULATIONS!" << endl;
	cout << "         You are victorious!" << endl << endl;
	drawScore();
	cout << "   Press any key to return to Menu" << endl;
	_getch();
}

void playerLoose()
{
	score[1]++; // player +0 : bot +1
	cout << endl;
	cout << "                OOOPS..." << endl;
	cout << "        You have been defeated..." << endl << endl;
	drawScore();
	cout << "   Press any key to return to Menu" << endl;
	_getch();
}

void itsDraw()
{
	cout << endl;
	cout << "                DRAW!" << endl;
	cout << "        Your powers are equal!" << endl << endl;
	drawScore();
	cout << "   Press any key to return to Menu" << endl;
	_getch();
}

int main()
{
	char prs_key = ' '; //pressed key
	bool update = true;
	showCursor(false);
	
	while(prs_key != exit_key)
	{
		if(update)
			drawMenu();
		
		prs_key = _getch();
		
		int bot_choice = rngRange(1, 3); //PC decides...
		switch(prs_key)
		{
			case rock_key:
				drawBattle(rock, bot_choice);
				if(bot_choice == 1)
					itsDraw();
				else if(bot_choice == 2)
					playerLoose();
				else
					playerWin();
				update = true;
			break;
			
			case paper_key:
				drawBattle(paper, bot_choice);
				if(bot_choice == 1)
					playerWin();
				else if(bot_choice == 2)
					itsDraw();
				else
					playerLoose();
				update = true;
			break;
			
			case scis_key:
				drawBattle(scis, bot_choice);
				if(bot_choice == 1)
					playerLoose();
				else if(bot_choice == 2)
					playerWin();
				else
					itsDraw();
				update = true;
			break;
			
			default:
				update = false;
		}
	}
	
	return 0;
}
