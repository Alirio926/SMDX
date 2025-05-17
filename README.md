# SDMX - Sega Mega Drive eXtended
SDMX - Sega Mega Drive eXtended é uma tentativa de acelerar e facilitar o desenvolvimento de jogos para o Mega Drive (Genesis) utilizando o SGDK (Sega Genesis Development Kit). 

A proposta é oferecer métodos de acesso rápido para criação de objetos comuns em jogos, com uma estrutura completa e, ao mesmo tempo, flexível.

Criei 3 ferramentas em java para exportar do tiled, não sendo necessário ajuste apos a extração:

Array de slops

Array de colisão

Array de objetos, se na definição do objeto forem definidos todos os itens necessários para um path_follower, o tools irá linkar o objeto com a path definida no Tiled, também irá concatenar objetos com os mesmos itens(variaveis) criando arrays. 

Segue exemplo:

```c
#include "fase1_obj.h"

const Vect2D_u16 path_34[] = {
    { 109, 354 },
    { 234, 354 },
};

const Vect2D_u16 path_43[] = {
    { 416, 551 },
    { 528, 551 },
    { 528, 487 },
};

// concatenou os itens por terem as mesmas propriedades definidas.
const Item_t Items[] = {
    { .id = 4, .nome = "Espada", .property = 4, .type = 0 },
    { .id = 4, .nome = "Espada2", .property = 4, .type = 0 },
};

// concatenou as plataformas por terem as mesmas propriedades definidas.
// Cria em forma de path_follower, colocando as propriedades no path_followr_def, a demais se houver, tambem serão incluidas no item externo.
const platform_t platforms[] = {
    { .agentDef = { .position = { 416, 551 }, .path = path_43, .path_len = 3, .delayInicial = 60, .path_mode = PATHMODE_LOOP, .velocidade = FIX16(1), .sprite = "spr_platform", .w = 80, .h = 16 } },
    { .agentDef = { .position = { 235, 310 }, .path = path_34, .path_len = 2, .delayInicial = 60, .path_mode = PATHMODE_PINGPONG, .velocidade = FIX16(1), .sprite = "spr_platform", .w = 80, .h = 16 } },
};

```

É necessário apenas passar o array para a função de criação, com o index do array que deseja instanciar.
### Importante:

Ainda estou trabalhando em desenvolver as funções, por isso a obj_factory não possui muitos metodos, tendo que recorrer as proprias entidades para criação.

```c
// Ao usar path_follower, pode usa calculos Euclides e manhattan, 
// só use euclides de fizer movimentos em diagonais, essa função não esta 
// não esta otimizada e contem bugs, embora funcione.
// para movimentos retos, use manhattan que é leve e funciona bem.
spawn_entity_platform(0, DIST_EUCLIDEAN);
```

## ⚠️ Estado atual
Este projeto ainda está em desenvolvimento, podendo conter bugs e comportamentos inesperados. A estrutura e as funcionalidades estão em constante evolução, e várias partes do código ainda estão sendo aprimoradas ou reestruturadas.

## 💡 Colabore!
Estou aberto a ideias, sugestões e melhorias contínuas! Sinta-se à vontade para abrir issues, enviar pull requests ou simplesmente compartilhar seu feedback. Toda contribuição é bem-vinda para tornar este engine mais robusto, versátil e acessível.

## Objetivos principais
Acelerar a criação de jogos no Mega Drive com o SGDK.

Facilitar a criação de objetos recorrentes (NPCs, inimigos, plataformas, itens, etc.).

Manter a engine modular e adaptável a diferentes estilos de jogo.

Reutilizar componentes e dados definidos em ferramentas como o Tiled

## Visão Geral

O módulo `gamestate_manager` é responsável por coordenar os estados principais do jogo (GameStates). Cada estado encapsula uma fase lógica do jogo, como menus, gameplay ou cenas especiais.

A transição entre estados é feita de forma explícita, através de um `enum GameState`, e cada estado possui quatro funções específicas associadas a ele:

* `init()`          → chamada ao entrar no estado
* `update()`        → chamada a cada frame
* `handlerInput()`  → chamada via SGDK (JOY handler)
* `unloadState()`   → chamada ao sair do estado

Essas funções são atribuídas por meio de uma tabela fixa (`stateTable[]`) indexada diretamente pelo enum.

