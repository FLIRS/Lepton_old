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

//gcc -std=gnu11 -fdiagnostics-color -Wall -Wno-missing-braces Lepton_Packets.h

#pragma once

#include "Lepton.h"
#include "Lepton_CRC16_CCITT.h"

#include <assert.h>

//For using: uint8_t, uint16_t
#include <stdint.h>

//For using: size_t
#include <stddef.h>

//For using: ntohs
#include <netinet/in.h>

//Each packet is the same size.
#define Lepton_Packet_Size 164

//Payload size it the size of the data in each packets.
#define Lepton_Packet_Payload_Size 160


//  FLIR Lepton Datasheet Page 31.
//  16 bits                        |  16 bits       | 160 Bytes
//  ID Field                       |  CRC Field     | Payload Field
//  -------------------------------|----------------|------------------
//  4 bits      | 12 bits          |  16 bits       | 160 Bytes
//  ID_Reserved | ID_Packet_Number |  CRC           | Payload
struct __attribute__((__packed__)) Lepton_Packet
{
   //The ID field is located at byte 0 .. 1.
   //The ID field is structured as following: (XXXX NNNN NNNN NNNN) 
   //where N is a packet number bit and X is reserved/secret.
   //The maximum packet number is 59 so the packet number can be 
   //extracted from (xxxx 0000 NNNN NNNN) i.e. second byte.
   //Lepton FLIR is big endian and Raspberry PI is little endian by 
   //defualt.
   uint8_t Reserved;
   uint8_t Number;
   
   //The CRC field is lcoated at bytes 2 .. 3.
   //The checksum is stored in network byte order.
   uint16_t Checksum;

   //The payload field is lcoated at bytes 4 .. 164.
   union
   {
      uint8_t Payload [Lepton_Packet_Payload_Size];
      uint16_t Line [Lepton_Width];
   };
};


//Return 0 when CRC mismatch
//Return 1 when CRC match
int Lepton_Packet_Is_Match (struct Lepton_Packet * Packet)
{
   int Success;
   uint8_t Preserve_Reserved = Packet->Reserved;
   //Checksum is larger than 16 bit so convert it from 
   //network byte order to host byte order.
   uint16_t Checksum = ntohs (Packet->Checksum);
   
   //FLIR Lepton Datasheet Page 31.
   //The four most-significant bits of the ID are set to zero 
   //for calculation of the CRC.
   Packet->Reserved = Packet->Reserved & 0x0F;
   
   //FLIR Lepton Datasheet Page 31.
   //All bytes of the CRC are set to zero for calculation the CRC.
   Packet->Checksum = 0;
   
   //FLIR Lepton Datasheet Page 31.
   //CRC16_CCITT: x^16 + x^12 + x^5 + x^0
   //Undocumented: CRC Seed equal zero.
   //Checksum > 0 might not be useful here.
   Success = (Checksum > 0 && Checksum == Lepton_CRC16_CCITT ((uint8_t *) Packet, sizeof (struct Lepton_Packet), 0, 0));
   
   //Checksum is matching.
   //Restore the checksum.
   Packet->Checksum = htons (Checksum);
   Packet->Reserved = Preserve_Reserved;

   return Success;
}


//Return 0 when not first packet
//Return 1 when first packet
int Lepton_Packet_Is_First (struct Lepton_Packet * Packet)
{
   //This checks if the packet is the first packet of a frame.
   return Packet->Number == 0;
}


//Return 0 when non discard
//Return 1 when discard
int Lepton_Packet_Is_Discard (struct Lepton_Packet * Packet)
{
   //This checks if the packet is a discard packet.
   return (Packet->Reserved & 0x0F) == 0x0F;
}


//Return 0 when no row is not valid
//Return 1 when row is valid
int Lepton_Packet_Is_Row_Valid (struct Lepton_Packet * Packet)
{
   return (Packet->Number >= 0) && (Packet->Number < Lepton_Height);
}


//Return 0 when CRC mismatch
//Return 1 when CRC match
int Lepton_Packet_Array_Is_Match (struct Lepton_Packet * Packet, size_t Count)
{
   size_t N = 0;
   for (int I = 0; I < Count; I = I + 1)
   {
      if (Lepton_Packet_Is_Match (Packet + I))
      {
         N = N + 1;
      }
   }
   return N;
}


//Return 0 when no row is not valid
//Return 1 when row is valid
int Lepton_Packet_Array_Is_Row_Valid (struct Lepton_Packet * Packet, size_t Count)
{
   for (int I = 0; I < Count; I = I + 1)
   {
      if (0 == Lepton_Packet_Is_Row_Valid (Packet + I))
      {
         return 0;
      }
   }
   return 1;
}


void Lepton_Packet_Net_To_Host (struct Lepton_Packet * Packet)
{
   for (int I = 0; I < Lepton_Width; I = I + 1)
   {
      //Everything larger than 8 bit need be converted from network byte order to host byte order.
      Packet->Line [I] = ntohs (Packet->Line [I]);
   }
}


//Check if the packets are ordered from 0 .. 59
int Lepton_Packet_Array_Check_Climbing_Number 
(
   struct Lepton_Packet * Packet_Array, 
   size_t Count
)
{
   for (int I = 0; I < (Count - 1); I = I + 1)
   {
      uint8_t X0 = Packet_Array [I + 0].Number;
      uint8_t X1 = Packet_Array [I + 1].Number;
      if ((X0 + 1) != X1)
      {
         return 0;
      }
   }
   return 1;
}
