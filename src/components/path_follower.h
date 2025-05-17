#ifndef PATH_FOLLOWER_H
#define PATH_FOLLOWER_H

#include "xtypes.h"
#include "components/rigidbody.h"
#include "components/path_def.h"



struct Entity; // Forward declaration
typedef struct PathFollower PathFollower;

typedef bool (*DistanceFunc)(PathFollower* pf, fix16 velocidade, Vect2D_f16* outDelta);

struct PathFollower{
    const Vect2D_u16* path;
    u8 path_len;
    u8 path_mode;      // LOOP, PINGPONG, ONESHOT
    u8 index;
    s8 direction;      // 1 ou -1
    u8 delay;
    u8 delayInicial;
    fix16 velocidade;          // usada se mover via path
    bool active;
    s32 posX, posY;    // posição atual (fix32)
    s32 lastPosX, lastPosY;
    s32 errX, errY;      /* 26.6 – restos acumulados          */

    DistanceFunc distanceFunc;
     /* ---- otimização ------------------------------------------------ */
    /* dados válidos ENQUANTO não chegar ao alvo corrente */
    s16 dirX, dirY;        /* ±1 ou 0                                */
    u16 remainPix;         /* distância restante em píxeis inteiros  */
    fix16 ratioX;          /* |dx|/dist  em fix16 (0‥64)             */
    fix16 ratioY;          /* |dy|/dist  em fix16 (0‥64)             */
    u32 ratioX32, ratioY32;   /* razões 16.16 */
};

PathFollower* path_follower_create(const PathAgentDef* def, DistanceType distanceType);
void pathFollower_init();
void follower_update(struct Entity* entity);
void pf_beginSegment(PathFollower* pf);
DistanceFunc getDistanceFunc(DistanceType type);

#endif // PATH_FOLLOWER_H 