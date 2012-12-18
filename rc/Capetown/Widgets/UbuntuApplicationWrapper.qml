import QtQuick 2.0
import Ubuntu.Application 0.1

Item {
    function switchToShareApplication() {
        console.log("Launching the share application.");
        ApplicationManager.focusFavoriteApplication(ApplicationManager.ShareApplication);
    }

    function switchToCameraApplication() {
        console.log("Launching the camera application.");
        ApplicationManager.focusFavoriteApplication(ApplicationManager.CameraApplication);
    }
}
