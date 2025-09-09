#include <cstdlib>  // Для функции rand() и srand()
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <array>
#include <map>

using namespace std;

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
	
	void logical_function(string func_name) {
		if (func_name == "NegTau") {
			Block_NegTau_logical_function();
		} else if (func_name == "ConPir") {
			Block_ConPir_logical_function();
		}
	}
	void change_state(string state_name) {
		if (state_name == "NegTau") {
			Block_NegTau_change_state();
		} else if (state_name == "ConPir") {
			Block_ConPir_change_state();
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
			std::cerr << value;
		}
		std::cerr << std::endl;
	}
	
	int work(string func_name, string state_name) {
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
			// Проверяем, было ли это состояние ранее
			auto it = std::find_if(history.begin(), history.end(), [&](const vector<double>& state) {return state == currentState;});
			classic_print_state();
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

void test_function(int input, int f1, int f2, int f3, int conf1, int conf2, int o1, int o2, int o3) {
	std::map<std::string, double> params; int count_of_blocks = 3;
	string func_name = "NegTau";
	string state_name = "NegTau";

	params["input_a1"] = input; params["input_b1"] = 0; params["state1"] = f1; params["output1"] = o1;
	params["input_a2"] = 0; params["input_b2"] = 0; params["state2"] = f2; params["output2"] = o2;
	params["input_a3"] = 0; params["input_b3"] = 0; params["state3"] = f3; params["output3"] = o3;

	Henkamono henkamono(conf1, conf2, count_of_blocks);
	henkamono.add_block(params["input_a1"], params["input_b1"], params["state1"], params["output1"]);
	henkamono.add_block(params["input_a2"], params["input_b2"], params["state2"], params["output2"]);
	henkamono.add_block(params["input_a3"], params["input_b3"], params["state3"], params["output3"]);

	henkamono.work(func_name, state_name);
}

int main() {

	// std::srand(static_cast<unsigned int>(std::time(nullptr)));

	vector<int> values;
	// vector<int> probs = {0, 100};

	int odd, max;
	int conf1 = 1; int conf2 = 1;
	// input, f1, f2, f3, conf1, conf2, o1, o2, o3
	// test_function(0, 1, 0, 1, conf1, conf2, 0, 0, 1);

	for (int conf1 = 0; conf1 < 2; conf1++) {
		for (int conf2 = 0; conf2 < 2; conf2++) {
			for (int input = 0; input < 2; input++) {
				for (int f1 = 0; f1 < 2; f1++) {
					for (int f2 = 0; f2 < 2; f2++) {
						for (int f3 = 0; f3 < 2; f3++) {
							for (int o1 = 0; o1 < 2; o1++) {
								for (int o2 = 0; o2 < 2; o2++) {
									for (int o3 = 0; o3 < 2; o3++) {
										if ((conf1 == 0) && (conf2 == 0)) {
											;
										} else {
											std::cerr << "i:" << input << " c:" << conf1 << conf2 << " f:" << f1 << f2 << f3 << " o:" << o1 << o2 << o3 << std::endl;
											test_function(input, f1, f2, f3, conf1, conf2, o1, o2, o3);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	/*
	std::cerr << "Count of all cycles = " << all << std::endl;
	std::cerr << "Medium value = " << sum / all << std::endl;
	std::cerr << "Mediana value = " << CalcM(values) << std::endl;
	*/
	
	return 0;
}