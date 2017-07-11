#include <iostream>
#include <windows.h>
#include <conio.h>
#include <thread>
#include <chrono>
#include <fstream>

using namespace std;
typedef unsigned long long ull;

const ull mod_rng = 0x10000000000; //2^40 modulus for RNG engine!
const char quit_key = 'q';
const char jump_key = ' ';
const char pl_avatar = '@';
const char ground = '='; //maybe procedural generation of ground etc. via loop??? :MeFromFuture: "OH, yea!!"
const char wall = '|';
const char ramp_up = '/';
const short int gravity = 1;
struct resolution { const short x = 90; const short y = 30; const short sqr = y*x; } res; //resolution

const string original_map = {

"                                                                                          "//1
"                                                                                          "//2
"                                                                                          "//3
"                                                                                          "//4
"                                                                                          "//5
"                                                                                          "//6
"                                                                                          "//7
"                                                                                          "//8
"\\_                                                                                        "//9
"__\\_                                                                                      "//10
"____\\                                                                                     "//11
"    |                                                                                     "//12
"    |                                                                                     "//13
"=== |                                                                                     "//14
" |  |                                                                                     "//15
" |  |                                                                                     "//16
"=== |                                                                                     "//17
" |  |                                                                                     "//18
" |  |                                                                                     "//19
"=== |                                                                                     "//20
" |  |                                /========                                            "//21
" |  |                               /        |                                            "//22
"=== |                              /         |                                            "//23
"    |                             /          |                                            "//24
"____|                            /           |                                            "//25
"##                              /            |                                            "//26
"##                             /             |      /\\  /\\  /\\  /\\  /\\  /\\                "//27
"=========== ===== ===== =======              |      ======================================"//28
"          | |   | |   | |                    |      |                                     "//29
"          | |   | |   | |                    |      |                                     "//30

};

string map;

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

const ull seed = getSeed();
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

class Player
{
	
	const int x;
	int y;
	int v_spd; //vertical speed
	int d_jumps; //double jumps
	int dist_run; //distanced runned
	bool can_d_jump;
	char icon;
	public:
		Player() : x(24)
		{
			y = 26; //resolution sensetive
			v_spd = dist_run = 0;
			d_jumps = 3;
			can_d_jump = true;
			icon = pl_avatar;
		}
		void Put_in_map()
		{
			checkForMoney(); 
			applyPhysics();

			int cur_pos = getPos(0); //current position
			if(!v_spd) //insert in map
			{
				if(map[cur_pos+1] == ramp_up) //go up ramp
					map[cur_pos] = icon, --y;
				else
					map[cur_pos] = icon;
			}
			else
				map[getPos(1)] = icon;
			
			++dist_run; //log distance
		}
		void Jump()
		{
			v_spd = 3; //jumping FORCE!
			if(flying()) ++v_spd;
		}
		bool Is_Dead()
		{
			bool wall_hit, pit_fall; //death causes
			wall_hit = map[getPos(0)+1] == wall || map[getPos(0)+1] == ground ? true : false;
			pit_fall = getPos(0) > res.sqr ? true : false;
			return wall_hit || pit_fall;
		}
		bool Can_Jump()
		{
			bool can_j = !flying() ? true : false;
			if(!can_d_jump && !flying()) can_d_jump = true;
			if(flying() && d_jumps > 0 && can_d_jump)
			{
				can_j = true;
				can_d_jump = false;
				--d_jumps;
			}
			return can_j;
		}
		void DisplayStats()
		{
			cout << "Money: $" << d_jumps << "                                              ";
			cout << "Distance traversed: ";
			if(dist_run < 1000) //convert to km
				cout << dist_run << " m" << endl;
			else if(dist_run%1000 == 0)
				cout << dist_run/1000 << " km        " << endl; //8 spaces to erase trash meters sighn
			else
				cout << dist_run/1000 << " km " << dist_run%1000 << " m" << endl;
		}
		int GetDistRun()
		{
			return dist_run;
		}
	private:
		bool flying(int dst = 1, int x_off = 0) //is player in air? par: distance down, xoffset
		{
			int u_pos = getPos(0, y+dst)+x_off; //position under player
			return map[u_pos] == ' ' || map[u_pos] == '$' || u_pos > res.sqr ? true : false;
		}
		void applyPhysics() //PHYSICS ENGINE!
		{
			if(flying() && flying(1, -1)) //check old position too for edges!
				v_spd -= gravity; //apply gravity
			
			if(!flying() && v_spd < 0) //not flying and negative speed
				v_spd = 0;
			else if(v_spd < 0)
			{
				for(int i = -2; i >= v_spd-1; --i)
				{
					if (!flying(abs(i)))
					{
						y -= i + 1;
						v_spd = 0;
					}
				}
			}
		}
		void checkForMoney() //check for money i.e. double jump power up
		{
			if(v_spd > 0)
			{
				for(int i = y-1; i >= y-v_spd; --i)
				{
					int tmp_pos = getPos(0, i);
					if(map[tmp_pos] == '$')
					{
						++d_jumps;
						map[tmp_pos] = ' ';
					}
				}
			}
			else if(v_spd < 0)
			{
				for(int i = y+1; i <= y+v_spd; ++i)
				{
					int tmp_pos = getPos(0, i);
					if(map[tmp_pos] == '$')
					{
						++d_jumps;
						map[tmp_pos] = ' ';
					}
				}
			}
			else
			{
				int tmp_pos = getPos(0) + 1;
				if(map[tmp_pos] == '$') ++d_jumps;
				if(tmp_pos == ramp_up && map[getPos(0, y-1)+1] == '$') ++d_jumps;
			}
		}
		int getPos(bool new_or_cur, int Y)
		{
			if(new_or_cur)
				return x + (Y -= v_spd) * res.x;
			return x + Y * res.x;
		}
		int getPos(bool new_or_cur) //get position
		{
			if(new_or_cur)
				return x + (y -= v_spd) * res.x;
			return x + y * res.x;
		}
};

