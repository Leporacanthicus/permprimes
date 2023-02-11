#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <pthread.h>
#include <vector>

struct Info
{
    std::atomic<uint64_t> count;
    std::atomic<uint64_t> primes;
};
Info                  info[2];
std::atomic<uint64_t> current;
std::atomic<uint64_t> totpchk;
std::atomic<uint64_t> totps;
static uint64_t       max_value = 7780;
static uint64_t       min_value = 1;
static int            verbosity;

bool is_prime(uint64_t p)
{
    totpchk++;
    if (p == 1)
    {
	return false;
    }
    if (p != 2)
    {
	if ((p % 2) == 0)
	{
	    return false;
	}
	uint64_t max = sqrt(p) + 1;
	for (uint64_t j = 3; j < max; j += 2)
	{
	    if (p % j == 0)
	    {
		return false;
	    }
	}
    }
    totps++;
    return true;
}

void check_for_prime_permutes(bool prime, uint64_t p)
{
    std::vector<int> digits;
    while (p)
    {
	digits.push_back(p % 10);
	p /= 10;
    }
    std::sort(digits.begin(), digits.end());
    std::vector<uint64_t> numbers;
    do
    {
	uint64_t v = 0;
	for (auto n : digits)
	{
	    v = 10 * v + n;
	}
	numbers.push_back(v);
    } while (std::next_permutation(digits.begin(), digits.end()));
    if (verbosity > 1)
    {
	std::cout << "Size=" << numbers.size() << std::endl;
	if (verbosity > 2)
	{
	    for (auto n : numbers)
	    {
		std::cout << n << ", ";
	    }
	    std::cout << std::endl;
	}
    }
    int        primes = 0;
    static int most_primes;
    for (auto n : numbers)
    {
	if (is_prime(n))
	{
	    primes++;
	    if (verbosity > 2)
		std::cout << n << " is prime and prime=" << prime << std::endl;
	}
	if (verbosity && primes > most_primes)
	{
	    most_primes = primes;
	    std::cout << "Most primes: " << most_primes << " for " << n << std::endl;
	}
    }
    info[prime].count++;
    info[prime].primes.fetch_add(primes);
}

std::ostream& operator<<(std::ostream& os, const Info& info)
{
    os << "Primes " << info.primes << " from " << info.count << " numbers " << std::fixed
       << std::setprecision(3) << static_cast<double>(info.primes) / info.count;
    return os;
}

static void* find_prime(void*)
{
    for (;;)
    {
	uint64_t p = current++;

	if (p > max_value)
	{
	    pthread_exit(NULL);
	}
	bool prime = is_prime(p);
	check_for_prime_permutes(prime, p);
	if (p % 1000 == 0)
	{
	    if (verbosity)
	    {
		std::cout << "Non-primes: " << info[false] << "\n"
		          << "Primes:     " << info[true] << std::endl;
	    }
	    else
	    {
		std::cout << "." << std::flush;
	    }
	}
    }
}

int main(int argc, char** argv)
{
    int threads = 1;

    for (int i = 1; i < argc; i++)
    {
	std::string arg = argv[i];
	if (arg == "-t" && argc > i + 1)
	{
	    i++;
	    threads = std::stol(argv[i], NULL, 0);
	}
	if (arg == "-e" && argc > i + 1)
	{
	    i++;
	    max_value = std::stol(argv[i], NULL, 0);
	}
	if (arg == "-s" && argc > i + 1)
	{
	    i++;
	    min_value = std::stol(argv[i], NULL, 0);
	}
	if (arg == "-v")
	{
	    verbosity++;
	}
    }

    current = min_value;

    pthread_t* thread_id = new pthread_t[threads];
    for (int i = 0; i < threads; i++)
    {
	int rc = pthread_create(&thread_id[i], NULL, find_prime, NULL);
	if (rc != 0)
	{
	    std::cerr << "Huh? Pthread couldn't be created. rc=" << rc << std::endl;
	}
    }
    for (int i = 0; i < threads; i++)
    {
	pthread_join(thread_id[i], NULL);
    }

    std::cout << "\n"
              << "Final summary:" << std::endl;
    std::cout << "Non primes: " << info[false] << std::endl;
    std::cout << "Primes:     " << info[true] << std::endl;
    std::cout << "Total prime number checks: " << totpchk << " and found " << totps << " primes" << std::endl;
}
