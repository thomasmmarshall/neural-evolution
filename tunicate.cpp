#include "tunicate.h" 

using namespace std;

/* Default constructor */
tunicate::tunicate()
{
    /* Set ID */
    this->ID = 0;
    this->alive = 1;
    this->p_mutate = 0.01;
    this->lastaction = 8;

    /* Initialize weights matrices */ 
    /* Layer 1 */
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 3; j++){
            this->weights0[i][j] = ((float) (rand()/(RAND_MAX/2.0) - 1.0) * sqrt(2.0/3.0));
            //cout << (float) (rand()/(RAND_MAX/2.0) - 1.0)  << endl;
        }
    }
    
    /* Layer 2 */
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 2; j++){
            this->weights1[i][j] = ((float) (rand()/(RAND_MAX/2.0) - 1.0) * sqrt(2.0/2.0));
        }
    }

    /* Layer 3 */
    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 9; j++){
            this->weights2[i][j] = ((float) (rand()/(RAND_MAX/2.0) - 1.0) * sqrt(2.0/9.0));
        }
    }

    /* Initialize to random position on the map */
    this->position = sf::Vector2f(rand() % 900 + 1, rand() % 800 + 100 + 1);

    /* Create FSML representation */
    this->visual.setRadius(1.f);
    this->visual.setPosition(this->position);
    this->initialpos = this->position;
    this->visual.setFillColor(sf::Color(0, 255, 255, rand() % 100 + 155));

}

void tunicate::greet()
{
    /* Outputs a small message indicating this individual's ID number and other parameters */
    cout << "Hello! I am tunicate number " << this->ID << endl;
    cout << "My position is: (" << this->position.x << "," << this->position.y << ")" <<  endl;
}

void tunicate::update()
{
    /* Step 1: Obtain observations from the environment as input */
    /* Currently just taking the X and Y position of the agent as the inputs */
    this->layer0[0][0] = this->position.x;
    this->layer0[0][1] = this->position.y;
    this->layer0[0][2] = this->lastaction;
    this->layer0[0][3] = 0.0;//(float) (rand()/(RAND_MAX/2.0) - 1.0)/100.0;
    this->layer0[0][4] = 0.0;//(float) (rand()/(RAND_MAX/2.0) - 1.0)/100.0;
    this->layer0[0][5] = 0.0;//(float) (rand()/(RAND_MAX/2.0) - 1.0)/100.0;
    this->layer0[0][6] = 0.0;//(float) (rand()/(RAND_MAX/2.0) - 1.0)/100.0;
    this->layer0[0][7] = 0.0;//(float) (rand()/(RAND_MAX/2.0) - 1.0)/100.0;

    /* Step 2: Forward pass the neural network brain */
    this->forwardpass();

    /* Step 3: Determine the output action */
    int action = 0;
    float max = this->output[0][0];
    for (size_t i = 0; i < 9; i++) if(max <= this->output[0][i]) action = i;
    this->lastaction = action;
    
    /* Step 5: Take action on the environment */
    switch (action)
    {
        case 0:
            /* Move UP */
            //cout << "Move UP" << endl;
            this->position.y -= 1;
            this->movements++;
            break;

        case 1:
            /* Move LEFT */
            //cout << "Move LEFT" << endl;
            this->position.x -= 1;
            this->movements++;
            break;

        case 2:
            /* Move DOWN */
            //cout << "Move DOWN" << endl;
            this->position.y += 1;
            this->movements++;
            break;

        case 3:
            /* Move RIGHT */
            //cout << "Move RIGHT" << endl;
            this->position.x += 1;
            this->movements++;
            break;

        case 4:
            /* Move UP LEFT */
            this->position.x -= 1;
            this->position.y -= 1;
            this->movements++;
            break;

        case 5:
            /* Move UP RIGHT */
            this->position.x += 1;
            this->position.y -= 1;            
            this->movements++;
            break;
        
        case 6:
            /* Move DOWN LEFT */
            this->position.x -= 1;
            this->position.y += 1;            
            this->movements++;
            break;
        
        case 7:
            /* Move DOWN RIGHT */
            this->position.x += 1;
            this->position.y += 1;
            this->movements++;
            break;

        case 8:
            /* Don't move! */
            //cout << "Move NONE" << endl;
            break;
        
        default:
            // dont move
            break;
    }

    /* Step 5: Colision correction */
    this->checkBoundary();

}

