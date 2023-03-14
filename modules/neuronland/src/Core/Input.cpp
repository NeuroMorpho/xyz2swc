//
//	Input.cpp
//

#include <algorithm>
#include <ctype.h>
#include <vector>

#include "Core/Debug.h"
#include "Core/InputFile.h"

unsigned g_bytesRead;

class InputSources: public std::vector<InputSource *>
{
	bool mClosing;

public:
	InputSources()
	{
		reserve(16);
		mClosing = false;
	}

	~InputSources()
	{
		mClosing = true;

		for (iterator i=begin(); i!=end(); ++i)
			report("io: Source '"+(*i)->getIdentifier()+"' was not explicitly freed.");
	}

	InputSource *find(const String& prefix)
	{
		for (iterator i=begin(); i!=end(); ++i)
			if (prefix == (*i)->getIdentifier())
			{
				return *i;
			}

		return 0;
	}

	void add(InputSource *pSource, bool bFront)
	{
		report("io: New source '"+pSource->getIdentifier()+"'");
		assert(!find(pSource->getIdentifier()));
		if (bFront)
		{
			insert(begin(), pSource);
		}
		else
		{
			push_back(pSource);
		}
	}

	void remove(InputSource *pSource)
	{
		if (!mClosing)
		{
			report("io: Deleting source '"+pSource->getIdentifier()+"'");
			erase( std::find( begin(), end(), pSource ) );
		}
	}


	static InputSources *pInstance;

	static InputSources &instance()
	{
		if (!pInstance)
		{
			pInstance = new InputSources();
//#ifdef _WIN32
//			atexit(cleanup);
//#endif
			new InputFile::Source("cwd", "", true, false, false, "");
		}
		return *pInstance;
	}

	static void cleanup()
	{
		delete pInstance;
		pInstance=0;
	}
};

InputSources *InputSources::pInstance = 0;

#define g_InputSources InputSources::instance()

InputSource::InputSource(const String& identifier, bool bFront): mIdentifier(identifier)
{
	g_InputSources.add(this, bFront);
}

InputSource::~InputSource()
{
	g_InputSources.remove(this);
}

Input::Input(InputSource* pSource)
{
	mpSource = pSource;
	clearFailFlag();
}

Input::~Input()
{
}

bool Input::exists(const String& name)
{
	Input *in=open(name);

	if (in)
	{
		delete in;
		return true;
	}

	return false;
}

Input *Input::open(const String& name)
{
	String prefix = name.substr(0, name.find_first_of(':'));
	InputSource *pSource = prefix.empty() ? 0 : g_InputSources.find(prefix);
	Input *pIn = 0;

	if (pSource)
	{
		pIn = pSource->openInput(name.substr(name.find_first_of(':') + 1));
	}
	else
	{
		for (std::vector<InputSource *>::iterator i=g_InputSources.begin(); i!=g_InputSources.end(); ++i)
		{
			pIn=(*i)->openInput(name);

			if (pIn)
			{
				pSource = *i;
				break;
			}
		}
	}

	if (pIn)
	{
		report("io: Reading "+name+" from "+pSource->getIdentifier());
		Callback(pSource, pIn, name);
	}
	else
	{
		report("io: Failed to read from "+name);
		Callback(0, 0, name);
	}

	return pIn;
}

void *Input::load(const String& name, int &length)
{
	Input *pIn=open(name);
	if (pIn)
	{
		length=pIn->remaining();
		u8 *memory= new u8[length];
		pIn->read(memory, length);
		delete pIn;
		return memory;
	}
	else
	{
		length=0;
		return 0;
	}
}

void *Input::load(const String& name)
{
	Input *pIn=open(name);
	if (pIn)
	{
		int length=pIn->remaining();

		u8 *memory= new u8[length];
		pIn->read(memory, length);
		delete pIn;
		return memory;
	}
	else
	{
		return 0;
	}
}

String Input::readLine()
{
	String result;

	if(remaining() == 0)
		return result;

	u8 ch;
	do
	{
		read(ch);
		if (ch && ch!=10 && ch!=13)
			result+=ch;
	}
	while (remaining() && ch && ch!=10 && ch!=13);

	// skip LF if found after CR
	if(remaining() && ch == 13)
	{
		read(ch);
		if(ch != 10)
			rollback();
	}
	//

	return result;
}

String Input::readString()
{
	String result;
	u8 ch;
	do
	{
		read(ch);
		if (ch)
			result+=ch;
	}
	while (remaining() && ch);
	return result;
}


static bool IsWhitespace( char ch )
{
	switch(ch)
	{
	case '\t':
	case ' ':
	case '\r':
	case '\n':
			return true;
	}
	return false;
}

static bool _isalnum( const char ch )
{
	return (ch == '_') || isalnum( ch );
}


void Input::rollback()
{
	seek( -1 );
}


void Input::Callback(const class InputSource *, const class Input *, const String&)
{

}
