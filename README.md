# Open addressing vs Buckets toy benchmarks

## TL;DR:

- At low load factors, open addressing outperforms bucketing for inserts and lookups
- As load factor increaes, open addressing and bucketing seem to perform similar for inserts and lookups
- At 99% load factor, insertions as 10x slower with open addressing. Lookups performance is same.
s