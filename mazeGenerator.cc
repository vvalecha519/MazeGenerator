#include <vector>
#include <utility>
#include <stdlib.h>
#include <stack>
#include <iostream>
#include <chrono>
#include <cairo.h>
#include <gtk/gtk.h>
#include <math.h>

using namespace std;

const int UP = 0;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 3;
int ONCE = 1;

struct Cell
{
	Cell(int x, int y) : pos{make_pair(x, y)} {}
	//position in the maze
	pair<int, int> pos;
	bool visited = false;
	//whether the cell has wall surround corresponding side
	bool left = true;
	bool right = true;
	bool up = true;
	bool down = true;
};

vector<vector<Cell>> intitalMaze(int size)
{
	vector<vector<Cell>> maze;
	for (int y = 0; y < size; y++)
	{
		vector<Cell> row;
		for (int x = 0; x < size; x++)
		{
			//create fully closed maze
			row.push_back(Cell{x, y});
		}
		maze.push_back(row);
	}
	return maze;
}

//returns number between 0 to 3
// 0: left, 1: up, 2: right, 3: down
int randomGenerator()
{

	return rand() % 4;
}

bool validCell(vector<vector<Cell>> &maze, Cell cell)
{
	int x = cell.pos.first;
	int y = cell.pos.second;
	int size = maze.size();
	bool isValid = (((y + 1 < size) && !(maze.at(y + 1).at(x).visited)) ||
					((y - 1 >= 0) && !(maze.at(y - 1).at(x).visited)) ||
					((x + 1 < size) && !(maze.at(y).at(x + 1).visited)) ||
					((x - 1 >= 0) && !(maze.at(y).at(x - 1).visited)));
	return isValid;
}

void finalMaze(vector<vector<Cell>> &maze, pair<int, int> &startPos, pair<int, int> &endPos)
{
	stack<Cell *> visitedCells;
	Cell *selectedCell = &(maze.at(startPos.second).at(startPos.first));
	Cell endCell = maze.at(endPos.second).at(endPos.first);
	int size = maze.size();
	int counter = 0;
	while (selectedCell->pos != endPos)
	{
		//check if valid move exists
		selectedCell->visited = true;
		if (validCell(maze, *selectedCell))
		{
			counter++;
			visitedCells.push(selectedCell);
			bool valid = false;
			while (!valid)
			{
				int direction = randomGenerator();
				if (direction == UP)
				{
					if ((selectedCell->pos.second + 1 != size) && !(maze.at(selectedCell->pos.second + 1).at(selectedCell->pos.first).visited))
					{
						//cout<<"up";
						//remove wall
						selectedCell->up = false;
						selectedCell = &(maze.at(selectedCell->pos.second + 1).at(selectedCell->pos.first));
						//remove wall
						selectedCell->down = false;
						valid = true;
					}
				}
				else if (direction == DOWN)
				{
					if ((selectedCell->pos.second - 1 != -1) && !(maze.at(selectedCell->pos.second - 1).at(selectedCell->pos.first).visited))
					{
						//cout<<"down";
						selectedCell->down = false;
						selectedCell = &(maze.at(selectedCell->pos.second - 1).at(selectedCell->pos.first));
						selectedCell->up = false;
						valid = true;
					}
				}
				else if (direction == LEFT)
				{
					if (selectedCell->pos.first - 1 != -1 && !(maze.at(selectedCell->pos.second).at(selectedCell->pos.first - 1).visited))
					{
						//cout<<"left";
						selectedCell->left = false;
						selectedCell = &(maze.at(selectedCell->pos.second).at(selectedCell->pos.first - 1));
						selectedCell->right = false;
						valid = true;
					}
				}
				else if (direction == RIGHT)
				{
					if (selectedCell->pos.first + 1 != size && !(maze.at(selectedCell->pos.second).at(selectedCell->pos.first + 1).visited))
					{
						//cout<<"right";
						selectedCell->right = false;
						selectedCell = &(maze.at(selectedCell->pos.second).at(selectedCell->pos.first + 1));
						selectedCell->left = false;
						valid = true;
					}
				}
			}
		}
		else
		{ //
			cout << "here";
			selectedCell = visitedCells.top();
			visitedCells.pop();
		}
		selectedCell->visited = true;
	}
}

