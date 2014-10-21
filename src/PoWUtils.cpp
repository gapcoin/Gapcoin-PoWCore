/**
 * Implementation of some Proof of Work related utilities.
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
#include <math.h>
#include <inttypes.h>
#include <sys/time.h>
#include <openssl/sha.h>
#include <gmp.h>
#include <mpfr.h>
#include "PoWUtils.h"

using namespace std;

/* return |x| */
#define abs(x) (((x) < 0) ? (x) * -1 : x)

/**
 * calculates the log2 from a mpz value,
 * the return value is 2^accuracy times grater than
 * the actual log2 value to provide a finer accuracy
 */
void PoWUtils::mpz_log2(mpz_t mpz_log, mpz_t mpz_src, uint32_t accuracy) {

  mpz_t mpz_tmp, mpz_n;
  mpz_init(mpz_tmp);
  mpz_init_set(mpz_n, mpz_src);
  
  /* log2 without the decimal part */
  mpz_set_ui64(mpz_log, mpz_sizeinbase(mpz_n, 2) - 1);

  uint32_t bits = 0;
  uint32_t shift = accuracy + mpz_get_ui64(mpz_log);

  /* add accuracy bits */
  mpz_mul_2exp(mpz_log, mpz_log, accuracy);
  mpz_mul_2exp(mpz_n,   mpz_n,   accuracy);

  for (;;) {

    mpz_div_2exp(mpz_tmp, mpz_n, shift);
    
    /* while n / 2^accuracy < 2 */
    while (mpz_get_ui64(mpz_tmp) < 2 && bits <= accuracy) {

      /* n = n^2 */
      mpz_mul(mpz_n, mpz_n, mpz_n);

      /* preserve accuracy */
      mpz_div_2exp(mpz_n, mpz_n, shift);
      mpz_div_2exp(mpz_tmp, mpz_n, shift);

      bits++;
    }

    if (bits > accuracy) break;

    /* log += 2^(accuracy - bits) */
    mpz_set_ui64(mpz_tmp, 1);
    mpz_mul_2exp(mpz_tmp, mpz_tmp, accuracy - bits);
    mpz_add(mpz_log, mpz_log, mpz_tmp);

    /* n = n / 2 */
    mpz_div_2exp(mpz_n, mpz_n, 1);
  }

  mpz_clear(mpz_tmp);
  mpz_clear(mpz_n);
}

/**
 * calculates the log from a mpz value
 * (double version for debugging)
 */
double PoWUtils::mpz_log(mpz_t mpz) {
  
  mpfr_t mpfr_tmp;
  mpfr_init_set_z(mpfr_tmp, mpz, MPFR_RNDD);
  mpfr_log(mpfr_tmp, mpfr_tmp, MPFR_RNDD);
  
  double res = mpfr_get_d(mpfr_tmp, MPFR_RNDD);
  mpfr_clear(mpfr_tmp);
 
  return res;
}

/**
 * calculates the merit of a given prime gap
 *
 * merit = gapsize/log(gapstart) = (gapsize * log2(e)) / log2(gapstart)
 *
 * the return value is 2^48 times grater than
 * the actual merit value to provide a 48 bit accuracy
 */
uint64_t PoWUtils::merit(mpz_t mpz_start, mpz_t mpz_end) {

  mpz_t mpz_merit, mpz_ld;
  mpz_init(mpz_merit);
  mpz_init(mpz_ld);

  /* merit = gaplen * log2(e) * 2^(64 + 48) */
  mpz_sub(mpz_merit, mpz_end, mpz_start);
  mpz_mul(mpz_merit, mpz_merit, mpz_log2e112);

  /* merit = merit / (log2(gapstart) * 2^64) */
  mpz_log2(mpz_ld, mpz_start, 64);
  mpz_div(mpz_merit, mpz_merit, mpz_ld);

  uint64_t merit = 0;

  if (mpz_fits_uint64_p(mpz_merit))
    merit = mpz_get_ui64(mpz_merit);

  mpz_clear(mpz_merit);
  mpz_clear(mpz_ld);


  if (debug) {
    double meritd = merit_d(mpz_start, mpz_end);
    double meriti = ((double) merit) / TWO_POW48;
    
    if (abs(meriti - meritd) > accuracy) {
      mpz_t mpz_len;
      mpz_init(mpz_len);
      mpz_sub(mpz_len, mpz_end, mpz_start);

      printf("[EE] merit doesn't match: \n"
             "  meriti: %0.30F\n  meritd: %0.30F\n"
             "  meriti: %" PRIu64 "\n  meritd: %" PRIu64 
             "\nlen: %" PRIu64 "\nstart: ",
             meriti,
             meritd,
             merit,
             (uint64_t) (meritd * TWO_POW48),
             (uint64_t) mpz_get_ui64(mpz_len));

      mpz_out_str(stdout, 10, mpz_start);
      printf("\n");
      mpz_clear(mpz_len);

    } else {
      printf("[DD] merit check [PASSED]\n");
    }
  }

  return merit;
}

