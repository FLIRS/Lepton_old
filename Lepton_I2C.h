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

//ioctl
#include <sys/ioctl.h>

//errno
#include <errno.h>

//read
#include <unistd.h>

//htons
#include <arpa/inet.h>

//assert
#include <assert.h>

//memcpy
#include <string.h>


#include "Lepton_Error.h"


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

//Page 103
#define Lepton_I2C_Command_GPIO_Mode_Get (0x4854)
#define Lepton_I2C_Command_GPIO_Mode_Set (0x4855)
#define Lepton_I2C_GPIO_Mode_GPIO 0
#define Lepton_I2C_GPIO_Mode_I2C_Master 1
#define Lepton_I2C_GPIO_Mode_SPI_Master_VLB_Data 2
#define Lepton_I2C_GPIO_Mode_SPIO_Master_Reg_Data 3
#define Lepton_I2C_GPIO_Mode_SPI_Slave_VLB_Data 4
#define Lepton_I2C_GPIO_Mode_Vsync 5

//Page 104
#define Lepton_I2C_Command_Vsyncdelay_Get (0x4858)
#define Lepton_I2C_Command_Vsyncdelay_Set (0x4859)
#define Lepton_I2C_Vsyncdelay_Minus3 (-3)
#define Lepton_I2C_Vsyncdelay_Minus2 (-2)
#define Lepton_I2C_Vsyncdelay_Minus1 (-1)
#define Lepton_I2C_Vsyncdelay_0      (0)
#define Lepton_I2C_Vsyncdelay_Plus1  (1)
#define Lepton_I2C_Vsyncdelay_Plus2  (2)
#define Lepton_I2C_Vsyncdelay_Plus3  (3)

//page 41
#define Lepton_I2C_Command_Ping 0x0202


//OEM Restore User Defaults.
//Page 108. Section 4.6.18
//This function will restore the OEM user defualts from OTP if OTP was previosly written with these defaults.
//If user defaults ware not written, an error code is returned.
#define Lepton_I2C_Command_Restore_User_Defaults (0x4862)


//Auxiliry help function.
#define Lepton_Bit_Subset(Value,Mask) (((Value) & (Mask)) == (Mask))


//Comand Type
//Page 18. Section 2.1.3.4
//A command type specifies what the command does.
#define Lepton_I2C_Command_Type_Get 0x00
#define Lepton_I2C_Command_Type_Set 0x01
#define Lepton_I2C_Command_Type_Run 0x02
#define Lepton_I2C_Command_Type_Mask 0x03


enum Lepton_Debug
{
   Lepton_Debug0,
   Lepton_Debug1,
   Lepton_Debug2
};


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


struct __attribute__((__packed__)) Lepton_I2C_GPIO_Mode
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


int Lepton_I2C_Open (enum Lepton_Debug Debug, char const * Name)
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

//Read.
int Lepton_I2C_Read 
(enum Lepton_Debug Debug, int Device, uint8_t const * Data, size_t Size8)
{
   int Result;
   errno = 0;
   Result = read (Device, (void *) Data, Size8);
   switch (Debug)
   {
      case Lepton_Debug0:
      break;
      
      case Lepton_Debug1:
         if (errno != 0) {Lepton_Print_Error (stderr, Result, errno);}
      case Lepton_Debug2:
         assert (errno == 0);
         assert (Result == Size8);
      break;
   }
   return Result;
}


//Write
int Lepton_I2C_Write 
(enum Lepton_Debug Debug, int Device, uint8_t * Data, size_t Size8)
{
   int Result;
   errno = 0;
   Result = write (Device, (void *) Data, Size8);
   switch (Debug)
   {
      case Lepton_Debug0:
      break;
      
      case Lepton_Debug1:
         if (errno != 0) {Lepton_Print_Error (stderr, Result, errno);}
      case Lepton_Debug2:
         assert (errno == 0);
         assert (Result == Size8);
      break;
   }
   return Result;
}


