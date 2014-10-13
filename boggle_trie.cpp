#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
//#include <iostream>
//#include <fstream>

/* Boggle solve - TRIE solution (prefix tree)
Input: dimensions, board, dictionary of valid words
Output: all words found from board in dictionary */

// Coordinate on boogle board
struct Index
{
	Index(int x, int y)
		: x(x)
		, y(y) {}

	bool operator () (const Index& rh) const
	{
		return (rh.x == x && rh.y == y);
	}

	int x;
	int y;
};

// Prefix tree
struct DictionaryNode
{
	DictionaryNode(char letter)
		: m_Letter(letter)
		, m_IsFullWord(false){}

	char m_Letter;
	bool m_IsFullWord;
	std::vector<DictionaryNode *> m_Children;
};

// Results for searching in the prefix tree
enum FindResult
{
	Exists = 0, // Letters exist, but may not be a full valid word
	FullWord = 1, // Full word from dictionary
	NotFound = 2 // Doesn't exist in prefix tree
};

std::set<std::string> const FindBoggleWords(int dimensions, char const * const board, DictionaryNode const * const dictionary);
std::vector<std::string> const FindWordsAtIndex(Index index, std::string letters, std::vector<Index> visitedLetters, int const dimensions, char const * const board,
	DictionaryNode const * const dictionary);
void WordInsertPrefixTree(DictionaryNode * const node, std::string const & word);
FindResult WordFindPrefixTree(DictionaryNode const * const node, std::string & word);
void AppendVectorToVector(std::vector<std::string> & target, std::vector<std::string> const & toAppend);
void AppendVectorToSet(std::set<std::string> & target, std::vector<std::string> const & toAppend);
void DeletePrefixTree(DictionaryNode * node);


int main(int argc, const char* argv[])
{
	std::set<std::string> const dictionarySet = { "bred", "merry", "bed", "lead", "xan", "ebvv" "yore", "byre", "abed", "oread", "bore", "orby", "robed", "broad", "byroad", "robe", "bored", "derby", "bade", "aero", "read", "orbed", "verb", "aery", "bead", "bread", "very", "road" };
	int const dimension = 3;
	DictionaryNode * dictionary = new DictionaryNode('\0');
	for (std::set<std::string>::const_iterator it = dictionarySet.cbegin(); it != dictionarySet.cend(); ++it)
	{
		WordInsertPrefixTree(dictionary, (*it));
	}
	
	/*For use with file input dictionary
	std::ifstream input("boggle.txt");
	std::string inputLine; 
	while (std::getline(input, inputLine))
	{
		WordInsertPrefixTree(dictionary, inputLine);
	} */
	
	char boggle[dimension * dimension + 1] = "yoxrbaved";
	std::set<std::string> const foundWords = FindBoggleWords(dimension, boggle, dictionary);
	DeletePrefixTree(dictionary);
	std::cin.get();
	return 0;
}

// Deletes the allocated prefix tree from the heap
void DeletePrefixTree(DictionaryNode * node)
{
	for (std::vector<DictionaryNode *>::const_iterator it = node->m_Children.cbegin(); it != node->m_Children.cend(); ++it)
	{
		DeletePrefixTree(*it);
	}
	
	delete node;
}

// Searches Prefix Tree for a word and returns whether or not it finds a dictionary word, or part of one
FindResult WordFindPrefixTree(DictionaryNode const * const node, std::string & word)
{
	if (node != nullptr && word != "\0") // \0 may not be necessary
	{
		for (std::vector<DictionaryNode *>::const_iterator it = node->m_Children.cbegin(); it != node->m_Children.cend(); ++it)
		{
			// If we match a letter, check if we've found a word or continue deeper in tree
			if ((*it)->m_Letter == word[0]) 
			{
				if (word.size() == 1 && (*it)->m_IsFullWord)
				{
					return FindResult::FullWord;
				}
				else
				{
					return WordFindPrefixTree(*it, word.substr(1, word.size()));
				}
			}
		}
	}

	// We're at the end of our letters, either we found part of a word or none at all
	if (word.size() > 0)
	{
		return FindResult::NotFound;
	}
	else
	{
		return FindResult::Exists;
	}
}

// Inserts a word into a prefix tree
void WordInsertPrefixTree(DictionaryNode * node, std::string const & word)
{
	if (node != nullptr && word != "\0")
	{
		bool existingLetter = false;
		for (std::vector<DictionaryNode *>::iterator it = node->m_Children.begin(); it != node->m_Children.end(); ++it)
		{
			// Found a letter existing in child, continue deeper
			if ((*it)->m_Letter == word[0])
			{
				WordInsertPrefixTree(*it, word.substr(1, word.size()));
				existingLetter = true;
				break;
			}
		}

		// New letter, add a child node
		if (!existingLetter)
		{
			DictionaryNode * newNode = new DictionaryNode(word[0]);
			node->m_Children.push_back(newNode);
			if (word.size() == 1) // Must mark a dictionary word if its complete, then stop
			{
				newNode->m_IsFullWord = true;
			}
			else
			{
				WordInsertPrefixTree(newNode, word.substr(1, word.size()));
			}
		}
	}
	else // End of a word
	{
		node->m_IsFullWord = true;
	}
}

