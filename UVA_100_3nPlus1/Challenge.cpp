/*
 * Challenge.cpp
 *
 *  Created on: 4 Nov 2011
 *      Author: Adrian Dale
 *
 *  Chapter 1.6.1 The 3n + 1 Problem
 *  From Programming Challenges by Miguel Skiena
 *
 *  Online Judge Ref:
 *  http://uva.onlinejudge.org/index.php?option=com_onlinejudge&
 *                   Itemid=8&category=29&page=show_problem&problem=36
 *
 * Accepted on first submission, ranked 1284 out of 45926 users who solved it
 *
 * Your best accepted try
 * Ranking 	Submission 	Run Time 	Language 	Submission Date
 * 1284 	9442112 	0.028 	C++ 	2011-11-04 23:42:09
 */
#include <iostream>
#include <cstring>
#include <assert.h>
using namespace std;

int memo[1000000];

int cycleLen(int in)
{
	assert(in>0 && in<1000000);

	unsigned int n = in;

	int cycLen = 1;
	while( n != 1 )
	{
		if (n<1000000 && memo[n] != 0)
		{
			cycLen += memo[n]-1;
			break;
		}
		else
			++cycLen;

		if (n%2 == 0)
			n /= 2;
		else
			n = 3*n + 1;
	}
	memo[in] = cycLen;
	return memo[in];
}

int MaxCycleLenInRange(int i, int j)
{
	if (j<i)
	{
		int t=i;
		i = j;
		j = t;
	}
	int nmax = 0;
	for(int n=i; n<=j; ++n)
		nmax = max(nmax, cycleLen(n));
	return nmax;
}

void Run()
{
	int i, j;
	while(cin >> i >> j)
	{
		assert(i>0 && i<1000000);
		assert(j>0 && j<1000000);
		cout << i << " " << j << " " << MaxCycleLenInRange(i, j) << endl;
	}
}

void Init()
{
	memset(memo, 0, sizeof(memo));
}
void SelfTest()
{
	assert( cycleLen(22)==16 );
	assert( cycleLen(113383)==248 );
	assert( MaxCycleLenInRange(1,10)==20 );
	assert( MaxCycleLenInRange(100,200)==125 );
	assert( MaxCycleLenInRange(201,210)==89 );
	assert( MaxCycleLenInRange(900,1000)==174 );
	assert( MaxCycleLenInRange(1,10) == MaxCycleLenInRange(10,1));
	assert( MaxCycleLenInRange(22,22) == 16);
}

int main()
{
	Init();
#ifndef ONLINE_JUDGE
	SelfTest();
#endif
	Run();
	return 0;
}

