#include "ADXL345.h"
#include <iostream>

ADXL345::ADXL345(){
  init();
  gForce = false;
}

ADXL345::ADXL345(bool enable_gforce){
  gForce = enable_gforce;
  init();
}

void ADXL345::init(){
  fd = wiringPiI2CSetup(ADXL345_Addr);

  wiringPiI2CWriteReg8(fd,BW_RATE,BW_RATE_100HZ);

  int dataformat = wiringPiI2CReadReg8(fd,DATA_FORMAT);

  dataformat &= ~0x0F;
  dataformat |= RANGE_2G;
  dataformat |= 0x08;

  wiringPiI2CWriteReg8(fd,DATA_FORMAT,dataformat);

  wiringPiI2CWriteReg8(fd,POWER_CTL,MEASURE);
}

void ADXL345::update(){
  int x = wiringPiI2CReadReg16(fd,Reg_X0);
  if(x & (1 << 16 - 1))
    x = x - (1<<16);

  int y = wiringPiI2CReadReg16(fd,Reg_Y0);
  if(y & (1 << 16 - 1))
    y = y - (1<<16);

  int z = wiringPiI2CReadReg16(fd,Reg_Z0);
  if(z & (1 << 16 - 1))
    z = z - (1<<16);

  acc_x = x * SCALE_MULTIPLIER;
  acc_y = y * SCALE_MULTIPLIER;
  acc_z = z * SCALE_MULTIPLIER;

  if(!gForce){
    acc_x *= EARTH_GRAVITY_MS2;
    acc_y *= EARTH_GRAVITY_MS2;
    acc_z *= EARTH_GRAVITY_MS2;
  }

}
