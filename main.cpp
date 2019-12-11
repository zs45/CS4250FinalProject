//
//  main.cpp
//  SideHero
//
//  Created by Zach Schrock on 11/18/19.
//  Copyright © 2019 Zach Schrock. All rights reserved.
//

#include <iostream>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <thread>
#include <chrono>
#include "unistd.h"
#include <cmath>

#define FPS 20

// Enumerations for different states
enum WEAPON {FIRE, ICE, ELECTRIC};
enum JUMPSTATE {WALKING, JUMPING, FALLING};
enum GAMESTATE {PLAYING, GAMEOVER, WIN};
// Position of player
float playerX = -0.8;
float playerY = -0.8;
// Enemy One variables
float enemyX = 1;
float enemyY = -0.8;
float enemyMoveX = 0.0025;
float enemyMoveY = 0.005;
bool enemyDirX = false;
bool enemyDirY = false;
bool enemyAlive = true;
float enemyAttackX = enemyX;
float enemyAttackY = enemyY;
float enemyAttackIncr = -0.05;
int enemyAttackTimer = 0;
bool enemyAttacking = false;
bool enemyContinuedAttack = false;
// Enemy Two variables
float enemyTwoX = 1;
float enemyTwoY = -0.85;
float enemyTwoMoveX = 0.0025;
float enemyTwoMoveY = 0.0075;
bool enemyTwoDirX = false;
bool enemyTwoDirY = false;
bool enemyTwoAlive = true;
int enemyTwoAttackTimer = 0;
bool enemyTwoAttacking = false;
bool enemyTwoContinuedAttack = false;
// Enemy Three variables
float enemyThreeX = 1.5;
float enemyThreeY = -0.8;
float enemyThreeMoveX = 0.0025;
float enemyThreeMoveY = 0.00875;
bool enemyThreeDirX = false;
bool enemyThreeDirY = false;
bool enemyThreeAlive = true;
float enemyThreeAttackX = enemyThreeX;
float enemyThreeAttackY = enemyThreeY;
float enemyThreeAttackIncr = -0.05;
int enemyThreeAttackTimer = 0;
bool enemyThreeAttacking = false;
bool enemyThreeContinuedAttack = false;
// Enemy Boss variables
float enemyBossX = 3;
float enemyBossY = -0.85;
float enemyBossMoveX = 0.0025;
float enemyBossMoveY = 0.005;
bool enemyBossDirX = false;
bool enemyBossDirY = false;
bool enemyBossAlive = true;
float enemyBossAttackX = enemyX;
float enemyBossAttackY = enemyY;
float enemyBossAttackIncr = -0.05;
int enemyBossAttackTimer = 0;
bool enemyBossAttacking = false;
bool enemyBossContinuedAttack = false;
// Attack positions and tracker
float attackX = playerX;
float attackY = playerY;
bool continuedAttack = false;
// Color of players
float colorR = 0.95;
float colorG = 0.2;
float colorB = 0.1;
// Initial states of the player om startup
WEAPON currentWeap = FIRE;
JUMPSTATE currentState = WALKING;
GAMESTATE currentPlay = PLAYING;
// Highest the player can jump
float jumpLimit = -0.5;
// Hit point variables
void hitPoints(float space);
void bossHitPoints(float space);
int points = 3;
bool hit = false;
bool hit2 = false;
bool hit3 = false;
bool hitBoss = false;
bool hitBoss2 = false;
int bossPoints = 5;
// Character creation function
void character();
// World creation functions
void floor();
void clouds(float dist, float height);
void renderClouds(int num, float cloudsDist, float cloudsHeight);
// Attack variables
bool attack = false;
int attackTimer = 0;
float elecIncr = 0.05;
// Attack functions
void fireAttack();
void iceAttack();
void electricAttack(float incr);
// Enemy One functions
void enemy();
void enemyMovement();
void enemyAttack(float incr);
// Enemy Two functions
void enemyTwo();
void enemyTwoMovement();
void enemyTwoAttack();
// Enemy Three functions
void enemyThree();
void enemyThreeMovement();
void enemyThreeAttack(float incr);
// Enemy Boss functions
void enemyBoss();
void enemyBossMovement();
void enemyBossAttack(float incr);
void enemyBossAttackTwo(float incr);
// End game functions
void gameOver();
int gameOverTimer = 0;
void win();
int winTimer = 0;
// Timer function, sets our frames per second
void timer(int){
    glutPostRedisplay();
    glutTimerFunc(1000/FPS, timer, 0);
}
// Initialization
void init()
{
    glClearColor(0.529, 0.808, 0.922, 0.0);
    glClearAccum(0.0, 0.0, 0.0, 0.0);
    gluOrtho2D(0, 640, 0, 480);
}
// Display callback function
void display(){
    
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glTranslatef(-playerX-0.75, 0, 0);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // If all hit points are gone, Game over
    if(points<1){
        currentPlay=GAMEOVER;
    }
    // If boss is defeated, currentPlay is equal to WIN
    if(!enemyBossAlive){
        currentPlay=WIN;
    }
    // Displays the game over function if currentPlay is GAMEOVER
    if(currentPlay==GAMEOVER){
        glPushMatrix();
        gameOver();
        glPopMatrix();
        
        gameOverTimer+=1;
        
        if(gameOverTimer > 180){
            exit(0);
        }
    }
    // Displays the win function if currentPlay is WIN
    if(currentPlay==WIN){
        glPushMatrix();
        win();
        glPopMatrix();
        
        winTimer+=1;
        
        if(winTimer > 180){
            exit(0);
        }
    }
    // If-else for when the player is jumping
    if (currentState==JUMPING){
        if(playerY < jumpLimit){
            playerY+=0.01;
        }else{
            currentState=FALLING;
        }
    }else if (currentState ==FALLING){
        if(playerY > -0.8){
            playerY-=0.01;
        }else{
            currentState=WALKING;
        }
    }
    // So the player cannot regenerate health
    if(points > 3){
        points = 3;
    }
    // Between the push and pop, this will display the hit points the player has left
    glPushMatrix();
    float pointsSpace = 0;
    character();
    for(int i = 0; i < points; ++i){
        hitPoints(pointsSpace);
        pointsSpace += 0.0375;
    }
    glPopMatrix();
    // Runs the AI for and displays enemy one if alive
    if(enemyAlive){
        enemyMovement();
        glPushMatrix();
        enemy();
        glPopMatrix();
        enemyAttackTimer +=1;
        if(enemyAttackTimer > 50){
            enemyAttacking = true;
        }
        if(enemyAttacking){
            if(!enemyContinuedAttack){
                enemyAttackX = enemyX;
                enemyAttackY = enemyY;
                enemyContinuedAttack = true;
            }
            glPushMatrix();
            enemyAttack(enemyAttackIncr);
            glPopMatrix();
            enemyAttackIncr -= 0.015;
            if(enemyAttackIncr < -1.5){
                enemyAttackIncr = 0.05;
                enemyAttackTimer = 0;
                enemyAttacking = false;
            }
        }
        if(!enemyAttacking){
            enemyAttackIncr = 0.05;
            enemyContinuedAttack = false;
        }
    }
    // Runs the AI for and displays enemy two if alive
    if(enemyTwoAlive){
        enemyTwoMovement();
        glPushMatrix();
        enemyTwo();
        glPopMatrix();
        enemyTwoAttackTimer +=1;
        if(enemyTwoAttackTimer > 50){
            enemyTwoAttacking = true;
        }
        if(enemyTwoAttacking){
            glPushMatrix();
            enemyTwoAttack();
            glPopMatrix();
        }
        if(enemyTwoAttackTimer > 100){
            enemyTwoAttacking = false;
            enemyTwoAttackTimer = 0;
        }
    }
    // Runs the AI for and displays enemy three if alive
    if(enemyThreeAlive){
        enemyThreeMovement();
        glPushMatrix();
        enemyThree();
        glPopMatrix();
        enemyThreeAttackTimer +=1;
        if(enemyThreeAttackTimer > 50){
            enemyThreeAttacking = true;
        }
        if(enemyThreeAttacking){
            if(!enemyThreeContinuedAttack){
                enemyThreeAttackX = enemyThreeX;
                enemyThreeAttackY = enemyThreeY;
                enemyThreeContinuedAttack = true;
            }
            glPushMatrix();
            enemyThreeAttack(enemyThreeAttackIncr);
            glPopMatrix();
            enemyThreeAttackIncr -= 0.015;
            if(enemyThreeAttackIncr < -1.5){
                enemyThreeAttackIncr = 0.05;
                enemyThreeAttackTimer = 0;
                enemyThreeAttacking = false;
            }
        }
        if(!enemyThreeAttacking){
            enemyThreeAttackIncr = 0.05;
            enemyThreeContinuedAttack = false;
        }
    }
    float bossPointsSpace = 0;
    // Runs the AI for and displays the boss if alive
    if(enemyBossAlive){
        enemyBossMovement();
        glPushMatrix();
        enemyBoss();
        for(int i = 0; i < bossPoints; ++i){
            bossHitPoints(bossPointsSpace);
            bossPointsSpace += 0.0375;
        }
        glPopMatrix();
        enemyBossAttackTimer +=1;
        if(enemyBossAttackTimer > 50){
            enemyBossAttacking = true;
        }
        if(enemyBossAttacking){
            if(!enemyBossContinuedAttack){
                enemyBossAttackX = enemyBossX;
                enemyBossAttackY = enemyBossY;
                enemyBossContinuedAttack = true;
            }
            glPushMatrix();
            enemyBossAttack(enemyBossAttackIncr);
            glPopMatrix();
            glPushMatrix();
            enemyBossAttackTwo(enemyBossAttackIncr);
            glPopMatrix();
            enemyBossAttackIncr -= 0.015;
            if(enemyBossAttackIncr < -1.5){
                enemyBossAttackIncr = 0.05;
                enemyBossAttackTimer = 0;
                enemyBossAttacking = false;
            }
        }
        if(!enemyBossAttacking){
            enemyBossAttackIncr = 0.05;
            enemyBossContinuedAttack = false;
        }
    }
    // Renders the floor
    floor();
    // Sets up values for the clouds
    float cloudsDist = -1;
    float cloudsHeight = 0.05;
    // Renders the clouds
    renderClouds(20, cloudsDist, cloudsHeight);
    // Runs the various conditionals and animations if the player is attacking
    if(attack){
        if(!continuedAttack){
            attackX = playerX;
            attackY = playerY;
            continuedAttack = true;
        }
        switch(currentWeap){
            case FIRE:
                fireAttack();
                attackTimer+=1;
                if(attackTimer > 30){
                    attackTimer = 0;
                    attack = false;
                }
                break;
            case ICE:
                iceAttack();
                attackTimer+=1;
                if(attackTimer > 50){
                    attackTimer = 0;
                    attack = false;
                }
                break;
            case ELECTRIC:
                electricAttack(elecIncr);
                elecIncr += 0.015;
                if(elecIncr > 1.5){
                    elecIncr = 0.05;
                    attack = false;
                }
                break;
        }
    }
    // Resets initial values
    if(!attack){
        elecIncr = 0.05;
        attackTimer = 0;
        continuedAttack = false;
    }
    
    
    glutSwapBuffers();
    
}
// Keyboard function (switch weapons, jump, attack)
void keyboard(unsigned char key, int x, int y){
    if(currentPlay==PLAYING){
        switch(key){
            case 'r':
                colorR = 0.95;
                colorG = 0.2;
                colorB = 0.1;
                currentWeap = FIRE;
                attack = false;
                break;
            case 'b':
                colorR = 0.55;
                colorG = 0.75;
                colorB = 1;
                currentWeap = ICE;
                attack = false;
                break;
            case 'y':
                colorR = 0.8;
                colorG = 0.8;
                colorB = 0.1;
                currentWeap = ELECTRIC;
                attack = false;
                break;
                
            case 'z':
                attack = !attack;
                break;
                
            /*case 'h':
                points-=1;
                break;
                
            case 'H':
                enemyAlive = true;
                break;*/
                
            /*case 'k':
                enemyAlive=false;
                break;
                */
                
            case ' ':
                if(currentState==WALKING){
                    currentState = JUMPING;
                }
                break;
                
        }
    }
    
    glutPostRedisplay();
    
}
// Left arrow key to move player left, right arrow key to move player right
void special(int key, int x, int y){
    if(currentPlay==PLAYING){
        switch(key){
            case GLUT_KEY_LEFT:
                playerX-= 0.03;
                break;
            case GLUT_KEY_RIGHT:
                playerX+= 0.03;
                break;
        }
    }
    glutPostRedisplay();
}
// Main OpenGL/GLUT/C++ function
int main(int argc, char * argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow("Side Hero");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutTimerFunc(0, timer, 0);
      
    glutMainLoop();
    return(EXIT_SUCCESS);
}

