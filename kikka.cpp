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
// Типы данных одного значения
std::map<std::string, int> vars; // Хранилище переменных типа int 
std::map<std::string, double> dvars; // Хранилище переменных типа double
std::map<std::string, char> cvars; // Хранилище переменных типа char
std::map<std::string, float> fvars; // Хранилище переменных типа float
std::map<std::string, int> alephs; // Хранилище переменных типа aleph
std::map<std::string, bool> bools; // Хранилище переменных типа bool
std::map<std::string, std::string> strings; // Хранилище переменных типа strings
// Массивые целых, не целых и строк
std::map<std::string, std::vector<int>> iarrs; // Хранилище массивов целых чисел
std::map<std::string, std::vector<float>> farrs; // Хранилище массивов float чисел
std::map<std::string, std::vector<double>> darrs; // Хранилище массивов double чисел
std::map<std::string, std::vector<bool>> barrs; // Хранилище массивов bool
std::map<std::string, std::vector<char>> carrs; // Хранилище массивов char
std::map<std::string, std::vector<std::string>> sarrs; // Хранилище массивов string
bool watchblock = false;
bool jikanwomiru = false;
clock_t start;

int lineNumber = 0;  // Номер текущей обрабатываемой строки программы

// Функции логического отрицания и тавтологии
int _negation(int input) {return !input;}

// Степень
double powerab(double a, int b) {return (b == 0) ? 1.0 : (b < 0 ? 1.0 / powerab(a, -b) : a * powerab(a, b - 1));}

// Факториал
int factor(int a) {return (a == 0) ? 1 : a * factor(a-1);}

// Число сочетаний из n по k
int cnk(int n, int k) {return factor(n) / (factor(n - k) * factor(k));}

// Синус
double sinn(double n) {
    double result = 0;
    int sign = 1;
    for (int i = 0; i <= 17; i++) {
        result += sign * powerab(n, 2 * i + 1) / factor(2 * i + 1);
        sign *= -1;
    }
	return result;
}

// Косинус
double cosn(double n) {
	double result = 0;
    int sign = 1;
	for (int i = 0; i <= 17; i++) {
		result += sign * powerab(n, 2 * i) / double(factor(2 * i));
		sign *= -1;
	}
	return result;
}

// e в степени икс
double expn(double n) {
	double result = 0;
	for (int i = 0; i <= 17; i++) {
		result += powerab(n, i) / double(factor(i));
	}
	return result;
}

// Натуральный логарифм 
double lnn(double n) {
	double result = 0;
    for (int i = 1; i <= 17; i++) {
		if (i % 2 == 0) {
			result -= powerab(n - 1, i) / i;
		} else {
			result += powerab(n - 1, i) / i;
		}
	}
	return result;
}

// Среднее арифемтическое
double calculateAverage(const std::string& vectorname) {
    if (darrs[vectorname].empty()) {
        throw std::invalid_argument("Vector is empty. Cannot calculate average.");
    }

    double sum = 0.0;
    for (const double& num : darrs[vectorname]) {
        sum += num;
    }

    return sum / darrs[vectorname].size();
}

// Кумулятивная сумма
double cumsum(const std::string& vectorname) {
    if (darrs[vectorname].empty()) {
        throw std::invalid_argument("Vector is empty. Cannot calculate average.");
    }

    double sum = 0.0;
    for (const double& num : darrs[vectorname]) {
        sum += num;
    }

    return sum;
}

// Дисперсия
double calcdisp(const std::string& vectorname) {
    if (darrs[vectorname].empty()) {
        throw std::invalid_argument("Vector is empty. Cannot calculate average.");
    }
	double mean = calculateAverage(vectorname);
	double n = darrs[vectorname].size();
    double sum = 0.0;
    for (const double& num : darrs[vectorname]) {
        sum += (num - mean)*(num - mean);
    }

    return (1/(n-1))*sum;
}

// Функция для проверки наличия ключа в заданном массиве
bool keyExists(const std::string& mapName, const std::string& key) {
    if (mapName == "vars") {
        return vars.find(key) != vars.end();
    } else if (mapName == "dvars") {
        return dvars.find(key) != dvars.end();
    } else if (mapName == "cvars") {
        return cvars.find(key) != cvars.end();
    } else if (mapName == "fvars") {
        return fvars.find(key) != fvars.end();
    } else if (mapName == "bools") {
        return bools.find(key) != bools.end();
    } else if (mapName == "strings") {
        return strings.find(key) != strings.end();
    } else if (mapName == "iarrs") {
        return iarrs.find(key) != iarrs.end();
    } else if (mapName == "farrs") {
        return farrs.find(key) != farrs.end();
    } else if (mapName == "darrs") {
        return darrs.find(key) != darrs.end();
    } else if (mapName == "carrs") {
        return carrs.find(key) != carrs.end();
    } else if (mapName == "barrs") {
        return barrs.find(key) != barrs.end();
    } else if (mapName == "sarrs") {
        return sarrs.find(key) != sarrs.end();
    } else {
        std::cerr << "Unknown array name: " << mapName << std::endl;
        return false;
    }
}

bool isint(const std::string& key) {return keyExists("vars", key);}
bool isfloat(const std::string& key) {return keyExists("fvars", key);}
bool isdouble(const std::string& key) {return keyExists("dvars", key);}
bool isbool(const std::string& key) {return keyExists("bools", key);}
bool ischar(const std::string& key) {return keyExists("cvars", key);}
bool isstring(const std::string& key) {return keyExists("strings", key);}

bool isiarr(const std::string& key) {return keyExists("iarrs", key);}
bool isfarr(const std::string& key) {return keyExists("farrs", key);}
bool isdarr(const std::string& key) {return keyExists("darrs", key);}
bool isbarr(const std::string& key) {return keyExists("barrs", key);}
bool iscarr(const std::string& key) {return keyExists("carrs", key);}
bool issarr(const std::string& key) {return keyExists("sarrs", key);}

// Не объявлена ли переменная и/или массив
bool isnotdeclared(const std::string& key) {
	if ((!isint(key)) && 
	(!isfloat(key)) && 
	(!isdouble(key)) && 
	(!isbool(key)) && 
	(!ischar(key)) && 
	(!isstring(key)) && 
	(!isiarr(key)) && 
	(!isfarr(key)) && 
	(!isdarr(key)) && 
	(!isbarr(key)) && 
	(!iscarr(key)) && 
	(!issarr(key))) {
		return true;
	} else {return false;}
}

bool isarray(const std::string& key) {
	if ((!isiarr(key)) || 
	(!isfarr(key)) || 
	(!isdarr(key)) || 
	(!isbarr(key)) || 
	(!iscarr(key)) || 
	(!issarr(key))) {
		return true;
	} else {return false;}
}

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

// Квадратный корень
double Sqrt(double x) {
    
    if (x == 0 || x == 1) {
        return x; // Квадратный корень из 0 и 1
    }

    double epsilon = 1e-10; // Точность
    double guess = x / 2.0; // Начальное предположение

    while (fabs(guess * guess - x) >= epsilon) {
        guess = (guess + x / guess) / 2.0; // Обновляем предположение
    }

    return guess;
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

// Функция для генерации случайного числа типа double с равномерным распределением
double RandomUniform(double min, double max) {
    // Генерация случайного числа
    double random = static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX); // Случайное число от 0 до 1
    return min + random * (max - min); // Масштабируем к диапазону [min, max]
}

// Распределение Бернулли
double RandomBernoulli(double p) {
    return RandomUniform(0.0, 1.0) < p ? 1 : 0; // Возвращаем 1 с вероятностью p, иначе 0
}

// Распределение Пуассона
double RandomPoisson(double lambda) {
    double L = expn(-lambda);
    int k = 0;
    double p = 1.0;

    do {
        k++;
        p *= RandomUniform(0.0, 1.0);
    } while (p > L);

    return k - 1; // Возвращаем значение k
}

