from time import perf_counter
import spf

lim = 10**7


start = perf_counter()
spf.precompute(lim)
print(perf_counter()-start)

print(spf.factors(2 * 3 * 113 * 1125899906842679))