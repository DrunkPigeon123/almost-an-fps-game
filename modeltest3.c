#include <stdio.h>
#include "raylib.h"
#include <math.h> // For sinf, cosf
#include <raymath.h>
#include <stdbool.h>

#define SENSITIVITY 0.1f
#define MOVE_SPEED 0.1f
#define PITCH_LIMIT 9.0f
#define MAX_RECTANGLES 15 // Number of moving rectangles


typedef struct MovingRectangle {
    Vector3 position; // Rectangle's position
    Vector3 velocity; // Rectangle's velocity
    float width;
    float height;
    float health;
} MovingRectangle;

typedef struct Manequin {
    Vector3 position;
    bool shoot;
    Model model;
    ModelAnimation animations;
} Manequin;

float MinimalAngleByDistance(Vector3 distanceVector) { 
    return sqrtf(distanceVector.x * distanceVector.x +
                 distanceVector.y * distanceVector.y +
                 distanceVector.z * distanceVector.z);
}

bool CalculateAngleDifference(Vector3 playerPos, Vector3 targetPos, Vector3 playerForward) {
    Vector3 directionToTarget = Vector3Subtract(targetPos, playerPos);
    directionToTarget = Vector3Normalize(directionToTarget);
    float dotProduct = Vector3DotProduct(playerForward, directionToTarget);
    float angleDifference = acosf(dotProduct);

    if (MinimalAngleByDistance(directionToTarget) > 1.0f) {
        return fabs(angleDifference) < 0.05f;// / (MinimalAngleByDistance(directionToTarget) * 20);
    } else {
        return fabs(angleDifference) < 0.2f;
    }
}

bool CalculateAngleDifference2(Vector3 playerPos, Vector3 targetPos, Vector3 playerForward)
{
    Vector3 distanceToTarget = Vector3Subtract(targetPos, playerPos);
    Vector3 directionToTarget = Vector3Normalize(distanceToTarget);
    Vector3 NForward = Vector3Normalize(playerForward);
    if(NForward.x>directionToTarget.x - 0.05f && NForward.x<directionToTarget.x + 0.05f && NForward.y>directionToTarget.y - 0.05f && NForward.y<directionToTarget.y + 0.05f && Vector3DotProduct(NForward, distanceToTarget) > 0.0f)
    {
        return true;
    }
    else return false;
}

/*bool CalculateAngleDifference3(Vector3 playerPos, Vector3 targetPos, Vector3 playerForward)
{
    
    Vector3 Nforward = Vector3Normalize(playerForward);
    bool cvar = false;
    float x[100];
    float y[100];
    float z[100];
    for(int i=0;i<100;i++)
    {
        x[i] = playerPos.x + Nforward.x * i;
        y[i] = playerPos.y + Nforward.y * i;
        z[i] = playerPos.z + Nforward.z * i;

    }
    
        for(int i=0;i<100;i++)
        {
            if(fabs(targetPos.x - x[i]) < 0.6f && 
               fabs(targetPos.y - y[i]) < 0.6f && 
               fabs(targetPos.z - z[i]) < 0.6f) cvar = true;
        }
    return cvar;
}*/
/*
bool CalculateAngleDifference3(Vector3 playerPos, Vector3 targetPos, Vector3 playerForward) {
    Vector3 normalizedForward = Vector3Normalize(playerForward);
    Vector3 directionToTarget = Vector3Subtract(targetPos, playerPos);
    float distanceToTarget = Vector3Length(directionToTarget);
    if (distanceToTarget > 30.0f) return false;
    float dotProduct = Vector3DotProduct(normalizedForward, directionToTarget);
    if (dotProduct < 0.0f) return false;
    Vector3 closestPoint = Vector3Add(playerPos, Vector3Scale(normalizedForward, distanceToTarget)); // Changed from dotProduct
    float distanceFromRay = Vector3Distance(targetPos, closestPoint);
    return distanceFromRay < 0.6f;
}*/
bool CalculateAngleDifference3(Vector3 playerPos, Vector3 targetPos, Vector3 playerForward) {
    Vector3 normalizedForward = Vector3Normalize(playerForward);
    Vector3 directionToTarget = Vector3Subtract(targetPos, playerPos);
    float distanceToTarget = Vector3Length(directionToTarget);
    if (distanceToTarget > 30.0f) return false;
    Vector3 closestPoint = Vector3Add(playerPos, Vector3Scale(normalizedForward, distanceToTarget)); // Changed from dotProduct
    float distanceFromRay = Vector3Distance(targetPos, closestPoint);
    return distanceFromRay < 0.6f;
}
bool CalculateAngleDifference4(Vector3 playerPos, Vector3 targetPos, Vector3 playerForward) {
    Vector3 normalizedForward = Vector3Normalize(playerForward);
    Ray aim = {playerPos,Vector3Scale(normalizedForward,100)};
    //GetRayCollisionBox(aim,boundingbox[i])
    }

//DrawGun()

