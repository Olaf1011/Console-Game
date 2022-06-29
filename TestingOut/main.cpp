#include <iostream>
#include <ctime>
#include <windows.h>
#include <algorithm>

#include "Timer.hpp"

#define TO_RAD(x) x * 3.14159 / 180

struct Character{
	Character(char c) : character(c), x(1), y(1), isAlive(true) {}
	//Character& operator=(Character&& rhs)
	//{
	//	this->isAlive = rhs.isAlive;
	//	this->character = rhs.character;
	//	this->dir.x = rhs.dir.x;
	//	this->dir.y = rhs.dir.y;
	//	this->x = rhs.x;
	//	this->y = rhs.y;
	//}
	char character;
	int x, y;
	bool isAlive;
	struct {
		int x = 0, y = 0;
	}dir;
};

static std::vector<Character> gsv_bullet;
static std::vector<Character> gsv_enemy;
static Character gs_player{ 'p' };
static constexpr char gsk_emptySpace = ' ';
static constexpr char gsk_wall = 'w';

static constexpr int gsk_widthGame	= 163;
static constexpr int gsk_heightGame = 21;
static constexpr int gsk_enemyAmount = 30;

static char gs_game[gsk_heightGame][gsk_widthGame];
static char gs_gameLine[gsk_heightGame * gsk_widthGame];
static bool gs_isDirty = true;
static double gs_timer = 0.0;

static double gs_timerMax = 256.0;

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


void HitCommand(const string& command)
{
	int xMod(0), yMod(0);
	if (command == ("north"))
		yMod = -1;
	else if (command == ("south"))
		yMod = 1;
	else if (command == ("west"))
		xMod = -1;
	else if (command == ("east"))
		xMod = 1;

	const char object = gs_game[gs_player.y + yMod][gs_player.x + xMod];
	if (object != gsk_emptySpace)
	{
		if (object == gsk_wall)
			cout << "You hit a wall...\n";
	}
}

void SpeedUp(const string& command)
{
	if (command == "nobrake")
		gs_timerMax = 1.0;
	else if (command == ("super"))
		gs_timerMax = 64.0;
	else if (command == ("fast"))
		gs_timerMax = 128.0;
	else if (command == ("normal"))
		gs_timerMax = 256.0;
	else if (command == ("slow"))
		gs_timerMax = 512.0;
	cout << gs_timerMax << endl;
}

void Shoot(const string& command)
{
	gsv_bullet.emplace_back('b');

	gsv_bullet.back().x = gs_player.x;
	gsv_bullet.back().y = gs_player.y;

	if (command == ("north"))
		gsv_bullet.back().dir.y = -1;
	else if (command == ("south"))
		gsv_bullet.back().dir.y = 1;
	else if (command == ("west"))
		gsv_bullet.back().dir.x = -1;
	else if (command == ("east"))
		gsv_bullet.back().dir.x = 1;

}

bool Update(const double deltaTime)
{
	gs_timer += deltaTime;
	if (gs_timer > gs_timerMax || gs_isDirty)
	{
		if (GetAsyncKeyState(0xC0)) // 0xC0 '`~'
		{
			std::string input[2];
			cout << "Input: ";
			cin.getline(gsp_inputLine, gsk_sizeInput);

			int counter = 0;
			int index = 0;
			while (counter != gsk_sizeInput && gsp_inputLine[counter] != '\0')
			{
				if (gsp_inputLine[counter] == ' ')
				{
					++index;
				}
				else
					input[index] += gsp_inputLine[counter];
				++counter;
			}


			ranges::for_each(input[0], [](char& c) {c = tolower(c); });
			ranges::for_each(input[1], [](char& c) {c = tolower(c); });

			if (input[0] == "quit")
				return true;
			else if (input[0] == "hit")
				HitCommand(input[1]);
			else if (input[0] == "speed")
				SpeedUp(input[1]);
			else if (input[0] == "clear")
				CreateField();
			else
			{
				if (input[0] == "shoot")
					Shoot(input[1]);
				else
					cout << "Invalid command\n";
				gs_isDirty = true;
			}

			system("pause");

		}

		gs_game[gs_player.y][gs_player.x] = gsk_emptySpace; //Clean previous position

		Controlls();
		Collision();

		gs_player.x += gs_player.dir.x;
		gs_player.y += gs_player.dir.y;

		gs_timer = 0.0;
		if(gs_player.dir.x != 0 || gs_player.dir.y != 0)
			gs_isDirty = true;

		if(gs_isDirty)
		{
			for(size_t i = 0; i < gsv_bullet.size(); ++i)
			{

				gs_game[gsv_bullet[i].y][gsv_bullet[i].x] = gs_game[gsv_bullet[i].y][gsv_bullet[i].x] == 'b' ? gsk_emptySpace : gs_game[gsv_bullet[i].y][gsv_bullet[i].x];

				gsv_bullet[i].x += gsv_bullet[i].dir.x;
				gsv_bullet[i].y += gsv_bullet[i].dir.y;

				if (gs_game[gsv_bullet[i].y][gsv_bullet[i].x] != gsk_emptySpace)
					gsv_bullet[i].isAlive = false;
			}
			for (int i = static_cast<int>(gsv_bullet.size()) - 1; i >= 0; --i)
			{
				if (!gsv_bullet[i].isAlive)
					gsv_bullet.erase(gsv_bullet.begin() + i);
			}
		}
	}
	return false;
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

	for (int i = 20; i != 0; --i)
	{
		gsv_enemy.emplace_back('e');
		gsv_enemy.back().x = RandomPosX();
		gsv_enemy.back().y = RandomPosY();

		gs_game[gsv_enemy.back().y][gsv_enemy.back().x] = gsv_enemy[0].character;
	}

	timer.start();
	double time = 0.0;

	while(!GetAsyncKeyState(VK_ESCAPE))
	{
		timer.start();
		if (Update(time))
			break;

		Sleep(1);
		if (gs_isDirty)
		{
			Clear();
			gs_game[gs_player.y][gs_player.x] = gs_player.character;

			for (size_t i = 0; i < gsv_bullet.size(); ++i)
			{
				gs_game[gsv_bullet[i].y][gsv_bullet[i].x] = gsv_bullet[i].character;
			}

			PrintBox();
			gs_isDirty = false;
		}

		timer.stop();
		time = timer.elapsedMilliseconds();
	}

	delete[] gsp_inputLine;

	return 0;
}