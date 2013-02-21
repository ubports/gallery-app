/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

import QtQuick 2.0
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.OnlineAccounts 0.1
import Gallery 1.0

/*!
*/
Item {
    id: sharemenu

    /*!
    */
    property string picturePath
    /*!
    */
    signal selected()

    height: childrenRect.height

    AccountServiceModel {
        id: accounts
        serviceType: "microblogging"
        includeDisabled: true
    }

    ListView {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: childrenRect.height

        interactive: false
        model: accounts
        delegate: Item {
            width: parent.width
            height: childrenRect.height
            AccountService {
                id: accts
                objectHandle: accountService
            }
                ListItem.Subtitled {
                text: accts.provider.displayName
                subText: displayName
                icon: "image://gicon/"+accts.provider.iconName
                __iconHeight: units.gu(5)
                __iconWidth: units.gu(5)

                onClicked: {
                    sharemenu.selected()
                    if (accts.provider.displayName == "Facebook") {
                        shareFile.writeShareFile(accountId, shareMenu.picturePath);
                        if (loader.status != Loader.Ready) console.log("Application launching not available on this platform");
                        else loader.item.switchToShareApplication(accountId);
                    } else {
                        console.log("Sharing to this service is not supported yet.")
                    }
                }
            }
        }
    }

    ShareFile {
        id: shareFile
    }

    Loader {
        id: loader
        source: "UbuntuApplicationWrapper.qml"
    }
}
