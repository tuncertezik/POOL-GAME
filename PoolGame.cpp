// Pool Game.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<glut.h>
#include<math.h>
#include<string.h>
#include"simulation.h"

#define PLAYER_1			("Player 1")
#define PLAYER_2			("Player 2")

//cue variables
float gCueAngle = 0.0;
float gCuePower = 0.25;
bool gCueControl[4] = {false,false,false,false};
float gCueAngleSpeed = 2.0f; //radians per second
float gCuePowerSpeed = 0.25f;
float gCuePowerMax = 0.75;
float gCuePowerMin = 0.1;
float gCueBallFactor = 8.0;
bool gDoCue = true;

//camera variables
vec3 gCamPos(0.0,0.7,2.25);
vec3 gCamLookAt(0.0,0.0,0.0);
bool gCamRotate = true;
float gCamRotSpeed = 0.2;
float gCamMoveSpeed = 0.5;
bool gCamL = false;
bool gCamR = false;
bool gCamU = false;
bool gCamD = false;
bool gCamZin = false;
bool gCamZout = false;

//rendering options
#define DRAW_SOLID	(0)

void DoCamera(int ms)
{
	static const vec3 up(0.0,1.0,0.0);

	if(gCamRotate)
	{
		if(gCamL)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localL = up.Cross(camDir);
			vec3 inc = (localL* ((gCamRotSpeed*ms)/1000.0) );
			gCamLookAt = gCamPos + camDir + inc;
		}
		if(gCamR)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = up.Cross(camDir);
			vec3 inc = (localR* ((gCamRotSpeed*ms)/1000.0) );
			gCamLookAt = gCamPos + camDir - inc;
		}
		if(gCamU)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 localUp = localR.Cross(camDir);
			vec3 inc = (localUp* ((gCamMoveSpeed*ms)/1000.0) );
			gCamLookAt = gCamPos + camDir + inc;
		}
		if(gCamD)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 localUp = localR.Cross(camDir);
			vec3 inc = (localUp* ((gCamMoveSpeed*ms)/1000.0) );
			gCamLookAt = gCamPos + camDir - inc;
		}		
	}
	else
	{
		if(gCamL)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localL = up.Cross(camDir);
			vec3 inc = (localL* ((gCamMoveSpeed*ms)/1000.0) );
			gCamPos += inc;
			gCamLookAt += inc;
		}
		if(gCamR)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 inc = (localR* ((gCamMoveSpeed*ms)/1000.0) );
			gCamPos += inc;
			gCamLookAt += inc;
		}
		if(gCamU)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 localUp = localR.Cross(camDir);
			vec3 inc = (localUp* ((gCamMoveSpeed*ms)/1000.0) );
			gCamPos += inc;
			gCamLookAt += inc;
		}
		if(gCamD)
		{
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 localDown = camDir.Cross(localR);
			vec3 inc = (localDown* ((gCamMoveSpeed*ms)/1000.0) );
			gCamPos += inc;
			gCamLookAt += inc;
		}
	}

	if(gCamZin)
	{
		vec3 camDir = (gCamLookAt - gCamPos).Normalised();
		vec3 inc = (camDir* ((gCamMoveSpeed*ms)/1000.0) );
		gCamPos += inc;
		gCamLookAt += inc;
	}
	if(gCamZout)
	{
		vec3 camDir = (gCamLookAt - gCamPos).Normalised();
		vec3 inc = (camDir* ((gCamMoveSpeed*ms)/1000.0) );
		gCamPos -= inc;
		gCamLookAt -= inc;
	}
}

void DrawText(float x, float y, const char *string)
{
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(x, y);
	int len, i;
	len = (int)strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter((void *)GLUT_BITMAP_9_BY_15, string[i]);
	}
	glPopMatrix();
}

void DrawCircle(float cx, float cy, float r, int num_segments)
{
	glBegin(GL_TRIANGLE_FAN);
	for (int ii = 0; ii < num_segments; ii++)
	{
		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle

		float x = r * cosf(theta);//calculate the x component
		float y = r * sinf(theta);//calculate the y component

		glVertex2f(x + cx, y + cy);//output vertex

	}
	glEnd();
}

