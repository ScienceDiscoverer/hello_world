#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h>

#define X 80
#define Y 40
#define MAX_STACK 128

using namespace std;

const string map_empty(X*Y, ' ');
string map = map_empty;

void drawLine(char x1, char y1, char x2, char y2, char brush);
void drawRectangle(char x, char y, char width, char height, char brush);
void drawPoint(char x, char y, char brush);
void drawTree(char x, char y, char type);
void drawBoulder(char x, char y);

enum Instruction
{
	LITERAL        = 0x00,
	DRAW_LINE      = 0x01,
	DRAW_RECTANGLE = 0x02,
	DRAW_POINT     = 0x03,
	DRAW_TREE      = 0x04,
	DRAW_BOULDER   = 0x05
};

class VM
{
public:
	VM() : stack_size(0) {}
	
	void interpret(string& data)
	{
		for(int i = 0; i < data.size(); ++i)
		{
			//Instruction instruct = (Instruction)data[i];
			
			switch(data[i])
			{
				case LITERAL:
					push(data[++i]);
					break;
				
				case DRAW_LINE:
				{
					char brush = pop();
					char y2 = pop();
					char x2 = pop();
					char y1 = pop();
					char x1 = pop();
					drawLine(x1, y1, x2, y2, brush);
					break;
				}
				
				case DRAW_RECTANGLE:
				{
					char brush = pop();
					char h = pop();
					char w = pop();
					char y = pop();
					char x = pop();
					cout << "Drawing rect: " << (int)x << " " << (int)y << " " 
					<< (int)w << " "  << (int)h << " "  << (int)brush << endl;
					drawRectangle(x, y, w, h, brush);
					break;
				}
					
				case DRAW_POINT:
				{
					char brush = pop();
					char y = pop();
					char x = pop();
					drawPoint(x, y, brush);
					break;
				}
					
				case DRAW_TREE:
				{
					char type = pop();
					char y = pop();
					char x = pop();
					drawTree(x, y, type);
					break;
				}
				
				case DRAW_BOULDER:
				{
					char y = pop();
					char x = pop();
					drawBoulder(x, y);
					break;
				}
			}
		}
	}
	
private:
	void push(char val)
	{
		if(stack_size == MAX_STACK)
		{
			cerr << "VM: STACK OVERFLOW!" << endl;
			return;
		}
		stack[stack_size++] = val;
	}
	
	char pop()
	{
		if(!stack_size)
		{
			cerr << "VM: ATTEMPT TO POP EMPTY STACK!" << endl;
			return -1;
		}
		
		return stack[--stack_size];
	}
	
	unsigned short stack_size;
	char stack[MAX_STACK];
};

bool loadLevel(string& file_name)
{
	VM v_machine;
	ifstream file(file_name.c_str(), ifstream::binary);
	
	if(!file.is_open()) return false;
	
	file.seekg(0, file.end);
	int file_size = file.tellg();
	file.seekg(0, file.beg);
	
	char buffer[file_size];
	file.read(buffer, file_size);	
	file.close();
	
	string bytecode(buffer, file_size);
	
	v_machine.interpret(bytecode);
	
	return true;
}

int main()
{
	system("mode 80, 41");
	bool gameOver = false;
	
	while(!gameOver)
	{
		//if(_kbhit())
		//	if(_getch() == 'q') gameOver = true;
		
		string lvl_name;
		cout << "Enter level file name to load: ";
		cin >> lvl_name;
		map = map_empty;
		
		bool lvl_loaded = loadLevel(lvl_name);
		
		if(!lvl_loaded) cout << "Map loading failed!" << endl;
		else cout << map;
		//sleep(40);
	}
	
	return 0;
}

void drawLine(char x1, char y1, char x2, char y2, char brush)
{
	if(x1 != x2 && y1 != y2) return;
	if(x1 > X || y1 > Y || x2 > X || y2 > Y) return;
	
	if(y1 == y2)
	{
		for(int i = y1 * X + x1; i < y1 * X + x2; ++i)
			map[i] = brush;
	}
	
	if(x1 == x2)
	{
		for(int i = y1 * X + x1; i < y2 * X + x1; i += X)
			map[i] = brush;
	}
}

void drawRectangle(char x, char y, char width, char height, char brush)
{
	--width, --height;
	if(x > X || y > Y || x+width > X || y+height > Y) return;
	
	drawLine(x, y, x+width, y, brush);
	drawLine(x, y, x, y+height, brush);
	drawLine(x+width, y, x+width, y+height+1, brush);
	drawLine(x, y+height, x+width, y+height, brush);
}

void drawPoint(char x, char y, char brush)
{
	if(x > X || y > Y ) return;
	
	map[y * X + x] = brush;
}

void drawTree(char x, char y, char type)
{
	if(x+3 > X || y+3 > Y || x-3 < 0 || y-3 < 0) return;
	
	if(type == 1)
	{
		drawPoint(x, y, 'H');
		drawPoint(x, y-1, 'H');
		drawLine(x-3, y-2, x+4, y-2, '^');
		drawLine(x-2, y-3, x+3, y-3, '^');
		drawLine(x-1, y-4, x+2, y-4, '^');
		drawPoint(x, y-5, '^');
	}
	else if(type == 2)
	{
		drawLine(x-3, y-2, x+3, y-2, '_');
		drawLine(x-1, y-3, x+4, y-3, '_');
		drawLine(x-4, y-4, x+2, y-4, '_');
		drawLine(x-2, y-5, x+1, y-5, '_');
		drawLine(x, y-5, x, y+1, 'I');
	}
}

void drawBoulder(char x, char y)
{
	if(x+3 > X || y+3 > Y || x-3 < 0 || y-3 < 0) return;
	
	drawLine(x-3, y, x+4, y, '&');
	drawLine(x-2, y+1, x+3, y+1, '&');
	drawLine(x-2, y-1, x+3, y-1, '&');
	drawLine(x-1, y-2, x+2, y-2, '&');
	drawLine(x-1, y+2, x+2, y+2, '&');
}