#ifndef FLIT_H
#define FLIT_H

class Packet;

class Flit {
public:
  Flit();
  Flit(Packet *p, bool header, uint32_t d);
  ~Flit();

  void setPacket(Packet *p);
  void setHead(bool header);
  void setData(uint32_t d);
  Packet *getPacket() const;
  bool isHead() const;
  uint32_t getData() const;

protected:
private:
  Packet *origin; //!< Pointer to packet that this flit belongs to
  bool head;      //!< Is this flit a head flit?
  uint32_t data;  //!< Data contained by this flit
};

#endif
