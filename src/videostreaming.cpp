/*
 * videostreaming.cpp -- preview the camera buffer
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

#include "videostreaming.h"
#include "common.h"
#include <QtCore/QCoreApplication>
#include <QtGui/QGuiApplication>
#include <QtWidgets>
#include <QDesktopWidget>
#include <QLabel>
#include <QMessageBox>
#include <QRectF>
#include <fcntl.h>
#include <QString>
#include <QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QtConcurrent>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

/* MACRO FOR BAYER10 TO RGB24 */
#define R(x, y, w)	y16BayerDestBuffer[0 + 3 * ((x) + (w) * (y))]
#define G(x, y, w)	y16BayerDestBuffer[1 + 3 * ((x) + (w) * (y))]
#define B(x, y, w)	y16BayerDestBuffer[2 + 3 * ((x) + (w) * (y))]

#define Bay(x, y, w) bayerIRBuffer[(x) + (w) * (y)]

//Modified by Nithyesh
/*
 * Previously it was
 * #define CLIP(x) ((x) < 0? 0 :((x) >= 255)? 255 : (x))
 */
#define CLIP(x) (((x) >= 255)? 255 : (x))

/* Jpeg-decode */
#define HEADERFRAME1 0xaf

#ifndef min
 #define min(a,b) ((a)<(b)?(a):(b))
#endif


QStringListModel Videostreaming::resolution;
QStringListModel Videostreaming::stillOutputFormat;
QStringListModel Videostreaming::videoOutputFormat;
QStringListModel Videostreaming::fpsList;
QStringListModel Videostreaming::encoderList;
int Videostreaming::deviceNumber;
QString Videostreaming::camDeviceName;

typedef void (*ftopict) (int * out, uint8_t *pic, int width) ;

//Added by Dhurka
/**
 * @brief Videostreaming::currentlySelectedCameraEnum - This contains currenly selected camera enum value
 */
CommonEnums::ECameraNames Videostreaming::currentlySelectedCameraEnum;

static GLfloat mVerticesDataPosition[] = {
    -1.f, 1.f, 0.0f, // Position 0
    -1.f, -1.f, 0.0f, // Position 1
    1.f, -1.f, 0.0f, // Position 2
    1.f, 1.f, 0.0f, // Position 3
};

static GLfloat mVerticesDataTextCord[] = {
     0.0f, 0.0f, // TexCoord 0
     0.0f, 1.0f, // TexCoord 1
     1.0f, 1.0f, // TexCoord 2
     1.0f, 0.0f // TexCoord 3
};

static unsigned short mIndicesData[] = { 0, 1, 2, 0, 2, 3 };

Videostreaming::Videostreaming() : m_t(0)
    , m_renderer(0)
{
    openSuccess = false;
    updateOnce = true;
    m_snapShot = false;
    m_burstShot = false;
    makeSnapShot = false;
    changeFpsAndShot = false;
    triggerShot = false;
    m_displayCaptureDialog = false;
    m_saveImage = false;
    m_VideoRecord = false;
    dotile = 0;
    // Modified by Sankari : Dec 5 2018, converted TJPF_RGB to TJPF_RGBA and use RGB[RGBA] shader
    pf = TJPF_RGBA;
    warmup = 1;
    //Modified by Nithyesh
    /*
     * Previously was sf = {1, 1};
     * See http://stackoverflow.com/questions/7007591/syntax-in-assigning-to-map-of-structs
     */
    sf.denom = 1;
    sf.num = 1;
    flags = TJFLAG_NOREALLOC;
    yuvpad = 1;
    frameToSkip = 1;
    fpsChangedForStill = false;
    imgSaveSuccessCount = 0;   
    // Initialize all buffers
    y16BayerDestBuffer = NULL;
    h264Decode = NULL;
    yuyvBuffer = NULL;
    bayerIRBuffer = NULL;
    yuv420pdestBuffer = NULL;
    y16BayerFormat = false;
    audio_buffer_data = NULL;
    m_capNotifier = NULL;
    m_capImage = NULL;
    frameSkip = false;
    tempSrcBuffer = NULL;

    connect(this, &QQuickItem::windowChanged, this, &Videostreaming::handleWindowChanged);
    connect(&audioinput, SIGNAL(captureAudio()), this, SLOT(doEncodeAudio()));
    connect(this, SIGNAL(captureVideo()), this, SLOT(recordVideo()));

    videoEncoder=new VideoEncoder();
}

Videostreaming::~Videostreaming()
{
    delete videoEncoder;
    videoEncoder=NULL;
}

void Videostreaming::fillRenderBuffer(){
    m_renderer->fillBuffer();
}

/**
* fillBuffer - In case of YUYV format, split YUYV to ybuffer, ubuffer, vbuffer.
*              In case of RGBA, no need to split - directly use rgb shader
*/
void FrameRenderer::fillBuffer(){    
    if(renderBufferFormat == CommonEnums::YUYV_BUFFER_RENDER){        
        if(renderMutex.tryLock()){
            if(yuvBuffer != NULL){
                if(gotFrame){
                    updateStop = false;   // If frame is available , draw in screen. otherwise no need to draw anything.
                }else{
                    updateStop = true;
                }
                u_int8_t *pyuv = yuvBuffer;
                const u_int8_t *pyuv_end = pyuv + (videoResolutionwidth*videoResolutionHeight*2);
                int j = 0, k = 0; // destination

                for( ; pyuv < pyuv_end;  ) {
                        yBuffer[j] = *(pyuv); // y
                        uBuffer[k] = *(pyuv + 1); // u
                        yBuffer[++j] = *(pyuv + 2); // y
                        vBuffer[k] = *(pyuv + 3); // v
                        ++j;
                        ++k;
                        pyuv += 4;
                    }
            }            
            renderMutex.unlock();
        }
    }
    else if(renderBufferFormat == CommonEnums::RGB_BUFFER_RENDER){
        if(gotFrame){
            updateStop = false;
        }else{
            updateStop = true;
        }        
    }
}

/**
*   When sequential animation 't' value changed, this function is called [Q_PROPERTY]
*/
void Videostreaming::setT(qreal t)
{    
    if (t == m_t)
        return;
    m_t = t;
    emit tChanged();    

    fillRenderBuffer();
    if (window()){
        window()->update();
    }
}

/**
* handleWindowChanged - This will be called when QQuickItem::windowChanged signal fires.
*/
void Videostreaming::handleWindowChanged(QQuickWindow *win)
{    
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &Videostreaming::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &Videostreaming::cleanup, Qt::DirectConnection);
//! [1]
        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
//! [3]
        win->setClearBeforeRendering(false);
    }
}

/**
* cleanup - This will be called when QQuickItem::sceneGraphInvalidated signal fires.
*/
void Videostreaming::cleanup()
{    
    if (m_renderer) {
        delete m_renderer;
        m_renderer = 0;
    }
}

/**
* cleanup - This will be called when QQuickItem::beforeSynchronizing signal fires.
*/
void Videostreaming::sync()
{    
    if (!m_renderer) {
        m_renderer = new FrameRenderer();
        m_renderer->videoResolutionwidth = 640; // need to check this assignment is needed.
        m_renderer->videoResolutionHeight = 480;
        connect(window(), &QQuickWindow::afterRendering, m_renderer, &FrameRenderer::paint, Qt::DirectConnection);
    }
    m_renderer->setViewportSize(QSize(window()->width(),window()->height()));
    m_renderer->setT(m_t);
    m_renderer->setWindow(window());
}


FrameRenderer::~FrameRenderer()
{    
    // Free buffers finally
    free(yBuffer);
    free(uBuffer);
    free(vBuffer);
    free(yuvBuffer);
    free(rgbaDestBuffer);    
    delete m_programRGB;
    delete m_programYUYV;
}


FrameRenderer::FrameRenderer(): m_t(0), m_programRGB(0),  m_programYUYV(0){    
    yBuffer = NULL;
    uBuffer = NULL;
    vBuffer = NULL;
    rgbaDestBuffer = NULL;   
    gotFrame = false;
    updateStop = true;
}

/**
 * @brief FrameRenderer::calculateViewportWidth - calculate view port width to maintain aspect ratio
 * @param vidResolutionWidth - video preview resolution width
 * @param vidResolutionHeight - video preview resolution height
 * @param windowHeight - window renderbackground area height
 * @param *x - to store x position
 * @param *y - to store y position
 * @param *destWindowWidth - to store target window viewport width
 * @param *destWindowHeight - to store target window viewport height 
 */
void FrameRenderer::calculateViewport(int vidResolutionWidth, int vidResolutionHeight, int windowWidth, int windowHeight, int *x, int *y, int *destWindowWidth, int *destWindowHeight){    
    int original_width = vidResolutionWidth;
    int original_height = vidResolutionHeight;
    int bound_width = windowWidth;
    int bound_height = windowHeight;
    int new_width = original_width;
    int new_height = original_height;

    // first check if we need to scale width
    if (original_width > bound_width) {
        //scale width to fit
        new_width = bound_width;
        //scale height to maintain aspect ratio
        new_height = (new_width * original_height) / original_width;
    }

    // then check if we need to scale even with the new height
    if (new_height > bound_height) {
        //scale height to fit instead
        new_height = bound_height;
        //scale width to maintain aspect ratio
        new_width = (new_height * original_width) / original_height;
    }

    *destWindowWidth  = new_width;
    *destWindowHeight  = new_height;
    *x = (((windowWidth - new_width)/2));
    *y = (((windowHeight - new_height)/2));
}


/**
 * @brief FrameRenderer::drawRGBABUffer - Shader for RGBA buffer and render
 */
void FrameRenderer::drawRGBBUffer(){
    if (!m_programRGB) {        
        initializeOpenGLFunctions();

        m_programRGB = new QOpenGLShaderProgram();

        m_programRGB->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                                    "attribute vec4 a_position;\n"
                                                    "attribute vec2 a_texCoord;\n"
                                                    "varying vec2 v_texCoord;\n"
                                                    "void main()\n"
                                                    "{\n"
                                                    "gl_Position = a_position;\n"
                                                    "v_texCoord = a_texCoord;\n"
                                                    "}\n");
        m_programRGB->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                           "varying vec2 v_texCoord;"
                                           "uniform sampler2D texture;"
                                           "vec4 color;"
                                           "void main() {"
                                               "color = texture2D(texture, v_texCoord);"
                                               "gl_FragColor = color;"
                                           "}\n");

        m_programRGB->bindAttributeLocation("a_position", 0);
        m_programRGB->bindAttributeLocation("a_texCoord", 1);
        m_programRGB->link();

        mPositionLoc = m_programRGB->attributeLocation("a_position");
        mTexCoordLoc = m_programRGB->attributeLocation("a_texCoord");


        /*********** Y-Texture**************/
        glEnable(GL_TEXTURE_2D);
        samplerLocRGB = m_programRGB->uniformLocation("texture");
        GLuint yTextureId;
        glGenTextures (1, &yTextureId); // Generate a texture object
        glActiveTexture(GL_TEXTURE1);
        glBindTexture (GL_TEXTURE_2D, yTextureId);
    }

    m_programRGB->bind();    

    glVertexAttribPointer(mPositionLoc, 3, GL_FLOAT, false, 12, mVerticesDataPosition);
    glVertexAttribPointer(mTexCoordLoc, 2, GL_FLOAT, false, 8, mVerticesDataTextCord);

    m_programRGB->enableAttributeArray(0);
    m_programRGB->enableAttributeArray(1);

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(samplerLocRGB, 1);

    // set necessary texture parameters
    glTexParameteri(GL_TEXTURE_2D,        GL_TEXTURE_WRAP_S,            GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,        GL_TEXTURE_WRAP_T,            GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D,        GL_TEXTURE_MIN_FILTER,        GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,        GL_TEXTURE_MAG_FILTER,        GL_LINEAR);

    int xMargin = 250; // [left margin + right margin ]
    int sidebarwidth;

    QRect rec = QApplication::desktop()->screenGeometry();
    if(sidebarAvailable){
        sidebarwidth = rec.width() - previewBgrdAreaWidth;
    }else{
        sidebarwidth = 0;
    }

    int x, y, destWindowWidth, destWindowHeight;
    if(previewBgrdAreaHeight == 0){
		calculateViewport(videoResolutionwidth, videoResolutionHeight, previewBgrdAreaWidth-xMargin, m_viewportSize.height(), &x, &y, &destWindowWidth, &destWindowHeight);
    }else{
		calculateViewport(videoResolutionwidth, videoResolutionHeight, previewBgrdAreaWidth-xMargin, previewBgrdAreaHeight, &x, &y, &destWindowWidth, &destWindowHeight);
    }
    glViewport(sidebarwidth+x+(xMargin/2), y+(m_viewportSize.height()-previewBgrdAreaHeight), destWindowWidth, destWindowHeight);

    QMutexLocker locker(&renderMutex);
    glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGBA, videoResolutionwidth, videoResolutionHeight, 0,GL_RGBA , GL_UNSIGNED_BYTE, rgbaDestBuffer);

    if(gotFrame && !updateStop){
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, mIndicesData);
    }

    m_programRGB->disableAttributeArray(0);
    m_programRGB->disableAttributeArray(1);
    m_programRGB->removeAllShaders();
    m_programRGB->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    m_window->resetOpenGLState();
}

