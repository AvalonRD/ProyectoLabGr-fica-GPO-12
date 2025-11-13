#include <iostream>
#include <cmath>
#include <vector>

// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

// stb_image
#include "stb_image.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SOIL2
#include "SOIL2/SOIL2.h"

// Proyecto
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Prototipos
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();
GLuint loadCubemap(const std::vector<std::string>& faces);

// Dimensiones
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Cámara
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
bool keys[1024] = { false };
bool firstMouse = true;

// Luz puntual fija
glm::vec3 pointLightPositions[] = {
    glm::vec3(0.0f, 2.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f)
};

// Cubo para lámpara visual (pos + normal)
float vertices[] = {
    -0.5f,-0.5f,-0.5f,  0.0f,  0.0f,-1.0f,   0.5f,-0.5f,-0.5f,  0.0f,  0.0f,-1.0f,   0.5f, 0.5f,-0.5f,  0.0f,  0.0f,-1.0f,
     0.5f, 0.5f,-0.5f,  0.0f,  0.0f,-1.0f,  -0.5f, 0.5f,-0.5f,  0.0f,  0.0f,-1.0f,  -0.5f,-0.5f,-0.5f,  0.0f,  0.0f,-1.0f,

    -0.5f,-0.5f, 0.5f,  0.0f,  0.0f, 1.0f,   0.5f,-0.5f, 0.5f,  0.0f,  0.0f, 1.0f,   0.5f, 0.5f, 0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, 0.5f, 0.5f,  0.0f,  0.0f, 1.0f,  -0.5f, 0.5f, 0.5f,  0.0f,  0.0f, 1.0f,  -0.5f,-0.5f, 0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f, 0.5f, 0.5f, -1.0f,  0.0f, 0.0f,  -0.5f, 0.5f,-0.5f, -1.0f,  0.0f, 0.0f,  -0.5f,-0.5f,-0.5f, -1.0f,  0.0f, 0.0f,
    -0.5f,-0.5f,-0.5f, -1.0f,  0.0f, 0.0f,  -0.5f,-0.5f, 0.5f, -1.0f,  0.0f, 0.0f,  -0.5f, 0.5f, 0.5f, -1.0f,  0.0f, 0.0f,

     0.5f, 0.5f, 0.5f,  1.0f,  0.0f, 0.0f,   0.5f, 0.5f,-0.5f,  1.0f,  0.0f, 0.0f,   0.5f,-0.5f,-0.5f,  1.0f,  0.0f, 0.0f,
     0.5f,-0.5f,-0.5f,  1.0f,  0.0f, 0.0f,   0.5f,-0.5f, 0.5f,  1.0f,  0.0f, 0.0f,   0.5f, 0.5f, 0.5f,  1.0f,  0.0f, 0.0f,

    -0.5f,-0.5f,-0.5f,  0.0f, -1.0f, 0.0f,   0.5f,-0.5f,-0.5f,  0.0f, -1.0f, 0.0f,   0.5f,-0.5f, 0.5f,  0.0f, -1.0f, 0.0f,
     0.5f,-0.5f, 0.5f,  0.0f, -1.0f, 0.0f,  -0.5f,-0.5f, 0.5f,  0.0f, -1.0f, 0.0f,  -0.5f,-0.5f,-0.5f,  0.0f, -1.0f, 0.0f,

    -0.5f, 0.5f,-0.5f,  0.0f,  1.0f, 0.0f,   0.5f, 0.5f,-0.5f,  0.0f,  1.0f, 0.0f,   0.5f, 0.5f, 0.5f,  0.0f,  1.0f, 0.0f,
     0.5f, 0.5f, 0.5f,  0.0f,  1.0f, 0.0f,  -0.5f, 0.5f, 0.5f,  0.0f,  1.0f, 0.0f,  -0.5f, 0.5f,-0.5f,  0.0f,  1.0f, 0.0f
};

// Skybox (solo posiciones)
float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

// Helper rotación local
inline void RotateAround(glm::mat4& M, const glm::vec3& p, float angDeg, const glm::vec3& axis) {
    M = glm::translate(M, p);
    M = glm::rotate(M, glm::radians(angDeg), axis);
    M = glm::translate(M, -p);
}

// Tiempos
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// ---------- Rigs y variables ----------

// Rig activo
enum Rig { RIG_JIR = 0, RIG_LEO = 1, RIG_ZEB = 2, RIG_PB = 3, RIG_FOX = 4, RIG_RHINO = 5 };
Rig activeRig = RIG_JIR;

// Jirafa
float jirFLegs = 0.0f, jirRLegs = 0.0f, jirHead = 0.0f, jirTail = 0.0f, jirRot = 0.0f;
glm::vec3 girPos(0.0f, 0.0f, 0.0f);

// Leona
float leoFLegs = 0.0f, leoRLegs = 0.0f, leoHead = 0.0f, leoTail = 0.0f, leoRot = 220.0f;
glm::vec3 leoPos(0.0f, -7.0f, 0.0f);

// Cebra
float zebFLegs = 0.0f, zebRLegs = 0.0f, zebHead = 0.0f, zebTail = 0.0f, zebRot = 160.0f;
glm::vec3 zebPos(0.0f, 0.0f, 0.0f);

// Oso polar
float pbFLegs = 0.0f, pbRLegs = 0.0f, pbHead = 0.0f, pbTail = 0.0f, pbRot = 70.0f;
glm::vec3 pbPos(0.0f, 0.0f, 0.0f);

// Zorro
float foxFLegs = 0.0f, foxRLegs = 0.0f, foxHead = 0.0f, foxTail = 0.0f, foxRot = -90.0f;
glm::vec3 foxPos(0.0f, 0.0f, 0.0f);

// Rinoceronte
float rhFLegs = 0.0f, rhRLegs = 0.0f, rhHead = 0.0f, rhTail = 0.0f, rhRot = 0.0f;
glm::vec3 rhPos(0.0f, 0.0f, 0.0f);

// ---------- Keyframes por rig ----------

#define MAX_FRAMES 12
const int MAX_STEPS = 190;

typedef struct _frame {
    float posX, posY, posZ, incX, incY, incZ;
    float rotY, rotYInc;
    float head, headInc;
    float tail, tailInc;
    float flegs, flegsInc;
    float rlegs, rlegsInc;
} FRAME;

// Timelines
FRAME KFJir[MAX_FRAMES], KFLeo[MAX_FRAMES], KFZeb[MAX_FRAMES],
KFPb[MAX_FRAMES], KFFox[MAX_FRAMES], KFRhino[MAX_FRAMES];

// Índices y estado
int idxJir = 0, idxLeo = 0, idxZeb = 0, idxPb = 0, idxFox = 0, idxRhino = 0;
bool playJir = false, playLeo = false, playZeb = false, playPb = false, playFox = false, playRhino = false;
int  segJir = 0, segLeo = 0, segZeb = 0, segPb = 0, segFox = 0, segRhino = 0;
int  curJir = 0, curLeo = 0, curZeb = 0, curPb = 0, curFox = 0, curRhino = 0;

