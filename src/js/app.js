Pebble.addEventListener('ready', function() {
	console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('showConfiguration', function() {
	//var url = 'http://127.0.0.1:8080/greenclock-face-config/';
	var url = 'http://ksbckr.github.io/dotted-watchface-config/';

	console.log('Showing configuration page: ' + url);

	Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
	var configData = JSON.parse(decodeURIComponent(e.response));

	console.log('Configuration page returned: ' + JSON.stringify(configData));

	var dict = {
		textColor: parseInt(configData.textColor, 16),
		bgColor: parseInt(configData.bgColor, 16)
	};

	if (configData.textColor) {
		Pebble.sendAppMessage(dict, function() {
			console.log('Send successful! ' +  JSON.stringify(dict));
		}, function() {
			console.log('Send failed!');
		});
	}
});