---

## Arquivo: gamestate\_manager.h

### Tipos definidos:

```c
typedef enum {
    INTRO = 0,
    TITLE,
    DEMO,
    MENU,
    LEVEL1,
    LEVEL1A
} GameState;
```

### API principal:

```c
void setState(GameState state);    // troca de estado
void updateGameState(void);        // executa o update do estado atual
```

---

## Arquivo: gamestate\_manager.c

### Estrutura principal:

```c
typedef struct {
    func init;
    func update;
    joyHandle handlerInput;
    func unloadState;
} StateFuncs;

static const StateFuncs stateTable[] = {
    [INTRO] = { GameInit, GameUpdate, GameHandlerInput, GameUnloadState },
    [LEVEL1] = { level1_init, level1_update, level1_input, level1_unload },
    // ...
};
```

### Lógica de transição:

```c
void setState(GameState state) {
    if (CurrentGameState.unloadState)
        CurrentGameState.unloadState();

    CurrentGameState = stateTable[state];

    if (CurrentGameState.init)
        CurrentGameState.init();

    JOY_setEventHandler(CurrentGameState.handlerInput);

    gameState = state;
}
```

### Execução contínua no loop principal:

```c
void updateGameState(void) {
    if (CurrentGameState.update)
        CurrentGameState.update();
}
```

---

## Ciclo de Vida de um Estado

1. `setState(...)` é chamado com um `GameState` do enum
2. Se houver estado atual, executa `unloadState()`
3. Atualiza os ponteiros com `stateTable[state]`
4. Executa `init()` do novo estado
5. Define o handler de controle com `JOY_setEventHandler(...)`
6. A cada frame, `updateGameState()` chama `update()` do estado atual

---

## Como adicionar um novo GameState

1. Adicione no enum `GameState`:

```c
FASE2,
```

2. Crie as 4 funções necessárias:

```c
void fase2_init(void);
void fase2_update(void);
void fase2_input(u16 joy, u16 changed, u16 state);
void fase2_unload(void);
```

3. Adicione na `stateTable[]`:

```c
[FASE2] = { fase2_init, fase2_update, fase2_input, fase2_unload },
```

4. Faça a transição com:

```c
setState(FASE2);
```

---

## Observações

* O sistema é simples, direto e baseado em enum.
* Não há alocação dinâmica de estados.
* `JOY_setEventHandler()` é automaticamente atualizado para cada estado.
* Cada estado é opcionalmente composto por até 4 funções, podendo usar `NULL` para handlers desnecessários.

---

## Parte 2 — Estado de Gameplay

O estado de gameplay representa o "coração" da execução do jogo em tempo real. Nele, o engine executa todas as rotinas que envolvem lógica, física, input, renderização e interação entre entidades.

---

### 🧭 Ciclo principal

A função `GameUpdate()` (ou `faseX_update()`) contém a seguinte sequência típica:

```c
void GameUpdate(void) {
    if (g_gameState == GAME_DIALOGUE) {
        if (dialogue_updateText(activeDialogue)) {
            if (joy_pressed_start_or_button()) {
                dialogue_boxCloseCenter(...);
                dialogue_restoreBackground(activeDialogue);
                activeDialogue->active = FALSE;
                g_gameState = GAME_RUNNING;
            }
        }
        entity_drawAll();
        SPR_update();
        SYS_doVBlankProcess();
        return;
    }

    update_all_entities();        // lógica e timers por entidade
    physics_updateAll();         // movimento e colisão
    camera_update();             // reposicionamento da câmera
    entity_drawAll();            // animações e renderização
    SPR_update();                // atualização dos sprites SGDK
    SYS_doVBlankProcess();       // sincronização com o VBlank
}
```

---

### 🔄 Atualização de entidades (`update_all_entities`)

* Chama `e->onUpdate(e)` para cada entidade ativa
* Verifica políticas de atualização com `should_update()`
* Entidades sem `RigidBody` também são consideradas (ex: itens, NPCs estáticos)

#### Entrada e interação:

* Entidades com `joyHandle` definem input por controle (ex: o jogador)
* `onInteract()` só é chamado por um sistema externo (ex: botão A pressionado + colisão com trigger/NPC)

---

