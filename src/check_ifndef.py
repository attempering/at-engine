#!/usr/bin/env python3

import os, re



def dir_is_excluded(dir, excluded_dirs):
    dir_els = dir.split("/")
    for excluded_dir in excluded_dirs:
        if excluded_dir in dir_els:
            return True
    return False


def check_ifndef_file(path, file):
    s = open(path).readlines()
    
    found = False
    
    tag_expected = file.upper().replace('.', '_').replace('-', '_') + '__'

    for i, line in enumerate(s):
        if line.startswith('#ifndef'):
            m = re.search('#ifndef\s+(.*)\s+', line)
            if m:
                tag = m.group(1)
                if tag == tag_expected:
                    return False
                else:
                    print(f"tag {tag}, vs. file {file}, at {path}")
                    print(line.strip())
                    print(s[i+1].strip())
                    print(f'#ifndef {tag_expected}')
                    print(f'#define {tag_expected}')
                    s[i] = f'#ifndef {tag_expected}\n'
                    s[i+1] = f'#define {tag_expected}\n'
                found = True
            break
    
    if not found:
        return False
    
    print("Updating #ifndef/ to", path)
    input()
    open(path, "w").writelines(s)
    return True

    
def check_ifndef_dir(root, excluded_dirs):

    for dir, subdirs, files in os.walk(root):
        if dir_is_excluded(dir, excluded_dirs):
            continue

        for file in files:
            if not file.endswith(".h") and not file.endswith(".c"):
                continue

            path = os.path.join(dir, file)

            if os.path.islink(path):
                continue

            check_ifndef_file(path, file)


check_ifndef_dir(".", excluded_dirs=["_backup", "gmxmock", "zcom", "tests"])

