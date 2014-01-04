import argparse
import struct


class VGM_upgrader(object):
    pcms = []
    pcm_lens = {}

    def __init__(self, infile, outfile):
        self.infile = infile
        self.outfile = outfile

    def print_header(self):
        with open(self.infile, 'rb') as h_in:
            header = h_in.read(192)

        vgm_ident = header[0:3]
        eof_offset = struct.unpack_from('I', header, 4)[0]
        ver_minor = ord(struct.unpack_from('c', header, 8)[0])
        ver_major = ord(struct.unpack_from('c', header, 9)[0])
        num_samples = struct.unpack_from('I', header, 0x18)[0]
        loop_offset = struct.unpack_from('I', header, 0x0C)[0]

        if ver_minor >= 0x50:
            vgm_offset = struct.unpack_from('I', header, 0x34)[0]
        else:
            vgm_offset = 0x0C

        self.vgm_start = 0x34 + vgm_offset

        print "IDENTIFIER: %s" % vgm_ident
        print "LENGTH: %s" % eof_offset
        print "VERSION: %x.%x" % (ver_major, ver_minor)
        print "#SAMPLES: %s" % num_samples
        print "LOOP OFFSET: %s" % loop_offset
        print "VGM OFFSET: 0x%x" % vgm_offset
        print "VGM START: 0x%x" % self.vgm_start

    
    def get_pcms(self):

        pcm_addrs = set()
        pcm_end = 0

        with open(self.infile, 'rb') as h_in:
            header = h_in.read(self.vgm_start)


            opcode = h_in.read(1)
            while opcode:
                #print "Opcode: ", hex(ord(opcode))
                nibble = hex(ord(opcode) & ord('\xF0'))
                if(opcode == '\x67'):
                    trash = h_in.read(1)
                    tt = h_in.read(1)
                    ss = struct.unpack('I', h_in.read(4))[0]
                    print "SS", ss
                    if ss > pcm_end:
                        pcm_end = ss
                    pcm_h_in = h_in.read(ss)
                    self.pcms.append((ss, pcm_h_in))
                
                elif(opcode == '\x50'):
                    op1 = h_in.read(1)
                
                elif(nibble == '0x50'):
                    aa = h_in.read(1)
                    dd = h_in.read(1)

                elif(opcode == '\x61'):
                    nn = h_in.read(1)
                    nn = h_in.read(1)

                elif(opcode == '\x62' or opcode == '\x63'):
                    pass

                elif(opcode == '\x66'):
                    break

                elif(nibble == '0x70'):
                    #print "0x7n"
                    pass

                elif(nibble == '0x80'):
                    #print "0x80"
                    pass

                elif(opcode == '\x90'):
                    print "0x90", opcode
                    ss = h_in.read(1)
                    tt = h_in.read(1)
                    pp = h_in.read(1)
                    cc = h_in.read(1)

                elif(opcode == '\x91'):
                    ss = h_in.read(1)
                    dd = h_in.read(1)
                    ll = h_in.read(1)
                    bb = h_in.read(1)
                    print "0x91: %s,%s,%s,%s" % (hex(ord(ss)),
                            hex(ord(dd)),hex(ord(ll)),hex(ord(bb))) 

                elif(opcode == '\x92'):
                    #print "0x92", opcode
                    ss = h_in.read(1)
                    ff = h_in.read(1)
                    ff = h_in.read(1)
                    ff = h_in.read(1)
                    ff = h_in.read(1)

                elif(opcode == '\x93'):
                    print "0x93", opcode
                    ss = h_in.read(1)
                    aa = h_in.read(1)
                    aa = h_in.read(1)
                    aa = h_in.read(1)
                    aa = h_in.read(1)
                    mm = h_in.read(1)
                    ll = h_in.read(1)
                    ll = h_in.read(1)
                    ll = h_in.read(1)
                    ll = h_in.read(1)

                elif(opcode == '\x94'):
                    print "0x94", opcode
                    ss = h_in.read(1)

                elif(opcode == '\x95'):
                    ss = h_in.read(1)
                    bb = h_in.read(1)
                    bb = h_in.read(1)
                    ff = h_in.read(1)
                    #print "0x95: %s,%s" % (hex(ord(ss)), hex(ord(bb))) 

                elif(opcode == '\xE0'):
                    data = h_in.read(4)
                    #print "%x " % ord(opcode),
                    #print "%x " % ord(data[0]),
                    #print "%x " % ord(data[1]),
                    #print "%x " % ord(data[2]),
                    #print "%x " % ord(data[3])
                    ss = struct.unpack('I', data)[0]
                    pcm_addrs.add(ss) 

                opcode = h_in.read(1)
        
        print "# Data Banks: %s" % len(self.pcms)
        print "# PCM ADDRS: %s" % len(pcm_addrs)
        
        addr_list = list(pcm_addrs)
        addr_list.sort()

        addr_list.append(pcm_end)
        addr_list.pop(0)

        index = 0
        prev_addr = 0
        for addr in addr_list:
            print prev_addr, " ",
            print addr, " ",
            addr_len = addr - prev_addr
            print addr_len
            data = self.pcms[0][1][prev_addr:addr]
            self.pcm_lens[prev_addr] = (index, addr_len, data)
            index += 1
            prev_addr = addr

        #print self.pcm_lens
    
    def upgrade(self):
        with open(self.infile, 'rb') as h_in:
            header = h_in.read(self.vgm_start)

            with open(self.outfile, 'wb') as h_out:
                h_out.write(header)
                
                if(self.pcm_lens):
                    #for i in xrange(len(self.pcm_lens)):
                    pcm_list = self.pcm_lens.values()
                    pcm_list.sort()
                    h_in.read(7)
                    for pcm_data in pcm_list:
                        pcm_idx = pcm_data[0]
                        pcm_len = pcm_data[1]
                        pcm_bytes = pcm_data[2]

                        print "PCM: ", pcm_idx, pcm_len

                        h_out.write('\x67')
                        h_out.write('\x66')
                        h_out.write('\x00')
                        h_out.write(struct.pack('I', pcm_len))
                       
                        h_out.write(pcm_bytes)
                        
                        h_in.read(pcm_len)
                    
                    h_out.write('\x90\x00\x02\x00\x2a')
                    h_out.write('\x91\x00\x00\x01\x00')
                    h_out.write('\x92')
                    h_out.write('\x00\x40\x1f\x00\x00')

                foo = """
                if(self.pcm_lens):
                    for pcm_len, pcm_data in self.pcms:
                        h_out.write('\x67')
                        h_out.write('\x66')
                        h_out.write('\x00')
                        h_out.write(struct.pack('I', pcm_len))
                        h_out.write(pcm_data)
                        h_in.read(7)
                        h_in.read(pcm_len)
                    
                    h_out.write('\x90\x00\x02\x00\x2a')
                    h_out.write('\x91\x00\x00\x01\x00')
                """

                opcode = 0
                wait = -1
                pcm_wait = 0
                
                opcode = h_in.read(1)
                while opcode:
                #while i < dlen: 
                    #prev_opcode = opcode
                    #opcode = data[i]

                    nibble_a = hex(ord(opcode) & ord('\xF0'))
                    nibble_b = ord(opcode) & ord('\x0F')

                    if(opcode == '\x67' and not self.pcm_lens):
                        h_out.write(opcode)
                        trash = h_in.read(1)
                        h_out.write(trash)
                        tt = h_in.read(1)
                        h_out.write(tt)
                        ss = h_in.read(4)
                        h_out.write(ss)
                        sslen = struct.unpack('I', ss)[0]
                        print sslen
                        pcm_h_in = h_in.read(sslen)
                        h_out.write(pcm_h_in)

                    if(nibble_a == '0x70'):
                        #print "0x7n"
                        wait += nibble_b
                        #h_out.write(opcode)
                    elif(wait >= 15):
                        h_out.write('\x61')
                        h_out.write(struct.pack('H', wait))
                        if wait > 65535:
                            print "WAIT SIZE OVERFLOW!"
                        wait = -1
                    elif(wait < 15 and wait > 0):
                        op = pcm_wait | 0x70
                        h_out.write(struct.pack('B', op))
                        #h_out.write(opcode)
                        wait = -1

                    if(nibble_a == '0x80'):
                        #print "0x80", opcode
                        pcm_wait += nibble_b
                    elif(pcm_wait > 0):
                        #print "wait done.", opcode
                        #print "PCM WAIT:", pcm_wait
                        if(pcm_wait > 15):
                            h_out.write('\x61')
                            h_out.write(struct.pack('H', pcm_wait))
                        else:
                            op = pcm_wait | 0x70
                            h_out.write(struct.pack('B', op))
                        if pcm_wait > 65535:
                            print "WAIT SIZE OVERFLOW!"
                        pcm_wait = 0

                    
                    if(opcode == '\x4f'):
                        operand = h_in.read(1)
                        h_out.write(opcode)
                        h_out.write(operand)
                    elif(opcode == '\x50'):
                        operand = h_in.read(1)
                        h_out.write(opcode)
                        h_out.write(operand)
                    elif(opcode == '\x52' or opcode == '\x53'):
                        aa = h_in.read(1)
                        dd = h_in.read(1)
                        h_out.write(opcode)
                        h_out.write(aa)
                        h_out.write(dd)
                    elif(opcode == '\x61'):
                        h_out.write(opcode)
                        operand = h_in.read(1)
                        h_out.write(operand)
                        operand = h_in.read(1)
                        h_out.write(operand)
                    elif(opcode == '\x62'):
                        h_out.write(opcode)
                    elif(opcode == '\x66'):
                        print "END ", h_in.tell()
                        h_out.write(opcode)
                        break
                    elif(opcode == '\x67'):
                        pass
                    elif(nibble_a == '0x70'):
                        pass
                    elif(nibble_a == '0x80'):
                        pass
                    elif(opcode == '\x90'):
                        h_out.write(opcode)
                        ss = h_in.read(1)
                        tt = h_in.read(1)
                        pp = h_in.read(1)
                        cc = h_in.read(1)
                        h_out.write(ss)
                        h_out.write(tt)
                        h_out.write(pp)
                        h_out.write(cc)
        
                    elif(opcode == '\x91'):
                        h_out.write(opcode)
                        ss = h_in.read(1)
                        dd = h_in.read(1)
                        ll = h_in.read(1)
                        bb = h_in.read(1)
                        h_out.write(ss)
                        h_out.write(dd)
                        h_out.write(ll)
                        h_out.write(bb)

                    elif(opcode == '\x92'):
                        h_out.write(opcode)
                        #print "0x92", opcode
                        ss = h_in.read(1)
                        h_out.write(ss)
                        ff = h_in.read(1)
                        h_out.write(ff)
                        ff = h_in.read(1)
                        h_out.write(ff)
                        ff = h_in.read(1)
                        h_out.write(ff)
                        ff = h_in.read(1)
                        h_out.write(ff)

                    elif(opcode == '\x93'):
                        h_out.write(opcode)
                        ss = h_in.read(1)
                        h_out.write(ss)
                        aa = h_in.read(1)
                        h_out.write(aa)
                        aa = h_in.read(1)
                        h_out.write(aa)
                        aa = h_in.read(1)
                        h_out.write(aa)
                        aa = h_in.read(1)
                        h_out.write(aa)
                        mm = h_in.read(1)
                        h_out.write(mm)
                        ll = h_in.read(1)
                        h_out.write(ll)
                        ll = h_in.read(1)
                        h_out.write(ll)
                        ll = h_in.read(1)
                        h_out.write(ll)
                        ll = h_in.read(1)
                        h_out.write(ll)

                    elif(opcode == '\x94'):
                        h_out.write(opcode)
                        ss = h_in.read(1)
                        h_out.write(ss)

                    elif(opcode == '\x95'):
                        h_out.write(opcode)
                        ss = h_in.read(1)
                        bb = h_in.read(2)
                        ff = h_in.read(1)
                        h_out.write(ss)
                        h_out.write(bb)
                        h_out.write(ff)
                    #print "0x95: %s,%s" % (hex(ord(ss)), hex(ord(bb))) 

                    elif(opcode == '\xE0'):
                        #print "e0", h_in.tell()
                        addr = struct.unpack('I', h_in.read(4))[0]
                        pcm_data = self.pcm_lens.get(addr)
                        if pcm_data:

                            #h_out.write('\x92')
                            #h_out.write('\x00\x40\x1f\x00\x00')

                            pcm_idx = pcm_data[0]
                            pcm_len = pcm_data[1]

                            foo="""
                            h_out.write('\x93')
                            h_out.write(struct.pack('I', addr))
                            h_out.write('\x01')
                            h_out.write(struct.pack('I', pcm_len))
                            """
                            h_out.write('\x95')
                            h_out.write('\x00')
                            h_out.write(struct.pack('H', pcm_idx))
                            h_out.write('\x00')
                            #print "0x95", pcm_idx
                    else:
                        print "(0x%x) Unknown opcode: 0x%s" % (h_in.tell(),
                                hex(ord(opcode)))
                        break

                    opcode = h_in.read(1)
                    #i+=1
                
                footer = h_in.read()
                h_out.write(footer)
                total_size = h_out.tell()
                print "Wrote %s bytes" % total_size
                h_out.seek(4)
                h_out.write(struct.pack('I', (total_size - 4)))
                    

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Upgrade VGM file")
    parser.add_argument('infile')
    parser.add_argument('outfile')
    args = parser.parse_args()
    vu = VGM_upgrader(infile=args.infile, outfile=args.outfile)
    vu.print_header()
    vu.get_pcms()
    vu.upgrade()