class Obstacle
{
	const int y_max;
	const int y_min;
	int y;
	int y_end;
	int length;
	int old_length; //tmp var for some obstacles
	int height;
	short int obs_index; //obstacle index, 0 = none
		
	int getPos()
	{
		return res.x - 1 + y * res.x;
	}
	int getPos(int Y) //custom y
	{
		return res.x - 1 + Y * res.x;
	}
	void genGround() //generate ground
	{
		if(length < 0) length = rngRange(5, 16); //ground min max length
		map[getPos()] = ground;
		if(!length--) obs_index = 0;
	}
	void genHole() //generate pit
	{
		if(length < 0) length = rngRange(2, 6), old_length = length; //pit min max size

		if(length == old_length || !length)
		{
			map[getPos()] = ground;
			for(int i = y+1; i < res.y; ++i)
				map[getPos(i)] = wall;
		}
		else
			map[getPos()] = ' ';
		
		if(!length--) obs_index = 0;
	}
	void genColumn() //generate column(pillar)
	{
		if(length < 0) length = rngRange(1, 10), old_length = length; //column min max length
		if(height < 0) height = rngRange(2, 6); //column min max height

		if(length == old_length || !length)
		{
			map[getPos()] = ground;
			if(!length) y += height;
			for(int i = 1; i < height; ++i)
				map[getPos(y-i)] = wall;
			if(length == old_length) y -= height;
			map[getPos()] = ground;
		}
		else
			map[getPos()] = ground;
		
		if(!length--) obs_index = 0, height = -1;
	}
	void genUpHill()
	{
		if(length < 0 && y == y_max) //if y is at max, get another obstacle!
		{
			obs_index = 0;
			map[getPos()] = ground;
			return;
		}
		
		if(length < 0)
		{
			length = rngRange(4, 7);
			old_length = rngRange(4, 7);
		}
		if(height < 0)
		{
			int tmp = y-y_max+1;
			if(tmp >= 3) height = rngRange(3, tmp);
			else height = 2;
		}
		
		if(old_length)
		{
			map[getPos()] = ground;
			--old_length;
		}
		else if(height)
		{
			map[getPos()] = ramp_up;
			--height;
			--y;
			if(!height) ++y;
		}
		else
		{
			map[getPos()] = ground;
			--length;
		}

		if(!length) obs_index = 0, length = height = -1;
	}
	void genDownHill()
	{
		if(length < 0 && y == y_min) //if y is at min, get another obstacle!
		{
			obs_index = 0;
			map[getPos()] = ground;
			return;
		}
		
		if(length < 0)
		{
			length = rngRange(4, 7);
			old_length = rngRange(4, 7);
		}
		if(height < 0)
		{
			int tmp = y_min-y+1;
			if(tmp >= 3) height = rngRange(3, tmp);
			else height = 2;
		}
		
		if(old_length)
		{
			map[getPos()] = ground;
			--old_length;
		}
		else if(height)
		{
			map[getPos()] = '\\';
			--height;
			++y;
			if(!height) --y;
		}
		else
		{
			map[getPos()] = ground;
			--length;
		}
		
		if(!length) obs_index = 0, length = height = -1;
	}
	void genCliff()
	{
		if(y >= (y_min-y_max)/2 + y_max)
		{
			height = rngRange(2, 6); //cliff min max height left
			map[getPos()] = ground;
			for(int i = y-1; i > y-height; --i)
				map[getPos(i)] = wall;
			
			y -= height;
			map[getPos()] = ground;
			obs_index = 0;
			height = -1;
		}
		else
		{
			int tmp = y_min-y;
			height = rngRange(2, tmp); //cliff min max height right
			map[getPos()] = ground;
			for(int i = y+1; i < y+height; ++i)
				map[getPos(i)] = wall;
			
			y += height;
			map[getPos()] = ground;
			obs_index = 0;
			height = -1;
		}
	}
	void genDoubleJump()
	{
		int y_min_tmp = y - 1;
		if(y > 12)
		{
			int y_max_tmp = y - 12;
			map[getPos(rngRange(y_max_tmp, y_min_tmp))] = '$';
		}
		else
			map[getPos(rngRange(0, y_min_tmp))] = '$';
	}
	public:
		