void printMaze(vector<vector<Cell>> &maze)
{
	for (int i = maze.size() - 1; i >= 0; i--)
	{
		vector<Cell> row = maze.at(i);
		//print up walls
		for (int k = 0; k < maze.size(); k++)
		{
			Cell cell = row.at(k);
			if (cell.up && !(cell.down && cell.up && cell.left && cell.right))
			{
				cout << " _ ";
			}
			else
				cout << "   ";
		}
		cout << endl;
		for (int k = 0; k < maze.size(); k++)
		{
			Cell cell = row.at(k);
			if ((cell.down && cell.up && cell.left && cell.right))
			{
				cout << "   ";
			}
			else
			{
				if (cell.left)
				{
					cout << "|";
				}
				else
					cout << " ";
				if (cell.visited)
				{
					cout << "-";
				}
				else
				{
					cout << " ";
				}
				if (cell.right)
				{
					cout << "|";
				}
				else
					cout << " ";
			}
		}
		cout << endl;
		for (int k = 0; k < maze.size(); k++)
		{
			Cell cell = row.at(k);
			if (cell.down && !(cell.down && cell.up && cell.left && cell.right))
			{
				cout << " _ ";
			}
			else
				cout << "   ";
		}
		cout << endl;
	}
}

static void do_drawing(cairo_t *cr, GtkWidget *widget, vector<vector<Cell>> maze)
{
	GtkWidget *win = gtk_widget_get_toplevel(widget);

	int width, height;
	gtk_window_get_size(GTK_WINDOW(win), &width, &height);

	cairo_set_line_width(cr, 2);
	cairo_set_source_rgb(cr, 0.69, 0.19, 0);

		int Y = 775; //add on to this 0 is top 800 is bottom
		int moveBy = (Y - 25) / maze.size();

	//review from here
	//start drawing
	for (int i = 0; i < maze.size(); i++)
	{
		int X = 25;	 //add on to this 0 is top 800 is bottom


		vector<Cell> row = maze.at(i);
		//print up walls
		for (int k = 0; k < maze.size(); k++)
		{
			Cell cell = row.at(k);
			if (cell.up && !(cell.down && cell.up && cell.left && cell.right))
			{
				cairo_move_to(cr, X, Y - moveBy);
				cairo_line_to(cr, X + moveBy, Y - moveBy);
				cairo_stroke_preserve(cr);
			}
			X = X + moveBy;
		}
		X = 25;
		////////////////////////
		for (int k = 0; k < maze.size(); k++)
		{
			Cell cell = row.at(k);
			if (!(cell.down && cell.up && cell.left && cell.right))
			{
				if (cell.left)
				{
					cairo_move_to(cr, X, Y - moveBy);
					cairo_line_to(cr, X, Y);
					cairo_stroke_preserve(cr);
				}

				if (cell.right)
				{
				cairo_move_to(cr, X + moveBy, Y - moveBy);
				cairo_line_to(cr, X + moveBy, Y);
				cairo_stroke_preserve(cr);
				}
			}
			X = X + moveBy;
		}

		X = 25;

		for (int k = 0; k < maze.size(); k++)
		{
			Cell cell = row.at(k);
			if (cell.down && !(cell.down && cell.up && cell.left && cell.right))
			{
				cairo_move_to(cr, X, Y);
				cairo_line_to(cr, X + moveBy, Y);
				cairo_stroke(cr);
			}
			X = X + moveBy;
		}
		X = 25;
		Y = Y - moveBy;
	}
	////////////////////////////
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr,
							  gpointer user_data)
{
	int size = 15;
	pair<int, int> startPos(0, 0);
	pair<int, int> endPos(size - 1, size - 1);
	vector<vector<Cell>> maze = intitalMaze(size);
	finalMaze(maze, startPos, endPos);
	cout << endl;
	printMaze(maze);
	do_drawing(cr, widget, maze);
	return FALSE;
}

int main(int argc, char *argv[])
{
	//create window

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	srand(seed);
	GtkWidget *window;
	GtkWidget *darea;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	darea = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), darea);

	g_signal_connect(G_OBJECT(darea), "draw",
					 G_CALLBACK(on_draw_event), NULL);
	g_signal_connect(G_OBJECT(window), "destroy",
					 G_CALLBACK(gtk_main_quit), NULL);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 800);
	gtk_window_set_title(GTK_WINDOW(window), "Fill & stroke");

	gtk_widget_show_all(window);

	gtk_main();
}