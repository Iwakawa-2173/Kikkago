#include <array>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include "windows.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

/*
Some assembly language commands in
this processor simulation software are
romanized Japanese words.

Author: Iwakawa-2173
License: MIT
*/

using namespace std;

// Declaring a global tape array
std::array<int, 257> tape = {};

int f1 = 0;
int f2 = 0;
int f3 = 0;
int cyc = 0;
int conf1 = 0;
int conf2 = 0;
int prob = 100;
int output = 0;
int addr = 0;
// Labels, line numbers, blocks
std::map<std::string, int> labels;
std::map<std::string, int> blocks;
std::map<std::string, int> dos; // Execution string storage
std::map<std::string, bool> inblocks; // Storage of variables "are we in a block"
bool watchblock = false;
int hajimaru = 0;
int lineNumber = hajimaru;  // Number of the current program line being processed


// Logical negation and tautology functions
int _negation(int input) {return !input;}

int tautology(int input) {return input;}

// Cell state changer functions
void henkamono(int input_tape, int f1, int f2, int f3, int cyc, int conf1, int conf2, int prob, int output_tape) {
    int input = tape[input_tape];
	int output1; 
	int output2; 
	int output3;
	int negation_value;
	int not_neg_v;

    /*
	00 - copies from one cell to another.
	01 - first the data is processed, then the processes are changed.
	10 - first the processes are changed, then the data is processed.
	11 - first there is a process, then a change, then data in the second...
	*/

	if (prob == 100) {
		negation_value = 1;
	} else if (prob == 0) {
		negation_value = 0;
	}
	not_neg_v = !negation_value;
	
	// Initial functions
    vector<int> functions = {f1, f2, f3};
	
    vector<vector<int>> history; // To store state history

	if ((conf1 != 0) && (conf2 != 0)) {
		output1 = f1;
		output2 = f2;
		output3 = f3;
	}
	
	// Initialize the random number generator
	std::srand(static_cast<unsigned int>(std::time(0)));
	
    while (true) {
		
		if (prob > 0 && prob < 100) {
			// Generate a random number from 0 to 99
			int randomNumber = std::rand() % 100;
			if (randomNumber < prob) {negation_value = 1;} 
			else {negation_value = 0;}
			not_neg_v = !negation_value;
		}
		
		if ((conf1 == 0) || (conf2 == 0)) {
			tape[output_tape] = tape[input_tape];
		} else if ((conf1 == 0) || (conf2 == 1)) {
			// Remember function outputs
			output1 = (functions[0] == negation_value) ? _negation(input) : tautology(input);
			output2 = (functions[1] == negation_value) ? _negation(output1) : tautology(output1);
			output3 = (functions[2] == negation_value) ? _negation(output2) : tautology(output2);

			// Write the current output values
			tape[f1] = output1;
			tape[f2] = output2;
			tape[f3] = output3;

			// Modify processes based on output values
			if (output3 == negation_value) {
				functions[1] = (functions[1] == not_neg_v) ? negation_value : not_neg_v; // Tautology <-> negation
			}
			if (output2 == negation_value) {
				functions[0] = (functions[0] == not_neg_v) ? negation_value : not_neg_v; // Tautology <-> negation
			}
			if (output1 == negation_value) {
				functions[2] = (functions[2] == not_neg_v) ? negation_value : not_neg_v; // Tautology <-> negation
			}
		} else if ((conf1 == 1) || (conf2 == 0)) {
			if (output3 == negation_value) {
				functions[1] = (functions[1] == not_neg_v) ? negation_value : not_neg_v; // Tautology <-> negation
			}
			if (output2 == negation_value) {
				functions[0] = (functions[0] == not_neg_v) ? negation_value : not_neg_v; // Tautology <-> negation
			}
			if (output1 == negation_value) {
				functions[2] = (functions[2] == not_neg_v) ? negation_value : not_neg_v; // Tautology <-> negation
			}
			output1 = (functions[0] == negation_value) ? _negation(input) : tautology(input);
			output2 = (functions[1] == negation_value) ? _negation(output1) : tautology(output1);
			output3 = (functions[2] == negation_value) ? _negation(output2) : tautology(output2);
			
			// Write the current output values
			tape[f1] = output1;
			tape[f2] = output2;
			tape[f3] = output3;
			
		} else if ((conf1 == 1) || (conf2 == 1)) {
			output1 = (functions[0] == negation_value) ? _negation(input) : tautology(input);
			if (output3 == negation_value) {
				functions[1] = (functions[1] == not_neg_v) ? negation_value : not_neg_v; // Tautology <-> negation
			}
			output2 = (functions[1] == negation_value) ? _negation(output1) : tautology(output1);
			if (output2 == negation_value) {
				functions[0] = (functions[0] == not_neg_v) ? negation_value : not_neg_v; // Tautology <-> negation
			}
			output3 = (functions[2] == negation_value) ? _negation(output2) : tautology(output2);
			if (output1 == negation_value) {
				functions[2] = (functions[2] == not_neg_v) ? negation_value : not_neg_v; // Tautology <-> negation
			}
			
			// Write the current output values
			tape[f1] = output1;
			tape[f2] = output2;
			tape[f3] = output3;
			
		}
		
        // Remember the state of the functions and the input value
        vector<int> currentState = {functions[0], functions[1], functions[2], input};

        // Check if this state has existed before
        auto it = std::find_if(history.begin(), history.end(), [&](const vector<int>& state) {return state == currentState;});

        if (it != history.end()) {break;}

        history.push_back(currentState); // Remember the current state

        // Set a new input value for the next iteration
        if (cyc == 1) {input = output3;}
		
		tape[output_tape] = output3;
    }

}

