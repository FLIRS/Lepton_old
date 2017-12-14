#pragma once

#include <stdio.h>

#include "Lepton_SPI.h"
#include "Lepton_I2C.h"


struct __attribute__((__packed__)) Lepton_App_Bank
{
   uint16_t Selector;
   union
   {
      uint16_t Data [16];
      struct Lepton_I2C_Temperature_100K AUX_Temp;
      struct Lepton_I2C_Temperature_100K FPA_Temp;
   };
};

#define Lepton_App_State_Start 0
#define Lepton_App_State_Running 1

struct Lepton_App
{
   int I2C_Device;
   int SPI_Device;
   uint16_t Status;
   uint16_t Command;
   size_t Command_Size8;
   size_t Command_Counter;
   int Command_Stage;
   struct Lepton_App_Bank Bank;
};

void Lepton_App_Init 
(struct Lepton_App * App, char * I2C_Name, char * SPI_Name)
{
   App->I2C_Device = Lepton_I2C_Open (I2C_Name);
   App->SPI_Device = Lepton_SPI_Open (SPI_Name);
   App->Status = 0;
   App->Command = 0;
   App->Command_Size8 = 0;
   App->Command_Counter = 0;
   App->Command_Stage = Lepton_App_State_Start;
   memset (&(App->Bank), 0, sizeof (struct Lepton_App_Bank));
   App->Bank.Selector = htons (Lepton_I2C_Register_Data_0);
}

void Lepton_App_Update 
(struct Lepton_App * App)
{
   if (App->Command == 0) {return;}
   App->Status = Lepton_I2C_Status (App->I2C_Device);
   if ((App->Status & htobe16 (Lepton_I2C_Register_Status_Ok_Mask)) == 0) {return;}
   switch (App->Command_Stage)
   {
      case Lepton_App_State_Start:
      if ((App->Command & 0x3) == 0x1)
      {
         Lepton_I2C_Write (App->I2C_Device, (uint8_t *) &(App->Bank), sizeof (uint16_t) + App->Command_Size8);
      }
      Lepton_I2C_Request (App->I2C_Device, App->Command, App->Command_Size8);
      App->Command_Stage = Lepton_App_State_Running;
      return;
      
      case Lepton_App_State_Running:
      Lepton_I2C_Select (App->I2C_Device, Lepton_I2C_Register_Data_0);
      Lepton_I2C_Read (App->I2C_Device, (uint8_t *) &(App->Bank.Data), App->Command_Size8);
      App->Status = Lepton_I2C_Status (App->I2C_Device);
      App->Command = 0;
      App->Command_Stage = Lepton_App_State_Start;
   };
   
}
