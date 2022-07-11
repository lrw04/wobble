from sys import argv

try:
    with open(argv[1], 'r') as f:
        n = int(f.read().strip())
except:
    n = 0
with open(argv[1], 'w') as f:
    f.write(str(n + 1) + '\n')
