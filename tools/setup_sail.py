#!/usr/bin/env python3

import os
import shutil
import subprocess
import pathlib
from contextlib import suppress

build_dir = pathlib.Path(os.getcwd()) / 'sail'
root = pathlib.Path(__file__).parent.parent

def compile_sail():
    sail_dir = str(root / 'lib/hakkun/hakkun/sail')
    subprocess.run(['cmake', sail_dir], cwd=build_dir)
    subprocess.run(["cmake", "--build", "."], cwd=build_dir)
    os.rename(build_dir / 'sail', root / 'tools/sail')

def main():
    shutil.rmtree(build_dir, ignore_errors=True)

    os.makedirs(build_dir)
    os.chdir(build_dir)
    compile_sail()


if __name__ == '__main__':
    main()
