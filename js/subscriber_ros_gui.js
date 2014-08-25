  // Connecting to ROS
  // -----------------
  var index = 0 ;
  var i = 0;
  var d = new Date();
  var time  = d.getTime();
  var timeM = time;
  var newpositionC = 0;
  var updateRate = 200;

  var ros = new ROSLIB.Ros({
    url : 'ws://localhost:9090'
  });

  // Subscribing to a Topic
  // ----------------------

  var listener = new ROSLIB.Topic({
    ros : ros,
    name : 'gps_fix', //topic name
    messageType : 'sensor_msgs/NavSatFix' //message Type
  });  

  listener.subscribe(function(message) {

    if(index == 0){ // firts entry

      startVisualization(message.latitude,message.longitude);

      putHome(message.latitude,message.longitude);
      oldCurrentPosition[index] = new google.maps.LatLng(message.latitude,message.longitude);

    }else{ // new entries

      newpositionC = new google.maps.LatLng(message.latitude,message.longitude);

      d = new Date();
      time  = d.getTime();

      if((time - timeM) >= updateRate) //marker and path update rate in milliseconds
      {
        markerCurrentPosition.setPosition(newpositionC);
        precisionCircle.setCenter(newpositionC);
        precisionCircle.setRadius(Math.sqrt(message.position_covariance[0]+message.position_covariance[4]));
        oldCurrentPosition[i] = newpositionC;

        path = flightPath.getPath();
        path.push(oldCurrentPosition[i]);

        timeM = time;
        i++;
      }

    }

    index++; // update index

  });