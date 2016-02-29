var myAPIKey = 'b8891693df7f4b9626085a4448711974';

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // Construct URL
  var url = "http://api.openweathermap.org/data/2.5/weather?lat=" +
      pos.coords.latitude + "&lon=" + pos.coords.longitude + '&appid=' + myAPIKey;

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      // Temperature in Kelvin requires adjustment
      var temperature = Math.round(json.main.temp - 273.15);
      console.log("Temperature is " + temperature);

      // Conditions
      var conditions = json.weather[0].main;      
      console.log("Conditions are " + conditions);
      
      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_TEMPERATURE": temperature,
        "KEY_CONDITIONS": conditions
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
    }      
  );
}

function locationError(err) {
  console.log("Error requesting location!");
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

    // Get the initial weather
    getWeather();
  }
);

Pebble.addEventListener('showConfiguration', 
  function(e) {
    //var url = 'http://localhost:8080';
    var url = 'http://jmsunseri.github.io/DCCompact/config/'
    
    
    console.log("Showing configuration page");
    
    Pebble.openURL(url);
  }
);

Pebble.addEventListener('webviewclosed', 
  function(e) {
    var configData = JSON.parse(decodeURIComponent(e.response));
    
    console.log("Configuration page returned: " + JSON.stringify(configData));
    
    
    if(configData.backgroundColor){
        var object = {
            backgroundColor: parseInt(configData.backgroundColor, 16),
            twentyFourHourFormat: configData.twentyFourHourFormat,
            hourColor: parseInt(configData.hourColor, 16),
            metricUnits: configData.metricUnits,
            minuteColor:parseInt(configData.minuteColor, 16),
            dateTextColor:parseInt(configData.dateTextColor, 16),
            tempTextColor:parseInt(configData.tempTextColor, 16),
            conditionsColor:parseInt(configData.conditionsColor, 16)      
        };
        Pebble.sendAppMessage(object, function() {
            console.log('Send successful! ' + JSON.stringify(object));
        }, function() {
            console.log('Send failed!');
        })
    }
  }
);


// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getWeather();
  }                     
);
