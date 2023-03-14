#ifndef __CORE_PARSER_H__NL__
#define __CORE_PARSER_H__NL__

#include <vector>
#include <utility>

#include "Core/String.hpp"

#include "Core/Input.h"
#include "Core/Output.h"

using namespace std;


/** 

	Simple parser functionality for parsing text files of various types, where certain characters are whitespace, special tokens, 
	comments.

	Derive from this to create Parsers with specialist/enhanced fucntionality to help parse specific file formats.

**/

class SimpleParser
{
	//
	vector<char> mWhitespace;
	vector<char> mIgnoreAfter;

	vector<String> mTokens;
	vector<String> mScoper;
	vector< pair<String, String> > mIgnoreBlocks;

public:

	Input& rIn;
	//std::ifstream &m_is;
	bool done;

	SimpleParser(Input& rIn);
	~SimpleParser();

// configure the parser
	void ResetDefaults();

	void SetDefaultWhiteSpace();

	void SetWhiteSpace(const char);
	bool IsWhitespace(const char c);

	void SetIgnoreAfter(const char c);
	bool IsIgnoreAfter(const char c);

	void SetToken(const String& token);
	void SetToken(const char c);
	bool IsToken(const char c);

	void SetIgnoreBlock(const String& open, const String& close);

// move through the stream
	bool SeekNextLine();
	bool SeekNextItem();
	bool SeekEndOfItem();

	void RollbackCurrentItem(const String& item);

	bool SeekStartOfNextLine();

	String GetNextItem();
	String GetQuotedItem();
	String GetRestOfLine();

	float GetNextItemAsFloat();
	int GetNextItemAsInt();

	void SkipUntil(const String& str);

	bool SeekPastSubstring(const String& str);
};


/*
//
// a better parser:  set up callbacks to handle parser events...
//
class CallbackDrivenParser
{
public:

	CallbackDrivenParser(ifstream &is);
	bool Parse();
};
*/

#endif // __CORE_PARSER_H__NL__
