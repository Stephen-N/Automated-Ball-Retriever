//UI shapes
var path = new Path();
path.strokeColor = 'red';
path.strokeWidth = 1;
var rektList = []
var wasFirst = true
var firstPoint = new Point(0, 0)
//Robot and Stats
var robot = new Raster('robotIMG.bmp') //path goes here
var robotOmega = 0
var robotVelocity = 0
var maxVelocity = 5
var maxOmega = 2
var velocityDelta = 5
var omegaDelta = 2
var stop = false;

//Usage Data
var coalition = []
var frameCounter = 0
var lagReset = 40

//---- Control ----
var routine = 'manual'
var doDampenVel = false
var doDampenOmeg = false

//Virtual Rays
var virtualRays = []
var virtualRayDistances = []

//Real Rays
var realRays = []
var realRayDistance = []

//Bounce
var bAngle = 0;
var isAtWall = false;
var BfreeCounter = 0;

//Discovery

//Snake
var targetHorizonalAngle = 0
var lastHorizontal = 0
var vertDistance = 0
var vertTarget = 25
var moveVert = true


robot.onLoad = function () {
    console.log('The raster has loaded.');
    for (var i = 0; i < view.size.width; i++) {
        var row = [];
        for (var j = 0; j < view.size.height; j++) {
            row.push(false);
        }
        coalition.push(row);
    }
    robot.position = new Point((view.size.width) / 2, (view.size.height) / 2)
    for (var ang = 0; ang < 8; ang += 1) {
        var virRay = makeVirtualRay(ang * 45)
        virtualRays.push(virRay)
        virtualRayDistances.push(0)
        realRayDistance.push(0)
    }
};

// Section START: functionality for buttons and information exchange
var wrapper = document.getElementById('counter');
var counter;
var count = 0;

function start(val) {
    var url = "savedata.php";

    // counter = setInterval(function () {
    if (val == 'w') {
        console.log("Robot Velocity: " + robotVelocity);
        console.log("doDampenVel: " + doDampenVel);
        $.ajax({
            type: 'POST',
            url: url,
            data: { data: "forward" },
            success: function (result) {
                console.log(result); //  response by PHP file.
            },
            error: function () {
                console.log('error');
            }
        });
    }
    if (val == 's') {
        $.ajax({
            type: 'POST',
            url: url,
            data: { data: "backward" },
            success: function (result) {
                console.log(result); // response by PHP file.
            },
            error: function () {
                console.log('error');
            }
        });
    }
    if (val == 'a') {
        $.ajax({
            type: 'POST',
            url: url,
            data: { data: "left" },
            success: function (result) {
                console.log(result); //  response by PHP file.
            },
            error: function () {
                console.log('error');
            }
        });
    }
    if (val == 'd') {
        $.ajax({
            type: 'POST',
            url: url,
            data: { data: "right" },
            success: function (result) {
                console.log(result); // response by PHP file.
            },
            error: function () {
                console.log('error');
            }
        });
    }
    console.log("Button value: " + val);
    // }, 100);
}

function end() {
    clearInterval(counter)
    console.log("end robotVelocity: " + robotVelocity);
    console.log("end doDampenVel: " + doDampenVel);
}

window.setInterval(readStorage, 10);

function readStorage() {
    // Retrieve Robot information
    var contents = localStorage.getItem("file-content");
    if (contents.trim() && contents != "") {
        document.getElementById("from-storage").innerHTML = contents;
    } else {
        console.log("File is empty");
    }
}
// Section END: functionality for buttons and information exchange

function showIntersections(path1, path2) {
    var intersections = path1.getIntersections(path2);
    // alert(intersections.length)
    for (var i = 0; i < intersections.length; i++) {
        new Path.Circle({
            center: intersections[i].point,
            radius: 15,
            fillColor: '#009dec'
        });
    }
}

