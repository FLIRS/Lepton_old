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

//gcc -std=gnu11 -fdiagnostics-color -Wall -Wno-missing-braces Lepton_Frames.h

#pragma once

#include "Lepton.h"
#include "Lepton_Packets.h"
#include "Lepton_CRC16_CCITT.h"

#include <assert.h>

//For using: ntohs
#include <netinet/in.h>

//For using uint8_t and uint16_t
#include <stdint.h>

//Frame size is the size of a whole image plus headers.
#define Lepton_Frame_Size (Lepton_Packet_Size * Lepton_Height)


//FLIR Lepton Datasheet page 32.
//A single Lepton frame contains data from all 60 packets, which is 60 rows of data.
struct __attribute__((__packed__)) Lepton_Frame
{
   struct Lepton_Packet Packet_Array [Lepton_Height];
};
   
//Check if the packets are ordered from 0 .. 59
int Lepton_Frame_Check_Climbing_Number (struct Lepton_Frame * Frame)
{
   for (int I = 0; I < (Lepton_Height - 1); I = I + 1)
   {
      uint8_t X0 = Frame->Packet_Array [I + 0].Number;
      uint8_t X1 = Frame->Packet_Array [I + 1].Number;
      if ((X0 + 1) != X1)
      {
         return 0;
      }
   }
   return 1;
}

int Lepton_Frame_Check_Row_Valid (int Row)
{
   if (Row >= Lepton_Height)
   {
      return 0;
   }
   if (Row < 0)
   {
      return 0;
   }
   return 1;
}

