import sys, math, numpy as np
from fpbinary import FpBinary

def main():
    if len(sys.argv) != 4:
        print("Failed. Required args: a quoted string representing the math function you want a LUT for, the function's period, and a boolean representing whether the LUT's length should be 256 or 512.")
        return
    func = sys.argv[1]
    period = int(sys.argv[2])
    trueStatements = {"True", "true", "1", "512"}
    isLrg = 1 if trueStatements.__contains__(sys.argv[3]) else 0
    x = FpBinary(int_bits=4, frac_bits=12, signed=True, value=0)
    iter = FpBinary(int_bits=4, frac_bits=12, signed=True, bit_field=(period) << (4 - isLrg))
    try:
        f = open("lut.c", "w")
    except:
        print("lut.c already exists. Exiting")
        return
    with open("lut.c", "a") as f:
        f.write("const s16 lut[] = {\n")
        while x < period:
            y = FpBinary(format_inst=x, value=eval(func))
            f.write("    0b" + np.binary_repr(y) + ",\n")
            x += iter
        f.write("};\n")

main()
