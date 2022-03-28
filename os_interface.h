#pragma once

#include "mathematics.h"

#define MOUSE_BUTTON_LEFT 0 
#define MOUSE_BUTTON_MIDDLE 1 
#define MOUSE_BUTTON_RIGHT 2 

struct FileHandle {
    void* handle;
};

struct WorkEntry {
    void (*function)(void*);
    void* data;
};

struct WorkQueue {
    static const u32 MAX_ENTRIES = 256;
    WorkEntry entries[MAX_ENTRIES];
    volatile u32 entryAddPos;
    volatile u32 entryStartPos;
    volatile u32 entriesAdded;
    volatile u32 entriesCompleted;
    void* semaphore;
};

struct AudioEmitter {
    void* emitter;
};

struct Texture2D {
    void* data1;
    void* data2;
    u32 format;
};

struct TextureCube {
    void* data1;
    void* data2;
    u32 format;
};

struct Model3D {
    Vector3 position;
    Vector3 scale;
    Quaternion orientation;
    Texture2D texture;
    Texture2D normalMap;
    u32 totalIndices;
    u32 indexOffset;
    u32 vertexOffset;
    u32 vertexSize;
};

struct Animesh {
    Vector3 position;
    Vector3 scale;
    Quaternion orientation;
    Texture2D texture;
    Texture2D normalMap;
    u32 totalIndices;
    u32 indexOffset;
    u32 vertexOffset;
    u32 vertexSize;
};

struct Skeleton {
    Matrix4* inverseBindTransforms;
    Matrix4* globalPositions;
    Quaternion* orientations;
    Vector3* positions;
    u32* parentIndices;
    u32 totalBones;

    void updateGlobalPositions(){
        globalPositions[0] = buildModelMatrix(positions[0], Vector3(1), orientations[0]);
        Matrix4 parents[32];
        parents[0] = globalPositions[0];
        for(u32 i = 1; i < totalBones; i++){
            u32 pi = parentIndices[i];
            globalPositions[i] = parents[pi] * buildModelMatrix(positions[i], Vector3(1), orientations[i]);
            parents[i] = globalPositions[i];
        }
    }

    void updatePositionsWithTarget(Vector3 target, Vector3 offset, u32 boneIndex){
        u32 bip1 = boneIndex + 1;
        u32 bip2 = boneIndex + 2;
        f32 ll1 = length(position(&globalPositions[bip1]) - position(&globalPositions[boneIndex]));
        f32 ll2 = length(position(&globalPositions[bip2]) - position(&globalPositions[bip1]));
        Matrix4 gpm = globalPositions[boneIndex];
        Vector3 gp = position(&gpm);
        Vector3 dif = target - (gp - offset);
        f32 q0 = atan2(dif.z, dif.x);
        Vector2 dif2(target.x, target.z);
        dif2 = rotateAroundPoint(dif2, Vector2(positions[boneIndex].x, positions[boneIndex].z), q0);
        f32 d2 = dif2.x * dif2.x + dif.y * dif.y;
        f32 lt2 = 2 * ll1 * ll1;
        f32 cosA = (d2 - ll1 * ll1 - ll2 * ll2) / lt2;
        if(cosA < -1) cosA = -1;
        else if(cosA > 1) cosA = 1;
        f32 q2 = acos(cosA);
        q2 = -q2;
        f32 q1 = atan2(dif2.x, dif.y) - atan2(ll2 * sin(q2), ll1 + ll2 * cos(q2));
        orientations[6] = rotationToQuaternion(Vector3(0, 1, 0), -q0);
        rotate(&orientations[6], Vector3(0, 0, 1), -q1);
        orientations[7] = rotationToQuaternion(Vector3(0, 0, 1), -q2);

        globalPositions[0] = buildModelMatrix(positions[0], Vector3(1), orientations[0]);
        Matrix4 parents[32];
        parents[0] = globalPositions[0];
        for(u32 i = 1; i < totalBones; i++){
            u32 pi = parentIndices[i];
            if(i != boneIndex){
                globalPositions[i] = parents[pi] * buildModelMatrix(positions[i], Vector3(1), orientations[i]);
            }else{
                Matrix4 tm = parents[pi] * buildModelMatrix(positions[i], Vector3(1), orientations[i]);
                globalPositions[i] = buildModelMatrix(positions[i], Vector3(1), orientations[i]);
                globalPositions[i].m2[3][0] = tm.m2[3][0];
                globalPositions[i].m2[3][1] = tm.m2[3][1];
                globalPositions[i].m2[3][2] = tm.m2[3][2];
            }
             parents[i] = globalPositions[i];
        }
    }
};