/**
 * calculates the merit of a given prime gap
 * (double version for debugging)
 */
double PoWUtils::merit_d(mpz_t mpz_start, mpz_t mpz_end) {

  mpz_t mpz_len;
  mpz_init(mpz_len);

  mpz_sub(mpz_len, mpz_end, mpz_start);
  double merit = 0.0;

  if (mpz_fits_uint64_p(mpz_len))
    merit = ((double) (mpz_get_ui64(mpz_len))) / mpz_log(mpz_start);

  mpz_clear(mpz_len);
  return merit;
}

/**
 * generates a pseudo random number from the given gap
 */
uint64_t PoWUtils::rand(mpz_t mpz_start, mpz_t mpz_end) {

  uint8_t tmp[SHA256_DIGEST_LENGTH];                                   
  uint8_t hash[SHA256_DIGEST_LENGTH];                                   
  
  uint8_t *start, *end;
  size_t start_len = 0, end_len = 0;

  start = (uint8_t *) mpz_to_ary(mpz_start, NULL, &start_len);
  end   = (uint8_t *) mpz_to_ary(mpz_end,  NULL, &end_len);

  /* hash the start and end prime */
  SHA256_CTX sha256;                                                          
  SHA256_Init(&sha256);                                                       
  SHA256_Update(&sha256, start, start_len);                                   
  SHA256_Update(&sha256, end, end_len);                                   
  SHA256_Final(tmp, &sha256); 

  /* hash the result again */
  SHA256_Init(&sha256);                                                       
  SHA256_Update(&sha256, tmp, SHA256_DIGEST_LENGTH);  
  SHA256_Final(hash, &sha256);

  /* generate an uint64_t value form the 256 bit hash */
  uint64_t rand, i, *ptr = (uint64_t *) hash;

  for (i = 1, rand = ptr[0]; 
       i < SHA256_DIGEST_LENGTH / sizeof(uint64_t); 
       i++) {
    
    /* xor the 64 bit parts of the hash */
    rand ^= ptr[i];
  }

  free(start);
  free(end);
  
  return rand;
}

/**
 * generates a pseudo random number from the given gap
 * (double version for debugging)
 */
double PoWUtils::rand_d(mpz_t mpz_start, mpz_t mpz_end) {

  uint8_t tmp[SHA256_DIGEST_LENGTH];                                   
  uint8_t hash[SHA256_DIGEST_LENGTH];                                   
  
  uint8_t *start, *end;
  size_t start_len = 0, end_len = 0;

  start = (uint8_t *) mpz_to_ary(mpz_start, NULL, &start_len);
  end   = (uint8_t *) mpz_to_ary(mpz_end,  NULL, &end_len);

  SHA256_CTX sha256;                                                          
  SHA256_Init(&sha256);                                                       
  SHA256_Update(&sha256, start, start_len);                                   
  SHA256_Update(&sha256, end, end_len);                                   
  SHA256_Final(tmp, &sha256); 

  /* hash the result again */
  SHA256_Init(&sha256);                                                       
  SHA256_Update(&sha256, tmp, SHA256_DIGEST_LENGTH);  
  SHA256_Final(hash, &sha256);

  uint32_t rand, i, *ptr = (uint32_t *) hash;

  for (i = 1, rand = ptr[0]; 
       i < SHA256_DIGEST_LENGTH / sizeof(uint32_t); 
       i++) {
    
    rand ^= ptr[i];
  }

  free(start);
  free(end);
  
  return ((double) rand) / ((double) UINT32_MAX);
}

