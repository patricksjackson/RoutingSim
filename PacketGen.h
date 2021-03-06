#ifndef PACKETGEN_H
#define PACKETGEN_H

class PacketGen {
public:
  PacketGen();
  explicit PacketGen(Address setAddress);
  ~PacketGen();

  void SetAddr(Address setAddress);
  void SetConnection(Address setAddress);
  void SetDirection(Direction edge);
  InputBuffer *GetInjection() const;
  OutputBuffer *GetEjection() const;
  Packet *InsertPacket(Address dest, size_t packet_size);
  void GenPacket();
  void RandomGenPacket(double chances);
  void Process();

protected:
private:
  InputBuffer *ibuf;       //!< Ejection queue
  OutputBuffer *obuf;      //!< Injection queue
  Direction dir;           //!< Direction this generator is pointing
  Address addr;            //!< Address of this generator
  uint32_t packets_out;    //!< Total packets that arrived in the ejection queue
  uint32_t flits_received; //!< Number of flits received in the current packet
  Packet *saved_p;    //!< Generated packet that we are waiting to insert to the
                      //!injection queue
  bool memcontroller; //!< Store whether or not this PG is a memory controller
};

#endif