struct Pose {
    Quaternion* orientations;
    Vector3 position;
};

struct Animation {
    Pose currentPose;
    Pose* poses;
    f32* frameLengths;
    f32 t;
    f32 frameTime;
    f32 totalTime;
    u32 totalPoses;
    u32 totalBones;
    u32 poseIndex;
};

struct FontMap {
    Texture2D bitmap;
    f32 yOffsets[128];
    f32 widths[128];
    f32 heights[128];
    f32 bitmapXs[128];
    f32 bitmapYs[128];
    f32 bitmapWs[128];
    f32 bitmapHs[128];
    u16 characterCodes[128];
    f32 spaceCharacterWidth;
    f32 spacing;
    u32 totalCharacters;
    u32 missingCharIndex;
    u32 bmw;
    u32 bmh;
};

struct Gamepad {
    bool buttons[16];
    bool locks[16];
    f32 leftStickX;
    f32 leftStickY;
    f32 rightStickX;
    f32 rightStickY;
    f32 leftTrigger;
    f32 rightTrigger;
};

struct Camera {
    Matrix4 projection;
    Matrix4 view;
    Quaternion orientation;
    Vector3 position;
    Vector3 forward;
    Vector3 up;
    Vector3 right;
    Vector2 rotateSpeed;
    f32 moveSpeed;
    f32 mouseSensitivity;

    Camera(){
        view = Matrix4(1);
        position = Vector3(0);
        forward = Vector3(0, 0, 1);
        up = Vector3(0, 1, 0);
        right = Vector3(1, 0, 0);
    }

    void updateCameraView(){
        view = Matrix4(1);
        Matrix4 camRotation = quaternionToMatrix4(orientation);
        translateMatrix(&view, -position);
        view = camRotation * view;
        right = Vector3(view.m2[0][0], view.m2[1][0], view.m2[2][0]);
        up = Vector3(view.m2[0][1], view.m2[1][1], view.m2[2][1]);
        forward = Vector3(-view.m2[0][2], -view.m2[1][2], -view.m2[2][2]);
        view = projection * view;
    }

    void lookAt(Vector3 pos, Vector3 target, Vector3 tup = Vector3(0, 1, 0)){
        position = pos;
        forward = normalOf(target - pos);
        right = normalOf(cross(forward, normalOf(tup)));
        up = normalOf(cross(right, forward));
        view.m2[0][0] = right.x;
        view.m2[0][1] = up.x;
        view.m2[0][2] = -forward.x;
        view.m2[0][3] = 0;
        view.m2[1][0] = right.y;
        view.m2[1][1] = up.y;
        view.m2[1][2] = -forward.y;
        view.m2[1][3] = 0;
        view.m2[2][0] = right.z;
        view.m2[2][1] = up.z;
        view.m2[2][2] = -forward.z;
        view.m2[2][3] = 0;
        view.m2[3][0] = dot(-right, pos);
        view.m2[3][1] = dot(-up, pos);
        view.m2[3][2] = dot(forward, pos);
        view.m2[3][3] = 1;
        orientation = matrix4ToQuaternion(&view);
        view = projection * view;
    }
};

