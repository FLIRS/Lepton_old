#pragma once

//Using:
//Lepton_Packet_Is_Discard
//Lepton_Packet_Is_Row
//Lepton_Packet_Is_Match
#include "Lepton.h"
#include "Lepton_Packets.h"
#include "Lepton_SPI.h"
#include "Lepton_Pixels.h"
#include "Lepton_Conversions.h"

//Using:
//size_t
#include <stddef.h>
#include <stdbool.h>


enum Lepton_Stream_Result
{
   Lepton_Stream_Invalid_Segment = Lepton_Error_Generator (1),
   Lepton_Stream_Discard         = Lepton_Error_Generator (2),
   Lepton_Stream_Mismatch        = Lepton_Error_Generator (3),
   Lepton_Stream_Invalid_Row     = Lepton_Error_Generator (4),
   Lepton_Stream_Shifting        = Lepton_Error_Generator (5)
};


//Return 1 if Stream is ordered.
//Return 0 if Stream is not ordered.
bool 
Lepton_Stream_Check_Climbing_Number 
(struct Lepton_Packet * Stream, size_t Count)
{
   Lepton_Assert (Count > 0, Lepton_Error_Range, "Count %i is non posetive", Count);
   for (size_t I = 0; I < (Count - 1); I = I + 1)
   {
      uint8_t X0 = Stream [I + 0].Number;
      uint8_t X1 = Stream [I + 1].Number;
      if ((X0 + 1) != X1) {return false;}
   }
   return true;
}


//Assigns the segment pixel map to correct position in `Pixmap`.
//Automaticly shifting the stream when the stream has an offset.
//Returns `Lepton_Stream_Shifting` when the stream is shifting.
//Returns the segment number `1 .. 4` on succesful.
//Returns error code on error.
int32_t 
Lepton_Stream_Accept 
(int Device, struct Lepton_Pixel_Grayscale16 * Pixmap)
{
   struct Lepton_Packet Stream [Lepton_Height];
   int Result;
   Result = Lepton_SPI_Transfer_Stream8 ((uint8_t *) Stream, sizeof (Stream), Device);
   if (Result != sizeof (Stream)) {return Result;}
   for (size_t I = 0; I < Lepton_Height; I = I + 1)
   {
      if (Lepton_Packet_Is_Discard (Stream + I) == 1) {return Lepton_Stream_Discard;}
      if (Lepton_Packet_Is_Row     (Stream + I) == 0) {return Lepton_Stream_Invalid_Row;}
      if (Lepton_Packet_Is_Match   (Stream + I) == 0) {return Lepton_Stream_Mismatch;}
   }
   if (Stream [20].Number != 20)
   {
      Lepton_SPI_Transfer_Stream (Stream, 1, Device);
      return Lepton_Stream_Shifting;
   }
   int Segment;
   Segment = Stream [20].Reserved >> 4;
   if ((Segment <= 0) || (Segment > 4)) {return Lepton_Stream_Invalid_Segment;}
   for (size_t I = 0; I < Lepton_Height; I = I + 1)
   {
      Lepton_Conversions_Packet_To_Grayscale16 (Stream + I, Pixmap, Segment - 1);
   }
   return Segment;
}