/**
 * generates the difficulty of a given prime gap
 */
uint64_t PoWUtils::difficulty(mpz_t mpz_start, mpz_t mpz_end) {

  mpz_t mpz_ld, mpz_tmp;
  mpz_init(mpz_ld);

  /* tmp = 2 * log2(e) * 2^(64 + 48) */
  mpz_init_set_ui64(mpz_tmp, 2);
  mpz_mul(mpz_tmp, mpz_tmp, mpz_log2e112);

  /* tmp corresponds to 2 / log(start) with 64 bit accuracy*/
  mpz_log2(mpz_ld, mpz_start, 64);
  mpz_div(mpz_tmp, mpz_tmp, mpz_ld);

  uint64_t min_gap_distance_merit = 1;
  
  /* we just calculated the merit of the (average) minimal distance 
   * till the next greater merit for the given prime gap */
  if (mpz_fits_uint64_p(mpz_tmp))
    min_gap_distance_merit = mpz_get_ui64(mpz_tmp);

  mpz_clear(mpz_ld);
  mpz_clear(mpz_tmp);

  /**
   * to refine the decimal part between the next greater merit
   * we use an CSPRNG (cryptographically secure pseudo random number generator)
   */
  uint64_t difficulty = merit(mpz_start, mpz_end) +
                        (rand(mpz_start, mpz_end) % min_gap_distance_merit);

  if (debug) {
    double difficultyd = difficulty_d(mpz_start, mpz_end);
    double difficultyi = ((double) difficulty) / TWO_POW48;

    double min_gap = 2.0 / mpz_log(mpz_start);

    if (abs(difficultyd - difficultyi) > min_gap) {
      printf("[EE] difficulty doesn't match: \n"
             "  difficultyi: %0.30F\n  difficultyd: %0.30F\n",
             difficultyi,
             difficultyd);
    } else {
      printf("[DD] difficulty check [PASSED]\n");
    }
  }

  /* difficulty = gap_size / ln(start) + 
   *             rand(start, end) % merit_of_distance_to_next_gap
   */
  return difficulty;
}

/**
 * generates the difficulty of this pow
 * (double version for debugging)
 */
double PoWUtils::difficulty_d(mpz_t mpz_start, mpz_t mpz_end) {

  double difficulty = merit_d(mpz_start, mpz_end) + 
                      (2.0 / mpz_log(mpz_start))  * 
                      rand_d(mpz_start, mpz_end);
  
  if (difficulty < 0.0)
    return 0;

  return difficulty; 
}

/**
 * returns the given difficulty in human readable format
 */
double PoWUtils::get_readable_difficulty(uint64_t difficulty) {
  return ((double) difficulty) / TWO_POW48;
}

/**
 * Create a new PoWUtils object
 */
PoWUtils::PoWUtils() {
  
  /* log2(e) * 2^(64 + 48) */
  mpz_init_set_str(mpz_log2e112, "171547652b82fe1777d0ffda0d23a", 16);

  /* log2(e) * 2^64 */
  mpz_init_set_str(mpz_log2e64,  "171547652b82fe177", 16);
}

PoWUtils::~PoWUtils() {
  
  mpz_clear(mpz_log2e112);
  mpz_clear(mpz_log2e64);
}

/**
 * returns the target gap size for a given difficulty and start index
 *
 * difficulty * log(start) 
 * = (difficulty * 2^48 * log2(start) * 2^64) / (log2(e) * 2^(48 + 64))
 */
