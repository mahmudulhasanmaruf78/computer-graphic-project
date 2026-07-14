
#include <cmath>
#include <vector>
#include <math.h>
#include <GL/glut.h>
#include <windows.h>
#include <mmsystem.h>
#define PI 3.14159265358979323846
///////////////////////////////////////////
bool showExplosion = false;
float explosionTime = 0.0f;
float ZtakeoffThresholdX = 0.45f;
float bombThresholdX = 0.45f;
//cloudOffset
float ZplaneX = -0.89f;
float ZplaneY = 0.04f;
bool ZtakeoffStarted = false;
bool bombStarted = false;
float bombX = -0.89f;
float bombY = 0.04f;
float scX = 1.0f;
float scY = 1.0f;
float trY = 0.0f;
//////////////////////////////////////////
struct Vec2 { float x, y; };
bool showInstruction = false;
float takeoffThresholdX = 0.7f;
float planeX = 0.0f;
float planeY = -0.4f;
float cloudOffset = 0.0f;
bool takeoffStarted = false;
bool showInformation = true;
int currentScene = 1;
/////////////////////////////////////////
GLfloat planePosX = 0.0f;
GLfloat planeSpeed = 0.004f;
float flameSize = 0.05f;
bool flameGrowing = true;
bool mtakeoffStarted = false;
float mplaneY = -0.8f;
float cloudOffsetx = 0.0f;
float cloudSpeed = 0.002f;
const float cloudWrapLimit = 2.0f;
bool cloudMoving = true;
////////////////////////////////////////
float aplaneX = -1.2f;
bool animate = false;
bool missileActive = false;
float missileYOffset = 0.0f;
bool timerRunning = false;
float speed = 0.01f;
float missileSpeed = 0.02f;
////////////////////////////////////////
float waveOffset = 0.0f;
float cloudOffset1 = 0.0f;
float cloudOffset2 = -0.5f;
bool isNightTransition = false;
//--- Fighter Jet Animation Control ---
float jetPosX = 2.0f;
float jetPosY = 1.2f;
float jetAngle = 0.0f;
float jetSpeed = 0.0f;
int jetLandingState = 0;
float landingApproachStartPosX = 0.0f;
float landingApproachStartPosY = 0.0f;
float landingTargetX = -0.3f;
float landingTargetY = -0.075f;
float landingCurrentTime = 0.0f;
float landingDuration = 150.0f;
float jetParkedX = 0.2f;
// --- Day/Night Cycle ---
float skyRed = 0.4f, skyGreen = 0.7f, skyBlue = 1.0f;
float sunAlpha = 1.0f; // For fading the sun
float starAlpha = 0.0f; // For fading in stars
float lightGlow = 0.0f; // For Helicarrier lights
float sunMoonAngle = 90.0f; // Start with the sun at the top
float searchlightAngle = 0.0f;
///////////////////////////////

void renderBitmapString(float x, float y, float z, void* font, const char* string) {

    glRasterPos3f(x, y, z);
    for (const char* c = string; *c != '\0'; ++c) {
        glutBitmapCharacter(font, *c);
    }
}

void instructionPage() {
    const char* labels[] = {
        "1-5", "T", "R", "U", "B", "I", "M", "Z", "X", "N", "D", "ESC"
    };

    const char* descriptions[] = {
        ": Switch between scenes",
        ": Initiate takeoff",
        ": Reset automatic plane",
        ": Toggle manual takeoff",
        ": Reset manual plane",
        ": Show information screen",
        ": Show instruction manual",
        ": Start bombing sequence",
        ": Return to previous position",
        ": Enable night mode",
        ": Enable day mode",
        ": Exit the application"
    };

    glColor3f(0, 0, 0);
    float y = 0.8f;
    float gap = 0.08f;
    float startY = 0.8f;

    for (int i = 0; i < 12; ++i) {
        glRasterPos2f(-0.9f, startY - i * 0.1f);
        for (int j = 0; labels[i][j]; ++j)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, labels[i][j]);

        glRasterPos2f(-0.6f, startY - i * 0.1f); // aligned second column
        for (int j = 0; descriptions[i][j]; ++j)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, descriptions[i][j]);
    }

}

