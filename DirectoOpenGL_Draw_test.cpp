#include <gl/freeglut.h>
#include <cmath>
#include <iostream>

struct Color
{
	float r;
	float g;
	float b;
};

bool spr = true;
float spd = 0.5f;

struct Player
{
	Player()
	{
		//sprite = new Color[30];
		Color tmp;
		tmp.r = 1.0f;
		tmp.g = 0.8f;
		tmp.b = 0.5f;
		
		for(int i = 0; i < 30; ++i)
		{
			sprite[i] = tmp;
		}
		
		Color black;
		black.r = 0.0f;
		black.g = 0.0f;
		black.b = 0.0f;
		
		sprite[2] = black, sprite[3] = black, sprite[4] = black;
		sprite[2+5] = black, sprite[3+5] = black, sprite[4+5] = black;
		sprite[2+5*2] = black, sprite[3+5*2] = black, sprite[4+5*2] = black;
		sprite[3+5*3] = black;
		sprite[3+5*4] = black;
		sprite[1+5*5] = sprite[2+5*5] = sprite[3+5*5] = sprite[4+5*5] = black;
		
		x = 10.f, y = 12.f;
	}
	
	void Render(Color* buf)
	{
		int ix = x;
		int iy = y;
		
		Color black;
		black.r = 0.0f;
		black.g = 0.0f;
		black.b = 0.0f;
		
		for(int i = 0; i < 6; ++i)
		{
			for(int j = 0; j < 5; ++j)
			{
				buf[500*(iy+i) + ix + j] = spr ? sprite[5*i + j] : black; //LOL
			}
		}
	}
	
	Color sprite[30];
	float x, y;
};

Color* myBuffer;
Player pl;

Color* buildBuffer()
{
	Color* buff = new Color[500*500];
	
	Color tmp;
	tmp.r = 1.0f;
	tmp.g = 0.8f;
	tmp.b = 0.5f;
	
	for(int i = 0; i < 500*500; ++i)
	{
		buff[i] = tmp;
	}
	
	return buff;
}

void clearMyBuffer()
{
	Color tmp;
	tmp.r = 1.0f;
	tmp.g = 0.8f;
	tmp.b = 0.5f;
	
	for(int i = 0; i < 500*500; ++i)
	{
		myBuffer[i] = tmp;
	}
}

void renderWorld()
{
	
	//glClear(GL_COLOR_BUFFER_BIT); //I DONT HEED THIS HAHAHAH!!! SUCK IT, OPENGL!!
	
	//clearMyBuffer();
	
	pl.Render(myBuffer);
	
	glDrawPixels(500, 500, GL_RGB, GL_FLOAT, myBuffer);
	
	glutSwapBuffers();
}

void asciiKeyPressed(unsigned char k, int x, int y)
{
	switch(k)
	{
		case 'i': pl.y += spd; break;
		case 'k': pl.y -= spd; break;
		case 'j': pl.x -= spd; break;
		case 'l': pl.x += spd; break;
		
		case 's': spr ? spr = false : spr = true; break;
		case 'w': spd += 0.3f; break;
	}
}

int main(int argc, char *argv[])
{
    myBuffer = buildBuffer();
	
	// initialise GLUT + create Window
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
    glutInitWindowPosition(400, 400);
    glutInitWindowSize(500, 500);
    glutCreateWindow("DirectDraw");
    
    // register callbacks for winAPI
    glutDisplayFunc(renderWorld);
    glutIdleFunc(renderWorld); // render scene every frame even when idle
    //glutIgnoreKeyRepeat(1); // ignore annoying windows key repeat delay
    
    // user input callbacks
    glutKeyboardFunc(asciiKeyPressed); // process "normal" keys
    //glutSpecialFunc(specKeyPressed); // process "special" keys
    //glutSpecialUpFunc(specKeyReleased); // check if "special" key is released
    //glutMouseFunc(mouseButton); // register mouse buttons press
    //glutMotionFunc(mouseMove); // register mouse movement with pressed button
    
    // OpenGL initialisation
    
    // enter glut event processing infinite loop
    glutMainLoop();
    
    delete[] myBuffer;
	    
    return 0;
}
