import serial
import numpy as np
import joblib

mlp_classifier = joblib.load('model.pkl')

SERIAL_PORT = '/dev/tty.usbserial-10'
BAUD_RATE = 115200
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
                    print(f"{line}")
        except serial.SerialException as e:
            print(f"Serial error: {e}")
            break

def normalize_data(data):
    data = np.array(data)
    data_min = data.min(axis=0)
    data_max = data.max(axis=0)
    data_range = data_max - data_min
    data_range[data_range == 0] = 1
    return (data - data_min) / data_range

def prepare_data(buffer):
    return buffer.flatten()

def predict_gesture(buffer):
    data = np.array(buffer)
    normalized_data = normalize_data(data)
    prepared_data = prepare_data(normalized_data)
    gesture = mlp_classifier.predict([prepared_data])
    return gesture[0]

def main():
    for buffer in read_imu_data(SERIAL_PORT):
        gesture = predict_gesture(buffer)
        print(f"Predicted Phrase: {gesture}")

if __name__ == '__main__':
    main()
