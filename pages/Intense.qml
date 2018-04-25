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

import QtQuick 2.0

import moneroComponents.Wallet 1.0
import moneroComponents.WalletManager 1.0
import moneroComponents.TransactionHistory 1.0
import moneroComponents.TransactionInfo 1.0
import moneroComponents.TransactionHistoryModel 1.0

import "../components"

Rectangle {
    id: root
    property var model

    QtObject {
        id: d
        property bool initialized: false
    }

    color: "#F0EEEE"



    Label {
          visible: !isMobile
          id: typeText
          anchors.left: parent.left
          anchors.top:  parent.top
          anchors.leftMargin: 17
          anchors.topMargin: 17
          width: 156
          text: qsTr("Type") + translationManager.emptyString
          fontSize: 14
      }

      ListModel {
          id: typeTransaction
          ListElement { column1: "VPN"; column2: ""; value: TransactionInfo.Direction_Both }
          ListElement { column1: "PROXY"; column2: ""; value: TransactionInfo.Direction_Out }
      }

      StandardDropdown {
          visible: !isMobile
          id: typeDrop
          anchors.left: parent.left
          anchors.top: typeText.bottom
          anchors.leftMargin: 17
          anchors.topMargin: 5
          width: 156
          shadowReleasedColor: "#4D0051"
          shadowPressedColor: "#2D002F"
          releasedColor: "#6B0072"
          pressedColor: "#4D0051"
          dataModel: typeTransaction
          z: 100
      }

      Label {
          visible: !isMobile
          id: maxPriceText
          anchors.left: typeText.right
          anchors.top:  parent.top
          anchors.leftMargin: 17
          anchors.topMargin: 17
          width: 156
          text: qsTr("Max Price") + translationManager.emptyString
          fontSize: 14
      }

      LineEdit {
          visible: !isMobile
          id: maxPriceLine
          anchors.left: typeDrop.right
          anchors.top: maxPriceText.bottom
          anchors.leftMargin: 17
          anchors.topMargin: 5
          width: 156
      }

      Label {
          visible: !isMobile
          id: minSpeedText
          anchors.left: maxPriceText.right
          anchors.top: parent.top
          anchors.leftMargin: 17
          anchors.topMargin: 17
          width: 156
          text: qsTr("Min Speed") + translationManager.emptyString
          fontSize: 14
      }

      LineEdit {
          visible: !isMobile
          id: minSpeedLine
          anchors.left: maxPriceLine.right
          anchors.top: minSpeedText.bottom
          anchors.leftMargin: 17
          anchors.topMargin: 5
          width: 156

      }

      CheckBox {
          visible: !isMobile
          id: favoriteFilter
          text: qsTr("Favorite") + translationManager.emptyString
          anchors.left: minSpeedLine.right
          anchors.bottom: parent.top
          anchors.leftMargin: 17
          checkedIcon: "../images/checkedVioletIcon.png"
          uncheckedIcon: "../images/uncheckedIcon.png"
          onClicked: {
          }
      }

/*
      CheckBox {
          visible: !isMobile
          id: advancedFilter
          text: qsTr("Advanced Search") + translationManager.emptyString
          anchors.left: favoriteFilter.right
          anchors.bottom: parent.top
          anchors.leftMargin: 17
          checkedIcon: "../images/checkedVioletIcon.png"
          uncheckedIcon: "../images/uncheckedIcon.png"
          onClicked: {
              if(checked) tableRect.height = Qt.binding(function(){ return tableRect.collapsedHeight })
              else tableRect.height = Qt.binding(function(){ return tableRect.middleHeight })
          }
      }
*/

      StandardButton {
          visible: !isMobile
          id: filterButton
          anchors.bottom: parent.top
          anchors.left: favoriteFilter.right
          anchors.leftMargin: 17
          width: 60
          text: qsTr("Filter") + translationManager.emptyString
          shadowReleasedColor: "#4D0051"
          shadowPressedColor: "#2D002F"
          releasedColor: "#6B0072"
          pressedColor: "#4D0051"
          onClicked:  {
              // Apply filter here;

              resetFilter(model)
              /*
              if (fromDatePicker.currentDate > toDatePicker.currentDate) {
                  console.error("Invalid date filter set: ", fromDatePicker.currentDate, toDatePicker.currentDate)
              } else {
                  model.dateFromFilter  = fromDatePicker.currentDate
                  model.dateToFilter    = toDatePicker.currentDate
              }

              if (advancedFilteringCheckBox.checked) {
                  if (amountFromLine.text.length) {
                      model.amountFromFilter = parseFloat(amountFromLine.text)
                  }

                  if (amountToLine.text.length) {
                      model.amountToFilter = parseFloat(amountToLine.text)
                  }

                  var directionFilter = transactionsModel.get(transactionTypeDropdown.currentIndex).value
                  console.log("Direction filter: " + directionFilter)
                  model.directionFilter = directionFilter
              }

              selectedAmount.text = getSelectedAmount()
              */
          }
      }

    Rectangle {
        id: tableRect
        property int expandedHeight: parent.height - parent.y - parent.height - 5
        property int middleHeight: parent.height - maxPriceLine.y - maxPriceLine.height - 17
        property int collapsedHeight: parent.height - typeDrop.y - typeDrop.height - 17
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
//        anchors.top: parent.top
        color: "#FFFFFF"
        z: 1

        height: (isMobile)? parent.height : middleHeight
        onHeightChanged: {
            if(height === middleHeight) z = 1
            else if(height === collapsedHeight) z = 0
            else z = 3
        }

        Behavior on height {
            NumberAnimation { duration: 200; easing.type: Easing.InQuad }
        }

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: 1
            color: "#DBDBDB"
        }

        ListView {
                id: listView
                anchors.fill: parent
                model: listModel
                delegate: Rectangle {
                    width: listView.width
                    height: listView.height / 4

                    Text {
                        text: listdata
                        anchors.centerIn: parent
                    }
                }
            }

            ListModel {
                id: listModel

                Component.onCompleted: {
                    var xmlhttp = new XMLHttpRequest();
                    var url = "https://jhx4eq5ijc.execute-api.us-east-1.amazonaws.com/dev/v1/services/search";

                    xmlhttp.onreadystatechange=function() {
                        if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
                            jsonParse(xmlhttp.responseText);
                            function jsonParse(response) {
                                console.log(response + "meu log")
                                var arr = JSON.parse(response);
                                console.log("numero de loop: " + arr.length)
                                for(var i = 0; i < arr.length; i++) {
                                    listView.model.append( {listdata: arr[i].provider +" "+ arr[i].name +" "+ arr[i].mStability +" "+ arr[i].vpn +" "+ arr[i].cost +" "+ arr[i].downloadSpeed})
                                }
                            }
                        }
                    }
                    xmlhttp.open("GET", url, true);
                    xmlhttp.send();
                }

            }