### ⚙️ Física (`physics_updateAll`)

* Aplica aceleração, velocidade e colisão via `RigidBody`
* Corrige posição com base nas colisões com o mundo e outras entidades

---

### 🎥 Câmera (`camera_update`)

* Segue o alvo (geralmente o player)
* Pode usar zona morta (deadzone), limites da fase, e efeitos de parallax

---

### 🧠 Diálogos e pausa

Quando um diálogo está ativo (`g_gameState == GAME_DIALOGUE`):

* Input e física são pausados
* Somente `dialogue_updateText()` e `entity_drawAll()` são executados
* O jogo retoma o ciclo normal quando o jogador fecha o diálogo

---

### ⏱ Eventos temporizados

Eventos disparados com atraso:

* Usam `eventTimer_schedule(delay, callback)`
* São verificados no update geral com `eventTimer_updateAll()`

---

## Parte 3 — Sistema de Entidades

O sistema de entidades é responsável por representar todos os objetos ativos no jogo: jogador, inimigos, itens, NPCs, triggers, entre outros. Cada entidade pode ter comportamento próprio, interagir com o mundo, receber input ou simplesmente exibir algo na tela.

---

### 🧱 Estrutura da entidade

As entidades são definidas por meio da struct `Entity`, que contém:

* Identificador de tipo (`tipo`) como PLAYER, ENEMY, ITEM, etc.
* Posição, geralmente via ponteiro para um `RigidBody`
* Sprite e animação (`AnimController`)
* Flags (ex: `FLAG_INTERACTABLE`, `FLAG_SOLID`)
* Ponteiros para funções específicas:

  * `onUpdate(Entity*)`
  * `onInteract(Entity*)`
  * `joyHandle(u16 joy, u16 changed, u16 state)`
  * `onDraw(Entity*)`

---

### 🔄 Atualização automática

Durante o `GameUpdate()`, a função `update_all_entities()` percorre todas as entidades ativas. Se `onUpdate` estiver definido e a política de lógica (`logicPolicy`) permitir, a função será chamada:

```c
if (e->onUpdate)
    e->onUpdate(e);
```

Entidades com `RigidBody` participam da física. Entidades sem `RigidBody` (como itens, NPCs simples) funcionam com base na posição fixa (`hitbox` via `pData`).

---

### 🎮 Input

Se a entidade possuir `joyHandle`, ela é registrada como manipuladora de input quando ativa:

```c
JOY_setEventHandler(e->joyHandle);
```

Isso é usado, por exemplo, para o jogador controlar um personagem.

---

### 🤝 Interação

O sistema externo (ex: ao pressionar o botão A) pode detectar colisões com entidades com `FLAG_INTERACTABLE`. Se houver `onInteract`, ela é chamada:

```c
if (aabb_intersect(playerAABB, npcAABB)) {
    e->onInteract(e);
}
```

---

### ✨ Animação e renderização

O campo `e->anim` contém um `AnimController`, que atualiza o sprite automaticamente. A renderização final ocorre em `entity_drawAll()`.

---

### 📦 Dados adicionais

O campo `pData` permite associar dados extras à entidade, como:

* `ItemDef*` para itens
* `TriggerDef*` para triggers
* `NpcSimpleDef*` para NPCs

Esses dados são utilizados dentro das funções como `onUpdate`, `onInteract` etc.

---

## Parte 4 — Interações e Triggers

O sistema de interações do engine permite que o jogador (ou outras entidades) acionem comportamentos ao entrar em contato com objetos do mundo. Esses objetos podem ser NPCs, itens, alavancas, zonas de evento ou qualquer entidade com lógica reativa.

---

### 🧩 Flags e checagem de interação

Para que uma entidade seja considerada interagível, ela precisa ter:

* A flag `FLAG_INTERACTABLE`
* Um `onInteract(Entity*)` definido

Durante o gameplay, o sistema de input (por exemplo, no handler de botão A) executa:

```c
tryInteract(&player) ;

// e no interact.c
// ...
if (e->flags & FLAG_INTERACTABLE)
    if (aabb_intersect(playerAABB, entityAABB))
        e->onInteract(e);
```

Essa checagem é feita sobre entidades próximas, considerando seu AABB (área de colisão) ou hitbox (se não tiver `RigidBody`).

---

