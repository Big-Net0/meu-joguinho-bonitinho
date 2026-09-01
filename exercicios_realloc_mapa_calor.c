#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600
#define TAM_CELULA 40

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float raio;
    Color cor;
} Bola;

int **criarMatriz(int linhas, int colunas) {
    int **matriz = (int **)malloc(linhas * sizeof(int *));

    if (matriz == NULL)
        return NULL;

    for (int i = 0; i < linhas; i++) {
        matriz[i] = (int *)malloc(colunas * sizeof(int));

        if (matriz[i] == NULL) {
            for (int k = 0; k < i; k++)
                free(matriz[k]);

            free(matriz);
            return NULL;
        }

        for (int j = 0; j < colunas; j++)
            matriz[i][j] = 0;
    }

    return matriz;
}

void liberarMatriz(int **matriz, int linhas) {
    if (matriz == NULL)
        return;

    for (int i = 0; i < linhas; i++)
        free(matriz[i]);

    free(matriz);
}

void desenharMatriz(int **matriz, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            Color cor;

            if (matriz[i][j] == 1)
                cor = (Color){255, 100, 100, 255};
            else
                cor = (Color){15, 30, 55, 255};

            DrawRectangle(
                j * TAM_CELULA,
                i * TAM_CELULA,
                TAM_CELULA - 2,
                TAM_CELULA - 2,
                cor
            );
        }
    }
}

void configurarBola(Bola *b) {
    b->pos = (Vector2){
        GetRandomValue(50, LARGURA_JANELA - 50),
        GetRandomValue(50, ALTURA_JANELA - 50)
    };

    b->vel = (Vector2){
        (float)GetRandomValue(-4, 4),
        (float)GetRandomValue(-4, 4)
    };

    if (b->vel.x == 0 && b->vel.y == 0) {
        b->vel.x = 2;
        b->vel.y = 2;
    }

    b->raio = (float)GetRandomValue(10, 25);

    b->cor = (Color){
        GetRandomValue(100, 255),
        GetRandomValue(100, 255),
        GetRandomValue(100, 255),
        255
    };
}

Bola *criarBolas(int quantidade) {
    Bola *bolas = (Bola *)malloc(quantidade * sizeof(Bola));

    if (bolas == NULL)
        return NULL;

    for (int i = 0; i < quantidade; i++)
        configurarBola(&bolas[i]);

    return bolas;
}

void atualizarBola(Bola *b) {
    b->pos.x += b->vel.x;
    b->pos.y += b->vel.y;

    if (b->pos.x - b->raio < 0) {
        b->pos.x = b->raio;
        b->vel.x *= -1;
    }

    if (b->pos.x + b->raio > LARGURA_JANELA) {
        b->pos.x = LARGURA_JANELA - b->raio;
        b->vel.x *= -1;
    }

    if (b->pos.y - b->raio < 0) {
        b->pos.y = b->raio;
        b->vel.y *= -1;
    }

    if (b->pos.y + b->raio > ALTURA_JANELA) {
        b->pos.y = ALTURA_JANELA - b->raio;
        b->vel.y *= -1;
    }
}

void verificarCelula(
    Bola *b,
    int **grade,
    int linhas,
    int colunas,
    int *celulasVisitadas
) {
    int coluna = (int)(b->pos.x / TAM_CELULA);
    int linha = (int)(b->pos.y / TAM_CELULA);

    if (linha >= 0 && linha < linhas &&
        coluna >= 0 && coluna < colunas) {

        if (grade[linha][coluna] == 0) {
            grade[linha][coluna] = 1;
            (*celulasVisitadas)++;
        }
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(
        LARGURA_JANELA,
        ALTURA_JANELA,
        "Exercicios - realloc e mapa de calor"
    );

    SetTargetFPS(60);

    int linhas = ALTURA_JANELA / TAM_CELULA;
    int colunas = LARGURA_JANELA / TAM_CELULA;

    int **grade = criarMatriz(linhas, colunas);

    if (grade == NULL) {
        CloseWindow();
        return 1;
    }

    int quantidadeBolas = 12;

    Bola *bolas = criarBolas(quantidadeBolas);

    if (bolas == NULL) {
        liberarMatriz(grade, linhas);
        CloseWindow();
        return 1;
    }

    int celulasVisitadas = 0;

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_SPACE)) {
            Bola *novoVetor = (Bola *)realloc(
                bolas,
                (quantidadeBolas + 1) * sizeof(Bola)
            );

            if (novoVetor != NULL) {
                bolas = novoVetor;
                configurarBola(&bolas[quantidadeBolas]);
                quantidadeBolas++;
            }
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (quantidadeBolas > 0) {
                quantidadeBolas--;

                Bola *novoVetor = (Bola *)realloc(
                    bolas,
                    quantidadeBolas * sizeof(Bola)
                );

                if (quantidadeBolas == 0) {
                    bolas = NULL;
                }
                else if (novoVetor != NULL) {
                    bolas = novoVetor;
                }
            }
        }

        for (int i = 0; i < quantidadeBolas; i++) {
            atualizarBola(bolas + i);

            verificarCelula(
                bolas + i,
                grade,
                linhas,
                colunas,
                &celulasVisitadas
            );
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        desenharMatriz(
            grade,
            linhas,
            colunas
        );

        for (int i = 0; i < quantidadeBolas; i++) {
            DrawCircleV(
                bolas[i].pos,
                bolas[i].raio,
                bolas[i].cor
            );
        }

        DrawText(
            "ESPACO = adicionar bola | BACKSPACE = remover bola",
            10,
            10,
            18,
            WHITE
        );

        DrawText(
            TextFormat(
                "Quantidade de bolas: %d",
                quantidadeBolas
            ),
            10,
            35,
            18,
            WHITE
        );

        DrawText(
            TextFormat(
                "Celulas visitadas: %d",
                celulasVisitadas
            ),
            10,
            60,
            18,
            WHITE
        );

        DrawText(
            "ESC = sair",
            10,
            ALTURA_JANELA - 25,
            16,
            WHITE
        );

        EndDrawing();
    }

    free(bolas);
    liberarMatriz(grade, linhas);

    CloseWindow();

    return 0;
}
