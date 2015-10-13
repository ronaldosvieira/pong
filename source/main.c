//
// PONG! v1.0
// LABORATÓRIO DE COMPUTAÇÃO I
// ALUNOS: RONALDO, RONI E THAMIRES
//

#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define telaH 640
#define telaW 320

int colisao(SDL_Rect a, SDL_Rect b, double acX, double acY);
void tocarSom(struct Mix_Chunk *sound);
int gameOver(int p);
int sair();
int moverp1(int velocidade);
int moverp2(int velocidade);

int main( int argc, char* args[] ){

    while(jogar());

    return 0;
}

int jogar()
{
    srand(time(NULL));

    // DECLARAÇÃO DE VARIÁVEIS
    SDL_Surface *tela;

    SDL_Surface *bola;
    SDL_Rect    fonte,destino;

    SDL_Surface *pts1,*pts2,*ps;
    SDL_Rect    placar1,placar2,pausadoFonte;
    SDL_Rect    placar1Fonte,placar2Fonte,pausado;

    SDL_Rect    player1,player2;

    SDL_Surface *telaGameOver;

    Mix_Chunk   *score = NULL;
    Mix_Chunk   *touch = NULL;
    Mix_Chunk   *gameOverSound = NULL;

    SDL_Surface *icone;

    Uint8       *pausar;

    FILE* config = fopen("config.ini","r");

    int
    pontos1     = 0,
    pontos2     = 0,
    fim         = 0,
    quadros     = 0,
    paused      = 0,
    corTela1    = 0,
    corTela2    = 0,
    corTela3    = 0,
    antibug     = 0,
    velocidade  = 3,
    velMax      = 10,
    maxPontos   = 3;

    double
    acrescimoX = velocidade,
    acrescimoY = velocidade;

    player1.x = 20;
    player1.y = 110;
    player1.h = 100;
    player1.w = 15;

    player2.x = telaH-35;
    player2.y = 110;
    player2.h = 100;
    player2.w = 15;

    pausado.x = 221;
    pausado.y = 145;
    pausado.h = 30;
    pausado.w = 198;
    pausadoFonte   = pausado;
    pausadoFonte.x = 0;
    pausadoFonte.y = 0;

    placar1.x = 265;
    placar1.y = 20;
    placar1.h = 50;
    placar1.w = 30;
    placar1Fonte   = placar1;
    placar1Fonte.x = 0;
    placar1Fonte.y = 0;

    placar2.x = 345;
    placar2.y = 20;
    placar2.h = 50;
    placar2.w = 30;
    placar2Fonte   = placar2;
    placar2Fonte.x = 0;
    placar2Fonte.y = 0;

    fonte.x = 0;
    fonte.y = 0;
    fonte.w = 20;
    fonte.h = 20;

    destino   = fonte;
    destino.x = 310;
    destino.y = 150;
    //


    // CHECA EXISTÊNCIA DO ARQUIVO "config.ini"
    if(config == NULL){
        printf("Nao foi possivel carregar o arquivo 'config.ini'.");
    }
    //

    // LÊ CONFIGURAÇÕES
    fscanf(config,"%d %d %d %d %d %d",&velocidade,&velMax,&maxPontos,&corTela1,&corTela2,&corTela3);
    fclose(config);
    //

    // INICIALIZA COMPONENTES DA BIBLIOTECA SDL
    if (SDL_Init(SDL_INIT_EVERYTHING)!=0){
        printf("Erro: %s\n", SDL_GetError());
        exit(-1);
    }

    // INICIALIZA O COMPONENTE DE SOM
    if(Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 2048) != 0){
      fprintf(stderr, "Nao foi possivel inicializar o audio: %s\n", Mix_GetError());
      exit(1);
    }
    //

    // DEFINE O TÍTULO E ÍCONE DA JANELA
    SDL_WM_SetCaption("Pong! v1.0",NULL);
    SDL_WM_SetIcon(SDL_LoadBMP("img\\pong.bmp"),NULL);
    //

    // CRIA A TELA DE JOGO E CHECA POR ERROS
        tela = SDL_SetVideoMode(telaH, telaW, 16, SDL_SWSURFACE|SDL_DOUBLEBUF);

    if(!tela){
        printf("Erro: %s\n", SDL_GetError());
        exit(-1);
    }
    //

    // INICIALIZA OS SONS QUE SERÃO USADOS E CHECA POR ERROS
    score         = Mix_LoadWAV("snd\\score.wav");
    touch         = Mix_LoadWAV("snd\\touch.wav");
    gameOverSound = Mix_LoadWAV("snd\\gameover.wav");
    if((score == NULL)||(touch == NULL)||(gameOverSound == NULL)) {
        fprintf(stderr, "Impossível carregar arquivo WAV: %s\n", Mix_GetError());
        exit(1);
    }
    //

    // INICIALIZA AS IMAGENS QUE SERÃO USADAS E CHECA POR ERROS
    bola = SDL_LoadBMP("img\\bola.bmp");
    SDL_SetColorKey(bola, SDL_SRCCOLORKEY|SDL_RLEACCEL,(Uint16)SDL_MapRGB(bola->format,0,255,0));

    ps = SDL_LoadBMP("img\\paused.bmp");
    SDL_SetColorKey(ps, SDL_SRCCOLORKEY|SDL_RLEACCEL,(Uint16)SDL_MapRGB(ps->format,0,255,0));

    pts1 = SDL_LoadBMP("img\\pts.bmp");
    SDL_SetColorKey(pts1, SDL_SRCCOLORKEY|SDL_RLEACCEL,(Uint16)SDL_MapRGB(pts1->format,0,255,0));

    pts2 = SDL_LoadBMP("img\\pts.bmp");
    SDL_SetColorKey(pts2, SDL_SRCCOLORKEY|SDL_RLEACCEL,(Uint16)SDL_MapRGB(pts2->format,0,255,0));

    if(!pts2||!pts1||!bola||!ps){
        printf("Erro ao carregar bitmap!");
        exit(-1);
    }
    //

    // CHAMA A TELA INICIAL DO JOGO APÓS CARREGAR RECURSOS
    telaInicial();
    //

    // LOOP PRINCIPAL - ONDE OCORRERÃO AS COISAS MAIS IMPORTANTES
    while(!fim){
        fim = sair();

        // MONITORA TECLADO PELA TECLA P
        SDL_PumpEvents();
        pausar = SDL_GetKeyState(0);
        if(pausar[SDLK_p]){
            paused = !paused;
            SDL_Delay(150);
        }
        //

        // DESLOCAMENTO DA BOLA
        if(!paused){
            destino.x += acrescimoX;
            destino.y += acrescimoY;
        }
        //

        // PREENCHER RETANGULOS
        SDL_FillRect(tela,NULL,SDL_MapRGB(tela->format,corTela1,corTela2,corTela3));
        SDL_FillRect(tela,&player1,SDL_MapRGB(tela->format,255,255,255));
        SDL_FillRect(tela,&player2,SDL_MapRGB(tela->format,255,255,255));
        //

        // COLISÃO DA BOLA COM AS PAREDES/PALHETAS E AUMENTO DE VELOCIDADE

        if( (destino.y + destino.w + acrescimoY > telaW) || (destino.y + acrescimoY < 0) ){
                acrescimoY = -acrescimoY;
        }else{
            if( (colisao(player1,destino,acrescimoX,acrescimoX) || colisao(player2,destino,acrescimoX,acrescimoY)) && antibug>50 ){
                acrescimoX  = -acrescimoX;
                antibug     = 0;

                if((acrescimoX <= velMax)&&(acrescimoX >= -velMax)){
                    acrescimoX *= 1.1;
                }
                tocarSom(touch);
            }
        }
        //

        // CONTAGEM PONTUAÇÃO
        if(destino.x + destino.h + acrescimoX > telaH){
            if(pontos1<maxPontos-1){
                pontos1++;
                destino.x = 310; destino.y = 150;
                acrescimoX = velocidade; acrescimoY = velocidade;
                tocarSom(score);
                SDL_Delay(250);
            }else{
                tocarSom(gameOverSound);
                return gameOver(1);
            }
        }
        if(destino.x + acrescimoX < 0){
            if(pontos2<maxPontos-1){
                pontos2++;
                destino.x = 310; destino.y = 150;
                acrescimoX = -velocidade; acrescimoY = -velocidade;
                tocarSom(score);
                SDL_Delay(100);
            }else{
                tocarSom(gameOverSound);
                return gameOver(2);
            }
}

        placar1Fonte.x = pontos1 * placar1.w;
        placar2Fonte.x = pontos2 * placar2.w;

        //

        // CHECAR COLISÃO DAS PALHETAS COM AS PAREDES
        if(!paused){
            if(!((player1.y+moverp1(velocidade)<=0)||(player1.y+player1.h+moverp1(velocidade)>=telaW)))
                player1.y += moverp1(velocidade);
            if(!((player2.y+moverp2(velocidade)<=0)||(player2.y+player2.h+moverp2(velocidade)>=telaW)))
                player2.y += moverp2(velocidade);
        }
        //

        if (paused) SDL_BlitSurface(ps,&pausadoFonte,tela,&pausado);
        SDL_BlitSurface(bola,&fonte,tela,&destino);
        SDL_BlitSurface(pts1,&placar1Fonte,tela,&placar1);
        SDL_BlitSurface(pts1,&placar2Fonte,tela,&placar2);
        SDL_Flip(tela);
        SDL_UpdateRect(tela,0,0,0,0);
        SDL_Delay(10);
        quadros++;
        antibug++;
    }

    Mix_CloseAudio();
    SDL_Quit();
    return 0;
}
//

