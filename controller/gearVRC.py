#!/usr/bin/python3

# The library is free.
# MIT License
# Copyright (c) 2019, Robert K. Dady
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# Prerequisite:
# $ pip3 install python-uinput pygatt gatt more-itertools cmath --user
# # modprobe uinput

import gatt
import uinput
import math
import time
import numpy as np
import sys
import os
import cmath
import more_itertools as mit

manager = gatt.DeviceManager(adapter_name='hci0')

def ror(l, n):
    return l[-n:] + l[:-n]

middle_counter = 0
up_counter = 0
down_counter = 0
left_counter = 0
right_counter = 0

class AnyDevice(gatt.Device):
    def connect_succeeded(self):
        super().connect_succeeded()
        #print("[%s] Connected" % (self.mac_address))

    def connect_failed(self, error):
        super().connect_failed(error)
        #print("[%s] Connection failed: %s" % (self.mac_address, str(error)))

    def disconnect_succeeded(self):
        super().disconnect_succeeded()
        #print("[%s] Disconnected" % (self.mac_address))
        device.connect()
        #sys.exit(0)

    def write(self, cmd, times):
        for i in range(times - 1):
            self.__setup_characteristic.write_value(cmd)

    def services_resolved(self):
        super().services_resolved()

        controller_data_service = next(
            s for s in self.services
            if s.uuid == '4f63756c-7573-2054-6872-65656d6f7465')

        controller_setup_data_characteristic = next(
            c for c in controller_data_service.characteristics
            if c.uuid == 'c8c51726-81bc-483b-a052-f7a14ea3d282')

        controller_data_characteristic = next(
            c for c in controller_data_service.characteristics
            if c.uuid == 'c8c51726-81bc-483b-a052-f7a14ea3d281')

        self.__setup_characteristic = controller_setup_data_characteristic
        self.__sensor_characteristic = controller_data_characteristic

        self.write(bytearray(b'\x01\x00'), 3)
        self.write(bytearray(b'\x06\x00'), 1)
        self.write(bytearray(b'\x07\x00'), 1)
        self.write(bytearray(b'\x08\x00'), 3)

        self.__max = 315
        self.__r = self.__max / 2
        self.__axisX = self.__axisY = 0
        self.__altX = self.__altY = 0

        self.__reset = self.__volbtn = self.__tchbtn = self.__trig = True

        self.__time = round(time.time()) + 10
        self.__lastupdated = 0
        self.__updatecounts = 0

        self.__dirDown = False
        self.__VR = False

        controller_data_characteristic.enable_notifications()

    def keepalive(self):
        # test time and each minute send a keepalive
        if (time.time() > self.__time):
            self.__time = round(time.time()) + 10
            cmd = bytearray(b'\x04\x00')
            for i in range(4):
                self.__setup_characteristic.write_value(cmd)

    def characteristic_value_updated(self, characteristic, value):
        if (characteristic == self.__sensor_characteristic):
            if self.__VR == False:
                self.__updatecounts += 1
                if self.__updatecounts == 20:
                    now = time.time()
                    self.__updatecounts = 0
                    deltatime = now - self.__lastupdated
                    self.__lastupdated = now
                    if deltatime > 0.23:
                        self.write(bytearray(b'\x06\x00'), 1)
                        self.write(bytearray(b'\x08\x00'), 3)
                        self.write(bytearray(b'\x07\x00'), 1)
                    else:
                        self.__VR = True

            self.keepalive()
            int_values = [x for x in value]
            if (len(int_values) < 60):
                self.__VR = True
                #print("VR mode is activated")
                self.write(bytearray(b'\x01\x00'), 3)
                self.__sensor_characteristic.enable_notifications()
                return

            axisX   = (((int_values[54] & 0xF)  << 6) + ((int_values[55] & 0xFC) >> 2)) & 0x3FF
            axisY   = (((int_values[55] & 0x3)  << 8) + ((int_values[56] & 0xFF) >> 0)) & 0x3FF
            accelX  = np.uint16((int_values[4]  << 8) + int_values[5])  * 10000.0 * 9.80665 / 2048.0
            accelY  = np.uint16((int_values[6]  << 8) + int_values[7])  * 10000.0 * 9.80665 / 2048.0
            accelZ  = np.uint16((int_values[8]  << 8) + int_values[9])  * 10000.0 * 9.80665 / 2048.0
            gyroX   = np.uint16((int_values[10] << 8) + int_values[11]) * 10000.0 * 0.017453292 / 14.285
            gyroY   = np.uint16((int_values[12] << 8) + int_values[13]) * 10000.0 * 0.017453292 / 14.285
            gyroZ   = np.uint16((int_values[14] << 8) + int_values[15]) * 10000.0 * 0.017453292 / 14.285
            magnetX = np.uint16((int_values[32] << 8) + int_values[33]) * 0.06
            magnetY = np.uint16((int_values[34] << 8) + int_values[35]) * 0.06
            magnetZ = np.uint16((int_values[36] << 8) + int_values[37]) * 0.06

            triggerButton    = True if ((int_values[58] &  1) ==  1) else False
            homeButton       = True if ((int_values[58] &  2) ==  2) else False
            backButton       = True if ((int_values[58] &  4) ==  4) else False
            touchpadButton   = True if ((int_values[58] &  8) ==  8) else False
            volumeUpButton   = True if ((int_values[58] & 16) == 16) else False
            volumeDownButton = True if ((int_values[58] & 32) == 32) else False
            NoButton         = True if ((int_values[58] & 64) == 64) else False
            
           
            if (axisX > 310):
                axisX = 310
            if (axisY > 310):
                axisY = 310
            global middle_counter
            global up_counter
            global down_counter
            global left_counter
            global right_counter
                        
            threshold = 2
            if (touchpadButton):
                if (axisX != 0 or axisY != 0):
                    axisX = axisX - 155
                    axisY = axisY - 155
                    
                    angle = math.atan2(axisX,axisY)
                    magnitude = axisX* axisX + axisY * axisY
                    
                    if(magnitude < 7000):
                        middle_counter = middle_counter + 1
                        up_counter = 0
                        down_counter = 0
                        left_counter = 0
                        right_counter = 0
                    else:
                        if (angle > -math.pi* 1/4  and angle < math.pi * 1/4):
                            middle_counter = 0
                            up_counter = 0
                            down_counter = down_counter + 1
                            left_counter = 0
                            right_counter = 0
                        elif (angle <= -math.pi* 1/4 and angle >= -math.pi * 3/4):
                            middle_counter = 0
                            up_counter = 0
                            down_counter = 0
                            left_counter = left_counter + 1
                            right_counter = 0
                        elif (angle >= math.pi* 1/4 and angle <= math.pi * 3/4):
                            middle_counter = 0
                            up_counter = 0
                            down_counter = 0
                            left_counter = 0
                            right_counter = right_counter + 1
                        else:
                            middle_counter = 0
                            up_counter = up_counter +1
                            down_counter = 0
                            left_counter = 0
                            right_counter = 0
            else:
                middle_counter = 0
                up_counter = 0
                down_counter = 0
                left_counter = 0
                right_counter = 0
                
                
            if (middle_counter > threshold):
                print("middle")
            if (up_counter > threshold):
                print("up")               
            if (left_counter > threshold):
                print("left")
            if (right_counter > threshold):
                print("right")   
            if (down_counter > threshold):
                print("down")   

                
def defint():
    global device
    device.write(bytearray(b'\x00\x00'), 3)
    #device.connect()
   
    ##sys.exit(0)



device = AnyDevice(mac_address='2C:BA:BA:4F:9B:30', manager=manager)
device.connect()

manager.run()
