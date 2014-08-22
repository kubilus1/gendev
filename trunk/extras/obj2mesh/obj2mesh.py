#!/bin/env python

import re
import numpy

from optparse import OptionParser

def normalized(a, axis=-1, order=2):
    l2 = numpy.atleast_1d(numpy.linalg.norm(a, order, axis))
    l2[l2==0] = 1
    return a / numpy.expand_dims(l2, axis)


class Obj2Mesh(object):

    vertices = []
    faces = []
    translate = {}


    VERT_RE = "^v (?P<x>[\-0-9\.]+) (?P<y>[\-0-9\.]+) (?P<z>[\-0-9\.]+)" 
    FACE_RE = "^f (?P<v0>[0-9]+) (?P<v1>[0-9]+) (?P<v2>[0-9]+)" 


    def __init__(self, filename):
        self.filename = filename
    
        vert_re = re.compile(self.VERT_RE)
        face_re = re.compile(self.FACE_RE)

        with open(filename, 'r') as objfile:

            vert_idx = 0

            for line in objfile.readlines():
                vm = vert_re.match(line) 
                fm = face_re.match(line)
                if vm:
                    #print m.groupdict()
                    vg = vm.groups()
                    temp_v = (float(vg[0]), float(vg[1]), float(vg[2]))
                    if temp_v in self.vertices:
                        # This vertice is a duplicate.
                        dupe_idx = self.vertices.index(temp_v)
                        self.translate[vert_idx] = dupe_idx
                    else:
                        # New vertice, so save it
                        self.vertices.append(temp_v)
                        self.translate[vert_idx] = self.vertices.index(temp_v) 
                
                    vert_idx+=1

                if fm:
                    fd = fm.groups()
                    # Check to see if a vertice is a dupe, use that instead
                    a = int(fd[0]) - 1
                    b = int(fd[1]) - 1
                    c = int(fd[2]) - 1

                    temp_f = (
                        self.translate.get(a,a),
                        self.translate.get(b,b),
                        self.translate.get(c,c)
                    )
                    if temp_v not in self.faces:
                        # Only note unique faces
                        self.faces.append(temp_f)

        print "#include \"genesis.h\""
        print "#ifndef _MESHS_H_"
        print "#define _MESHS_H_"
        print "#define verts %s" % len(self.vertices)
        print "#define faces %s" % len(self.faces)
        print "#define edges %s" % (len(self.faces) * 3)
        print 
        print "const Vect3D_f16 cube_coord[verts] = {"
        for vert in self.vertices:
            print " {FIX16(%s), FIX16(%s), FIX16(%s)}," % (vert[0], vert[1], vert[2])
        print "};"
        print
        print "const u16 cube_poly_ind[faces * 3] = {"
        for face in self.faces:
            print " %s, %s, %s," % (face[0], face[1], face[2])
        print "};"
        print
        print "const u16 cube_line_ind[edges * 2] = {"
        for face in self.faces:
            print " %s, %s, " % (face[0], face[1])
            print " %s, %s, " % (face[1], face[2])
            print " %s, %s, " % (face[2], face[0])
        print "};"
        print
        print "const Vect3D_f16 cube_face_norm[faces] = {"
        for face in self.faces:
            a = numpy.array(self.vertices[face[0]])
            b = numpy.array(self.vertices[face[1]])
            c = numpy.array(self.vertices[face[2]])
            cross = numpy.cross((b-a), (c-a))
            norm = normalized(cross)[0]
            print " {FIX16(%s), FIX16(%s), FIX16(%s)}," % (int(norm[0]), int(norm[1]), int(norm[2]))
        print "};"
        print
        print "#endif"


if __name__ == "__main__":

    parser = OptionParser()

    parser.add_option(
            "-f",
            "--infile",
            dest="infile"
    )

    opts, args = parser.parse_args()

    o2m = Obj2Mesh(opts.infile)
