#!/usr/bin/python

import os
import subprocess

if not os.path.isdir("build"):
    os.mkdir("build")

os.chdir("build")

subprocess.call("cmake .. && make", shell=True)
