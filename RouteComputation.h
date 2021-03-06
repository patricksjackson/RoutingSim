#ifndef ROUTECOMPUTATION_H
#define ROUTECOMPUTATION_H

#include "EventTarget.h"
#include "InputChannel.h"

class RouteComputation : public EventTarget {
public:
  RouteComputation();
  explicit RouteComputation(Address a);
  virtual ~RouteComputation();

  void setAddr(Address a);
  void RouteXYEscape(InputChannel *vc, Packet *p, size_t a, size_t b);
  void RouteXY(InputChannel *vc, Packet *p, size_t c);
  void RouteAdaptive(InputChannel *vc, Packet *p, size_t c);
  void ProcessEvent(Event e);
  InputChannel *getNext(size_t dir, size_t c) const;
  void Remove(InputChannel *vc);

protected:
private:
  void Insert(InputChannel *vc, Direction d, size_t c);
  void _Remove(InputChannel *vc, InputChannel *arr[5][13]);

  Address addr; //!< Address of router
  InputChannel
      *ivc_xy[5][13]; //!< Pointer to InputBuffers that desire each OutputBuffer
  InputChannel *ivc_ad[5][13]; //!< Pointer to input VC's that desire each
                               //!adaptive output VC
  InputChannel *
      ivc_xya[5][13]; //!< Pointer to InputBuffers that desire each OutputBuffer
};

#endif
