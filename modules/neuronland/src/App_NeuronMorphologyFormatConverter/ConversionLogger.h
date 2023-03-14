#ifndef _NLMC_CONVERSIONLOGGER_HPP_
#define _NLMC_CONVERSIONLOGGER_HPP_

#include "Core/Logger.h"
#include "Core/String.hpp"

class ConversionLogger : public Logger 
{
public:
	ConversionLogger(bool echoToStdout, bool storeLog) : mEchoToStdout(echoToStdout), mStoreLog(storeLog) 
	{
	}

	virtual void Log(const String& str)
	{
		if(mEchoToStdout)
			printf( String( str + "\n").c_str() );

		if(mStoreLog)
			mLogText.push_back(str);
	}

	void ResetLog()
	{
		mLogText.clear();
	}

	void OutputLog(Output* pOut)
	{
		for(Strings::const_iterator it = mLogText.begin(); it != mLogText.end(); ++it)
			pOut->writeLine( *it );
	}

private:
	Strings mLogText;
	bool mEchoToStdout;
	bool mStoreLog;
};


#endif // _NLMC_CONVERSIONLOGGER_HPP_