void update(int value) {

     timerRunning = true;


    if (animate) {
        aplaneX += speed;
        if (aplaneX > 1.9f) {
            animate = false;
            aplaneX = -1.2f;  // Reset for next fly
        }
    }

    if (missileActive) {
        missileYOffset += missileSpeed;
        if (missileYOffset > 1.2f) {
            missileActive = false;
            missileYOffset = 0.0f;
        }
    }

    // Keep timer alive if any animation active
    if (animate || missileActive) {
        glutTimerFunc(16, update, 0);  // ~60 FPS
    } else {
        timerRunning = false;
    }
    // --- Main plane takeoff ---
    if (takeoffStarted) {
        if (planeX < takeoffThresholdX) {
            planeX += 0.01f;
        } else {
            planeX += 0.01f;
            planeY += 0.005f;
        }

        if (planeY > 0.5f || planeX > 1.9f) {
            takeoffStarted = false;
        }
    }

    // --- Missile plane (2nd plane) animation ---
    if (mtakeoffStarted && planePosX <= 2.0f) {
        planePosX += planeSpeed;
    }
    if (cloudMoving) {
        cloudOffsetx += cloudSpeed;
        if (cloudOffsetx > 20.0f) {
            cloudOffsetx -= 20.0f;
        }
    }

    // --- Cloud animation ---
    cloudOffset += 0.0003f;
    if (cloudOffset > 2.0f) cloudOffset = -2.0f;

    //cloudOffsetx += cloudSpeed;
    //if (cloudOffsetx > 2.0f) cloudOffsetx -= 4.0f;

    cloudOffset1 += 0.001f;
    cloudOffset2 += 0.002f;

    // --- Wave animation ---
    waveOffset += 0.02f;

    // --- Searchlight animation ---
    searchlightAngle += 0.5f;
    if (searchlightAngle > 360.0f) {
        searchlightAngle -= 360.0f;
    }

    // --- Jet landing animation ---
    switch (jetLandingState) {
        case 1: {
            landingCurrentTime += 1.0f;
            if (landingCurrentTime > landingDuration)
                landingCurrentTime = landingDuration;

            float t = landingCurrentTime / landingDuration;
            float oneMinusT = 1.0f - t;

            float control1X = -0.8f, control1Y = 0.3f;
            float control2X = -0.4f, control2Y = -0.10f;

            float newPosX = oneMinusT * oneMinusT * oneMinusT * landingApproachStartPosX +
                            3 * oneMinusT * oneMinusT * t * control1X +
                            3 * oneMinusT * t * t * control2X +
                            t * t * t * landingTargetX;

            float newPosY = oneMinusT * oneMinusT * oneMinusT * landingApproachStartPosY +
                            3 * oneMinusT * oneMinusT * t * control1Y +
                            3 * oneMinusT * t * t * control2Y +
                            t * t * t * landingTargetY;

            float tangentX = 3 * oneMinusT * oneMinusT * (control1X - landingApproachStartPosX) +
                             6 * oneMinusT * t * (control2X - control1X) +
                             3 * t * t * (landingTargetX - control2X);

            float tangentY = 3 * oneMinusT * oneMinusT * (control1Y - landingApproachStartPosY) +
                             6 * oneMinusT * t * (control2Y - control1Y) +
                             3 * t * t * (landingTargetY - control2Y);

            jetPosX = newPosX;
            jetPosY = newPosY;
            jetAngle = atan2(tangentY, tangentX) * 180.0f / PI;

            if (landingCurrentTime >= landingDuration) {
                jetLandingState = 2;
                jetPosX = landingTargetX;
                jetPosY = landingTargetY;
                jetAngle = 0.0f;
                jetSpeed = 0.015f;
            }
        }
        break;

        case 2: {
            jetPosX += jetSpeed;
            jetSpeed *= 0.985f;

            if (jetPosX >= jetParkedX) {
                jetLandingState = 0;
                jetSpeed = 0.0f;
                jetPosX = jetParkedX;
            }
        }
        break;
    }

    // --- Day/Night transition ---
    if (isNightTransition) {
        if (skyGreen > 0.2f) skyGreen -= 0.001f;
        if (skyRed > 0.05f) skyRed -= 0.001f;
        if (skyBlue > 0.15f) skyBlue -= 0.002f;
        if (sunAlpha > 0.0f) sunAlpha -= 0.005f;
        if (starAlpha < 1.0f) starAlpha += 0.005f;
        if (lightGlow < 1.0f) lightGlow += 0.005f;
    } else {
        if (skyGreen < 0.7f) skyGreen += 0.001f;
        if (skyRed < 0.4f) skyRed += 0.001f;
        if (skyBlue < 1.0f) skyBlue += 0.002f;
        if (sunAlpha < 1.0f) sunAlpha += 0.005f;
        if (starAlpha > 0.0f) starAlpha -= 0.005f;
        if (lightGlow > 0.0f) lightGlow -= 0.005f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void information(void) {
    struct Member {
        const char* id;
        const char* name;
        const char* contribution;
    };

    Member members[] = {
        {"22-49176-3", "MD.REDUONE AHMED", "20"},
        {"22-48580-3", "MAHMUDUL HASAN MARUF", "20"},
        {"22-48582-3", "MD.MARUF HASAN CHOWDHURY", "20"},
        {"22-47127-1", "SHAH MUSTAIM ANIK", "20"},
        {"22-49359-3", "MD.SAKIB HOSSAN SAMAD", "20"}
    };

    glClearColor(0.95f, 0.95f, 0.95f, 1.0f); // Light gray background
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Header
    glColor3f(0.2f, 0.1f, 0.6f); // Dark Blue
    renderBitmapString(-0.30f, 0.9f, 0.0, GLUT_BITMAP_HELVETICA_18, " Project Title: Bombings on Israel");
    glColor3f(0.0f, 0.0f, 0.0f);
    renderBitmapString(-0.20f, 0.83f, 0.0, GLUT_BITMAP_HELVETICA_18, "Computer Graphics [C]");
    renderBitmapString(-0.85f, 0.75f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "Group Members");

    // Header Bar
    glColor3f(0.0f, 0.0f, 0.0f); glBegin(GL_QUADS);
        glVertex2f(-0.9f, 0.70f); glVertex2f(0.9f, 0.70f);
        glVertex2f(0.9f, 0.63f); glVertex2f(-0.9f, 0.63f);
    glEnd();

    // Header Text
    glColor3f(1.0f, 1.0f, 1.0f); // White
    renderBitmapString(-0.78f, 0.645f, 0.0f, GLUT_BITMAP_HELVETICA_18, "ID");
    renderBitmapString(-0.15f, 0.645f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Name");
    renderBitmapString(0.6f, 0.645f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Contribution");

    // Member Rows
    float y = 0.58f;
    for (int i = 0; i < 5; ++i) {
        glColor3f(0.0f, 0.0f, 0.0f); // Black text
        renderBitmapString(-0.78f, y, 0.0f, GLUT_BITMAP_HELVETICA_18, members[i].id);
        renderBitmapString(-0.15f, y, 0.0f, GLUT_BITMAP_HELVETICA_18, members[i].name);
        renderBitmapString(0.6f, y, 0.0f, GLUT_BITMAP_HELVETICA_18, members[i].contribution);

        // Row Line
        glColor3f(0.7f, 0.7f, 0.7f);
        glBegin(GL_LINES);
            glVertex2f(-0.9f, y - 0.02f); glVertex2f(0.9f, y - 0.02f);
        glEnd();
        y -= 0.1f;
    }

    // Supervised by Section
    glColor3f(0.3f, 0.3f, 0.3f);
    renderBitmapString(-0.23f, -0.05f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Supervised by:");
    glColor3f(0.0f, 0.0f, 0.0f);
    renderBitmapString(-0.27f, -0.12f, 0.0f, GLUT_BITMAP_HELVETICA_18, "MAHFUJUR RAHMAN");
    renderBitmapString(-0.25f, -0.18f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Assistant Professor");
    renderBitmapString(-0.35f, -0.24f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Department of Computer Science");
    renderBitmapString(-0.36f, -0.30f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Faculty of Science and Technology");
    renderBitmapString(-0.5f, -0.36f, 0.0f, GLUT_BITMAP_HELVETICA_18, "American International University-Bangladesh (AIUB)");
    renderBitmapString(0.5f, -0.8f, 0.0f, GLUT_BITMAP_HELVETICA_18, "PRESS m / M For Manual");


    glFlush();
}

// --- Drawing Utilities ---

// Draw a semicircular hill

void drawHill(const Vec2& center, float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(center.x, center.y);
    for (int i = 0; i <= segments; ++i) {
        float theta = M_PI * i / segments;
        glVertex2f(center.x + cosf(theta) * radius,
                   center.y + sinf(theta) * radius);
    }
    glEnd();
}

// Draw all hills (distant, middle, foreground)

void drawHills() {
    struct HillConfig { Vec2 center; float radius; float r, g, b; };
    std::vector<HillConfig> hills = {
        // Distant
        {{-0.9f, 0.05f}, 0.2f, 0.6f, 0.8f, 0.5f},
        {{-0.4f, 0.05f}, 0.25f,0.6f, 0.8f, 0.5f},
        {{ 0.1f, 0.05f}, 0.18f,0.6f, 0.8f, 0.5f},
        {{ 0.6f, 0.05f}, 0.22f,0.6f, 0.8f, 0.5f},
        // Middle
        {{-0.85f,0.0f}, 0.28f,0.3f, 0.6f, 0.3f},
        {{-0.3f, 0.0f}, 0.29f,0.3f, 0.6f, 0.3f},
        {{ 0.3f, 0.0f}, 0.26f,0.3f, 0.6f, 0.3f},
        {{ 0.85f,0.0f}, 0.3f, 0.3f, 0.6f, 0.3f},
        // Foreground
        {{-0.95f,-0.02f},0.22f,0.0f, 0.4f, 0.0f},
        {{-0.4f,-0.02f}, 0.25f,0.0f, 0.4f, 0.0f},
        {{ 0.2f,-0.02f}, 0.25f, 0.0f, 0.4f, 0.0f},
        {{ 0.75f,-0.02f},0.28f,0.0f, 0.4f, 0.0f}
    };
    for (auto& h : hills) {
        glColor3f(h.r, h.g, h.b);
        drawHill(h.center, h.radius, 100);
    }
}

void drawFilledCircle(float x, float y, float radius, int segments, float r, float g, float b) {
    GLfloat angle;
    glColor3f(r, g, b); // Set color
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // Center of circle
        for (int i = 0; i <= segments; i++) {
            angle = 2.0f * PI * i / segments;
            glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
        }
    glEnd();
}

void drawPlane() {


     glPushMatrix();
    // Move the entire plane to the desired position on the runway
    //glTranslatef(0.0f, -0.4f, 0.0f);
    glTranslatef(planeX, planeY, 0.0f);


	glBegin(GL_POLYGON);            //Buttom black side
    glColor3f(0.1451f, 0.2235f, 0.2157f);
	glVertex2f(-0.875f, 0.267f);//v
	glVertex2f(-0.843f, 0.228f);
	glVertex2f(-0.8f, 0.2f);
	glVertex2f(-0.6f, 0.2f);
	glVertex2f(-0.591f, 0.207f);

	glVertex2f(-0.5960f, 0.2147f);
	glVertex2f(-0.4558f, 0.252f);//I
	glEnd();

	glBegin(GL_POLYGON); //middle body part

	glColor3f(0.4902f, 0.5059f, 0.5020f);
	glVertex2f(-0.875f, 0.267f);//v
	glVertex2f(-0.4558f, 0.252f);//I
	glVertex2f(-0.5655f, 0.3049f);//j
	glVertex2f(-0.5684f, 0.3083f);
	glVertex2f(-0.5721f, 0.3120f);

	glVertex2f(-0.5760f, 0.3161f);//m1
	glVertex2f(-0.58f,  0.32f);
	glVertex2f(-0.5843f, 0.3240f);
	glVertex2f(-0.5882f, 0.3276f);
	glVertex2f(-0.5918f,  0.3310f);

	glVertex2f(-0.5966f, 0.3349f);//h1
	glVertex2f(-0.6017f, 0.3372f);
	glVertex2f(-0.6082f, 0.3397f);
	glVertex2f(-0.6135f, 0.3391f);
	glVertex2f(-0.6204f, 0.3385f);

	glVertex2f(-0.6263f, 0.3379f);//c1
	glVertex2f(-0.6322f, 0.3361f);
	glVertex2f(-0.6388f, 0.3338f);
	glVertex2f(-0.6445f, 0.3316f);
	glVertex2f(-0.6509f, 0.3283f);//k

	glVertex2f(-0.6520f, 0.32810f);//L2
	glVertex2f(-0.7329f, 0.3134f);
	glVertex2f(-0.7850f, 0.3281f);
	glVertex2f(-0.846f, 0.315f);//b4
	glVertex2f(-0.8744f, 0.3240f);
	glVertex2f(-0.8849f, 0.3227f);
	glVertex2f(-0.8944f, 0.3171f);
	glVertex2f(-0.8949f, 0.2735f);
	glVertex2f(-0.8852f, 0.2674f);//u
	glEnd();

    glBegin(GL_POLYGON);//Fin part
	glColor3f(0.6416f, 0.6573f, 0.6573f);
	glVertex2f(-0.785f, 0.3281f);//M
	glVertex2f(-0.8286f, 0.4231f);
	glVertex2f(-0.8497f, 0.4231f);
    glVertex2f(-0.8750f,  0.3556f);//p
    glVertex2f(-0.8744f, 0.324f);//Q
    glVertex2f(-0.846f, 0.315f);//b4
    glEnd();
    //Main fraim ends here

    glBegin(GL_POLYGON);//Pilot sit
	glColor3f(0.7416f, 0.7573f, 0.7573f);
    glVertex2f(-0.5655f, 0.3049f);//j
	glVertex2f(-0.5684f, 0.3083f);
	glVertex2f(-0.5721f, 0.3120f);

	glVertex2f(-0.5760f, 0.3161f);//m1
	glVertex2f(-0.58f,  0.32f);
	glVertex2f(-0.5843f, 0.3240f);
	glVertex2f(-0.5882f, 0.3276f);
	glVertex2f(-0.5918f,  0.3310f);

	glVertex2f(-0.5966f, 0.3349f);//h1
	glVertex2f(-0.6017f, 0.3372f);
	glVertex2f(-0.6082f, 0.3397f);
	glVertex2f(-0.6135f, 0.3391f);
	glVertex2f(-0.6204f, 0.3385f);

	glVertex2f(-0.6263f, 0.3379f);//c1
	glVertex2f(-0.6322f, 0.3361f);
	glVertex2f(-0.6388f, 0.3338f);
	glVertex2f(-0.6445f, 0.3316f);
	glVertex2f(-0.6509f, 0.3283f);//k

	glVertex2f(-0.6476f, 0.3234f);//P1
	glVertex2f( -0.6432f, 0.3195f);
	glVertex2f( -0.6382f, 0.3159f);
	glVertex2f( -0.6330f, 0.3128f);
	glVertex2f( -0.6268f, 0.3094f);//T1

	glVertex2f( -0.6219f, 0.3071f);//u1
	glVertex2f( -0.6164f, 0.3047f);
	glVertex2f( -0.6115f, 0.3029f);
	glVertex2f( -0.6056f, 0.3011f);//z1
	glVertex2f( -0.6f, 0.3f);//A2

	glVertex2f( -0.5948f, 0.2990f);//B2
	glVertex2f( -0.5896f, 0.2986f);
	glVertex2f( -0.5849f, 0.2983f);
	glVertex2f( -0.5808f, 0.2979f);
	glVertex2f( -0.5775f, 0.2985f);//F2

	glVertex2f( -0.5745f, 0.2991f);//G2
	glVertex2f( -0.5724f, 0.2994f);
	glVertex2f( -0.57f, 0.3f);
	glVertex2f( -0.5677f, 0.3013f);
	glVertex2f( -0.5664f, 0.3028f);//K2
    glEnd();

    glBegin(GL_POLYGON);//Black Vorder1
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(-0.6476f, 0.3234f);//P1
	glVertex2f( -0.6432f, 0.3195f);
	glVertex2f( -0.6382f, 0.3159f);
	glVertex2f( -0.6330f, 0.3128f);
	glVertex2f( -0.6298f, 0.3091f);//v2
	glVertex2f( -0.6326f, 0.3096f);//U2
	glVertex2f( -0.636f, 0.3108f);//s2
	glVertex2f( -0.6409f, 0.3144f);//Q2
	glVertex2f( -0.6451f, 0.3176f);//o2
	glVertex2f( -0.6483f, 0.3206f);
	glVertex2f( -0.6503f, 0.3243f);
	glVertex2f( -0.652f, 0.3281f);//L2
	glVertex2f( -0.6509f, 0.3283f);//K
    glEnd();

    glBegin(GL_POLYGON);//Black Vorder2
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f( -0.6056f, 0.3011f);//z1
	glVertex2f( -0.6f, 0.3f);//A2
	glVertex2f( -0.5948f, 0.2990f);//B2
	glVertex2f( -0.5896f, 0.2986f);//c2
	glVertex2f( -0.5889f, 0.2955f);
	glVertex2f( -0.5918f,  0.295f);//j3
	glVertex2f( -0.5957f, 0.2943f);
	glVertex2f( -0.6007f, 0.2947f);//h3
	glVertex2f( -0.6053f, 0.296f);//G3
	glVertex2f( -0.6096f, 0.2972f);//F3
	glVertex2f( -0.6140f, 0.2986f);//E3
	glVertex2f( -0.6172f, 0.3f);//D3
	glVertex2f( -0.6201f, 0.3014f);//C3
	glVertex2f( -0.623f, 0.3027f);//B3
	glVertex2f( -0.627f, 0.3057f);//Z2
	glVertex2f( -0.6298f, 0.3091f);//V2
	glVertex2f( -0.6268f, 0.3094f);//T1
	glVertex2f( -0.6219f, 0.3071f);//u1
	glVertex2f( -0.6164f, 0.3047f);//v1
	glVertex2f( -0.6115f, 0.3029f);//w1
    glEnd();

     glBegin(GL_POLYGON);//Black border3
	glColor3f(0.0f, 0.0f, 0.0f);

	glVertex2f( -0.5666f, 0.2999f);//Q3
	glVertex2f( -0.5708f, 0.2959f);//P3
	glVertex2f( -0.5744f, 0.2948f);//O3
	glVertex2f( -0.578f, 0.2931f);//N3
	glVertex2f( -0.5822f, 0.2927f);//M3
	glVertex2f( -0.586f, 0.2941f);//L3
	glVertex2f( -0.5889f, 0.2955f);//K3
	glVertex2f( -0.5896f, 0.2986f);//c2
	glVertex2f( -0.5849f, 0.2983f);
	glVertex2f( -0.5808f, 0.2979f);
	glVertex2f( -0.5775f, 0.2985f);//F2
	glVertex2f( -0.5745f, 0.2991f);//G2
	glVertex2f( -0.5724f, 0.2994f);//H2
	glVertex2f( -0.57f, 0.3f);//I2
	glVertex2f( -0.5677f, 0.3013f);//J2
	glVertex2f( -0.5664f, 0.3028f);//K2
	glVertex2f(-0.5655f, 0.3049f);//j
	glVertex2f( -0.566f, 0.302f);//s3
	glVertex2f( -0.5666f, 0.2999f);//R3
    glEnd();

    //Fin Rudder
    glBegin(GL_POLYGON);
    glColor3f(0.8039f, 0.9922f, 1.0000f);
    glVertex2f( -0.8733f, 0.3602f);//P
    glVertex2f( -0.86f, 0.36f);//E4
    glVertex2f( -0.8399f, 0.4162f);//D4
    glVertex2f( -0.8521f, 0.4166f);//C4
    glEnd();
    //Fin rectangle
    glBegin(GL_POLYGON);
    glColor3f(0.2000f, 0.6000f, 0.3961f);
    glVertex2f( -0.8379f, 0.4001f);//G4
    glVertex2f( -0.8259f, 0.4001f);//H4
    glVertex2f( -0.8259f, 0.3916f);//I4
    glVertex2f( -0.8379f, 0.3916f);//J4
    glEnd();

    //Blaster or ZET engine part
    glBegin(GL_POLYGON);//1
    glColor3f(0.1961f, 0.0000f, 0.0000f);
    glVertex2f( -0.8744f, 0.324f);//Q
    glVertex2f( -0.8849f, 0.3227f);//R
    glVertex2f( -0.8852f, 0.2674f);//U
    glVertex2f( -0.8751f, 0.2672f);//V
    glEnd();

    glBegin(GL_POLYGON);//2
    glColor3f(0.6000f, 0.0039f, 0.0000f);
    glVertex2f( -0.8849f, 0.3227f);//R
    glVertex2f( -0.8852f, 0.2674f);//U
    glVertex2f( -0.8949f, 0.2735f);//T
    glVertex2f( -0.8944f, 0.3171f);//S
    glEnd();

    glBegin(GL_LINES);//3
    glColor3f(0.0f, 0.0000f, 0.0000f);

    glVertex2f( -0.8945f, 0.3097f);//O4
    glVertex2f( -0.885f, 0.3139f);//P4

    glVertex2f( -0.8946f, 0.3004f);//Q4
    glVertex2f( -0.885f, 0.304f);//R4

    glVertex2f( -0.8947f, 0.2952f);//W4
    glVertex2f( -0.885f, 0.2942f);//Z4

    glVertex2f( -0.8947f, 0.2884f);//U4
    glVertex2f( -0.885f, 0.2842f);//V4

    glVertex2f( -0.8947f, 0.281f);//S4
    glVertex2f( -0.885f, 0.2747f);//T4
    glEnd();

    //MIDDLE HAND PART
    glBegin(GL_POLYGON);
    glColor3f(0.7294f, 0.7294f, 0.7294f);
    glVertex2f( -0.801f, 0.2622f);//G5
    glVertex2f( -0.74f, 0.25f);//F5
    glVertex2f( -0.5937f, 0.2596f);//N4
    glVertex2f( -0.6821f, 0.2909f);//L5
    glVertex2f( -0.74f, 0.2668f);//K5
    glVertex2f( -0.8f, 0.274f);//J5
    glVertex2f( -0.86f, 0.3f);//i5
    glVertex2f( -0.8687f, 0.2863f);//H5
    glEnd();

    //MESILE ON THE BODY
    glBegin(GL_POLYGON);//1
    glColor3f(0.5843f, 0.6824f, 0.7608f);
    glVertex2f( -0.7559f, 0.2327f);//Q5
    glVertex2f( -0.756f, 0.21f);//U5
    glVertex2f( -0.6686f, 0.2102f);//T5
    glVertex2f( -0.6564f, 0.2211f);//S5
    glVertex2f( -0.6688f, .2325f);//R5
    glEnd();
    glBegin(GL_LINES);//2
    glColor3f(0.0f, 0.0000f, 0.0000f);
    glVertex2f( -0.7559f, 0.2217f);//
    glVertex2f( -0.6564f, 0.2211f);//S5
    glEnd();

    //wheel base1
    glBegin(GL_POLYGON);
    glColor3f(0.7294f, 0.7294f, 0.7294f);
    glVertex2f( -0.7929f, 0.2f);//A6
    glVertex2f( -0.793f, 0.1801f);//B6
    glVertex2f( -0.7872f, 0.1799f);//C6
    glVertex2f( -0.7871f, 0.2f);//D6
    glVertex2f( -0.7768f, 0.2321f);//C5
    glVertex2f( -0.7991f, 0.2321f);//B5
    glVertex2f( -0.8051f, 0.2159f);//A5
    glVertex2f( -0.8f, 0.2f);//
    glEnd();
    //wheel base2
    glBegin(GL_POLYGON);
    glColor3f(0.7294f, 0.7294f, 0.7294f);
    glVertex2f( -0.6085f, 0.2f);//H6
    glVertex2f( -0.6146f, 0.2f);//E6
    glVertex2f( -0.6085f, 0.1799f);//F6
    glVertex2f( -0.6047f, 0.1799f);//G6
    glEnd();
// Example usage of drawCircle(x, y, radius, segments, r, g, b)
    drawFilledCircle(-0.60649f, 0.17805f, 0.0099f, 100, 0.0f, 0.0f, 0.0f); // Front Wheel
    drawFilledCircle(-0.79f, 0.178f, 0.0099f, 100, 0.0f, 0.0f, 0.0f); // Back wheel

    //glVertex2f( f, f);//
	glFlush();  // Render now
	  glPopMatrix();

}
// Draw clouds

void drawCloud(const Vec2& position, float scale) {
    const int segments = 40;
    const float baseRadius = 0.1f * scale;
    glColor3f(0.7f, 0.9f, 1.0f);
    float offsets[5][2] = { {-0.15f,0},
                            {-0.05f,0.05f},
                            {0.05f,0},
                            {0.15f,0.05f},
                            {0, -0.05f}};
    float radii[5] = {1.0f,0.8f,1.0f,0.8f,0.9f};
    for (int i = 0; i < 5; ++i) {
        Vec2 c = { position.x + offsets[i][0] * scale,
                   position.y + offsets[i][1] * scale };
        float r = baseRadius * radii[i];
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(c.x, c.y);
        for (int j = 0; j <= segments; ++j) {
            float angle = 2 * M_PI * j / segments;
            glVertex2f(c.x + cosf(angle) * r,
                       c.y + sinf(angle) * r);
        }
        glEnd();
    }
}

void drawClouds() {
    // Draw original clouds
    drawCloud({-0.7f + cloudOffset, 0.8f}, 1.0f);
    drawCloud({0.0f + cloudOffset, 0.85f}, 0.9f);
    drawCloud({0.7f + cloudOffset, 0.75f}, 1.1f);

    // Draw mirrored clouds wrapping from left (cloudOffset - 2.0f)
    drawCloud({-0.7f + cloudOffset - 2.0f, 0.8f}, 1.0f);
    drawCloud({0.0f + cloudOffset - 2.0f, 0.85f}, 0.9f);
    drawCloud({0.7f + cloudOffset - 2.0f, 0.75f}, 1.1f);
}

// Draw sky background

void drawSky() {
    glColor3f(0.4f,0.8f,1.0f);
    glBegin(GL_QUADS);
        glVertex2f(-1,0);
        glVertex2f( 1,0);
        glVertex2f( 1,1);
        glVertex2f(-1,1);
    glEnd();
}

// Draw ground

void drawGround() {
    glColor3f(0.3f,0.5f,0.2f);
    glBegin(GL_QUADS);
      glVertex2f(-1,-1);
      glVertex2f( 1,-1);
      glVertex2f( 1, 0);
      glVertex2f(-1, 0);
    glEnd();
}

// Draw runway
void drawRunway() {
    glColor3f(0.1f,0.1f,0.1f);
    glBegin(GL_QUADS);
        glVertex2f(-1,-0.5f);
        glVertex2f( 1,-0.5f);
        glVertex2f( 1,-0.1f);
        glVertex2f(-1,-0.1f);
    glEnd();
    glColor3f(1,1,1);
    const float dashLen=0.12f, dashH=0.04f, gap=0.06f;
    for (float x=-0.99f; x<1.0f; x= x+dashLen+gap) {
        glBegin(GL_QUADS);
          glVertex2f(x,           -0.3f);
          glVertex2f(x+dashLen,   -0.3f);
          glVertex2f(x+dashLen,   -0.3f+dashH);
          glVertex2f(x,           -0.3f+dashH);
        glEnd();
    }
}

// Draw SUN

void drawSun() {
    float radius = 0.1f;
    int polygonAmount = 40; // Number of vertices to approximate the circle
    float twicePi = 2.0f * 3.1416f;

    glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    glBegin(GL_POLYGON);
    for (int i = 0; i <= polygonAmount; i++) {
        float angle = i * twicePi / polygonAmount;
        float x = 0.7f + radius * cosf(angle);
        float y = 0.8f + radius * sinf(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

//draw  tree

void drawTree(float x, float y, float trunkHeight, float trunkWidth, float foliageRadius) {
    // Draw trunk
    glColor3f(0.55f, 0.27f, 0.07f);  // brown color
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + trunkWidth, y);
        glVertex2f(x + trunkWidth, y + trunkHeight);
        glVertex2f(x, y + trunkHeight);
    glEnd();

    // Draw 3 overlapping green circles
    float foliageY = y + trunkHeight;
    glColor3f(0.0f, 0.5f, 0.0f);  // dark green

    // Center circle
    drawFilledCircle(x + trunkWidth / 2, foliageY + foliageRadius / 2, foliageRadius, 50, 0.0f, 0.5f, 0.0f);
    // Left circle
    drawFilledCircle(x + trunkWidth / 2 - foliageRadius / 1.5f, foliageY, foliageRadius, 50, 0.0f, 0.5f, 0.0f);
    // Right circle
    drawFilledCircle(x + trunkWidth / 2 + foliageRadius / 1.5f, foliageY, foliageRadius, 50, 0.0f, 0.5f, 0.0f);
}

void drawTrees() {
    glPushMatrix();
        glTranslatef(0.2f, 0.0f, 0.0f);
        drawTree(-0.7f, -0.1f, 0.15f, 0.05f, 0.1f);
    glPopMatrix();

    glPushMatrix();
        glScaled(0.5f, 0.5f, 0.0f);
        drawTree(0.2f, -0.05f, 0.2f, 0.06f, 0.12f);
    glPopMatrix();

    drawTree(0.7f, -0.02f, 0.12f, 0.04f, 0.09f);
}

void marufDisplay() {
    drawSky();
    drawSun();
    drawClouds();
    drawHills();
    drawGround();
    drawRunway();
    drawTrees();
    drawPlane();
}


/////////////////  MARUF PART END  //////////////////////////////////////

void mdrawSky() {
    glColor3f(0.53f, 0.81f, 0.98f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();
}

void mdrawSun() {
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.7f, 0.7f);
    for (int i = 0; i <= 360; i += 10) {
        float rad = i * 3.1416f / 180.0f;
        glVertex2f(0.7f + cos(rad) * 0.15f, 0.7f + sin(rad) * 0.15f);
    }
    glEnd();
}

// Draw one smooth "cloud puff" circle at (x,y) with radius r

void mdrawCircle(float x, float y, float r) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int angle = 0; angle <= 360; angle += 10) {
        float rad = angle * 3.1416f / 180.0f;
        glVertex2f(x + cos(rad) * r, y + sin(rad) * r);
    }
    glEnd();
}

void mdrawFilledCircle(float x, float y, float radius, int segments, float r, float g, float b) {
    GLfloat angle;
    glColor3f(r, g, b); // Set color
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // Center of circle
        for (int i = 0; i <= segments; i++) {
            angle = 2.0f * PI * i / segments;
            glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
        }
    glEnd();
}

void drawCloudm(const Vec2& position, float scale) {
    const int segments = 40;
    const float baseRadius = 0.1f * scale;
    glColor3f(1.0f, 1.0f, 1.0f);
    float offsets[5][2] = { {-0.15f,0},
                            {-0.05f,0.05f},
                            {0.05f,0},
                            {0.15f,0.05f},
                            {0, -0.05f}};
    float radii[5] = {1.0f,0.8f,1.0f,0.8f,0.9f};
    for (int i = 0; i < 5; ++i) {
        Vec2 c = { position.x + offsets[i][0] * scale,
                   position.y + offsets[i][1] * scale };
        float r = baseRadius * radii[i];
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(c.x, c.y);
        for (int j = 0; j <= segments; ++j) {
            float angle = 2 * M_PI * j / segments;
            glVertex2f(c.x + cosf(angle) * r,
                       c.y + sinf(angle) * r);
        }
        glEnd();
    }
}

void drawCloudms() {
    for (int i = 0; i < 10; ++i) {
        float offsetx = cloudOffsetx + (i * -2.0f);  // প্রতিটি মেঘ 2.0f দূরত্বে
        drawCloudm({-0.7f + offsetx, 0.9f}, 1.0f);
        drawCloudm({0.0f + offsetx, 0.95f}, 0.9f);
        drawCloudm({0.7f + offsetx, 0.85f}, 1.1f);
    }
}

void mdrawHill(float centerX, float height, float baseWidth, float r, float g, float b) {
    float left = centerX - baseWidth / 2.0f;
    float right = centerX + baseWidth / 2.0f;
    float peakY = -1.0f + height;

    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
        glVertex2f(left, -1.0f);
        glVertex2f(right, -1.0f);
        glVertex2f(centerX, peakY);
    glEnd();
}

void mdrawAircraft(){
    glPushMatrix(); // Save current transformation state
    glTranslatef(planePosX - 0.1f, 0.3f, 0.0f); // Plan upore o bame
    glRotatef(50.0f, 70.0f, 0.0f, 10.0f);

    glBegin(GL_POLYGON);            //Buttom black side
    glColor3f(0.1451f, 0.2235f, 0.2157f);
	glVertex2f(-0.875f, 0.267f);//v
	glVertex2f(-0.843f, 0.228f);
	glVertex2f(-0.8f, 0.2f);
	glVertex2f(-0.6f, 0.2f);
	glVertex2f(-0.591f, 0.207f);

	glVertex2f(-0.5960f, 0.2147f);
	glVertex2f(-0.4558f, 0.252f);//I
	glEnd();

	glBegin(GL_POLYGON); //middle body part

	glColor3f(0.4902f, 0.5059f, 0.5020f);
	glVertex2f(-0.875f, 0.267f);//v
	glVertex2f(-0.4558f, 0.252f);//I
	glVertex2f(-0.5655f, 0.3049f);//j
	glVertex2f(-0.5684f, 0.3083f);
	glVertex2f(-0.5721f, 0.3120f);

	glVertex2f(-0.5760f, 0.3161f);//m1
	glVertex2f(-0.58f,  0.32f);
	glVertex2f(-0.5843f, 0.3240f);
	glVertex2f(-0.5882f, 0.3276f);
	glVertex2f(-0.5918f,  0.3310f);

	glVertex2f(-0.5966f, 0.3349f);//h1
	glVertex2f(-0.6017f, 0.3372f);
	glVertex2f(-0.6082f, 0.3397f);
	glVertex2f(-0.6135f, 0.3391f);
	glVertex2f(-0.6204f, 0.3385f);

	glVertex2f(-0.6263f, 0.3379f);//c1
	glVertex2f(-0.6322f, 0.3361f);
	glVertex2f(-0.6388f, 0.3338f);
	glVertex2f(-0.6445f, 0.3316f);
	glVertex2f(-0.6509f, 0.3283f);//k

	glVertex2f(-0.6520f, 0.32810f);//L2
	glVertex2f(-0.7329f, 0.3134f);
	glVertex2f(-0.7850f, 0.3281f);
	glVertex2f(-0.846f, 0.315f);//b4
	glVertex2f(-0.8744f, 0.3240f);
	glVertex2f(-0.8849f, 0.3227f);
	glVertex2f(-0.8944f, 0.3171f);
	glVertex2f(-0.8949f, 0.2735f);
	glVertex2f(-0.8852f, 0.2674f);//u
	glEnd();

    glBegin(GL_POLYGON);//Fin part
	glColor3f(0.6416f, 0.6573f, 0.6573f);
	glVertex2f(-0.785f, 0.3281f);//M
	glVertex2f(-0.8286f, 0.4231f);
	glVertex2f(-0.8497f, 0.4231f);
    glVertex2f(-0.8750f,  0.3556f);//p
    glVertex2f(-0.8744f, 0.324f);//Q
    glVertex2f(-0.846f, 0.315f);//b4
    glEnd();
    //Main fraim ends here

    glBegin(GL_POLYGON);//Pilot sit
	glColor3f(0.7416f, 0.7573f, 0.7573f);
    glVertex2f(-0.5655f, 0.3049f);//j
	glVertex2f(-0.5684f, 0.3083f);
	glVertex2f(-0.5721f, 0.3120f);

	glVertex2f(-0.5760f, 0.3161f);//m1
	glVertex2f(-0.58f,  0.32f);
	glVertex2f(-0.5843f, 0.3240f);
	glVertex2f(-0.5882f, 0.3276f);
	glVertex2f(-0.5918f,  0.3310f);

	glVertex2f(-0.5966f, 0.3349f);//h1
	glVertex2f(-0.6017f, 0.3372f);
	glVertex2f(-0.6082f, 0.3397f);
	glVertex2f(-0.6135f, 0.3391f);
	glVertex2f(-0.6204f, 0.3385f);

	glVertex2f(-0.6263f, 0.3379f);//c1
	glVertex2f(-0.6322f, 0.3361f);
	glVertex2f(-0.6388f, 0.3338f);
	glVertex2f(-0.6445f, 0.3316f);
	glVertex2f(-0.6509f, 0.3283f);//k

	glVertex2f(-0.6476f, 0.3234f);//P1
	glVertex2f( -0.6432f, 0.3195f);
	glVertex2f( -0.6382f, 0.3159f);
	glVertex2f( -0.6330f, 0.3128f);
	glVertex2f( -0.6268f, 0.3094f);//T1

	glVertex2f( -0.6219f, 0.3071f);//u1
	glVertex2f( -0.6164f, 0.3047f);
	glVertex2f( -0.6115f, 0.3029f);
	glVertex2f( -0.6056f, 0.3011f);//z1
	glVertex2f( -0.6f, 0.3f);//A2

	glVertex2f( -0.5948f, 0.2990f);//B2
	glVertex2f( -0.5896f, 0.2986f);
	glVertex2f( -0.5849f, 0.2983f);
	glVertex2f( -0.5808f, 0.2979f);
	glVertex2f( -0.5775f, 0.2985f);//F2

	glVertex2f( -0.5745f, 0.2991f);//G2
	glVertex2f( -0.5724f, 0.2994f);
	glVertex2f( -0.57f, 0.3f);
	glVertex2f( -0.5677f, 0.3013f);
	glVertex2f( -0.5664f, 0.3028f);//K2
    glEnd();

    glBegin(GL_POLYGON);//Black Vorder1
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(-0.6476f, 0.3234f);//P1
	glVertex2f( -0.6432f, 0.3195f);
	glVertex2f( -0.6382f, 0.3159f);
	glVertex2f( -0.6330f, 0.3128f);
	glVertex2f( -0.6298f, 0.3091f);//v2
	glVertex2f( -0.6326f, 0.3096f);//U2
	glVertex2f( -0.636f, 0.3108f);//s2
	glVertex2f( -0.6409f, 0.3144f);//Q2
	glVertex2f( -0.6451f, 0.3176f);//o2
	glVertex2f( -0.6483f, 0.3206f);
	glVertex2f( -0.6503f, 0.3243f);
	glVertex2f( -0.652f, 0.3281f);//L2
	glVertex2f( -0.6509f, 0.3283f);//K
    glEnd();

    glBegin(GL_POLYGON);//Black Vorder2
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f( -0.6056f, 0.3011f);//z1
	glVertex2f( -0.6f, 0.3f);//A2
	glVertex2f( -0.5948f, 0.2990f);//B2
	glVertex2f( -0.5896f, 0.2986f);//c2
	glVertex2f( -0.5889f, 0.2955f);
	glVertex2f( -0.5918f,  0.295f);//j3
	glVertex2f( -0.5957f, 0.2943f);
	glVertex2f( -0.6007f, 0.2947f);//h3
	glVertex2f( -0.6053f, 0.296f);//G3
	glVertex2f( -0.6096f, 0.2972f);//F3
	glVertex2f( -0.6140f, 0.2986f);//E3
	glVertex2f( -0.6172f, 0.3f);//D3
	glVertex2f( -0.6201f, 0.3014f);//C3
	glVertex2f( -0.623f, 0.3027f);//B3
	glVertex2f( -0.627f, 0.3057f);//Z2
	glVertex2f( -0.6298f, 0.3091f);//V2
	glVertex2f( -0.6268f, 0.3094f);//T1
	glVertex2f( -0.6219f, 0.3071f);//u1
	glVertex2f( -0.6164f, 0.3047f);//v1
	glVertex2f( -0.6115f, 0.3029f);//w1
    glEnd();

     glBegin(GL_POLYGON);//Black border3
	glColor3f(0.0f, 0.0f, 0.0f);

	glVertex2f( -0.5666f, 0.2999f);//Q3
	glVertex2f( -0.5708f, 0.2959f);//P3
	glVertex2f( -0.5744f, 0.2948f);//O3
	glVertex2f( -0.578f, 0.2931f);//N3
	glVertex2f( -0.5822f, 0.2927f);//M3
	glVertex2f( -0.586f, 0.2941f);//L3
	glVertex2f( -0.5889f, 0.2955f);//K3
	glVertex2f( -0.5896f, 0.2986f);//c2
	glVertex2f( -0.5849f, 0.2983f);
	glVertex2f( -0.5808f, 0.2979f);
	glVertex2f( -0.5775f, 0.2985f);//F2
	glVertex2f( -0.5745f, 0.2991f);//G2
	glVertex2f( -0.5724f, 0.2994f);//H2
	glVertex2f( -0.57f, 0.3f);//I2
	glVertex2f( -0.5677f, 0.3013f);//J2
	glVertex2f( -0.5664f, 0.3028f);//K2
	glVertex2f(-0.5655f, 0.3049f);//j
	glVertex2f( -0.566f, 0.302f);//s3
	glVertex2f( -0.5666f, 0.2999f);//R3
    glEnd();

    //Fin Rudder
    glBegin(GL_POLYGON);
    glColor3f(0.8039f, 0.9922f, 1.0000f);
    glVertex2f( -0.8733f, 0.3602f);//P
    glVertex2f( -0.86f, 0.36f);//E4
    glVertex2f( -0.8399f, 0.4162f);//D4
    glVertex2f( -0.8521f, 0.4166f);//C4
    glEnd();
    //Fin rectangle
    glBegin(GL_POLYGON);
    glColor3f(0.2000f, 0.6000f, 0.3961f);
    glVertex2f( -0.8379f, 0.4001f);//G4
    glVertex2f( -0.8259f, 0.4001f);//H4
    glVertex2f( -0.8259f, 0.3916f);//I4
    glVertex2f( -0.8379f, 0.3916f);//J4
    glEnd();

    //Blaster or ZET engine part
    glBegin(GL_POLYGON);//1
    glColor3f(0.1961f, 0.0000f, 0.0000f);
    glVertex2f( -0.8744f, 0.324f);//Q
    glVertex2f( -0.8849f, 0.3227f);//R
    glVertex2f( -0.8852f, 0.2674f);//U
    glVertex2f( -0.8751f, 0.2672f);//V
    glEnd();

    glBegin(GL_POLYGON);//2
    glColor3f(0.6000f, 0.0039f, 0.0000f);
    glVertex2f( -0.8849f, 0.3227f);//R
    glVertex2f( -0.8852f, 0.2674f);//U
    glVertex2f( -0.8949f, 0.2735f);//T
    glVertex2f( -0.8944f, 0.3171f);//S
    glEnd();

    glBegin(GL_LINES);//3
    glColor3f(0.0f, 0.0000f, 0.0000f);

    glVertex2f( -0.8945f, 0.3097f);//O4
    glVertex2f( -0.885f, 0.3139f);//P4

    glVertex2f( -0.8946f, 0.3004f);//Q4
    glVertex2f( -0.885f, 0.304f);//R4

    glVertex2f( -0.8947f, 0.2952f);//W4
    glVertex2f( -0.885f, 0.2942f);//Z4

    glVertex2f( -0.8947f, 0.2884f);//U4
    glVertex2f( -0.885f, 0.2842f);//V4

    glVertex2f( -0.8947f, 0.281f);//S4
    glVertex2f( -0.885f, 0.2747f);//T4
    glEnd();

    //MIDDLE HAND PART
    glBegin(GL_POLYGON);
    glColor3f(0.7294f, 0.7294f, 0.7294f);
    glVertex2f( -0.801f, 0.2622f);//G5
    glVertex2f( -0.74f, 0.25f);//F5
    glVertex2f( -0.5937f, 0.2596f);//N4
    glVertex2f( -0.6821f, 0.2909f);//L5
    glVertex2f( -0.74f, 0.2668f);//K5
    glVertex2f( -0.8f, 0.274f);//J5
    glVertex2f( -0.86f, 0.3f);//i5
    glVertex2f( -0.8687f, 0.2863f);//H5
    glEnd();

    //MESILE ON THE BODY
    glBegin(GL_POLYGON);//1
    glColor3f(0.5843f, 0.6824f, 0.7608f);
    glVertex2f( -0.7559f, 0.2327f);//Q5
    glVertex2f( -0.756f, 0.21f);//U5
    glVertex2f( -0.6686f, 0.2102f);//T5
    glVertex2f( -0.6564f, 0.2211f);//S5
    glVertex2f( -0.6688f, .2325f);//R5
    glEnd();
    glBegin(GL_LINES);//2
    glColor3f(0.0f, 0.0000f, 0.0000f);
    glVertex2f( -0.7559f, 0.2217f);//
    glVertex2f( -0.6564f, 0.2211f);//S5
    glEnd();

    //wheel base1
    glBegin(GL_POLYGON);
    glColor3f(0.7294f, 0.7294f, 0.7294f);
    glVertex2f( -0.7929f, 0.2f);//A6
    glVertex2f( -0.793f, 0.1801f);//B6
    glVertex2f( -0.7872f, 0.1799f);//C6
    glVertex2f( -0.7871f, 0.2f);//D6
    glVertex2f( -0.7768f, 0.2321f);//C5
    glVertex2f( -0.7991f, 0.2321f);//B5
    glVertex2f( -0.8051f, 0.2159f);//A5
    glVertex2f( -0.8f, 0.2f);//
    glEnd();
    //wheel base2
    glBegin(GL_POLYGON);
    glColor3f(0.7294f, 0.7294f, 0.7294f);
    glVertex2f( -0.6085f, 0.2f);//H6
    glVertex2f( -0.6146f, 0.2f);//E6
    glVertex2f( -0.6085f, 0.1799f);//F6
    glVertex2f( -0.6047f, 0.1799f);//G6
    glEnd();
// Example usage of drawCircle(x, y, radius, segments, r, g, b)
    mdrawFilledCircle(-0.60649f, 0.17805f, 0.0099f, 100, 0.0f, 0.0f, 0.0f); // Front Wheel
    mdrawFilledCircle(-0.79f, 0.178f, 0.0099f, 100, 0.0f, 0.0f, 0.0f); // Back wheel



    glBegin(GL_TRIANGLES);
// কমলা বাহিরের আগুন
glColor3f(1.0f, 0.5f, 0.0f);
glVertex2f(-0.895f, 0.27f);  // নিচ
glVertex2f(-0.925f, 0.295f); // মাঝ বরাবর পিছনে
glVertex2f(-0.895f, 0.32f);  // উপর

glEnd();

glBegin(GL_TRIANGLES);
// ভেতরের হলুদ আগুন
glColor3f(1.0f, 1.0f, 0.0f);
glVertex2f(-0.895f, 0.28f);  // নিচ
glVertex2f(-0.915f, 0.295f); // মাঝ
glVertex2f(-0.895f, 0.31f);  // উপর
glEnd();

glBegin(GL_TRIANGLES);
// মাঝখানে লাল অংশ
glColor3f(1.0f, 0.0f, 0.0f);
glVertex2f(-0.895f, 0.285f);  // নিচ
glVertex2f(-0.907f, 0.295f);  // মাঝ
glVertex2f(-0.895f, 0.305f);  // উপর
glEnd();

    glPopMatrix();



}

void marufCWDisplay(){

glClear(GL_COLOR_BUFFER_BIT);
    mdrawSky();
    mdrawSun();
    drawCloudms();

    mdrawAircraft(); // Now it will be rotated

     mdrawHill(-0.7f, 0.6f, 0.8f, 0.6f, 0.4f, 0.2f);
     mdrawHill(-0.3f, 0.9f, 0.8f, 0.5f, 0.7f, 0.3f);
     mdrawHill(0.1f, 1.2f, 0.8f, 0.4f, 0.5f, 0.2f);
     mdrawHill(0.6f, 0.8f, 0.8f, 0.7f, 0.4f, 0.1f);
     mdrawHill(0.9f, 0.6f, 0.8f, 0.82f, 0.41f, 0.12f);


    mdrawHill(-0.95f, 0.5f, 0.7f, 0.3f, 0.2f, 0.1f);
    mdrawHill(-0.5f, 0.7f, 0.6f, 0.5f, 0.3f, 0.2f);
    mdrawHill(0.35f, 1.0f, 0.7f, 0.4f, 0.6f, 0.3f);
    mdrawHill(0.75f, 0.7f, 0.5f, 0.6f, 0.5f, 0.4f);
    mdrawHill(1.1f, 0.5f, 0.6f, 0.7f, 0.3f, 0.1f);
    mdrawHill(-0.05f, 0.85f, 0.6f, 0.5f, 0.4f, 0.2f);
    mdrawHill(0.25f, 0.9f, 0.6f, 0.6f, 0.5f, 0.3f);



    // Far right, low


    glFlush();

}

/////////////////  MARUF CW PART END  //////////////////////////////////////

// Function to draw a filled circle (for wheels)

void adrawFilledCircle(float x, float y, float radius, int segments, float r, float g, float b) {
    GLfloat angle;
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // center
    for (int i = 0; i <= segments; i++) {
        angle = 2.0f * PI * i / segments;
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
}

// Initialization

void initGL() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Background color white
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
}

//circle for clouds

void adrawCircle(float x, float y, float radius) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 50; i++) {
        float angle = 2.0f * PI * i / 50;
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
}

void adrawCloud(float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f); // white color for cloud
    adrawCircle(x, y, 0.06f);
    adrawCircle(x + 0.05f, y + 0.02f, 0.06f);
    adrawCircle(x + 0.1f, y, 0.06f);
    adrawCircle(x + 0.05f, y - 0.02f, 0.05f);
}

void adrawSkySunAndClouds() {
    // Sky
    glBegin(GL_QUADS);
    glColor3f(0.5725f, 0.7922f, 0.9059f); // light blue
    glVertex2f(-1, 1);
    glVertex2f(1, 1);
    glVertex2f(1, -0.8);
    glVertex2f(-1, -0.8);
    glEnd();

    // Sun
    GLfloat x = 0.8f;
    GLfloat y = 0.8f;
    GLfloat radius = 0.1f;
    glColor3f(1.0f, 1.0f, 0.0f); // yellow sun
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 100; i++) {
    float angle = 2.0f * PI * i / 100;
    glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();

    // Clouds
    adrawCloud(-0.8f, 0.85f);
    adrawCloud(-0.2f, 0.75f);
    adrawCloud(0.3f, 0.8f);
    adrawCloud(0.7f, 0.75f);
}


/*void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        aplaneX = -1.2f;   // left to right side
        animate = true;
        if (!timerRunning) {
            glutTimerFunc(0, update, 0);
        }
    }

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (!missileActive) {
            missileActive = true;
            missileYOffset = 0.0f;
            if (!timerRunning) {
                glutTimerFunc(0, update, 0);
            }
        }
    }
}*/

void asifDisplay(){

     glClear(GL_COLOR_BUFFER_BIT);

adrawSkySunAndClouds();


//building code 1
glBegin(GL_QUADS);
    glColor3f(0.6f, 0.6f, 0.6f); // Gray color
    glVertex2f(-0.98f, -0.8f);
    glVertex2f(-0.98f, -0.35f);
    glVertex2f(-0.84f, -0.35f);
    glVertex2f(-0.84f, -0.8f);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.96f, -0.75f);
    glVertex2f(-0.96f, -0.4f);
    glVertex2f (-0.94f, -0.4f);
    glVertex2f(-0.94f, -0.75f);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.96f, -0.75f);
    glVertex2f(-0.96f, -0.4f);
    glVertex2f (-0.94f, -0.4f);
    glVertex2f(-0.94f, -0.75f);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.92f, -0.75f);
    glVertex2f(-0.92f, -0.4f);
    glVertex2f(-0.9f, -0.4f);
    glVertex2f(-0.9f, -0.75f);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.88, -0.75);
    glVertex2f(-0.88, -0.4);
    glVertex2f(-0.86, -0.4);
    glVertex2f(-0.86, -0.75);
    glEnd();


//Building Code 2

    glBegin(GL_QUADS);
    glColor3f(0.6f, 0.6f, 0.6f); // Gray color
    glVertex2f(-0.82, -0.8);
    glVertex2f(-0.82, -0.45);
    glVertex2f(-0.72, -0.45);
    glVertex2f(-0.72, -0.8);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.8, -0.75);
    glVertex2f(-0.8, -0.6);
    glVertex2f(-0.78, -0.6);
     glVertex2f(-0.78, -0.75);
    glEnd();

 glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.76, -0.75);
    glVertex2f(-0.76, -0.6);
    glVertex2f(-0.74, -0.6);
     glVertex2f(-0.74, -0.75);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.8, -0.55);
    glVertex2f(-0.8, -0.5);
    glVertex2f(-0.74, -0.5);
     glVertex2f(-0.74, -0.55);
    glEnd();


