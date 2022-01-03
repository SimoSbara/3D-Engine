#include "3DEngine.h"
#include <math.h>
using namespace std;

// Define our static variables
std::atomic<bool> olcConsoleGameEngine::m_bAtomActive(false);
std::condition_variable olcConsoleGameEngine::m_cvGameFinished;
std::mutex olcConsoleGameEngine::m_muxGame;
Engine3D* currentInstance;

Engine3D::Engine3D()
{
	m_sAppName = L"3D Demo";
	currentInstance = this;
}

bool Engine3D::OnUserDestroy()
{

	isRunning = false;

	return true;
}

DWORD WINAPI checkKeys(LPVOID lpParam)
{
	while (currentInstance->isRunning)
	{
		if (!currentInstance->isDrawing)
		{
			currentInstance->isCheckingKeys = true;

			for (int i = 0; i < currentInstance->keysIndeces.size(); i++)
			{
				if (currentInstance->GetKey(currentInstance->keysIndeces[i]).bHeld)
				{
					currentInstance->activateKeyFunction(currentInstance->keysIndeces[i]);
				}
			}

			currentInstance->isCheckingKeys = false;
			Sleep(120);
		}
	}

	return 0;
}

void Engine3D::activateKeyFunction(int key)
{
	int currentMode = getCurrentMode();

	switch (key)
	{
	case ARROW_UP:
		if (currentMode == SELECT_MODE)
		{
			break;
		}
		else if (currentMode == ROTATE_MODE)
		{
			//codice rotazione
		}
		else if (currentMode == MOVE_MODE)
		{
			if (vertsOffset[currentSelection].z <= 8)
				vertsOffset[currentSelection].z += 0.1f;
		}
		break;
	case ARROW_DOWN:
		if (currentMode == SELECT_MODE)
		{
			break;
		}
		else if (currentMode == ROTATE_MODE)
		{
			//codice rotazione
		}
		else if (currentMode == MOVE_MODE)
		{
			if (vertsOffset[currentSelection].z >= 2)
				vertsOffset[currentSelection].z -= 0.1f;
		}
		break;
	case ARROW_LEFT:
		if (currentMode == SELECT_MODE)
		{
			if (currentSelection > 0)
				currentSelection--;
		}
		else if (currentMode == ROTATE_MODE)
		{
			//codice rotazione
		}
		else if (currentMode == MOVE_MODE)
		{
			vertsOffset[currentSelection].x -= 0.1f;
			//codice movimento
		}
		break;
	case ARROW_RIGHT:
		if (currentMode == SELECT_MODE)
		{
			if (currentSelection < models.size()-1)
				currentSelection++;
		}
		else if (currentMode == ROTATE_MODE)
		{
			//codice rotazione
		}
		else if (currentMode == MOVE_MODE)
		{
			vertsOffset[currentSelection].x += 0.1f;
			//codice movimento
		}
		break;
	case M_KEY:
		setCurrentMode(MOVE_MODE);
		break;
	case S_KEY:
		setCurrentMode(SELECT_MODE);
		break;
	case R_KEY:
		setCurrentMode(ROTATE_MODE);
		break;
	case ESC_KEY:
		break;
	default:
		break;
	}
}

void Engine3D::MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m)
{
		
	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0f)
	{
		o.x /= w; o.y /= w; o.z /= w;
	}
}

void Engine3D::RotateMesh(triangle& i, triangle& o, mat4x4 m)
{
	MultiplyMatrixVector(i.p[0], o.p[0], m);
	MultiplyMatrixVector(i.p[1], o.p[1], m);
	MultiplyMatrixVector(i.p[2], o.p[2], m);
}

