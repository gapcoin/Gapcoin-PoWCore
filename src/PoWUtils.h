/**
 * Header file for some Proof of Work related utilities.
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
#ifndef __POWUTILS_H__
#define __POWUTILS_H__

#include <stdlib.h>
#include <inttypes.h>
#include <openssl/sha.h>
#include <gmp.h>
#include <mpfr.h>
#include <vector>
#include <string>

using namespace std;

/* globally enable debugging */
//#define DEBUG

/**
 * converts an byte array to an mpz value
 *
 * last significant word first
 * last significant byte first
 */
#define ary_to_mpz(mpz_res, ary, len) \
  mpz_import(mpz_res, len, -1, sizeof(uint8_t), -1, 0, ary)

/**
 * converts an mpz value to an byte array
 *
 * last significant word first
 * last significant byte first
 */
#define mpz_to_ary(mpz_src, ary, len) \
  mpz_export(ary, len, -1, sizeof(uint8_t), -1, 0, mpz_src)


/* 2^48 */
#define TWO_POW48 (((uint64_t) 1) << 48)

/**
 * sets a mpz to an uint64_t value
 * by checking if unsigned long has 32 or 64 bits
 */
#if __WORDSIZE == 64
#define mpz_set_ui64(mpz, ui64) mpz_set_ui(mpz, ui64)
#else
inline void mpz_set_ui64(mpz_t mpz, uint64_t ui64) {

  mpz_set_ui(mpz, (uint32_t) (ui64 >> 32));      
  mpz_mul_2exp(mpz, mpz, 32);                     
  mpz_add_ui(mpz, mpz, (uint32_t) ui64);       
} 
#endif

/**
 * sets and init a mpz to an uint64_t value
 * by checking if unsigned long has 32 or 64 bits
 */
#if __WORDSIZE == 64
#define mpz_init_set_ui64(mpz, ui64) mpz_init_set_ui(mpz, ui64)
#else
inline void mpz_init_set_ui64(mpz_t mpz, uint64_t ui64) {

  mpz_init_set_ui(mpz, (uint32_t) (ui64 >> 32));  
  mpz_mul_2exp(mpz, mpz, 32);                      
  mpz_add_ui(mpz, mpz, (uint32_t) ui64);        
}
#endif

/**
 * returns the 64 lower bits of an mpz value
 */
#if __WORDSIZE == 64
#define mpz_get_ui64(mpz) mpz_get_ui(mpz)
#else
inline uint64_t mpz_get_ui64(mpz_t mpz) {
  mpz_t mpz_tmp;
  mpz_init_set(mpz_tmp, mpz);
  uint64_t ui64 = mpz_get_ui(mpz_tmp) & 0xffffffff;
  
  mpz_div_2exp(mpz_tmp, mpz_tmp, 32);
  ui64 |= (((uint64_t) mpz_get_ui(mpz_tmp)) & 0xffffffff) << 32;

  mpz_clear(mpz_tmp);
  return ui64;
}
#endif

/**
 * returns whether the given mpz fits an uint64_t
 */
#if __WORDSIZE == 64
#define mpz_fits_uint64_p(mpz) mpz_fits_ulong_p(mpz)
#else
#define mpz_fits_uint64_p(mpz) (mpz_sizeinbase(mpz, 2) <= 64)
#endif

/**
 * sets a mpfr to an uint64_t value
 * by checking if unsigned long has 32 or 64 bits
 */
#if __WORDSIZE == 64
#define mpfr_set_ui64(mpfr, ui64, rand) mpfr_set_ui(mpfr, ui64, rand)
#else
inline void mpfr_set_ui64(mpfr_t mpfr, uint64_t ui64, mp_rnd_t rand) {

  mpfr_set_ui(mpfr, (uint32_t) (ui64 >> 32), rand);        
  mpfr_mul_2exp(mpfr, mpfr, 32, rand);                      
  mpfr_add_ui(mpfr, mpfr, (uint32_t) ui64, rand);        
}
#endif

/**
 * sets a mpfr to an uint64_t value
 * by checking if unsigned long has 32 or 64 bits
 */
#if __WORDSIZE == 64
#define mpfr_init_set_ui64(mpfr, ui64, rand) \
  mpfr_init_set_ui(mpfr, ui64, rand)
#else
inline void mpfr_init_set_ui64(mpfr_t mpfr, uint64_t ui64, mp_rnd_t rand) {

  mpfr_init_set_ui(mpfr, (uint32_t) (ui64 >> 32), rand);        
  mpfr_mul_2exp(mpfr, mpfr, 32, rand);                      
  mpfr_add_ui(mpfr, mpfr, (uint32_t) ui64, rand);        
}
#endif


/* converts an hex char to the lower byte nibble */
#define hex_to_nibble(hex) (((hex) > 57) ? (hex) - 87 : (hex) - 48)

/* converts an lower byte nibble to an hex char */
#define nibble_to_hex(nibble) (((nibble) >= 10) ? (nibble) + 87 : (nibble) + 48)

/* converts two chars of a hex string to a byte */
#define hex_to_byte(str, i)                                           \
  ((hex_to_nibble((str)->c_str()[i]) << 4) | hex_to_nibble((str)->c_str()[i + 1]))

