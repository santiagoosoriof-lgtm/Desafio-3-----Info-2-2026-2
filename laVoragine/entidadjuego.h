#ifndef ENTIDADJUEGO_H
#define ENTIDADJUEGO_H

class EntidadJuego {
protected:
    float posX;
    float posY;
    float ancho;
    float alto;
    bool activa;

public:
    EntidadJuego(float x, float y, float w, float h)
        : posX(x), posY(y), ancho(w), alto(h), activa(true) {}

    virtual ~EntidadJuego() = default;

    // metodo con polimorfismo para actualizar fisicas en cada frame (dt en segundos)
    virtual void actualizarFisica(float dt) = 0;

    // deteccion de colisiones
    bool verificarColision(const EntidadJuego* otra) const {
        if (!otra || !activa || !otra->activa) return false;
        return (posX < otra->posX + otra->ancho &&
                posX + ancho > otra->posX &&
                posY < otra->posY + otra->alto &&
                posY + alto > otra->posY);
    }

    // Getters y Setters
    float getX() const { return posX; }
    float getY() const { return posY; }
    float getAncho() const { return ancho; }
    float getAlto() const { return alto; }
    bool estaActiva() const { return activa; }
    void desactivar() { activa = false; }
    void setPosicion(float x, float y) { posX = x; posY = y; }
};

#endif // ENTIDADJUEGO_H
