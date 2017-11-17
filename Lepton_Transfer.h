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

#include <assert.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <string.h>

#include "Lepton_Packets.h"
#include "Lepton_Frames.h"
#include "Lepton_SPI.h"


//Return 0 when not successful.
//Return 1 when successful.
//Receive <Count> number of <Data> from SPI <Device>
int Lepton_Transfer8 (uint8_t * Data, size_t Count, int Device)
{
   struct spi_ioc_transfer Transfer =
   {
      .tx_buf = (unsigned long) NULL,
      .rx_buf = (unsigned long) Data,
      .len = Count,
      .delay_usecs = 0,
      .speed_hz = Lepton_SPI_Speed_Recomended,
      .bits_per_word = Lepton_SPI_Bits_Per_Word
   };
   
   //memset might not be useful here.
   memset ((void *) Data, 0, Count);
   
   int Result = ioctl (Device, SPI_IOC_MESSAGE (1), &Transfer);
   
   return (Result == Count);
}

//Return 0 when not successful.
//Return 1 when successful.
//Receive <Count> number of <Packet_Array> from SPI <Device>
int Lepton_Transfer_Packet (struct Lepton_Packet * Packet_Array, size_t Count, int Device)
{
   int Result;
   Result = Lepton_Transfer8 ((uint8_t *) Packet_Array, sizeof (struct Lepton_Packet) * Count, Device);
   return Result;
}


//Return 1 when succesful.
//Return 0 when error.
//Receive a <Frame> from SPI <Device>
int Lepton_Transfer_Packet_Array 
(
   struct Lepton_Packet * Packet_Array, 
   size_t Count, 
   int Device
)
{
   assert (Count == Lepton_Height);
   
   //Check if the first packet is valid.
   if 
   (
      0 == Lepton_Transfer_Packet           (Packet_Array + 0, 1, Device)  ||
      0 == Lepton_Packet_Is_First           (Packet_Array + 0)             ||
      0 == Lepton_Packet_Array_Is_Row_Valid (Packet_Array + 0, 1)          ||
      0 == Lepton_Packet_Array_Is_Match     (Packet_Array + 0, 1)          ||
      0
   )
   {return 0;}
   
   //If the first packet were valid then receive rest of them.
   if 
   (
      0 == Lepton_Transfer_Packet           (Packet_Array + 1, Count - 1, Device) ||
      0 == Lepton_Packet_Array_Is_Row_Valid (Packet_Array + 1, Count - 1)         ||
      0 == Lepton_Packet_Array_Is_Match     (Packet_Array + 1, Count - 1)         ||
      0
   )
   {return 0;}
   
   return 1;
}



int Lepton_Transfer_Packet_Array1 
(
   struct Lepton_Packet * Packet_Array, 
   size_t Count, 
   int Device
)
{
   assert (Count == Lepton_Height);
   Lepton_Transfer_Packet (Packet_Array, 1, Device);
   if (Packet_Array [0].Reserved & 0x0F) {return 0;}
   if (Packet_Array [0].Number != 0) {return 0;}
   Lepton_Transfer_Packet (Packet_Array + 1, Count - 1, Device);
   return 1;
}




void Lepton_Transfer_Init 
(
   struct spi_ioc_transfer * Transfer,
   struct Lepton_Packet * Data,
   size_t Count,
   size_t Speed
)
{
   memset (Transfer, 0, sizeof (struct spi_ioc_transfer));
   Transfer->tx_buf        = (unsigned long) NULL;
   Transfer->rx_buf        = (unsigned long) Data;
   Transfer->len           = sizeof (struct Lepton_Packet) * Count;
   Transfer->delay_usecs   = 0;
   Transfer->speed_hz      = Speed;
   Transfer->bits_per_word = Lepton_SPI_Bits_Per_Word;
   Transfer->cs_change     = 0;
   Transfer->pad           = 0;
}
