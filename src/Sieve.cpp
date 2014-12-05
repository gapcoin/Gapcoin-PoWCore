/**
 * Implementation of Gapcoins Proof of Work calculation unit.
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
#ifndef __STDC_FORMAT_MACROS 
#define __STDC_FORMAT_MACROS 
#endif
#ifndef __STDC_LIMIT_MACROS  
#define __STDC_LIMIT_MACROS  
#endif
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include <gmp.h>
#include <mpfr.h>

#include "Sieve.h"

using namespace std;


#if __WORDSIZE == 64
/**
 * Sets the given bit-position in a 64-bit array
 */
#define set_bit(ary, i) (ary[(i) >> 6] |= (1L << ((i) & 0x3f)))
    
/**
 * Unset the given bit-position in a 64-bit array
 */
#define unset_bit(ary, i) (ary[(i) >> 6] &= ~(1L << ((i) & 0x3f)))

/**
 * returns whether the given bit-position in a 64-bit array is set or not
 */
#define bit_at(ary, i) (ary[(i) >> 6] & (1L << ((i) & 0x3f)))
#else
/**
 * Sets the given bit-position in a 32-bit array
 */
#define set_bit(ary, i) (ary[(i) >> 5] |= (1 << ((i) & 0x1f)))
    
/**
 * Unset the given bit-position in a 32-bit array
 */
#define unset_bit(ary, i) (ary[(i) >> 5] &= ~(1 << ((i) & 0x1f)))

/**
 * returns whether the given bit-position in a 32-bit array is set or not
 */
#define bit_at(ary, i) (ary[(i) >> 5] & (1 << ((i) & 0x1f)))
#endif

/**
 * returns whether the given index is a prime or not
 */
#define is_prime(ary, i) !bit_at(ary, i)

/**
 * marks the given index in the given array as composite
 */
#define set_composite(ary, i) set_bit(ary, i)

/**
 * sets x to the next greater number divisible by y
 */
#define bound(x, y) ((((x) + (y) - 1) / (y)) * (y))

/**
 * returns the sieve limit for an simple sieve of Eratosthenes
 */
#define sieve_limit(x) ((uint64_t) (sqrt((double) (x)) + 1))

/**
 * generate x^2
 */
#define POW(X) ((X) * (X))

/**
 * create a new Sieve
 */
Sieve::Sieve(PoWProcessor *pprocessor, uint64_t n_primes, uint64_t sievesize) {

  this->pprocessor       = pprocessor;
  this->sievesize        = bound(sievesize, sizeof(sieve_t) * 8);
  this->n_primes         = n_primes;
  this->found_primes     = 0;
  this->tests            = 0;
  this->cur_tests        = 0;
  this->passed_time      = 0;
  this->cur_found_primes = 0;
  this->cur_passed_time  = 0;
  this->reset_stats      = true;
  this->sieve            = (sieve_t *) malloc(this->sievesize / 8);
  this->primes           = (sieve_t *) malloc(sizeof(sieve_t) * n_primes);
  this->primes2          = (sieve_t *) malloc(sizeof(sieve_t) * n_primes);
  this->starts           = (sieve_t *) malloc(sizeof(sieve_t) * n_primes);
  this->utils            = new PoWUtils();
  mpz_init(this->mpz_start);
  mpz_init(this->mpz_e);
  mpz_init(this->mpz_r);
  mpz_init_set_ui64(this->mpz_two, 2);
  init_primes(n_primes);
}


Sieve::~Sieve() {
  
  free(sieve);
  free(primes);
  free(primes2);
  free(starts);

  mpz_clear(mpz_start);
  mpz_clear(mpz_e);
  mpz_clear(mpz_r);
  mpz_clear(mpz_two);

  delete utils;
}

/**
 * sets the PoWProcessor of this
 */
void Sieve::set_pprocessor(PoWProcessor *pprocessor) {
  this->pprocessor = pprocessor;
}

/** 
 * sieve for the given header hash 
 *
 * Sets the pow adder to a prime starting a gap greater than difficulty,
 * if found
 *
 * The Sieve works in two stages, first it checks every odd number
 * if it is divisible by one of the pre-calculated primes.
 * Then it uses the Fermat-test to test the remaining numbers.
 */
