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


//Page 108
#define Lepton_I2C_Command_Restore_User_Defaults (0x4862)


#define Lepton_Bit_Subset(Value,Mask) (((Value) & (Mask)) == (Mask))


#define Lepton_I2C_Stage_Error -1
#define Lepton_I2C_Stage_Start 1
#define Lepton_I2C_Stage_Request 2
#define Lepton_I2C_Stage_Complete 3


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
   uint32_t Value;
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

//Read.
void Lepton_I2C_Read 
(int Device, uint8_t const * Data, size_t Size8)
{
   int Result;
   errno = 0;
   Result = read (Device, (void *) Data, Size8);
   assert (errno == 0);
   assert (Result == Size8);
}


//Write
void Lepton_I2C_Write 
(int Device, uint8_t * Data, size_t Size8)
{
   int Result;
   errno = 0;
   Result = write (Device, (void *) Data, Size8);
   assert (errno == 0);
   assert (Result == Size8);
}


//Select which register to read from or write to.
//It is recomended to only use this for reading.
void Lepton_I2C_Select 
(int Device, uint16_t Register)
{
   uint16_t Buffer [] = {htons (Register)};
   Lepton_I2C_Write (Device, (uint8_t *) Buffer, sizeof (Buffer));
}


//Write to length register.
void Lepton_I2C_Write_Length 
(int Device, size_t Size8)
{
   //Lepton uses 16b unit size.
   //We must write two 8b for every 16b.
   assert ((Size8 % 2) == 0);
   uint16_t Size16 = Size8 / sizeof (uint16_t);
   uint16_t Buffer [] = {htons (Lepton_I2C_Register_Length), htons (Size16)};
   Lepton_I2C_Write (Device, (uint8_t *) Buffer, sizeof (Buffer));
}


//Write to command register.
void Lepton_I2C_Write_Command 
(int Device, uint16_t Command)
{
   //First 16 bit selects the register to write to.
   //The rest goes to that register.
   uint16_t Buffer [] = {htons (Lepton_I2C_Register_Command), htons (Command)};
   Lepton_I2C_Write (Device, (uint8_t *) Buffer, sizeof (Buffer));
}


//Requests data by writing a command to command register 
//and size to length register.
void Lepton_I2C_Request 
(int Device, uint16_t Command, uint16_t Size)
{
   Lepton_I2C_Write_Length (Device, Size);
   Lepton_I2C_Write_Command (Device, Command);
}


//Read status
uint16_t Lepton_I2C_Status (int Device)
{
   uint16_t Status;
   //Select the status register.
   Lepton_I2C_Select (Device, Lepton_I2C_Register_Status);
   //Read from the selected register which is the status register.
   Lepton_I2C_Read (Device, (uint8_t *) &Status, sizeof (uint16_t));
   return Status;
}


//Run a command with checking
void Lepton_I2C_Run 
(int Device, uint16_t Command)
{
   //TODO: Check status.
   Lepton_I2C_Write_Command (Device, Command);
   //TODO: Check status.
}


uint16_t Lepton_I2C_Get 
(int Device, uint16_t Command, uint8_t * Data, size_t Size8, int * Stage)
{
   uint16_t Status;
   switch (*Stage)
   {
      case Lepton_I2C_Stage_Start: 
      //Check status register to see if it is ok to request data now.
      Status = Lepton_I2C_Status (Device);
      if ((Status & htons (Lepton_I2C_Register_Status_Ok_Mask)) == 0) {break;}
      Lepton_I2C_Request (Device, Command, Size8);
      *Stage = Lepton_I2C_Stage_Request;
      
      case Lepton_I2C_Stage_Request:
      Lepton_I2C_Select (Device, Lepton_I2C_Register_Data_0);
      Lepton_I2C_Read (Device, Data, Size8);
      //Check status register to see if the readed data was successful.
      Status = Lepton_I2C_Status (Device);
      if (Status != htons (Lepton_I2C_Register_Status_Ok_Mask)) {break;}
      *Stage = Lepton_I2C_Stage_Complete;
      
      case Lepton_I2C_Stage_Complete:
      break;
   }
   return Status;
}


uint16_t Lepton_I2C_Set 
(int Device, uint16_t Command, uint8_t * Data, size_t Size8, int * Stage)
{
   uint16_t Status;
   switch (*Stage)
   {
      case Lepton_I2C_Stage_Start: 
      //Check status register to see if it is ok to request data now.
      Status = Lepton_I2C_Status (Device);
      if ((Status & htons (Lepton_I2C_Register_Status_Ok_Mask)) == 0) {break;}
      //TODO: Select and write can be done with a single transfer.
      //Lepton_I2C_Select (Device, Lepton_I2C_Register_Data_0);
      //Lepton_I2C_Write (Device, Data, Size8);
      Lepton_I2C_Request (Device, Command, Size8);
      *Stage = Lepton_I2C_Stage_Request;
      
      case Lepton_I2C_Stage_Request:
      //Check status register to see if setting data was successful.
      Status = Lepton_I2C_Status (Device);
      if (Status != htons (Lepton_I2C_Register_Status_Ok_Mask)) {break;}
      *Stage = Lepton_I2C_Stage_Complete;
      
      case Lepton_I2C_Stage_Complete:
      break;
   }
   return Status;
}



