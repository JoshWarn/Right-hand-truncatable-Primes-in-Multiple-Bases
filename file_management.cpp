#include<filesystem>
#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<thread>
#include <fstream>  
#include <sstream>



int subbase_selector(std::vector<int> base_primes, int base, std::string path, std::string ext, uint64_t identifier)
{
	/*
	* What this does:
	*	- scans for all .temp files in a directory
	*	- breaks .temp file names into integer designation by "-".
	*	- adds a .temp file that has a prime value larger than all other ints
	*	- checks if .temp has already been created by a different thread - allowing for unrestricted parallelization within a base.
	* What this returns:
	*	- Returns thread_prime integer
	*	- Returns 0 if all others already used.
	* TODO:
	*	- Move the file extension outside of the script - as it's used in other defs (eg. File writer)
	*/

	//Manual Inputs
	int check_count = 10;				// file scan count for redundancy. 5 seems to be safe.


	int thread_prime = base_primes[0];
	bool subfile_selected = false;

	// getting thread identifier

	while (subfile_selected == false)
	{
		//check for highest text file name created
		int highest_subfile_int = 0;
		for (auto& p : std::filesystem::recursive_directory_iterator(path))
		{
			if (p.path().extension() == ext)
			{
				std::string subfile_allname = p.path().stem().string();
				std::string::size_type pos = p.path().stem().string().find('-');	//trashing everything but num
				if (pos != std::string::npos) {

					int subfile_int = std::stoi(subfile_allname.substr(0, pos));

					if (highest_subfile_int <= subfile_int)	// for some reason, using only "<" doesn't work - must be "<=". Inefficient.
					{
						highest_subfile_int = subfile_int;
					}
				}
			}
		}

		if (highest_subfile_int == base_primes[base_primes.size() - 1])
		{
			return 0;
		}

		// Getting thread_prime value from subfile int
		if (highest_subfile_int == 0)	// if no subbases exist
		{
			thread_prime = 2;
		}

		else							// if subbase exists
		{
			int subfile_index;
			auto finder = std::find(base_primes.begin(), base_primes.end(), highest_subfile_int);
			if (finder != base_primes.end())
			{
				subfile_index = finder - base_primes.begin();

				if (subfile_index < base_primes.size())
				{
					thread_prime = base_primes[subfile_index + int(1)];	// if index is out of range, it'll set to 0 I think.
				}
			}
			else
			{
				std::cout << "What is going on" << std::endl;
			}
		}

		// if digit > base, return 0.
		if (thread_prime == 0 or thread_prime >=  base)
		{
			return 0;
		}

		//make temporary file with identifier
		std::string temp_filename = std::to_string(thread_prime) + "-" + std::to_string(identifier) + ext;
		std::ofstream request(temp_filename);
		request.close();

		// re-scan for files with base
		int check_iteration = 0;
		bool smallest_number = true;
		while ((check_iteration < check_count) and (smallest_number == true))
		{
			for (auto& p : std::filesystem::recursive_directory_iterator(path))
			{
				if (p.path().extension() == ext)
				{
					std::string subfile_allname = p.path().stem().string();
					std::string::size_type pos = subfile_allname.find('-');	//trashing everything but num
					if (pos != std::string::npos) {
						std::string::size_type end = subfile_allname.find_last_of(subfile_allname.back());
						if (thread_prime == std::stoi(subfile_allname.substr(0, pos)))
						{
							uint64_t fileident = std::stoi(subfile_allname.substr(pos + 1, end));
							if (identifier > fileident)
							{
								smallest_number = false;
							}
						}
					}
				}
			}
			check_iteration += 1;
		}

		if (smallest_number == true) // exit loop
		{
			subfile_selected = true;
			return thread_prime;
			//std::cout << thread_prime << std::endl;
		}
		else	// remove last created file.
		{
			std::filesystem::remove(temp_filename);
		}
	}
}


void subbase_writer(int subbase, std::string extension, uint64_t identifier, int primecount, int digitcount, int digitsum, std::vector<int> distribution)
{
	/*
		* What this does:
		*	- Populate a file with the subbase data
		* What this returns:
		*	- Nothing.
		* TODO:
		*	- Everything
		*/
	std::fstream subbasedata(std::to_string(subbase) + "-" + std::to_string(identifier) + extension, std::ios::app);
	std::string appendedstring = std::to_string(primecount) + " " + std::to_string(digitcount) + " " + std::to_string(digitsum);
	for (int index = 1; index < distribution.size(); index++)
	{
		appendedstring = appendedstring + " " + std::to_string(distribution[index]);
		//std::cout << distribution[index] << std::endl;
	}

	subbasedata << appendedstring << std::endl;
	subbasedata.close();


}