// Utilidades
inline void zeroFrames(FRAME* KF) { for (int i = 0; i < MAX_FRAMES; ++i) KF[i] = { 0 }; }
struct KFInit {
    KFInit() {
        zeroFrames(KFJir);
        zeroFrames(KFLeo);
        zeroFrames(KFZeb);
        zeroFrames(KFPb);
        zeroFrames(KFFox);
        zeroFrames(KFRhino);
    }
} _kfInit;

// ---- Bindings Jirafa ----
inline void saveJir() {
    if (idxJir >= MAX_FRAMES) return;
    KFJir[idxJir].posX = girPos.x; KFJir[idxJir].posY = girPos.y; KFJir[idxJir].posZ = girPos.z;
    KFJir[idxJir].rotY = jirRot;   KFJir[idxJir].head = jirHead;  KFJir[idxJir].tail = jirTail;
    KFJir[idxJir].flegs = jirFLegs; KFJir[idxJir].rlegs = jirRLegs; idxJir++;
}
inline void resetJir() {
    if (idxJir == 0) return;
    girPos.x = KFJir[0].posX; girPos.y = KFJir[0].posY; girPos.z = KFJir[0].posZ;
    jirRot = KFJir[0].rotY;   jirHead = KFJir[0].head;  jirTail = KFJir[0].tail;
    jirFLegs = KFJir[0].flegs; jirRLegs = KFJir[0].rlegs;
}
inline void interpJir() {
    int i = curJir;
    KFJir[i].incX = (KFJir[i + 1].posX - KFJir[i].posX) / MAX_STEPS;
    KFJir[i].incY = (KFJir[i + 1].posY - KFJir[i].posY) / MAX_STEPS;
    KFJir[i].incZ = (KFJir[i + 1].posZ - KFJir[i].posZ) / MAX_STEPS;
    KFJir[i].rotYInc = (KFJir[i + 1].rotY - KFJir[i].rotY) / MAX_STEPS;
    KFJir[i].headInc = (KFJir[i + 1].head - KFJir[i].head) / MAX_STEPS;
    KFJir[i].tailInc = (KFJir[i + 1].tail - KFJir[i].tail) / MAX_STEPS;
    KFJir[i].flegsInc = (KFJir[i + 1].flegs - KFJir[i].flegs) / MAX_STEPS;
    KFJir[i].rlegsInc = (KFJir[i + 1].rlegs - KFJir[i].rlegs) / MAX_STEPS;
}
inline void tickJir() {
    girPos.x += KFJir[curJir].incX; girPos.y += KFJir[curJir].incY; girPos.z += KFJir[curJir].incZ;
    jirRot += KFJir[curJir].rotYInc; jirHead += KFJir[curJir].headInc; jirTail += KFJir[curJir].tailInc;
    jirFLegs += KFJir[curJir].flegsInc; jirRLegs += KFJir[curJir].rlegsInc;
}

// ---- Bindings Leona ----
inline void saveLeo() {
    if (idxLeo >= MAX_FRAMES) return;
    KFLeo[idxLeo].posX = leoPos.x; KFLeo[idxLeo].posY = leoPos.y; KFLeo[idxLeo].posZ = leoPos.z;
    KFLeo[idxLeo].rotY = leoRot;   KFLeo[idxLeo].head = leoHead;  KFLeo[idxLeo].tail = leoTail;
    KFLeo[idxLeo].flegs = leoFLegs; KFLeo[idxLeo].rlegs = leoRLegs; idxLeo++;
}
inline void resetLeo() {
    if (idxLeo == 0) return;
    leoPos.x = KFLeo[0].posX; leoPos.y = KFLeo[0].posY; leoPos.z = KFLeo[0].posZ;
    leoRot = KFLeo[0].rotY;   leoHead = KFLeo[0].head;  leoTail = KFLeo[0].tail;
    leoFLegs = KFLeo[0].flegs; leoRLegs = KFLeo[0].rlegs;
}
inline void interpLeo() {
    int i = curLeo;
    KFLeo[i].incX = (KFLeo[i + 1].posX - KFLeo[i].posX) / MAX_STEPS;
    KFLeo[i].incY = (KFLeo[i + 1].posY - KFLeo[i].posY) / MAX_STEPS;
    KFLeo[i].incZ = (KFLeo[i + 1].posZ - KFLeo[i].posZ) / MAX_STEPS;
    KFLeo[i].rotYInc = (KFLeo[i + 1].rotY - KFLeo[i].rotY) / MAX_STEPS;
    KFLeo[i].headInc = (KFLeo[i + 1].head - KFLeo[i].head) / MAX_STEPS;
    KFLeo[i].tailInc = (KFLeo[i + 1].tail - KFLeo[i].tail) / MAX_STEPS;
    KFLeo[i].flegsInc = (KFLeo[i + 1].flegs - KFLeo[i].flegs) / MAX_STEPS;
    KFLeo[i].rlegsInc = (KFLeo[i + 1].rlegs - KFLeo[i].rlegs) / MAX_STEPS;
}
inline void tickLeo() {
    leoPos.x += KFLeo[curLeo].incX; leoPos.y += KFLeo[curLeo].incY; leoPos.z += KFLeo[curLeo].incZ;
    leoRot += KFLeo[curLeo].rotYInc; leoHead += KFLeo[curLeo].headInc; leoTail += KFLeo[curLeo].tailInc;
    leoFLegs += KFLeo[curLeo].flegsInc; leoRLegs += KFLeo[curLeo].rlegsInc;
}

// ---- Bindings Cebra ----
inline void saveZeb() {
    if (idxZeb >= MAX_FRAMES) return;
    KFZeb[idxZeb].posX = zebPos.x; KFZeb[idxZeb].posY = zebPos.y; KFZeb[idxZeb].posZ = zebPos.z;
    KFZeb[idxZeb].rotY = zebRot;   KFZeb[idxZeb].head = zebHead;  KFZeb[idxZeb].tail = zebTail;
    KFZeb[idxZeb].flegs = zebFLegs; KFZeb[idxZeb].rlegs = zebRLegs; idxZeb++;
}
inline void resetZeb() {
    if (idxZeb == 0) return;
    zebPos.x = KFZeb[0].posX; zebPos.y = KFZeb[0].posY; zebPos.z = KFZeb[0].posZ;
    zebRot = KFZeb[0].rotY;   zebHead = KFZeb[0].head;  zebTail = KFZeb[0].tail;
    zebFLegs = KFZeb[0].flegs; zebRLegs = KFZeb[0].rlegs;
}
inline void interpZeb() {
    int i = curZeb;
    KFZeb[i].incX = (KFZeb[i + 1].posX - KFZeb[i].posX) / MAX_STEPS;
    KFZeb[i].incY = (KFZeb[i + 1].posY - KFZeb[i].posY) / MAX_STEPS;
    KFZeb[i].incZ = (KFZeb[i + 1].posZ - KFZeb[i].posZ) / MAX_STEPS;
    KFZeb[i].rotYInc = (KFZeb[i + 1].rotY - KFZeb[i].rotY) / MAX_STEPS;
    KFZeb[i].headInc = (KFZeb[i + 1].head - KFZeb[i].head) / MAX_STEPS;
    KFZeb[i].tailInc = (KFZeb[i + 1].tail - KFZeb[i].tail) / MAX_STEPS;
    KFZeb[i].flegsInc = (KFZeb[i + 1].flegs - KFZeb[i].flegs) / MAX_STEPS;
    KFZeb[i].rlegsInc = (KFZeb[i + 1].rlegs - KFZeb[i].rlegs) / MAX_STEPS;
}
inline void tickZeb() {
    zebPos.x += KFZeb[curZeb].incX; zebPos.y += KFZeb[curZeb].incY; zebPos.z += KFZeb[curZeb].incZ;
    zebRot += KFZeb[curZeb].rotYInc; zebHead += KFZeb[curZeb].headInc; zebTail += KFZeb[curZeb].tailInc;
    zebFLegs += KFZeb[curZeb].flegsInc; zebRLegs += KFZeb[curZeb].rlegsInc;
}

