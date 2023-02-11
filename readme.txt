Counter for primes formed by the permutations of number.

This is a small program I wrote quite quickly to calculate the
number of permutations from a given number, that are prime numbers.

It tracks whether the original number is a prime number or a non-prime,
and reports separately for the two categories. This was to find the
answer to a question on Quora, that asked if it is more likely that
the permutations of a prime number is more likely to be prime than
the non-prime numbers.

I'm sure some mathematician has a wonderful proof that this is the
case.

Note that this code is quite quickly produced, it is by FAR not
the most optimial solution. It will repeatedly check if a particular
number is a prime or not. This is to keep the code relatively simple
and be able to multithread the process. Keeping a table of "known primes
up to X" would be a very effective way to speed things up.

I may indeed update this in the future to optimise it. Or it may
remain as is.

It need C++20 compatible headers, as it is using std::next_permutation.

It is also using pthreads. It probably should use C++ std::thread instead,
but I took some existing code using pthreads that I've had for many years
as a starting point.