function virtualIntersectionsHelper(path1, path2) {
    var intersections = path1.getIntersections(path2);
    if (intersections.length == 0) {
        return null;
    }
    var point;
    var minDis = 9007199254740991; //max int
    for (var i = 0; i < intersections.length; i++) {
        var tempPoint = intersections[i].point

        var xDif = tempPoint.x - robot.position.x
        var yDif = tempPoint.y - robot.position.y
        var distance = Math.sqrt(Math.pow(xDif, 2) + Math.pow(yDif, 2))
        //alert(distance + ' = ' + xDif + ', ' + yDif)

        if (distance < minDis) {
            minDis = distance;
            point = tempPoint;
        }
    }
    //alert(point.x + ', ' + point.y + ', ' + minDis)
    return { x: point.x, y: point.y, dis: minDis }
}

function degToRad(num) {
    return num / 180 * Math.PI
}

function findEdgeHelper(angle) {
    var slope = Math.tan(degToRad(angle));

    var xIntersect;
    if (angle >= 0 && angle < 180) {
        xIntersect = view.size.width; // Line intersects with the right edge of the rectangle
    } else {
        xIntersect = 0; // Line intersects with the left edge of the rectangle
    }

    var yIntersect = robot.position.y + (xIntersect - robot.position.x) * slope;

    if (yIntersect < 0) {
        yIntersect = 0; // Line intersects with the top edge of the rectangle
        xIntersect = robot.position.x + (yIntersect - robot.position.y) / slope;
    } else if (yIntersect > view.size.height) {
        yIntersect = view.size.height; // Line intersects with the bottom edge of the rectangle
        xIntersect = robot.position.x + (yIntersect - robot.position.y) / slope;
    }

    return { x: xIntersect, y: yIntersect };
}

function makeVirtualRay(angle) {
    var virRay = new Path();
    var endPoint = findEdgeHelper(angle);
    virRay.segments = [[robot.position.x, robot.position.y], [endPoint.x, endPoint.y]];
    virRay.selected = true;
    virRay.strokeColor = 'blue';
    virRay.strokeWidth = 3;
    virRay.strokeCap = 'round';
    virRay.dashArray = [10, 12];
    return virRay;
}

function makeRealRay(angle) {
    var realRay = new Path();
    var endPoint = findEdgeHelper(angle);
    realRay.segments = [[robot.position.x, robot.position.y], [endPoint.x, endPoint.y]];
    realRay.selected = true;
    realRay.strokeColor = 'yellow';
    realRay.strokeWidth = 5;
    realRay.strokeCap = 'round';
    realRay.dashArray = [7, 10];
    return realRay;
}

function updateRays() {
    //Virtual Draw
    for (var ray = 0; ray < 8; ray += 1) {
        var endPoint = findEdgeHelper(Math.abs(robot.rotation) % 45 + 45 * ray)
        virtualRays[ray].segments = [[robot.position.x, robot.position.y], [endPoint.x, endPoint.y]]
    }

    for (var i = 0; i < 8; i += 1) {
        for (var j = 0; j < rektList.length; j += 1) {
            var inter = virtualIntersectionsHelper(virtualRays[i], rektList[j]);
            if (inter == null) {
                continue;
            } else {
                //alert('Inter\n' + inter);
                virtualRays[i].segments = [[robot.position.x, robot.position.y], [inter.x, inter.y]];
                virtualRayDistances[i] = inter.dis;
            }
        }
    }

}

function readOut(isRay) {
    var mainInfo = '\nCurrent Velocity: ' + robotVelocity.toFixed(1) + ', Current Omega: ' + robotOmega.toFixed(1) + '\nCurrent Location: (' + robot.position.x.toFixed(1) + ', ' + robot.position.y.toFixed(1) + ') , Current Angle: ' + robot.rotation.toFixed(1)
    mainInfo += '\nStop: ' + stop + ' Bounce: ' + bAngle + ' Tar: ' + targetHorizonalAngle + ' mvVert: ' + moveVert + 'vertD: ' + vertDistance;

    /*var targetHorizonalAngle = 0
    var lastHorizontal = 0
    var vertDistance = 0
    var vertTarget = 25
    var moveVert = true*/

    if (isRay) {
        var rayInfo = ''
        for (var i = 0; i < 8; i += 1) {
            var rayNum = i + 1;
            rayNym = Math.round(rayNum) + ': ';
            if (i != 7) {
                rayInfo += rayNym + virtualRayDistances[i].toFixed(1) + ',  '
            } else {
                rayInfo += rayNym + virtualRayDistances[i].toFixed(1)
            }
        }
        alert('Full Readout! -- Advanced:' + mainInfo + '\nVirtualRays: ' + rayInfo)
    } else {
        alert('Basic Readout:' + mainInfo)
    }
}

