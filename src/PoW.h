/**
 * Header file of Gapcoins Proof of Work functionality.
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
#ifndef __POW_H__
#define __POW_H__

#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <limits.h>
#include <openssl/sha.h>
#include <gmp.h>
#include <mpfr.h>
#include <vector>
#include <string>
#include "PoWUtils.h"

using namespace std;

class PoW {
  
  public :
    PoW(mpz_t mpz_hash, 
        uint16_t shift, 
        mpz_t mpz_adder, 
        uint64_t difficulty, 
        uint32_t nonce = 0);
    PoW(const vector<uint8_t> *const hash, 
        const uint16_t shift, 
        const vector<uint8_t> *const adder, 
        const uint64_t difficulty,
        uint32_t nonce = 0);
    ~PoW();
    uint64_t difficulty();
    uint64_t merit();
    bool get_gap(vector<uint8_t> *start, vector<uint8_t> *end);
    uint64_t gap_len();
    bool valid();
    uint64_t target_size(mpz_t mpz_start);
    string to_s();
    void     get_hash(mpz_t mpz_hash);
    uint16_t get_shift();
    uint32_t get_nonce();
    void     set_shift(uint16_t shift);
    void     get_adder(mpz_t mpz_adder);
    void     get_adder(vector<uint8_t> *adder);
    void     set_adder(mpz_t mpz_adder);
    void     set_adder(vector<uint8_t> *adder);
};

#endif /* __POW_H__ */