### 🎯 Sistema de Triggers

Triggers são zonas invisíveis com comportamento específico quando o jogador entra nelas. Cada trigger é uma entidade com:

* Tipo `ENTITY_TYPE_TRIGGER`
* Campo `pData` apontando para um `TriggerDef*`

Exemplo de `TriggerDef`:

```c
typedef struct {
    Box hitbox;
    TriggerType type;
    bool active;
    bool triggered;
    TriggerCallback onEnter;
    TriggerCallback onExit;
    void* context;
    TriggerZoneAction zoneAction;
} TriggerDef;
```

#### Execução:

* Quando o jogador entra no `hitbox` da trigger:

  * A callback é agendada via `eventTimer_schedule(...)`
  * Pode haver delay, repetição ou execução única

---

### 🕹 NPCs e Diálogos

NPCs simples usam `onInteract` para iniciar um diálogo:

```c
void npc_simple_onInteract(Entity* e) {
    NpcSimpleDef* def = e->pData;
    dialogue_beginText(def->dlg, ...);
    g_gameState = GAME_DIALOGUE;
    activeDialogue = def->dlg;
}
```

Esse padrão permite que triggers, NPCs, placas e eventos usem o mesmo mecanismo de chamada — `onInteract`, com contexto.

---

### 💡 Boas práticas

* Verifique sempre `FLAG_INTERACTABLE` antes de chamar `onInteract`
* Padronize os dados em `pData` com structs claras
* Use `eventTimer_schedule(...)` para atrasar ou encadear ações
* Evite triggers com callbacks longas ou pesadas diretamente

---

## Parte 5 — Sistema de Diálogos

O sistema de diálogos permite exibir falas com efeito de rolagem de texto, caixa animada e controle de fluxo. Ele pode ser usado para interações com NPCs, mensagens de tutoriais, eventos narrativos ou gatilhos contextuais.

---

### 🧱 Estrutura principal

Cada diálogo usa uma instância de `DialogueState`, que contém:

* Fonte e paleta
* Texto a ser exibido
* Área da caixa (x, y, w, h)
* Estado atual: ativo, finalizado, índice de caractere
* Controle de animação de caixa (cornerToggle, updateBoxCtr)
* Backup do fundo (tiles antes da caixa)

Exemplo:

```c
typedef struct {
    u16 fontBaseTile, fontPalette;
    u16 boxBaseTile;
    u16 txtX, txtY, visW, visH;
    const char* src;
    u16 curCol, curRow, frameCnt, i;
    bool finished, active;
    u16 boxX, boxY, boxW, boxH;
    u8 cornerToggle, updateBoxCtr;
    u16* bgBackup;
    bool hasBackup;
} DialogueState;
```

---

### 🗨 Iniciando um diálogo

Para começar um diálogo, chama-se:

```c
// global dialog
DialogueState activeDialogue;

// ...
dialogue_init(&activeDialogue, PAL1, VDPTilesFilled);
dialogue_setFont(&activeDialogue, &custom_font, PAL1);    

NpcSimpleDef tia = {
    .hitbox = { 450, 560, 32, 64 },
    .texts = { "Oi, querido!", "Aproveite sua jornada.", "Passe em casa depois." },
    .textCount = 3,
    .textMode = NPC_TEXTMODE_STOP_LAST
};

npc_createSimple(&tia);
```

Essa função:

* Armazena a área da caixa
* Salva os tiles do fundo
* Limpa a área de texto
* Desenha a caixa com efeito animado
* Marca o diálogo como ativo

---

### 🔄 Atualização

Todo frame, no estado `GAME_DIALOGUE`, deve chamar:

```c
if (dialogue_updateText(dlg)) {
    // texto completo
    if (joy_pressed_button()) {
        dialogue_boxCloseCenter(...);
        dialogue_restoreBackground(dlg);
        dlg->active = FALSE;
        g_gameState = GAME_RUNNING;
    }
}
```
As opções de texto são:
```c
/*
Modo	    Comportamento
LOOP	    Repete ciclicamente as falas
STOP_LAST	Para na última, e repete sempre ela
ADVANCE	    Avança e nunca mais repete (ex: tutorial, cutscene)
*/
typedef enum {
    NPC_TEXTMODE_LOOP,      // volta ao início quando termina
    NPC_TEXTMODE_STOP_LAST, // para na última fala
    NPC_TEXTMODE_ADVANCE    // avança para a próxima, e depois... nada
} NpcTextMode;
```
O `updateText()` imprime 1 caractere por chamada e pisca os cantos da caixa periodicamente.

