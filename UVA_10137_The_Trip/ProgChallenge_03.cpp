/*
 * ProgChallenge_03.cpp
 *
 *  Created on: 5 Nov 2011
 *      Author: Adrian Dale
 *
 *  Chapter 1.6.3 The Trip
 *  From Programming Challenges by Miguel Skiena
 *
 *  UVA Online Judge Ref: 10137
 *
 * Ranking 	Submission 	Run Time 	Language 	Submission Date
 * 2655 	9444136 	0.024 	C++ 	2011-11-05 16:10:11
 *
 * Got it on my (shameful!) fourth submission.
 * Spent way too long on this and got messed about by using doubles
 * and also trying to come up with a mathematical answer, without really
 * thinking the algorithm through properly until I was sure it would work.
 * Once I switched to using ints it became easier to visualise (and didn't have
 * weird double representation issues) and I came up with a simple algorithm
 * really quickly.
 * Didn't test for big numbers but did back of envelope check that
 * 1000000 max expense * 1000 students wouldn't overflow.
 */
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <numeric>
#include <functional>
#include <cmath>
#include <algorithm>
#include <assert.h>
using namespace std;

const int S_MAX = 1000;
const double E_MAX = 1000000;

typedef int ExpenseType;

int Solve( vector<ExpenseType> &expenses)
{
	// Can't have zero students
	assert(expenses.size() > 0);

	// This is almost certainly unnecessary
	// The tests pass without it.
	// Actually got a worse submission time without this line, but that's just the
	// judge being messed up - only possible explanation.
	//sort(expenses.begin(), expenses.end(), greater<ExpenseType>());

	// Get the average (mean) expense
	ExpenseType sum = accumulate(expenses.begin(), expenses.end(), 0, plus<ExpenseType>() );

	ExpenseType avg = sum / expenses.size();
	ExpenseType remainder = sum - (avg*expenses.size());

	//cout << "sum=" << sum << " avg=" << avg << " rem=" << remainder << endl;

	// Remaining pennies are shared out amongst the students who spent the
	// most, thus minimising the amount they have to exchange
	int exchanged = 0;
	for(size_t i=0; i<expenses.size(); ++i)
	{
		if (expenses[i] > avg)
		{
			ExpenseType diff = expenses[i]-avg;
			if (remainder > 0)
			{
				--remainder;
				diff -= 1;
			}
			exchanged += diff;
		}
	}
	return exchanged;
}

void Run()
{
	while(true)
	{
		int s;
		cin >> s;

		// Input pre-conditions
		assert(s>=0 && s<=S_MAX);

		if ( s==0 )
			break;

		vector<ExpenseType> expenses;
		for(int i=0; i<s; ++i)
		{
			string line;
			cin >> line;
			ExpenseType expense;

			size_t pointPos = line.find(".");
			if ( pointPos != string::npos)
			{
				line.replace(pointPos, 1, "");
			}
			istringstream lineStr(line);
			lineStr >> expense;

			assert(expense <= E_MAX);
			expenses.push_back(expense);
		}

		ExpenseType result = Solve(expenses);

		cout << "$" << (result / 100) << "."
			<< setw(2) << setfill('0') << (result % 100) << endl;
	}
}

void Init()
{
}

void SelfTest()
{
	vector<ExpenseType> test;
	test.push_back(1000);
	test.push_back(2000);
	test.push_back(3000);
	assert( Solve(test)==1000);

	test.clear();
	test.push_back(1500);
	test.push_back(1501);
	test.push_back(300);
	test.push_back(301);
	assert( 1199 == Solve(test) );

	test.clear();
	test.push_back(1502);
	test.push_back(1501);
	test.push_back(300);
	test.push_back(301);
	assert( 1201 == Solve(test) );

	test.clear();
	test.push_back(1503);
	test.push_back(1501);
	test.push_back(300);
	test.push_back(301);
	assert( 1201 == Solve(test) );

	test.clear();
	test.push_back(1000);
	// NB Nothing needs to be exchanged
	assert( 0 == Solve(test) );

	test.clear();
	test.push_back(1000);
	test.push_back(1000);
	test.push_back(1000);
	test.push_back(1000);
	// NB Nothing needs to be exchanged
	assert( 0 == Solve(test) );

	test.clear();
	test.push_back(1000);
	test.push_back(1000);
	test.push_back(1000);
	test.push_back(999);
	// NB Nothing needs to be exchanged
	assert( 0 == Solve(test) );

	test.clear();
	test.push_back(1000);
	test.push_back(1000);
	test.push_back(1000);
	test.push_back(1001);
	// NB Nothing needs to be exchanged
	assert( 0 == Solve(test) );
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
