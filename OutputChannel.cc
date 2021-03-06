#include "Global.h"

OutputChannel::OutputChannel() {
  available_space = 4;
  flits_sent = 0;
  wb = nullptr;
  target = nullptr;
}

OutputChannel::OutputChannel(size_t entries)
    : VirtualChannel((uint8_t)entries) {
  available_space = 4;
  flits_sent = 0;
  wb = nullptr;
  target = nullptr;
}

OutputChannel::~OutputChannel() {}

/** ProcessEvent
 *  handles an incoming flit by adding it to the virtual channel
 *
 *  @arg e  Incoming event
 */
void OutputChannel::ProcessEvent(Event e) {
  assert(DATA == e.t || CREDIT == e.t);
  switch (e.t) {
  case CREDIT:
    available_space += *(size_t *)e.d;
    delete (size_t *)e.d;
    if (target->Size() == available_space &&
        cur_packet->GetSize() == flits_sent)
      cur_packet = nullptr;
    break;

  case DATA:
    assert(FlitsRemaining() < Size());
    if (((Flit *)e.d)->isHead()) {
      assert(isReady());
      assert(FlitsRemaining() == 0);
      wb = (InputChannel *)e.o;
      flits_sent = 0;
    }

    InsertFlit((Flit *)e.d);
    break;
  }
}

/** sendFlit
 *  send the head flit to the target VC if necessary
 *
 *  @return Boolean indicating whether send was successful
 */
bool OutputChannel::sendFlit() {
  if (FlitsRemaining() == 0)
    return false;
  if (nullptr == target) {
    // Ejection Queue
    flits_sent++;
    PopFlit();
    if (flits_sent == cur_packet->GetSize())
      cur_packet = nullptr;
    return true;
  }
  if (0 == available_space)
    return false;
  if (GetFlit()->isHead() && target->Size() != available_space)
    return false;
  Event e = {DATA, GetFlit(), this};
  Global_Queue.Add(e, target, Global_Time + 1);
  available_space--;
  PopFlit();
  flits_sent++;
  assert(flits_sent <= cur_packet->GetSize());
  return true;
}

/** SetTarget
 *  sets the route for the current packet
 *
 *  @arg t VC target that we are sending data to
 */
void OutputChannel::setTarget(InputChannel *t) { target = t; }

/** getTarget
 *  gets the route for the current packet
 *
 *  @return Pointer to the target InputChannel
 */
InputChannel *OutputChannel::getTarget() const { return target; }

/** setWB
 *  saves the VC we are receiving data from
 *
 *  @arg writeback Pointer to VC we are receiving from
 */
void OutputChannel::setWB(InputChannel *writeback) { wb = writeback; }

/** getWB
 *  returns a pointer to the VC we are receiving from
 *
 *  @return Pointer to VC
 */
InputChannel *OutputChannel::getWB() const { return wb; }

/** isReady
 *  returns a boolean indicating whether this buffer is ready to receive
 *  a new Packet
 *
 *  @return Whether or not we have sent all flits and those have cleared from
 * target
 */
bool OutputChannel::isReady() const {
  // First time checking
  if (nullptr == cur_packet)
    return true;
  // Ejection queue
  else if (nullptr == target && FlitsRemaining() < buf_size)
    return true;
  else if (cur_packet->GetSize() == flits_sent &&
           available_space == target->Size()) {
    assert(FlitsRemaining() == 0);
    return true;
  } else
    return false;
}

/** isWorking
 *  returns a boolean indicating whether this buffer is in the process of
 *  moving a packet
 *
 *  @return Whether or not we are processing a packet
 */
bool OutputChannel::isWorking() const {
  if (FlitsRemaining() > 0)
    return true;
  else if (nullptr == cur_packet)
    return false;
  else if (flits_sent < cur_packet->GetSize())
    return true;
  else if (nullptr == target)
    return false;
  else if (available_space != target->Size())
    return true;
  else
    return false;
}
