
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include <queue>
#include <set>
#include "Cell.h"
#include <iostream>

using namespace std;

class Compare
{
	public:
		bool operator()(Cell* c1, Cell* c2)
		{
			// return "true" if "c1".f is smaller than "c2".f
			return c1->getF() > c2->getF();
		}
};

const int MSZ = 50;

const int SPACE = 0;
const int WALL = 1;
const int START = 2;
const int TARGET = 3;
const int GRAY = 4;
const int BLACK = 5;
const int PATH = 6;

int maze[MSZ][MSZ] = {0}; // space
bool runBestFirst = false;
bool runAStar = false;

// priority for the Best First Search
priority_queue<Cell*, vector<Cell*>, Compare> pq;

// open list for the A* algorithm
priority_queue<Cell*, vector<Cell*>, Compare> openList;
// closed list for the A* algorithm  with the value of the f function
int closedList[MSZ][MSZ] = {-1};


int targetR, targetC;

void InitMaze();

void init()
{
	glClearColor(0.5,0.5,0.5,0);// color of window background
	glOrtho(0, MSZ, 0, MSZ, -1, 1); // set the coordinates system

	srand((unsigned int)time(NULL));


	InitMaze();
}

int heuristic(int r, int c)
{
	// Manhattan distance to the target cell
	return abs(r - targetR) + abs(c - targetC);
}

void InitMaze()
{
	int i, j;

	for (i = 0;i < MSZ;i++)
	{
		maze[0][i] = WALL;
		maze[MSZ-1][i] = WALL;
		maze[i][0] = WALL;
		maze[i][MSZ - 1] = WALL;
	}

	for(i=1;i<MSZ-1;i++)
		for (j = 1;j < MSZ - 1;j++)
		{
			if (i % 2 == 1) // mostly spaces
			{
				if (rand() % 10 < 1) // WALL
					maze[i][j] = WALL;
				else maze[i][j] = SPACE;
			}
			else // even lines
			{
				if (rand() % 10 < 4) // space
					maze[i][j] = SPACE;
				else
					maze[i][j] = WALL;
			}
		}

	maze[1][1] = START;
	targetR = rand() % MSZ;
	targetC = rand() % MSZ;
	maze[targetR][targetC] = TARGET;

	// add the start cell to the priority queue with heuristic value
	pq.push(new Cell(1, 1, heuristic(1, 1),0, nullptr));

	// add the start cell to the open list
	openList.push(new Cell(1, 1, heuristic(1, 1), 0, nullptr));
	
}

void DrawMaze()
{
	int i, j;

	for(i=0;i<MSZ;i++)
		for (j = 0;j < MSZ;j++)
		{
			switch (maze[i][j]) // set convinient color of a cell
			{
			case SPACE:
				glColor3d(1, 1, 1); // white
				break;
			case WALL:
				glColor3d(0,0, 0); // black
				break;
			case START:
				glColor3d(0.5, 0.5, 1); // blue
				break;
			case TARGET:
				glColor3d(1, 0, 0); // red
				break;
			case GRAY:
				glColor3d(1, 1, 0); // yellow
				break;
			case BLACK:
				glColor3d(0.8, 1, 0.8); // green
				break;
			case PATH:
				glColor3d(0.5, 0, 0.5); // green
				break;
			}// switch

			glBegin(GL_POLYGON);
			glVertex2d(j, i);
			glVertex2d(j, i + 1);
			glVertex2d(j+1, i + 1);
			glVertex2d(j+1 , i );
			glEnd();
		}
}

bool isValid(int r, int c)
{
	return r >= 0 && r < MSZ && c >= 0 && c < MSZ && maze[r][c] != WALL;
}

void restorePath(Cell* cell)
{
	while (cell->getParent() != nullptr)
	{
		int r = cell->getRow();
		int c = cell->getCol();
		maze[r][c] = PATH;
		cell = cell->getParent();
	}
}

bool isTarget(int r, int c, Cell * cell)
{
	if (r < 0 || r >= MSZ || c < 0 || c >= MSZ)
		return false;

	if (maze[r][c] == TARGET) {
		cout << "Path found" << endl;
		runBestFirst = false;
		runAStar = false;
		restorePath(new Cell(r, c, 0,0, cell));
		return true;
	}

	return false;
}

