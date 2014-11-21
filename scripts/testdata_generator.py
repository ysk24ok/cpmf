#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import random

"""
usage: python3 testdata_generator.py 1000 1000 10000 1..5 ../input/testdata
 - arg1: the number of users
 - arg2: the number of items
 - arg3: the number of ratings
 - arg4: the range of ratings
         Minimum and maximum rating should be specified.
         Rating value is the floating point value rounded by 1 digits
         after the decimal point.
 - arg5: path of the output file
"""

def get_msg(user_id, item_id, rating):
    return str(user_id) + " " + str(item_id) + " " + str(rating) + "\n"

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
    minimum_rating = rating_scope[0]
    maximum_rating = rating_scope[1]

    testdata_path = sys.argv[5]


    print("Now processing...")
    existing = []
    f = open(testdata_path, "w")

    # the case of the maximum numbers of users and items
    f.writelines( get_msg(random.randint(1, num_users), num_items,
                    round(random.uniform(minimum_rating, maximum_rating), 1)) );
    f.writelines( get_msg(num_users, random.randint(1, num_items),
                    round(random.uniform(minimum_rating, maximum_rating), 1)) );

    for i in range(0, num_ratings - 2):
        rand_rating = round(random.uniform(minimum_rating, maximum_rating), 1)

        # avoid existing user-item combinations
        while True:
            rand_user_id = random.randint(1, num_users)
            rand_item_id = random.randint(1, num_items)
            combination = (rand_user_id, rand_item_id)
            if combination not in existing:
                existing.append(combination)
                break
        f.writelines( get_msg(rand_user_id, rand_item_id, rand_rating) )
    f.close()
    print("Finished.")