void Sieve::run_sieve(PoW *pow, vector<uint8_t> *offset) {

  /* speed measurement */
  uint64_t start_time = PoWUtils::gettime_usec();
  
  if (reset_stats) {
    reset_stats      = false;
    cur_found_primes = 0;
    cur_tests        = 0;
    cur_passed_time  = 0;
  }
    

  mpz_t mpz_offset;
  mpz_init_set_ui64(mpz_offset, 0);

  if (offset != NULL)
    ary_to_mpz(mpz_offset, offset->data(), offset->size());

  /* make sure offset (and later start) is divisible by two */
  if (mpz_get_ui64(mpz_offset) & 0x1)
    mpz_add_ui(mpz_offset, mpz_offset, 1L);

  mpz_t mpz_adder, mpz_tmp;
  mpz_init(mpz_tmp);
  mpz_init(mpz_adder);

  pow->get_hash(mpz_start);
  mpz_mul_2exp(mpz_start, mpz_start, pow->get_shift());
  mpz_add(mpz_start, mpz_start, mpz_offset);

  /* clear the sieve */
  memset(sieve, 0, sievesize / 8);

  /* calculates for each prime, the first index in the sieve
   * which is divisible by that prime */
  calc_muls();

  /* sieve all small primes (skip 2) */
  for (sieve_t i = 1; i < n_primes; i++) {

    /**
     * sieve all odd multiplies of the current prime
     */
    for (sieve_t p = starts[i]; p < sievesize; p += primes2[i])
      set_composite(sieve, p);
  }

  /* make sure min_len is divisible by two */
  sieve_t min_len    = pow->target_size(mpz_start) & ~((sieve_t) 1);
  sieve_t i          = 1;
  sieve_t start      = sievesize;

  /* find the first prime */
  for (/* declared */; i < sievesize; i += 2) {
    
    if (is_prime(sieve, i)) {
      cur_tests++;
      tests++;
      mpz_add_ui(mpz_tmp, mpz_start, i);

      if (fermat_test(mpz_tmp)) {
        start = i;
        break;
      }
    }
  }

  /* scan the sieve in steps of size min_len */
  for (/* declared */; i < sievesize; i += min_len) {
    
    /* scan the current gap */
    for (/* declared */; i > start; i -= 2) {

      if (is_prime(sieve, i)) {
        cur_tests++;
        tests++;
        mpz_add_ui(mpz_tmp, mpz_start, i);
     
        if (fermat_test(mpz_tmp)) {
          start = i;
          break;
        }
      }
    }

    if (i == start) {
      mpz_set_ui64(mpz_adder, (uint64_t) start);
      mpz_add(mpz_adder, mpz_adder, mpz_offset);
 
      pow->set_adder(mpz_adder);
 
      if (pow->valid()) {
        pprocessor->process(pow);
      }

      i += min_len;
    }
  }

  mpz_clear(mpz_offset);
  mpz_clear(mpz_adder);
  mpz_clear(mpz_tmp);
  passed_time     += PoWUtils::gettime_usec() - start_time;
  cur_passed_time += PoWUtils::gettime_usec() - start_time;

  /* approximate the number of primes within the sieve */
  double log_start = log(mpz_get_d(mpz_start));
  cur_found_primes += sievesize / log_start;
  found_primes += sievesize / log_start;

  if (debug && is_sieve_valid(i))
    printf("[DD] sieve check [PASSED]\n");
  else if (debug)
    printf("[EE] sieve check [FAILED]\n");
}

/**
 * returns the average primes per seconds
 */
double Sieve::avg_primes_per_sec() {
  return ((double)  found_primes) / 
         (((double) passed_time) / 1000000.0L);
}


/**
 * returns the primes per seconds
 */
double Sieve::primes_per_sec() {
  
  /* reset measurement */
  reset_stats = true;

  return ((double)  cur_found_primes) / 
         (((double) cur_passed_time) / 1000000.0L);
}


/**
 * return the total number of found primes
 */
uint64_t Sieve::get_found_primes() {
  return found_primes;
}


/**
 * returs the prime tests per second
 */
double Sieve::tests_per_second() {
  return ((double)  cur_tests) / 
         (((double) cur_passed_time) / 1000000.0L);
}

/**
 * returs average the prime tests per second
 */
double Sieve::avg_tests_per_second() {
  return ((double)  tests) / 
         (((double) passed_time) / 1000000.0L);
}


/**
 * Generates the first n primes using the sieve of Eratosthenes
 */
