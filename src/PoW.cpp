/**
 * Implementation of Gapcoins Proof of Work functionality.
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
#include <openssl/sha.h>
#include <gmp.h>
#include <mpfr.h>
#include <sstream>
#include <string>
#include "PoW.h"

using namespace std;

/**
 * Create a new PoW out of the given hash, shift, adder and difficulty
 * in calculation format
 */
PoW::PoW(mpz_t mpz_hash, 
         uint16_t shift, 
         mpz_t mpz_adder, 
         uint64_t difficulty,
         uint32_t nonce) {
  
  if (mpz_hash == NULL)
    mpz_init_set_ui64(this->mpz_hash, 0);
  else
    mpz_init_set(this->mpz_hash, mpz_hash);

  if (mpz_adder == NULL)
    mpz_init_set_ui64(this->mpz_adder, 0);
  else
    mpz_init_set(this->mpz_adder, mpz_adder);

  this->nonce = nonce;
  this->shift = shift;
  this->utils = new PoWUtils();

  target_difficulty = difficulty;
}

/**
 * Create a new PoW out of the given hash, shift, adder and difficulty
 * in block header format
 */
PoW::PoW(const vector<uint8_t> *const hash, 
         const uint16_t shift, 
         const vector<uint8_t> *const adder, 
         const uint64_t difficulty,
         uint32_t nonce) {
  
  mpz_init_set_ui64(mpz_hash, 0);
  mpz_init_set_ui64(mpz_adder, 0);
  this->nonce = nonce;
  this->shift = shift;
  this->utils = new PoWUtils();

  if (hash != NULL)
    ary_to_mpz(mpz_hash, hash->data(), hash->size());

  if (adder != NULL)
    ary_to_mpz(mpz_adder, adder->data(), adder->size());

  target_difficulty = difficulty;
}

PoW::~PoW() {
  
  mpz_clear(mpz_hash);
  mpz_clear(mpz_adder);
  delete utils;
}

/**
 * calculates the start and end prime for this pow
 * returns whether the start and end calculated correctly
 */
bool PoW::get_end_points(mpz_t mpz_start, mpz_t mpz_end) {

  /**
   * shift hast to be greater or equal than 14
   */
  if (shift < 14)
    return false;

/* Compile time opt-in protection 
 * from dos attacks with high shift */
#ifdef MAX_SHIFT
  if (shift > MAX_SHIFT)
    return false;
#endif

  /**
   * make sure that hash is in range (2^255, 2^256)
   */
  if (mpz_sizeinbase(mpz_hash, 2) != 256)
    return false;

  /**
   * make sure adder is smaller than 2^shift
   */
  if (mpz_sizeinbase(mpz_adder, 2) > shift)
    return false;

  mpz_init_set(mpz_start, mpz_hash);
  mpz_mul_2exp(mpz_start, mpz_start, shift);
  mpz_add(mpz_start, mpz_start, mpz_adder);

  /* start has to be a prime */
  if (!mpz_probab_prime_p(mpz_start, 25)) {
    
    mpz_clear(mpz_start);
    return false;
  }

  mpz_init(mpz_end);
  mpz_nextprime(mpz_end, mpz_start);

  return true;
}

/**
 * returns the uint64 difficulty
 */
uint64_t PoW::difficulty() {

  mpz_t mpz_start, mpz_end;
  if (!get_end_points(mpz_start, mpz_end))
    return 0;

  uint64_t diff = utils->difficulty(mpz_start, mpz_end);

  mpz_clear(mpz_start);
  mpz_clear(mpz_end);

  return diff;
}

/**
 * returns the uint64 merit
 */
uint64_t PoW::merit() {

  mpz_t mpz_start, mpz_end;
  if (!get_end_points(mpz_start, mpz_end))
    return 0;

  uint64_t merit = utils->merit(mpz_start, mpz_end);

  mpz_clear(mpz_start);
  mpz_clear(mpz_end);

  return merit;
}

/**
 * returns the vector gap
 */
bool PoW::get_gap(vector<uint8_t> *start, vector<uint8_t> *end) {

  start->assign(1, 0);
  end->assign(1, 0);
  
  mpz_t mpz_start, mpz_end;
  if (!get_end_points(mpz_start, mpz_end))
    return false;

  uint8_t *start_ary, *end_ary;
  size_t start_len = 0, end_len = 0;

  start_ary = (uint8_t *) mpz_to_ary(mpz_start, NULL, &start_len);
  end_ary   = (uint8_t *) mpz_to_ary(mpz_end,  NULL, &end_len);

  start->assign(start_ary, start_ary + start_len);
  end->assign(end_ary, end_ary + end_len);

  return true;
}

/**
 * returns the gap length for this PoW
 */
uint64_t PoW::gap_len() {

  mpz_t mpz_start, mpz_end;
  if (!get_end_points(mpz_start, mpz_end))
    return 0;

  mpz_t mpz_len;
  mpz_init(mpz_len);

  mpz_sub(mpz_len, mpz_end, mpz_start);
  uint64_t len = 0;

  if (mpz_fits_uint64_p(mpz_len))
    len = mpz_get_ui64(mpz_len);

  mpz_clear(mpz_len);
  mpz_clear(mpz_start);
  mpz_clear(mpz_end);
  return len;
}

/* returns whether this PoW is valid or not */
bool PoW::valid() { 
  return difficulty() >= target_difficulty; 
}

/**
 * returns the target min gap size for a given start
 */ 
uint64_t PoW::target_size(mpz_t mpz_start) {
  return utils->target_size(mpz_start, target_difficulty);
}

/*****************************/
/* getter and setter methods */
/*****************************/

void PoW::get_hash(mpz_t mpz_hash) { 
  mpz_set(mpz_hash, this->mpz_hash); 
}

uint16_t PoW::get_shift() { 
  return shift; 
}

uint32_t PoW::get_nonce() {
  return nonce;
}

void PoW::set_shift(uint16_t shift) { 
  this->shift = shift; 
}

void PoW::get_adder(mpz_t mpz_adder) {
  mpz_set(mpz_adder, this->mpz_adder);
}

void PoW::get_adder(vector<uint8_t> *adder) {

  size_t len;
  uint8_t *ary = (uint8_t *) mpz_to_ary(mpz_adder, NULL, &len);
  
  adder->assign(ary, ary + len);
  free(ary);
}

void PoW::set_adder(mpz_t mpz_adder) {
  mpz_set(this->mpz_adder, mpz_adder);
}

void PoW::set_adder(vector<uint8_t> *adder) {
  
  if (adder != NULL)
    ary_to_mpz(mpz_adder, adder->data(), adder->size());
}

/* returns a string representation of this */
string PoW::to_s() {
  stringstream ss;

  ss << "PoW: " << (valid() ? "valid" : "invalid") << "\n";
  ss << "  hash:  " << mpz_to_hex(mpz_hash) << "\n";
  ss << "  nonce: " << nonce << "\n";
  ss << "  shift: " << shift << "\n";
  ss << "  adder: " << mpz_to_hex(mpz_adder) << "\n";
  ss << "  diff:  " << target_difficulty << "\n";

  mpz_t mpz_start, mpz_end;
  if (get_end_points(mpz_start, mpz_end)) {
    
    ss << "---------\n";
    ss << "  start: " << mpz_to_hex(mpz_start) << "\n";
    ss << "  end:   " << mpz_to_hex(mpz_end) << "\n";
    ss << "  len:   " << gap_len() << "\n";
    ss << "  merit: " << (((double) merit()) / TWO_POW48) << "\n";
    mpz_clear(mpz_start);
    mpz_clear(mpz_end);
  }

  return ss.str();
}