function onMouseDown(event) {
    if (wasFirst) {
        firstPoint = new Point(event.point.x, event.point.y)
        wasFirst = false
    }
    // add event.point to the path
    path.add(event.point)
    // make the path smooth
    path.smooth({ type: 'continuous' });
}

function onMouseMove(event) {
    if (!wasFirst) {
        path.add(event.point)
        path.smooth({ type: 'continuous' });
    }
}

function onMouseUp(event) {
    var orgin = new Point(firstPoint.x, firstPoint.y)
    var size = new Size((event.point.x - firstPoint.x), (event.point.y - firstPoint.y))
    var rekt = new Path.Rectangle(orgin, size);
    rekt.strokeColor = 'red'
    rekt.fillColor = 'black'
    rekt.strokeWidth = 3
    rektList.push(rekt)

    wasFirst = true
    path.clear()
    updateVirtualNoGo()
}

function onKeyDown(event) {
    //alert(event.key)
    // event.key returns the name of the pressed button on your keyboard
    if (event.key == 'z') {

        rektList[rektList.length - 1].clear()
        rektList.pop()
        updateVirtualNoGo()
    }
    if (event.key == 'm') {
        routine = 'manual';
    }
    if (event.key == 'b') {
        routine = 'bounce';
        stop = true;
    }
    if (event.key == 'm') {
        routine = 'manual';
        stop = true;
    }
    if (event.key == 'n') {
        routine = 'discovery';
        stop = true;
    }
    if (event.key == 'v') {
        routine = 'snake';
        targetHorizonalAngle = 0
        stop = true;
    }
    if (routine == 'manual') {
        if (event.key == 'w' || event.key == 'up') {
            robotVelocity = robotVelocity - velocityDelta
            doDampenVel = false
            // start(event.key)
        }
        if (event.key == 's' || event.key == 'down') {
            robotVelocity = robotVelocity + velocityDelta
            doDampenVel = false
            // start(event.key)
        }
        if (event.key == 'a' || event.key == 'left') {
            robotOmega = robotOmega - omegaDelta
            doDampenOmeg = false
            // start(event.key)
        }
        if (event.key == 'd' || event.key == 'right') {
            robotOmega = robotOmega + omegaDelta
            doDampenOmeg = false
            // start(event.key)
        }
    }
    if (event.key == 'i') {
        readOut(false)
    }
    if (event.key == 'o') {
        readOut(true)
    }
    if (event.key == 'r') {
        robot.position = new Point((view.size.width) / 2, (view.size.height) / 2)
        robot.rotation = 0
        // alert('Moved to default position!')
    }
    if (event.key == 't') {
        if (rektList.length < 1) {
            var scW = view.size.width
            var scH = view.size.height

            var topLeft = new Point(scW / 4, scH / 4)
            var topRight = new Point(3 * scW / 4, scH / 4)
            var botLeft = new Point(scW / 4, 3 * scH / 4)

            var sizeLong = new Size(scW / 2, 10)
            var sizeTall = new Size(10, scH / 2)
            //Top
            var rekt1 = new Path.Rectangle(topLeft, sizeLong);
            rekt1.strokeColor = 'red'
            rekt1.fillColor = 'black'
            rekt1.strokeWidth = 3
            rektList.push(rekt1)
            //Right
            var rekt2 = new Path.Rectangle(topRight, sizeTall);
            rekt2.strokeColor = 'red'
            rekt2.fillColor = 'black'
            rekt2.strokeWidth = 3
            rektList.push(rekt2)
            //Bot
            var rekt3 = new Path.Rectangle(botLeft, sizeLong);
            rekt3.strokeColor = 'red'
            rekt3.fillColor = 'black'
            rekt3.strokeWidth = 3
            rektList.push(rekt3)
            //Left
            var rekt4 = new Path.Rectangle(topLeft, sizeTall);
            rekt4.strokeColor = 'red'
            rekt4.fillColor = 'black'
            rekt4.strokeWidth = 3
            rektList.push(rekt4)

            updateVirtualNoGo()
        }
    }
}

