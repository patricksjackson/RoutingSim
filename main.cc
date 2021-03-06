#include "Global.h"
#include <cstdlib>
#include <iostream>

using namespace std;

// Defining global variables
uint32_t Global_Time;
Address MC[8];
NetworkInfo NInfo;
EventQueue Global_Queue;
Router *NArray;
Router *NArray2;

// Defining all of the counters for grabbing statistics
uint64_t packet_injections = 0;
uint64_t packets_blocked = 0;
uint64_t packets_sent = 0;
uint64_t packet_ejections = 0;
uint64_t packet_latency = 0;
uint64_t link_util = 0;
uint64_t aibuf_util = 0;
uint64_t eibuf_util = 0;
uint64_t aobuf_util = 0;
uint64_t eobuf_util = 0;

void RunSimulation(uint32_t simulation_end, double injection_chance);
void SetupMemCont(size_t type);

int main(int argc, char **argv) {
  // Defining default length of simulation and chance of injection
  uint32_t simulation_end = END_TIME;
  double injection_chance;

  // Initialize default network settings
  NInfo.width = 8;
  NInfo.height = 8;
  NInfo.dest_func = BIT_REV;
  NInfo.adaptive = true;
  NInfo.memcont = false;
  // SetupMemCont( 1 );

  // Seed random number generator
  srand(4);
  // srand(time(nullptr));

  // Parse arguments
  if (argc > 1) {
    SetupMemCont(size_t(atoi(argv[1])));
    NInfo.adaptive = (1 == atoi(argv[2])) ? true : false;
    if (argc > 3) {
      simulation_end = uint32_t(atoi(argv[3]));
      if (argc > 4) {
        injection_chance = atof(argv[4]);
        RunSimulation(simulation_end, injection_chance);
        return EXIT_SUCCESS;
      }
    }
  }

  // Generate headers for the output
  cout << "Packet Injections, ";
  cout << "Packet Ejections, ";
  cout << "Offered Load, ";
  cout << "Network Throughput, ";
  cout << "Packet Latency, ";
  cout << "Link Utilization, ";
  cout << "Adaptive Input Buffer Utilization, ";
  cout << "Adaptive Output Buffer Utilization, ";
  cout << "Escape Input Buffer Utilization, ";
  cout << "Escape Output Buffer Utilization, ";
  cout << "Simulation Time, ";
  cout << "Injection Chance" << endl;

  // Set number of tests
  uint32_t tests = 100;

  // Iterate and run tests
  for (size_t i = tests; i-- > 0;) {
    injection_chance = ((double)i) / tests;
    RunSimulation(simulation_end, injection_chance);
    Global_Queue.Clear();
  }

  return EXIT_SUCCESS;
}

/** RunSimulation
 *  executs a simulation with memory controllers
 *
 *  @arg simulation_end   Total time to run the simulation
 *  @arg injection_chance Chance of each Packet Generator injecting a
 *                          packet each cycle
 */
void RunSimulation(uint32_t simulation_end, double injection_chance) {
  // Set injection chance
  NInfo.chance = injection_chance / 3;

  // Create Router and packet generators for request network
  NArray = new Router[NInfo.width * NInfo.height];
  for (uint8_t i = 0; i < NInfo.width; i++) {
    for (uint8_t j = 0; j < NInfo.height; j++) {
      Address addr = {i, j};
      NArray[IND(i, j)].SetAddr(addr);
      NArray[IND(i, j)].Connect(NORTH, &NArray[IND(i, (j + 1) % NInfo.height)]);
      NArray[IND(i, j)].Connect(EAST, &NArray[IND((i + 1) % NInfo.width, j)]);
      NArray[IND(i, j)].Connect(
          SOUTH, &NArray[IND(i, (j + NInfo.height - 1) % NInfo.height)]);
      NArray[IND(i, j)].Connect(
          WEST, &NArray[IND((i + NInfo.width - 1) % NInfo.width, j)]);
    }
  }
  // Initialize response network if we're doing the memory controller simulation
  if (NInfo.memcont) {
    NArray2 = new Router[NInfo.width * NInfo.height];
    for (uint8_t i = 0; i < NInfo.width; i++) {
      for (uint8_t j = 0; j < NInfo.height; j++) {
        Address addr = {i, j};
        NArray2[IND(i, j)].SetAddr(addr);
        NArray2[IND(i, j)].Connect(NORTH,
                                   &NArray2[IND(i, (j + 1) % NInfo.height)]);
        NArray2[IND(i, j)].Connect(EAST,
                                   &NArray2[IND((i + 1) % NInfo.width, j)]);
        NArray2[IND(i, j)].Connect(
            SOUTH, &NArray2[IND(i, (j + NInfo.height - 1) % NInfo.height)]);
        NArray2[IND(i, j)].Connect(
            WEST, &NArray2[IND((i + NInfo.width - 1) % NInfo.width, j)]);
      }
    }
  }

  for (Global_Time = 0; Global_Time < simulation_end; Global_Time++) {
    Global_Queue.Process(); // Process all packet movements in the queue
    for (uint8_t i = 0; i < NInfo.width; i++) {
      for (uint8_t j = 0; j < NInfo.height; j++) {
        // Process each router in request network for a time step
        NArray[IND(i, j)].Process();
        // Place new packet into injection queue
        NArray[IND(i, j)].GetPacketGen()->RandomGenPacket(NInfo.chance);
        if (NInfo.memcont) {
          // Process each router in response network for a time step
          NArray2[IND(i, j)].Process();
        }
      }
    }
  }

  // Print out the counter data gathered during this simulation run
  cout << packet_injections << ", ";
  cout << packet_ejections << ", ";
  cout << ((17 * (double)packet_injections) / 8.0) / simulation_end << ", ";
  cout << ((17 * (double)packet_ejections) / 8.0) / simulation_end << ", ";
  cout << (double)packet_latency / packet_ejections << ", ";
  cout << (double)link_util / (NInfo.width * NInfo.height * 4 * simulation_end)
       << ", ";
  cout << (double)aibuf_util /
              (NInfo.width * NInfo.height * 4 * 4 * simulation_end)
       << ", ";
  cout << (double)aobuf_util /
              (NInfo.width * NInfo.height * 4 * 1 * simulation_end)
       << ", ";
  cout << (double)eibuf_util /
              (NInfo.width * NInfo.height * 4 * 4 * simulation_end)
       << ", ";
  cout << (double)eobuf_util /
              (NInfo.width * NInfo.height * 4 * 1 * simulation_end)
       << ", ";
  cout << simulation_end << ", ";
  cout << injection_chance << endl;

  // Reset counters
  packet_injections = 0;
  packet_ejections = 0;
  packets_blocked = 0;
  packets_sent = 0;
  packet_latency = 0;
  link_util = 0;
  aibuf_util = 0;
  eibuf_util = 0;
  aobuf_util = 0;
  eobuf_util = 0;

  // Memory cleanup
  delete[] NArray;
  delete[] NArray2;

  return;
}

