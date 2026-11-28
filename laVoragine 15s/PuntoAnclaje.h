#ifndef PUNTOANCLAJE_H
#define PUNTOANCLAJE_H

// puntos para colgarse
struct PuntoAnclaje {
    float x;
    float y;
    float radioAlcance;

    PuntoAnclaje(float posX = 0.0f, float posY = 0.0f, float alcance = 180.0f)
        : x(posX), y(posY), radioAlcance(alcance) {}
};

#endif // PUNTOANCLAJE_H
