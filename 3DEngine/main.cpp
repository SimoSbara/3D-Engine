#include "3DEngine.h"

int main()
{
	Engine3D demo;
	if (demo.ConstructConsole(256, 218, 4, 4))
		demo.Start();
	return 0;
}