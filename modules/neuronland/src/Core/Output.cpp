//	Output.cpp

#include <algorithm>
#include <vector>

#include "Core/OutputFile.h"
#include "Core/Debug.h"


class OutputSources: public std::vector<OutputSource *>
{
public:
	OutputSources()
	{
	}

	~OutputSources()
	{
		while (!empty())
			delete back();
	}

	OutputSource *find(const String& identifier)
	{
		for (iterator i=begin(); i!=end(); ++i)
			if (identifier == (*i)->getIdentifier())
			{
				return *i;
			}

		return 0;
	}

	static OutputSources *g_pSources;

public:
	void add(OutputSource *pSource)
	{
		assert(!find(pSource->getIdentifier()));
		push_back(pSource);
	}

	void remove(OutputSource *pSource)
	{
		erase( std::find( begin(), end(), pSource ) );
	}

	static OutputSources &instance()
	{
		if (g_pSources)
		{
			return *g_pSources;
		}
		else
		{
			g_pSources=new OutputSources();

			new OutputFile::Source("cwd", "", true, false, "");

			return *g_pSources;
		}
	}

	static void cleanup()
	{
		delete g_pSources;
		g_pSources=0;
	}
};

OutputSources *OutputSources::g_pSources=0;

OutputSource::OutputSource(const String& identifier): mIdentifier(identifier)
{
	OutputSources::instance().add(this);
}

Output *OutputSource::appendOutput(const String& /* url */)
{
	return 0;
}

OutputSource::~OutputSource()
{
	OutputSources::instance().remove(this);
}

Output::Output(OutputSource *pSource)
{
	mpSource = pSource;
	ClearFailed();
}

Output *Output::open(const String& url)
{
	OutputSources &sources = OutputSources::instance();

	String prefix= url.substr(0, url.find_first_of(':'));
	OutputSource *pSource = (prefix.length()>0) ? sources.find(prefix) : 0;
	Output *pOut=0;

	if (pSource)
	{
		pOut=pSource->openOutput(url.substr(url.find_first_of(':')+1));
	}
	else
	{
		for (std::vector<OutputSource *>::iterator i=sources.begin(); pOut==0 && i!=sources.end(); ++i)
		{
			pOut=(*i)->openOutput(url);
			if (pOut)
			{
				pSource=*i;
				break;
			}
		}
	}

#ifdef _DEBUG
	if (pOut)
		report("io: Writing "+url+" to "+pSource->getIdentifier());
	else
		report("io: Failed to write to "+url);
#endif

	return pOut;
}

Output *Output::overwrite(const String& url)
{
	OutputSources &sources = OutputSources::instance();

	String prefix= url.substr(0, url.find_first_of(':'));
	OutputSource *pSource = (prefix.length()>0) ? sources.find(prefix) : 0;
	Output *pOut=0;

	if (pSource)
	{
		pOut=pSource->overwriteOutput(url.substr(url.find_first_of(':')));
	}
	else
	{
		for (std::vector<OutputSource *>::iterator i=sources.begin(); pOut==0 && i!=sources.end(); ++i)
		{
			pOut=(*i)->overwriteOutput(url);
			if (pOut)
			{
				pSource=*i;
				break;
			}
		}
	}

#ifdef _DEBUG
	if (pOut)
		report("io: Writing "+url+" to "+pSource->getIdentifier());
	else
		report("io: Failed to write to "+url);
#endif

	return pOut;
}

Output *Output::append(const String& url)
{
	OutputSources &sources = OutputSources::instance();

	String prefix= url.substr(0, url.find_first_of(':'));
	OutputSource *pSource = (prefix.length()>0) ? sources.find(prefix) : 0;
	Output *pOut=0;

	if (pSource)
	{
		pOut=pSource->appendOutput(url.substr(url.find_first_of(':')));
	}
	else
	{
		for (std::vector<OutputSource *>::iterator i=sources.begin(); pOut==0 && i!=sources.end(); ++i)
		{
			pOut=(*i)->appendOutput(url);
			if (pOut)
			{
				pSource=*i;
				break;
			}
		}
	}

#ifdef _DEBUG
	if (pOut)
		report("io: Appending to "+url);
	else
		report("io: Failed to write to "+url);
#endif

	return pOut;
}

void Output::writeString(const String& s)
{
	write(s.c_str(), s.length());
	write((char)0);
}

void Output::writeStringNoZero(const String& s )
{
	write(s.c_str(), s.length());
}

void Output::writeLine(const String& s, bool crlf)
{
	write(s.c_str(), s.length());
	if (crlf)
		write((u8) 0xd);
	write('\n');
}

bool Output::save(const String& file, const u8 *data, int length)
{
	Output *pOut = Output::open(file);
	if (pOut)
	{
		pOut->write(data, length);
		delete pOut;
		return true;
	}
	else
		return false;

}

Output::~Output()
{
}

OutputSource* Output::GetSource(void) const
{
	return mpSource;
}

Output::Size Output::Seek(Offset, SeekMode) const
{
	OutputSource *pSource = GetSource();
	String desc = pSource ? GetName()+" (opened from "+pSource->getIdentifier()+")": GetName();
	fatal("Output::Seek not implemented for "+desc); // If you hit this - you need to override and implement Seek in your derived class
	return 0;
}
