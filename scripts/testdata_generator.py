#!/usr/bin/env puthon
# -*- coding: utf-8 -*-

import sys
import random

"""
usage: python3 testdata_generator.py 1000 1000 10000 1..5 ../input/testdata
arg1: the number of users
arg2: the number of items
arg3: the number of ratings
arg4: the range of ratings (lower bound and upper bound should be specified)
arg4: path of the output file
"""
if __name__ == "__main__":
    num_users = int(sys.argv[1])
    if num_users <= 0:
        sys.exit("the number of users should be greater than 0")

    num_items = int(sys.argv[2])
    if num_items <= 0:
        sys.exit("the number of items should be greater than 0")

    num_ratings = int(sys.argv[3])
    if num_ratings <= 0:
        sys.exit("the number of ratings should be greater than 0")

    rating_scope = list( map(int, sys.argv[4].split("..")) )
    lower_bound = rating_scope[0]
    upper_bound = rating_scope[1]

    testdata_path = sys.argv[5]


    print("Now processing...")
    existing = []
    f = open(testdata_path, "w")
    for i in range(0, num_ratings):
        # avoid existing user-item combinations
        while True:
            rand_user = random.randint(0, num_users)
            rand_item = random.randint(0, num_items)
            combination = (rand_user, rand_item)
            if combination not in existing:
                existing.append(combination)
                break
        rand_rating = random.randint(lower_bound, upper_bound)
        f.writelines([str(rand_user) + " ", str(rand_item) + " ", str(rand_rating)+"\n"])
    f.close()
    print("Finished.")
