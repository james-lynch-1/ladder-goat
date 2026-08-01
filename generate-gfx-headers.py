import os
from pathlib import Path
import re

with open("include/graphics/maps.h", "w") as f:
    f.write("")
with open("include/graphics/sprites.h", "w") as f:
    f.write("")
line = "NA"
def generateHeaders(srcPath, incPath):
    if not os.path.isdir(srcPath):
        os.makedirs(srcPath)
    for filename in os.listdir(srcPath):
        with open(srcPath/filename, "r") as source:
            for line in source:
                if line[0:5] == "const":
                    line = "extern " + line
                    line = line[0:line.find("]")]
                    line = line + "];"
                    with open(incPath, "a") as f:
                        f.write("#define " + line.split(' ')[4].rsplit('[')[0] + 'Len ')
                        num = int(line.rsplit('[')[1].rsplit(']')[0]) * 2
                        # numArr = re.findall("[*0-9]", line)
                        # numArr = int(int(''.join(numArr)) * 2)
                        for char in str(num):
                            f.write(char)
                        f.write("\n")
                        f.write(line + '\n')

generateHeaders(Path(os.getcwd())/'source'/'graphics'/'maps', "include/graphics/maps.h")
generateHeaders(Path(os.getcwd())/'source'/'graphics'/'sprites', "include/graphics/sprites.h")
