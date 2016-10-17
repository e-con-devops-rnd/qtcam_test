import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.1
import econ.camera.uvcsettings 1.0
import econ.camera.see3cam130 1.0
import QtQuick.Layouts 1.1
import "../../JavaScriptFiles/tempValue.js" as JS

Item {
    width:268
    height:750
    property int denoiseMin: 0
    property int denoiseMax: 15
    property int qFactorMin: 10
    property int qFactorMax: 96
    property int iHDRMin: 1
    property int iHDRMax: 4
    property bool settingWhenUpdateInUI : false
    property bool settingafWinSizeWhenUpdateInUI: false
    property bool settingautoExpWinSizeWhenUpdateInUI: false
    property bool settingBurstLengthWhenUpdateInUI: false
    property bool settingiHDRWhenUpdateInUI: false
    property int afROIwindowSizeCurrentIndex: afWindowSizeCombo.currentIndex
    property bool afROImode: afCentered.checked
    property int autoExpROIwindowSizeCurrentIndex: autoExpoWinSizeCombo.currentIndex
    property bool autoExpROImode: autoexpFull.checked
    property int burstLengthCurrentIndex: burstLengthCombo.currentIndex

    Action {
        id: triggerAction
        onTriggered: {
            seecam130.setAutoFocusMode(See3Cam130.OneShot)
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
        onTriggered:
        {
            getSerialNumber()
        }
    }
    ScrollView{
        id: scrollview
        x: 10
        y: 189.5
        width: 257
        height: 500
        style: econscrollViewStyle
        ColumnLayout{
            x:2
            y:5
            spacing:20

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
                    //checked: true
                    onClicked: {
                        seecam130.setSceneMode(See3Cam130.SCENE_NORMAL)
                    }
                }
                RadioButton {
                    id: sceneDoc
                    style:  econRadioButtonStyle
                    text: qsTr("Document")
                    exclusiveGroup: sceneInputGroup
                    activeFocusOnPress: true
                    onClicked: {
                        seecam130.setSceneMode(See3Cam130.SCENE_DOCUMENT)
                    }
                }

            }

            Text {
                id: special_effects
                text: "--- Special Effects ---"
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
                    //checked: true
                    onClicked: {
                        seecam130.setEffectMode(See3Cam130.EFFECT_NORMAL)
                    }
                }
                RadioButton {
                    id: effectBW
                    style:  econRadioButtonStyle
                    text: qsTr("Black and White")
                    exclusiveGroup: effectInputGroup
                    activeFocusOnPress: true
                    onClicked: {
                        seecam130.setEffectMode(See3Cam130.EFFECT_BLACK_WHITE)
                    }
                }
                RadioButton {
                    id: effectNegative
                    style:  econRadioButtonStyle
                    text: qsTr("Negative")
                    exclusiveGroup: effectInputGroup
                    activeFocusOnPress: true
                    onClicked: {
                        seecam130.setEffectMode(See3Cam130.EFFECT_NEGATIVE)
                    }
                }
                RadioButton {
                    id: effectGrayscale
                    style:  econRadioButtonStyle
                    text: qsTr("Grayscale")
                    exclusiveGroup: effectInputGroup
                    activeFocusOnPress: true
                    onClicked: {
                        seecam130.setEffectMode(See3Cam130.EFFECT_GREYSCALE)
                    }
                }
                RadioButton {
                    id: effectSketch
                    style:  econRadioButtonStyle
                    text: qsTr("Sketch")
                    exclusiveGroup: effectInputGroup
                    activeFocusOnPress: true
                    onClicked: {
                        seecam130.setEffectMode(See3Cam130.EFFECT_SKETCH)
                    }
                }

            }
            Text {
                id: autoFocusMode
                text: "--- Auto Focus Mode ---"
                font.pixelSize: 14
                font.family: "Ubuntu"
                color: "#ffffff"
                smooth: true
                Layout.alignment: Qt.AlignCenter
                opacity: 0.50196078431373
            }

            Row{
                  spacing:10
                  ExclusiveGroup { id: afgroup }
                  RadioButton {
                      exclusiveGroup: afgroup
                      id: radioContin
                      text: "Continuous"
                      activeFocusOnPress: true
                      style: econRadioButtonStyle                      
                      opacity: enabled ? 1 : 0.1
                      onClicked: {
                        seecam130.setAutoFocusMode(See3Cam130.Continuous);
                      }
                      Keys.onReturnPressed: {

                      }
                  }
            }
            Row{
                spacing:25
                RadioButton {
                    exclusiveGroup: afgroup
                    id: radioOneshot
                    text: "One-Shot"
                    activeFocusOnPress: true
                    style: econRadioButtonStyle                    
                    opacity: enabled ? 1 : 0.1
                    onClicked: {
                        seecam130.setAutoFocusMode(See3Cam130.OneShot);
                    }
                    Keys.onReturnPressed: {

                    }
                }
                Button {
                    id: trigger
                    activeFocusOnPress : true
                    text: "Trigger"
                    style: econButtonStyle
                    opacity: (radioOneshot.checked) ? 1 : 0.1
                    implicitHeight: 25
                    implicitWidth: 120
                    action: (radioOneshot.checked) ? triggerAction : null
                    Keys.onReturnPressed: {

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
                        seecam130.setiHDRMode(See3Cam130.HdrOff, 0)
                    }

                    Keys.onReturnPressed: {
                        seecam130.setiHDRMode(See3Cam130.HdrOff, 0)
                    }
                }
                RadioButton {
                    exclusiveGroup: iHDRgroup
                    id:hdrAuto
                    text: "Auto"
                    activeFocusOnPress: true
                    style: econRadioButtonStyle
                    onClicked: {
                        seecam130.setiHDRMode(See3Cam130.HdrAuto, 0)
                    }

                    Keys.onReturnPressed: {
                        seecam130.setiHDRMode(See3Cam130.HdrAuto, 0)
                    }
                }
                RadioButton {
                    exclusiveGroup: iHDRgroup
                    id: hdrManual
                    text: "Manual"
                    activeFocusOnPress: true
                    style: econRadioButtonStyle
                    onClicked:{
                        seecam130.setiHDRMode(See3Cam130.HdrManual, iHDRSlider.value)
                    }
                    Keys.onReturnPressed: {
                        seecam130.setiHDRMode(See3Cam130.HdrManual, iHDRSlider.value)
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
                        if(settingiHDRWhenUpdateInUI){
                            seecam130.setiHDRMode(See3Cam130.HdrManual, iHDRSlider.value)
                        }
                        settingiHDRWhenUpdateInUI = true

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
                        if(settingWhenUpdateInUI){
                            seecam130.setDenoiseValue(deNoiseSlider.value)
                        }
                        settingWhenUpdateInUI = true
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
                id: roiAutoFocusMode
                text: "--- ROI - Auto Focus ---"
                font.pixelSize: 14
                font.family: "Ubuntu"
                color: "#ffffff"
                smooth: true
                Layout.alignment: Qt.AlignCenter
                opacity: 0.50196078431373
            }

            Row{
                  spacing:25
                  ExclusiveGroup { id: roiAfgroup }
                  RadioButton {
                      exclusiveGroup: roiAfgroup
                      id: afCentered
                      text: "Centered"
                      activeFocusOnPress: true
                      style: econRadioButtonStyle                     
                      opacity: enabled ? 1 : 0.1
                      // setROIAutoFoucs() args:  mode, videoresolnWidth, videoresolnHeight, mouseXCord, mouseYCord, WinSize]
                      // videoresolnWidth, videoresolnHeight, mouseXCord, mouseYCord - these parameters are required only when click in preview]
                      // winSize is required only for manual mode
                      onClicked: {
                            seecam130.setROIAutoFoucs(See3Cam130.AFCentered, 0, 0, 0, 0, 0);
                      }                      
                      Keys.onReturnPressed: {
                            seecam130.setROIAutoFoucs(See3Cam130.AFCentered, 0, 0, 0, 0, 0);
                      }
                  }
                  RadioButton {
                      exclusiveGroup: roiAfgroup
                      id: afManual
                      text: "Manual"
                      activeFocusOnPress: true
                      style: econRadioButtonStyle                     
                      opacity: enabled ? 1 : 0.1
                      onClicked: {
                            seecam130.setROIAutoFoucs(See3Cam130.AFManual, 0, 0, 0, 0, afWindowSizeCombo.currentText)
                      }
                      Keys.onReturnPressed: {
                            seecam130.setROIAutoFocusMode(See3Cam130.AFManual, 0, 0, 0, 0, afWindowSizeCombo.currentText);
                      }
                  }
            }

            ComboBox
            {
                id: afWindowSizeCombo                
                enabled: afManual.checked ? true : false
                opacity: enabled ? 1 : 0.1
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
                    if(settingafWinSizeWhenUpdateInUI){
                        seecam130.setROIAutoFoucs(See3Cam130.AFManual, 0, 0, 0, 0, afWindowSizeCombo.currentText)
                    }
                    settingafWinSizeWhenUpdateInUI = true
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
                  spacing:25
                  ExclusiveGroup { id: roiExpogroup }
                  RadioButton {
                      exclusiveGroup: roiExpogroup
                      id: autoexpFull
                      text: "Full"
                      activeFocusOnPress: true
                      style: econRadioButtonStyle                      
                      opacity: enabled ? 1 : 0.1
                      onClicked: {
                        seecam130.setROIAutoExposure(See3Cam130.AutoExpFull, 0, 0, 0, 0, 0);
                      }
                      Keys.onReturnPressed: {

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
                          seecam130.setROIAutoExposure(See3Cam130.AutoExpManual, 0, 0, 0, 0, 0);
                      }
                      Keys.onReturnPressed: {

                      }
                  }
            }

            ComboBox
            {
                id: autoExpoWinSizeCombo
                opacity: autoexpManual.checked ? 1 : 0.1                
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
                    if(settingautoExpWinSizeWhenUpdateInUI){
                        seecam130.setROIAutoExposure(See3Cam130.AutoExpManual, 0, 0, 0, 0, autoExpoWinSizeCombo.currentText)
                    }
                    settingautoExpWinSizeWhenUpdateInUI = true
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
                        if(settingWhenUpdateInUI){
                            seecam130.setQFactor(qFactorSlider.value)
                        }
                        settingWhenUpdateInUI = true
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
                id: imgCapText
                text: "--- Image Capture ---"
                font.pixelSize: 14
                font.family: "Ubuntu"
                color: "#ffffff"
                smooth: true
                Layout.alignment: Qt.AlignCenter
                opacity: 0.50196078431373
            }
            Text {
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
                model: ListModel {
                    ListElement { text: "1" }
                    ListElement { text: "2" }
                    ListElement { text: "3" }
                    ListElement { text: "4" }
                    ListElement { text: "5" }
                }
                activeFocusOnPress: true
                style: econComboBoxStyle
                onCurrentIndexChanged: {
                    if(settingBurstLengthWhenUpdateInUI){
                        seecam130.setBurstLength(burstLengthCombo.currentText)
                    }
                    settingBurstLengthWhenUpdateInUI = true
                }
            }
            Row{
                Layout.alignment: Qt.AlignCenter
                Button {
                    id: f_wversion_selectedCU130
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
            }
        }
    }


    See3Cam130 {
        id: seecam130
        onSendSceneMode: {
            defaultSceneMode(sceneMode)
        }
        onSendEffectMode: {
            defaultEffectMode(effectMode)
        }
        onSendDenoiseValue:{
            deNoiseSlider.value = denoiseValue
        }        
        onSendAfMode:{
            defaultAfMode(afMode)
        }
        onSendHDRMode:{
            defaultHDRMode(hdrMode)
            if(hdrMode == See3Cam130.HdrManual){
                iHDRSlider.value = hdrValue
            }
        }
        onSendqFactor:{
              qFactorSlider.value = qFactor
        }
        onSendROIAfMode:{
            if(roiMode == See3Cam130.AFCentered){                
                afCentered.checked = true
            }else if(roiMode == See3Cam130.AFManual){
                settingafWinSizeWhenUpdateInUI = false
                afManual.checked = true
                afWindowSizeCombo.currentIndex = winSize-1
            }else if(roiMode == See3Cam130.AFDisabled){
                afCentered.enabled = false
                afManual.enabled = false
                afWindowSizeCombo.enabled = false
            }
        }
        onSendROIAutoExpMode:{
            if(roiMode == See3Cam130.AutoExpFull){                
                autoexpFull.checked = true
            }else if(roiMode == See3Cam130.AutoExpManual){
                settingautoExpWinSizeWhenUpdateInUI = false
                autoexpManual.checked = true
                autoExpoWinSizeCombo.currentIndex = winSize-1
            }
            else if(roiMode == See3Cam130.AutoExpDisabled){
                autoexpFull.enabled = false
                autoexpManual.enabled = false
                autoExpoWinSizeCombo.enabled = false
            }
        }
        onSendBurstLength:{
            settingBurstLengthWhenUpdateInUI = false
            burstLengthCombo.currentIndex = burstLength - 1
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
    }}

    Component {
        id: econComboBoxStyle
        ComboBoxStyle {
            background: Image {
                id: burstLengthCombo_bkgrnd
                source: "../../videocapturefilter/images/device_box.png"
                //source: "../videocapturefilter/images/device_box.png"
                Rectangle {
                    width: burstLengthCombo_bkgrnd.sourceSize.width  - 28
                    height: burstLengthCombo_bkgrnd.sourceSize.height
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

    Component.onCompleted:{
        //uvccamera.initExtensionUnit("See3CAM_130")
        seecam130.getSceneMode()
        seecam130.getEffectMode()
        seecam130.getAutoFocusMode()
        seecam130.getiHDRMode()
        seecam130.getDenoiseValue()
        seecam130.getQFactor()
        seecam130.getBurstLength()
        seecam130.getAutoFocusROIModeAndWindowSize()
        seecam130.getAutoExpROIModeAndWindowSize()
    }
    Component.onDestruction: {
        //uvccamera.exitExtensionUnit()
    }

    function getSerialNumber() {
        uvccamera.getSerialNumber()
        messageDialog.open()
    }

    function getFirmwareVersion() {
        uvccamera.getFirmWareVersion()
        messageDialog.open()
    }

    function defaultSceneMode(mode)
    {
        switch(mode)
        {
            case See3Cam130.SCENE_NORMAL:
                sceneNormal.checked = true
                break;
            case See3Cam130.SCENE_DOCUMENT:
                sceneDoc.checked = true
                break;
        }
    }
    function defaultEffectMode(mode)
    {
        switch(mode)
        {
            case See3Cam130.EFFECT_NORMAL:
                effectNormal.checked = true
                break;
            case See3Cam130.EFFECT_BLACK_WHITE:
                effectBW.checked = true
                break;
            case See3Cam130.EFFECT_GREYSCALE:
                effectGrayscale.checked = true
                break;
            case See3Cam130.EFFECT_NEGATIVE:
                effectNegative.checked = true
                break;
            case See3Cam130.EFFECT_SKETCH:
                effectSketch.checked = true
                break;
        }
    }

    function defaultAfMode(mode)
    {
        switch(mode)
        {
            case See3Cam130.Continuous:
                radioContin.checked = true
                break;
            case See3Cam130.OneShot:
                radioOneshot.checked = true
                break;
            case See3Cam130.AfModeDisabled:
                radioContin.enabled = false
                radioOneshot.enabled = false
                trigger.enabled = false
                break;
        }
    }

    function defaultHDRMode(mode)
    {
        switch(mode)
        {
            case See3Cam130.HdrOff:
                hdrOff.checked = true
                break;
            case See3Cam130.HdrAuto:
                hdrAuto.checked = true
                break;
            case See3Cam130.HdrManual:
                hdrManual.checked = true
                break;
        }
    }

}

