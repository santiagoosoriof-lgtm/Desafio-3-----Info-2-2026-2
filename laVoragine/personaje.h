#ifndef PERSONAJE_H
#define PERSONAJE_H

#include "EntidadJuego.h"

class Personaje : public EntidadJuego {
protected:
    int salud;
    int saludMaxima;
    float velocidadBase;
    float velX;
    float velY;



public:
    Personaje(float x, float y, float w, float h, int saludInicial, float vel)
        : EntidadJuego(x, y, w, h), salud(saludInicial), saludMaxima(saludInicial),
        velocidadBase(vel), velX(0.0f), velY(0.0f) {}

    virtual ~Personaje() = default;

    virtual void mover(float dx, float dy) {
        posX += dx;
        posY += dy;
    }

    virtual void recibirDano(int cantidad) {
        salud -= cantidad;
        if (salud <= 0) {
            salud = 0;
            desactivar();
        }
    }

    virtual void curar(int cantidad) {
        if (!activa || cantidad <= 0) return;
        salud += cantidad;
        if (salud > saludMaxima) {
            salud = saludMaxima;
        }
    }

    int getSalud() const { return salud; }
    void setVelocidad(float vx, float vy) { velX = vx; velY = vy; }
    float getVelX() const { return velX; }
    float getVelY() const { return velY; }
};

#endif // PERSONAJE_