void DrawScoreboard(void)
{
	glPushMatrix();
	//glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	//glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
	// last row
	glVertex3f(-1.0f, 0.0f, -5.0f);
	glVertex3f(1.0f, 0.0f, -5.0f);
	glVertex3f(1.0f, 0.25f, -5.0f);
	glVertex3f(-1.0f, 0.25f, -5.0f);
	// middle row
	glVertex3f(-1.0f, 0.25f, -5.0f);
	glVertex3f(1.0f, 0.25f, -5.0f);
	glVertex3f(1.0f, 0.5f, -5.0f);
	glVertex3f(-1.0f, 0.5f, -5.0f);
	// top row
	glVertex3f(-1.0f, 0.5f, -5.0f);
	glVertex3f(1.0f, 0.5f, -5.0f);
	glVertex3f(1.0f, 0.75f, -5.0f);
	glVertex3f(-1.0f, 0.75f, -5.0f);
	// split column
	glVertex3f(0.4f, 0.75f, -5.0f);
	glVertex3f(0.4f, 0.0f, -5.0f);
	glVertex3f(-1.0f, 0.0f, -5.0f);
	glVertex3f(-1.0f, 0.75f, -5.0f);
	glEnd();
	glPopMatrix();

	char buf[3] = { 0 };
	DrawText(-0.27, 0.67, "Player");
	DrawText(0.17, 0.67, "Score");
	DrawText(-0.27, 0.60, gTable.sb.players[0].GetName());
	sprintf(buf, "%d", gTable.sb.players[0].GetScore());
	DrawText(0.17, 0.60, buf);
	DrawText(-0.27, 0.52, gTable.sb.players[1].GetName());
	sprintf(buf, "%d", gTable.sb.players[1].GetScore());
	DrawText(0.17, 0.52, buf);
	
}

void RenderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set camera
	glLoadIdentity();
	gluLookAt(gCamPos(0),gCamPos(1),gCamPos(2),gCamLookAt(0),gCamLookAt(1),gCamLookAt(2),0.0f,1.0f,0.0f);

	//draw the ball
	int ballsInPocket = 0;
	for(int i=1;i<NUM_BALLS;i++)
	{
		if (gTable.balls[i].pPocket == NULL)
		{
			glColor3f(0.0, 0.0, 1.0);
			glPushMatrix();
			glTranslatef(gTable.balls[i].position(0), (BALL_RADIUS / 2.0), gTable.balls[i].position(1));
#if DRAW_SOLID
			glutSolidSphere(gTable.balls[i].radius, 32, 32);
#else
			glutWireSphere(gTable.balls[i].radius, 12, 12);
#endif
			glPopMatrix();
		}
		else
		{
			ballsInPocket++;
		}
	}
	
	// draw cue ball
	if (ballsInPocket < (NUM_BALLS - 1) && gTable.balls[0].pPocket == NULL)
	{
		glColor3f(1.0, 1.0, 1.0);
		glPushMatrix();
		glTranslatef(gTable.balls[0].position(0), (BALL_RADIUS / 2.0), gTable.balls[0].position(1));
#if DRAW_SOLID
		glutSolidSphere(gTable.balls[0].radius, 32, 32);
#else
		glutWireSphere(gTable.balls[0].radius, 12, 12);
#endif
		glPopMatrix();
	}
		
	//draw the table
	glColor3f(1.0, 1.0, 1.0);
	for(int i=0;i<NUM_CUSHIONS;i++)
	{	
		glBegin(GL_LINE_LOOP);
		glVertex3f (gTable.cushions[i].vertices[0](0), 0.0, gTable.cushions[i].vertices[0](1));
		glVertex3f (gTable.cushions[i].vertices[0](0), 0.1, gTable.cushions[i].vertices[0](1));
		glVertex3f (gTable.cushions[i].vertices[1](0), 0.1, gTable.cushions[i].vertices[1](1));
		glVertex3f (gTable.cushions[i].vertices[1](0), 0.0, gTable.cushions[i].vertices[1](1));
		glEnd();
	}

	//draw the pockets
	glPushMatrix();
	glRotatef(-90, 1.0, 0, 0);
	// bottom left
	DrawCircle(gTable.pockets[0].position(0), gTable.pockets[0].position(1), gTable.pockets[0].raduis, 20);
	// bottom right
	DrawCircle(gTable.pockets[1].position(0), gTable.pockets[1].position(1), gTable.pockets[0].raduis, 20);
	// top left
	DrawCircle(gTable.pockets[2].position(0), gTable.pockets[2].position(1), gTable.pockets[0].raduis, 20);
	// top right
	DrawCircle(gTable.pockets[3].position(0), gTable.pockets[3].position(1), gTable.pockets[0].raduis, 20);
	// middle left
	DrawCircle(gTable.pockets[4].position(0), gTable.pockets[4].position(1), gTable.pockets[0].raduis, 20);
	// middle right
	DrawCircle(gTable.pockets[5].position(0), gTable.pockets[5].position(1), gTable.pockets[0].raduis, 20);
	glPopMatrix();

	for(int i=0;i<gTable.parts.num;i++)
	{
		glColor3f(1.0,0.0,0.0);
		glPushMatrix();
		glTranslatef(gTable.parts.particles[i]->position(0),gTable.parts.particles[i]->position(1),gTable.parts.particles[i]->position(2));
		#if DRAW_SOLID
		glutSolidSphere(0.002f,32,32);
		#else
		glutWireSphere(0.002f,12,12);
		#endif
		glPopMatrix();		
	}

	//draw the cue
	char text[100] = { 0 };
	if(gDoCue && ballsInPocket < (NUM_BALLS-1))
	{
		// remove cue ball from pocket
		if (gTable.balls[0].pPocket != NULL)
		{
			gTable.balls[0].pPocket = NULL;
			gTable.balls[0].Reset();
		}
		glBegin(GL_LINES);
		float cuex = sin(gCueAngle) * gCuePower;
		float cuez = cos(gCueAngle) * gCuePower;
		glColor3f(1.0,0.0,0.0);
		glVertex3f (gTable.balls[0].position(0), (BALL_RADIUS/2.0f), gTable.balls[0].position(1));
		glVertex3f ((gTable.balls[0].position(0)+cuex), (BALL_RADIUS/2.0f), (gTable.balls[0].position(1)+cuez));
		glColor3f(1.0,1.0,1.0);
		glEnd();

		//draw player name
		if (gTable.sb.player1Turn)
			strcpy(text, gTable.sb.players[0].GetName());
		else
			strcpy(text, gTable.sb.players[1].GetName());

		strcat(text, " take shot");
	}

	//draw scoreboard
	DrawScoreboard();

	//declare winner if all balls are in pocket
	if (ballsInPocket == (NUM_BALLS - 1))
	{
		if (gTable.sb.players[0].GetScore() > gTable.sb.players[1].GetScore())
			sprintf(text, "%s wins the game!", gTable.sb.players[0].GetName());
		else if (gTable.sb.players[1].GetScore() > gTable.sb.players[0].GetScore())
			sprintf(text, "%s wins the game!", gTable.sb.players[1].GetName());
		else
			sprintf(text, "It's a draw!");
	}

	DrawText(-0.2, 0.4, text);

	glFlush();
	glutSwapBuffers();
}

void SpecKeyboardFunc(int key, int x, int y) 
{
	switch(key)
	{
		case GLUT_KEY_LEFT:
		{
			gCueControl[0] = true;
			break;
		}
		case GLUT_KEY_RIGHT:
		{
			gCueControl[1] = true;
			break;
		}
		case GLUT_KEY_UP:
		{
			gCueControl[2] = true;
			break;
		}
		case GLUT_KEY_DOWN:
		{
			gCueControl[3] = true;
			break;
		}
	}
}

void SpecKeyboardUpFunc(int key, int x, int y) 
{
	switch(key)
	{
		case GLUT_KEY_LEFT:
		{
			gCueControl[0] = false;
			break;
		}
		case GLUT_KEY_RIGHT:
		{
			gCueControl[1] = false;
			break;
		}
		case GLUT_KEY_UP:
		{
			gCueControl[2] = false;
			break;
		}
		case GLUT_KEY_DOWN:
		{
			gCueControl[3] = false;
			break;
		}
	}
}

