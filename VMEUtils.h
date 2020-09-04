#ifndef VMEUtils_h
#define VMEUtils_h

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iomanip>
#include <sstream>

#include "VMEController.h"

using std::string;
using std::setw;
using std::cout;
using std::endl;
using std::dec;

namespace VMEUtils {
  string FixLength(unsigned int Number, unsigned int Length, bool isHex);
  void VMEWrite (emu::pc::VMEController * const vmeController, unsigned short int address, unsigned short int command, unsigned int slot, string comment);
  unsigned short int VMERead (emu::pc::VMEController * const vmeController, unsigned short int address, unsigned int slot, string comment);

  // Check if vcc is okay
  //unsigned short regbuf[3000];
  //int n=vmeController.vcc_read_command(0x0E, 7, regbuf);
  //cout<<"n: "<<n<<endl;

  string FixLength(unsigned int Number, unsigned int Length, bool isHex){
    std::stringstream Stream;
    if(isHex) Stream << std::hex << Number;
    else Stream << std::dec << Number;
    string sNumber = Stream.str();
    for(unsigned int cha=0; cha<sNumber.size(); cha++) sNumber[cha] = toupper(sNumber[cha]);
    while(sNumber.size() < Length) sNumber = " " + sNumber;
    return sNumber;
  }
  
  void VMEWrite (emu::pc::VMEController * const vmeController, unsigned short int address, unsigned short int command, unsigned int slot, string comment) {
    unsigned int shiftedSlot = slot << 19;
    int addr = (address & 0x07ffff) | shiftedSlot;
    char rcv[2];
    vmeController->vme_controller(3,addr,&command,rcv);
    // Get timestamp
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char timestamp[30];
    strftime (timestamp,30,"%D %X",timeinfo);
    // Write to log
    std::cout << "W" << std::setw(7) << FixLength(address,4,true)
        << std::setw(7) << FixLength(command,4,true)
        << std::setw(15) << dec << slot 
        << "   " << timestamp
        << "    " << comment.c_str() << endl;
  }

  unsigned short int VMERead (emu::pc::VMEController * const vmeController, unsigned short int address, unsigned int slot, string comment) {
    unsigned int shiftedSlot = slot << 19;
    int addr = (address & 0x07ffff) | shiftedSlot;
    unsigned short int data;
    char rcv[2];
    vmeController->vme_controller(2,addr,&data,rcv);
    unsigned short int result = (rcv[1] & 0xff) * 0x100 + (rcv[0] & 0xff);
      
    // Get timestamp
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char timestamp[30];
    strftime (timestamp,30,"%D %X",timeinfo);
    // Get formatting
    bool readHex = true;
    if((address >= 0x321C && address <= 0x337C) || (address >= 0x33FC && address < 0x35AC)  || 
   (address > 0x35DC && address <= 0x3FFF) || address == 0x500C || address == 0x510C ||
   address == 0x520C || address == 0x530C || address == 0x540C   || address == 0x550C || address == 0x560C 
   || address == 0x8004 ||  (address == 0x5000 && result < 0x1000)) {
  readHex = false;
    }
    // Write to log
    std::cout << "R" << std::setw(7) << FixLength(address,4,true);
    if (readHex) std::cout<< std::setw(14) << FixLength(result,4,true);
    else std::cout<< std::setw(12) << dec << result << "_d";
    std::cout << std::setw(8) << dec << slot 
        << "   " << timestamp
        << "    " << comment.c_str() << endl;
    return result;
  }

}

#endif