// Function that creates and translates character
void character(){
    glTranslatef(playerX, playerY, 0);
    glBegin(GL_QUADS);
    glColor3f(colorR, colorG, colorB);
    
    glVertex2f(0, 0.05);
    glVertex2f(0.1, 0.05);
    glVertex2f(0.1, -0.05);
    glVertex2f(0, -0.05);
    
    glVertex2f(0.025, 0.05);
    glVertex2f(0.075, 0.05);
    glVertex2f(0.075, 0.11);
    glVertex2f(0.025, 0.11);
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(0.1, 0.1, 0.1);
    
    glVertex2f(0, -0.05);
    glVertex2f(0, 0.05);

    glVertex2f(0, -0.05);
    glVertex2f(0.1, -0.05);

    glVertex2f(0.1, -0.05);
    glVertex2f(0.1, 0.05);

    glVertex2f(0, 0.05);
    glVertex2f(0.025, 0.05);

    glVertex2f(0.1, 0.05);
    glVertex2f(0.075, 0.05);
    
    glVertex2f(0.025, 0.05);
    glVertex2f(0.025, 0.11);
    
    glVertex2f(0.075, 0.11);
    glVertex2f(0.075, 0.05);
    
    glVertex2f(0.025, 0.11);
    glVertex2f(0.075, 0.11);
    
    glEnd();

    
}
// function that creates the floor
void floor(){
    glBegin(GL_QUADS);
    
    glColor3f(0.75,0.5,0.5);
    glVertex2f(-100, -0.85);
    glVertex2f(100, -0.85);
    glVertex2f(100, -10);
    glVertex2f(-100, -10);
    
    glEnd();
}
// Function to create ONE cloud
void clouds(float dist, float height){
    
        
        glBegin(GL_POLYGON);
        glColor3f(1,1,1);

        glVertex2f(dist,0.5-height);
        glVertex2f(dist+0.1, 0.5-height);
        glVertex2f(dist+0.2, 0.375-height);
        glVertex2f(dist+0.1, 0.25-height);
        glVertex2f(dist, 0.25-height);
        glVertex2f(dist-0.1, 0.375-height);

        glEnd();
        
}
// Function that will create multiple instances of the cloud function
void renderClouds(int num, float cloudsDist, float cloudsHeight){
    for(int i = 0; i < num; ++i){
        clouds(cloudsDist, cloudsHeight);
        cloudsDist+= 0.4;
        if(cloudsHeight == 0){
            cloudsHeight = 0.05;
        }else{
            cloudsHeight = 0;
        }
    }
}
// Characters function for fire attack
void fireAttack(){
    glBegin(GL_QUADS);
    glColor3f(1, 0.8, 0);
    
    glVertex2f(playerX+0.1, playerY);
    glVertex2f(playerX+0.25, playerY);
    glVertex2f(playerX+0.25, playerY+0.025);
    glVertex2f(playerX+0.1, playerY+0.025);
    
    glEnd();
}
// Characters function for ice attack
void iceAttack(){
    glBegin(GL_QUADS);
    glColor3f(0, 0.1, 0.75);
    
    glVertex2f(attackX+0.1, -0.85);
    glVertex2f(attackX+0.25, -0.85);
    glVertex2f(attackX+0.25, -0.825);
    glVertex2f(attackX+0.1, -0.825);
    
    glEnd();
}
// Characters function for electric attack
void electricAttack(float incr){
    
    glTranslatef(incr, 2*incr/5, 0);
    glBegin(GL_QUADS);
    glColor3f(0, 1, 1);
    
    glVertex2f(attackX+0.05, attackY+0.025);
    glVertex2f(attackX+0.075, attackY+0.025);
    glVertex2f(attackX+0.075, attackY+0.05);
    glVertex2f(attackX+0.05, attackY+0.05);
    
    glEnd();
    
}
// Function that will display hit points for character above head
void hitPoints(float space){
    glBegin(GL_QUADS);
    glColor3f(0.1, 1, 0.1);
    
    glVertex2f(space, 0.175);
    glVertex2f(space + 0.025, 0.175);
    glVertex2f(space + 0.025, 0.15);
    glVertex2f(space, 0.15);
    
    glEnd();
}
// Function that will display hit points for boss above head
void bossHitPoints(float space){
    glBegin(GL_QUADS);
    glColor3f(1, 0.1, 0.1);
    
    glVertex2f(space+0.0125, 0.45);
    glVertex2f(space + 0.0375, 0.45);
    glVertex2f(space + 0.0375, 0.475);
    glVertex2f(space+0.0125, 0.475);
    
    glEnd();
}

