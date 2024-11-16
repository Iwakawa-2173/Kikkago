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
#include <algorithm> // Для std::find_if
#include <cstdlib>  // Для функции rand() и srand()
#include <ctime>    // Для функции time()

using namespace std;

// Объявление глобального массива ленты
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
int inaddr = 0;
int cycles = 0; // Сколько прошло циклов программы
// Метки, номера строк, блоки
std::map<std::string, int> labels; // Хранилище меток
std::map<std::string, int> blocks; // Хранилище блоков
std::map<std::string, int> dos; // Хранилище строк исполнения
std::map<std::string, bool> inblocks; // Хранилище переменных "находимся ли мы в блоке"
bool watchblock = false;

int lineNumber = 0;  // Номер текущей обрабатываемой строки программы


// Функции логического отрицания и тавтологии
int _negation(int input) {return !input;}

int tautology(int input) {return input;}

// Функции изменителя состояния ячейки
void henkamono(int input_tape, int f1, int f2, int f3, int cyc, int conf1, int conf2, int prob, int output_tape) {
    int input = tape[input_tape];
	int output1; 
	int output2; 
	int output3;
	int negation_value;
	int not_neg_v;

    /*
	00 - копирует из одной ячейки в другую.
	01 - сначала прогоняются данные, потом процессы меняются.
	10 - сначала меняются процессы, потом прогоняются данные.
	11 - сначала идёт процесс, потом изменение, потом данные во второй...
	*/

	if (prob == 100) {
		negation_value = 1;
	} else if (prob == 0) {
		negation_value = 0;
	}
	not_neg_v = !negation_value;
	
	// Изначальные функции
    vector<int> functions = {f1, f2, f3};
	
    vector<vector<int>> history; // Для хранения истории состояний

	if ((conf1 != 0) && (conf2 != 0)) {
		output1 = f1;
		output2 = f2;
		output3 = f3;
	}
	
    while (true) {
		
		if (prob > 0 && prob < 100) {
			// Генерация случайного числа от 0 до 99
			int randomNumber = std::rand() % 100;
			if (randomNumber < prob) {negation_value = 1;} 
			else {negation_value = 0;}
			not_neg_v = !negation_value;
		}
		
		if ((conf1 == 0) || (conf2 == 0)) {
			tape[output_tape] = tape[input_tape];
		} else if ((conf1 == 0) || (conf2 == 1)) {
			// Запоминаем выходы функций
			output1 = (functions[0] == negation_value) ? _negation(input) : tautology(input);
			output2 = (functions[1] == negation_value) ? _negation(output1) : tautology(output1);
			output3 = (functions[2] == negation_value) ? _negation(output2) : tautology(output2);

			// Записываем текущие выходные значения
			tape[f1] = output1;
			tape[f2] = output2;
			tape[f3] = output3;

			// Изменяем процессы на основе выходных значений
			if (output3 == negation_value) {
				functions[1] = (functions[1] == not_neg_v) ? negation_value : not_neg_v; // Тавтология <-> отрицание
			}
			if (output2 == negation_value) {
				functions[0] = (functions[0] == not_neg_v) ? negation_value : not_neg_v; // Тавтология <-> отрицание
			}
			if (output1 == negation_value) {
				functions[2] = (functions[2] == not_neg_v) ? negation_value : not_neg_v; // Тавтология <-> отрицание
			}
		} else if ((conf1 == 1) || (conf2 == 0)) {
			if (output3 == negation_value) {
				functions[1] = (functions[1] == not_neg_v) ? negation_value : not_neg_v; // Тавтология <-> отрицание
			}
			if (output2 == negation_value) {
				functions[0] = (functions[0] == not_neg_v) ? negation_value : not_neg_v; // Тавтология <-> отрицание
			}
			if (output1 == negation_value) {
				functions[2] = (functions[2] == not_neg_v) ? negation_value : not_neg_v; // Тавтология <-> отрицание
			}
			output1 = (functions[0] == negation_value) ? _negation(input) : tautology(input);
			output2 = (functions[1] == negation_value) ? _negation(output1) : tautology(output1);
			output3 = (functions[2] == negation_value) ? _negation(output2) : tautology(output2);
			
			// Записываем текущие выходные значения
			tape[f1] = output1;
			tape[f2] = output2;
			tape[f3] = output3;
			
		} else if ((conf1 == 1) || (conf2 == 1)) {
			output1 = (functions[0] == negation_value) ? _negation(input) : tautology(input);
			if (output3 == negation_value) {
				functions[1] = (functions[1] == not_neg_v) ? negation_value : not_neg_v; // Тавтология <-> отрицание
			}
			output2 = (functions[1] == negation_value) ? _negation(output1) : tautology(output1);
			if (output2 == negation_value) {
				functions[0] = (functions[0] == not_neg_v) ? negation_value : not_neg_v; // Тавтология <-> отрицание
			}
			output3 = (functions[2] == negation_value) ? _negation(output2) : tautology(output2);
			if (output1 == negation_value) {
				functions[2] = (functions[2] == not_neg_v) ? negation_value : not_neg_v; // Тавтология <-> отрицание
			}
			
			// Записываем текущие выходные значения
			tape[f1] = output1;
			tape[f2] = output2;
			tape[f3] = output3;
			
		}
		
        // Запоминаем состояние функций и входного значения
        vector<int> currentState = {functions[0], functions[1], functions[2], input};

        // Проверяем, было ли это состояние ранее
        auto it = std::find_if(history.begin(), history.end(), [&](const vector<int>& state) {return state == currentState;});

        if (it != history.end()) {break;}

        history.push_back(currentState); // Запоминаем текущее состояние

        // Устанавливаем новое входное значение для следующей итерации
        if (cyc == 1) {input = output3;}
		
		tape[output_tape] = output3;
    }

}

