import csv
import decimal as dec
import struct
import sys
import random as rand


def bin2float(b):
    h = int(b, 2).to_bytes(8, byteorder="big")
    return struct.unpack('>d', h)[0]


def float2bin(f):
    [d] = struct.unpack(">Q", struct.pack(">d", f))
    b = f'{d:064b}'
    return ''.join([b[:1], " 0x", b[1:12], " 0x", b[12:]])


def compute_trace(filename):
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

        print("{},{},{}".format(cnt, tot, tot / cnt))


def generate_trace(filename, callback, num):
    with open(filename, "w") as f:
        f.write("\n")

        for n in range(num):
            f.write("{}\n".format(str(callback(n))))


def random(n):
    return rand.random()


if __name__ == '__main__':
    dec.getcontext().prec = 2000
    compute_trace("traces/test.csv")

    generate_trace("traces/test.csv", random, 15)
