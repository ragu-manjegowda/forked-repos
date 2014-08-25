var markerHome = 0;
var flightPath = 0;
var markerCurrentPosition = 0;
var precisionCircle = 0;
var oldCurrentPosition = Array();

function initialize() {
var starting = new google.maps.LatLng(47.413157, 8.548645); //Zurich

var mapOptions = {
	center: starting,
	zoom: 18,
	mapTypeId: 'satellite'
};

map = new google.maps.Map(document.getElementById("map-canvas"),mapOptions);
map.setTilt(0); //disable 45° view

}

function putHome(lat, lng)
{
	var position = new google.maps.LatLng(lat,lng);
	var Home = 'images/homeMarker.png';

	markerHome = new google.maps.Marker({
		position: position,
		map: map,
		title: "Home",
		icon : Home
	});
}

function startVisualization(lat, lng)
{
	var position =  new google.maps.LatLng(lat,lng);

	var CurPos = {
		url: 'images/simpleMarker.png',
		origin: new google.maps.Point(0, 0),
		//anchor: new google.maps.Point(20, 20), //use this to set the anchor point in the center of the image, comment it out to anchor in the bottom
	};

	markerCurrentPosition = new google.maps.Marker({
		position: position,
		map: map,
		title: "Drone",
		icon : CurPos,
		zIndex: google.maps.Marker.MAX_ZINDEX + 1 // Current-position marker always on top
	});

	precisionCircle = new google.maps.Circle({
		strokeColor:"#FFFFFF",
		strokeOpacity:1,
		strokeWeight:2,
		fillColor:"#0000FF",
		fillOpacity:0.2
	});

	flightPath = new google.maps.Polyline({
		path: oldCurrentPosition,
		geodesic: true,
		strokeColor: '#FFFF00',
		strokeOpacity: 1.0,
		strokeWeight: 2
	});

	flightPath.setMap(map);
	precisionCircle.setMap(map);

}

google.maps.event.addDomListener(window, 'load', initialize);