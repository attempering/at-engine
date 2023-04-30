#!/usr/bin/env python3

import os, re

def prepend_at(root, prefix):
    for dir, subdirs, files in os.walk(root):
        for file in files:
            if not file.endswith(".h"):
                continue
            old_path = os.path.join(dir, file)

            if not file.startswith(prefix):
                new_path = os.path.join(dir, prefix + file)
            else:
                new_path = old_path

            if old_path != new_path:
                os.rename(old_path, new_path)
                print(old_path, new_path)


#prepend_at(".", "at_")



def modify_include_line(line, prefix, prefix_exclusions):
    m = re.search(r'#include\s*"(.*)"', line)
    if m:
        path = m.group(1)
        x = path.split("/")

        for prefix_ex in prefix_exclusions:
            if x[-1].startswith(prefix_ex):
                return line

        if not x[-1].startswith(prefix):
            x[-1] = prefix + x[-1]
            
        new_path = '/'.join(x)
        new_line = '#include "%s"\n' % new_path
        return new_line

    return line



def modify_includes_file(path, prefix, prefix_exclusions):
    lines = open(path).readlines()
    
    modified = False
    
    for i, line in enumerate(lines):
        if line.startswith("#include"):
            new_line = modify_include_line(line, prefix, prefix_exclusions)
            if new_line != line:
                print(path, "\n", line, new_line)
                #input()
                modified = True
                lines[i] = new_line
            
    if modified:
        print("modified", path)
        input()
        open(path, "w").writelines(lines)



def modify_includes(root, prefix, prefix_exclusions):
    for dir, subdirs, files in os.walk(root):
        for file in files:
            if not file.endswith(".h"):
                continue

            modify_includes_file(os.path.join(dir, file), prefix, prefix_exclusions)

modify_includes(".", "at_", ["zcom"])
