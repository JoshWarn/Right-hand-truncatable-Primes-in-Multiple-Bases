#include <vector>

// Sieve of Eranthanso for primes less than or equal to N; Memory intensive, but fast.
std::vector<int> eratosthene(int n)
{
    /*
    * What this does:
    *	- Makes a bool vector with any multipes of prime numbers set to true.
    *   - Copies values from bool vector to seperate int vector
    * What this returns:
    *	- Vector containing primes less than n
    */

    std::vector<bool> numberlist(n);    //false is prime, true is composite.
    std::vector<int> primevector;

    // For every prime int, remove multiples less than n
    for (int p = 2; p * p < n; p++)
    {
        if (numberlist[p] == false)
        {
            for (int i = p * p; i < n; i += p)
                numberlist[i] = true;
        }
    }

    // creating vector list from all remaining primes
    for (int p = 2; p < n; p++)
    {
        if (numberlist[p] == false)
        {
            primevector.push_back(p);
        }
    }
    return primevector;
}

std::vector<int> modulusprimes(std::vector<int> primes_under_base, int base)
{
    std::vector<int> modulusprimes;
    for (int index = 0; index < primes_under_base.size(); index++)
    {
        if (primes_under_base[index] <= base / 2)
        {
            if (base % primes_under_base[index] == 0)
            {
                modulusprimes.push_back(primes_under_base[index]);
            }
            index += 1;
        }
        else
        {
            return modulusprimes;
        }
    }
    return modulusprimes;
}

// Composite Accelerator 1
// Does this even deserve it's own function? Probably not honestly.
bool composite_accelerator_1(int number, std::vector<int> prime_factors)
{
    bool test = true;
    return test;
}

