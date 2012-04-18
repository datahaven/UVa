/*
 * UVA_10003_Cutting_Sticks.cpp
 *
 *  Created on: 27 Nov 2011
 *      Author: Adrian Dale
 *
 * Your best accepted try
 * Ranking Submission Run Time Language Submission Date
 *   2518    9511415   1.124      C++   2011-11-27 13:41:15
 *
 * Not the fastest bit of code - I think there is a better algorithm
 * for doing this that runs bottom-up.
 * A bit surprised how much difference removing the memset made.
 * Would be neater if I didn't waste so much space in my Memo, too,
 * but doesn't really affect run time
 * I also wonder if there is an even more efficient algorithm based on
 * lengths of the cuts, that doesn't worry about the cut positions
 * like this one does.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <utility>
#include <cstring> // For memset
#include <assert.h>
#ifndef ONLINE_JUDGE
#include "Timer.hpp"
#endif
using namespace std;

unsigned int StickLength;
vector<int> Cuts;

unsigned int MAXMEMOVAL = 0xFFFFFFFF;

unsigned int Memo[1000][1000];

// Minimum cost of cutting a section of stick
// between start and finish
unsigned int MinStickCost(int start, int finish)
{
	// Use a reference so that our code automatically updates
	// our memo.
	unsigned int &msc = Memo[start][finish];

	if (msc != MAXMEMOVAL)
		return msc;

	for(unsigned int cp=0; cp<Cuts.size(); ++cp)
	{
		// Try each cut in turn and see if it falls in the section
		// of stick we are interested in
		if (Cuts[cp] <= start || Cuts[cp] >= finish)
			continue;

		// See if a cut at this position beats our minimum so far
		unsigned int mscL = MinStickCost(start, Cuts[cp]);
		unsigned int mscR = MinStickCost(Cuts[cp], finish);
		msc = min(msc, finish - start + mscL + mscR);
	}

	// ie no cut can be made, hence no cost
	if (msc == MAXMEMOVAL)
		msc = 0;

	return msc;
}

void SolveTestCase()
{
	// Turns out this is WAY too slow. It is a 1000 * 1000 array,
	// after all.
	//memset(Memo, MAXMEMOVAL & 0xFF, sizeof(Memo));

	// We really only need entries for each of the possible stick
	// lengths, so there's a lot of wasted space in our 1000*1000 memo.
	// It could be only 51 *51, so the code below just initialises the bits
	// we need. Looks more complicated, but runs faster.
	Memo[0][StickLength] = MAXMEMOVAL;
	for(unsigned int i=0; i<Cuts.size(); ++i)
	{
		Memo[0][Cuts[i]] = MAXMEMOVAL;
		Memo[Cuts[i]][StickLength] = MAXMEMOVAL;
		for(unsigned int j=0; j<Cuts.size(); ++j)
			Memo[Cuts[i]][Cuts[j]] = MAXMEMOVAL;
	}

	int msc = MinStickCost(0, StickLength);
	cout << "The minimum cutting is " << msc << "." << endl;
}

void ReadTestCases()
{
#ifndef ONLINE_JUDGE
		TimedSection clock("Run Time");
#endif

	Cuts.reserve(50);

	while(true)
	{
		string line;
		getline(cin, line);
		istringstream Parser(line);
		Parser >> StickLength;
		if (StickLength == 0)
			break;

		getline(cin, line);
		Parser.str(line);
		Parser.clear();

		int nCuts;
		Parser >> nCuts;

		assert(StickLength >= 0 && StickLength < 1000);
		assert(nCuts >= 0 && nCuts < 50);

		Cuts.clear();

		getline(cin, line);
		Parser.str(line);
		Parser.clear();

		while(nCuts-- > 0)
		{
			unsigned int cutPos;
			Parser >> cutPos;

			assert(cutPos >= 0 && cutPos <= StickLength);
			Cuts.push_back(cutPos);
		}

		SolveTestCase();
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
