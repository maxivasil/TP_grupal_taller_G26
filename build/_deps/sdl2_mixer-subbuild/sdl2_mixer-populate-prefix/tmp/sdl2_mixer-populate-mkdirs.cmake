# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/machi/AA_FIUBA/Taller/TP_grupal_taller_G26/build/_deps/sdl2_mixer-src"
  "/home/machi/AA_FIUBA/Taller/TP_grupal_taller_G26/build/_deps/sdl2_mixer-build"
  "/home/machi/AA_FIUBA/Taller/TP_grupal_taller_G26/build/_deps/sdl2_mixer-subbuild/sdl2_mixer-populate-prefix"
  "/home/machi/AA_FIUBA/Taller/TP_grupal_taller_G26/build/_deps/sdl2_mixer-subbuild/sdl2_mixer-populate-prefix/tmp"
  "/home/machi/AA_FIUBA/Taller/TP_grupal_taller_G26/build/_deps/sdl2_mixer-subbuild/sdl2_mixer-populate-prefix/src/sdl2_mixer-populate-stamp"
  "/home/machi/AA_FIUBA/Taller/TP_grupal_taller_G26/build/_deps/sdl2_mixer-subbuild/sdl2_mixer-populate-prefix/src"
  "/home/machi/AA_FIUBA/Taller/TP_grupal_taller_G26/build/_deps/sdl2_mixer-subbuild/sdl2_mixer-populate-prefix/src/sdl2_mixer-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/machi/AA_FIUBA/Taller/TP_grupal_taller_G26/build/_deps/sdl2_mixer-subbuild/sdl2_mixer-populate-prefix/src/sdl2_mixer-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/machi/AA_FIUBA/Taller/TP_grupal_taller_G26/build/_deps/sdl2_mixer-subbuild/sdl2_mixer-populate-prefix/src/sdl2_mixer-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
