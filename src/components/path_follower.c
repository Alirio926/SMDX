/**
 * @file path_follower.c
 * @brief Sistema de seguimento de caminho para entidades
 * 
 * Este arquivo implementa o sistema de PathFollower, que permite que entidades
 * sigam um caminho predefinido com diferentes modos de movimento:
 * - LOOP: Repete o caminho indefinidamente
 * - PINGPONG: Vai e volta no caminho
 * - ONESHOT: Segue o caminho uma única vez
 */

#include "path_follower.h"
#include "core/logger.h"
#include "core/game_config.h"
#include "entity.h"

bool pathFollower_distanceManhattan(PathFollower* pf, fix16 velocidade, Vect2D_f16* outDelta);
bool pathFollower_distanceEuclidean(PathFollower* pf, fix16 velPos, Vect2D_f16* out);

static u32 fastSqrt32(u32 v);

/**
 * @brief Avança para o próximo ponto no modo LOOP
 * @param pf PathFollower a ser atualizado
 */
static void path_step_loop(PathFollower* pf) {
    s8 next = (s8)pf->index + pf->direction;
    
    if (next < 0 || next >= pf->path_len) {
        pf->index = (pf->direction > 0) ? 0 : pf->path_len - 1;
    } else {
        pf->index = (u8)next;
    }
}

/**
 * @brief Avança para o próximo ponto no modo PINGPONG
 * @param pf PathFollower a ser atualizado
 */
static void path_step_pingpong(PathFollower* pf) {
    s8 next = (s8)pf->index + pf->direction;
    if (next < 0 || next >= pf->path_len) {
        pf->direction *= -1;
        pf->index += pf->direction;
    } else {
        pf->index = (u8)next;
    }
}

/**
 * @brief Avança para o próximo ponto no modo ONESHOT
 * @param pf PathFollower a ser atualizado
 */
static void path_step_oneshot(PathFollower* pf) {
    s8 next = (s8)pf->index + pf->direction;
    if (next >= 0 && next < pf->path_len) {
        pf->index = (u8)next;
    } else {
        pf->active = FALSE;
    }    
}

/**
 * @brief Cria um novo PathFollower
 * @param def Definição do PathFollower
 * @param distanceType Tipo de cálculo de distância a ser usado
 * @return PathFollower* novo PathFollower criado
 */
PathFollower* path_follower_create(const PathAgentDef* def, DistanceType distanceType) {
    PathFollower* pf = (PathFollower*)MEM_alloc(sizeof(PathFollower));
    pf->path = def->path;
    pf->path_len = def->path_len;
    pf->path_mode = def->path_mode;
    pf->velocidade = def->velocidade;
    pf->index = 0;
    pf->direction = 1;
    pf->delayInicial = def->delayInicial;
    pf->delay = def->delayInicial;
    pf->distanceFunc = distanceType == DIST_MANHATTAN ? pathFollower_distanceManhattan : pathFollower_distanceEuclidean;
    pf->active = true;
    pf->posX = INT_TO_POS(def->position.x);
    pf->posY = INT_TO_POS(def->position.y);
    pf->active = true;

    return pf;
}

/**
 * @brief Atualiza a posição da entidade baseada no PathFollower
 * @param entity Entidade a ser atualizada
 * 
 * Esta função:
 * 1. Calcula o próximo movimento usando a função de distância
 * 2. Atualiza a posição da entidade
 * 3. Atualiza a animação se necessário
 * 4. Chama o callback onEndPath quando apropriado
 */
void follower_update(Entity* entity) {
    PathFollower* pf = (PathFollower*)entity->pData;

    if(pf->active) {
        bool movedOrTicked = pf->distanceFunc(pf, pf->velocidade, &entity->body->delta);
        //bool movedOrTicked = pathFollower_distanceEuclidean(pf, entity->velocidade, &entity->body->delta);
        
        if(movedOrTicked) {
            entity->body->velocity.fixX = entity->body->delta.x;
            entity->body->velocity.fixY = entity->body->delta.y;
            entity->body->velocity.x = F16_toInt(entity->body->delta.x);

            if(entity->tipo != ENTITY_TYPE_PLATFORM) 
                animcontroller_update(&entity->anim, entity->body);
            
            if(entity->onEndPath || (pf->path_mode == PATHMODE_ONESHOT && pf->index >= pf->path_len)) {
                entity->onEndPath(entity);
            }
        }
    }
    
}

/**
 * @brief Obtém a função de cálculo de distância baseada no tipo
 * @param type Tipo de cálculo de distância
 * @return DistanceFunc função de cálculo de distância
 */
DistanceFunc getDistanceFunc(DistanceType type) {
    switch (type) {
        case DIST_MANHATTAN: return pathFollower_distanceManhattan;
        case DIST_EUCLIDEAN:
        default: return pathFollower_distanceEuclidean;
    }
}

/**
 * @brief Calcula o movimento usando distância Manhattan
 * @param pf PathFollower
 * @param velocidade Velocidade de movimento
 * @param outDelta Vetor de saída com o delta de movimento
 * @return true se houve movimento
 */
