from time import perf_counter
import spf

lim = 10**8


start = perf_counter()
spf.precompute(lim)
print(perf_counter()-start)

print(spf.nth_prime(10**6))
