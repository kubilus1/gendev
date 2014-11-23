#  timez80.py
#
#       Calculate z80 cycle times from a listing file.
#
#   Matt Kubilus (c) 2013
#
#

import re
import csv
import argparse

DEBUG=False

def timeit(listingfile):

    z80res = []
    z80times = []

    with open('z80.csv', 'rb') as z80ops:
        z80dict = csv.DictReader(z80ops)
        for item in z80dict:
            opcodes = []
            opcode = item['Opcode']

            if DEBUG:
                print opcode
            if 'r' in opcode and 'b' in opcode:
                if DEBUG:
                    print "Original:", opcode
                ops = opcode.split()
                prefix = []
                sufix = []
                found = False
                for op in ops:
                    if 'r' in op and 'b' in op:
                        op = "0x%s" % op
                        op = [["%.2x" % eval(
                            op.replace('r',str(r)).replace('b',str(b))
                            ) for r in range(0,8)] for b in range(0,8)]
                    elif found:
                        sufix.append(op)
                    else:
                        prefix.append(op)
                biglist = []
                [biglist.extend(x) for x in op]
                op = [" ".join((prefix + [x] + sufix)) for x in biglist]
                if DEBUG:
                    print "Converted:", op
                opcodes.extend(op)
            elif 'r' in opcode:    
                if DEBUG:
                    print "Original:", opcode
                ops = opcode.split()
                outops = []
                prefix = []
                sufix = []
                found = False
                for op in ops:
                    if 'r*' in op:
                        op = "0x%s" % op
                        outops = ["%x" % eval(
                            op.replace('r*',str(r))
                        ) for r in range(0,8)]
                        found = True
                    elif 'r' in op:
                        op = "0x%s" % op
                        outops = ["%.2x" % eval(
                            op.replace('r',str(r))
                        ) for r in range(0,8)]
                        found = True
                    elif found:
                        sufix.append(op)
                    else:
                        prefix.append(op)
                
                op = [" ".join((prefix + [x] + sufix)) for x in outops]
                if DEBUG:
                    print "Converted:", op
                opcodes.extend(op)
            elif 'b' in opcode:
                if DEBUG:
                    print "Original:", opcode
                ops = opcode.split()
                prefix = []
                sufix = []
                found = False
                for op in ops:
                    if 'b' in op:
                        op = "0x%s" % op
                        op = ["%.2x" % eval(
                            op.replace('b',str(r))
                        ) for r in range(0,8)]
                    elif found:
                        sufix.append(op)
                    else:
                        prefix.append(op)
                op = [" ".join((prefix + [x] + sufix)) for x in op]
                opcodes.extend(op)
                if DEBUG:
                    print "Converted:", op
            else:
                opcodes.append(opcode)

            for op in opcodes:
                RE = "[0-9A-F]{4}\s%s  " % op.replace(
                        'X', '[0-9A-F]'
                    ).replace(' ','[ rs]')
                if DEBUG:
                    print RE
                tempre = re.compile(RE, re.I)
                z80res.append((tempre,item['Time'],RE))

    with open(listingfile, 'r') as listing:
        lines = listing.readlines()
        for line in lines:
            ztime = ""
            matchRE = ""
            for z80re, z80time, RE in z80res:
                 
                if z80re.search(line):
                    ztime = "%2s" % z80time
                    matchRE = RE.strip()
                    break
            if DEBUG:
                print "%6s | %35s | %s " % (ztime, matchRE, line),      
            else:
                print "%6s   %s " % (ztime, line),      
                
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("listingfile", type=str)
    args = parser.parse_args()
    timeit(args.listingfile)


if __name__ == "__main__":
    main()
