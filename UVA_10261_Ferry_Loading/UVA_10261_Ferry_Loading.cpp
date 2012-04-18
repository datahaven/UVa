/*
 * UVA_10261_Ferry_Loading.cpp
 *
 *  Created on: 21 Nov 2011
 *      Author: Adrian Dale
 *
 *      Total Submissions Users that tried it Users that solved it
			7002 				1048 				680
Your best accepted try
Ranking Submission Run Time Language Submission Date
634       9500161   2.152     C++     2011-11-23 20:33:43
 *
 * This was after my first attempt exceeded the three second time limit.
 * This second version is borderline, too - one of the slowest successful
 * entrants.
 *
 * In fact, I think I made a bit of a dog's breakfast of this problem.
 * I have a feeling I probably only needed to consider one side of the ferry,
 * which I'd pretty much attempted to do, but hadn't quite got working.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <utility>
#include <assert.h>
#ifndef ONLINE_JUDGE
#include "Timer.hpp"
#endif
using namespace std;

int FerryLength;
vector<int> Cars;

struct MemoState
{
	int mLSpace;
	int mRSpace;
	unsigned int mCarNo;
	bool operator<(const MemoState &rhs) const;
};

// We need a less than operator for our data type so that we
// can use it as the key field in an STL map
bool MemoState::operator <(const MemoState &rhs) const
{
	if (this->mCarNo == rhs.mCarNo)
	{
		if (this->mLSpace == rhs.mLSpace)
		{
			return this->mRSpace < rhs.mRSpace;
		}
		else
			return this->mLSpace < rhs.mLSpace;
	}
	else
		return this->mCarNo < rhs.mCarNo;
}

typedef map<MemoState, pair<int,char> > MemoType;
MemoType Memo;

// This is the memoized version of the recursive function below
// Attempt some short cuts to prevent TLE
int MaxCars(int LSpace, int RSpace, unsigned int CarNo)
{
	//cout << "MaxCars(" << LSpace << ", " << RSpace << ", " << CarNo << ")" << endl;
	int result;
	char side = 'x';

	MemoState m;
	m.mCarNo = CarNo;
	m.mLSpace = LSpace;
	m.mRSpace = RSpace;

	// If we already know the answer (given that we know our
	// algorithm asks for the same answers multiple times)
	// then look it up and return it.
	MemoType::iterator it = Memo.find(m);
	if (it != Memo.end())
	{
		return it->second.first;
	}
	// Otherwise we'll calculate the answer once now, and store it
	// for future reference. Dynamic Programming FTW!

	if (CarNo >= Cars.size())
	{
		// If we've ran out of cars to load, then Max no. of extra cars
		// we can load is 0
		result = CarNo;
	}
	else
	{
		int maxL = CarNo;
		int maxR = CarNo;
		int curCarLen = Cars[CarNo];

		// Recursively decide whether we fit in more cars if we park
		// this one on the left side, or on the right side
		if (LSpace >= curCarLen)
			maxL = MaxCars(LSpace-curCarLen, RSpace, CarNo+1);

		if (RSpace >= curCarLen)
			maxR = MaxCars(LSpace, RSpace-curCarLen, CarNo+1);

		// We're also storing which side of the ferry gave us the best
		// solution so that we can print out where to park the cars
		if ( maxL > maxR)
		{
			side = 'L';
			result = maxL;
		}
		else
		{
			side = 'R';
			result = maxR;
		}
	}

	Memo.insert( make_pair(m, make_pair(result, side)));

	// Insert opposite pair - not an ideal solution.
	// I preferred it when I always kept L side greater than R side,
	// but couldn't get that to work.
	if (m.mLSpace != m.mRSpace)
	{
		int temp = m.mLSpace;
		m.mLSpace = m.mRSpace;
		m.mRSpace = temp;
		if (side =='L')
			side = 'R';
		else
			side = 'L';
		Memo.insert( make_pair(m, make_pair(result, side)));
	}
	return result;
}

// I kept this function in for illustration purposes, but it isn't
// called. It is the simple recursive solution to the problem,
// which of course will only work for relatively small problem sizes.
int MaxCarsNonMemoVersion(int LSpace, int RSpace, unsigned int CarNo)
{
	// cout << "MaxCars(" << LSpace << ", " << RSpace << ", " << CarNo << ")" << endl;

	if (CarNo >= Cars.size())
	{
		// If we've ran out of cars to load, then Max no. of extra cars
		// we can load is 0
		return CarNo;
	}

	int maxL = CarNo;
	int maxR = CarNo;

	int curCarLen = Cars[CarNo];

	// Recursively decide whether we fit in more cars if we park
	// this one on the left side, or on the right side
	if (LSpace >= curCarLen)
		maxL = MaxCarsNonMemoVersion(LSpace-curCarLen, RSpace, CarNo+1);

	if (RSpace >= curCarLen)
		maxR = MaxCarsNonMemoVersion(LSpace, RSpace-curCarLen, CarNo+1);

	return max(maxL, maxR);
}

// Use the memo we built while solving to display the optimum
// ordering of the cars. We do this by stepping through the puzzle
// states, parking each car, then seeing which state that leads us to.
// This leaves us a handy sanity check that we haven't parked too many
// cars or got room for more
void ShowResult(int cars)
{
	MemoState m;
	m.mCarNo = 0;
	m.mLSpace = FerryLength;
	m.mRSpace = FerryLength;

	for(int i=0; i<cars; ++i)
	{
		m.mCarNo = i;

		MemoType::iterator it = Memo.find(m);
		assert(it != Memo.end());

		if (it->second.second == 'L')
		{
			cout << "port" << endl;
			m.mLSpace -= Cars[i];
		}
		else if (it->second.second == 'R')
		{
			cout << "starboard" << endl;
			m.mRSpace -= Cars[i];
		}
		else
		{
			assert(false);
		}
	}
	assert(m.mLSpace >= 0); // Check we didn't exceed ferry size
	assert(m.mRSpace >= 0);
	if (static_cast<unsigned int>(cars) < Cars.size())
	{
		// Check we don't have room for the next car.
		// This doesn't actually tell us we have the optimum solution,
		// but it's a better check than nothing.
		assert(m.mLSpace < Cars[cars]);
		assert(m.mRSpace < Cars[cars]);
	}
}

void SolveTestCase()
{
	//cout << "FerryLength=" << FerryLength << " Cars.size()=" << Cars.size() << endl;
	int result= MaxCars(FerryLength, FerryLength, 0);
	cout << result << endl;
	ShowResult(result);
}

void ReadTestCases()
{
#ifndef ONLINE_JUDGE
		TimedSection clock("Run Time");
#endif
	string line;
	getline(cin, line);
	istringstream Parser(line);

	int TestCases;
	Parser >> TestCases;
	while(TestCases-- > 0)
	{
		// Blank line between inputs
		getline(cin, line);

		getline(cin, line);
		Parser.str(line);
		Parser.clear();
		Parser >> FerryLength;
		assert(FerryLength >= 1 && FerryLength <= 100);

		// Convert the ferry length to cm to match the car lengths
		FerryLength *= 100;
		Cars.clear();
		while(true)
		{
			getline(cin, line);
			Parser.str(line);
			Parser.clear();
			int carLength;
			Parser >> carLength;
			if (carLength==0)
				break;
			assert(carLength >= 100 && carLength <= 10000);
			Cars.push_back(carLength);
		}

		Memo.clear();

		SolveTestCase();

		// Outputs of consecutive cases (but not last case) followed by a
		// blank line
		if (TestCases > 0)
			cout << endl;
	}
}

int main()
{
// #define DEBUG_FILE 1

#ifdef DEBUG_FILE
	cout << "WARNING: DEBUG BUILD" << endl;
	// Switch so we read our input from a file.
	// Means I don't have to set up a command line in visual studio to
	// run with the debugger
	streambuf *backup, *psbuf;
	ifstream filestr;
	filestr.open("test.txt");
	backup = cin.rdbuf();
	psbuf = filestr.rdbuf();
	cin.rdbuf(psbuf);
#endif


	ReadTestCases();



#ifdef DEBUG_FILE
	// Put our input stream back to normal
	// Not terribly important if we're about to exit anyway!
	cin.rdbuf(backup);
	filestr.close();
#endif
	return 0;
}
