#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string>
#include <time.h>

#define IDENX 80
#define IDENY -140

typedef struct
{
    char estacion[25];
    int visitado;
    int x;
    int y;
    int color;
    int e;
}TDatos;

typedef struct nodoa
{
    struct nodoa *sig;
    struct nodo *origen;
    struct nodo *destino;
}ARISTA;

typedef struct nodo
{
    TDatos info;
    struct nodo *sig;
    struct nodo *anterior;
    ARISTA *arista;
}VERTICE;

typedef struct
{
    int tam;
    int ini;
    int fin;
    VERTICE **Cola;
}TCola;

typedef struct
{
    VERTICE *Cabeza;
    TCola Camino;
}TCabeza;


int creaRutas(VERTICE **Cab);
int insertaFinal(VERTICE **Cab,TDatos info);
VERTICE *creaVertice(TDatos dato);
void dibujaRutas(VERTICE *Cab);
int creaAristas(VERTICE **Cab);
int insercionArista(VERTICE **Cab,char ori[],char dest[]);
VERTICE *buscaVertice(VERTICE *Vr,char V[]);
ARISTA *creaArista(VERTICE *origen,VERTICE *destino);
VERTICE *buscaNombreVertice(VERTICE *Cab);
void SeleccionaCamino(VERTICE *Cab,TCola *Camino);
int iniciaCabeza(TCabeza *Cab);
int calculaCamino(VERTICE *origen,VERTICE *actual,VERTICE *destino,TCola *Camino);
int enCola(TCola *Camino,VERTICE *dir);
int Colallena(TCola C);
int ExtraerCola(TCola *C);
int ColaVacia(TCola C);
void dibujaCamino(VERTICE *Cab,TCola C,void *metro);
void reiniciaCola(TCola *C,VERTICE *Cab);
void guardaimagen(void **metro,int *imagetam);

int main()
{
    int v=0;
    void *metro;
    int imagentam;

    TCabeza Cab;
    VERTICE *Cabeza=NULL;
    initwindow(700,800);
    setbkcolor(RGB(250,250,250));
    setfillstyle(11,15);
    guardaimagen(&metro,&imagentam);
    iniciaCabeza(&Cab);
    creaRutas(&Cab.Cabeza);
    creaAristas(&Cab.Cabeza);
    setcolor(BLACK);
    setactivepage(1);

    while(1)
    {
        setactivepage(v);
        cleardevice();
        dibujaRutas(Cab.Cabeza);

        delay(16);

        setvisualpage(v);
        SeleccionaCamino(Cab.Cabeza,&Cab.Camino);
        dibujaCamino(Cab.Cabeza,Cab.Camino,metro);
        reiniciaCola(&Cab.Camino,Cab.Cabeza);
        v=!v;
    }
}

void guardaimagen(void **metro,int *imagentam)
{
    readimagefile("metro.jpg",0,0,31,31);
    *imagentam=imagesize(0,0,31,31);
    *metro=malloc(*imagentam);
    getimage(0,0,31,31,*metro);
}

void reiniciaCola(TCola *C,VERTICE *Cab)
{
    int i;
    VERTICE *aux;
    for(aux=Cab;aux;aux=aux->sig)
    {
        aux->info.e=aux->info.visitado=0;
    }
    C->ini=C->fin=0;
}

void dibujaCamino(VERTICE *Cab,TCola C,void *metro)
{
    int i,v=0,y=0;
    setactivepage(1);

    for(i=C.ini;i<C.fin;i++)
    {
            setactivepage(v);
            cleardevice();
            if(textwidth(C.Cola[i]->info.estacion)>textwidth("Estacion actual: "))
                rectangle((700-IDENX*2)-5,295,(700-IDENX*2)+5+textwidth(C.Cola[i]->info.estacion),300+(textheight(C.Cola[i]->info.estacion)*2)+5);
                else rectangle((700-IDENX*2)-5,295,(700-IDENX*2)+5+textwidth("Estacion actual: "),300+(textheight(C.Cola[i]->info.estacion)*2)+5);

            outtextxy(700-IDENX*2,300,"Estacion actual: ");
            outtextxy(700-IDENX*2,300+textheight("Estacion actual: "),C.Cola[i]->info.estacion);
            dibujaRutas(Cab);
            setfillstyle(9,RED);
            floodfill(C.Cola[i]->info.x+IDENX,C.Cola[i]->info.y-(IDENY),BLACK);
            putimage((C.Cola[i]->info.x+IDENX)-16,(C.Cola[i]->info.y-(IDENY))-32,metro,3);
            setvisualpage(v);
            v=!v;
            delay(500);
    }
}