/**
 * @brief FrameRenderer::drawYUYVBUffer - Shader for yuyv to RGB conversion and render buffer
 */
void FrameRenderer::drawYUYVBUffer(){    
    if (!m_programYUYV) {
            initializeOpenGLFunctions();
            m_programYUYV = new QOpenGLShaderProgram();
            m_programYUYV->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                                        "attribute vec4 a_position;\n"
                                                        "attribute vec2 a_texCoord;\n"
                                                        "varying vec2 v_texCoord;\n"
                                                        "void main()\n"
                                                        "{\n"
                                                        "gl_Position = a_position;\n"
                                                        "v_texCoord = a_texCoord;\n"
                                                        "}\n");
            m_programYUYV->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                                        "#ifdef GL_ES\n"
                                                                 "precision highp float;\n"
                                                                 "#endif\n"

                                                                 "varying vec2 v_texCoord;\n"
                                                                 "uniform sampler2D y_texture;\n"
                                                                 "uniform sampler2D u_texture;\n"
                                                                 "uniform sampler2D v_texture;\n"


                                                                 "void main()\n"
                                                                 "{\n"
                                                                     "float r, g, b, y, u, v;\n"

                                                                     //We had put the Y values of each pixel to the R,G,B components by
                                                                     //GL_LUMINANCE, that's why we're pulling it from the R component,
                                                                     //we could also use G or Ba_position
                                                                     "y = texture2D(y_texture, v_texCoord).r;\n"

                                                                     //We had put the U and V values of each pixel to the A and R,G,B
                                                                     //components of the texture respectively using GL_LUMINANCE_ALPHA.
                                                                     //Since U,V bytes are interspread in the texture, this is probably
                                                                     //the fastest way to use them in the shader
                                                                     "u = texture2D(u_texture, v_texCoord).r - 0.5;\n"
                                                                     "v = texture2D(v_texture, v_texCoord).r - 0.5;\n"

                                                                     //The numbers are just YUV to RGB conversion constants
                                                                     "r = y + 1.5701 * v;\n"
                                                                     "g = y - 0.1870 * u - 0.4664 * v;\n"
                                                                     "b = y + 1.8556 * u;\n"
                                                                     "gl_FragColor = vec4(r,g,b,1.0);\n"
                                                                 "}\n");


            m_programYUYV->bindAttributeLocation("a_position", 0);
            m_programYUYV->bindAttributeLocation("a_texCoord", 1);
            m_programYUYV->link();

            mPositionLoc = m_programYUYV->attributeLocation("a_position");
            mTexCoordLoc = m_programYUYV->attributeLocation("a_texCoord");

            /*********** Y-Texture**************/
            glEnable(GL_TEXTURE_2D);
            samplerLocY = m_programYUYV->uniformLocation("y_texture");
            GLuint yTextureId;
            glGenTextures (1, &yTextureId); // Generate a texture object
            glActiveTexture(GL_TEXTURE1);
            glBindTexture (GL_TEXTURE_2D, yTextureId);

            /*********** U-Texture**************/
            glEnable(GL_TEXTURE_2D);
            samplerLocU = m_programYUYV->uniformLocation("u_texture");
            GLuint uTextureId;
            glGenTextures (1, &uTextureId); // Generate a texture object
            glActiveTexture(GL_TEXTURE2);
            glBindTexture (GL_TEXTURE_2D, uTextureId);

            /*********** V-Texture**************/
            glEnable(GL_TEXTURE_2D);
            samplerLocV = m_programYUYV->uniformLocation("v_texture");
            GLuint vTextureId;
            glGenTextures (1, &vTextureId); // Generate a texture object
            glActiveTexture(GL_TEXTURE3);
            glBindTexture (GL_TEXTURE_2D, vTextureId);
            updateStop = true;

        }
    //! [4] //! [5]
    //!
        renderyuyvMutex.lock();

        m_programYUYV->bind();        

        glVertexAttribPointer(mPositionLoc, 3, GL_FLOAT, false, 12, mVerticesDataPosition);
        glVertexAttribPointer(mTexCoordLoc, 2, GL_FLOAT, false, 8, mVerticesDataTextCord);

        m_programYUYV->enableAttributeArray(0);
        m_programYUYV->enableAttributeArray(1);


    int xMargin = 250; // [left margin + right margin ]
    int sidebarwidth;

    QRect rec = QApplication::desktop()->screenGeometry();
    if(sidebarAvailable){
        sidebarwidth = rec.width() - previewBgrdAreaWidth;
    }else{
        sidebarwidth = 0;
    }
	
	// calculate view port 
	int x, y, destWindowWidth, destWindowHeight;
	if(previewBgrdAreaHeight == 0){
		calculateViewport(videoResolutionwidth, videoResolutionHeight, previewBgrdAreaWidth-xMargin, m_viewportSize.height(), &x, &y, &destWindowWidth, &destWindowHeight);
	}else{
		calculateViewport(videoResolutionwidth, videoResolutionHeight, previewBgrdAreaWidth-xMargin, previewBgrdAreaHeight, &x, &y, &destWindowWidth, &destWindowHeight);
	}

        // set view port
	glViewport(sidebarwidth+x+(xMargin/2), y+(m_viewportSize.height()-previewBgrdAreaHeight), destWindowWidth, destWindowHeight);

        if (yBuffer != NULL && uBuffer != NULL && vBuffer != NULL){
            if(gotFrame && !updateStop){
	     // set active texture and give input y buffer
            glActiveTexture(GL_TEXTURE1);
            glUniform1i(samplerLocY, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, videoResolutionwidth, videoResolutionHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, yBuffer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	    // set active texture and give input u buffer
            glActiveTexture(GL_TEXTURE2);
            glUniform1i(samplerLocU, 2);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, videoResolutionwidth/2, videoResolutionHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, uBuffer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	    // set active texture and give input v buffer
            glActiveTexture(GL_TEXTURE3);
            glUniform1i(samplerLocV, 3);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, videoResolutionwidth/2, videoResolutionHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, vBuffer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);            
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, mIndicesData);
            }
        }

        m_programYUYV->disableAttributeArray(0);
        m_programYUYV->disableAttributeArray(1);


        m_programYUYV->removeAllShaders();
        m_programYUYV->release();

        // Not strictly needed for this example, but generally useful for when
        // mixing with raw OpenGL.
        m_window->resetOpenGLState();

    renderyuyvMutex.unlock();
}

/**
* paint in Quick painted item (qml)
*/
void FrameRenderer::paint()
{    
    if(renderBufferFormat == CommonEnums::RGB_BUFFER_RENDER){ // RGBA
        drawRGBBUffer();
    }else if(renderBufferFormat == CommonEnums::YUYV_BUFFER_RENDER){ // YUYV
        drawYUYVBUffer();
    }
}

void Videostreaming::setDevice(QString deviceName) {    
    close();
    deviceName.append(QString::number(deviceNumber,10));
    if(open(deviceName,false)) {
        emit logDebugHandle("Device Opened - "+deviceName);
        m_convertData = v4lconvert_create(fd());
        m_buftype= V4L2_BUF_TYPE_VIDEO_CAPTURE;
        openSuccess = true;

        // Added by Sankari: 12 Feb 2018
        // Get the bus info details and send to qml for selected camera
        struct v4l2_capability querycapability;
        if (querycap(querycapability)) {
            QString bus(reinterpret_cast< char* >(querycapability.bus_info));
            emit pciDeviceBus(bus);
        }
    } else {
        emit logCriticalHandle("Device Opening Failed - "+deviceName);
    }
}

void Videostreaming::getFirstDevice(int deviceNode) {    
    deviceNumber = deviceNode;
}

void Videostreaming::getCameraName(QString deviceName){    
    camDeviceName=deviceName;
}


double Videostreaming::getTimeInSecs()
{
    struct timeval tv;
    if(gettimeofday(&tv, NULL)<0) return 0.0;
    else return (double)tv.tv_sec+((double)tv.tv_usec/1000000.);
}

/**
* SaveRawFile - Direct Buffer dump
* @param: inputBuffer - buffer to save.
* @param: buffersize - input buffer size
* return true/false
*/
bool Videostreaming::saveRawFile(void *inputBuffer, int buffersize){    
    if(inputBuffer == NULL){
        return false;
    }
    bool ret = false;
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly)) {
        int tmpRet = file.write((const char*)inputBuffer, buffersize);
        if(tmpRet != -1) {
            ret = true;
        } else {
            emit logCriticalHandle("Failure to save raw image");
        }
        file.close();
    }
    return ret;
}

/**
* SaveIRImage 
* return true/false
*/
bool Videostreaming::saveIRImage(){    
    if(!bayerIRBuffer){
        return false;
    }

    unsigned char *irBuffer = NULL;
    irBuffer = (unsigned char *)malloc(width * height/4);
    if(extractIRImage(bayerIRBuffer, irBuffer)){
        QImage qImage2(irBuffer, width/2, height/2, QImage::Format_Indexed8);
        QImageWriter writer(filename);

        /* For 8 bit bmp, We have to use Format_Indexed8 and set color table */
        QVector<QRgb>table;
        for(int i=0; i<256; i++)
            table.push_back(qRgb(i,i,i));
        qImage2.setColorTable(table);

        if(!writer.write(qImage2)) {            
            emit logCriticalHandle("Error while saving an image:"+writer.errorString());
            free(irBuffer); irBuffer = NULL;
            return false;
        }

    }
    free(irBuffer); irBuffer = NULL;
    return true;
}


void Videostreaming::setPreviewBgrndArea(int width, int height, bool sidebarAvailable){    
    if(m_renderer){
        m_renderer->previewBgrdAreaHeight = height;
        m_renderer->previewBgrdAreaWidth = width;
        m_renderer->sidebarAvailable = sidebarAvailable;
    }    
}