		Obstacle() : y_max(17), y_min(27) //MIN/MAX height of obstacles resolution sensetive
		{
			y = y_min;
			length = y_end = height = -1;
			obs_index = 0;
		}
		
		void Put_in_map()
		{
			if(!obs_index) obs_index = rngRange(1, 6); //RNG
			
			int dice60 = rngRange(1, 60); //roll 60 sided dice
			if(dice60 == 31) genDoubleJump(); //spawning double jump bonus
			
			switch(obs_index)
			{
				case 1: genGround();
				break;
				case 2: genHole();
				break;
				case 3: genColumn();
				break;
				case 4: genUpHill();
				break;
				case 5: genDownHill();
				break;
				case 6: genCliff();
				break;
			}	
		}
};

void showCursor(bool showFlag) //delete the cursor, muahha!
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag;
	SetConsoleCursorInfo(out, &cursorInfo);
}

void resetCursor(short y = 0) //reset console cursor to (0,0)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD home_coord = {0,y};
	SetConsoleCursorPosition(out, home_coord);
}

void drawMap()
{
	for(int i = 0; i < res.sqr; ++i)
	{
		if(i > 0 && i % res.x == 0)
			cout << endl;
		
		cout << map[i];
	}
}

void moveMap()
{
	for(int i = 0; i < res.sqr; ++i)
	{
		if(map[i] != ' ' && i % res.x) //NOT space AND NOT first element of row
		{
			if(map[i] == pl_avatar) map[i] = ' '; //clear player's "tail"
			map[i-1] = map[i];
			map[i] = ' ';
		}
		else if(i%res.x == 0 && map[i+1] == ' ') //IS first AND nothing ahead
			map[i] = ' ';
	}
}

void insertText(string& m_map, string& text, int y) //insert text in menu dummy map
{
	int mid_pos = res.x * y + res.x/2 - text.size()/2 + 1; //calculate middle position
	for(unsigned int i = mid_pos, j = 0; j < text.size(); ++j, ++i)
		m_map[i] = text[j];
}
void drawMenu()
{
	string menu_map = original_map;
	string msg1 = "Wellcome to Console Runner!";
	string msg2 = "You escaped from Prison Castle, try to run as far as you can.";
	string msg3 = "Space to jump. Double jumps will cost you money.";
	string msg4 = "Further you go, faster you run.";
	string msg5 = "Press any key to start, press Q to exit.";
	insertText(menu_map, msg1, 8);
	insertText(menu_map, msg2, 10);
	insertText(menu_map, msg3, 11);
	insertText(menu_map, msg4, 12);
	insertText(menu_map, msg5, 16);
	menu_map[2364] = pl_avatar;
	cout << endl;
	for(int i = 0; i < res.sqr; ++i)
	{
		if(i > 0 && i % res.x == 0)
			cout << endl;
		
		cout << menu_map[i];
	}
}

void insertGameOverBox()
{
	string text[10];
		text[0] = "______________________________";
		text[1] = "|                            |";
		text[9] = "|____________________________|";
		text[2] = text[3] = text[4] = text[5] = text[6] = text[7] = text[8] = text[1];
		for(int i = 0; i < 10; ++i)
			insertText(map, text[i], 6+i);
}

string convertToString(int& score) //convert to km and string
{
	if(score < 1000)
		return to_string(score) + " m";
	else if(score%1000 == 0)
		return to_string(score/1000) + " km";
	else
		return to_string(score/1000) + " km, " + to_string(score%1000) + " m";
}