// ---- Bindings Oso polar ----
inline void savePb() {
    if (idxPb >= MAX_FRAMES) return;
    KFPb[idxPb].posX = pbPos.x; KFPb[idxPb].posY = pbPos.y; KFPb[idxPb].posZ = pbPos.z;
    KFPb[idxPb].rotY = pbRot;   KFPb[idxPb].head = pbHead;  KFPb[idxPb].tail = pbTail;
    KFPb[idxPb].flegs = pbFLegs; KFPb[idxPb].rlegs = pbRLegs; idxPb++;
}
inline void resetPb() {
    if (idxPb == 0) return;
    pbPos.x = KFPb[0].posX; pbPos.y = KFPb[0].posY; pbPos.z = KFPb[0].posZ;
    pbRot = KFPb[0].rotY; pbHead = KFPb[0].head; pbTail = KFPb[0].tail;
    pbFLegs = KFPb[0].flegs; pbRLegs = KFPb[0].rlegs;
}
inline void interpPb() {
    int i = curPb;
    KFPb[i].incX = (KFPb[i + 1].posX - KFPb[i].posX) / MAX_STEPS;
    KFPb[i].incY = (KFPb[i + 1].posY - KFPb[i].posY) / MAX_STEPS;
    KFPb[i].incZ = (KFPb[i + 1].posZ - KFPb[i].posZ) / MAX_STEPS;
    KFPb[i].rotYInc = (KFPb[i + 1].rotY - KFPb[i].rotY) / MAX_STEPS;
    KFPb[i].headInc = (KFPb[i + 1].head - KFPb[i].head) / MAX_STEPS;
    KFPb[i].tailInc = (KFPb[i + 1].tail - KFPb[i].tail) / MAX_STEPS;
    KFPb[i].flegsInc = (KFPb[i + 1].flegs - KFPb[i].flegs) / MAX_STEPS;
    KFPb[i].rlegsInc = (KFPb[i + 1].rlegs - KFPb[i].rlegs) / MAX_STEPS;
}
inline void tickPb() {
    pbPos.x += KFPb[curPb].incX; pbPos.y += KFPb[curPb].incY; pbPos.z += KFPb[curPb].incZ;
    pbRot += KFPb[curPb].rotYInc; pbHead += KFPb[curPb].headInc; pbTail += KFPb[curPb].tailInc;
    pbFLegs += KFPb[curPb].flegsInc; pbRLegs += KFPb[curPb].rlegsInc;
}

// ---- Bindings Zorro ----
inline void saveFox() {
    if (idxFox >= MAX_FRAMES) return;
    KFFox[idxFox].posX = foxPos.x; KFFox[idxFox].posY = foxPos.y; KFFox[idxFox].posZ = foxPos.z;
    KFFox[idxFox].rotY = foxRot;   KFFox[idxFox].head = foxHead;  KFFox[idxFox].tail = foxTail;
    KFFox[idxFox].flegs = foxFLegs; KFFox[idxFox].rlegs = foxRLegs; idxFox++;
}
inline void resetFox() {
    if (idxFox == 0) return;
    foxPos.x = KFFox[0].posX; foxPos.y = KFFox[0].posY; foxPos.z = KFFox[0].posZ;
    foxRot = KFFox[0].rotY; foxHead = KFFox[0].head; foxTail = KFFox[0].tail;
    foxFLegs = KFFox[0].flegs; foxRLegs = KFFox[0].rlegs;
}
inline void interpFox() {
    int i = curFox;
    KFFox[i].incX = (KFFox[i + 1].posX - KFFox[i].posX) / MAX_STEPS;
    KFFox[i].incY = (KFFox[i + 1].posY - KFFox[i].posY) / MAX_STEPS;
    KFFox[i].incZ = (KFFox[i + 1].posZ - KFFox[i].posZ) / MAX_STEPS;
    KFFox[i].rotYInc = (KFFox[i + 1].rotY - KFFox[i].rotY) / MAX_STEPS;
    KFFox[i].headInc = (KFFox[i + 1].head - KFFox[i].head) / MAX_STEPS;
    KFFox[i].tailInc = (KFFox[i + 1].tail - KFFox[i].tail) / MAX_STEPS;
    KFFox[i].flegsInc = (KFFox[i + 1].flegs - KFFox[i].flegs) / MAX_STEPS;
    KFFox[i].rlegsInc = (KFFox[i + 1].rlegs - KFFox[i].rlegs) / MAX_STEPS;
}
inline void tickFox() {
    foxPos.x += KFFox[curFox].incX; foxPos.y += KFFox[curFox].incY; foxPos.z += KFFox[curFox].incZ;
    foxRot += KFFox[curFox].rotYInc; foxHead += KFFox[curFox].headInc; foxTail += KFFox[curFox].tailInc;
    foxFLegs += KFFox[curFox].flegsInc; foxRLegs += KFFox[curFox].rlegsInc;
}

