var robot = new Raster('/home/garrettvanetten/hci/robotIMG.bmp') //path goes here

robot.onLoad = function() {
    console.log('The raster has loaded.');
};

var path = new Path();
path.strokeColor = 'red';
path.strokeWidth = 5;

var path = new Path();
path.strokeColor = 'black';
path.strokeWidth = 2;

var rektList = []
var wasFirst = true
var firstPoint = new Point(0,0)

function onMouseDown(event) {
	if(wasFirst){
		firstPoint = new Point(event.x, event.y)
	}
    // add event.point to the path
    path.add(event.point)
    // make the path smooth
    path.smooth({ type: 'continuous' });
}

function onMouseUp(event){
	var orgin = new Point((firstPoint.x - event.x)/2, (firstPoint.y - event.y)/2)
	var size = new Size(firstPoint.x - event.x, firstPoint.y - event.y)
	var rekt = new Path.Rectangle(orgin, size);
	rekt.strokeColor='orange'
	rekt.fillColor='black'
	rekt.strokeWidth=5
	rektList.push(rekt)
}

function onKeyDown(event) {
	// event.key returns the name of the pressed button on your keyboard
	// check if space is pressed then set move to true
	// also set counter to 0
	if(event.key == 'space'){
		counter = 0
		move = true
	}
}