import QtQuick 2.0
import Ubuntu.Application 0.1

Item {
    function switchToShareApplication(argument) {
        console.log("Launching the share application with argument: " + argument)
        applicationManager.focusFavoriteApplication(ApplicationManager.Share, argument)
    }

    function switchToCameraApplication() {
        console.log("Launching the camera application.");
        ApplicationManager.focusFavoriteApplication(ApplicationManager.Camera, "");
    }
}
