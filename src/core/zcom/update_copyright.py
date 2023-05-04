#!/usr/bin/env python3

import os, re

def dir_is_excluded(dir, excluded_dirs):
    dir_els = dir.split("/")
    for excluded_dir in excluded_dirs:
        if excluded_dir in dir_els:
            return True
    return False


copyright_template = '''/*{description}
 * Copyright (C) {year}  {author}
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

'''

def has_copyright(s):
    if re.search(r'^/\*.*?Copyright \(C\).*?\*/', s, re.DOTALL):
        return True
    return False


def build_copyright(copyright_info):
    s = copyright_template
    s = s.replace('{author}', copyright_info['author'])
    s = s.replace('{year}', copyright_info['year'])
    s = s.replace('{description}', ' ' + copyright_info['description'])
    return s



def update_copyright(path, copyright):
    s = open(path).read()
    if has_copyright(s):
        return False
    new_s = copyright + s
    print("Adding copyright notice to", path)
    input()
    open(path, "w").write(new_s)
    return True

    
def update_copyright_dir(root, copyright_info, excluded_dirs):
    
    copyright = build_copyright(copyright_info)
    
    for dir, subdirs, files in os.walk(root):
        if dir_is_excluded(dir, excluded_dirs):
            continue

        for file in files:
            if not file.endswith(".h") and not file.endswith(".c"):
                continue

            path = os.path.join(dir, file)

            if os.path.islink(path):
                continue

            update_copyright(path, copyright)

copyright_info = {
    "author": "AT-Engine Developers",
    "year": "2010-2023",
    "description": "Specialized version of the zcom library for common routines",
}


update_copyright_dir(".", copyright_info, excluded_dirs=["_backup"])