uint64_t PoWUtils::target_size(mpz_t mpz_start, uint64_t difficulty) {

  mpz_t mpz_target_size, mpz_difficulty;
  mpz_init(mpz_target_size);
  mpz_init_set_ui64(mpz_difficulty, difficulty);

  /* target_size = (difficulty * log2(start)) / log2(e) */
  mpz_log2(mpz_target_size, mpz_start, 64);
  mpz_mul(mpz_target_size, mpz_target_size, mpz_difficulty);
  mpz_div(mpz_target_size, mpz_target_size, mpz_log2e112);

  uint64_t target_size = 0;

  if (mpz_fits_uint64_p(mpz_target_size))
    target_size = mpz_get_ui64(mpz_target_size);

  mpz_clear(mpz_target_size);
  mpz_clear(mpz_difficulty);

  if (debug && 
      target_size != (uint64_t) ((((double) difficulty) / TWO_POW48) * 
                                 mpz_log(mpz_start))) {
    
    printf("[EE] target size check [FAILED]\n");
  } else if (debug) 
    printf("[DD] target size check [PASSED]\n");

  return target_size;
}


/**
 * returns the estimated work required to find 
 * a gap with the given difficulty, which is e^difficulty
 * (work are the total among of primes to calculate)
 */
void PoWUtils::target_work(vector<uint8_t> *n_primes, 
                           uint64_t difficulty) {
  
  mpz_t mpz_n_primes;
  mpz_init(mpz_n_primes);

  /* calculate with mpfr, cause we only use the integer part,
   * so it should be exact enough, even if we could get slightly different
   * decimal parts on some machines */
  mpfr_t mpfr_difficulty;
  mpfr_init_set_ui64(mpfr_difficulty, difficulty, MPFR_RNDD);
  mpfr_div_2exp(mpfr_difficulty, mpfr_difficulty, 48, MPFR_RNDD);

  mpfr_exp(mpfr_difficulty, mpfr_difficulty, MPFR_RNDD);
  mpfr_get_z(mpz_n_primes, mpfr_difficulty, MPFR_RNDD);

  size_t len;
  uint8_t *ary = (uint8_t *) mpz_to_ary(mpz_n_primes, NULL, &len);

  n_primes->assign(ary, ary + len);

  if (debug && abs(mpfr_get_d(mpfr_difficulty, MPFR_RNDD) -
                   target_work_d(difficulty)) > 0.001) {
    
    printf("[EE] target_work check [FAILED]\n");
    printf("  target_work:   %.30F\n"
           "  target_work_d: %.30F\n",
           mpfr_get_d(mpfr_difficulty, MPFR_RNDD),
           target_work_d(difficulty));

  } else if (debug)
    printf("[DD] target_work check [PASSED]\n");
  
  free(ary);
  mpz_clear(mpz_n_primes);
  mpfr_clear(mpfr_difficulty);
}


/**
 * returns the estimated work required to find 
 * a gap with the given difficulty, which is e^difficulty
 * (work are the total among of primes to calculate)
 * (double version)
 */
double PoWUtils::target_work_d(uint64_t difficulty) {
  
  double ddifficulty = ((double) difficulty) / TWO_POW48;
  double work = exp(ddifficulty);

  return work;
}

/**
 * returns the current time in microseconds
 */
uint64_t PoWUtils::gettime_usec() {

  struct timeval time;
  if (gettimeofday(&time, NULL) == -1)
    return ((uint64_t) -1);

  return ((uint64_t) time.tv_sec) * ((uint64_t) 1000000) + 
         ((uint64_t) time.tv_usec);
}

/**
 * calculates the next difficulty according to 
 * the given target and actual timespan between the last two blocks
 * the difficulty is only changed about 1/256 of the 
 * actual increase and about 1/64 of the actual decrease
 *
 * calculates difficulty + log(target_timespan / actual_timespan)
 * <=> d + log(t / a) 
 *   = d + log(t) - log(a)
 *   = d + log2(t) / log2(e) - log2(a) / log2(e)
 * 
 * difficulty increases logarithmically:
 *   a 2,718(e) block speed increase is a +1 difficulty increase
 */
