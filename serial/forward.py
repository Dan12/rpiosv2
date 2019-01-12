import serial

port = '/dev/cu.SLAB_USBtoUART'
baud = '115200'

def write_byte(b):
  ser.flushInput()
  ser.write(bytes([b]))

def write_packet(p):
  # print("Sending packet")
  # print(p)
  for i in range(10):
    write_byte(p[i])

def read_until_proc(read=False, conf=0):
  c = False
  if read:
    raw = ser.read(2)
    # print(raw)
    if raw[0] == 0x88 and raw[1] == conf:
      c = True
  while True:
      raw = ser.read()
      # print(raw)
      if raw[0] == 0xff:
        break

  return c

def write_write(addr, bts):
  bytes = [0x01]
  for i in range(4):
    bytes.append(addr & 0xff)
    addr >>= 8
  for b in bts:
    bytes.append(b & 0xff)
  while(len(bytes) < 9):
    bytes.append(0x00)
  cs = 0
  for i in range(9):
    cs = cs ^ bytes[i]
  bytes.append(cs)
  write_packet(bytes)
  read_until_proc()

def write_read(addr, ex=0):
  bytes = [0x04]
  for i in range(4):
    bytes.append(addr & 0xff)
    addr >>= 8
  for i in range(4):
    bytes.append(0xff)
  cs = 0
  for i in range(9):
    cs = cs ^ bytes[i]
  bytes.append(cs)
  write_packet(bytes)
  return read_until_proc(read=True, conf=ex)

def write_jump(addr):
  bytes = [0x02]
  for i in range(4):
    bytes.append(addr & 0xff)
    addr >>= 8
  for i in range(4):
    bytes.append(0xff)
  cs = 0
  for i in range(9):
    cs = cs ^ bytes[i]
  bytes.append(cs)
  write_packet(bytes)
  # read_until_proc()

def conf_write(addr, vals):
  write_write(addr, vals)
  for i in range(4):
    if (write_read(addr+i, ex=vals[i])):
      print('Suc write')
    else:
      print('Write failed')

# def write_stuff():
#   conf_write(0x1000, 0xde)
#   conf_write(0x1001, 0xad)
#   conf_write(0x1002, 0xbe)
#   conf_write(0x1003, 0xef)

def all_zeros(c):
  for b in c:
    if b != 0:
      return False
  return True

def read_file():
  addr = 0x8000
  numw = 0
  with open("../kernel7.img", "rb") as f:
    while True:
      chunk = f.read(128)
      if chunk:
        if all_zeros(chunk):
          addr += 128
        else:
          for i in range(0, len(chunk), 4):
            write_write(addr, chunk[i:i+4])
            addr += 4
            numw += 1
            # print("write "+str(numw))
      else:
        # for i in range(0x8000, addr):
        #   print('confirming ' + str(i) + ": " + str(write_read(i)))
        print('wrote '+str(numw))
        print('jumping')
        write_jump(0x8000)
        break

ser = serial.Serial(port,baud)
while(True):
  raw = ser.readline()
  line = raw.decode('utf-8', errors='ignore')
  print(raw)
  if 'Waiting for input' in line:
    # write_stuff()
    read_file()
    print('done reading file')
    while True:
      raw = ser.readline()
      line = raw.decode('utf-8', errors='ignore')
      print(line, end='')
    