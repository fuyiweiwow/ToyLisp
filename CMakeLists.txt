cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_C_COMPILER "D:/Tools/bin/gcc.exe")
set(CMAKE_MAKE_PROGRAM "D:/Tools/bin/mingw32-make.exe")
project(ToyLisp VERSION 0.1.0 LANGUAGES C)


add_executable  (ToyLisp 
                    main.c
                    ./mpc/mpc.c
                    parsing.c
                    evaluation.c
                    data_handling.c
                )