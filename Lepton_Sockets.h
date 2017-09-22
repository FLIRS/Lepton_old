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

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

#include "Lepton_Packets.h"
#include "Lepton_Pixels.h"
#include "Lepton_Conversions.h"

ssize_t Lepton_Sockets_Receive_Packet 
(
   int Socket, 
   struct sockaddr_in * Address, 
   socklen_t * Length, 
   struct Lepton_Packet * Packet
)
{
   ssize_t Result;
   Result = recvfrom (Socket, Packet, sizeof (struct Lepton_Packet), 0, (void *) Address, Length);
   return Result;
}


int Lepton_Sockets_Receive_Packet_Pixmap 
(
   int Socket, 
   struct Lepton_Pixel_Grayscale16 * Pixmap
)
{
   ssize_t Result;
   socklen_t Length;
   struct sockaddr_in Address;
   struct Lepton_Packet Packet [1];
   size_t I = 0;
   for (I = 0; I < Lepton_Height; I = I + 1)
   //while (I < Lepton_Height)
   {
      Result = Lepton_Sockets_Receive_Packet (Socket, &Address, &Length, Packet);
      if (Result != sizeof (struct Lepton_Packet))
      {
         break;
      }
      Lepton_Conversions_Packet_To_Grayscale16 (Packet, Pixmap);
      //printf ("%i\n", Packet->Number);
      //I = I + 1;
   }
   return I;
}

