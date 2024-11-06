
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

static const char *TAG = "mpu6050 test";
static mpu6050_handle_t mpu6050 = NULL;

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

void app_main()
{
    esp_err_t ret;
    uint8_t mpu6050_deviceid;
    mpu6050_acce_value_t acce;
    mpu6050_gyro_value_t gyro;
    mpu6050_temp_value_t temp;

    i2c_sensor_mpu6050_init();

    typedef struct {
        char phrase[20];
        int duration;
    } Phrase;

    Phrase phrases[] = {
        {"Yes", 2},
        {"No", 2},
        {"Help", 2},
        {"Stop", 2},
        {"Wait", 2},
        {"Call_911", 4},
        {"I_need_a_doctor", 6}
    };

    #define NUM_PHRASES (sizeof(phrases) / sizeof(phrases[0]))

    for (int i = 0; i < NUM_PHRASES; i++) {

        // Wait for a pre-configured delay before starting data collection for the phrase
        int delay_ms = 8000;  // 5-second delay
        ESP_LOGI(TAG, "Phrase: %s, collecting data in %d seconds...", phrases[i].phrase, delay_ms / 1000);
        vTaskDelay(delay_ms / portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "Phrase: %s, Collecting data for %d seconds", phrases[i].phrase, phrases[i].duration);

        printf("Time (ms), Acce_X, Acce_Y, Acce_Z, Gyro_X, Gyro_Y, Gyro_Z\n");
        int num_iterations = (phrases[i].duration * 100);  // 100 iterations per second for specified duration
        for (int i = 0; i < num_iterations; i++) {
            mpu6050_acce_value_t acce;
            mpu6050_gyro_value_t gyro;
            float temp;
            esp_err_t ret;

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

            ret = mpu6050_get_temp(mpu6050, &temp);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to get temperature data");
                return;
            }

            printf("%d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",
                    i, acce.acce_x, acce.acce_y, acce.acce_z,
                    gyro.gyro_x, gyro.gyro_y, gyro.gyro_z);

            vTaskDelay(10 / portTICK_PERIOD_MS);  // 10 ms delay between samples
        }
    }

    mpu6050_delete(mpu6050);
    ret = i2c_driver_delete(I2C_MASTER_NUM);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to delete I2C driver");
    }
}



// #include <stdio.h>
// #include "unity.h"
// #include "driver/i2c.h"
// #include "mpu6050.h"
// #include "esp_system.h"
// #include "esp_log.h"

// #define I2C_MASTER_SCL_IO 1      /*!< gpio number for I2C master clock */
// #define I2C_MASTER_SDA_IO 0      /*!< gpio number for I2C master data  */
// #define I2C_MASTER_NUM I2C_NUM_0  /*!< I2C port number for master dev */
// #define I2C_MASTER_FREQ_HZ 100000 /*!< I2C master clock frequency */

// static const char *TAG = "mpu6050 test";
// static mpu6050_handle_t mpu6050 = NULL;

// /**
//  * @brief i2c master initialization
//  */
// static void i2c_bus_init(void)
// {
//     i2c_config_t conf;
//     conf.mode = I2C_MODE_MASTER;
//     conf.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO;
//     conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
//     conf.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO;
//     conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
//     conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
//     conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

//     esp_err_t ret = i2c_param_config(I2C_MASTER_NUM, &conf);
//     if (ret != ESP_OK)
//     {
//         ESP_LOGE(TAG, "I2C config returned error");
//         return;
//     }

//     ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
//     if (ret != ESP_OK)
//     {
//         ESP_LOGE(TAG, "I2C install returned error");
//         return;
//     }
// }

// /**
//  * @brief i2c master initialization
//  */
// static void i2c_sensor_mpu6050_init(void)
// {
//     esp_err_t ret;

//     i2c_bus_init();
//     mpu6050 = mpu6050_create(I2C_MASTER_NUM, MPU6050_I2C_ADDRESS);
//     if (mpu6050 == NULL)
//     {
//         ESP_LOGE(TAG, "MPU6050 create returned NULL");
//         return;
//     }

//     ret = mpu6050_config(mpu6050, ACCE_FS_4G, GYRO_FS_500DPS);
//     if (ret != ESP_OK)
//     {
//         ESP_LOGE(TAG, "MPU6050 config error");
//         return;
//     }

//     ret = mpu6050_wake_up(mpu6050);
//     if (ret != ESP_OK)
//     {
//         ESP_LOGE(TAG, "MPU6050 wake up error");
//         return;
//     }
// }

// void app_main()
// {
//     esp_err_t ret;
//     uint8_t mpu6050_deviceid;
//     mpu6050_acce_value_t acce;
//     mpu6050_gyro_value_t gyro;
//     mpu6050_temp_value_t temp;

//     i2c_sensor_mpu6050_init();

//     printf("Time (ms), Acce_X, Acce_Y, Acce_Z, Gyro_X, Gyro_Y, Gyro_Z\n");

//     for (int i = 0; i < 100 * 4; i++)
//     {
//         ret = mpu6050_get_acce(mpu6050, &acce);
//         if (ret != ESP_OK)
//         {
//             ESP_LOGE(TAG, "Failed to get accelerometer data");
//             return;
//         }

//         ret = mpu6050_get_gyro(mpu6050, &gyro);
//         if (ret != ESP_OK)
//         {
//             ESP_LOGE(TAG, "Failed to get gyroscope data");
//             return;
//         }

//         ret = mpu6050_get_temp(mpu6050, &temp);
//         if (ret != ESP_OK)
//         {
//             ESP_LOGE(TAG, "Failed to get temperature data");
//             return;
//         }

//        printf("%d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n",
//                i,
//                acce.acce_x, acce.acce_y, acce.acce_z,
//                gyro.gyro_x, gyro.gyro_y, gyro.gyro_z);

//         vTaskDelay(10 / portTICK_PERIOD_MS); 
//     }

//     mpu6050_delete(mpu6050);
//     ret = i2c_driver_delete(I2C_MASTER_NUM);
//     if (ret != ESP_OK)
//     {
//         ESP_LOGE(TAG, "Failed to delete I2C driver");
//     }
// }