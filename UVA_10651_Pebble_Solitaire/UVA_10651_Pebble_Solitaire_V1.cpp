// UVa 10651 - Pebble Solitaire
// Adrian Dale 25/09/2012
// http://uva.onlinejudge.org/index.php?option=com_onlinejudge&Itemid=8&category=437&page=show_problem&problem=1592
//
// This one turned out really easy - I think the limits are just too low:
// 10649556 	10651 	Pebble Solitaire 	Accepted 	C++ 	0.008 	2012-09-25 11:42:08
// I'm ranked 848th out of 1176 solvers. However, I knew my first cut wasn't great as it does
// a lot of converting from bits to string. I could speed that up by doing everything with
// bits. Or, for even more fun, create a hardcoded lookup table in advance and use that instead!
// (I also think that because it is an old puzzle that all the top times are recorded as 0.000,
// which could mean they are anything)
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cstring>
using namespace std;

int StringToBits(string &s)
{
	int result = 0;
	for(unsigned int i=0; i<s.length(); ++i)
		if (s[i] == 'o')
			result |= (1<<i);
	return result;
}

string BitsToString(int b)
{
	string pebs;
	for(unsigned int i=0; i<12; ++i)
		if ( (b&(1<<i)) != 0)
			pebs.push_back('o');
		else
			pebs.push_back('-');
	return pebs;
}

int Memo[4096];

vector<string> AllPossibleMoves(string &pebs)
{
	vector<string> results;

	// Look for all right jumps
	for(int i=0; i<10; ++i)
	{
		string newpebs = pebs;
		if (pebs[i] == 'o' && pebs[i+1]=='o' && pebs[i+2]=='-')
		{
			newpebs[i] = '-';
			newpebs[i+1] = '-';
			newpebs[i+2] = 'o';
			results.push_back(newpebs);
		}
	}
	// Look for all left jumps
	for(int i=0; i<10; ++i)
	{
		string newpebs = pebs;
		if (pebs[i] == '-' && pebs[i+1]=='o' && pebs[i+2]=='o')
		{
			newpebs[i] = 'o';
			newpebs[i+1] = '-';
			newpebs[i+2] = '-';
			results.push_back(newpebs);
		}
	}
	return results;
}

int MinMoves(int pebs)
{
	if (Memo[pebs] != 0)
		return Memo[pebs];
	int result = 0;

	// Get all possible moves from this position
	string pebStr = BitsToString(pebs);
	vector<string> allPoss = AllPossibleMoves( pebStr );
	// if none, then return no of pebs on board
	if (allPoss.size() == 0)
	{
		result = count(pebStr.begin(), pebStr.end(), 'o');
	}
	else
	{
		// Otherwise try all the new positions
		result = 12;
		for(vector<string>::iterator it = allPoss.begin(); it != allPoss.end(); ++it)
		{
			int res = MinMoves( StringToBits(*it ) );
			if (res == 1)
			{
				result = 1;
				break;
			}
			result = min(res, result);
		}
	}

	Memo[pebs] = result;
	return Memo[pebs];
}

void Solve(string &pebbles)
{
	//cout << pebbles << " " << StringToBits(pebbles) << endl;
	//cout << BitsToString( StringToBits(pebbles) ) << endl;
	
	cout << MinMoves( StringToBits(pebbles) ) << endl;
}

void ReadTestCases()
{
	string line;
	getline(cin, line);
	istringstream parser(line);
	int TestCount;
	parser >> TestCount;

	while(TestCount-- > 0)
	{
		getline(cin, line);
		

#ifndef ONLINE_JUDGE
		clock_t starttime = clock();
#endif
		
		Solve(line);

#ifndef ONLINE_JUDGE
		double endtime = (clock() - starttime) / static_cast<double>(CLOCKS_PER_SEC);

		cout << "Time Taken: " << endtime << " seconds" << endl;
#endif
	}
}

void BuildCheatTable()
{
	for(int i=0; i<4096; ++i)
	{
		cout << MinMoves(i) << ",";
		if (i%17 == 0) cout << endl;
	}
	cout << endl;
}

int main()
{
#ifndef ONLINE_JUDGE
		clock_t starttime = clock();
#endif
	memset(Memo, 0, sizeof(Memo));
	//ReadTestCases();
	BuildCheatTable();
#ifndef ONLINE_JUDGE
		double endtime = (clock() - starttime) / static_cast<double>(CLOCKS_PER_SEC);

		cout << "Total Time Taken: " << endtime << " seconds" << endl;
#endif
	return 0;
}