void gameOver(int new_score)
{
	fstream score("high_scores.txt"); //open file
	int high_score = 0;
	string date;
	chrono::system_clock::time_point today = chrono::system_clock::now();
	long tt = chrono::system_clock::to_time_t(today);
	string cur_date = ctime(&tt);
	cur_date.pop_back(); //delete newline
	
	if(score.is_open()) //file exist
	{
		score.seekg(0, ios::end);
		if(score.tellg()) //file is not empty
		{
			score.seekg(0, ios::end);
			char tmp = '~';
			while(tmp != '\n')
			{
				score.seekg(-3, ios::cur); //-3 offset anomaly! 0_0
				
				if((int)score.tellg() <= 0) //start of file is start of line
				{
					score.seekg(0);
					break;
				}
				tmp = score.get();
			}

			score >> high_score;
			char date_buff[30];
			score.getline(date_buff, 50);
			date = date_buff;
			date.erase(0, 1); //erase space in the beginning
		}
	}
	else //create file, if there is no file
	{
		score.open("high_scores.txt", ios::out);
		score.close();
		score.open("high_scores.txt");
	}
	
	resetCursor(1);
	if(new_score > high_score)
	{
		string msg1 = "Congratulations!";
		string msg2 = "New Record: ";
		msg2 += convertToString(new_score) + '!';
		string msg3 = "Press P to reset scores,";
		string msg4 = "Q to quit, R to restart...";
		insertGameOverBox();
		insertText(map, msg1, 8);
		insertText(map, msg2, 9);
		insertText(map, cur_date, 11);
		insertText(map, msg3, 13);
		insertText(map, msg4, 14);
		drawMap();
		score << new_score << ' ' << cur_date << endl;
	}
	else if(new_score < high_score)
	{
		string msg1 = "Game over, man, GAME OVER!";
		string msg2 = "Last Record: ";
		msg2 += convertToString(high_score) + '!';
		string msg3 = "Press P to reset scores,";
		string msg4 = "Q to quit, R to restart...";
		insertGameOverBox();
		insertText(map, msg1, 8);
		insertText(map, msg2, 9);
		insertText(map, date, 11);
		insertText(map, msg3, 13);
		insertText(map, msg4, 14);
		drawMap();
	}
	else
	{
		string msg1 = "Good, you got last record!";
		string msg2 = "Last Record: ";
		msg2 += convertToString(high_score) + '!';
		string msg3 = "Press P to reset scores,";
		string msg4 = "Q to quit, R to restart...";
		insertGameOverBox();
		insertText(map, msg1, 8);
		insertText(map, msg2, 9);
		insertText(map, date, 11);
		insertText(map, msg3, 13);
		insertText(map, msg4, 14);
		drawMap();
	}
	score.close();
}

void gameLoop(char& key_prs)
{
	Player dog; // @ - is a dog!
	Obstacle obst;
	map = original_map;
	short int game_speed = 100;
	while(key_prs != 'r' && key_prs != quit_key)
	{
		dog.DisplayStats();
		dog.Put_in_map();
		drawMap();
		
		if(_kbhit())
			key_prs = _getch();
			
		switch(key_prs)
		{
			case jump_key:
				if(dog.Can_Jump())
					dog.Jump();
				key_prs = '~';
			break;
		}
		
		if(dog.Is_Dead())
		{
			gameOver(dog.GetDistRun()-1);
			break;
		}
		
		moveMap();
		obst.Put_in_map(); //procedual map generation
		if(game_speed > 0 && dog.GetDistRun()%10 == 0) --game_speed; //increse game speed every 10 m
		this_thread::sleep_for(chrono::milliseconds(game_speed)); //game speed!
		resetCursor();
	}
}

int main()
{
	showCursor(false); //hide console cursor
	system("mode 91, 31"); //set console window size
	char key_prs; //key pressed
	
	drawMenu();
	if(_getch() == quit_key) return 0;
	resetCursor();
	
	while(key_prs != quit_key)
	{
		key_prs = '~';
		gameLoop(key_prs);
		if(key_prs == '~')
		{
			while(true)
			{
				key_prs = _getch();
				if(key_prs == 'p') //reset highscores
				{
					fstream scores("high_scores.txt", ios::trunc | ios::out);
					scores.close();
					resetCursor(1);
					string txt1 = "|                            |";
					string txt2 = "|     High scores reset!     |";
					string txt3 = "|____________________________|";
					insertText(map, txt1, 16);
					insertText(map, txt2, 17);
					insertText(map, txt3, 18);
					drawMap();
				}
				if(key_prs == 'r' || key_prs == quit_key) break; //MAKE RESET HIGHSCORES KEY VIA TRUNCATE!
			}
		}
		system("CLS");
	}
	
	return 0;
}
