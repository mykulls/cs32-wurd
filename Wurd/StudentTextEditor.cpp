#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {
	// O(1)
	m_r = 0;
	m_c = 0;
	m_lines.push_back("");
	m_it = m_lines.begin();
}

// no body needed, list is automatically destructed as it isn't dynamically allocated
StudentTextEditor::~StudentTextEditor() {}

bool StudentTextEditor::load(std::string file) {
	// load file from pathname
	std::ifstream infile(file);
	// if file in pathname exists
	if (infile) {
		// O(M + U)
		reset();

		bool first = true;
		std::string str;
		// for every line of the file
		// O(N) for N characters (goes through each line and pushes each string)
		while (std::getline(infile, str)) {
			// carriage return at end of line, so remove it
			// pop_back() is O(1)
			if (str.size() != 0 && str[str.size() - 1] == '\r')
				str.pop_back();

			// inserting first line overrides the empty line inserted by reset()
			if (first) {
				*m_it = str;
				first = false;
			}
			else {
				// otherwise, append the line to the list
				m_lines.push_back(str);
			}
		}

		return true;
	}

	// no file in pathname, return false
	return false;
}

bool StudentTextEditor::save(std::string file) {
	// load file from pathname
	std::ofstream outfile(file);
	// if file in pathname exists
	if (outfile) {
		// writes every line to file
		// O(M) for the number of characters in file
		for (std::list<std::string>::iterator i = m_lines.begin(); i != m_lines.end(); ++i) {
			outfile << *i << '\n';
		}
		
		return true;
	}

	// no file in pathname, return false
	return false;
}

void StudentTextEditor::reset() {
	// removes each row being edited
	// O(N) for N rows
	m_lines.clear();

	// resets cursor
	m_r = 0;
	m_c = 0;

	// re-initializing
	m_lines.push_back("");
	m_it = m_lines.begin();

	// clears undo stack, runs in O(U) for U number of items on stack
	getUndo()->clear();
}

void StudentTextEditor::move(Dir dir) {
	// all O(1) operations, increment/decrement/set

	if (dir == Dir::UP) {
		if (m_r != 0) {
			// move position up if possible
			--m_r;
			--m_it;
			if (m_c > m_it->size()) {
				// move column if column is now past the current line
				m_c = m_it->size();
			}
		}
	}
	else if (dir == Dir::DOWN) {
		if (m_r != m_lines.size() - 1) {
			// move position down if possible
			++m_r;
			++m_it;
			if (m_c > m_it->size()) {
				// move column if column is now past the current line
				m_c = m_it->size();
			}
		}
	}
	else if (dir == Dir::LEFT) {
		if (m_c == 0 && m_r != 0) {
			// move position to the end of the previous line if at beginning of line
			--m_r;
			--m_it;
			m_c = m_it->size();
		}
		else if (m_c != 0) {
			// move position left if not at beginning of file
			--m_c;
		}
	}
	else if (dir == Dir::RIGHT) {
		if (m_c == m_it->size() && m_r != m_lines.size() - 1) {
			// move position to the start of the next line if at end of line
			++m_r;
			++m_it;
			m_c = 0;
		}
		else if (m_c != m_it->size()) {
			// move position right if not at end of file
			++m_c;
		}
	}
	else if (dir == Dir::HOME) {
		// set position to start of line
		m_c = 0;
	}
	else if (dir == Dir::END) {
		// set position to end of line
		m_c = m_it->size();
	}
}

void StudentTextEditor::del() {
	if (m_c == m_it->size() && m_r != m_lines.size() - 1) {
		// del at end of line

		std::string str = *m_it;	// copy current line, O(L1) for L1 length
		m_it = m_lines.erase(m_it);	// erase line from list, O(1) list deletion
		str += *m_it;				// append new line to string, O(L2) for L2 length
		(*m_it) = str;				// copy back to list, O(L1 + L2) for L1 + L2 length
		
		// submit action to Undo stack, O(1)
		getUndo()->submit(Undo::Action::JOIN, m_r, m_c);
	}
	else if (m_c != m_it->size()) {
		// del not at end of file, if end of file do nothing

		char toDel = (*m_it)[m_c];
		// delete specified character, O(L) for L length
		m_it->erase(m_c, 1);

		// submit action to Undo stack, O(1)
		getUndo()->submit(Undo::Action::DELETE, m_r, m_c, toDel);
	}
}