bool pathFollower_distanceManhattan(PathFollower* pf, fix16 velocidade, Vect2D_f16* outDelta) {
    if (!pf || (!pf->active && pf->delay == 0) || pf->path_len == 0){
        return false;
    }
    // Delay ativo? só decrementa
    if (pf->delay > 0) {
        pf->delay--;
        outDelta->x = 0;
        outDelta->y = 0;
        return true;
    }

    const Vect2D_u16* target = &pf->path[pf->index];
    s32 deltaX = INT_TO_POS(target->x) - pf->posX;
    s32 deltaY = INT_TO_POS(target->y) - pf->posY;

    // Se chegou próximo o suficiente do alvo
    u32 distt = getApproximatedDistance(deltaX, deltaY);
    if(distt <= 64/*abs(deltaX) <= 64 && abs(deltaY) <= 64*/){
        // Atualiza posição exata
        pf->posX = INT_TO_POS(target->x);
        pf->posY = INT_TO_POS(target->y);
        pf->delay = pf->delayInicial;
        
        // Atualiza próximo ponto do caminho
        switch(pf->path_mode){
            case PATHMODE_LOOP:     path_step_loop(pf); break;
            case PATHMODE_PINGPONG: path_step_pingpong(pf); break;
            case PATHMODE_ONESHOT:  path_step_oneshot(pf); break;
        }

        return false;
    }
    
    s32 dist = abs(deltaX) + abs(deltaY);
    
    if(dist > 0){
        s32 dirX = (deltaX > 0) ? 1 : ((deltaX < 0) ? -1 : 0);
        s32 dirY = (deltaY > 0) ? 1 : ((deltaY < 0) ? -1 : 0);

        fix32 ratioX = ((s32)abs(deltaX) << 16) / dist;
        fix32 ratioY = ((s32)abs(deltaY) << 16) / dist;
        
        fix16 scaledVel = F16_mul(velocidade, FIX16(64));

        fix16 dx = F16_mul(scaledVel, (dirX != 0) ? (fix16)((dirX * ratioX) >> 16) : 0);        
        fix16 dy = F16_mul(scaledVel, (dirY != 0) ? (fix16)((dirY * ratioY) >> 16) : 0);

        pf->posX += dx;
        pf->posY += dy;

        outDelta->x = dx;
        outDelta->y = dy;

        return true;
    }

    return false;
}

/**
 * @brief Calcula raiz quadrada inteira rapidamente
 * @param v Valor para calcular raiz quadrada
 * @return u32 raiz quadrada calculada
 */
static u32 fastSqrt32(u32 v)
{
    u32 r = 0, b = 1u << 30;
    while (b > v) b >>= 2;
    while (b) {
        if (v >= r + b) { v -= r + b; r = (r >> 1) + b; }
        else            { r >>= 1; }
        b >>= 2;
    }
    return r;
}

/**
 * @brief Calcula o movimento usando distância Euclidiana
 * @param pf PathFollower
 * @param velPos Velocidade de movimento
 * @param out Vetor de saída com o delta de movimento
 * @return true se houve movimento
 */
bool pathFollower_distanceEuclidean(PathFollower* pf, fix16 velPos, Vect2D_f16* out)
{
    if (!pf || (!pf->active && pf->delay == 0) || pf->path_len == 0) return false;
    if (pf->delay) { pf->delay--; out->x = out->y = 0; return true; }

    /* delta em píxel inteiro ---------------------------------- */
    const Vect2D_u16* tgt = &pf->path[pf->index];
    s32 dPixX = (INT_TO_POS(tgt->x) - pf->posX) >> FIX16_FRAC_BITS;
    s32 dPixY = (INT_TO_POS(tgt->y) - pf->posY) >> FIX16_FRAC_BITS;

    /* chegou? -------------------------------------------------- */
    if (abs(dPixX) <= 1 && abs(dPixY) <= 1)
    {
        pf->posX = INT_TO_POS(tgt->x);
        pf->posY = INT_TO_POS(tgt->y);
        pf->errX = pf->errY = 0;
        pf->delay = pf->delayInicial;
        switch (pf->path_mode) { case PATHMODE_LOOP: path_step_loop(pf); break;
                                 case PATHMODE_PINGPONG: path_step_pingpong(pf); break;
                                 case PATHMODE_ONESHOT: path_step_oneshot(pf); break; }
        out->x = out->y = 0;
        return true;
    }

    /* comprimento inteiro ------------------------------------- */
    u32 dist = fastSqrt32((u32)abs(dPixX)*abs(dPixX) + (u32)abs(dPixY)*abs(dPixY));

    /* passo desejado em px (≥1) --------------------------------*/
    u32 step = F16_toInt(velPos);
    if (step == 0) step = 1;
    if (step >= dist) step = dist;        /* evita overshoot     */

    /* erro acumulado (26.6) -- soma passo na escala global -----*/
    pf->errX += (s32)dPixX * step;
    pf->errY += (s32)dPixY * step;

    /* decide quantos px avançar em cada eixo -------------------*/
    s32 moveXpix = pf->errX / (s32)dist;
    s32 moveYpix = pf->errY / (s32)dist;
    pf->errX    -= moveXpix * (s32)dist;
    pf->errY    -= moveYpix * (s32)dist;

    /* converte para 26.6 e aplica ------------------------------*/
    s32 dxPos = moveXpix << FIX16_FRAC_BITS;   /* *64 */
    s32 dyPos = moveYpix << FIX16_FRAC_BITS;

    pf->posX += (fix16)dxPos;
    pf->posY += (fix16)dyPos;

    out->x = (fix16)dxPos;
    out->y = (fix16)dyPos;
    return true;
}

