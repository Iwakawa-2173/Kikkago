#include <cstdlib>  // Для функции rand() и srand()
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <array>
#include <map>

using namespace std;

// Степень
double powerab(double a, int b) {return (b == 0) ? 1.0 : (b < 0 ? 1.0 / powerab(a, -b) : a * powerab(a, b - 1));}

// Факториал
int factor(int a) {return (a == 0) ? 1 : a * factor(a-1);}

// e в степени икс
double expn(double n) {
	double result = 0;
	for (int i = 0; i <= 17; i++) {
		result += powerab(n, i) / double(factor(i));
	}
	return result;
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

class Block {
public:
	double input_a;
	double input_b;
	double state_of_block;
	double output;
	
	void Block_NegTau_change_state() {
		state_of_block = abs(state_of_block - input_b);
	}
	void Block_NegTau_logical_function() {
		output = abs(state_of_block - input_a);
	}
	
	void Block_ConPir_change_state() {
		state_of_block = abs((state_of_block - input_a)*(state_of_block - input_b));
	}
	void Block_ConPir_logical_function() {
		output = abs((state_of_block - input_a)*(state_of_block - input_b));
	}
	
	void Block_RandomBernoulli_logical_function() {
		output = RandomBernoulli(input_a);
	}
	void Block_RandomBernoulli_change_state() {
		state_of_block = RandomBernoulli(input_b);
	}
	
	void logical_function(string func_name) {
		if (func_name == "NegTau") {
			Block_NegTau_logical_function();
		} else if (func_name == "ConPir") {
			Block_ConPir_logical_function();
		} else if (func_name == "RndBern") {
			Block_RandomBernoulli_logical_function();
		}
	}
	void change_state(string state_name) {
		if (state_name == "NegTau") {
			Block_NegTau_change_state();
		} else if (state_name == "ConPir") {
			Block_ConPir_change_state();
		} else if (state_name == "RndBern") {
			Block_RandomBernoulli_change_state();
		}
	}
	
	Block(double p_input_a, double p_input_b, double p_state_of_block, double p_output)
		: input_a(p_input_a), input_b(p_input_b), state_of_block(p_state_of_block), output(p_output) {}

	Block() : input_a(0), input_b(0), state_of_block(0), output(0) {}

};

class Henkamono {
public:
    int count_of_blocks;
    int conf1, conf2;
    std::vector<Block> blocks;

    Henkamono(int p_conf1, int p_conf2, int p_count_of_blocks)
        : conf1(p_conf1), conf2(p_conf2), count_of_blocks(p_count_of_blocks) {}
	
	Henkamono() : conf1(0), conf2(1), count_of_blocks(3) {}

    void add_block(double input_a, double input_b, double state_of_block, double output) {
		Block block(input_a, input_b, state_of_block, output);
        blocks.push_back(block);
    }
	
	void add_blocks(int count_of_blocks) {
		for (int i = 0; i < count_of_blocks; i++) {
			Block block(0, 0, 0, 0);
			blocks.push_back(block);
		}
    }
	
	// Расчёт номера "следующего" блока по схеме изменений
	int calc_next_num(int n) {return ((n+(count_of_blocks-1)) % count_of_blocks);}

	// Расчёт номера "предыдущего" блока по схеме изменений (на данный момент нигде не используется)
	int calc_prev_num(int n) {return (n % count_of_blocks) + 1;}
	
	// Это функция отработки одного блока с изменением полученного значения
	void work_of_block(int i, string func_name) {
		blocks[i].logical_function(func_name);
	}
	
	// Это функция для пересылки значения с выхода на соответствующие входы следующих блоков
	void send_values(int i) {
		blocks[calc_next_num(i)].input_b = blocks[i].output; // Передача изменяющего значения
		blocks[((i+1) % count_of_blocks)].input_a = blocks[i].output; // Передача изменяемого значения
		//if (((i+1) % count_of_blocks) == 0) {std::cerr << "a" << std::endl;}
	}
	
	// Передача изменяющих значений
	void send_change_value(int i) {
		blocks[calc_next_num(i)].input_b = blocks[i].output;
	}
	// Передача изменяемых значений
	void send_processing_value(int i) {
		blocks[((i+1) % count_of_blocks)].input_a = blocks[i].output;
	}
	
	void change_block(int i, string state_name) {
		blocks[i].change_state(state_name);
	}
	
	// Режимы работы Henkamono: 01, 10 и 11
	void work_change(string func_name, string state_name) { // 01
		for (int i = 0; i < count_of_blocks; i++) { // Отработка блоков
			work_of_block(i, func_name);
			send_values(i);
		}
		for (int i = 0; i < count_of_blocks; i++) { // Изменение блоков
			change_block(i, state_name);
		}
	}
	void change_work(string func_name, string state_name) { // 10
		for (int i = 0; i < count_of_blocks; i++) { // Пересылка значений
			send_change_value(i);
		}
		for (int i = 0; i < count_of_blocks; i++) { // Изменение блоков
			change_block(i, state_name);
		}
		for (int i = 0; i < count_of_blocks; i++) { // Отработка блоков
			work_of_block(i, func_name);
			send_values(i);
		}
	}
	void work_and_change (string func_name, string state_name) { // 11
		for (int i = 0; i < count_of_blocks; i++) {
			work_of_block(i, func_name);
			send_values(i);
			
			change_block(calc_next_num(i), state_name); // Изменение блока
		}
	}
	
	void new_print_state() {
		// Запоминаем состояние блоков и входного значения
		vector<double> currentState;
		//currentState.push_back(blocks[0].input_a);
		for (int j = 0; j < count_of_blocks; j++) {
			currentState.push_back(blocks[j].input_a);
			currentState.push_back(blocks[j].state_of_block);
			currentState.push_back(blocks[j].output);
		}	
		int iter = 0;
		for (const auto& value : currentState) {
			if (iter % 3 == 0) {std::cerr << " ";}
			std::cerr << value;
			iter++;
		}
		std::cerr << std::endl;
	}
	void classic_print_state() {
		// Запоминаем состояние блоков и входного значения
		vector<double> currentState;
		//currentState.push_back(blocks[0].input_a);
		currentState.push_back(blocks[0].input_a);
		for (int j = 0; j < count_of_blocks; j++) {
			currentState.push_back(blocks[j].state_of_block);
			currentState.push_back(blocks[j].output);
		}
		for (const auto& value : currentState) {
			std::cerr << value << " ";
		}
		std::cerr << std::endl;
	}
	
	void init_blocks(std::vector<double> init_state) {
		int num_of_pos = 0;
		blocks[0].input_a = init_state[0];
		for (int i = 0; i < blocks.size(); i++) {
			num_of_pos = 1 + i*2;
			blocks[i].state_of_block = init_state[num_of_pos];
			blocks[i].output = init_state[num_of_pos+1];
		}
	}
	
	int work(string func_name, string state_name, bool print, vector<vector<double>>& all_worked_numbers) {
		vector<vector<double>> history; // Для хранения истории состояний всей системы, а не отдельного блока
		int iters = 0;
		while (true) {
			// Запоминаем состояние блоков и входного значения
			vector<double> currentState;
			currentState.push_back(blocks[0].input_a);
			for (int j = 0; j < count_of_blocks; j++) {
				currentState.push_back(blocks[j].state_of_block);
				currentState.push_back(blocks[j].output);
			}
			if (iters > 0) {
				all_worked_numbers.push_back(currentState);
			}
			
			// Проверяем, было ли это состояние ранее
			auto it = std::find_if(history.begin(), history.end(), [&](const vector<double>& state) {return state == currentState;});
			if (print == true) {
				classic_print_state();
			}
			if ((conf1 == 0) && (conf2 == 0)) {
				;
			} else if ((conf1 == 0) && (conf2 == 1)) {
				work_change(func_name, state_name);
			} else if ((conf1 == 1) && (conf2 == 0)) {
				change_work(func_name, state_name);
			} else if ((conf1 == 1) && (conf2 == 1)) {
				work_and_change(func_name, state_name);
			}

			iters++;

			if (it != history.end()) {break;}

			history.push_back(currentState); // Запоминаем текущее состояние
		}
		return iters;
	}
};

double CalcM(vector<double> v) {
    size_t size = v.size();
    if (size == 0)
        return 0;  // Undefined
    else
    {
        sort(v.begin(), v.end());
        if (size % 2 == 0)
            return (v[size / 2 - 1] + v[size / 2]) / 2;
        else
            return v[size / 2];
    }
}

int test_function(double input, double f1, double f2, double f3, int conf1, int conf2, double o1, double o2, double o3, bool print) {
	std::map<std::string, double> params; int count_of_blocks = 3;
	vector<vector<double>> all_numbers;
	string func_name = "RndBern";
	string state_name = "RndBern";

	params["input_a1"] = input; params["input_b1"] = 0; params["state1"] = f1; params["output1"] = o1;
	params["input_a2"] = 0; params["input_b2"] = 0; params["state2"] = f2; params["output2"] = o2;
	params["input_a3"] = 0; params["input_b3"] = 0; params["state3"] = f3; params["output3"] = o3;

	Henkamono henkamono(conf1, conf2, count_of_blocks);
	henkamono.add_block(params["input_a1"], params["input_b1"], params["state1"], params["output1"]);
	henkamono.add_block(params["input_a2"], params["input_b2"], params["state2"], params["output2"]);
	henkamono.add_block(params["input_a3"], params["input_b3"], params["state3"], params["output3"]);

	return henkamono.work(func_name, state_name, print, all_numbers);
}

// Функция для генерации всех двоичных векторов длины n
std::vector<std::vector<double>> generateAllBinaryVectors(int count_of_blocks) {
	int n = 1 + count_of_blocks*2;
	std::vector<std::vector<double>> out;
    int total = 1 << n; // 2^n
    out.reserve(total);
    for (int mask = 0; mask < total; ++mask) {
        std::vector<double> vec(n);
        for (int i = 0; i < n; ++i) {
            vec[i] = (mask >> i) & 1;
        }
		std::reverse(vec.begin(), vec.end());
        out.push_back(vec);
    }
	return out;
}

std::ostream& operator<<(std::ostream& os, const std::vector<double>& v) {
    for (size_t i = 0; i < v.size(); ++i) {
        os << v[i];
	if (i + 1 < v.size()) {os << " ";}
    }
    return os;
}

void not_included_nums(int count_of_blocks) {
	
	vector<vector<double>> all_numbers;
	vector<vector<double>> all_worked_numbers;
	
	all_numbers = generateAllBinaryVectors(count_of_blocks);
	
	std::string func_name = "NegTau";
    std::string state_name = "NegTau";
	for (int conf1 = 0; conf1 < 2; conf1++) {
		for (int conf2 = 0; conf2 < 2; conf2++) {
			if ((conf1 == 0) && (conf2 == 0)) {
				continue;
			}
			for (int i = 0; i < all_numbers.size(); i++) {
				Henkamono henkamono(conf1, conf2, count_of_blocks);
				henkamono.add_blocks(count_of_blocks);
				henkamono.init_blocks(all_numbers[i]);
				//std::cerr << "A B f*1 f f*2 Fi f*3" << std::endl;
				henkamono.work(func_name, state_name, false, all_worked_numbers);
			}
		}
	}
	
	for (int i = 0; i < all_numbers.size(); i++) {
		auto it = find_if(all_worked_numbers.begin(), all_worked_numbers.end(), [&](const vector<double>& number) {
            return number == all_numbers[i];
        });
		//std::cerr << all_numbers[i] << std::endl;
        if (it != all_worked_numbers.end()) {
			std::cerr << all_numbers[i] << std::endl;
        }
	}
}

void print_all_states(int count_of_blocks) {
	vector<vector<double>> all_numbers;
	vector<vector<double>> all_worked_numbers;
	
	all_numbers = generateAllBinaryVectors(count_of_blocks);
	
	std::string func_name = "NegTau";
    std::string state_name = "NegTau";
	for (int conf1 = 0; conf1 < 2; conf1++) {
		for (int conf2 = 0; conf2 < 2; conf2++) {
			if ((conf1 == 0) && (conf2 == 0)) {
				continue;
			}
			for (int i = 0; i < all_numbers.size(); i++) {
				Henkamono henkamono(conf1, conf2, count_of_blocks);
				henkamono.add_blocks(count_of_blocks);
				henkamono.init_blocks(all_numbers[i]);
				std::cerr << "A B f*1 f f*2 Fi f*3" << std::endl;
				henkamono.work(func_name, state_name, true, all_worked_numbers);
			}
		}
	}
}

int main() {

	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	
	not_included_nums(6);
	
	return 0;
}