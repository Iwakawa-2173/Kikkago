#include <cstdlib>  // Для функции rand() и srand()
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <array>

using namespace std;

class Block {
public:
	double input_a;
	double input_b;
	double state_of_block;
	double output;
	virtual void change_state() {}
	virtual void logical_function() {}
	
	Block(double p_input_a, double p_input_b, double p_state_of_block, double p_output) {
		input_a = p_input_a;
		input_b = p_input_b;
		state_of_block = p_state_of_block;
		output = p_output;
	}
	
	Block() : input_a(0), input_b(0), state_of_block(0), output(0) {}

};

class Block_NegTau : public Block {
public:
	void change_state() override {
		state_of_block = abs(state_of_block - input_b);
	}
	void logical_function() override {
		output = abs(state_of_block - input_a);
	}
};

class Block_ConPir : public Block {
public:
	void change_state() override {
		state_of_block = (state_of_block - input_a)*(state_of_block - input_b);
	}
	void logical_function() override {
		state_of_block = (state_of_block - input_a)*(state_of_block - input_b);
	}
};

class Henkamono {
public:
    int count_of_blocks; double input; int conf1; int conf2; int prob; int print;
    vector<Block> blocks;

    Henkamono(int p_input, int p_conf1, int p_conf2, int p_prob, int p_count_of_blocks, int p_print)
    : input(p_input), conf1(p_conf1), conf2(p_conf2), prob(p_prob), print(p_print), count_of_blocks(p_count_of_blocks), blocks(p_count_of_blocks){}
	
	int calc_next_num(int n) {return ((n+1) % count_of_blocks) + 1;}

	int calc_prev_num(int n) {return ((n-1) % count_of_blocks) - 1;}
	
	void work_change() {
		for (int i = 0; i < count_of_blocks; i++) { // Отработка блоков
			blocks[i].logical_function();
			blocks[calc_next_num(i)].input_b = blocks[i].output; // Передача изменяющего значения
			blocks[(i % (count_of_blocks-1))].input_a = blocks[i].output;
		}
		for (int i = 0; i < count_of_blocks; i++) { // Изменение блоков
			blocks[i].change_state();
		}
	}
	void change_work() {
		for (int i = 0; i < count_of_blocks; i++) { // Изменение блоков
			blocks[i].change_state();
		}
		for (int i = 0; i < count_of_blocks; i++) { // Отработка блоков
			blocks[i].logical_function();
			blocks[calc_next_num(i)].input_b = blocks[i].output; // Передача изменяющего значения
			blocks[(i % (count_of_blocks-1))].input_a = blocks[i].output;
		}
	}
	void work_and_change () {
		for (int i = 0; i < count_of_blocks; i++) {
			blocks[i].logical_function(); // Отработка блока
			blocks[calc_next_num(i)].input_b = blocks[i].output; // Передача изменяющего значения
			blocks[(i % (count_of_blocks-1))].input_a = blocks[i].output;
			
			blocks[i].change_state(); // Изменение блока
		}
	}
	
	int work(int conf1, int conf2, int cyc) {
		vector<vector<double>> history; // Для хранения истории состояний
		int iters = 0;
		std::vector<double> args(count_of_blocks + 1);
		while (true) {
			if ((conf1 == 0) && (conf2 == 0)) {
				return iters;
			} else if ((conf1 == 0) && (conf2 == 1)) {
				work_change();
			} else if ((conf1 == 1) && (conf2 == 0)) {
				change_work();
			} else if ((conf1 == 1) && (conf2 == 1)) {
				work_and_change();
			}
			input = blocks[count_of_blocks-1].output;
			
			// Запоминаем состояние блоков и входного значения
			vector<double> currentState;
			for (int j = 0; j < count_of_blocks-1; j++) {
				currentState.push_back(blocks[j].state_of_block);
				currentState.push_back(blocks[j].output);
			}
			currentState.push_back(input);
			// Проверяем, было ли это состояние ранее
			auto it = std::find_if(history.begin(), history.end(), [&](const vector<double>& state) {return state == currentState;});
			
			if (print == 1) {
				for (const auto& value : currentState) {
					std::cerr << value << " ";
				}
				std::cerr << std::endl;
			}

			iters++;

			if (it != history.end()) {break;}

			history.push_back(currentState); // Запоминаем текущее состояние

			// Устанавливаем новое входное значение для следующей итерации
			if (cyc == 1) {input = blocks[count_of_blocks-1].output;}
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

int main() {
	
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	
	vector<int> values;
	vector<int> probs = {0, 100};

	int input, f1, f2, f3, odd, conf1, conf2, cyc, prob, o1, o2, o3, max;
	
	vector<int> f = {f1, f2, f3};
	vector<int> o = {o1, o2, o3};
	
	int value = 0;
	float sum = 0;
	float all = 0;
	cyc = 1;
	for(int prob : probs) {
		for (int conf1 = 0; conf1 < 2; conf1++) {
			for (int conf2 = 0; conf2 < 2; conf2++) {
				for (input = 0; input < 2; input++) {
					for (f1 = 0; f1 < 2; f1++) {
						for (f2 = 0; f2 < 2; f2++) {
							for (f3 = 0; f3 < 2; f3++) {
								for (o1 = 0; o1 < 2; o1++) {
									for (o2 = 0; o2 < 2; o2++) {
										for (o3 = 0; o3 < 2; o3++) {
											/*
											if (max < henkamono(input, f1, f2, f3, cyc, conf1, conf2, prob, o1, o2, o3, 0)) {
												std::cerr << "Conf: " << conf1 << conf2 << std::endl;
												std::cerr << "State:" << input << f1 << f2 << f3 << o1 << o2 << o3 << std::endl;
												max = henkamono(input, f1, f2, f3, cyc, conf1, conf2, prob, o1, o2, o3, 1);
												std::cerr << "____________________" << std::endl;
											}
											
											std::cerr << "Prob: " << prob << std::endl;
											std::cerr << "Conf: " << conf1 << conf2 << std::endl;
											std::cerr << "State:" << input << f1 << f2 << f3 << o1 << o2 << o3 << std::endl;
											value = henkamono(input, f1, f2, f3, cyc, conf1, conf2, prob, o1, o2, o3, 1);
											sum += value;
											values.push_back(value);
											std::cerr << "____________________" << std::endl;
											all++;
											*/
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