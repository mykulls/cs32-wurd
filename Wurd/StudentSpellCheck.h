#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

class StudentSpellCheck : public SpellCheck {
public:
	StudentSpellCheck();
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	// represents each node of the Trie
	struct Node {
		char val;
		bool end; 		// true if word can end here
		Node* next[27];	// 0-25 are letters, 26 is apostrophe
		Node(char c = 0) : val(c), end(false) {
			for (int i = 0; i < 27; ++i) {
				next[i] = nullptr;
			}
		}
	};

	void clear(Node* node);					// helper function for destructor, runs recursively
	int getIndex(char ch) const;			// helper function to get proper index for letter/apostrophe
	bool checkWord(std::string word) const;	// returns true if word is in the dictionary

	Node* m_head;
};

#endif  // STUDENTSPELLCHECK_H_
