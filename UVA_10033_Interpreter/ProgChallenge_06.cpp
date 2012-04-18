/*
 * ProgChallenge_06.cpp
 *
 *  Created on: 5 Nov 2011
 *      Author: Adrian Dale
 *
 *  Chapter 1.6.6 Interpreter
 *  From Programming Challenges by Miguel Skiena
 *
 *  UVA Online Judge Ref: 10033
 *
 * Total Submissions 	Users that tried it 	Users that solved it
 * 25552                  3809                       2702
 * Your best accepted try
 * Ranking 	Submission 	Run Time 	Language 	Submission Date
 *  2577 	9445019 	0.064 	C++ 	2011-11-05 23:00:39
 *
 *  Finally, I solved one first go!
 *  And a hard one, too, by the looks of the stats. That's an awful
 *  lot of submissions per solver.
 *  (OK, so I also got 3n+1 right first time, as well as Ancient Messages,
 *  which was a World Finals problem. Others were close, but no cigar!)
 *
 *  I think the insight that saved this was that I could comment the
 *  test files. That's a really neat tip, which should help a lot in
 *  other challenges, too. Made it a lot easier to work out test cases
 */
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <assert.h>
using namespace std;

#define ONLINE_JUDGE 1

const int RAM_SIZE = 1000;
const int REGISTER_COUNT = 10;
const int BYTE_SIZE = 1000;

int RAM[RAM_SIZE];
int Registers[REGISTER_COUNT];

// Initialise our computer
void InitTestCase()
{
	memset(RAM, 0, sizeof(RAM));
	memset(Registers, 0, sizeof(Registers));
}

inline void ParseInstruction(int Instruction, int &cmd, int &d1, int &d2)
{
	assert(Instruction >=0 && Instruction < BYTE_SIZE);

	d2 = Instruction % 10;
	Instruction /= 10;
	d1 = Instruction % 10;
	Instruction /= 10;
	cmd = Instruction % 10;
	//cout << "cmd=" << cmd << " d1=" << d1 << " d2=" << d2 << endl;
	assert(cmd >= 0 && cmd <= 9);
	assert(d1 >= 0 && d1 <= 9);
	assert(d2 >= 0 && d2 <= 9);
}

inline void ReduceRegisters()
{
	for(int i=0; i<REGISTER_COUNT; ++i)
		Registers[i] %= BYTE_SIZE;
}

int RunCode()
{
	int InstructionCount = 0;
	bool Running = true;
	int PC = 0; // Program Counter
	while(Running == true)
	{
		++InstructionCount;
		// Check to see if we need a bigger data type
		assert(InstructionCount > 0);

		int cmd, d1, d2;
		ParseInstruction( RAM[PC], cmd, d1, d2 );

		int nextInstruction = PC + 1;

		switch(cmd)
		{
		case(1): // Halt
			// Technically only instruction 100 means Halt, but the
			// spec doesn't say what should happen for 101-199
			Running = false;
			break;
		case(2):
			Registers[d1] = d2;
			break;
		case(3):
			Registers[d1] += d2;
			break;
		case(4):
			Registers[d1] *= d2;
			break;
		case(5):
			Registers[d1] = Registers[d2];
			break;
		case(6):
			Registers[d1] += Registers[d2];
			break;
		case(7):
			Registers[d1] *= Registers[d2];
			break;
		case(8):
			Registers[d1] = RAM[Registers[d2]] % BYTE_SIZE;
			break;
		case(9):
			RAM[Registers[d2]] = Registers[d1] % BYTE_SIZE;
			break;
		case(0):
			if (Registers[d2] != 0)
				nextInstruction = Registers[d1];
			break;
		default:
			break;
		}

		ReduceRegisters();

		// Spec isn't really clear on what happens if we fall off the end, but does
		// say "all results are reduced modulo 1000"
		PC = nextInstruction % BYTE_SIZE;
		assert(PC < RAM_SIZE);
	}
	return InstructionCount;
}

void Run()
{
	string line;
	getline(cin, line);

	int TestCount;
	istringstream tcparser(line);
	tcparser >> TestCount;

	// Remove the starting blank line
	getline(cin, line);

	int TestNo = 1;
	while(TestNo <= TestCount)
	{
		InitTestCase();

		int memLoc=0;

		while(true)
		{
			string line;
			getline(cin, line);

			// Blank line marks end of input.
			if (line.empty()==true)
				break;

			istringstream parser(line);
			int value;
			parser >> value;
			assert(value >= 0 && value <= 999);
			RAM[memLoc++] = value;
		}

		// Need blank line between cases
		if (TestNo != 1)
			cout << endl;

#ifndef ONLINE_JUDGE
		cout << "Test Result:" << endl;
#endif
		cout << RunCode() << endl;
#ifndef ONLINE_JUDGE
		cout << "Registers: ";
		for(int i=0; i<REGISTER_COUNT; ++i)
			cout << Registers[i] << " ";
		cout << endl;
#endif
		++TestNo;
	}
}

void Init()
{
}

void SelfTest()
{
	int cmd, d1, d2;
	ParseInstruction(100, cmd, d1, d2);
	assert(cmd==1 && d1==0 && d2 == 0);

	ParseInstruction(0, cmd, d1, d2);
	assert(cmd==0 && d1==0 && d2==0);

	ParseInstruction(279, cmd, d1, d2);
	assert(cmd==2 && d1==7 && d2==9);

	ParseInstruction(209, cmd, d1, d2);
	assert(cmd==2 && d1==0 && d2==9);
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