// ---- Bindings Rinoceronte ----
inline void saveRhino() {
    if (idxRhino >= MAX_FRAMES) return;
    KFRhino[idxRhino].posX = rhPos.x; KFRhino[idxRhino].posY = rhPos.y; KFRhino[idxRhino].posZ = rhPos.z;
    KFRhino[idxRhino].rotY = rhRot;   KFRhino[idxRhino].head = rhHead;  KFRhino[idxRhino].tail = rhTail;
    KFRhino[idxRhino].flegs = rhFLegs; KFRhino[idxRhino].rlegs = rhRLegs; idxRhino++;
}
inline void resetRhino() {
    if (idxRhino == 0) return;
    rhPos.x = KFRhino[0].posX; rhPos.y = KFRhino[0].posY; rhPos.z = KFRhino[0].posZ;
    rhRot = KFRhino[0].rotY; rhHead = KFRhino[0].head; rhTail = KFRhino[0].tail;
    rhFLegs = KFRhino[0].flegs; rhRLegs = KFRhino[0].rlegs;
}
inline void interpRhino() {
    int i = curRhino;
    KFRhino[i].incX = (KFRhino[i + 1].posX - KFRhino[i].posX) / MAX_STEPS;
    KFRhino[i].incY = (KFRhino[i + 1].posY - KFRhino[i].posY) / MAX_STEPS;
    KFRhino[i].incZ = (KFRhino[i + 1].posZ - KFRhino[i].posZ) / MAX_STEPS;
    KFRhino[i].rotYInc = (KFRhino[i + 1].rotY - KFRhino[i].rotY) / MAX_STEPS;
    KFRhino[i].headInc = (KFRhino[i + 1].head - KFRhino[i].head) / MAX_STEPS;
    KFRhino[i].tailInc = (KFRhino[i + 1].tail - KFRhino[i].tail) / MAX_STEPS;
    KFRhino[i].flegsInc = (KFRhino[i + 1].flegs - KFRhino[i].flegs) / MAX_STEPS;
    KFRhino[i].rlegsInc = (KFRhino[i + 1].rlegs - KFRhino[i].rlegs) / MAX_STEPS;
}
inline void tickRhino() {
    rhPos.x += KFRhino[curRhino].incX; rhPos.y += KFRhino[curRhino].incY; rhPos.z += KFRhino[curRhino].incZ;
    rhRot += KFRhino[curRhino].rotYInc; rhHead += KFRhino[curRhino].headInc; rhTail += KFRhino[curRhino].tailInc;
    rhFLegs += KFRhino[curRhino].flegsInc; rhRLegs += KFRhino[curRhino].rlegsInc;
}

// Shaders globales
Shader* pLighting = nullptr, * pLamp = nullptr;

// Skybox globals
GLuint skyboxVAO = 0, skyboxVBO = 0;
GLuint cubemapTexture = 0;
Shader* pSkybox = nullptr;

