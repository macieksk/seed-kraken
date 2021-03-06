/*
 * Copyright 2013-2015, Derrick Wood <dwood@cs.jhu.edu>
 *
 * This file is part of the Kraken taxonomic sequence classification system.
 *
 * Kraken is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kraken is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kraken.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KRAKENUTIL_HPP
#define KRAKENUTIL_HPP

#include "kraken_headers.hpp"

#include "seedmod/seedmod.hpp"

namespace kraken {
  // Build a map of node to parent from an NCBI taxonomy nodes.dmp file
  std::map<uint32_t, uint32_t> build_parent_map(std::string filename);

  // Return the lowest common ancestor of a and b, according to parent_map
  // NOTE: LCA(0,x) = LCA(x,0) = x
  uint32_t lca(std::map<uint32_t, uint32_t> &parent_map,
    uint32_t a, uint32_t b);

  // Resolve classification tree
  uint32_t resolve_tree(std::map<uint32_t, uint32_t> &hit_counts,
                        std::map<uint32_t, uint32_t> &parent_map);

  class KmerScanner {
    public:
	typedef __uint128_t base_type_in;
	typedef uint64_t base_type_in_half;  //Should be half the size of base_type_in
	typedef uint64_t base_type_out;

    KmerScanner(std::string &seq, size_t start=0, size_t finish=~0);
    base_type_in *next_kmer();  // NULL when seq exhausted
    bool ambig_kmer();  // does last returned kmer have non-ACGT?


    static uint8_t get_k();
    // MUST be called before first invocation of KmerScanner()
    static void set_k(uint8_t n);

    //Chaining input/output iterators to squash seed
    static void squash_kmer_for_read(const char * seed, const base_type_in & fmer, base_type_out & ret_m){
    	kraken::squash_kmer_for_read(seed, k,fmer,ret_m);
    };
    static void squash_kmer_for_index(const char * seed, const base_type_in & fmer, base_type_out & ret_m){
    	kraken::squash_kmer_for_index(seed, k,fmer,ret_m);
    };

    // Code mostly from Jellyfish 1.6 source    
    /*
    static uint64_t reverse_complement(uint64_t kmer) {
      kmer = ((kmer >> 2)  & 0x3333333333333333UL) | ((kmer & 0x3333333333333333UL) << 2);
      kmer = ((kmer >> 4)  & 0x0F0F0F0F0F0F0F0FUL) | ((kmer & 0x0F0F0F0F0F0F0F0FUL) << 4);
      kmer = ((kmer >> 8)  & 0x00FF00FF00FF00FFUL) | ((kmer & 0x00FF00FF00FF00FFUL) << 8);
      kmer = ((kmer >> 16) & 0x0000FFFF0000FFFFUL) | ((kmer & 0x0000FFFF0000FFFFUL) << 16);
      kmer = ( kmer >> 32                        ) | ( kmer                         << 32);
      return (((uint64_t)-1) - kmer) >> (8 * sizeof(kmer) - (k << 1)); 
    */
    // Code mostly from Jellyfish 1.6 source
    static __uint128_t reverse_complement(__uint128_t kmer) {
      uint64_t kmer_h1,kmer_h2;
      kmer_h1 = ((kmer >> 2)  & 0x3333333333333333UL) | ((kmer & 0x3333333333333333UL) << 2);
      kmer_h1 = ((kmer_h1 >> 4)  & 0x0F0F0F0F0F0F0F0FUL) | ((kmer_h1 & 0x0F0F0F0F0F0F0F0FUL) << 4);
      kmer_h1 = ((kmer_h1 >> 8)  & 0x00FF00FF00FF00FFUL) | ((kmer_h1 & 0x00FF00FF00FF00FFUL) << 8);
      kmer_h1 = ((kmer_h1 >> 16) & 0x0000FFFF0000FFFFUL) | ((kmer_h1 & 0x0000FFFF0000FFFFUL) << 16);
      kmer_h1 = ( kmer_h1 >> 32                        ) | ( kmer_h1                         << 32);
      kmer_h2 = ((kmer >> 66)  & 0x3333333333333333UL) | (((kmer>>64) & 0x3333333333333333UL) << 2);
      kmer_h2 = ((kmer_h2 >> 4)  & 0x0F0F0F0F0F0F0F0FUL) | ((kmer_h2 & 0x0F0F0F0F0F0F0F0FUL) << 4);
      kmer_h2 = ((kmer_h2 >> 8)  & 0x00FF00FF00FF00FFUL) | ((kmer_h2 & 0x00FF00FF00FF00FFUL) << 8);
      kmer_h2 = ((kmer_h2 >> 16) & 0x0000FFFF0000FFFFUL) | ((kmer_h2 & 0x0000FFFF0000FFFFUL) << 16);
      kmer_h2 = ( kmer_h2 >> 32                        ) | ( kmer_h2                         << 32);
      kmer = kmer_h1;
      kmer <<=64;
      kmer |= kmer_h2;
      return (((__uint128_t)-1) - kmer) >> (8 * sizeof(kmer) - (k << 1));
    }

    private:
    std::string *str;
    size_t curr_pos, pos1, pos2;
    base_type_in kmer;  // the kmer, address is returned (don't share b/t thr.)
    base_type_in_half ambig; // is there an ambiguous nucleotide in the kmer?
    int64_t loaded_nt;

    static uint8_t k;  // init. to 0 b/c static
    static base_type_in kmer_mask;
    static base_type_in_half mini_kmer_mask;
  };
}

#endif
