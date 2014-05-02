
import libnodave
import struct

link = libnodave.libnodave()

speed = libnodave.daveSpeed187k        # 2
protocol = libnodave.daveProtoMPI_IBH  # 223
localMPI = 0
plcMPI = 2

#link.new_interface('if1', localMPI, protocol, libnodave.daveSpeed187k)

link.open_socket('10.0.0.10', 1099)

link.new_interface('if1', 0, 223, 2)

link.set_timeout(50000000)

link.init_adapter()

link.connect_plc(2, 0, 2)


def read_real():
    print link.dave.daveGetFloat(link.dc)


def read_real_old():
    data = []

    for i in range(4):
        data.append(link.dave.daveGetU8(link.dc))

    real = struct.unpack('>f', struct.pack('4B', *data))[0]
    print real


def read_floats():
    link.read_bytes(0x84, 10, 92, 3 * 4)
    for i in range(3):
        read_real_old()


def read_new_floats():
    print link.read_bytes(0x84, 10, 92, 3 * 4)
    for i in range(3):
        read_real()

read_floats()

print 'YAY'

#link.stop()
link.disconnect()
