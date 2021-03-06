#ifndef INPUTBUFFER_H
#define INPUTBUFFER_H

#include "OutputBuffer.h"
#include "RouteComputation.h"

class InputBuffer {
public:
  InputBuffer();
  explicit InputBuffer(size_t entries);
  ~InputBuffer();

  void setAddr(Address newAddr);
  void setRC(RouteComputation *rcomp);
  void WriteBack(OutputBuffer *write_back);
  InputChannel *getIC(size_t channel) const;

protected:
private:
  InputChannel *ic;     //!< Pointer to input channels
  size_t channel_count; //!< Total number of virtual channels
};

#endif
