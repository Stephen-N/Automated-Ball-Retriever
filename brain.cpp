#include "common.h"

using namespace std;

// variables for GUI creation and managemnt
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 960;

const int BUTTON_WIDTH = SCREEN_WIDTH / 2;
const int BUTTON_HEIGHT = SCREEN_HEIGHT / 2;

const int wiggleRoom = 5;
const int OUTSTANDING_UPDATE = 150;

const bool virtualRays = true;

int userRect, discoverdRect, CURRENT_ROUTINE;

int userDistanceToCollition[8];  // 0 = 0 deg
int discoverDistanceToCollition[8];

SDL_Renderer* gRender = NULL;

SDL_Rect gSpriteClips[4];

bool badArea[SCREEN_WIDTH][SCREEN_HEIGHT];

enum KeyPress {
    KEY_PRESS_DEFAULT,
    KEY_PRESS_UP,
    KEY_PRESS_DOWN,
    KEY_PRESS_LEFT,
    KEY_PRESS_RIGHT,
    KEY_PRESS_B,
    KEY_PRESS_N,
    KEY_PRESS_M,
    KEY_PRESS_TOTAL
};

enum LButtonSprite {
    BUTTON_SPRITE_MOUSE_OUT = 0,
    BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
    BUTTON_SPRITE_MOUSE_DOWN = 2,
    BUTTON_SPRITE_MOUSE_UP = 3,
    BUTTON_SPRITE_TOTAL = 4
};

enum Routines {
    ROUTINE_ERROR,
    ROUTINE_MANUAL,
    ROUTINE_BOUNCE,
    ROUTINE_SWEEP,
    ROUTINE_DISCOVERY
};

struct rectPoints {
    int xStart = -1;
    int yStart = -1;
    int xFinish = -1;
    int yFinish = -1;
    int R = 0xFF;
    int G = 0x00;
    int B = 0x00;
    int Alpha = 0xFF;
    double confidence = 0.00;
};

bool updateArea(vector<rectPoints>& list, int startPos = 0) {
    // printf("\nUpdate Start Value:\t%d\n", startPos);
    if (startPos == 0) {
        for (size_t i = 0; i < SCREEN_WIDTH; i++) {
            for (size_t j = 0; j < SCREEN_WIDTH; j++) {
                badArea[i][j] = false;
            }
        }
    }

    if (startPos > list.size() - 1) {
        return false;
    }

    for (size_t i = startPos; i < list.size(); i++) {
        for (size_t x = list.at(i).xStart; x < list.at(i).xFinish; x++) {
            for (size_t y = list.at(i).yStart; y < list.at(i).yFinish; y++) {
                badArea[x][y] = true;
            }
        }
    }
    return true;
}

class LTexture {
   public:
    LTexture() {
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }

    ~LTexture() {
        free();
    }

    bool loadFromFile(string path) {
        free();

        SDL_Texture* loadTexture = NULL;

        SDL_Surface* surf = IMG_Load(path.c_str());
        if (surf == NULL) {
            printf("Unable to load image %s! SDL_image Errir: %s\n", path.c_str(), IMG_GetError());
            EXITONERROR();
        }

        SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0, 0xFF, 0xFF));

        loadTexture = SDL_CreateTextureFromSurface(gRender, surf);
        if (loadTexture == NULL) {
            printf("Unable to create texture from %s! SDL Error %s\n", path.c_str(), SDL_GetError());
            EXITONERROR();
        }

        mWidth = surf->w;
        mHeight = surf->h;

        SDL_FreeSurface(surf);

        mTexture = loadTexture;
        return mTexture != NULL;
    }

#if defined(SDL_TTF_MAJOR_VERSION)
    bool loadFromRenderedText(string textureText, SDL_Color textColor) {
        free();

        SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);

        if (SDL_Surface = NULL) {
            printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
            EXITONERROR();
        }

        mWidth = textSurface->w;
        mHeight = textSurface->h;

        SDL_FreeSurface(textSurface);

        return mTexture != NULL;
    }
#endif

    void free() {
        if (mTexture != NULL) {
            SDL_DestroyTexture(mTexture);
            mTexture = NULL;
            mWidth = 0;
            mHeight = 0;
        }
    }

    void setColor(Uint8 red, Uint8 green, Uint8 blue) {
        SDL_SetTextureColorMod(mTexture, red, green, blue);
    }

    void setBlendMode(SDL_BlendMode blend) {
        SDL_SetTextureBlendMode(mTexture, blend);
    }

    void setAlpha(Uint8 alpha) {
        SDL_SetTextureAlphaMod(mTexture, alpha);
    }

    void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0,
                SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE) {
        SDL_Rect renderQuad = {x, y, mWidth, mHeight};

        if (clip != NULL) {
            renderQuad.w = clip->w;
            renderQuad.h = clip->h;
        }

        SDL_RenderCopyEx(gRender, mTexture, clip, &renderQuad, angle, center, flip);
    }

    int getWidth() {
        return mWidth;
    }

    int getHeight() {
        return mHeight;
    }

   private:
    SDL_Texture* mTexture;

    int mWidth;
    int mHeight;
};

// Pre Requ Obj
LTexture gButtonSpriteSheetTexture;
LTexture gRobotTexture;

vector<rectPoints> userList;
vector<rectPoints> discoverList;

class LTimer {
   public:
    LTimer();

    // The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    // Gets the timer's time
    Uint32 getTicks();

    // Checks the status of the timer
    bool isStarted();
    bool isPaused();

   private:
    // The clock time when the timer started
    Uint32 mStartTicks;

    // The ticks stored when the timer was paused
    Uint32 mPausedTicks;

    // The timer status
    bool mPaused;
    bool mStarted;
};

class robot {
   public:
    // The dimensions of the Robot
    static const int Robot_WIDTH = 10;
    static const int Robot_HEIGHT = 20;

    // Maximum axis velocity of the Robot
    static const int Robat_Max_Velocity = 15;

    // Maximum axis velocity of the Robot
    static const int Robat_Max_Omega = 15;

    // Initializes the variables
    robot() {
        // Initialize the offsets
        mPosX = SCREEN_WIDTH / 2 - Robot_WIDTH;
        mPosY = SCREEN_HEIGHT / 2 - Robot_HEIGHT;

        // Initialize the velocity
        mVelocity = 0;

        // Initialize the angle
        mAngle = 0.0;
        mOmega = 0.0;

        atWall = false;
    }

