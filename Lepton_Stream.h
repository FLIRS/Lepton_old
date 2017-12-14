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

//Return 1 if Stream is ordered.
//Return 0 if Stream is not ordered.
int Lepton_Stream_Check_Climbing_Number 
(struct Lepton_Packet * Stream, size_t Count)
{
   assert (Count > 0);
   for (size_t I = 0; I < (Count - 1); I = I + 1)
   {
      uint8_t X0 = Stream [I + 0].Number;
      uint8_t X1 = Stream [I + 1].Number;
      if ((X0 + 1) != X1)
      {
         return 0;
      }
   }
   return 1;
}

//Return 1 if <Stream> is valid.
//Return 0 if <Stream> is not valid.
int Lepton_Stream_Validity 
(struct Lepton_Packet * Stream, size_t Count)
{
   for (size_t I = 0; I < Count; I = I + 1)
   {
      if (Lepton_Packet_Is_Discard (Stream + I) == 1) {return 0;}
      if (Lepton_Packet_Is_Row     (Stream + I) == 0) {return 0;}
      if (Lepton_Packet_Is_Match   (Stream + I) == 0) {return 0;}
   }
   return 1;
}




#define Lepton_Stream_Invalid_Segment   (0)
#define Lepton_Stream_Discard           (-1)
#define Lepton_Stream_Mismatch          (-2)
#define Lepton_Stream_Invalid_Row       (-3)
#define Lepton_Stream_Shifting          (-4)
#define Lepton_Stream_SPI_Error         (-5)

//Assigns the segment pixel map to correct position in `Pixmap`.
//Automaticly shifting the stream when the stream has an offset.
//Returns `Lepton_Stream_Shifting` when the stream has an offset.
//Returns the segment number `1 .. 4` on succesful.
//Returns error code on error.
int Lepton_Stream_Accept (int Device, struct Lepton_Pixel_Grayscale16 * Pixmap)
{
   struct Lepton_Packet Stream [Lepton_Height];
   int Success;
   int Segment;
   Success = Lepton_SPI_Transfer_Stream (Stream, Lepton_Height, Device);
   if (!Success) {return Lepton_Stream_SPI_Error;}
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
   Segment = Stream [20].Reserved >> 4;
   if ((Segment <= 0) || (Segment > 4)) {return Lepton_Stream_Invalid_Segment;}
   for (size_t I = 0; I < Lepton_Height; I = I + 1)
   {
      Lepton_Conversions_Packet_To_Grayscale16 (Stream + I, Pixmap, Segment - 1);
   }
   return Segment;
}

