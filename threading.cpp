#include<iostream> // used for cout
#include<vector>	// used for digit_cache
#include<mpir.h>

typedef unsigned long long int ulli;
typedef unsigned int ui;

void threading(ui base) {

	ulli prime_count = 0;					// How many primes in a base
	ulli digit_count = 0;					// Max digits in a base
	ulli digit_sum = 0;						// Sum of all digits in a base
	ui last_digit;							// Last digit of digit_cache
	bool single_digit = false;				// If number is one digit long

	mpz_t largest_prime;					// Creating/initalizing largest_prime
	mpz_init(largest_prime);				// Using mpir because is expected > 2^64.

	int probability = 500;					// 1/(2^p) probability for composite to return as prime;

	mpz_t number_sum;
	mpz_init(number_sum);

	mpz_t dc_digit;
	mpz_init(dc_digit);

	mpz_t power;
	mpz_init(power);

	mpz_t fbase;
	mpz_init(fbase);
	mpz_set_ui(fbase, base);

	gmp_randstate_t state;
	gmp_randinit_default(state);

	mpir_ui digit_position;
	mpir_ui div;

	// Hardcoded thread stuff because I'm dumb.
	// Was unable to pass in thread paramater, so instead calculates 
	// expected thread based on the calculated base
	ui thread_count = 12;					// Thread count used
	ui thread = (base - 3) % thread_count;	// Expect thread

	std::vector<ui> digit_cache = { 2 };	//max base is 2^32; perhaps reduce this to short int?
											//Starts first digit as "2", as it is the lowest prime.

	// while number longer than 1 digit   or   last digit is smaller than the base's limit
	while ((digit_cache.size() != 1) or (digit_cache[digit_cache.size() - 1] < base)) {

		// an if statement should work here, but using while is safer. Maybe change in future revisions?
		// while last digit equals base   and   number longer than 1 digit
		while ((digit_cache[digit_cache.size() - 1] == base) and (digit_cache.size() > 1)) {

			// If one digit long, enable single_digit
			//TODO: Remove this through more elegant code <----------
			if (digit_cache.size() > 1) {							// If num > 1 digit and last digit is max:
				digit_cache.pop_back();								// remove a digit and increment new last
				last_digit = digit_cache[digit_cache.size() - 1];	// digit by one
				digit_cache.pop_back();
				digit_cache.push_back(last_digit + 1);

			}

		}

		if ((digit_cache.size() == 1) and (digit_cache[digit_cache.size() - 1] > base - 1)) {
			single_digit = true;
			//std::cout << "Is single digit ending" << std::endl;
		}

		// Replace with (digit_cache.size() == 1) ? I think it has to be a saved state from earlier, so that wouldn't work.
		if (single_digit == false) {
			// Start of new modified code with mpir

			// setting mpz variables
			mpz_set_ui(number_sum, 0);
			mpz_set_ui(dc_digit, 0);
			mpz_set_ui(power, 0);
			div = 0;

			// for each digit in digit_cache;
			for (ui digit = 0; digit < digit_cache.size(); digit++) {
				mpz_set_ui(dc_digit, digit_cache[digit]);							// setting dc_digit to digit_cache[digit]. unsigned int should be safe here.
				digit_position = digit_cache.size()- digit - 1;

				mpz_pow_ui(power, fbase, digit_position);
				//mpz_set_ui(power, std::pow(base, digit_cache.size() - digit - 1)); // pow(base, digit_cache.size() - digit - 1);			<--- isue with unsigned int?
				mpz_addmul(number_sum, dc_digit, power); // number_sum = number_sum + dc_digit * power

			}

			//std::cout << "Tested Number: " << number_sum << std::endl;
			if (mpz_probable_prime_p(number_sum, state, probability, div) == 1){
				digit_sum += digit_cache.size();
				//std::cout<< "test" << std::endl;
				//std::cout << number_sum << std::endl;
				if (number_sum > largest_prime) {		// if largest number in base recorded
					std::cout << largest_prime << std::endl;
					std::cout << "test" << std::endl;
				}
				if (digit_cache.size() > digit_count) {
					digit_count = digit_cache.size();
				}
				//std::cout << "Tested Prime: " << number_sum << std::endl;


				digit_cache.push_back(1);
				prime_count += 1;

			}
			else {
				last_digit = digit_cache[digit_cache.size() - 1];
				digit_cache.pop_back();
				digit_cache.push_back(last_digit + 1);
			}
			//mpz_clear(number_sum);
			//mpz_clear(dc_digit);
			//mpz_clear(power);
			//mpz_clear(*state); how to clear?
			//mpz_clear(*div); how to clear?
		}
	}
	std::cout << "Base Tested: " << base << "   " << prime_count << "   " << digit_count << std::endl;
	mpz_clear(number_sum);
	mpz_clear(dc_digit);
	mpz_clear(power);
	mpz_clear(fbase);
}