import os

f2 = open("docid_progid.txt", "w")
res = {}

f = open("nsfabs_part1_out/idnsfid.txt")
lines = f.readlines()
docid_filename = {l.split()[1]:l.split()[0] for l in lines}
f.close()


for subdir in os.listdir("Part1"):
    if os.path.isdir("Part1" + "/" + subdir):
        for subsubfile in os.listdir("Part1" + "/" + subdir):
            if os.path.isdir("Part1" + "/" + subdir + "/" + subsubfile):
                for file in os.listdir("Part1" + "/" + subdir + "/" + subsubfile):
                    if file.endswith('.txt'):
                        f = open("Part1" + "/" + subdir + "/" + subsubfile + "/" + file)
                        lines = f.readlines()
                        i = 0
                        line = lines[i]
                        while "NSF Program" not in line:
                            i+=1
                            line = lines[i]
                        programID = line.split()[3]
                        docName = os.path.splitext(file)[0]
                        if docName in docid_filename.keys():
                            res[int(docid_filename[docName])] = programID

f = open("nsfabs_part2_out/idnsfid.txt")
lines = f.readlines()
docid_filename = {l.split()[1]:l.split()[0] for l in lines}
f.close()


for subdir in os.listdir("Part2"):
    if os.path.isdir("Part2" + "/" + subdir):
        for subsubfile in os.listdir("Part2" + "/" + subdir):
            if os.path.isdir("Part2" + "/" + subdir + "/" + subsubfile):
                for file in os.listdir("Part2" + "/" + subdir + "/" + subsubfile):
                    if file.endswith('.txt'):
                        f = open("Part2" + "/" + subdir + "/" + subsubfile + "/" + file)
                        lines = f.readlines()
                        i = 0
                        line = lines[i]
                        while "NSF Program" not in line:
                            i+=1
                            line = lines[i]
                        programID = line.split()[3]
                        docName = os.path.splitext(file)[0]
                        if docName in docid_filename.keys():
                            res[int(docid_filename[docName])] = programID

f = open("nsfabs_part3_out/idnsfid.txt")
lines = f.readlines()
docid_filename = {l.split()[1]:l.split()[0] for l in lines}
f.close()


for subdir in os.listdir("Part3"):
    if os.path.isdir("Part3" + "/" + subdir):
        for subsubfile in os.listdir("Part3" + "/" + subdir):
            if os.path.isdir("Part3" + "/" + subdir + "/" + subsubfile):
                for file in os.listdir("Part3" + "/" + subdir + "/" + subsubfile):
                    if file.endswith('.txt'):
                        f = open("Part3" + "/" + subdir + "/" + subsubfile + "/" + file)
                        lines = f.readlines()
                        i = 0
                        line = lines[i]
                        while "NSF Program" not in line:
                            i+=1
                            line = lines[i]
                        programID = line.split()[3]
                        docName = os.path.splitext(file)[0]
                        if docName in docid_filename.keys():
                            res[int(docid_filename[docName])] = programID

import collections
od = collections.OrderedDict(sorted(res.items()))
for k, v in od.iteritems():
    f2.write(str(k) + " " + v + "\n")

print(len(res))