#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <cctype>
#include <iostream>
#include <fstream>

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::StudentSpellCheck() {
	m_head = new Node(); // head is a dummy node
}

StudentSpellCheck::~StudentSpellCheck() {
	clear(m_head);
}

void StudentSpellCheck::clear(Node* node) {
	for (int i = 0; i < 27; ++i) {
		if (node->next[i] != nullptr) {
			clear(node->next[i]);
		}
	}

	delete node;
}

int StudentSpellCheck::getIndex(char ch) const {
	if (ch == '\'') {
		return 26;
	}
	else if (ch - 'a' >= 0 && ch - 'a' <= 25) {
		return ch - 'a';
	}
	else {
		return -1;
	}
}

bool StudentSpellCheck::checkWord(std::string word) const {
	Node* cur = m_head;

	// O(L)
	for (int i = 0; i < word.size() && cur != nullptr; ++i) {
		char c = tolower(word[i]);
		cur = cur->next[getIndex(c)];
	}

	// if we aren't at a nullptr, that means the loop went through in its entirety
	// thus we've reached the end of the word, so if the current Node's end is true
	// this is a valid word
	if (cur != nullptr && cur->end == true) {
		return true;
	}

	return false;
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
	// load file via specified pathname
	std::ifstream infile(dictionaryFile);

	// pathname exists
	if (infile) {
		Node* cur;
		std::string str;

		// while there are still lines in the file
		while (getline(infile, str)) {
			cur = m_head;
			// start from head, adding trie nodes if necessary or following trie nodes for every word
			// O(N) for N lines
			for (int i = 0; i < str.size(); ++i) {
				char c = tolower(str[i]);
				int index = getIndex(c);

				if (index != -1) {
					if (cur->next[index] == nullptr) {
						cur->next[index] = new Node(c);
					}

					cur = cur->next[index];
				}
			}

			cur->end = true; // end of line, word complete
		}

		return true;
	}

	// pathname fails, return false
	return false;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	// checks if word exists in dictionary
	if (checkWord(word))
		return true;

	// O(oldS)
	suggestions.clear();

	// O(L^2 + maxSuggestions)
	Node* cur = m_head;
	std::string curSug = "";	// string to hold current suggestion
	for (int s = 0, j = 0; j < word.size() && s < max_suggestions && cur != nullptr; ++j) {
		char c = tolower(word[j]);

		// for each possible next letter that ISN'T the current letter of word
		// O(L), runs O(L) operations within the loop a constant number of times
		for (int l = 0; l < 27; ++l) {
			Node* temp = cur->next[l];
			if (temp != nullptr && temp->val != c) {
				// add new letter to current suggestion
				curSug.push_back(temp->val);

				// follow the letters of the rest of the word to see if there exists a suggestion
				// O(L)
				for (int k = j + 1; k < word.size() && temp != nullptr; ++k) {
					char c2 = tolower(word[k]);
					curSug.push_back(c2);
					temp = temp->next[getIndex(c2)];
				}

				// O(L) due to substr
				// if we aren't at a nullptr, that means the loop went through in its entirety
				// thus we've reached the end of the word, so if the current Node's end is true
				// this is a valid word
				if (temp != nullptr && temp->end == true) {
					suggestions.push_back(curSug);
					++s;
				}
				curSug = curSug.substr(0, j);
			}
		}

		// check suggestions where the current character of word is not the one that's changed
		curSug.push_back(c);
		// move current node to the proper node in the Trie to search
		cur = cur->next[getIndex(c)];
	}

	return false; // TODO
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	// TODO

	// O(oldP)
	problems.clear();

	SpellCheck::Position pos;
	pos.start = -1;
	pos.end = 0;

	// O(S)
	for (int i = 0; i <= line.size(); ++i) {
		// past last char or on invalid char
		if (i == line.size() || getIndex(tolower(line[i])) == -1) {
			// if there was previously a start position, indicating a string to check
			// and the string is NOT a word, push_back() to problems
			// O(W * L)
			if (pos.start != -1 && !checkWord(line.substr(pos.start, i - pos.start))) {
				pos.end = i - 1;
				problems.push_back(pos);
			}
			pos.start = -1;
		}
		// valid char, and start pos has not been set yet, indicating first valid char of string
		else if (pos.start == -1) {
			pos.start = i;
		}
	}
}