void Engine3D::drawModel(mesh model, int index)
{
	// Draw Triangles
	for (auto tri : model.tris)
	{
		triangle triProjected, triTranslated, triRotated;

		// Rotate in X-Axis
		//RotateMesh(tri, triRotated, matRotX);

		/*// Rotate in Z-Axis
		RotateMesh(tri, triRotated, matRotZ);*/

		/*// Rotate in Z-Axis then X-Axis
		RotateMesh(tri, triRotated, matRotZ);
		RotateMesh(triRotated, triRotated, matRotX);*/

		// Offset into the screen
		triTranslated = tri;// triRotated;
		triTranslated.p[0].z = tri.p[0].z + vertsOffset[index].z;
		triTranslated.p[1].z = tri.p[1].z + vertsOffset[index].z;
		triTranslated.p[2].z = tri.p[2].z + vertsOffset[index].z;

		triTranslated.p[0].x = tri.p[0].x + vertsOffset[index].x;
		triTranslated.p[1].x = tri.p[1].x + vertsOffset[index].x;
		triTranslated.p[2].x = tri.p[2].x + vertsOffset[index].x;

		// Project triangles from 3D --> 2D
		MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
		MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
		MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

		// Scale into view
		triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
		triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
		triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
		triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
		triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
		triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
		triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
		triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
		triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

		int currentColor;

		if (currentSelection == index)
		{
			switch (getCurrentMode())
			{
			case SELECT_MODE:
				currentColor = COLOUR::FG_RED;
				break;
			case ROTATE_MODE:
				currentColor = COLOUR::FG_BLUE;
				break;
			case MOVE_MODE:
				currentColor = COLOUR::FG_MAGENTA;
				break;
			}
		}
		else
			currentColor = COLOUR::FG_WHITE;


		// Rasterize triangle
		DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
			triProjected.p[1].x, triProjected.p[1].y,
			triProjected.p[2].x, triProjected.p[2].y,
			PIXEL_SOLID, currentColor);
	}
}

bool Engine3D::OnUserCreate()
{
	mesh meshCube;
	mesh meshPyramid;

	meshCube.tris = {

		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// NORTH                                                     
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		// WEST                                                      
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		// TOP                                                       
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

	};

	meshPyramid.tris = {
		//BOTTOM
		{ 0.0f, 0.0f, 0.0f,    0.5f, 0.0f, (float)sqrt(0.75),    1.0f, 0.0f, 0.0f },

		//FACE1
		{ 0.5f, 0.0f, (float)sqrt(0.75),    0.5f, (float)sqrt(2.25 / 4), (float)(sqrt(0.75) / 2),    0.0f, 0.0f, 0.0f },
			
		//FACE2
		{ 0.0f, 0.0f, 0.0f,    0.5f, (float)sqrt(2.25/4), (float)(sqrt(0.75)/2),    1.0f, 0.0f, 0.0f },

		//FACE3
		{ 1.0f, 0.0f, 0.0f,    0.5f, (float)sqrt(2.25 / 4), (float)(sqrt(0.75) / 2),    0.5f, 0.0f, (float)sqrt(0.75) },
	};

	models.push_back(meshCube);
	models.push_back(meshPyramid);
	vertsOffset.push_back({ 0, 0, 2 });
	vertsOffset.push_back({ 0, 0, 2 });

	// Projection Matrix
	float fNear = 0.1f;
	float fFar = 1000.0f;
	float fFov = 90.0f;
	float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
	float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f;

	k = 2;

	isRunning = true;

	checkKeysThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		checkKeys,       // thread function name
		NULL,          // argument to thread function 
		0,                      // use default creation flags 
		&checkKeysThreadID   // returns the thread identifier 
	);

	return true;
}

bool Engine3D::OnUserUpdate(float fElapsedTime)
{
	// Clear Screen
	Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

	// Set up rotation matrices
	mat4x4 matRotZ, matRotX;
	fTheta += 0.001f;// 2.0f /** fElapsedTime*/;

	// Rotation Z Matrix Population
	matRotZ.m[0][0] = cosf(fTheta);
	matRotZ.m[0][1] = sinf(fTheta);
	matRotZ.m[1][0] = -sinf(fTheta);
	matRotZ.m[1][1] = cosf(fTheta);
	matRotZ.m[2][2] = 1;
	matRotZ.m[3][3] = 1;
		
	// Rotation X Matrix Population
	matRotX.m[0][0] = 1;
	matRotX.m[1][1] = cosf(fTheta );// *0.5f
	matRotX.m[1][2] = sinf(fTheta);
	matRotX.m[2][1] = -sinf(fTheta);
	matRotX.m[2][2] = cosf(fTheta);
	matRotX.m[3][3] = 1;

	while (isCheckingKeys);

	isDrawing = true;

	for (int i = 0; i < models.size(); i++)
	{
		drawModel(models[i], i);
	}

	DrawString(0, 0, L"S - Select Mode\n R - Rotate Mode\n M - Move Mode");

	isDrawing = false;

	return true;
}