void gameOver(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1.0);
    
    // G
    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, 0.02);
    glVertex2f(playerX, 0.02);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.02, 0);
    glVertex2f(playerX+0.02, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX, -0.1);
    glVertex2f(playerX+0.1, -0.1);
    glVertex2f(playerX+0.1, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX+0.08, -0.05);
    glVertex2f(playerX+0.1, -0.05);
    glVertex2f(playerX+0.1, -0.1);
    glVertex2f(playerX+0.08, -0.1);
    
    glVertex2f(playerX+0.04, -0.05);
    glVertex2f(playerX+0.08, -0.05);
    glVertex2f(playerX+0.08, -0.07);
    glVertex2f(playerX+0.04, -0.07);
    
    glEnd();
        
    // A
    glTranslatef(0.15, 0, 0);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, 0.02);
    glVertex2f(playerX, 0.02);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.02, 0);
    glVertex2f(playerX+0.02, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX+0.08, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, -0.12);
    glVertex2f(playerX+0.08, -0.12);
    
    glVertex2f(playerX+0.02, -0.05);
    glVertex2f(playerX+0.08, -0.05);
    glVertex2f(playerX+0.08, -0.07);
    glVertex2f(playerX+0.02, -0.07);
    
    glEnd();
    
    
    // M
    glTranslatef(0.15, 0, 0);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, 0.02);
    glVertex2f(playerX, 0.02);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.02, 0);
    glVertex2f(playerX+0.02, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX+0.08, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, -0.12);
    glVertex2f(playerX+0.08, -0.12);
    
    glVertex2f(playerX+0.04, 0);
    glVertex2f(playerX+0.06, 0);
    glVertex2f(playerX+0.06, -0.12);
    glVertex2f(playerX+0.04, -0.12);
    
    glEnd();
    
    // E
    glTranslatef(0.15, 0, 0);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, 0.02);
    glVertex2f(playerX, 0.02);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.02, 0);
    glVertex2f(playerX+0.02, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX, -0.1);
    glVertex2f(playerX+0.1, -0.1);
    glVertex2f(playerX+0.1, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX, -0.04);
    glVertex2f(playerX+0.075, -0.04);
    glVertex2f(playerX+0.075, -0.06);
    glVertex2f(playerX, -0.06);
    
    glEnd();
    
    // O
    glTranslatef(0.25, 0, 0);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, 0.02);
    glVertex2f(playerX, 0.02);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.02, 0);
    glVertex2f(playerX+0.02, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX, -0.1);
    glVertex2f(playerX+0.1, -0.1);
    glVertex2f(playerX+0.1, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX+0.08, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, -0.12);
    glVertex2f(playerX+0.08, -0.12);
    
    glEnd();
    
    // V
    glTranslatef(0.15, 0, 0);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(playerX, 0.02);
    glVertex2f(playerX+0.02, 0.02);
    glVertex2f(playerX+0.02, -0.06);
    glVertex2f(playerX, -0.06);
    
    glVertex2f(playerX, -0.06);
    glVertex2f(playerX+0.02, -0.06);
    glVertex2f(playerX+0.06, -0.12);
    glVertex2f(playerX+0.04, -0.12);
    
    glVertex2f(playerX+0.04, -0.12);
    glVertex2f(playerX+0.06, -0.12);
    glVertex2f(playerX+0.1, -0.06);
    glVertex2f(playerX+0.08, -0.06);
    
    glVertex2f(playerX+0.08, 0.02);
    glVertex2f(playerX+0.1, 0.02);
    glVertex2f(playerX+0.1, -0.06);
    glVertex2f(playerX+0.08, -0.06);
    
    glEnd();
    
    // E
    glTranslatef(0.15, 0, 0);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, 0.02);
    glVertex2f(playerX, 0.02);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.02, 0);
    glVertex2f(playerX+0.02, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX, -0.1);
    glVertex2f(playerX+0.1, -0.1);
    glVertex2f(playerX+0.1, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX, -0.04);
    glVertex2f(playerX+0.075, -0.04);
    glVertex2f(playerX+0.075, -0.06);
    glVertex2f(playerX, -0.06);
    
    glEnd();
    
    // R
    glTranslatef(0.15, 0, 0);
    
    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, 0.02);
    glVertex2f(playerX, 0.02);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.02, 0);
    glVertex2f(playerX+0.02, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX+0.02, -0.04);
    glVertex2f(playerX+0.08, -0.04);
    glVertex2f(playerX+0.08, -0.06);
    glVertex2f(playerX+0.02, -0.06);
    
    glVertex2f(playerX+0.08, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, -0.06);
    glVertex2f(playerX+0.08, -0.06);
    
    glVertex2f(playerX, -0.06);
    glVertex2f(playerX+0.02, -0.06);
    glVertex2f(playerX+0.1, -0.12);
    glVertex2f(playerX+0.08, -0.12);
    
    glEnd();
}

