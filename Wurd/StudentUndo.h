#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>

class StudentUndo : public Undo {
public:
	virtual ~StudentUndo();

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
	// ActData holds the data members for each Undo call from submit
	struct ActData {
		Action action;
		int row;
		int col;
		char ch;
		ActData(Action a, int r, int c, char cr) : action(a), row(r), col(c), ch(cr) {}
	};

	std::stack<ActData*> m_stack;
};

#endif // STUDENTUNDO_H_
