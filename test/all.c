//gcc test_vsync.c -std=gnu11 -fdiagnostics-color -Wall -Wno-missing-braces -lwiringPi && ./a.out

#include "../Lepton_SPI.h"
#include "../Lepton_I2C.h"
#include "../Lepton_Packets.h"
#include "../Lepton_Stream.h"
#include <assert.h>

int main (int argc, char * argv [])
{ 
   assert (argc == 1);
   assert (argv [0] != NULL);
   return 0;
}



