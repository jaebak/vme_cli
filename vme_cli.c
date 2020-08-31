#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <getopt.h>
#include "VMEController.h"

using std::string;
using std::setw;
using std::cout;
using std::endl;
using std::dec;

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

int GetOptions(int argc, char *argv[]);
namespace{
  // MAC_address, schar_port, eth_name, vme_write_read, vme_command, vme_data, slot, comment
  string vcc_mac_address = "02:00:00:00:00:4A";
  int schar_port = 2;
  string eth_name = "p5p2";
  string vme_write_read = "r";
  unsigned short int vme_command = 0x4100;
  unsigned int vme_slot = 19;
  unsigned short int vme_data = 0x0;
}

int main(int argc, char *argv[]) {
  string comment = "";
  int error = GetOptions(argc, argv);
  if (error) return error;

  if (vme_write_read == "r") 
  cout<<"vcc_mac_address: "<<vcc_mac_address<<" schar_port: "<<schar_port
  <<" eth_name: "<<eth_name<<" vme_write_read: "<<vme_write_read
  <<" vme_command: 0x"<<FixLength(vme_command, 4, true)
  <<" vme_slot: "<<vme_slot<<endl;
  else
  cout<<"vcc_mac_address: "<<vcc_mac_address<<" schar_port: "<<schar_port
  <<" eth_name: "<<eth_name<<" vme_write_read: "<<vme_write_read
  <<" vme_command: 0x"<<FixLength(vme_command, 4, true)
  <<" vme_data: 0x"<<FixLength(vme_data, 4, true)
  <<" vme_slot: "<<vme_slot<<endl;

  emu::pc::VMEController vmeController;
  vmeController.init(vcc_mac_address, schar_port, eth_name);
  if (vme_write_read == "w") VMEWrite(&vmeController, vme_command, vme_data, vme_slot, comment);
  else if (vme_write_read == "r")  VMERead(&vmeController, vme_command, vme_slot, comment);
  return 0;
}


int GetOptions(int argc, char *argv[]){
  int nParameters = 7;
  int readParameters = 0;
  
  while(true){
    static struct option long_options[] = {
      {"vcc_mac_address", required_argument, 0, 0},
      {"schar_port", required_argument, 0, 0},
      {"eth_name", required_argument, 0, 0},
      {"vme_write_read", required_argument, 0, 0},
      {"vme_command", required_argument, 0, 0},
      {"vme_data", required_argument, 0, 0},
      {"vme_slot", required_argument, 0, 0},
      {0, 0, 0, 0}
    };

    char opt = -1;
    int option_index;
    opt = getopt_long(argc, argv, "", long_options, &option_index);

    if( opt == -1) break;

    string optname;
    switch(opt){
    case 0:
      optname = long_options[option_index].name;
      if(optname == "vcc_mac_address"){
        vcc_mac_address = optarg; readParameters++;
      } else if(optname == "schar_port"){
        schar_port = atoi(optarg); readParameters++;
      } else if(optname == "eth_name"){
        eth_name = optarg; readParameters++;
      } else if(optname == "vme_write_read"){
        vme_write_read = optarg; readParameters++;
      } else if(optname == "vme_command"){
        std::stringstream converter;
        converter << std::hex << optarg;
        converter >> vme_command; readParameters++;
      } else if(optname == "vme_data"){
        std::stringstream converter;
        converter << std::hex << optarg;
        converter >> vme_data; readParameters++;
      } else if(optname == "vme_slot"){
        vme_slot = atoi(optarg); readParameters++;
      }else{
        printf("Bad option! Found option name %s\n", optname.c_str());
      }
      break;
    default:
      printf("Bad option! getopt_long returned character code 0%o\n", opt);
      break;
    }
  }
  // Quick check if all parameters are set
  int error = 0;
  if (vme_write_read=="r" && readParameters != nParameters-1) error = 1;
  if (vme_write_read=="w" && readParameters != nParameters) error = 1;
  if (error) {
    cout<<"[Error] Parameters were not set correctly."<<endl;
    cout<<"[Examples] "<<endl;
    cout<<argv[0]<<" --vcc_mac_address 02:00:00:00:00:4A"
    <<" --schar_port 2"
    <<" --eth_name p5p2"
    <<" --vme_write_read r"
    <<" --vme_command 4100"
    <<" --vme_slot 19"<<endl;
    cout<<argv[0]<<" --vcc_mac_address 02:00:00:00:00:4A"
    <<" --schar_port 2"
    <<" --eth_name p5p2"
    <<" --vme_write_read w"
    <<" --vme_command 1020"
    <<" --vme_data ff"
    <<" --vme_slot 19"<<endl;
    return 1;
  }
  return 0;
}
