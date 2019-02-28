import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.1
import econ.camera.uvcsettings 1.0
import econ.camera.storagecamcu135 1.0

import QtQuick.Layouts 1.1
import cameraenum 1.0
import "../../JavaScriptFiles/tempValue.js" as JS

Item {
    id: storagecamcu135Id
    width:268
    height:750
    property int denoiseMin: 0
    property int denoiseMax: 15
    property int qFactorMin: 10
    property int qFactorMax: 96
    property int iHDRMin: 1
    property int iHDRMax: 4
    property int frameRateMin: 1
    property int frameRateMax: 120
    property int expoCompMin: 8000
    property int expoCompMax: 1000000

    property int stillFormatCurrentIndex
    property int stillResolutionCurrentIndex
    property bool skipUpdateUIOnSetttings : false
    property bool skipUpdateUIQFactor : false
    property bool skipUpdateUIDenoise : false
    property bool skipUpdateUIFrameRate: false
    property bool skipUpdateUIiHDR: false
    property bool skipUpdateUIOnExpWindowSize: false
    property bool setButtonClicked: false

    // Used when selecting auto exposure in image Quality settings menu

    Timer {
        id: getAutoExpsoureControlValues
        interval: 1000
        onTriggered: {
            storagecamcu135.getAutoExpROIModeAndWindowSize()
            stop()
        }
    }

    Timer {
        id: getexposureCompFrameRateCtrlTimer
        interval: 500
        onTriggered: {
            storagecamcu135.getExposureCompensation()
            storagecamcu135.getFrameRateCtrlValue()
            stop()
        }
    }



    Action {
        id: setDefault
        onTriggered:
        {
            setToDefaultValues()
        }
    }

    Action {
        id: firmwareVersion
        onTriggered:
        {
            getFirmwareVersion()
        }
    }

    Action {
        id: serialNumber
        onTriggered: {
            getSerialNumber()
        }
    }

    Timer {
        id: enableSettings
        interval: 3000
        onTriggered: {
            root.enableAllSettingsTab()
            stop()
        }
    }

    Timer {
        id: snapShotTimer
        interval: 1000
        onTriggered: {
            root.imageCapture(CommonEnums.SNAP_SHOT);
            stop()
        }
    }

    Timer{
        id:setMaster
        interval:1000
        onTriggered: {
            setMasterOnDemandMode()
        }
    }

    Connections
    {
        target: root
        onTakeScreenShot:
        {
            if(storagecamcu135.setFaceRectState(false)){ // true - enable/ false - disable
                snapShotTimer.start()
            }
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
        onMouseRightClicked:{
            if(autoexpManual.enabled && autoexpManual.checked){
                storagecamcu135.setROIAutoExposure(Storagecamcu135.AutoExpManual, width, height, x, y, autoExpoWinSizeCombo.currentText)
            }
        }
        onAutoExposureSelected:{
            enableDisableAutoExposureControls(autoExposureSelect)
        }
        onVideoResolutionChanged:{
            getexposureCompFrameRateCtrlTimer.start()
        }
        onPreviewFPSChanged:{
            getexposureCompFrameRateCtrlTimer.start()
        }
        onVideoColorSpaceChanged:{
            getexposureCompFrameRateCtrlTimer.start()
        }

        onEnableFaceRectafterBurst:{
            storagecamcu135.setFaceRectState(true)
        }

        onBeforeRecordVideo:{
            storagecamcu135.setFaceRectState(false)
        }
        onAfterRecordVideo:{
            storagecamcu135.setFaceRectState(true)
        }
        onQueryFrame:{
            setMaster.start()
            if((streamMasterOnDemand.checked || streamHwTrigger.checked)){
                if(retriveframe){
                    if(!storagecamcu135.grabStillFrame(frameIndexCombo.currentIndex, stillFormatCurrentIndex+1, stillResolutionCurrentIndex+1)){
                        displayMessageBox("Failure", "Image is not available in given index")
                    }
                }else{
                    storagecamcu135.grabPreviewFrame();
                }
            }
        }

        onStillFormatChanged:{
            stillFormatCurrentIndex = stillFormatcurrentIndex
            stillResolutionCurrentIndex = stillResolncurrentIndex
            storagecamcu135.setStillResolution(stillFormatCurrentIndex+1, stillResolutionCurrentIndex+1);
            storagecamcu135.getNumberOfFramesCanStore(stillFormatCurrentIndex+1, stillResolutionCurrentIndex+1)
        }

        onStillResolutionChanged:{
            stillResolutionCurrentIndex = stillresolutionIndex
            stillFormatCurrentIndex = stillFormatIndex
            storagecamcu135.setStillResolution(stillFormatCurrentIndex+1, stillResolutionCurrentIndex+1);
            storagecamcu135.getNumberOfFramesCanStore(stillFormatCurrentIndex+1, stillResolutionCurrentIndex+1)
        }

        onCaptureFrameTimeout:{
            if(streamMasterOnDemand.checked){
                root.enableAllSettingsTab()
                storagecamcu135.grabPreviewFrame()
            }
        }

        onExtensionTabVisible:{
            if(visible){
                frameIndexCombo.currentIndex = 0
            }
        }

    }

    ScrollView{
        id: scrollview
        x: 10
        y: 189.5
        width: 257
        height: 500
        style: econscrollViewStyle
        Item{
            height:1800
            ColumnLayout{
                x:2
                y:5
                spacing:20
                Text {
                    id: effectMode
                    text: "--- Effect Mode ---"
                    font.pixelSize: 14
                    font.family: "Ubuntu"
                    color: "#ffffff"
                    smooth: true
                    Layout.alignment: Qt.AlignCenter
                    opacity: 0.50196078431373
                }
                Grid {
                    columns: 2
                    spacing: 20

                    ExclusiveGroup { id: effectInputGroup }
                    RadioButton {
                        id: effectNormal
                        style:  econRadioButtonStyle
                        text:   qsTr("Normal")
                        exclusiveGroup: effectInputGroup
                        activeFocusOnPress: true
                        onClicked: {
                            storagecamcu135.setEffectMode(Storagecamcu135.EFFECT_NORMAL)
                        }
                        Keys.onReturnPressed: {
                            storagecamcu135.setEffectMode(Storagecamcu135.EFFECT_NORMAL)
                        }
                    }
                    RadioButton {
                        id: effectBW
                        style:  econRadioButtonStyle
                        text: qsTr("Black and White")
                        exclusiveGroup: effectInputGroup
                        activeFocusOnPress: true
                        onClicked: {
                            storagecamcu135.setEffectMode(Storagecamcu135.EFFECT_BLACK_WHITE)
                        }
                        Keys.onReturnPressed: {
                            storagecamcu135.setEffectMode(Storagecamcu135.EFFECT_BLACK_WHITE)
                        }
                    }
                    RadioButton {
                        id: effectNegative
                        style:  econRadioButtonStyle
                        text: qsTr("Negative")
                        exclusiveGroup: effectInputGroup
                        activeFocusOnPress: true
                        onClicked: {
                            storagecamcu135.setEffectMode(Storagecamcu135.EFFECT_NEGATIVE)
                        }
                        Keys.onReturnPressed: {
                            storagecamcu135.setEffectMode(Storagecamcu135.EFFECT_NEGATIVE)
                        }

                    }
                    RadioButton {
                        id: effectGrayscale
                        style:  econRadioButtonStyle
                        text: qsTr("Grayscale")
                        exclusiveGroup: effectInputGroup
                        activeFocusOnPress: true
                        onClicked: {
                            storagecamcu135.setEffectMode(Storagecamcu135.EFFECT_GREYSCALE)
                        }
                        Keys.onReturnPressed: {
                            storagecamcu135.setEffectMode(Storagecamcu135.EFFECT_GREYSCALE)
                        }
                    }
                    RadioButton {
                        id: effectSketch
                        style:  econRadioButtonStyle
                        text: qsTr("Sketch")
                        exclusiveGroup: effectInputGroup
                        activeFocusOnPress: true
                        onClicked: {
                            storagecamcu135.setEffectMode(Storagecamcu135.EFFECT_SKETCH)
                        }
                        Keys.onReturnPressed: {
                            storagecamcu135.setEffectMode(Storagecamcu135.EFFECT_SKETCH)
                        }
                    }
                }
                Text {
                    id: scene_mode
                    text: "--- Scene Mode ---"
                    font.pixelSize: 14
                    font.family: "Ubuntu"
                    color: "#ffffff"
                    smooth: true
                    Layout.alignment: Qt.AlignCenter
                    opacity: 0.50196078431373
                }
                Grid {
                    columns: 2
                    spacing: 20
                    ExclusiveGroup { id: sceneInputGroup }
                    RadioButton {
                        id: sceneNormal
                        style:  econRadioButtonStyle
                        text:   qsTr("Normal")
                        exclusiveGroup: sceneInputGroup
                        activeFocusOnPress: true
                        onClicked: {
                            storagecamcu135.setSceneMode(Storagecamcu135.SCENE_NORMAL)
                        }
                        Keys.onReturnPressed: {
                            storagecamcu135.setSceneMode(Storagecamcu135.SCENE_NORMAL)
                        }
                    }
                    RadioButton {
                        id: sceneDoc
                        style:  econRadioButtonStyle
                        text: qsTr("Document")
                        exclusiveGroup: sceneInputGroup
                        activeFocusOnPress: true
                        onClicked: {
                            storagecamcu135.setSceneMode(Storagecamcu135.SCENE_DOCUMENT)
                        }
                        Keys.onReturnPressed: {
                            storagecamcu135.setSceneMode(Storagecamcu135.SCENE_DOCUMENT)
                        }

                    }

                }
                Text {
                    id: denoiseText
                    text: "--- De-Noise ---"
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
                        id: deNoiseSlider
                        width: 150
                        stepSize: 1
                        style:econSliderStyle
                        minimumValue: denoiseMin
                        maximumValue: denoiseMax
                        onValueChanged:  {
                            deNoiseTextField.text = deNoiseSlider.value
                            if(skipUpdateUIDenoise){
                                storagecamcu135.setDenoiseValue(deNoiseSlider.value)
                            }
                            skipUpdateUIDenoise = true
                        }
                    }
                    TextField {
                        id: deNoiseTextField
                        text: deNoiseSlider.value
                        font.pixelSize: 10
                        font.family: "Ubuntu"
                        smooth: true
                        horizontalAlignment: TextInput.AlignHCenter
                        style: econTextFieldStyle
                        validator: IntValidator {bottom: deNoiseSlider.minimumValue; top: deNoiseSlider.maximumValue}
                        onTextChanged: {
                            if(text.length > 0){
                                deNoiseSlider.value = deNoiseTextField.text
                            }
                        }
                    }
                }
                Text {
                    id: qFactorText
                    text: "--- Q Factor ---"
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
                        id: qFactorSlider
                        width: 150
                        stepSize: 1
                        style:econSliderStyle
                        minimumValue: qFactorMin
                        maximumValue: qFactorMax
                        onValueChanged:  {
                            qFactorTextField.text = qFactorSlider.value
                            if(skipUpdateUIQFactor){
                                storagecamcu135.setQFactor(qFactorSlider.value)
                            }
                            skipUpdateUIQFactor = true
                        }
                    }
                    TextField {
                        id: qFactorTextField
                        text: qFactorSlider.value
                        font.pixelSize: 10
                        font.family: "Ubuntu"
                        smooth: true
                        horizontalAlignment: TextInput.AlignHCenter
                        style: econTextFieldStyle
                        validator: IntValidator {bottom: qFactorSlider.minimumValue; top: qFactorSlider.maximumValue}
                        onTextChanged: {
                            if(text.length > 0){
                                qFactorSlider.value = qFactorTextField.text
                            }
                        }
                    }
                }
                Text {
                    id: iHdrMode
                    text: "--- iHDR ---"
                    font.pixelSize: 14
                    font.family: "Ubuntu"
                    color: "#ffffff"
                    smooth: true
                    Layout.alignment: Qt.AlignCenter
                    opacity: 0.50196078431373
                }

                Row{
                    spacing:25
                    ExclusiveGroup { id: iHDRgroup }
                    RadioButton {
                        exclusiveGroup: iHDRgroup
                        id: hdrOff
                        text: "Off"
                        activeFocusOnPress: true
                        style: econRadioButtonStyle
                        onClicked:{
                            storagecamcu135.setiHDRMode(Storagecamcu135.HdrOff, 0)
                        }

                        Keys.onReturnPressed: {
                            storagecamcu135.setiHDRMode(Storagecamcu135.HdrOff, 0)
                        }
                    }
                    RadioButton {
                        exclusiveGroup: iHDRgroup
                        id:hdrAuto
                        text: "Auto"
                        activeFocusOnPress: true
                        style: econRadioButtonStyle
                        onClicked: {
                            storagecamcu135.setiHDRMode(Storagecamcu135.HdrAuto, 0)
                        }

                        Keys.onReturnPressed: {
                            storagecamcu135.setiHDRMode(Storagecamcu135.HdrAuto, 0)
                        }
                    }
                    RadioButton {
                        exclusiveGroup: iHDRgroup
                        id: hdrManual
                        text: "Manual"
                        activeFocusOnPress: true
                        style: econRadioButtonStyle
                        onClicked:{
                            storagecamcu135.setiHDRMode(Storagecamcu135.HdrManual, iHDRSlider.value)
                        }
                        Keys.onReturnPressed: {
                            storagecamcu135.setiHDRMode(Storagecamcu135.HdrManual, iHDRSlider.value)
                        }
                    }
                }
                Row{
                    spacing: 35
                    Slider {
                        activeFocusOnPress: true
                        updateValueWhileDragging: false
                        id: iHDRSlider
                        width: 150
                        stepSize: 1
                        style:econSliderStyle
                        minimumValue: iHDRMin
                        maximumValue: iHDRMax
                        enabled: (hdrManual.enabled && hdrManual.checked) ? true : false
                        opacity: enabled ? 1 : 0.1
                        onValueChanged:  {
                            iHDRTextField.text = iHDRSlider.value
                            if(skipUpdateUIiHDR){
                                storagecamcu135.setiHDRMode(Storagecamcu135.HdrManual, iHDRSlider.value)
                            }
                            skipUpdateUIiHDR = true

                        }
                    }
                    TextField {
                        id: iHDRTextField
                        text: iHDRSlider.value
                        font.pixelSize: 10
                        font.family: "Ubuntu"
                        smooth: true
                        horizontalAlignment: TextInput.AlignHCenter
                        style: econTextFieldStyle
                        enabled: (hdrManual.enabled && hdrManual.checked) ? true : false
                        opacity: enabled ? 1 : 0.1
                        validator: IntValidator {bottom: iHDRSlider.minimumValue; top: iHDRSlider.maximumValue}
                        onTextChanged: {
                            if(text.length > 0){
                                iHDRSlider.value = iHDRTextField.text
                            }
                        }
                    }
                }
                Text {
                    id: streamModeText
                    text: "--- Stream Mode ---"
                    font.pixelSize: 14
                    font.family: "Ubuntu"
                    color: "#ffffff"
                    smooth: true
                    Layout.alignment: Qt.AlignCenter
                    opacity: 0.50196078431373
                }

                Row{
                    Column{
                        spacing:20
                        ExclusiveGroup { id: streamModeGroup }
                         /*          RadioButton {
                            exclusiveGroup: streamModeGroup
                            id: streamMasterContinuous
                            text: "Master(Continuous)"          // We need to send hid command one time .
                            activeFocusOnPress: true
                            style: econRadioButtonStyle
                            onClicked:{
                                storagecamcu135.grabPreviewFrame()
                            }
                            Keys.onReturnPressed: {
                                storagecamcu135.grabPreviewFrame()
                            }
                        }*/
                        RadioButton {
                            exclusiveGroup: streamModeGroup
                            id: streamMasterOnDemand
                            checked:false
                            text: "Master"                    // We need to send "queryNext" hid command every time. It gives one frame at a time
                            activeFocusOnPress: true          // grabPreviewFrame
                            style: econRadioButtonStyle
                            onClicked:{
                                setMasterOnDemandMode()
                              //  storagecamcu135.grabPreviewFrame()
                             //   storagecamcu135.grabPreviewFrame()

                            }
                            Keys.onReturnPressed: {
                                setMasterOnDemandMode()

                            }
                        }
                        Row{
                            spacing : 10

                            RadioButton {
                                exclusiveGroup: streamModeGroup  // We need to send commands for storeframe and grab frame
                                id: streamSwTrigger
                                text: "Software Trigger Mode"
                                activeFocusOnPress: true
                                style: econRadioButtonStyle
                                onClicked: {
                                    root.captureBtnEnable(false)
                                    root.videoRecordBtnEnable(false)
                                    storagecamcu135.setStreamMode(Storagecamcu135.STREAM_SOFTWARE_TRIGGER)

                                }
                                Keys.onReturnPressed: {
                                    root.captureBtnEnable(false)
                                    root.videoRecordBtnEnable(false)
                                    storagecamcu135.setStreamMode(Storagecamcu135.STREAM_SOFTWARE_TRIGGER)

                                }
                            }
                            Button {
                                id: swTrigger
                                activeFocusOnPress : true
                                text: "Grab"
                                style: econButtonStyle
                                opacity: streamSwTrigger.checked ? 1 : 0.1

                                implicitHeight: 20
                                implicitWidth: 60
                                onClicked: {
                                    storagecamcu135.storePreviewFrame()
                                   storagecamcu135.grabPreviewFrame()


                                }
                                Keys.onReturnPressed: {
                                    storagecamcu135.storePreviewFrame()
                                    storagecamcu135.grabPreviewFrame()


                                }
                            }
                        }
                            RadioButton {
                                exclusiveGroup: streamModeGroup
                                id: streamHwTrigger
                                text: "Hardware Trigger Mode"
                                activeFocusOnPress: true
                                style: econRadioButtonStyle
                                onClicked: {
                                    root.captureBtnEnable(false)
                                    root.videoRecordBtnEnable(false)
                                    storagecamcu135.setStreamMode(Storagecamcu135.STREAM_HARDWARE_TRIGGER)
                                }
                                Keys.onReturnPressed: {
                                    root.captureBtnEnable(false)
                                    root.videoRecordBtnEnable(false)
                                    storagecamcu135.setStreamMode(Storagecamcu135.STREAM_HARDWARE_TRIGGER)
                                }
                            }


                    }
                }
                    Text {
                        id: flipText
                        text: "--- Flip Control ---"
                        font.pixelSize: 14
                        font.family: "Ubuntu"
                        color: "#ffffff"
                        smooth: true
                        Layout.alignment: Qt.AlignCenter
                        opacity: 0.50196078431373
                    }
                    Row{
                        spacing: 55
                        CheckBox {
                            id: flipCtrlHorizotal
                            activeFocusOnPress : true
                            text: "Horizontal"
                            style: econCheckBoxStyle
                            onClicked:{
                                storagecamcu135.setOrientation(flipCtrlHorizotal.checked, flipCtrlVertical.checked)
                            }
                            Keys.onReturnPressed: {
                                storagecamcu135.setOrientation(flipCtrlHorizotal.checked, flipCtrlVertical.checked)
                            }
                        }
                        CheckBox {
                            id: flipCtrlVertical
                            activeFocusOnPress : true
                            text: "Vertical"
                            style: econCheckBoxStyle
                            onClicked:{
                                storagecamcu135.setOrientation(flipCtrlHorizotal.checked, flipCtrlVertical.checked)
                            }
                            Keys.onReturnPressed: {
                                storagecamcu135.setOrientation(flipCtrlHorizotal.checked, flipCtrlVertical.checked)
                            }
                        }
                    }
                    Text {
                        id: imgCapText
                        text: "--- Still Image Capture ---"
                        font.pixelSize: 14
                        font.family: "Ubuntu"
                        color: "#ffffff"
                        smooth: true
                        Layout.alignment: Qt.AlignCenter
                        opacity: 0.50196078431373
                    }
                    Text {
                        id: storeFrameText
                        text: "Store Frame:"
                        font.pixelSize: 14
                        font.family: "Ubuntu"
                        color: "#ffffff"
                        smooth: true
                        opacity: 1
                    }
                    Button {
                        id: storeFrame
                        activeFocusOnPress : true
                        text: "Store"
                        style: econButtonStyle
                        enabled: streamMasterOnDemand.checked ? true : false

                        opacity: enabled ? 1 : 0.1
                        //                    enabled: true
                        //                    opacity: 1

                        implicitHeight: 30
                        implicitWidth: 100
                        onClicked: {
                            storeFrame.enabled = false
                            storagecamcu135.storeStillFrame(stillFormatCurrentIndex+1, stillResolutionCurrentIndex+1);
                        }
                        Keys.onReturnPressed: {
                            storagecamcu135.storeStillFrame(stillFormatCurrentIndex+1, stillResolutionCurrentIndex+1);
                        }
                    }
                    Text {
                        id: retrieveFrameText
                        text: "Retrieve Frame:"
                        font.pixelSize: 14
                        font.family: "Ubuntu"
                        color: "#ffffff"
                        smooth: true
                        opacity: 1
                    }

                    Text {
                        id: frameIndex
                        text: "Frame index :"
                        font.pixelSize: 14
                        font.family: "Ubuntu"
                        color: "#ffffff"
                        smooth: true
                        opacity: 1
                    }


                  Row{
                        spacing: 10

                        ComboBox
                        {
                            id: frameIndexCombo
                            enabled: streamMasterOnDemand.checked ? true : false
                            opacity: enabled ? 1 : 0.1
                            //                        enabled: true
                            //                        opacity: 1
                            model: ListModel {
                                id: frameIndexModel
                            }
                            activeFocusOnPress: true

                            style: ComboBoxStyle {
                                background: Image {
                                    id: frameIndexBg
                                    source: "../../Views/images/smallsizeCombobox.png"
                                    Rectangle {
                                        width: frameIndexBg.sourceSize.width - 20
                                        height: frameIndexBg.sourceSize.height + 3
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
                            onCurrentIndexChanged: {

                            }
                        }
                        Button {
                            id: retrieveFrame
                            activeFocusOnPress : true
                            text: "Retrieve"
                            enabled: streamMasterOnDemand.checked ? true : false
                            opacity: enabled ? 1 : 0.1
                            style: econButtonStyle
                            implicitHeight: 25
                            implicitWidth: 100
                            onClicked: {
                                root.retrieveFrameFromStorageCamera()
                            }
                        }
                    }

                    Text {
                        id: roiAutoExpMode
                        text: "--- ROI - Auto Exposure ---"
                        font.pixelSize: 14
                        font.family: "Ubuntu"
                        color: "#ffffff"
                        smooth: true
                        Layout.alignment: Qt.AlignCenter
                        opacity: 0.50196078431373
                    }

                    Row{
                        spacing:38
                        ExclusiveGroup { id: roiExpogroup }

                        // Added by Sankari 13th Sep 2017 : Added Face ROI mode
                        RadioButton {
                            exclusiveGroup: roiExpogroup
                            id: autoexpFace
                            text: "Face"
                            activeFocusOnPress: true
                            style: econRadioButtonStyle
                            opacity: enabled ? 1 : 0.1
                            // setROIAutoExposure() args:  mode, videoresolnWidth, videoresolnHeight, mouseXCord, mouseYCord, WinSize]
                            // videoresolnWidth, videoresolnHeight, mouseXCord, mouseYCord - these parameters are required only when click in preview]
                            // winSize is required only for manual mode
                            onClicked: {
                                storagecamcu135.setROIAutoExposure(Storagecamcu135.AutoExpFace, 0, 0, 0, 0, 0);
                                autoExpoWinSizeCombo.enabled = false
                            }
                            Keys.onReturnPressed: {
                                storagecamcu135.setROIAutoExposure(Storagecamcu135.AutoExpFace, 0, 0, 0, 0, 0);
                                autoExpoWinSizeCombo.enabled = false
                            }
                        }
                        RadioButton {
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
                                storagecamcu135.setROIAutoExposure(Storagecamcu135.AutoExpFull, 0, 0, 0, 0, 0);
                                autoExpoWinSizeCombo.enabled = false
                            }
                            Keys.onReturnPressed: {
                                storagecamcu135.setROIAutoExposure(Storagecamcu135.AutoExpFull, 0, 0, 0, 0, 0);
                                autoExpoWinSizeCombo.enabled = false
                            }
                        }
                        RadioButton {
                            exclusiveGroup: roiExpogroup
                            id: autoexpManual
                            text: "Manual"
                            activeFocusOnPress: true
                            style: econRadioButtonStyle
                            opacity: enabled ? 1 : 0.1
                            onClicked: {
                                storagecamcu135.setROIAutoExposure(Storagecamcu135.AutoExpManual, 0, 0, 0, 0, 0);
                                autoExpoWinSizeCombo.enabled = true
                            }
                            Keys.onReturnPressed: {
                                storagecamcu135.setROIAutoExposure(Storagecamcu135.AutoExpManual, 0, 0, 0, 0, 0);
                                autoExpoWinSizeCombo.enabled = true
                            }
                        }
                    }
                    ComboBox
                    {
                        id: autoExpoWinSizeCombo
                        enabled: (autoexpManual.enabled && autoexpManual.checked) ? true : false
                        opacity: (autoexpManual.enabled && autoexpManual.checked) ? 1 : 0.1
                        model: ListModel {
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
                                storagecamcu135.setROIAutoExposure(Storagecamcu135.AutoExpManual, 0, 0, 0, 0, autoExpoWinSizeCombo.currentText)
                            }
                            skipUpdateUIOnExpWindowSize = true
                        }
                    }
                    Text {
                        id: exposureCompTextTitle
                        text: "--- Exposure Compensation ---"
                        font.pixelSize: 14
                        font.family: "Ubuntu"
                        color: "#ffffff"
                        smooth: true
                        Layout.alignment: Qt.AlignCenter
                        opacity: 0.50196078431373
                    }
                    Row{
                        spacing: 10
                        Text {
                            id: exposureCompText
                            text: "value(µs)[8000 - 1000000]"
                            font.pixelSize: 14
                            font.family: "Ubuntu"
                            color: "#ffffff"
                            smooth: true
                            width: 80
                            wrapMode: Text.WordWrap
                            opacity: 1
                        }
                        TextField {

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
                        Button {
                            id: exposureCompSet
                            activeFocusOnPress : true
                            text: "Set"
                            style: econButtonStyle
                            enabled: true
                            opacity: 1
                            implicitHeight: 25
                            implicitWidth: 60
                            onClicked: {
                                exposureCompSet.enabled = false
                                setButtonClicked = true
                                storagecamcu135.setExposureCompensation(exposureCompValue.text)
                                setButtonClicked = false
                                exposureCompSet.enabled = true
                            }
                            Keys.onReturnPressed: {
                                exposureCompSet.enabled = false
                                setButtonClicked = true
                                storagecamcu135.setExposureCompensation(exposureCompValue.text)
                                setButtonClicked = false
                                exposureCompSet.enabled = true
                            }
                        }
                    }
                    Text {
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
                            minimumValue: frameRateMin
                            maximumValue: frameRateMax
                            onValueChanged:  {
                                frameRateTextField.text = frameRateSlider.value
                                if(skipUpdateUIFrameRate){
                                    storagecamcu135.setFrameRateCtrlValue(frameRateSlider.value)
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
                    Text {
                        id: faceDetectionText
                        text: "--- Face Detection ---"
                        font.pixelSize: 14
                        font.family: "Ubuntu"
                        color: "#ffffff"
                        smooth: true
                        Layout.alignment: Qt.AlignCenter
                        opacity: 0.50196078431373
                    }

                    Row{
                        spacing: 62
                        ExclusiveGroup { id: faceRectGroup }
                        RadioButton {
                            exclusiveGroup: faceRectGroup
                            id: faceRectEnable
                            text: "Enable"
                            activeFocusOnPress: true
                            style: econRadioButtonStyle
                            onClicked:{
                                storagecamcu135.setFaceDetectionRect(true, faceDetectEmbedData.checked, overlayRect.checked)
                            }
                            Keys.onReturnPressed: {
                                storagecamcu135.setFaceDetectionRect(true, faceDetectEmbedData.checked, overlayRect.checked)
                            }
                        }
                        RadioButton {
                            exclusiveGroup: faceRectGroup
                            id:faceRectDisable
                            text: "Disable"
                            activeFocusOnPress: true
                            style: econRadioButtonStyle
                            onClicked: {
                                storagecamcu135.setFaceDetectionRect(false, faceDetectEmbedData.checked, overlayRect.checked)
                            }
                            Keys.onReturnPressed: {
                                storagecamcu135.setFaceDetectionRect(false, faceDetectEmbedData.checked, overlayRect.checked)
                            }
                        }
                    }
                    Row{
                        spacing: 5
                        CheckBox {
                            id: faceDetectEmbedData
                            activeFocusOnPress : true
                            text: "Embed \nData"
                            style: econCheckBoxTextWrapModeStyle
                            enabled: faceRectEnable.checked ? true : false
                            opacity: enabled ? 1 : 0.1
                            onClicked:{
                                enableFaceDetectEmbedData()
                            }
                            Keys.onReturnPressed: {
                                enableFaceDetectEmbedData()
                            }
                        }
                        CheckBox {
                            id: overlayRect
                            activeFocusOnPress : true
                            text: "Overlay Rectangle"
                            style: econCheckBoxTextWrapModeStyle
                            enabled: faceRectEnable.checked ? true : false
                            opacity: enabled ? 1 : 0.1
                            onClicked:{
                                storagecamcu135.setFaceDetectionRect(faceRectEnable.checked, faceDetectEmbedData.checked, checked)
                            }
                            Keys.onReturnPressed: {
                                storagecamcu135.setFaceDetectionRect(faceRectEnable.checked, faceDetectEmbedData.checked, checked)
                            }
                        }
                    }
                    Text {
                        id: smileDetectionText
                        text: "--- Smile Detection ---"
                        font.pixelSize: 14
                        font.family: "Ubuntu"
                        color: "#ffffff"
                        smooth: true
                        Layout.alignment: Qt.AlignCenter
                        opacity: 0.50196078431373
                    }
                    Row{
                        spacing: 62
                        ExclusiveGroup { id: smileDetectGroup }
                        RadioButton {
                            exclusiveGroup: smileDetectGroup
                            id: smileDetectEnable
                            text: "Enable"
                            activeFocusOnPress: true
                            style: econRadioButtonStyle
                            onClicked:{
                                storagecamcu135.setSmileDetection(true, smileDetectEmbedData.checked, smileThreshold.text)
                            }
                            Keys.onReturnPressed: {
                                storagecamcu135.setSmileDetection(true, smileDetectEmbedData.checked, smileThreshold.text)
                            }
                        }
                        RadioButton {
                            exclusiveGroup: smileDetectGroup
                            id:smileDetectDisable
                            text: "Disable"
                            activeFocusOnPress: true
                            style: econRadioButtonStyle
                            onClicked: {
                                storagecamcu135.setSmileDetection(false, smileDetectEmbedData.checked, smileThreshold.text)
                            }
                            Keys.onReturnPressed: {
                                storagecamcu135.setSmileDetection(false, smileDetectEmbedData.checked, smileThreshold.text)
                            }
                        }
                    }
                    Row{
                        spacing: 5
                        CheckBox {
                            id: smileDetectEmbedData
                            activeFocusOnPress : true
                            text: "Embed Data"
                            style: econCheckBoxStyle
                            enabled: smileDetectEnable.checked ? true : false
                            opacity: enabled ? 1 : 0.1
                            onClicked:{
                                enableSmileDetectEmbedData()
                            }
                            Keys.onReturnPressed: {
                                enableSmileDetectEmbedData()
                            }
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
                            action: setDefault
                            style: econButtonStyle
                            Keys.onReturnPressed: {
                                setToDefaultValues()
                            }
                        }
                    }
                    Row{
                        Button {
                            id: f_wversion_selectedCU135
                            opacity: 1
                            action: firmwareVersion
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
                            Keys.onReturnPressed: {
                                getFirmwareVersion()
                            }
                        }
                        Button {
                            id: serial_no_selected
                            opacity: 1
                            action: serialNumber
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
                            Keys.onReturnPressed: {
                                getSerialNumber()
                            }
                        }
                    }
                }
            }
        }



        function setMasterOnDemandMode(){
            root.captureBtnEnable(true)
            root.videoRecordBtnEnable(true)
            storagecamcu135.setStreamMode(Storagecamcu135.STREAM_MASTER_ONDEMAND)
            storagecamcu135.grabPreviewFrame()

        }

        function displayMessageBox(title, text){
            messageDialog.title = qsTr(title)
            messageDialog.text = qsTr(text)
            messageDialog.open()

        }

        // set to default values
        function setToDefaultValues(){
            storagecamcu135.setToDefault()
            //Added by Navya:28-Feb-2019 -To get a popup window
            messageDialog.title=qsTr("Restore Default")
            messageDialog.text=qsTr("When we click extension unit restore default option, all the controls will be updated to their default values except stream mode. The stream mode control can be changed manually")
             messageDialog.open()
            getValuesFromCamera()
            // Added by Sankari: 24 Apr 2017. To get preview in master mode
            root.startUpdatePreviewInMasterMode()
        }

        // Enable Face detect embed data
        function enableFaceDetectEmbedData(){
            if(storagecamcu135.setFaceDetectionRect(faceRectEnable.checked, faceDetectEmbedData.checked, overlayRect.checked)){
                if(faceDetectEmbedData.checked){
                    messageDialog.title = qsTr("Status")
                    messageDialog.text = qsTr("The last part of the frame will be replaced by face data.Refer document StorageCam_CU135_Face_and_Smile_Detection for more details")
                    messageDialog.open()
                }
            }
        }

        // enable smile detect embed data
        function enableSmileDetectEmbedData(){
            if(storagecamcu135.setSmileDetection(true, smileDetectEmbedData.checked)){
                if(smileDetectEmbedData.checked){
                    messageDialog.title = qsTr("Status")
                    messageDialog.text = qsTr("The last part of the frame will be replaced by smile data.Refer document StorageCam_CU135_Face_and_Smile_Detection for more details")
                    messageDialog.open()
                }
            }
        }

        // enable/disable auto exposure controls in extension settings based on selection of expoure in Image Quality Settings
        function enableDisableAutoExposureControls(autoExposureSelect){
            if(autoExposureSelect){
                autoexpManual.enabled = true
                autoexpFull.enabled = true
                autoexpFace.enabled = true
                if(autoexpManual.checked)
                    autoExpoWinSizeCombo.enabled = true
                if(autoexpFull.checked || autoexpFace.checked)
                    autoExpoWinSizeCombo.enabled = false
                autoexpManual.opacity = 1
                autoexpFull.opacity = 1
                autoexpFace.opacity = 1
                exposureCompValue.enabled = true
                exposureCompValue.opacity = 1
                exposureCompSet.enabled = true
                exposureCompSet.opacity = 1
                exposureCompText.opacity = 1
            }else{
                autoexpManual.enabled = false
                autoexpFull.enabled = false
                autoexpFace.enabled = false
                autoExpoWinSizeCombo.enabled = false
                autoexpManual.opacity = 0.1
                autoexpFull.opacity = 0.1
                autoexpFace.opacity = 0.1
                exposureCompValue.enabled = false
                exposureCompValue.opacity = 0.1
                exposureCompSet.enabled = false
                exposureCompSet.opacity = 0.1
                exposureCompText.opacity = 0.1
            }
            getAutoExpsoureControlValues.start()
        }

        // Effect mode
        function currentEffectModeValue(mode)
        {
            switch(mode)
            {
            case Storagecamcu135.EFFECT_NORMAL:
                effectNormal.checked = true
                break;
            case Storagecamcu135.EFFECT_BLACK_WHITE:
                effectBW.checked = true
                break;
            case Storagecamcu135.EFFECT_GREYSCALE:
                effectGrayscale.checked = true
                break;
            case Storagecamcu135.EFFECT_NEGATIVE:
                effectNegative.checked = true
                break;
            case Storagecamcu135.EFFECT_SKETCH:
                effectSketch.checked = true
                break;
            }
        }

        // scene mode
        function currentSceneModeValue(mode)
        {
            switch(mode)
            {
            case Storagecamcu135.SCENE_NORMAL:
                sceneNormal.checked = true
                break;
            case Storagecamcu135.SCENE_DOCUMENT:
                sceneDoc.checked = true
                break;
            }
        }

        // HDR mode
        function currentHDRMode(mode)
        {
            switch(mode)
            {
            case Storagecamcu135.HdrOff:
                hdrOff.checked = true
                break;
            case Storagecamcu135.HdrAuto:
                hdrAuto.checked = true
                break;
            case Storagecamcu135.HdrManual:
                hdrManual.checked = true
                break;
            }
        }

        // set trigger stream mode
        function setStreamModes(){
            root.stopUpdatePreviewInTriggerMode()
            storagecamcu135.setStreamMode(Storagecamcu135.STREAM_TRIGGER)
            displayMessageBox(qsTr("Trigger Mode"), qsTr("Frames will be out only when external hardware pulses are given to PIN 5 of CN3. Refer the document See3CAM_CU135_Trigger_Mode"))
        }

        // Get the control values in extension settings
        function getValuesFromCamera(){
            storagecamcu135.getEffectMode()
            storagecamcu135.getSceneMode()
            storagecamcu135.getDenoiseValue()
            storagecamcu135.getQFactor()
            storagecamcu135.getiHDRMode()
            storagecamcu135.getStreamMode()
            storagecamcu135.getAutoExpROIModeAndWindowSize()
            storagecamcu135.getFaceDetectMode()
            storagecamcu135.getSmileDetectMode()
            storagecamcu135.getOrientation()
            getexposureCompFrameRateCtrlTimer.start()

        }

        // Flip mirror modes
        function currentFlipMirrorMode(mode)
        {
            switch(mode)
            {
            case Storagecamcu135.SetBothFlipEnable:
                flipCtrlVertical.checked = true
                flipCtrlHorizotal.checked = true
                break;
            case Storagecamcu135.SetVertiFlip:
                flipCtrlVertical.checked = true
                flipCtrlHorizotal.checked = false
                break;
            case Storagecamcu135.SetHorzFlip:
                flipCtrlVertical.checked = false
                flipCtrlHorizotal.checked = true
                break;
            case Storagecamcu135.SetBothFlipDisable:
                flipCtrlVertical.checked = false
                flipCtrlHorizotal.checked = false
                break;
            }
        }

        // current ROI auto exposure mode
        function currentROIAutoExposureMode(roiMode, winSize){
            switch(roiMode){
            case Storagecamcu135.AutoExpFace:
                autoexpFace.checked = true
                autoExpoWinSizeCombo.enabled = false
                break
            case Storagecamcu135.AutoExpFull:
                autoexpFull.checked = true
                autoExpoWinSizeCombo.enabled = false
                break
            case Storagecamcu135.AutoExpManual:
                skipUpdateUIOnExpWindowSize = false
                autoexpManual.checked = true
                // If window size is got from camera is 0 then set window size to 1 in UI
                if(winSize == 0){
                    autoExpoWinSizeCombo.currentIndex = 0
                }else
                    autoExpoWinSizeCombo.currentIndex = winSize-1
                break
            case Storagecamcu135.AutoExpDisabled:
                autoexpFace.enabled = false
                autoexpFull.enabled = false
                autoexpManual.enabled = false
                autoExpoWinSizeCombo.enabled = false
                break
            }
        }

        Uvccamera {
            id: uvccamera
            onTitleTextChanged: {
                displayMessageBox(qsTr(_title), qsTr(_text))
            }
            onSerialNumber:{
                displayMessageBox(qsTr("Serial Number"), serialNumber)
            }
        }

        function getSerialNumber() {
            uvccamera.getSerialNumber()
            messageDialog.open()
        }

        function getFirmwareVersion() {
            uvccamera.getFirmWareVersion()
            messageDialog.open()
        }

        function fillFrameIndexCombo(numOfImages){ // need to fill based on resolution
            frameIndexModel.clear()
            for (var i=1; i <= numOfImages; i++){
                frameIndexModel.append({"text": i})
            }
        }

        Storagecamcu135 {
            id: storagecamcu135
            onSceneModeValue:{
                currentSceneModeValue(sceneMode)
            }
            onEffectModeValue:{
                currentEffectModeValue(effectMode)
            }
            onDenoiseValue:{
                skipUpdateUIDenoise = false
                deNoiseSlider.value = denoiseVal
            }
            onQFactorValue:{
                skipUpdateUIQFactor = false
                qFactorSlider.value = qFactor
            }
            onHdrModeValue:{
                currentHDRMode(hdrMode)
                if(hdrMode == Storagecamcu135.HdrManual){
                    iHDRSlider.value = hdrValue
                }
            }
            onStreamModeValue:{
                if(streamMode == Storagecamcu135.STREAM_MASTER_ONDEMAND){
                    streamMasterOnDemand.checked = true
                    storagecamcu135.grabPreviewFrame()

                    storagecamcu135.grabPreviewFrame()

                }else if(streamMode == Storagecamcu135.STREAM_SOFTWARE_TRIGGER){
                    streamSwTrigger.checked = true
                }else if(streamMode == Storagecamcu135.STREAM_HARDWARE_TRIGGER){
                    streamHwTrigger.checked = true
                }
            }
            onFlipMirrorModeChanged:{
                currentFlipMirrorMode(flipMirrorMode)
            }
            onRoiAutoExpMode:{
                currentROIAutoExposureMode(roiMode, winSize)
            }
            onExposureCompValue:{
                exposureCompValue.text = exposureCompensation
            }
            onFrameRateCtrlValue:{
                skipUpdateUIFrameRate = false
                frameRateSlider.value = frameRateVal
            }
            onFaceDetectModeValue:{
                if(faceDetectMode == Storagecamcu135.FaceRectEnable){
                    faceRectEnable.checked = true
                    if(faceDetectEmbedDataValue == Storagecamcu135.FaceDetectEmbedDataEnable){
                        faceDetectEmbedData.checked = true
                    }
                    if(faceDetectOverlayRect == Storagecamcu135.FaceDetectOverlayRectEnable){
                        overlayRect.checked = true
                    }
                }else if(faceDetectMode == Storagecamcu135.FaceRectDisable){
                    faceRectDisable.checked = true
                    if(faceDetectEmbedDataValue == Storagecamcu135.FaceDetectEmbedDataEnable){
                        faceDetectEmbedData.checked = true
                    }else{
                        faceDetectEmbedData.checked = false
                    }
                    if(faceDetectOverlayRect == Storagecamcu135.FaceDetectOverlayRectEnable){
                        overlayRect.checked = true
                    }else{
                        overlayRect.checked = false
                    }
                }
            }
            onSmileDetectModeValue:{
                if(smileDetectMode == Storagecamcu135.SmileDetectEnable){
                    smileDetectEnable.checked = true
                    if(smileDetectEmbedDataValue == Storagecamcu135.SmileDetectEmbedDataEnable){
                        smileDetectEmbedData.checked = true
                    }
                }else if(smileDetectMode == Storagecamcu135.SmileDetectDisable){
                    smileDetectDisable.checked = true
                    if(smileDetectEmbedDataValue == Storagecamcu135.SmileDetectEmbedDataEnable){
                        smileDetectEmbedData.checked = true
                    }else{
                        smileDetectEmbedData.checked = false
                    }
                }
            }
            onIndicateCommandStatus:{
                displayMessageBox(title, text)
            }

            onIndicateExposureValueRangeFailure:{
                if(setButtonClicked){
                    displayMessageBox(title, text)
                    setButtonClicked = false
                    storagecamcu135.getExposureCompensation()
                }
            }

            onIndicateSmileThresholdRangeFailure:{
                displayMessageBox(title, text)
                storagecamcu135.getSmileDetectMode()
            }
            onNumberOfframesStoreCapacity:{
                fillFrameIndexCombo(numberOfFrames)
            }

            onGrabStillFrameStatus:{
                if(grabStatus){ // If grab still frame command is success, the save the frame in PC
                    root.imageCapture(CommonEnums.STORECAM_RETRIEVE_SHOT)
                }else{ // If grab still frame command is failure, then ignore
                    switchToCamFrameSettings(false)
                    storagecamcu135.grabPreviewFrame()
                }
            }

            onStoreStillFrameSucceess:{
                storeFrame.enabled = true
                storagecamcu135.grabPreviewFrame()
            }

            onStillsettingsReceived:{
                storagecamcu135.getNumberOfFramesCanStore(stillformat, stillResolution)
                root.changeStillSettings(stillformat, stillResolution)
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
            id: econCheckBoxTextWrapModeStyle
            CheckBoxStyle {
                label: Text {
                    text: control.text
                    font.pixelSize: 14
                    font.family: "Ubuntu"
                    color: "#ffffff"
                    smooth: true
                    opacity: 1
                    width: 100
                    wrapMode: Text.WordWrap
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

        Component.onCompleted: {
            setMasterOnDemandMode()
            getValuesFromCamera()
            storagecamcu135.getStillResolution()
        }
    }