void printtape(int a1, int a2){
	std::vector<int> tapeprint;
	for (int i = a1; i <= a2; i++) {
		tapeprint.push_back(tape[i]);
	}
	// Print values
    for (const auto& value : tapeprint) {
        std::cerr << value << " ";
    }
    std::cerr << std::endl;
}

int interpretline(string progline) {
	std::istringstream iss(progline);
    std::string operation, label, block;
	iss >> operation;
	int a1 = 0;
	int a2 = 0;
	// Retrieve the operation, variable and value and perform the operation depending on the command
	if (watchblock == false) {
		if ((operation == "label") || (operation == "to")) {
			iss >> label;
			if (operation == "label") {labels[label] = lineNumber;}
			else if (operation == "to") {lineNumber = labels[label];}
		} else if ((operation == "do") || (operation == "break") || (operation == "block")) {
			iss >> block;
			if (operation == "do") {
				inblocks[block] = true;
				dos[block] = lineNumber;
				lineNumber = blocks[block];
			} else if (operation == "break") {
				if (inblocks[block] == true) {
					lineNumber = dos[block];
					inblocks[block] = false;
				}
			} else if (operation == "block") {
				blocks[block] = lineNumber;
				watchblock = true;
			}
		} else {
			iss >> a1 >> a2;
			if (operation == "ugoku") {
				if ((a1 == -1) && (a2 != -1)) {
					tape[addr] = tape[a2];
				} else if ((a1 != -1) && (a2 == -1)) {
					addr = a1;
					tape[a1] = tape[addr];
				} else if ((a1 == -1) && (a2 == -1)) {
					tape[addr] = tape[addr];
				} else {
					addr = a1;
					tape[a1] = tape[a2];
				}
			} else if (operation == "henkamono") {
				if ((a1 == -1) && (a2 != -1)) {
					henkamono(a2, f1, f2, f3, cyc, conf1, conf2, prob, addr);
				} else if ((a1 != -1) && (a2 == -1)) {
					addr = a1;
					henkamono(addr, f1, f2, f3, cyc, conf1, conf2, prob, a1);
				} else if ((a1 == -1) && (a2 == -1)) {
					henkamono(addr, f1, f2, f3, cyc, conf1, conf2, prob, addr);
				} else {
					addr = a1;
					henkamono(a2, f1, f2, f3, cyc, conf1, conf2, prob, a1);
				}
			} else if (operation == "bunkiten") {
				if ((a1 == -1) && (a2 != -1)) {
					if (tape[addr] == tape[a2]) {
						lineNumber += 0;
					} else {lineNumber += 1;}
				} else if ((a1 != -1) && (a2 == -1)) {
					addr = a1;
					if (tape[a1] == tape[addr]) {
						lineNumber += 0;
					} else {lineNumber += 1;}
				} else if ((a1 == -1) && (a2 == -1)) {
					if (tape[addr] == tape[addr]) {
						lineNumber += 0;
					} else {lineNumber += 1;}
				} else {
					addr = a1;
					if (tape[a1] == tape[a2]) {
						lineNumber += 0;
					} else {lineNumber += 1;}
				}
			} else if (operation == "conf") {
				if ((a1 == -1) && (a2 != -1)) {
					conf1 = tape[addr];
					conf2 = tape[a2];
				} else if ((a1 != -1) && (a2 == -1)) {
					addr = a1;
					conf1 = tape[a1];
					conf2 = tape[addr];
				} else if ((a1 == -1) && (a2 == -1)) {
					conf1 = tape[addr];
					conf2 = tape[addr];
				} else {
					addr = a1;
					conf1 = tape[a1];
					conf2 = tape[a2];
				}
			} else if (operation == "kaku") {
				addr = a2;
				printtape(a1, a2);
			} else if (operation == "cycle") {
				if (a1 == -1) {
					cyc = addr;
				} else {
					cyc = a1;
				}
			} else if (operation == "conf1") {
				if (a1 == -1) {
					conf1 = tape[addr];
				} else {
					conf1 = tape[a1];
				}
			} else if (operation == "conf2") {
				if (a1 == -1) {
					conf2 = tape[addr];
				} else {
					conf2 = tape[a1];
				}
			} else if (operation == "zero") {
				if (a1 == -1) {
					tape[addr] = 0;
				} else {
					tape[a1] = 0;
				}
			} else if (operation == "hitotsu") {
				if (a1 == -1) {
					tape[addr] = 1;
				} else {
					tape[a1] = 1;
				}
			} else if (operation == "f1") {
				if (a1 == -1) {
					f1 = tape[addr];
				} else {
					f1 = tape[a1];
				}
			} else if (operation == "f2") {
				if (a1 == -1) {
					f2 = tape[addr];
				} else {
					f2 = tape[a1];
				}
			} else if (operation == "f3") {
				if (a1 == -1) {
					f3 = tape[addr];
				} else {
					f3 = tape[a1];
				}
			} else if (operation == "addr") {
				if (a1 > 256) {
					addr = a1 - 256 - 1; // To account for -1 -> 256 and so on
				} else if (a1 < 0) {
					addr = 256 + a1 - 1; // To account for 257 -> 0 and so on
				} else {addr = a1;}
			} else if (operation == "addrwokaku") {std::cerr << addr << std::endl;}
			else if (operation == "mojiwokaku") {std::cerr << char(addr);}
			else if (operation == "goto") {
				if (a1 == -1) {
					lineNumber = addr - 1;
				} else { // Since we will then add 1 after the function has completed
					lineNumber = a1 - 1;
				}
			} else if (operation == ">") {
				if (addr == 256) {
					addr = 0; // To account for 257 -> 0 and so on
				} else {addr++;}
			} else if (operation == "<") {
				if (addr == 0) {
					addr = 256; // To account for -1 -> 256 and so on
				} else {addr--;}
			}
			else if (operation == "owari") {return 2;} 
			else if (operation == ";") {lineNumber += 0;}
			else if (operation == "") {lineNumber += 0;}
			else {return 1;}
		}
	} else {
		if (operation == "break") {watchblock = false;}
	}

	return 0;
}

