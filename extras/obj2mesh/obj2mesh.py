#!/bin/env python

import re
import os
import numpy

from optparse import OptionParser

DEBUG=False

def normalized(a, axis=-1, order=2):
    l2 = numpy.atleast_1d(numpy.linalg.norm(a, order, axis))
    l2[l2==0] = 1
    return a / numpy.expand_dims(l2, axis)

def get_center(vertices):
    return [float(sum(val))/float(len(vertices)) for val in zip(*vertices)]

class polymesh(object):
    def __init__(self, name, reverse=False):
        self.vertices = []
        self.faces = []
        self.translate = {}
        self.face_size = 0
        self.name = name.strip()
        self.reverse = reverse
        self.facecolor = []

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


        print "const u16 %s_face_color[%s] = {" % (self.name, self.num_faces)
        for fc in self.facecolor:
            print "%s," % fc
        print "};"

        center = get_center(self.vertices)

        print """polymesh %s_p = {
    %s,
    %s,
    %s,
    {FIX16(%s),FIX16(%s),FIX16(%s)},
    (Vect3D_f16 *) %s_coord,
    (u16 *) %s_poly_ind,
    (u16 *) %s_line_ind,
    (Vect3D_f16 *) %s_face_norm,
    {FIX16(0)},
    (u16 *) %s_face_color
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
            self.name,
            self.name
        )


class Obj2Mesh(object):

    objs = []
    colors = {}

    VERT_RE = "^v (?P<x>[\-0-9\.e\+]+) (?P<y>[\-0-9\.e\+]+) (?P<z>[\-0-9\.e\+]+)" 
    FACE_RE = "(?P<v>[0-9]+)/?(?P<vt>[0-9]*)/?(?P<vn>[0-9]*)" 
    OBJ_RE = "^o (?P<name>.*)" 
    USEMAT_RE = "^usemtl (?P<name>.*)"

    NEWMAT_RE = "^newmtl (?P<name>.*)"
    COLOR_RE = "^Kd (?P<r>[0-1]\.[0-9]+) (?P<g>[0-1]\.[0-9]+) (?P<b>[0-1]\.[0-9]+)"

    def __init__(self, file_list, reverse=False):
        self.reverse = reverse    

        # First parse material files
        for filename in file_list:
            material = "%s.mtl" % (os.path.splitext(filename)[0])
            if os.path.isfile(material):
                self.read_materials(material)
        if DEBUG:         
            print "COLORS:", self.colors


        for filename in file_list:
            self.parse_obj(filename)

        print "#include \"genesis.h\""
        print "#include \"polymesh.h\""
        print "#ifndef _MESHS_H_"
        print "#define _MESHS_H_"

        #print "#define verts %s" % len(self.vertices)
        #print "#define faces %s" % len(self.faces)
        #print "#define edges %s" % (len(self.faces) * 3)
        #print "#define edges %s" % self.face_size


        for obj in self.objs:
            #print obj 
            obj.print_points()

        # Sort palette by the index
        print "const u16 polymesh_palette[%s] = {" % len(self.colors)
        for k,v in sorted(self.colors.items(), key=lambda x: x[1][1]):
            print "%s," % v[0]
        print "};"

        print "u8 polymesh_pal_len = %s;" % len(self.colors)

        print "polymesh* polymesh_list[%s] = {" % len(self.objs)
        for obj in self.objs:
            print "&%s_p," % obj.name
        print "};"

        print "u16 polymesh_list_len = %s;" % len(self.objs)

        print "#endif"


    
    def parse_obj(self, filename):

        vert_re = re.compile(self.VERT_RE)
        face_re = re.compile(self.FACE_RE)
        obj_re = re.compile(self.OBJ_RE)
        usemat_re = re.compile(self.USEMAT_RE)


        with open(filename, 'r') as objfile:

            curobj = None
            last_vertidx = 0
            vert_idx = 0
            curcolor = None

            for line in objfile.readlines():
                om = obj_re.match(line)
                if om:
                    name = om.group('name')
                    curobj = polymesh(name, self.reverse)
                    self.objs.append(curobj)
                    last_vertidx = vert_idx + last_vertidx
                    vert_idx = 0
                    curcolor = None
                    continue 
                    

                vm = vert_re.match(line) 
                if vm:
                    if curobj is None:
                        name = "object"
                        curobj = polymesh(name, self.reverse)
                        self.objs.append(curobj)
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

                um = usemat_re.match(line)
                if um:
                    if self.colors:
                        colname = um.group('name')
                        curcolor = self.colors.get(colname)

                if line.startswith("f "):
                    temp_face = []
                    for fm in face_re.finditer(line):
                        v = fm.group('v')
                        vt = fm.group('vt')
                        vn = fm.group('vn')
                        #print "Vert: %s, LastVIdx: %s, Vert_idx: %s" % (v,
                        #        last_vertidx, vert_idx)
                        temp_face.append(int(v) - last_vertidx - 1)

                    if temp_face:
                        # Check to see if a vertice is a dupe, use that instead
                        temp_f = []
                        for v in temp_face:
                            temp_f.append(curobj.translate.get(v,v))

                        if temp_f not in curobj.faces:
                            curobj.faces.append(temp_f)
                            if curcolor:
                                curobj.facecolor.append(curcolor[1])
                            curobj.face_size += len(temp_f)
                        



    def read_materials(self, material_file):

        newmat_re = re.compile(self.NEWMAT_RE)
        color_re = re.compile(self.COLOR_RE)

        with open(material_file, 'r') as mat_file:
            curcolor = None
            for line in mat_file.readlines():
                mm = newmat_re.match(line)
                if mm:
                    curcolor = mm.group('name')

                cm = color_re.match(line)
                if cm:
                    if not self.colors.has_key(curcolor):
                        self.colors[curcolor] = (self.to_rgb9(cm.group('r'), cm.group('g'), cm.group('b')), len(self.colors))
                        if DEBUG:
                            print "ADDING COLOR:", curcolor
               

    def to_rgb9(self, r, g, b):
        return "0x{:04x}".format( 
            (int(float(b)*7.0)*512) +
            (int(float(g)*7.0)*32) +
            (int(float(r)*7.0)*2)
        )


if __name__ == "__main__":

    parser = OptionParser()

    parser.add_option(
            "-f",
            "--infile",
            dest="infile",
            action="append"
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
