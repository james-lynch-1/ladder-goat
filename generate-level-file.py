# takes a csv file(s) exported by Tiled and converts to our level data format
# takes as arguments the relative path to the directory containing the csv file(s) and the name of the level

import sys, os
from pathlib import Path

def replaceField(string: str, field: str, val: int):
    start = string.find(field + " = ") + len(field) + 3
    end = start
    while string[end].isdigit():
        end += 1
    return string[:start] + str(val) + string[end:]

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
    numLayers -= 1

    if (os.path.isfile("source/levels/" + levelName + ".c")):
        with open("source/levels/" + levelName + ".c", "r") as f:
            result = [f.read().split(".clsn")[0] + ".clsn =\n    {\n"]
    else:
        result = ["#include \"levels.h\"\n\n" +\
        "const LevelEntData level" + levelNum + "EntArr[] = {\n};\n\n" +\
        "const LevelData " + levelName + " = {\n" +\
        "    .levelId = " + levelNum + ",\n" +\
        "    .bgIndex = 0,\n" +\
        "    .entArr = " + levelName + "EntArr,\n" + \
        "    .entArrLength = sizeof(" + levelName + "EntArr) / sizeof(LevelEntData),\n" +\
        "    .playerPos = { 6, 0, 6, SOUTHEAST },\n" +\
        "    .yHeight = 3,\n" +\
        "    .xWidth = 16,\n" +\
        "    .zWidth = 16,\n" +\
        "    .vp = { 0, 0, 0, INT32_MAX, 0, 0, 0, INT32_MAX, false },\n" +\
        "    .clsn =\n    {\n"]
    chunks = [""]
    y = 1
    csvList = None
    while y <= numLayers:
        filePath = path + "/" + levelName + "_Tile Layer " + str(y) + ".csv"
        with open(filePath, "r") as csv:
            csvStr = csv.read()
        csvList = csvStr.splitlines()
        i = 0
        while i < len(csvList):
            csvList[i] = csvList[i].replace("-1", "0") # empty spaces should be 0 but Tiled exports them as -1
            csvList[i] = "        " + csvList[i] + ",\n"
            i += 1
        chunks = chunks + csvList
        chunks.append("\n")
        y = y + 1
    chunks[len(chunks) - 1] = "    }\n};\n" # replace the last newline with the closing braces

    if csvList is not None:
        xWidth = csvList[0].count(",")
        zWidth = len(csvList)
        result[0] = replaceField(result[0], "xWidth", xWidth)
        result[0] = replaceField(result[0], "zWidth", zWidth)

    with open("source/levels/" + levelName + ".c", "w") as f:
        f.write("".join(result + chunks))

main()