void Sieve::init_primes(uint64_t n) {
  
  uint64_t sievesize = n * log(n) + n * log(log(n));
  sievesize = bound(sievesize, sizeof(sieve_t) * 8);

  sieve_t *sieve = (sieve_t *) malloc(sievesize / 8);
  memset(sieve, 0, sievesize / 8);

  /* we only have to sieve till sqrt(sievesize); */
  sieve_t limit = sieve_limit(sievesize);

  /* 0 and 1 are no primes */
  set_bit(sieve, 0);
  set_bit(sieve, 1);

  primes[0]  = 2;
  primes2[0] = 2 << 1;

  /**
   * run the sieve (skip all even numbers)
   */
  for (sieve_t i = 1; i < limit; i += 2)
    if (is_prime(sieve, i))
      for (sieve_t p = POW(i); p < sievesize; p += i << 1)
        set_composite(sieve, p);

  /* save the primes */
  for (sieve_t i = 1, p = 1; i < sievesize && p < n; i += 2) {
    
    if (is_prime(sieve, i)) {
      this->primes[p]  = i;
      this->primes2[p] = i << 1;
      p++;
    }
  }

  free(sieve);

  if (debug && are_primes_valid())
    printf("[DD] primes check [PASSED]\n");
  else if (debug)
    printf("[EE] primes check [FAILED]\n");
}

/**
 * calculate for every prime the first
 * index in the sieve which is divisible by that prime
 * (and not divisible by two)
 */
void Sieve::calc_muls() {

  for (sieve_t i = 0; i < n_primes; i++) {

    starts[i] = primes[i] - mpz_tdiv_ui(mpz_start, primes[i]);

    if (starts[i] == primes[i])
      starts[i] = 0;

    /* is start index divisible by two 
     * (this check works because mpz_start is divisible by two)
     */
    if ((starts[i] & 1) == 0)
      starts[i] += primes[i];
  }
}

/**
 * Fermat pseudo prime test
 */
inline bool Sieve::fermat_test(mpz_t mpz_p) {

  /* tmp = p - 1 */
  mpz_sub_ui(mpz_e, mpz_p, 1);

  /* res = 2^tmp mod p */
  mpz_powm(mpz_r, mpz_two, mpz_e, mpz_p);

  if (mpz_cmp_ui(mpz_r, 1) == 0)
    return true;

  return false;
}

/**
 * verifies a given gap
 */
bool Sieve::is_gap_valid(uint64_t index, uint64_t length) {
  
  mpz_t mpz_start, mpz_end, mpz_len;
  mpz_init(mpz_len);
  mpz_init(mpz_end);

  mpz_init_set_ui64(mpz_start, index);
  mpz_add(mpz_start, mpz_start, this->mpz_start);
  
  mpz_nextprime(mpz_end, mpz_start);

  mpz_sub(mpz_len, mpz_end, mpz_start);
  
  bool result = !mpz_cmp_ui(mpz_len, length);
  mpz_clear(mpz_start);
  mpz_clear(mpz_end);
  mpz_clear(mpz_len);

  return result;
}

/**
 * verifies that the sieve was sieved correctly
 */
bool Sieve::is_sieve_valid(sieve_t db_break) {

  mpz_t mpz_p;
  mpz_init(mpz_p);

  bool result = true;

  /* run primality test for all remaining prime candidates */
  for (sieve_t i = 1; i < db_break && i < sievesize && result; i += 2) {
    
    mpz_add_ui(mpz_p, mpz_start, i);

    /* is_prime(sieve, i) <=> miller_rabin_test(start + i) */
    result = !(is_prime(sieve, i) xor (mpz_probab_prime_p(mpz_p, 25) > 0));
  }

  mpz_clear(mpz_p);
  return result;
}

/**
 * verify the first n primes
 */
bool Sieve::are_primes_valid() {

  mpz_t mpz_p, mpz_next;
  mpz_init_set_ui64(mpz_next, 0);
  mpz_init_set_ui64(mpz_p, 2);
  
  bool result = primes[0] == 2 && primes2[0] == (2 << 1);

  for (sieve_t i = 1; i < n_primes && result; i++) {
    
    mpz_nextprime(mpz_next, mpz_p);
    result = mpz_get_ui64(mpz_next) == primes[i] && 
             (mpz_get_ui64(mpz_next) << 1) == primes2[i];

    if (!result)
      printf("[EE] primes[%" PRISIEVE "] = %" PRISIEVE
             ", primes[%" PRISIEVE "] = %" PRISIEVE ", next: %" PRIu64 "\n",
             i - 1, primes[i - 1], i, primes[i], mpz_get_ui64(mpz_next));

    mpz_set(mpz_p, mpz_next);
  }

  mpz_clear(mpz_next);
  mpz_clear(mpz_p);

  return result;
}