/** SetupMemCont
 *  initializes the locations of the memory controllers in the network based on
 * the input
 *
 *  @arg type Specifies the setup of the network
 */
void SetupMemCont(size_t type) {
  switch (type) {
  case 1:
    // Random placement
    cout << "Random placement, " << endl;
    MC[0].x = 0;
    MC[0].y = 0;
    MC[1].x = 1;
    MC[1].y = 0;
    MC[2].x = 2;
    MC[2].y = 0;
    MC[3].x = 3;
    MC[3].y = 0;
    MC[4].x = 4;
    MC[4].y = 0;
    MC[5].x = 5;
    MC[5].y = 0;
    MC[6].x = 6;
    MC[6].y = 0;
    MC[7].x = 7;
    MC[7].y = 0;
    break;

  case 2:
    // Single Row
    cout << "Single Row, " << endl;
    MC[0].x = 0;
    MC[0].y = 4;
    MC[1].x = 1;
    MC[1].y = 4;
    MC[2].x = 2;
    MC[2].y = 4;
    MC[3].x = 3;
    MC[3].y = 4;
    MC[4].x = 4;
    MC[4].y = 4;
    MC[5].x = 5;
    MC[5].y = 4;
    MC[6].x = 6;
    MC[6].y = 4;
    MC[7].x = 7;
    MC[7].y = 4;
    break;

  case 3:
    // Single Col
    cout << "Single Column, " << endl;
    MC[0].x = 4;
    MC[0].y = 0;
    MC[1].x = 4;
    MC[1].y = 1;
    MC[2].x = 4;
    MC[2].y = 2;
    MC[3].x = 4;
    MC[3].y = 3;
    MC[4].x = 4;
    MC[4].y = 4;
    MC[5].x = 4;
    MC[5].y = 5;
    MC[6].x = 4;
    MC[6].y = 6;
    MC[7].x = 4;
    MC[7].y = 7;
    break;

  case 4:
    // Diagonal
    cout << "Diagonal, " << endl;
    MC[0].x = 0;
    MC[0].y = 0;
    MC[1].x = 1;
    MC[1].y = 1;
    MC[2].x = 2;
    MC[2].y = 2;
    MC[3].x = 3;
    MC[3].y = 3;
    MC[4].x = 4;
    MC[4].y = 4;
    MC[5].x = 5;
    MC[5].y = 5;
    MC[6].x = 6;
    MC[6].y = 6;
    MC[7].x = 7;
    MC[7].y = 7;
    break;

  case 5:
    // Split Diagonal
    cout << "Split Diagonal, " << endl;
    MC[0].x = 0;
    MC[0].y = 0;
    MC[1].x = 1;
    MC[1].y = 1;
    MC[2].x = 2;
    MC[2].y = 5;
    MC[3].x = 3;
    MC[3].y = 4;
    MC[4].x = 4;
    MC[4].y = 3;
    MC[5].x = 5;
    MC[5].y = 2;
    MC[6].x = 6;
    MC[6].y = 6;
    MC[7].x = 7;
    MC[7].y = 7;
    break;

  case 6:
    // Unique Row, Column, and Diagonal
    cout << "Unique Row, Column, and Diagonal, " << endl;
    MC[0].x = 0;
    MC[0].y = 4;
    MC[1].x = 1;
    MC[1].y = 0;
    MC[2].x = 2;
    MC[2].y = 3;
    MC[3].x = 3;
    MC[3].y = 5;
    MC[4].x = 4;
    MC[4].y = 7;
    MC[5].x = 5;
    MC[5].y = 1;
    MC[6].x = 6;
    MC[6].y = 6;
    MC[7].x = 7;
    MC[7].y = 2;
    break;

  default:
    cout << "Invalid configuration setup.\n" << endl;
    exit(1);
  }
}