int colisao(SDL_Rect a, SDL_Rect b, double acX, double acY){
    // TESTA COLISÕES POR TODOS OS LADOS
    if(b.x + b.w + acX <= a.x) return 0;
    if(b.x + acX >= a.x + a.w) return 0;

    if(b.y + b.h + acY <= a.y) return 0;
    if(b.y + acY >= a.y + a.h) return 0;

    return 1;
    //
}

void tocarSom(struct Mix_Chunk *sound){
    // TOCA "SOUND" E TESTA SE NÃO HOUVE ERROS
    if (Mix_PlayChannel(-1, sound, 0) != 0) {
        fprintf(stderr, "Impossível reproduzir arquivo WAV: %s\n", Mix_GetError());
        exit(1);
    }
    //
}

int gameOver(int winner){
    // DECLARAÇÁO DE VARIÁVEIS
    SDL_Surface *tela = SDL_SetVideoMode(telaH, telaW, 16, SDL_SWSURFACE|SDL_DOUBLEBUF);
    SDL_Surface *telaGameOver;
    SDL_Rect telaGameOverPos;

    Uint8 *teclado;

    telaGameOverPos.x = 0;
    telaGameOverPos.y = 0;
    telaGameOverPos.w = 640;
    telaGameOverPos.h = 320;
    //

    // SE VENCEDOR FOR 1 (PLAYER 1), telaGamOver RECEBE IMAGEM 1. SE NÃO, telaGameOver RECEBE IMAGEM 2.
    if(winner==1)       telaGameOver = SDL_LoadBMP("img\\gameover1.bmp");
    else if(winner==2)  telaGameOver = SDL_LoadBMP("img\\gameover2.bmp");
    //

    // DESENHA A IMAGEM NA TELA
    SDL_SetColorKey(telaGameOver, SDL_SRCCOLORKEY|SDL_RLEACCEL,(Uint16)SDL_MapRGB(telaGameOver->format,0,255,0));
    SDL_BlitSurface(telaGameOver,NULL,tela,&telaGameOverPos);
    //

    // ATUALIZA A TELA
    SDL_Flip(tela);
    SDL_UpdateRect(tela,0,0,0,0);
    //

    // MONITORA TECLADO
    while(1){
        SDL_PumpEvents();
        teclado = SDL_GetKeyState(0);
        if(teclado[SDLK_SPACE]) return 1;   // Se "espaço" pressionado, retorna 1. (Jogar novamente)
        if(teclado[SDLK_ESCAPE]) return 0;  // Se "ESQ" pressionado, retorna 0. (Sair do jogo)
    }
    //

}