int iniciaCabeza(TCabeza *Cab)
{
    int b=0;
    Cab->Cabeza=NULL;

    Cab->Camino.Cola=(VERTICE**)malloc(sizeof(VERTICE*)*32);
    if(Cab->Camino.Cola)
    {
        Cab->Camino.ini=Cab->Camino.fin=0;
        Cab->Camino.tam=32;
        b++;
    }
    return(b);
}

void SeleccionaCamino(VERTICE *Cab,TCola *Camino)
{
    VERTICE *origen=NULL, *destino=NULL;
    int i,c=0;

    while(!origen && !destino)
    {
        setfillstyle(1,1);
        origen=buscaNombreVertice(Cab);
        floodfill(origen->info.x+IDENX,origen->info.y-IDENY,BLACK);
        outtextxy((700-700/2)+textwidth("Da click en un vertice origen: "),800-800/5,origen->info.estacion);

        setfillstyle(1,2);
        outtextxy(700-700/2,(800-800/5)+textheight("Origen seleccionado: "),"Da click en un vertice destino: ");
        destino=buscaNombreVertice(Cab);
        floodfill(destino->info.x+IDENX,destino->info.y-IDENY,BLACK);
        outtextxy(700-700/2,(800-800/5)+textheight("Origen seleccionado: "),"Destino seleccionado: ");
        outtextxy((700-700/2)+textwidth("Destino seleccionado: "),(800-800/5)+textheight("Origen seleccionado: "),destino->info.estacion);
        delay(600);

    }

    calculaCamino(origen,origen,destino,Camino);
    enCola(Camino,destino);
    for(i=0;i<Camino->fin;i++)
    {
        if(Camino->Cola[i]==origen)
            c++;
    }
    if(c==2)
    {
        c=0;
        for(i=Camino->ini;c!=2;i++)
        {
            ExtraerCola(Camino);
            if(Camino->Cola[i]==origen)
                c++;
        }
    }
}

int calculaCamino(VERTICE *origen,VERTICE *actual,VERTICE *destino,TCola *Camino)
{
    ARISTA *aux;
    enCola(Camino,actual);
    actual->info.visitado=1;
    if(origen->sig)
    {
        if(actual==origen && origen->info.e==0)
            actual->info.visitado=0;
    }
    actual->info.e=1;
    int band=0;

    for(aux=actual->arista;aux;aux=aux->sig)
    {
        if(aux->destino==destino || band==1)
        {
            return(1);
        }

        if(aux->destino->info.visitado!=1)
            band=calculaCamino(origen,aux->destino,destino,Camino);
    }
}

int enCola(TCola *Camino,VERTICE *dir)
{
    if(Colallena(*Camino)==1)
        return(0);
    Camino->Cola[Camino->fin]=dir;
    Camino->fin++;
    if(Camino->fin==Camino->tam)
    {
        Camino->fin=0;
    }
    return(1);
}

int Colallena(TCola C)
{
    if((C.fin+1)%C.tam==C.ini)
        return(1);
    return(0);
}

int ExtraerCola(TCola *C)
{
    if(ColaVacia(*C)==1)
        return(0);
    C->ini=(C->ini+1)%C->tam;
    return(1);
}

int ColaVacia(TCola C)
{
    if(C.ini==C.fin)
        return(1);
    return(0);
}

VERTICE *buscaNombreVertice(VERTICE *Cab)
{
    VERTICE *aux, *vertice=NULL;
    int ratonx=-1,ratony=-1;

    while(vertice==NULL)
    {
        outtextxy(700-700/2,800-800/5,"Da click en un vertice origen: ");
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            getmouseclick(WM_LBUTTONDOWN,ratonx,ratony);
        }
        for(aux=Cab;aux && !vertice && (ratonx>-1 && ratony>-1);aux=aux->sig)
        {
            if((ratonx>((aux->info.x+IDENX)-10) && ratony>((aux->info.y-IDENY)-10)) && (ratonx<((aux->info.x+IDENX)+10) && ratony<((aux->info.y-IDENY)+10)))
                vertice=aux;

        }
    }

    return(vertice);

}

