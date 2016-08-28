# RaspberryPi-SmartWifi
Alternative Wifi client configuration with automatic AP configuration if no known networks are found

SmartWifi is an alternative Wifi configuration script for Debian derived Linuxes and headless devices such as a Raspberry PI or Odroid.

Upon boot it searches for "known" Wifi networks and connects to the first one found.

If it does not find a known Wifi network it switches the Wifi device to AP mode and provides an insecure network which you can connect to via another device such a smartphone to configure the Wifi.

It does not change the configuration of the utilities it uses.   It creates temporary configuration files used during the AP mode operations.

Installing the software

Dependencies

sudo apt-get install hostapd dnsmasq iptables cpanminus

sudo cpanm HTTP::Server::Simple

Configuration

Disable the existing Wifi management

Generally this means editing /etc/network/interfaces and commenting out all wlan0 related lines leaving this line:

iface wlan0 inet manual

Put the wifi4.pl script somewhere useful on your system,

I suggest something like

/home/pi/wifi4.pl

Make sure the script is executable

chmod +x /home/pi/wifi4.pl

then add this line to /etc/rc.local :

/home/pi/wifi4.pl >> /var/log/wifi4.log 2>&1 < /dev/null &

Then reboot your Raspberry PI.

Using your smartphone or other device search for a network that starts with RPI-

Select that network and eventhough your smartphone may report the network does not have
connectivity tell it you want to stay to connected.

Using the smartphone's browser visit 192.168.4.1 (or generally any URL)

In the menu that pops up enter the ssid and passphrase for the network you want the RPI to connect to in the future.

After you hit submit the RPI will reboot and try to connect to the network you configured.

The list of networks you have configured are kept in /var/tmp/wifi4.conf

If you delete that file you will have to start over configuring networks.

You can also edit that file with your favorite editor.

Every network you configure will be remembered and tried in the future on each reboot.

Caveats:

The AP mode browser needs to control port 80.  It kills the various processes which might have control of the port.  However, it might miss some applications, in which case the web service will fail.  You can add the appropriate killall to the script.  The killed processes will restart when the system reboots back to client (station) mode.


