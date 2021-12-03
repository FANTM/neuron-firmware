def format_packet(value, channel):
    packet = bytes(4)
    packet[0] = (value >> 8) & 0xFE
    packet[1] = (value >> 1) & 0xFE
    packet[2] = ((value) << 6) & 0xC0) | ((channel << 2) & 0x3C)
    packet[3] = 0x01
    return packet
