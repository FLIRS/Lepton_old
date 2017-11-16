#pragma once
#include "Lepton_I2C.h"
#include "Lepton_Strings.h"
#include <endian.h>

struct Lepton_I2C_Basket
{
   struct Lepton_I2C_Debug Debug [1];
   int Device;
   int Command;
   int Size;
   union
   {
      uint16_t Data [16];
      struct Lepton_I2C_Uptime_100s Uptime;
      struct Lepton_I2C_Temperature_100K Temperature;
      struct Lepton_I2C_Shutter_Mode Shutter_Mode;
      struct Lepton_I2C_FFC_Status FFC_Status;
   };
};


void Lepton_I2C_Basket_Get (struct Lepton_I2C_Basket * Item, size_t Count)
{
   for (size_t I = 0; I < Count; I = I + 1)
   {
      Lepton_I2C_Get 
      (
         Item [I].Debug, 
         Item [I].Device, 
         Item [I].Command, 
         Item [I].Data, 
         Item [I].Size
      );
   }
}
