//
// parser.cpp
//

#include <algorithm>

#include "assert.h"

#include "Core/String.hpp"
#include "Core/Parser.h"
#include "Core/Input.h"

SimpleParser::SimpleParser(Input& rIn) : rIn(rIn)
{
	SetDefaultWhiteSpace();
	done = false;
}

SimpleParser::~SimpleParser()
{
}

void SimpleParser::ResetDefaults()
{
	mWhitespace.clear();
	mIgnoreAfter.clear();
	mIgnoreBlocks.clear();
	mScoper.clear();
	mTokens.clear();

	SetDefaultWhiteSpace();
}


void SimpleParser::SetDefaultWhiteSpace()
{
	// default whitespace: tab, CR, LF, FF, space
	SetWhiteSpace(0x09);
	SetWhiteSpace(0x0a);
	SetWhiteSpace(0x1a);
	SetWhiteSpace(0x0c);
	SetWhiteSpace(0x0d);
	SetWhiteSpace(0x20);
}

void SimpleParser::SetWhiteSpace(const char c)
{
	mWhitespace.push_back(c);
}

void SimpleParser::SetIgnoreAfter(const char token)
{	
	mIgnoreAfter.push_back( token );
}

void SimpleParser::SetIgnoreBlock(const String& open, const String& close)
{
	mIgnoreBlocks.push_back( pair<String, String>(open, close) );
}

void SimpleParser::SetToken(const String& token)
{
	mTokens.push_back( token );
}

void SimpleParser::SetToken(const char c)
{
	String tok;
	tok += c;

	mTokens.push_back( tok );
}

bool SimpleParser::IsToken(const char c)
{
	String tok;
	tok += c;

	return find(mTokens.begin(), mTokens.end(), tok ) != mTokens.end();
}

bool SimpleParser::IsWhitespace(const char c)
{
	vector<char>::const_iterator i;

	for(i=mWhitespace.begin();i!=mWhitespace.end();++i)
	{
		if(c == (*i) )
			return true;
	}

	return false;
}

bool SimpleParser::IsIgnoreAfter(const char c)
{
    vector<char>::const_iterator i;

	for(i=mIgnoreAfter.begin();i!=mIgnoreAfter.end();++i)
	{
		if(c == (*i) )
			return true;
	}

	return false;
}

bool SimpleParser::SeekStartOfNextLine()
{
	char c;

	do
	{
		if( rIn.remaining() == 0 || rIn.didFail() ) 
			return false;

		rIn.read(c);
	}
	while(c != 0xa);


	rIn.read(c);

	if(c != 0xd)
		rIn.rollback();

	return true;
}

bool SimpleParser::SeekNextLine()
{
	char c;

	do
	{
		if( rIn.remaining() == 0 || rIn.didFail() ) 
			return false;

		rIn.read(c);
	}
	while(c != 0xa);

	rIn.rollback();

	return true;
}

bool SimpleParser::SeekNextItem()
{
	if(rIn.remaining() == 0)
		return false;

	char c;
	do
	{
		do
		{
			rIn.read(c);

			/*
			if(m_is.fail() || m_is.bad() )
			{
				assert(0);
			}
			*/

			if(c == 0x1a)
			{
				c= ' ';
				assert(0);
			}
		}
		while(IsWhitespace(c) && rIn.remaining() > 0);


		if( rIn.remaining() == 0 || rIn.didFail() ) 
		{
			return false;
		}

		if(IsIgnoreAfter(c))
		{
			if(!SeekNextLine())
			{
				//assert(0);
				return false;
			}
		}
		/*
		else if(IsIgnorableBlock(ptr))
		{	
			SkipUntil();
		}
		*/
		else
		{
			
			rIn.rollback();
			return true;
		}

	} while(rIn.remaining() > 0);

	return false;
}



bool SimpleParser::SeekEndOfItem()
{
	char c;

	if(rIn.remaining() == 0)
	{
		return false;
	}
	rIn.read(c);

	if( IsToken(c) )
    {
		return true;
	}
	else
	{
		while( !IsToken(c) && !IsWhitespace(c))
		{
			rIn.read(c);
		}
		rIn.rollback();
		return true;
	}
}


String SimpleParser::GetRestOfLine()
{
	String result;
	char c;

	do
	{
		rIn.read(c);

		if(c != 0xa)
			result += c;
		else
		{
			rIn.rollback();
			break;
		}
	}
	while(rIn.remaining() > 0);

	return result;
}

//
String SimpleParser::GetNextItem()
{
	String result;
	int index=0;
	char c;

	if(rIn.remaining() == 0)
	{
		return String("");
	}

	rIn.read(c);
	result += c;

	if( IsToken(c) )
    {
		return result;
	}
	else
	{
		while(1)
		{
			rIn.read(c);
			if(IsToken(c) || IsWhitespace(c) || IsIgnoreAfter(c) ) break;
			result += c;
		}
		rIn.rollback();
		return result;
	}
}

void SimpleParser::RollbackCurrentItem(const String& item)
{
	u32 count = item.length();

	while(count)
	{
		rIn.rollback();
		--count;
	}
}

float SimpleParser::GetNextItemAsFloat()
{
	return std::stof(GetNextItem());
}

int SimpleParser::GetNextItemAsInt()
{
	return std::stoi(GetNextItem());
}

String SimpleParser::GetQuotedItem()
{
	char c;
	int index=0;
	String result;
	rIn.read(c);

	if(c == '"')
	{
		while(1)
		{
			rIn.read(c);
			if(c == '"')
			{
				return result;
			}
			else
			{
				result += c;
			}
		}
	}
	else
	{
		rIn.rollback();
		return String("");
	}
}

bool SimpleParser::SeekPastSubstring(const String& str)
{
	char c;
	String copy;
	int index = 0;

	while( index < str.length() && rIn.remaining() > 0 )
	{
		rIn.read(c);

		if(IsIgnoreAfter(c))
		{
			SeekNextLine();
			index = 0;
			continue;
		}

		if(c != str[index])
		{
			index = 0;
		}
		else
		{
			index++;
		}
	}

	return( index == str.length() );
}


void SimpleParser::SkipUntil(const String& end)
{
	/*
    char *ptr = c;
    int bracketcount = 0;

    while(bracketcount >= 0)
    {
        ptr = SeekNextItem(ptr);
        if( *ptr == '(' ) bracketcount++;
        else if( *ptr == ')' ) bracketcount--;
        ptr = SeekEndOfItem(ptr);
    }

    ptr++; // increment past block (i.e. past final bracket)

    return ptr;
	*/
}


