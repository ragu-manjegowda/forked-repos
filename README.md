Rendezvous
==========

Rendezvous is a **simple** javascript GUI that runs in your browser and allows to track the position of your device/robot on a global map.
The application supports the _ROS_ (Robotic Operating System) standard GPS-location message [sensor_msgs/NavSatFix](http://docs.ros.org/hydro/api/sensor_msgs/html/msg/NavSatFix.html) and can be easily customized to fit your needs.

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

![Rendezvous screenshot](http://oi61.tinypic.com/kejiiu.jpg "Rendezvous screenshot")

Hints:
------
* You can adjust the data update-rate in the GUI modifying the `updateRate` variable in `subscriber_ros_gui.js`;
* You can change the displayed icon modifying the `url` entry in the `startVisualization()` function (`map_init_gmaps.js`). the `images` folder contains some alternative icons you may want to use. Remember to specify the correct anchor point for the new icon in order to correctly align it to the GPS trace.

GMAPS API DISCLAIMER: 
---------------------
At the moment (Aug 2014) Google Maps is the only visualization engine integrated in Rendezvous, therefore using this software you *must* comply with the Google Maps/Google Earth APIs Terms of Service (https://developers.google.com/maps/terms?hl=it).  

LICENSE:
--------
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
