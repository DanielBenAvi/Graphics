#include "Cell.h"

Cell::Cell()
{
	parent = nullptr;
}

Cell::Cell(int r, int c,int h,int g, Cell* p)
{
	row = r;
	col = c;
	this->h = h;
	this->g = g;
	this->f = h + g;
	parent = p;

	
}
