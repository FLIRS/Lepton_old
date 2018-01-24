#include "../Lepton_I2C.h"
#include "../Lepton_SPI.h"
#include <assert.h>


int main (int argc, char * argv [])
{
   assert (argc == 1);
   assert (argv [0] != NULL);
   
   int SPI_Device;
   int I2C_Device;
   
   SPI_Device = Lepton_SPI_Open ("/dev/spidev0.0");
   I2C_Device = Lepton_I2C_Open ("/dev/i2c-1");
   
}
