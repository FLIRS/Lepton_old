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

#pragma once

#include "Lepton.h"
#include "Lepton_Frames.h"
#include "Lepton_Pixels.h"
#include "Lepton_Map.h"

//memcpy
#include <string.h>

//assert
#include <assert.h>

//uint8_t
#include <stdint.h>





//Copy all frame video lines to raw grayscale 16bit depth image.
void Lepton_Conversions_Frame_To_Grayscale16 
(
   struct Lepton_Frame * Frame, 
   struct Lepton_Pixel_Grayscale16 * Pixmap
)
{
   for (size_t Row = 0; Row < Lepton_Height; Row = Row + 1)
   {
      for (size_t Column = 0; Column < Lepton_Width; Column = Column + 1)
      {
         uint16_t Pixel;
         size_t Index;
         //Everything larger than 8 bit need be converted from network byte order to host byte order.
         Pixel = ntohs (Frame->Packet_Array [Row].Line [Column]);
         Index = Lepton_Map_2D (Column, Row, Lepton_Width, Lepton_Height);
         Pixmap [Index].Grayscale = Pixel;
      }
   }
}

//Copy packet video line to pixmap.
void Lepton_Conversions_Packet_To_Grayscale16 
(
   struct Lepton_Packet * Packet, 
   struct Lepton_Pixel_Grayscale16 * Pixmap
)
{
   uint8_t Row;
   Row = Packet->Number;
   assert (Lepton_Frame_Check_Row_Valid (Row));
   Lepton_Packet_Net_To_Host (Packet);
   memcpy (Pixmap + (Lepton_Width * Row), Packet->Payload, Lepton_Packet_Payload_Size);
}





