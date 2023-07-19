/*
 * see3camcu83.qml -- extension settings for other cameras
 * Copyright © 2015  e-con Systems India Pvt. Limited
 *
 * This file is part of Qtcam.
 *
 * Qtcam is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * Qtcam is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qtcam. If not, see <http://www.gnu.org/licenses/>.
 */
import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.1
import econ.camera.uvcsettings 1.0
import QtQuick.Layouts 1.1
import cameraenum 1.0
import econ.camera.see3camcu83 1.0

Item{
    width:240
    height:720

    property int denoiseMin: 0
    property int denoiseMax: 15
    property int frameRateMin: 0
    property int frameRateMax: 60
    property int expoCompMin: 50
    property int expoCompMax: 1000000

    property bool skipUpdateUIDenoise: false
    property bool skipUpdateUIOnExpWindowSize: false
    property bool skipUpdateUIFrameRate: false
    property bool skipUpdateUIOnBurstLength: false
    property bool skipUpdateUIOnAntiFlickerMode:false

    property bool setButtonClicked: false

    signal destroyWindow()

    Timer {
        id: getCamValuesTimer
        interval: 500
        onTriggered: {
            see3camcu83.getExposureCompensation()
            see3camcu83.getFrameRateCtrlValue()
            stop()
        }
    }
    Timer {
        id: getexposureCompTimer
        interval: 500
        onTriggered: {
            see3camcu83.getExposureCompensation()
            stop()
        }
    }

    // Used when selecting auto exposure in image Quality settings menu
    Timer {
        id: getAutoExpsoureControlValues
        interval: 500
        onTriggered: {
            see3camcu83.getAutoExpROIModeAndWindowSize()
            stop()
        }
    }
    Connections
    {
        target: root
        onTakeScreenShot:
        {
            root.imageCapture(CommonEnums.BURST_SHOT);
        }
        onGetVideoPinStatus:
        {
            root.enableVideoPin(true);
        }
        onGetStillImageFormats:
        {
            var stillImageFormat = []
            stillImageFormat.push("jpg")
            stillImageFormat.push("bmp")
            stillImageFormat.push("raw")
            stillImageFormat.push("png")
            root.insertStillImageFormat(stillImageFormat);
        }
        onAutoExposureSelected:
        {
            enableDisableAutoExposureControls(autoExposureSelect)
        }
        onCreateIrWindow:{
            //To enable CheckBox when IR window is created by resolution switch
            irWindowCheckBox.checked = true

            //Enabling IR window CheckBox when switched to dual resolution
            irWindowCheckBox.enabled = true
            irWindowCheckBox.opacity = 1

            root.irPreviewWindow()
        }

        //signal for getting resolution from videostreaming.cpp
        onSendResolution:{
            if(((width == 4440)&&(height == 2160)) || ((width == 3120)&&(height == 1080)))
            {
                root.videoRecordBtnEnable(false)

                //Enable checkBox for IR window
                irWindowCheckBox.enabled = true
                irWindowCheckBox.opacity = 1
            }
            else
            {
                root.videoRecordBtnEnable(true)

                //Disable CheckBox for IR window
                irWindowCheckBox.enabled = false
                irWindowCheckBox.opacity = 0.1
            }
        }
        onVideoResolutionChanged:{
            getexposureCompTimer.start()
            getCamValuesTimer.start()
        }
        onPreviewFPSChanged:{
            getexposureCompTimer.start()
            getCamValuesTimer.start()
        }
        onVideoColorSpaceChanged:{
            getexposureCompTimer.start()
            getCamValuesTimer.start()
        }
        onWakeOnMotionSettings:{
            wakeOnMotion(isEnable)
        }
        onSetExpCompensation:{
            see3camcu83.setExposureCompensation(exposureCompValue.text)
        }

        //signal to uncheck irWindowCheckBox
        onIrWindowCloseBtnSignal:
        {
            irWindowCheckBox.checked = false
        }

        onMouseRightClicked:{
            if(autoexpManual.enabled && autoexpManual.checked){
                see3camcu83.setROIAutoExposure(See3Cam_CU83.AE_DISABLED, 0, 0, 0, 0, 0);
            }
        }
    }

    ScrollView
    {
        id: scrollview
        x: 10
        y: 189.5
        width: 257
        height: 500
        style: econscrollViewStyle

    Item{
        height: 1100
        ColumnLayout
        {
            x:2
            y:5
            spacing:20

        CheckBox
        {
            id: irWindowCheckBox
            activeFocusOnPress : true
            text: "IR Window"
            style: econCheckBoxStyle
            tooltip: "Enable this checkBox to create IR window"
            Layout.alignment: Qt.AlignCenter
            onClicked:{
                irWindowEnable()
            }
            Keys.onReturnPressed: {
                irWindowEnable()
            }
        }
        Text
        {
             id: special_effects
             text: "--- Special Effects ---"
             font.pixelSize: 14
             font.family: "Ubuntu"
             color: "#ffffff"
             smooth: true
             Layout.alignment: Qt.AlignCenter
             opacity: 0.50196078431373
        }
        Grid
        {
            columns: 2
            spacing: 20

            ExclusiveGroup { id: effectInputGroup }
            RadioButton
            {
                id: rdoEffectNormal
                style:  econRadioButtonStyle
                text:   qsTr("Normal")
                exclusiveGroup: effectInputGroup
                activeFocusOnPress: true
                onClicked: {
                    defaultValue.enabled = true
                    see3camcu83.setSpecialMode(See3Cam_CU83.EFFECT_NORMAL)
                }
                Keys.onReturnPressed:  {
                    defaultValue.enabled = true
                    see3camcu83.setSpecialMode(See3Cam_CU83.EFFECT_NORMAL)
                }
            }
            RadioButton
            {
                id: rdoEffectBW
                style:  econRadioButtonStyle
                text: qsTr("Black and White")
                exclusiveGroup: effectInputGroup
                activeFocusOnPress: true
                onClicked: {
                    see3camcu83.setSpecialMode(See3Cam_CU83.EFFECT_BLACK_WHITE)
                }
                Keys.onReturnPressed: {
                    see3camcu83.setSpecialMode(See3Cam_CU83.EFFECT_BLACK_WHITE)
                }
            }
            RadioButton
            {
                id: rdoEffectGreyScale
                style:  econRadioButtonStyle
                text: qsTr("GreyScale")
                exclusiveGroup: effectInputGroup
                activeFocusOnPress: true
                onClicked: {
                    see3camcu83.setSpecialMode(See3Cam_CU83.EFFECT_GREYSCALE)
                }
                Keys.onReturnPressed: {
                    see3camcu83.setSpecialMode(See3Cam_CU83.EFFECT_GREYSCALE)
                }
            }
            RadioButton
            {
                id: rdoEffectSketch
                style:  econRadioButtonStyle
                text: qsTr("Sketch")
                exclusiveGroup: effectInputGroup
                activeFocusOnPress: true
                onClicked: {
                    see3camcu83.setSpecialMode(See3Cam_CU83.EFFECT_SKETCH)
                }
                Keys.onReturnPressed: {
                    see3camcu83.setSpecialMode(See3Cam_CU83.EFFECT_SKETCH)
                }
            }
            RadioButton
            {
                id: rdoEffectNegative
                style:  econRadioButtonStyle
                text: qsTr("Negative")
                exclusiveGroup: effectInputGroup
                activeFocusOnPress: true
                onClicked:
                {
                    see3camcu83.setSpecialMode(See3Cam_CU83.EFFECT_NEGATIVE)
                }
                Keys.onReturnPressed:
                {
                    see3camcu83.setSpecialMode(See3Cam_CU83.EFFECT_NEGATIVE)
                }
            }
        }

        Text
        {
            id: denoiseText
            text: "--- De-Noise ---"
            font.pixelSize: 14
            font.family: "Ubuntu"
            color: "#ffffff"
            smooth: true
            Layout.alignment: Qt.AlignCenter
            opacity: 0.50196078431373
        }

        Row
        {
            spacing: 35
            Slider
            {
                activeFocusOnPress: true
                updateValueWhileDragging: false
                id: deNoiseSlider
                width: 150
                stepSize: 1
                style:econSliderStyle
                minimumValue: denoiseMin
                maximumValue: denoiseMax
                onValueChanged:
                {
                    deNoiseTextField.text = deNoiseSlider.value
                    if(skipUpdateUIDenoise)
                    {
                        see3camcu83.setDenoiseValue(deNoiseSlider.value)
                    }
                    skipUpdateUIDenoise = true
                }
            }
            TextField
            {
                id: deNoiseTextField
                text: deNoiseSlider.value
                font.pixelSize: 10
                font.family: "Ubuntu"
                smooth: true
                horizontalAlignment: TextInput.AlignHCenter
                style: econTextFieldStyle
                validator: IntValidator {bottom: deNoiseSlider.minimumValue; top: deNoiseSlider.maximumValue}
                onTextChanged:
                {
                    if(text.length > 0)
                    {
                        deNoiseSlider.value = deNoiseTextField.text
                    }
                }
            }
        }

        Text
        {
            id: roiAutoExpMode
            text: "--- ROI - Auto Exposure ---"
            font.pixelSize: 14
            font.family: "Ubuntu"
            color: "#ffffff"
            smooth: true
            Layout.alignment: Qt.AlignCenter
            opacity: 0.50196078431373
        }
        Grid
        {
              columns: 2
              spacing: 20
              ExclusiveGroup { id: roiExpogroup }

              RadioButton
              {
                  exclusiveGroup: roiExpogroup
                  id: autoexpDisabled
                  text: "Disable"
                  activeFocusOnPress: true
                  style: econRadioButtonStyle
                  opacity: enabled ? 1 : 0.1

                  // setROIAutoExposure() args:  mode, videoresolnWidth, videoresolnHeight, mouseXCord, mouseYCord, WinSize]
                  // videoresolnWidth, videoresolnHeight, mouseXCord, mouseYCord - these parameters are required only when click in preview]
                  // winSize is required only for manual mode

                  onClicked: {
                      see3camcu83.setROIAutoExposure(See3Cam_CU83.AE_DISABLED, 0, 0, 0, 0, 0);
                      autoExpoWinSizeCombo.enabled = false
                      autoExpoWinSizeCombo.opacity = 0.1

                  }
                  Keys.onReturnPressed: {
                      see3camcu83.setROIAutoExposure(See3Cam_CU83.AE_DISABLED, 0, 0, 0, 0, 0);
                      autoExpoWinSizeCombo.enabled = false
                      autoExpoWinSizeCombo.opacity = 0.1

                  }
              }
              RadioButton
              {
                  exclusiveGroup: roiExpogroup
                  id: autoexpFull
                  text: "Full"
                  activeFocusOnPress: true
                  style: econRadioButtonStyle
                  opacity: enabled ? 1 : 0.1

                  // setROIAutoExposure() args:  mode, videoresolnWidth, videoresolnHeight, mouseXCord, mouseYCord, WinSize]
                  // videoresolnWidth, videoresolnHeight, mouseXCord, mouseYCord - these parameters are required only when click in preview]
                  // winSize is required only for manual mode

                  onClicked: {
                      see3camcu83.setROIAutoExposure(See3Cam_CU83.FULL_ROI, 0, 0, 0, 0, 0);
                      autoExpoWinSizeCombo.enabled = false
                      autoExpoWinSizeCombo.opacity = 0.1
                  }
                  Keys.onReturnPressed: {
                      see3camcu83.setROIAutoExposure(See3Cam_CU83.FULL_ROI, 0, 0, 0, 0, 0);
                      autoExpoWinSizeCombo.enabled = false
                      autoExpoWinSizeCombo.opacity = 0.1
                  }
              }
              RadioButton
              {
                  exclusiveGroup: roiExpogroup
                  id: autoexpManual
                  text: "Manual"
                  activeFocusOnPress: true
                  style: econRadioButtonStyle
                  opacity: enabled ? 1 : 0.1
                  onClicked: {
                      see3camcu83.setROIAutoExposure(See3Cam_CU83.MANUAL_ROI, 0, 0, 0, 0, autoExpoWinSizeCombo.currentText);
                      autoExpoWinSizeCombo.enabled = true
                      autoExpoWinSizeCombo.opacity = 1
                  }
                  Keys.onReturnPressed: {
                      see3camcu83.setROIAutoExposure(See3Cam_CU83.MANUAL_ROI, 0, 0, 0, 0, autoExpoWinSizeCombo.currentText);
                      autoExpoWinSizeCombo.enabled = true
                      autoExpoWinSizeCombo.opacity = 1
                  }
              }
        }
        ComboBox
        {
            id: autoExpoWinSizeCombo
            enabled: (autoexpManual.enabled && autoexpManual.checked) ? true : false
            opacity: (autoexpManual.enabled && autoexpManual.checked) ? 1 : 0.1
            model: ListModel
                   {
                        ListElement { text: "1" }
                        ListElement { text: "2" }
                        ListElement { text: "3" }
                        ListElement { text: "4" }
                        ListElement { text: "5" }
                        ListElement { text: "6" }
                        ListElement { text: "7" }
                        ListElement { text: "8" }
                    }
            activeFocusOnPress: true
            style: econComboBoxStyle
            onCurrentIndexChanged: {
                if(skipUpdateUIOnExpWindowSize){
                    see3camcu83.setROIAutoExposure(See3Cam_CU83.MANUAL_ROI, 0, 0, 0, 0, autoExpoWinSizeCombo.currentText)
                }
                skipUpdateUIOnExpWindowSize = true
            }
        }

        Text
        {
            id: flipText
            text: "--- Flip Control ---"
            font.pixelSize: 14
            font.family: "Ubuntu"
            color: "#ffffff"
            smooth: true
            Layout.alignment: Qt.AlignCenter
            opacity: 0.50196078431373
        }
        Row
        {
            spacing: 40
            CheckBox
            {
                id: flipCtrlHorizotal
                activeFocusOnPress : true
                text: "Horizontal"
                style: econCheckBoxStyle
                onClicked:{
                    see3camcu83.setOrientation(flipCtrlHorizotal.checked, flipCtrlVertical.checked)
                }
                Keys.onReturnPressed: {
                    see3camcu83.setOrientation(flipCtrlHorizotal.checked, flipCtrlVertical.checked)
                }
            }
            CheckBox
            {
                id: flipCtrlVertical
                activeFocusOnPress : true
                text: "Vertical"
                style: econCheckBoxStyle
                onClicked:{
                    see3camcu83.setOrientation(flipCtrlHorizotal.checked, flipCtrlVertical.checked)
                }
                Keys.onReturnPressed: {
                    see3camcu83.setOrientation(flipCtrlHorizotal.checked, flipCtrlVertical.checked)
                }
            }
        }

        Text
        {
             id: exposureCompTextTitle
             text: "--- Exposure Compensation ---"
             font.pixelSize: 14
             font.family: "Ubuntu"
             color: "#ffffff"
             smooth: true
             Layout.alignment: Qt.AlignCenter
             opacity: 0.50196078431373
        }
        Row
        {
                spacing: 9
                Text
                {
                    id: exposureCompText
                    text: "value(µs)[50 - 1000000]"
                    font.pixelSize: 14
                    font.family: "Ubuntu"
                    color: "#ffffff"
                    smooth: true
                    width: 80
                    wrapMode: Text.WordWrap
                    opacity: 1
                }
                TextField
                {
                    id: exposureCompValue
                    font.pixelSize: 10
                    font.family: "Ubuntu"
                    smooth: true
                    horizontalAlignment: TextInput.AlignHCenter
                    opacity: 1
                    style: econTextFieldStyle
                    implicitHeight: 25
                    implicitWidth: 80
                    validator: IntValidator {bottom: expoCompMin; top: expoCompMax}
                }
                Button
                {
                    id: exposureCompSet
                    activeFocusOnPress : true
                    text: "Set"
                    tooltip: "You can set the required exposure compensation value by changing the value in the text box and click the Set button"
                    style: econButtonStyle
                    enabled: true
                    opacity: 1
                    implicitHeight: 25
                    implicitWidth: 60
                    onClicked:
                    {
                        exposureCompSet.enabled = false
                        setButtonClicked = true
                        see3camcu83.setExposureCompensation(exposureCompValue.text)
                        exposureCompSet.enabled = true
                    }
                    Keys.onReturnPressed:
                    {
                        exposureCompSet.enabled = false
                        setButtonClicked = true
                        see3camcu83.setExposureCompensation(exposureCompValue.text)
                        exposureCompSet.enabled = true
                    }
                }
            }

            Text
            {
                id: frameRateText
                text: "--- Frame Rate Control ---"
                font.pixelSize: 14
                font.family: "Ubuntu"
                color: "#ffffff"
                smooth: true
                Layout.alignment: Qt.AlignCenter
                opacity: 0.50196078431373
            }
            Row{
                spacing: 35
                Slider {
                    activeFocusOnPress: true
                    updateValueWhileDragging: false
                    id: frameRateSlider
                    width: 150
                    stepSize: 1
                    style:econSliderStyle
                    minimumValue: frameRateSlider.minimumValue
                    maximumValue: frameRateSlider.maximumValue
                    onValueChanged:  {
                        frameRateTextField.text = frameRateSlider.value
                        if(skipUpdateUIFrameRate){
                            see3camcu83.setFrameRateCtrlValue(frameRateSlider.value)
                        }
                        skipUpdateUIFrameRate = true
                    }
                }
                TextField {
                    id: frameRateTextField
                    text: frameRateSlider.value
                    font.pixelSize: 10
                    font.family: "Ubuntu"
                    smooth: true
                    horizontalAlignment: TextInput.AlignHCenter
                    style: econTextFieldStyle
                    validator: IntValidator {bottom: frameRateSlider.minimumValue; top: frameRateSlider.maximumValue}
                    onTextChanged: {
                        if(text.length > 0){
                            frameRateSlider.value = frameRateTextField.text
                        }
                    }
                }
            }

            Text
            {
                id: burstLength
                text: "Burst Length :"
                font.pixelSize: 14
                font.family: "Ubuntu"
                color: "#ffffff"
                smooth: true
                opacity: 1
            }
            ComboBox
            {
                id: burstLengthCombo
                opacity: 1
                enabled: true
                model: ListModel
                       {
                            ListElement { text: "1" }
                            ListElement { text: "2" }
                            ListElement { text: "3" }
                            ListElement { text: "4" }
                            ListElement { text: "5" }
                        }
                activeFocusOnPress: true
                style: econComboBoxStyle
                onCurrentIndexChanged: {
                    root.stillBurstLength(burstLengthCombo.currentIndex + 1) // combobox index starts from 0
                    if(skipUpdateUIOnBurstLength){
                        see3camcu83.setBurstLength(burstLengthCombo.currentText)
                    }
                    skipUpdateUIOnBurstLength = true
                }
            }

            Text
            {
                 id: wakeOnMode
                 text: "--- WakeOn Mode ---"
                 font.pixelSize: 14
                 font.family: "Ubuntu"
                 color: "#ffffff"
                 smooth: true
                 Layout.alignment: Qt.AlignCenter
                 opacity: 0.50196078431373
            }
            Row
            {
                  spacing:90

                ExclusiveGroup { id: wakeOnGroup }
                RadioButton
                {
                    id: wakeOnEnable
                    style:  econRadioButtonStyle
                    text:   qsTr("Enable")
                    exclusiveGroup: wakeOnGroup
                    activeFocusOnPress: true
                    onClicked: {
                        defaultValue.enabled = true
                        see3camcu83.setWakeOnMotion(See3Cam_CU83.ENABLE)
                    }
                    Keys.onReturnPressed:  {
                        defaultValue.enabled = true
                        see3camcu83.setWakeOnMotion(See3Cam_CU83.ENABLE)
                    }
                }
                RadioButton
                {
                    id: wakeOnDisable
                    style:  econRadioButtonStyle
                    text: qsTr("Disable")
                    exclusiveGroup: wakeOnGroup
                    activeFocusOnPress: true
                    onClicked: {
                        see3camcu83.setWakeOnMotion(See3Cam_CU83.DISABLE)
                    }
                    Keys.onReturnPressed: {
                        see3camcu83.setWakeOnMotion(See3Cam_CU83.DISABLE)
                    }
                }
            }

            Text
            {
                id: antiFlickerMode
                text: "--- Anti Flicker Mode ---"
                font.pixelSize: 14
                font.family: "Ubuntu"
                color: "#ffffff"
                smooth: true
                Layout.alignment: Qt.AlignCenter
                opacity: 0.50196078431373
            }
            Row
            {
                  spacing:90
                  ExclusiveGroup { id: antiFlickerModegroup }

                  RadioButton
                  {
                      exclusiveGroup: antiFlickerModegroup
                      id: antiFlickerModeAuto
                      text: "Auto"
                      activeFocusOnPress: true
                      style: econRadioButtonStyle
                      opacity: enabled ? 1 : 0.1
                      onClicked: {
                          defaultValue.enabled = true
                          see3camcu83.setAntiFlickerMode(See3Cam_CU83.MODE_AUTO);
                      }
                      Keys.onReturnPressed: {
                          defaultValue.enabled = true
                          see3camcu83.setAntiFlickerMode(See3Cam_CU83.MODE_AUTO);
                      }
                  }
                  RadioButton
                  {
                      exclusiveGroup: antiFlickerModegroup
                      id: antiFlickerModeManual
                      text: "Manual"
                      activeFocusOnPress: true
                      style: econRadioButtonStyle
                      opacity: enabled ? 1 : 0.1
                      onClicked: {
                          defaultValue.enabled = true
                          setAntiFlickerMode()
                      }
                      Keys.onReturnPressed: {
                          defaultValue.enabled = true
                          setAntiFlickerMode()
                      }
                  }
            }

            Text
            {
                id: frequency
                text: "Frequency :"
                font.pixelSize: 14
                font.family: "Ubuntu"
                color: "#ffffff"
                smooth: true
                opacity: (antiFlickerModeManual.enabled && antiFlickerModeManual.checked) ? 1 : 0.1
            }
            ComboBox
            {
                id: antiFlickerCombo
                enabled: (antiFlickerModeManual.enabled && antiFlickerModeManual.checked) ? true : false
                opacity: (antiFlickerModeManual.enabled && antiFlickerModeManual.checked) ? 1 : 0.1
                model: ListModel
                       {
                            ListElement { text: "AUTO" }
                            ListElement { text: "50 Hz" }
                            ListElement { text: "60 Hz" }
                            ListElement { text: "DISABLE" }
                       }
                activeFocusOnPress: true
                style: econComboBoxStyle
                onCurrentIndexChanged: {
                    if(skipUpdateUIOnAntiFlickerMode){
                        setAntiFlickerMode()
                    }
                    skipUpdateUIOnAntiFlickerMode = true
                }
            }

            Row{
                Layout.alignment: Qt.AlignCenter
                Button {
                    id: defaultValue
                    opacity: 1
                    activeFocusOnPress : true
                    text: "Default"
                    tooltip: "Click to set default values in extension controls"
                    style: econButtonStyle
                    onClicked:{
                        setToDefaultValues()
                    }
                    Keys.onReturnPressed: {
                        setToDefaultValues()
                    }
                }
            }

            Row{
                Button {
                    id: f_wversion_selected130
                    opacity: 1
                    activeFocusOnPress : true
                    tooltip: "Click to view the firmware version of the camera"
                    style: ButtonStyle {
                        background: Rectangle {
                            border.width: control.activeFocus ? 3 :0
                            color: "#222021"
                            border.color: control.activeFocus ? "#ffffff" : "#222021"
                            radius: 5
                        }
                        label: Image {
                            source: "images/f_wversion_selected.png"
                        }
                    }
                    onClicked: {
                        getFirmwareVersion()
                    }
                    Keys.onReturnPressed: {
                        getFirmwareVersion()
                    }
                }
                Button {
                    id: serial_no_selected
                    opacity: 1
                    activeFocusOnPress : true
                    tooltip: "Click to view the Serial Number"
                    style: ButtonStyle {
                        background: Rectangle {
                            border.width: control.activeFocus ? 3 :0
                            color: "#222021"
                            border.color: control.activeFocus ? "#ffffff" : "#222021"
                            radius: 5
                        }
                        label: Image {
                            source: "images/serial_no_selected.png"
                        }
                    }
                    onClicked: {
                        getSerialNumber()
                    }
                    Keys.onReturnPressed: {
                        getSerialNumber()
                    }
                }
            }

        }
          }
            }
    Component {
        id: econRadioButtonStyle
        RadioButtonStyle {
            label: Text {
                text: control.text
                font.pixelSize: 14
                font.family: "Ubuntu"
                color: "#ffffff"
                smooth: true
                opacity: 1
            }
            background: Rectangle {
                color: "#222021"
                border.color: control.activeFocus ? "#ffffff" : "#222021"
            }
        }
    }
    Component {
        id: econscrollViewStyle
        ScrollViewStyle {
            scrollToClickedPosition: true
            handle: Image {
                id: scrollhandle
                source: "images/scroller.png"
            }
            scrollBarBackground: Image {
                id: scrollStyle
                source: "images/Scroller_bg.png"
            }
            incrementControl: Image {
                id: increment
                source: "images/down_arrow.png"
            }
            decrementControl: Image {
                id: decrement
                source: "images/up_arrow.png"
            }
        }
    }
    Component {
        id: econComboBoxStyle
        ComboBoxStyle {
            background: Image {
                id: combo_bkgrnd
                source: "../../Views/images/device_box.png"
                Rectangle {
                    width: combo_bkgrnd.sourceSize.width  - 28
                    height: combo_bkgrnd.sourceSize.height
                    color: "#222021"
                    border.color: "white"
                    border.width: control.activeFocus ? 3 : 1
                    radius: control.activeFocus ? 5 : 0
                }
            }
            label:  Text{
                anchors.fill: parent
                color: "#ffffff"
                elide: Text.ElideRight
                text: control.currentText
                verticalAlignment: Text.AlignVCenter
                maximumLineCount: 1
                font.family: "Ubuntu"
                font.pixelSize: 14
            }
        }
    }
    Component {
        id: econCheckBoxStyle
        CheckBoxStyle {
            label: Text {
                text: control.text
                font.pixelSize: 14
                font.family: "Ubuntu"
                color: "#ffffff"
                smooth: true
                opacity: 1
            }
            background: Rectangle {
                color: "#222021"
                border.color: control.activeFocus ? "#ffffff" : "#222021"
            }
        }
    }

    Component {
        id: econTextFieldStyle
        TextFieldStyle {
            textColor: "black"
            background: Rectangle {
                radius: 2
                implicitWidth: 50
                implicitHeight: 20
                border.color: "#333"
                border.width: 2
                y: 1
            }
        }
    }

    Component {
        id: econButtonStyle
        ButtonStyle {
            background: Rectangle {
                implicitHeight: 38
                implicitWidth: 104
                border.width: control.activeFocus ? 3 :0
                color: "#e76943"
                border.color: control.activeFocus ? "#ffffff" : "#222021"
                radius: control.activeFocus ? 5 : 0
            }
            label: Text {
                color: "#ffffff"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.family: "Ubuntu"
                font.pointSize: 10
                text: control.text
            }
        }
    }

    Uvccamera {
        id: uvccamera
        onTitleTextChanged: {
            messageDialog.title = _title.toString()
            messageDialog.text = _text.toString()
            messageDialog.open()
        }
        onSerialNumber:{
            messageDialog.title = qsTr("Serial Number")
            messageDialog.text = serialNumber;
        }
    }

    See3Cam_CU83{
        id: see3camcu83
        onSpecialModeReceived:
        {
            switch(specialMode)
            {
                case See3Cam_CU83.EFFECT_NORMAL:
                    rdoEffectNormal.checked = true
                    break
                case See3Cam_CU83.EFFECT_BLACK_WHITE:
                    rdoEffectBW.checked = true
                    break
                case See3Cam_CU83.EFFECT_GREYSCALE:
                    rdoEffectGreyScale.checked = true
                    break
                case See3Cam_CU83.EFFECT_NEGATIVE:
                    rdoEffectNegative.checked = true
                    break
                case See3Cam_CU83.EFFECT_SKETCH:
                    rdoEffectSketch.checked = true
                    break
            }
        }

        onDenoiseValueReceived:
        {
            skipUpdateUIDenoise = false
            deNoiseSlider.value = denoiseValue
            skipUpdateUIDenoise = true
        }

        onRoiAutoExpModeRecieved:
        {
            currentROIAutoExposureMode(roiMode, winSize)
        }

        onFlipMirrorModeReceived:
        {
            currentFlipMirrorMode(flipMode)
        }

        onExposureCompValueRecieved:
        {
             exposureCompValue.text = exposureCompVal
        }

        onFrameRateCtrlValueRecieved:
        {
            skipUpdateUIFrameRate = false
            frameRateSlider.value = frameRateVal
            skipUpdateUIFrameRate = true
        }
        onMinimumFramesReceived: {
            frameRateSlider.minimumValue = minimumFps
        }
        onMaximumFramesReceived: {
            frameRateSlider.maximumValue = maximumFps
        }

        onBurstLengthValueRecieved:
        {
            skipUpdateUIOnBurstLength = false
            burstLengthCombo.currentIndex = burstLenVal - 1
            skipUpdateUIOnBurstLength = true
        }

        onAntiFlickerModeRecieved:
        {
            switch(antiFlickerVal)
            {
                case See3Cam_CU83.MODE_AUTO:
                    antiFlickerModeAuto.checked = true
                    break
                case See3Cam_CU83.MODE_50Hz:
                    antiFlickerModeManual.checked = true
                    skipUpdateUIOnAntiFlickerMode = false
                    antiFlickerCombo.currentIndex = 0
                    skipUpdateUIOnAntiFlickerMode = true
                    break
                case See3Cam_CU83.MODE_60Hz:
                    antiFlickerModeManual.checked = true
                    skipUpdateUIOnAntiFlickerMode = false
                    antiFlickerCombo.currentIndex = 1
                    skipUpdateUIOnAntiFlickerMode = true
                    break
                case See3Cam_CU83.MODE_DISABLE://need to check the flags
                    antiFlickerModeManual.checked = true
                    skipUpdateUIOnAntiFlickerMode = false
                    antiFlickerCombo.currentIndex = 2
                    skipUpdateUIOnAntiFlickerMode = true
                    break
             }
        }

        onWakeonModeReceived: {
            setWakeOnMode(wakeOn)
        }

        //Signal for command Prompt
        onIndicateCommandStatus:{
            if(setButtonClicked){
                displayMessageBox(title, text)
                setButtonClicked = false
            }
        }
        onIndicateExposureValueRangeFailure:{
            if(setButtonClicked){
                displayMessageBox(title, text)
                setButtonClicked = false
                see3camcu83.getExposureCompensation()
            }
        }
    }

    function currentROIAutoExposureMode(roiMode, winSize)
    {
        switch(roiMode){
            case See3Cam_CU83.FULL_ROI:
                autoexpFull.checked = true
                //To disable comboBox in full roi mode
                autoExpoWinSizeCombo.enabled = false
                autoExpoWinSizeCombo.opacity = 0.1
                break
            case See3Cam_CU83.MANUAL_ROI:
                skipUpdateUIOnExpWindowSize = false
                autoexpManual.enabled = true
                autoexpManual.checked = true
                //To enable comboBox in manual mode
                autoExpoWinSizeCombo.enabled = true
                autoExpoWinSizeCombo.opacity = 1
                // If window size is got from camera is 0 then set window size to 1 in UI
                if(winSize == 0){
                    autoExpoWinSizeCombo.currentIndex = 0
                }else
                    autoExpoWinSizeCombo.currentIndex = winSize-1
                break
            case See3Cam_CU83.AE_DISABLED:
                autoexpFull.enabled = false
                autoexpManual.enabled = false
                //To disable comboBox in Disable mode
                autoExpoWinSizeCombo.enabled = false
                autoExpoWinSizeCombo.opacity = 0.1
                break
        }
    }

    function currentFlipMirrorMode(mode)
    {
        switch(mode)
        {
        case See3Cam_CU83.VERTICAL:
            flipCtrlVertical.checked = true
            flipCtrlHorizotal.checked = false
            break;
        case See3Cam_CU83.HORIZONTAL:
            flipCtrlVertical.checked = false
            flipCtrlHorizotal.checked = true
            break;
        case See3Cam_CU83.BOTH:
            flipCtrlVertical.checked = true
            flipCtrlHorizotal.checked = true
            break;
        default:
            flipCtrlVertical.checked = false
            flipCtrlHorizotal.checked = false
            break;
        }
    }

    function setWakeOnMode(mode)
    {
        switch(mode)
        {
        case See3Cam_CU83.ENABLE:
            wakeOnEnable.checked  = true
            wakeOnDisable.checked = false
            break;
        case See3Cam_CU83.DISABLE:
            wakeOnEnable.checked  = false
            wakeOnDisable.checked = true
            break;
        }
    }

    function wakeOnMotion(isEnable)
    {
        if(isEnable)
        {
            wakeOnEnable.enabled  = true
            wakeOnEnable.opacity  = 1
            wakeOnDisable.enabled = true
            wakeOnDisable.opacity = 1
        }
        else
        {
            wakeOnEnable.enabled  = false
            wakeOnEnable.opacity  = 0.1
            wakeOnDisable.enabled = false
            wakeOnDisable.opacity = 0.1
        }
    }

    function setAntiFlickerMode()
    {
        if(antiFlickerCombo.currentIndex === 0)
            see3camcu83.setAntiFlickerMode(See3Cam_CU83.MODE_AUTO)
        else if(antiFlickerCombo.currentIndex === 1)
            see3camcu83.setAntiFlickerMode(See3Cam_CU83.MODE_50Hz)
        else if(antiFlickerCombo.currentIndex === 2)
            see3camcu83.setAntiFlickerMode(See3Cam_CU83.MODE_60Hz)
        else if(antiFlickerCombo.currentIndex === 3)
            see3camcu83.setAntiFlickerMode(See3Cam_CU83.MODE_DISABLE)
    }

    function enableDisableAutoExposureControls(autoExposureSelect){
        if(autoExposureSelect){
            //To enable exposure compensation when device is in manual exposure mode in UVC
            root.enableDisableExposureCompensation(autoExposureSelect)

            autoexpManual.enabled = true
            autoexpFull.enabled = true
            if(autoexpManual.checked)
                autoExpoWinSizeCombo.enabled = true
            if(autoexpFull.checked)
                autoExpoWinSizeCombo.enabled = false
            autoexpManual.opacity = 1
            autoexpFull.opacity = 1
            exposureCompValue.enabled = true
            exposureCompValue.opacity = 1
            exposureCompSet.enabled = true
            exposureCompSet.opacity = 1
            exposureCompText.opacity = 1
        }else{
            //To disable exposure compensation when device is in manual exposure mode in UVC
            root.enableDisableExposureCompensation(autoExposureSelect)

            autoexpManual.enabled = false
            autoexpFull.enabled = false
            autoExpoWinSizeCombo.enabled = false
            autoexpManual.opacity = 0.1
            autoexpFull.opacity = 0.1
            exposureCompValue.enabled = false
            exposureCompValue.opacity = 0.1
            exposureCompSet.enabled = false
            exposureCompSet.opacity = 0.1
            exposureCompText.opacity = 0.1
        }
        getAutoExpsoureControlValues.start()
    }

    function displayMessageBox(title, text){
        messageDialog.title = qsTr(title)
        messageDialog.text = qsTr(text)
        messageDialog.open()
    }

    function getSerialNumber() {
        uvccamera.getSerialNumber()
        messageDialog.open()
    }

    function getFirmwareVersion() {
        uvccamera.getFirmWareVersion()
        messageDialog.open()
    }

    //DefaultValue
    function setToDefaultValues(){
        root.checkForTriggerMode(false)
        root.captureBtnEnable(true)
        root.videoRecordBtnEnable(false)
        defaultValue.enabled = false

        if(see3camcu83.setToDefaultValues()){
            getCurrentValuesFromCamera()
        }
        defaultValue.enabled = true
    }

    function getCurrentValuesFromCamera(){
        getCamValuesTimer.start()
        see3camcu83.getSpecialMode()
        see3camcu83.getDenoiseValue()
        see3camcu83.getAutoExpROIModeAndWindowSize()
        see3camcu83.getOrientation()
        see3camcu83.getBurstLength()
        see3camcu83.getAntiFlickerMode()
        see3camcu83.getWakeOnMotion()
    }

    //function to create and destroy IR window via CheckBox
    function irWindowEnable()
    {
       if(irWindowCheckBox.checked)
       {
           root.irPreviewWindow()
           root.irCheckboxStatus(true)
       }
       else
       {
           root.closeIrWindow()
           root.irCheckboxStatus(false)
       }
    }

    Component.onCompleted: {
        getCurrentValuesFromCamera()
    }
    Component.onDestruction: {
        // Stopping stillTimeOutTimer when this Component is destroyed
        vidstreamproperty.stopStillTimeOutTimer();
    }
}