// Main
int main()
{
    // Init GLFW
    glfwInit();

    // Window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto final - Keyframes", nullptr, nullptr);
    if (!window) { std::cout << "Failed to create GLFW window\n"; glfwTerminate(); return EXIT_FAILURE; }
    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    // Callbacks
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    // GLEW
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) { std::cout << "Failed to initialize GLEW\n"; return EXIT_FAILURE; }

    // Viewport
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    // Shaders
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting_ibl.frag");
    Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");
    Shader skyboxShader("Shader/skybox.vs", "Shader/skybox.frag");

    pLighting = &lightingShader;
    pLamp = &lampShader;
    pSkybox = &skyboxShader;

    // Modelos
    Model Piso((char*)"Models/piso.obj");
	Model Acacia1((char*)"Models/acacia1.obj");
	Model Hojas1((char*)"Models/hojas1.obj");

    // Jirafa
    Model j_Head((char*)"Models/j_Head.obj");
    Model j_tail((char*)"Models/j_tail.obj");
    Model j_body((char*)"Models/j_body.obj");
    Model jF_rightleg((char*)"Models/jF_rightleg.obj");
    Model jF_leftleg((char*)"Models/jF_leftleg.obj");
    Model jB_rightleg((char*)"Models/jB_rightleg.obj");
    Model jB_leftleg((char*)"Models/jB_leftleg.obj");

    // Leona
    Model l_Head((char*)"Models/l_head.obj");
    Model l_tail((char*)"Models/l_tail.obj");
    Model l_body((char*)"Models/l_body.obj");
    Model lF_rightleg((char*)"Models/lF_rightleg.obj");
    Model lF_leftleg((char*)"Models/lF_leftleg.obj");
    Model lB_rightleg((char*)"Models/lB_rightleg.obj");
    Model lB_leftleg((char*)"Models/lB_leftleg.obj");

    // Cebra
    Model z_Head((char*)"Models/z_head.obj");
    Model z_tail((char*)"Models/z_tail.obj");
    Model z_body((char*)"Models/z_body.obj");
    Model zF_rightleg((char*)"Models/zF_rightleg.obj");
    Model zF_leftleg((char*)"Models/zF_leftleg.obj");
    Model zB_leftleg((char*)"Models/zB_leftleg.obj");
    Model zB_rightleg((char*)"Models/zB_rightleg.obj");

    // Lobo Minecraft
    Model pb_head((char*)"Models/pb_head.obj");
    Model pb_tail((char*)"Models/pb_tail.obj");
    Model pb_body((char*)"Models/pb_body.obj");
    Model pbF_rightleg((char*)"Models/pbF_rightleg.obj");
    Model pbF_leftleg((char*)"Models/pbF_leftleg.obj");
    Model pbB_rightleg((char*)"Models/pbB_rightleg.obj");
    Model pbB_leftleg((char*)"Models/pbB_leftleg.obj");

    // PINGUINO
    Model f_head((char*)"Models/f_head.obj");
    Model f_tail((char*)"Models/f_tail.obj");
    Model f_body((char*)"Models/f_body.obj");
    Model fF_rightleg((char*)"Models/fF_rightleg.obj");
    Model fF_leftleg((char*)"Models/fF_leftleg.obj");
    Model fB_rightleg((char*)"Models/fB_rightleg.obj");
    Model fB_leftleg((char*)"Models/fB_leftleg.obj");

    // VAO/VBO lámpara
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Skybox VAO/VBO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // Carga cubemap
    std::vector<std::string> faces{
        "cubemap/px.png",
        "cubemap/nx.png",
        "cubemap/py.png",
        "cubemap/ny.png",
        "cubemap/pz.png",
        "cubemap/nz.png"
    };
    cubemapTexture = loadCubemap(faces);

    // Materiales + samplers
    lightingShader.Use();
    glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
    glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);
    glUniform1i(glGetUniformLocation(lightingShader.Program, "environmentMap"), 2); // IBL

    skyboxShader.Use();
    glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);

    glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

    // Loop
    while (!glfwWindowShouldClose(window))
    {
        // Delta
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input/Anim
        glfwPollEvents();
        DoMovement();
        Animation();

        // Clear
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model(1.0f);

        // ---------- Escena con IBL ----------
        lightingShader.Use();

        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        // ===== LUZ DIRECCIONAL (SOL CÁLIDO SUAVE) =====
        glUniform3f(
            glGetUniformLocation(lightingShader.Program, "dirLight.direction"),
            -0.3f, -1.0f, -0.2f   // desde arriba, ligeramente inclinado
        );

        // Ambiente bajo para no lavar todo
        glUniform3f(
            glGetUniformLocation(lightingShader.Program, "dirLight.ambient"),
            0.38f, 0.36f, 0.50f   // tono 
        );

        // Luz directa del sol
        glUniform3f(
            glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"),
            1.00f, 0.96f, 0.88f
        );

        // Brillos del sol
        glUniform3f(
            glGetUniformLocation(lightingShader.Program, "dirLight.specular"),
            0.50f, 0.75f, 0.80f
        );

        // ===== LUZ PUNTUAL (RELLENO BLANCO NEUTRO) =====
        const glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // blanca

        glUniform3f(
            glGetUniformLocation(lightingShader.Program, "pointLights[0].position"),
            pointLightPositions[0].x,
            pointLightPositions[0].y,
            pointLightPositions[0].z
        );

        // Ambiente muy bajo para que solo rellene sombras
        glUniform3f(
            glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"),
            lightColor.x * 0.15f,
            lightColor.y * 0.15f,
            lightColor.z * 0.15f
        );

        // Difusa moderada
        glUniform3f(
            glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"),
            lightColor.x * 0.8f,
            lightColor.y * 0.8f,
            lightColor.z * 0.8f
        );

        // Especular fuerte (para brillos)
        glUniform3f(
            glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"),
            lightColor.x,
            lightColor.y,
            lightColor.z
        );

        // Caída un poco más fuerte (menos quemado lejos)
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);

        // ===== SPOTLIGHT EN LA CÁMARA (LINTERNA SUAVE) =====
        glUniform3f(
            glGetUniformLocation(lightingShader.Program, "spotLight.position"),
            camera.GetPosition().x,
            camera.GetPosition().y,
            camera.GetPosition().z
        );
        glUniform3f(
            glGetUniformLocation(lightingShader.Program, "spotLight.direction"),
            camera.GetFront().x,
            camera.GetFront().y,
            camera.GetFront().z
        );

        // Sin ambiente, solo se nota cuando apuntas
        glUniform3f(
            glGetUniformLocation(lightingShader.Program, "spotLight.ambient"),
            0.0f, 0.0f, 0.0f
        );

        // Luz blanca ligeramente fría
        glUniform3f(
            glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"),
            0.8f, 0.8f, 1.0f
        );

        // Especular moderado
        glUniform3f(
            glGetUniformLocation(lightingShader.Program, "spotLight.specular"),
            0.6f, 0.6f, 1.0f
        );

        // Menos alcance 
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.14f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.07f);

        // Spot más cerrado
        glUniform1f(
            glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"),
            glm::cos(glm::radians(10.0f))
        );
        glUniform1f(
            glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"),
            glm::cos(glm::radians(15.0f))
        );

        // ===== MATERIAL =====
        // Más brillante 
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);

        // Matrices
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
        GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Bind cubemap para IBL (unit 2)
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

        // ================== JIRAFA ==================
        glm::mat4 jirafaRoot = glm::mat4(1.0f);
        jirafaRoot = glm::translate(jirafaRoot, girPos);
        jirafaRoot = glm::translate(jirafaRoot, glm::vec3(-8.0f, -6.5f, 32.0f));
        jirafaRoot = glm::scale(jirafaRoot, glm::vec3(1.1f));
        jirafaRoot = glm::rotate(jirafaRoot, glm::radians(jirRot), glm::vec3(0.0f, 1.0f, 0.0f));

        model = jirafaRoot;
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        j_body.Draw(lightingShader);

        // Cabeza
        model = jirafaRoot;
        model = glm::translate(model, glm::vec3(0.0f, 1.2314f, -2.17594f));
        RotateAround(model, glm::vec3(0.031548f, -2.13134f, 2.57094f), jirHead, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        j_Head.Draw(lightingShader);

        // Cola
        model = jirafaRoot;
        model = glm::translate(model, glm::vec3(0.004535f, -0.979609f, 2.08606f));
        RotateAround(model, glm::vec3(0.0f, 1.12552f, -0.786921f), jirTail, glm::vec3(0.0f, 0.0f, -1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        j_tail.Draw(lightingShader);

        // Pata delantera izq
        model = jirafaRoot;
        model = glm::translate(model, glm::vec3(-0.535868f, -2.75433f, -2.0333f));
        RotateAround(model, glm::vec3(0.241107f, 2.60274f, 0.291154f), jirFLegs, glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        jF_leftleg.Draw(lightingShader);

        // Pata delantera der
        model = jirafaRoot;
        model = glm::translate(model, glm::vec3(0.532419f, -2.75433f, -2.03423f));
        RotateAround(model, glm::vec3(-0.234082f, 2.60953f, 0.338552f), jirFLegs, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        jF_rightleg.Draw(lightingShader);

        // Pata trasera izq
        model = jirafaRoot;
        model = glm::translate(model, glm::vec3(-0.419637f, -3.17289f, 0.769238f));
        RotateAround(model, glm::vec3(0.308814f, 2.65397f, 0.064164f), jirRLegs, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        jB_leftleg.Draw(lightingShader);

        // Pata trasera der
        model = jirafaRoot;
        model = glm::translate(model, glm::vec3(0.419637f, -3.17289f, 0.770694f));
        RotateAround(model, glm::vec3(-0.307823f, 2.65654f, 0.121217f), jirRLegs, glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        jB_rightleg.Draw(lightingShader);

        // ================== LEONA ==================
        glm::mat4 leonaRoot = glm::mat4(1.0f);
        leonaRoot = glm::translate(leonaRoot, leoPos);
        leonaRoot = glm::translate(leonaRoot, glm::vec3(3.0f, -2.7f, 31.0f));
        leonaRoot = glm::scale(leonaRoot, glm::vec3(0.8f));
        leonaRoot = glm::rotate(leonaRoot, glm::radians(leoRot), glm::vec3(0.0f, 1.0f, 0.0f));

        model = leonaRoot;
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        l_body.Draw(lightingShader);

        // Cabeza
        model = leonaRoot;
        model = glm::translate(model, glm::vec3(0.000025f, 0.613692f, 2.63399f));
        RotateAround(model, glm::vec3(0.01017f, 0.007062f, -0.902425f), leoHead, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        l_Head.Draw(lightingShader);

        // Cola
        model = leonaRoot;
        model = glm::translate(model, glm::vec3(0.000024f, -0.058634f, -2.65759f));
        RotateAround(model, glm::vec3(0.0f, 0.438905f, 0.70112f), leoTail, glm::vec3(0.0f, 0.0f, -1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        l_tail.Draw(lightingShader);

        // Delantera der
        model = leonaRoot;
        model = glm::translate(model, glm::vec3(-0.335907f, -1.93567f, 1.36874f));
        RotateAround(model, glm::vec3(0.0f, 2.22404f, -0.083255f), leoFLegs, glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        lF_rightleg.Draw(lightingShader);

        // Delantera izq
        model = leonaRoot;
        model = glm::translate(model, glm::vec3(0.335809f, -1.93567f, 1.36874f));
        RotateAround(model, glm::vec3(0.0f, 2.22404f, -0.083255f), leoFLegs, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        lF_leftleg.Draw(lightingShader);

        // Trasera der
        model = leonaRoot;
        model = glm::translate(model, glm::vec3(-0.373882f, -1.88819f, -1.45326f));
        RotateAround(model, glm::vec3(0.0f, 2.02932f, 0.211069f), leoRLegs, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        lB_rightleg.Draw(lightingShader);

        // Trasera izq
        model = leonaRoot;
        model = glm::translate(model, glm::vec3(0.373784f, -1.88819f, -1.45326f));
        RotateAround(model, glm::vec3(0.0f, 2.02932f, 0.211069f), leoRLegs, glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        lB_leftleg.Draw(lightingShader);

        // ================== CEBRA ==================
        glm::mat4 cebraRoot = glm::mat4(1.0f);
        cebraRoot = glm::translate(cebraRoot, zebPos);
        cebraRoot = glm::translate(cebraRoot, glm::vec3(-4.0f, -9.3f, 34.0f));
        cebraRoot = glm::scale(cebraRoot, glm::vec3(0.08f));
        cebraRoot = glm::rotate(cebraRoot, glm::radians(zebRot), glm::vec3(0.0f, 1.0f, 0.0f));

        model = cebraRoot;
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        z_body.Draw(lightingShader);

        // Cabeza
        model = cebraRoot;
        model = glm::translate(model, glm::vec3(0.000025f, 6.51619f, 30.5584f));
        RotateAround(model, glm::vec3(0.103417f , -3.20447f, 13.3856f ), zebHead, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        z_Head.Draw(lightingShader);

        // Cola
        model = cebraRoot;
        model = glm::translate(model, glm::vec3(0.0f, -2.7f, -25.82f));
        RotateAround(model, glm::vec3(0.0f, 6.1442f, -7.09142f), zebTail, glm::vec3(0.0f, 0.0f, -1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        z_tail.Draw(lightingShader);

        // Delantera der
        model = cebraRoot;
        model = glm::translate(model, glm::vec3(4.14623f, -17.6062f, 14.477f));
        RotateAround(model, glm::vec3(-1.9753f, 15.7052f, 0.961041f), zebFLegs, glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        zF_leftleg.Draw(lightingShader);

        // Delantera izq
        model = cebraRoot;
        model = glm::translate(model, glm::vec3(-4.2194f, -17.5819f, 14.5057f));
        RotateAround(model, glm::vec3(-1.9753f, 15.7052f, 0.961041f), zebFLegs, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        zF_rightleg.Draw(lightingShader);

        // Trasera izq
        model = cebraRoot;
        model = glm::translate(model, glm::vec3(4.17668f, -17.2324f, -14.5597f));
        RotateAround(model, glm::vec3(0.0f, 22.1421f, -2.52463f), zebRLegs, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        zB_leftleg.Draw(lightingShader);

        // Trasera der
        model = cebraRoot;
        model = glm::translate(model, glm::vec3(-4.17668f, -17.2324f, -14.5597f));
        RotateAround(model, glm::vec3(0.0f, 22.1421f, -2.52463f), zebRLegs, glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        zB_rightleg.Draw(lightingShader);

        // ================== LOBO Minecraft ==================
        glm::mat4 pbRoot = glm::mat4(1.0f);
        pbRoot = glm::translate(pbRoot, pbPos);
        pbRoot = glm::translate(pbRoot, glm::vec3(20.0f, -8.5f, 16.0f));
        pbRoot = glm::scale(pbRoot, glm::vec3(0.25f));
        pbRoot = glm::rotate(pbRoot, glm::radians(pbRot), glm::vec3(0.0f, 1.0f, 0.0f));

        // Cuerpo
        model = pbRoot;
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        pb_body.Draw(lightingShader);

        // Cabeza
        model = pbRoot;
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.10306f));
        RotateAround(model, glm::vec3(0.0f), pbHead, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        pb_head.Draw(lightingShader);

        // Cola
        model = pbRoot;
        model = glm::translate(model, glm::vec3(0.0f, 1.45762f, 4.7439f));
        RotateAround(model, glm::vec3(0.0f), pbTail, glm::vec3(0.0f, 0.0f, -1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        pb_tail.Draw(lightingShader);

        // Delantera derecha
        model = pbRoot;
        model = glm::translate(model, glm::vec3(-1.06755f, -4.18697f, -3.11546f));
        RotateAround(model, glm::vec3(0.0f, 2.51375f, 0.0f), pbFLegs, glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        pbF_rightleg.Draw(lightingShader);

        // Delantera izquierda
        model = pbRoot;
        model = glm::translate(model, glm::vec3(1.06755f, -4.18697f, -3.11546f));
        RotateAround(model, glm::vec3(0.0f, 2.51375f, 0.0f), pbFLegs, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        pbF_leftleg.Draw(lightingShader);

        // Trasera derecha
        model = pbRoot;
        model = glm::translate(model, glm::vec3(1.24672f, -4.18697f, 3.11546f));
        RotateAround(model, glm::vec3(0, 2.51375f, 0), pbRLegs, glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        pbB_rightleg.Draw(lightingShader);

        // Trasera izquierda
        model = pbRoot;
        model = glm::translate(model, glm::vec3(-1.06755f, -4.18697f, 3.11546f));
        RotateAround(model, glm::vec3(0.0f, 2.51375f, 0.0f), pbRLegs, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        pbB_rightleg.Draw(lightingShader);

        // ================== PINGUINO ==================
        glm::mat4 foxRoot = glm::mat4(1.0f);
        foxRoot = glm::translate(foxRoot, foxPos);
		foxRoot = glm::translate(foxRoot, glm::vec3(22.0f, -10.60f, -20.0f));
        foxRoot = glm::scale(foxRoot, glm::vec3(0.28f));
        foxRoot = glm::rotate(foxRoot, glm::radians(foxRot), glm::vec3(0.0f, 1.0f, 0.0f));

        // Cuerpo
        model = foxRoot;
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        f_body.Draw(lightingShader);

        // Cabeza
        model = foxRoot;
        model = glm::translate(model, glm::vec3(0.0f, 2.23485f, 0.760477f));
        RotateAround(model, glm::vec3(0, -0.520997f, -0.566971f), foxHead, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        f_head.Draw(lightingShader);

        // Ala derecha
        model = foxRoot;
        model = glm::translate(model, glm::vec3(-2.19607f, -0.579184f, -0.352313f));
        RotateAround(model, glm::vec3(1.31875f, 0.453932f, 0.047048f), foxFLegs, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        fF_rightleg.Draw(lightingShader);

        // Ala izquierda
        model = foxRoot;
        model = glm::translate(model, glm::vec3(2.22225f, -0.589658f, -0.352313f));
        RotateAround(model, glm::vec3(-1.34585f, 0.423875f, 0.040529f), foxFLegs, glm::vec3(0.0f, -1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        fF_leftleg.Draw(lightingShader);

        // Pata derecha
        model = foxRoot;
        model = glm::translate(model, glm::vec3(-0.626399f, -4.30908f, -0.350244f));
        RotateAround(model, glm::vec3(0.109641f, 0.632256f, -0.720169f), foxRLegs, glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        fB_rightleg.Draw(lightingShader);

        // Pata izquierda
        model = foxRoot;
        model = glm::translate(model, glm::vec3(0.6264f, -4.30908f, -0.350244f));
        RotateAround(model, glm::vec3(-0.089517f, 0.644821f, -0.719191f), foxRLegs, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        fB_leftleg.Draw(lightingShader);

        // ================== PISO ==================
        glm::mat4 pisoRoot = glm::mat4(1.0f);
        pisoRoot = glm::translate(pisoRoot, glm::vec3(0.0f, -5.0f, 0.0f));
        pisoRoot = glm::scale(pisoRoot, glm::vec3(0.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pisoRoot));
        Piso.Draw(lightingShader);

        // ---------- Lámpara visual ----------
        lampShader.Use();
        modelLoc = glGetUniformLocation(lampShader.Program, "model");
        GLint viewLoc2 = glGetUniformLocation(lampShader.Program, "view");
        GLint projLoc2 = glGetUniformLocation(lampShader.Program, "projection");
        glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc2, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(0);

        // ---------- Skybox ----------
        glDepthFunc(GL_LEQUAL);
        skyboxShader.Use();
        glm::mat4 viewNoTrans = glm::mat4(glm::mat3(view)); // sin traslación
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(viewNoTrans));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        // Swap
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// Movimiento: cámara + rig activo
void DoMovement()
{
    // Cámara
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])    camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])  camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])  camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);

    auto add = [](float& v, float d) { v += d; };
    const float dPos = 0.001f;
    const float dAng = 0.3f;

    switch (activeRig) {
    case RIG_JIR: {
        if (keys[GLFW_KEY_G]) add(girPos.x, -dPos);
        if (keys[GLFW_KEY_J]) add(girPos.x, dPos);
        if (keys[GLFW_KEY_Y]) add(girPos.z, dPos);
        if (keys[GLFW_KEY_H]) add(girPos.z, -dPos);

        if (keys[GLFW_KEY_Q]) add(jirRot, dAng);
        if (keys[GLFW_KEY_E]) add(jirRot, -dAng);

        if (keys[GLFW_KEY_4]) add(jirHead, dAng);
        if (keys[GLFW_KEY_5]) add(jirHead, -dAng);
        if (keys[GLFW_KEY_6]) add(jirTail, dAng);
        if (keys[GLFW_KEY_7]) add(jirTail, -dAng);
        if (keys[GLFW_KEY_1]) add(jirFLegs, dAng);
        if (keys[GLFW_KEY_0]) add(jirFLegs, -dAng);
        if (keys[GLFW_KEY_8]) add(jirRLegs, dAng);
        if (keys[GLFW_KEY_9]) add(jirRLegs, -dAng);
    } break;

    case RIG_LEO: {
        if (keys[GLFW_KEY_G]) add(leoPos.x, -dPos);
        if (keys[GLFW_KEY_J]) add(leoPos.x, dPos);
        if (keys[GLFW_KEY_Y]) add(leoPos.z, dPos);
        if (keys[GLFW_KEY_H]) add(leoPos.z, -dPos);

        if (keys[GLFW_KEY_Q]) add(leoRot, dAng);
        if (keys[GLFW_KEY_E]) add(leoRot, -dAng);

        if (keys[GLFW_KEY_4]) add(leoHead, dAng);
        if (keys[GLFW_KEY_5]) add(leoHead, -dAng);
        if (keys[GLFW_KEY_6]) add(leoTail, dAng);
        if (keys[GLFW_KEY_7]) add(leoTail, -dAng);
        if (keys[GLFW_KEY_1]) add(leoFLegs, dAng);
        if (keys[GLFW_KEY_0]) add(leoFLegs, -dAng);
        if (keys[GLFW_KEY_8]) add(leoRLegs, dAng);
        if (keys[GLFW_KEY_9]) add(leoRLegs, -dAng);
    } break;

    case RIG_ZEB: {
        if (keys[GLFW_KEY_G]) add(zebPos.x, -dPos);
        if (keys[GLFW_KEY_J]) add(zebPos.x, dPos);
        if (keys[GLFW_KEY_Y]) add(zebPos.z, dPos);
        if (keys[GLFW_KEY_H]) add(zebPos.z, -dPos);

        if (keys[GLFW_KEY_Q]) add(zebRot, dAng);
        if (keys[GLFW_KEY_E]) add(zebRot, -dAng);

        if (keys[GLFW_KEY_4]) add(zebHead, dAng);
        if (keys[GLFW_KEY_5]) add(zebHead, -dAng);
        if (keys[GLFW_KEY_6]) add(zebTail, dAng);
        if (keys[GLFW_KEY_7]) add(zebTail, -dAng);
        if (keys[GLFW_KEY_1]) add(zebFLegs, dAng);
        if (keys[GLFW_KEY_0]) add(zebFLegs, -dAng);
        if (keys[GLFW_KEY_8]) add(zebRLegs, dAng);
        if (keys[GLFW_KEY_9]) add(zebRLegs, -dAng);
    } break;

    case RIG_PB: {
        if (keys[GLFW_KEY_G]) add(pbPos.x, -dPos);
        if (keys[GLFW_KEY_J]) add(pbPos.x, dPos);
        if (keys[GLFW_KEY_Y]) add(pbPos.z, dPos);
        if (keys[GLFW_KEY_H]) add(pbPos.z, -dPos);

        if (keys[GLFW_KEY_Q]) add(pbRot, dAng);
        if (keys[GLFW_KEY_E]) add(pbRot, -dAng);

        if (keys[GLFW_KEY_4]) add(pbHead, dAng);
        if (keys[GLFW_KEY_5]) add(pbHead, -dAng);
        if (keys[GLFW_KEY_6]) add(pbTail, dAng);
        if (keys[GLFW_KEY_7]) add(pbTail, -dAng);
        if (keys[GLFW_KEY_1]) add(pbFLegs, dAng);
        if (keys[GLFW_KEY_0]) add(pbFLegs, -dAng);
        if (keys[GLFW_KEY_8]) add(pbRLegs, dAng);
        if (keys[GLFW_KEY_9]) add(pbRLegs, -dAng);
    } break;

    case RIG_FOX: {
        if (keys[GLFW_KEY_G]) add(foxPos.x, -dPos);
        if (keys[GLFW_KEY_J]) add(foxPos.x, dPos);
        if (keys[GLFW_KEY_Y]) add(foxPos.z, dPos);
        if (keys[GLFW_KEY_H]) add(foxPos.z, -dPos);

        if (keys[GLFW_KEY_Q]) add(foxRot, dAng);
        if (keys[GLFW_KEY_E]) add(foxRot, -dAng);

        if (keys[GLFW_KEY_4]) add(foxHead, dAng);
        if (keys[GLFW_KEY_5]) add(foxHead, -dAng);
        if (keys[GLFW_KEY_6]) add(foxTail, dAng);
        if (keys[GLFW_KEY_7]) add(foxTail, -dAng);
        if (keys[GLFW_KEY_1]) add(foxFLegs, dAng);
        if (keys[GLFW_KEY_0]) add(foxFLegs, -dAng);
        if (keys[GLFW_KEY_8]) add(foxRLegs, dAng);
        if (keys[GLFW_KEY_9]) add(foxRLegs, -dAng);
    } break;

    case RIG_RHINO: {
        if (keys[GLFW_KEY_G]) add(rhPos.x, -dPos);
        if (keys[GLFW_KEY_J]) add(rhPos.x, dPos);
        if (keys[GLFW_KEY_Y]) add(rhPos.z, dPos);
        if (keys[GLFW_KEY_H]) add(rhPos.z, -dPos);

        if (keys[GLFW_KEY_Q]) add(rhRot, dAng);
        if (keys[GLFW_KEY_E]) add(rhRot, -dAng);

        if (keys[GLFW_KEY_4]) add(rhHead, dAng);
        if (keys[GLFW_KEY_5]) add(rhHead, -dAng);
        if (keys[GLFW_KEY_6]) add(rhTail, dAng);
        if (keys[GLFW_KEY_7]) add(rhTail, -dAng);
        if (keys[GLFW_KEY_1]) add(rhFLegs, dAng);
        if (keys[GLFW_KEY_0]) add(rhFLegs, -dAng);
        if (keys[GLFW_KEY_8]) add(rhRLegs, dAng);
        if (keys[GLFW_KEY_9]) add(rhRLegs, -dAng);
    } break;
    }
}

// Teclado
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)   keys[key] = true;
        else if (action == GLFW_RELEASE) keys[key] = false;
    }

    // Selección de rig
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_F1) activeRig = RIG_JIR;
        if (key == GLFW_KEY_F2) activeRig = RIG_LEO;
        if (key == GLFW_KEY_F3) activeRig = RIG_ZEB;
        if (key == GLFW_KEY_F4) activeRig = RIG_PB;
        if (key == GLFW_KEY_F5) activeRig = RIG_FOX;
        if (key == GLFW_KEY_F6) activeRig = RIG_RHINO;

        // Guardar keyframe del rig activo
        if (key == GLFW_KEY_K) {
            switch (activeRig) {
            case RIG_JIR:   if (idxJir < MAX_FRAMES) saveJir();   break;
            case RIG_LEO:   if (idxLeo < MAX_FRAMES) saveLeo();   break;
            case RIG_ZEB:   if (idxZeb < MAX_FRAMES) saveZeb();   break;
            case RIG_PB:    if (idxPb < MAX_FRAMES) savePb();    break;
            case RIG_FOX:   if (idxFox < MAX_FRAMES) saveFox();   break;
            case RIG_RHINO: if (idxRhino < MAX_FRAMES) saveRhino(); break;
            }
        }

        // Reproducir/Pausar rig activo
        if (key == GLFW_KEY_L) {
            switch (activeRig) {
            case RIG_JIR:
                if (!playJir && idxJir > 1) { resetJir(); curJir = 0; segJir = 0; interpJir(); playJir = true; }
                else playJir = false;
                break;
            case RIG_LEO:
                if (!playLeo && idxLeo > 1) { resetLeo(); curLeo = 0; segLeo = 0; interpLeo(); playLeo = true; }
                else playLeo = false;
                break;
            case RIG_ZEB:
                if (!playZeb && idxZeb > 1) { resetZeb(); curZeb = 0; segZeb = 0; interpZeb(); playZeb = true; }
                else playZeb = false;
                break;
            case RIG_PB:
                if (!playPb && idxPb > 1) { resetPb(); curPb = 0; segPb = 0; interpPb(); playPb = true; }
                else playPb = false;
                break;
            case RIG_FOX:
                if (!playFox && idxFox > 1) { resetFox(); curFox = 0; segFox = 0; interpFox(); playFox = true; }
                else playFox = false;
                break;
            case RIG_RHINO:
                if (!playRhino && idxRhino > 1) { resetRhino(); curRhino = 0; segRhino = 0; interpRhino(); playRhino = true; }
                else playRhino = false;
                break;
            }
        }

        // Reset al keyframe 0 del rig activo
        if (key == GLFW_KEY_R) {
            switch (activeRig) {
            case RIG_JIR:   resetJir();   playJir = false;   break;
            case RIG_LEO:   resetLeo();   playLeo = false;   break;
            case RIG_ZEB:   resetZeb();   playZeb = false;   break;
            case RIG_PB:    resetPb();    playPb = false;    break;
            case RIG_FOX:   resetFox();   playFox = false;   break;
            case RIG_RHINO: resetRhino(); playRhino = false; break;
            }
        }
    }
}

// Animación
void Animation()
{
    // Jirafa
    if (playJir) {
        if (segJir >= MAX_STEPS) {
            curJir++;
            if (curJir > idxJir - 2) { playJir = false; curJir = 0; }
            else { segJir = 0; interpJir(); }
        }
        else { tickJir(); segJir++; }
    }

    // Leona
    if (playLeo) {
        if (segLeo >= MAX_STEPS) {
            curLeo++;
            if (curLeo > idxLeo - 2) { playLeo = false; curLeo = 0; }
            else { segLeo = 0; interpLeo(); }
        }
        else { tickLeo(); segLeo++; }
    }

    // Cebra
    if (playZeb) {
        if (segZeb >= MAX_STEPS) {
            curZeb++;
            if (curZeb > idxZeb - 2) { playZeb = false; curZeb = 0; }
            else { segZeb = 0; interpZeb(); }
        }
        else { tickZeb(); segZeb++; }
    }

    // Oso polar
    if (playPb) {
        if (segPb >= MAX_STEPS) {
            curPb++;
            if (curPb > idxPb - 2) { playPb = false; curPb = 0; }
            else { segPb = 0; interpPb(); }
        }
        else { tickPb(); segPb++; }
    }

    // Zorro
    if (playFox) {
        if (segFox >= MAX_STEPS) {
            curFox++;
            if (curFox > idxFox - 2) { playFox = false; curFox = 0; }
            else { segFox = 0; interpFox(); }
        }
        else { tickFox(); segFox++; }
    }

    // Rinoceronte
    if (playRhino) {
        if (segRhino >= MAX_STEPS) {
            curRhino++;
            if (curRhino > idxRhino - 2) { playRhino = false; curRhino = 0; }
            else { segRhino = 0; interpRhino(); }
        }
        else { tickRhino(); segRhino++; }
    }
}

// Mouse
void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse) { lastX = (GLfloat)xPos; lastY = (GLfloat)yPos; firstMouse = false; }
    GLfloat xOffset = (GLfloat)xPos - lastX;
    GLfloat yOffset = lastY - (GLfloat)yPos; // invertido
    lastX = (GLfloat)xPos;
    lastY = (GLfloat)yPos;
    camera.ProcessMouseMovement(xOffset, yOffset);
}

// Carga de cubemap
GLuint loadCubemap(const std::vector<std::string>& faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false); // para cubemaps normalmente no se voltea

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format = GL_RGB;
            if (nrChannels == 4) format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "Failed to load cubemap face: " << faces[i] << std::endl;
        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