int base_reader(std::string filename, std::string extension)
{
	/*
	* What this does:
	*	- Read in base file to see what base to start calculations.
		- If no base file exists, it will crease one.
	* What this returns:
	*	- base to start from
	* TODO:
	*	- Everything
	*/
	int from_file_base = 0;
	if (std::filesystem::exists(filename + extension)) {
		std::fstream data(filename + extension, std::ios::in);
		std::string line;
		std::getline(data, line);				// Throws away header
		while (getline(data, line)) {
			//std::cout << line << std::endl;
			std::string::size_type pos = line.find(', ');	// finds location of first ', '

			if (pos != std::string::npos) {
				from_file_base = std::stoi(line.substr(0, pos)) + 1;
			}
		}
		data.close();
	}
	else {
		std::fstream data(filename + extension, std::ios::app);
		data << "base, prime count, digit count, digit sum, largest prime" << std::endl;
		data.close();
	}
	return from_file_base;
}

void base_writer(int base, std::string path, std::string ext, std::string b_filename, std::string b_ext)
{
	/*
	* What this does:
	*	- Reads in each subbase file
	*	- Combines data into single set
	*	- Appends data into main data file
	* What this returns:
	*	- Nothing.
	* TODO:
	*	- implement largest prime per base
	*	- Has to print out string
	*/
	unsigned long long int prime_count = 0;
	unsigned long long int digit_max = 0;
	unsigned long long int digit_sum = 0;
	std::vector<int> digit_breakdown = { 0 };
	std::vector<int> file_vector = {};
	// std::vector<int> largest_prime = {};

	for (auto& p : std::filesystem::recursive_directory_iterator(path))
	{
		std::vector<std::string> line_character_vector = {};
		std::vector<std::string> line_string_vector = {};

		if (p.path().extension() == ext)
		{
			// primecount, digitcount, digitsum, distribution
			// read line in
			std::fstream subbasedata(p, std::ios::in);
			std::string line;
			std::getline(subbasedata, line);
			//std::cout << line << std::endl;

			std::string word = "";

			for (int i = 0; i < line.length();++i)
			{
				if (line[i] == ' ')
				{
					line_string_vector.push_back(word);
					word = "";
				}
				else
				{
					word = word + line[i];
				}
			}
			line_string_vector.push_back(word);	// Catches last word that has no space after it.

			for (int num = 0; num < line_string_vector.size(); num++)
			{
				if (num == 0)	//prime count
				{
					prime_count += std::stoi(line_string_vector[num]) + 1;	// + 1 is for the unincluded subfile.
				}
				else if (num == 1)	// digitcount
				{
					int subbase_max = std::stoi(line_string_vector[num]);
					if (subbase_max > digit_max)
					{
						digit_max = subbase_max;
					}

				}
				else if (num == 2) // digitsum
				{
					digit_sum += std::stoi(line_string_vector[num]) + 1;
				}
				else if (num == 3)
				{
					digit_breakdown[0] = digit_breakdown[0] + 1;
				}
				else if (num > 3) // distribution
				{
					if (digit_breakdown.size() <= num - 3)
					{
						digit_breakdown.push_back(0);
					}
					digit_breakdown[num - 3] = digit_breakdown[num - 3] + std::stoi(line_string_vector[num]);
				}
			}
		}
	}
	std::string base_append_string;
	base_append_string = std::to_string(base) + " " + std::to_string(prime_count) + " " +
		std::to_string(digit_max) + " " + std::to_string(digit_sum);

	for (int item = 0; item < digit_breakdown.size(); item++)
	{
		base_append_string = base_append_string + " " + std::to_string(digit_breakdown[item]);
	}

	std::fstream data(b_filename + b_ext, std::ios::app);
	data << base_append_string <<std::endl;
	data.close();

	for (auto& p : std::filesystem::recursive_directory_iterator(path))
	{
		if (p.path().extension() == ext)
		{
			std::filesystem::remove(p);
		}
	}


}
