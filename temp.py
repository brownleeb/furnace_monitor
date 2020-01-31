#!/usr/bin/python2
import serial, time, glob, os, datetime,sys
#uncomment next line for exception handling
#import serial.serialutil
import syslog
import traceback

loop_delay=5.0

base_dir = '/sys/bus/w1/devices/'
device_folder = glob.glob(base_dir + '28*')[0]
device_file = device_folder + '/w1_slave'

valid_modes=[0,2,4,5,6,7,8,10]
ut=['off','one','Fan','three','HeatPump','Cool','HeatPump','Cool','Gas','nine','Gas','eleven','twelve','thirteen','fourteen','fifteen']

def read_temp_raw():
    f = open(device_file, 'r')
    lines = f.readlines()
    f.close()
    return lines

def read_temp():
    lines = read_temp_raw()
    while lines[0].strip()[-3:] != 'YES':
        time.sleep(0.2)
        lines = read_temp_raw()
    equals_pos = lines[1].find('t=')
    if equals_pos != -1:
        temp_string = lines[1][equals_pos+2:]
        temp_c = float(temp_string) / 1000.0
        temp_f = temp_c * 9.0 / 5.0 + 32.0
        return temp_f

delay_ct=0
prev_mode=0
system_on=False
syslog.syslog("PyTemp - Initializing...")
ser=serial.Serial('/dev/serial/by-id/usb-Arduino_LLC_Arduino_Leonardo-if00', 9600)
if os.path.exists('/run/hvac/fan'):
   ser.write('F')
   fan_on=True
else:
   fan_on=False
while 1:
   time.sleep(2)
   ser.flushInput()
   while 1:
         ser.write('P')
         temp=ser.readline()
#         print temp
         atemp=temp.strip().split(',')
#           format is garage,supply,return
#            print "Garage "+atemp[0]+"\n"
#            print "Supply "+atemp[1]+"\n"
#            print "Return "+atemp[2]+"\n"
#    furnace monitor:   gas,heat,fan,rev-valve
#        rev valve stays on until heat calls
#      Cool:  0111
#      Heat:  0110
#       Fan:  0010
#       Gas:  1000
         res=int(atemp[3])
         with open('/run/garage','w') as myfile:
            myfile.write(atemp[0][:-1]+'.'+atemp[0][-1])
         with open('/run/supply_temp','w') as myfile:
            myfile.write(str((int(atemp[1])*.0140625)+32))
         with open('/run/return_temp','w') as myfile:
            myfile.write(str((int(atemp[2])*.0140625)+32))
         if not fan_on and os.path.exists('/run/hvac/fan'):
            ser.write('F')
            fan_on=True
         if fan_on and not os.path.exists('/run/hvac/fan'):
            fan_on=False
            ser.write('X')
         with open('/run/outside') as g:
            with open('/run/temp') as h:
               outside=g.readline().strip()
               th_temp=h.readline().strip()
         if res>0:
            with open('/run/today','a') as f:
               f.write("%s,%5.1f,%5.1f,%s,%s,%d,%s\n"%(datetime.datetime.now(),(int(atemp[2])*.0140625)+32,(int(atemp[1])*.0140625)+32,outside,th_temp,res,','.join(atemp[4:])))
         if res in valid_modes and ut[res] != ut[prev_mode]:
            if delay_ct<2:
               delay_ct+=1
            else:
               with open('/run/furnace', 'w') as f:
                  f.write(ut[res]+','+str(time.time()))
               with open('/user1/hvac/furnace.log','a') as f:
                  if res==0:
                     f.write("%s,%s,%s,%s,%d\n"%(datetime.datetime.now().strftime("%Y,%m,%d,%H,%M,%S"),ut[res],outside,th_temp,res))
                  else:
                     if prev_mode!=0:
                        f.write("%s,%s,%s,%s,%d\n"%(datetime.datetime.now().strftime("%Y,%m,%d,%H,%M,%S"),ut[prev_mode],outside,th_temp,res))
                     f.write("%s,%s,%s,%s,%d\n"%(datetime.datetime.now().strftime("%Y,%m,%d,%H,%M,%S"),ut[res],outside,th_temp,res))
               prev_mode=res
               delay_ct=0
         x=str(round(read_temp(),1))
         with open('/run/MBR','w') as myfile:
            myfile.write(x)
         time.sleep(loop_delay)
