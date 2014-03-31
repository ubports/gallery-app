import QtQuick 2.0

MultiPointTouchArea {
    signal clicked(var mouse)
    signal doubleClicked(var mouse)

    Timer {
        id: dblTapTimeout
        interval: 600
        repeat: false
        onTriggered: dblTapHandler.pressCount = 0
    }

    touchPoints: TouchPoint {
        id: dblTapHandler
        property int pressCount: 0
        onPressedChanged: {
            if (pressed) {
                pressCount++;
                dblTapTimeout.running = true;
            } else {
                if (pressCount == 1) {
                    clicked({ x: dblTapHandler.x, y: dblTapHandler.y });
                } else if (pressCount === 2) {
                    doubleClicked({ x: dblTapHandler.x, y: dblTapHandler.y });
                    pressCount = 0;
                    dblTapTimeout.stop();
                }
            }
        }
    }
}
