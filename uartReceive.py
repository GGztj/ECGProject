import threading
import serial
from threading import Lock,Thread
import time,os
import matplotlib.pyplot as plt
import serial.tools.list_ports

portx = "COM6" # 端口，GNU / Linux上的/ dev / ttyUSB0 等 或 Windows上的 COM3 等
bps = 9600 # 波特率，标准值之一：50,75,110,134,150,200,300,600,1200,1800,2400,4800,9600,19200,38400,57600,115200
timex = 5 # 超时设置,None：永远等待操作，0为立即返回请求结果，其他值为等待超时时间(单位为秒）

rxflag = True #是否接收数据
ECGdata = []
ax = []  # 定义一个 x 轴的空列表用来接收动态的数据
t = 0
ay = []  # 定义一个 y 轴的空列表用来接收动态的数据


class Drawing (threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name

    def run(self):
        print ("开始线程：" + self.name)


        plt.ion()  # 开启一个画图的窗口

        # plt.rcParams['savefig.dpi'] = 200 #图片像素
        # plt.rcParams['figure.dpi'] = 200 #分辨率
        plt.rcParams['figure.figsize'] = (10, 10)  # 图像显示大小
        plt.rcParams['font.sans-serif'] = ['SimHei']  # 防止中文标签乱码，还有通过导入字体文件的方法
        plt.rcParams['axes.unicode_minus'] = False
        plt.rcParams['lines.linewidth'] = 0.5  # 设置曲线线条宽度
        while True:
            plt.clf()  # 清除刷新前的图表，防止数据量过大消耗内存
            plt.suptitle("总标题", fontsize=30)  # 添加总标题，并设置文字大小
            ax.append(t)  # 追加x坐标值
            t = t+1 #后续应改为AD采集的速率
            ay.append(g1)  # 追加y坐标值
            agraphic = plt.subplot(2, 1, 1)
            agraphic.set_title('子图表标题1')  # 添加子标题
            agraphic.set_xlabel('x轴', fontsize=10)  # 添加轴标签
            agraphic.set_ylabel('y轴', fontsize=20)
            plt.plot(ax, ay, 'g-')  # 等于agraghic.plot(ax,ay,'g-')
            plt.pause(0.4)  # 设置暂停时间，太快图表无法正常显示
            #if num == 15:
            #    plt.savefig('picture.png', dpi=300)  # 设置保存图片的分辨率
            #    # break
            #num = num + 1

        plt.ioff()  # 关闭画图的窗口，即关闭交互模式
        plt.show()  # 显示图片，防止闪退




        print ("退出线程：" + self.name)

class Receiving (threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name

    def run(self):
        print ("开始线程：" + self.name)
        # 检查端口
        port_list = list(serial.tools.list_ports.comports())
        print(port_list)
        if len(port_list) == 0:
            print('无可用串口')
        else:
            for i in range(0, len(port_list)):
                print(port_list[i])

        # 开始读取
        try:

            ser = serial.Serial(portx, bps, timeout=timex)

            while (rxflag):
                ECGdata.append(ser.read(100))

            ser.close()  # 关闭串口

        except Exception as e:
            print("---异常---：", e)

        print ("退出线程：" + self.name)



if __name__=="__main__":

    thread1 = Receiving(1, "Thread-1")
    thread1.start()
    thread1.join()
    print("退出主线程")
