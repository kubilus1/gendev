#!/usr/bin/env python
import argparse
import json

class Tile2C(object):
    def __init__(self, infile, outfile):
        self.inputf = infile
        self.outputf = outfile

    def run(self):
        f = open(self.inputf)
        try:
            data = f.read()
        finally:
            f.close()

        tiled_data = json.loads(data)
        
        print dir(tiled_data)
        print tiled_data.viewkeys()

        print "Orientation: ", tiled_data.get("orientation")
        print "Layer Count: ", len(tiled_data.get("layers", []))
        print "Height:      ", tiled_data.get("height")
        print "Width:       ", tiled_data.get("width")
        print "Tile Height: ", tiled_data.get("tileheight")
        print "Tile Width:  ", tiled_data.get("tilewidth")
        print "Properties:  ", tiled_data.get("properties")
        print "Version:     ", tiled_data.get("version")
        print "Tile Sets:   ", tiled_data.get("tilesets")
        print "Tile Image:  ", tiled_data.get("tilesets",[{}])[0].get("image")

        tiles_name = tiled_data.get("tilesets",[{}])[0].get("name")

        layer0 = tiled_data.get("layers",[{}])[0]
        print layer0.viewkeys()
       
        tiles = layer0.get("data")
        width = layer0.get("width")
        height = layer0.get("height")

        f = open(self.outputf, "w")
        try:
            f.write("const unsigned char %s_map_data[%s] = {\n" % (tiles_name, width*height))

            for i in xrange(height):
                f.write(",".join(["%s" % hex(x) for x in tiles[width*i:width*(i+1)]]))
                f.write(",\n")

            f.write("};\n")
            f.write("const tilemap %s_map = {\n" % tiles_name)
            f.write("%s,\n" % width)
            f.write("%s,\n" % height)
            f.write("(unsigned char *) %s_map_data\n};\n" % tiles_name) 
        finally:
            f.close()

        print "LEN:", len(["%s" % hex(x) for x in tiles[width*i:width*(i+1)]])

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
            description="Turn Tiled json files into .h files for SGDK."
    )

    parser.add_argument(
            "infile",
            type=str,
            help="Input json file that should be read."
    )
    parser.add_argument(
            "outfile",
            type=str,
            help="Output file."
    )

    args = parser.parse_args()

    t = Tile2C(args.infile, args.outfile)
    t.run()
