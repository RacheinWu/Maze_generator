#include <iostream>
#include <windows.h>
#include <queue>
#include <vector>
#include <conio.h>
#include <time.h>
#include <bits/stdc++.h>

using namespace std;

const int ROW = 25 ;//迷宫的列数
const int COLUMN = 25;//迷宫的行数
const int MAZE_R = 2 * ROW + 1;//实际迷宫的行数(数组长度)
const int MAZE_C = 2 * COLUMN + 1;//实际迷宫的列数(数组长度)
const int START_R = 1;//(数组位置)起点位置r
const int START_C = 0;//(数组位置)起点位置c
const int END_R = MAZE_R - 2;//(数组位置)终点位置r
const int END_C = MAZE_C -1 ;//(数组位置)终点位置c
const int DIRECTION_R[] = {1, -1, 0, 0};//4个邻居方向r
const int DIRECTION_C[] = {0, 0, -1, 1};//4个邻居方向c

const int ROAD = 0;//路
const int WALKED = 1;//玩家已走路
const int ROAD_MUST_WALK = 2;//路点(必经点)
const int ROAD_CHECKED = 3;//计算机寻路痕迹
const int ROAD_WAIT = 4;//待选路点
const int ROAD_AI = 5;//计算机行走路线
const int CURR = 6;//玩家当前位置
const int WALL = 9;//墙体
const int SELECT = 7;//被选中状态
const int KEY_DOWN = 1;
const int KEY_UP = 2;
const int KEY_LEFT = 3;
const int KEY_RIGHT = 4;

const int SPEED = 300;
const int K = 0;


int maze[MAZE_R][MAZE_C];//(全局变量)存放迷宫单元格的数组
int game_cur_r = 1,  game_cur_c = 1;



/*迷宫单元格*/
struct Point {
public :
    int r,c;
    Point* f;
    int w = 0;
    Point(int r, int c) {
        this->r = r;
        this->c = c;
    }
};

/*函数声明*/
void init();//初始化数组,为生成迷宫做好准备
void prime();//Prime算法 生成迷宫
void join(int r, int c);//Prime算法下调用, 用于添加临时路点
void connect(int r, int c);//Prime算法下调用，用于打通A与随机B之间的墙壁
void display();//控制台打印输出 显示整个迷宫的布局，其中显示玩家的位置，走过的路径等等
void BFS(int r, int c);//BFS算法进行探索路径
bool judge(int r, int c);//判断是否数组越界，是否属于墙体等等
void help();//当game()函数状态下，进行提示下一步该怎么走，一般提示5步,其中调用了BFS算法，内置优化内存方案，避免重复计算
void refresh();//在game()函数状态下，进行刷新显示，对显示操作进行优化等等
void fa(Point* p);
void gameJudge(int i);//自己控制方向走迷宫，用键盘进行操控，目标是墙体时，不能行走。
void gameController();
void gotoxy(int x, int y);

void refresh();


vector<Point*> tempList;//用于存储待选路点
vector<Point*> RMWList;//用于存储路点



/************/
void init() {
    //将所有单元格设置为墙:
    for (int i = 0; i < MAZE_R; ++i) {
        for (int j = 0; j < MAZE_C; ++j) {
            maze[i][j] = WALL;
        }
    }

    //设置必经点位置:
    for (int i = 0; i < ROW; ++i) {
        for (int j = 0; j < COLUMN; ++j) {
            maze[2 * i + 1][ 2 * j + 1] = ROAD_MUST_WALK;//设置状态
//            maze[2 * i + 1][ 2 * j + 1] = ROAD_MUST_WALK;//设置状态
//            Point* point = new Point(2 * i + 1,  2 * j + 1);//创建指针
//            RMWList.push_back(point);//必经点(路点)存入集合
        }
    }
    //设置起点和终点:
    maze[START_R][START_C] = ROAD;
    maze[END_R][END_C] = ROAD;
//    maze[START_R][START_C] = ROAD_MUST_WALK;
//    maze[END_R][END_C] = ROAD_MUST_WALK;
//    Point* startP = new Point(START_R, START_C);//创建指针
//    Point* endP = new Point(END_R, END_C);//创建指针
//
//    RMWList.push_back(startP);//必经点(路点)存入集合
//    RMWList.push_back(endP);//必经点(路点)存入集合

    //-------------------------




}

