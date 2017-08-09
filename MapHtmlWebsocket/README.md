Rendezvous
==========

Rendezvous is a **simple** javascript GUI that runs in your browser and displays the position of your device/robot on a global map in real time.

Rendezvous offers basic visualization of:
* GPS Position
* Magnetic heading
* Accuracy of the GPS position
* GPS Fix status

The application supports the _ROS_ (Robotic Operating System) standard GPS-location  [sensor_msgs/NavSatFix](http://docs.ros.org/hydro/api/sensor_msgs/html/msg/NavSatFix.html) and magnetometer [sensor_msgs/MagneticField](http://docs.ros.org/hydro/api/sensor_msgs/html/msg/MagneticField.html) messages
and it can be very easily customized to fit your needs.

Dependencies:
-------------
[Rosbridge](http://wiki.ros.org/rosbridge_suite) is needed to run Rendezvous.
    
HowTo:
------

* [Generate](https://code.google.com/apis/console) your personal Gmaps API key. Then paste it in the `index.html` file:
    `src="https://maps.googleapis.com/maps/api/js?key=XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX">`
* In the `subscriber_ros_gui.js` be sure to set the `url` parameter in the `ros` variable to match your rosbridge configuration and
* set the `name` parameter in the `listener` variable accordingly to your GPS message naming convention;
* Launch `rosbridge` together with your custom ROS environment;
* Open `index.html` with your favourite browser.

![Rendezvous screenshot](http://s7.postimg.org/3ofu9acbv/rendezvous.png "Rendezvous screenshot")

Notes:
------
* You can adjust the on-screen update-rate in the GUI modifying the `updateRate` variable in `subscriber_ros_gui.js`;
* The heading angle is computed very roughly directly from the magnetic field components in the magnetometer message without tilt compensation. This is very likely to produce unprecise and biased heading estimates. You've been warned :)

GMAPS API DISCLAIMER: 
---------------------
At the moment (Aug 2014) Google Maps is the only visualization engine integrated in Rendezvous, therefore using this software you *must* comply with the Google Maps/Google Earth APIs Terms of Service (https://developers.google.com/maps/terms?hl=it).  

LICENSE:
--------
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
