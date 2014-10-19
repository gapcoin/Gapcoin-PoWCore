/**
 * Header file of Gapcoins Proof of Work calculation unit.
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
#ifndef __PRIME_H__
#define __PRIME_H__
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <gmp.h>
#include <mpfr.h>

#include "PoW.h"
#include "PoWUtils.h"
#include "PoWProcessor.h"

using namespace std;

class Sieve {

  public :

#ifdef DEBUG
    static const bool debug = true;
#else
    static const bool debug = false;
#endif

    Sieve(PoWProcessor *pprocessor, uint64_t n_primes, uint64_t sievesize);
    ~Sieve();
    void set_pprocessor(PoWProcessor *pprocessor);
    void run_sieve(PoW *pow, vector<uint8_t> *offset);
    double primes_per_sec();
    double gaps10_per_hour();
    double gaps15_per_hour();
    double avg_primes_per_sec();
    double avg_gaps10_per_hour();
    double avg_gaps15_per_hour();
    double gaps_per_day();
    uint64_t get_found_primes();
};
#endif /* __PRIME_H__ */