void RunAStar()
{
	if (pq.empty())
	{
		cout << "No path found" << endl;
		runBestFirst = false;
		return;
	}

	Cell* cell = pq.top(); // get the cell with the smallest h value 
	pq.pop(); // remove the cell from the priority queue

	// get the cell's row and column
	int r = cell->getRow();
	int c = cell->getCol();

	// if the cell is the target cell
	// mark the cell as black
	maze[r][c] = BLACK;


	Cell* neighbors[4] = {
		new Cell(r - 1, c, heuristic(r - 1, c), cell->getG() + 1, cell),
		new Cell(r + 1, c, heuristic(r + 1, c), cell->getG() + 1, cell),
		new Cell(r, c - 1, heuristic(r, c - 1), cell->getG() + 1, cell),
		new Cell(r, c + 1, heuristic(r, c + 1), cell->getG() + 1, cell)
	};


	// for each neighbor
	for (int i = 0; i < 4; i++) {
		if ((isValid(neighbors[i]->getRow(), neighbors[i]->getCol()) && maze[neighbors[i]->getRow()][neighbors[i]->getCol()] != BLACK))
		{
			// check if the neighbor is the target
			if (isTarget(neighbors[i]->getRow(), neighbors[i]->getCol(), cell))
				return;


			// if the neighbor is not in the closed list or the f value is larger than the the f value in the closed list
			if (closedList[neighbors[i]->getRow()][neighbors[i]->getCol()] == -1 && closedList[neighbors[i]->getRow()][neighbors[i]->getCol()] < neighbors[i]->getF())
			{
				continue;
			}

			bool inOpenList = false;

			// if the neighbor is not in the open list or the f value is larger than the the f value in the open list
			for (size_t j = 0; j < openList.size(); j++)
			{
				Cell* temp = openList.top();
				openList.pop();
				if (temp->getRow() == neighbors[i]->getRow() && temp->getCol() == neighbors[i]->getCol() && temp->getF() < neighbors[i]->getF())
				{
					inOpenList = true;
				}
				openList.push(temp);
			}

			if (inOpenList)
				continue;



			// add the neighbor to the priority queue with the heuristic value
			pq.push(neighbors[i]);
			// mark the neighbor as gray
			maze[neighbors[i]->getRow()][neighbors[i]->getCol()] = GRAY;
		}
	}
}

void RunBestFirst()
{
	if (pq.empty())
	{
		cout << "No path found" << endl;
		runBestFirst = false;
		return;
	}

	Cell* cell = pq.top(); // get the cell with the smallest h value 
	pq.pop(); // remove the cell from the priority queue

	// get the cell's row and column
	int r = cell->getRow();
	int c = cell->getCol();

	// if the cell is the target cell
	// mark the cell as black
	maze[r][c] = BLACK;


	Cell* neighbors[4] = {
		new Cell(r - 1, c, heuristic(r - 1, c), 0, cell),
		new Cell(r + 1, c, heuristic(r + 1, c), 0, cell),
		new Cell(r, c - 1, heuristic(r, c - 1), 0, cell),
		new Cell(r, c + 1, heuristic(r, c + 1), 0, cell)
	};


	// for each neighbor
	for (int i = 0; i < 4; i++) {
		if ((isValid(neighbors[i]->getRow(), neighbors[i]->getCol()) && maze[neighbors[i]->getRow()][neighbors[i]->getCol()] != BLACK))
		{
			// check if the neighbor is the target
			if (isTarget(neighbors[i]->getRow(), neighbors[i]->getCol(), cell))
				return;
			// add the neighbor to the priority queue with the heuristic value
			pq.push(neighbors[i]);
			// mark the neighbor as gray
			maze[neighbors[i]->getRow()][neighbors[i]->getCol()] = GRAY;
		}
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer with background color

	DrawMaze();

	glutSwapBuffers(); // show all
}

void idle() 
{

	if(runBestFirst)
		RunBestFirst();
	if(runAStar)
		RunAStar();

	glutPostRedisplay(); // call to display indirectly
}

void menu(int choice)
{
	if (choice == 1) {
		runBestFirst = true;
	}
	else if (choice == 2)
	{
		runAStar = true;
	}
}

int main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("BFS Example");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle);  // updates function
	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("run Best First", 1);
	glutAddMenuEntry("run A*", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();

	return 0;
}