int telaInicial(){
    // DECLARAÇÁO DE VARIÁVEIS
    SDL_Surface *tela = SDL_SetVideoMode(telaH, telaW, 16, SDL_SWSURFACE|SDL_DOUBLEBUF);
    SDL_Surface *telaInicial = SDL_LoadBMP("img\\inicio.bmp");
    SDL_Surface *jogar = SDL_LoadBMP("img\\jogar.bmp");

    SDL_Rect telaInicialPos;
    SDL_Rect jogarPos, jogarDestino;

    Uint8 *teclado;

    telaInicialPos.x = 0;
    telaInicialPos.y = 0;
    telaInicialPos.w = 640;
    telaInicialPos.h = 320;

    jogarPos.x = 155;
    jogarPos.y = 170;
    jogarPos.w = 330;
    jogarPos.h = 26;

    jogarDestino = jogarPos;
    jogarDestino.x = 0;
    jogarDestino.y = 0;

    int quadros = 0;
    //

    // MONITORA TECLADO
    while(1){
        SDL_PumpEvents();
        teclado = SDL_GetKeyState(0);
        if(teclado[SDLK_SPACE]) return 1;   // Se "espaço" pressionado, retorna 1. (Iniciar jogo)
        if(teclado[SDLK_ESCAPE]) return 0;  // Se "ESQ" pressionado, retorna 0. (Sair do jogo)

        // DESENHA A IMAGEM NA TELA
        SDL_SetColorKey(telaInicial, SDL_SRCCOLORKEY|SDL_RLEACCEL,(Uint16)SDL_MapRGB(telaInicial->format,0,255,0));
        SDL_BlitSurface(telaInicial,NULL,tela,&telaInicialPos);
        SDL_SetColorKey(jogar, SDL_SRCCOLORKEY|SDL_RLEACCEL,(Uint16)SDL_MapRGB(jogar->format,0,255,0));
        SDL_BlitSurface(jogar,&jogarDestino,tela,&jogarPos);
        //

        jogarDestino.y = ((quadros/30)%7)*26;

        // ATUALIZA A TELA
        SDL_Flip(tela);
        SDL_UpdateRect(tela,0,0,0,0);
        quadros++;
        //

    }
    //

}

