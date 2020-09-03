#include <Python.h>
#include <stdbool.h>

// ---------------- C procedures --------------------- //

long * spf; //spf[i] stores the smallest prime factor of 2i+1
long * primes;
long primes_count;
long precomputed_n;
long smallPrimes[100] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541};

/*
Creates array of length n//2, (and remembers n). Sets ith element to spf(2i+1).
Benchmarks:
	n = 10^6:   0.0037s
	n = 10^7:   0.0522s
	n = 10^8:   0.6024s
	n = 5*10^8: 2.6884s
Algo: A Linear Sieve Algorithm for Finding Prime Numbers
https://cp-algorithms.com/algebra/prime-sieve-linear.html
*/
static void _precompute(long n){
	precomputed_n = n;
	long n_half = n>>1;
	spf = malloc(sizeof(long) * n_half);
	long primes_alloc = n;
	if(n >= 100000)      primes_alloc = n / 10;
	if(n >= 10000000)   primes_alloc = n / 15;
	if(n >= 200000000)  primes_alloc = n / 18;
	if(n >= 520000000)primes_alloc = n / 19;
	if(n >= 1000000000)primes_alloc = n / 19.6666; //TODO: breaks for n this large idk why
	primes = malloc(sizeof(long) * primes_alloc);
	primes[0] = 2;
	primes_count = 1;	
    
	for(long i = 0; i < n_half; i++){
		spf[i] = 0;
	}
	
	for (long i=1; i<n_half; i++) {
		long num = (i<<1)^1; //2i+1
		//saving prime
		if (spf[i] == 0) {
			spf[i] = num;
			primes[primes_count++] = num;
		}
		//iterating tru odd primes
		for (long j=1; j<primes_count; ++j){
			long prime = primes[j];
			if(prime > spf[i])
				break;
			long multiple = num*prime;
			if(multiple > n)
				break;
			spf[multiple>>1] = prime;
		}
	}
}

/*
Returns spf(n) (smallest prime factor of n) provided n is odd.
Case of n > precomputed_n is supported (via trial division), provided spf(n) < precomputed_n.
*/
//TODO: use all primes
static long long _spfOdd(long long n){
	//iterate tru odd primes upto sqrt(n) and return the first one that divides n
	if(n > precomputed_n){
		for(long i = 1; i < primes_count; i++){
			long long prime = primes[i];
			if(n % prime == 0){
				return prime;
			}
			if(prime*prime > n){
				return n;
			}
		}
		return -n;
	}
	return spf[n>>1]; //n is odd and small => we have it precomputed
}
/*
Returns spf(n).
Case of n > precomputed_n is supported (via trial division), provided spf(n) < precomputed_n.
*/
//TODO: use all primes
static long long _spf(long long n){
	if(n & 1){
		return _spfOdd(n);
	}
	return 2L;
}


/*
Calculates phi(n) using spf.
*/
//TODO: This might overflow for n > 2^31
static long long _phi(long long n){
	long long result = n;
	if((n&1)^1){
		result >>= 1;
		while((n&1)^1){
			n >>= 1;
		}
	}
	while(n > 1){
		long p = _spfOdd(n);
		while(n % p == 0){
			n /= p;
		}
		result = result * (long long)(p-1) / p;
	}
	return result;
}

/*
Returns pi(n).
*/
static long _pi(long n){
	if(n == precomputed_n){
		return primes_count;
	}
	if(precomputed_n < n+1){
		_precompute(n+1);
	}
	for(long i = 0; i <= primes_count; i++) {
		if(primes[i] > n)
			return i;
	}
	return primes_count;
}

// ----------------- Python functions --------------- //

/*
Returns spf array as a Python list.
*/
static PyObject* spf_table(PyObject* self, PyObject* args){
	PyObject* result = PyList_New(precomputed_n);
	for(long i = 0; i < precomputed_n>>1; i++) {
		PyList_SetItem(result, i, PyLong_FromLong(spf[i]));
	}
	return result;
}

/*
Returns spf(n).
*/
static PyObject* py_spf(PyObject* self, PyObject* args){
	long long n;
	if(!PyArg_ParseTuple(args, "L", &n))
        return NULL;
	return PyLong_FromLongLong(_spf(n));
}

