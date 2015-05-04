/**
 * Header file of Gapcoins Proof of Work calculation unit.
 *
 * Copyright (C)  2014  Jonny Frey  <j0nn9.fr39@gmail.com>
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

/**
 * define the sieve array word size
 */
#if __WORDSIZE == 64
#define sieve_t uint64_t
#define ssieve_t int64_t
#define SIEVE_MAX UINT64_MAX
#define PRISIEVE PRIu64
#else
#define sieve_t uint32_t
#define ssieve_t int32_t
#define SIEVE_MAX UINT32_MAX
#define PRISIEVE PRIu32
#endif


class Sieve {

  public :

  /* should we debug */
#ifdef DEBUG
    static const bool debug = true;
#else
    static const bool debug = false;
#endif

    /**
     * create a new Sieve
     */
    Sieve(PoWProcessor *pprocessor, uint64_t n_primes, uint64_t sievesize);

    ~Sieve();

    /**
     * sets the PoWProcessor of this
     */
    void set_pprocessor(PoWProcessor *pprocessor);
 
    /** 
     * sieve for the given header hash 
     *
     * returns an adder (within pow)  starting a gap greater than difficulty
     *         or NULL if no such prime was found
     */
   void run_sieve(PoW *pow, vector<uint8_t> *offset);
 
    /**
     * returns the primes per seconds
     */
    double primes_per_sec();

    /**
     * returns the average primes per seconds
     */
    double avg_primes_per_sec();

    /**
     * returns the prime gaps per second
     */
    double gaps_per_second();

    /**
     * returns average the prime gaps per second
     */
    double avg_gaps_per_second();

    /**
     * returns the prime tests per second
     */
    double tests_per_second();

    /**
     * returns average the prime tests per second
     */
    double avg_tests_per_second();

    /**
     * returns the estimated gaps (blocks) per day
     */ 
    double gaps_per_day();

    /**
     * return the total number of found primes
     */
    uint64_t get_found_primes();

  protected :

    /* number of sieve filter primes */
    sieve_t n_primes;
    
    /* array of the first n primes */
    sieve_t *primes;

    /* array of the first n primes * 2 */
    sieve_t *primes2;
 
    /**
     * array of the start indexes for each prime.
     * while sieving the current hash
     */
    sieve_t *starts;
 
    /* sieve size in bits */
    sieve_t sievesize;
 
    /* the sieve as an ary of 64 bit words */
    sieve_t *sieve;
 
    /* the start of the sieve */
    mpz_t mpz_start;
 
    /* overall found primes */
    uint64_t found_primes;

    /* overall prime gaps */
    uint64_t n_gaps;

    /* current prime gaps */
    uint64_t cur_n_gaps;

    /* overall prime tests */
    uint64_t tests;

    /* current prime tests */
    uint64_t cur_tests;

    /* passed time mining */
    uint64_t passed_time;

    /* current found primes */
    uint64_t cur_found_primes;

    /* time passed since the last interval */
    uint64_t cur_passed_time;

    /* callback object to process an calculated PoW */
    PoWProcessor *pprocessor;
 
    /* PoW calculation utils */
    PoWUtils *utils;

    /**
     * Generates the first n primes using the sieve of Eratosthenes
     */
    void init_primes(uint64_t n);
 
    /**
     * calculate the sieve start indexes;
     */
    void calc_muls();
 
    /**
     * Fermat pseudo prime test
     */
    inline bool fermat_test(mpz_t mpz_p);
 
    /**
     * verifies a given gap
     */
    bool is_gap_valid(uint64_t index, uint64_t length);
 
    /**
     * verifies that the sieve was sieved correctly
     */
    bool is_sieve_valid(sieve_t db_break);
 
    /**
     * verify the first n primes
     */
    bool are_primes_valid();

  private :

    /* primality testing */
    mpz_t mpz_e, mpz_r, mpz_two;
};
#endif /* __PRIME_H__ */