//Building Code 3

glBegin(GL_POLYGON);
glColor3f(0.6f, 0.6f, 0.6f); // Gray color

	glVertex2f(-0.7, -0.8);
	glVertex2f(-0.7, -0.35);
	glVertex2f(-0.63, -0.3);
	glVertex2f(-0.56, -0.35);
	glVertex2f(-0.56, -0.8);
 glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.68, -0.5);
    glVertex2f(-0.68, -0.4);
    glVertex2f(-0.64, -0.4);
     glVertex2f(-0.64, -0.5);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.62, -0.5);
    glVertex2f(-0.62, -0.4);
    glVertex2f(-0.58, -0.4);
     glVertex2f(-0.58, -0.5);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.68, -0.65);
    glVertex2f(-0.68, -0.55);
    glVertex2f(-0.64, -0.55);
     glVertex2f(-0.64, -0.65);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.62, -0.65);
    glVertex2f(-0.62, -0.55);
    glVertex2f(-0.58, -0.55);
     glVertex2f(-0.58, -0.65);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.68, -0.8);
    glVertex2f(-0.68, -0.7);
    glVertex2f(-0.64, -0.7);
     glVertex2f(-0.64, -0.8);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.62, -0.8);
    glVertex2f(-0.62, -0.7);
    glVertex2f(-0.58, -0.7);
     glVertex2f(-0.58, -0.8);
    glEnd();


 //Building Code 4

