/*
FLIR Lepton Camera API Code
Copyright (C) 2017  Kenth Johan Söderlind Åström

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//gcc -std=gnu11 -fdiagnostics-color -Wall -Wno-missing-braces Lepton.h

#pragma once


//Width by pixels.
#define Lepton_Width 80

//Height by pixels.
#define Lepton_Height 60

#define Lepton_Frame_Per_Seconds 27

#define Lepton_Nano_Seconds_Per_Frame (1000000000/27)


/*
Page 34.
Deassert /CS and idle SCK for at least 5 frame periods (>185 msec).
This step ensures a timeout of the VoSPI interface, 
which puts the Lepton in the proper state to establish (or re-establish) synchronization.
*/
#define Lepton_Synchronization_Idle_Microseconds 200000
