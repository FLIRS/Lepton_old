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



void Lepton_Strings_Base_printf (int Value, size_t Width, size_t Base)
{
   char Buffer [100] = {'\0'};
   assert (Width < sizeof (Buffer));
   memset (Buffer, '0', Width);
   Lepton_Strings_Base_Converter (Value, Buffer, Width, Base);
   printf ("%s", Buffer);
}