glBegin(GL_QUADS);
 glColor3f(0.6f, 0.6f, 0.6f); // Gray color
    glVertex2f(-0.54, -0.8);
    glVertex2f(-0.54, -0.35);
    glVertex2f(-0.32, -0.35);
     glVertex2f(-0.32, -0.8);
    glEnd();


glBegin(GL_QUADS);
     glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color

    glVertex2f(-0.52, -0.75);
    glVertex2f(-0.52, -0.7);
    glVertex2f(-0.34, -0.7);
     glVertex2f(-0.34, -0.75);
    glEnd();

glBegin(GL_QUADS);
     glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color

    glVertex2f(-0.52, -0.6);
    glVertex2f(-0.34, -0.6);
    glVertex2f(-0.34, -0.65);
     glVertex2f(-0.52, -0.65);
    glEnd();

glBegin(GL_QUADS);
     glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color

    glVertex2f(-0.52, -0.55);
    glVertex2f(-0.52, -0.5);
    glVertex2f(-0.34, -0.5);
     glVertex2f(-0.34, -0.55);
    glEnd();

glBegin(GL_QUADS);
     glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color

    glVertex2f(-0.52, -0.45);
    glVertex2f(-0.52, -0.4);
    glVertex2f(-0.34, -0.4);
     glVertex2f(-0.34, -0.45);
    glEnd();


//Buildinf Code 5

glBegin(GL_QUADS);
    glColor3f(0.6f, 0.6f, 0.6f); // Gray color
    glVertex2f(-0.3, -0.8);
    glVertex2f(-0.3, -0.35);
    glVertex2f(-0.18, -0.3);
     glVertex2f(-0.18, -0.8);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.28, -0.75);
    glVertex2f(-0.28, -0.4);
    glVertex2f(-0.26, -0.4);
     glVertex2f(-0.26, -0.75);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.22, -0.75);
    glVertex2f(-0.22, -0.4);
    glVertex2f(-0.2, -0.4);
     glVertex2f(-0.2, -0.75);
    glEnd();


//building 6

glBegin(GL_QUADS);
    glColor3f(0.6f, 0.6f, 0.6f); // Gray color
    glVertex2f(-0.16, -0.8);
    glVertex2f(-0.16, -0.3);
    glVertex2f(-0.02, -0.3);
    glVertex2f(-0.02, -0.8);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.14, -0.75);
    glVertex2f(-0.14, -0.4);
    glVertex2f(-0.12, -0.4);
     glVertex2f(-0.12, -0.75);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.1, -0.75);
    glVertex2f(-0.1, -0.4);
    glVertex2f(-0.08, -0.4);
     glVertex2f(-0.08, -0.75);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(-0.06, -0.75);
    glVertex2f(-0.06, -0.4);
    glVertex2f(-0.04, -0.4);
    glVertex2f(-0.04, -0.75);
    glEnd();


//building 7

glBegin(GL_QUADS);
    glColor3f(0.6f, 0.6f, 0.6f); // Gray color
    glVertex2f(0.02, -0.8);
    glVertex2f(0.02, -0.3);
    glVertex2f(0.16, -0.3);
    glVertex2f(0.16, -0.8);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.04, -0.75);
    glVertex2f(0.04, -0.65);
    glVertex2f(0.08, -0.65);
     glVertex2f(0.08, -0.75);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.04, -0.6);
    glVertex2f(0.04, -0.5);
    glVertex2f(0.08, -0.5);
     glVertex2f(0.08, -0.6);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.04, -0.45);
    glVertex2f(0.04, -0.35);
    glVertex2f(0.08, -0.35);
     glVertex2f(0.08, -0.45);
    glEnd();


glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.1, -0.45);
    glVertex2f(0.1, -0.35);
    glVertex2f(0.14, -0.35);
     glVertex2f(0.14, -0.45);
    glEnd();


glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.1, -0.6);
    glVertex2f(0.1, -0.5);
    glVertex2f(0.14, -0.5);
     glVertex2f(0.14, -0.6);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.1, -0.75);
    glVertex2f(0.1, -0.65);
    glVertex2f(0.14, -0.65);
     glVertex2f(0.14, -0.75);
    glEnd();


//Building 8

glBegin(GL_QUADS);
    glColor3f(0.6f, 0.6f, 0.6f); // Gray color
    glVertex2f(0.18, -0.8);
    glVertex2f(0.18, -0.35);
    glVertex2f(0.36, -0.35);
    glVertex2f(0.36, -0.8);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.2, -0.75);
    glVertex2f(0.2, -0.7);
    glVertex2f(0.34, -0.7);
    glVertex2f(0.34, -0.75);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.2, -0.65);
    glVertex2f(0.2, -0.6);
    glVertex2f(0.34, -0.6);
     glVertex2f(0.34, -0.65);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.2, -0.55);
    glVertex2f(0.2, -0.5);
    glVertex2f(0.34, -0.5);
     glVertex2f(0.34, -0.55);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.2, -0.45);
    glVertex2f(0.2, -0.4);
    glVertex2f(0.34, -0.4);
     glVertex2f(0.34, -0.45);
    glEnd();


//Building code 9

glBegin(GL_QUADS);
    glColor3f(0.6f, 0.6f, 0.6f); // Gray color
    glVertex2f(0.38, -0.8);
    glVertex2f(0.38, -0.4);
    glVertex2f(0.52, -0.35);
    glVertex2f(0.52, -0.8);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.4, -0.75);
    glVertex2f(0.4, -0.45);
    glVertex2f(0.42, -0.45);
    glVertex2f(0.42, -0.75);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.48, -0.75);
    glVertex2f(0.48, -0.45);
    glVertex2f(0.5, -0.45);
    glVertex2f(0.5, -0.75);
    glEnd();


//Building 10

glBegin(GL_QUADS);
    glColor3f(0.6f, 0.6f, 0.6f); // Gray color
    glVertex2f(0.54, -0.8);
    glVertex2f(0.54, -0.35);
    glVertex2f(0.68, -0.3);
    glVertex2f(0.68, -0.8);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.56, -0.75);
    glVertex2f(0.56, -0.4);
    glVertex2f(0.58, -0.4);
    glVertex2f(0.58, -0.75);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.64, -0.75);
    glVertex2f(0.64, -0.4);
    glVertex2f(0.66, -0.4);
    glVertex2f(0.66, -0.75);
    glEnd();


//Building 11

glBegin(GL_POLYGON);
    glColor3f(0.6f, 0.6f, 0.6f); // Gray color
	glVertex2f(0.7, -0.8);
	glVertex2f(0.7, -0.35);
	glVertex2f(0.77, -0.32);
	glVertex2f(0.84, -0.35);
	glVertex2f(0.84, -0.8);
     glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.72, -0.45);
    glVertex2f(0.72, -0.4);
    glVertex2f(0.82, -0.4);
    glVertex2f(0.82, -0.45);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.72, -0.55);
    glVertex2f(0.72, -0.5);
    glVertex2f(0.82, -0.5);
    glVertex2f(0.82, -0.55);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.72, -0.6);
    glVertex2f(0.82, -0.6);
    glVertex2f(0.82, -0.65);
    glVertex2f(0.72, -0.65);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.72, -0.7);
    glVertex2f(0.82, -0.7);
    glVertex2f(0.82, -0.75);
    glVertex2f(0.72, -0.75);
    glEnd();


//Building 12

glBegin(GL_QUADS);
    glColor3f(0.6f, 0.6f, 0.6f); // Gray color
    glVertex2f(0.86, -0.8);
    glVertex2f(0.86, -0.3);
    glVertex2f(0.98, -0.3);
    glVertex2f(0.98, -0.8);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.88, -0.75);
    glVertex2f(0.88, -0.4);
    glVertex2f(0.9, -0.4);
    glVertex2f(0.9, -0.75);
    glEnd();

glBegin(GL_QUADS);
    glColor3f(0.95f, 0.95f, 0.9f);  // Off-white color
    glVertex2f(0.94, -0.75);
    glVertex2f(0.94, -0.4);
    glVertex2f(0.96, -0.4);
    glVertex2f(0.96, -0.75);
    glEnd();


//Ground

glBegin(GL_QUADS);
    glColor3f(0.5333f, 0.8980f, 0.5608f);
    glVertex2f(-1, -0.8);
    glVertex2f(1, -0.8);
    glVertex2f(1, -1);
    glVertex2f(-1, -1);
    glEnd();


//Plane Drawing

    glPushMatrix();
    glTranslatef(aplaneX, 0.0f, 0.0f); // Translate plane horizontally

    // ---------------------------------------------
    // Your full plane drawing code starts here
    glBegin(GL_POLYGON);
    glBegin(GL_POLYGON);            //Buttom black side
    glColor3f(0.1451f, 0.2235f, 0.2157f);
	glVertex2f(-0.875f, 0.267f);//v
	glVertex2f(-0.843f, 0.228f);
	glVertex2f(-0.8f, 0.2f);
	glVertex2f(-0.6f, 0.2f);
	glVertex2f(-0.591f, 0.207f);

	glVertex2f(-0.5960f, 0.2147f);
	glVertex2f(-0.4558f, 0.252f);//I
	glEnd();

	glBegin(GL_POLYGON); //middle body part

	glColor3f(0.4902f, 0.5059f, 0.5020f);
	glVertex2f(-0.875f, 0.267f);//v
	glVertex2f(-0.4558f, 0.252f);//I
	glVertex2f(-0.5655f, 0.3049f);//j
	glVertex2f(-0.5684f, 0.3083f);
	glVertex2f(-0.5721f, 0.3120f);

	glVertex2f(-0.5760f, 0.3161f);//m1
	glVertex2f(-0.58f,  0.32f);
	glVertex2f(-0.5843f, 0.3240f);
	glVertex2f(-0.5882f, 0.3276f);
	glVertex2f(-0.5918f,  0.3310f);

	glVertex2f(-0.5966f, 0.3349f);//h1
	glVertex2f(-0.6017f, 0.3372f);
	glVertex2f(-0.6082f, 0.3397f);
	glVertex2f(-0.6135f, 0.3391f);
	glVertex2f(-0.6204f, 0.3385f);

	glVertex2f(-0.6263f, 0.3379f);//c1
	glVertex2f(-0.6322f, 0.3361f);
	glVertex2f(-0.6388f, 0.3338f);
	glVertex2f(-0.6445f, 0.3316f);
	glVertex2f(-0.6509f, 0.3283f);//k

	glVertex2f(-0.6520f, 0.32810f);//L2
	glVertex2f(-0.7329f, 0.3134f);
	glVertex2f(-0.7850f, 0.3281f);
	glVertex2f(-0.846f, 0.315f);//b4
	glVertex2f(-0.8744f, 0.3240f);
	glVertex2f(-0.8849f, 0.3227f);
	glVertex2f(-0.8944f, 0.3171f);
	glVertex2f(-0.8949f, 0.2735f);
	glVertex2f(-0.8852f, 0.2674f);//u
	glEnd();

    glBegin(GL_POLYGON);//Fin part
	glColor3f(0.6416f, 0.6573f, 0.6573f);
	glVertex2f(-0.785f, 0.3281f);//M
	glVertex2f(-0.8286f, 0.4231f);
	glVertex2f(-0.8497f, 0.4231f);
    glVertex2f(-0.8750f,  0.3556f);//p
    glVertex2f(-0.8744f, 0.324f);//Q
    glVertex2f(-0.846f, 0.315f);//b4
    glEnd();
    //Main fraim ends here

    glBegin(GL_POLYGON);//Pilot sit
	glColor3f(0.7416f, 0.7573f, 0.7573f);
    glVertex2f(-0.5655f, 0.3049f);//j
	glVertex2f(-0.5684f, 0.3083f);
	glVertex2f(-0.5721f, 0.3120f);

	glVertex2f(-0.5760f, 0.3161f);//m1
	glVertex2f(-0.58f,  0.32f);
	glVertex2f(-0.5843f, 0.3240f);
	glVertex2f(-0.5882f, 0.3276f);
	glVertex2f(-0.5918f,  0.3310f);

	glVertex2f(-0.5966f, 0.3349f);//h1
	glVertex2f(-0.6017f, 0.3372f);
	glVertex2f(-0.6082f, 0.3397f);
	glVertex2f(-0.6135f, 0.3391f);
	glVertex2f(-0.6204f, 0.3385f);

	glVertex2f(-0.6263f, 0.3379f);//c1
	glVertex2f(-0.6322f, 0.3361f);
	glVertex2f(-0.6388f, 0.3338f);
	glVertex2f(-0.6445f, 0.3316f);
	glVertex2f(-0.6509f, 0.3283f);//k

	glVertex2f(-0.6476f, 0.3234f);//P1
	glVertex2f( -0.6432f, 0.3195f);
	glVertex2f( -0.6382f, 0.3159f);
	glVertex2f( -0.6330f, 0.3128f);
	glVertex2f( -0.6268f, 0.3094f);//T1

	glVertex2f( -0.6219f, 0.3071f);//u1
	glVertex2f( -0.6164f, 0.3047f);
	glVertex2f( -0.6115f, 0.3029f);
	glVertex2f( -0.6056f, 0.3011f);//z1
	glVertex2f( -0.6f, 0.3f);//A2

	glVertex2f( -0.5948f, 0.2990f);//B2
	glVertex2f( -0.5896f, 0.2986f);
	glVertex2f( -0.5849f, 0.2983f);
	glVertex2f( -0.5808f, 0.2979f);
	glVertex2f( -0.5775f, 0.2985f);//F2

	glVertex2f( -0.5745f, 0.2991f);//G2
	glVertex2f( -0.5724f, 0.2994f);
	glVertex2f( -0.57f, 0.3f);
	glVertex2f( -0.5677f, 0.3013f);
	glVertex2f( -0.5664f, 0.3028f);//K2
    glEnd();

    glBegin(GL_POLYGON);//Black Vorder1
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(-0.6476f, 0.3234f);//P1
	glVertex2f( -0.6432f, 0.3195f);
	glVertex2f( -0.6382f, 0.3159f);
	glVertex2f( -0.6330f, 0.3128f);
	glVertex2f( -0.6298f, 0.3091f);//v2
	glVertex2f( -0.6326f, 0.3096f);//U2
	glVertex2f( -0.636f, 0.3108f);//s2
	glVertex2f( -0.6409f, 0.3144f);//Q2
	glVertex2f( -0.6451f, 0.3176f);//o2
	glVertex2f( -0.6483f, 0.3206f);
	glVertex2f( -0.6503f, 0.3243f);
	glVertex2f( -0.652f, 0.3281f);//L2
	glVertex2f( -0.6509f, 0.3283f);//K
    glEnd();

    glBegin(GL_POLYGON);//Black Vorder2
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f( -0.6056f, 0.3011f);//z1
	glVertex2f( -0.6f, 0.3f);//A2
	glVertex2f( -0.5948f, 0.2990f);//B2
	glVertex2f( -0.5896f, 0.2986f);//c2
	glVertex2f( -0.5889f, 0.2955f);
	glVertex2f( -0.5918f,  0.295f);//j3
	glVertex2f( -0.5957f, 0.2943f);
	glVertex2f( -0.6007f, 0.2947f);//h3
	glVertex2f( -0.6053f, 0.296f);//G3
	glVertex2f( -0.6096f, 0.2972f);//F3
	glVertex2f( -0.6140f, 0.2986f);//E3
	glVertex2f( -0.6172f, 0.3f);//D3
	glVertex2f( -0.6201f, 0.3014f);//C3
	glVertex2f( -0.623f, 0.3027f);//B3
	glVertex2f( -0.627f, 0.3057f);//Z2
	glVertex2f( -0.6298f, 0.3091f);//V2
	glVertex2f( -0.6268f, 0.3094f);//T1
	glVertex2f( -0.6219f, 0.3071f);//u1
	glVertex2f( -0.6164f, 0.3047f);//v1
	glVertex2f( -0.6115f, 0.3029f);//w1
    glEnd();

     glBegin(GL_POLYGON);//Black border3
	glColor3f(0.0f, 0.0f, 0.0f);

	glVertex2f( -0.5666f, 0.2999f);//Q3
	glVertex2f( -0.5708f, 0.2959f);//P3
	glVertex2f( -0.5744f, 0.2948f);//O3
	glVertex2f( -0.578f, 0.2931f);//N3
	glVertex2f( -0.5822f, 0.2927f);//M3
	glVertex2f( -0.586f, 0.2941f);//L3
	glVertex2f( -0.5889f, 0.2955f);//K3
	glVertex2f( -0.5896f, 0.2986f);//c2
	glVertex2f( -0.5849f, 0.2983f);
	glVertex2f( -0.5808f, 0.2979f);
	glVertex2f( -0.5775f, 0.2985f);//F2
	glVertex2f( -0.5745f, 0.2991f);//G2
	glVertex2f( -0.5724f, 0.2994f);//H2
	glVertex2f( -0.57f, 0.3f);//I2
	glVertex2f( -0.5677f, 0.3013f);//J2
	glVertex2f( -0.5664f, 0.3028f);//K2
	glVertex2f(-0.5655f, 0.3049f);//j
	glVertex2f( -0.566f, 0.302f);//s3
	glVertex2f( -0.5666f, 0.2999f);//R3
    glEnd();

    //Fin Rudder
    glBegin(GL_POLYGON);
    glColor3f(0.8039f, 0.9922f, 1.0000f);
    glVertex2f( -0.8733f, 0.3602f);//P
    glVertex2f( -0.86f, 0.36f);//E4
    glVertex2f( -0.8399f, 0.4162f);//D4
    glVertex2f( -0.8521f, 0.4166f);//C4
    glEnd();
    //Fin rectangle
    glBegin(GL_POLYGON);
    glColor3f(0.2000f, 0.6000f, 0.3961f);
    glVertex2f( -0.8379f, 0.4001f);//G4
    glVertex2f( -0.8259f, 0.4001f);//H4
    glVertex2f( -0.8259f, 0.3916f);//I4
    glVertex2f( -0.8379f, 0.3916f);//J4
    glEnd();

    //Blaster or ZET engine part
    glBegin(GL_POLYGON);//1
    glColor3f(0.1961f, 0.0000f, 0.0000f);
    glVertex2f( -0.8744f, 0.324f);//Q
    glVertex2f( -0.8849f, 0.3227f);//R
    glVertex2f( -0.8852f, 0.2674f);//U
    glVertex2f( -0.8751f, 0.2672f);//V
    glEnd();

    glBegin(GL_POLYGON);//2
    glColor3f(0.6000f, 0.0039f, 0.0000f);
    glVertex2f( -0.8849f, 0.3227f);//R
    glVertex2f( -0.8852f, 0.2674f);//U
    glVertex2f( -0.8949f, 0.2735f);//T
    glVertex2f( -0.8944f, 0.3171f);//S
    glEnd();

    glBegin(GL_LINES);//3
    glColor3f(0.0f, 0.0000f, 0.0000f);

    glVertex2f( -0.8945f, 0.3097f);//O4
    glVertex2f( -0.885f, 0.3139f);//P4

    glVertex2f( -0.8946f, 0.3004f);//Q4
    glVertex2f( -0.885f, 0.304f);//R4

    glVertex2f( -0.8947f, 0.2952f);//W4
    glVertex2f( -0.885f, 0.2942f);//Z4

    glVertex2f( -0.8947f, 0.2884f);//U4
    glVertex2f( -0.885f, 0.2842f);//V4

    glVertex2f( -0.8947f, 0.281f);//S4
    glVertex2f( -0.885f, 0.2747f);//T4
    glEnd();

    //MIDDLE HAND PART
    glBegin(GL_POLYGON);
    glColor3f(0.7294f, 0.7294f, 0.7294f);
    glVertex2f( -0.801f, 0.2622f);//G5
    glVertex2f( -0.74f, 0.25f);//F5
    glVertex2f( -0.5937f, 0.2596f);//N4
    glVertex2f( -0.6821f, 0.2909f);//L5
    glVertex2f( -0.74f, 0.2668f);//K5
    glVertex2f( -0.8f, 0.274f);//J5
    glVertex2f( -0.86f, 0.3f);//i5
    glVertex2f( -0.8687f, 0.2863f);//H5
    glEnd();


    glPushMatrix();