---

### 🎨 Visual e animação

A caixa é desenhada em tiles com canto, borda e centro, usando `dialogue_boxOpenCenter()` e `dialogue_boxToggleCorners()` para efeito visual.

O texto usa tiles de fonte ASCII 8x8 e pode ser trocado com:

```c
dialogue_setFont(&activeDialogue, &custom_font, PAL1);   
```

---

### 💬 Diálogos encadeados

O sistema suporta múltiplas falas por NPC usando `textIndex`:

```c
const char* frases[] = {"Oi!", "Boa sorte!", "Volte sempre."};

def->texts = frases;
def->textCount = 3;
def->textIndex = 0;
```

Cada chamada de `onInteract` pode iniciar a próxima fala com rotação, parada ou avanço único (ver Parte 4).

---

### 📌 Considerações

* Caixa pode ser desenhada em qualquer lugar da tela
* O fundo é restaurado após fechamento
* Input do jogador é ignorado enquanto `g_gameState == GAME_DIALOGUE`
* O sistema pode ser usado para texto narrativo, balões, placas e mais

---

## Parte 6 — Sistema de Física e Colisão

O sistema de física é responsável por movimentar entidades, aplicar aceleração e velocidade, resolver colisões com o mundo e outras entidades, além de fornecer informações como contato com o chão (grounded) ou suporte sob os pés.

---

### ⚙️ Estrutura `RigidBody`

Entidades com física possuem um ponteiro `body` para um `RigidBody`, que representa seu corpo físico:

```c
typedef struct {
    Vect2D_s16 position;       // posição local
    Vect2D_s16 globalPosition; // posição absoluta
    AABB aabb;                 // caixa de colisão
    Vect2D_u16 centerOffset;  // centro do corpo
    Vect2D_fix16 velocity;    // velocidade (fix16)
    Vect2D_fix16 delta;       // deslocamento acumulado
    bool grounded;            // está tocando o chão?
    Entity* owner;            // entidade dona
    // ... outros campos: layer, mask, flags
} RigidBody;
```

Cada `RigidBody` é criado com:

```c
rigidbody_create(updateMode, layer, mask, tag);
```

E precisa ser associado a uma `Entity` via `body->owner = e`.

---

### 🔁 Atualização física

No `GameUpdate()`, a função `physics_updateAll()` percorre todos os corpos ativos:

1. Aplica aceleração e velocidade
2. Move a posição com base em `velocity * deltaTime`
3. Verifica colisão com tiles sólidos
4. Corrige a posição se necessário (empurra para fora)
5. Atualiza flags como `grounded`

O campo `delta` serve para armazenar o movimento real aplicado, o que pode ser usado por plataformas e sincronização.

Vale dizer que todo corpo tem `EntityPhysicsParams` com os parametros padrão, esse struct pode ser acessado e modificado individualmente em cada corpo. Em outras palavras, cada corpo tem a sua propria fisica.

---

### 🧱 Colisão com o mundo

🧹 Integração com o Tiled

Os dados de colisão são gerados automaticamente a partir de mapas criados no Tiled usando uma ferramenta auxiliar escrita em Java: o java_tools. Essa ferramenta:

Lê o arquivo .tmx exportado pelo Tiled.

Extrai a camada de colisão.

Constrói um array linear representando os tiles sólidos e vazios.

Opcionalmente, comprime os dados com RLE (Run-Length Encoding).

Gera automaticamente uma estrutura CollisionArray pronta para ser incluída no jogo.

