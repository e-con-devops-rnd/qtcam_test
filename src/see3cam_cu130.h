#ifndef SEE3CAM_CU130_H
#define SEE3CAM_CU130_H

#include <QObject>
#include "uvccamera.h"

class See3CAM_CU130 : public QObject
{
    Q_OBJECT

private:
    unsigned char g_out_packet_buf[BUFFER_LENGTH];
    unsigned char g_in_packet_buf[BUFFER_LENGTH];
    uvccamera uvc;

public:
    explicit See3CAM_CU130(QObject *parent = 0);

    enum sceneModes {
        SCENE_NORMAL = 0x01,
        SCENE_PORTRAIT,
        SCENE_LANDSCAPE,
        SCENE_SPORT,
        SCENE_CLOSE_UP,
        SCENE_NIGHT,
        SCENE_TWILIGHT,
        SCENE_BACKLIGHT,
        SCENE_HIGH_SENSITIVE,
        SCENE_NIGHT_PORTRAIT,
        SCENE_BEACH,
        SCENE_DOCUMENT,
        SCENE_PARTY,
        SCENE_FIREWORKS,
        SCENE_SUNSET,
        SCENE_AUTO_DETECT,
    };
    Q_ENUMS(sceneModes)

    enum specialEffects {
        EFFECT_NORMAL = 0x01,
        EFFECT_ALIEN,
        EFFECT_ANTIQUE,
        EFFECT_BLACK_WHITE,
        EFFECT_EMBOSS,
        EFFECT_EMBOSS_COLORED,
        EFFECT_GRAY,
        EFFECT_NEGATIVE,
        EFFECT_BLUE,
        EFFECT_GREEN,
        EFFECT_RED,
        EFFECT_POSTERIZE_1,
        EFFECT_POSTERIZE_2,
        EFFECT_SEPIA_1,
        EFFECT_SEPIA_2,
        EFFECT_SKETCH,
        EFFECT_SOLARIZE,
        EFFECT_FOGGY,
    };

     Q_ENUMS(specialEffects)

signals:
     void sendSceneMode(uint sceneMode);
     void sendEffectMode(uint effectMode);
     void sendDenoiseValue(uint denoiseValue);

public slots:
    void getSceneMode();
    void getEffectMode();
    void getDenoiseValue();
    void setSceneMode(const sceneModes& sceneMode);
    void setEffectMode(const specialEffects& specialEffect);
    void setDenoiseValue(const uint& noiseValue);

};

#endif // SEE3CAM_CU130_H