void Videostreaming::capFrame()
{    
    __u32 buftype = m_buftype;
    v4l2_plane planes[VIDEO_MAX_PLANES];
    v4l2_buffer buf;
    bool again;

    memset(planes, 0, sizeof(planes));
    buf.length = VIDEO_MAX_PLANES;
    buf.m.planes = planes;
    if (!dqbuf_mmap(buf, buftype, again)) {
        closeDevice();
        // Added by Sankari:19 Dec 2017.
        //Bug Fix: 1. Streaming is not available for higher resolution when unplug and plug cu130 camera without closing application
        v4l2_requestbuffers reqbufs;
        if (m_buffers == NULL)
            return;

        for (uint i = 0; i < m_nbuffers; ++i)
            for (unsigned p = 0; p < m_buffers[i].planes; p++)
                if (-1 == munmap(m_buffers[i].start[p], m_buffers[i].length[p]))
                    perror("munmap");

        // Free all buffers.
        reqbufs_mmap(reqbufs, V4L2_BUF_TYPE_VIDEO_CAPTURE, 1);  // videobuf workaround
        reqbufs_mmap(reqbufs, V4L2_BUF_TYPE_VIDEO_CAPTURE, 0);

        // When device is unplugged, Stop rendering.
        m_renderer->updateStop = true;

        emit deviceUnplugged("Disconnected","Device Not Found");
        emit logCriticalHandle("Device disconnected");
        return;
    }

    if (again) {
        return;
    }

    if (buf.flags & V4L2_BUF_FLAG_ERROR) {
        qbuf(buf);
        return;
    }


   
    // prepare yuyv/rgba buffer and give to shader.
    if(!prepareBuffer(m_capSrcFormat.fmt.pix.pixelformat, m_buffers[buf.index].start[0], buf.bytesused)){
        qbuf(buf);
        return;
    }

    if(!m_snapShot){
        m_renderer->gotFrame = true;
    }
   

    if(m_snapShot || m_burstShot){
        int err = -1;
        if(!y16BayerFormat){ //  y16 bayer format means these conversions are not needed. Calculations are done in "prepareBuffer" function itself.
            // Ex: cu40 camera
            if(m_capSrcFormat.fmt.pix.pixelformat == V4L2_PIX_FMT_Y16){ // y16
                copy.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
                err = v4lconvert_convert(m_convertData, &copy, &m_capDestFormat,
                                         (unsigned char *)m_renderer->yuvBuffer, buf.bytesused,
                                         m_capImage->bits(), m_capDestFormat.fmt.pix.sizeimage); // yuyv to rgb conversion

            }else if(m_capSrcFormat.fmt.pix.pixelformat == V4L2_PIX_FMT_H264 && !m_VideoRecord){ // capture and save image in h264 format[not for video recording]
                v4l2_format tmpSrcFormat = m_capSrcFormat;
                tmpSrcFormat.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
                err = v4lconvert_convert(m_convertData, &tmpSrcFormat, &m_capDestFormat,
                                         (unsigned char *)yuv420pdestBuffer, (width* height * 3)/2,
                                         m_capImage->bits(), m_capDestFormat.fmt.pix.sizeimage); // yuv420p to rgb conversion
            }else{
                err = v4lconvert_convert(m_convertData, &m_capSrcFormat, &m_capDestFormat,
                                         (unsigned char *)m_buffers[buf.index].start[0], buf.bytesused,
                        m_capImage->bits(), m_capDestFormat.fmt.pix.sizeimage); // src format to rgb conversion
            }
            if(err == -1){
                logCriticalHandle(v4lconvert_get_error_message(m_convertData));
            }
        }

    }

    // Taking single shot or burst shot - Skip frames if needed
    if(((m_frame > frameToSkip) && m_snapShot) || ((m_frame > frameToSkip) && m_burstShot)){
         getFileName(getFilePath(),getImageFormatType());

        if(formatType == "raw"){ // save incoming buffer directly
            if(saveRawFile(m_buffers[buf.index].start[0], buf.bytesused)) {imgSaveSuccessCount++;}
        }else if(formatType == "IR data(8bit BMP)"){ // save IR data
            if(saveIRImage()) {imgSaveSuccessCount++;}
        }
        else{ // save png, jpg, bmp files
            unsigned char *bufferToSave = NULL;
            if(y16BayerFormat){ // y16 format - ex: cu40 camera
                bufferToSave = y16BayerDestBuffer;
            }else{
                bufferToSave = m_capImage->bits(); // image data converted using v4l2convert
            }
            QImage qImage3(bufferToSave, width, height, QImage::Format_RGB888);
            QImageWriter writer(filename);
            if(!writer.write(qImage3)) {
                emit logCriticalHandle("Error while saving an image:"+writer.errorString());                
            }else{
                imgSaveSuccessCount++;
            }
        }
        if(triggerShot) {
            captureSaveTime("Capture time: " +(QString::number((double)captureTime.elapsed()/1000)) + "seconds");
            makeSnapShot = false;
            m_snapShot = false;
            m_burstShot = false;
        } else if(changeFpsAndShot){
            captureSaveTime("Capture time: " +(QString::number((double)captureTime.elapsed()/1000)) + "seconds");
            doAfterChangeFPSAndShot();
        }else {
            if(m_saveImage){
                captureSaveTime("Capture time: " +(QString::number((double)captureTime.elapsed()/1000)) + "seconds");
            }
            makeSnapShot = false;
            m_snapShot = false;
            if(m_burstNumber == m_burstLength){
                m_snapShot = false;
                if (!((stillSize == lastPreviewSize) && (stillOutFormat == lastFormat)))
                {
                    if(m_displayCaptureDialog){
                        formatSaveSuccess(m_burstShot);
                    }
                    m_burstShot = false;

                    // after taking shot(s), restore preview resoln and format.
                    stopCapture();
                    vidCapFormatChanged(lastFormat);
                    setResoultion(lastPreviewSize);
                    startAgain();
                    //m_renderer->updateStop = false;
                    return void();
                }
                else{
                    emit logDebugHandle("still and preview resolution and format are same");
                }
                if(m_displayCaptureDialog){
                    formatSaveSuccess(m_burstShot);
                }
                m_burstShot = false;
            }
            // increasing burst number
            m_burstNumber++;
        }
    }
    if(y16BayerDestBuffer){
	free(y16BayerDestBuffer);
	y16BayerDestBuffer = NULL;
    }

    if(bayerIRBuffer){
	free(bayerIRBuffer);
	bayerIRBuffer = NULL;
    }   
    qbuf(buf);
}

/**
* jpegDecode - mjpeg decode to RGB
*/
int Videostreaming::jpegDecode(Videostreaming *obj, unsigned char **pic, unsigned char *buf, unsigned long bytesUsed)
{
    QMutexLocker locker(&obj->m_renderer->renderMutex);


    tjhandle handle = NULL;
    tjtransform *t = NULL;

    int w = 0, h = 0, subsamp = -1, _w, _h;
    int i, tilew, tileh, ntilesw = 1, ntilesh =1 , retval = 0;
    int _tilew, _tileh, xformopt=0;

    unsigned char **jpegbuf = NULL, *srcbuf = NULL;
    unsigned long *jpegsize = NULL, srcSize;

    srcSize = bytesUsed;

    if((srcbuf=(unsigned char *)malloc(srcSize))==NULL)
        obj->logDebugHandle("allocating memory");

    memcpy(srcbuf,buf,srcSize);

    if((handle = tjInitTransform()) == NULL){
        obj->logDebugHandle("executing tjInitTransform()");
		goto bailout;
    }

    if(tjDecompressHeader2(handle, srcbuf, srcSize, &w, &h, &subsamp)==-1){
        obj->logDebugHandle("tjDecompressHeader2()");
		goto bailout;
    }

    if(w == 0){ goto bailout;}
    if(h == 0){ goto bailout;} //If tilew or tileh is zero, goto end


    for(tilew=obj->dotile? 16:w, tileh=obj->dotile? 16:h; ; tilew*=2, tileh*=2)
    {
        if(tilew>w)
            tilew = w;

        if(tileh>h)
            tileh = h;

	        ntilesw = (w+tilew-1) / tilew;
        ntilesh = (h+tileh-1) / tileh;

        if((jpegbuf = (unsigned char **)malloc(sizeof(unsigned char *)
                                             *ntilesw*ntilesh)) == NULL){
            obj->logDebugHandle("allocating JPEG tile array ");
       	    goto bailout;
	}

        memset(jpegbuf, 0, sizeof(unsigned char *)*ntilesw*ntilesh);

        if((jpegsize = (unsigned long *)malloc(sizeof(unsigned long)
                                             *ntilesw*ntilesh)) == NULL){
            obj->logDebugHandle("allocating JPEG size array");
	    	goto bailout;
	}

        memset(jpegsize, 0, sizeof(unsigned long)*ntilesw*ntilesh);

        if((obj->flags&TJFLAG_NOREALLOC)!=0 || !obj->dotile)
            for(i=0; i<ntilesw*ntilesh; i++)
            {
                if((jpegbuf[i] = (unsigned char *)tjAlloc(tjBufSize(tilew, tileh,
                                                                  subsamp))) == NULL){
                    obj->logDebugHandle("allocating JPEG tiles");
		    		goto bailout;
		}
            }

        _tilew = tilew;
        _tileh = tileh;

        _w = w;
        _h = h;

        jpegsize[0] = srcSize;
        memcpy(jpegbuf[0], srcbuf, srcSize); /* Important Step */

        if(w == tilew)
            _tilew = _w;

        if(h == tileh)
            _tileh = _h;

        if(!(xformopt & TJXOPT_NOOUTPUT))
        {
            if(decomp(obj, jpegbuf, jpegsize, NULL, _w, _h, 0,
                          _tilew, _tileh, pic)==-1){
                    goto bailout;
            }
        }

        for(i=0; i<ntilesw*ntilesh; i++)
        {
            tjFree(jpegbuf[i]);
            jpegbuf[i] = NULL;
        }
        free(jpegbuf);
        jpegbuf = NULL;

        if(jpegsize)
        {
            free(jpegsize);
            jpegsize = NULL;
        }
        if(tilew==w && tileh==h)
            break;
    }

  if(obj->m_VideoRecord){
       if(obj->videoEncoder!=NULL) {           
           QMutexLocker lockerRecord(&obj->recordMutex);
	   if(obj->videoEncoder->ok){
	   	obj->videoEncoder->encodeImage(*pic, true);
	   }
           lockerRecord.unlock();
       }
   }
    if(!obj->m_VideoRecord){ // when stop recording , we need to close the recorded file and do not allow record. So return.
            QMutexLocker lockerRecord(&obj->recordMutex);
	    if(obj->videoEncoder->ok){
            	obj->videoEncoder->closeFile();
	    }
            lockerRecord.unlock();
    }
   
bailout:
    if(jpegbuf)
    {
        for(i=0; i<ntilesw*ntilesh; i++)
        {
            if(jpegbuf[i])
                tjFree(jpegbuf[i]);

            jpegbuf[i] = NULL;
        }
        free(jpegbuf);
        jpegbuf = NULL;
    }
    if(jpegsize) {
        free(jpegsize);
        jpegsize = NULL;
    }
    if(srcbuf) {
        free(srcbuf);
        srcbuf = NULL;
    }
    if(t) {
        free(t);
        t = NULL;
    }
    if(handle) {
        tjDestroy(handle);
        handle = NULL;
    }

   locker.unlock();

   obj->frameSkip = false;
   return retval;
}

/* Decompression test */
int Videostreaming::decomp(Videostreaming *obj,unsigned char **jpegbuf,
    unsigned long *jpegsize, unsigned char *dstbuf, int w, int h,
    int jpegqual, int tilew, int tileh,unsigned char **pic)
{
    tjhandle handle = NULL;
    char  qualstr[6] = "\0";

    double elapsed, elapsedDecode;

    int ps = tjPixelSize[obj->pf];
    int scaledw = TJSCALED(w, obj->sf);
    int scaledh = TJSCALED(h, obj->sf);
    int pitch = scaledw * ps;
    int ntilesw = (w + tilew - 1) / tilew;
    int ntilesh = (h + tileh - 1)/tileh;
    int row, col, iter = 0, retval = 0;

    unsigned char *dstptr, *dstptr2;

    if(jpegqual>0)
    {
        snprintf(qualstr, 6, "_Q%d", jpegqual);
        qualstr[5]=0;
    }

    if((handle = tjInitDecompress()) == NULL){
        emit obj->logCriticalHandle("tjInitDecompress() failed");
        retval = -1;
        goto bailout;
    }

    /* Benchmark */
    iter = -obj->warmup;
    elapsed = elapsedDecode = 0.;

    if(1)
    {
        int tile = 0;
        double start = obj->getTimeInSecs();
        for(row=0, dstptr=dstbuf; row<ntilesh; row++, dstptr+=pitch*tileh)
        {
            for(col=0, dstptr2=dstptr; col<ntilesw; col++, tile++, dstptr2+=ps*tilew)
            {
                int width = obj->dotile? min(tilew, w-col*tilew):scaledw;
                int height = obj->dotile? min(tileh, h-row*tileh):scaledh;

                if(tjDecompress2(handle, jpegbuf[tile], jpegsize[tile], *pic,
                                    width, pitch, height, obj->pf, obj->flags) == -1){
                    //emit logCriticalHandle("tjDecompress2() failed");
                    retval = -1;
                    goto bailout;
                }
            }
        }
        iter++;
        if(iter >= 1)
            elapsed += obj->getTimeInSecs()-start;
    }

    if(1) elapsed -= elapsedDecode;

    if(tjDestroy(handle) == -1){
        //emit logCriticalHandle("tjDestroy() failed");
        retval = -1;
        goto bailout;
    }

    handle = NULL;

    bailout:
    if(handle)
        tjDestroy(handle);

    return retval;
}


