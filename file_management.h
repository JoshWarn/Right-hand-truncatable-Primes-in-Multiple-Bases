#pragma once
#include<vector>
#include<string>
int subbase_selector(std::vector<int> base_primes, int base, std::string path, std::string ext, uint64_t identifier);
void subbase_writer(int subbase, std::string extension, uint64_t identifier, int primecount, int digitcount, int digitsum, std::vector<int> distribution);
void base_writer(int base, std::string path, std::string ext, std::string b_filename, std::string b_ext);
int base_reader(std::string filename, std::string extension);
void subbase_cleaner(std::string path, std::string sb_ext);
