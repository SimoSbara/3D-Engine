#pragma once
#include <olcConsoleGameEngine.h>
#include <vector>
#include <atomic>

#define ARROW_UP 38
#define ARROW_DOWN 40
#define ARROW_LEFT 37
#define ARROW_RIGHT 39

#define M_KEY	77
#define S_KEY	83
#define R_KEY	82
#define ESC_KEY 27

#define SELECT_MODE 1
#define ROTATE_MODE 2
#define MOVE_MODE 3

class Engine3D : public olcConsoleGameEngine
{
public:
	Engine3D();

	struct vec3d
	{
		float x, y, z;
	};

	struct triangle
	{
		vec3d p[3];
	};

	struct mesh
	{
		std::vector<triangle> tris;
	};

	struct mat4x4
	{
		float m[4][4] = { 0 };
	};

	std::vector<int> keysIndeces = { ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT, M_KEY, S_KEY, R_KEY, ESC_KEY };

	void activateKeyFunction(int key);

	std::atomic<bool> isDrawing = false;
	std::atomic<bool> isRunning = false;
	std::atomic<bool> isCheckingKeys = false;

private:

	std::vector<mesh> models;
	std::vector<vec3d> vertsOffset;

	HANDLE checkKeysThread;
	DWORD checkKeysThreadID;

	int currentSelection;

	mat4x4 matProj;

	float k = 0;

	bool meshFar = false;
	float fTheta;

	bool selectMode = true;
	bool rotateMode = false;
	bool moveMode = false;

	void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m);
	void RotateMesh(triangle& i, triangle& o, mat4x4 m);
	bool OnUserCreate();
	bool OnUserUpdate(float fElapsedTime);
	bool OnUserDestroy();

	int getCurrentMode()
	{
		if (selectMode)
			return 1;
		else if (rotateMode)
			return 2;
		else if (moveMode)
			return 3;

		return 0;
	}

	void setCurrentMode(int mode)
	{
		if (mode == SELECT_MODE)
		{
			rotateMode = 0;
			selectMode = 1;
			moveMode = 0;
		}
		else if (mode == ROTATE_MODE)
		{
			rotateMode = 1;
			selectMode = 0;
			moveMode = 0;
		}
		else if (mode == MOVE_MODE)
		{
			rotateMode = 0;
			selectMode = 0;
			moveMode = 1;
		}
	}

	void drawModel(mesh model, int index);
};