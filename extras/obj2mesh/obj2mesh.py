#!/bin/env python

import re
import numpy

from optparse import OptionParser

def normalized(a, axis=-1, order=2):
    l2 = numpy.atleast_1d(numpy.linalg.norm(a, order, axis))
    l2[l2==0] = 1
    return a / numpy.expand_dims(l2, axis)

def get_center(vertices):
    return [float(sum(val))/float(len(vertices)) for val in zip(*vertices)]

class polygon(object):
    def __init__(self, name, reverse=False):
        self.vertices = []
        self.faces = []
        self.translate = {}
        self.face_size = 0
        self.name = name.strip()
        self.reverse = reverse

#    def __str__(self):
    def print_points(self):
        self.num_verts = len(self.vertices)
        self.num_faces = len(self.faces)
        self.num_edges = self.face_size

        print 
        print "const Vect3D_f16 %s_coord[%s] = {" % (self.name, self.num_verts)
        for vert in self.vertices:
            print " {FIX16(%s), FIX16(%s), FIX16(%s)}," % (vert[0], vert[1], vert[2])
        print "};"
        print
        print "const u16 %s_poly_ind[%s] = {" % (self.name, self.face_size + len(self.faces))
        for face in self.faces:
            print "%s," % len(face),
            if self.reverse:
                face.reverse()
            print ",".join(str(x) for x in face),
            print ","
        print "};"
        print
        print "const u16 %s_line_ind[%s] = {" % (self.name, (self.num_edges * 2))
        for face in self.faces:
            for x in range(len(face)-1):
                print " %s, %s, " % (face[x], face[x+1])
            print " %s, %s, " % (face[x+1], face[0])
        print "};"
        print
        print "const Vect3D_f16 %s_face_norm[%s] = {" % (self.name, self.num_faces)
        for face in self.faces:
            a = numpy.array(self.vertices[face[0]])
            b = numpy.array(self.vertices[face[1]])
            c = numpy.array(self.vertices[face[len(face)-1]])
            cross = numpy.cross((b-a), (c-a))
            norm = normalized(cross)[0]
            print " {FIX16(%s), FIX16(%s), FIX16(%s)}," % (float(norm[0]), float(norm[1]), float(norm[2]))
        print "};"
        print

        center = get_center(self.vertices)

        print """polygon %s_p = {
    %s,
    %s,
    %s,
    {FIX16(%s),FIX16(%s),FIX16(%s)},
    (Vect3D_f16 *) %s_coord,
    (u16 *) %s_poly_ind,
    (u16 *) %s_line_ind,
    (Vect3D_f16 *) %s_face_norm
};
""" % (
            self.name,
            self.num_verts,
            self.num_faces,
            self.num_edges,
            center[0],
            center[1],
            center[2],
            self.name,
            self.name,
            self.name,
            self.name
        )


class Obj2Mesh(object):

    objs = []

    VERT_RE = "^v (?P<x>[\-0-9\.e\+]+) (?P<y>[\-0-9\.e\+]+) (?P<z>[\-0-9\.e\+]+)" 
    FACE_RE = "(?P<v>[0-9]+)/?(?P<vt>[0-9]*)/?(?P<vn>[0-9]*)" 
    OBJ_RE = "^o (?P<name>.*)" 

    def __init__(self, filename, reverse=False):
        self.filename = filename
        self.reverse = reverse
    
        vert_re = re.compile(self.VERT_RE)
        face_re = re.compile(self.FACE_RE)
        obj_re = re.compile(self.OBJ_RE)

        with open(filename, 'r') as objfile:

            curobj = None
            last_vertidx = 0
            vert_idx = 0

            for line in objfile.readlines():
                om = obj_re.match(line)
                if om:
                    name = om.group('name')
                    curobj = polygon(name, self.reverse)
                    self.objs.append(curobj)
                    last_vertidx = vert_idx
                    vert_idx = 0
                    continue 
                    

                vm = vert_re.match(line) 
                if vm:
                    #print m.groupdict()
                    vg = vm.groups()
                    temp_v = (float(vg[0]), float(vg[1]), float(vg[2]))
                    if temp_v in curobj.vertices:
                        # This vertice is a duplicate.
                        dupe_idx = curobj.vertices.index(temp_v)
                        curobj.translate[vert_idx] = dupe_idx
                    else:
                        # New vertice, so save it
                        curobj.vertices.append(temp_v)
                        curobj.translate[vert_idx] = curobj.vertices.index(temp_v) 
                
                    vert_idx+=1
                    continue

                if line.startswith("f "):

                    temp_face = []
                    for fm in face_re.finditer(line):
                        v = fm.group('v')
                        vt = fm.group('vt')
                        vn = fm.group('vn')
                        temp_face.append(int(v) - last_vertidx - 1)

                    if temp_face:
                        # Check to see if a vertice is a dupe, use that instead
                        temp_f = []
                        for v in temp_face:
                            temp_f.append(curobj.translate.get(v,v))

                        if temp_f not in curobj.faces:
                            curobj.faces.append(temp_f)
                            curobj.face_size += len(temp_f)
                        


        print "#include \"genesis.h\""
        print "#include \"polygon.h\""
        print "#ifndef _MESHS_H_"
        print "#define _MESHS_H_"

        #print "#define verts %s" % len(self.vertices)
        #print "#define faces %s" % len(self.faces)
        #print "#define edges %s" % (len(self.faces) * 3)
        #print "#define edges %s" % self.face_size


        for obj in self.objs:
            #print obj 
            obj.print_points()

        print "polygon polygon_list[%s] = {" % len(self.objs)
        for obj in self.objs:
            print "&%s_p," % obj.name
        print "};"

        print "u16 polygon_list_len = %s;" % len(self.objs)

        print "#endif"


if __name__ == "__main__":

    parser = OptionParser()

    parser.add_option(
            "-f",
            "--infile",
            dest="infile"
    )
    parser.add_option(
            "-r",
            "--reverse",
            action="store_true",
            dest="reverse",
            default=False
    )

    opts, args = parser.parse_args()

    o2m = Obj2Mesh(opts.infile, opts.reverse)
