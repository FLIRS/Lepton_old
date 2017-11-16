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

//uint16_t
#include <stdint.h>

//open
#include <fcntl.h>

//I2C_SlAVE
#include <linux/i2c-dev.h>

//size_t
#include <sys/types.h>

//errno
#include <errno.h>

#include <stdio.h>


//I2C settings.
//Page 14.
#define Lepton_I2C_Address 0x2A
#define Lepton_I2C_Transfer_Bit_Width 16
#define Lepton_I2C_Transfer_Byte_Width 2


//Register addresses.
//Page 9.
#define Lepton_I2C_Register_Status (0x0002)
#define Lepton_I2C_Register_Command (0x0004)
#define Lepton_I2C_Register_Length (0x0006)
#define Lepton_I2C_Register_Data_0 (0x0008)
#define Lepton_I2C_Register_Data_1 (0x000A)
#define Lepton_I2C_Register_Data_2 (0x000C)
#define Lepton_I2C_Register_Data_3 (0x000E)


//Page 18.
#define Lepton_I2C_Register_Status_0 (0x0000)
#define Lepton_I2C_Register_Status_Busy_Mask (0x0001)
#define Lepton_I2C_Register_Status_Boot_Mode_Mask (0x0002)
#define Lepton_I2C_Register_Status_Boot_Status_Mask (0x0004)
#define Lepton_I2C_Register_Status_Ok_Mask (Lepton_I2C_Register_Status_Boot_Mode_Mask | Lepton_I2C_Register_Status_Boot_Status_Mask)


//Page 17.
#define Lepton_I2C_Command_Uptime           (0x020C)
#define Lepton_I2C_Command_AUX_Kelvin       (0x0210)
#define Lepton_I2C_Command_FPA_Kelvin       (0x0214)
#define Lepton_I2C_Command_Reboot           (0x4842)


//Page 58
#define Lepton_I2C_Command_Shutter_Position_Get (0x0238)
#define Lepton_I2C_Command_Shutter_Position_Set (0x0239)
#define Lepton_I2C_Shutter_Position_Unkown -1
#define Lepton_I2C_Shutter_Position_Idle 0
#define Lepton_I2C_Shutter_Position_Open 1
#define Lepton_I2C_Shutter_Position_Closed 2
#define Lepton_I2C_Shutter_Position_Break_On 3


//Page 59
#define Lepton_I2C_Command_FFC_Mode_Get (0x023C)
#define Lepton_I2C_Command_FFC_Mode_Set (0x023D)
#define Lepton_I2C_FFC_Profile_Manual 0
#define Lepton_I2C_FFC_Profile_Auto 1
#define Lepton_I2C_FFC_Profile_External 2


//Page 63
#define Lepton_I2C_Command_FFC_Status (0x0244)
#define Lepton_I2C_FFC_Ready 0
#define Lepton_I2C_FFC_Busy 1
#define Lepton_I2C_FFC_Average 2


//page 41
#define Lepton_I2C_Write_Command_Ping 0x0202


//Page 108
#define Lepton_I2C_Command_Restore_User_Defaults (0x4862)


#define Lepton_Bit_Subset(Value,Mask) (((Value) & (Mask)) == (Mask))


struct __attribute__((__packed__)) Lepton_I2C_FFC_Status
{
   uint16_t Value;
};


struct __attribute__((__packed__)) Lepton_I2C_Temperature_100K
{
   uint16_t Value;
};

struct __attribute__((__packed__)) Lepton_I2C_Uptime_100s
{
   uint16_t Value;
};

struct __attribute__((__packed__)) Lepton_I2C_Shutter_Mode 
{
   uint16_t Shutter_Mode;
   uint16_t Padding_0;
   uint16_t Temp_Lockout_State;
   uint16_t Padding_1;
   uint16_t Video_Freeze_During_FFC;
   uint16_t Padding_2;
   uint16_t FFC_Desired;
   uint16_t Padding_3;
   uint32_t Elapsed_Time_Since_Last_FFC;
   //uint16_t Padding_4;
   uint32_t Desired_FFC_Period;
   //uint16_t Padding_5;
   uint16_t	Explicit_Command_To_Open;
   uint16_t Padding_6;
   uint16_t Desired_FFC_Temp_Delta;
   uint16_t Imminent_Delay;
};

int Lepton_I2C_Open (char const * Name)
{
   int Device;
   Device = open (Name, O_RDWR);
   assert (Device != -1);
   {
      int Result;
      Result = ioctl (Device, I2C_SLAVE, Lepton_I2C_Address);
      assert (Result == 0);
   }
   return Device;
}


struct Lepton_I2C_Debug
{
   int Stage;
   int Result;
   int Error;
   int Size;
   uint16_t Status;
};


void Lepton_I2C_Print (struct Lepton_I2C_Debug * Item)
{
   printf ("Stage  %06i\n", Item->Stage);
   printf ("Result %06i\n", Item->Result);
   printf ("Error  %06i\n", Item->Error);
   printf ("Size   %06i\n", Item->Size);
   printf ("Status %06i\n", Item->Status);
};


