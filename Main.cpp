#include"master_threading.h"
#include"file_management.h"
#include"math_functions.h"
#include<vector>
#include<thread>
#include<iostream>

int main()
{
	int const thread_count = 12;												// thread count (1, inf)
	int base_start = 75;															// starting base (2, )? Maybe start with 3.
	int base_end = 75;															// ending base
	int base = base_start;
	std::string path("C:/Users/source/repos/RHTP_6/RHTP_6");	// working directory
	std::string sb_ext(".subbase");												// extension for subbase files
	std::string b_ext(".csv");													// extension for base files
	std::string b_filename("basedata");											// Name of combined base data file


	std::thread t[thread_count];
	// starting base number 
	int file_base = base_reader(b_filename, b_ext);

	// TODO: Add code that removes and empty subfiles. Allows for proper continuation of a base.

	if (file_base > base_start)
	{
		base = file_base;
	}

	// Main Loop:
	for (base ; base <= base_end; base++)
	{
		std::cout << "Base = " << base << std::endl;
		//Generate list of all primes less than base
		std::vector<int> subprimes = eratosthene(base);
		//List of all primes less than half base
		std::vector<int> modprimes = modulusprimes(subprimes, base);

		// Opening threads
		for (int thread = 0; thread < thread_count; thread++)
		{
			//std::cout << "thread " << thread + 1 << std::endl;
			t[thread] = std::thread(master_threading, subprimes, modprimes, base, path, sb_ext);
		}

		// Closing threads
		for (int thread = 0; thread < thread_count; thread++)
		{
			t[thread].join();
		}


		// Combining the data from each subfile into a major file
		base_writer(base, path, sb_ext, b_filename, b_ext);
		// removing all subfilies

	}
	std::cout << "Task Completed" << std::endl;
}
