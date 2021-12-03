echo "Installing pico SDK, might need to run as sudo"
sudo apt install wget
wget https://raw.githubusercontent.com/raspberrypi/pico-setup/master/pico_setup.sh
chmod +x pico_setup.sh
./pico_setup.sh
echo "Please run sudo reboot to finish installation"