void KeyboardFunc(unsigned char key, int x, int y) 
{
	switch(key)
	{
	case(13):
		{
			if(gDoCue)
			{
				vec2 imp(	(-sin(gCueAngle) * gCuePower * gCueBallFactor),
							(-cos(gCueAngle) * gCuePower * gCueBallFactor));
				gTable.balls[0].ApplyImpulse(imp);
				gTable.sb.player1Turn = !gTable.sb.player1Turn;
			}
			break;
		}
	case(27):
		{
			for(int i=0;i<NUM_BALLS;i++)
			{
				gTable.balls[i].Reset();
			}
			break;
		}
	case(32):
		{
			gCamRotate = false;
			break;
		}
	case('z'):
		{
			gCamL = true;
			break;
		}
	case('c'):
		{
			gCamR = true;
			break;
		}
	case('s'):
		{
			gCamU = true;
			break;
		}
	case('x'):
		{
			gCamD = true;
			break;
		}
	case('f'):
		{
			gCamZin = true;
			break;
		}
	case('v'):
		{
			gCamZout = true;
			break;
		}
	}

}

void KeyboardUpFunc(unsigned char key, int x, int y) 
{
	switch(key)
	{
	case(32):
		{
			gCamRotate = true;
			break;
		}
	case('z'):
		{
			gCamL = false;
			break;
		}
	case('c'):
		{
			gCamR = false;
			break;
		}
	case('s'):
		{
			gCamU = false;
			break;
		}
	case('x'):
		{
			gCamD = false;
			break;
		}
	case('f'):
		{
			gCamZin = false;
			break;
		}
	case('v'):
		{
			gCamZout = false;
			break;
		}
	}
}

void ChangeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0) h = 1;
	float ratio = 1.0* w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45,ratio,0.2,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(0.0,0.7,2.1, 0.0,0.0,0.0, 0.0f,1.0f,0.0f);
	gluLookAt(gCamPos(0),gCamPos(1),gCamPos(2),gCamLookAt(0),gCamLookAt(1),gCamLookAt(2),0.0f,1.0f,0.0f);
}

void InitLights(void)
{
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	GLfloat light_ambient[] = { 2.0, 2.0, 2.0, 1.0 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);
}

void UpdateScene(int ms) 
{
	if(gTable.AnyBallsMoving()==false) gDoCue = true;
	else gDoCue = false;

	if(gDoCue)
	{
		if(gCueControl[0]) gCueAngle -= ((gCueAngleSpeed * ms)/1000);
		if(gCueControl[1]) gCueAngle += ((gCueAngleSpeed * ms)/1000);
		if (gCueAngle <0.0) gCueAngle += TWO_PI;
		if (gCueAngle >TWO_PI) gCueAngle -= TWO_PI;

		if(gCueControl[2]) gCuePower += ((gCuePowerSpeed * ms)/1000);
		if(gCueControl[3]) gCuePower -= ((gCuePowerSpeed * ms)/1000);
		if(gCuePower > gCuePowerMax) gCuePower = gCuePowerMax;
		if(gCuePower < gCuePowerMin) gCuePower = gCuePowerMin;
	}

	DoCamera(ms);

	gTable.Update(ms);

	glutTimerFunc(SIM_UPDATE_MS, UpdateScene, SIM_UPDATE_MS);
	glutPostRedisplay();
}

int _tmain(int argc, _TCHAR* argv[])
{
	gTable.SetupCushions();
	gTable.SetupPockets();
	gTable.sb.SetupPlayers(PLAYER_1, PLAYER_2);
	gTable.sb.player1Turn = true;
	gTable.balls[0].SetCueBall();

	glutInit(&argc, ((char **)argv));
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE| GLUT_RGBA);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(1000,700);
	//glutFullScreen();
	glutCreateWindow("MSc Workshop : Pool Game");
	#if DRAW_SOLID
	InitLights();
	#endif
	glutDisplayFunc(RenderScene);
	glutTimerFunc(SIM_UPDATE_MS, UpdateScene, SIM_UPDATE_MS);
	glutReshapeFunc(ChangeSize);
	glutIdleFunc(RenderScene);
	
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(KeyboardFunc);
	glutKeyboardUpFunc(KeyboardUpFunc);
	glutSpecialFunc(SpecKeyboardFunc);
	glutSpecialUpFunc(SpecKeyboardUpFunc);
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
}
