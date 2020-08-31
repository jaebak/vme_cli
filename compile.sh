g++ -o vme_cli vme_cli.c VMEController.cc 
echo "Try below command"
echo ./vme_cli --vcc_mac_address 02:00:00:00:00:4A --schar_port 2 --eth_name p5p2 --vme_write_read r --vme_command 4100 --vme_slot 18