struct OSInterface {
    s8 executablePath[256];
    bool keyInputs[128];
    bool keyLocks[128];
    bool mouseInputs[4];
    bool mouseLocks[4];
    s8 keyTypedBuffer[16];
    Gamepad gamepad1;

    Vector4 clearColor;
    Vector2 mousePosition;

    Camera* camera3D;

    u8* shortTermBuffer;
    u8* longTermBuffer;

    f32* model3DVertexBufferPointer;
    u16* model3DIndexBufferPointer;

    u32 longTermBufferOffset;

    bool (*readFileIntoBuffer)(const s8* fileName, void* data, u32* fileLength);
    bool (*writeToFile)(const s8* fileName, void* data, u32 dataSize);
    u32 (*bindTexture2D)(Texture2D* texture);
    u32 (*bindTextureCube)(TextureCube* cube);
    u64 (*getSystemTime)();
    void (*debugBreak)();

    void (*initializeWorkQueue)(WorkQueue*, u32);
    void (*completeWorkQueueEntries)(WorkQueue*);
    void (*addWorkQueueEntry)(WorkQueue*, void(*)(void*), void*);

    void (*playAudioBuffer)(AudioEmitter* ae, void* buffer, u32 bufferSize);
    void (*setCameraPosition2D)(Vector2 cameraPosition);
    void (*setMasterVolume)(f32 volume);
    void (*changeAudioEmitterFrequencyRatio)(AudioEmitter* ae, f32 rat);
    void (*setAudioEmitterChannelVolumes)(AudioEmitter* ae, f32 left, f32 right);
    void (*setAudioEmitterVolume)(AudioEmitter* ae, f32 volume);

    void (*setFileHandlePointer)(FileHandle* handle, u32 amount);
    void (*readFromFileHandle)(FileHandle* handle, u8* buffer, u32 amount);
    void (*closeFileHandle)(FileHandle* handle);

    void (*debugSkeleton)(Skeleton* skeleton);
    void (*debugBlock)(Vector3 position, Vector3 scale, Quaternion orientation, Vector4 color);
    void (*debugLine)(Vector3 start, Vector3 end, Vector4 color, f32 lineSize);
    void (*debugPlane)(Vector3 p1, Vector3 p2, Vector3 p3, Vector4 color);
    void (*debugBox)(Vector3 position, Vector3 scale, Vector4 color, f32 lineSize);
    void (*debugSphere)(Vector3 position, Vector3 scale, Vector4 color);
    void (*renderModel3D)(Model3D* model, Vector3 lightPosition);
    void (*renderModel3DInstances)(Model3D* model, Matrix4* modelMatrices, u32 totalInstances, Vector3 lightPosition);
    void (*renderAnimesh)(Animesh* mesh, Skeleton* skeleton, Vector3 lightPosition);
    void (*showCursor)(bool s);
    void (*setCursorPosition)(f32 x, f32 y);
    void (*renderQuad)(Vector4 bounds, Vector4 color);
    void (*renderTexture2D)(Texture2D* t, Vector4 bounds);
    void (*renderText)(s8* text, f32 x, f32 y, f32 scale, Vector4 color);
    void (*renderSkybox)(TextureCube* skybox, Camera* camera);
    void (*setFont)(FontMap* font);
    void (*shadowMapCameraLookAt)(Vector3 position, Vector3 target, Vector3 up);
    void (*set3DCamera)(Camera* camera);

