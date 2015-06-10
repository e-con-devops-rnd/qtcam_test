.pragma library
var masterMode_M=1;
var triggerMode_M=0;
var masterMode_B=1;
var triggerMode_B=0;
var masterMode_11cug=1;
var triggerMode_11cug=0;
var masterMode_ar0130=1;
var triggerMode_ar0130=0;
var masterMode_cu51=1;
var triggerMode_cu51=0;
var stillCaptureFormat
var stillCaptureResolution
var videoCaptureFormat
var videoCaptureResolution
var videoExtension
var videoEncoder
var cameraComboIndex

function enableTriggerMode_10cugM() {
    triggerMode_M=1;
    masterMode_M=0;   
}

function enableMasterMode_10cugM() {
    triggerMode_M=0;
    masterMode_M=1;
}

function enableTriggerMode_10cugB() {
    triggerMode_B=1;
    masterMode_B=0;
}

function enableMasterMode_10cugB() {
    triggerMode_B=0;
    masterMode_B=1;
}

function enableTriggerMode_11cug() {
    triggerMode_11cug=1;
    masterMode_11cug=0;
}

function enableMasterMode_11cug() {
    triggerMode_11cug=0;
    masterMode_11cug=1;
}

function enableTriggerMode_ar0130() {
    triggerMode_11cug=1;
    masterMode_11cug=0;
}

function enableMasterMode_ar0130() {
    triggerMode_ar0130=0;
    masterMode_ar0130=1;
}

function enableTriggerMode_cu51() {
    triggerMode_11cug=1;
    masterMode_11cug=0;
}

function enableMasterMode_cu51() {
    triggerMode_cu51=0;
    masterMode_cu51=1;
}

function checkVideoExtension() {
    videoExtension = 'mkv'
}
