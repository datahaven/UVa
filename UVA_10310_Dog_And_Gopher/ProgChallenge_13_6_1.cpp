/*
 * ProgChallenge_13_6_1.cpp
 *
 *  Created on: 6th Nov 2011
 *      Author: Adrian Dale
 *
 *  Chapter 13.6.1 Dog And Gopher
 *  From Programming Challenges by Miguel Skiena
 *
 *  UVA Online Judge Ref: 10310
 *
 * Solved correctly at first attempt
 *
 * Total Submissions 	Users that tried it 	Users that solved it
 * 13763			 	3361				 	2755
 * Your best accepted try
 * Ranking 	Submission 	Run Time 	Language 	Submission Date
 * 822	 	9446438 	0.024 		C++ 	2011-11-06 12:26:19
 */
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <cmath>
#include <assert.h>
using namespace std;

#define ONLINE_JUDGE 1

const double E = 0.0000001;

inline double Distance(double p1x, double p1y, double p2x, double p2y)
{
	double dx2 = (p2x-p1x) * (p2x-p1x);
	double dy2 = (p2y-p1y) * (p2y-p1y);
	assert(dx2+dy2 >= 0.0);
	return sqrt( dx2 + dy2 );
}

string FormatCoord(const double x, const double y)
{
	ostringstream oss;
	oss << "(";
	oss << fixed << setprecision(3) << x << "," << y << ")";
	return oss.str();
}

string RunTest(double gopx, double gopy, double dogx, double dogy,
			vector< pair<double, double> > &Holes )
{
	for( size_t i=0; i<Holes.size(); ++i)
	{
		double gop_to_hole = Distance(gopx, gopy, Holes[i].first, Holes[i].second);
		double dog_to_hole = Distance(dogx, dogy, Holes[i].first, Holes[i].second);
		// Gopher escapes if both reach hole at same time
		if ( gop_to_hole - dog_to_hole/2.0 < E)
		{
			ostringstream oss;
			oss << "The gopher can escape through the hole at ";
			oss << FormatCoord(Holes[i].first, Holes[i].second) << ".";
			return oss.str();
		}
	}
	return string("The gopher cannot escape.");
}

void Run()
{
	string line;
	while( getline(cin, line) )
	{
		int holes;
		double gopherx, gophery, dogx, dogy;

		istringstream parser(line);
		parser >> holes >> gopherx >> gophery >> dogx >> dogy;
#ifndef ONLINE_JUDGE
		cout << "Holes=" << holes << " g=(" << gopherx << "," << gophery << ") "
				<< "d=(" << dogx << "," << dogy << ")" << endl;
#endif

		vector<pair<double,double> > Holes;

		while( holes-- > 0)
		{
			getline(cin, line);
			double hx, hy;
			parser.str(line);
			parser.clear();
			parser >> hx >> hy;
			Holes.push_back(make_pair<double,double>(hx,hy));
#ifndef ONLINE_JUDGE
			cout << hx << "," << hy << endl;
#endif
		}

		cout << RunTest(gopherx, gophery, dogx, dogy, Holes) << endl;

		// Blank line between test cases
		getline(cin, line);
	}
}

void Init()
{
}

void SelfTest()
{
	assert(Distance(0.0,0.0,0.0,0.0) < E);
	assert(Distance(3.0,3.0,4.0,4.0) - 5.0 < E);
	assert(Distance(-3.0,-3.0,-4.0,-4.0) - 5.0 < E);
	assert(FormatCoord(0.001, 0.001) == FormatCoord(0.0005, 0.0005));
	assert(FormatCoord(0.001, 0.001) == FormatCoord(0.0006, 0.0006));
	assert(FormatCoord(0.000, 0.000) == FormatCoord(0.0004, 0.0004));
}

int main()
{
//#define DEBUG_FILE 1

#ifdef DEBUG_FILE
	cout << "WARNING: DEBUG BUILD" << endl;
	// Switch so we read our input from a file.
	// Means I don't have to set up a command line in visual studio to
	// run with the debugger
	streambuf *backup, *psbuf;
	ifstream filestr;
	//filestr.open("ancient.in");
	filestr.open("sample.txt");
	backup = cin.rdbuf();
	psbuf = filestr.rdbuf();
	cin.rdbuf(psbuf);
#endif
	Init();
#ifndef ONLINE_JUDGE
	SelfTest();
#endif
	Run();
#ifdef DEBUG_FILE
	// Put our input stream back to normal
	// Not terribly important if we're about to exit anyway!
	cin.rdbuf(backup);
	filestr.close();
#endif
	return 0;
}