glTranslatef(0.0f, -missileYOffset, 0.0f);


    //MESILE ON THE BODY
    glBegin(GL_POLYGON);//1
    glColor3f(0.5843f, 0.6824f, 0.7608f);
    glVertex2f( -0.7559f, 0.2327f);//Q5
    glVertex2f( -0.756f, 0.21f);//U5
    glVertex2f( -0.6686f, 0.2102f);//T5
    glVertex2f( -0.6564f, 0.2211f);//S5
    glVertex2f( -0.6688f, .2325f);//R5
    glEnd();

    glBegin(GL_LINES);//2
    glColor3f(0.0f, 0.0000f, 0.0000f);
    glVertex2f( -0.7559f, 0.2217f);//
    glVertex2f( -0.6564f, 0.2211f);//S5
    glEnd();

    glPopMatrix();

    //wheel base1
    glBegin(GL_POLYGON);
    glColor3f(0.7294f, 0.7294f, 0.7294f);
    glVertex2f( -0.7929f, 0.2f);//A6
    glVertex2f( -0.793f, 0.1801f);//B6
    glVertex2f( -0.7872f, 0.1799f);//C6
    glVertex2f( -0.7871f, 0.2f);//D6
    glVertex2f( -0.7768f, 0.2321f);//C5
    glVertex2f( -0.7991f, 0.2321f);//B5
    glVertex2f( -0.8051f, 0.2159f);//A5
    glVertex2f( -0.8f, 0.2f);//
    glEnd();
    //wheel base2
    glBegin(GL_POLYGON);
    glColor3f(0.7294f, 0.7294f, 0.7294f);
    glVertex2f( -0.6085f, 0.2f);//H6
    glVertex2f( -0.6146f, 0.2f);//E6
    glVertex2f( -0.6085f, 0.1799f);//F6
    glVertex2f( -0.6047f, 0.1799f);//G6
    glEnd();

// Example usage of adrawCircle(x, y, radius, segments, r, g, b)
    adrawFilledCircle(-0.60649f, 0.17805f, 0.0099f, 100, 0.0f, 0.0f, 0.0f); // Front Wheel
    adrawFilledCircle(-0.79f, 0.178f, 0.0099f, 100, 0.0f, 0.0f, 0.0f); // Back wheel

	glFlush();  // Render now
    glPopMatrix();
    glFlush();
}
/////////////////  ASIF PART END  //////////////////////////////////////

// ---------- EXPLOSION ----------

void drawExplosion(float centerX, float centerY, float time){
    glPushMatrix();
    glTranslatef( -0.65, -0.230f, 0.0f);
    float radius = time * 0.3f;

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 0.5f, 0.0f);
    glVertex2f(centerX, centerY);
    for (int angle = 0; angle <= 360; angle += 10)
    {
        float rad = angle * 3.14159f / 180;
        glColor3f(1.0f, 0.2f + 0.8f * (1.0f - time), 0.0f);
        glVertex2f(centerX + cos(rad) * radius, centerY + sin(rad) * radius);
    }
    glEnd();

    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 100; i++)
    {
        float angle = i * 2 * 3.14159f / 100;
        float r = radius + 0.02f * (rand() % 10);
        float x = centerX + r * cos(angle);
        float y = centerY + r * sin(angle);
        glColor3f(1.0f, (rand() % 100) / 100.0f, 0.0f);
        glVertex2f(x, y);
    }
    glEnd();

    glPopMatrix();
}

// Global function to draw a filled circle

void zdrawFilledCircle(float x, float y, float radius, int segments, float r, float g, float b) {
    GLfloat angle;
    glColor3f(r, g, b); // Set color
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // Center of circle
        for (int i = 0; i <= segments; i++) {
            angle = 2.0f * PI * i / segments;
            glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
        }
    glEnd();
}

// Draw sky background

void ZdrawSky() {
    glColor3f(0.4f,0.8f,1.0f);
    glBegin(GL_QUADS);
        glVertex2f(-1,-0.6);
        glVertex2f( 1,-0.6);
        glVertex2f( 1,1);
        glVertex2f(-1,1);
    glEnd();
}

//WINDOW SIZE1c1

void window1_c1(){
    glColor3f(0.0745f, 0.7137f, 0.7098f);
    glBegin(GL_POLYGON);
    glVertex2f( -0.02f, 0.02f);//
    glVertex2f( -0.02f, -0.02f);//
    glVertex2f( 0.02f, -0.02f);//
    glVertex2f( 0.02f, 0.02f);//
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f( -0.02f, 0.02f);//
    glVertex2f( -0.02f, -0.02f);//
    glVertex2f( 0.02f, -0.02f);//
    glVertex2f( 0.02f, 0.02f);//
    glEnd();
}

void window101_c1(){
    glPushMatrix();
    glScalef(0.6,0.6,0);
    glColor3f(0.0745f, 0.7137f, 0.7098f);
    glBegin(GL_POLYGON);
    glVertex2f( -0.02f, 0.02f);//
    glVertex2f( -0.02f, -0.02f);//
    glVertex2f( 0.02f, -0.02f);//
    glVertex2f( 0.02f, 0.02f);//
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f( -0.02f, 0.02f);//
    glVertex2f( -0.02f, -0.02f);//
    glVertex2f( 0.02f, -0.02f);//
    glVertex2f( 0.02f, 0.02f);//
    glEnd();
    glPopMatrix();
}

void window10_c1(){

    glPushMatrix();
    glScalef(0.4,0.4,0);
    glColor3f(0.0745f, 0.7137f, 0.7098f);
    glBegin(GL_POLYGON);
    glVertex2f( -0.02f, 0.02f);//
    glVertex2f( -0.02f, -0.02f);//
    glVertex2f( 0.02f, -0.02f);//
    glVertex2f( 0.02f, 0.02f);//
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f( -0.02f, 0.02f);//
    glVertex2f( -0.02f, -0.02f);//
    glVertex2f( 0.02f, -0.02f);//
    glVertex2f( 0.02f, 0.02f);//
    glEnd();
    glPopMatrix();
}
//WINDOW SIZE1c2

void window1_c2(){

    glColor3f(0.3412f, 0.3412f, 0.3412f);
    glBegin(GL_POLYGON);
    glVertex2f( -0.02f, 0.02f);//
    glVertex2f( -0.02f, -0.02f);//
    glVertex2f( 0.02f, -0.02f);//
    glVertex2f( 0.02f, 0.02f);//
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f( -0.02f, 0.02f);//
    glVertex2f( -0.02f, -0.02f);//
    glVertex2f( 0.02f, -0.02f);//
    glVertex2f( 0.02f, 0.02f);//
    glEnd();
}
//WINDOW SIZE2c1

void window2_c1(){
    glColor3f(0.3412f, 0.3412f, 0.3412f);
    glBegin(GL_POLYGON);
    glVertex2f( 0.01f, 0.02f);//
    glVertex2f( 0.01f, -0.02f);//
    glVertex2f( -0.01f, -0.02f);//
    glVertex2f( -0.01f, 0.02f);//
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f( 0.01f, 0.02f);//
    glVertex2f( 0.01f, -0.02f);//
    glVertex2f( -0.01f, -0.02f);//
    glVertex2f( -0.01f, 0.02f);//
    glEnd();
}

//WINDOW SIZE2c2

void window2_c2(){

    glColor3f(0.0745f, 0.7137f, 0.7098f);
    glBegin(GL_POLYGON);
    glVertex2f( 0.01f, 0.02f);//
    glVertex2f( 0.01f, -0.02f);//
    glVertex2f( -0.01f, -0.02f);//
    glVertex2f( -0.01f, 0.02f);//
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f( 0.01f, 0.02f);//
    glVertex2f( 0.01f, -0.02f);//
    glVertex2f( -0.01f, -0.02f);//
    glVertex2f( -0.01f, 0.02f);//
    glEnd();
}
void Zdrawbomb(){

    glPushMatrix();
    glTranslatef(bombX, bombY, 0.0f);
        glColor3f(0.1569f, 0.1569f, 0.1569f);
        glBegin(GL_POLYGON);//bom body
        glVertex2f( -0.7671f, 0.235f);//
        glVertex2f( -0.667f, 0.2352f);//
        glVertex2f( -0.6371f, 0.2652f);//top
        glVertex2f( -0.6671f, 0.28526776f);//
        glVertex2f( -0.767167f, 0.285f);//
        glVertex2f( -0.7777f, 0.2950f);//
        glVertex2f( -0.777f, 0.2252f);//
        glEnd();

        glColor3f(0.3373f, 0.3373f, 0.3373f);
        glBegin(GL_POLYGON);//bom fin
        glVertex2f( -0.7514f, 0.258f);//
        glVertex2f( -0.751f, 0.265f);//
        glVertex2f( -0.7736f, 0.2681f);//
        glVertex2f( -0.773f, 0.2532f);//
        glEnd();
    glPopMatrix();
}

void ZdrawPlane() {

     glPushMatrix();
    // Move the entire plane to the desired position on the runway
    //glTranslatef(0.0f, 0.0f, 0.0f);
    glTranslatef(ZplaneX, ZplaneY, 0.0f);
	glBegin(GL_POLYGON);            //Buttom black side
    glColor3f(0.1451f, 0.2235f, 0.2157f);
	glVertex2f(-0.875f, 0.267f);//v
	glVertex2f(-0.843f, 0.228f);
	glVertex2f(-0.8f, 0.2f);
	glVertex2f(-0.6f, 0.2f);
	glVertex2f(-0.591f, 0.207f);

	glVertex2f(-0.5960f, 0.2147f);
	glVertex2f(-0.4558f, 0.252f);//I
	glEnd();

	glBegin(GL_POLYGON); //middle body part

	glColor3f(0.4902f, 0.5059f, 0.5020f);
	glVertex2f(-0.875f, 0.267f);//v
	glVertex2f(-0.4558f, 0.252f);//I
	glVertex2f(-0.5655f, 0.3049f);//j
	glVertex2f(-0.5684f, 0.3083f);
	glVertex2f(-0.5721f, 0.3120f);

	glVertex2f(-0.5760f, 0.3161f);//m1
	glVertex2f(-0.58f,  0.32f);
	glVertex2f(-0.5843f, 0.3240f);
	glVertex2f(-0.5882f, 0.3276f);
	glVertex2f(-0.5918f,  0.3310f);

	glVertex2f(-0.5966f, 0.3349f);//h1
	glVertex2f(-0.6017f, 0.3372f);
	glVertex2f(-0.6082f, 0.3397f);
	glVertex2f(-0.6135f, 0.3391f);
	glVertex2f(-0.6204f, 0.3385f);

	glVertex2f(-0.6263f, 0.3379f);//c1
	glVertex2f(-0.6322f, 0.3361f);
	glVertex2f(-0.6388f, 0.3338f);
	glVertex2f(-0.6445f, 0.3316f);
	glVertex2f(-0.6509f, 0.3283f);//k

	glVertex2f(-0.6520f, 0.32810f);//L2
	glVertex2f(-0.7329f, 0.3134f);
	glVertex2f(-0.7850f, 0.3281f);
	glVertex2f(-0.846f, 0.315f);//b4
	glVertex2f(-0.8744f, 0.3240f);
	glVertex2f(-0.8849f, 0.3227f);
	glVertex2f(-0.8944f, 0.3171f);
	glVertex2f(-0.8949f, 0.2735f);
	glVertex2f(-0.8852f, 0.2674f);//u
	glEnd();

    glBegin(GL_POLYGON);//Fin part
	glColor3f(0.6416f, 0.6573f, 0.6573f);
	glVertex2f(-0.785f, 0.3281f);//M
	glVertex2f(-0.8286f, 0.4231f);
	glVertex2f(-0.8497f, 0.4231f);
    glVertex2f(-0.8750f,  0.3556f);//p
    glVertex2f(-0.8744f, 0.324f);//Q
    glVertex2f(-0.846f, 0.315f);//b4
    glEnd();
    //Main fraim ends here

    glBegin(GL_POLYGON);//Pilot sit
	glColor3f(0.7416f, 0.7573f, 0.7573f);
    glVertex2f(-0.5655f, 0.3049f);//j
	glVertex2f(-0.5684f, 0.3083f);
	glVertex2f(-0.5721f, 0.3120f);

	glVertex2f(-0.5760f, 0.3161f);//m1
	glVertex2f(-0.58f,  0.32f);
	glVertex2f(-0.5843f, 0.3240f);
	glVertex2f(-0.5882f, 0.3276f);
	glVertex2f(-0.5918f,  0.3310f);

	glVertex2f(-0.5966f, 0.3349f);//h1
	glVertex2f(-0.6017f, 0.3372f);
	glVertex2f(-0.6082f, 0.3397f);
	glVertex2f(-0.6135f, 0.3391f);
	glVertex2f(-0.6204f, 0.3385f);

	glVertex2f(-0.6263f, 0.3379f);//c1
	glVertex2f(-0.6322f, 0.3361f);
	glVertex2f(-0.6388f, 0.3338f);
	glVertex2f(-0.6445f, 0.3316f);
	glVertex2f(-0.6509f, 0.3283f);//k

	glVertex2f(-0.6476f, 0.3234f);//P1
	glVertex2f( -0.6432f, 0.3195f);
	glVertex2f( -0.6382f, 0.3159f);
	glVertex2f( -0.6330f, 0.3128f);
	glVertex2f( -0.6268f, 0.3094f);//T1

	glVertex2f( -0.6219f, 0.3071f);//u1
	glVertex2f( -0.6164f, 0.3047f);
	glVertex2f( -0.6115f, 0.3029f);
	glVertex2f( -0.6056f, 0.3011f);//z1
	glVertex2f( -0.6f, 0.3f);//A2

	glVertex2f( -0.5948f, 0.2990f);//B2
	glVertex2f( -0.5896f, 0.2986f);
	glVertex2f( -0.5849f, 0.2983f);
	glVertex2f( -0.5808f, 0.2979f);
	glVertex2f( -0.5775f, 0.2985f);//F2

	glVertex2f( -0.5745f, 0.2991f);//G2
	glVertex2f( -0.5724f, 0.2994f);
	glVertex2f( -0.57f, 0.3f);
	glVertex2f( -0.5677f, 0.3013f);
	glVertex2f( -0.5664f, 0.3028f);//K2
    glEnd();

    glBegin(GL_POLYGON);//Black Vorder1
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(-0.6476f, 0.3234f);//P1
	glVertex2f( -0.6432f, 0.3195f);
	glVertex2f( -0.6382f, 0.3159f);
	glVertex2f( -0.6330f, 0.3128f);
	glVertex2f( -0.6298f, 0.3091f);//v2
	glVertex2f( -0.6326f, 0.3096f);//U2
	glVertex2f( -0.636f, 0.3108f);//s2
	glVertex2f( -0.6409f, 0.3144f);//Q2
	glVertex2f( -0.6451f, 0.3176f);//o2
	glVertex2f( -0.6483f, 0.3206f);
	glVertex2f( -0.6503f, 0.3243f);
	glVertex2f( -0.652f, 0.3281f);//L2
	glVertex2f( -0.6509f, 0.3283f);//K
    glEnd();

    glBegin(GL_POLYGON);//Black Vorder2
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f( -0.6056f, 0.3011f);//z1
	glVertex2f( -0.6f, 0.3f);//A2
	glVertex2f( -0.5948f, 0.2990f);//B2
	glVertex2f( -0.5896f, 0.2986f);//c2
	glVertex2f( -0.5889f, 0.2955f);
	glVertex2f( -0.5918f,  0.295f);//j3
	glVertex2f( -0.5957f, 0.2943f);
	glVertex2f( -0.6007f, 0.2947f);//h3
	glVertex2f( -0.6053f, 0.296f);//G3
	glVertex2f( -0.6096f, 0.2972f);//F3
	glVertex2f( -0.6140f, 0.2986f);//E3
	glVertex2f( -0.6172f, 0.3f);//D3
	glVertex2f( -0.6201f, 0.3014f);//C3
	glVertex2f( -0.623f, 0.3027f);//B3
	glVertex2f( -0.627f, 0.3057f);//Z2
	glVertex2f( -0.6298f, 0.3091f);//V2
	glVertex2f( -0.6268f, 0.3094f);//T1
	glVertex2f( -0.6219f, 0.3071f);//u1
	glVertex2f( -0.6164f, 0.3047f);//v1
	glVertex2f( -0.6115f, 0.3029f);//w1
    glEnd();

     glBegin(GL_POLYGON);//Black border3
	glColor3f(0.0f, 0.0f, 0.0f);

	glVertex2f( -0.5666f, 0.2999f);//Q3
	glVertex2f( -0.5708f, 0.2959f);//P3
	glVertex2f( -0.5744f, 0.2948f);//O3
	glVertex2f( -0.578f, 0.2931f);//N3
	glVertex2f( -0.5822f, 0.2927f);//M3
	glVertex2f( -0.586f, 0.2941f);//L3
	glVertex2f( -0.5889f, 0.2955f);//K3
	glVertex2f( -0.5896f, 0.2986f);//c2
	glVertex2f( -0.5849f, 0.2983f);
	glVertex2f( -0.5808f, 0.2979f);
	glVertex2f( -0.5775f, 0.2985f);//F2
	glVertex2f( -0.5745f, 0.2991f);//G2
	glVertex2f( -0.5724f, 0.2994f);//H2
	glVertex2f( -0.57f, 0.3f);//I2
	glVertex2f( -0.5677f, 0.3013f);//J2
	glVertex2f( -0.5664f, 0.3028f);//K2
	glVertex2f(-0.5655f, 0.3049f);//j
	glVertex2f( -0.566f, 0.302f);//s3
	glVertex2f( -0.5666f, 0.2999f);//R3
    glEnd();

    //Fin Rudder
    glBegin(GL_POLYGON);
    glColor3f(0.8039f, 0.9922f, 1.0000f);
    glVertex2f( -0.8733f, 0.3602f);//P
    glVertex2f( -0.86f, 0.36f);//E4
    glVertex2f( -0.8399f, 0.4162f);//D4
    glVertex2f( -0.8521f, 0.4166f);//C4
    glEnd();
    //Fin rectangle
    glBegin(GL_POLYGON);
    glColor3f(0.2000f, 0.6000f, 0.3961f);
    glVertex2f( -0.8379f, 0.4001f);//G4
    glVertex2f( -0.8259f, 0.4001f);//H4
    glVertex2f( -0.8259f, 0.3916f);//I4
    glVertex2f( -0.8379f, 0.3916f);//J4
    glEnd();

    //Blaster or ZET engine part
    glBegin(GL_POLYGON);//1
    glColor3f(0.1961f, 0.0000f, 0.0000f);
    glVertex2f( -0.8744f, 0.324f);//Q
    glVertex2f( -0.8849f, 0.3227f);//R
    glVertex2f( -0.8852f, 0.2674f);//U
    glVertex2f( -0.8751f, 0.2672f);//V
    glEnd();

    glBegin(GL_POLYGON);//2
    glColor3f(0.6000f, 0.0039f, 0.0000f);
    glVertex2f( -0.8849f, 0.3227f);//R
    glVertex2f( -0.8852f, 0.2674f);//U
    glVertex2f( -0.8949f, 0.2735f);//T
    glVertex2f( -0.8944f, 0.3171f);//S
    glEnd();

    glBegin(GL_LINES);//3
    glColor3f(0.0f, 0.0000f, 0.0000f);

    glVertex2f( -0.8945f, 0.3097f);//O4
    glVertex2f( -0.885f, 0.3139f);//P4

    glVertex2f( -0.8946f, 0.3004f);//Q4
    glVertex2f( -0.885f, 0.304f);//R4

    glVertex2f( -0.8947f, 0.2952f);//W4
    glVertex2f( -0.885f, 0.2942f);//Z4

    glVertex2f( -0.8947f, 0.2884f);//U4
    glVertex2f( -0.885f, 0.2842f);//V4

    glVertex2f( -0.8947f, 0.281f);//S4
    glVertex2f( -0.885f, 0.2747f);//T4
    glEnd();

    //MIDDLE HAND PART
    glBegin(GL_POLYGON);
    glColor3f(0.7294f, 0.7294f, 0.7294f);
    glVertex2f( -0.801f, 0.2622f);//G5
    glVertex2f( -0.74f, 0.25f);//F5
    glVertex2f( -0.5937f, 0.2596f);//N4
    glVertex2f( -0.6821f, 0.2909f);//L5
    glVertex2f( -0.74f, 0.2668f);//K5
    glVertex2f( -0.8f, 0.274f);//J5
    glVertex2f( -0.86f, 0.3f);//i5
    glVertex2f( -0.8687f, 0.2863f);//H5
    glEnd();

    //MESILE ON THE BODY
    glBegin(GL_POLYGON);//1
    glColor3f(0.5843f, 0.6824f, 0.7608f);
    glVertex2f( -0.7559f, 0.2327f);//Q5
    glVertex2f( -0.756f, 0.21f);//U5
    glVertex2f( -0.6686f, 0.2102f);//T5
    glVertex2f( -0.6564f, 0.2211f);//S5
    glVertex2f( -0.6688f, .2325f);//R5
    glEnd();
    glBegin(GL_LINES);//2
    glColor3f(0.0f, 0.0000f, 0.0000f);
    glVertex2f( -0.7559f, 0.2217f);//
    glVertex2f( -0.6564f, 0.2211f);//S5
    glEnd();

    //wheel base1
    glBegin(GL_POLYGON);
    glColor3f(0.7294f, 0.7294f, 0.7294f);
    glVertex2f( -0.7929f, 0.2f);//A6
    glVertex2f( -0.793f, 0.1801f);//B6
    glVertex2f( -0.7872f, 0.1799f);//C6
    glVertex2f( -0.7871f, 0.2f);//D6
    glVertex2f( -0.7768f, 0.2321f);//C5
    glVertex2f( -0.7991f, 0.2321f);//B5
    glVertex2f( -0.8051f, 0.2159f);//A5
    glVertex2f( -0.8f, 0.2f);//
    glEnd();
    //wheel base2
    glBegin(GL_POLYGON);
    glColor3f(0.7294f, 0.7294f, 0.7294f);
    glVertex2f( -0.6085f, 0.2f);//H6
    glVertex2f( -0.6146f, 0.2f);//E6
    glVertex2f( -0.6085f, 0.1799f);//F6
    glVertex2f( -0.6047f, 0.1799f);//G6
    glEnd();
// Example usage of drawCircle(x, y, radius, segments, r, g, b)
    zdrawFilledCircle(-0.60649f, 0.17805f, 0.0099f, 100, 0.0f, 0.0f, 0.0f); // Front Wheel
    zdrawFilledCircle(-0.79f, 0.178f, 0.0099f, 100, 0.0f, 0.0f, 0.0f); // Back wheel

    //glVertex2f( f, f);//
	glFlush();  // Render now
	  glPopMatrix();
}

