#include <random>
#include <string>
#include <bangtal.h>


enum PUZZLE_WAY {NOT=-1,LEFT=0,RIGHT,UP,DOWN}; //퍼즐 이동방향

typedef struct {  //좌표 구조체
	int x;
	int y;
} Point;


SceneID scene;
ObjectID whiteBoard; //흰색 퍼즐 배경
ObjectID piece[16];  // 퍼즐 조각 오브젝트
ObjectID puzzle[4][4]; // 섞인 상태의 퍼즐 조각 오브젝트
ObjectID blank; // 빈 퍼즐 조각
ObjectID buttonStart, buttonEnd; // 시작, 종료 버튼

SoundID sound; // 퍼즐 이동시 사운드

TimerID timer;
bool gameState = true; // true:실행중, false:끝
int score = 0; // 퍼즐 조각 이동 횟수


void mouseCallback(ObjectID id, int x, int y, MouseAction act);
void timerCallback(TimerID id);

void clickPuzzle(ObjectID id, int x, int y, MouseAction act);	//퍼즐 조각 클릭 처리
void startPuzzle();	//게임 시작
void endPuzzle();	//게임 종료 여부 판단
void makeObject();	//오브젝트 생성
void mixPuzzle();	//퍼즐 조각 섞기
Point movePuzzle(Point target, int way, bool shown=false);	//퍼즐 이동 처리
int getMoveWay(Point point);	//이동 가능한 퍼즐 방향
bool isBlank(int x, int y);	//빈 조각 판단
void showPuzzle(bool shown);	//퍼즐 전체 보이기
int getRandom(int min, int max);	//랜덤 처리
int getX(int x);	//퍼즐 조각 x좌표
int getY(int y);	//퍼즐 조각 y좌표


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
	//버튼 숨기기
	hideObject(buttonEnd);
	hideObject(buttonStart);

	//퍼즐 보이기
	showObject(whiteBoard);
	showPuzzle(true);

	//게임 초기화
	gameState = true;
	score = 0;
	mixPuzzle();
	setTimer(timer, 1800);
	startTimer(timer);
	showTimer(timer);

	startGame(scene);
}



void endPuzzle() //퍼즐 종료 판단
{
	bool success = true;
	int count = 0;

	// puzzle이 piece와 같은 순서로 있으면 완료
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
	if (success == false) return; //미완료면 return

	// 게임 완료 처리
	stopTimer(timer);
	hideTimer();
	gameState = false;
	showObject(buttonStart);
	showObject(buttonEnd);
	hideObject(whiteBoard);
	showPuzzle(false);

	// 완료 메세지(시간,움직인 횟수)
	std::string message = "완료!! (" + std::to_string(1800 - (int)getTimer(timer)) + "초, " + std::to_string(score) + "번)";
	showMessage(message.c_str());
}



void mouseCallback(ObjectID id, int x, int y, MouseAction act) // 마우스 콜백함수
{
	clickPuzzle(id, x,y,act); // 퍼즐조각 클릭 처리
	
	if (id == buttonStart)
		startPuzzle();
	else if (id == buttonEnd)
		endGame();
}


void timerCallback(TimerID id) // 타이머 콜백
{
	if (id == timer) {
		if (getTimer(timer) < 1) // 시간 초과시 종료
		{
			stopTimer(timer);
			hideTimer();
			showObject(buttonStart);
			showObject(buttonEnd);
			showMessage("실패, 다시 도전하시겠습니까?");
		}
	}
}




void makeObject()  // 오브젝트 생성
{
	scene = createScene("scene", "Images/background1.png");
	whiteBoard = createObject("Images/white.png");
	locateObject(whiteBoard, scene, 333, 89);

	// 퍼즐조각 오브젝트 4*4 생성
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

	// 퍼즐조각 오브젝트를 4*4 배열에 넣기
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


void mixPuzzle() // 퍼즐 섞기
{
	Point target = {getRandom(0, 3),getRandom(0, 3)}; // 빈 퍼즐 조각 위치 랜덤
	int count = 0;


	// 완성된 상태에서 빈조각을 움직여서 퍼즐을 섞는다.
	while (count < 100)	//섞는 횟수
	{
		Point move = movePuzzle(target, getRandom(0, 3));
		if (target.x != move.x || target.y != move.y)
		{
			target.x = move.x;
			target.y = move.y;
			count++;
		}
	}
	blank = puzzle[target.x][target.y]; // 빈조각 관리, 게임이동가능여부 판단시 사용됨

	// 섞인 퍼즐 화면에 보이기
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			locateObject(puzzle[i][j], scene, getX(j), getY(i));
		}
	}


	showPuzzle(true); // 전제 조각 보이기
	hideObject(blank); // 빈조각은 숨김
}





void clickPuzzle(ObjectID id, int x, int y, MouseAction act) // 퍼즐조각 클릭 처리
{
	if (gameState == false) return; // 게임상태가 진행중이 아니면 리턴
	if (act != MouseAction::MOUSE_CLICK) return; // 마우스 클릭이 아니면 리턴

	Point target = { -1,-1 }; // 클릭된 퍼즐 조각 위치 변수

	// 클릭된 퍼즐 조각 위치 찾기
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

	if (target.x == -1) return; //퍼즐 조각이 아니면 return

	int way = getMoveWay(target);	//이동가능한 방향 가져오기
	if (way != PUZZLE_WAY::NOT) {	 //이동이 가능한 퍼즐이면
		score++;
		movePuzzle(target, way, true);
		endPuzzle(); //퍼즐 완료 여부 판단
	}
}




int getMoveWay(Point point) // 이동 가능한 방향 확인
{
	if (isBlank(point.x - 1, point.y)) { //위쪽 체크
		return PUZZLE_WAY::UP;
	}

	if (isBlank(point.x, point.y + 1)) { //오른쪽 체크
		return PUZZLE_WAY::RIGHT;
	}

	if (isBlank(point.x + 1, point.y)) { //아래쪽 체크
		return PUZZLE_WAY::DOWN;
	}

	if (isBlank(point.x, point.y - 1)) { //왼쪽 체크
		return PUZZLE_WAY::LEFT;
	}

	return PUZZLE_WAY::NOT;
}


bool isBlank(int x, int y) //빈 조각 여부 확인
{
	if (x < 0 || x > 3 || y < 0 || y > 3)
		return  false;

	if (puzzle[x][y] == blank) return true;

	return false;
}


Point movePuzzle(Point target, int way, bool shown)  //퍼즐 이동
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



int getX(int x) // 퍼즐 x좌표 계산
{
	int left = 333 + 135*x;

	return left;
}


int getY(int y) // 퍼즐 y좌표 계산
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