    double getAngle() {
        return mAngle;
    }

    int getAngleasI() {
        return static_cast<int>(mAngle);
    }

    int getXpos() {
        return mPosX;
    }

    int getYpos() {
        return mPosY;
    }

    bool isStopped() {
        return mOmega == 0 && mVelocity == 0;
    }

    bool isAtWall() {
        switch (getAngleasI()) {
            case 0:
                return wallAbove();
            case 90:
                return wallToRight();
            case 180:
                return wallBelow();
            case 270:
                return wallToLeft();
            default:
                return atWall;
        }
        // return wallToRight() || wallToLeft() || wallAbove() || wallBelow();
    }

    void stop() {
        lastOmega = mOmega;
        lastVelocity = mVelocity;
        mOmega = 0;
        mVelocity = 0;
    }

    int getVelocity() {
        return mVelocity;
    }

    int getOmega() {
        return static_cast<int>(mOmega);
    }

    bool turn(bool turnRight) {
        if (turnRight) {
            mOmega += 5;
            if (mOmega > Robat_Max_Omega) {
                mOmega = Robat_Max_Omega;
            }
        } else {
            mOmega -= 5;
            if (mOmega < -1 * Robat_Max_Omega) {
                mOmega = -1 * Robat_Max_Omega;
            }
        }
        return true;
    }

    bool foward(bool accelerate) {
        if (accelerate) {
            mVelocity += 5;
            if (mVelocity > Robat_Max_Velocity) {
                mVelocity = Robat_Max_Velocity;
            }
            if (fowardToCollition() < mVelocity) {
                stop();
                return false;
            }
        } else {
            mVelocity -= 5;
            if (mVelocity < -1 * Robat_Max_Velocity) {
                mVelocity = -1 * Robat_Max_Velocity;
            }
        }
        return true;
    }

    bool fowardUnit(int distance, int safe = 40) {
        if (fowardToCollition() != -1) {
            if (fowardToCollition() <= safe) {
                if (mVelocity == 0) {
                    // printf("At Wall\n");
                    return false;
                } else {
                    // printf("Approaching Wall\n");
                    atWall = false;
                    mVelocity -= 5;
                    return false;
                }
            }
        }
        if (distance == 0) {
            if (mVelocity == 0) {
                return false;
            } else if (mVelocity == 5) {
                mVelocity = 0;
            }
        } else if (distance == 5) {
            mVelocity = 5;
        } else if (distance == 10) {
            if (mVelocity == 15) {
                mVelocity == 10;
            }
            mVelocity = 5;
        } else if (distance == 15) {
            if (mVelocity <= 5) {
                mVelocity = 5;
            } else {
                mVelocity = 10;
            }
        } else if (distance == 20) {
            if (mVelocity == 0) {
                mVelocity = 5;
            } else {
                mVelocity = 10;
            }
        } else if (distance >= 25) {
            if (mVelocity == 0) {
                mVelocity = 5;
            } else if (mVelocity == 5) {
                mVelocity = 10;
            } else {
                mVelocity = 15;
            }
        }
        return true;
    }

    bool backword(bool accelerate) {
        if (accelerate) {
            mVelocity -= 5;
            if (mVelocity > Robat_Max_Velocity) {
                mVelocity = Robat_Max_Velocity;
            }
        } else {
            mVelocity += 5;
            if (mVelocity < -1 * Robat_Max_Velocity) {
                mVelocity = -1 * Robat_Max_Velocity;
            }
        }
        return true;
    }

    void infoReadOut(bool showRays = false) {
        printf("\nInformation Readout\nCurrent Angle:\t\t%lf\nCurrent Veloctiy:\t%d\nCurrent Omega:\t\t%d\n", mAngle, lastVelocity, lastOmega);
        if (showRays) {
            for (size_t i = 0; i < 8; i++) {
                cout << "Distance @ Ray#" << i << ":\t" << userDistanceToCollition[i] << "\n";
            }
            cout << "\n";
            cout.flush();
        }
    }

    // Takes key presses and adjusts the Robot's velocity
    void handleEvent(SDL_Event& e) {
        // If a key was pressed
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_q) {
                e.quit;
                return;
            }
            if (e.key.keysym.sym == SDLK_b) {
                printf("Now in bounce mode!\n");
                CURRENT_ROUTINE = ROUTINE_BOUNCE;
            }
            if (e.key.keysym.sym == SDLK_n) {
                printf("Now in discovery mode!\n");
                CURRENT_ROUTINE = ROUTINE_DISCOVERY;
            }
            if (e.key.keysym.sym == SDLK_m) {
                printf("Now in manual mode!\n");
                CURRENT_ROUTINE = ROUTINE_MANUAL;
            }
            if (e.key.keysym.sym == SDLK_v) {
                printf("Now in sweep mode!\n");
                CURRENT_ROUTINE = ROUTINE_SWEEP;
            }
            if (e.key.keysym.sym == SDLK_z) {
                if (userRect != 0) {
                    userRect--;
                    userList.pop_back();
                }
                updateArea(userList);
            }
            if (e.key.keysym.sym == SDLK_r) {
                mPosX = SCREEN_WIDTH / 2 - Robot_WIDTH;
                mPosY = SCREEN_HEIGHT / 2 - Robot_HEIGHT;

                // Initialize the velocity
                mVelocity = 0;

                // Initialize the angle
                mAngle = 0.0;
                mOmega = 0.0;
            }
            if (e.key.keysym.sym == SDLK_t) {
                while (userList.size() > 0) {
                    userList.pop_back();
                }
                userRect = 4;

                rectPoints a = rectPoints();
                a.xStart = SCREEN_WIDTH / 3;
                a.xFinish = 2 * SCREEN_WIDTH / 3;
                a.yStart = SCREEN_HEIGHT / 3;
                a.yFinish = SCREEN_HEIGHT / 3 + 10;
                userList.push_back(a);

                rectPoints b = rectPoints();
                b.xStart = SCREEN_WIDTH / 3 - 10;
                b.xFinish = SCREEN_WIDTH / 3;
                b.yStart = SCREEN_HEIGHT / 3;
                b.yFinish = 2 * SCREEN_HEIGHT / 3;
                userList.push_back(b);

                rectPoints c = rectPoints();
                c.xStart = 2 * SCREEN_WIDTH / 3;
                c.xFinish = 2 * SCREEN_WIDTH / 3 + 10;
                c.yStart = SCREEN_HEIGHT / 3;
                c.yFinish = 2 * SCREEN_HEIGHT / 3;
                userList.push_back(c);

                rectPoints d = rectPoints();
                d.xStart = SCREEN_WIDTH / 3;
                d.xFinish = 2 * SCREEN_WIDTH / 3;
                d.yStart = 2 * SCREEN_HEIGHT / 3 - 10;
                d.yFinish = 2 * SCREEN_HEIGHT / 3;
                userList.push_back(d);

                updateArea(userList);
            }
            if (e.key.keysym.sym == SDLK_i) {
                infoReadOut(true);
            }
            if (CURRENT_ROUTINE != ROUTINE_MANUAL) {
                stop();
                return;
            }