void tunicate::forwardpass()
{
    /* Computes forward pass of the neural network brain with ReLU */
    /* Layer 1 - 1x8 X 8x3*/
    int R1 = 1;
    int R2 = 8;
    int C2 = 3;

    for (int i = 0; i < R1; i++) {
        for (int j = 0; j < C2; j++) {
            this->layer1[i][j] = 0.0;

            for (int k = 0; k < R2; k++) {
                this->layer1[i][j] += this->layer0[i][k] * this->weights0[k][j];
                
                /* ReLU */
                if(this->layer1[i][j] < 0.0) this->layer1[i][j] = 0.0;
            }
        }
    }
    
    /* Layer 2 - 1x3 X 3x2 */
    R1 = 1;
    R2 = 3;
    C2 = 2;

    for (int i = 0; i < R1; i++) {
        for (int j = 0; j < C2; j++) {
            this->layer2[i][j] = 0.0;

            for (int k = 0; k < R2; k++) {
                this->layer2[i][j] += this->layer1[i][k] * this->weights1[k][j];

                /* ReLU */
                if(this->layer2[i][j] < 0.0) this->layer2[i][j] = 0.0;
            }
        }
    }

    /* Layer 3 1x2 X 2x5 */
    R1 = 1;
    R2 = 2;
    C2 = 9;

    for (int i = 0; i < R1; i++) {
        for (int j = 0; j < C2; j++) {
            this->output[i][j] = 0.0;

            for (int k = 0; k < R2; k++) {
                this->output[i][j] += this->layer2[i][k] * this->weights2[k][j];

                /* ReLU */
                if(this->output[i][j] < 0.0) this->output[i][j] = 0.0;
            }
        }
    }

}

sf::CircleShape tunicate::getDrawable()
{
    /* Update and return the drawable object for FSML*/

    return this->visual;
}

void tunicate::setID(int ID)
{
    this->ID = ID;
}

int tunicate::getID()
{
    return this->ID;
}

void tunicate::checkBoundary()
{
    /* Check the bounary conditions of the position and correct if necessary, this is applied after the update */
    this->position.x > 890 ? this->position.x = 890: 0;
    this->position.x < 0 ? this->position.x = 0: 0;

    this->position.y > 890 ? this->position.y = 890: 0;
    this->position.y < 100 ? this->position.y = 100: 0;

    this->visual.setPosition(this->position);
    this->lastaction = 8;
}

void tunicate::setAlive(int alive)
{
    this->alive = alive;
}

int tunicate::getAlive()
{
    return this->alive;
}

void tunicate::mutate()
{
    /* Chance of random mutation to occur in the brain of an individual */

    /* Layer 1 */
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 3; j++){
            if(((float) rand()/(RAND_MAX)) < this->p_mutate)
                this->weights0[i][j] += ((float) rand()/(RAND_MAX/2.0)/1.0) - 1.0f;   
        }
    }
    
    /* Layer 2 */
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 2; j++){
            if(((float) rand()/(RAND_MAX)) < this->p_mutate)
                this->weights1[i][j] += ((float) rand()/(RAND_MAX/2.0)/1.0) - 1.0f;   
        }
    }

    /* Layer 3 */
    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 9; j++){
            if(((float) rand()/(RAND_MAX)) < this->p_mutate)
                this->weights2[i][j] += ((float) rand()/(RAND_MAX/2.0)/1.0) - 1.0f;        
        }
    }
}

tunicate tunicate::mate(tunicate partner)
{
    /* Combines the genes of two tunicates and returns a single child */
    /* Neural crossover: every weight in the brain has an equal chance of coming from either parent */

    tunicate child = tunicate();
    /*
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 3; j++){
            if(((float) rand()/(RAND_MAX)) > 0.5)
                child.weights0[i][j] = this->weights0[i][j];
            else
                child.weights0[i][j] = partner.weights0[i][j];
        }
    }
    
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 2; j++){
            if(((float) rand()/(RAND_MAX)) > 0.5)
                child.weights1[i][j] = this->weights1[i][j];
            else
                child.weights1[i][j] = partner.weights1[i][j];
        }
    }

    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 5; j++){
            if(((float) rand()/(RAND_MAX)) > 0.5)
                child.weights2[i][j] = this->weights2[i][j];
            else
                child.weights2[i][j] = partner.weights2[i][j];      
        }
    }*/
    

    /* Layer 1 */
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 3; j++){
            child.weights0[i][j] = (this->weights0[i][j] + partner.weights0[i][j])/2.0;
        }
    }
    
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 2; j++){
            child.weights1[i][j] = (this->weights1[i][j] + partner.weights1[i][j])/2.0;

        }
    }

    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 9; j++){
            child.weights2[i][j] = (this->weights2[i][j] + partner.weights2[i][j])/2.0;
     
        }
    }

    return child;
}

float tunicate::displacement()
{
    float x_1 = (float) this->initialpos.x;
    float y_1 = (float) this->initialpos.y;
    float x_2 = (float) this->position.x;
    float y_2 = (float) this->position.y;

    return sqrt(pow(x_1 - x_2, 2.0) + pow(y_1 - y_2, 2.0));

}