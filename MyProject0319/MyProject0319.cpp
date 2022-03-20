#include <random>
#include <string>
#include <bangtal.h>


enum PUZZLE_WAY {NOT=-1,LEFT=0,RIGHT,UP,DOWN}; //���� �̵�����

typedef struct {  //��ǥ ����ü
	int x;
	int y;
} Point;


SceneID scene;
ObjectID whiteBoard; //��� ���� ���
ObjectID piece[16];  // ���� ���� ������Ʈ
ObjectID puzzle[4][4]; // ���� ������ ���� ���� ������Ʈ
ObjectID blank; // �� ���� ����
ObjectID buttonStart, buttonEnd; // ����, ���� ��ư

SoundID sound; // ���� �̵��� ����

TimerID timer;
bool gameState = true; // true:������, false:��
int score = 0; // ���� ���� �̵� Ƚ��


void mouseCallback(ObjectID id, int x, int y, MouseAction act);
void timerCallback(TimerID id);

void clickPuzzle(ObjectID id, int x, int y, MouseAction act);	//���� ���� Ŭ�� ó��
void startPuzzle();	//���� ����
void endPuzzle();	//���� ���� ���� �Ǵ�
void makeObject();	//������Ʈ ����
void mixPuzzle();	//���� ���� ����
Point movePuzzle(Point target, int way, bool shown=false);	//���� �̵� ó��
int getMoveWay(Point point);	//�̵� ������ ���� ����
bool isBlank(int x, int y);	//�� ���� �Ǵ�
void showPuzzle(bool shown);	//���� ��ü ���̱�
int getRandom(int min, int max);	//���� ó��
int getX(int x);	//���� ���� x��ǥ
int getY(int y);	//���� ���� y��ǥ


int main()
{
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	//setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	setMouseCallback(mouseCallback);
	setTimerCallback(timerCallback);
	
	makeObject();
	startPuzzle();
}


void startPuzzle()
{	
	//��ư �����
	hideObject(buttonEnd);
	hideObject(buttonStart);

	//���� ���̱�
	showObject(whiteBoard);
	showPuzzle(true);

	//���� �ʱ�ȭ
	gameState = true;
	score = 0;
	mixPuzzle();
	setTimer(timer, 1800);
	startTimer(timer);
	showTimer(timer);

	startGame(scene);
}



void endPuzzle() //���� ���� �Ǵ�
{
	bool success = true;
	int count = 0;

	// puzzle�� piece�� ���� ������ ������ �Ϸ�
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (puzzle[i][j] != piece[count])
			{
				success = false;
				break;
			}
			count++;
		}
		if (success == false) break;
	}
	if (success == false) return; //�̿Ϸ�� return

	// ���� �Ϸ� ó��
	stopTimer(timer);
	hideTimer();
	gameState = false;
	showObject(buttonStart);
	showObject(buttonEnd);
	hideObject(whiteBoard);
	showPuzzle(false);

	// �Ϸ� �޼���(�ð�,������ Ƚ��)
	std::string message = "�Ϸ�!! (" + std::to_string(1800 - (int)getTimer(timer)) + "��, " + std::to_string(score) + "��)";
	showMessage(message.c_str());
}



void mouseCallback(ObjectID id, int x, int y, MouseAction act) // ���콺 �ݹ��Լ�
{
	clickPuzzle(id, x,y,act); // �������� Ŭ�� ó��
	
	if (id == buttonStart)
		startPuzzle();
	else if (id == buttonEnd)
		endGame();
}


void timerCallback(TimerID id) // Ÿ�̸� �ݹ�
{
	if (id == timer) {
		if (getTimer(timer) < 1) // �ð� �ʰ��� ����
		{
			stopTimer(timer);
			hideTimer();
			showObject(buttonStart);
			showObject(buttonEnd);
			showMessage("����, �ٽ� �����Ͻðڽ��ϱ�?");
		}
	}
}




void makeObject()  // ������Ʈ ����
{
	scene = createScene("scene", "Images/background1.png");
	whiteBoard = createObject("Images/white.png");
	locateObject(whiteBoard, scene, 333, 89);

	// �������� ������Ʈ 4*4 ����
	piece[0] = createObject("Images/1.png");
	piece[1] = createObject("Images/2.png");
	piece[2] = createObject("Images/3.png");
	piece[3] = createObject("Images/4.png");
	piece[4] = createObject("Images/5.png");
	piece[5] = createObject("Images/6.png");
	piece[6] = createObject("Images/7.png");
	piece[7] = createObject("Images/8.png");
	piece[8] = createObject("Images/9.png");
	piece[9] = createObject("Images/10.png");
	piece[10] = createObject("Images/11.png");
	piece[11] = createObject("Images/12.png");
	piece[12] = createObject("Images/13.png");
	piece[13] = createObject("Images/14.png");
	piece[14] = createObject("Images/15.png");
	piece[15] = createObject("Images/16.png");

	// �������� ������Ʈ�� 4*4 �迭�� �ֱ�
	int count = 0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			puzzle[i][j] = piece[count];
			count++;
		}
	}
	
	timer = createTimer(1800);
	buttonStart = createObject("Images/restart.png");
	locateObject(buttonStart, scene, 590, 70);
	buttonEnd = createObject("Images/end.png");
	locateObject(buttonEnd, scene, 590, 20);
	sound = createSound("Sounds/Deadbolt Lock.mp3");
}


