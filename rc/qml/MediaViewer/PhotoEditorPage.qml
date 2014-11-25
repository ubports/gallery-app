import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.Extras 0.1 as Extras

Page {
    id: page
    property string photo
    signal done(bool photoWasModified)

    title: i18n.tr("Edit Photo")

    head.backAction: Action {
        iconName: "back"
        onTriggered: editor.close(true)
    }
    head.actions: editor.actions

    Extras.PhotoEditor {
        id: editor
        anchors.fill: parent
        onClosed: page.done(photoWasModified)
    }

    onActiveChanged: {
        if (active) editor.open(page.photo)
    }
}