Exemplo abaixo é para um mapa de (60 * 16) x (42 * 16) = 960 x 672
```c
#pragma once
#include <genesis.h>
#include "tiled_map.h"

const CollisionArray fase1_col = {
    .data = (const u8[]){
        190, 1, 180, 0, 133, 1, 182, 0, 132, 1, 183, 0, 131, 1, 183, 0, 131, 1, 183, 0, 131, 1, 183, 0, 131, 1, 177, 0, 129, 1, 131, 0, 131, 1, 177, 0, 129, 1, 131, 0, 131, 1, 161, 0, 145, 1, 131, 0, 131, 1, 134, 0, 129, 1, 133, 0, 164, 1, 131, 0, 131, 1, 129, 2, 132, 0, 129, 1, 129, 0, 168, 1, 131, 0, 131, 1, 134, 0, 172, 1, 131, 0, 131, 1, 134, 0, 172, 1, 131, 0, 131, 1, 134, 0, 172, 1, 131, 0, 131, 1, 132, 0, 134, 1, 171, 0, 131, 1, 132, 0, 132, 1, 173, 0, 131, 1, 183, 0, 131, 1, 181, 0, 129, 2, 133, 1, 181, 0, 133, 1, 180, 0, 136, 1, 178, 0, 136, 1, 179, 0, 135, 1, 139, 0, 133, 1, 143, 0, 153, 1, 138, 0, 134, 1, 143, 0, 171, 1, 143, 0, 171, 1, 130, 2, 140, 0, 171, 1, 143, 0, 165, 1, 150, 0, 161, 1, 171, 0, 141, 1, 173, 0, 137, 1, 177, 0, 132, 1, 182, 0, 131, 1, 183, 0, 130, 1, 184, 0, 130, 1, 184, 0, 130, 1, 169, 0, 3, 145, 1, 167, 0, 147, 1, 145, 0, 3, 167, 1, 145, 0, 129, 1, 143, 0, 171, 1, 143, 0, 171, 1, 143, 0, 149, 1
    },
    .width = 60,
    .height = 42,
    .compressed = TRUE
};

```

A colisão é baseada em tiles do mapa, com lógica como:

* `tile_isSolid(tile)`
* `tile_isSlope(tile)`

A posição do corpo é usada para calcular as bordas e consultar a camada de colisão (geralmente uma layer de inteiros).

Funções auxiliares importantes:

* `getTileBounds(...)`
* `aabb_intersect(...)`
* `posToTile(...)`

---

### 🧍‍♂️ Suporte e chão

Ao detectar colisão no eixo Y inferior, o corpo pode armazenar um ponteiro para o objeto que está sendo pisado:

```c
body->grounded = TRUE;
body->support = outraEntidadeOuNULL;
```

Isso é fundamental para herdar movimento de plataformas ou impedir pulo no ar.
 ### A colisão com Slops

É tratada usando uma linha no Tiled que será exportada, na verificação da fisica é verificado se esta visivel(dentro da camera) e neste caso, é feito a verificação de colisão com a rampa para correção do eixo y, 
```c
#define SLOPE_COUNT 3
const Slope slopes[SLOPE_COUNT] = {
    { 247, 639, 328, 607, 0 },
    { 211, 398, 280, 366, 0 },
    { 630, 607, 704, 576, 0 }
};
```

---

### 🔄 Integração com Entidade

Durante o update da entidade:

* O campo `body->delta` pode ser usado para saber quanto a entidade se moveu
* `body->grounded` pode ativar pulo ou transições de animação
* `body->velocity` pode ser modificado diretamente pelo jogador ou IA

---

### 🛠 Componentes auxiliares

* `physics_debugDraw()` pode desenhar AABBs ou detectar problemas
* `physics_resetBody()` limpa todos os campos físicos para reutilização

---

## Parte 7 — Sistema de Câmera

O sistema de câmera é responsável por seguir um alvo (geralmente o jogador), limitar a visão dentro dos limites da fase, aplicar zonas de amortecimento (deadzone) e efeitos como parallax.

---

### 🧠 Estrutura interna

A câmera usa uma estrutura com:

* Posição atual (em pixels)
* Alvo (`RigidBody*`) a ser seguido
* Limites da fase (`Rect` ou `AABB`)
* Zona morta (deadzone), que define um retângulo interno onde o alvo pode se mover sem mover a câmera
* Offsets e ajustes para suavizar o movimento

```c
typedef struct {
    Vect2D_s16 position;      // posição da câmera
    RigidBody* target;        // corpo sendo seguido
    AABB bounds;              // limites da fase
    AABB deadzone;            // zona de amortecimento
} CameraState;
```

---

### 🎯 Seguindo o alvo

No `camera_update()`, a câmera verifica a posição do alvo em relação à `deadzone`:

