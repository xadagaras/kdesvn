####
 #   Copyright (C) 2005-2009 by Rajko Albrecht  ral@alwins-world.de        #
 #   http://kdesvn.alwins-world.de/                                        #
 #                                                                         #
 #   This program is free software; you can redistribute it and/or modify  #
 #   it under the terms of the GNU General Public License as published by  #
 #   the Free Software Foundation; either version 2 of the License, or     #
 #   (at your option) any later version.                                   #
 #                                                                         #
 #   This program is distributed in the hope that it will be useful,       #
 #   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
 #   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
 #   GNU General Public License for more details.                          #
 #                                                                         #
 #   You should have received a copy of the GNU General Public License     #
 #   along with this program; if not, write to the                         #
 #   Free Software Foundation, Inc.,                                       #
 #   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         #
 ####

include(TestCXXAcceptsFlag)

check_cxx_accepts_flag("-fexceptions" CXX_EXCEPTION)
if(CXX_EXCEPTION)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
endif()

# check if we can use setenv
try_compile(HAS_SETENV
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/TestSetenv.cxx
        OUTPUT_VARIABLE OUTPUT
)

if(HAS_SETENV)
    message(STATUS "Checking for setenv - yes")
else()
    message(STATUS "Checking for setenv - no")
endif()