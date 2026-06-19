# Edge Impulse - OpenMV Object Detection Example

import sensor, image, time, os, tf, math, uos, gc

sensor.reset()                         # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565)    # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)      # Set frame size to QVGA (320x240)
sensor.set_windowing((240, 240))       # Set 240x240 window.
sensor.skip_frames(time=2000)          # Let the camera adjust.

net = None
labels = None
min_confidence = 0.5
from pyb import UART  # 模块导入
uart = UART(3, 115200)  # 串口配置
uart.init(115200, bits=8, parity=None, stop=1)

try:
    # load the model, alloc the model file on the heap if we have at least 64K free after loading
    net = tf.load("trained.tflite", load_to_fb=uos.stat('trained.tflite')[6] > (gc.mem_free() - (64*1024)))
except Exception as e:
    raise Exception('Failed to load "trained.tflite", did you copy the .tflite and labels.txt file onto the mass-storage device? (' + str(e) + ')')

try:
    labels = [line.rstrip('\n') for line in open("labels.txt")]
except Exception as e:
    raise Exception('Failed to load "labels.txt", did you copy the .tflite and labels.txt file onto the mass-storage device? (' + str(e) + ')')

colors = [ # Add more colors if you are detecting more than 7 types of classes at once.
    (255,   0,   0),
    (  0, 255,   0),
    (255, 255,   0),
    (  0,   0, 255),
    (255,   0, 255),
    (  0, 255, 255),
    (255, 255, 255),
]

clock = time.clock()
K1 = 500   #每次改变物体，K值都要重新计算 K1 = 距离（25cm已知）*直径像素Lm（20)
K2 = 0.16  #实际距离 = k * 直径的像素,K2 = 实际大小（宽4cm已知）/宽像素b[2]（25）
K3 = 0.16  #实际距离 = k * 直径的像素,K3 = 实际大小（长4cm已知）/长像素b[3]（25）
while(True):
    clock.tick()

    img = sensor.snapshot()

    # detect() returns all objects found in the image (splitted out per class already)
    # we skip class index 0, as that is the background, and then draw circles of the center
    # of our objects

    for i, detection_list in enumerate(net.detect(img, thresholds=[(math.ceil(min_confidence * 255), 255)])):
        if (i == 0): continue # background class
        if (len(detection_list) == 0): continue # no detections for this class?

        print("********** %s **********" % labels[i])
        center_x=0
        center_y=0
        for d in detection_list:
            [x, y, w, h] = d.rect()
            center_x = math.floor(x + (w / 2))
            center_y = math.floor(y + (h / 2))
            Lm = ( math.floor(w+h))/2 #色块的长与宽 直径像素点个数
            length = K1 / Lm
            print('x %d\ty %d' % (center_x, center_y))
            img.draw_rectangle(d.rect(), color=(255,0,0))# 物体的框框.
            img.draw_cross((center_x, center_y), color=(0, 255, 0))
            data = bytearray([0xb3, 0xb3, center_x, center_y, int(length), 0x0d, 0x0a])  # 要发送的数据
            uart.write(data)  # 数据发送
            print(data, end="\n\n")
    print(clock.fps(), "fps", end="\n\n")
