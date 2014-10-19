/**
 * Header file for some Proof of Work related utilities.
 *
 * Copyright (C)  2014  The Gapcoin developers  <info@gapcoin.org>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __POWUTILS_H__
#define __POWUTILS_H__

#include <stdlib.h>
#include <inttypes.h>
#include <openssl/sha.h>
#include <gmp.h>
#include <mpfr.h>
#include <vector>
#include <string>

using namespace std;

/* 2^48 */
#define TWO_POW48 (((uint64_t) 1) << 48)

class PoWUtils {
  
  public :

#ifdef DEBUG
    static const bool debug = true;
#else
    static const bool debug = false;
#endif

    static const uint64_t min_difficulty = 16 * TWO_POW48;
    static const uint64_t min_test_difficulty = TWO_POW48;
    static uint64_t gettime_usec();
    void mpz_log2(mpz_t mpz_log, mpz_t mpz_src, uint32_t accuracy);
    uint64_t merit(mpz_t mpz_start, mpz_t mpz_end);
    uint64_t rand(mpz_t mpz_start, mpz_t mpz_end);
    uint64_t difficulty(mpz_t mpz_start, mpz_t mpz_end);
    double get_readable_difficulty(uint64_t difficulty);
    uint64_t target_size(mpz_t mpz_start, uint64_t difficulty);
    void target_work(vector<uint8_t> *n_primes, uint64_t difficulty);
    uint64_t next_difficulty(uint64_t difficulty, 
                             uint64_t actual_timespan,
                             bool testnet);
    static uint64_t max_difficulty_decrease(uint64_t difficulty, 
                                            int64_t time,
                                            bool testnet);
    double gaps_per_day(double pps, uint64_t difficulty);
    PoWUtils();
    ~PoWUtils();
};

#endif /* __POWUTILS_H__ */