// Draw clouds

void zdrawCloud(const Vec2& position, float scale) {
    const int segments = 40;
    const float baseRadius = 0.1f * scale;
    glColor3f(0.7f, 0.9f, 1.0f);
    float offsets[5][2] = { {-0.15f,0},
                            {-0.05f,0.05f},
                            {0.05f,0},
                            {0.15f,0.05f},
                            {0, -0.05f}};
    float radii[5] = {1.0f,0.8f,1.0f,0.8f,0.9f};
    for (int i = 0; i < 5; ++i) {
        Vec2 c = { position.x + offsets[i][0] * scale,
                   position.y + offsets[i][1] * scale };
        float r = baseRadius * radii[i];
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(c.x, c.y);
        for (int j = 0; j <= segments; ++j) {
            float angle = 2 * M_PI * j / segments;
            glVertex2f(c.x + cosf(angle) * r,
                       c.y + sinf(angle) * r);
        }
        glEnd();
    }
}

void ZdrawClouds() {
    zdrawCloud({-0.7f,0.8f}, 1.0f);
    zdrawCloud({ 0.0f,0.85f},0.9f);
    glPushMatrix();
        glTranslatef(-0.15f, -0.1f, 0.0f);
            zdrawCloud({ 0.7f,0.75f},1.1f);
    glPopMatrix();
}

// Draw ground

void ZdrawGround() {
    glColor3f(0.3f,0.5f,0.2f);
    glBegin(GL_QUADS);
      glVertex2f(-1,-0.6);
      glVertex2f( -1,-0.91);
      glVertex2f( 1, -0.91);
      glVertex2f(1, -0.6);
    glEnd();
}

// Draw road

void ZdrawRoads() {
    glColor3f(0.1f,0.1f,0.1f);
    glBegin(GL_QUADS);
        glVertex2f(-1,-0.89f);
        glVertex2f( 1,-0.89f);
        glVertex2f( 1,-0.8f);
        glVertex2f(-1,-0.8f);
    glEnd();
    glColor3f(1,1,1);
    const float dashLen=0.1f, dashH=0.012f, gap=0.04f;
    for (float x=-1.f; x<1.0f; x= x+dashLen+gap) {
        glBegin(GL_QUADS);
          glVertex2f(x,           -0.85f);
          glVertex2f(x+dashLen,   -0.85f);
          glVertex2f(x+dashLen,   -0.85f+dashH);
          glVertex2f(x,           -0.85f+dashH);
        glEnd();
    }
}

//permanent Building

void PBuilding(){
    glColor3f(0.6f, 0.6f, 0.65f);
    glLineWidth(1);

glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);//bUILDING LEFT 7
    glVertex2f( 0.728f, -0.8);//F2
    glVertex2f( 0.728f, -0.67f);//G2
    glVertex2f( 0.784f, -0.67f);//H2
    glVertex2f( 0.784f, -0.8f);//A2
    glEnd();

glBegin(GL_POLYGON);//Builfing Left 1
    glVertex2f( -0.882f, -0.662f);//M
    glVertex2f( -0.881f, -0.455f);//N
    glVertex2f( -0.74f, -0.45f);//O
    glVertex2f( -0.74f, -0.8f);//P
    glVertex2f( -0.912f, -0.801f);//I
    glVertex2f( -0.911f, -0.663f);//J
    glEnd();
glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);//Builfing Left 1 bORDER
    glVertex2f( -0.882f, -0.662f);//M
    glVertex2f( -0.881f, -0.455f);//N
    glVertex2f( -0.74f, -0.45f);//O
    glVertex2f( -0.74f, -0.8f);//P
    glVertex2f( -0.912f, -0.801f);//I
    glVertex2f( -0.911f, -0.663f);//J
    glEnd();

glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);//Builfing Left 2
    glVertex2f( 0.427f, -0.668f);//K3
    glVertex2f( 0.426f, -0.595f);//J3
    glVertex2f( 0.457f, -0.603f);//I3
    glVertex2f( 0.456f, -0.8f);//
    glVertex2f( 0.4f, -0.8f);//M3
    glVertex2f( 0.4f, -0.668f);//L3
    glEnd();
glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);//Builfing Left 2 bORDER
    glVertex2f( 0.4f, -0.8f);//M3
    glVertex2f( 0.4f, -0.668f);//L3
    glVertex2f( 0.427f, -0.668f);//K3
    glVertex2f( 0.426f, -0.595f);//J3
    glVertex2f( 0.457f, -0.603f);//I3
    glVertex2f( 0.456f, -0.8f);//H3
    glEnd();

glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);//Builfing Left 3
    glVertex2f( 0.456f, -0.8f);//H3
    glVertex2f( 0.457f, -0.543f);//G3
    glVertex2f( 0.476f, -0.543f);//F3
    glVertex2f( 0.476f, -0.8f);//P2
    glEnd();
glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);//Builfing Left 3 bORDER
    glVertex2f( 0.456f, -0.8f);//H3
    glVertex2f( 0.457f, -0.543f);//G3
    glVertex2f( 0.476f, -0.543f);//F3
    glVertex2f( 0.476f, -0.8f);//P2
    glEnd();
glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);//Builfing Left 5
    glVertex2f( 0.565f, -0.798f);//W2
    glVertex2f( 0.565f, -0.682f);//Z2
    glVertex2f( 0.59f, -0.682f);//A3
    glVertex2f( 0.59f, -0.8f);//I2
    glEnd();

glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);//Builfing Left 4
    glVertex2f(0.497f, -0.49f);//R2
    glVertex2f( 0.497f, -0.442f);//S2
    glVertex2f( 0.546f, -0.442f);//T2
    glVertex2f( 0.546f, -0.485f);//U2
    glVertex2f( 0.566f, -0.486f);//V2
    glVertex2f( 0.565f, -0.798f);//W2
    glVertex2f( 0.476f, -0.8f);//P2
    glVertex2f( 0.476f, -0.491f);//Q2
    glEnd();
glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);//Builfing Left 4 bORDER
    glVertex2f(0.497f, -0.49f);//R2
    glVertex2f( 0.497f, -0.442f);//S2
    glVertex2f( 0.546f, -0.442f);//T2
    glVertex2f( 0.546f, -0.485f);//U2
    glVertex2f( 0.566f, -0.486f);//V2
    glVertex2f( 0.565f, -0.798f);//W2
    glVertex2f( 0.476f, -0.8f);//P2
    glVertex2f( 0.476f, -0.491f);//Q2
    glEnd();

glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);//bUILDING LEFT 6
    glVertex2f( 0.612f, -0.518f);//K2
    glVertex2f( 0.612f, -0.494f);//L2
    glVertex2f( 0.704f, -0.493f);//M2
    glVertex2f( 0.704f, -0.518f);//N2
    glVertex2f( 0.7286f, -0.518f);//O2
    glVertex2f( 0.728f, -0.8);//F2
    glVertex2f( 0.59f, -0.8f);//I2
    glVertex2f( 0.59f, -0.518f);//
    glEnd();
glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);//bUILDING LEFT 6 BORDER
    glVertex2f( 0.612f, -0.518f);//K2
    glVertex2f( 0.612f, -0.494f);//L2
    glVertex2f( 0.704f, -0.493f);//M2
    glVertex2f( 0.704f, -0.518f);//N2
    glVertex2f( 0.7286f, -0.518f);//O2
    glVertex2f( 0.728f, -0.8);//F2
    glVertex2f( 0.59f, -0.8f);//I2
    glVertex2f( 0.59f, -0.518f);//J2
    glEnd();



glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);//bUILDING LEFT 8
    glVertex2f( 0.8f, -0.7f);//W1
    glVertex2f( 0.799f, -0.482f);//V1
    glVertex2f( 0.834f, -0.437f);//U1
    glVertex2f( 0.87f, -0.483f);//T1
    glVertex2f( 0.87f,  -0.7f);//S1
    glVertex2f( 0.885f, -0.7f);//R1
    glVertex2f( 0.885f, -0.8f);//L1
    glVertex2f( 0.784f, -0.8f);//A2
    glVertex2f( 0.784f, -0.7f);//Z1
    glEnd();

    glColor3f(0.8078f, 1.0000f, 0.9765f);
    glBegin(GL_POLYGON);//8 INSIDE
    glVertex2f( 0.82f, -0.78f);//C2
    glVertex2f( 0.85f, -0.78f);//D2
    glVertex2f( 0.85f, -0.5f);//E2
    glVertex2f( 0.82f, -0.5f);//B2
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f( 0.82f, -0.78f);//C2
    glVertex2f( 0.85f, -0.78f);//D2
    glVertex2f( 0.85f, -0.5f);//E2
    glVertex2f( 0.82f, -0.5f);//B2
    glEnd();
    glBegin(GL_LINES);
    glVertex2f( 0.83f, -0.77f);//L6
    glVertex2f( 0.83f, -0.52f);//K6
    glVertex2f( 0.84f, -0.52f);//N6
    glVertex2f( 0.84f, -0.77f);//M6
    glEnd();
glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);//bUILDING LEFT 8 BORDER
    glVertex2f( 0.8f, -0.7f);//W1
    glVertex2f( 0.799f, -0.482f);//V1
    glVertex2f( 0.834f, -0.437f);//U1
    glVertex2f( 0.87f, -0.483f);//T1
    glVertex2f( 0.87f,  -0.7f);//S1
    glVertex2f( 0.885f, -0.7f);//R1
    glVertex2f( 0.885f, -0.8f);//L1
    glVertex2f( 0.784f, -0.8f);//A2
    glVertex2f( 0.784f, -0.7f);//Z1
    glEnd();
glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);//bUILDING LEFT 9
    glVertex2f( 0.922f, -0.457f);//O1
    glVertex2f( 0.923f, -0.36f);//P1
    glVertex2f( 1.0f, -0.36f);//Q1
    glVertex2f( 1.0f, -0.8f);//L
    glVertex2f( 0.885f, -0.799f);//L1
    glVertex2f( 0.885f, -0.457f);//M1
    glEnd();
glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);//bUILDING LEFT 9 BORDER
    glVertex2f( 0.885f, -0.799f);//L1
    glVertex2f( 0.885f, -0.457f);//M1

    glVertex2f( 0.885f, -0.457f);//M1
    glVertex2f( 0.922f, -0.457f);//O1

    glVertex2f( 0.922f, -0.457f);//O1
    glVertex2f( 0.923f, -0.36f);//P1

    glVertex2f( 0.923f, -0.36f);//P1
    glVertex2f( 1.0f, -0.36f);//Q1
    glEnd();
    ////////////////////////////////////Windows For Building 9
        glPushMatrix();
            glTranslated(0.9199,-0.4999,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated(0.9199,-0.5599,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated(0.9199,-0.6358,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated(0.9199,-0.6999,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated(0.9199,-0.7601,0.0);
            window1_c1();
        glPopMatrix();
        //right
        glPushMatrix();
            glTranslated(0.9803,-0.4999,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated(0.9803,-0.5599,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated(0.9803,-0.6358,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated(0.9803,-0.6999,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated(0.9803,-0.7601,0.0);
            window1_c1();
        glPopMatrix();
        ////////////////////////////////////Windows For Building 1
        glPushMatrix();
            glTranslated( -0.84,-0.5 ,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( -0.84,-0.55 ,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( -0.84, -0.6,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( -0.84,-0.65 ,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( -0.84, -0.7,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( -0.84,-0.75 ,0.0);
            window1_c1();
        glPopMatrix();
        //right
        glPushMatrix();
            glTranslated( -0.78,-0.5 ,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( -0.78,-0.55 ,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( -0.78, -0.6,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( -0.78,-0.65 ,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( -0.78, -0.7,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( -0.78,-0.75 ,0.0);
            window1_c1();
        glPopMatrix();
        ////////////////////////////////////Windows For Building 6
        glPushMatrix();
            glTranslated( 0.629, -0.579,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.629, -0.639,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.629, -0.698,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.629, -0.758,0.0);
            window1_c1();
        glPopMatrix();
        //right
        glPushMatrix();
            glTranslated( 0.691, -0.579,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.691, -0.639,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.691, -0.698,0.0);
            window1_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.691, -0.758,0.0);
            window1_c1();
        glPopMatrix();
        ////////////////////////////////////Windows For Building 4

        glPushMatrix();
            glTranslated( 0.498, -0.514,0.0);
            window101_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.498, -0.579,0.0);
            window101_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.498, -0.64,0.0);
            window101_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.498, -0.7,0.0);
            window101_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.498, -0.759,0.0);
            window101_c1();
        glPopMatrix();
        //right
        glPushMatrix();
            glTranslated( 0.539, -0.514,0.0);
            window101_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.539, -0.579,0.0);
            window101_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.539, -0.64,0.0);
            window101_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.539, -0.7,0.0);
            window101_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.539, -0.759,0.0);
            window101_c1();
        glPopMatrix();

        ////////////////////////////////////Windows For Building 2
        glPushMatrix();
            glTranslated( 0.412, -0.713,0.0);
            window10_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.412, -0.771,0.0);
            window10_c1();
        glPopMatrix();
        //right
        glPushMatrix();
            glTranslated( 0.44, -0.713,0.0);
            window10_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.44, -0.771,0.0);
            window10_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( 0.44, -0.634,0.0);
            window10_c1();
        glPopMatrix();



}

void P2Building(){
    glColor3f(0.6f, 0.6f, 0.65f);
    glLineWidth(1);

    //p2 1
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( -0.74f, -0.8f);//p
    glVertex2f( -0.737f, -0.35f);//q
    glVertex2f( -0.625f, -0.443f);//s5
    glVertex2f( -0.625f, -0.799f);//s
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( -0.74f, -0.8f);//p
    glVertex2f( -0.737f, -0.35f);//q
    glVertex2f( -0.625f, -0.443f);//s5
    glVertex2f( -0.625f, -0.799f);//s
    glEnd();

    glPushMatrix();
            glTranslated( -0.68, -0.46 ,0.0);
            window1_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.68, -0.56 ,0.0);
            window1_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.68, -0.64 ,0.0);
            window1_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.68, -0.74 ,0.0);
            window1_c1();
    glPopMatrix();
////////////////////////////////////////////////////////////////////////////////
    //p2 2
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( -0.529f, -0.564f);//q5
    glVertex2f( -0.625f, -0.443f);//s5
    glVertex2f( -0.625f, -0.799f);//s
    glVertex2f( -0.45f, -0.8f);//o5
    glVertex2f( -0.45f, -0.5f);//p5
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);

    glVertex2f( -0.625f, -0.443f);//s5
    glVertex2f( -0.625f, -0.799f);//s
    glVertex2f( -0.45f, -0.8f);//o5
    glVertex2f( -0.45f, -0.5f);//p5
    glEnd();
    glPushMatrix();
            glTranslated( -0.58, -0.6 ,0.0);
            window1_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.5, -0.65 ,0.0);
            window1_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.58, -0.7 ,0.0);
            window1_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.5, -0.75 ,0.0);
            window1_c1();
    glPopMatrix();