/*
        ListModel {
            id: columnsModel

            ListElement { columnName: "Provider"; columnWidth: 127 }
            ListElement { columnName: "Plan"; columnWidth: 140 }
            ListElement { columnName: "Rating"; columnWidth: 90 }
            ListElement { columnName: "Type"; columnWidth: 90 }
            ListElement { columnName: "Price per minute"; columnWidth: 178 }
            ListElement { columnName: "Speed"; columnWidth: 178 }
            ListElement { columnName: "Favorite"; columnWidth: 148 }
            ListElement { columnName: "Action"; columnWidth: 148 }

        }

        IntenseHeader {
            id: header
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: 17
            anchors.leftMargin: 14
            anchors.rightMargin: 14
            dataModel: columnsModel
            offset: 20
            onSortRequest: {
                console.log("column: " + column + " desc: " + desc)
                switch (column) {
                case 0:
                    // Payment ID
                    model.sortRole = TransactionHistoryModel.TransactionPaymentIdRole
                    break;
                case 1:
                    // Date (actually sort by timestamp as we want to have transactions sorted within one day as well);
                    model.sortRole = TransactionHistoryModel.TransactionTimeStampRole
                    break;
                case 2:
                    // BlockHeight;
                    model.sortRole = TransactionHistoryModel.TransactionBlockHeightRole
                    break;
                case 3:
                    // Amount;
                    model.sortRole = TransactionHistoryModel.TransactionAmountRole
                    break;

                case 4:
                    // Amount;
                    model.sortRole = TransactionHistoryModel.TransactionAmountRole
                    break;

                case 5:
                    // Amount;
                    model.sortRole = TransactionHistoryModel.TransactionAmountRole
                    break;

                case 6:
                    // Amount;
                    model.sortRole = TransactionHistoryModel.TransactionAmountRole
                    break;

                case 7:
                    // Amount;
                    model.sortRole = TransactionHistoryModel.TransactionAmountRole
                    break;

                case 8:
                    // Amount;
                    model.sortRole = TransactionHistoryModel.TransactionAmountRole
                    break;
                }
                model.sort(0, desc ? Qt.DescendingOrder : Qt.AscendingOrder)
            }
        }
        */
 /*
        Scroll {
            id: flickableScroll
            anchors.right: table.right
            anchors.rightMargin: -14
            anchors.top: table.top
            anchors.bottom: table.bottom
            flickable: table
        }



        IntenseTable {
            id: table
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: header.bottom
            anchors.bottom: parent.bottom
            anchors.leftMargin: 14
            anchors.rightMargin: 14
            onContentYChanged: flickableScroll.flickableContentYChanged()
            model: root.model
            addressBookModel: null
        }
        */
    }

    function onPageCompleted() {

        //table.addressBookModel = appWindow.currentWallet ? appWindow.currentWallet.addressBookModel : null
    }
}


/*

        StandardButton {
            id: getAll
            width: 80
            shadowReleasedColor: "#983CFF"
            shadowPressedColor: "#B32D00"
            releasedColor: "#813CFF"
            pressedColor: "#983CFF"
            text: qsTr("Refresh");
            onClicked: {
                var xmlhttp = new XMLHttpRequest();
                var url = "https://jhx4eq5ijc.execute-api.us-east-1.amazonaws.com/dev/v1/services/search";

                xmlhttp.onreadystatechange=function() {
                    if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
                        var obj = xmlhttp.responseText
                        console.log("meu log: " + obj)
                    }
                }
                xmlhttp.open("GET", url, true);
                xmlhttp.send();
            }
        }

        */
