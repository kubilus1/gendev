GENDEV - Genesis development environment for Linux.

Gendev exists to setup a Linux (or presumably other Unix) system for Sega
Genesis development. Gendev is not intended to replace, but to be a companion project to the SGDK
(Sega Genesis Development Kit).


#BASIC INSTRUCTIONS

The quickest way to get going would be to simply install one of the release
packages for this project.  Both a 'txz' and 'deb' package are provided.

#USAGE

##SETUP
Set the GENDEV environment variable to the install location of the project.
Typcially this should be '/opt/gendev'

```
$ export GENDEV=/opt/gendev
```

You may, perhaps, add this into your bash profile for ease of use.

##BUILDING SGDK PROJECTS

You may enter your project directory, or perhaps SGDK sample directory and
build the projects by specifying the provided makefile.gen.

```
$ cd myaweseomeproject
$ make -f $GENDEV/sgdk/mkfiles/makefilegen clean all
```

You may wish, alternatively, to copy the makefile directly to your project.

#PROJECT STRUCTURE

* gendev - Top level where things generally start from.  Start building and
  packaging here.
* gendev/toolchain - Where the GCC toolchain build lives
* gendev/sgdk - Where the sgdk patches and setup lives
* gendev/tools - Where support tools live
* gendev/examples - Old example projects
* gendev/extras - Old helper projects

#TOOLCHAIN BUILDING

The toolchain is now setup with Travis-CI for continuous integration. This
should ease porting and allow better validation of incoming patches.

You may decide to build the full toolchain by making sure you have the texinfo
package installed, and then:

```
$ cd gendev
$ make
```
... and wait about 45 minutes or so.

The .travis-ci.yml script is a good place to look to see how the individual
build stages are executed.


#DESIGN PHILOSOPHY

Initally this project was a quick hack pulling together GCC build chain
scripts from ChillyWilly, and slapping together some hacks to get SGDK to
compile under linux.

In effect this was at least 2 projects in one:  

1. A m68k build chain geared towards Genesis/Megadrive development
2. An SGDK port to linux.

The original approaches led to encouraging users to rebuild toolchains, long
build times, and large file footprint.  Plus it was very difficult to
maintain.

I am attempting to:

1. Not necessarilly require building everything 
2. Shorten the build time
3. Hook up with continuous integration tools
4. Reduce the footprint
5. Organize better
6. Minimize changes to SGDK to make porting version easier


#NOTES:

* The 32x stuff is currently deprecated. I may create steps to build this
  toolchain again in the future.
* All testing is done on 64bit Ubuntu installations, specifically Zesty and
  Trusty.  Your mileage may vary with other distros.
* This project is still way too messy, and hopefully I have time to cleanup
  cruft bit by bit.
* I welcome patches and pull requests.
