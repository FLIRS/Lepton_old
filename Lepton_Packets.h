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


//Using:
//Lepton_Width
//Lepton_Packet_Size
#include "Lepton.h"

//Using:
//Lepton_CRC16_CCITT
#include "Lepton_CRC16_CCITT.h"

//Using:
//assert
#include <assert.h>

//Using: 
//uint8_t
//uint16_t
#include <stdint.h>

//Using: 
//size_t
#include <stddef.h>

//Using: 
//be16toh
#include <endian.h>


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
   
   //Might not be necessary.
   //This holds segment number value at packet 20.
   uint8_t Preserve_Reserved = Packet->Reserved;
   
   //Checksum is big endian. Convert byte order to host.
   uint16_t Checksum = be16toh (Packet->Checksum);
   
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
   
   //Restore Checksum and Reserved.
   Packet->Checksum = htobe16 (Checksum);
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
   //Lepton Datasheet Page 41 - Packet Header Encoding.
   return (Packet->Reserved & 0x0F) == 0x0F;
}


//Return 0 when row is not valid
//Return 1 when row is valid
int Lepton_Packet_Is_Row (struct Lepton_Packet * Packet)
{
   return (Packet->Number >= 0) && (Packet->Number < Lepton_Height);
}


//Convert the packet to host byte order.
void Lepton_Packet_To_Host (struct Lepton_Packet * Packet)
{
   for (int I = 0; I < Lepton_Width; I = I + 1)
   {
      //Everything larger than 8 bit need be converted 
      //from big endian byte order to host byte order.
      Packet->Line [I] = be16toh (Packet->Line [I]);
   }
}





//Temporary:
#include <stdio.h>

void Lepton_Packet_Print (struct Lepton_Packet * Packet)
{
   printf ("Reserved %04x\n", Packet->Reserved);
   printf ("Number   %04x\n", Packet->Number);
   printf ("Checksum %04x\n", Packet->Checksum);
   for (size_t I = 0; I < Lepton_Width; I = I + 1)
   {
      printf ("%04x ", Packet->Line [I]);
   }
}


void Lepton_Packet_Print1 (struct Lepton_Packet * Packet)
{
   uint8_t * B = (uint8_t *)Packet;
   for (size_t I = 0; I < Lepton_Packet_Size; I = I + 1)
   {
      printf ("%02x ", B [I]);
   }
}
