import threading
import serial
from threading import Lock,Thread
import time,os
import matplotlib.pyplot as plt
import serial.tools.list_ports
# 端口，GNU / Linux上的/ dev / ttyUSB0 等 或 Windows上的 COM3 等
portx = "COM6"
# 波特率，标准值之一：50,75,110,134,150,200,300,600,1200,1800,2400,4800,9600,19200,38400,57600,115200
bps = 9600
# 超时设置,None：永远等待操作，0为立即返回请求结果，其他值为等待超时时间(单位为秒）
timex = 5
# 打开串口，并得到串口对象
rxflag = True #是否接收数据
ser = serial.Serial(portx, bps, timeout=timex)

while (rxflag):
    while(1):
        print(ser.read(80))

ser.close()  # 关闭串口