/* sets the context of val from the given hex string */
#define set_from_hex(val, i, str, j)                                  \
  ((uint8_t *) &(val))[i] = hex_to_byte(str, j)
                          

/* adds the context of ary at index i to an hex string */
#define ary_push_hex(str, val, i) do {                                \
  (str).push_back(nibble_to_hex((val)[i] >> 4));                      \
  (str).push_back(nibble_to_hex((val)[i] & 0xf));                     \
} while (0)

/* adds the context of val to an hex string */
#define push_hex(str, val, i) ary_push_hex(str, ((uint8_t *) &val), i)

/* converts an mpz to its hex representation */
inline string mpz_to_hex(mpz_t mpz) {
    
  string hex;
  size_t len;
  uint8_t *ary = (uint8_t *) mpz_to_ary(mpz, NULL, &len);

  /* swap order because of little endian format */
  for (size_t i = len; i > 0; i--)
    ary_push_hex(hex, ary, i - 1);
 
  return hex;
}

class PoWUtils {
  
  public :

  /* should we debug */
#ifdef DEBUG
    static const bool debug = true;
#else
    static const bool debug = false;
#endif

    /**
     * the minimum difficulty
     */
    static const uint64_t min_difficulty = 16 * TWO_POW48;
    static const uint64_t min_test_difficulty = TWO_POW48;

    /**
     * returns the current time in microseconds
     */
    static uint64_t gettime_usec();

    /**
     * calculates the log2 from a mpz value,
     * the return value is 2^accuracy times grater than
     * the actual log2 value to provide a accuracy-bit accuracy
     */
    void mpz_log2(mpz_t mpz_log, mpz_t mpz_src, uint32_t accuracy);

    /**
     * calculates the merit of a given prime gap
     * the return value is 2^48 times grater than
     * the actual merit value to provide a 48 bit accuracy
     */
    uint64_t merit(mpz_t mpz_start, mpz_t mpz_end);

    /**
     * generates a pseudo random number from the given gap
     */
    uint64_t rand(mpz_t mpz_start, mpz_t mpz_end);

    /**
     * generates the current difficulty, which is merit + random(start, end)
     * the return value is 2^48 times grater than
     * the actual merit + rand value to provide a 48 bit accuracy
     */
    uint64_t difficulty(mpz_t mpz_start, mpz_t mpz_end);

    /**
     * returns the given difficulty in human readable format
     */
    double get_readable_difficulty(uint64_t difficulty);

    /**
     * returns the target gap size for a given difficulty and start index
     */
    uint64_t target_size(mpz_t mpz_start, uint64_t difficulty);

    /**
     * returns the estimated work required to find 
     * a gap with the given difficulty, which is e^difficulty
     * (work are the total among of primes to calculate)
     */
    void target_work(vector<uint8_t> *n_primes, uint64_t difficulty);

    /**
     * calculates the next difficulty according to 
     * the given target and actual timespan
     *
     * calculates difficulty + log(target_timespan / actual_timespan)
     * 
     * Note: difficulty increases logarithmically:
     *       a 2,718(e) block speed increase is a +1 difficulty increase
     */
    uint64_t next_difficulty(uint64_t difficulty, 
                             uint64_t actual_timespan,
                             bool testnet);


    /**
     * compute the maximum difficulty decrease from
     * the given difficulty in the given time
     */
    static uint64_t max_difficulty_decrease(uint64_t difficulty, 
                                            int64_t time,
                                            bool testnet);

    /**
     * returns the estimated gaps (blocks) per day 
     * for the given primes per second and difficulty
     */
    double gaps_per_day(double pps, uint64_t difficulty);

    /**
     * allow more than one instance of this to be more cache friendly
     */
    PoWUtils();
    ~PoWUtils();

  private :

    /* log2(e) * 2^(64 + 48) */
    mpz_t mpz_log2e112;

    /* log2(e) * 2^64 */
    mpz_t mpz_log2e64;

    /* target spacing */
    static const int64_t target_spacing = 150;

    /* log(target_spacing) * 2^48 */
    static const uint64_t log_150_48 = 0x502b8fea053a6LL;

    /* 1.0 / 2^47 (for debugging) (not 48 bytes because rounding errors could count as failure) */
    static const double accuracy = 7.105427357601002e-15;

    /**
     * calculates the log from a mpz value
     */
    double mpz_log(mpz_t mpz);
 
    /**
     * calculates the merit of a given prime gap
     * (double version for debugging)
     */
    double merit_d(mpz_t mpz_start, mpz_t mpz_end);
 
    /**
     * generates a pseudo random number from the given gap
     * (double version for debugging)
     */
    double rand_d(mpz_t mpz_start, mpz_t mpz_end);

    /**
     * generates the current difficulty
     * (double version for debugging)
     */
    double difficulty_d(mpz_t mpz_start, mpz_t mpz_end);

    /**
     * calculates the next difficulty according to 
     * the given target and actual timespan
     * (double version for debugging)
     */
    double next_difficulty_d(double difficulty, 
                             uint64_t actual_timespan,
                             bool testnet);

    /**
     * returns the estimated work required to find 
     * a gap with the given difficulty, which is e^difficulty
     * (work are the total among of primes to calculate)
     * (double version)
     */
    double target_work_d(uint64_t difficulty);
};

#endif /* __POWUTILS_H__ */