//Read.
void Lepton_I2C_Read 
(
   struct Lepton_I2C_Debug * Debug, 
   int Device, 
   uint16_t const * Data, 
   size_t Size)
{
   if (Debug == NULL)
   {
      read (Device, (void *) Data, Size);
   }
   else
   {
      errno = 0;
      Debug->Size = Size; 
      Debug->Result = read (Device, (void *) Data, Size);
      Debug->Error = errno;
   }
}


//Write
void Lepton_I2C_Write 
(
   struct Lepton_I2C_Debug * Debug,
   int Device, 
   uint16_t * Data, 
   size_t Size
)
{
   if (Debug == NULL)
   {
      write (Device, (void *) Data, Size);
   }
   else
   {
      errno = 0;
      Debug->Size = Size; 
      Debug->Result = write (Device, (void *) Data, Size);
      Debug->Error = errno;
   }
}


//Check.
int Lepton_I2C_Check (struct Lepton_I2C_Debug * Debug)
{
   return 
   (Debug->Error == 0) &&
   (Debug->Status == htons (Lepton_I2C_Register_Status_Ok_Mask)) &&
   (Debug->Result == Debug->Size);
}


//Select which register to read from or write to.
//It is recomended to only use this for reading.
void Lepton_I2C_Select 
(
   struct Lepton_I2C_Debug * Debug, 
   int Device, 
   uint16_t Register
)
{
   uint16_t Buffer [] = {htons (Register)};
   Lepton_I2C_Write (Debug, Device, Buffer, sizeof (Buffer));
}


//Write to length register.
void Lepton_I2C_Write_Length 
(
   struct Lepton_I2C_Debug * Debug, 
   int Device, 
   uint16_t Size
)
{
   //Lepton uses 16b unit size.
   //We must write two 8b for every 16b.
   assert ((Size % 2) == 0);
   uint16_t Size16 = Size / sizeof (uint16_t);
   uint16_t Buffer [] = {htons (Lepton_I2C_Register_Length), htons (Size16)};
   Lepton_I2C_Write (Debug, Device, (void *) Buffer, sizeof (Buffer));
}


//Write to command register.
void Lepton_I2C_Write_Command 
(
   struct Lepton_I2C_Debug * Debug, 
   int Device, 
   uint16_t Command
)
{
   //First 16 bit selects the register to write to.
   //The rest goes to that register.
   uint16_t Buffer [] = {htons (Lepton_I2C_Register_Command), htons (Command)};
   Lepton_I2C_Write (Debug, Device, Buffer, sizeof (Buffer));
}


//Requests data by writing a command to command register 
//and size to length register.
void Lepton_I2C_Request 
(
   struct Lepton_I2C_Debug * Debug, 
   int Device, 
   uint16_t Command, 
   uint16_t Size
)
{
   Lepton_I2C_Write_Length (Debug, Device, Size);
   if (!Lepton_I2C_Check (Debug)) {return;};
   Lepton_I2C_Write_Command (Debug, Device, Command);
}


//Read status
void Lepton_I2C_Status (struct Lepton_I2C_Debug * Debug, int Device)
{
   //Select the status register.
   Lepton_I2C_Select (Debug, Device, Lepton_I2C_Register_Status);
   if ((Debug->Error != 0) || (Debug->Result != Debug->Size)) {return;};
   //Start read from the selected register.
   Lepton_I2C_Read (Debug, Device, &(Debug->Status), sizeof (uint16_t));
}


//Run a command with checking
void Lepton_I2C_Run 
(
   struct Lepton_I2C_Debug * Debug, 
   int Device, 
   uint16_t Command
)
{
   //Before requesting data we need to check if the camera  
   //is booted, has no error, is not busy.
   Lepton_I2C_Status (Debug, Device);
   if (!Lepton_I2C_Check (Debug)) {return;};
   
   //Run the command.
   Lepton_I2C_Write_Command (Debug, Device, Command);
   
   //It is up to the user to check if the command is successful.
}


//Get data with checking.
void Lepton_I2C_Get 
(
   struct Lepton_I2C_Debug * Debug, 
   int Device, 
   uint16_t Command, 
   uint16_t * Data, 
   uint16_t Size
)
{
   //Before requesting data we need to check if the camera  
   //is booted, has no error, is not busy.
   Lepton_I2C_Status (Debug, Device);
   if (!Lepton_I2C_Check (Debug)) {return;};
   
   //Tell which data we need and how much data.
   Lepton_I2C_Request (Debug, Device, Command, Size);
   if (!Lepton_I2C_Check (Debug)) {return;};
   
   //Check if the data requested is available.
   Lepton_I2C_Status (Debug, Device);
   if (!Lepton_I2C_Check (Debug)) {return;};
   
   //Select the data register and read from that.
   Lepton_I2C_Select (Debug, Device, Lepton_I2C_Register_Data_0);
   if (!Lepton_I2C_Check (Debug)) {return;};
   Lepton_I2C_Read (Debug, Device, Data, Size);
}