void printtape(int a1, int a2){
	std::vector<int> tapeprint;
	for (int i = a1; i <= a2; i++) {
		tapeprint.push_back(tape[i]);
	}
	// Вывод значений на печать
    for (const auto& value : tapeprint) {
        std::cerr << value << " ";
    }
    std::cerr << std::endl;
}

int interpretline(string progline) {
	
	// Инициализация генератора случайных чисел
	std::srand(static_cast<unsigned int>(std::time(0)));
	
	std::istringstream iss(progline);
    std::string operation, label, block;
	iss >> operation;
	int a1 = 0;
	int a2 = 0;
	// Извлекаем операцию, переменную и значение и выполняем операцию в зависимости от команды, всё в зависимости от типа переменной
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
					if (tape[addr] == tape[a2]) {lineNumber += 0;} 
					else {lineNumber += 1;}
				} else if ((a1 != -1) && (a2 == -1)) {
					if (tape[a1] == tape[addr]) {lineNumber += 0;}
					else {lineNumber += 1;}
				} else if ((a1 == -1) && (a2 == -1)) {
					if (tape[addr] == tape[addr]) {lineNumber += 0;}
					else {lineNumber += 1;}
				} else {
					if (tape[a1] == tape[a2]) {lineNumber += 0;}
					else {lineNumber += 1;}
				}
			} else if (operation == "conf") {
				if ((a1 == -1) && (a2 != -1)) {
					conf1 = tape[addr];
					conf2 = tape[a2];
				} else if ((a1 != -1) && (a2 == -1)) {
					conf1 = tape[a1];
					conf2 = tape[addr];
				} else if ((a1 == -1) && (a2 == -1)) {
					conf1 = tape[addr];
					conf2 = tape[addr];
				} else {
					conf1 = tape[a1];
					conf2 = tape[a2];
				}
			} else if (operation == "kaku") {
				if ((a1 == -1) && (a2 != -1)) {
					printtape(addr, a2);
				} else if ((a1 != -1) && (a2 == -1)) {
					printtape(a1, addr);
				} else if ((a1 == -1) && (a2 == -1)) {
					printtape(addr, addr);
				} else {
					printtape(a1, a2);
				}
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
				if (a1 < 0) {
					addr = 257 - std::abs(a1);
					// Если addr меньше 0, то применяем ту же логику, что и для -258 и ниже
					while (addr < 0) {addr = 257 - std::abs(addr);}
				} else {addr = a1 % 257;}
			} else if (operation == "addrwokaku") {std::cerr << addr << std::endl;}
			else if (operation == "mojiwokaku") {std::cerr << char(addr);}
			else if (operation == "goto") {
				if (a1 == -1) {lineNumber = addr - 1;}
				else {lineNumber = a1 - 1;}
			} else if (operation == ">") {
				if (addr == 256) {addr = 0;}
				else {addr++;}
			} else if (operation == "<") {
				if (addr == 0) {addr = 256;}
				else {addr--;}
			}
			else if (operation == "owari") {return 2;}
			else if (operation == "prob") {
				if (a1 == -1) {prob = addr % 101;}
				else {prob = std::abs(a1) % 101;}
			}
			else if (operation == ";") {lineNumber += 0;}
			else if (operation == "") {lineNumber += 0;}
			else if (operation == "kyouki") {lineNumber = std::rand() % (lineNumber+1);}
			else if (operation == "hajimaru") {lineNumber = std::rand() % (lineNumber+1);}
			else if (operation == "inaddr") {
				std::cin >> inaddr;
				if (inaddr < 0) {
					addr = 257 - std::abs(inaddr);
					// Если addr меньше 0, то применяем ту же логику, что и для -258 и ниже
					while (addr < 0) {addr = 257 - std::abs(addr);}
				} else {addr = inaddr % 257;}
			}
			else if (operation == "loop") {
				if (cycles > 256) {addr = cycles % 257;}
				else {addr = cycles;}
			}
			else {return 1;}
		}
	} else {
		if (operation == "break") {watchblock = false;}
	}

	return 0;
}

// Основная функця процессора, интерпретирующая ассемблер
int main(int argc, char* argv[]) {

	// Проверяем, был ли передан файл
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
		std::cerr << "IBN-KIKKA-24 - First Temporal Processor" << std::endl;
		std::cerr << "Version 1.0.0 Release" << std::endl;
        return 1;
    }

    // Получаем имя файла из аргументов командной строки
    std::string filename = argv[1];
	std::cerr << "IBN-KIKKA-24 - First Temporal Processor" << std::endl;
	std::cerr << "Version 1.0.0 Release" << std::endl;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "File not found." << std::endl;
        return 1;
    }

    std::vector<std::string> program;
    std::string line;
	int lineNum = 0; // Для номера строки
	int hajimaru = 0;

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

    lineNumber = hajimaru;
	int next_lineNumber = lineNumber + 1;

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