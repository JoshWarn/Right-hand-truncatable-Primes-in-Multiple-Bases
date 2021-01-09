#include <iostream> 
#include <thread>
#include<array>
#include"threaded.h"
#include<string>
#include<vector>
#include<fstream>
#include <filesystem>

typedef unsigned long long int ulli;
typedef unsigned int ui;

int main() {

	using namespace std;
	static const int thread_count = 12;										// Number of threads to use
	thread t[thread_count];
	ui const end_base = 1000;									// Number of bases to calculate RHT primes. Starts at 3.
	int start_base = 3;										// Default starting base - can be overwritten if base is detected in existing data file
	std::vector<string> string_cache(thread_count);
	string string_matrix[thread_count];

	auto start = std::chrono::high_resolution_clock::now();		// starting timer

	if (std::filesystem::exists("data.csv")) {
		fstream data("data.csv", ios::in);
		string line;
		std::getline(data, line);				// Throws away header
		while (getline(data, line)) {
			std::cout << line << endl;
			std::string::size_type pos = line.find(', ');	// finds location of first ', '

			if (pos != std::string::npos) {
				start_base = std::stoi(line.substr(0, pos)) + 1;
			}
		}
		data.close();
		std::cout << "Starting Base = " << start_base << endl;
	}
	else {
		fstream data("data.csv", ios::app);
		data << "base, prime count, digit count, digit sum, largest prime" << endl;
		data.close();
		std::cout << "File DNE or Empty: Starting_base = 3" << endl;
	}

	int base = start_base;

	while (base < end_base) {
		cout << "Base Chunk: "<< base << " to " << base + thread_count << endl;
		for (int thread = 0; thread < thread_count; thread++) {
			int tested_base = base + thread;
			t[thread] = std::thread(threading, tested_base);
		}

		for (int thread = 0; thread < thread_count; thread++) {
			t[thread].join();
		}
		base += thread_count;

		fstream data("data.csv", ios::app);
		for (int thread = 0; thread < thread_count; thread++) {
			fstream cache(to_string(thread) + ".csv", ios::in);
			string line;
			std::getline(cache, line);
			std::cout << line << endl;
			data << line << endl;
		}
		data.close();
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto elapse = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	std::cout << "Total Time Elapsed: " << elapse << endl;

}