uint64_t PoWUtils::next_difficulty(uint64_t difficulty, 
                                   uint64_t actual_timespan,
                                   bool testnet) {
    
  /* calculate log(actual_timespan) * 2^48 */
  mpz_t mpz_log_actual;
  mpz_init_set_ui64(mpz_log_actual, actual_timespan);
  
  /* log_actual = (log2(actual_timespan) * 2^(64 + 48)) / (log2(e) * 2^64) */
  mpz_log2(mpz_log_actual, mpz_log_actual, 64 + 48);
  mpz_div(mpz_log_actual, mpz_log_actual, mpz_log2e64);

  const uint64_t log_target = log_150_48;
  const uint64_t log_actual = mpz_get_ui64(mpz_log_actual);

  mpz_clear(mpz_log_actual);

  uint64_t next = difficulty;
  uint64_t shift = 8;

  /* correct hash rate lose faster 
   * (with out this the difficulty would mostly adjust to high) */
  if (log_actual > log_target)
    shift = 6;

  next += log_target >> shift;
  next -= log_actual >> shift;

  /* avoid difficulty underflow */
  if (log_actual > log_target && difficulty < ((log_actual >> shift) - (log_target >> shift)))
    next = (testnet ? min_test_difficulty : min_difficulty);

  /* this should never happen, but avoid difficulty overflow */
  if (log_actual < log_target && UINT64_MAX - ((log_target >> shift) - (log_actual >> shift)) < difficulty)
    next = UINT64_MAX;


  /* difficulty can only change about +/- 1 per block */
  if (next > difficulty + TWO_POW48)
    next = difficulty + TWO_POW48;
  if (next < difficulty - TWO_POW48)
    next = difficulty - TWO_POW48;

  if (testnet && next < min_test_difficulty)
    next = min_test_difficulty;
  else if (!testnet && next < min_difficulty)
    next = min_difficulty;

   if (debug && abs(((double) next) / TWO_POW48 -
                    next_difficulty_d(((double) difficulty) / TWO_POW48,
                                      actual_timespan,
                                      testnet)) > accuracy) {

      printf("[EE] next_difficulty check [FAILED]\n");
   } else if (debug)
      printf("[DD] next_difficulty check [PASSED]\n");

   return next;
}


/**
 * calculates the next difficulty according to 
 * the given target and actual timespan
 *
 * calculates difficulty + log(target_timespan / actual_timespan)
 * 
 * difficulty increases logarithmically:
 *   a 2,718(e) block speed increase is a +1 difficulty increase
 */
double PoWUtils::next_difficulty_d(double difficulty, 
                                   uint64_t actual_timespan,
                                   bool testnet) {

  uint64_t shift = 8;

  /* correct hash rate lose faster */
  if (actual_timespan > 150)
    shift = 6;
    
  double next = difficulty + log(150.0 / ((double) actual_timespan)) / (1 << shift);

  /* difficulty can only change about +/- 1 per block */
  if (next > difficulty + 1.0)
    next = difficulty + 1.0;
  if (next < difficulty - 1.0)
    next = difficulty - 1.0;

  if (testnet && next < ((double) min_test_difficulty) / TWO_POW48)
    next = ((double) min_test_difficulty) / TWO_POW48;
  else if (!testnet && next < ((double) min_difficulty) / TWO_POW48)
    next = ((double) min_difficulty) / TWO_POW48;


  return next;
}

/**
 * compute the maximum possible difficulty decrease from
 * the given difficulty in the given time
 */
uint64_t PoWUtils::max_difficulty_decrease(uint64_t difficulty, 
                                           int64_t time,
                                           bool testnet) {
  
  /* Testnet has min-difficulty blocks
   * after target_spacing * 100 time between blocks:
   */
  if (testnet && target_spacing * 100 < time)
    return min_test_difficulty;

  while (time > 0 && difficulty > min_difficulty) {

    /* difficulty can max decrease about +/- 1 which is factor ~174  */
    if (difficulty >= TWO_POW48)
      difficulty -= TWO_POW48;

    time -= 26100; // 174 * 150
  }

  if (difficulty < min_difficulty)
    difficulty = min_difficulty;

  return difficulty;
}

/**
 * returns the estimated gaps (blocks) per day 
 * for the given primes per second and difficulty
 */
double PoWUtils::gaps_per_day(double pps, uint64_t difficulty) {
  return (60 * 60 * 24) / (target_work_d(difficulty) / pps);
}
