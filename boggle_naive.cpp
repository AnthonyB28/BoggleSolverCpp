#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <iostream>
#include <fstream>

/* Boggle solve - naive solution
   Input: dimensions, board, dictionary of valid words
   Output: all words found from board in dictionary
*/

struct Index;
std::vector<std::string> const FindBoggleWords(int dimensions, char const * const board, std::set<std::string> const & dictionary);
std::vector<std::string> const FindWordsAtIndex(Index index, std::string letters, std::vector<Index> visitedLetters, int const dimensions, char const * const board,
	std::set<std::string> const & dictionary);
void AppendVector(std::vector<std::string> & target, std::vector<std::string> const & toAppend);

int main(int argc, const char* argv[])
{
	//std::set<std::string> const dictionary = { "bred", "merry", "bed", "lead", "xan", "ebvv" "yore", "byre", "abed", "oread", "bore", "orby", "robed", "broad", "byroad", "robe", "bored", "derby", "bade", "aero", "read", "orbed", "verb", "aery", "bead", "bread", "very", "road" };
	int const dimension = 4;
	std::ifstream input("boggle.txt");
	std::string inputLine;
	std::set<std::string> dictionary;
	while (std::getline(input, inputLine))
	{
		dictionary.insert(inputLine);
	}
	char boggle[dimension * dimension + 1] = "idboauiyaoaedeuu";
	std::vector<std::string> const foundWords = FindBoggleWords(dimension, boggle, dictionary);
	std::cout << foundWords.size();
	std::cin.get();
	return 0;
}

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

// Returns words found on a flat 2D boggle board array from a dictionary of valid words
// Note: Directions did not say to disregard words less than 3 characters so I left it out. I believe that is a Boggle rule though.
std::vector<std::string> const FindBoggleWords(int dimensions, char const * const board, std::set<std::string> const & dictionary)
{
	std::vector<std::string> foundWords;
	for (int x = 0; x < dimensions; ++x)
	{
		for (int y = 0; y < dimensions; ++y)
		{
			AppendVector(foundWords, FindWordsAtIndex(Index(x, y), std::string(""), std::vector<Index>(), dimensions, board, dictionary));
		}
	}
	return foundWords;
}

/* Recursively finds words using valid Boggle rules (left,right,up,down,diagonals, not reusing indexes used)
   1) Could be cleaner - dimensions, board, and dictionary would be better in a class instead of parameters.
   2) Running time is not the best, I believe it is a rather large permutation. One possible way to improve would be to -
   eliminate any searches where the string path we're searching is NOT in the dictionary.
   E.g if letters = "app" and the dictionary does not contain any words that start with "app" like "apples" we stop searching.
   This would mean the dictionary would be best as a prefix tree as opposed to using an ordered set.
*/
std::vector<std::string> const FindWordsAtIndex(Index index, std::string letters, std::vector<Index> visitedLetters, int const dimensions, char const * const board,
	std::set<std::string> const & dictionary)
{
	std::vector<std::string> foundWords; // Setup our vector
	if (std::find_if(visitedLetters.begin(), visitedLetters.end(), index) == visitedLetters.end())
	{
		// We have not visited this space before, perform search.
		letters.push_back(board[dimensions * index.x + index.y]);
		visitedLetters.push_back(index); // Don't visit this index again on the board before returning up a level of recursion.

		// Check if we found a word yet
		std::set<std::string>::const_iterator wordSearch = dictionary.find(letters);
		if (wordSearch != dictionary.cend())
		{
			foundWords.push_back(*wordSearch);
		}

		// Search adjacent & diagonal letters
		if (index.x - 1 >= 0) // up
		{
			AppendVector(foundWords, FindWordsAtIndex(Index(index.x - 1, index.y), letters, visitedLetters, dimensions, board, dictionary));
			if (index.y + 1 < dimensions) // up Right
			{
				AppendVector(foundWords, FindWordsAtIndex(Index(index.x - 1, index.y + 1), letters, visitedLetters, dimensions, board, dictionary));
			}
			if (index.y - 1 >= 0) // up Left
			{
				AppendVector(foundWords, FindWordsAtIndex(Index(index.x - 1, index.y - 1), letters, visitedLetters, dimensions, board, dictionary));
			}
		}

		if (index.x + 1 < dimensions) // Down
		{
			AppendVector(foundWords, FindWordsAtIndex(Index(index.x + 1, index.y), letters, visitedLetters, dimensions, board, dictionary));

			if (index.y + 1 < dimensions) // down Right
			{
				AppendVector(foundWords, FindWordsAtIndex(Index(index.x + 1, index.y + 1), letters, visitedLetters, dimensions, board, dictionary));
			}
			if (index.y - 1 >= 0) // down Left
			{
				AppendVector(foundWords, FindWordsAtIndex(Index(index.x + 1, index.y - 1), letters, visitedLetters, dimensions, board, dictionary));
			}
		}

		if (index.y + 1 < dimensions) // right
		{
			AppendVector(foundWords, FindWordsAtIndex(Index(index.x, index.y + 1), letters, visitedLetters, dimensions, board, dictionary));
		}
		if (index.y - 1 >= 0) // left
		{
			AppendVector(foundWords, FindWordsAtIndex(Index(index.x, index.y - 1), letters, visitedLetters, dimensions, board, dictionary));
		}
	}

	return foundWords;
}

// Utility to nicely append vectors
void AppendVector(std::vector<std::string> & target, std::vector<std::string> const & toAppend)
{
	if (toAppend.size() > 0)
	{
		target.insert(target.end(), toAppend.begin(), toAppend.end());
	}
}