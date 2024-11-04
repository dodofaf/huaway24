// Algotester Generator Library
// Version 1.0.497

#pragma once
#include <random>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <climits>
#include <sstream>

class AlgotesterGenerator{
public:
    AlgotesterGenerator(unsigned long long seed = 0);

    AlgotesterGenerator(int argc, char ** argv);

    /// @brief Generate uniform random unsigned long long
    unsigned long long randUInt();

    /// @brief Generate uniform random unsigned long long in range [0, n)
    /// @note To make random number uniform several tries could be made. Expected value of number of tries is less than 2.
    unsigned long long randUInt(unsigned long long n);

    /// @brief Generate uniform random unsigned long long in range [l, r]
    unsigned long long randUInt(unsigned long long l, unsigned long long r);

    /// @brief Generate uniform random long long
    long long randInt();

    /// @brief Generate uniform random long long in range [0, n)
    long long randInt(long long n);

    /// @brief Generate uniform random long long in range [l, r]
    long long randInt(long long l, long long r);

    /// @brief Generate random long double in range [0, 1].
    long double randDouble();

    /// @brief Generate random long double in range [0, n].
    long double randDouble(long double n);

    /// @brief Generate random long double in range [l, r].
    long double randDouble(long double l, long double r);

    /// @brief Random shuffle vector A
    template <class T>
    void shuffle(std::vector<T> & A) {
        shuffle(A.begin(), A.end());
    }

    /// @brief Random shuffle the elements in the range [first,last)
    template <class RandomAccessIterator>
    void shuffle(RandomAccessIterator first, RandomAccessIterator last) {
        int n = last - first;
        if (n < 0)
            throw std::runtime_error("Cannot shuffle, first iterator is later than last iterator");
        for(int i = 0; i < n; ++i) {
            int j = randInt(i, n - 1);
            std::swap(first[i], first[j]);
        }
    }

    /// @brief Choose random element of vector A
    template<class T>
    T randomElement(std::vector<T> & A) {
        if (A.empty())
            throw std::runtime_error("Cannot choose random element of empty vector");
        return A[randInt(A.size())];
    }

    /// @brief Generate random long long in range [1, n] such that log is uniformly distributed.
    /// @note Useful in cases when you want to generate both small and large values.
    long long randLogUniform(long long n);

    /// @brief Generate biased long long in range [0, n).
    /// @note When $t=0$ result is uniform.
    /// @note When $t<0$ result is minimum of t+1 uniform values. The mean value equals $(n-1)/(t+1)$.
    /// @note When $t>0$ result is maximum of t+1 uniform values. The mean value equals $(n-1)*t/(t+1)$.
    /// @note This method can be very innacurate for large values of n or t.
    long long randBiasedInt(long long n, long long t);

    /// @brief Generate biased long long in range [l, r].
    /// @note When $t=0$ result is uniform.
    /// @note When $t<0$ result is minimum of t+1 uniform values. The mean value equals $l+(r-l)/(t+1)$.
    /// @note When $t>0$ result is maximum of t+1 uniform values. The mean value equals $l+(r-l)*t/(t+1)$.
    /// @note This method can be very innacurate for large values of n or t.
    long long randBiasedInt(long long l, long long r, long long t);

    /// @brief Generate normal distribution random long double with specific mean and standard deviation values.
    long double randDoubleNormal(long double mean, long double stddev);

    /// @brief Get argument.
    /// @tparam T Type of the argument.
    /// @param index Argument index(1-based).
    template<class T>
    T getArg(int index) {
        if (index < 0 || index >= _argc)
            throw std::out_of_range("Wrong argument index");
        std::stringstream ss(_argv[index]);
        T res;
        ss >> res;
        return res;
    }

private:
    int _argc;
    char ** _argv;
    std::mt19937_64 rng;
};

/// @brief Initialize AlgotesterGenerator with seed based on CLI arguments.
inline AlgotesterGenerator initGenerator(int argc, char ** argv) {
    return AlgotesterGenerator(argc, argv);
}

