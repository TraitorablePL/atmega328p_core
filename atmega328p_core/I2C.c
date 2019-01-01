/*
 * I2C.c
 *
 * Created: 01.01.2019 02:50:20
 *  Author: Trait
 */ 
#include <util/twi.h>

#define I2C_BUSCLOCK 200000

#define I2C_STARTError 1
#define I2C_NoNACK 3
#define I2C_NoACK 4

#define ALTITUDE_SENS_ADDR 0xC0

uint8_t I2C_Error = 0;

void I2C_SetBusSpeed(){
	TWSR = TWSR & ~((1<<TWPS1)|(1<<TWPS0));
	TWBR = 72;
}

void I2C_Init(void){
	TWCR = (1<<TWEA) | (1<<TWEN);
	I2C_SetBusSpeed();
}

void I2C_Start(void){
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	I2C_WaitForComplete();
	if (TW_STATUS != TW_START) I2C_SetError(I2C_STARTError);
}

void I2C_WaitForComplete(void){
	while (!(TWCR & (1<<TWINT))){};
}

static inline void I2C_Stop(void){
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

inline void I2C_SetError(uint8_t error){
	I2C_Error = error;
}

void I2C_SendAddr(uint8_t address){
	uint8_t Status;
	if(0 == (address & 0x01)){
		Status = TW_MT_SLA_ACK;
	}
	TWDR = address;
	TWCR = (1<<TWINT) | (1<<TWEN);
	I2C_WaitForComplete();
	if (TW_STATUS != Status) I2C_SetError(I2C_NoNACK);
}

void I2C_SendStartAndSelect(uint8_t addr){
	I2C_Start();
	I2C_SendAddr(addr);
}

void I2C_SendByte(uint8_t byte){
	TWDR = byte;
	TWCR = (1<<TWINT) | (1<<TWEN);
	I2C_WaitForComplete();
	if(TW_STATUS!=TW_MT_DATA_ACK) I2C_SetError(I2C_NoACK);
}

uint8_t I2C_RecieveData_NACK(){
	TWCR = (1<<TWINT) | (1<<TWEN);
	I2C_WaitForComplete();
	if(TW_STATUS!=TW_MT_DATA_NACK) I2C_SetError(I2C_NoNACK);
	return TWDR;
}

uint8_t I2C_RecieveData_ACK(){
	TWCR = (1<<TWEA) | (1<<TWINT) | (1<<TWEN);
	I2C_WaitForComplete();
	if(TW_STATUS!=TW_MT_DATA_ACK) I2C_SetError(I2C_NoACK);
	return TWDR;
}

uint8_t AltitudeSens_ReadReg(uint8_t reg){
	I2C_SendStartAndSelect(ALTITUDE_SENS_ADDR | TW_WRITE);
	I2C_SendByte(reg);
	I2C_SendStartAndSelect(ALTITUDE_SENS_ADDR | TW_READ);
	uint8_t res = I2C_RecieveData_NACK();
	I2C_Stop();
	return res;
}

void AltitudeSens_WriteReg(uint8_t reg, uint8_t value){
	I2C_SendStartAndSelect(ALTITUDE_SENS_ADDR | TW_WRITE);
	I2C_SendByte(reg);
	I2C_SendByte(value);
	I2C_Stop();
}