//Select which register to read from or write to.
//It is recomended to only use this for reading.
void Lepton_I2C_Select 
(enum Lepton_Debug Debug, int Device, uint16_t Register)
{
   uint16_t Buffer [] = {htons (Register)};
   Lepton_I2C_Write (Debug, Device, (uint8_t *) Buffer, sizeof (Buffer));
}


//Write to length register.
void Lepton_I2C_Write_Length 
(enum Lepton_Debug Debug, int Device, size_t Size8)
{
   //Lepton uses 16b unit size.
   //We must write two 8b for every 16b.
   assert ((Size8 % 2) == 0);
   uint16_t Size16 = Size8 / sizeof (uint16_t);
   uint16_t Buffer [] = {htons (Lepton_I2C_Register_Length), htobe16 (Size16)};
   Lepton_I2C_Write (Debug, Device, (uint8_t *) Buffer, sizeof (Buffer));
}


//Write to command register.
void Lepton_I2C_Write_Command 
(enum Lepton_Debug Debug, int Device, uint16_t Command)
{
   //First 16 bit selects the register to write to.
   //The rest goes to that register.
   uint16_t Buffer [] = {htons (Lepton_I2C_Register_Command), htobe16 (Command)};
   Lepton_I2C_Write (Debug, Device, (uint8_t *) Buffer, sizeof (Buffer));
}


//Read status
uint16_t Lepton_I2C_Status (enum Lepton_Debug Debug, int Device)
{
   uint16_t Status;
   //Select the status register.
   Lepton_I2C_Select (Debug, Device, Lepton_I2C_Register_Status);
   //Read from the selected register which is the status register.
   Lepton_I2C_Read (Debug, Device, (uint8_t *) &Status, sizeof (uint16_t));
   return Status;
}


//Run a command with checking
void Lepton_I2C_Execute1 
(enum Lepton_Debug Debug, int Device, uint16_t Command, void * Data, size_t Size8)
{
   struct __attribute__((__packed__))
   {
      uint16_t Register;
      uint16_t Data [16];
   } Bank;
   switch (Command & Lepton_I2C_Command_Type_Mask)
   {
      case Lepton_I2C_Command_Type_Get:
         Lepton_I2C_Write_Length (Debug, Device, Size8);
         Lepton_I2C_Write_Command (Debug, Device, Command);
         break;
         
      case Lepton_I2C_Command_Type_Set:
         Bank.Register = htobe16 (Lepton_I2C_Register_Data_0);
         assert (Size8 <= 16);
         memcpy (Bank.Data, Data, Size8);
         Lepton_I2C_Write (Debug, Device, (uint8_t *) &Bank, Size8 + sizeof (uint16_t));
         Lepton_I2C_Write_Length (Debug, Device, Size8);
         Lepton_I2C_Write_Command (Debug, Device, Command);
         break;
         
      case Lepton_I2C_Command_Type_Run:
         Lepton_I2C_Write_Command (Debug, Device, Command);
         break;
   };
}


void Lepton_I2C_Execute
(enum Lepton_Debug Debug, int Device, uint16_t Command, void * Data, size_t Size8, uint16_t * Status, int * Stage)
{
   switch (*Stage)
   {
      case 0:
      *Stage = 1;
      
      case 1:
         //First check if it is busy or not.
         *Status = Lepton_I2C_Status (Debug, Device);
         if ((*Status & htobe16 (Lepton_I2C_Register_Status_Ok_Mask)) == 0) {return;}
         //It is not busy then carry out the command.
         Lepton_I2C_Execute1 (Debug, Device, Command, Data, Size8);
         *Stage = 2;
         
      case 2:
         //Check if command is finnished.
         *Status = Lepton_I2C_Status (Debug, Device);
         if ((*Status & htobe16 (Lepton_I2C_Register_Status_Ok_Mask)) == 0) {return;}
         if ((Command & Lepton_I2C_Command_Type_Mask) == Lepton_I2C_Command_Type_Get)
         {
            Lepton_I2C_Select (Debug, Device, Lepton_I2C_Register_Data_0);
            Lepton_I2C_Read (Debug, Device, Data, Size8);
         }
         //Command is finnished.
         *Stage = 0;
   };
}