////////////////////////////////////////////////////////////////////////////////
    //p2 3
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( -0.452f, -0.595f);//n5
    glVertex2f( -0.350f, -0.662f);//l5
    glVertex2f( -0.281f, -0.728f);//f5
    glVertex2f( -0.282f, -0.8f);//e5
    glVertex2f( -0.45f, -0.8f);//o5
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( -0.452f, -0.595f);//n5
    glVertex2f( -0.45f, -0.8f);//o5

    glVertex2f( -0.281f, -0.728f);//f5
    glVertex2f( -0.282f, -0.8f);//e5

    glEnd();
    glPushMatrix();
            glTranslated( -0.42, -0.74 ,0.0);
            window101_c1();
        glPopMatrix();
        glPushMatrix();
            glTranslated( -0.34, -0.74 ,0.0);
            window101_c1();
        glPopMatrix();
////////////////////////////////////////////////////////////////////////////////
    //p2 4
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( -0.281f, -0.728f);//f5
    glVertex2f( -0.282f, -0.8f);//e5
    glVertex2f( -0.123f, -0.798f);//b5
    glVertex2f( -0.123f, -0.735f);//g5
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( -0.281f, -0.728f);//f5
    glVertex2f( -0.282f, -0.8f);//e5
    glVertex2f( -0.123f, -0.798f);//b5
    glVertex2f( -0.123f, -0.735f);//g5
    glEnd();
    glPushMatrix();
            glTranslated( -0.2082, -0.7682 ,0.0);
            window101_c1();
        glPopMatrix();
////////////////////////////////////////////////////////////////////////////////
    //p2 5
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( -0.123f, -0.798f);//b5
    glVertex2f( -0.052f, -0.728f);//n4
    glVertex2f( -0.053f, -0.797f);//m4
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f( -0.052f, -0.728f);//n4
    glVertex2f( -0.053f, -0.797f);//m4
    glEnd();
////////////////////////////////////////////////////////////////////////////////
    //p2 6
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( 0.0026f, -0.728f);//o4
    glVertex2f( -0.052f, -0.728f);//n4
    glVertex2f( -0.053f, -0.797f);//m4
    glVertex2f( 0.124f, -0.8f);//i4
    glVertex2f( 0.125f, -0.665f);//j4
    glVertex2f( 0.068f, -0.684f);//q4
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( -0.052f, -0.728f);//n4
    glVertex2f( -0.053f, -0.797f);//m4
    glVertex2f( 0.124f, -0.8f);//i4
    glVertex2f( 0.125f, -0.665f);//j4

    glEnd();
    glPushMatrix();
            glTranslated( 0.06, -0.74 ,0.0);
            window101_c1();
        glPopMatrix();
////////////////////////////////////////////////////////////////////////////////
    //p2 7
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( 0.124f, -0.8f);//i4
    glVertex2f( 0.125f, -0.665f);//j4
    glVertex2f( 0.237f, -0.621f);//d4
    glVertex2f( 0.235f, -0.8f);//c4
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( 0.124f, -0.8f);//i4
    glVertex2f( 0.125f, -0.665f);//j4
    glVertex2f( 0.237f, -0.621f);//d4
    glVertex2f( 0.235f, -0.8f);//c4
    glEnd();
    glPushMatrix();
            glTranslated( 0.1799, -0.6883 ,0.0);
            window101_c1();
        glPopMatrix();
    glPushMatrix();
            glTranslated( 0.18, -0.76 ,0.0);
            window101_c1();
        glPopMatrix();
////////////////////////////////////////////////////////////////////////////////
     //p2 8
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( 0.237f, -0.621f);//d4
    glVertex2f( 0.235f, -0.8f);//c4
    glVertex2f( 0.337f, -0.799f);//r3
    glVertex2f( 0.338f, -0.544f);//s3
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( 0.237f, -0.621f);//d4
    glVertex2f( 0.235f, -0.8f);//c4
    glVertex2f( 0.337f, -0.799f);//r3
    glVertex2f( 0.338f, -0.544f);//s3
    glEnd();
    glPushMatrix();
            glTranslated( 0.29, -0.64 ,0.0);
            window101_c1();
        glPopMatrix();
    glPushMatrix();
            glTranslated( 0.29, -0.7 ,0.0);
            window101_c1();
        glPopMatrix();
    glPushMatrix();
            glTranslated( 0.29, -0.76 ,0.0);
            window101_c1();
        glPopMatrix();
////////////////////////////////////////////////////////////////////////////////
    //p2 9
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( 0.381f,  -0.527f);//v3
    glVertex2f( 0.338f, -0.544f);//s3
    glVertex2f( 0.337f, -0.799f);//r3
    glVertex2f( 0.394f, -0.79f);//z3
    glVertex2f( 0.395f, -0.485f);//w3
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( 0.338f, -0.544f);//s3
    glVertex2f( 0.337f, -0.8f);//r3
    glVertex2f( 0.394f, -0.8f);//z3
    glVertex2f( 0.395f, -0.485f);//w3
    glEnd();
    glPushMatrix();
            glTranslated( 0.365, -0.58 ,0.0);
            window10_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( 0.365, -0.64 ,0.0);
            window10_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( 0.365, -0.7 ,0.0);
            window10_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( 0.365, -0.76 ,0.0);
            window10_c1();
    glPopMatrix();
}

void TBuildingT1_1(){
    glPushMatrix();
    glTranslatef(0.0f , trY , 0.0f);
    //T1_1
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( -0.737f, -0.35f);//q
    glVertex2f( -0.625f, -0.443f);//s5
    glVertex2f( -0.625f, -0.35f);//r3
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( -0.737f, -0.35f);//q
    glVertex2f( -0.625f, -0.35f);//r3
    glVertex2f( -0.625f, -0.35f);//r3
    glVertex2f( -0.625f, -0.443f);//s5
    glEnd();
glPopMatrix();
}

void TBuildingT2_1(){
    //T1_1
glPushMatrix();
    //glScalef(scX, scY, 1.0f);
    glTranslatef(0.0f , trY , 0.0f);

    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( -0.625f, -0.443f);//s5
    glVertex2f( -0.625f, -0.374f);//u5
    glVertex2f( -0.45f,  -0.371f);//t5
    glVertex2f( -0.45f, -0.5f);//p5
    glVertex2f( -0.529f, -0.564f);//q5
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( -0.625f, -0.443f);//s5
    glVertex2f( -0.625f, -0.374f);//u5
    glVertex2f( -0.625f, -0.374f);//u5
    glVertex2f( -0.45f,  -0.371f);//t5
    glVertex2f( -0.45f,  -0.371f);//t5
    glVertex2f( -0.45f, -0.5f);//p5
    glEnd();

    glPushMatrix();
            glTranslated( -0.584, -0.4272 ,0.0);
            window1_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.4890, -0.4865 ,0.0);
            window1_c1();
    glPopMatrix();
glPopMatrix();
}

void TBuildingT3_1(){
    glPushMatrix();
    glTranslatef(0.0f , trY , 0.0f);
    //T1_1
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( -0.367f, -0.241f);//w5
    glVertex2f( -0.448f, -0.241f);//v5
    glVertex2f( -0.452f, -0.595f);//n5
    glVertex2f( -0.350f, -0.662f);//l5
    glVertex2f( -0.281f, -0.728f);//f5
    glVertex2f( -0.28f, -0.192f);//a6
    glVertex2f( -0.365f, -0.191f);//z5
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( -0.367f, -0.241f);//w5
    glVertex2f( -0.448f, -0.241f);//v5
    glVertex2f( -0.448f, -0.241f);//v5
    glVertex2f( -0.452f, -0.595f);//n5
    glVertex2f( -0.281f, -0.728f);//f5
    glVertex2f( -0.28f, -0.192f);//a6
    glVertex2f( -0.28f, -0.192f);//a6
    glVertex2f( -0.365f, -0.191f);//z5
    glVertex2f( -0.365f, -0.191f);//z5
    glVertex2f( -0.367f, -0.241f);//w5
    glEnd();

    glPushMatrix();
            glTranslated( -0.4, -0.55 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.3262805108016, -0.5498586262095 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.4, -0.45 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.3219498419325, -0.4491705750021 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.4, -0.35 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.3203258411065, -0.3492945242077 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.3262791485867, -0.2506898848908 ,0.0);
            window101_c1();
    glPopMatrix();
    glPopMatrix();
}

void TBuildingT4_1(){
    glPushMatrix();
    glTranslatef(0.0f , trY , 0.0f);
    //T1_1
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( -0.2817f, -0.728f);//f5
    glVertex2f( -0.2804f, -0.4038f);//j5
    glVertex2f( -0.1268f, -0.399f);//k5
    glVertex2f( -0.1232f, -0.7353f);//g5
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( -0.2817f, -0.728f);//f5
    glVertex2f( -0.2804f, -0.4038f);//j5
    glVertex2f( -0.2804f, -0.4038f);//j5
    glVertex2f( -0.1268f, -0.399f);//k5
    glVertex2f( -0.1268f, -0.399f);//k5
    glVertex2f( -0.1232f, -0.7353f);//g5
    glEnd();

    glPushMatrix();
            glTranslated( -0.2036016367424, -0.4580053618672 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.2029047202714, -0.5322 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.2, -0.6 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.2029047202714, -0.6754 ,0.0);
            window101_c1();
    glPopMatrix();
    glPopMatrix();
}

