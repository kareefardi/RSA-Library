import os

with open("primes.txt") as f:
    primes = f.readlines()

primes = [num.rstrip() for num in primes]

if os.path.exists("primes_list.h"):
    os.remove("primes_list.h")
out = open("primes_list.h", "a+")
out.write("long long primes_list[%d] = {" % len(primes))

count = 0
for num in primes:
    out.write(num)
    out.write(", ")
    if(count == 13):
        out.write("\n")
        count = 0
    count = count + 1

out.seek(-2, os.SEEK_CUR)
out.truncate()
out.write("};")
