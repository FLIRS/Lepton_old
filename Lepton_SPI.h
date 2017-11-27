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

//gcc -std=gnu11 -fdiagnostics-color -Wall -Wno-missing-braces Lepton_SPI.h

#pragma once

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <assert.h>
#include <string.h>

#include "Lepton_Packets.h"


//Lepton FLIR Datasheet page 29.
//"The Lepton uses SPI Mode 3 (CPOL=1, CPHA=1)".
//"SCK is HIGH when idle."
#define Lepton_SPI_Mode SPI_MODE_3


//Page 30.
//The maximum clock rate is 20 MHz.
#define Lepton_SPI_Speed_Max 20000000


//Page 30.
//The minimum clock rate is a function of the number of bits of data per frame that need to be retrieved.
//As described in the sections that follow, the number of bits of data varies 
//depending upon user settings (video format mode, telemetry mode).
//For default conditions (Raw14 mode, telemetry disabled), 
//there are 60 video packets per frame, 
//each 1312 bits long, at approximately 25.9 frames per second.
//Therefore, the minimum rate is on the order of 2 MHz.
#define Lepton_SPI_Speed_Min 2000000
#define Lepton_SPI_Speed_Recomended 16000000


//TODO: Why 8 bit?
#define Lepton_SPI_Bits_Per_Word 8



int Lepton_SPI_Open (char const * Name)
{
   int Device;
   int Mode = Lepton_SPI_Mode;
   int Bits_Per_Word = Lepton_SPI_Bits_Per_Word;
   int SPI_Max_Speed = Lepton_SPI_Speed_Recomended;
   int Result;
   
   
   Device = open (Name, O_RDWR, S_IRUSR | S_IWUSR);
   assert (Device != -1);
   
   Result = ioctl (Device, SPI_IOC_WR_MODE, &Mode);
   assert (Result != -1 && "can't set spi mode");
   
   Result = ioctl (Device, SPI_IOC_WR_MAX_SPEED_HZ, &SPI_Max_Speed);
   assert (Result != -1 && "can't set max speed hz");
   
   Result = ioctl (Device, SPI_IOC_WR_BITS_PER_WORD, &Bits_Per_Word);
   assert (Result != -1 && "can't set bits per word");
   
   
   Result = ioctl (Device, SPI_IOC_RD_MODE, &Mode);
   assert (Result != -1 && "can't get spi mode");
   assert (Mode == Lepton_SPI_Mode);

   Result = ioctl (Device, SPI_IOC_RD_BITS_PER_WORD, &Bits_Per_Word);
   assert (Result != -1 && "can't get bits per word");
   assert (Bits_Per_Word == Lepton_SPI_Bits_Per_Word);

   Result = ioctl (Device, SPI_IOC_RD_MAX_SPEED_HZ, &SPI_Max_Speed);
   assert (Result != -1 && "can't get max speed hz");
   assert (SPI_Max_Speed == Lepton_SPI_Speed_Recomended);

   return Device;
}


void Lepton_SPI_Transfer_Init 
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



//Return 0 when not successful.
//Return 1 when successful.
//Receive <Count> number of uint8_t <Data> from SPI <Device>
int Lepton_SPI_Transfer_Stream8 
(uint8_t * Data, size_t Count, int Device)
{
   struct spi_ioc_transfer Transfer =
   {
      .tx_buf = (unsigned long) NULL,
      .rx_buf = (unsigned long) Data,
      .len = Count,
      .delay_usecs = 0,
      .speed_hz = Lepton_SPI_Speed_Max,
      .bits_per_word = Lepton_SPI_Bits_Per_Word
   };
   
   //memset might not be useful here.
   memset ((void *) Data, 0, Count);
   
   int Result = ioctl (Device, SPI_IOC_MESSAGE (1), &Transfer);
   
   return (Result == Count);
}


//Return 0 when not successful.
//Return 1 when successful.
//Receive <Count> number of Lepton_Packet <Stream> from SPI <Device>
int Lepton_SPI_Transfer_Stream 
(struct Lepton_Packet * Stream, size_t Count, int Device)
{
   int Result;
   Result = Lepton_SPI_Transfer_Stream8 ((uint8_t *) Stream, sizeof (struct Lepton_Packet) * Count, Device);
   return Result;
}
