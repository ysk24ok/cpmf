cpmf: Collection of Parallel Matrix Factorization
====

This Library is under construction ...

# Prerequisite

## must

*cpmf* needs picojson as a JSON parser.

```
$ git clone https://github.com/kazuho/picojson.git vendor/picojson
```

## optional

If you want to use *MassiveThreads* as a task parallel library, please `svn checkout`.
```
$ svn checkout http://massivethreads.googlecode.com/svn/trunk/ massivethreads
```


# How to use

```
$ make
$ ./mf train config.json
```
