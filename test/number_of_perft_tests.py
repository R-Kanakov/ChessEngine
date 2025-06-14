import sys
with open(sys.argv[1], 'r', encoding='utf-8') as f:
    print(sum(1 for _ in f))