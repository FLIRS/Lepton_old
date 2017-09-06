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

#define Lepton_Major_Success          0x00000001
#define Lepton_Major_Fail             0x00000002
#define Lepton_Major_Transfer_Success 0x00000010
#define Lepton_Major_Transfer_Fail    0x00000020
#define Lepton_Major_Unsynced         0x00000100
#define Lepton_Major_Synced           0x00000200
#define Lepton_Major_Synced           0x00000200



struct Lepton_Major_SPI
{
   char const *            Name;
   int                     Handle;
   int                     Status;
   int                     Error;
   char *                  Error_Message;
};


void Lepton_Major_SPI_Contruct (struct Lepton_Major_SPI * Item)
{
   Item->Name = NULL;
   Item->Handle = -1;
   Item->Status = 0;
   Item->Error = 0;
   Item->Error_Message = NULL;
}


void Lepton_Major_SPI_Open 
(
   struct Lepton_Major_SPI * Item, 
   char const * Name
)
{
   Item->Name = Name;
   errno = 0;
   Item->Handle = open (Item->Name, O_RDWR, S_IRUSR | S_IWUSR);
   Item->Error = errno;
   Item->Error_Message = strerror (Item->Error);
   
   if (Item->Error == -1) {return;};
   
   int Mode = Lepton_SPI_Mode;
   int Result;
   int SPI_Max_Speed = Lepton_SPI_Speed_Recomended;
   int Bits_Per_Word = Lepton_SPI_Bits_Per_Word;
   
   errno = 0;
   Result = ioctl (Item->Handle, SPI_IOC_WR_MODE, &Mode);
   Item->Error = errno;
   Item->Error_Message = strerror (Item->Error);
   if (Result == -1) {return;};
   
   errno = 0;
   Result = ioctl (Item->Handle, SPI_IOC_WR_MAX_SPEED_HZ, &SPI_Max_Speed);
   Item->Error = errno;
   Item->Error_Message = strerror (Item->Error);
   if (Result == -1) {return;};
   
   errno = 0;
   Result = ioctl (Item->Handle, SPI_IOC_WR_BITS_PER_WORD, &Bits_Per_Word);
   Item->Error = errno;
   Item->Error_Message = strerror (Item->Error);
   if (Result == -1) {return;};
   
   errno = 0;
   Result = ioctl (Item->Handle, SPI_IOC_RD_MODE, &Mode);
   Item->Error = errno;
   Item->Error_Message = strerror (Item->Error);
   if (Result == -1) {return;};
   assert (Mode == Lepton_SPI_Mode);

   errno = 0;
   Result = ioctl (Item->Handle, SPI_IOC_RD_BITS_PER_WORD, &Bits_Per_Word);
   Item->Error = errno;
   Item->Error_Message = strerror (Item->Error);
   if (Result == -1) {return;};
   assert (Mode == Lepton_SPI_Mode);
   assert (Bits_Per_Word == Lepton_SPI_Bits_Per_Word);

   errno = 0;
   Result = ioctl (Item->Handle, SPI_IOC_RD_MAX_SPEED_HZ, &SPI_Max_Speed);
   Item->Error = errno;
   Item->Error_Message = strerror (Item->Error);
   if (Result == -1) {return;};
   assert (SPI_Max_Speed == Lepton_SPI_Speed_Recomended);
}


void Lepton_Major_SPI_Close (struct Lepton_Major_SPI * Item)
{
   errno = 0;
   close (Item->Handle);
   Item->Error = errno;
   Item->Error_Message = strerror (Item->Error);
}


int Lepton_Major_SPI_Transfer 
(
   struct Lepton_Major_SPI * Item, 
   struct spi_ioc_transfer * Transfer
)
{
   int Result;
   assert (Item->Handle != 0);
   errno = 0;
   Result = ioctl (Item->Handle, SPI_IOC_MESSAGE (1), Transfer);
   Item->Error = errno;
   Item->Error_Message = strerror (Item->Error);
   assert (Result == Transfer->len);
   if (Result == Transfer->len) 
   {
      return 1;
   }
   else
   {
      return 0;
   };
}


struct Lepton_Major
{
   struct Lepton_Major_SPI           SPI [1];
   int                               Width;
   int                               Height;
   struct Lepton_Packet            * Packet_Array;
   struct Lepton_Pixel_Grayscale16 * Pixmap;
   struct spi_ioc_transfer           Transfer [2];
   uint64_t                          Received_Count;
   uint64_t                          Unreceived_Count;
   uint64_t                          Transaction_Count;
};