AlgotesterGenerator::AlgotesterGenerator(unsigned long long seed) {
    rng = std::mt19937_64(seed);
    _argc = 0;
}

AlgotesterGenerator::AlgotesterGenerator(int argc, char ** argv) {
    unsigned long long seed = 0;
    unsigned long long p = 4447;
    for(int i = 1; i < argc; ++i) {
        int len = strlen(argv[i]);
        for(int j = 0; j < len; ++j) {
            seed = seed * p + argv[i][j];
        }
        seed = seed * p;
    }
    rng = std::mt19937_64(seed);
    _argc = argc;
    _argv = argv;
}

unsigned long long AlgotesterGenerator::randUInt() {
    return rng();
}

unsigned long long AlgotesterGenerator::randUInt(unsigned long long n) {
    if (n == 0)
        throw std::runtime_error("Cannot generate random integer when range is empty");
    int num_bits = 64 - __builtin_clzll(n);
    unsigned long long mask = ULLONG_MAX;
    if (num_bits < 64) {
        mask = (1LL << num_bits) - 1;
    }
    while (true) {
        unsigned long long value = rng() & mask;
        if (value < n)
            return value;
    }
}

unsigned long long AlgotesterGenerator::randUInt(unsigned long long l, unsigned long long r) {
    if (l > r)
        throw std::runtime_error("Cannot generate random integer when range is empty");
    if (l == 0 && r == ULLONG_MAX)
        return randUInt();
    return l + randUInt(r - l + 1);
}

long long AlgotesterGenerator::randInt() {
    return rng();
}

long long AlgotesterGenerator::randInt(long long n) {
    if (n < 0)
        throw std::runtime_error("Cannot generate random number for negative n");
    return randUInt(n);
}

long long AlgotesterGenerator::randInt(long long l, long long r) {
    if (l > r)
        throw std::runtime_error("Cannot generate random number when l > r");
    if (l == LLONG_MIN && r == LLONG_MAX)
        return randInt();
    return l + randUInt(r - l + 1);
}

long double AlgotesterGenerator::randDouble() {
    return (long double)randInt(1LL << 60) / (long double)(1LL << 60);
}
 
long double AlgotesterGenerator::randDouble(long double n) {
    if (n < 0)
        throw std::runtime_error("Cannot generate random double when n < 0");
    return randDouble() * n;
}

long double AlgotesterGenerator::randDouble(long double l, long double r) {
    if (l > r)
        throw std::runtime_error("Cannot generate random double when l > r");
    return l + randDouble() * (r - l);
}

long long AlgotesterGenerator::randLogUniform(long long n) {
    if (n <= 0)
        throw std::runtime_error("Cannot generate random number for non-positive n");
    long double x = randDouble(log((long double)n + 1));
    return std::min(std::max((long long)exp(x), 1LL), n);
}

long long AlgotesterGenerator::randBiasedInt(long long n, long long t) {
    if (n <= 0)
        throw std::runtime_error("Cannot generate random number for non-positive n");
    if (t == 0)
        return randInt(n);
    if (t < 0)
        return n - 1 - randBiasedInt(n, -t);
    long double x = randDouble(1);
    x = pow(x, (long double)1 / (1 + t));
    return std::min(std::max((long long)(x * n), 0LL), n - 1);
}

long long AlgotesterGenerator::randBiasedInt(long long l, long long r, long long t) {
    if (l > r)
        throw std::runtime_error("Cannot generate random number when l > r");
    return l + randBiasedInt(r - l + 1, t);
}

long double AlgotesterGenerator::randDoubleNormal(long double mean, long double stddev) {
    if (stddev < 0)
        throw std::runtime_error("Standard deviation cannot be negative");
    
    long double u1 = randDouble();
    long double u2 = randDouble();
    // Box-Muller transform
    long double z0 = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
    return mean + stddev * z0;
}

