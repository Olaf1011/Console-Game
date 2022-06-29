#include <iostream>
#include <ctime>
#include <windows.h>

#include "Timer.hpp"

#define TO_RAD(x) x * 3.14159 / 180

struct {
	int x, y;
	struct {
		int x, y;
	}dir;
}g_ball;


static constexpr char gsk_emptySpace = ' ';
static constexpr char gsk_wall = 'w';

static constexpr int gsk_widthGame	= 163;
static constexpr int gsk_heightGame = 21;

static char gs_game[gsk_heightGame][gsk_widthGame];
static char gs_gameLine[gsk_heightGame * gsk_widthGame];
static bool gs_isDirty = true;
static double gs_timer = 0.0f;
static int gs_counter = 1;
using namespace std;


int RandomPosX()
{
	return 1 + (rand() % (gsk_widthGame - 2));
}

int RandomPosY()
{
	return 1 + (rand() % (gsk_heightGame - 2));
}

void PrintBox()
{
	int counter = 0;
	for (int i = 0; i < gsk_heightGame; ++i)
	{
		for (int j = 0; j < gsk_widthGame; ++j)
		{
			gs_gameLine[counter] = gs_game[i][j];
			++counter;
		}
	}

	cout.write(gs_gameLine, counter);
}

void Clear()
{
	system("cls");
}

void Collision()
{
	const int pos_x = g_ball.x + g_ball.dir.x;
	const int pos_y = g_ball.y + g_ball.dir.y;

	bool changed = false;
	if (pos_x > gsk_widthGame || pos_x < 0)
	{ 
		g_ball.dir.x = 0;
		changed = true;
	}
	if (pos_y > gsk_heightGame || pos_y < 0)
	{
		g_ball.dir.y = 0;
		changed = true;
	}

	if (!changed)
	{
		if (gs_game[pos_y][pos_x] != gsk_emptySpace)
		{
				g_ball.dir.x = 0;
				g_ball.dir.y = 0;
		}
	}

}

void Controlls()
{
	if (GetAsyncKeyState(VK_UP))
		g_ball.dir.y = -1;
	else if (GetAsyncKeyState(VK_DOWN))
		g_ball.dir.y = 1;
	else
		g_ball.dir.y = 0;

	if (GetAsyncKeyState(VK_RIGHT))
		g_ball.dir.x = 1;
	else if (GetAsyncKeyState(VK_LEFT))
		g_ball.dir.x = -1;
	else
		g_ball.dir.x = 0;
}

void Update(const double deltaTime)
{
	gs_timer += deltaTime;
	if (gs_timer > 256.0 || gs_isDirty)
	{
		//s_box[g_ball.x][g_ball.y] = s_emptySpace; //Clean previous position

		//Controlls();


		g_ball.y = -int(sin(TO_RAD(gs_counter * 10.f)) * 10.f);
		g_ball.x = gs_counter++;
		Collision();

		g_ball.y += gsk_heightGame / 2;

		//g_ball.x += g_ball.dir.x;
		//g_ball.y += g_ball.dir.y;

		gs_timer = 0.0;
		//if(g_ball.dir.x != 0 || g_ball.dir.y != 0)
		if(g_ball.x < gsk_widthGame - 2)
			gs_isDirty = true;
	}
}

void CreateField()
{
	for (int i = 0; i < gsk_heightGame; ++i)
	{
		for (int j = 0; j < gsk_widthGame; ++j)
		{
			if (j == gsk_widthGame - 1)
				gs_game[i][j] = '\n';
			else if ((i == 0 || i == gsk_heightGame - 1) || (j == 0 || j == gsk_widthGame - 2))
				gs_game[i][j] = gsk_wall;
			else
				gs_game[i][j] = gsk_emptySpace;
		}
	}
}

int main()
{
	Timer timer;
	std::srand(std::time(nullptr));

	//g_ball.x = RandomPosX();
	//g_ball.y = RandomPosY();
	//g_ball.x = 1;
	//g_ball.y = gsk_heightGame / 2;

	CreateField();

	//for (int i = 20; i != 0; --i)
	//{
	//	gs_game[RandomPosY()][RandomPosX()] = gsk_wall;
	//}

	timer.start();
	double time = 0.0;

	while(!GetAsyncKeyState(VK_ESCAPE))
	{
		timer.start();
		Update(time);
		Sleep(1);

		if (gs_isDirty)
		{
			Clear();
			gs_game[g_ball.y][g_ball.x] = 'x';

			PrintBox();
			Sleep(3);
			gs_isDirty = false;
		}
		timer.stop();
		time = timer.elapsedMilliseconds();

		if (GetAsyncKeyState(0xC0)) // 0xC0 '`~'
		{
			std::string input;
			cout << "Input: ";
			cin >> input;
			cout << "Okay";
		}
	}

	return 0;
}