#include "common.h"
#include "config.h"
#include "driver/i2c.h"
#include "myi2c.h"

esp_err_t i2c_WriteRegister(i2c_port_t i2c_num, uint8_t deviceAddr, uint8_t regAddr, uint8_t regData) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, deviceAddr << 1, I2C_ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regAddr, I2C_ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regData, I2C_ACK_CHECK_EN);
    i2c_master_stop(cmd);
    int ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_FAIL) {
        return ret;
		}
    return ESP_OK;
	}


esp_err_t i2c_WriteCommands(i2c_port_t i2c_num, uint8_t deviceAddr, uint8_t* pCmdBuffer, int numBytes) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, deviceAddr << 1, I2C_ACK_CHECK_EN);
	for (int n = 0; n < numBytes; n++) {
		i2c_master_write_byte(cmd, pCmdBuffer[n], I2C_ACK_CHECK_EN);
		}
    i2c_master_stop(cmd);
    int ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_FAIL) {
        return ret;
		}
    return ESP_OK;
	}

	
esp_err_t i2c_ReadRegister(i2c_port_t i2c_num, uint8_t deviceAddr, uint8_t regAddr, uint8_t* pRegData) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, deviceAddr << 1, I2C_ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regAddr, I2C_ACK_CHECK_EN);
    i2c_master_stop(cmd);
    int ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
	if (ret == ESP_FAIL) {
		return ret;
		}
	
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (deviceAddr << 1) | 1, I2C_ACK_CHECK_EN);
    i2c_master_read_byte(cmd, pRegData, I2C_NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_FAIL) {
        return ESP_FAIL;
		}
    return ESP_OK;
    }

	
esp_err_t i2c_ReadBuffer(i2c_port_t i2c_num, uint8_t deviceAddr, uint8_t regAddr, uint8_t* pBuffer, int numBytes) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, deviceAddr << 1, I2C_ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regAddr, I2C_ACK_CHECK_EN);
    i2c_master_stop(cmd);
    int ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
	if (ret == ESP_FAIL) {
		return ret;
		}
	
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (deviceAddr << 1) | 1, I2C_ACK_CHECK_EN);
	for (int n = 0; n < numBytes-1; n++) {
		i2c_master_read_byte(cmd, &pBuffer[n], I2C_ACK_VAL);
		}
	i2c_master_read_byte(cmd, &pBuffer[numBytes-1], I2C_NACK_VAL);
		
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_FAIL) {
        return ESP_FAIL;
		}
    return ESP_OK;
    }
	