// Returns words found on a flat 2D boggle board array from a dictionary of valid words
// Note: Directions did not say to disregard words less than 3 characters so I left it out. I believe that is a Boggle rule though.
std::set<std::string> const FindBoggleWords(int dimensions, char const * const board, DictionaryNode const * const dictionary)
{
	std::set<std::string> foundUniqueWords;
	for (int x = 0; x < dimensions; ++x)
	{
		for (int y = 0; y < dimensions; ++y)
		{
			// We don't want duplicate words
			AppendVectorToSet(foundUniqueWords, FindWordsAtIndex(Index(x, y), std::string(""), std::vector<Index>(), dimensions, board, dictionary));
		}
	}
	return foundUniqueWords;
}

/* Recursively finds words using valid Boggle rules (left,right,up,down,diagonals, not reusing indexes used)
1) Could be cleaner - dimensions, board, and dictionary would be better in a class instead of parameters.
2) Running time is an improvement over my naive solution of regular recursive searching.
3) Might be able to do away with vectors and go entirely with sets, but I think appending them to one set later is faster */
std::vector<std::string> const FindWordsAtIndex(Index index, std::string letters, std::vector<Index> visitedLetters, int const dimensions, char const * const board,
	DictionaryNode const * const dictionary)
{
	std::vector<std::string> foundWords; // Setup our vector
	
	if (std::find_if(visitedLetters.begin(), visitedLetters.end(), index) == visitedLetters.end())
	{
		// Have not visited this space before, perform search.
		letters.push_back(board[dimensions * index.x + index.y]);
		visitedLetters.push_back(index); // Don't visit this index again on the board before returning up a level of recursion.

		// Check if found a word yet
		FindResult searchResult = WordFindPrefixTree(dictionary, letters);
		bool fullWord = searchResult == FindResult::FullWord ? true : false;
		if (searchResult == FindResult::Exists || fullWord)
		{
			if (fullWord) // Tree might only find part of a word, if so, don't add it
			{
				foundWords.push_back(letters);
			}
			// Search adjacent & diagonal letters
			if (index.x - 1 >= 0) // up
			{
				AppendVectorToVector(foundWords, FindWordsAtIndex(Index(index.x - 1, index.y), letters, visitedLetters, dimensions, board, dictionary));
				if (index.y + 1 < dimensions) // up Right
				{
					AppendVectorToVector(foundWords, FindWordsAtIndex(Index(index.x - 1, index.y + 1), letters, visitedLetters, dimensions, board, dictionary));
				}
				if (index.y - 1 >= 0) // up Left
				{
					AppendVectorToVector(foundWords, FindWordsAtIndex(Index(index.x - 1, index.y - 1), letters, visitedLetters, dimensions, board, dictionary));
				}
			}

			if (index.x + 1 < dimensions) // Down
			{
				AppendVectorToVector(foundWords, FindWordsAtIndex(Index(index.x + 1, index.y), letters, visitedLetters, dimensions, board, dictionary));

				if (index.y + 1 < dimensions) // down Right
				{
					AppendVectorToVector(foundWords, FindWordsAtIndex(Index(index.x + 1, index.y + 1), letters, visitedLetters, dimensions, board, dictionary));
				}
				if (index.y - 1 >= 0) // down Left
				{
					AppendVectorToVector(foundWords, FindWordsAtIndex(Index(index.x + 1, index.y - 1), letters, visitedLetters, dimensions, board, dictionary));
				}
			}

			if (index.y + 1 < dimensions) // right
			{
				AppendVectorToVector(foundWords, FindWordsAtIndex(Index(index.x, index.y + 1), letters, visitedLetters, dimensions, board, dictionary));
			}
			if (index.y - 1 >= 0) // left
			{
				AppendVectorToVector(foundWords, FindWordsAtIndex(Index(index.x, index.y - 1), letters, visitedLetters, dimensions, board, dictionary));
			}
		}
	}

	return foundWords;
}

// Utility to nicely append vectors to vectors
void AppendVectorToVector(std::vector<std::string> & target, std::vector<std::string> const & toAppend)
{
	if (toAppend.size() > 0)
	{
		target.insert(target.end(), toAppend.begin(), toAppend.end());
	}
}

// Utility to nicely append vectors to sets
void AppendVectorToSet(std::set<std::string> & target, std::vector<std::string> const & toAppend)
{
	if (toAppend.size() > 0)
	{
		for (std::vector<std::string>::const_iterator it = toAppend.cbegin(); it != toAppend.cend(); ++it)
		{
			target.insert(*it);
		}
	}
}