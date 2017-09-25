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
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "Lepton_Packets.h"
#include "Lepton_Frames.h"
#include "Lepton_SPI.h"
#include "Lepton_Pixels.h"
#include "Lepton_Transfer.h"


struct Lepton_SPI_80x60
{
   struct Lepton_Packet              Packet_Array [Lepton_Height];
   struct spi_ioc_transfer           Transfer [2];
   uint64_t                          Received_Count;
   uint64_t                          Unreceived_Count;
   uint64_t                          Transaction_Count;
};


void Lepton_SPI_80x60_Construct (struct Lepton_SPI_80x60 * Item)
{
   Item->Received_Count    = 0;
   Item->Unreceived_Count  = 0;
   Item->Transaction_Count = 0;
   Lepton_Transfer_Init 
   (
      Item->Transfer + 0, 
      Item->Packet_Array + 0, 
      1, 
      Lepton_SPI_Speed_Recomended
   );
   Lepton_Transfer_Init 
   (
      Item->Transfer + 1, 
      Item->Packet_Array + 1, 
      Lepton_Height - 1, 
      Lepton_SPI_Speed_Recomended
   );
}


int Lepton_SPI_80x60_Transfer (struct Lepton_SPI_80x60 * Item, int Device)
{
   //memset might not be useful here.
   memset ((void *) Item->Packet_Array, 0, sizeof (struct Lepton_Packet) * Lepton_Height);
   
   Item->Transaction_Count = Item->Transaction_Count + 1;
   
   //Check if the first packet is valid.
   Lepton_SPI_Transfer (Device, Item->Transfer + 0);
   if 
   (
      0 == Lepton_Packet_Is_First           (Item->Packet_Array + 0)        ||
      0 == Lepton_Packet_Array_Is_Row_Valid (Item->Packet_Array + 0, 1)     ||
      0 == Lepton_Packet_Array_Is_Match     (Item->Packet_Array + 0, 1)
   )
   {
      Item->Unreceived_Count = Item->Unreceived_Count + 1;
      return 0;
   }
   
   //If the first packet were valid then receive rest of them.
   Lepton_SPI_Transfer (Device, Item->Transfer + 1);
   if 
   (
      0 == Lepton_Packet_Array_Is_Row_Valid (Item->Packet_Array + 1, Lepton_Height - 1) ||
      0 == Lepton_Packet_Array_Is_Match     (Item->Packet_Array + 1, Lepton_Height - 1)
   )
   {
      Item->Unreceived_Count = Item->Unreceived_Count + 1;
      return 0;
   }
   
   Item->Received_Count = Item->Received_Count + 1;
   
   return 1;
}


void Lepton_SPI_80x60_Printf (struct Lepton_SPI_80x60 * Item)
{
   /*
   printf ("SPI tx_buf         %10p    %10p\n", (void *)(intptr_t)Item->Transfer [0].tx_buf, (void *)(intptr_t)Item->Transfer [1].tx_buf);
   printf ("SPI rx_buf         %10p    %10p\n", (void *)(intptr_t)Item->Transfer [0].rx_buf, (void *)(intptr_t)Item->Transfer [1].rx_buf);
   printf ("SPI len            %10i    %10i\n", Item->Transfer [0].len, Item->Transfer [1].len);
   printf ("SPI delay_usecs    %10i    %10i\n", Item->Transfer [0].delay_usecs, Item->Transfer [1].delay_usecs);
   printf ("SPI speed_hz       %10i Hz %10i Hz\n", Item->Transfer [0].speed_hz, Item->Transfer [1].speed_hz);
   printf ("SPI bits_per_word  %10i    %10i\n", Item->Transfer [0].bits_per_word, Item->Transfer [1].bits_per_word);
   */
   printf ("Frame Received     %10lli\n", Item->Received_Count);
   printf ("Frame Unreceived   %10lli\n", Item->Unreceived_Count);
   printf ("Transactions       %10lli\n", Item->Transaction_Count);
   printf ("\n");
}
