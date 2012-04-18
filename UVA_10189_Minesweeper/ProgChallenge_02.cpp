/*
 * ProgChallenge_02.cpp
 *
 *  Created on: 5 Nov 2011
 *      Author: Adrian Dale
 *
 *  Chapter 1.6.2 Minesweeper
 *  From Programming Challenges by Miguel Skiena
 *
 *  UVA Online Judge Ref: 10189
 *
 * Your best accepted try
 * Ranking 	Submission 	Run Time 	Language 	Submission Date
 * 5823 	9443475 	0.012 	C++ 	2011-11-05 12:25:26
 *
 * Failed on first attempt because I was outputting a blank line
 * after every test, whereas there shouldn't have been one after
 * the final test. Sneaky!
 */
#include <iostream>
#include <string>
#include <cstring>
#include <assert.h>
using namespace std;

const int M_MAX = 100;
const int N_MAX = 100;

char board[N_MAX+2][M_MAX+2];

void ReadTestCase(int im, int in)
{
	// Re-write the parts of the border that get overwritten
	for( int n=0; n<in+2; ++n)
		board[n][im+1] = '.';
	for( int m=0; m<im+2; ++m)
		board[in+1][m] = '.';

	// Read in input and store it
	for( int n=0; n<in; ++n)
	{
		string line;
		cin >> line;
		// Pre-condition that input only contains
		// input chars and end of line char
		assert(line.size() == im);
		for( int m=0; m<im; ++m )
		{
			// Start inside the top corner to leave the
			// border intact
			board[n+1][m+1] = line[m];
		}
	}
}

inline int NeighbourCount(int in, int im)
{
	int DX[] = {0, 1, 1,  1,  0, -1, -1, -1};
	int DY[] = {1, 1, 0, -1, -1, -1,  0,  1};
	assert(sizeof(DX)==8*sizeof(int));
	assert(sizeof(DY)==8*sizeof(int));
	int nc = 0;

	for(size_t d=0; d<sizeof(DX)/sizeof(int); ++d)
	{
		assert(in+DY[d] < N_MAX+2);
		assert(im+DX[d] < M_MAX+2);
		if (board[in + DY[d]][im + DX[d]]=='*')
			++nc;
	}
	return nc;
}

void OutputResults(int im, int in)
{
	// Count neighbours for every square inside the border
	for(int n=1; n<in+1; ++n)
	{
		for(int m=1; m<im+1; ++m)
		{
			if ( board[n][m] == '*')
				cout << '*';
			else
			{
				int nc = NeighbourCount(n, m);
				assert(nc>=0 && nc <=8);
				cout << nc;
			}
		}
		cout << endl;
	}
}

void Run()
{
	int TestNo = 1;
	while(true)
	{
		int n, m;
		cin >> n >> m;

		// Input pre-conditions
		assert(m>=0 && m<=100);
		assert(n>=0 && n<=100);
		// 0,0 for end of input
		// Can't have one of n,m ==0 and not the other
		assert( (n==0 && m==0) || (n!=0 && m!=0));

		if ( n==0 && m==0 )
			break;

		// Slightly weird positioning so we don't print
		// a line feed after the last test, which, quite frankly
		// was rather an annoying way to get a Wrong Answer
		if (TestNo > 1)
			cout << endl;

		cout << "Field #" << TestNo << ":" << endl;
		ReadTestCase(m, n);
		OutputResults(m, n);
		++TestNo;
	}
}

void Init()
{
	memset(board, '.', sizeof(board));
}

void SelfTest()
{

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