void Lepton_Major_Construct (struct Lepton_Major * Item)
{
   Lepton_Major_SPI_Contruct (Item->SPI);
   Item->Width             = Lepton_Width;
   Item->Height            = Lepton_Height;
   Item->Received_Count    = 0;
   Item->Unreceived_Count  = 0;
   Item->Transaction_Count = 0;
   Item->Packet_Array      = malloc (sizeof (struct Lepton_Packet) * Item->Height);
   Item->Pixmap            = malloc (sizeof (struct Lepton_Pixel_Grayscale16) * Item->Height);
   assert (Item->Packet_Array != NULL);
   assert (Item->Pixmap != NULL);
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


void Lepton_Major_Open 
(
   struct Lepton_Major * Item, 
   char const * SPI_Name,
   char const * I2C_Name
)
{
   Lepton_Major_SPI_Contruct (Item->SPI);
   Lepton_Major_SPI_Open (Item->SPI, SPI_Name);
}


int Lepton_Major_Transfer (struct Lepton_Major * Item)
{
   //memset might not be useful here.
   memset ((void *) Item->Packet_Array, 0, sizeof (struct Lepton_Packet) * Item->Height);
   
   Item->Transaction_Count = Item->Transaction_Count + 1;
   
   //Check if the first packet is valid.
   if 
   (
      0 == Lepton_Major_SPI_Transfer        (Item->SPI, Item->Transfer + 0) ||
      0 == Lepton_Packet_Is_First           (Item->Packet_Array + 0)        ||
      0 == Lepton_Packet_Array_Is_Row_Valid (Item->Packet_Array + 0, 1)     ||
      0 == Lepton_Packet_Array_Is_Match     (Item->Packet_Array + 0, 1)
   )
   {
      Item->Unreceived_Count = Item->Unreceived_Count + 1;
      return 0;
   }
   
   //If the first packet were valid then receive rest of them.
   if 
   (
      0 == Lepton_Major_SPI_Transfer        (Item->SPI, Item->Transfer + 1)            ||
      0 == Lepton_Packet_Array_Is_Row_Valid (Item->Packet_Array + 1, Item->Height - 1) ||
      0 == Lepton_Packet_Array_Is_Match     (Item->Packet_Array + 1, Item->Height - 1)
   )
   {
      Item->Unreceived_Count = Item->Unreceived_Count + 1;
      return 0;
   }
   
   Item->Received_Count = Item->Received_Count + 1;
   
   return 1;
}


void Lepton_Major_Printf (struct Lepton_Major * Item)
{
   printf ("SPI Name           %10s\n", Item->SPI->Name);
   printf ("SPI Handle         %10i\n", Item->SPI->Handle);
   printf ("SPI Error          %10i\n", Item->SPI->Error);
   printf ("SPI Error_Message  %10s\n", Item->SPI->Error_Message);
   printf ("SPI Status         %10i\n", Item->SPI->Status);
   printf ("SPI tx_buf         %10p    %10p\n", (void *)(intptr_t)Item->Transfer [0].tx_buf, (void *)(intptr_t)Item->Transfer [1].tx_buf);
   printf ("SPI rx_buf         %10p    %10p\n", (void *)(intptr_t)Item->Transfer [0].rx_buf, (void *)(intptr_t)Item->Transfer [1].rx_buf);
   printf ("SPI len            %10i    %10i\n", Item->Transfer [0].len, Item->Transfer [1].len);
   printf ("SPI delay_usecs    %10i    %10i\n", Item->Transfer [0].delay_usecs, Item->Transfer [1].delay_usecs);
   printf ("SPI speed_hz       %10i Hz %10i Hz\n", Item->Transfer [0].speed_hz, Item->Transfer [1].speed_hz);
   printf ("SPI bits_per_word  %10i    %10i\n", Item->Transfer [0].bits_per_word, Item->Transfer [1].bits_per_word);
   printf ("Width              %10i\n", Item->Width);
   printf ("Height             %10i\n", Item->Height);
   printf ("Packet_Array       %10p\n", Item->Packet_Array);
   printf ("Pixmap             %10p\n", Item->Pixmap);
   printf ("Received           %10lli\n", Item->Received_Count);
   printf ("Unreceived         %10lli\n", Item->Unreceived_Count);
   printf ("Transactions       %10lli\n", Item->Transaction_Count);
   printf ("\n");
}
