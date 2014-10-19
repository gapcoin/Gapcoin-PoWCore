/**
 * Implementation of Gapcoins Proof of Work calculation unit.
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
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include <gmp.h>
#include <mpfr.h>

#include "Sieve.h"

using namespace std;


Sieve::Sieve(PoWProcessor *pprocessor, uint64_t n_primes, uint64_t sievesize) {
  (void) pprocessor;
  (void) n_primes;
  (void) sievesize;
}


Sieve::~Sieve() { }

void Sieve::set_pprocessor(PoWProcessor *pprocessor) {
  (void) pprocessor;
}

void Sieve::run_sieve(PoW *pow, vector<uint8_t> *offset) {
  (void) pow;
  (void) offset;
}

double Sieve::avg_primes_per_sec() {
  return 0.0;
}

double Sieve::avg_gaps10_per_hour() {
  return 0.0;
}

double Sieve::avg_gaps15_per_hour() {
  return 0.0;
}

double Sieve::primes_per_sec() {
  return 0.0;
}

double Sieve::gaps10_per_hour() {
  return 0.0;
}

double Sieve::gaps15_per_hour() {
  return 0.0;
}

uint64_t Sieve::get_found_primes() {
  return 0.0;
}
