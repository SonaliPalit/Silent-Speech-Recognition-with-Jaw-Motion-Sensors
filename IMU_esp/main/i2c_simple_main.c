#include <stdio.h>
#include "unity.h"
#include "driver/i2c.h"
#include "mpu6050.h"
#include "esp_system.h"
#include "esp_log.h"

#define I2C_MASTER_SCL_IO 1      /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 0      /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0  /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000 /*!< I2C master clock frequency */

#define BUFFER_SIZE 400

static const char *TAG = "mpu6050 test";
static mpu6050_handle_t mpu6050 = NULL;

typedef struct {
    float acce_x, acce_y, acce_z;
    float gyro_x, gyro_y, gyro_z;
} imu_data_t;

imu_data_t data_buffer[BUFFER_SIZE];

/**
 * @brief i2c master initialization
 */
static void i2c_bus_init(void)
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

    esp_err_t ret = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C config returned error");
        return;
    }

    ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C install returned error");
        return;
    }
}

/**
 * @brief i2c master initialization
 */
static void i2c_sensor_mpu6050_init(void)
{
    esp_err_t ret;

    i2c_bus_init();
    mpu6050 = mpu6050_create(I2C_MASTER_NUM, MPU6050_I2C_ADDRESS);
    if (mpu6050 == NULL)
    {
        ESP_LOGE(TAG, "MPU6050 create returned NULL");
        return;
    }

    ret = mpu6050_config(mpu6050, ACCE_FS_4G, GYRO_FS_500DPS);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "MPU6050 config error");
        return;
    }

    ret = mpu6050_wake_up(mpu6050);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "MPU6050 wake up error");
        return;
    }
}

void app_main() {
    esp_err_t ret;
    mpu6050_acce_value_t acce;
    mpu6050_gyro_value_t gyro;

    i2c_sensor_mpu6050_init();

    while (1) {
        ESP_LOGI(TAG, "Collecting data for 4 seconds...");
        for (int i = 0; i < BUFFER_SIZE; i++) {
            ret = mpu6050_get_acce(mpu6050, &acce);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to get accelerometer data");
                return;
            }

            ret = mpu6050_get_gyro(mpu6050, &gyro);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to get gyroscope data");
                return;
            }

            data_buffer[i].acce_x = acce.acce_x;
            data_buffer[i].acce_y = acce.acce_y;
            data_buffer[i].acce_z = acce.acce_z;
            data_buffer[i].gyro_x = gyro.gyro_x;
            data_buffer[i].gyro_y = gyro.gyro_y;
            data_buffer[i].gyro_z = gyro.gyro_z;

            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        ESP_LOGI(TAG, "Data collection complete. Sending data...");
        for (int i = 0; i < BUFFER_SIZE; i++) {
            printf("%.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",
                   data_buffer[i].acce_x, data_buffer[i].acce_y, data_buffer[i].acce_z,
                   data_buffer[i].gyro_x, data_buffer[i].gyro_y, data_buffer[i].gyro_z);
        }

        ESP_LOGI(TAG, "Data sent. Waiting before next collection...");
        vTaskDelay(5000 / portTICK_PERIOD_MS);  //5-second delay between collections of data
    }

    mpu6050_delete(mpu6050);
    ret = i2c_driver_delete(I2C_MASTER_NUM);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to delete I2C driver");
    }
}