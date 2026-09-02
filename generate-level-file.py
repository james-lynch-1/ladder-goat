# takes a csv file(s) and turns into our level data format
# takes as arguments the relative path to the directory containing the csv file(s) and the name of the level

import sys, os
from pathlib import Path

def main():
    if len(sys.argv) != 3:
        print("Invalid args. Arg 1: relative path to dir containing csv file(s). Arg 2: name of the level.")
        return
    path = os.getcwd() + "/" + sys.argv[1]
    levelName = sys.argv[2]
    levelNum = levelName.rsplit("level", 1)[1]
    if not os.path.isdir(path):
        print("Invalid path supplied as arg 1.")
        return
    
    filePath = path + "/" + levelName + "_Tile Layer 1" + ".csv"
    if not os.path.isfile(filePath):
        print("File not found.")
        return
        
    numLayers = 1
    while numLayers <= 8:
        filePath = path + "/" + levelName + "_Tile Layer " + str(numLayers) + ".csv"
        if os.path.isfile(filePath):
            numLayers = numLayers + 1
        else:
            break
    numLayers = numLayers - 1
    beginStr = "#include \"levels.h\"\n\nconst LevelData " + levelName + " = {\n    " + levelNum + ", 0, " + str(numLayers) + ", {7, 0, 7}, {7, 0, 7},\n    {\n"
    mapWidth = 19
    
    with open("source/levels/" + levelName + ".c", "w") as f:
        f.write("")
    with open("source/levels/" + levelName + ".c", "a") as f:
        f.write(beginStr)
        y = 1
        while y <= numLayers:
            f.write("        {{\n")
            filePath = path + "/" + levelName + "_Tile Layer " + str(y) + ".csv"
            with open(filePath, "r") as csv:
                csvStr = csv.read()
                for z in range(mapWidth):
                    f.write("            {")
                    rowArr = csvStr.split("\n")[z].split(",")
                    for x in rowArr:
                        f.write(x + ",")
                    f.seek(f.seek(0, os.SEEK_END) - 1)
                    f.truncate()
                    f.write("},\n")
                f.seek(f.seek(0, os.SEEK_END) - 2)
                f.truncate()
                f.write("\n")
            f.write("        }},\n")
            y = y + 1
        f.write("    }\n};\n")

main()
