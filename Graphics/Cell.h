#pragma once
class Cell
{
private:
	int row;
	int col;
	int h;
	int g;
	int f;
	Cell* parent;

public:
	Cell();
	Cell(int r, int c ,int h,int g, Cell* p);
	int getRow() { return row; }
	int getCol() { return col; }
	int getH() { return h; }
	int getG() { return g; }
	int getF() { return f; }
	Cell* getParent() { return parent; }

	// compare cells by their h value (for the priority queue) return true if this cell has a smaller h value
	bool operator<(const Cell& other) const
	{
		return f > other.f;
	}
};