void win(){
    // V
    
    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(playerX, 0.02);
    glVertex2f(playerX+0.02, 0.02);
    glVertex2f(playerX+0.02, -0.06);
    glVertex2f(playerX, -0.06);
    
    glVertex2f(playerX, -0.06);
    glVertex2f(playerX+0.02, -0.06);
    glVertex2f(playerX+0.06, -0.12);
    glVertex2f(playerX+0.04, -0.12);
    
    glVertex2f(playerX+0.04, -0.12);
    glVertex2f(playerX+0.06, -0.12);
    glVertex2f(playerX+0.1, -0.06);
    glVertex2f(playerX+0.08, -0.06);
    
    glVertex2f(playerX+0.08, 0.02);
    glVertex2f(playerX+0.1, 0.02);
    glVertex2f(playerX+0.1, -0.06);
    glVertex2f(playerX+0.08, -0.06);
    
    glEnd();
    
    // I
    
    glTranslatef(0.15, 0, 0);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(playerX, 0.02);
    glVertex2f(playerX+0.02, 0.02);
    glVertex2f(playerX+0.02, -0.12);
    glVertex2f(playerX, -0.12);
    
    glEnd();
    
    // C
    
    glTranslatef(0.07, 0, 0);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, 0.02);
    glVertex2f(playerX, 0.02);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.02, 0);
    glVertex2f(playerX+0.02, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX, -0.1);
    glVertex2f(playerX+0.1, -0.1);
    glVertex2f(playerX+0.1, -0.12);
    glVertex2f(playerX, -0.12);
    
    glEnd();
    
    // T
    
    glTranslatef(0.15, 0, 0);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, 0.02);
    glVertex2f(playerX, 0.02);
    
    glVertex2f(playerX+0.04, 0);
    glVertex2f(playerX+0.06, 0);
    glVertex2f(playerX+0.06, -0.12);
    glVertex2f(playerX+0.04, -0.12);
    
    glEnd();
    
    // O
    glTranslatef(0.15, 0, 0);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, 0.02);
    glVertex2f(playerX, 0.02);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.02, 0);
    glVertex2f(playerX+0.02, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX, -0.1);
    glVertex2f(playerX+0.1, -0.1);
    glVertex2f(playerX+0.1, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX+0.08, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, -0.12);
    glVertex2f(playerX+0.08, -0.12);
    
    glEnd();
    
    // R
    glTranslatef(0.15, 0, 0);
    
    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, 0.02);
    glVertex2f(playerX, 0.02);
    
    glVertex2f(playerX, 0);
    glVertex2f(playerX+0.02, 0);
    glVertex2f(playerX+0.02, -0.12);
    glVertex2f(playerX, -0.12);
    
    glVertex2f(playerX+0.02, -0.04);
    glVertex2f(playerX+0.08, -0.04);
    glVertex2f(playerX+0.08, -0.06);
    glVertex2f(playerX+0.02, -0.06);
    
    glVertex2f(playerX+0.08, 0);
    glVertex2f(playerX+0.1, 0);
    glVertex2f(playerX+0.1, -0.06);
    glVertex2f(playerX+0.08, -0.06);
    
    glVertex2f(playerX, -0.06);
    glVertex2f(playerX+0.02, -0.06);
    glVertex2f(playerX+0.1, -0.12);
    glVertex2f(playerX+0.08, -0.12);
    
    glEnd();
    
}