void StudentTextEditor::backspace() {
	if (m_c == 0 && m_r != 0) {
		// backspace at beginning of line

		std::string str = *m_it;	// copy current line, O(L1) for L1 length
		m_it = m_lines.erase(m_it);	// erase line from list, O(1) list deletion
		--m_it;						// move to previous line
		--m_r;						// move cursor accordingly
		m_c = m_it->size();
		(*m_it) += str;				// append old line, O(L1) for L1 length

		// submit action to Undo stack, O(1)
		getUndo()->submit(Undo::Action::JOIN, m_r, m_c);
	}
	else if (m_c != 0) {
		// backspace not at front of file, if front of file do nothing

		char toDel = (*m_it)[m_c - 1];
		// delete specified character, O(L) for L length
		m_it->erase(m_c - 1, 1);
		--m_c;

		// submit action to Undo stack, O(1)
		getUndo()->submit(Undo::Action::DELETE, m_r, m_c, toDel);
	}
}

void StudentTextEditor::insert(char ch) {
	if (ch == '\t') {
		// if tab, insert space 4 times, recursively
		// O(1) recursive calls
		for (int i = 0; i < 4; ++i) {
			insert(' ');
		}
	}
	else {
		// insert on string is an O(L) operation where L is the length of the string
		m_it->insert(m_it->begin() + m_c, ch);
		++m_c;

		// submit action to Undo stack, O(1)
		getUndo()->submit(Undo::Action::INSERT, m_r, m_c, ch);
	}
}

void StudentTextEditor::enter() {
	// submit action to Undo stack, O(1)
	getUndo()->submit(Undo::Action::SPLIT, m_r, m_c);	// row and col BEFORE enter

	std::string newLine = m_it->substr(m_c);	// copy string to separate out O(L) for L length
	m_it->resize(m_c);							// resize to remove separated string, O(L) for L length
	++m_it;										// increment iterator
	m_it = m_lines.insert(m_it, newLine);		// insert separated string after old line, O(1) list insertion
	++m_r;
	m_c = 0;
}

void StudentTextEditor::getPos(int& row, int& col) const {
	row = m_r;
	col = m_c;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	if (startRow < 0 || numRows < 0 || startRow > m_lines.size())
		return -1;

	// O(oldR)
	lines.clear();

	std::list<std::string>::const_iterator it = m_lines.begin();
	for (int i = 0; i < startRow && it != m_lines.end(); ++i, ++it);	// move iterator to right spot, O(startRow)
	for (int j = 0; j < numRows && it != m_lines.end(); ++j, ++it) {
		// append current line to lines for numRows lines, O(numRows * L) for L length
		lines.push_back(*it);
	}
	
	return lines.size();
}

void StudentTextEditor::undo() {
	std::string text;
	int newRow = 0;
	int count = 0;
	// fetches action to undo
	Undo::Action action = getUndo()->get(newRow, m_c, count, text);

	// only process if there is an undo action
	if (action != Undo::Action::ERROR) {
		// adjusts iterator accordingly
		if (newRow > m_r) {
			for (int i = 0; i < newRow - m_r; ++i, ++m_it);
		}
		else {
			for (int i = 0; i < m_r - newRow; ++i, --m_it);
		}
		m_r = newRow;

		if (action == Undo::Action::INSERT) {
			// insert the text at specified position
			m_it->insert(m_c, text);
		}
		else if (action == Undo::Action::DELETE) {
			// erase the text from specified position for count length
			m_it->erase(m_c, count);
		}
		else if (action == Undo::Action::SPLIT) {
			// same process as enter(), minus the submit()

			std::string newLine = m_it->substr(m_c);	// get string to separate out
			m_it->resize(m_c);							// resize to remove separated string
			++m_it;										// increment iterator
			m_it = m_lines.insert(m_it, newLine);		// insert separated string after old line
			++m_r;
			m_c = 0;
		}
		else if (action == Undo::Action::JOIN) {
			++m_it;							// go to next line
			std::string joinLine = *m_it;	// grab string from that line
			m_it = m_lines.erase(m_it);		// erase old line
			--m_it;							// go back to original line
			m_it->insert(m_c, joinLine);	// add string back
		}
	}
	
	// do nothing if action == Undo::Action::ERROR
}
