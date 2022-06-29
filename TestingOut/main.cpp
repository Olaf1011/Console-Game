#include <iostream>
#include <ctime>
#include <windows.h>
#include <algorithm>

#include "Timer.hpp"

#define TO_RAD(x) x * 3.14159 / 180

struct Character{
	Character(char c) : character(c), x(1), y(1) {};
	const char character;
	int x, y;
	struct {
		int x = 0, y = 0;
	}dir;
};

static Character gs_player{ 's' };
static constexpr char gsk_emptySpace = ' ';
static constexpr char gsk_wall = 'w';

static constexpr int gsk_widthGame	= 163;
static constexpr int gsk_heightGame = 21;

static char gs_game[gsk_heightGame][gsk_widthGame];
static char gs_gameLine[gsk_heightGame * gsk_widthGame];
static bool gs_isDirty = true;
static double gs_timer = 0.0f;
static int gs_counter = 1;
static constexpr int gsk_sizeInput = 30;
static char* gsp_inputLine = new char[gsk_sizeInput];
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
	const int pos_x = gs_player.x + gs_player.dir.x;
	const int pos_y = gs_player.y + gs_player.dir.y;

	bool changed = false;
	if (pos_x > gsk_widthGame || pos_x < 0)
	{ 
		gs_player.dir.x = 0;
		changed = true;
	}
	if (pos_y > gsk_heightGame || pos_y < 0)
	{
		gs_player.dir.y = 0;
		changed = true;
	}

	if (!changed)
	{
		if (gs_game[pos_y][pos_x] != gsk_emptySpace)
		{
				gs_player.dir.x = 0;
				gs_player.dir.y = 0;
		}
	}
}

void Controlls()
{
	if (GetAsyncKeyState(VK_UP))
		gs_player.dir.y = -1;
	else if (GetAsyncKeyState(VK_DOWN))
		gs_player.dir.y = 1;
	else
		gs_player.dir.y = 0;

	if (GetAsyncKeyState(VK_RIGHT))
		gs_player.dir.x = 1;
	else if (GetAsyncKeyState(VK_LEFT))
		gs_player.dir.x = -1;
	else
		gs_player.dir.x = 0;
}

void Update(const double deltaTime)
{
	gs_timer += deltaTime;
	if (gs_timer > 256.0 || gs_isDirty)
	{
		//s_box[g_ball.x][g_ball.y] = s_emptySpace; //Clean previous position

		Controlls();
		Collision();


		gs_player.x += gs_player.dir.x;
		gs_player.y += gs_player.dir.y;

		gs_timer = 0.0;
		if(gs_player.dir.x != 0 || gs_player.dir.y != 0)
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

void WallHit()
{
	cout << "You hit a wall...";
}

void HitCommand(const string& command)
{
	int xMod(0), yMod(0);
	if (command == "north")
	{
		yMod = -1;
	}
	else if (command == "south")
		yMod = 1;
	else if (command == "west")
		xMod = -1;
	else if (command == "east")
		xMod = 1;

	const char object = gs_game[gs_player.y  + yMod][gs_player.x + xMod];
	if (object != gsk_emptySpace)
	{
		if (object == gsk_wall)
			WallHit();
	}
}

int main()
{
	Timer timer;
	std::srand(std::time(nullptr));

	gs_player.x = RandomPosX();
	gs_player.y = RandomPosY();
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
			gs_game[gs_player.y][gs_player.x] = gs_player.character;

			PrintBox();
			Sleep(3);
			gs_isDirty = false;
		}
		timer.stop();
		time = timer.elapsedMilliseconds();

		if (GetAsyncKeyState(0xC0)) // 0xC0 '`~'
		{
			std::string input[2];
			cout << "Input: ";
			cin.getline(gsp_inputLine, gsk_sizeInput);

			int counter = 0;

			while (counter != gsk_sizeInput)
			{
				if (gsp_inputLine[counter] == ' ' || gsp_inputLine[counter] == '\0')
					break;

				input[0] += gsp_inputLine[counter];
				++counter;
			}

			while(gsp_inputLine[counter] != '\0')
			{
				++counter;
				input[1] += gsp_inputLine[counter];
			}

			ranges::for_each(input[0], [](char& c) {c = tolower(c); });
			ranges::for_each(input[1], [](char& c) {c = tolower(c); });

			if (input[0] == "quit")
				break;

			else if (input[0] == "hit")
				HitCommand(input[1]);

			system("pause");

			gs_isDirty = true;
		}
	}

	return 0;
}