// The main processor function that interprets the assembler
int main(int argc, char* argv[]) {

	// Check if the file was transferred
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
		std::cerr << "IBN-KIKKA-24 - First Strange Processor" << std::endl;
		std::cerr << "Version 0.0.1 Development" << std::endl;
        return 1;
    }

    // Get the file name from the command line arguments
    std::string filename = argv[1];
	std::cerr << "IBN-KIKKA-24 - First Strange Processor" << std::endl;
	std::cerr << "Version 0.0.1 Development" << std::endl;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "File not found." << std::endl;
        return 1;
    }

    std::vector<std::string> program;
    std::string line;
	int lineNum = 0; // For line number

    while (std::getline(file, line)) {
		std::istringstream lstr(line);
		std::string declare, block;

		lstr >> declare;
		if (declare == "block"){
			lstr >> block;
			blocks[block] = lineNum;
		} else if (declare == "hajimaru") {hajimaru = lineNum;}
        program.push_back(line);
		lineNum++;
    }
    file.close();

    
	int next_lineNumber = lineNumber + 1;
	int cycles = 0; // How many program cycles have passed

    while (lineNumber < program.size()) {
		int code = interpretline(program[lineNumber]);
		if (code == 2) {return 0;}
		else if (code == 1) {
			std::cerr << "Unknown operation in line " << lineNumber << std::endl;
			return 1;
		}
        lineNumber++;
		next_lineNumber = lineNumber + 1;
		if (next_lineNumber > program.size()) {
			lineNumber = 0;
			cycles += 1;
		}
    }

    return 0;
}