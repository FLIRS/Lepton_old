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

//size_t
#include <stddef.h>

//assert
#include <assert.h>

//Map linear memory to surface.
size_t Lepton_Map_2D 
(
   size_t X, 
   size_t Y, 
   size_t W, 
   size_t H
)
{
   size_t Index;
   assert (X < W);
   assert (Y < H);
   Index = W * Y + X;
   assert (Index < W*H);
   return Index;
}

