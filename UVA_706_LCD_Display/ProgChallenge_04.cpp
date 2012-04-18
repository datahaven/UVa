/*
 * ProgChallenge_04.cpp
 *
 *  Created on: 5 Nov 2011
 *      Author: Adrian Dale
 *
 *  Chapter 1.6.4 LCD Display
 *  From Programming Challenges by Miguel Skiena
 *
 *  UVA Online Judge Ref: 706
 *
 * Your best accepted try
 * Ranking 	Submission 	Run Time 	Language 	Submission Date
 * 1424 	9444358 	0.060 	C++ 	2011-11-05 17:47:41
 *
 * Took two goes! Damn!
 * Missed that my convert digits loop wasn't working when passed n==0
 * If I'd put that in separate fn (and tested it properly) that wouldn't
 * have happened.
 */
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
using namespace std;

char Segments[10][7] = {
		{1,1,1,0,1,1,1}, // 0
		{0,0,1,0,0,1,0}, // 1
		{1,0,1,1,1,0,1}, // 2
		{1,0,1,1,0,1,1}, // 3
		{0,1,1,1,0,1,0}, // 4
		{1,1,0,1,0,1,1}, // 5
		{1,1,0,1,1,1,1}, // 6
		{1,0,1,0,0,1,0}, // 7
		{1,1,1,1,1,1,1}, // 8
		{1,1,1,1,0,1,1}  // 9
};

// Could do with breaking out the two repeated Horiz and Vertical
// chunks but seems to work anyway
void OutputDigits(int s, int n)
{
	// Break the number up into digits
	// and store in easy to access format
	vector<int> Digits;

	// Evil bug! That the below assertion will now catch
	// Wasn't counting 0 as a digit when n == 0
	if (n==0)
		Digits.push_back(0);

	while(n != 0)
	{
		Digits.push_back(n%10);
		n /= 10;
	}

	assert(Digits.size() > 0);

	// Because the above decodes the digits back to front
	reverse(Digits.begin(), Digits.end());

	string horizSegOn(s, '-');
	horizSegOn = " " + horizSegOn + " ";
	string horizSegOff(s+2, ' ');

	string vert1SegOn(s/2, ' '), vert1SegOff(s/2, ' ');
	vert1SegOn = "|" + vert1SegOn;
	vert1SegOff = " " + vert1SegOff;

	string vert2SegOn(s/2, ' '), vert2SegOff(s/2, ' ');
	vert2SegOn = vert2SegOn + "|";
	vert2SegOff = vert2SegOff + " ";
	if (s%2==1)
	{
		vert2SegOn = " " + vert2SegOn;
		vert2SegOff = " " + vert2SegOff;
	}

	// Top row of horizontal segments
	for(size_t i=0; i<Digits.size(); ++i)
	{
		cout << (Segments[Digits[i]][0] ? horizSegOn : horizSegOff);
		// Column between digits but not after final digit on line
		if (i < Digits.size()-1)
			cout << " ";
	}
	cout << endl;

	// Upper vertical segments
	for(int j=0; j<s; ++j)
	{
		for(size_t i=0; i<Digits.size(); ++i)
		{
			cout << (Segments[Digits[i]][1] ? vert1SegOn : vert1SegOff);
			cout << (Segments[Digits[i]][2] ? vert2SegOn : vert2SegOff);
			// Column between digits but not after final digit on line
			if (i < Digits.size()-1)
				cout << " ";
		}
		cout << endl;
	}

	// Middle row of horizontal segments
	for(size_t i=0; i<Digits.size(); ++i)
	{
		cout << (Segments[Digits[i]][3] ? horizSegOn : horizSegOff);
		// Column between digits but not after final digit on line
		if (i < Digits.size()-1)
			cout << " ";
	}
	cout << endl;

	// Lower vertical segments
	for(int j=0; j<s; ++j)
	{
		for(size_t i=0; i<Digits.size(); ++i)
		{
			cout << (Segments[Digits[i]][4] ? vert1SegOn : vert1SegOff);
			cout << (Segments[Digits[i]][5] ? vert2SegOn : vert2SegOff);
			// Column between digits but not after final digit on line
			if (i < Digits.size()-1)
				cout << " ";
		}
		cout << endl;
	}

	// Bottom row of horizontal segments
	for(size_t i=0; i<Digits.size(); ++i)
	{
		cout << (Segments[Digits[i]][6] ? horizSegOn : horizSegOff);
		// Column between digits but not after final digit on line
		if (i < Digits.size()-1)
			cout << " ";
	}
	cout << endl;

}

void Run()
{
	while(true)
	{
		int s, n;
		cin >> s >> n;

		// Input pre-conditions
		assert(s>=0 && s<=10);
		assert((s==0 && n!=0)==false);
		assert(n>=0 && n<=99999999);

		if ( s==0 && n==0 )
			break;

		OutputDigits(s, n);

		// Output a blank line after each number
		cout << endl;
	}
}

void Init()
{
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
