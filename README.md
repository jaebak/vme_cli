Hacked odmbdev and emulib to make a simple cli for vme.
odmbdev: https://github.com/odmb/odmbdev
emulib: https://gitlab.cern.ch/CMS-Emu/CERN/TriDAS/-/blob/master/emu/emuDCS/PeripheralCore/include/emu/pc/VMEController.h

Do ./compile.sh to make vme_cli.

Below is an example command
./vme_cli --vcc_mac_address 02:00:00:00:00:4A --schar_port 2 --eth_name p5p2 --vme_write_read r --vme_command 4100 --vme_slot 19
./vme_cli --vcc_mac_address 02:00:00:00:00:4A --schar_port 2 --eth_name p5p2 --vme_write_read w --vme_command 1020 --vme_data ff --vme_slot 19
