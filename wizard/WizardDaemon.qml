// Copyright (c) 2019, Lethean Privacy
// Copyright (c) 2014-2015, The Monero Project
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import moneroComponents.WalletManager 1.0
import QtQuick 2.2
import QtQuick.Layouts 1.1
import "../components"
import "utils.js" as Utils

ColumnLayout {
    Layout.leftMargin: wizardLeftMargin
    Layout.rightMargin: wizardRightMargin

    id: daemonPage
    opacity: 0
    visible: false
    property alias titleText: titleText.text
    Behavior on opacity {
        NumberAnimation { duration: 100; easing.type: Easing.InQuad }
    }

    onOpacityChanged: visible = opacity !== 0

    function onPageOpened(settingsObject) {
        wizard.nextButton.enabled = true
        daemonPage.titleText = "Sync the entire blockchain?"
    }

    function onPageClosed(settingsObject) {
        return true
    }

    RowLayout {
        id: dotsRow
        Layout.alignment: Qt.AlignRight

        ListModel {
            id: dotsModel
            ListElement { dotColor: "#36B05B" }
            ListElement { dotColor: "#36B05B" }
            ListElement { dotColor: "#36B05B" }
            //ListElement { dotColor: "#FFE00A" }
            ListElement { dotColor: "#DBDBDB" }
        }

        Repeater {
            model: dotsModel
            delegate: Rectangle {
                // TODO: make this dynamic for all pages in wizard
                visible: true
                width: 12; height: 12
                radius: 6
                color: dotColor
            }
        }
    }

    ColumnLayout {
        id: headerColumn

        Text {
            Layout.fillWidth: true
            id: titleText
            font.family: "Arial"
            font.pixelSize: 28
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            //renderType: Text.NativeRendering
            color: "#3F3F3F"

        }

        Text {
            Layout.fillWidth: true
            Layout.bottomMargin: 30
            font.family: "Arial"
            font.pixelSize: 18
            wrapMode: Text.Wrap
            //renderType: Text.NativeRendering
            color: "#4A4646"
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("<br>The blockchain requires a great deal of storage space on your computer and requires several hours to download in most cases.<br/><br/>
                        A simpler and faster alternative is to use a remote daemon, which is another computer that securely stores the blockchain for you.<br/><br/>
                        If you can afford the storage space, running a full blockchain node offers more privacy and is a nice thing to do as it bolsters the Lethean network.<br/><br/>
                        <b>If you do not understand the above, make sure the below option is enabled!</b>")
                    + translationManager.emptyString
        }
    }

    Component.onCompleted: {
        parent.wizardRestarted.connect(onWizardRestarted)
    }

    RowLayout {
        Layout.leftMargin: wizardLeftMargin
        Layout.rightMargin: wizardRightMargin
        Layout.topMargin: 30
        Layout.alignment: Qt.AlignCenter
        Layout.fillWidth: true

        Rectangle {
            width: 200
            CheckBox {
                id: useRemoteDaemon
                text: qsTr("Use remote daemon") + translationManager.emptyString
                background: "#FFFFFF"
                fontColor: "#4A4646"
                fontSize: 16
                checkedIcon: "../images/checkedVioletIcon.png"
                uncheckedIcon: "../images/uncheckedIcon.png"
                checked: persistentSettings.daemon_address === 'seed.lethean.io:48782';
                onClicked: {
                    persistentSettings.daemon_address = useRemoteDaemon.checked ? 'seed.lethean.io:48782' : 'localhost:48782';
                    console.log("daemon address set to ", persistentSettings.daemon_address)
                }
            }
        }
    }
}
