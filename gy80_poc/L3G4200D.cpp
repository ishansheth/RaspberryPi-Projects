#include "L3G4200D.h"
#include <stdio.h>
#include<iostream>

L3G4200D::L3G4200D(){
  if(!L3G4200D::init(250))
    printf("L3G4200D initialization failed!!");
}

L3G4200D::L3G4200D(int scale){
  if(!L3G4200D::init(scale))
    printf("L3G4200D initialization failed!!");
}

bool L3G4200D::init(int scale){
  fd = wiringPiI2CSetup(L3G4200D_ADDR);
  if(fd == -1)
    return false;

  wiringPiI2CWriteReg8(fd,CTRL_REG1,0b10001111);

  if(scale == 250)
    wiringPiI2CWriteReg8(fd,CTRL_REG4,0b10000000);
  else if(scale == 500)
    wiringPiI2CWriteReg8(fd,CTRL_REG4,0b00010000);
  else
    wiringPiI2CWriteReg8(fd,CTRL_REG4,0b00110000);

  return true;
}

void L3G4200D::update(){
  unsigned int statusRegBit = wiringPiI2CReadReg8(fd,STATUS_REG);
  //  std::cout<<"status reg val:"<<std::hex<<statusRegBit<<std::endl;
  
  uint8_t X1 = wiringPiI2CReadReg8(fd,L3G4200D_REG_XM);
  uint8_t X0 = wiringPiI2CReadReg8(fd,L3G4200D_REG_XL);
  int16_t X = (int16_t)X1 << 8 | (int16_t)X0;

  gyro_x = (float)X / 32768.0f;
  gyro_x *= 2000.0f;


  uint8_t Y1 = wiringPiI2CReadReg8(fd,L3G4200D_REG_YM);
  uint8_t Y0 = wiringPiI2CReadReg8(fd,L3G4200D_REG_YL);
  int16_t Y = (int16_t)Y1 << 8 | (int16_t)Y0;

  gyro_y = (float)Y / 32768.0f;
  gyro_y *= 2000.0f;

  uint8_t Z1 = wiringPiI2CReadReg8(fd,L3G4200D_REG_ZM);
  uint8_t Z0 = wiringPiI2CReadReg8(fd,L3G4200D_REG_ZL);
  int16_t Z = (int16_t)Z1 << 8 | (int16_t)Z0;

  gyro_z = (float)Z / 32768.0f;
  gyro_z *= 2000.0f;

}
