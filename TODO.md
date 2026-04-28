# TODO for MindDrive: Brain-Controlled Car with LED Demo Option

## Tasks
- [x] Create brain_esp32_sender.ino - Reads EEG from GPIO34 and sends to serial
- [x] Create car_esp32_receiver.ino - Wi-Fi TCP server on port 8080, receives commands, controls TB6612FNG motors
- [x] Create run_car_realtime.py - Reads EEG from serial, processes, predicts with car_model.joblib, sends Wi-Fi command to car ESP32
- [x] Create train_car_model.py - Trains Random Forest on eeg_car_dataset.npz for 5 commands, saves car_model.joblib
- [x] Update brain_esp32_sender.ino to include LED control for demo mode
- [x] Update run_car_realtime.py to add --mode flag for 'car' or 'led' demonstration
