import argparse
import sys

parser = argparse.ArgumentParser()
parser.add_argument('filepath', help='file tp convert')
parser.add_argument('-s', '--separator', default='\t', help='separator')
args = parser.parse_args()

s = ''
with open(args.filepath) as f:
    for line in f.readlines():
        user_id, item_id, rating, _ = line.split(args.separator)
        s += '{} {} {}\n'.format(user_id, item_id, rating)
print(s.strip())