//To do: need to move in a separate file
void convert_border_bayer_line_to_bgr24( uint8_t* bayer, uint8_t* adjacent_bayer,
    uint8_t *bgr, int width, uint8_t start_with_green, uint8_t blue_line)
{
    int t0, t1;

    if (start_with_green)
    {
    /* First pixel */
        if (blue_line)
        {
            *bgr++ = bayer[1];
            *bgr++ = bayer[0];
            *bgr++ = adjacent_bayer[0];
        }
        else
        {
            *bgr++ = adjacent_bayer[0];
            *bgr++ = bayer[0];
            *bgr++ = bayer[1];
        }
        /* Second pixel */
        t0 = (bayer[0] + bayer[2] + adjacent_bayer[1] + 1) / 3;
        t1 = (adjacent_bayer[0] + adjacent_bayer[2] + 1) >> 1;
        if (blue_line)
        {
            *bgr++ = bayer[1];
            *bgr++ = t0;
            *bgr++ = t1;
        }
        else
        {
            *bgr++ = t1;
            *bgr++ = t0;
            *bgr++ = bayer[1];
        }
        bayer++;
        adjacent_bayer++;
        width -= 2;
    }
    else
    {
        /* First pixel */
        t0 = (bayer[1] + adjacent_bayer[0] + 1) >> 1;
        if (blue_line)
        {
            *bgr++ = bayer[0];
            *bgr++ = t0;
            *bgr++ = adjacent_bayer[1];
        }
        else
        {
            *bgr++ = adjacent_bayer[1];
            *bgr++ = t0;
            *bgr++ = bayer[0];
        }
        width--;
    }

    if (blue_line)
    {
        for ( ; width > 2; width -= 2)
        {
            t0 = (bayer[0] + bayer[2] + 1) >> 1;
            *bgr++ = t0;
            *bgr++ = bayer[1];
            *bgr++ = adjacent_bayer[1];
            bayer++;
            adjacent_bayer++;

            t0 = (bayer[0] + bayer[2] + adjacent_bayer[1] + 1) / 3;
            t1 = (adjacent_bayer[0] + adjacent_bayer[2] + 1) >> 1;
            *bgr++ = bayer[1];
            *bgr++ = t0;
            *bgr++ = t1;
            bayer++;
            adjacent_bayer++;
        }
    }
    else
    {
        for ( ; width > 2; width -= 2)
        {
            t0 = (bayer[0] + bayer[2] + 1) >> 1;
            *bgr++ = adjacent_bayer[1];
            *bgr++ = bayer[1];
            *bgr++ = t0;
            bayer++;
            adjacent_bayer++;

            t0 = (bayer[0] + bayer[2] + adjacent_bayer[1] + 1) / 3;
            t1 = (adjacent_bayer[0] + adjacent_bayer[2] + 1) >> 1;
            *bgr++ = t1;
            *bgr++ = t0;
            *bgr++ = bayer[1];
            bayer++;
            adjacent_bayer++;
        }
    }

    if (width == 2)
    {
        /* Second to last pixel */
        t0 = (bayer[0] + bayer[2] + 1) >> 1;
        if (blue_line)
        {
            *bgr++ = t0;
            *bgr++ = bayer[1];
            *bgr++ = adjacent_bayer[1];
        }
        else
        {
            *bgr++ = adjacent_bayer[1];
            *bgr++ = bayer[1];
            *bgr++ = t0;
        }
        /* Last pixel */
        t0 = (bayer[1] + adjacent_bayer[2] + 1) >> 1;
        if (blue_line)
        {
            *bgr++ = bayer[2];
            *bgr++ = t0;
            *bgr++ = adjacent_bayer[1];
        }
        else
        {
            *bgr++ = adjacent_bayer[1];
            *bgr++ = t0;
            *bgr++ = bayer[2];
        }
    }
    else
    {
        /* Last pixel */
        if (blue_line)
        {
            *bgr++ = bayer[0];
            *bgr++ = bayer[1];
            *bgr++ = adjacent_bayer[1];
        }
        else
        {
            *bgr++ = adjacent_bayer[1];
            *bgr++ = bayer[1];
            *bgr++ = bayer[0];
        }
    }
}

//To do: need to move in a separate file
void bayer_to_rgbbgr24(uint8_t *bayer,
    uint8_t *bgr, int width, int height,
    uint8_t start_with_green, uint8_t blue_line)
{
    /* render the first line */
    convert_border_bayer_line_to_bgr24(bayer, bayer + width, bgr, width,
        start_with_green, blue_line);
    bgr += width * 3;

    /* reduce height by 2 because of the special case top/bottom line */
    for (height -= 2; height; height--)
    {
        int t0, t1;
        /* (width - 2) because of the border */
        uint8_t *bayerEnd = bayer + (width - 2);

        if (start_with_green)
        {
            /* OpenCV has a bug in the next line, which was
            t0 = (bayer[0] + bayer[width * 2] + 1) >> 1; */
            t0 = (bayer[1] + bayer[width * 2 + 1] + 1) >> 1;
            /* Write first pixel */
            t1 = (bayer[0] + bayer[width * 2] + bayer[width + 1] + 1) / 3;
            if (blue_line)
            {
                *bgr++ = t0;
                *bgr++ = t1;
                *bgr++ = bayer[width];
            }
            else
            {
                *bgr++ = bayer[width];
                *bgr++ = t1;
                *bgr++ = t0;
            }

            /* Write second pixel */
            t1 = (bayer[width] + bayer[width + 2] + 1) >> 1;
            if (blue_line)
            {
                *bgr++ = t0;
                *bgr++ = bayer[width + 1];
                *bgr++ = t1;
            }
            else
            {
                *bgr++ = t1;
                *bgr++ = bayer[width + 1];
                *bgr++ = t0;
            }
            bayer++;
        }
        else
        {
            /* Write first pixel */
            t0 = (bayer[0] + bayer[width * 2] + 1) >> 1;
            if (blue_line)
            {
                *bgr++ = t0;
                *bgr++ = bayer[width];
                *bgr++ = bayer[width + 1];
            }
            else
            {
                *bgr++ = bayer[width + 1];
                *bgr++ = bayer[width];
                *bgr++ = t0;
            }
        }

        if (blue_line)
        {
            for (; bayer <= bayerEnd - 2; bayer += 2)
            {
                t0 = (bayer[0] + bayer[2] + bayer[width * 2] +
                    bayer[width * 2 + 2] + 2) >> 2;
                t1 = (bayer[1] + bayer[width] +
                    bayer[width + 2] + bayer[width * 2 + 1] +
                    2) >> 2;
                *bgr++ = t0;
                *bgr++ = t1;
                *bgr++ = bayer[width + 1];

                t0 = (bayer[2] + bayer[width * 2 + 2] + 1) >> 1;
                t1 = (bayer[width + 1] + bayer[width + 3] +
                    1) >> 1;
                *bgr++ = t0;
                *bgr++ = bayer[width + 2];
                *bgr++ = t1;
            }
        }
        else
        {
            for (; bayer <= bayerEnd - 2; bayer += 2)
            {
                t0 = (bayer[0] + bayer[2] + bayer[width * 2] +
                    bayer[width * 2 + 2] + 2) >> 2;
                t1 = (bayer[1] + bayer[width] +
                    bayer[width + 2] + bayer[width * 2 + 1] +
                    2) >> 2;
                *bgr++ = bayer[width + 1];
                *bgr++ = t1;
                *bgr++ = t0;

                t0 = (bayer[2] + bayer[width * 2 + 2] + 1) >> 1;
                t1 = (bayer[width + 1] + bayer[width + 3] +
                    1) >> 1;
                *bgr++ = t1;
                *bgr++ = bayer[width + 2];
                *bgr++ = t0;
            }
        }

        if (bayer < bayerEnd)
        {
            /* write second to last pixel */
            t0 = (bayer[0] + bayer[2] + bayer[width * 2] +
                bayer[width * 2 + 2] + 2) >> 2;
            t1 = (bayer[1] + bayer[width] +
                bayer[width + 2] + bayer[width * 2 + 1] +
                2) >> 2;
            if (blue_line)
            {
                *bgr++ = t0;
                *bgr++ = t1;
                *bgr++ = bayer[width + 1];
            }
            else
            {
                *bgr++ = bayer[width + 1];
                *bgr++ = t1;
                *bgr++ = t0;
            }
            /* write last pixel */
            t0 = (bayer[2] + bayer[width * 2 + 2] + 1) >> 1;
            if (blue_line)
            {
                *bgr++ = t0;
                *bgr++ = bayer[width + 2];
                *bgr++ = bayer[width + 1];
            }
            else
            {
                *bgr++ = bayer[width + 1];
                *bgr++ = bayer[width + 2];
                *bgr++ = t0;
            }
            bayer++;
        }
        else
        {
            /* write last pixel */
            t0 = (bayer[0] + bayer[width * 2] + 1) >> 1;
            t1 = (bayer[1] + bayer[width * 2 + 1] + bayer[width] + 1) / 3;
            if (blue_line)
            {
                *bgr++ = t0;
                *bgr++ = t1;
                *bgr++ = bayer[width + 1];
            }
            else
            {
                *bgr++ = bayer[width + 1];
                *bgr++ = t1;
                *bgr++ = t0;
            }
        }

        /* skip 2 border pixels */
        bayer += 2;

        blue_line = !blue_line;
        start_with_green = !start_with_green;
    }

    /* render the last line */
    convert_border_bayer_line_to_bgr24(bayer + width, bayer, bgr, width,
        !start_with_green, !blue_line);
}

//To do: need to move in a separate file
void rgb2yuyv(uint8_t *prgb, uint8_t *pyuv, int width, int height)
{

    int i=0;
    for(i=0;i<(width*height*3);i=i+6)
    {  // use integer conversion
        /* y */
        *pyuv++ =CLIP(0.299 * (prgb[i] - 128) + 0.587 * (prgb[i+1] - 128) + 0.114 * (prgb[i+2] - 128) + 128);
        /* u */
        *pyuv++ =CLIP(((- 0.147 * (prgb[i] - 128) - 0.289 * (prgb[i+1] - 128) + 0.436 * (prgb[i+2] - 128) + 128) +
            (- 0.147 * (prgb[i+3] - 128) - 0.289 * (prgb[i+4] - 128) + 0.436 * (prgb[i+5] - 128) + 128))/2);
        /* y1 */
        *pyuv++ =CLIP(0.299 * (prgb[i+3] - 128) + 0.587 * (prgb[i+4] - 128) + 0.114 * (prgb[i+5] - 128) + 128);
        /* v*/
        *pyuv++ =CLIP(((0.615 * (prgb[i] - 128) - 0.515 * (prgb[i+1] - 128) - 0.100 * (prgb[i+2] - 128) + 128) +
            (0.615 * (prgb[i+3] - 128) - 0.515 * (prgb[i+4] - 128) - 0.100 * (prgb[i+5] - 128) + 128))/2);
    }
}

