import socket
import sys
import time
# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

server_address = ('127.0.0.1', 1234)
message =  b'\x00\x00\x00\x09\x00\x00\x00\xff\x40\xD1\xCF\x4C\x42\x38\x00\x00'
message2 = b'\x00\x00\x30\xff\x00\x00\x00\xff\x40\xD1\xCF\x4C\x42\x38\x00\x00'

try:
    timeout = 0.1
    # Send data
    print >>sys.stderr, 'sending "%s"' % message
    for it in range(1000):
        if (it % 2) == 0:
            sent = sock.sendto(message, server_address)
            time.sleep(timeout)
        else:
            sent = sock.sendto(message2, server_address)
            time.sleep(timeout)

finally:
    print >>sys.stderr, 'closing socket'
    sock.close()