    FileHandle (*getFileHandleForReading)(s8* fileName);
    FileHandle (*getFileHandleForWriting)(s8* fileName, bool overwrite);
    AudioEmitter (*createAudioEmitter)(u32 numberOfChannels, u32 sampleRate, u32 byteRate, u32 blockAlign, u32 bitsPerSample);
    Texture2D (*createTexture2D)(void* data, u32 width, u32 height, u32 format);
    Texture2D (*createTexture2DFromFile)(s8* fileName);
    Model3D (*createModel3D)(f32* vData, u32 vDataSize, u16* iData, u32 iDataSize);
    Model3D (*createModel3DFromFile)(s8* fileName);
    Animesh (*createAnimesh)(f32* vData, u32 vDataSize, u16* iData, u32 iDataSize);
    Animesh (*createAnimeshFromFile)(s8* file);
    TextureCube (*createTextureCube)(void* data, u32 width, u32 height, u32 format);
    TextureCube (*createTextureCubeFromFile)(s8* fileName);
    Skeleton (*loadSkeletonFromFile)(s8* file);
    Pose (*loadPose)(Vector3 postion, Quaternion* orientations, u32 totalBones);
    Pose (*loadPoseFromFile)(s8* file);
    Animation (*loadAnimationFromFile)(s8* file);

    f32 deltaTime;
    f32 elapsedTime;
    u32 totalCores;
    u32 totalKeysTyped;
    u32 windowWidth;
    u32 windowHeight;
    u32 displayWidth;
    u32 displayHeight;
    s32 mouseScrollDelta;
    bool fullScreen;
    bool vsync;
    bool running;

    u8 KEY_0;
    u8 KEY_1;
    u8 KEY_2;
    u8 KEY_3;
    u8 KEY_4;
    u8 KEY_5;
    u8 KEY_6;
    u8 KEY_7;
    u8 KEY_8;
    u8 KEY_9;
    u8 KEY_A;
    u8 KEY_B;
    u8 KEY_C;
    u8 KEY_D;
    u8 KEY_E;
    u8 KEY_F;
    u8 KEY_G;
    u8 KEY_H;
    u8 KEY_I;
    u8 KEY_J;
    u8 KEY_K;
    u8 KEY_L;
    u8 KEY_M;
    u8 KEY_N;
    u8 KEY_O;
    u8 KEY_P;
    u8 KEY_Q;
    u8 KEY_R;
    u8 KEY_S;
    u8 KEY_T;
    u8 KEY_U;
    u8 KEY_V;
    u8 KEY_W;
    u8 KEY_X;
    u8 KEY_Y;
    u8 KEY_Z;
    u8 KEY_F1;
    u8 KEY_F2;
    u8 KEY_F3;
    u8 KEY_F4;
    u8 KEY_F5;
    u8 KEY_F6;
    u8 KEY_F7;
    u8 KEY_F8;
    u8 KEY_F9;
    u8 KEY_F10;
    u8 KEY_F11;
    u8 KEY_F12;
    u8 KEY_LEFT_SHIFT;
    u8 KEY_RIGHT_SHIFT;
    u8 KEY_LEFT_CTRL;
    u8 KEY_RIGHT_CTRL;
    u8 KEY_UP;
    u8 KEY_DOWN;
    u8 KEY_LEFT;
    u8 KEY_RIGHT;
    u8 KEY_SPACE;
    u8 KEY_ENTER;
    u8 KEY_ESCAPE;
    u8 KEY_SEMI_COLON;
    u8 GAMEPAD_A;
    u8 GAMEPAD_B;
    u8 GAMEPAD_X;
    u8 GAMEPAD_Y;
    u8 GAMEPAD_LB;
    u8 GAMEPAD_RB;
    u8 GAMEPAD_L3;
    u8 GAMEPAD_R3;
    u8 GAMEPAD_D_UP;
    u8 GAMEPAD_D_DOWN;
    u8 GAMEPAD_D_LEFT;
    u8 GAMEPAD_D_RIGHT;
    u8 GAMEPAD_START;
    u8 GAMEPAD_BACK;

    u32 TEXTURE_FORMAT_R8;
    u32 TEXTURE_FORMAT_RG8;
    u32 TEXTURE_FORMAT_RGBA8;
    u32 TEXTURE_FORMAT_R32F;
    u32 TEXTURE_FORMAT_BC1;
    u32 TEXTURE_FORMAT_BC4S;
    u32 TEXTURE_FORMAT_BC4U;
    u32 TEXTURE_FORMAT_BC5;
};