void TBuildingT5_1(){
    glPushMatrix();
    glTranslatef(0.0f , trY , 0.0f);
    //T1_1
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( -0.123f, -0.798f);//b5
    glVertex2f( -0.0524f, -0.7280f);//n5
    glVertex2f( -0.0505f, -0.6278f);//d5
    glVertex2f( -0.1230f, -0.627f);//c5
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( -0.123f, -0.798f);//b5
    glVertex2f( -0.1230f, -0.627f);//c5
    glVertex2f( -0.0524f, -0.7280f);//n5
    glVertex2f( -0.0505f, -0.6278f);//d5
    glVertex2f( -0.0505f, -0.6278f);//d5
    glVertex2f( -0.1230f, -0.627f);//c5
    glEnd();

    glPushMatrix();
            glTranslated( -0.0900583617603, -0.665 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( -0.0900583617603, -0.7281 ,0.0);
            window101_c1();
    glPopMatrix();
    glPopMatrix();
}

void TBuildingT6_1(){
    glPushMatrix();
    glTranslatef(0.0f , trY , 0.0f);
    //T1_1
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( 0.00264f, -0.728f);//o4
    glVertex2f( 0.0680f, -0.684f);//q4
    glVertex2f( 0.0688f, -0.4916f);//v4
    glVertex2f( -0.0507f,  -0.490505f);//t4
    glVertex2f( -0.0524f,  -0.728f);//n4
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( 0.0680f, -0.684f);//q4
    glVertex2f( 0.0688f, -0.4916f);//v4
    glVertex2f( 0.0688f, -0.4916f);//v4
    glVertex2f( -0.0507f,  -0.490505f);//t4
    glVertex2f( -0.0507f,  -0.490505f);//t4
    glVertex2f( -0.0524f,  -0.728f);//n4
    glEnd();

    glPushMatrix();
            glTranslated( 0.005, -0.54 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( 0.005, -0.6 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( 0.005, -0.66 ,0.0);
            window101_c1();
    glPopMatrix();
    glPopMatrix();
}

void TBuildingT7_1(){
    glPushMatrix();
    glTranslatef(0.0f , trY , 0.0f);
    //T1_1
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( 0.12536f, -0.6650f);//j4
    glVertex2f( 0.237f, -0.6219f);//d4
    glVertex2f( 0.237f, -0.290f);//l4
    glVertex2f( 0.1269f, -0.415f);//k4
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( 0.237f, -0.6219f);//d4
    glVertex2f( 0.237f, -0.290f);//l4
    glVertex2f( 0.237f, -0.290f);//l4
    glVertex2f( 0.1269f, -0.415f);//k4
    glVertex2f( 0.1269f, -0.415f);//k4
    glVertex2f( 0.12536f, -0.6650f);//j4
    glEnd();

    glPushMatrix();
            glTranslated( 0.18, -0.45 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( 0.18, -0.53 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( 0.18, -0.59 ,0.0);
            window101_c1();
    glPopMatrix();
    glPopMatrix();
}

void TBuildingT8_1(){
    glPushMatrix();
    glTranslatef(0.0f , trY , 0.0f);
    //T1_1
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( 0.2831f, -0.586f);//e4
    glVertex2f( 0.3387f, -0.544f);//s3
    glVertex2f( 0.339f, -0.4319f);//g4
    glVertex2f( 0.2360f, -0.433f);//h4
    glVertex2f( 0.23702f, -0.6219f);//d4
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( 0.3387f, -0.544f);//s3
    glVertex2f( 0.339f, -0.4319f);//g4
    glVertex2f( 0.339f, -0.4319f);//g4
    glVertex2f( 0.2360f, -0.433f);//h4
    glVertex2f( 0.2360f, -0.433f);//h4
    glVertex2f( 0.23702f, -0.6219f);//d4
    glEnd();

    glPushMatrix();
            glTranslated( 0.2842302692056, -0.4873 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( 0.2845414325405, -0.5293 ,0.0);
            window101_c1();
    glPopMatrix();
    glPopMatrix();
}

void TBuildingT9_1(){
    glPushMatrix();
    glTranslatef(0.0f , trY , 0.0f);
    //T1_1
    glColor3f(0.6f, 0.6f, 0.65f);
    glBegin(GL_POLYGON);
    glVertex2f( 0.338f, -0.544f);//s3
    glVertex2f( 0.381f,  -0.527f);//v3
    glVertex2f( 0.395f, -0.485f);//w3
    glVertex2f( 0.398f, -0.3821f);//b4
    glVertex2f( 0.339f, -0.3821648f);//a4
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f( 0.395f, -0.485f);//w3
    glVertex2f( 0.398f, -0.3821f);//b4
    glVertex2f( 0.398f, -0.3821f);//b4
    glVertex2f( 0.339f, -0.3821648f);//a4
    glVertex2f( 0.339f, -0.3821648f);//a4
    glVertex2f( 0.338f, -0.544f);//s3
    glEnd();

    glPushMatrix();
            glTranslated( 0.369089750339, -0.4266 ,0.0);
            window101_c1();
    glPopMatrix();
    glPushMatrix();
            glTranslated( 0.3680363898412, -0.484316 ,0.0);
            window101_c1();
    glPopMatrix();
    glPopMatrix();
}

void anikupdate(int value) {

    if (ZtakeoffStarted)
    {
        if (ZplaneX < ZtakeoffThresholdX) {
            // Still on runway
            ZplaneX += 0.01f;
        } else {
            // Take off diagonally
            ZplaneX += 0.02f;
            ZplaneY += 0.005f;
        }

        // Stop flying after reaching desired height or distance
        if (ZplaneY > 0.5f || ZplaneX > 1.9f) {
            ZtakeoffStarted = false;  // ❗Do not reset position
            trY = -0.54f;
            scX = 0.50f;
            scY = 0.20f;
        }
    }
        //cloudOffset += 0.0003f; // Slow speed, adjust if needed
        //if (cloudOffset > 2.0f)  // Reset after drifting far right
          //  cloudOffset = -2.0f;
     if (bombStarted)
    {
        if (bombX < bombThresholdX) {
            // Still on runway
            bombX += 0.01f;
        } else {
            // Take off diagonally
            bombX += 0.0004f;
            bombY -= 0.009f;
        }
        if(bombY >= -0.02){
            explosionTime=0;
            showExplosion = true;
        }

        // Stop flying after reaching desired height or distance

    }

    // Explosion timing
        if (showExplosion)
        {
            explosionTime += 0.02f;
            if (explosionTime > 2.8f)
            {
                showExplosion = false;
            }
        }

    glutPostRedisplay();
    glutTimerFunc(16, anikupdate, 0);
}

void anikDisplay(){
    glClear(GL_COLOR_BUFFER_BIT);   // Clear the color buffer with current clearing color

    ZdrawSky();
    ZdrawClouds();
    ZdrawGround();
    ZdrawRoads();
    PBuilding();
    P2Building();
    TBuildingT1_1();
    TBuildingT2_1();
    TBuildingT3_1();
    TBuildingT4_1();
    TBuildingT5_1();
    TBuildingT6_1();
    TBuildingT7_1();
    TBuildingT8_1();
    TBuildingT9_1();
    Zdrawbomb();
	ZdrawPlane();


    if (showExplosion)
        drawExplosion(0.5f, 0.0f, explosionTime);
	glFlush();  // Render now
}

/////////////////  ANIK PART END  //////////////////////////////////////

void drawStars() {
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    glColor4f(1.0f, 1.0f, 1.0f, starAlpha);
    for(int i = 0; i < 100; i++) {
        float x = (float) ( (i * 1234) % 2000 - 1000) / 1000.0f;
        float y = (float) ( (i * 5678) % 1000) / 1000.0f;
        if(y > 0.1) glVertex2f(x, y); //  draw stars in the upper part of the sky
    }
    glEnd();
    glPointSize(1.0f);
}

void drawGradientSky() {
    drawStars(); // Draw stars
    glBegin(GL_QUADS);

    glColor4f(skyRed * 0.7f, skyGreen * 0.7f, skyBlue * 0.9f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);

    glColor4f(skyRed, skyGreen, skyBlue, 1.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(-1.0f, 0.0f);
    glEnd();
}

void SdrawCircle(float cx, float cy, float r, float red, float green, float blue, float alpha) {
    glColor4f(red, green, blue, alpha);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 50; ++i) {
        float angle = 2.0f * PI * i / 50;
        glVertex2f(cx + r * cos(angle), cy + r * sin(angle));
    }
    glEnd();
}

void drawSunAndMoon() {

    float celestialX = 1.0f * cos(sunMoonAngle * PI / 180.0f);
    float celestialY = 0.8f * sin(sunMoonAngle * PI / 180.0f);

    if (sunAlpha > 0.5f) {
        SdrawCircle(celestialX, celestialY, 0.1f, 1.0f, 0.9f, 0.0f, sunAlpha); // Sun color
    } else {
        SdrawCircle(celestialX, celestialY, 0.08f, 0.9f, 0.9f, 0.9f, starAlpha); // Moon color
    }
}

void SdrawCloud(float x, float y) {
    SdrawCircle(x, y, 0.07f, 1.0f, 1.0f, 1.0f, 0.8f);
    SdrawCircle(x + 0.06f, y + 0.02f, 0.07f, 1.0f, 1.0f, 1.0f, 0.8f);
    SdrawCircle(x - 0.06f, y + 0.02f, 0.07f, 1.0f, 1.0f, 1.0f, 0.8f);
}

void drawWaves() {
    // Base ocean color
    glColor3f(0.0f, 0.4f * (sunAlpha * 0.7f + 0.3f), 0.8f * (sunAlpha * 0.7f + 0.3f));
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(-1.0f, 0.0f);
    glEnd();

    // Wave ripples
    glColor4f(0.2f, 0.6f, 1.0f, sunAlpha * 0.5f + 0.5f);
    glBegin(GL_TRIANGLE_STRIP);
    for (float x = -1.0f; x <= 1.05f; x += 0.02f) {
        float y = 0.02f * sin(10.0f * x + waveOffset);
        glVertex2f(x, 0.0f + y);
        glVertex2f(x, -0.05f + y);
    }
    glEnd();
}

void drawRotatingSearchlight(float cx, float cy) {
    float beamLength = 1.5f;
    float angleRad = searchlightAngle * PI / 180.0f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Enable blending
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 1.0f, 0.8f, 0.0f);
    glColor4f(1.0f, 1.0f, 0.8f, 0.2f * lightGlow); // Fading color
    glVertex2f(cx, cy);
    glVertex2f(cx + beamLength * cos(angleRad - 0.02f), cy + beamLength * sin(angleRad - 0.02f));
    glVertex2f(cx + beamLength * cos(angleRad + 0.02f), cy + beamLength * sin(angleRad + 0.02f));
    glEnd();
    glDisable(GL_BLEND);
}

// Draws the fighter jet using multiple polygons

void drawFighterJet() {

    glBegin(GL_POLYGON);
    glColor3f(0.1451f, 0.2235f, 0.2157f);
    glVertex2f(-0.875f, 0.267f);
    glVertex2f(-0.843f, 0.228f);
    glVertex2f(-0.8f, 0.2f);
    glVertex2f(-0.6f, 0.2f);
    glVertex2f(-0.591f, 0.207f);
    glVertex2f(-0.5960f, 0.2147f);
    glVertex2f(-0.4558f, 0.252f);
    glEnd();

    // Main fuselage and wing section
    glBegin(GL_POLYGON);
    glColor3f(0.4902f, 0.5059f, 0.5020f);
    glVertex2f(-0.875f, 0.267f);
    glVertex2f(-0.4558f, 0.252f);
    glVertex2f(-0.5655f, 0.3049f);
    glVertex2f(-0.5684f, 0.3083f);
    glVertex2f(-0.5721f, 0.3120f);
    glVertex2f(-0.5760f, 0.3161f);
    glVertex2f(-0.58f,  0.32f);
    glVertex2f(-0.5843f, 0.3240f);
    glVertex2f(-0.5882f, 0.3276f);
    glVertex2f(-0.5918f,  0.3310f);
    glVertex2f(-0.5966f, 0.3349f);
    glVertex2f(-0.6017f, 0.3372f);
    glVertex2f(-0.6082f, 0.3397f);
    glVertex2f(-0.6135f, 0.3391f);
    glVertex2f(-0.6204f, 0.3385f);
    glVertex2f(-0.6263f, 0.3379f);
    glVertex2f(-0.6322f, 0.3361f);
    glVertex2f(-0.6388f, 0.3338f);
    glVertex2f(-0.6445f, 0.3316f);
    glVertex2f(-0.6509f, 0.3283f);
    glVertex2f(-0.6520f, 0.32810f);
    glVertex2f(-0.7329f, 0.3134f);
    glVertex2f(-0.7850f, 0.3281f);
    glVertex2f(-0.846f, 0.315f);
    glVertex2f(-0.8744f, 0.3240f);
    glVertex2f(-0.8849f, 0.3227f);
    glVertex2f(-0.8944f, 0.3171f);
    glVertex2f(-0.8949f, 0.2735f);
    glVertex2f(-0.8852f, 0.2674f);
    glEnd();

    // Tail fin
    glBegin(GL_POLYGON);
    glColor3f(0.6416f, 0.6573f, 0.6573f);
    glVertex2f(-0.785f, 0.3281f);
    glVertex2f(-0.8286f, 0.4231f);
    glVertex2f(-0.8497f, 0.4231f);
    glVertex2f(-0.8750f,  0.3556f);
    glVertex2f(-0.8744f, 0.324f);
    glVertex2f(-0.846f, 0.315f);
    glEnd();

    // Canopy/cockpit area
    glBegin(GL_POLYGON);
    glColor3f(0.7416f, 0.7573f, 0.7573f);
    glVertex2f(-0.5655f, 0.3049f);
    glVertex2f(-0.5684f, 0.3083f);
    glVertex2f(-0.5721f, 0.3120f);
    glVertex2f(-0.5760f, 0.3161f);
    glVertex2f(-0.58f,  0.32f);
    glVertex2f(-0.5843f, 0.3240f);
    glVertex2f(-0.5882f, 0.3276f);
    glVertex2f(-0.5918f,  0.3310f);
    glVertex2f(-0.5966f, 0.3349f);
    glVertex2f(-0.6017f, 0.3372f);
    glVertex2f(-0.6082f, 0.3397f);
    glVertex2f(-0.6135f, 0.3391f);
    glVertex2f(-0.6204f, 0.3385f);
    glVertex2f(-0.6263f, 0.3379f);
    glVertex2f(-0.6322f, 0.3361f);
    glVertex2f(-0.6388f, 0.3338f);
    glVertex2f(-0.6445f, 0.3316f);
    glVertex2f(-0.6509f, 0.3283f);
    glVertex2f(-0.6520f, 0.32810f);
    glVertex2f(-0.7329f, 0.3134f);
    glVertex2f(-0.7850f, 0.3281f);
    glVertex2f(-0.846f, 0.315f);
    glVertex2f(-0.8744f, 0.3240f);
    glVertex2f(-0.8849f, 0.3227f);
    glVertex2f(-0.8944f, 0.3171f);
    glVertex2f(-0.8949f, 0.2735f);
    glVertex2f(-0.8852f, 0.2674f);
    glEnd();
}

void drawHelicarrier() {

    glColor3f(0.18f, 0.18f, 0.18f); // Darker base color
    glBegin(GL_POLYGON);
    glVertex2f(-0.8f, -0.2f);
    glVertex2f(-0.7f, 0.05f);
    glVertex2f(0.7f, 0.05f);
    glVertex2f(0.8f, -0.2f);
    glVertex2f(0.7f, -0.25f);
    glVertex2f(-0.7f, -0.25f);
    glEnd();


    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.7f, -0.15f);
    glVertex2f(-0.6f, 0.02f);
    glVertex2f(0.6f, 0.02f);
    glVertex2f(0.7f, -0.15f);
    glVertex2f(0.6f, -0.2f);
    glVertex2f(-0.6f, -0.2f);
    glEnd();

    // Runway
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-0.4f, 0.0f);
    glVertex2f(0.4f, 0.0f);
    glVertex2f(0.4f, -0.15f);
    glVertex2f(-0.4f, -0.15f);
    glEnd();

    // Runway edge lights
    glColor4f(1.0f, 0.8f, 0.8f, lightGlow);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glVertex2f(-0.4f, 0.0f);
    glVertex2f(0.4f, 0.0f);
    glVertex2f(0.4f, -0.15f);
    glVertex2f(-0.4f, -0.15f);
    glEnd();
    glPointSize(1.0f);

    // Center line
    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x00FF);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex2f(-0.4f, -0.075f);
    glVertex2f(0.4f, -0.075f);
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    // Control Tower
    glColor3f(0.35f, 0.35f, 0.35f);
    glBegin(GL_POLYGON);
    glVertex2f(0.15f, 0.05f);
    glVertex2f(0.35f, 0.05f);
    glVertex2f(0.32f, 0.2f);
    glVertex2f(0.18f, 0.2f);
    glEnd();

    // Tower top
    glColor3f(0.45f, 0.45f, 0.45f);
    glBegin(GL_POLYGON);
    glVertex2f(0.17f, 0.18f);
    glVertex2f(0.33f, 0.18f);
    glVertex2f(0.3f, 0.22f);
    glVertex2f(0.2f, 0.22f);
    glEnd();


    glColor4f(0.9f, 1.0f, 1.0f, 0.3f + lightGlow * 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(0.20f, 0.1f);
    glVertex2f(0.30f, 0.1f);
    glVertex2f(0.28f, 0.16f);
    glVertex2f(0.22f, 0.16f);
    glEnd();

    // Side thrusters
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(-0.85f, -0.1f);
    glVertex2f(-0.65f, -0.1f);
    glVertex2f(-0.65f, -0.22f);
    glVertex2f(-0.85f, -0.22f);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(0.65f, -0.1f);
    glVertex2f(0.85f, -0.1f);
    glVertex2f(0.85f, -0.22f);
    glVertex2f(0.65f, -0.22f);
    glEnd();

    // Searchlights
    drawRotatingSearchlight(-0.5f, -0.2f);
    drawRotatingSearchlight(0.5f, -0.2f);

    glColor4f(0.9f, 0.9f, 0.9f, 0.8f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);

    glVertex2f(-0.6f, 0.0f); glVertex2f(-0.45f, 0.0f);
    glVertex2f(-0.6f, -0.05f); glVertex2f(-0.45f, -0.05f);

    glVertex2f(0.45f, 0.0f); glVertex2f(0.6f, 0.0f);
    glVertex2f(0.45f, -0.05f); glVertex2f(0.6f, -0.05f);
    glEnd();
    glLineWidth(1.0f);


    SdrawCircle(0.0f, -0.1f, 0.03f, 0.9f, 0.9f, 0.9f, 0.8f); // White circle on lower deck
    // Helipad H
    glColor4f(0.1f, 0.1f, 0.1f, 0.9f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.015f, -0.09f);
    glVertex2f(-0.015f, -0.11f);
    glVertex2f(0.015f, -0.09f);
    glVertex2f(0.015f, -0.11f);
    glVertex2f(-0.015f, -0.10f);
    glVertex2f(0.015f, -0.10f);
    glEnd();
    glLineWidth(1.0f);

    // Railings
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-0.8f, -0.15f);
    glVertex2f(-0.7f, -0.15f);
    glVertex2f(-0.7f, -0.1f);
    glVertex2f(-0.8f, -0.1f); // Left lower
    glVertex2f(0.7f, -0.15f);
    glVertex2f(0.8f, -0.15f);
    glVertex2f(0.8f, -0.1f);
    glVertex2f(0.7f, -0.1f); // Right lower
    glEnd();

    // Thruster
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_LINES);
    glVertex2f(-0.75f, -0.15f);
    glVertex2f(-0.75f, -0.05f); // Left thruster line
    glVertex2f(0.75f, -0.15f);
    glVertex2f(0.75f, -0.05f); // Right thruster line
    glEnd();
}

void drawHelicopter() {

    glColor3f(0.3f, 0.3f, 0.4f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.07f, 0.02f);
    glVertex2f(0.03f, 0.02f);
    glVertex2f(0.05f, 0.00f);
    glVertex2f(0.03f, -0.02f);
    glVertex2f(-0.07f, -0.02f);
    glEnd();

    // Tail boom
    glColor3f(0.25f, 0.25f, 0.35f);
    glBegin(GL_QUADS);
    glVertex2f(-0.07f, 0.01f);
    glVertex2f(-0.12f, 0.005f);
    glVertex2f(-0.12f, -0.005f);
    glVertex2f(-0.07f, -0.01f);
    glEnd();

    glColor3f(0.35f, 0.35f, 0.45f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.12f, 0.005f);
    glVertex2f(-0.13f, 0.025f);
    glVertex2f(-0.12f, -0.005f);
    glEnd();

    // Tail rotor
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_LINES);
    glVertex2f(-0.13f, 0.015f);
    glVertex2f(-0.13f, 0.035f);
    glEnd();

    // Main rotor base
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(-0.02f, 0.02f);
    glVertex2f(0.0f, 0.02f);
    glVertex2f(0.0f, 0.03f);
    glVertex2f(-0.02f, 0.03f);
    glEnd();

    // Main rotor blade
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.01f, 0.03f);
    glVertex2f(0.1f, 0.03f);
    glEnd();

    // Main rotor blade 2
    glBegin(GL_LINES);
    glVertex2f(-0.01f, 0.03f);
    glVertex2f(-0.01f, 0.13f);
    glEnd();
    glLineWidth(1.0f);

    // Skids
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.05f, -0.02f);
    glVertex2f(-0.05f, -0.04f);
    glVertex2f(0.0f, -0.04f);
    glVertex2f(0.0f, -0.02f);
    glEnd();

    //  Window
    glColor3f(0.6f, 0.7f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(0.03f, 0.015f);
    glVertex2f(0.045f, 0.015f);
    glVertex2f(0.04f, -0.005f);
    glVertex2f(0.025f, -0.005f);
    glEnd();
}

void sakibDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawGradientSky();
    drawSunAndMoon();

    glPushMatrix();
    glTranslatef(cloudOffset1, 0, 0);
    SdrawCloud(-0.8f, 0.8f);
    SdrawCloud(0.0f, 0.9f);
    if(cloudOffset1 > 2.2f) cloudOffset1 = -1.2f;
    glPopMatrix();

    glPushMatrix();
    glTranslatef(cloudOffset2, 0, 0);
    SdrawCloud(0.6f, 0.75f);
    SdrawCloud(-0.2f, 0.65f);
    if(cloudOffset2 > 2.2f) cloudOffset2 = -1.2f;
    glPopMatrix();

    drawWaves();
    drawHelicarrier();

    glPushMatrix();
    glTranslatef(-0.65f, -0.05f, 0.0f);
    glScalef(0.2592f, 0.2592f, 1.0f);
    drawHelicopter();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.65f, -0.05f, 0.0f);
    glScalef(0.2592f, 0.2592f, 1.0f);
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    drawHelicopter();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.45f, -0.2f, 0.0f);
    glScalef(0.18375f, 0.18375f, 1.0f);
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    drawFighterJet();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.3f, 0.0f, 0.0f);
    glScalef(0.18375f, 0.18375f, 1.0f);
    drawFighterJet();
    glPopMatrix();

    if (jetLandingState > 0 || jetPosX < 2.0f) {
        glPushMatrix();
        glTranslatef(jetPosX, jetPosY, 0.0f);
        glRotatef(jetAngle, 0.0f, 0.0f, 1.0f);
        glScalef(0.153125f, 0.153125f, 1.0f);
        drawFighterJet();
        glPopMatrix();
    }

    glFlush();
}

///////////////////////////SAKIB CODE END //////////////////////

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // ESC key
            exit(0);
            break;

        case 't':
        case 'T':
            takeoffStarted = true;
            break;

        case 'm':
        case 'M':
            showInstruction = true;
            showInformation = false;
            takeoffStarted = false;
            glutPostRedisplay();
            break;

        case 'i':
        case 'I':
            showInformation = true;
            showInstruction = false;
            takeoffStarted = false;
            glutPostRedisplay();
            break;

        case 'r':
        case 'R':
            takeoffStarted = false;
            planeX = 0.0f;
            planeY = -0.4f;
            glutPostRedisplay();
            break;

        case 'u':
        case 'U':
            mtakeoffStarted = !mtakeoffStarted;
            break;

        case 'b':
        case 'B':
            mtakeoffStarted = false;
            planePosX = 0.0f;
            mplaneY = -0.4f;
            glutPostRedisplay();
            break;

        case 'n':
        case 'N':
            isNightTransition = true;
            glutPostRedisplay();
            break;

        case 'd':
        case 'D':
            isNightTransition = false;
            glutPostRedisplay();
            break;

        case 'z':
        case 'Z':
            ZtakeoffStarted = true;
            bombStarted = true;
            explosionTime = 0;
            showExplosion = false;
            break;

        case 'x':
        case 'X':
            if (currentScene == 3) {
                aplaneX = -1.2f;
                animate = true;
                if (!timerRunning) {
                    glutTimerFunc(0, anikupdate, 0);
                    }
                } else {
                    ZtakeoffStarted = false;
                    bombStarted = false;
                    ZplaneX = -0.89f;
                    ZplaneY = 0.04f;
                    bombX = -0.89f;
                    bombY = 0.04f;
                }
                break;

        case 'y':
        case 'Y':
            if (currentScene == 3 && !missileActive) {
                missileActive = true;
                missileYOffset = 0.0f;
                    if (!timerRunning) {
                        glutTimerFunc(0, update, 0);
                    }
                }
                break;

        case 'l':
        case 'L':
            if (jetLandingState == 0) {
                jetLandingState = 1;
                landingApproachStartPosX = -1.2f;
                landingApproachStartPosY = 0.6f;
                jetPosX = landingApproachStartPosX;
                jetPosY = landingApproachStartPosY;
                jetAngle = 0.0f;
                landingCurrentTime = 0.0f;
                glutPostRedisplay();
            }
            break;

        default:
            // Handle digit keys '1' to '5' for changing scenes
            if (key >= '1' && key <= '5') {
                currentScene = key - '0';
                showInformation = false;
                showInstruction = false;
                glutPostRedisplay();
            }
            break;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (showInformation) {
        information();
    } else if (showInstruction) {
        instructionPage();
    } else {
        switch (currentScene) {
            case 1:
                marufDisplay();
                break;
            case 2:
                marufCWDisplay();
                break;
            case 3:
                asifDisplay();
                break;
            case 4:
                anikDisplay();
                break;
            case 5:
                sakibDisplay();
                break;
        }
    }
    glFlush();
}

void init() {
    glClearColor(1,1,1,1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1,1,-1,1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(1030, 680);
    glutCreateWindow("Scenic View with Plane and Hills");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    //glutMouseFunc(mouseClick);
    init();
    initGL();
    update(0);
    anikupdate(0);
    glutMainLoop();
    glEnable(GL_POINT_SMOOTH); // Enable anti-aliasing for points
    glEnable(GL_BLEND); // Enable blending for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set blending function
    glutTimerFunc(16, update, 0); // Start the animation timer
    sndPlaySound("ocean", SND_ASYNC);
    return 0;
}
