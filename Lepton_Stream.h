#pragma once

//Using:
//Lepton_Packet_Is_Discard
//Lepton_Packet_Is_Row
//Lepton_Packet_Is_Match
#include "Lepton_Packets.h"

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
