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

// Using:
// Lepton_Width
// Lepton_Height
#include "Lepton.h"

// Using:
// struct Lepton_Packet
// Lepton_Packet_Is_Row
#include "Lepton_Packets.h"

// Using:
// struct Lepton_Pixel_Grayscale16
#include "Lepton_Pixels.h"

// Using:
// memcpy
#include <string.h>

// Using:
// assert
#include <assert.h>

// Using:
// uint8_t
#include <stdint.h>


//Copy and convert the <Packet> video line to a portion of the <Pixmap> 
//depending on which row <Packet> are.
void Lepton_Conversions_Packet_To_Grayscale16 
(
   struct Lepton_Packet * Packet, 
   struct Lepton_Pixel_Grayscale16 * Pixmap,
   uint8_t Segment_Number
)
{
   //The row must be valid because it is used to 
   //position video line in pixmap.
   assert (Lepton_Packet_Is_Row (Packet));
   
   //Segment_Number must be inside 0 .. 3
   assert (0 <= Segment_Number && Segment_Number <= 3);
   
   //Pixels values are 16b big endian.
   //Convert pixel values to host byte order.
   Lepton_Packet_To_Host (Packet);
   
   //Assign video line to correct position in the pixmap.
   memcpy 
   (
      Pixmap + (Segment_Number * Lepton_Height * Lepton_Width + Lepton_Width * Packet->Number), 
      Packet->Payload, 
      Lepton_Packet_Payload_Size
   );
}