// Нормальное распределение
double RandomNormal(double mean, double stddev) {
	double pi = 3.14159265358979323846;
    double u1 = RandomUniform(0.0, 1.0); // Первое равномерное случайное число
    double u2 = RandomUniform(0.0, 1.0); // Второе равномерное случайное число
    double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * pi * u2); // Преобразование Бокса-Мюллера
    return z0 * stddev + mean; // Масштабируем к нормальному распределению
}

int interpretline(string progline) {
	
	std::istringstream iss(progline);
    std::string operation, label, block, varname, varname2, type, varname3;
	iss >> operation;
	std::string stv;
	int a1 = 0;
	int a2 = 0;
	int value = 0;
	char cvalue = 0;
	float fvalue = 0;
	double dvalue = 0;
	
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
		} else if ((operation == "ugoku") || 
		(operation == "henkamono") || 
		(operation == "bunkiten") || 
		(operation == "conf") || 
		(operation == "conf1") || 
		(operation == "conf2") || 
		(operation == "kaku") || 
		(operation == "cycle") || 
		(operation == "zero") || 
		(operation == "hitotsu") || 
		(operation == "f1") || 
		(operation == "f2") || 
		(operation == "f3") || 
		(operation == "addr") || 
		(operation == "addrwokaku") || 
		(operation == "mojiwokaku") || 
		(operation == "<-") || 
		(operation == "->") || 
		(operation == "") || 
		(operation == ";") || 
		(operation == "goto") || 
		(operation == "owari") || 
		(operation == "prob") || 
		(operation == "hajimaru") || 
		(operation == "inaddr") || 
		(operation == "loop") || 
		(operation == "kyouki")) {
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
			else if (operation == "hajimaru") {;}
			else if (operation == "goto") {
				if (a1 == -1) {lineNumber = addr - 1;}
				else {lineNumber = a1 - 1;}
			} else if (operation == "->") {
				if (addr == 256) {addr = 0;}
				else {addr++;}
			} else if (operation == "<-") {
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
		} else {
			iss >> varname;
		if (operation == "int") {
			iss >> value;
			if (isnotdeclared(varname)) {
				vars[varname] = value;
			} else return 3;
		} else if (operation == "float") {
			iss >> fvalue;
			if (isnotdeclared(varname)) {
				fvars[varname] = fvalue;
			} else return 3;
		} else if (operation == "double") {
			iss >> dvalue;
			if (isnotdeclared(varname)) {
				dvars[varname] = dvalue;
			} else return 3;
		} else if (operation == "char") {
			iss >> cvalue;
			if (isnotdeclared(varname)) {
				cvars[varname] = cvalue;
			} else return 3;
		} else if (operation == "bool") {
			iss >> value;
			if (isnotdeclared(varname)) {
				bools[varname] = (value == 0) ? 0 : 1;
			} else return 3;
		} else if (operation == "string") {
			iss >> stv;
			if (isnotdeclared(varname)) {
				strings[varname] = stv;
			} else return 3;
		} else if (operation == "array") {
				iss >> type >> varname2;
				if (isnotdeclared(varname)) {
					if (type == "int") {
						if (isint(varname2)) {
							iarrs[varname] = std::vector<int>(vars[varname2]);
						} else if (isfloat(varname2)) {
							iarrs[varname] = std::vector<int>(int(fvars[varname2]));
						} else if (isdouble(varname2)) {
							iarrs[varname] = std::vector<int>(int(dvars[varname2]));
						} else {iarrs[varname] = std::vector<int>(std::stoi(varname2));}
					} else if (type == "float") {
						if (isint(varname2)) {
							farrs[varname] = std::vector<float>(float(vars[varname2]));
						} else if (isfloat(varname2)) {
							farrs[varname] = std::vector<float>(fvars[varname2]);
						} else if (isdouble(varname2)) {
							farrs[varname] = std::vector<float>(float(dvars[varname2]));
						} else {farrs[varname] = std::vector<float>(std::stof(varname2));}
					} else if (type == "double") {
						if (isint(varname2)) {
							darrs[varname] = std::vector<double>(double(vars[varname2]));
						} else if (isfloat(varname2)) {
							darrs[varname] = std::vector<double>(double(fvars[varname2]));
						} else if (isdouble(varname2)) {
							darrs[varname] = std::vector<double>(dvars[varname2]);
						} else {darrs[varname] = std::vector<double>(std::stod(varname2));}
					} else if (type == "bool") {
						if (isint(varname2)) {
							barrs[varname] = std::vector<bool>(bool(vars[varname2]));
						} else if (isfloat(varname2)) {
							barrs[varname] = std::vector<bool>(bool(fvars[varname2]));
						} else if (isdouble(varname2)) {
							barrs[varname] = std::vector<bool>(bool(dvars[varname2]));
						} else {return 5;}
					} else if (type == "char") {
						if (ischar(varname2)) {
							carrs[varname] = std::vector<char>(cvars[varname2]);
						} else {return 5;}
					} else if (type == "string") {
						if (isint(varname2)) {
							sarrs[varname] = std::vector<std::string>{std::to_string(vars[varname2])};
						} else if (isfloat(varname2)) {
							sarrs[varname] = std::vector<std::string>{std::to_string(fvars[varname2])};
						} else if (isdouble(varname2)) {
							sarrs[varname] = std::vector<std::string>{std::to_string(dvars[varname2])};
						} else if (isstring(varname2)) {
							sarrs[varname] = std::vector<std::string>{(strings[varname2])};
						} else {sarrs[varname] = std::vector<std::string>{varname2};}
					}
				} else {return 3;}
			} else if (operation == "print") {
				if (isint(varname)) {
					std::cout << vars[varname] << std::endl;
				} else if (isfloat(varname)) {
					std::cout << fvars[varname] << std::endl;
				} else if (isdouble(varname)) {
					std::cout << dvars[varname] << std::endl;
				} else if (ischar(varname)) {
					std::cout << cvars[varname] << std::endl;
				} else if (isbool(varname)) {
					std::cout << bools[varname] << std::endl;
				} else if (isstring(varname)) {
					std::cout << strings[varname] << std::endl;
				} else if (isarray(varname)) {
					std::cout << "[ ";
					if (isiarr(varname)) {
						for (const auto& str : iarrs[varname]) {
							std::cout << str << " ";
						}
					} else if (isfarr(varname)) {
						for (const auto& str : farrs[varname]) {
							std::cout << str << " ";
						}
					} else if (isdarr(varname)) {
						for (const auto& str : darrs[varname]) {
							std::cout << str << " ";
						}
					} else if (isbarr(varname)) {
						for (const auto& str : barrs[varname]) {
							std::cout << str << " ";
						}
					} else if (iscarr(varname)) {
						for (const auto& str : carrs[varname]) {
							std::cout << str << " ";
						}
					} else if (issarr(varname)) {
						for (const auto& str : sarrs[varname]) {
							std::cout << str << " ";
						}
					}
					std::cout << "]";
					std::cout << std::endl;
				} else {return 5;}
			} else if (operation == "sum") {
				iss >> varname2;
				if (isint(varname)) {
					if (isint(varname2)){
						vars[varname] += vars[varname2];
					} else if (isfloat(varname2)) {
						vars[varname] += int(fvars[varname2]);
					} else if (isdouble(varname2)) {
						vars[varname] += int(dvars[varname2]);
					} else {vars[varname] += std::stoi(varname2);}
				} else if (isfloat(varname)) {
					if (isint(varname2)){
						fvars[varname] += float(vars[varname2]);
					} else if (isfloat(varname2)) {
						fvars[varname] += fvars[varname2];
					} else if (isdouble(varname2)) {
						fvars[varname] += float(dvars[varname2]);
					} else {fvars[varname] += std::stof(varname2);}
				} else if (isdouble(varname)) {
					if (isint(varname2)){
						dvars[varname] += double(vars[varname2]);
					} else if (isfloat(varname2)) {
						dvars[varname] += double(fvars[varname2]);
					} else if (isdouble(varname2)) {
						dvars[varname] += dvars[varname2];
					} else {dvars[varname] += std::stod(varname2);}
				} else {return 5;}
			} else if (operation == "mult") {
				iss >> varname2;
				if (isint(varname)) {
					if (isint(varname2)){
						vars[varname] *= vars[varname2];
					} else if (isfloat(varname2)) {
						vars[varname] *= int(fvars[varname2]);
					} else if (isdouble(varname2)) {
						vars[varname] *= int(dvars[varname2]);
					} else {vars[varname] *= std::stoi(varname2);}
				} else if (isfloat(varname)) {
					if (isint(varname2)){
						fvars[varname] *= float(vars[varname2]);
					} else if (isfloat(varname2)) {
						fvars[varname] *= fvars[varname2];
					} else if (isdouble(varname2)) {
						fvars[varname] *= float(dvars[varname2]);
					} else {fvars[varname] *= std::stof(varname2);}
				} else if (isdouble(varname)) {
					if (isint(varname2)){
						dvars[varname] *= double(vars[varname2]);
					} else if (isfloat(varname2)) {
						dvars[varname] *= double(fvars[varname2]);
					} else if (isdouble(varname2)) {
						dvars[varname] *= dvars[varname2];
					} else {dvars[varname] *= std::stod(varname2);}
				} else return 5;
			} else if (operation == "sub") {
				iss >> varname2;
				if (isint(varname)) {
					if (isint(varname2)){
						vars[varname] -= vars[varname2];
					} else if (isfloat(varname2)) {
						vars[varname] -= int(fvars[varname2]);
					} else if (isdouble(varname2)) {
						vars[varname] -= int(dvars[varname2]);
					} else {vars[varname] -= std::stoi(varname2);}
				} else if (isfloat(varname)) {
					if (isint(varname2)){
						fvars[varname] -= float(vars[varname2]);
					} else if (isfloat(varname2)) {
						fvars[varname] -= fvars[varname2];
					} else if (isdouble(varname2)) {
						fvars[varname] -= float(dvars[varname2]);
					} else {fvars[varname] -= std::stof(varname2);}
				} else if (isdouble(varname)) {
					if (isint(varname2)){
						dvars[varname] -= double(vars[varname2]);
					} else if (isfloat(varname2)) {
						dvars[varname] -= double(fvars[varname2]);
					} else if (isdouble(varname2)) {
						dvars[varname] -= dvars[varname2];
					} else {dvars[varname] -= std::stod(varname2);}
				} else return 5;
			} else if (operation == "div") {
				iss >> varname2;
				if (isint(varname)) {
					if (isint(varname2)){
						if (vars[varname2] == 0) {return 4;}
						vars[varname] /= vars[varname2];
					} else if (isfloat(varname2)) {
						if (fvars[varname2] == 0) {return 4;}
						vars[varname] /= int(fvars[varname2]);
					} else if (isdouble(varname2)) {
						if (dvars[varname2] == 0) {return 4;}
						vars[varname] /= int(dvars[varname2]);
					} else {
						if (std::stoi(varname2) == 0) {return 4;}
						vars[varname] /= std::stoi(varname2);
					}
				} else if (isfloat(varname)) {
					if (isint(varname2)){
						if (vars[varname2] == 0) {return 4;}
						fvars[varname] /= float(vars[varname2]);
					} else if (isfloat(varname2)) {
						if (fvars[varname2] == 0) {return 4;}
						fvars[varname] /= fvars[varname2];
					} else if (isdouble(varname2)) {
						if (dvars[varname2] == 0) {return 4;}
						fvars[varname] /= float(dvars[varname2]);
					} else {
						if (std::stoi(varname2) == 0) {return 4;}
						fvars[varname] /= std::stof(varname2);
					}
				} else if (isdouble(varname)) {
					if (isint(varname2)){
						if (vars[varname2] == 0) {return 4;}
						dvars[varname] /= double(vars[varname2]);
					} else if (isfloat(varname2)) {
						if (fvars[varname2] == 0) {return 4;}
						dvars[varname] /= double(fvars[varname2]);
					} else if (isdouble(varname2)) {
						if (dvars[varname2] == 0) {return 4;}
						dvars[varname] /= dvars[varname2];
					} else {
						if (std::stoi(varname2) == 0) {return 4;}
						dvars[varname] /= std::stod(varname2);
					}
				} else return 5;
			} else if (operation == "pow") {
				iss >> varname2;
				if (isint(varname)) {
					if (isint(varname2)){
						vars[varname] = int(powerab(double(vars[varname]), vars[varname2]));
					} else if (isfloat(varname2)) {
						vars[varname] = int(powerab(double(vars[varname]), int(fvars[varname2])));
					} else if (isdouble(varname2)) {
						vars[varname] = int(powerab(double(vars[varname]), int(dvars[varname2])));
					} else {
						vars[varname] = int(powerab(double(vars[varname]), std::stoi(varname2)));
					}
				} if (isfloat(varname)) {
					if (isint(varname2)){
						fvars[varname] = float(powerab(double(vars[varname]), vars[varname2]));
					} else if (isfloat(varname2)) {
						fvars[varname] = float(powerab(double(vars[varname]), int(fvars[varname2])));
					} else if (isdouble(varname2)) {
						fvars[varname] = float(powerab(double(vars[varname]), int(dvars[varname2])));
					} else {
						fvars[varname] = float(powerab(double(vars[varname]), std::stoi(varname2)));
					}
				} if (isdouble(varname)) {
					if (isint(varname2)){
						dvars[varname] = powerab(dvars[varname], vars[varname2]);
					} else if (isfloat(varname2)) {
						dvars[varname] = powerab(dvars[varname], int(fvars[varname2]));
					} else if (isdouble(varname2)) {
						dvars[varname] = powerab(dvars[varname], int(dvars[varname2]));
					} else {
						dvars[varname] = powerab(dvars[varname], std::stoi(varname2));
					}
				} else return 5;
			} else if (operation == "sqrt") {
				if (isdouble(varname)) {
					if (dvars[varname] < 0) {
						return 7;
					} else {dvars[varname] = Sqrt(dvars[varname]);}
				} else return 5;
			} else if (operation == "<") {
				iss >> varname2;
				if (isint(varname)) {
					if (isint(varname2)){
						if (vars[varname] < vars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (vars[varname] < int(fvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (vars[varname] < int(dvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (vars[varname] < std::stoi(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else if (isfloat(varname)) {
					if (isint(varname2)){
						if (fvars[varname] < float(vars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (fvars[varname] < fvars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (fvars[varname] < float(dvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (fvars[varname] < std::stof(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else if (isdouble(varname)) {
					if (isint(varname2)){
						if (dvars[varname] < double(vars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (dvars[varname] < double(fvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (dvars[varname] < dvars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (dvars[varname] < std::stod(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else return 5;
			} else if (operation == ">") {
				iss >> varname2;
				if (isint(varname)) {
					if (isint(varname2)){
						if (vars[varname] > vars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (vars[varname] > int(fvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (vars[varname] > int(dvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (vars[varname] > std::stoi(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else if (isfloat(varname)) {
					if (isint(varname2)){
						if (fvars[varname] > float(vars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (fvars[varname] > fvars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (fvars[varname] > float(dvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (fvars[varname] > std::stof(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else if (isdouble(varname)) {
					if (isint(varname2)){
						if (dvars[varname] > double(vars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (dvars[varname] > double(fvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (dvars[varname] > dvars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (dvars[varname] > std::stod(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else return 5;
			} else if (operation == "==") {
				iss >> varname2;
				if (isint(varname)) {
					if (isint(varname2)){
						if (vars[varname] == vars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (vars[varname] == int(fvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (vars[varname] == int(dvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (vars[varname] == std::stoi(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else if (isfloat(varname)) {
					if (isint(varname2)){
						if (fvars[varname] == float(vars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (fvars[varname] == fvars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (fvars[varname] == float(dvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (fvars[varname] == std::stof(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else if (isdouble(varname)) {
					if (isint(varname2)){
						if (dvars[varname] == double(vars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (dvars[varname] == double(fvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (dvars[varname] == dvars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (dvars[varname] == std::stod(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else return 5;
			} else if (operation == "!=") {
				iss >> varname2;
				if (isint(varname)) {
					if (isint(varname2)){
						if (vars[varname] != vars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (vars[varname] != int(fvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (vars[varname] != int(dvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (vars[varname] != std::stoi(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else if (isfloat(varname)) {
					if (isint(varname2)){
						if (fvars[varname] != float(vars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (fvars[varname] != fvars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (fvars[varname] != float(dvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (fvars[varname] != std::stof(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else if (isdouble(varname)) {
					if (isint(varname2)){
						if (dvars[varname] != double(vars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (dvars[varname] != double(fvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (dvars[varname] == dvars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (dvars[varname] != std::stod(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else return 5;
			} else if (operation == ">=") {
				iss >> varname2;
				if (isint(varname)) {
					if (isint(varname2)){
						if (vars[varname] >= vars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (vars[varname] >= int(fvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (vars[varname] >= int(dvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (vars[varname] >= std::stoi(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else if (isfloat(varname)) {
					if (isint(varname2)){
						if (fvars[varname] >= float(vars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (fvars[varname] >= fvars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (fvars[varname] >= float(dvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (fvars[varname] >= std::stof(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else if (isdouble(varname)) {
					if (isint(varname2)){
						if (dvars[varname] >= double(vars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (dvars[varname] >= double(fvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (dvars[varname] >= dvars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (dvars[varname] >= std::stod(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else return 5;
			} else if (operation == "<=") {
				iss >> varname2;
				if (isint(varname)) {
					if (isint(varname2)){
						if (vars[varname] <= vars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (vars[varname] <= int(fvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (vars[varname] <= int(dvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (vars[varname] <= std::stoi(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else if (isfloat(varname)) {
					if (isint(varname2)){
						if (fvars[varname] <= float(vars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (fvars[varname] <= fvars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (fvars[varname] <= float(dvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (fvars[varname] <= std::stof(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else if (isdouble(varname)) {
					if (isint(varname2)){
						if (dvars[varname] <= double(vars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isfloat(varname2)) {
						if (dvars[varname] <= double(fvars[varname2])) {lineNumber += 0;}
						else {lineNumber++;}
					} else if (isdouble(varname2)) {
						if (dvars[varname] <= dvars[varname2]) {lineNumber += 0;}
						else {lineNumber++;}
					} else {
						if (dvars[varname] <= std::stod(varname2)) {lineNumber += 0;}
						else {lineNumber++;}
					}
				} else return 5;
			} else if (operation == "factor") {
				if (isint(varname)) {
					vars[varname] = factor(vars[varname]);
				} else if (isfloat(varname)) {
					fvars[varname] = float(factor(int(fvars[varname])));
				} else if (isdouble(varname)) {
					dvars[varname] = double(factor(int(dvars[varname])));
				} else return 5;
			} else if (operation == "cnk") {
				iss >> varname2;
				if (isint(varname)) {
					if (isint(varname2)){
						vars[varname] = cnk(vars[varname], vars[varname2]);
					} else if (isfloat(varname2)) {
						vars[varname] = cnk(vars[varname], int(fvars[varname2]));
					} else if (isdouble(varname2)) {
						vars[varname] = cnk(vars[varname], int(dvars[varname2]));
					} else {vars[varname] = cnk(vars[varname], std::stoi(varname2));}
				} else if (isfloat(varname)) {
					if (isint(varname2)){
						fvars[varname] = float(cnk(int(fvars[varname]), vars[varname2]));
					} else if (isfloat(varname2)) {
						fvars[varname] = float(cnk(int(fvars[varname]), int(fvars[varname2])));
					} else if (isdouble(varname2)) {
						fvars[varname] = float(cnk(int(fvars[varname]), int(dvars[varname2])));
					} else {fvars[varname] = float(cnk(int(fvars[varname]), std::stoi(varname2)));}
				} else if (isdouble(varname)) {
					if (isint(varname2)){
						dvars[varname] = double(cnk(int(dvars[varname]), vars[varname2]));
					} else if (isfloat(varname2)) {
						dvars[varname] = double(cnk(int(dvars[varname]), int(fvars[varname2])));
					} else if (isdouble(varname2)) {
						dvars[varname] = double(cnk(int(dvars[varname]), int(dvars[varname2])));
					} else {dvars[varname] = double(cnk(int(dvars[varname]), std::stoi(varname2)));}
				} else return 5;
			} else if (operation == "sin") {
				if (isint(varname)) {
					vars[varname] = int(sinn(double(vars[varname])));
				} else if (isfloat(varname)) {
					fvars[varname] = float(sinn(double(fvars[varname])));
				} else if (isdouble(varname)) {
					dvars[varname] = sinn(double(dvars[varname]));
				} else return 5;
			} else if (operation == "cos") {
				if (isint(varname)) {
					vars[varname] = int(cosn(double(vars[varname])));
				} else if (isfloat(varname)) {
					fvars[varname] = float(cosn(double(fvars[varname])));
				} else if (isdouble(varname)) {
					dvars[varname] = cosn(double(dvars[varname]));
				} return 5;
			} else if (operation == "exp") {
				if (isint(varname)) {
					vars[varname] = int(expn(double(vars[varname])));
				} else if (isfloat(varname)) {
					fvars[varname] = float(expn(double(fvars[varname])));
				} else if (isdouble(varname)) {
					dvars[varname] = expn(double(dvars[varname]));
				} else return 5;
			} else if (operation == "ln") {
				if (isint(varname)) {
					vars[varname] = int(lnn(double(vars[varname])));
				} else if (isfloat(varname)) {
					fvars[varname] = float(lnn(double(fvars[varname])));
				} else if (isdouble(varname)) {
					dvars[varname] = lnn(double(dvars[varname]));
				} else return 5;
			} else if (operation == "equal") {
				iss >> varname2;
				if (isint(varname)) {
					if (isint(varname2)){
						vars[varname] = vars[varname2];
					} else if (isfloat(varname2)) {
						vars[varname] = int(fvars[varname2]);
					} else if (isdouble(varname2)) {
						vars[varname] = int(dvars[varname2]);
					} else {vars[varname] = std::stoi(varname2);}
				} else if (isfloat(varname)) {
					if (isint(varname2)){
						fvars[varname] = float(vars[varname2]);
					} else if (isfloat(varname2)) {
						fvars[varname] = fvars[varname2];
					} else if (isdouble(varname2)) {
						fvars[varname] = float(dvars[varname2]);
					} else {fvars[varname] = std::stof(varname2);}
				} else if (isdouble(varname)) {
					if (isint(varname2)){
						dvars[varname] = double(vars[varname2]);
					} else if (isfloat(varname2)) {
						dvars[varname] = double(fvars[varname2]);
					} else if (isdouble(varname2)) {
						dvars[varname] = dvars[varname2];
					} else {dvars[varname] = std::stod(varname2);}
				} else return 5;
			} else if (operation == "run") {
				std::string command = "kikka " + strings[varname];
				system(command.c_str());
			} else if (operation == "mod") {
				iss >> varname2;
				if (isint(varname)) {
					if (isint(varname2)){
						vars[varname] %= vars[varname2];
					} else if (isfloat(varname2)) {
						vars[varname] %= int(fvars[varname2]);
					} else if (isdouble(varname2)) {
						vars[varname] %= int(dvars[varname2]);
					} else {vars[varname] %= std::stoi(varname2);}
				} else if (isfloat(varname)) {
					if (isint(varname2)){
						fvars[varname] = float(int(fvars[varname]) % vars[varname2]);
					} else if (isfloat(varname2)) {
						fvars[varname] = float(int(fvars[varname]) % int(fvars[varname2]));
					} else if (isdouble(varname2)) {
						fvars[varname] = float(int(fvars[varname]) % int(dvars[varname2]));
					} else {fvars[varname] = float(int(fvars[varname]) % std::stoi(varname2));}
				} else if (isdouble(varname)) {
					if (isint(varname2)){
						dvars[varname] = double(int(dvars[varname]) % vars[varname2]);
					} else if (isfloat(varname2)) {
						dvars[varname] = double(int(dvars[varname]) % int(fvars[varname2]));
					} else if (isdouble(varname2)) {
						dvars[varname] = double(int(dvars[varname]) % int(dvars[varname2]));
					} else {dvars[varname] = double(int(dvars[varname]) % std::stoi(varname2));}
				} else return 5;
			} else if (operation == "prec") {;} 
			else if (operation == "tint") {;} 
			else if (operation == "tfloat") {;} 
			else if (operation == "tdouble") {;} 
			else if (operation == "tchar") {;} 
			else if (operation == "tbool") {;} 
			else if (operation == "tstring") {;} 
			else if (operation == "set") {
				iss >> varname2 >> varname3;
				if (isiarr(varname)) { // int array
					if (isint(varname2)) {
						if (isint(varname3)) {
							iarrs[varname][vars[varname2]] = vars[varname3];
						} else if (isfloat(varname3)) {
							iarrs[varname][vars[varname2]] = int(fvars[varname3]);
						} else if (isdouble(varname3)) {
							iarrs[varname][vars[varname2]] = int(dvars[varname3]);
						} else return 5;
					} else if (isfloat(varname2)) {
						if (isint(varname3)) {
							iarrs[varname][int(fvars[varname2])] = vars[varname3];
						} else if (isfloat(varname3)) {
							iarrs[varname][int(fvars[varname2])] = int(fvars[varname3]);
						} else if (isdouble(varname3)) {
							iarrs[varname][int(fvars[varname2])] = int(dvars[varname3]);
						} else return 5;
					} else if (isdouble(varname2)) {
						if (isint(varname3)) {
							iarrs[varname][int(dvars[varname2])] = vars[varname3];
						} else if (isfloat(varname3)) {
							iarrs[varname][int(dvars[varname2])] = int(fvars[varname3]);
						} else if (isdouble(varname3)) {
							iarrs[varname][int(dvars[varname2])] = int(dvars[varname3]);
						} else return 5;
					} else return 5;
				} else if (isfarr(varname)) { // float array
					if (isint(varname2)) {
						if (isint(varname3)) {
							farrs[varname][vars[varname2]] = float(vars[varname3]);
						} else if (isfloat(varname3)) {
							farrs[varname][vars[varname2]] = fvars[varname3];
						} else if (isdouble(varname3)) {
							farrs[varname][vars[varname2]] = float(dvars[varname3]);
						} else return 5;
					} else if (isfloat(varname2)) {
						if (isint(varname3)) {
							farrs[varname][int(fvars[varname2])] = float(vars[varname3]);
						} else if (isfloat(varname3)) {
							farrs[varname][int(fvars[varname2])] = fvars[varname3];
						} else if (isdouble(varname3)) {
							farrs[varname][int(fvars[varname2])] = float(dvars[varname3]);
						} else return 5;
					} else if (isdouble(varname2)) {
						if (isint(varname3)) {
							farrs[varname][int(dvars[varname2])] = float(vars[varname3]);
						} else if (isfloat(varname3)) {
							farrs[varname][int(dvars[varname2])] = fvars[varname3];
						} else if (isdouble(varname3)) {
							farrs[varname][int(dvars[varname2])] = float(dvars[varname3]);
						} else return 5;
					} else return 5;
				} else if (isdarr(varname)) { // double array
					if (isint(varname2)) {
						if (isint(varname3)) {
							darrs[varname][vars[varname2]] = double(vars[varname3]);
						} else if (isfloat(varname3)) {
							darrs[varname][vars[varname2]] = double(fvars[varname3]);
						} else if (isdouble(varname3)) {
							darrs[varname][vars[varname2]] = dvars[varname3];
						} else return 5;
					} else if (isfloat(varname2)) {
						if (isint(varname3)) {
							darrs[varname][int(fvars[varname2])] = double(vars[varname3]);
						} else if (isfloat(varname3)) {
							darrs[varname][int(fvars[varname2])] = double(fvars[varname3]);
						} else if (isdouble(varname3)) {
							darrs[varname][int(fvars[varname2])] = dvars[varname3];
						} else return 5;
					} else if (isdouble(varname2)) {
						if (isint(varname3)) {
							darrs[varname][int(dvars[varname2])] = double(vars[varname3]);
						} else if (isfloat(varname3)) {
							darrs[varname][int(dvars[varname2])] = double(fvars[varname3]);
						} else if (isdouble(varname3)) {
							darrs[varname][int(dvars[varname2])] = dvars[varname3];
						} else return 5;
					} else return 5;
				} else if (iscarr(varname)) { // char array
					if (isint(varname2)) {
						if (ischar(varname3)) {
							carrs[varname][vars[varname2]] = cvars[varname3];
						} else return 5;
					} else if (ischar(varname2)) {
						if (isint(varname3)) {
							carrs[varname][int(fvars[varname2])] = cvars[varname3];
						} else return 5;
					} else if (isdouble(varname2)) {
						if (ischar(varname3)) {
							darrs[varname][int(dvars[varname2])] = cvars[varname3];
						} else return 5;
					} else return 5;
				} else if (isbarr(varname2)) { // bool array
					if (isint(varname2)) {
						if (isint(varname3)) {
							barrs[varname][vars[varname2]] = bools[varname3];
						} else return 5;
					} else if (isfloat(varname2)) {
						if (isint(varname3)) {
							barrs[varname][int(fvars[varname2])] = bools[varname3];
						} else return 5;
					} else if (isdouble(varname2)) {
						if (isint(varname3)) {
							barrs[varname][int(dvars[varname2])] = bools[varname3];
						} else return 5;
					} else return 5;
				} else if (issarr(varname)) { // string array
					if (isint(varname2)) {
						if (isint(varname3)) {
							sarrs[varname][vars[varname2]] = std::to_string(vars[varname3]);
						} else if (isfloat(varname3)) {
							sarrs[varname][vars[varname2]] = std::to_string(fvars[varname3]);
						} else if (isdouble(varname3)) {
							sarrs[varname][vars[varname2]] = std::to_string(dvars[varname3]);
						} else if (ischar(varname3)) {
							sarrs[varname][vars[varname2]] = std::to_string(cvars[varname3]);
						} else if (isbool(varname3)) {
							sarrs[varname][vars[varname2]] = std::to_string(bools[varname3]);
						} else if (isstring(varname3)) {
							sarrs[varname][vars[varname2]] = strings[varname3];
						} else return 5;
					} else if (isfloat(varname2)) {
						if (isint(varname3)) {
							sarrs[varname][int(fvars[varname2])] = std::to_string(vars[varname3]);
						} else if (isfloat(varname3)) {
							sarrs[varname][int(fvars[varname2])] = std::to_string(fvars[varname3]);
						} else if (isdouble(varname3)) {
							sarrs[varname][int(fvars[varname2])] = std::to_string(dvars[varname3]);
						} else if (ischar(varname3)) {
							sarrs[varname][int(fvars[varname2])] = std::to_string(cvars[varname3]);
						} else if (isbool(varname3)) {
							sarrs[varname][int(fvars[varname2])] = std::to_string(bools[varname3]);
						} else if (isstring(varname3)) {
							sarrs[varname][int(fvars[varname2])] = strings[varname3];
						} else return 5;
					} else if (isdouble(varname2)) {
						if (isint(varname3)) {
							sarrs[varname][int(dvars[varname2])] = std::to_string(vars[varname3]);
						} else if (isfloat(varname3)) {
							sarrs[varname][int(dvars[varname2])] = std::to_string(fvars[varname3]);
						} else if (isdouble(varname3)) {
							sarrs[varname][int(dvars[varname2])] = std::to_string(dvars[varname3]);
						} else if (ischar(varname3)) {
							sarrs[varname][int(dvars[varname2])] = std::to_string(cvars[varname3]);
						} else if (isbool(varname3)) {
							sarrs[varname][int(dvars[varname2])] = std::to_string(bools[varname3]);
						} else if (isstring(varname3)) {
							sarrs[varname][int(dvars[varname2])] = strings[varname3];
						} else return 5;
					} else return 5;
				} else return 5;
			} else if (operation == "get") {
				iss >> varname2 >> varname3;
				if (isiarr(varname)) { // int array
					if (isint(varname2)) {
						if (isint(varname3)) {
							vars[varname3] = iarrs[varname][vars[varname2]];
						} else return 5;
					} else if (isfloat(varname2)) {
						if (isint(varname3)) {
							vars[varname3] = iarrs[varname][int(fvars[varname2])];
						} else return 5;
					} else if (isdouble(varname2)) {
						if (isint(varname3)) {
							vars[varname3] = iarrs[varname][int(dvars[varname2])];
						} else return 5;
					} else return 5;
				} else if (isfarr(varname)) { // float array
					if (isint(varname2)) {
						if (isfloat(varname3)) {
							fvars[varname3] = farrs[varname][vars[varname2]];
						} else return 5;
					} else if (isfloat(varname2)) {
						if (isfloat(varname3)) {
							fvars[varname3] = farrs[varname][int(fvars[varname2])];
						} else return 5;
					} else if (isdouble(varname2)) {
						if (isfloat(varname3)) {
							fvars[varname3] = farrs[varname][int(dvars[varname2])];
						} else return 5;
					} else return 5;
				} else if (isdarr(varname)) { // double array
					if (isint(varname2)) {
						if (isdouble(varname3)) {
							dvars[varname3] = darrs[varname][vars[varname2]];
						} else return 5;
					} else if (isfloat(varname2)) {
						if (isdouble(varname3)) {
							dvars[varname3] = darrs[varname][int(fvars[varname2])];
						} else return 5;
					} else if (isdouble(varname2)) {
						if (isdouble(varname3)) {
							dvars[varname3] = darrs[varname][int(dvars[varname2])];
						} else return 5;
					} else return 5;
				} else if (iscarr(varname)) { // char array
					if (isint(varname2)) {
						if (ischar(varname3)) {
							cvars[varname3] = carrs[varname][vars[varname2]];
						} else return 5;
					} else if (ischar(varname2)) {
						if (isint(varname3)) {
							cvars[varname3] = carrs[varname][int(fvars[varname2])];
						} else return 5;
					} else if (isdouble(varname2)) {
						if (ischar(varname3)) {
							cvars[varname3] = darrs[varname][int(dvars[varname2])];
						} else return 5;
					} else return 5;
				} else if (isbarr(varname2)) { // bool array
					if (isint(varname2)) {
						if (isint(varname3)) {
							bools[varname3] = barrs[varname][vars[varname2]];
						} else return 5;
					} else if (isfloat(varname2)) {
						if (isint(varname3)) {
							bools[varname3] = barrs[varname][int(fvars[varname2])];
						} else return 5;
					} else if (isdouble(varname2)) {
						if (isint(varname3)) {
							bools[varname3] = barrs[varname][int(dvars[varname2])];
						} else return 5;
					} else return 5;
				} else if (issarr(varname)) { // string array
					if (isint(varname2)) {
						if (isstring(varname3)) {
							strings[varname3] = sarrs[varname][vars[varname2]];
						} else return 5;
					} else if (isfloat(varname2)) {
						if (isstring(varname3)) {
							strings[varname3] = sarrs[varname][int(fvars[varname2])];
						} else return 5;
					} else if (isdouble(varname2)) {
						if (isstring(varname3)) {
							strings[varname3] = sarrs[varname][int(dvars[varname2])];
						} else return 5;
					} else return 5;
				} else return 5;
			} else if (operation == "uniform") {
				iss >> varname2 >> varname3;
				if (isdouble(varname)) {
					if (isdouble(varname2) && (isdouble(varname3))) {
						if (dvars[varname2] < dvars[varname3]) {
							dvars[varname] = RandomUniform(dvars[varname2], dvars[varname3]);
						} else return 6;
					} else if (isdouble(varname2) && (!isdouble(varname3))) {
						if (dvars[varname2] < std::stod(varname3)) {
							dvars[varname] = RandomUniform(dvars[varname2], std::stod(varname3));
						} else return 6;
					} else if (!isdouble(varname2) && (isdouble(varname3))) {
						if (std::stod(varname2) < dvars[varname3]) {
							dvars[varname] = RandomUniform(std::stod(varname2), dvars[varname3]);
						} else return 6;
					} else if (!isdouble(varname2) && (!isdouble(varname3))) {
						if (std::stod(varname2) < std::stod(varname3)) {
							dvars[varname] = RandomUniform(std::stod(varname2), std::stod(varname3));
						} else return 6;
					} 
				} else return 5;
			} else if (operation == "rand") {;}
			else if (operation == "normal") {
				iss >> varname2 >> varname3;
				if (isdouble(varname)) {
					if (isdouble(varname2) && (isdouble(varname3))) {
						if (dvars[varname3] > 0) {
							dvars[varname] = RandomNormal(dvars[varname2], dvars[varname3]);
						} else return 8;
					} else if (isdouble(varname2) && (!isdouble(varname3))) {
						if (std::stod(varname3) > 0) {
							dvars[varname] = RandomUniform(dvars[varname2], std::stod(varname3));
						} else return 8;
					} else if (!isdouble(varname2) && (isdouble(varname3))) {
						if (dvars[varname3] > 0) {
							dvars[varname] = RandomNormal(std::stod(varname2), dvars[varname3]);
						} else return 8;
					} else if (!isdouble(varname2) && (!isdouble(varname3))) {
						if (std::stod(varname3) > 0) {
							dvars[varname] = RandomNormal(std::stod(varname2), std::stod(varname3));
						} else return 8;
					} 
				} else return 5;
			} else if (operation == "bernoulli") {
				iss >> varname2;
				if (isint(varname)) {
					if (isdouble(varname2)) {
						if ((dvars[varname2] >= 0) && (dvars[varname2] <= 1)) {
							vars[varname] = RandomBernoulli(dvars[varname2]);
						} else return 9;
					} else {
						if ((std::stod(varname2) >= 0) && (std::stod(varname2) <= 1)) {
							vars[varname] = RandomBernoulli(std::stod(varname2));
						} else return 9;
					}
				} else return 5;
			} else if (operation == "poisson") {
				iss >> varname2;
				if (isint(varname)) {
					if (isdouble(varname2)) {
						if (dvars[varname2] > 0) {
							vars[varname] = RandomPoisson(dvars[varname2]);
						} else return 9;
					} else {
						if (std::stod(varname2) > 0) {
							vars[varname] = RandomPoisson(std::stod(varname2));
						} else return 9;
					}
				} else return 5;
			} else if (operation == "pop") {
				iss >> varname2;
				if (isiarr(varname)) { // int array
					if (isint(varname2)) {
						vars[varname2] = iarrs[varname].back();
					} else if (isfloat(varname2)) {
						fvars[varname2] = float(iarrs[varname].back());
					} else if (isdouble(varname2)) {
						dvars[varname2] = double(iarrs[varname].back());
					} else return 5;
					iarrs[varname].pop_back();
				} else if (isfarr(varname)) { // float array
					if (isint(varname2)) {
						vars[varname2] = int(farrs[varname].back());
					} else if (isfloat(varname2)) {
						fvars[varname2] = farrs[varname].back();
					} else if (isdouble(varname2)) {
						dvars[varname2] = double(farrs[varname].back());
					} else return 5;
					farrs[varname].pop_back();
				} else if (isdarr(varname)) { // double array
					if (isint(varname2)) {
						vars[varname2] = int(darrs[varname].back());
					} else if (isfloat(varname2)) {
						fvars[varname2] = float(darrs[varname].back());
					} else if (isdouble(varname2)) {
						dvars[varname2] = darrs[varname].back();
					} else return 5;
					darrs[varname].pop_back();
				} else if (iscarr(varname)) { // char array
					if (ischar(varname2)) {
						cvars[varname2] = carrs[varname].back();
					} else return 5;
					carrs[varname].pop_back();
				} else if (isbarr(varname2)) { // bool array
					if (isbool(varname2)) {
						bools[varname2] = barrs[varname].back();
					} else return 5;
					barrs[varname].pop_back();
				} else if (issarr(varname)) { // string array
					if (isstring(varname2)) {
						strings[varname2] = sarrs[varname].back();
					} else return 5;
					sarrs[varname].pop_back();
				} else return 5;
			} else if (operation == "append") {
				iss >> varname2;
				if (isiarr(varname)) { // int array
					if (isint(varname2)) {
						iarrs[varname].push_back(vars[varname2]);
					} else if (isfloat(varname2)) {
						iarrs[varname].push_back(int(fvars[varname2]));
					} else if (isdouble(varname2)) {
						iarrs[varname].push_back(int(dvars[varname2]));
					} else return 5;
				} else if (isfarr(varname)) { // float array
					if (isint(varname2)) {
						farrs[varname].push_back(float(vars[varname2]));
					} else if (isfloat(varname2)) {
						farrs[varname].push_back(fvars[varname2]);
					} else if (isdouble(varname2)) {
						farrs[varname].push_back(float(dvars[varname2]));
					} else return 5;
				} else if (isdarr(varname)) { // double array
					if (isint(varname2)) {
						darrs[varname].push_back(double(vars[varname2]));
					} else if (isfloat(varname2)) {
						darrs[varname].push_back(double(fvars[varname2]));
					} else if (isdouble(varname2)) {
						darrs[varname].push_back(dvars[varname2]);
					} else return 5;
				} else if (iscarr(varname)) { // char array
					if (ischar(varname2)) {
						carrs[varname].push_back(cvars[varname2]);
					} else return 5;
				} else if (isbarr(varname2)) { // bool array
					if (isbool(varname2)) {
						barrs[varname].push_back(bools[varname2]);
					} else return 5;
				} else if (issarr(varname)) { // string array
					if (isint(varname2)) {
						sarrs[varname].push_back(std::to_string(vars[varname2]));
					} else if (isfloat(varname2)) {
						sarrs[varname].push_back(std::to_string(fvars[varname2]));
					} else if (isdouble(varname2)) {
						sarrs[varname].push_back(std::to_string(dvars[varname2]));
					} else if (ischar(varname2)) {
						sarrs[varname].push_back(std::to_string(cvars[varname2]));
					} else if (isbool(varname2)) {
						sarrs[varname].push_back(std::to_string(bools[varname2]));
					} else if (isstring(varname2)) {
						sarrs[varname].push_back(strings[varname2]);
					} else return 5;
				} else return 5;
			} else if (operation == "write") {
				iss >> varname2;
				if (!isstring(varname) && !issarr(varname2)) {return 5;}
				std::ofstream file(strings[varname]);
				if (file.is_open()) {
					for (const auto& line : sarrs[varname2]) {
						file << line << std::endl;
					}
					file.close();
				} else {return 10;}
			} else if (operation == "read") {
				iss >> varname2;
				if (!issarr(varname) && !isstring(varname2)) {return 5;}
				std::ifstream file(strings[varname2]);
				if (file.is_open()) {
					std::string line;
					while (getline(file, line)) {
						sarrs[varname].push_back(line);
					}
					file.close();
				} else {return 10;}
			} else if (operation == "jikannohajimaru") {;}
			else if (operation == "jikannoowari") {
				if (jikanwomiru == true) {
					clock_t end = clock();
					double seconds = (double)(end - start) / CLOCKS_PER_SEC;
					std::cerr << "Seconds: " << seconds << std::endl;
					jikanwomiru = false;
				}
			} else if (operation == "wasurete") {
				if (isint(varname)) {
					vars.erase(varname);
				} else if (isfloat(varname)) {
					fvars.erase(varname);
				} else if (isdouble(varname)) {
					dvars.erase(varname);
				} else if (ischar(varname)) {
					cvars.erase(varname);
				} else if (isbool(varname)) {
					bools.erase(varname);
				} else if (isstring(varname)) {
					strings.erase(varname);
				} else return 5;
			} else if ((operation == "inInt") || 
			(operation == "inFlo") || 
			(operation == "inDou") || 
			(operation == "inCha") || 
			(operation == "inBoo") || 
			(operation == "inStr")) {;}
			else if (operation == "ToInt") {
				iss >> varname2;
				if (isint(varname)) {
					if (isfloat(varname2)) {
						vars[varname] = int(fvars[varname2]);
					} else if (isdouble(varname2)) {
						vars[varname] = int(dvars[varname2]);
					} else if (isstring(varname2)) {
						vars[varname] = std::stoi(strings[varname2]);
					} else return 5;
				} else return 5;
			} else if (operation == "ToFloat") {
				iss >> varname2;
				if (isfloat(varname)) {
					if (isint(varname2)) {
						fvars[varname] = float(vars[varname2]);
					} else if (isdouble(varname2)) {
						fvars[varname] = float(dvars[varname2]);
					} else if (isstring(varname2)) {
						fvars[varname] = std::stof(strings[varname2]);
					} else return 5;
				} return 5;
			} else if (operation == "ToDouble") {
				iss >> varname2;
				if (isdouble(varname)) {
					if (isint(varname2)) {
						dvars[varname] = double(vars[varname2]);
					} else if (isfloat(varname2)) {
						dvars[varname] = double(fvars[varname2]);
					} else if (isstring(varname2)) {
						dvars[varname] = std::stod(strings[varname2]);
					} else 5;
				} else return 5;
			} else if (operation == "lenght") {
				iss >> varname2;
				if (isint(varname)) {
					if (isiarr(varname2)) {
						vars[varname] = iarrs[varname2].size();
					} else if (isfarr(varname2)) {
						vars[varname] = farrs[varname2].size();
					} else if (isdarr(varname2)) {
						vars[varname] = darrs[varname2].size();
					} else if (isbarr(varname2)) {
						vars[varname] = barrs[varname2].size();
					} else if (iscarr(varname2)) {
						vars[varname] = carrs[varname2].size();
					} else if (issarr(varname2)) {
						vars[varname] = sarrs[varname2].size();
					} else return 5;
				} else return 5;
			} else if (operation == "/*") {watchblock = true;}
			else if (operation == "*/") {;}
			else if (operation == "mean") {
				iss >> varname2;
				if (isdouble(varname) && isdarr(varname2)) {
					dvars[varname] = calculateAverage(varname2);
				} else return 5;
			} else if (operation == "cumsum") {
				iss >> varname2;
				if (isdouble(varname) && isdarr(varname2)) {
					dvars[varname] = cumsum(varname2);
				} else return 5;
			} else if (operation == "dispersion") {
				iss >> varname2;
				if (isdouble(varname) && isdarr(varname2)) {
					dvars[varname] = calcdisp(varname2);
				} else return 5;
			} else if (operation == "ceil") {
				iss >> varname2;
				if (isint(varname)) {
					if (isfloat(varname2)) {
						vars[varname] = ceil(fvars[varname2]);
					} else if (isdouble(varname2)) {
						vars[varname] = ceil(dvars[varname2]);
					} else return 5;
				} else return 5;
			} else if (operation == "floor") {
				iss >> varname2;
				if (isint(varname)) {
					if (isfloat(varname2)) {
						vars[varname] = floor(fvars[varname2]);
					} else if (isdouble(varname2)) {
						vars[varname] = floor(dvars[varname2]);
					} else return 5;
				} else return 5;
			} else if (operation == "round") {
				iss >> varname2;
				if (isint(varname)) {
					if (isfloat(varname2)) {
						if ((fvars[varname2] - floor(fvars[varname2])) < 0.5) {
							vars[varname] = floor(fvars[varname2]);
						} else {vars[varname] = ceil(fvars[varname2]);}
					} else if (isdouble(varname2)) {
						if ((dvars[varname2] - floor(dvars[varname2])) < 0.5) {
							vars[varname] = floor(dvars[varname2]);
						} else {vars[varname] = ceil(dvars[varname2]);}
					} else return 5;
				} else return 5;
			} else if (operation == "trunc") {
				iss >> varname2;
				if (isint(varname)) {
					if (isfloat(varname2)) {
						vars[varname] = int(fvars[varname2]);
					} else if (isdouble(varname2)) {
						vars[varname] = int(dvars[varname2]);
					} else return 5;
				} else return 5;
			} 
			else return 1;
		}
	} else {
		if (operation == "break") {watchblock = false;}
		else if (operation == "*/") {watchblock = false;}
	}

	return 0;
}

// Основная функця процессора, интерпретирующая ассемблер
int main(int argc, char* argv[]) {

	// Проверяем, был ли передан файл
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
		std::cerr << "Kikkago - Interpreted assembler? For what?" << std::endl;
		std::cerr << "Version 1.0.2" << std::endl;
        return 1;
    }

    // Получаем имя файла из аргументов командной строки
    std::string filename = argv[1];
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "File not found." << std::endl;
        return 1;
    }

    std::vector<std::string> program;
    std::string line;
	int lineNum = 0; // Для номера строки
	int hajimaru = 0;
	vars["cycles"] = 0; // Сколько прошло циклов программы записывается в техническую переменную
	
	dvars["pi"] = 3.14159265358979323846;
	dvars["euler"] = 2.71828182845904523536;
	bool dec_owari = false;
    
	while (std::getline(file, line)) {
		std::istringstream lstr(line);
		std::string declare, block, varname;
		std::string stv;
		int value = 0;
		char cvalue = 0;
		float fvalue = 0;
		double dvalue = 0;
		
		lstr >> declare;
		if (declare == "jikannohajimaru") {
			start = clock();
			jikanwomiru = true;
		} else if (declare == "block"){
			lstr >> block;
			blocks[block] = lineNum;
		} else if (declare == "hajimaru") {hajimaru = lineNum;}
		else if (declare == "prec") {
			lstr >> value;
			std::cout << std::fixed << std::setprecision(value);
		} else if (declare == "tint") {
			lstr >> varname >> value;
			if (isnotdeclared(varname)) {
				vars[varname] = value;
			} else {
				std::cerr << "Declaration of existing variable in line " << lineNum << std::endl;
				return 3;
			}
		} else if (declare == "tfloat") {
			lstr >> varname >> fvalue;
			if (isnotdeclared(varname)) {
				fvars[varname] = fvalue;
			} else {
				std::cerr << "Declaration of existing variable in line " << lineNum << std::endl;
				return 3;
			}
		} else if (declare == "tdouble") {
			lstr >> varname >> dvalue;
			if (isnotdeclared(varname)) {
				dvars[varname] = dvalue;
			} else {
				std::cerr << "Declaration of existing variable in line " << lineNum << std::endl;
				return 3;
			}
		} else if (declare == "tchar") {
			lstr >> varname >> cvalue;
			if (isnotdeclared(varname)) {
				cvars[varname] = cvalue;
			} else {
				std::cerr << "Declaration of existing variable in line " << lineNum << std::endl;
				return 3;
			}
		} else if (declare == "tbool") {
			lstr >> varname >> value;
			if (isnotdeclared(varname)) {
				bools[varname] = (value == 0) ? 0 : 1;
			} else {
				std::cerr << "Declaration of existing variable in line " << lineNum << std::endl;
				return 3;
			}
		} else if (declare == "tstring") {
			lstr >> varname >> stv;
			if (isnotdeclared(varname)) {
				strings[varname] = stv;
			} else {
				std::cerr << "Declaration of existing variable in line " << lineNum << std::endl;
				return 3;
			}
		} else if (declare == "owari") {
			dec_owari = true;
		} else if (declare == "rand") { // Инициализация генератора случайных чисел
			lstr >> value;
			if (value == 0) {
				std::srand(static_cast<unsigned int>(std::time(0)));
			} else {
				std::srand(value);
			}
		} else if ((declare == "inInt") || 
		(declare == "inFlo") || 
		(declare == "inDou") || 
		(declare == "inCha") || 
		(declare == "inBoo") || 
		(declare == "inStr")) {
			lstr >> varname;
			if (isnotdeclared(varname)) {
				if (declare == "inInt") {
					lstr >> value;
					std::cout << varname << " : ";
					std::cin >> vars[varname];
				} else if (declare == "inFlo") {
					lstr >> fvalue;
					std::cout << varname << " : ";
					std::cin >> fvars[varname];
				} else if (declare == "inDou") {
					lstr >> dvalue;
					std::cout << varname << " : ";
					std::cin >> dvars[varname];
				} else if (declare == "inCha") {
					lstr >> cvalue;
					std::cout << varname << " : ";
					std::cin >> cvars[varname];
				} else if (declare == "inStr") {
					lstr >> stv;
					std::cout << varname << " : ";
					std::cin >> strings[varname];
				} 
			} else {return 3;}
		} 
        program.push_back(line);
		lineNum++;
    }
    file.close();
	
	if (dec_owari == false) {
		std::cerr << "The 'owari' operator was never written down! Your program will never stop!" << std::endl;
		return 1;
	}
	
    lineNumber = hajimaru;
	int next_lineNumber = lineNumber + 1;
	char choice;
    while (lineNumber < program.size()) {
		int code = interpretline(program[lineNumber]);
		
		if (code == 0) {
			;
		} else if (code == 2) {
			std::cerr << "The program " << filename << " has completed successfully!" << std::endl;
			if (jikanwomiru == true) {
				jikanwomiru = false;
				clock_t end = clock();
				double seconds = (double)(end - start) / CLOCKS_PER_SEC;
				std::cerr << "Seconds: " << seconds << std::endl;
			}
			return 0;
		} else {
			if (code == 1) {
				std::cerr << "Unknown operation in line!" << std::endl;
			} else if (code == 3) {
				std::cerr << "Declaration of existing variable or array in line!" << std::endl;
			} else if (code == 4) {
				std::cerr << "Division by zero in line!" << std::endl;
			} else if (code == 5) {
				std::cerr << "Operand error!"<< std::endl;
				std::cerr << "The type of the operand is incorrect, or a variable was not passed as the first operand!" << std::endl;
			} else if (code == 6) {
				std::cerr << "Range error!"<< std::endl;
				std::cerr << "Left operand must be less than right operand!" << std::endl;
			} else if (code == 7) {
				std::cerr << "Range error!"<< std::endl;
				std::cerr << "Operand must be greater or equal zero!" << std::endl;
			} else if (code == 8) {
				std::cerr << "Range error!"<< std::endl;
				std::cerr << "Operand must be greater than zero!" << std::endl;
			} else if (code == 9) {
				std::cerr << "Range error!"<< std::endl;
				std::cerr << "Operand must be greater or equal zero and less than or equal one!" << std::endl;
			} else if (code == 10) {
				std::cerr << "'File not found' error!"<< std::endl;
				std::cerr << "Create the required file!" << std::endl;
			}
			std::cerr << "" << std::endl;
			std::cerr << "The line on which the error occurred:" << std::endl;
			std::cerr << "" << std::endl;
			std::cerr << "Line "<< lineNumber << ": " << program[lineNumber] << std::endl;
			std::cerr << "" << std::endl;
			do {
				std::cout << "Ignore the error (the line will be skipped, even bigger errors are possible!)? (y/n):";
				std::cin >> choice;
				if (choice == 'y' || choice == 'Y') {
					std::cerr << "The program will continue!" << std::endl;
					std::cerr << "" << std::endl;
					break;
				} else if (choice == 'n' || choice == 'N') {
					std::cerr << "The program stops!" << std::endl;
					return code;
				} else {
					std::cerr << "Invalid input. Please enter 'y' or 'n'." << std::endl;
					std::cerr << "" << std::endl;
				}
			} while ((choice != 'y') || (choice != 'Y') || (choice != 'n') || (choice != 'N'));
		}
        lineNumber++;
		next_lineNumber = lineNumber + 1;
		if (next_lineNumber > program.size()) {
			lineNumber = 0;
			cycles += 1;
			vars["cycles"] = cycles;
		}
    }

    return 0;
}