cpmf: Collection of Parallel Matrix Factorization
====

## Prerequisite

### required

`piconjson` is needed to parse config.json.

```sh
$ git clone https://github.com/kazuho/picojson.git vendor/picojson
```

### optional

If you want to use MassiveThreads as a task parallel library,
install it by the following command.

```sh
$ git clone https://github.com/massivethreads/massivethreads.git vendor/massivethreads
$ cd vendor/massivethreads
$ ./configure --prefix=/usr/local
$ make && make install
```

When you change PREFIX from `/usr/local`, be sure to also change `MYTH_PATH` in Makefile.

## Converting MovieLens data

Use `scripts/convert_movielens.py` to convert MovieLens data format to cpmf format.

To convert [MovieLens 100K Dataset](https://grouplens.org/datasets/movielens/100k/),

```sh
$ python scripts/convert_movielens.py PATH/ml-100k/u.data > input/ml-100k
```

To convert [MovieLens 1M dataset](https://grouplens.org/datasets/movielens/1m/),

```sh
$ python scripts/convert_movielens.py PATH/ml-1m/ratings.dat --separator :: > input/ml-1m
```

To convert [MovieLens 10M dataset](https://grouplens.org/datasets/movielens/10m/)

```sh
$ python scripts/convert_movielens.py PATH/ml-10M100K/ratings.dat --separator :: > input/ml-10m
```

## Parallel methods

Users can designate the parallel method by `DPARALLEL` in Makefile.

### FPSGD

In FPSGD, the rating matrix is divided into many blocks
and multiple threads work on blocks not to share the same row or column.

If you want to use FPSGD method, specify `DPARALLEL = -DFPSGD`.

- Reference

  Y.Zhuang, W-S.Chin and Y-C.Juan and C-J.Lin,
  "A fast parallel SGD for matrix factorization in shared memory systems",
  RecSys'13, [paper](http://dl.acm.org/citation.cfm?id=2507164)

### dcMF (by Intel Cilk or MassiveThreads)

dcMF is our proposing way to parallelize matrix factorization
by recursively dividing the rating matrix into 4 smaller blocks
and dynamically assigning the created tasks to threads.

If you want to use dcMF, specify `DPARALLEL = -DTP_BASED`.

To decide which task parallel library to use, you should set as follows:
`DTP = -DTP_CILK` for Intel Cilk or `DTP = -DTP_MYTH` for MassiveThreads.

- Reference

  Y. Nishioka, and K. Taura. "Scalable task-parallel SGD on matrix factorization in multicore architectures." Parallel and Distributed Processing Symposium Workshop (IPDPSW), 2015 IEEE International. [paper](https://ieeexplore.ieee.org/document/7284444)


## How to use

Just make and run!

```
$ make
$ ./mf train config.json
```