void prime() {
    //随机选择一个路点变成路，并且将其四周的路点都加入到待选路点
//    int size = RMWList.size();
//    int randomN =  rand() % size;
//    Point* curP = RMWList[randomN];
//    join(curP->r, curP->c);
    maze[START_R][START_C + 1] = ROAD;
    join(START_R, START_C+1);
    /*循环： (当待选路点集合 不为空时)*/
    cout << tempList.size() <<endl;
    while (tempList.size() > 0) {

        if (K) {
            display();
            Sleep(SPEED);
        }
        int i = rand() % tempList.size();//随机数
        Point* A = tempList.at(i);//随机选择一个A
        maze[A->r][A->c] = SELECT;
        if (K) {
            display();
            Sleep(SPEED);
        }
//        maze[A->r][A->c] = ROAD_WAIT;
        connect(A->r, A->c);//随机地选取一个其四周的一个路B,将这两个点的中间墙打通:
        if (K) {
            display();
            Sleep(SPEED);
        }
        maze[A->r][A->c] = ROAD;//将A变成路
        if (K) {
            display();
            Sleep(SPEED);
        }
        join(A->r, A->c);//将A四周的路点加入待选路点中
        tempList.erase(tempList.begin() + i);//从待选路点中移除A
    }

}

void join(int r, int c) {
    //上路点：
    if (r - 2 >= 0 && maze[r - 2][c] == ROAD_MUST_WALK) {
        Point* p = new Point(r - 2, c);
        tempList.push_back(p);//存入待选列表中
        maze[r-2][c] = ROAD_WAIT; //将此点转换为待选状态
    }
    //下路点：
    if (r + 2 <= MAZE_R - 1 && maze[r + 2][c] == ROAD_MUST_WALK) {
        Point* p = new Point(r + 2, c);
        tempList.push_back(p);//存入待选列表中
        maze[r + 2][c] = ROAD_WAIT; //将此点转换为待选状态
    }
    //左路点
    if (c - 2 >= 0 && maze[r][c - 2] == ROAD_MUST_WALK) {
        Point* p = new Point(r, c - 2);
        tempList.push_back(p);//存入待选列表中
        maze[r][c-2] = ROAD_WAIT; //将此点转换为待选状态
    }
    //右路点
    if (c + 2 <= MAZE_C - 1 && maze[r][c + 2] == ROAD_MUST_WALK) {
        Point* p = new Point(r, c + 2);
        tempList.push_back(p);//存入待选列表中
        maze[r][c+2] = ROAD_WAIT; //将此点转换为待选状态
    }
}

/*随机地选取一个其四周的一个路B,将这两个点的中间墙打通*/
void connect(int r, int c) {
    /*定义一个局部集合，用于存储四周的路*/
    vector<Point*> arr;
    if (r - 2 >= 0 && maze[r - 2][c] == ROAD) {
        Point* p = new Point(r - 2, c);
        arr.push_back(p);
    }
    //下路点：
    if (r + 2 <= MAZE_R - 1 && maze[r + 2][c] == ROAD) {
        Point* p = new Point(r + 2, c);
        arr.push_back(p);
    }
    //左路点
    if (c - 2 >= 0 && maze[r][c - 2] == ROAD) {
        Point* p = new Point(r, c - 2);
        arr.push_back(p);
    }
    //右路点
    if (c + 2 <= MAZE_C - 1 && maze[r][c + 2] == ROAD) {
        Point* p = new Point(r, c + 2);
        arr.push_back(p);
    }
    int i = rand() % arr.size();
    Point* road = arr[i];//随机抽取一个路
    if (road->c == c)
        maze[(r + road->r)/2][c] = ROAD;
    else
        maze[r][(c + road->c)/2] = ROAD;

}


void BFS(int r, int c) {
    queue<Point*> queue;
    Point* curP = new Point(r, c);
    queue.push(curP);
    maze[r][c] = ROAD_CHECKED;
    while (!queue.empty()) {
        Point* top = queue.front();
        queue.pop();//抛出
//        cout << top->f->r << ":" << top->f->c << endl;
        if (top->r == END_R && top->c == END_C-1) {
//            cout << top->f->r << ":" << top->f->c << endl;
//            cout << top->f->f->r << ":" << top->f->f->c << endl;
//            cout << top->f->f->f->r << ":" << top->f->f->f->c << endl;

            fa(top);
            cout << "the shortest road length :" << top->w << endl;
            return;
        }
        for (int i = 0; i < 4; ++i) {
            int nextP_r = top->r + DIRECTION_R[i];
            int nextP_c = top->c + DIRECTION_C[i];
            if (judge(nextP_r, nextP_c)) {
                Point* nextP = new Point(nextP_r, nextP_c);
                queue.push(nextP);  //加入队列:
                nextP->f = top;
                nextP->w = top->w + 1;
                maze[nextP_r][nextP_c] = ROAD_CHECKED;
            }

        }
    }

}

