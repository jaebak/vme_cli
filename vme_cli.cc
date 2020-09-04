#include <iostream>
#include <getopt.h>

#include "VMEController.h"
#include "VMEUtils.h"

using std::string;
using std::setw;
using std::cout;
using std::endl;
using std::dec;

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
  bool verbose = false;
}

int main(int argc, char *argv[]) {
  // Parse options
  int error = GetOptions(argc, argv);
  if (error) return error;

  // Print selected option
  if (verbose) {
    if (vme_write_read == "r") 
    cout<<"vcc_mac_address: "<<vcc_mac_address<<" schar_port: "<<schar_port
    <<" eth_name: "<<eth_name<<" vme_write_read: "<<vme_write_read
    <<" vme_command: 0x"<<VMEUtils::FixLength(vme_command, 4, true)
    <<" vme_slot: "<<vme_slot<<endl;
    else
    cout<<"vcc_mac_address: "<<vcc_mac_address<<" schar_port: "<<schar_port
    <<" eth_name: "<<eth_name<<" vme_write_read: "<<vme_write_read
    <<" vme_command: 0x"<<VMEUtils::FixLength(vme_command, 4, true)
    <<" vme_data: 0x"<<VMEUtils::FixLength(vme_data, 4, true)
    <<" vme_slot: "<<vme_slot<<endl;
  }

  // Initialize vmeController
  emu::pc::VMEController vmeController;
  vmeController.init(vcc_mac_address, schar_port, eth_name);
  if (verbose) vmeController.Debug(1000);

  // Send vme write or read command.
  string log_comment = "";
  if (vme_write_read == "w") VMEUtils::VMEWrite(&vmeController, vme_command, vme_data, vme_slot, log_comment);
  else if (vme_write_read == "r")  VMEUtils::VMERead(&vmeController, vme_command, vme_slot, log_comment);
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
      {"verbose", no_argument, 0, 0},
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
      } else if(optname == "verbose") {
        verbose = true;
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