// Function to check if the model is rotated enough to face the player
bool ShotModelAI(Vector3 modelPosition, Vector3 playerPosition, float angleThreshold) {
    // Get the direction from the model to the player
    Vector3 directionToPlayer = Vector3Subtract(playerPosition, modelPosition);
    directionToPlayer = Vector3Normalize(directionToPlayer);
    
    directionToPlayer.y = 0.0f;
    
    // Get the model's forward direction (assuming model is facing along the Z-axis)
    Vector3 modelForward = { 1.0f, 0.0f, 0.0f }; // The model's forward direction

    // Calculate the angle between the model's forward vector and the direction to the player
    float angleBetween = acosf(Vector3DotProduct(modelForward, directionToPlayer));

    // If the angle between is less than the threshold, return true (meaning the model is facing the player)
    return (angleBetween < angleThreshold);
}

float ModelAngle(Vector3 modelPosition, Vector3 playerPosition)
{
// Get the direction from the model to the player
    Vector3 directionToPlayer = Vector3Subtract(playerPosition, modelPosition);
    directionToPlayer = Vector3Normalize(directionToPlayer);
    
    directionToPlayer.y = 0.0f;
    
    // Get the model's forward direction (assuming model is facing along the Z-axis)
    Vector3 modelForward = { 1.0f, 0.0f, 0.0f }; // The model's forward direction

    // Calculate the angle between the model's forward vector and the direction to the player
    float angleBetween = acosf(Vector3DotProduct(modelForward, directionToPlayer));
    
    return angleBetween;
}

// Function to rotate the model using a matrix
void RotateModelOnce(Model* model, float angle, Vector3 axis) {
    // Create a rotation matrix
    Matrix rotationMatrix = MatrixRotate(axis, angle);

    // Apply the rotation to the model's transformation matrix
    model->transform = MatrixMultiply(model->transform, rotationMatrix);
}

bool ToggleEverySecond() {
    static double lastTime = 0.0;
    double currentTime = GetTime();
    
    if (currentTime - lastTime >= 0.8) {
        lastTime = currentTime;
        return true;
    }
    
    return false;
}

void DrawRectangles(MovingRectangle rectangles[])
{
     for (int i = 0; i < MAX_RECTANGLES; i++) {
            DrawCube(rectangles[i].position, rectangles[i].width, 0.5f, rectangles[i].height, RED);
        }
}


