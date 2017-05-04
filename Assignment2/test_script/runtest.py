#!/usr/bin/env python
##########################################
## Kevin Turkington
## 3/5/17
## CS344
## py assignment
##########################################
import string
import random
import os

#dir is not keyword
def makedir(dirname):
    try:
        os.makedirs(dirname)
    except OSError:
        pass
    # let exception propagate if we just can't
    # cd into the specified directory
    os.chdir(dirname)

# NAME: GenRandomletters
# DESC: generates and returns a random lowercase string.
def GenRandomletters(len):
    RandoString = random.choice(string.ascii_lowercase)
    for i in range(len):
        RandoString += (random.choice(string.ascii_lowercase)) #gen random char and append.
    return RandoString

#####Begin Program#####
makedir('testdir')

for i in range(random.randint(10,50)):#write file
    WritePtr = open('test'+str(i)+'.txt','w+')
    # for additional newline replace with the following
    # WritePtr.write(GenRandomletters() + '\n')
    WritePtr.write(GenRandomletters(random.randint(100,10000)))
    WritePtr.close()

#####End Program#######