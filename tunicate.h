#ifndef TUNICATE_H
#define TUNICATE_H

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <cmath>

namespace std

{
    class tunicate
    {

    public:
        /* Public functions */
        tunicate();
        void greet();
        void update();
        void setID(int);
        int getID();
        void setAlive(int);
        int getAlive();
        sf::CircleShape getDrawable();
        void matmul(int R1, int R2, int C2, float A[][100], float B[][100], float C[][100]);
        void checkBoundary();
        void forwardpass();
        void mutate();
        tunicate mate(tunicate);
        float displacement();

        /* Public variables */
        int ID;
        int alive;
        float p_mutate;
        int movements;
        int lastaction;
        sf::Vector2f position;
        sf::Vector2f initialpos;
        sf::CircleShape visual;

        /* Define neural network structure */
        /* Neuron layers */
        float layer0[1][8];
        float layer1[1][3];
        float layer2[1][2];
        float output[1][9];

        /* Weights connecting the layers*/
        float weights0[8][3];
        float weights1[3][2];
        float weights2[2][9];

    };
}

#endif