* Se o alvo ultrapassar a borda da zona, a câmera se move
* Se o alvo estiver dentro da zona, a câmera permanece parada

Isso evita jitter e movimento excessivo da tela.

---

### 🌍 Limites da fase

A câmera nunca ultrapassa os limites da fase:

```c
camera.position.x = clamp(camera.position.x, bounds.min.x, bounds.max.x - SCREEN_WIDTH);
camera.position.y = clamp(camera.position.y, bounds.min.y, bounds.max.y - SCREEN_HEIGHT);
```

Isso garante que áreas fora do mapa não sejam visíveis.

---

### 🌌 Parallax e camadas

A câmera pode controlar múltiplas camadas de background:

* `BG_A` (principal)
* `BG_B` (HUD ou paralaxe traseiro)

Com `VDP_setHorizontalScroll` e `VDP_setVerticalScroll`, o valor da posição da câmera pode ser multiplicado por um fator para criar efeito de parallax:

```c
VDP_setHorizontalScroll(BG_B, camera.position.x / 2);
```

---

### 🔁 Atualização

A função `camera_update()` deve ser chamada após `physics_updateAll()` e antes de `entity_drawAll()`:

```c
void GameUpdate(void) {
    update_all_entities();
    physics_updateAll();
    camera_update();
    entity_drawAll();
    ...
}
```

---

### 🎮 Modos de Câmera e Controle com o Jogador

A câmera pode operar em diferentes modos que afetam seu comportamento em relação ao jogador:

```c
typedef enum {
    CAMERA_MODE_DRAG_PLAYER,    // A câmera puxa o jogador quando ele atinge a borda da tela
    CAMERA_MODE_INDEPENDENT     // A câmera se move sozinha; o jogador pode sair do centro
} CameraMode;
```

* `CAMERA_MODE_DRAG_PLAYER`: o jogador é "empurrado" pela câmera ao atingir as bordas da tela. Ideal para jogos com rolagem lateral contínua.
* `CAMERA_MODE_INDEPENDENT`: o jogador se move livremente, e a câmera só o acompanha dentro da zona de amortecimento. Útil para exploração ou áreas abertas.

Você pode trocar o modo com:

```c
camera_setMode(CAM_MODE_FOLLOW_TARGET);
camera_setMode(CAM_MODE_AUTOSCROLL);
```

---

#### Modo automático

Com `CAM_MODE_AUTOSCROLL`, a câmera move sozinha a cada frame:

```c
camera_setAutoScroll(1, 0); // 1 pixel/frame horizontal, 0 vertical
```

Útil para fases de fuga, fases com scroll forçado ou sequências especiais. A posição da câmera ignora o alvo neste modo.

---

Durante cutscenes ou eventos especiais, a câmera pode ser desconectada do player, e pode serr conectada a qualquer entidade que tenha um rigidbody:

```c
camera_setTarget(NULL);
// ou
camera_setTarget(pf_platforma_body);
camera.position = ...;
```

Isso permite sequências com controle total da posição da câmera.

---

## Parte 8 — Sistema de Animações

O sistema de animações do engine é baseado no componente `AnimController`, que encapsula o controle de quadros de sprite para qualquer entidade visível.

Esse sistema permite alternar animações, controlar quadros manualmente ou automaticamente, e sincronizar lógica e visual com facilidade.

---

### 🎞 Estrutura `AnimController`

Cada entidade com sprite visível possui uma instância de `AnimController`, que contém:

* Ponteiro para o `Sprite` SGDK (`SPRITE*`)
* Conjunto de animações (`AnimationSet*`)
* Estado atual de animação (índice, quadro atual, timer)
* Flag de visibilidade

```c
typedef struct {
    Sprite* sprite;
    AnimationSet* animSet;
    u16 currentAnim;
    u16 currentFrame;
    u16 frameTimer;
    bool visible;
} AnimController;
```

---

### 🔁 Atualização automática

A cada frame, o método `animcontroller_update()` pode ser chamado para atualizar o quadro da animação com base no tempo:

```c
animcontroller_update(&e->anim);
```

Esse método:

* Verifica se a animação atual está ativa
* Avança o quadro com base no timer do frame
* Atualiza o sprite se necessário

---

### 🎬 Trocar animações