void fa(Point* p) {
    if (p->r == game_cur_r && p->c == game_cur_c) return;
    maze[p->r][p->c] = ROAD_AI;
    fa(p->f);
}

void gameJudge(int i) {
    cout << i <<endl;
    int r,c;
    if (i == KEY_UP) {
        r = game_cur_r - 1;
        c = game_cur_c;
    }
    else if (i == KEY_DOWN) {
        r = game_cur_r + 1;
        c = game_cur_c;
    }
    else if (i == KEY_LEFT) {
        r = game_cur_r ;
        c = game_cur_c - 1;
    }
    else if (i == KEY_RIGHT) {
        r = game_cur_r;
        c = game_cur_c + 1;
    }

    /*交给判决*/
    if (maze[r][c] != WALL) {
//        system("cls");//刷新屏幕;
        maze[game_cur_r][game_cur_c] = WALKED;
        game_cur_c = c;
        game_cur_r = r;
        maze[game_cur_r][game_cur_c] = CURR;
        display();//重新显示

    }
}

void gameController() {
    maze[game_cur_r][game_cur_c] = CURR;
//    system("cls");
    display();
    while (1)
    {
//        //获取某键状态
//        if (GetKeyState(VK_SPACE) < 0)		//如果某键被按下
//        {
//            //	cout << "a=" << a << endl;
//            printf("0.0\n");
//
//        }
//        cout << "??" << endl;
        int down = GetKeyState(VK_DOWN);//获取下箭头键状态   如果down>0 表示被松开  down<0 表示被按下
        int up = GetKeyState(VK_UP);//获取下箭头键状态   如果down>0 表示被松开  down<0 表示被按下
        int left = GetKeyState(VK_LEFT);//获取下箭头键状态   如果down>0 表示被松开  down<0 表示被按下
        int right = GetKeyState(VK_RIGHT);//获取下箭头键状态   如果down>0 表示被松开  down<0 表示被按下
        int space = GetKeyState(VK_SPACE);//获取下箭头键状态   如果down>0 表示被松开  down<0 表示被按下
        if (down < 0)      //如果下箭头键被按下
        {
            gameJudge(KEY_DOWN);
        }
        if (up < 0)      //如果下箭头键被按下
        {
            gameJudge(KEY_UP);
        }
        if (left < 0)      //如果下箭头键被按下
        {
            gameJudge(KEY_LEFT);
        }
        if (right < 0)      //如果下箭头键被按下
        {
            gameJudge(KEY_RIGHT);
        }
        if (space < 0) {
            BFS(game_cur_r, game_cur_c);
            display();
            break;
        }
        Sleep(100);//到时候实践看看要不要，没有sleep的话太快了，正常按下一次会有很多读入键盘
    }
}

void refresh() {
    COORD pos = {0,0};
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOut, pos);
    system("cls");
}


bool judge(int r, int c) {
    if (r >= MAZE_R || c >= MAZE_C || r < 0 || c < 0) return false;
    if (maze[r][c] == WALL || maze[r][c] == ROAD_CHECKED) return false;
    else return true;
}

void display() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);//控制台字体颜色设置
    for (int i = 0; i < MAZE_R; ++i) {
        for (int j = 0; j < MAZE_C; ++j) {
            if (maze[i][j] == WALL) {
                SetConsoleTextAttribute(hConsole, 150);//设置颜色
            }
            else if (maze[i][j] == ROAD || maze[i][j] == ROAD_CHECKED) {
                SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);//不设置颜色
            }
            else if (maze[i][j] == ROAD_AI){
                SetConsoleTextAttribute(hConsole,  77);
            }
            else if (maze[i][j] == CURR || maze[i][j] == ROAD_WAIT) {
                SetConsoleTextAttribute(hConsole, 166);//设置颜色
            }
            else if  (maze[i][j] == WALKED || maze[i][j] == SELECT) {
                SetConsoleTextAttribute(hConsole, 121);//设置颜色
            }
            cout << "  ";
        }
        cout << endl;
    }
    cout << endl;
    cout << endl;
    SetConsoleTextAttribute(hConsole,37);//恢复原色
}

//void gotoxy(int c, int s) {
//    COORD pos = {c,s};
//            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
//    SetConsoleCursorPosition(hOut, pos);
//}

int main() {
    init();
//    display();
//    cout << "-------------------------------" << endl;
    prime();
    display();
//    cout << "-------------------------------" << endl;
//    BFS(START_R, START_C + 1);
//    display();

    gameController();


    return 0;
}