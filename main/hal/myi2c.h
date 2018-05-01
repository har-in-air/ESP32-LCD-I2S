#ifndef MYI2C_H_
#define MYI2C_H_


#define I2C_ACK_CHECK_EN   	0x1     // I2C master will check ack from slave
#define I2C_ACK_CHECK_DIS  	0x0     // I2C master will not check ack from slave 
#define I2C_ACK_VAL    		0x0          
#define I2C_NACK_VAL   		0x1    

esp_err_t i2c_WriteRegister(i2c_port_t i2c_num, uint8_t deviceAddr, uint8_t regAddr, uint8_t regData);
esp_err_t i2c_WriteCommands(i2c_port_t i2c_num, uint8_t deviceAddr, uint8_t* pCmdBuffer, int numBytes);

esp_err_t i2c_ReadRegister(i2c_port_t i2c_num, uint8_t deviceAddr, uint8_t regAddr, uint8_t* pRegData);
esp_err_t i2c_ReadBuffer(i2c_port_t i2c_num, uint8_t deviceAddr, uint8_t regAddr, uint8_t* pBuffer, int numBytes);

#endif
