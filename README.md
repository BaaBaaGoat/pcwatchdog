# 硬件

https://item.taobao.com/item.htm?id=660959328146

AC90-250V单路

esp下好固件后,拔掉编程器,电脑电源线剪断,插头一侧火线接板子L,零线接板子N和电脑一侧零线,板子L接COM,板子NC接电脑一侧火线,NO空着.市电和编程器不要一起接

注意用电安全,电到人概不负责



# 通信协议

 * 蓝牙(BLE)和mqtt均可向板子发送数据
 * 数据为json格式 mqtt topic为/pcwatchdog
 * ```{"Cmd":"config","MQTTIP":"192.168.1.1","MQTTPort":1883,"MQTTAccount":"","MQTTPass":"","SSID":"eulalawrence","WifiPass":"testonly","WatchDogInterval_sec":900}``` 配置参数
 * ```{"Cmd":"reboot"} ```立马重启
 * ```{"Cmd":"feed"} ```喂狗
