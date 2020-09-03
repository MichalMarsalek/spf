# SPF - Smallest prime factors and related stuff
The name of this Python extension (written in C) comes from its main purpose - precalculating *spf(i)*, for *i* in the int32 range (that is *1 <= i < 2^31*).  
After this precalculation is done, one can use the module to get a list of prime numbers in the range, factorisation of any number in the range, *φ(n)* for any *n* in the range and more.

## Installation
One can install the module by running
```
python setup.py install
```
in the source directory.

## What this module is not
This module does **NOT** aim at providing
* [Fast eratosthenes sieve](https://github.com/kimwalisch/primesieve),
* [Eratosthenes sieve for an interval not starting at 0](https://github.com/kimwalisch/primesieve),
* [General or specialiased factorisation algorithm](https://sourceforge.net/p/yafu/wiki/Home/),
* [Fast prime counting function](https://github.com/kimwalisch/primecount),
* (Propabilistic) primality checking.

## Features
All functions require *n* to fit in int64 (that is *1 < n < 2<sup>63</sup>*) since all calculations are performed using this native type.
* ```spf.precalculate(precomputed_n: int) -> None```  
Performs the precalculation up to *precomputed_n < 2<sup>31</sup>*. This *must* be called before any other function is.

* ```spf.spf(n: int) -> int```  
Returns *spf(n)*. Case of *n > precomputed_n* is supported (via trial division). If the result is negative, it means that
no prime in the precomputed range divides it, but it might be a composite.  
This affects other functions,
for example if `precomputed_n = 10**7` then,  
`spf.factors(2 * 3 * 113 * 1125899906842679)` returns `[2, 3, 113, -1125899906842679]` because based on the precomputed primes we don't know if there isn't any divisor of 1125899906842679.  
If `precomputed_n = 10**8` it returns `[2, 3, 113, 1125899906842679]`.

* ```spf.phi(n: int) -> n```  
Calculates *φ(n)*. If the result is negative, it means that its absolute value is the correct value ⇔ n has just one prime factor above the precomputed range.  

* ```spf.pi(n: int) -> n```  
Calculates *π(n)*. If *n > precalculated_n*, performs a new calculation.

* ```spf.factorisation(n: int) -> List[int]```  
Returns prime number factorisation of *n*, with repeated factors.

* ```spf.factors(n: int) -> List[int]```  
Similar to *spf.factorisation* but with distinct factors.

* ```spf.primes() -> List[int]```  
Returns the list of all primes in the precomputed range.  

* ```spf.is_prime(n: int) -> bool```  
Checks whether *n < precomputed_n* is prime.

* ```spf.nth_prime(n: int) -> bool```  
Returns the *n*<sup>th</sup> prime. Only works in the precomputed range, that is *n < π(precomputed_n)*.

* ```spf.spf_table() -> List[int]```  
Returns the internal spf table as a list. Careful, `spf.spf_table()[i]` is the value of *spf(2i+1)*.

## Notation
* *spf(x) =* smallest prime factor of *x*
* [*φ(x)*](https://en.wikipedia.org/wiki/Euler%27s_totient_function) = number of numbers less than *x* that are comprime to it
* [*π(x)*](https://en.wikipedia.org/wiki/Prime-counting_function) = number of primes below *x*

## Timings
Precalculation phase on my laptop took:

| n                | time    |
|------------------|---------|
| 10<sup>6</sup>   | 0.0037s |
| 10<sup>7</sup>   | 0.0522s |
| 10<sup>8</sup>   | 0.6024s |
| 5·10<sup>8</sup> | 2.6884s |

Getting a *φ* value or prime factorisation then takes **less than *1µs*** on average (in the 10<sup>7</sup> range).

## How it works
Internally module holds an array of length *n/2*. At position *i* it stores the smallest prime factor of the number *2i+1*. (This is to that we don't store all those 2s for the evens ⇒ we halve the memory consupmtion. We still waste a lot of space by storing 3s, 5s... but I felt like doing a full wheel sieve would be an overkill).  
The way this array is populated is by a sieving process, similar to the one of the regular [Eratosthenes sieve](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes). Contrary to the Sieve of Eratosthenes, this sieve not only marks out composite numbers, but it also stores the witness of compositeness (the divisor). Furthermore it visits each number exactly once, achieving (slightly) better time complexity than the regular sieve.

## Motivation
Functions like the ones in this module are often usefull for solving [Project Euler problems](https://projecteuler.net). Also I've never coded an extension module in C, so this was a learning project for me to figure out how Python internals work.

## Considerations
This module is not well tested and can produce errors (for large numbers). Generating the SPF table and prime list consumes a nontrivial amount of memory (around 2.5n bytes, that is 2.5 GB for *n=10<sup>9</sup>*).