function onKeyUp(event) {
    if (routine == 'manual') {
        if (event.key == 'w' || event.key == 'up') {
            doDampenVel = true
        }
        if (event.key == 's' || event.key == 'down') {
            doDampenVel = true
        }
        if (event.key == 'a' || event.key == 'left') {
            doDampenOmeg = true
        }
        if (event.key == 'd' || event.key == 'right') {
            doDampenOmeg = true
        }
        // end(event.key)
    }
}

function clearMatrix() {
    for (var i = 0; i < view.size.width; i++) {
        for (var j = 0; j < view.size.height; j++) {
            coalition[i][j] = false
        }
    }
}

function updateVirtualNoGo() {
    clearMatrix()
    for (var l = 0; l < rektList.length; l++) {
        var bounds = rektList[l].bounds
        var xLim = Math.floor(bounds.x + bounds.width)
        var yLim = Math.floor(bounds.y + bounds.height)

        for (var i = Math.floor(bounds.x); i < xLim; i++) {
            for (var j = Math.floor(bounds.y); j < yLim; j++) {
                if ((i < 0 || i >= view.size.width) || (j < 0 || j >= view.size.height)) {
                    continue
                } else {
                    coalition[i][j] = true;
                }
            }
        }
    }
}

function isObstacle(point) {
    var isInWay = false
    var xLim = Math.floor(point.x + maxVelocity * 2)
    var yLim = Math.floor(point.y + maxVelocity * 2)

    for (var i = Math.floor(point.x - maxVelocity * 2); i < xLim; i++) {
        for (var j = Math.floor(point.y - maxVelocity * 2); j < yLim; j++) {
            if ((i < 0 || i >= view.size.width) || (j < 0 || j >= view.size.height)) {
                isInWay = true
                return true
            } else {
                //if()
                isInWay = isInWay || coalition[i][j]
            }
        }
    }
    return isInWay
}

function balenceSpeed() {
    if (robotOmega > maxOmega) {
        // alert('omega too high!');
        // readOut(false);
    }

    if (stop) {
        doDampenOmeg = true;
        doDampenVel = true;
        if (robotVelocity == 0 && robotOmega == 0) {
            // alert('Stop Completed! :)')
            stop = false;
        }
    }
    if (robotVelocity < -1 * maxVelocity) {
        robotVelocity = -1 * maxVelocity
    }
    if (robotVelocity > maxVelocity) {
        robotVelocity = maxVelocity
    }
    if (robotOmega < -1 * maxOmega) {
        robotOmega = -1 * maxOmega
    }
    if (robotOmega > maxOmega) {
        robotOmega = maxOmega
    }
    if (doDampenVel) {
        robotVelocity = 0.95 * robotVelocity
        if (Math.abs(robotVelocity) < 1) {
            robotVelocity = 0
        }
    }
    if (doDampenOmeg) {
        robotOmega = 0.5 * robotOmega
        if (Math.abs(robotOmega) < 0.2) {
            robotOmega = 0
        }
    }
    // export velocity and omega
    var url = "savedata.php";
    var str_robotVelocity = robotVelocity.toString();
    var str_doDampenVel = doDampenVel.toString();
    var str_data = str_robotVelocity.concat(" ", str_doDampenVel);
    console.log("String data: " + str_data);
    $.ajax({
        type: 'POST',
        url: url,
        data: { data: str_data },
        success: function (result) {
            console.log(result); //  response by PHP file.
        },
        error: function () {
            console.log('error');
        }
    });
}

