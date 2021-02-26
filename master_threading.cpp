#include<filesystem>
#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<thread>
#include<fstream>  
#include<sstream>
#include"file_management.h"
#include<mpir.h>

typedef unsigned int ui;
typedef unsigned long long int ulli;



void master_threading(std::vector<int> base_primes, std::vector<int> modulusprimes, int base, std::string path, std::string sb_ext)
{
	int subbase = 1;
	//std::cout << "This should only be hit once" << std::endl;
	while (subbase > 0)	// While there is an operatable subfile:
	{
		// Getting thread identifier
		std::stringstream ss;
		ss << std::this_thread::get_id();
		uint64_t identifier = std::stoull(ss.str());


		subbase = subbase_selector(base_primes, base, path, sb_ext, identifier);	//Selects starting digit to use.
		std::string printstring = "subbase: " + std::to_string(subbase);
		std::cout << printstring << std::endl;

		if (subbase != 0)
		{
			// Establishes base values
			std::vector<int> digit_vector = { subbase, 1 };	// Starting value to be tested as prime
			int start_vec_size = digit_vector.size();
			bool overflow = false;	// When len = start_vec_size and last digit greater/equal to base.
			// Calculate length of digit_vector to further parallelized the process?
			std::vector<int> digit_data = {};
			ulli prime_count = 0;			// Number of primes in subbase
			ulli digit_count = 0;			// Highest number of digits in a subbase
			ulli digit_sum = 0;
			int probability = 50;			// 1/(2^p) for a composite to return as prime. Maybe move this to main?

			//Establishing misc variables
			int last_digit;
			std::vector<int> largest_prime;	// store largest prime number as a vector? Should I do this?

			mpz_t number_sum;
			mpz_init(number_sum);
			mpz_t dc_digit;
			mpz_init(dc_digit);
			mpz_t power;
			mpz_init(power);
			mpz_t powerbase;				// Used to multiply a digit by powerbase^position
			mpz_init(powerbase);
			mpz_set_ui(powerbase, base);
			gmp_randstate_t state;
			gmp_randinit_default(state);
			mpir_ui digit_position;
			mpir_ui div;


			//Everyting after this needs a rewrite!

			// while number longer than 1 digit   or   last digit is smaller than the base's limit
			while ((digit_vector.size() != start_vec_size) or digit_vector[digit_vector.size() - 1] < base)
			{
				// while last digit equals base   and   number longer than starting vector
				while ((digit_vector[digit_vector.size() - 1] == base) and (digit_vector.size() > start_vec_size))
				{
					if (digit_vector.size() > start_vec_size)
					{
						digit_vector.pop_back();
						last_digit = digit_vector[digit_vector.size() - 1];
						digit_vector.pop_back();
						digit_vector.push_back(last_digit + 1);
					}
				}
				//std::cout << "Size " << digit_vector.size() << std::endl;
				//std::cout << "LaDi " << digit_vector[digit_vector.size() - 1] << std::endl;
				if ((digit_vector.size() == start_vec_size) and (digit_vector[digit_vector.size() - 1] > base - 1))
				{
					overflow = true;
				}

				// IF base%prime = 0 and last digit%prime = 0; must be composite
				bool digit_composite = false;
				for (int mprime = 0; mprime < modulusprimes.size(); mprime++)
				{
					if (digit_vector[digit_vector.size() - 1] % modulusprimes[mprime] == 0)
					{
						digit_composite = true;
					}
				}
				if (digit_composite == true)
				{
					//std::cout << "Composite test is true:" << digit_vector[digit_vector.size() - 1] << std::endl;
				}


				if ((overflow == false) and (digit_composite == false))
				{
					//setting mpz variables
					mpz_set_ui(number_sum, 0);
					mpz_set_ui(dc_digit, 0);
					mpz_set_ui(power, 0);
					div = 0;

					for (ui digit = 0; digit < digit_vector.size(); digit++)
					{
						mpz_set_ui(dc_digit, digit_vector[digit]);			// setting dc_digit to digit_cache[digit]. unsigned int should be safe here.
						digit_position = digit_vector.size() - digit - 1;
						mpz_pow_ui(power, powerbase, digit_position);
						mpz_addmul(number_sum, dc_digit, power);			// number_sum = number_sum + dc_digit * power
					}

					if (mpz_probable_prime_p(number_sum, state, probability, div) == 1)
					{
						digit_sum += digit_vector.size();


						// add return for largest number here

						if (digit_vector.size() > digit_count)
						{
							digit_count = digit_vector.size();
						}

						if (digit_vector.size() >= digit_data.size())
						{
							while (digit_vector.size() > digit_data.size())
							{
								digit_data.push_back(0);
							}
							digit_data.push_back(1);
						}
						else
						{
							digit_data[digit_vector.size()] = digit_data[digit_vector.size()] + 1;
						}

						digit_vector.push_back(1);
						prime_count += 1;

					}
					else
					{
						last_digit = digit_vector[digit_vector.size() - 1];
						digit_vector.pop_back();
						digit_vector.push_back(last_digit + 1);
					}
				}
				else if (digit_composite == true)
				{
					last_digit = digit_vector[digit_vector.size() - 1];
					digit_vector.pop_back();
					digit_vector.push_back(last_digit + 1);
				}
			}
			std::string digit_breakdown = ", ";
			for (int dataiter = 1; dataiter < digit_data.size(); dataiter++)
			{
				digit_breakdown += std::to_string(digit_data[dataiter]) + ", ";
			}

			mpz_clear(number_sum);
			mpz_clear(dc_digit);
			mpz_clear(power);
			mpz_clear(powerbase);

			// Writing file
			//std::cout << "writing file - Subbase = " << subbase << std::endl;
			//std::cout << "Digit Data Size: " << digit_data.size() <<std::endl;
			//for (int i = 0; i < digit_data.size(); i++)
			//{
			//	std::cout << subbase <<" "<< digit_data[i] <<std::endl;
			//}

			subbase_writer(subbase, sb_ext, identifier, prime_count, digit_count, digit_sum, digit_data);
		}
	}
}
