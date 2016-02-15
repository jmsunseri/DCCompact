(function() {
  loadOptions();
  submitHandler();
})();

function submitHandler() {
  var $submitButton = $('#submitButton');

  $submitButton.on('click', function() {
    console.log('Submit');

    var return_to = getQueryParam('return_to', 'pebblejs://close#');
    document.location = return_to + encodeURIComponent(JSON.stringify(getAndStoreConfigData()));
  });
}

function loadOptions() {
    var $backgroundColorPicker = $('#backgroundColorPicker');
    var $twentyFourHourFormatCheckbox = $('#twentyFourHourFormatCheckbox');
    var $metricUnitsCheckbox = $('#metricUnitsCheckbox');
    //var $themeSelectList = $('#themeSelectList');
    var $hourColorPicker = $('#hourColorPicker');
    var $minuteColorPicker = $('#minuteColorPicker');
    //var $frameColorPicker = $('#frameColorPicker');
    //var $dateCircleBackgroundColorPicker = $('#dateCircleBackgroundColorPicker');
    var $dateTextColorPicker = $('#dateTextColorPicker');
    //var $tempBackgroundColorPicker = $('#tempBackgroundColorPicker');
    var $tempTextColorPicker = $('#tempTextColorPicker');
    var $conditionsColorPicker = $('#conditionsColorPicker');

    if (localStorage.backgroundColor) {
        $backgroundColorPicker[0].value = localStorage.backgroundColor;
        $twentyFourHourFormatCheckbox[0].checked = localStorage.twentyFourHourFormat === 'true';
        $metricUnitsCheckbox[0].checked = localStorage.metricUnits === 'true';
        //$themeSelectList.value = localStorage.theme;
        $hourColorPicker[0].value = localStorage.hourColor;
        $minuteColorPicker[0].value = localStorage.minuteColor;
        //$frameColorPicker.value = localStorage.frameColor;
        //$dateCircleBackgroundColorPicker.value = localStorage.dateCircleBackgroundColor;
        $dateTextColorPicker[0].value = localStorage.dateTextColor;
        //$tempBackgroundColorPicker.value = localStorage.tempBackgroundColor;
        $tempTextColorPicker[0].value =  localStorage.tempTextColor; 
        $conditionsColorPicker[0].value = localStorage.conditionsColor;
    }
}

function getAndStoreConfigData() {
  var $backgroundColorPicker = $('#backgroundColorPicker');
  var $twentyFourHourFormatCheckbox = $('#twentyFourHourFormatCheckbox');
  var $metricUnitsCheckbox = $('#metricUnitsCheckbox');
  //var $themeSelectList = $('#themeSelectList');
  var $hourColorPicker = $('#hourColorPicker');
  var $minuteColorPicker = $('#minuteColorPicker');
  //var $frameColorPicker = $('#frameColorPicker');
  //var $dateCircleBackgroundColorPicker = $('#dateCircleBackgroundColorPicker');
  var $dateTextColorPicker = $('#dateTextColorPicker');
  //var $tempBackgroundColorPicker = $('#tempBackgroundColorPicker');
  var $tempTextColorPicker = $('#tempTextColorPicker');
  var $conditionsColorPicker = $('#conditionsColorPicker');
  
  
  var options = {
    backgroundColor: $backgroundColorPicker.val(),
    twentyFourHourFormat: $twentyFourHourFormatCheckbox[0].checked,
    metricUnits: $metricUnitsCheckbox[0].checked,
    //theme: $themeSelectList.val(),
    hourColor: $hourColorPicker.val(),
    minuteColor: $minuteColorPicker.val(),
    //frameColor: $frameColorPicker.val(),
    //dateCircleBackgroundColor: $dateCircleBackgroundColorPicker.val(),
    dateTextColor: $dateTextColorPicker.val(),
    //tempBackgroundColor: $tempBackgroundColorPicker.val(),
    tempTextColor: $tempTextColorPicker.val(),
    conditionsColor: $conditionsColorPicker.val()   
  };

  localStorage.backgroundColor = options.backgroundColor;
  localStorage.twentyFourHourFormat = options.twentyFourHourFormat;
  localStorage.metricUnits = options.metricUnits;
  //localStorage.theme = options.theme;
  localStorage.hourColor = options.hourColor;
  localStorage.minuteColor = options.minuteColor;
  //localStorage.frameColor = options.frameColor;
  //localStorage.dateCircleBackgroundColor = options.dateCircleBackgroundColor;
  localStorage.dateTextColor = options.dateTextColor;
  //localStorage.tempBackgroundColor = options.tempBackgroundColor;
  localStorage.tempTextColor = options.tempTextColor;
  localStorage.conditionsColor = options.conditionsColor;

  console.log('Got options: ' + JSON.stringify(options));
  return options;
}

function getQueryParam(variable, defaultValue) {
  var query = location.search.substring(1);
  var vars = query.split('&');
  for (var i = 0; i < vars.length; i++) {
    var pair = vars[i].split('=');
    if (pair[0] === variable) {
      return decodeURIComponent(pair[1]);
    }
  }
  return defaultValue || false;
}
