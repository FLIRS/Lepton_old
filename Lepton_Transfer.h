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


#define Lepton_Transfer_Status_Ok             0
#define Lepton_Transfer_Status_Discard        1
#define Lepton_Transfer_Status_Unsynced       2
#define Lepton_Transfer_Status_CRC_Mismatch_1 3
#define Lepton_Transfer_Status_CRC_Mismatch_2 4
#define Lepton_Transfer_Status_Error_ioctl    5


char const * Lepton_Transfer_Status_Text (int Lepton_Transfer_Status)
{
   switch (Lepton_Transfer_Status)
   {
      case Lepton_Transfer_Status_Ok:
      return "Lepton_Transfer_Status_Ok";
      break;
      case Lepton_Transfer_Status_Discard:
      return "Lepton_Transfer_Status_Discard";
      break;
      case Lepton_Transfer_Status_Unsynced:
      return "Lepton_Transfer_Status_Unsynced";
      break;
      case Lepton_Transfer_Status_CRC_Mismatch_1:
      return "Lepton_Transfer_Status_CRC_Mismatch_1";
      break;
      case Lepton_Transfer_Status_CRC_Mismatch_2:
      return "Lepton_Transfer_Status_CRC_Mismatch_2";
      break;
      default:
      assert (0);
      break;
   }
}

//Return 0 when not successful.
//Return 1 when successful.
//Receive <Count> number of <Data> from <Device>
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
   
   //memset might not be needed.
   memset ((void *) Data, 0, Count);
   
   int Result = ioctl (Device, SPI_IOC_MESSAGE (1), &Transfer);
   
   return (Result == Count);
}

//Return 0 when not successful.
//Return 1 when successful.
//Receive <Count> number of <Packet_Array> from <Device>
int Lepton_Transfer_Packet (struct Lepton_Packet * Packet_Array, size_t Count, int Device)
{
   int Result;
   Result = Lepton_Transfer8 ((uint8_t *) Packet_Array, sizeof (struct Lepton_Packet) * Count, Device);
   return Result;
}


//Return <Lepton_Transfer_Status_Ok> when succesful.
//Return <Lepton_Transfer_Status_Error_ioctl> when SPI error.
//Receive <Frame> from <Device>
int Lepton_Transfer_Frame (struct Lepton_Frame * Frame, int Device)
{
   if (1 != Lepton_Transfer_Packet (Frame->Packet_Array + 0, 1, Device))
   {
      assert (0);
      return Lepton_Transfer_Status_Error_ioctl;
   }
   
   if (1 == Lepton_Packet_Is_Discard (Frame->Packet_Array + 0))
   {
      return Lepton_Transfer_Status_Discard;
   }
   
   if (0 == Lepton_Packet_Array_Is_Match (Frame->Packet_Array + 0, 1))
   {
      return Lepton_Transfer_Status_CRC_Mismatch_1;
   }
   
   if (0 == Lepton_Packet_Is_First (Frame->Packet_Array + 0))
   {
      return Lepton_Transfer_Status_Unsynced;
   }
   
   if (1 != Lepton_Transfer_Packet (Frame->Packet_Array + 1, 59, Device))
   {
      return Lepton_Transfer_Status_Error_ioctl;
   }
   
   if (0 == Lepton_Packet_Array_Is_Match (Frame->Packet_Array + 1, 59))
   {
      return Lepton_Transfer_Status_CRC_Mismatch_2;
   }
   
   return Lepton_Transfer_Status_Ok;
}