int sair(){
    // DECLARAÇÃO DE VARIÁVEIS
    Uint8 *teclado;
    //

    // MONITORA TECLADO
    SDL_PumpEvents();
    teclado = SDL_GetKeyState(0);   // Registra em "teclado" as teclas pressionadas no teclado
    return teclado[SDLK_ESCAPE];    // se tecla "ESQ" pressionada, retorna 1.
    //
}

int moverp1(int velocidade){
    // DECLARAÇÃO DE VARIÁVEIS
    Uint8 *teclado;
    //

    // MONITORA TECLADO
    SDL_PumpEvents();
    teclado = SDL_GetKeyState(0);   // Registra em "teclado" as teclas pressionadas no teclado

    if(teclado[SDLK_w])             // Se tecla "w" pressionada, retorna um número negativo
        return -velocidade;         // que diminuirá a coordenada y da palheta 1.
    else if(teclado[SDLK_s])        // Analogamente, se tecla "s" pressionada, a coordenada y
        return velocidade;          // da palheta 1 será aumentada.
    else return 0;                  // Se nenhuma dessas teclas for pressionada, a coordenada y
                                    // da palheta 1 permanecerá inalterada.
    //
}

int moverp2(int velocidade){
    // DECLARAÇÃO DE VARIÁVEIS
    Uint8 *teclado;
    //

    // MONITORA TECLADO
    SDL_PumpEvents();
    teclado = SDL_GetKeyState(0);   // Registra em "teclado" as teclas pressionadas no teclado

    if(teclado[SDLK_UP])            // Se tecla "para cima" pressionada, retorna um número negativo
        return -velocidade;         // que diminuirá a coordenada y da palheta 2.
    else if(teclado[SDLK_DOWN])     // Analogamente, se tecla "para baixo" pressionada, a coordenada y
        return velocidade;          // da palheta 2 será aumentada.
    else return 0;                  // Se nenhuma dessas teclas for pressionada, a coordenada y
                                    // da palheta 2 permanecerá inalterada.
    //
}