/*
Returns phi(n).
*/
static PyObject* phi(PyObject* self, PyObject* args){
	long long n;
	if(!PyArg_ParseTuple(args, "L", &n))
        return NULL;
	return PyLong_FromLongLong(_phi(n));
}

/*
Returns prime factorisation of n as Python list (factors are repeated).
*/
static PyObject* factorisation(PyObject* self, PyObject* args){
	long long i;
	if(!PyArg_ParseTuple(args, "L", &i))
        return NULL;
	PyObject* result = PyList_New(0);
	while((i&1)^1){
		PyList_Append(result, PyLong_FromLong(2L));
		i >>= 1;
	}
	while(i > 1){
		long long factor = _spfOdd(i);		
		i /= factor;		
		PyList_Append(result, PyLong_FromLongLong(factor));
	}
	return result;
}

/*
Returns prime factors of n as Python list (distinct factors).
*/
static PyObject* factors(PyObject* self, PyObject* args){
	long long i;
	if(!PyArg_ParseTuple(args, "L", &i))
        return NULL;
	PyObject* result = PyList_New(0);
	if((i&1)^1){
		PyList_Append(result, PyLong_FromLong(2L));
		while((i&1)^1){
			i >>= 1;
		}
	}
	while(i > 1){
		long long factor = _spfOdd(i);
		while(i % factor == 0)
			i /= factor;
		PyList_Append(result, PyLong_FromLongLong(factor));
	}
	return result;
}

/*
Performs precomputations upto n.
*/
static PyObject* precompute(PyObject* self, PyObject* args)
{
    long n;
	if(!PyArg_ParseTuple(args, "l", &n))
        return NULL;
	_precompute(n);	
	return Py_None;
}

/*
Returns all primes in the precomputed range.
*/
static PyObject* py_primes(PyObject* self, PyObject *args) {
    PyObject* result = PyList_New(0);
	for(long i = 0; i < precomputed_n; i++) {
		if(_spf(i) == i){
			PyList_Append(result, PyLong_FromLong(i));
		}
	}
	return result;
}

/*
Returns pi(n).
*/
static PyObject* pi(PyObject* self, PyObject *args) {
    long n;
	if(!PyArg_ParseTuple(args, "l", &n))
        return NULL;
	return PyLong_FromLong(_pi(n));
}

/*
Returns whether n is prime.
*/
static PyObject* is_prime(PyObject* self, PyObject *args) {
    long n;
	if(!PyArg_ParseTuple(args, "l", &n))
        return NULL;
	if(n == 2 || ((n & 1) && spf[n>>1] == n)){
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

/*
Returns the nth prime.
*/
static PyObject* nth_prime(PyObject* self, PyObject *args) {
    long n;
	if(!PyArg_ParseTuple(args, "l", &n))
        return NULL;
	if(n > primes_count)
		return NULL;
	return PyLong_FromLong(primes[n-1]);
}



// Our Module's Function Definition struct
// We require this `NULL` to signal the end of our method
// definition
static PyMethodDef myMethods[] = {
    {"spf_table", spf_table, METH_NOARGS, "Returns spf table as a list."},
	{"spf", py_spf, METH_VARARGS, "Returns spf(n)."},
	{ "phi", phi, METH_VARARGS, "Returns phi(n)." },
    { "factorisation", factorisation, METH_VARARGS, "Returns prime factorisation of n as Python list (factors are repeated)." },
    { "factors", factors, METH_VARARGS, "Returns prime factors of n as Python list (distinct factors)." },
    { "precompute", precompute, METH_VARARGS, "Performs precomputations upto n." },
    { "primes", py_primes, METH_NOARGS, "Returns a list of all primes in the precomputed range." },
    { "pi", pi, METH_VARARGS, "Returns pi(n)." },
    { "is_prime", is_prime, METH_VARARGS, "Checks whether n is prime." },
    { "nth_prime", nth_prime, METH_VARARGS, "Returns the nth prime." },
    { NULL, NULL, 0, NULL }
};

// Our Module Definition struct
static struct PyModuleDef myModule = {
    PyModuleDef_HEAD_INIT,
    "EulerTotient",
    "Test Module",
    -1,
    myMethods
};

// Initializes our module using our above struct
PyMODINIT_FUNC PyInit_spf(void)
{
    PyObject *m;
	m = PyModule_Create(&myModule);
    if (m == NULL)
        return NULL;

    return m;
}