            // Adjust the velocity
            if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_w) {
                mVelocity += 5;
                if (mVelocity > Robat_Max_Velocity) {
                    mVelocity = Robat_Max_Velocity;
                }
            }
            if (e.key.keysym.sym == SDLK_DOWN || e.key.keysym.sym == SDLK_s) {
                mVelocity -= 5;
                if (mVelocity < -1 * Robat_Max_Velocity) {
                    mVelocity = -1 * Robat_Max_Velocity;
                }
            }
            if (e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_a) {
                mOmega -= 5;
                if (mOmega < -1 * Robat_Max_Omega) {
                    mOmega = -1 * Robat_Max_Omega;
                }
            }
            if (e.key.keysym.sym == SDLK_RIGHT || e.key.keysym.sym == SDLK_d) {
                mOmega += 5;
                if (mOmega > Robat_Max_Omega) {
                    mOmega = Robat_Max_Omega;
                }
            }
        }
        // If a key was released
        else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
            // Adjust the velocity
            switch (e.key.keysym.sym) {
                case SDLK_w:
                case SDLK_UP:
                    mVelocity = 0;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    mVelocity = 0;
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    mOmega = 0;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    mOmega = 0;
                    break;
            }
        }
    }

    bool wallToRight() {
        if (userDistanceToCollition[1] <= 32 && userDistanceToCollition[3] <= 32 && userDistanceToCollition[2] <= 24) {
            return userDistanceToCollition[0] <= 16;
        }
        if (userDistanceToCollition[5] <= 32 && userDistanceToCollition[7] <= 32 && userDistanceToCollition[6] <= 24) {
            return userDistanceToCollition[0] <= 16;
        }
        return userDistanceToCollition[1] <= 23 || userDistanceToCollition[7] <= 23 || userDistanceToCollition[0] <= 16;
    }

    bool wallToLeft() {
        if (userDistanceToCollition[1] <= 32 && userDistanceToCollition[3] <= 32 && userDistanceToCollition[2] <= 24) {
            return userDistanceToCollition[4] <= 16;
        }
        if (userDistanceToCollition[5] <= 32 && userDistanceToCollition[7] <= 32 && userDistanceToCollition[6] <= 24) {
            return userDistanceToCollition[4] <= 16;
        }
        return userDistanceToCollition[3] <= 23 || userDistanceToCollition[5] <= 23 || userDistanceToCollition[4] <= 16;
    }

    bool wallAbove() {
        if (userDistanceToCollition[3] <= 32 && userDistanceToCollition[5] <= 32 && userDistanceToCollition[4] <= 24) {
            return userDistanceToCollition[2] <= 16;
        }
        if (userDistanceToCollition[1] <= 32 && userDistanceToCollition[7] <= 32 && userDistanceToCollition[0] <= 24) {
            return userDistanceToCollition[2] <= 16;
        }
        return userDistanceToCollition[1] <= 23 || userDistanceToCollition[3] <= 23 || userDistanceToCollition[2] <= 16;
    }

    bool wallBelow() {
        if (userDistanceToCollition[3] <= 32 && userDistanceToCollition[5] <= 32 && userDistanceToCollition[4] <= 24) {
            return userDistanceToCollition[6] <= 16;
        }
        if (userDistanceToCollition[1] <= 32 && userDistanceToCollition[7] <= 32 && userDistanceToCollition[0] <= 24) {
            return userDistanceToCollition[6] <= 16;
        }
        return userDistanceToCollition[5] <= 23 || userDistanceToCollition[7] <= 23 || userDistanceToCollition[6] <= 16;
    }

    // Moves the Robot
    void move() {
        int xVel = round(mVelocity * sin(mAngle / 180 * M_PI)), yVel = -1 * round(mVelocity * cos(mAngle / 180 * M_PI));
        mAngle = static_cast<int>(round(mAngle + mOmega)) % 360;
        if (mAngle < 0) {
            mAngle = 360 + mAngle;
        }

        atWall = isAtWall();

        if (fowardToCollition() != -1) {
            if (fowardToCollition() < 30 && mVelocity >= 5) {
                mVelocity -= 5;
                atWall = true;
            }
        }
        if (CURRENT_ROUTINE == ROUTINE_MANUAL) {
            if (xVel < 0 && wallToLeft()) {
                xVel = 0;
                atWall = true;
                stop();
            } else if (xVel > 0 && wallToRight()) {
                xVel = 0;
                atWall = true;
                stop();
            }

            if (yVel < 0 && wallAbove()) {
                yVel = 0;
                atWall = true;
                stop();
            } else if (yVel > 0 && wallBelow()) {
                yVel = 0;
                atWall = true;
                stop();
            }
        }

        /*bool one = userDistanceToCollition[3] <=32 && userDistanceToCollition[5] <= 32 && userDistanceToCollition[4] <= 24;
        bool two = userDistanceToCollition[2] <= 16;

        bool three = userDistanceToCollition[1] <= 32 && userDistanceToCollition[7] <= 32 && userDistanceToCollition[0] <= 24;
        bool four = userDistanceToCollition[2] <= 16;


        if(atWall){
                printf("atWall\t%d\t%d\t%d\t%d\n", one, two, three, four);
        }*/

        /*if(mVelocity != 0 || mOmega != 0){
                printf("angle:\t%lf\ncosAngle:\t%lf\nsinAngle:\t%lf\n", mAngle, cos(mAngle/180*M_PI), sin(mAngle/180*M_PI));
                printf("xVelocity:\t%d\nyVelocity:\t%d\n\n", xVel, yVel);
        }*/

        // Move the Robot left or right
        mPosX += xVel;

        // If the Robot went too far to the left or right
        if ((mPosX < 0) || (mPosX + Robot_WIDTH > SCREEN_WIDTH)) {
            // Move back
            mPosX -= xVel;
        }

        // Move the Robot up or down
        mPosY += yVel;

        // If the Robot went too far up or down
        if ((mPosY < 0) || (mPosY + Robot_HEIGHT > SCREEN_HEIGHT)) {
            // Move back
            mPosY -= yVel;
        }
        lastOmega = mOmega;
        lastVelocity = mVelocity;
    }

    bool wallBehind() {
        if (mAngle >= 0 || mAngle < 90) {
            return wallBelow() || wallToLeft();
        }
        if (mAngle >= 90 || mAngle < 180) {
            return wallBelow() || wallToRight();
        }
        if (mAngle >= 180 || mAngle < 270) {
            return wallAbove() || wallToRight();
        }
        if (mAngle >= 270) {
            return wallAbove() || wallToLeft();
        }
    }

    bool wallAhead() {
        return !wallBehind();
    }

    int fowardToCollition() {
        switch (getAngleasI()) {
            case 0:
                return userDistanceToCollition[2];
            case 45:
                return userDistanceToCollition[1];
            case 90:
                return userDistanceToCollition[0];
            case 135:
                return userDistanceToCollition[7];
            case 180:
                return userDistanceToCollition[6];
            case 225:
                return userDistanceToCollition[5];
            case 270:
                return userDistanceToCollition[4];
            case 315:
                return userDistanceToCollition[3];
        }
        return -1;
    }

    bool turnToAngle(int target) {  // Goes clockwise to get to target angel
        if (mVelocity != 0) {
            stop();
            return true;
        }
        int distance;
        target = target % 360;
        if (target == getAngleasI()) {
            distance = 0;
        } else if (target == 0) {
            distance = 360 - getAngleasI();
        } else if (target > getAngleasI()) {
            distance = target - getAngleasI();
        } else if (target < getAngleasI()) {
            distance = target + getAngleasI();
        }

        if (distance == 0) {  // Dis is 0
            if (mOmega == 0) {
                return false;
            } else if (mOmega == 5) {
                mOmega = 0;
                return true;
            } else {
                mOmega = 15;
                return true;
            }
        } else if (distance == 5) {  // Dis is 5
            if (mOmega <= 10) {
                mOmega = 5;
                return true;
            } else {
                mOmega = 15;
                return true;
            }
        } else if (distance == 10) {  // Dis is 10
            if (mOmega == 0 || mOmega == 10) {
                mOmega = 5;
                return true;
            } else if (mOmega == 5 || mOmega == 15) {
                return true;
            }
        } else if (distance == 15) {  // Dis is 15
            if (mOmega == 0) {
                mOmega = 5;
                return true;
            } else if (mOmega == 5) {
                mOmega = 10;
                return true;
            } else if (mOmega == 10) {
                return true;
            } else if (mOmega == 15) {
                mOmega = 10;
                return true;
            }
        } else if (distance == 20) {  // Dis is 20
            if (mOmega == 0) {
                mOmega = 5;
                return true;
            } else if (mOmega >= 5) {
                mOmega = 10;
                return true;
            }
        } else if (distance >= 25) {  // Dis is 25+
            if (mOmega == 0) {
                mOmega = 5;
                return true;
            } else if (mOmega == 5) {
                mOmega = 10;
                return true;
            } else if (mOmega == 10) {
                return true;
            } else if (mOmega == 15) {
                mOmega = 15;
                return true;
            }
        }
    }

    bool pilotToPointX(int xTarget) {
        int angleTarget;
        if ((wallToLeft() || wallToRight()) && mVelocity == 0) {
            return false;
        }
        if (xTarget == mPosX && mVelocity == 0) {
            return false;
        }
        if (xTarget < mPosX) {
            angleTarget = 270;
        } else if (xTarget > mPosX) {
            angleTarget = 90;
        }
        if (turnToAngle(angleTarget)) {
            return true;
        } else {
            int distance = abs(xTarget - mPosX);
            return fowardUnit(distance);
        }
    }

    bool pilotToPoint(int targetX, int yTarget) {
        if (pilotToPointX(targetX)) {
            return true;
        }
        if (yTarget == mPosY && mVelocity == 0) {
            return false;
        }
        int angleTarget;
        if (yTarget < mPosY) {
            angleTarget = 360;
        } else if (yTarget > mPosY) {
            angleTarget = 180;
        }
        if (turnToAngle(angleTarget)) {
            return true;
        } else {
            int distance = abs(yTarget - mPosY);
            return fowardUnit(distance);
        }
    }

    // Shows the Robot on the screen
    void render() {
        // Show the Robot
        gRobotTexture.render(mPosX, mPosY, NULL, mAngle);
    }

   private:
    // The X and Y offsets of the Robot
    int mPosX, mPosY;

    // The velocity of the Robot
    int mVelocity;

    // Rotational information
    double mAngle;
    double mOmega;

    int lastOmega, lastVelocity;

    bool atWall;
};