void enemy(){
    glTranslatef(enemyX, enemyY, 0);
    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    
    glVertex2f(0, 0);
    glVertex2f(0.1, 0);
    glVertex2f(0.1, 0.1);
    glVertex2f(0, 0.1);
    
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    
    glVertex2f(0, 0);
    glVertex2f(0.1, 0);
    
    glVertex2f(0.1, 0);
    glVertex2f(0.1, 0.1);
    
    glVertex2f(0.1, 0.1);
    glVertex2f(0, 0.1);
    
    glVertex2f(0, 0.1);
    glVertex2f(0, 0);
    
    glEnd();
    
    float weaponX = -1000;
    float weaponY = -1000;
    
    if((enemyAlive)&&(currentWeap==FIRE)&&(attack)){
        weaponX = playerX;
        weaponY = playerY;
        if(((weaponX+0.25)>=enemyX)&&((enemyX+0.1)>=(weaponX+0.1))){
            if(((weaponY+0.025)>=enemyY)&&((enemyY+0.1)>=(weaponY))){
                enemyAlive = false;
                attack = false;
            }
        }
    }
    if((enemyAlive)&&(currentWeap==ICE)&&(attack)){
        weaponX = attackX;
        weaponY = attackY;
        if(((weaponX+0.25)>=enemyX)&&((enemyX+0.1)>=(weaponX+0.1))){
            if(((-0.825)>=enemyY)&&((enemyY+0.1)>=(-0.85))){
                enemyAlive = false;
                attack = false;
            }
        }
    }
    if((enemyAlive)&&(currentWeap==ELECTRIC)&&(attack)){
        weaponX = attackX+elecIncr;
        weaponY = attackY+(2*elecIncr/5);
        if(((weaponX+0.075)>=enemyX)&&((enemyX+0.1)>=(weaponX+0.05))){
            if(((weaponY+0.075)>=enemyY)&&((enemyY+0.1)>=(weaponY+0.05))){
                enemyAlive = false;
                attack = false;
            }
        }
    }

}

void enemyMovement(){
    if(enemyDirX){
        enemyX+=enemyMoveX;
        if(enemyX > 2){
            enemyDirX = false;
        }
    }
    if(!enemyDirX){
        enemyX-=enemyMoveX;
        if(enemyX < 0){
            enemyDirX = true;
        }
    }
    
    if(enemyDirY){
        enemyY+=enemyMoveY;
        if(enemyY > -0.1){
            enemyDirY = false;
        }
    }
    if(!enemyDirY){
        enemyY-=enemyMoveY;
        if(enemyY < -0.3){
            enemyDirY = true;
        }
    }
}