function onFrame(event) {
    //Frame Checking Optimisation
    frameCounter = frameCounter + 1
    if (frameCounter > lagReset) {
        updateVirtualNoGo()
        frameCounter = 0
    }

    //Speed Balencing
    balenceSpeed();

    lastLocation = robot.position
    var xVelocity = robotVelocity * Math.cos(degToRad(90 + robot.rotation))
    var yVelcoity = robotVelocity * Math.sin(degToRad(90 + robot.rotation))
    nextLocation = new Point(lastLocation.x + xVelocity, lastLocation.y + yVelcoity)
    var nextLocObstacle = isObstacle(nextLocation);

    if (stop) {
        stop = true;
    }
    else if (routine == 'manual' && !nextLocObstacle) {
        robot.position = nextLocation
        robot.rotation = robot.rotation + robotOmega
        updateRays()
    }
    else if (routine == 'bounce') {
        /*if(BfreeCounter >= 0){
            alert('B:' + BfreeCounter)
        }*/
        if (nextLocObstacle && !isAtWall) {
            if (BfreeCounter <= 0) {
                bAngle += 30 * Math.floor(1 + Math.random() * 2)
                isAtWall = true;
                // alert('Conflict! Setting to: ' + bAngle)
                BfreeCounter = 10;
            }
        }

        if (robot.rotation == bAngle && !isAtWall) {
            robotVelocity += velocityDelta
        } else if (bAngle < robot.rotation) {
            robotOmega -= omegaDelta;
            balenceSpeed();
        } else if (maxOmega < (robot.rotation - bAngle)) {
            robotOmega += omegaDelta;
            balenceSpeed();
        } else {
            robotOmega = bAngle - robot.rotation
        }

        BfreeCounter--;

        if (isAtWall) {
            robot.rotation = robot.rotation + robotOmega;
            updateRays();
            if (bAngle == robot.rotation) {
                isAtWall = false;
                //alert('Arrived: ' + bAngle)
            }
        } else if (!nextLocObstacle) {
            robot.position = nextLocation;
            updateRays();
        }
    }
    else if (routine == 'snake') { //targetHorizonalAngle | lastHorizontal = -90 (left) or 90(right) or 180(down)
        if (nextLocObstacle) {
            vertDistance = 0;
            moveVert = true;
            stop = true;
            if (targetHorizonalAngle == 0) {
                lastHorizontal = 0;
                targetHorizonalAngle = 0;
            } else {
                lastHorizontal = targetHorizonalAngle;
                targetHorizonalAngle = 180;
            }
        }

        if (robot.rotation != targetHorizonalAngle) {
            if (targetHorizonalAngle < robot.rotation) {
                robotOmega -= omegaDelta;
                balenceSpeed();
            } else if (maxOmega < (robot.rotation - targetHorizonalAngle)) {
                robotOmega += omegaDelta;
                balenceSpeed();
            } else {
                robotOmega = targetHorizonalAngle - robot.rotation
            }
            robot.rotation = robot.rotation + robotOmega
        } else {
            if (vertDistance > vertTarget && targetHorizonalAngle != 0) {
                // alert('Finished moving')
                stop = true;
                moveVert = false;
                switch (lastHorizontal) {
                    case 0:
                        targetHorizonalAngle = 90;
                        break;
                    case 90:
                        targetHorizonalAngle = -90;
                        break;
                    case -90:
                        targetHorizonalAngle = 90;
                        break;
                    default:
                        stop = true;
                        routine = 'error';
                        alert('ERROR!!!\nSnake: Invalid Target Angle!')
                }
            }

            if (!stop) {
                if (!nextLocObstacle) {
                    robot.position = nextLocation

                    updateRays()
                }

                if (robot.rotation == targetHorizonalAngle) {
                    robotVelocity += velocityDelta;
                    if (targetHorizonalAngle == 0 || targetHorizonalAngle == 180) {
                        vertDistance += robotVelocity;
                    }
                }
            }
        }
    }
}