int main() {
    const int screenWidth = 1600;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Upgraded 3D Camera Movement with Moving Rectangles");

    InitAudioDevice();

    // Load sound effect
    Sound shootSound = LoadSound("shoot.wav");
    SetSoundVolume(shootSound, 0.2f);

    Camera camera = {
        .position = {0.0f, 1.0f, 5.0f},
        .target = {0.0f, 1.0f, 4.0f},
        .up = {0.0f, 1.0f, 0.0f},
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE
    };

    Vector3 playerPos = {0.0f, 1.0f, 5.0f};
    float playerYaw = 0.0f;
    float playerPitch = 0.0f;

    // Load gun model and animations
    Model gun = LoadModel("gunanimated.glb");
    int animCountGun = 0;
    ModelAnimation *gunAnimations = LoadModelAnimations("gunanimated.glb", &animCountGun);

    Model shotModel = LoadModel("shotmodel.glb");
    int animCountShot = 0;
    ModelAnimation *shotAnimations = LoadModelAnimations("shotmodel.glb", &animCountShot);

    if (gun.meshCount == 0 || shotModel.meshCount == 0) {
        printf("Failed to load one or both models!");
        CloseWindow();
        CloseAudioDevice();
        return 1;
    }

    printf("Models and animations loaded successfully!");

    // Disable cursor and set frame rate
    DisableCursor();
    SetTargetFPS(60);

    Vector3 gunPosition = playerPos;
    gunPosition.y -= 0.066f;
    Vector3 modelPosition = {3.0f,0.0f,0.0f};
    float gunAnimationTime = 0.0f;
    int gunCurrentAnim = 1;
    float gunAnimationSpeed = 24.0f;
    bool isPlayingGunAnimation = false;

    float shotAnimationTime = 0.0f;
    int shotCurrentAnim = 0;
    float shotAnimationSpeed = 24.0f;
    bool isPlayingShotAnimation = false;
    RotateModelOnce(&shotModel,PI/2,(Vector3){1.0f,0.0f,0.0f});
    int hits = 0;
    
    // Initialize moving rectangles
    MovingRectangle rectangles[MAX_RECTANGLES];
    for (int i = 0; i < MAX_RECTANGLES; i++) {
        rectangles[i].position = (Vector3){GetRandomValue(-10, 10), 1.0f, GetRandomValue(-10, 10)};
        rectangles[i].velocity = (Vector3){GetRandomValue(-1, 1) * 0.05f, 0.0f, GetRandomValue(-1, 1) * 0.05f};
        rectangles[i].width = 1.0f;
        rectangles[i].height = 1.0f;
        rectangles[i].health = 100;
    }

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        Vector2 mouseDelta = GetMouseDelta();
        playerYaw -= mouseDelta.x * SENSITIVITY;
        playerPitch -= mouseDelta.y * SENSITIVITY;
        
        playerYaw = fmod(playerYaw, 360.0f);
        if (playerYaw < 0.0f) {
            playerYaw += 360.0f;
        }
        
        if (playerPitch > PITCH_LIMIT) playerPitch = PITCH_LIMIT;
        if (playerPitch < -PITCH_LIMIT) playerPitch = -PITCH_LIMIT;

        Vector3 forward = {
            sinf(DEG2RAD * playerYaw),
            0.0f,
            cosf(DEG2RAD * playerYaw)
        };
        Vector3 right = {
            sinf(DEG2RAD * (playerYaw + 90.0f)),
            0.0f,
            cosf(DEG2RAD * (playerYaw + 90.0f))
        };

        if (IsKeyDown(KEY_W)) {
            playerPos.x += forward.x * MOVE_SPEED;
            playerPos.z += forward.z * MOVE_SPEED;
        }
        if (IsKeyDown(KEY_S)) {
            playerPos.x -= forward.x * MOVE_SPEED;
            playerPos.z -= forward.z * MOVE_SPEED;
        }
        if (IsKeyDown(KEY_D)) {
            playerPos.x -= right.x * MOVE_SPEED;
            playerPos.z -= right.z * MOVE_SPEED;
        }
        if (IsKeyDown(KEY_A)) {
            playerPos.x += right.x * MOVE_SPEED;
            playerPos.z += right.z * MOVE_SPEED;
        }

        camera.position = playerPos;
        camera.target = (Vector3){
            playerPos.x + sinf(DEG2RAD * playerYaw) * cosf(DEG2RAD * playerPitch),
            playerPos.y + sinf(DEG2RAD * playerPitch),
            playerPos.z + cosf(DEG2RAD * playerYaw) * cosf(DEG2RAD * playerPitch)
        };
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && ToggleEverySecond()) {
            isPlayingGunAnimation = true;
            gunAnimationTime = 0.0f;
            PlaySound(shootSound);
            for (int i = 0; i < MAX_RECTANGLES; i++) {
                if (CalculateAngleDifference3(playerPos, rectangles[i].position, forward)) {
                    rectangles[i].health-=25;
                    if(rectangles[i].health==0)rectangles[i].position = (Vector3){100, 100, 100};
                    DrawText("HIT", screenWidth / 2, screenHeight / 2, 20, PURPLE);
                    hits++;
                }
            }
        }
        
        

         if(ShotModelAI(modelPosition,playerPos,0.3f)){isPlayingShotAnimation=true;}

        DrawText(TextFormat("Score: %d", hits), 100, 100, 20, DARKGRAY);
      
        if (isPlayingGunAnimation && animCountGun > 0) {
            gunAnimationTime += deltaTime * gunAnimationSpeed;
            if (gunAnimationTime >= gunAnimations[gunCurrentAnim].frameCount) {
                gunAnimationTime = 0.0f;
                isPlayingGunAnimation = false;
            }
            int currentGunFrame = (int)gunAnimationTime;
            UpdateModelAnimation(gun, gunAnimations[gunCurrentAnim], currentGunFrame);
        }

        if (isPlayingShotAnimation && animCountShot > 0) {
            shotAnimationTime += deltaTime * shotAnimationSpeed;
            if (shotAnimationTime >= shotAnimations[shotCurrentAnim].frameCount) {
                shotAnimationTime = 0.0f;
                isPlayingShotAnimation = false;
            }
            int currentShotFrame = (int)shotAnimationTime;
            UpdateModelAnimation(shotModel, shotAnimations[shotCurrentAnim], currentShotFrame);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawEllipse(screenWidth / 2, screenHeight / 2, 3, 3, BLACK);
        BeginMode3D(camera);
        DrawGrid(20, 1.0f);

        /*for (int i = 0; i < MAX_RECTANGLES; i++) {
            DrawCube(rectangles[i].position, rectangles[i].width, 0.5f, rectangles[i].height, RED);
        }*/
        DrawRectangles(rectangles);

        gunPosition.x = playerPos.x + forward.x * 0.05f + right.x * -0.025f;
        gunPosition.z = playerPos.z + forward.z * 0.05f + right.z * -0.025f;

        DrawModelEx(gun, gunPosition, (Vector3){0.0f, 1.0f, 0.0f}, playerYaw + 95.0f, (Vector3){0.01f, 0.01f, 0.01f}, WHITE);
        DrawModelEx(shotModel, modelPosition , (Vector3){0.0f, 1.0f, 0.0f}, 95.0f, (Vector3){0.01f, 0.01f, 0.01f}, RED);
        
        EndMode3D();

        DrawText("shoot!", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    UnloadSound(shootSound);
    UnloadModel(gun);
    UnloadModel(shotModel);
    UnloadModelAnimations(gunAnimations, animCountGun);
    UnloadModelAnimations(shotAnimations, animCountShot);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}


