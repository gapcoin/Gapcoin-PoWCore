/**
 * Implementation of some Proof of Work related utilities.
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

void PoWUtils::mpz_log2(mpz_t mpz_log, mpz_t mpz_src, uint32_t accuracy) {
  (void) mpz_log;
  (void) mpz_src;
  (void) accuracy;
}

uint64_t PoWUtils::merit(mpz_t mpz_start, mpz_t mpz_end) {
  (void) mpz_start;
  (void) mpz_end;
  return 0;
}

uint64_t PoWUtils::rand(mpz_t mpz_start, mpz_t mpz_end) {
  (void) mpz_start;
  (void) mpz_end;
  return 0;
}

uint64_t PoWUtils::difficulty(mpz_t mpz_start, mpz_t mpz_end) {
  (void) mpz_start;
  (void) mpz_end;
  return 0;
}

double PoWUtils::get_readable_difficulty(uint64_t difficulty) {
  (void) difficulty;
  return 0.0;
}

PoWUtils::PoWUtils() { }

PoWUtils::~PoWUtils() { }

uint64_t PoWUtils::target_size(mpz_t mpz_start, uint64_t difficulty) {
  (void) mpz_start;
  (void) difficulty;
  return 0;
}

void PoWUtils::target_work(vector<uint8_t> *n_primes, 
                           uint64_t difficulty) {
  (void) n_primes;
  (void) difficulty;
}

uint64_t PoWUtils::gettime_usec() {
  return 0;
}

uint64_t PoWUtils::next_difficulty(uint64_t difficulty, 
                                   uint64_t actual_timespan,
                                   bool testnet) {
  (void) difficulty;
  (void) actual_timespan;
  (void) testnet;
  return 0;
}

uint64_t PoWUtils::max_difficulty_decrease(uint64_t difficulty, 
                                           int64_t time,
                                           bool testnet) {
  (void) difficulty;
  (void) time;
  (void) testnet;
  return 0;
}

double PoWUtils::gaps_per_day(double pps, uint64_t difficulty) {
  (void) pps;
  (void) difficulty;
  return 0.0;
}