void dibujaRutas(VERTICE *Cab)
{
    VERTICE *aux;
    for(aux=Cab;aux;aux=aux->sig)
    {
        setcolor(aux->info.color);
        setlinestyle(1,1,15);

        if(aux->anterior)
            line(aux->anterior->info.x+IDENX,aux->anterior->info.y-IDENY,aux->info.x+IDENX,aux->info.y-IDENY);
    }

    setlinestyle(0,0,2);
    setcolor(BLACK);

    for(aux=Cab;aux;aux=aux->sig)
    {
        if(aux->info.color==2963683 && strcmp(aux->info.estacion,"El Rosario")!=0)
        {
            settextstyle(8,VERT_DIR,1);
            outtextxy(aux->info.x+IDENX-8,aux->info.y-IDENY-14,aux->info.estacion);
        }else
        {
            settextstyle(11,0,1);
            outtextxy((aux->info.x+IDENX)-(textwidth(aux->info.estacion)+10),aux->info.y-(IDENY+8),aux->info.estacion);
        }
        setfillstyle(SOLID_FILL,WHITE);
        circle(aux->info.x+IDENX,aux->info.y-IDENY,10);
        floodfill(aux->info.x+IDENX,aux->info.y-IDENY,BLACK);
    }
}

int creaAristas(VERTICE **Cab)
{
    FILE *a;
    a=fopen("Aristas.txt","r");
    char origen[25];
    char destino[25];
    int band;

    while(!feof(a))
    {
        fgets(origen,25,a);
        fgets(destino,25,a);
        //printf("origen: %s destino: %s",origen,destino);
        band=insercionArista(Cab,origen,destino);
        //printf("band: %d\n",band);
    }
}

int insercionArista(VERTICE **Cab,char ori[],char dest[])
{
    int band=0;ARISTA *nuevo;
    VERTICE *origen,*destino;

    origen=buscaVertice(*Cab,ori);
    destino=buscaVertice(*Cab,dest);
    if(origen && destino)
    {
        nuevo=creaArista(origen,destino);
        if(nuevo)
        {
            nuevo->sig=origen->arista;
            origen->arista=nuevo;
        }
        band++;
    }
    return(band);
}

VERTICE *buscaVertice(VERTICE *Vr,char V[])
{
    VERTICE *aux;
    for(aux=Vr;aux && strcmp(aux->info.estacion,V)!=0;aux=aux->sig);
    return(aux);
}

ARISTA *creaArista(VERTICE *origen,VERTICE *destino)
{
    ARISTA *nuevo;
    nuevo=(ARISTA*)malloc(sizeof(ARISTA));
    if(nuevo)
    {
        nuevo->origen=origen;
        nuevo->destino=destino;
        nuevo->sig=NULL;
    }
    return(nuevo);
}

int creaRutas(VERTICE **Cab)
{
    FILE *a;
    a=fopen("Vertices.txt","r");
    TDatos datos;

    while(!feof(a))
    {
        fgets(datos.estacion,25,a);
        fscanf(a,"%d",&datos.x);
        fscanf(a,"%d",&datos.y);
        fscanf(a,"%d",&datos.color);

        insertaFinal(Cab,datos);
    }
}

int insertaFinal(VERTICE **Cab,TDatos info)
{
    int ret=0;
    VERTICE *nuevo,*aux;

    nuevo=creaVertice(info);
    if(nuevo)
    {
        if(*Cab==NULL)
        {
            *Cab=nuevo;
        }else
        {
            for(aux=*Cab;aux->sig;aux=aux->sig);
            aux->sig=nuevo;
            nuevo->anterior=aux;
        }
        ret++;
    }
    return ret;
}

VERTICE *creaVertice(TDatos dato)
{
    VERTICE *nuevo;
    nuevo=(VERTICE *)malloc(sizeof(VERTICE));

    if(nuevo)
    {
        strcpy(nuevo->info.estacion,dato.estacion);
        nuevo->info.x=dato.x;
        nuevo->info.y=dato.y;
        nuevo->info.color=dato.color;
        nuevo->info.visitado=nuevo->info.e=0;
        nuevo->sig=NULL;
        nuevo->anterior=NULL;
        nuevo->arista=NULL;
    }

    return(nuevo);
}
