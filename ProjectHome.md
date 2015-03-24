Gendev is not intended to replace, but to be a companion project to the SGDK (Sega Genesis Development Kit).

Gendev exists to setup a Linux (or presumably other Unix) system for Sega Genesis development.

## BASIC INSTRUCTIONS: ##
```
    $ svn checkout http://gendev.googlecode.com/svn/trunk/ gendev-read-only 
    $ cd gendev-read-only 
    $ make 
```
Woo hoo, tools are all setup. Now let's build sgdk ...
```
    $ . ~/.gendev 
    $ cd gendev-read-only/sgdk 
    $ make install 
```
Now I want to create a project ...
```
    $ cp gendev-read-only/sgdk/skeleton mycoolproject 
    $ cd mycoolproject 
```
**type type type**
```
    $ make 
```


---


## Troubleshooting and FAQ ##
### makeinfo is missing ###
Try installing the 'texinfo' package.  On Ubuntu this would be 'sudo apt-get install texinfo'
### How do I build this on 