#define SerialXXX Serial

class Packet {
private:
  byte bytes[512];
  int index = 0;
public:
  int Index() { return index; }
  byte GetByte(int i) { return bytes[i]; }
  void Reset() { index = 0; }
  int ToInt(int startIndex) { return bytes[startIndex] + (bytes[startIndex + 1] << 8); }

  void CutOut(int endIndex) {
    if (endIndex <= index) {
      for (int i = 0; i < index - endIndex; i++) { bytes[i] = bytes[endIndex + i]; }
      index = index - endIndex;
    }
  }

  void AddInt(int addInt) {
    bytes[index] = byte(addInt);
    bytes[index + 1] = byte(addInt >> 8);
    index += 2;
  }

  void AddByte(byte addByte) {
    bytes[index] = addByte;
    index++;
  }

  byte Bcc(int startIndex, int endIndex) {
    byte bcc = 0;
    for (int i = startIndex; i < endIndex; i++) { bcc ^= bytes[i]; }
    return (byte)(bcc & 0xFF);
  }

  void SerialWrite() {
    SerialXXX.write(bytes, index);
    SerialXXX.flush();
  }

};

class SerialRCF {
private:
  const byte Header[2] = { 0xFF, 0x7F };
  Packet readPacket, sendPacket;

  void ClearBuffer() {
    while (SerialXXX.available()) { SerialXXX.read(); }
  }

public:
  int ReceiveData[128] = {};
  int ErrorCode[3] = {};

  void Send(int intArray[], int arrayLength) {
    sendPacket.Reset();
    sendPacket.AddByte(Header[0]);
    sendPacket.AddByte(Header[1]);
    byte packetLength = (byte)(4 + 2 * arrayLength);
    sendPacket.AddByte(packetLength);
    for (int i = 0; i < arrayLength; i++)
      sendPacket.AddInt(intArray[i]);
    byte bcc = sendPacket.Bcc(2, sendPacket.Index());
    sendPacket.AddByte(bcc);
    sendPacket.SerialWrite();
  }

  void Receive() {
    if (62 < SerialXXX.available()) {
      ErrorCode[0]++;  // Buffer Capacity Error
      ClearBuffer();
    } else {
      while (SerialXXX.available()) {
        readPacket.AddByte(SerialXXX.read());
      }
    }

    if (2 < readPacket.Index()) {
      if (readPacket.GetByte(0) == Header[0] && readPacket.GetByte(1) == Header[1]) {
        byte packetLength = readPacket.GetByte(2);
        if (packetLength <= readPacket.Index()) {
          byte bcc = readPacket.GetByte(packetLength - 1);
          if (bcc == readPacket.Bcc(2, packetLength - 1)) {
            for (int i = 0, j = 3; j < packetLength - 1; i++, j += 2)
              ReceiveData[i] = readPacket.ToInt(j);
            readPacket.CutOut(packetLength);
          } else {
            ErrorCode[1]++;  // Checksum (BCC) Error
            readPacket.Reset();
          }
        }
      } else {
        ErrorCode[2]++;  // Header Error
        readPacket.Reset();
      }
    }
  }

};

SerialRCF SerialRCF;
