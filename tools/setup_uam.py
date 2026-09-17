#!/usr/bin/env python3

import os
import shutil
import subprocess
import pathlib
from contextlib import suppress

root = pathlib.Path(__file__).parent.parent
uam_dir = root / 'lib/uam-nvn'
build_dir = uam_dir / 'build'


def compile_uam():
    subprocess.run(['meson', 'build'], cwd=uam_dir)
    subprocess.run(["ninja"], cwd=build_dir)
    os.rename(build_dir / 'uam', root / 'tools/uam')

def main():
    shutil.rmtree(build_dir, ignore_errors=True)

    os.makedirs(build_dir)
    os.chdir(build_dir)
    compile_uam()


if __name__ == '__main__':
    main()
