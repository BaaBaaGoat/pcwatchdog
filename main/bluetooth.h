void Init_Bluetooth(void);
void Bluetooth_Uplink_callback(unsigned char* data,int* len/*<600Bytes*/);
void Bluetooth_Downlink_callback(unsigned char* data,int len);
extern char BLEDeviceName[18];
