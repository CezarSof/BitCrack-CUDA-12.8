# BitCrack-CUDA-12.8
This is https://github.com/brichard19/BitCrack modified to work with CUDA 12.8


BitCrack
A tool for brute-forcing Bitcoin private keys. The main purpose of this project is to contribute to the effort of solving the Bitcoin puzzle transaction: A transaction with 32 addresses that become increasingly difficult to crack.


Using BitCrack


```-i```, --in FILE
    Read addresses from FILE, one address per line. If FILE is "-" then stdin is read

```-o```, --out FILE
    Append private keys to FILE, one per line

```-d```, --device N
    Use device with ID equal to N

```-b```, --blocks BLOCKS
    The number of CUDA blocks

```-t```, --threads THREADS
    Threads per block

```-p```, --points NUMBER
    Each thread will process NUMBER keys at a time

```--keyspace``` KEYSPACE
    Specify the range of keys to search, where KEYSPACE is in the format,

START:END start at key START, end at key END
	START:+COUNT start at key START and end at key START + COUNT
    :END start at key 1 and end at key END
	:+COUNT start at key 1 and end at key 1 + COUNT

```-c```, --compressed
    Search for compressed keys (default). Can be used with -u to also search uncompressed keys

```-u```, --uncompressed
    Search for uncompressed keys, can be used with -c to search compressed keys

```--compression MODE```
    Specify the compression mode, where MODE is 'compressed' or 'uncompressed' or 'both'

```--list-devices```
    List available devices

```--stride NUMBER```
    Increment by NUMBER

```--share M/N```
    Divide the keyspace into N equal sized shares, process the Mth share

```--continue FILE```
    Save/load progress from FILE


Examples


The simplest usage, the keyspace will begin at 0, and the CUDA parameters will be chosen automatically

```./bin/cuBitCrack-CUDA-12.8 1FshYsUh3mqgsG29XpZ23eLjWV8Ur3VwH```
Multiple keys can be searched at once with minimal impact to performance. Provide the keys on the command line, or in a file with one address per line

```./bin/cuBitCrack-CUDA-12.8 1FshYsUh3mqgsG29XpZ23eLjWV8Ur3VwH 15JhYXn6Mx3oF4Y7PcTAv2wVVAuCFFQNiP 19EEC52krRUK1RkUAEZmQdjTyHT7Gp1TYT```
To start the search at a specific private key, use the --keyspace option:

```./bin/cuBitCrack-CUDA-12.8 --keyspace 766519C977831678F0000000000 1FshYsUh3mqgsG29XpZ23eLjWV8Ur3VwH```
The --keyspace option can also be used to search a specific range:

```./bin/cuBitCrack-CUDA-12.8 --keyspace 80000000:ffffffff 1FshYsUh3mqgsG29XpZ23eLjWV8Ur3VwH```


To periodically save progress, the --continue option can be used. This is useful for recovering after an unexpected interruption:
   
    
Choosing the right parameters for your device


GPUs have many cores. Work for the cores is divided into blocks. Each block contains threads.

There are 3 parameters that affect performance: blocks, threads per block, and keys per thread.

blocks: Should be a multiple of the number of compute units on the device. The default is 32.

threads: The number of threads in a block. This must be a multiple of 32. The default is 256.

Keys per thread: The number of keys each thread will process. The performance (keys per second) increases asymptotically with this value. The default is256. Increasing this value will cause the kernel to run longer, but more keys will be processed.


Building in Linux
Using make:

Build CUDA:

```make BUILD_CUDA=1```