Você pode trocar a animação de uma entidade com:

```c
animcontroller_setAnim(&e->anim, ANIM_WALK);
```

Isso reinicia a animação, resetando o quadro e timer. Se já estiver na mesma animação, não faz nada.

---

### 📁 Estrutura `AnimationSet`

Define um conjunto de animações para uma entidade. Cada animação é uma sequência de quadros com duração:

```c
typedef struct {
    const Animation* anims;
    u16 count;
} AnimationSet;

typedef struct {
    const u16* frames;
    u16 length;
    u16 frameDuration;
    bool loop;
} Animation;
```

Os índices (ex: `ANIM_IDLE`, `ANIM_WALK`) são usados para selecionar a animação dentro do `AnimationSet`.

---

### 👁 Visibilidade e sincronização

* `anim.visible = FALSE` oculta o sprite
* O sprite sempre acompanha a posição do `RigidBody` se houver
* É possível forçar quadros com `animcontroller_setFrame(...)`

---

## Parte 9 — Sistema de Path\_Follower

O sistema `PathFollower` permite que entidades sigam caminhos pré-definidos usando pontos posicionais definidos em ferramentas como o Tiled. Esses caminhos são utilizados para plataformas móveis, NPCs em patrulha, inimigos com rota fixa ou efeitos visuais.

---

### 📌 Estrutura de caminho

Os caminhos são definidos como uma sequência de pontos:

```c
typedef struct {
    Vect2D_s16* points;     // lista de pontos
    u16 count;              // quantidade de pontos
    u8 mode;                // modo de repetição (loop, ping-pong, etc)
} PathDef;
```

---

### 🤖 Estrutura do agente

Cada entidade que segue um caminho possui um `PathFollower`, que mantém o estado da navegação:

```c
typedef struct {
    PathDef* path;
    u16 index;              // ponto atual
    fix16 speed;            // velocidade em fix16
    u16 waitFrames;         // espera entre pontos
    u8 direction;           // 1 = frente, 0 = trás (para ping-pong)
    Vect2D_f16 pos;         // posição interpolada
} PathFollower;
```

Esse componente pode ser atualizado por:

```c
pathfollower_update(PathFollower* f);
```

---

### 🔁 Modos de repetição

O campo `mode` do `PathDef` define como o caminho se comporta:

* `PATHMODE_LOOP`: reinicia do início após o último ponto
* `PATHMODE_PINGPONG`: vai até o fim e volta
* `PATHMODE_ONCE`: executa uma única vez

---

### ⚙️ Integração com entidades

A entidade pode usar a posição de `PathFollower` para atualizar seu `RigidBody`, sprite ou posição direta:

```c
e->body->position.x = fix16ToInt(pf->pos.x);
e->body->position.y = fix16ToInt(pf->pos.y);
```

Plataformas móveis normalmente atualizam `delta.x` com a diferença entre frames para mover o jogador junto.

---

### 🧩 Criação via Tiled

No Tiled:

* Desenhe um `polyline` para representar o caminho
* Use um objeto para indicar o agente e vincular via `path` (nome/id)
* O parser converte isso em `PathDef` e associa com o `PathFollower`

---

### 🧍‍♂️ PathFollower como entidade

Cada `PathFollower` é implementado como uma `Entity`, o que significa que:

* Ele pode ter um `RigidBody` associado, permitindo colisões ou transporte de outras entidades (como plataformas móveis).
* Ele pode ter um `AnimController`, exibindo animações visuais ao se mover (ex: drones, elevadores).
* Seu `onUpdate` é automaticamente chamado pelo sistema, integrando-se ao `update_all_entities()`.
* Pode conter `flags` como `FLAG_SOLID`, `FLAG_INTERACTABLE`, ou `FLAG_VISIBLE`, dependendo do comportamento esperado.

Isso permite que agentes de caminho se comportem como qualquer outra entidade do jogo, mantendo lógica, física e visual em sincronia. Além disso, por serem entidades reais, eles podem ser instanciados, ativados/desativados, e interagir com triggers ou outros sistemas.

---

### 💡 Considerações

* `speed` pode ser alterada dinamicamente
* `waitFrames` permite pausas entre pontos
* `pathfollower_reset()` pode reiniciar o movimento a qualquer momento

---