class LButton {
   public:
    LButton() {
        mPosition.x = 0;
        mPosition.y = 0;

        mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
    }

    void setPosition(int x, int y) {
        mPosition.x = x;
        mPosition.y = y;
    }

    void handleEvent(SDL_Event* e) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        bool inside = true;

        if (x < mPosition.x) {
            inside = false;
        }
        if (x > mPosition.x + BUTTON_WIDTH) {
            inside = false;
        } else if (y < mPosition.y) {
            inside = false;
        } else if (y > mPosition.y + BUTTON_HEIGHT) {
            inside = false;
        }

        if (!inside) {
            mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
        } else {
            switch (e->type) {
                case SDL_MOUSEMOTION:
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
                    break;
                case SDL_MOUSEBUTTONUP:
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
                    break;
            }
        }
    }

    void render() {
        gButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gSpriteClips[mCurrentSprite]);
    }

   private:
    SDL_Point mPosition;

    LButtonSprite mCurrentSprite;
};

bool loadImage(SDL_Surface*& target, string path) {
    if (target != NULL) {
        EXITONERROR("Surface Already in use!!");
    }

    target = SDL_LoadBMP(path.c_str());

    if (target == NULL) {
        printf("Unable to load image %s!! SDL Error: %s\n", path, SDL_GetError());
        return false;
    }

    return true;
}

