/**
 * Implementation of Gapcoins Proof of Work functionality.
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
#include <openssl/sha.h>
#include <gmp.h>
#include <mpfr.h>
#include <sstream>
#include <string>
#include "PoW.h"

using namespace std;

PoW::PoW(mpz_t mpz_hash, 
         uint16_t shift, 
         mpz_t mpz_adder, 
         uint64_t difficulty,
         uint32_t nonce) {
  (void) mpz_hash;
  (void) shift;
  (void) mpz_adder;
  (void) difficulty;
  (void) nonce;
}

PoW::PoW(const vector<uint8_t> *const hash, 
         const uint16_t shift, 
         const vector<uint8_t> *const adder, 
         const uint64_t difficulty,
         uint32_t nonce) {
  (void) hash;
  (void) shift;
  (void) adder;
  (void) difficulty;
  (void) nonce;
}

PoW::~PoW() { }

uint64_t PoW::difficulty() {
  return 0;
}

uint64_t PoW::merit() {
  return 0;
}

bool PoW::get_gap(vector<uint8_t> *start, vector<uint8_t> *end) {
  (void) start;
  (void) end;
  return false;
}

uint64_t PoW::gap_len() {
  return 0;
}

bool PoW::valid() { 
  return false;
}

uint64_t PoW::target_size(mpz_t mpz_start) {
  (void) mpz_start;
  return 0;
}

void PoW::get_hash(mpz_t mpz_hash) { 
  (void) mpz_hash;
}

uint16_t PoW::get_shift() { 
  return 0; 
}

uint32_t PoW::get_nonce() {
  return 0;
}

void PoW::set_shift(uint16_t shift) { 
  (void) shift; 
}

void PoW::get_adder(mpz_t mpz_adder) {
  (void) mpz_adder;
}

void PoW::get_adder(vector<uint8_t> *adder) {
  (void) adder;
}

void PoW::set_adder(mpz_t mpz_adder) {
  (void) mpz_adder;
}

void PoW::set_adder(vector<uint8_t> *adder) {
  (void) adder;
}

string PoW::to_s() {
  return string("");
}