// Added by Sankari: Nov 8 2017 . prepare yuv buffer and give to shader.
bool Videostreaming::prepareBuffer(__u32 pixformat, void *inputbuffer, __u32 bytesUsed){
    if(pixformat == V4L2_PIX_FMT_MJPEG){
        m_renderer->renderBufferFormat = CommonEnums::RGB_BUFFER_RENDER;
        if(m_capSrcFormat.fmt.pix.pixelformat == V4L2_PIX_FMT_MJPEG){
            if(bytesUsed <= HEADERFRAME1) {
                emit logCriticalHandle("Ignoring empty buffer");
                return false;
            }
            if(((uint8_t *) inputbuffer)[0] == 0xFF && ((uint8_t *) inputbuffer)[1] == 0xD8){               
                if(!frameSkip){       		    
		    getFrameRates();             
                    frameSkip = true;
                    memcpy(tempSrcBuffer, (unsigned char *)inputbuffer, bytesUsed);
                    QtConcurrent::run(jpegDecode, this, &m_renderer->rgbaDestBuffer, tempSrcBuffer, bytesUsed);

                }else{                
                }
            }
            else{
                return false;
            }
        }
        return true;
    }else{
        uint8_t *srcBuffer = NULL;
        uint8_t *destBuffer = NULL;
        getFrameRates();
        m_renderer->renderyuyvMutex.lock();

        // cu40 cam - flag
        if(y16BayerFormat){ // y16 - 10bit bayer

            m_renderer->renderBufferFormat = CommonEnums::YUYV_BUFFER_RENDER;

            y16BayerDestBuffer = (unsigned char *)malloc(width * height * 3);

            bayerIRBuffer = (unsigned short int *)malloc(m_renderer->videoResolutionwidth * m_renderer->videoResolutionHeight * 2);

            if(bayerIRBuffer == NULL){
                return false;
            }
            memcpy(bayerIRBuffer, inputbuffer, (width*height*2));
            __u32 x, y;
            for(x = 0; x < width; x += 2)  /* Nearest neighbour interpolation algorithm - y16 to RGB24 conversion */
            {
                for( y = 0; y < height; y += 2)
                {
                   B(x, y, width) = B(x + 1, y, width) = B(x, y + 1, width) = B(x + 1, y + 1, width) = CLIP(Bay(x, y, width));
                   G(x, y, width) = G(x + 1, y, width) = G(x, y + 1, width) = G(x + 1, y + 1, width) = CLIP(Bay(x + 1, y, width));
                   R(x, y, width) = R(x + 1, y, width) = R(x, y + 1, width) = R(x + 1, y + 1, width) = CLIP(Bay(x + 1, y + 1, width));
                }
            }
            rgb2yuyv(y16BayerDestBuffer, yuyvBuffer, width, height);
            m_renderer->yuvBuffer = yuyvBuffer;
        }else{
            switch(pixformat){
                case V4L2_PIX_FMT_YUYV:{
                    m_renderer->renderBufferFormat = CommonEnums::YUYV_BUFFER_RENDER;
                    m_renderer->yuvBuffer = (uint8_t *)inputbuffer; /* directly giving yuyv to render */
                }
                break;

                case V4L2_PIX_FMT_SGRBG8:{  // BA8 to yuyv conversion
                    m_renderer->renderBufferFormat = CommonEnums::YUYV_BUFFER_RENDER;
                    destBuffer = (uint8_t *)malloc(width * height * 3);
                    bayer_to_rgbbgr24((uint8_t *)inputbuffer, destBuffer, width, height, 1, 1);
                    rgb2yuyv(destBuffer, yuyvBuffer, width, height);
                    m_renderer->yuvBuffer = yuyvBuffer;
                    freeBuffer(destBuffer);
                }
                break;

                case V4L2_PIX_FMT_GREY:{ // grey to yuyv conversion
                    m_renderer->renderBufferFormat = CommonEnums::YUYV_BUFFER_RENDER;
                    uint8_t *pfmb = yuyvBuffer;
                    for(int h=0;h<height;h++)
                    {
                        int offset = width * h;
                        for(int w=0;w<width;w++)
                        {
                            *pfmb++=(((uint8_t *) inputbuffer)[w + offset]); // y
                            *pfmb++=0x80;                  //U or V
                        }
                    }
                    m_renderer->yuvBuffer = yuyvBuffer;
                }
                break;

                case V4L2_PIX_FMT_UYVY:{ // uyvy to yuyv conversion
                    m_renderer->renderBufferFormat = CommonEnums::YUYV_BUFFER_RENDER;
                    uint8_t *ptmp = (uint8_t *)inputbuffer;
                    uint8_t *pfmb = yuyvBuffer;
                    for(int h=0;h<height;h++)             /* uyvy to yuyv conversion */
                    {
                        int w=0;
                        for(w=0;w<(width*2);w+=4)
                        {
                            pfmb[0] = ptmp[1]; /* Y0 */
                            pfmb[1] = ptmp[0]; /* U */
                            pfmb[2] = ptmp[3]; /* Y1 */
                            pfmb[3] = ptmp[2]; /* V */
                            ptmp += 4;
                            pfmb += 4;
                        }
                    }
                    m_renderer->yuvBuffer = yuyvBuffer;
                }
                break;
                case V4L2_PIX_FMT_H264:{
                    m_renderer->renderBufferFormat = CommonEnums::YUYV_BUFFER_RENDER;
                    // check - decode h264 to yuyv available

                    h264Decode->decodeH264(yuv420pdestBuffer, (uint8_t *) inputbuffer, bytesUsed); /* decode h264 to yuv420p */
                    h264Decode->yu12_to_yuyv(yuyvBuffer, yuv420pdestBuffer, width, height); /*yuv420p to yuyv conversion */
                    m_renderer->yuvBuffer = yuyvBuffer;
                }
                break;

                case V4L2_PIX_FMT_Y16:{
                    m_renderer->renderBufferFormat = CommonEnums::YUYV_BUFFER_RENDER;
                    srcBuffer = (uint8_t *)malloc(width * height * 2);

                    uint8_t *pfmb = yuyvBuffer;
                    memcpy(srcBuffer, inputbuffer, (width * height * 2));
                    for(__u32 l=0; l<(width * height*2); l=l+2) /* Y16 to YUYV conversion */
                    {
                       *pfmb++ = (((srcBuffer[l] & 0xF0) >> 4) | (srcBuffer[l+1] & 0x0F) << 4);
                       *pfmb++ = 0x80;
                    }
                    m_renderer->yuvBuffer = yuyvBuffer;
                    freeBuffer(srcBuffer);
                }
                break;
            }
        }
        if(m_renderer->renderBufferFormat == CommonEnums::YUYV_BUFFER_RENDER){
            if(m_VideoRecord){
                if(videoEncoder!=NULL) {
        #if LIBAVCODEC_VER_AT_LEAST(54,25)
                        if(pixformat == V4L2_PIX_FMT_H264 && videoEncoder->pOutputFormat->video_codec == AV_CODEC_ID_H264){
        #else
                        if(pixformat == V4L2_PIX_FMT_H264 && videoEncoder->pOutputFormat->video_codec == CODEC_ID_H264){
        #endif
                            videoEncoder->writeH264Image(inputbuffer, bytesUsed);
                        }else{
                            captureVideo();
                        }
                }
            }
        }
        m_renderer->renderyuyvMutex.unlock();
    }
    return true;
}

// Added by Sankari - 04 Jan 2017
/**
 * @brief Videostreaming::doAfterChangeFPSAndShot
 */
void Videostreaming::doAfterChangeFPSAndShot(){
    makeSnapShot = false;
    m_snapShot = false;
    m_burstShot = false;
    // Restore preview color space, resolution, fps.
    if(fpsChangedForStill){
        stopCapture();
        vidCapFormatChanged(lastFormat);
        setResoultion(lastPreviewSize);
        frameIntervalChanged(lastFPSValue.toUInt());
        startAgain();
    }
}

void Videostreaming::freeBuffer(unsigned char *ptr)
{
    if(ptr) {
        free(ptr); ptr = NULL;
    }
}

//Modified by Nithyesh
//Changed type of imgHeight and imgWidth from int to __u32
bool Videostreaming::extractIRImage(unsigned short int *srcBuffer, unsigned char *irBuffer)
{
    bool ret = 1;
    unsigned int irBufferLocation = 0;

    if(srcBuffer != NULL && irBuffer != NULL)
    {
        for(__u32 imgHeight = 1; imgHeight < height; imgHeight += 2)
        {
            for(__u32 imgWidth = 0; imgWidth < width; imgWidth += 2)
            {
                irBuffer[irBufferLocation++] = srcBuffer[(imgHeight * width) + imgWidth] >> 2;
            }
        }
    }
    else
    {
        ret = 0;
    }
    return ret;
}

void Videostreaming::freeBuffers(unsigned char *destBuffer, unsigned char *copyBuffer)
{
    if(copyBuffer || destBuffer)
    {
        free(copyBuffer);
        copyBuffer = NULL;
        destBuffer = NULL;
    }

}

void Videostreaming::getFrameRates() {
    struct timeval tv, res;
    if (m_frame == 0)
        gettimeofday(&m_tv, NULL);
    gettimeofday(&tv, NULL);
    timersub(&tv, &m_tv, &res);
    if (res.tv_sec) {
        m_fps = (100 * (m_frame - m_lastFrame)) / (res.tv_sec * 100 + res.tv_usec / 10000);
        m_lastFrame = m_frame;
        m_tv = tv;
    }
    ++m_frame;
    emit averageFPS(m_fps);
}

bool Videostreaming::startCapture()
{    
    __u32 buftype = m_buftype;
    v4l2_requestbuffers req;
    unsigned int i;

    memset(&req, 0, sizeof(req));

    if (!reqbufs_mmap(req, buftype, 3)) {
        emit logCriticalHandle("Cannot capture");
        return false;
    }

    if (req.count < 2) {
        emit logCriticalHandle("Too few buffers");
        reqbufs_mmap(req, buftype);
        return false;
    }

    m_buffers = (buffer *)calloc(req.count, sizeof(*m_buffers));

    if (!m_buffers) {
        emit logCriticalHandle("Out of memory");
        reqbufs_mmap(req, buftype);
        return false;
    }

    for (m_nbuffers = 0; m_nbuffers < req.count; ++m_nbuffers) {
        v4l2_plane planes[VIDEO_MAX_PLANES];
        v4l2_buffer buf;

        memset(&buf, 0, sizeof(buf));

        buf.type        = buftype;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = m_nbuffers;
        buf.length      = VIDEO_MAX_PLANES;
        buf.m.planes    = planes;

        if (-1 == ioctl(VIDIOC_QUERYBUF, &buf)) {
            perror("VIDIOC_QUERYBUF");
            return false;
        }

        m_buffers[m_nbuffers].planes = 1;
        m_buffers[m_nbuffers].length[0] = buf.length;
        m_buffers[m_nbuffers].start[0] = mmap(buf.length, buf.m.offset);
        if (MAP_FAILED == m_buffers[m_nbuffers].start[0]) {
            perror("mmap");
            return false;
        }
    }
    for (i = 0; i < m_nbuffers; ++i) {
        if (!qbuf_mmap(i, buftype)) {
            emit logCriticalHandle("Querying Buffer failed");
            perror("VIDIOC_QBUF");
            return false;
        }
    }
    if (!streamon(buftype)) {
        emit logCriticalHandle("Stream ON failed");
        perror("VIDIOC_STREAMON");
        return false;
    }
    return true;
}

int Videostreaming::findMax(QList<int> *list) {

    int array[list->count()];
    for(int j=0;j<list->count();j++)
        array[j] = list->at(j);
    int index_of_min = 0;
    for(int x=0; x<list->count(); x++)
    {
        index_of_min = x;
        for(int y=x; y<list->count(); y++)
        {
            if(array[index_of_min] > array[y])
            {
                index_of_min = y;
            }
        }
        int temp = array[x];
        array[x] = array[index_of_min];
        array[index_of_min] = temp;
    }
    return array[index_of_min];
}

/**
 * @brief Videostreaming::updateFrameToSkip
 * @param stillSkip
 */
void Videostreaming::updateFrameToSkip(uint stillSkip){
    frameToSkip = stillSkip;
}

void Videostreaming::makeShot(QString filePath,QString imgFormatType) {    
    captureTime.start();
    // Added by Sankari : to set still skip
    emit stillSkipCount(stillSize, lastPreviewSize, stillOutFormat);
    m_snapShot = true;
    m_burstShot = false;
    m_burstNumber = 1;
    m_burstLength = 1; // for single shot
    m_saveImage = true;

    /* cu40 - IR image in bmp format */
    if(imgFormatType == "IR data(8bit BMP)"){
        formatType = imgFormatType;
        imgFormatType = "bmp";
    }else{ // other image formats or other cameras
        formatType = imgFormatType;
    }

    getFileName(filePath, imgFormatType);
    makeSnapShot = true;
    triggerShot = false;
    changeFpsAndShot = false;
    m_displayCaptureDialog = true;

    if (!((stillSize == lastPreviewSize) && (stillOutFormat == lastFormat)))
    {
        m_renderer->updateStop = true;
        stopCapture();
        vidCapFormatChanged(stillOutFormat);
        setResoultion(stillSize);
        startAgain();
    }
}

/**
 * @brief Videostreaming::changeFPSandTakeShot - Change fps and take shot
 * @param filePath - file path - file location to save
 * @param imgFormatType - image format type like jpg,png,raw,bmp
 * @param fpsIndex - fps list index value need to set
 */
void  Videostreaming::changeFPSandTakeShot(QString filePath,QString imgFormatType, uint fpsIndex){
    captureTime.start();
    m_snapShot = true;
    m_burstShot = false;
    m_burstNumber = 1;
    m_burstLength = 1; // for single shot
    m_saveImage = true;

    formatType = imgFormatType;

    getFileName(filePath, imgFormatType);
    makeSnapShot = true;
    triggerShot = false;
    m_displayCaptureDialog = true;
    changeFpsAndShot = true;
    fpsChangedForStill = false;

    // If lastly setfpsvalue and currently set fps value is different, then set currently selected fps to take still.
    if (!((fpsIndex == lastFPSValue.toUInt()) && (stillSize == lastPreviewSize) && (stillOutFormat == lastFormat)))
    {
        emit stillSkipCountWhenFPSChange(true);
        stopCapture();
        vidCapFormatChanged(stillOutFormat);
        setResoultion(stillSize);
        frameIntervalChanged(fpsIndex);
        startAgain();
        fpsChangedForStill = true;
    }else{
        emit stillSkipCountWhenFPSChange(false);
    }

}

void Videostreaming::triggerModeShot(QString filePath,QString imgFormatType) {

    captureTime.restart();
    m_snapShot = true;
    m_burstShot = false;
    m_burstLength = 1;
    m_burstNumber = 1;

    // emit signal to set still skip count
    emit stillSkipCount(stillSize, lastPreviewSize, stillOutFormat);

    /* cu40 - IR image in bmp format */
    if(imgFormatType == "IR data(8bit BMP)"){
        formatType = imgFormatType;
        imgFormatType = "bmp";
    }else{ // other image formats or other cameras
        formatType = imgFormatType;
    }

    getFileName(filePath, imgFormatType);

    makeSnapShot = true;
    changeFpsAndShot = false;
    triggerShot = true;
    m_saveImage = true;
    m_displayCaptureDialog = false;
    m_frame = 3;
}

void Videostreaming::getFileName(QString filePath,QString imgFormatType){
    QDateTime dateTime = QDateTime::currentDateTime();
    QDir tmpDir;
    if(tmpDir.cd(filePath)) {
        QStringList filters,list;
        filters << "Qtcam_" + dateTime.toString("yy_MM_dd:hh_mm_ss")+"-*"+imgFormatType;
        tmpDir.setNameFilters(filters);
        list << tmpDir.entryList(filters,QDir::Files);
        if(!list.isEmpty()) {
            QList<int> tmpStr;
            for(int i=0;i<list.count();i++) {
                tmpStr.append(list.at(i).split(".").at(0).split("-").at(1).toInt());
            }

            int lastNumber = findMax(&tmpStr);
            lastNumber++;
            filename = filePath +"/Qtcam_" + dateTime.toString("yy_MM_dd:hh_mm_ss")+"-" +QString::number(lastNumber,10)+"."+ imgFormatType;
        } else {
            filename = filePath +"/Qtcam_" + dateTime.toString("yy_MM_dd:hh_mm_ss")+"-" +QString::number(1,10)+"."+ imgFormatType;
        }
    } else {
        QStringList filters,list;
        filters << "Qtcam_" + dateTime.toString("yy_MM_dd:hh_mm_ss")+"-*"+imgFormatType;
        tmpDir.cd(tmpDir.currentPath());
        tmpDir.setNameFilters(filters);
        list << tmpDir.entryList(filters,QDir::Files);
        if(!list.isEmpty()) {
            QList<int> tmpStr;
            for(int i=0;i<list.count();i++) {
                tmpStr.append(list.at(i).split(".").at(0).split("-").at(1).toInt());
            }
            int lastNumber = findMax(&tmpStr);
            lastNumber++;
            filename = "Qtcam_" + dateTime.toString("yy_MM_dd:hh_mm_ss")+"-" +QString::number(lastNumber,10)+"."+ imgFormatType;
        } else {
            filename = "Qtcam_" + dateTime.toString("yy_MM_dd:hh_mm_ss")+"-" +QString::number(1,10)+"."+ imgFormatType;
        }
    }

    setFilePath(filePath);
    setImageFormatType(imgFormatType);
}

void Videostreaming::setFilePath(QString filePath){
    m_filePath = filePath;
}

QString Videostreaming::getFilePath(){
    return m_filePath;
}

void Videostreaming::setImageFormatType(QString imgFormatType){
    m_imgFormatType = imgFormatType;
}

QString Videostreaming::getImageFormatType(){
    return m_imgFormatType;
}


void Videostreaming::makeBurstShot(QString filePath,QString imgFormatType, uint burstLength){    
    captureTime.start();
    m_burstShot = true;
    m_snapShot = false;
    imgSaveSuccessCount = 0;

    // emit signal to set still skip count
    emit stillSkipCount(stillSize, lastPreviewSize, stillOutFormat);

    getFileName(filePath, imgFormatType);
    m_burstLength = burstLength; // no of shots to take
    m_burstNumber = 1;
    formatType = imgFormatType;

    makeSnapShot = true;
    triggerShot = false;
    m_displayCaptureDialog = true;
    m_saveImage = true;
    if (!((stillSize == lastPreviewSize) && (stillOutFormat == lastFormat)))
    {
        // Added by Sankari: disable paint in preview while capturing image when still and preview resolution
        //are different
        m_renderer->updateStop = true;
        stopCapture();
        vidCapFormatChanged(stillOutFormat);
        setResoultion(stillSize);
        startAgain();
    }
}

void Videostreaming::formatSaveSuccess(bool burstFlag) {
    QString imgSaveSuccessCntStr = QString::number(imgSaveSuccessCount);
    if(imgSaveSuccessCount) {
        if(burstFlag){
            _title = "Captured";
            _text = "Captured " +imgSaveSuccessCntStr+ " image(s) and saved successfully in the location:" + m_filePath;
        }else{
            _title = "Captured";
            _text = "Image saved in the location:" + filename;
        }
        emit logDebugHandle("Captured image(s) is(are) saved successfully");
        emit titleTextChanged(_title,_text);
    } else {
        _title = "Failure";
        _text = "Image not saved in the selected location";
        emit logCriticalHandle("Still image not saved successfully");
        emit titleTextChanged(_title,_text);
    }
    // After capturing image need to enable RF rect in See3CAM_130 or See3CAM_30 cam
    emit enableRfRectBackInPreview();

    // After capturing image need to enable face rect in See3CAM_130, See3CAM_CU30 and See3CAM_CU135 cam
    emit enableFactRectInPreview();
}

bool Videostreaming::getInterval(struct v4l2_fract &interval)
{    
    if (m_has_interval)
        interval = m_interval;
    return m_has_interval;
}

void Videostreaming::displayFrame() {    
    emit logDebugHandle("Start Previewing");
    m_frame = m_lastFrame = m_fps = 0;
    emit averageFPS(m_fps);

    __u32 buftype = m_buftype;
    g_fmt_cap(buftype, m_capSrcFormat);

    // if (try_fmt(m_capSrcFormat)) {
    if(!s_fmt(m_capSrcFormat)) {
        emit titleTextChanged("Error", "Device or Resource is busy");
        emit logCriticalHandle("Device or Resource is busy");
        if (fd() >= 0) {
            v4lconvert_destroy(m_convertData);
            close();
        }
        return void();
    }
    //} else {
    //return void();
    //}

    if (getInterval(interval))
        set_interval(buftype, interval);

    emit logDebugHandle("Preview settings completed");

    m_capDestFormat = m_capSrcFormat;
    m_capDestFormat.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;

    //m_capDestFormat.fmt.pix.sizeimage = (m_capDestFormat.fmt.pix.width * m_capDestFormat.fmt.pix.height * 3);
    v4l2_format copy = m_capSrcFormat;
    v4lconvert_try_format(m_convertData, &m_capDestFormat, &m_capSrcFormat);
    // v4lconvert_try_format sometimes modifies the source format if it thinks
    // that there is a better format available. Restore our selected source
    // format since we do not want that happening.

    m_capSrcFormat = copy;
    m_capDestFormat.fmt.pix.width  = m_capSrcFormat.fmt.pix.width;
    m_capDestFormat.fmt.pix.height = m_capSrcFormat.fmt.pix.height;
    width = m_capDestFormat.fmt.pix.width;
    height = m_capDestFormat.fmt.pix.height;
    pixfmt = m_capDestFormat.fmt.pix.pixelformat;
    m_capDestFormat.fmt.pix.sizeimage = width*height*3;

    m_capImage = new QImage(width, height, QImage::Format_RGB888);
    //Modified by Dhurka - 14th Oct 2016
    /*
     * Added camera enum comparision
     * Before its like camera name comparision
     */
    if(currentlySelectedCameraEnum == CommonEnums::ECON_1MP_BAYER_RGB) {
        m_capSrcFormat.fmt.pix.pixelformat = V4L2_PIX_FMT_SGRBG8;
    }

    if(currentlySelectedCameraEnum == CommonEnums::SEE3CAM_CU40) {
        y16BayerFormat = true;
    }

    if(m_capSrcFormat.fmt.pix.pixelformat == V4L2_PIX_FMT_H264){
        h264Decode = new H264Decoder();
        h264Decode->initH264Decoder(width, height);
        yuv420pdestBuffer = (uint8_t *)malloc(width * (height + 8) * 2);
    }

    if (startCapture()) {
        sprintf(header,"P6\n%d %d 255\n",width,height);
        m_capNotifier = new QSocketNotifier(fd(), QSocketNotifier::Read);
        connect(m_capNotifier, SIGNAL(activated(int)), this, SLOT(capFrame()));
    }
}

void Videostreaming::stopCapture() {    

    if(h264Decode!=NULL){
        h264Decode->closeFile();
        delete h264Decode;
        h264Decode=NULL;
    }
    yuyvBuffer = NULL;
    yuv420pdestBuffer = NULL;
    tempSrcBuffer = NULL;

    if(yuyvBuffer != NULL ){
        free(yuyvBuffer);
        yuyvBuffer = NULL;
    }

    if(yuv420pdestBuffer != NULL){
        free(yuv420pdestBuffer);
        yuv420pdestBuffer = NULL;
    }

    if(tempSrcBuffer != NULL){
        free(tempSrcBuffer);
        tempSrcBuffer = NULL;
    }
   
    m_renderer->gotFrame = false;
    m_renderer->updateStop = true;

    y16BayerFormat = false; // BY default this will be false, If cu40 [ y16 bayer format ] is selected ,
    //this will be enabled.

    if (fd() >= 0) {
        emit logDebugHandle("Stop Previewing...");
        v4l2_requestbuffers reqbufs;
        if (m_buffers == NULL)
            return;
        if (!streamoff(V4L2_BUF_TYPE_VIDEO_CAPTURE)) {
            perror("VIDIOC_STREAMOFF");
            emit logCriticalHandle("Stream OFF failed");
        }
        for (uint i = 0; i < m_nbuffers; ++i)
            for (unsigned p = 0; p < m_buffers[i].planes; p++)
                if (-1 == munmap(m_buffers[i].start[p], m_buffers[i].length[p]))
                    perror("munmap");
        // Free all buffers.
        reqbufs_mmap(reqbufs, V4L2_BUF_TYPE_VIDEO_CAPTURE, 1);  // videobuf workaround
        reqbufs_mmap(reqbufs, V4L2_BUF_TYPE_VIDEO_CAPTURE, 0);
        emit logDebugHandle("Value of FD is:"+ QString::number(fd(),10));
        if (m_capNotifier) {           
            delete m_capNotifier;
            m_capNotifier = NULL;
        }        
        if (m_capImage != NULL) {            
//          delete m_capImage;
            m_capImage = NULL;
        }
    }    
}

void Videostreaming::closeDevice() {
    emit logDebugHandle("Closing the current camera device");
    if (fd() >= 0) {
        if (m_capNotifier) {
            delete m_capNotifier;
            delete m_capImage;
            m_capNotifier = NULL;
            m_capImage = NULL;
        }
        v4lconvert_destroy(m_convertData);
        close();
    }    
}

void Videostreaming::startAgain() {    
    m_renderer->videoResolutionwidth = m_width;
    m_renderer->videoResolutionHeight = m_height;

    int buffLength = m_width * m_height;
    int buffHalfLength = (m_width * m_height)/ 2;

    m_renderer->yBuffer = (uint8_t*)malloc(buffLength);
    m_renderer->uBuffer = (uint8_t*)malloc(buffHalfLength);
    m_renderer->vBuffer = (uint8_t*)malloc(buffHalfLength);
    m_renderer->yuvBuffer = (uint8_t*)malloc(buffLength*2);

    m_renderer->rgbaDestBuffer = (unsigned char *)malloc(m_renderer->videoResolutionwidth * (m_renderer->videoResolutionHeight) * 4);
    tempSrcBuffer = (unsigned char *)malloc(m_renderer->videoResolutionwidth * (m_renderer->videoResolutionHeight) * 2);
   
    yuyvBuffer = (uint8_t *)malloc(m_renderer->videoResolutionwidth * m_renderer->videoResolutionHeight * 2);
    
    if(openSuccess) {
        displayFrame();
    }
}

void Videostreaming::lastPreviewResolution(QString resolution,QString format) {    
    lastPreviewSize = resolution;
    lastFormat = format;
    emit logDebugHandle("Last Resolution displayed at::"+resolution);
}

/**
 * @brief Videostreaming::lastFPS - setting lastly set FPS
 * @param fps
 */
void Videostreaming::lastFPS(QString fps) {
    lastFPSValue = fps;
}

void Videostreaming::setResoultion(QString resolution)
{    
    emit logDebugHandle("Resolution set at::"+resolution);
    v4l2_format fmt;
    unsigned int width, height;
    QStringList tempResList = resolution.split('x');
    width = tempResList.value(0).toInt();
    height = tempResList.value(1).toInt();
    g_fmt_cap(V4L2_BUF_TYPE_VIDEO_CAPTURE, fmt);
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    m_width = width;
    m_height = height;
    try_fmt(fmt);
    s_fmt(fmt);
}

/**
 * @brief Videostreaming::getResoultion - get the resolution
 * @return - QString - current resolution
 */
QString Videostreaming::getResoultion()
{

    v4l2_format fmt;
    unsigned int width, height;
    QString resolutionStr;
    g_fmt_cap(V4L2_BUF_TYPE_VIDEO_CAPTURE, fmt);
    width = fmt.fmt.pix.width;
    height = fmt.fmt.pix.height;

    resolutionStr.append(QString::number(width));
    resolutionStr.append("x");
    resolutionStr.append(QString::number(height));

    return resolutionStr;
}

void Videostreaming::displayStillResolution() {
    g_fmt_cap(V4L2_BUF_TYPE_VIDEO_CAPTURE, fmt);

    QStringList dispStillRes;

    m_pixelformat = fmt.fmt.pix.pixelformat;
    m_width       = fmt.fmt.pix.width;
    m_height      = fmt.fmt.pix.height;

    unsigned int indexCount = -1;
    v4l2_frmsizeenum frmsize;
    bool ok = false;
    ok = enum_framesizes(frmsize, m_pixelformat);
    if (ok && frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
        do {
            indexCount++;
            dispStillRes.append(QString("%1x%2").arg(frmsize.discrete.width).arg(frmsize.discrete.height));
            if (frmsize.discrete.width == m_width && frmsize.discrete.height == m_height) {
                emit defaultStillFrameSize(indexCount);
            }
        } while (enum_framesizes(frmsize));
    }
    stillOutputFormat.setStringList(dispStillRes);
    emit logDebugHandle("Supported still Resolution: " +dispStillRes.join(", "));
}

void Videostreaming::displayEncoderList(){
    QStringList encoders;
    QString fileContent;
    encoders.clear();
    // read
    QFile f("/etc/issue");
    if (f.open(QFile::ReadOnly)){
        QTextStream in(&f);
        fileContent.append(in.readAll());
        if((-1 != fileContent.indexOf("12.04")) || (-1 != fileContent.indexOf("14.04"))){
            encoders<<"YUY"<<"MJPG"<<"H264"<<"VP8";
            ubuntuVersion = "<15"; // version less than 15 [ Here ubuntu 12.04 and ubuntu 14.04 ]
        }else{
            encoders<<"MJPG"<<"H264"<<"VP8";
            ubuntuVersion = ">=15"; // version >=  15 [ Here ubuntu 15.10 and ubuntu 16.04 , Linux Mint 18, ubuntu 17.04 ]
        }
        encoderList.setStringList(encoders);
    }
}

void Videostreaming::displayVideoResolution() {
    g_fmt_cap(V4L2_BUF_TYPE_VIDEO_CAPTURE, fmt);

    QStringList dispVideoRes;

    m_pixelformat = fmt.fmt.pix.pixelformat;
    m_width       = fmt.fmt.pix.width;
    m_height      = fmt.fmt.pix.height;

    unsigned int indexCount = -1;
    unsigned int defaultWidth;
    unsigned int defaultHeight;

    v4l2_frmsizeenum frmsize;
    bool ok = false;
    ok = enum_framesizes(frmsize, m_pixelformat);
    if (ok && frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
        do {
            indexCount++;
            dispVideoRes.append(QString("%1x%2").arg(frmsize.discrete.width).arg(frmsize.discrete.height));
            if (frmsize.discrete.width == m_width && frmsize.discrete.height == m_height) {
                defaultWidth = m_width;
                defaultHeight = m_height;
                emit defaultFrameSize(indexCount, defaultWidth, defaultHeight);
            }
        } while (enum_framesizes(frmsize));
    }

    videoOutputFormat.setStringList(dispVideoRes);
    emit logDebugHandle("Supported video Resolution: " +dispVideoRes.join(", "));
}

void Videostreaming::vidCapFormatChanged(QString idx)
{
    v4l2_fmtdesc desc;
    enum_fmt_cap(desc, m_buftype, true, idx.toInt());
    v4l2_format fmt;
    g_fmt_cap(m_buftype, fmt);
    fmt.fmt.pix.pixelformat = desc.pixelformat;
    try_fmt(fmt);
    s_fmt(fmt);
    if(!makeSnapShot){
        updateVidOutFormat();       
    }
}

void Videostreaming::updateVidOutFormat()
{
    v4l2_fmtdesc desc;
    v4l2_format fmt;
    g_fmt_cap(m_buftype, fmt);
    m_pixelformat = fmt.fmt.pix.pixelformat;
    m_width       = fmt.fmt.pix.width;
    m_height      = fmt.fmt.pix.height;
    if (enum_fmt_cap(desc, m_buftype, true)) {
        do {
            if (desc.pixelformat == fmt.fmt.pix.pixelformat)
                break;
        } while (enum_fmt_cap(desc, m_buftype));
    }
    if (desc.pixelformat != fmt.fmt.pix.pixelformat)
        return;
    emit defaultOutputFormat(desc.index);
    emit logDebugHandle("Color Space set to: "+pixfmt2s(m_pixelformat));
}

void Videostreaming::displayOutputFormat() {
    QStringList dispOutFormat;
    v4l2_fmtdesc fmt;
    pixFormat.clear();
    if (enum_fmt_cap(fmt, V4L2_BUF_TYPE_VIDEO_CAPTURE, true)) {
        do {
            QString s(pixfmt2s(fmt.pixelformat) + " (");
            pixFormat.insert(pixfmt2s(fmt.pixelformat), QString::number(fmt.pixelformat));
            if (fmt.flags & V4L2_FMT_FLAG_EMULATED)
                dispOutFormat.append(s + "Emulated)");
            else {
                dispOutFormat.append(s + (const char *)fmt.description + ")");
            }
        } while (enum_fmt_cap(fmt, V4L2_BUF_TYPE_VIDEO_CAPTURE));

    }
    emit logDebugHandle("Output format supported: " +dispOutFormat.join(", "));
    resolution.setStringList(dispOutFormat);
    updateVidOutFormat();
}

void Videostreaming::updateFrameInterval(QString pixelFormat, QString frameSize)
{
    v4l2_frmivalenum frmival;
    bool ok;
    QStringList tempResList = frameSize.split('x');
    width = tempResList.value(0).toInt();
    height = tempResList.value(1).toInt();
    QStringList tempPixFmt = pixelFormat.split(' ');
    QString pixFmtValue = tempPixFmt.value(0);

    /* Actual Format of "Y16" is "Y16 " [Y16 with space]. So append space char */
    if (0 == QString::compare(pixFmtValue, "Y16")){
        pixFmtValue.append(" ");
    }

    ok = enum_frameintervals(frmival,pixFormat.value(pixFmtValue).toInt(), width, height);
    m_has_interval = ok && frmival.type == V4L2_FRMIVAL_TYPE_DISCRETE;
    availableFPS.clear();
    if (m_has_interval) {
        m_interval = frmival.discrete;
    // Added by Sankari: 07 Dec 2017 - Bugfix - Fps index is not updating properly
        emit defaultFrameInterval(frmival.index);
        do {
            availableFPS.append(QString::number((double)frmival.discrete.denominator / frmival.discrete.numerator).append(" FPS"));
        // Removed by Sankari: 07 Dec 2017 - Bugfix - Fps index is not updating properly
        } while (enum_frameintervals(frmival));
    }
    emit logDebugHandle("Available FPS:"+ availableFPS.join(", "));
    // send fps list to qml
    emit sendFPSlist(availableFPS.join(", "));

}

// Added by Sankari: setting stringlist model is moved from updateFrameInterval() as a separate function
void Videostreaming::enumerateFPSList(){
    fpsList.setStringList(availableFPS);
}

void Videostreaming::frameIntervalChanged(int idx)
{
    v4l2_frmivalenum frmival;
    emit logDebugHandle("Pixel Format:"+ QString::number(m_pixelformat));
    emit logDebugHandle("Width:"+ QString::number(m_width));
    emit logDebugHandle("Height:"+ QString::number(m_height));
    emit logDebugHandle("IDX Value:"+QString::number(idx));
    if (enum_frameintervals(frmival, m_pixelformat, m_width, m_height, idx)
            && frmival.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
        if (set_interval(m_buftype, frmival.discrete)) {
            m_interval = frmival.discrete;
        }
    }
}

void Videostreaming::cameraFilterControls(bool actualValue) {
    v4l2_queryctrl qctrl;
    v4l2_querymenu qmenu;
    int indexValue;
    qctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
    emit logDebugHandle("Available Controls:");
    while(queryctrl(qctrl)) {
        emit logDebugHandle((char*)qctrl.name);
        switch (qctrl.type) {
        case V4L2_CTRL_TYPE_BOOLEAN:
            ctrlName = (char*)qctrl.name;
            ctrlType = QString::number(qctrl.type,10);
            ctrlID = QString::number(qctrl.id,10);
            ctrlStepSize = QString::number(qctrl.step,10);
            if(actualValue) {
                emit newControlAdded(ctrlName,ctrlType,ctrlID,ctrlStepSize,QString::number(0,10),QString::number(1,10),getSettings(qctrl.id));
            } else {
                emit newControlAdded(ctrlName,ctrlType,ctrlID,ctrlStepSize,QString::number(0,10),QString::number(1,10),QString::number(qctrl.default_value,10));
            }
            break;
        case V4L2_CTRL_TYPE_INTEGER:
            ctrlName = (char*)qctrl.name;
            ctrlType = QString::number(qctrl.type,10);
            ctrlID = QString::number(qctrl.id,10);
            ctrlMaxValue = QString::number(qctrl.maximum,10);
            ctrlMinValue = QString::number(qctrl.minimum,10);
            ctrlDefaultValue = QString::number(qctrl.default_value,10);
            ctrlStepSize = QString::number(qctrl.step,10);
            if(actualValue) {
                emit newControlAdded(ctrlName,ctrlType,ctrlID,ctrlStepSize,ctrlMinValue,ctrlMaxValue,getSettings(qctrl.id));
            } else {
                emit newControlAdded(ctrlName,ctrlType,ctrlID,ctrlStepSize,ctrlMinValue,ctrlMaxValue,QString::number(qctrl.default_value,10));
            }
            break;
        case V4L2_CTRL_TYPE_MENU:
        case V4L2_CTRL_TYPE_INTEGER_MENU:
            ctrlName = (char*)qctrl.name;
            ctrlType = QString::number(qctrl.type,10);
            ctrlID = QString::number(qctrl.id,10);
            for (int i = qctrl.minimum; i <= qctrl.maximum; i++) {
                qmenu.id = qctrl.id;
                qmenu.index = i;
                if (!querymenu(qmenu))
                    continue;
                if (qctrl.type == V4L2_CTRL_TYPE_MENU) {
                    emit newControlAdded((char *)qmenu.name,ctrlType,ctrlID);
                }
                else {
                    //combo->addItem(QString("%1").arg(qmenu.value));
                    //menuItems.append(qmenu.value);
                }
            }
            indexValue =  getSettings(qctrl.id).toInt();
            if(actualValue) {
                emit newControlAdded(ctrlName,ctrlType,ctrlID,ctrlStepSize,ctrlMinValue,ctrlMaxValue,QString::number(getMenuIndex(qctrl.id,indexValue),10));
            } else {
                emit newControlAdded(ctrlName,ctrlType,ctrlID,ctrlStepSize,ctrlMinValue,ctrlMaxValue,QString::number(getMenuIndex(qctrl.id,qctrl.default_value),10));
            }
            break;
        case V4L2_CTRL_TYPE_BITMASK:
            break;
        case V4L2_CTRL_TYPE_BUTTON:
            break;
        case V4L2_CTRL_TYPE_INTEGER64:
            break;
        case V4L2_CTRL_TYPE_STRING:
            break;
        case V4L2_CTRL_TYPE_CTRL_CLASS:
            break;
        }
        qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
}

QString Videostreaming::getSettings(unsigned int id) {
    struct v4l2_control c;
    c.id = id;
    //Modified by Nithyesh
    /*
     * Previosuly it was
     * if (ioctl(VIDIOC_G_CTRL, &c)) {
     *      v4l2_queryctrl qctrl;
     *      qctrl.id = id;
     *      emit logCriticalHandle("Unable to get the Value, setting the Default value: "+ QString::number(c.value,10));
     *      return QString::number(c.value,10);
     *  }
     *  QString value = QString::number(c.value,10);
     *  return value;
     */
    c.value = 0;
    if (ioctl(VIDIOC_G_CTRL, &c)) {
        //v4l2_queryctrl qctrl;
        //qctrl.id = id;
        emit logCriticalHandle("Unable to get the Value, setting the Default value: "+ QString::number(c.value,10));
        return QString::number(c.value,10);
    }
    QString value = QString::number(c.value,10);
    return value;
}

void Videostreaming::changeSettings(unsigned int id, QString value) {
    struct v4l2_control c;
    c.id = id;
    c.value = value.toInt();
    if (ioctl(VIDIOC_S_CTRL, &c)) {
        emit logCriticalHandle("Error in setting the Value");
    }
}


void Videostreaming::selectMenuIndex(unsigned int id, int value) {
    v4l2_queryctrl qctrl;
    v4l2_querymenu qmenu;
    qctrl.id = id;
    queryctrl(qctrl);
    int i;
    for (i = qctrl.minimum; i <= qctrl.maximum; i++) {
        qmenu.id = qctrl.id;
        qmenu.index = i;
        if (!querymenu(qmenu))
            continue;
        if (value-- == 0)
            break;
    }
    changeSettings(id,QString::number(i,10));
}

int Videostreaming::getMenuIndex(unsigned int id,int value) {
    v4l2_queryctrl qctrl;
    v4l2_querymenu qmenu;
    qctrl.id = id;
    queryctrl(qctrl);
    int i, j = 0;
    for (i = qctrl.minimum; i <= qctrl.maximum; i++) {
        qmenu.id = qctrl.id;
        qmenu.index = i;
        if (!querymenu(qmenu)) {
            value--;
            continue;
        }
        if (value-- == 0)
            break;
        j++;
    }
    return j;
}

void Videostreaming::setStillVideoSize(QString stillValue, QString stillFormat) {
    stillSize = stillValue;
    stillOutFormat = stillFormat;
}


void Videostreaming::enumerateAudioProperties(){    
    audioinput.audio_init(); 
}

void Videostreaming::setChannelCount(uint index){
    audioinput.setChannelCount(index);
}

void Videostreaming::setSampleRate(uint index){
    audioinput.setSampleRate(index);
}

void Videostreaming::recordVideo(){
    videoEncoder->encodeImage(m_renderer->yuvBuffer, false /* other than rgba format means, false */);
}

void Videostreaming::recordBegin(int videoEncoderType, QString videoFormatType, QString fileLocation, int audioDeviceIndex, int channels) {
    m_VideoRecord = true;
    if(videoFormatType.isEmpty()) {
        videoFormatType = "avi";        //Application never enters in this condition
    }
#if !LIBAVCODEC_VER_AT_LEAST(54, 25)
    if(ubuntuVersion == ">=15"){
        switch(videoEncoderType) {
        case 0:
            videoEncoderType = CODEC_ID_MJPEG;
            break;
        case 1:
            videoEncoderType = CODEC_ID_H264;
            break;
        case 2:
            videoEncoderType = CODEC_ID_VP8;
            break;
        }
    } else if(ubuntuVersion == "<15"){
        switch(videoEncoderType) {
        case 0:
            videoEncoderType = CODEC_ID_RAWVIDEO;
            break;
        case 1:
            videoEncoderType = CODEC_ID_MJPEG;
            break;
        case 2:
            videoEncoderType = CODEC_ID_H264;
            break;
        case 3:
            videoEncoderType = CODEC_ID_VP8;
            break;
        }

    }
#else
    if(ubuntuVersion == ">=15"){
        switch(videoEncoderType) {       
        case 0:
            videoEncoderType = AV_CODEC_ID_MJPEG;
            break;
        case 1:
            videoEncoderType = AV_CODEC_ID_H264;
            break;
        case 2:
            videoEncoderType = AV_CODEC_ID_VP8;
            break;
        }
    } else if(ubuntuVersion == "<15"){
        switch(videoEncoderType) {
        case 0:
            videoEncoderType = AV_CODEC_ID_RAWVIDEO;
            break;
        case 1:
            videoEncoderType = AV_CODEC_ID_MJPEG;
            break;
        case 2:
            videoEncoderType = AV_CODEC_ID_H264;
            break;
        case 3:
            videoEncoderType = AV_CODEC_ID_VP8;
            break;
        }

    }
#endif    

    fileName = fileLocation +"/Qtcam-" + QDateTime::currentDateTime().toString("yy_MM_dd:hh_mm_ss")+"."+ videoFormatType;

    // Fixed issue: Incorrect frame rate for video recording
    v4l2_fract temp_interval;

    if (m_has_interval) {
        temp_interval = m_interval;        
    }
    else {
        v4l2_frmivalenum frmival;
        enum_frameintervals(frmival, m_pixelformat, m_width, m_height);
        temp_interval = frmival.discrete;        
    }
    audiorecordStart = false;

    if((audioDeviceIndex-1) >= 0){
        audioinput.audio_set_pulseaudio_device(audioDeviceIndex-1);
        audioinput.audio_start_pulseaudio(videoEncoderObj);
        audio_buffer_data = audioinput.audio_get_buffer();
        audiorecordStart = true;
    }

#if LIBAVCODEC_VER_AT_LEAST(54,25)
    bool tempRet = videoEncoder->createFile(fileName,(AVCodecID)videoEncoderType, m_capDestFormat.fmt.pix.width,m_capDestFormat.fmt.pix.height,temp_interval.denominator,temp_interval.numerator,10000000, audioDeviceIndex, audioinput.audio_context->samprate, audioinput.audio_context->channels);
#else
    bool tempRet = videoEncoder->createFile(fileName,(CodecID)videoEncoderType, m_capDestFormat.fmt.pix.width,m_capDestFormat.fmt.pix.height,temp_interval.denominator,temp_interval.numerator,10000000, audioDeviceIndex, audioinput.audio_context->samprate, audioinput.audio_context->channels);
#endif
    if(!tempRet){
        emit rcdStop("Unable to record the video");
    }
}

void Videostreaming::recordStop() {    
    emit videoRecord(fileName);
    m_VideoRecord = false;    

    if(audiorecordStart){
        if(audioinput.audio_context->stream_flag == AUDIO_STRM_ON)
            audioinput.audio_stop_pulseaudio();

        audioinput.audio_delete_buffer(audio_buffer_data);
    }

    audiorecordStart = false;
    // Jpeg decode and record is in separate thread. If we close recorded file here, It might lead to crash since it will be
    // trying to encode image in background
    if(videoEncoder!=NULL && m_capSrcFormat.fmt.pix.pixelformat!= V4L2_PIX_FMT_MJPEG){
        videoEncoder->closeFile();
    }
}

void Videostreaming::doEncodeAudio(){
    if(m_VideoRecord){        
        if(audio_buffer_data != NULL){            
            int ret = audioinput.audio_get_next_buffer(audio_buffer_data);
            if(ret > 0){
                //qDebug()<<"ret > 0"<<ret;

            }
            if(ret== 0){                
                videoEncoder->encodeAudio(audio_buffer_data->data);
            }
        }
    }
}


void Videostreaming::stopUpdatePreview() {
    m_renderer->updateStop = true;
}

void Videostreaming::triggerModeEnabled() {
   stopUpdatePreview();
}

void Videostreaming::masterModeEnabled() {
    m_renderer->updateStop = false;
}
//Added by Dhurka - 13th Oct 2016
/**
 * The below slot is used to get teh currently selected camera enum value for comparision
 */
void Videostreaming::selectedCameraEnum(CommonEnums::ECameraNames selectedDeviceEnum)
{
    currentlySelectedCameraEnum = selectedDeviceEnum;
}

/**
 * @brief Videostreaming::disableImageCaptureDialog - Disable image saving pop up for trigger shots
 */
void Videostreaming::disableImageCaptureDialog(){
    m_displayCaptureDialog = false;
}

/**
 * @brief Videostreaming::disableSavingImage - disable writing image file
 */
void Videostreaming::disableSavingImage(){
    m_saveImage = false;
}

/**
 * @brief Videostreaming::setUvcExtControlValue - pass the xquery structure and set values
 * @param xquery
 * @return true/false
 */
bool Videostreaming::setUvcExtControlValue(struct uvc_xu_control_query xquery){

    int ret = extQueryCtrl(xquery);
    if(ret){
        return true;
    }
    return false;
}