SDL_Texture* loadTexture(string path) {
    SDL_Texture* toReturn = NULL;

    SDL_Surface* surfaceToLoad = IMG_Load(path.c_str());

    if (surfaceToLoad == NULL) {
        printf("Unable to load image %s!! SDL Error: %s\n", path, SDL_GetError());
        EXITONERROR();
    }

    SDL_FreeSurface(surfaceToLoad);

    return toReturn;
}

void initiliseWindow(SDL_Window*& window) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL-Error: %s\n", SDL_GetError());
        EXITONERROR();
    }

    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        printf("Linear Texture filtering is NOT enabled!\n");
    }

    window = SDL_CreateWindow("Service Ball Retriver", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("Window was not created! SDL_Error: %s\n", SDL_GetError());
        EXITONERROR();
    }

    // V-Sync render
    gRender = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (gRender == NULL) {
        printf("Renderer could not be creaed! SDL Error: %s\n", SDL_GetError());
        EXITONERROR();
    }

    SDL_SetRenderDrawColor(gRender, 0xFF, 0xFF, 0xFF, 0xFF);

    int imgFlags = IMG_INIT_PNG;
    if (!IMG_Init(imgFlags) & !imgFlags) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        EXITONERROR();
    }
}

LButton gButtons[4];

void drawAndRenderRectangle(rectPoints* rectOutline) {
    if (rectOutline->xStart == -1 || rectOutline->yStart == -1 || rectOutline->xFinish == -1 || rectOutline->yFinish == -1) {
        // printf("Failed to create rectangle, had uninilised points!\n");
        return;
    }
    SDL_Rect fillRect = {rectOutline->xStart, rectOutline->yStart, rectOutline->xFinish - rectOutline->xStart, rectOutline->yFinish - rectOutline->yStart};
    SDL_SetRenderDrawColor(gRender, rectOutline->R, rectOutline->G, rectOutline->B, rectOutline->Alpha);
    SDL_RenderFillRect(gRender, &fillRect);
}

void printRect(rectPoints* rectOutline) {
    printf("Starting X: %d\tY: %d\nFinal X: %d\t Y: %d\n", rectOutline->xStart, rectOutline->yStart, rectOutline->xFinish, rectOutline->yFinish);
    printf("Colors R: %d\tG: %d\tB: %d\tAlpha: %d\n", rectOutline->R, rectOutline->G, rectOutline->B, rectOutline->Alpha);
}

void drive(robot* myRover, GPIO_PINSTACK) {
    while (true) {
        int currentSpeed = myRover.getVelocity() int currentOmega = myRover.getOmega() if (currentSpeed > 0) {
            GPIO_PINSTACK.activate
        }
        if (currentOmega > 0) {
            GPIO_PINSTACK.activate
        }
    }
}

