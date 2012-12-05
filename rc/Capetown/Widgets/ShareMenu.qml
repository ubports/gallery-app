import QtQuick 2.0
import Ubuntu.Components.ListItems 0.1 as ListItem

Item {
    id: sharemenu
    property string picturePath

    height: childrenRect.height

    ListView {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: childrenRect.height

        interactive: false
        model: ListModel {
            ListElement { service: "Facebook"; user: "lolachang2010@yahoo.co.uk"; iconPath: "img/facebook.png" }
            ListElement { service: "Twitter"; user: "@lola_chang"; iconPath: "img/twitter.png" }
            ListElement { service: "Ubuntu One"; user: "lolachang"; iconPath: "img/ubuntuone.png" }
            ListElement { service: "Gmail"; user: "lolachang2010@gmail.com"; iconPath: "img/gmail.png" }
            ListElement { service: "Pinterest"; user: "lolachang2010@yahoo.co.uk"; iconPath: "img/pinterest.png" }
        }

        delegate: ListItem.Subtitled {
            text: service
            subText: user
            icon: Qt.resolvedUrl(iconPath)
            __iconHeight: units.gu(5)
            __iconWidth: units.gu(5)

            onClicked: {
                if (service == "Facebook") {
                    if (loader.status != Loader.Ready) console.log("Application launching not available on this platform");
                    else loader.item.switchToShareApplication(sharemenu.picturePath)
                } else {
                    console.log("Sharing to this service is not supported yet.")
                }
            }
        }
    }

    Loader {
        id: loader
        source: "UbuntuApplicationWrapper.qml"
    }
}