void enemyAttack(float incr){
    glTranslatef(incr, 3*incr/5, 0);
    glBegin(GL_QUADS);
    glColor3f(1, 0, 1);
    
    glVertex2f(enemyAttackX-0.05, enemyAttackY-0.025);
    glVertex2f(enemyAttackX-0.075, enemyAttackY-0.025);
    glVertex2f(enemyAttackX-0.075, enemyAttackY-0.05);
    glVertex2f(enemyAttackX-0.05, enemyAttackY-0.05);
    
    glEnd();
    
    float enemyWeaponX, enemyWeaponY;
    
    if((enemyAlive)&&(!hit)){
        enemyWeaponX = enemyAttackX+incr;
        enemyWeaponY = enemyAttackY+(3*incr/5);
        if(((enemyWeaponX-0.05)>=playerX)&&((playerX+0.1)>=(enemyWeaponX-0.075))){
            if(((enemyWeaponY-0.025)>=playerY-0.05)&&((playerY+0.05)>=(enemyWeaponY-0.05))){
                points-=1;
                hit = true;
            }
        }
    }
    if((enemyAlive)&&(hit)){
        enemyWeaponX = enemyAttackX+incr;
        enemyWeaponY = enemyAttackY+(3*incr/5);
        if(((enemyWeaponX-0.05)<=playerX)||((playerX+0.1)<=(enemyWeaponX-0.075))||
            ((enemyWeaponY-0.025)<=playerY-0.05)||((playerY+0.05)<=(enemyWeaponY-0.05))){
                hit = false;
            }
        
    }
}








void enemyTwo(){
    glTranslatef(enemyTwoX, enemyTwoY, 0);
    glBegin(GL_QUADS);
    glColor3f(0.2, 1, 0.2);
    
    glVertex2f(0, 0);
    glVertex2f(0.15, 0);
    glVertex2f(0.15, 0.05);
    glVertex2f(0, 0.05);
    
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    
    glVertex2f(0, 0);
    glVertex2f(0.15, 0);
    
    glVertex2f(0.15, 0);
    glVertex2f(0.15, 0.05);
    
    glVertex2f(0.15, 0.05);
    glVertex2f(0, 0.05);
    
    glVertex2f(0, 0.05);
    glVertex2f(0, 0);
    
    glEnd();
    
    float weaponX = -1000;
    float weaponY = -1000;
    
    if((enemyTwoAlive)&&(currentWeap==FIRE)&&(attack)){
        weaponX = playerX;
        weaponY = playerY;
        if(((weaponX+0.25)>=enemyTwoX)&&((enemyTwoX+0.15)>=(weaponX+0.1))){
            if(((weaponY+0.025)>=enemyTwoY)&&((enemyTwoY+0.05)>=(weaponY))){
                enemyTwoAlive = false;
                attack = false;
            }
        }
    }
    if((enemyTwoAlive)&&(currentWeap==ICE)&&(attack)){
        weaponX = attackX;
        weaponY = attackY;
        if(((weaponX+0.25)>=enemyTwoX)&&((enemyTwoX+0.15)>=(weaponX+0.1))){
            if(((-0.825)>=enemyTwoY)&&((enemyTwoY+0.05)>=(-0.85))){
                enemyTwoAlive = false;
                attack = false;
            }
        }
    }
    if((enemyTwoAlive)&&(currentWeap==ELECTRIC)&&(attack)){
        weaponX = attackX+elecIncr;
        weaponY = attackY+(2*elecIncr/5);
        if(((weaponX+0.075)>=enemyTwoX)&&((enemyTwoX+0.15)>=(weaponX+0.05))){
            if(((weaponY+0.075)>=enemyTwoY)&&((enemyTwoY+0.05)>=(weaponY+0.05))){
                enemyTwoAlive = false;
                attack = false;
            }
        }
    }
}


void enemyTwoMovement(){
    if(enemyTwoDirX){
        enemyTwoX+=enemyTwoMoveX;
        if(enemyTwoX > 1.5){
            enemyTwoDirX = false;
        }
    }
    if(!enemyTwoDirX){
        enemyTwoX-=enemyTwoMoveX;
        if(enemyTwoX < 1){
            enemyTwoDirX = true;
        }
    }
}

void enemyTwoAttack(){
    glBegin(GL_QUADS);
    glColor3f(0.75, 0.6, 0.1);
    
    glVertex2f(enemyTwoX-0.2, enemyTwoY);
    glVertex2f(enemyTwoX, enemyTwoY);
    glVertex2f(enemyTwoX, enemyTwoY+0.025);
    glVertex2f(enemyTwoX-0.2, enemyTwoY+0.025);
    
    glEnd();
    
    
    if((enemyTwoAlive)&&(!hit2)){
        if(((enemyTwoX)>=playerX)&&((playerX+0.1)>=(enemyTwoX-0.2))){
            if(((enemyTwoY+0.025)>=playerY-0.05)&&((playerY+0.05)>=(enemyTwoY))){
                points-=1;
                hit2 = true;
            }
        }
    }
    if((enemyTwoAlive)&&(hit2)){
        if((((enemyTwoX)<playerX)||((playerX+0.1)<(enemyTwoX-0.2)))||
            (((enemyTwoY+0.025)<playerY-0.05)||((playerY+0.05)<(enemyTwoY)))){
                hit2 = false;
        }
    }
}