int main(/*int argc, char* argv[]*/) {
    // Muliprogramming Open other files
    int pid = fork();
    if (pid == 0) {
        execl("./sensor", "1", "ultraSound");
    }
    pid = fork();
    if (pid == 0) {
        execl("./sensor", "2", "ultraSound");
    }
    pid = fork();
    if (pid == 0) {
        execl("./sensor", "3", "ultraSound");
    }
    pid = fork();
    if (pid == 0) {
        execl("./sensor", "4", "ultraSound");
    }
    pid = fork();
    if (pid == 0) {
        execl("./sensor", "5", "ultraSound");
    }

    CURRENT_ROUTINE = ROUTINE_MANUAL;

    updateArea(userList, 0);

    robot ballRetriver = robot();

    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    SDL_Surface* img = NULL;

    userList = vector<rectPoints>();
    discoverList = vector<rectPoints>();

    int disFirstWallx = -1, disFirstWally = -1, disFirstWall = 0;

    int discoverLowX = SCREEN_WIDTH, discoverLowY = SCREEN_HEIGHT, discoverHighX = -1, discoverHighY = -1, discoverAngle = 0, corners = 0;

    int sweepCount = -1, sweepX;

    int bounceLastAngle = -1;

    int movmentCount = 0;

    int bounceMoveCount = 0;

    string userInput;

    for (size_t i = 0; i < 8; i++) {
        userDistanceToCollition[i] = 0;
        discoverDistanceToCollition[i] = 0;
    }

    int framesScinceUpdate = 0;

    initiliseWindow(window);

    screenSurface = SDL_GetWindowSurface(window);

    if (screenSurface == NULL) {
        printf("Unable to get surface! SDL Error: %s\n", SDL_GetError());
        EXITONERROR();
    }

    // Open basic window and fill black
    // SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
    // SDL_UpdateWindowSurface(window);

    // Test to load image
    /*loadImage(img, "image/default.bmp");
    SDL_BlitSurface(img, NULL, screenSurface, NULL);
    SDL_UpdateWindowSurface(window);*/

    // Loading and Setting up Buttons
    /*
    if(!gButtonSpriteSheetTexture.loadFromFile("image/button.png")){
                    printf("Failed to load button sprite texture!\n");
                    EXITONERROR();
    } else {
            for( int i = 0; i < BUTTON_SPRITE_TOTAL; ++i ){
                    gSpriteClips[ i ].x = 0;
                    gSpriteClips[ i ].y = i * 200;
                    gSpriteClips[ i ].w = BUTTON_WIDTH;
                    gSpriteClips[ i ].h = BUTTON_HEIGHT;
            }

            //Set buttons in corners
            gButtons[ 0 ].setPosition( 0, 0 );
            gButtons[ 1 ].setPosition( SCREEN_WIDTH - BUTTON_WIDTH, 0 );
            gButtons[ 2 ].setPosition( 0, SCREEN_HEIGHT - BUTTON_HEIGHT );
            gButtons[ 3 ].setPosition( SCREEN_WIDTH - BUTTON_WIDTH, SCREEN_HEIGHT - BUTTON_HEIGHT );
    }//*/

    if (!gRobotTexture.loadFromFile("image/robot_text.bmp")) {
        printf("Failed to load texture!\n");
        EXITONERROR();
    }
    // main running loop
    SDL_Event e;
    bool end = false;
    while (!end) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                end = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                rectPoints toAdd = rectPoints();
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (x + wiggleRoom >= SCREEN_WIDTH) {
                    x = SCREEN_WIDTH - (wiggleRoom + 1);
                } else if (x - wiggleRoom <= 0) {
                    x = (wiggleRoom + 1);
                }
                if (y + wiggleRoom >= SCREEN_HEIGHT) {
                    y = SCREEN_HEIGHT - (wiggleRoom + 1);
                } else if (y - wiggleRoom <= 0) {
                    y = (wiggleRoom + 1);
                }
                toAdd.xStart = x;
                toAdd.yStart = y;
                userList.push_back(toAdd);
                // printRect(&toAdd);
                userRect++;
            }
            if (e.type == SDL_MOUSEBUTTONUP) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                int startX = userList.at(userRect - 1).xStart, startY = userList.at(userRect - 1).yStart;

                // Start is always to left and down
                if (x < startX) {
                    userList.at(userRect - 1).xStart = x;
                    x = startX;
                    startX = userList.at(userRect - 1).xStart;
                }
                if (y < startY) {
                    userList.at(userRect - 1).yStart = y;
                    y = startY;
                    startY = userList.at(userRect - 1).yStart;
                }

                // Heighth and Width are always at least wiggle
                if (x - wiggleRoom < startX) {
                    x = startX + wiggleRoom;
                }
                if (y - wiggleRoom < startY) {
                    y = startY + wiggleRoom;
                }

                // Save and Scribe Rectangles
                userList.at(userRect - 1).xFinish = x;
                userList.at(userRect - 1).yFinish = y;
                if (userRect < 2) {
                    updateArea(userList);
                }
                updateArea(userList, userRect - 2);
                printf("Added Recktangle, Updating area...\n");
                // drawAndRenderRectangle(&(userList.at(userRect-1)));
                // printRect(&(userList.at(userRect-1)));
            }

            // manual robat handeling
            ballRetriver.handleEvent(e);

            // Buttons Handeling
            /*
            for( int i = 0; i < 4; ++i ) {
                    gButtons[ i ].handleEvent( &e );
            }*/
        }

        if (CURRENT_ROUTINE == ROUTINE_DISCOVERY) {
            int curX = ballRetriver.getXpos();
            int curY = ballRetriver.getYpos();
            if (curX < discoverLowX) {
                discoverLowX = curX;
            } else if (curX > discoverHighX) {
                discoverHighX = curX;
            }
            if (curY < discoverLowY) {
                discoverLowY = curY;
            } else if (curY > discoverHighY) {
                discoverHighY = curY;
            }

            if (!ballRetriver.turnToAngle(discoverAngle)) {
                if (!ballRetriver.fowardUnit(ballRetriver.fowardToCollition(), 30)) {
                    discoverAngle += 90;
                    corners++;
                }
            }

            if (discoverAngle >= 360) {
                CURRENT_ROUTINE == ROUTINE_SWEEP;
                // printf("Discovery Finshed Now entering Sweep Mode!\n");
                // printf("Found Values\tHigh: (%d,%d)\tLow: (%d,%d)\n", discoverHighX, discoverHighY, discoverLowX, discoverLowY);
            }

            if (corners >= 4) {
                // e.key.keysym.sym
                CURRENT_ROUTINE == ROUTINE_SWEEP;
                printf("Discovery Completed!\n");
                sleep(5);
                // cin >> userInput;
            }

            /*if(!ballRetriver.isStopped()){
                    ballRetriver.stop();
            } else if(static_cast<int>(ballRetriver.getAngle()) %90 != 0){
                    if(disFirstWall == 0){
                            disFirstWallx = ballRetriver.getXpos();
                            disFirstWally = ballRetriver.getYpos();
                            disFirstWall = 1;
                    }
                    ballRetriver.turn(true);
            } else if(ballRetriver.isAtWall()) {
                    ballRetriver.backword(true);
                    ballRetriver.turn(true);
            } else {
                    ballRetriver.foward(true);
                    if(disFirstWallx == ballRetriver.getXpos() && disFirstWally == ballRetriver.getYpos()
                    && ballRetriver.getOmega() == 0 && static_cast<int>(ballRetriver.getAngle()) % 45 == 0){
                            if(disFirstWall++ >= 2){
                                    printf("Returned to first wall now sweeping!\n");
                                    CURRENT_ROUTINE = ROUTINE_SWEEP;
                                    initalAngle = ballRetriver.getAngle();
                            }
                    }
            }*/
        }

        if (CURRENT_ROUTINE == ROUTINE_SWEEP) {
            if (sweepCount == -1) {
                if (!ballRetriver.pilotToPoint(discoverLowX, discoverLowY)) {
                    printf("One Sweep Complete!\n");
                    sweepCount++;
                    sweepX = discoverLowX;
                }
            } else {
                if (sweepCount % 2 == 0) {
                    if (!ballRetriver.pilotToPointX(sweepX)) {
                        sweepCount++;
                        printf("Sweep Increaesed: %d\n", sweepCount);
                        sweepX += 20;
                        printf("Now heading for (%d,%d)", ballRetriver.getXpos(), ballRetriver.getYpos() - 20);
                    }
                } else {
                    if (sweepX >= discoverHighX) {
                        sweepX = discoverLowX;
                    } else {
                        if (!ballRetriver.pilotToPoint(ballRetriver.getXpos(), ballRetriver.getYpos() - 20)) {
                            sweepCount++;
                            printf("Sweep Increaesed: %d\n", sweepCount);
                            printf("Now heading for (%d,%d)", sweepX, ballRetriver.getYpos());
                        }
                    }
                }
            }
        }

        if (CURRENT_ROUTINE == ROUTINE_BOUNCE) {
            /*if(!ballRetriver.pilotToPoint(SCREEN_WIDTH/4, SCREEN_HEIGHT/4)){
                    printf("Arrived!\n");
            }*/
            if (bounceLastAngle == -1) {
                bounceLastAngle = 45;
            }
            if (!ballRetriver.turnToAngle(bounceLastAngle)) {
                if (!ballRetriver.fowardUnit(ballRetriver.fowardToCollition(), 20)) {
                    // bounceLastAngle = (bounceLastAngle + 90)%360;
                }
                if (ballRetriver.getVelocity() == 0 && ballRetriver.getOmega() == 0) {
                    // ballRetriver.stop();
                    bounceLastAngle = (bounceLastAngle + 45 * (rand() % 3 + 1)) % 360;
                }
            }

            /*if(!ballRetriver.pilotToPoint(ballRetriver.getXpos()-100, ballRetriver.getYpos()-100)){
                    printf("I am here!\n");
            }
            /*if(ballRetriver.isAtWall()) {
                    ballRetriver.backword(true);
                    ballRetriver.turn(true);
                    //bounceMoveCount = 1;
            } else {
                    ballRetriver.stop();
                    ballRetriver.foward(true);
            }
            if(ballRetriver.wallBehind()){
                    ballRetriver.stop();
                    ballRetriver.foward(true);
            }
            /*else if (bounceMoveCount == 1) {
                    bounceMoveCount = 0;
                    ballRetriver.stop();
                    ballRetriver.backword(true);
            } else {
                    if(ballRetriver.getOmega() != 0){
                            ballRetriver.stop();
                    }
                    ballRetriver.foward(true);
            }*/
        }

        ballRetriver.move();

        // clears screan
        SDL_SetRenderDrawColor(gRender, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRender);

        ballRetriver.render();

        // Render buttons
        /*
        for( int i = 0; i < 4; ++i ) {
                gButtons[i].render();
        }
        */

        // Render all rectangles
        for (size_t i = 0; i < userRect; i++) {
            drawAndRenderRectangle(&(userList.at(i)));
        }

        for (size_t i = 0; i < discoverdRect; i++) {
            drawAndRenderRectangle(&(discoverList.at(i)));
        }

        /*framesScinceUpdate++;
        if(framesScinceUpdate >= OUTSTANDING_UPDATE){
                printf("Update\n");
                updateArea(userList);
                framesScinceUpdate = 0;
        }*/

        // Render all rays
        if (virtualRays) {
            for (size_t i = 0; i < 8; i++) {
                userDistanceToCollition[i] = 0;
            }

            SDL_SetRenderDrawColor(gRender, 0xFF, 0x00, 0xFF, 0xFF);
            int robXpos = ballRetriver.getXpos() + ballRetriver.Robot_WIDTH / 2, robYpos = ballRetriver.getYpos() + ballRetriver.Robot_HEIGHT / 2;
            for (int j = robYpos; j < SCREEN_HEIGHT; j += (wiggleRoom - 1)) {
                if (badArea[robXpos][j]) {
                    break;
                }
                SDL_RenderDrawPoint(gRender, robXpos, j);
                userDistanceToCollition[6] += 4;  // 6 = Down
            }
            for (int j = robYpos; j > 0; j -= (wiggleRoom - 1)) {
                if (badArea[robXpos][j]) {
                    break;
                }
                SDL_RenderDrawPoint(gRender, robXpos, j);
                userDistanceToCollition[2] += 4;  // 2 = Up
            }
            for (int j = robXpos; j < SCREEN_WIDTH; j += (wiggleRoom - 1)) {
                if (badArea[j][robYpos]) {
                    break;
                }
                SDL_RenderDrawPoint(gRender, j, robYpos);
                userDistanceToCollition[0] += 4;  // 0 = Right
            }
            for (int j = robXpos; j > 0; j -= (wiggleRoom - 1)) {
                if (badArea[j][robYpos]) {
                    break;
                }
                SDL_RenderDrawPoint(gRender, j, robYpos);
                userDistanceToCollition[4] += 4;  // 4 = Left
            }
            int xRay = robXpos, yRay = robYpos;
            while (xRay + (wiggleRoom - 1) < SCREEN_WIDTH && yRay + (wiggleRoom - 1) < SCREEN_HEIGHT) {
                xRay += (wiggleRoom - 1);
                yRay += (wiggleRoom - 1);
                if (badArea[xRay][yRay]) {
                    break;
                }
                SDL_RenderDrawPoint(gRender, xRay, yRay);
                userDistanceToCollition[7] += 4;  // 7 = Down & Right
            }
            xRay = robXpos;
            yRay = robYpos;
            while (xRay - (wiggleRoom - 1) > 0 && yRay - (wiggleRoom - 1) > 0) {
                xRay -= (wiggleRoom - 1);
                yRay -= (wiggleRoom - 1);
                if (badArea[xRay][yRay]) {
                    break;
                }
                SDL_RenderDrawPoint(gRender, xRay, yRay);
                userDistanceToCollition[3] += 4;  // 3  = Up & Left
            }
            xRay = robXpos;
            yRay = robYpos;
            while (xRay - (wiggleRoom - 1) > 0 && yRay + (wiggleRoom - 1) < SCREEN_HEIGHT) {
                xRay -= (wiggleRoom - 1);
                yRay += (wiggleRoom - 1);
                if (badArea[xRay][yRay]) {
                    break;
                }
                SDL_RenderDrawPoint(gRender, xRay, yRay);
                userDistanceToCollition[5] += 4;  // 5 = Down & Left
            }
            xRay = robXpos;
            yRay = robYpos;
            while (xRay + (wiggleRoom - 1) < SCREEN_WIDTH && yRay - (wiggleRoom - 1) > 0) {
                xRay += (wiggleRoom - 1);
                yRay -= (wiggleRoom - 1);
                if (badArea[xRay][yRay]) {
                    break;
                }
                SDL_RenderDrawPoint(gRender, xRay, yRay);
                userDistanceToCollition[1] += 4;  // 1 = Up & Right
            }
        }

        // int x, y;
        // SDL_GetMouseState(&x,&y);
        // printf("Mouse pos: %d, %d\n", x, y);

        // Update screen
        SDL_RenderPresent(gRender);

        // Drawing Shapes
        /*
        //Render Main Red Shape
        SDL_Rect fillRect = {SCREEN_WIDTH/4, SCREEN_HEIGHT/4, SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
        SDL_SetRenderDrawColor(gRender, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(gRender, &fillRect);
        SDL_RenderPresent(gRender);
        sleep(2);
        //Render Green outline
        SDL_Rect outlineRect = {SCREEN_WIDTH/6, SCREEN_HEIGHT/6, SCREEN_WIDTH*2/3, SCREEN_HEIGHT*2/3};
        SDL_SetRenderDrawColor(gRender, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderDrawRect(gRender, &outlineRect);
        SDL_RenderPresent(gRender);
        sleep(2);
        //Draw Blue Horizontal Line
        SDL_SetRenderDrawColor(gRender, 0x00, 0x00, 0xFF, 0xFF);
        SDL_RenderDrawLine(gRender, 0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2);
        SDL_RenderPresent(gRender);
        sleep(2);
        //Draw Yellow Vertical Dots
        SDL_SetRenderDrawColor(gRender, 0xFF, 0xFF, 0x00, 0xFF);
        for(int i = 0; i < SCREEN_HEIGHT; i+=4){
                SDL_RenderDrawPoint(gRender, SCREEN_WIDTH/2, i);
        }
        SDL_RenderPresent(gRender);
        sleep(2);
        end=true;
        printf("Ending!\n");
        sleep(10);*/
    }

    discoverList.empty();
    userList.empty();

    // Close and Destroy
    SDL_DestroyRenderer(gRender);
    SDL_FreeSurface(img);
    SDL_FreeSurface(screenSurface);
    SDL_DestroyWindow(window);

    window = NULL;
    gRender = NULL;

    IMG_Quit();
    SDL_Quit();

    return 0;

    // SHM Test

    // Muliprogramming Open other files
    int pid = fork();
    if (pid == 0) {
        execl("./sensor", "", nullptr);
    }
    pid = fork();
    if (pid == 0) {
        execl("./website", "", nullptr);
    }
    printf("I am Brain\n");

    // Tool for Forcing syncronation | ONLY FOR TESTING PURPOSESES
    SHMRequestChannel* wait = process_newSending_request("waiting");
    char* emp = new char[1];
    int errors = 0;

    // 1
    cin >> emp;
    wait->cwrite(emp, 1);

    // Testing For Threading and shared memory
    SHMRequestChannel* chan = new SHMRequestChannel("control", RECIVING_SIDE, MAX_MESSAGE);

    printf("Brain: Have created receiver\n");

    char* buffer = new char[sizeof(ultraSonicData)];
    chan->cread(buffer, sizeof(ultraSonicData));

    ultraSonicData* m = (ultraSonicData*)buffer;
    ultraSonicData* mTest = new ultraSonicData(5, 7.19);

    printf("\nBrain: Reading data now\n");

    printUltraSonicdata(m);

    errors = m->sensor != mTest->sensor || !nonpreciseEquals(m->value, mTest->value, 5) ? 1 : 0;
    if (errors != 0) {
        printf("Brain: Test failed!\n");
    } else {
        printf("Brain: Test Passed!\n");
    }
    errors = 0;

    // 2
    cin >> emp;
    wait->cwrite(emp, 1);

    printf("Brain: Strating many readings\n");

    int sensor = 0;
    double value = 0.001;

    delete mTest;
    mTest = new ultraSonicData(sensor, value);

    errors = 0;

    for (size_t i = 0; i < 1000; i++) {
        chan->cread(buffer, sizeof(ultraSonicData));
        m = (ultraSonicData*)buffer;
        delete mTest;
        mTest = new ultraSonicData(sensor, value);
        if (m->sensor != mTest->sensor || m->value != mTest->value) {
            errors++;
            printf("Brain: Inconsistancy!!\nOld Data:\n");
            printUltraSonicdata(mTest);

            printf("Brain: New Data:\n");
            printUltraSonicdata(m);
        }
        sensor++;
        value += 1.02;
    }

    if (errors == 0) {
        printf("Brain: Test Passed No issues one thread many point readings\n\n");
    } else {
        printf("Brain: Test Failed!\n");
    }

    errors = 0;

    // 3
    cin >> emp;
    wait->cwrite(emp, 1);

    printf("Brain: Now testing multi Thread funcions\n");

    read_request(chan, buffer);
    delete mTest;
    mTest = new ultraSonicData(43, 2.09723);
    m = (ultraSonicData*)buffer;

    printf("Brain: Read value:\n");
    printUltraSonicdata(m);

    errors = m->sensor != mTest->sensor || !nonpreciseEquals(m->value, mTest->value, 5) ? 1 : 0;
    if (errors != 0) {
        printf("Brain: value reed wrong!\n");
    } else {
        printf("Brain: Test Passed, Data Read Correctly from thread function!\n\n");
    }

    // 4
    cin >> emp;
    wait->cwrite(emp, 1);

    errors = 0;

    printf("\nBrain: Attempting to Reading too much now for blocking!\n");
    read_request(chan, buffer);
    delete mTest;
    mTest = new ultraSonicData(7, 8.9);
    m = (ultraSonicData*)buffer;
    printUltraSonicdata(m);

    errors = m->sensor != mTest->sensor || !nonpreciseEquals(m->value, mTest->value, 5) ? 1 : 0;
    if (errors != 0) {
        printf("Brain: Test failed!\n");
    } else {
        printf("Brain: Test Passed!\n");
    }
    errors = 0;

    // 5
    cin >> emp;
    wait->cwrite(emp, 1);

    printf("Brain: Now demonstrating full multithread style!\n");
    char* multiBuf = new char[sizeof(ultraSonicData)];
    SHMRequestChannel* multi = process_newReciving_request("MultiThread", multiBuf);
    printf("\n\nBrain: Detached thread allows to continue without a block\n");
    cout << "Brain: Counting to show non blocking\n";

    for (size_t i = 0; i < 5; i++) {
        cout << i << " ";
        cout.flush();
        sleep(1);
    }

    cout << "\nBrain: Current buffer is empty:\t" << multiBuf << endl;
    cout << "Brain: Now wait for time pass...\n\n";
    sleep(3);
    printf("Brain: Now showing update to buffer after waiting:\t");
    cout << multiBuf << endl;
    m = (ultraSonicData*)multiBuf;
    printUltraSonicdata(m);
    delete mTest;
    mTest = new ultraSonicData(13, 8.221);

    errors = m->sensor != mTest->sensor || !nonpreciseEquals(m->value, mTest->value, 5) ? 1 : 0;
    if (errors != 0) {
        printf("Brain: Test failed!\n");
    } else {
        printf("Brain: Test Passed!\n");
    }

    // 6
    cin >> emp;
    wait->cwrite(emp, 1);

    delete[] buffer;
    delete[] emp;
    delete mTest;
    delete chan;
    delete wait;

    return 0;  //*/
}
