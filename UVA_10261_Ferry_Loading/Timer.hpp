/*
 * Timer.hpp
 *
 *  Created on: 19 Nov 2011
 *      Author: Adrian
 */

#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <Windows.h>
using namespace std;

class TimedSection
{
public:
	TimedSection(string name = "") : mName(name)
	{
		QueryPerformanceFrequency((LARGE_INTEGER *)&mFreq);
		QueryPerformanceCounter((LARGE_INTEGER *)&mStart);
	}
	~TimedSection()
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&mFinish);
		cout << "Section: " << mName << " " << ((mFinish-mStart)*1.0 / mFreq) << " seconds" << endl;
	}
private:
       __int64 mStart;
       __int64 mFinish;
       __int64 mFreq;
       string mName;
};


#endif /* TIMER_HPP_ */
