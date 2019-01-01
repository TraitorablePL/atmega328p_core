/*
 * I2C.h
 *
 * Created: 01.01.2019 02:48:55
 *  Author: Trait
 */ 


#ifndef I2C_H_
#define I2C_H_

void I2C_Init(void);
void I2C_Start(void);

void I2C_SetBusSpeed();
void I2C_SendAddr(uint8_t address);
void I2C_SendStartAndSelect(uint8_t addr);
void I2C_SendByte(uint8_t byte);

static inline void I2C_WaitForComplete(void);
static inline void I2C_Stop(void);
static inline void I2C_WaitTillStopWasSent(void);

inline void I2C_SetError(uint8_t error);

uint8_t I2C_RecieveData_NACK(void);
uint8_t I2C_RecieveData_ACK(void);

void AltitudeSens_WriteReg(uint8_t reg, uint8_t value);
uint8_t AltitudeSens_ReadReg(uint8_t reg);

#endif /* I2C_H_ */