void enemyThree(){
    glTranslatef(enemyThreeX, enemyThreeY, 0);
    glBegin(GL_QUADS);
    glColor3f(0.5, 0.5, 0.5);
    
    glVertex2f(0, 0);
    glVertex2f(0.2, 0);
    glVertex2f(0.2, 0.2);
    glVertex2f(0, 0.2);
    
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    
    glVertex2f(0, 0);
    glVertex2f(0.2, 0);
    
    glVertex2f(0.2, 0);
    glVertex2f(0.2, 0.2);
    
    glVertex2f(0.2, 0.2);
    glVertex2f(0, 0.2);
    
    glVertex2f(0, 0.2);
    glVertex2f(0, 0);
    
    glEnd();
    
    float weaponX = -1000;
    float weaponY = -1000;
    
    if((enemyThreeAlive)&&(currentWeap==FIRE)&&(attack)){
        weaponX = playerX;
        weaponY = playerY;
        if(((weaponX+0.25)>=enemyThreeX)&&((enemyThreeX+0.2)>=(weaponX+0.1))){
            if(((weaponY+0.025)>=enemyThreeY)&&((enemyThreeY+0.2)>=(weaponY))){
                enemyThreeAlive = false;
                attack = false;
            }
        }
    }
    if((enemyThreeAlive)&&(currentWeap==ICE)&&(attack)){
        weaponX = attackX;
        weaponY = attackY;
        if(((weaponX+0.25)>=enemyThreeX)&&((enemyThreeX+0.2)>=(weaponX+0.1))){
            if(((-0.825)>=enemyThreeY)&&((enemyThreeY+0.2)>=(-0.85))){
                enemyThreeAlive = false;
                attack = false;
            }
        }
    }
    if((enemyThreeAlive)&&(currentWeap==ELECTRIC)&&(attack)){
        weaponX = attackX+elecIncr;
        weaponY = attackY+(2*elecIncr/5);
        if(((weaponX+0.075)>=enemyThreeX)&&((enemyThreeX+0.2)>=(weaponX+0.05))){
            if(((weaponY+0.075)>=enemyThreeY)&&((enemyThreeY+0.2)>=(weaponY+0.05))){
                enemyThreeAlive = false;
                attack = false;
            }
        }
    }

}

void enemyThreeMovement(){
    /*if(enemyThreeDirX){
        enemyThreeX+=enemyThreeMoveX;
        if(enemyThreeX > 3){
            enemyThreeDirX = false;
        }
    }
    if(!enemyThreeDirX){
        enemyThreeX-=enemyThreeMoveX;
        if(enemyThreeX < 1.5){
            enemyThreeDirX = true;
        }
    }*/
    
    if(enemyThreeDirY){
        enemyThreeY+=enemyThreeMoveY;
        if(enemyThreeY > -0.6){
            enemyThreeDirY = false;
        }
    }
    if(!enemyThreeDirY){
        enemyThreeY-=enemyThreeMoveY;
        if(enemyThreeY < -0.85){
            enemyThreeDirY = true;
        }
    }
}

void enemyThreeAttack(float incr){
    glTranslatef(incr, incr/10, 0);
    glBegin(GL_QUADS);
    glColor3f(0.75, 0.25, 0.9);
    
    glVertex2f(enemyThreeAttackX-0.05, enemyThreeAttackY-0.025);
    glVertex2f(enemyThreeAttackX-0.075, enemyThreeAttackY-0.025);
    glVertex2f(enemyThreeAttackX-0.075, enemyThreeAttackY-0.05);
    glVertex2f(enemyThreeAttackX-0.05, enemyThreeAttackY-0.05);
    
    glEnd();
    
    float enemyWeaponX, enemyWeaponY;
    
    if((enemyThreeAlive)&&(!hit3)){
        enemyWeaponX = enemyThreeAttackX+incr;
        enemyWeaponY = enemyThreeAttackY+(incr/10);
        if(((enemyWeaponX-0.05)>=playerX)&&((playerX+0.1)>=(enemyWeaponX-0.075))){
            if(((enemyWeaponY-0.025)>=playerY-0.05)&&((playerY+0.05)>=(enemyWeaponY-0.05))){
                points-=1;
                hit3 = true;
            }
        }
    }
    if((enemyThreeAlive)&&(hit3)){
        enemyWeaponX = enemyThreeAttackX+incr;
        enemyWeaponY = enemyThreeAttackY+(incr/10);
        if(((enemyWeaponX-0.05)<playerX)||((playerX+0.1)<(enemyWeaponX-0.075))||
            ((enemyWeaponY-0.025)<playerY-0.05)||((playerY+0.05)<(enemyWeaponY-0.05))){
                hit3 = false;
            }
        
    }
}







void enemyBoss(){
    glTranslatef(enemyBossX, enemyBossY, 0);
    glBegin(GL_QUADS);
    glColor3f(0.1, 0.1, 0.1);
    
    glVertex2f(0, 0);
    glVertex2f(0.2, 0);
    glVertex2f(0.2, 0.3);
    glVertex2f(0, 0.3);
    
    glVertex2f(0.075, 0.3);
    glVertex2f(0.125, 0.3);
    glVertex2f(0.125, 0.4);
    glVertex2f(0.075, 0.4);
    
    glVertex2f(0.075, -0.1);
    glVertex2f(0.125, -0.1);
    glVertex2f(0.125, 0);
    glVertex2f(0.075, 0);
    
    glEnd();
    
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    
    glVertex2f(0, 0);
    glVertex2f(0.075, 0);
    
    glVertex2f(0.075, 0);
    glVertex2f(0.075, -0.1);
    
    glVertex2f(0.075, -0.1);
    glVertex2f(0.125, -0.1);
    
    glVertex2f(0.125, -0.1);
    glVertex2f(0.125, 0);

    glVertex2f(0.125, 0);
    glVertex2f(0.2, 0);
    
    glVertex2f(0.2, 0);
    glVertex2f(0.2, 0.3);
    
    glVertex2f(0.2, 0.3);
    glVertex2f(0.125, 0.3);
    
    glVertex2f(0.125, 0.3);
    glVertex2f(0.125, 0.4);
    
    glVertex2f(0.125, 0.4);
    glVertex2f(0.075, 0.4);
    
    glVertex2f(0.075, 0.4);
    glVertex2f(0.075, 0.3);
    
    glVertex2f(0.075, 0.3);
    glVertex2f(0, 0.3);

    glVertex2f(0, 0.3);
    glVertex2f(0, 0);
    
    glEnd();
    
    float weaponX = -1000;
    float weaponY = -1000;
    
    if((enemyBossAlive)&&(currentWeap==FIRE)&&(attack)){
        weaponX = playerX;
        weaponY = playerY;
        if(((weaponX+0.25)>=enemyBossX)&&((enemyBossX+0.2)>=(weaponX+0.1))){
            if(((weaponY+0.025)>=enemyBossY)&&((enemyBossY+0.3)>=(weaponY))){
                bossPoints-=1;
                attack = false;
            }
        }
    }
    if((enemyBossAlive)&&(currentWeap==ICE)&&(attack)){
        weaponX = attackX;
        weaponY = attackY;
        if(((weaponX+0.25)>=enemyBossX)&&((enemyBossX+0.2)>=(weaponX+0.1))){
            if(((-0.825)>=enemyBossY)&&((enemyBossY+0.3)>=(-0.85))){
                bossPoints-=1;
                attack = false;
            }
        }
    }
    if((enemyBossAlive)&&(currentWeap==ELECTRIC)&&(attack)){
        weaponX = attackX+elecIncr;
        weaponY = attackY+(2*elecIncr/5);
        if(((weaponX+0.075)>=enemyBossX)&&((enemyBossX+0.2)>=(weaponX+0.05))){
            if(((weaponY+0.075)>=enemyBossY)&&((enemyBossY+0.3)>=(weaponY+0.05))){
                bossPoints-=1;
                attack = false;
            }
        }
    }
    
    if(bossPoints==0){
        enemyBossAlive = false;
    }

}

