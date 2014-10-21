/**
 * Header file of Gapcoins Proof of Work functionality.
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

/**
 * Compile time opt-in protection
 * from dos attacks with high shift
 * (uncomment this)
 */
//#define MAX_SHIFT 512


using namespace std;

class PoW {
  
  public :

    /**
     * Create a new PoW out of the given hash, shift, adder and difficulty
     * in calculation format
     */
    PoW(mpz_t mpz_hash, 
        uint16_t shift, 
        mpz_t mpz_adder, 
        uint64_t difficulty, 
        uint32_t nonce = 0);
 
    /**
     * Create a new PoW out of the given hash, shift, adder and difficulty
     * in block header format
     */
    PoW(const vector<uint8_t> *const hash, 
        const uint16_t shift, 
        const vector<uint8_t> *const adder, 
        const uint64_t difficulty,
        uint32_t nonce = 0);
 
    ~PoW();
 
    /**
     * returns the uint64 difficulty
     */
    uint64_t difficulty();
 
    /**
     * returns the uint64 merit
     */
    uint64_t merit();

    /**
     * returns the vector gap
     */
    bool get_gap(vector<uint8_t> *start, vector<uint8_t> *end);

 
    /**
     * returns the gap length for this PoW
     */
    uint64_t gap_len();
 
    /**
     * returns whether this PoW is valid or not 
     */
    bool valid();

    /**
     * returns the target min gap size for a given start
     */ 
    uint64_t target_size(mpz_t mpz_start);

    /* returns a string representation of this */
    string to_s();

 
    /*****************************/
    /* getter and setter methods */
    /*****************************/
 
    void     get_hash(mpz_t mpz_hash);
    uint16_t get_shift();
    uint32_t get_nonce();
    void     set_shift(uint16_t shift);
    void     get_adder(mpz_t mpz_adder);
    void     get_adder(vector<uint8_t> *adder);
    void     set_adder(mpz_t mpz_adder);
    void     set_adder(vector<uint8_t> *adder);

  private :
    
    /* the block header hash */
    mpz_t mpz_hash;

    /* block nonce (for compatibility) */
    uint32_t nonce;
    
    /* the shift amount */
    uint16_t shift;

    /* the adder to the hash */
    mpz_t mpz_adder;

    /* the target difficulty */
    uint64_t target_difficulty;

    /* PoW calculation utils */
    PoWUtils *utils;

    /**
     * calculates the start and end prime for this pow.
     * returns whether the start and end were calculated correctly
     */
    bool get_end_points(mpz_t mpz_start, mpz_t mpz_end);

};

#endif /* __POW_H__ */
