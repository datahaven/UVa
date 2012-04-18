/*
 * ProgChallenge_05.cpp
 *
 *  Created on: 5 Nov 2011
 *      Author: Adrian Dale
 *
 *  Chapter 1.6.5 Graphical Editor
 *  From Programming Challenges by Miguel Skiena
 *
 *  UVA Online Judge Ref: 10267
 *
 * Ranking 	Submission 	Run Time 	Language 	Submission Date
 * 1000 	9444802 	0.088 	C++ 	2011-11-05 21:06:51
 *
 * Third time lucky!
 *
 * First failed due to Time Limit Exceeded - an infinite loop when
 * filling a colour over the top of itself. Odd, 'cos I thought I'd tested
 * that but my case didn't trigger the bug properly
 *
 * Second failed because H and V params were allowed to be back to front,
 * which I'd been fooled about because the rectangle params are not
 */
#include <iostream>
#include <string>
#include <sstream>
#include <assert.h>
#include <set>
#include <utility>
using namespace std;

const int N_MAX = 250;
const int M_MAX = 250;

// Room for a border, too
// May not be able to use this method, though, depending on what happens
// when image is resized
char Pixels[N_MAX+2][M_MAX+2];
int N = 0;
int M = 0;

void ClearImage()
{
	for(int n=1; n<=N; ++n)
	{
		for(int m=1; m<=M; ++m)
		{
			Pixels[n][m] = 'O';
		}
	}
}

void CreateNewImage(string &line)
{
	istringstream parser(line);
	string cmd;
	parser >> cmd >> M >> N;

	ClearImage();
}

void ColourPixel(string &line)
{
	istringstream parser(line);
	string cmd;
	int x, y;
	char c;
	parser >> cmd >> x >> y >> c;
	Pixels[y][x] = c;
}

void VerticalSegment(string &line)
{
	istringstream parser(line);
	string cmd;
	int x, y1, y2;
	char c;
	parser >> cmd >> x >> y1 >> y2 >> c;
	if (y2 < y1)
	{
		int t=y1;
		y1 = y2;
		y2 = t;
	}
	for(int n=y1; n<=y2; ++n)
		Pixels[n][x] = c;
}

void HorizontalSegment(string &line)
{
	istringstream parser(line);
	string cmd;
	int x1, x2, y;
	char c;
	parser >> cmd >> x1 >> x2 >> y >> c;
	if (x2 < x1)
	{
		int t=x1;
		x1 = x2;
		x2 = t;
	}
	for(int m=x1; m<=x2; ++m)
		Pixels[y][m] = c;
}

void FilledRectangle(string &line)
{
	istringstream parser(line);
	string cmd;
	int x1, y1, x2, y2;
	char c;
	parser >> cmd >> x1 >> y1 >> x2 >> y2 >> c;
	for(int n=y1; n<=y2; ++n)
		for(int m=x1; m<=x2; ++m)
			Pixels[n][m] = c;
}

void FillRegion(string &line)
{
	istringstream parser(line);
	string cmd;
	int x, y;
	char c;
	parser >> cmd >> x >> y >> c;

	// Drop a border round our region
	for(int n=0; n<=N+1; ++n)
	{
		Pixels[n][0] = 'a';
		Pixels[n][M+1] = 'a';
	}
	for(int m=0; m<=M+1; ++m)
	{
		Pixels[0][m] = 'a';
		Pixels[N+1][m] = 'a';
	}

	int DX[] = {0,1,0,-1};
	int DY[] = {1,0,-1,0};
	set<pair<int,int> > toVisit;

	toVisit.insert( make_pair<int,int>(y,x));
	char checkCol = Pixels[y][x];

	while(toVisit.empty() == false)
	{
		// Pop top square off set
		pair<int,int> coord = *toVisit.begin();
		toVisit.erase(toVisit.begin());
		if (Pixels[coord.first][coord.second]==checkCol)
		{
			Pixels[coord.first][coord.second] = 'b';
			for(size_t d=0; d<sizeof(DX)/sizeof(int); ++d)
			{
				if (Pixels[coord.first+DY[d]][coord.second+DX[d]] == checkCol)
				{
					toVisit.insert( make_pair<int,int>(coord.first+DY[d], coord.second+DX[d]));
				}
			}
		}
	}

	// Replace our temp field
	for(int n=1; n<=N; ++n)
		for(int m=1; m<=M; ++m)
			if (Pixels[n][m]=='b')
				Pixels[n][m]=c;
}

void OutputImage(string &line)
{
	istringstream parser(line);
	string cmd, filename;
	parser >> cmd >> filename;
	cout << filename << endl;
	for(int n=1; n<=N; ++n)
	{
		for(int m=1; m<=M; ++m)
		{
			cout << Pixels[n][m];
		}
		cout << endl;
	}
}

void Run()
{
	bool quit = false;
	while(quit == false)
	{
		string line;
		getline(cin, line);

		// Behaviour is undefined for blank lines
		// but this saves us crashing when we try to get the first char
		if (line.empty()==true)
			continue;

		char command = line[0];
		switch(command)
		{
		case('I'):
			CreateNewImage(line);
			break;
		case('C'):
			ClearImage();
			break;
		case('L'):
			ColourPixel(line);
			break;
		case('V'):
			VerticalSegment(line);
			break;
		case('H'):
			HorizontalSegment(line);
			break;
		case('K'):
			FilledRectangle(line);
			break;
		case('F'):
			FillRegion(line);
			break;
		case('S'):
			OutputImage(line);
			break;
		case('X'):
			quit=true;
			break;
		default:
			break;
		}
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
