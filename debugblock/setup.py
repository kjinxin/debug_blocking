from distutils.core import setup, Extension
from Cython.Build import cythonize

setup(ext_modules = cythonize([

    Extension("debugblocker_cython", sources=["debugblocker_cython.pyx",
                                              "GenerateRecomLists.cpp",
                                              ],
              language="c++", libraries=["m"],
              #extra_compile_args = ["-O3", "-ffast-math", "-march=native", "-fopenmp", "-std=c++11", "-pthread"]),
              extra_compile_args = ["-std=c++11"]),
              #extra_link_args=['-fopenmp']),

 ]))