void enemyBossMovement(){
    if(enemyBossDirX){
        enemyBossX+=enemyBossMoveX;
        if(enemyBossX > 3.5){
            enemyBossDirX = false;
        }
    }
    if(!enemyBossDirX){
        enemyBossX-=enemyBossMoveX;
        if(enemyBossX < 2.75){
            enemyBossDirX = true;
        }
    }
    
    if(enemyBossDirY){
        enemyBossY+=enemyBossMoveY;
        if(enemyBossY > -0.5){
            enemyBossDirY = false;
        }
    }
    if(!enemyBossDirY){
        enemyBossY-=enemyBossMoveY;
        if(enemyBossY < -0.6){
            enemyBossDirY = true;
        }
    }
}

void enemyBossAttack(float incr){
    glTranslatef(incr, 2*incr/7, 0);
    glBegin(GL_QUADS);
    glColor3f(1, 0, 1);
    
    glVertex2f(enemyBossAttackX-0.05, enemyBossAttackY-0.025);
    glVertex2f(enemyBossAttackX-0.075, enemyBossAttackY-0.025);
    glVertex2f(enemyBossAttackX-0.075, enemyBossAttackY-0.05);
    glVertex2f(enemyBossAttackX-0.05, enemyBossAttackY-0.05);
    
    glEnd();
    
    float enemyWeaponX, enemyWeaponY;
    
    if((enemyBossAlive)&&(!hitBoss)){
        enemyWeaponX = enemyBossAttackX+incr;
        enemyWeaponY = enemyBossAttackY+(2*incr/7);
        if(((enemyWeaponX-0.05)>=playerX)&&((playerX+0.1)>=(enemyWeaponX-0.075))){
            if(((enemyWeaponY-0.025)>=playerY-0.05)&&((playerY+0.05)>=(enemyWeaponY-0.05))){
                points-=1;
                hitBoss = true;
            }
        }
    }
    if((enemyBossAlive)&&(hitBoss)){
        enemyWeaponX = enemyBossAttackX+incr;
        enemyWeaponY = enemyBossAttackY+(2*incr/7);
        if(((enemyWeaponX-0.05)<playerX)||((playerX+0.1)<(enemyWeaponX-0.075))||
            ((enemyWeaponY-0.025)<playerY-0.05)||((playerY+0.05)<(enemyWeaponY-0.05))){
                hitBoss = false;
            }
        
    }
}

void enemyBossAttackTwo(float incr){
    glTranslatef(incr, (6*incr/5)+0.3, 0);
    glBegin(GL_QUADS);
    glColor3f(1, 0, 1);
    
    glVertex2f(enemyBossAttackX-0.05, enemyBossAttackY-0.025);
    glVertex2f(enemyBossAttackX-0.075, enemyBossAttackY-0.025);
    glVertex2f(enemyBossAttackX-0.075, enemyBossAttackY-0.05);
    glVertex2f(enemyBossAttackX-0.05, enemyBossAttackY-0.05);
    
    glEnd();
    
    float enemyWeaponX, enemyWeaponY;
    
    if((enemyBossAlive)&&(!hitBoss2)){
        enemyWeaponX = enemyBossAttackX+incr;
        enemyWeaponY = enemyBossAttackY+(6*incr/5)+0.3;
        if(((enemyWeaponX-0.05)>=playerX)&&((playerX+0.1)>=(enemyWeaponX-0.075))){
            if(((enemyWeaponY-0.025)>=playerY-0.05)&&((playerY+0.05)>=(enemyWeaponY-0.05))){
                points-=1;
                hitBoss2 = true;
            }
        }
    }
    if((enemyBossAlive)&&(hitBoss2)){
        enemyWeaponX = enemyBossAttackX+incr;
        enemyWeaponY = enemyBossAttackY+(6*incr/5)+0.3;
        if(((enemyWeaponX-0.05)<playerX)||((playerX+0.1)<(enemyWeaponX-0.075))||
            ((enemyWeaponY-0.025)<playerY-0.05)||((playerY+0.05)<(enemyWeaponY-0.05))){
                hitBoss2 = false;
            }
        
    }
}
