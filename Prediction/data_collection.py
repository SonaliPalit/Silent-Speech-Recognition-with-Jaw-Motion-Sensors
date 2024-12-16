import serial
import numpy as np
import os

SERIAL_PORT = '/dev/tty.usbserial-10'
BAUD_RATE = 115200
OUTPUT_DIR = 'collected_data'

os.makedirs(OUTPUT_DIR, exist_ok=True)

def read_imu_data(serial_port):
    buffer = []
    ser = serial.Serial(serial_port, BAUD_RATE, timeout=1)
    print("Waiting for data...")

    while True:
        try:
            line = ser.readline().decode('utf-8').strip()
            if line:
                try:
                    values = list(map(float, line.split(',')))

                    buffer.append(values)

                    if len(buffer) == 400:
                        yield buffer
                        buffer = []
                except ValueError:
                    print(f"Invalid data: {line}")
        except serial.SerialException as e:
            print(f"Serial error: {e}")
            break

def save_buffer_to_file(buffer, filename):
    filepath = os.path.join(OUTPUT_DIR, filename)
    header = "Time (ms), Acce_X, Acce_Y, Acce_Z, Gyro_X, Gyro_Y, Gyro_Z"
    formatted_buffer = np.array(buffer)
    formatted_buffer[:, 0] = np.arange(len(formatted_buffer))
    np.savetxt(filepath, formatted_buffer, delimiter=',', header=header, comments='', fmt='%.2f')
    print(f"Saved {filename}")

def collect_data_for_phrase(phrase, start_index=21, end_index=30):
    print(f"collecting data for the phrase: '{phrase}'")
    sample_count = 0

    for buffer in read_imu_data(SERIAL_PORT):
        filename = f"{phrase}_{start_index + sample_count}.txt"
        save_buffer_to_file(buffer, filename)
        sample_count += 1

        if sample_count > (end_index - start_index):
            print(f"completing collecting data for phrase: '{phrase}'")
            break

def main():
    phrases = ["help", "water", "call_nurse", "pain"]
    for phrase in phrases:
        collect_data_for_phrase(phrase)

if __name__ == '__main__':
    main()
