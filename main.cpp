#include <SFML/Graphics.hpp>
#include<string>  
#include "tunicate.h"

int main()
{
    /* Seed RNG */
    srand(time(0));

    /* Global variables */
    int N = 100000;  /* Population size N */
    int tickspergeneration = 1000;
    int childrenperparent = 2;

    int ticks = 0;
    int generation = 0;
    int individuals_alive = N;
   
    /* Load font */
    sf::Font font;
    if (!font.loadFromFile("./fonts/LEMONMILK-Regular.otf")){/* Font error */}

    /* Create render window for program */
    sf::RenderWindow window(sf::VideoMode(900, 900), "Neural Evolution");

    /* Create divider of header and simulation space */
    sf::RectangleShape divider(sf::Vector2f(120.f, 50.f));
    divider.setSize(sf::Vector2f(900.f, 1.f));
    divider.setPosition(sf::Vector2f(0.f, 95.f));

    /* Death zone indicator */
    sf::RectangleShape deathzone(sf::Vector2f(120.f, 50.f));
    deathzone.setSize(sf::Vector2f(200.f, 200.f));
    deathzone.setPosition(sf::Vector2f(0.f, 95.f));
    deathzone.setFillColor(sf::Color(0, 255, 0, 255/3));
    
    /* Initialize population */
    std::tunicate *population = (std::tunicate*) malloc(sizeof(std::tunicate) * N);

    for (size_t i = 0; i < N; i++)
    {
        population[i] = std::tunicate();
        population[i].setID(i);
    }
    
    while (window.isOpen())
    {
        /* Prepare exit event */
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        /* Make header for program */
        sf::Text header;
        header.setFont(font);
        header.setString("Population size: " + std::to_string(individuals_alive) + "\nGeneration: " + std::to_string(generation) + "\nTick: " + std::to_string(ticks));
        header.setCharacterSize(20);
        header.setFillColor(sf::Color::White);
        header.setPosition(sf::Vector2f(15.f, 10.f));

        /* Clear screen */
        window.clear();

        /* Update logic */
        for (size_t i = 0; i < N; i++)
        {
            if(population[i].getAlive() == 1){
                
                /* Sense environment and take action for each living agent */
                population[i].update();
            }
        }

        /* Draw UI elements */
        window.draw(deathzone);
        window.draw(divider);
        window.draw(header);

        if(individuals_alive == 0)
        {
            //

        } else {
            
            
            /* Draw population */
            for (size_t i = 0; i < N; i++)
            {
                if(population[i].getAlive() == 1){
                    window.draw(population[i].getDrawable());
                }
            }

            /* Show window */
            window.display();

            /* End of a generation */
            std::tunicate *newpopulation = (std::tunicate*) malloc(sizeof(std::tunicate) * N);
            if(ticks == tickspergeneration) {
                /* Increase generation counter and reset tick counter */
                ticks = 0; 
                generation++;

                /* Cull all of the individuals on the right half of the screen */
                int stillalive = 0;
                for (size_t i = 0; i < N; i++)
                {
                    if(population[i].getAlive() == 1){
                        if((population[i].position.x > 400/2 || population[i].position.y > 500/2) || population[i].displacement() < 100.0){
                            population[i].setAlive(0);
                        } else {
                            population[i].mutate();
                            stillalive++;
                        }
                    }

                }
                std::cout << "Generation " << generation << " has " << (float)stillalive/individuals_alive << " survival rate" << std::endl;

                int k = 0;

                /* Mutate and mate the survivors */
                /* For each individual, find an alive partner and make children for the new population */
                for (size_t i = 0; i < N; i++)
                {
                    if(k == N) break;

                    int birthrate = 0;
                    for (size_t j = 1; j < N-1; j++) 
                    {
                        if(birthrate == childrenperparent)
                        {
                            break;

                        } else {

                            if(population[i].getAlive() == 1 && population[j].getAlive() == 1)
                            {
                                /* Mate alive parents */
                                birthrate++;
                                k++;
                                newpopulation[k] = population[i].mate(population[j]);
                            }
                        }
                    }
                }

                

                /* Recount population */
                individuals_alive = k;
                for (size_t i = 0; i < N; i++)
                {
                    if(i < k)
                        population[i] = newpopulation[i];
                    else
                        population[i].setAlive(0);
                }
            }

            /* Increase tick counter */
            ticks++;
        }
    }

    /* Destroy pointers */
    population = nullptr;

    return 0;
}