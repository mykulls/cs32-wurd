#include "StudentUndo.h"
#include <stack>
#include <list>

Undo* createUndo()
{
	return new StudentUndo;
}

StudentUndo::~StudentUndo() {
	clear();
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
	// batching done in get()
	// O(1)
	m_stack.push(new ActData(action, row, col, ch));
}

StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) {
	// return error and don't change parameters if stack empty
	if (m_stack.empty()) 
		return Action::ERROR;

	// fetches top item from stack
	ActData* cur = m_stack.top();
	m_stack.pop();
	text = "";
	count = 1;

	if (cur->action == Action::DELETE) {
		std::list<char> textList;	// list to keep track of characters to insert back, used for O(1) insert anywhere
		textList.push_back(cur->ch);
		std::list<char>::iterator lastIns = textList.begin();	// location of last inserted character
		row = cur->row;
		col = cur->col; // most recent deletion is the col to start inserting back

		// batches additional deletes, if they exist
		// O(L)
		while (!m_stack.empty() && m_stack.top()->action == cur->action) {
			ActData* temp = m_stack.top();
			if (cur->row == temp->row && (cur->col == temp->col || cur->col == temp->col - 1)) {
				if (cur->col == temp->col) {
					// undo delete, insert in front of last inserted char
					lastIns = textList.insert(lastIns, temp->ch);
				}
				else {
					// undo backspace, insert behind last inserted char
					++lastIns;
					lastIns = textList.insert(lastIns, temp->ch);
				}
				delete cur;
				cur = temp;
				m_stack.pop();
			}
			else
				break;
		}

		// O(L)
		// converts character list to string
		for (std::list<char>::iterator it = textList.begin(); it != textList.end(); ++it) {
			text.push_back(*it);
		}

		delete cur;
		return Action::INSERT;
	}
	else if (cur->action == Action::INSERT) {
		row = cur->row;

		// batches additional insertions, if they exist
		// O(L)
		while (!m_stack.empty() && m_stack.top()->action == cur->action) {
			ActData* temp = m_stack.top();
			if (cur->row == temp->row && cur->col == temp->col + 1) {
				++count;
				delete cur;
				cur = temp;
				m_stack.pop();
			}
			else
				break;
		}

		col = cur->col - 1; // earliest insertion is the col to start delete from

		delete cur;
		return Action::DELETE;
	}
	else if (cur->action == Action::SPLIT) {
		// O(1)
		row = cur->row;
		col = cur->col;

		delete cur;
		return Action::JOIN;
	}
	else if (cur->action == Action::JOIN) {
		// O(1)
		row = cur->row;
		col = cur->col;

		delete cur;
		return Action::SPLIT;
	}

	// this should never occur, as stack actions should always be insert, delete, split, or join
	// this is to ensure proper compilation
	return Action::ERROR; 
}

void StudentUndo::clear() {
	while (!m_stack.empty()) {
		delete m_stack.top();
		m_stack.pop();
	}
}
