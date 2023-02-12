#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <thread>
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
#if !NDEBUG
static int            verbosity;
#endif

std::unique_ptr<bool[]> prime_table;

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
#if !NDEBUG
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
#endif
    int        primes = 0;

#if !NDEBUG
    static int most_primes;
#endif
    for (auto n : numbers)
    {
	if (prime_table[n])
	{
	    primes++;
#if !NDEBUG
	    if (verbosity > 2)
		std::cout << n << " is prime and prime=" << prime << std::endl;
#endif
	}
#if !NDEBUG
	if (verbosity && primes > most_primes)
	{
	    most_primes = primes;
	    std::cout << "Most primes: " << most_primes << " for " << n << std::endl;
	}
#endif
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

static void check_numbers(uint64_t num_needed)
{
    for (;;)
    {
	uint64_t p = current++;

	if (p > num_needed)
	{
	    return;
	}
	bool prime = prime_table[p];
	check_for_prime_permutes(prime, p);
	if (p % 1000 == 0)
	{
#if !NDEBUG
	    if (verbosity)
	    {
		std::cout << "Non-primes: " << info[false] << "\n"
		          << "Primes:     " << info[true] << std::endl;
	    }
	    else
#endif
	    {
		std::cout << "." << std::flush;
	    }
	}
    }
}

static void find_primes(uint64_t num_needed)
{
    for (;;)
    {
	uint64_t p = current++;

	if (p > num_needed)
	{
	    return;
	}
	bool prime = is_prime(p);
	prime_table[p] = prime;
    }
}

template<typename FN>
void run_threads(unsigned num_threads, uint64_t num_needed, FN func)
{
    auto thread = std::make_unique<std::thread[]>(num_threads);
    for (unsigned i = 0; i < num_threads; i++)
    {
	thread[i] = std::thread(func, num_needed);
    }
    for (unsigned i = 0; i < num_threads; i++)
    {
	thread[i].join();
    }
}

// Calculate a number that is the next power of 10, higher than or equal to val.
// We then need one more [if original value is a power of 10, so just add it anyway]
static uint64_t calc_num_needed(uint64_t val)
{
    uint64_t n = 1;
    while (n < val)
    {
	n *= 10;
    }
    return n + 1;
}

int main(int argc, char** argv)
{
    int num_threads = 1;
    uint64_t min_value = 1;
    uint64_t max_value = 7780;

    for (int i = 1; i < argc; i++)
    {
	std::string arg = argv[i];
	if (arg == "-t" && argc > i + 1)
	{
	    i++;
	    num_threads = std::stol(argv[i], NULL, 0);
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
#if !NDEBUG
	if (arg == "-v")
	{
	    verbosity++;
	}
#endif
    }
    uint64_t needed = calc_num_needed(max_value);

    prime_table = std::make_unique<bool[]>(needed);

    current = min_value;
    run_threads(num_threads, needed, find_primes);

    current = min_value;
    run_threads(num_threads, max_value, check_numbers);

    std::cout << "\n"
              << "Final summary:" << std::endl;
    std::cout << "Non primes: " << info[false] << std::endl;
    std::cout << "Primes:     " << info[true] << std::endl;
    std::cout << "Total prime number checks: " << totpchk << " and found " << totps << " primes" << std::endl;
}
