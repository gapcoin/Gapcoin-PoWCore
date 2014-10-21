# Gapcoin-PoWCore - Gapcoins Proof of Work functionality
---

## The algorithm:


The average length of a prime gap with the starting
prime p, is log(p), which means that the average prime gap size 
increases with lager primes.
<br>
Instead of the pure length, Gapcoin uses the merit of a prime gap, 
which is the ratio of the gap's size to the average gap size.
<br>
Let p be the prime starting a prime gap, then m&nbsp;=&nbsp;gapsize/log(p) 
will be the merit of this prime gap.
<br>
Also a pseudo random number is calculated from p to provide finer 
difficulty adjustment.
<br>
Let rand(p) be a pseudo random function with 
0&nbsp;&#x3c;&nbsp;rand(p)&nbsp;&#x3c;&nbsp;1
<br>
Then, for a prime gap starting at prime p with size s, the 
difficulty will be s/log(p)&nbsp;+&nbsp;2/log(p)&nbsp;&lowast;&nbsp;rand(p), where 2/log(p) 
is the average distance between a gap of size s and s&nbsp;+&nbsp;2 
(the next greater gap) in the proximity of p.
<br>
When it actually comes to mining, there are two additional fields 
added to the Blockheader, named “shift” and “adder”.
<br>
We will calculate the prime p as sha256(Blockheader)&nbsp;&lowast;&nbsp;2^shift&nbsp;+&nbsp;adder.
<br>
As an additional criterion the adder has to be smaller 
than 2^shift to avoid that the PoW could be reused.
<br>

## Mining:

For mining, PoWCore uses a basic prime sieve with some slightly improvements:

### The sieving steps:

 - Calculate the first n primes.
 - In the actual sieve we skip all even numbers, so we want to only sieve
   the odd multiplies of each prime.
 - So, create an additional set of primes and multiply each with two.
 - Make sure the start&ndash;index of the sieve is divisible by two
 - Now calculate for each prime the first odd number in the sieve,
   which is divisible by that prime (called pindex).
 - For each prime p: mark the pindex as composite, add 2 &lowast; p to pindex and 
   mark it as composite, redo till we reach the end of the sieve.
 - For each remaining prime candidate, check primality with the
   Fermat-pseudo-prime-test as it is faster than the Miller-Rabin-test
   (Fermat is not as accurate as the Miller-Rabin and maybe some valid sieve 
   results will not be accepted, but this should be very rare)
 - Now scan the remaining (pseudo) primes for a big prime gap.

#### Additional notes:

  - start&ndash;index can be hash &lowast; 2^shift + [0, 2^shift)
  - max sieve size depends on start index, and is limited by 
    (hash + 2^shift) - start&ndash;index.
  - shift can theoretically be in range [14, 2^16)
    but nodes can choose to only accept shifts till a given amount (e.g. 512)
