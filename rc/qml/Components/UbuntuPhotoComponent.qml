import QtQuick 2.0
import Ubuntu.Components 0.1

Item {
  id: ubuntuPhotoComponent

  property alias mediaSource: photoComponent.mediaSource
  property alias load: photoComponent.load
  property alias ownerName: photoComponent.ownerName
  property alias isCropped: photoComponent.isCropped
  property alias isPreview: photoComponent.isPreview
  property alias isAnimate: photoComponent.isAnimate

  property alias radius: shape.radius

  UbuntuShape {
    id: shape
    anchors.fill: parent
    image: photoComponent.image
  }

  GalleryPhotoComponent {
    id: photoComponent
    anchors.fill: parent

    // This looks weird here, but the UbuntuShape being visible and tracking
    // this thing's Image component will get it on the screen anyway.
    visible: false
  }
}