void mixPuzzle() // ���� ����
{
	Point target = {getRandom(0, 3),getRandom(0, 3)}; // �� ���� ���� ��ġ ����
	int count = 0;


	// �ϼ��� ���¿��� �������� �������� ������ ���´�.
	while (count < 100)	//���� Ƚ��
	{
		Point move = movePuzzle(target, getRandom(0, 3));
		if (target.x != move.x || target.y != move.y)
		{
			target.x = move.x;
			target.y = move.y;
			count++;
		}
	}
	blank = puzzle[target.x][target.y]; // ������ ����, �����̵����ɿ��� �Ǵܽ� ����

	// ���� ���� ȭ�鿡 ���̱�
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			locateObject(puzzle[i][j], scene, getX(j), getY(i));
		}
	}


	showPuzzle(true); // ���� ���� ���̱�
	hideObject(blank); // �������� ����
}





void clickPuzzle(ObjectID id, int x, int y, MouseAction act) // �������� Ŭ�� ó��
{
	if (gameState == false) return; // ���ӻ��°� �������� �ƴϸ� ����
	if (act != MouseAction::MOUSE_CLICK) return; // ���콺 Ŭ���� �ƴϸ� ����

	Point target = { -1,-1 }; // Ŭ���� ���� ���� ��ġ ����

	// Ŭ���� ���� ���� ��ġ ã��
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (id == puzzle[i][j])
			{
				target.x = i;
				target.y = j;
				break;
			}
		}

		if (target.x != -1) break;
	}

	if (target.x == -1) return; //���� ������ �ƴϸ� return

	int way = getMoveWay(target);	//�̵������� ���� ��������
	if (way != PUZZLE_WAY::NOT) {	 //�̵��� ������ �����̸�
		score++;
		movePuzzle(target, way, true);
		endPuzzle(); //���� �Ϸ� ���� �Ǵ�
	}
}




int getMoveWay(Point point) // �̵� ������ ���� Ȯ��
{
	if (isBlank(point.x - 1, point.y)) { //���� üũ
		return PUZZLE_WAY::UP;
	}

	if (isBlank(point.x, point.y + 1)) { //������ üũ
		return PUZZLE_WAY::RIGHT;
	}

	if (isBlank(point.x + 1, point.y)) { //�Ʒ��� üũ
		return PUZZLE_WAY::DOWN;
	}

	if (isBlank(point.x, point.y - 1)) { //���� üũ
		return PUZZLE_WAY::LEFT;
	}

	return PUZZLE_WAY::NOT;
}


bool isBlank(int x, int y) //�� ���� ���� Ȯ��
{
	if (x < 0 || x > 3 || y < 0 || y > 3)
		return  false;

	if (puzzle[x][y] == blank) return true;

	return false;
}


Point movePuzzle(Point target, int way, bool shown)  //���� �̵�
{
	Point move = { -1,-1 };

	if (way == PUZZLE_WAY::LEFT) { //left 
		move.x = target.x;
		move.y = target.y - 1;
	}
	else if (way == PUZZLE_WAY::RIGHT) { //right
		move.x = target.x;
		move.y = target.y + 1;
	}
	else if (way == PUZZLE_WAY::UP) { //top
		move.x = target.x - 1;
		move.y = target.y;
	}
	else if (way == PUZZLE_WAY::DOWN) { //bottom
		move.x = target.x + 1;
		move.y = target.y;
	}

	if (move.x < 0 || move.x > 3 || move.y < 0 || move.y > 3) return target;

	int old = puzzle[move.x][move.y];
	puzzle[move.x][move.y] = puzzle[target.x][target.y];
	puzzle[target.x][target.y] = old;

	if (shown == true)
	{
		locateObject(puzzle[target.x][target.y], scene, getX(target.y), getY(target.x));
		locateObject(puzzle[move.x][move.y], scene, getX(move.y), getY(move.x));
		playSound(sound);
	}

	return move;
}


void showPuzzle(bool shown)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (shown == true)
				showObject(puzzle[i][j]);
			else
				hideObject(puzzle[i][j]);
		}
	}

}



int getX(int x) // ���� x��ǥ ���
{
	int left = 333 + 135*x;

	return left;
}


int getY(int y) // ���� y��ǥ ���
{
	int top = 494 -135*y;

	return top;
}

int getRandom(int min, int max)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int>dist(min, max);

	return dist(gen);
}

