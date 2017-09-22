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
#include "Lepton_Pixels.h"
#include "Lepton_Map.h"

//memcpy
#include <string.h>

//assert
#include <assert.h>

//uint8_t
#include <stdint.h>


//Copy and convert the <Packet> video line to a portion of the <Pixmap> 
//depending on which row <Packet> are.
void Lepton_Conversions_Packet_To_Grayscale16 
(
   struct Lepton_Packet * Packet, 
   struct Lepton_Pixel_Grayscale16 * Pixmap
)
{
   //The row must be valid because it is used to 
   //position video line in pixmap.
   assert (Lepton_Packet_Is_Row_Valid (Packet));
   
   //Packet data is by default network order.
   //This might change in the future.
   Lepton_Packet_Net_To_Host (Packet);
   
   //Assign video line to correct position in the pixmap.
   memcpy 
   (
      Pixmap + (Lepton_Width * Packet->Number), 
      Packet->Payload, 
      Lepton_Packet_Payload_Size
   );
}
