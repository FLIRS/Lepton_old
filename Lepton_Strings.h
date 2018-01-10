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

//size_t
#include <sys/types.h>

//uint16_t
#include <stdint.h>

//memset
#include <string.h>

//printf
#include <stdio.h>

void Lepton_Strings_Base_Converter (int Value, char * String, size_t Size, size_t Base)
{
   char Hex [] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   size_t I = Size;
   while (1)
   {
      if (Value == 0 || I == 0)
      {
         break;
      }
      I = I - 1;
      String [I] = Hex [Value % Base];
      Value = Value / Base;
   }
}



void Lepton_Strings_Base_printf (FILE * File, int Value, size_t Width, size_t Base, char const * Format)
{
   char Buffer [100] = {'\0'};
   assert (Width < sizeof (Buffer));
   memset (Buffer, '0', Width);
   Lepton_Strings_Base_Converter (Value, Buffer, Width, Base);
   fprintf (File, Format, Buffer);
}




char const * Lepton_Strings_Command (int Command)
{
   switch (Command)
   {
      case Lepton_I2C_Command_AUX_Kelvin:
      return "AUX_Kelvin";
      case Lepton_I2C_Command_FPA_Kelvin:
      return "FPA_Kelvin";
      case Lepton_I2C_Command_Uptime:
      return "Uptime";
      case Lepton_I2C_Command_FFC_Mode_Get:
      return "FFC_Mode_Get";
      case Lepton_I2C_Command_FFC_Status:
      return "FFC_Status";
      default:
      return "Unkown";
   }
}


char const * Lepton_Strings_FFC_Profile (int Command)
{
   switch (Command)
   {
      case Lepton_I2C_FFC_Profile_Manual:
      return "FFC_Profile_Manual";
      case Lepton_I2C_FFC_Profile_Auto:
      return "FFC_Profile_Auto";
      case Lepton_I2C_FFC_Profile_External:
      return "FFC_Profile_External";
      default:
      return "Unkown";
   }
}


char const * Lepton_Strings_FFC_Status (int Command)
{
   switch (Command)
   {
      case Lepton_I2C_FFC_Ready:
      return "FFC_Ready";
      case Lepton_I2C_FFC_Busy:
      return "FFC_Busy";
      case Lepton_I2C_FFC_Average:
      return "FFC_Average";
      default:
      return "Unkown";
   }
}
