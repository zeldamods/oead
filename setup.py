import setuptools
import versioneer

import os
import re
import sys
import platform
import subprocess
from pathlib import Path

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

# Intended to make building in manylinux images easier.
# CentOS (or the EPEL package?) calls CMake cmake3...
cmake3_path = Path("/usr/bin/cmake3")
cmake_name = "cmake3" if cmake3_path.exists() else "cmake"

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[str(p) for p in Path(sourcedir).glob('**/*')])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        debug = bool(os.getenv('DEBUG', False))

        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable]

        cfg = 'Debug' if debug else 'Release'
        build_args = [f'-j{os.cpu_count()}', '--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call([cmake_name, ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call([cmake_name, '--build', '.'] + build_args, cwd=self.build_temp)


with open("readme.rst", "r") as fh:
    long_description = fh.read()

cmdclass = versioneer.get_cmdclass()
cmdclass["build_ext"] = CMakeBuild

setuptools.setup(
    name="oead",
    version=versioneer.get_version(),
    cmdclass=cmdclass,
    author="leoetlino",
    author_email="leo@leolam.fr",
    description="Library for recent Nintendo EAD formats in first-party games",
    long_description=long_description,
    long_description_content_type="text/x-rst",
    url="https://github.com/zeldamods/oead",
    packages=setuptools.find_packages(),
    classifiers=[
        "License :: OSI Approved :: GNU General Public License v2 or later (GPLv2+)",
        "Topic :: Software Development :: Libraries",
        "Operating System :: OS Independent",
        "Programming Language :: C++",
        "Programming Language :: Python :: 3 :: Only",
    ],
    python_requires=">=3.6",
    ext_modules=[CMakeExtension(name="oead", sourcedir="py")],
    data_files=[('data', [str(p) for p in Path('data').glob('**/*')])],
    zip_safe=False,
)
