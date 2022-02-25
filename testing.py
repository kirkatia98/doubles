import csv
import decimal as dec
import struct
import sys
import random as rand
import math


def bin2float(b):
    h = int(b, 2).to_bytes(8, byteorder="big")
    return struct.unpack('>d', h)[0]


def float2bin(f):
    [d] = struct.unpack(">Q", struct.pack(">d", f))
    b = f'{d:064b}'
    return ''.join([b[:1], " 0x", b[1:12], " 0x", b[12:]])


def generate_trace(filename, func, num):
    with open(filename, "w") as f:
        for n in range(num):
            f.write("\n")
            val = func(n)
            f.write("{0:.60f}".format(val))


def compute_trace(filename):
    dec.getcontext().prec = 2000
    with open(filename, "r") as f:
        f.readline()
        tot = dec.Decimal(0)
        cnt = dec.Decimal(0)

        lines = f.readlines()
        for ln in lines:
            tup = ln.split(",")
            val = dec.Decimal(tup[0])

            if len(tup) > 1:
                mult = dec.Decimal(tup[1])
            else:
                mult = dec.Decimal(1)

            cnt += mult
            tot += val * mult

        print("n: {}".format(cnt))
        print("sum: {}".format(tot))
        print("avg: {}".format(tot/cnt))
        print()


def output_trace(filename):
    with open(filename, "r") as f:
        for ln in f:
            print(ln.strip())


# Switch statement of callback functions that take an integer argument.
# Each function is configured with the args specific to it
def callback(*args):
    f = args[0]

    def default(n):
        return rand.random()

    def uniform(n):
        a = args[1]
        b = args[2]
        return rand.uniform(a, b)

    def gauss(n):
        mu = args[1]
        sigma = args[2]
        return rand.gauss(mu, sigma)

    def triangular(n):
        low = args[1]
        high = args[2]
        mode = args[3]
        return rand.triangular(low, high, mode)

    def sine(x):
        freq = args[1]
        phase = args[2]
        amp = args[3]
        shift = args[4]
        return amp * math.sin(x * freq + phase) + shift

    def tan(x):
        freq = args[1]
        phase = args[2]
        amp = args[3]
        shift = args[4]
        return amp * math.tan(x * freq + phase) + shift

    funcs = {
        "uni": uniform,
        "tri": triangular,
        "gauss": gauss,
        "sine": sine,
        "tan": tan
    }

    return funcs.get(f, default)


if __name__ == '__main__':

    file = "traces/uni-small.csv"
    func = callback("uni", 10, 100)

    generate_trace(file, func, 100000)
    compute_trace(file)
