#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#define STEP 10

 typedef struct cvor {
    int id;
    double x, y, radius;
} cvor;

///@param n: broj cvorova u grafu.
typedef struct graf {
    int **matrica, n, nextID;
    cvor *cvorovi;
} graf;

typedef struct queue {
    int size, front, rear;
    int *array;
} queue;

/// Kreira prazan queue za cuvanje cvorova velicine size\n
/// Vraca pointer na queue
queue* create_queue(int size) {
    queue *myQueue = (queue*)malloc(sizeof(queue));

    myQueue->size = size;
    myQueue->front = -1;
    myQueue->rear = -1;
    myQueue->array = (int*)malloc(size*sizeof(int));

    return myQueue;
}

int queue_is_empty(queue *myQueue) {
    return myQueue->rear == -1;
}

int queue_is_full (queue *myQueue) {
    return myQueue->front == myQueue->rear && myQueue->front != -1;
}

void queue_insert(queue *myQueue, int element) {
    if (queue_is_full(myQueue)) {
        printf("GRESKA: Queue overflow");
        exit(0);
    }

    if (myQueue->front == -1) {
        myQueue->front = 0;
        myQueue->rear = 0;
    }

    myQueue->array[myQueue->front] = element;
    myQueue->front = (myQueue->front + 1) % myQueue->size;
}

int queue_remove(queue *myQueue) {
    if (queue_is_empty(myQueue)) {
        printf("GRESKA: Queue underflow");
        exit(0);
    }

    int element = myQueue->array[myQueue->rear];
    myQueue->rear = (myQueue->rear + 1) % myQueue->size;
    if (myQueue->rear == myQueue->front) {
        myQueue->rear = -1;
        myQueue->front = -1;
    }

    return element;
}

void delete_queue(queue *myQueue) {
    free(myQueue->array);
    free(myQueue);
}

/// Kreira prazan graf sa n cvorova\n Vraca pointer na graf\n
/// TODO: Dodaj provere da li je memorija stvarno alocirana
graf* kreiraj_graf(int n) {
    graf *mojGraf = (graf*)malloc(sizeof(graf));

    int **matrica = (int**)malloc(n*sizeof(int*));
    for (int i = 0; i < n; i++) {
        matrica[i] = (int*)calloc(n, sizeof(int));
    }

    cvor *cvorovi = (cvor*)calloc(n,sizeof(cvor));
    for (int i = 0; i < n; i++) {
        cvorovi[i].id = i;
    }

    mojGraf->matrica = matrica;
    mojGraf->cvorovi = cvorovi;
    mojGraf->n = n;
    mojGraf->nextID = n;
    return mojGraf;
}

/// Dodaje prazan cvor u graf, vraca pointer ka njemu
/// @param mojGraf: pointer na graf u koji dodajemo cvor
cvor* dodaj_cvor(graf *mojGraf) {
    mojGraf->matrica = realloc(mojGraf->matrica, (mojGraf->n + 1) * sizeof(int*));
    mojGraf->matrica[mojGraf->n] = NULL;

    for (int i = 0; i < mojGraf->n; i++) {
        mojGraf->matrica[i] = realloc(mojGraf->matrica[i], (mojGraf->n + 1) * sizeof(int));
        mojGraf->matrica[i][mojGraf->n] = 0;
    }

    mojGraf->matrica[mojGraf->n] = (int*)calloc(mojGraf->n + 1, sizeof(int));

    mojGraf->cvorovi = realloc(mojGraf->cvorovi, (mojGraf->n+1)*sizeof(cvor));
    mojGraf->cvorovi[mojGraf->n].id = mojGraf->nextID;
    mojGraf->nextID++; mojGraf->n++;

    return &mojGraf->cvorovi[mojGraf->n-1];
}

/// Uklanja cvor sa zadatim indeksom iz grafa
/// @param mojGraf: graf iz kog se uklanja cvor
/// @param cvorID: ID cvora koji se uklanja
void ukloni_cvor(graf *mojGraf, int cvorID) {
    int cvorIndex;
    for (int i = 0; i < mojGraf->n; i++) {
        if (mojGraf->cvorovi[i].id == cvorID) {
            cvorIndex = i;
            break;
        }
    }

    for (int i = cvorIndex; i < mojGraf->n-1; i++) {
        mojGraf->cvorovi[i] = mojGraf->cvorovi[i+1];
    }
    mojGraf->cvorovi = realloc(mojGraf->cvorovi, (mojGraf->n-1)*sizeof(cvor));

    free(mojGraf->matrica[cvorIndex]);
    for (int i = cvorIndex; i < mojGraf->n-1; i++) {
        mojGraf->matrica[i] = mojGraf->matrica[i+1];
    }
    mojGraf->matrica = realloc(mojGraf->matrica, (mojGraf->n-1)*sizeof(int*));

    for (int i = 0; i < mojGraf->n-1; i++) {
        for (int j = cvorIndex; j < mojGraf->n-1; j++) {
            mojGraf->matrica[i][j] = mojGraf->matrica[i][j+1];
        }
        mojGraf->matrica[i] = realloc(mojGraf->matrica[i], (mojGraf->n-1)*sizeof(int));
    }

    mojGraf->n--;
}

/// Dodaje usmerenu granu izmedju cvora 1 i cvora 2 grafa
/// @param mojGraf: graf u koji se dodaje grana
/// @param cvorID1: cvor od kog je grana usmerena
/// @param cvorID2: cvor na koji je grana usmerena
void dodaj_granu(graf *mojGraf, int cvorID1, int cvorID2) {
    int cvorIndex1 = -1, cvorIndex2 = -1;
    for (int i = 0; i < mojGraf->n; i++) {
        if (mojGraf->cvorovi[i].id == cvorID1) {
            cvorIndex1 = i;
            if (cvorIndex2 != -1) break;
        }
        if (mojGraf->cvorovi[i].id == cvorID2) {
            cvorIndex2 = i;
            if (cvorIndex1 != -1) break;
        }
    }

    mojGraf->matrica[cvorIndex1][cvorIndex2] = 1;
}

/// Uklanja usmerenu granu izmedju cvora 1 i cvora 2 grafa
/// @param mojGraf: graf iz kog se uklanja grana
/// @param cvorID1: cvor od kog je grana usmerena
/// @param cvorID2: cvor na koji je grana usmerena
void ukloni_granu(graf *mojGraf, int cvorID1, int cvorID2) {
    int cvorIndex1 = -1, cvorIndex2 = -1;
    for (int i = 0; i < mojGraf->n; i++) {
        if (mojGraf->cvorovi[i].id == cvorID1) {
            cvorIndex1 = i;
            if (cvorIndex2 != -1) break;
        } else if (mojGraf->cvorovi[i].id == cvorID2) {
            cvorIndex2 = i;
            if (cvorIndex1 != -1) break;
        }
    }

    mojGraf->matrica[cvorIndex1][cvorIndex2] = 0;
}

/// Funkcija ispisuje reprezentaciju grafa\n
/// Prvi queue i prva kolona sadrze ID-jeve cvorova
/// @param mojGraf: graf ciju reprezentaciju ispisujemo
void ispisi_graf(graf *mojGraf) {
    printf("   ");
    for (int i = 0; i < mojGraf->n; i++) {
        printf("%2d ", mojGraf->cvorovi[i].id);
    }
    printf("\n");

    for (int i = 0; i < mojGraf->n; i++) {
        printf("%2d ", mojGraf->cvorovi[i].id);
        for (int j = 0; j < mojGraf->n; j++) {
            printf("%2d ", mojGraf->matrica[i][j]);
        }
        printf("\n");
    }
}

/// Brise uneti graf iz memorije
/// @param mojGraf: graf za brisanje
void obrisi_graf(graf *mojGraf) {
    for (int i = 0; i < mojGraf->n; i++) {
        free(mojGraf->matrica[i]);
    }
    free(mojGraf->matrica);
    free(mojGraf->cvorovi);
    free(mojGraf);
}

/// Ispituje da li cvor P pripada radijusu detonacije cvora Q
/// @param Q: cvor koji se detonira
/// @param P: cvor koji se ispituje
int pripada_radijusu(cvor Q, cvor P) {
    return sqrt(pow(Q.x - P.x, 2) + pow(Q.y - P.y, 2)) <= Q.radius;
}

/// Ucitava jednu liniju teksta iz tekstualne datoteke, staje kada dodje do '\\n' ili EOF
/// @param doc: pointer na ulaznu datoteku
char* read_file_line(FILE *doc) {
    char *string = NULL, c;
    int i = 0;
    while ((c = (char)fgetc(doc))) {
        if (i % STEP == 0) {
            string = realloc(string, i + STEP);
        }

        if (c == '\n' || c == EOF) {
            string[i] = '\0';
            break;
        }

        string[i] = c;
        i++;
    }

    string = realloc(string, i+1);
    return string;
}

char** read_file(char *docName) {
    FILE *doc = fopen(docName, "r");
    if (doc == NULL) {
        printf("Trazena datoteka ne postoji!\n");
        return NULL;
    }

    char **strings = NULL;
    int n = 0;
    while (1) {
        char *string = read_file_line(doc);
        if (strcmp(string, "") == 0) {
            break;
        }

        strings = realloc(strings, (n+1)*sizeof(char*));
        strings[n++] = string;
    }

    if (fclose(doc)) {
        printf("Doslo je do greske prilikom zatvaranja datoteke!\n");
        exit(0);
    }

    return strings;
}

graf* ucitaj_podatke(char **strings) {
    int n;
    char c;
    if (sscanf(strings[0], "%d %c", &n, &c) != 1) {
        printf("Sadrzaj datoteke nije odgovarajuceg formata!\n");
        exit(0);
    }

    graf *mojGraf = kreiraj_graf(n);
    for (int i = 0; i < n; i++) {
        double x, y, radius;
        if (sscanf(strings[i+1], "%lf %lf %lf %c", &x, &y, &radius, &c) != 3) {
            printf("Sadrzaj datoteke nije odgovarajuceg formata!\n");
            exit(0);
        }

        mojGraf->cvorovi[i].x = x;
        mojGraf->cvorovi[i].y = y;
        mojGraf->cvorovi[i].radius = radius;
    }

    for (int i = 0; i < n; i++) {
        cvor Q = mojGraf->cvorovi[i];
        for (int j = 0; j < n; j++) {
            cvor P = mojGraf->cvorovi[j];
            if (pripada_radijusu(Q, P)) {
                dodaj_granu(mojGraf, Q.id, P.id);
            }
        }
    }

    return mojGraf;
}

int in_array(const int *array, int n, int x) {
    for (int i = 0; i < n; i++) {
        if (array[i] == x) return 1;
    }
    return 0;
}

/// Funkcija vraca pointer na dinamicki alociranu listu ID-jeva obidjenih cvorova. Obilazak se vrsi po sirini.
/// @param mojGraf: graf koji se obilazi
/// @param cvorID: ID cvora od kog polazi obilazak
int* obilazak_grafa(graf *mojGraf, int cvorID, int *cnt) {
    int cvorIndex = -1;
    for (int i = 0; i < mojGraf->n; i++) {
        if (cvorID == mojGraf->cvorovi[i].id) {
            cvorIndex = i;
            break;
        }
    }
    if (cvorIndex == -1) {
        printf("Cvor sa zadatim ID-jem ne postoji!\n");
        exit(0);
    }

    queue *mojRed = create_queue(mojGraf->n - 1);
    queue_insert(mojRed, cvorIndex);

    int *poseceni_cvorovi = malloc(10 * sizeof(int));
    poseceni_cvorovi[0] = cvorIndex;
    *cnt = 1;

    while (!queue_is_empty(mojRed)) {
        cvorIndex = queue_remove(mojRed);

        for (int i = 0; i < mojGraf->n; i++) {
            if (mojGraf->matrica[cvorIndex][i] == 1 && !in_array(poseceni_cvorovi, *cnt, i)) {

                if (*cnt % STEP == 0) {
                    poseceni_cvorovi = realloc(poseceni_cvorovi, (*cnt + STEP) * sizeof(int));
                }
                poseceni_cvorovi[*cnt] = i;
                (*cnt)++;

                queue_insert(mojRed, i);
            }
        }
    }
    delete_queue(mojRed);
    poseceni_cvorovi = realloc(poseceni_cvorovi, *cnt * sizeof(int));

    for (int i = 0; i < *cnt; i++) {
        poseceni_cvorovi[i] = mojGraf->cvorovi[poseceni_cvorovi[i]].id;
    }

    return poseceni_cvorovi;
}

void efikasnost_mine(graf *mojGraf, int cvorID) {
    int cnt;
    int *poseceni_cvorovi = obilazak_grafa(mojGraf, cvorID, &cnt);

    printf("Efikasnost zadate mine je: %d\n", cnt);
    for (int i = 0; i < cnt; i++) {
        printf("Detonirana mina: %d\n", poseceni_cvorovi[i]);
    }

    free(poseceni_cvorovi);
}

///Funkcija vraca ID najefikasnije mine
///Ukoliko vise mina ima istu efikasnost, vraca prvu minu sa tom efikasnoscu
/// @param mojGraf: graf koji se ispituje
int najefikasnija_mina(graf *mojGraf) {
    int max = 0, maxID, cnt;

    for (int i = 0; i < mojGraf->n; i++) {
        int *poseceni_cvorovi = obilazak_grafa(mojGraf, mojGraf->cvorovi[i].id, &cnt);
        free(poseceni_cvorovi);

        if (cnt > max) {
            max = cnt;
            maxID = mojGraf->cvorovi[i].id;
        }
    }

    return maxID;
}

void raketa(graf *mojGraf, double x, double y, double radius) {
    cvor *mojCvor = dodaj_cvor(mojGraf);
    mojCvor->x = x;
    mojCvor->y = y;
    mojCvor->radius = radius;

    for (int i = 0; i < mojGraf->n-1; i++) {
        if (pripada_radijusu(mojGraf->cvorovi[i], *mojCvor)) {
            dodaj_granu(mojGraf, mojGraf->cvorovi[i].id, mojCvor->id);
        }
    }

    for (int i = 0; i < mojGraf->n; i++) {
        if (pripada_radijusu(*mojCvor, mojGraf->cvorovi[i])) {
            dodaj_granu(mojGraf, mojCvor->id, mojGraf->cvorovi[i].id);
        }
    }

    int cnt;
    int *poseceni_cvorovi = obilazak_grafa(mojGraf, mojCvor->id, &cnt);

    printf("Raketa je eksplodirala i detonirala sledece mine...\n");
    for (int i = 1; i < cnt; i++) {
        printf("Detonirana mina: %d\n", poseceni_cvorovi[i]);
    }

    ukloni_cvor(mojGraf, mojCvor->id);
    mojGraf->nextID--;
}

int postoji_ID(graf *mojGraf, int minaID) {
    for (int i = 0; i < mojGraf->n; i++) {
        if (mojGraf->cvorovi[i].id == minaID) return 1;
    }
    return 0;
}

double min(double niz[], int size) {
    double minimum = DBL_MAX;
    for (int i = 0; i < size; i++) {
        if (niz[i] < minimum) {
            minimum = niz[i];
        }
    }
    return minimum;
}

double max(double niz[], int size) {
    double maximum = -DBL_MAX;
    for (int i = 0; i < size; i++) {
        if (niz[i] > maximum) {
            maximum = niz[i];
        }
    }
    return maximum;
}

void odredi_povrsinu(graf *mojGraf) {
    int maxID = najefikasnija_mina(mojGraf), n;
    int *eksplodirale = obilazak_grafa(mojGraf, maxID, &n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < mojGraf->n; j++) {
            if (eksplodirale[i] == mojGraf->cvorovi[j].id) {
                eksplodirale[i] = j;
                break;
            }
        }
    }

    double minX[mojGraf->n], maxX[mojGraf->n], minY[mojGraf->n], maxY[mojGraf->n];
    for (int i = 0; i < mojGraf->n; i++) {
        minX[i] = mojGraf->cvorovi[i].x - mojGraf->cvorovi[i].radius;
        maxX[i] = mojGraf->cvorovi[i].x + mojGraf->cvorovi[i].radius;
        minY[i] = mojGraf->cvorovi[i].y - mojGraf->cvorovi[i].radius;
        maxY[i] = mojGraf->cvorovi[i].y + mojGraf->cvorovi[i].radius;
    }

    double sX = min(minX, mojGraf->n);
    double bX = max(maxX, mojGraf->n);
    double sY = min(minY, mojGraf->n);
    double bY = max(maxY, mojGraf->n);

    double rangeX = fabs(bX - sX);
    double rangeY = fabs(bY - sY);

    double Xs[100000];
    for (int i = 0; i < 100000; i++) {
        Xs[i] = (double)rand()/RAND_MAX * rangeX + sX;
    }

    double Ys[100000];
    for (int i = 0; i < 100000; i++) {
        Ys[i] = (double)rand()/RAND_MAX *rangeY +sY;
    }

    int cnt = 0;
    for (int i = 0; i < 100000; i++) {
        cvor tacka = {0, Xs[i], Ys[i], 0};
        for (int j = 0; j < n; j++) {
            if (pripada_radijusu(mojGraf->cvorovi[eksplodirale[j]], tacka)) {
                cnt++;
                break;
            }
        }
    }
    free(eksplodirale);

    printf("\nPovrsina eksplozije najefikasnije mine je %.2lf!\n", ((double)cnt/100000)*rangeX*rangeY);
}

int main() {
    int izbor;
    while(1) {
        fflush(stdin);
        printf("Dobrodosli!\nOdaberite zadatak koji zelite da testirate:\n1. Zadatak 1\n2. Zadatak 2\n");
        if (scanf("%d", &izbor) == 0 || izbor < 1 || izbor > 2) {
            printf("Vrednost koju ste uneli nije validna!");
            continue;
        }
        break;
    }

    if (izbor == 1) {
        printf("Dobrodosli u zadatak 1!\n");
        graf *mojGraf = NULL;

        while (1) {
            fflush(stdin);
            printf("\n~~~Glavni meni~~~\n\n");
            printf("1. Kreiraj prazan graf\n2. Dodaj cvor u graf\n3. Ukloni cvor iz grafa\n4. Dodaj granu u graf\n"
                   "5. Ukloni granu iz grafa\n6. Ispisi reprezentaciju grafa\n\n7. Obrisi graf iz memorije\n8. Prekini program\n\n");
            printf("Izaberite jednu od ponudjenih opcija:\n");

            if (scanf("%d", &izbor) == 0 || izbor < 1 || izbor > 8) {
                printf("\nVrednost koju ste uneli nije validna!\n\n");
                continue;
            }

            if (mojGraf == NULL && (izbor != 1 && izbor != 8)) {
                printf("\nDa biste koristili ovu funkciju prvo morate kreirati graf!\n");
                continue;
            }

            switch (izbor) {
                case 1:
                    fflush(stdin);
                    int n;
                    printf("\nUnesite broj cvorova grafa:\n");
                    if (scanf("%d", &n) == 0 || n < 1) {
                        printf("\nVrednost koju ste uneli nije validna!\n");
                        continue;
                    }
                    mojGraf = kreiraj_graf(n);
                    break;
                case 2:
                    printf("\nDodali ste novi cvor u graf!\n");
                    dodaj_cvor(mojGraf);
                    break;
                case 3:
                    fflush(stdin);
                    int cvorID;
                    printf("\nUnesite ID cvora koji zelite da obrisete:\n");
                    if (scanf("%d", &cvorID) == 0 || !postoji_ID(mojGraf, cvorID)) {
                        printf("\nCvor sa zadatim ID-jem ne postoji!\n");
                        continue;
                    }
                    ukloni_cvor(mojGraf, cvorID);
                    break;
                case 4:
                    fflush(stdin);
                    int cvor1ID, cvor2ID;
                    printf("\nUnesite ID-jeve cvorova koje zelite da povezete:\n");
                    if (scanf("%d %d", &cvor1ID, &cvor2ID) < 2 || !postoji_ID(mojGraf, cvor1ID) || !postoji_ID(mojGraf, cvor2ID)) {
                        printf("\nCvor sa zadatim ID-jem ne postoji!\n");
                        continue;
                    }
                    dodaj_granu(mojGraf, cvor1ID, cvor2ID);
                    break;
                case 5:
                    fflush(stdin);
                    printf("\nUnesite ID-jeve cvorova ciju granu zelite da raskinete:\n");
                    if (scanf("%d %d", &cvor1ID, &cvor2ID) < 2 || !postoji_ID(mojGraf, cvor1ID) || !postoji_ID(mojGraf, cvor2ID)) {
                        printf("\nCvor sa zadatim ID-jem ne postoji!\n");
                        continue;
                    }
                    ukloni_granu(mojGraf, cvor1ID, cvor2ID);
                    break;
                case 6:
                    ispisi_graf(mojGraf);
                    break;
                case 7:
                    printf("\nVas graf je obrisan!\n");
                    obrisi_graf(mojGraf);
                    mojGraf = NULL;
                    break;
                default:
                    obrisi_graf(mojGraf);
                    printf("\nDo sledeceg vidjenja! :*\n");
                    return 0;
            }
        }
    } else {
        char buffer[100];
        char **strings;
        graf *mojGraf;

        while (1) {
            fflush(stdin);
            printf("Dobrodosli!\n\nUnesite ime datoteke iz koje biste ucitali graf:\n");
            fgets(buffer, 100, stdin);
            buffer[strlen(buffer) - 1] = '\0';

            strings = read_file(buffer);
            if (strings == NULL) {
                continue;
            }

            mojGraf = ucitaj_podatke(strings);
            break;
        }

        while (1) {
            fflush(stdin);
            printf("\n~~~Glavni meni~~~\n\n");
            printf("1. Prikazi graf\n2. Odredi efikasnost mine\n3. Odredi najefikasniju minu\n4. Iskoristi raketu\n5. Odredi povrsinu eksplozije najefikasnije mine\n\n6. Ucitaj novi graf\n7. Prekini program\n\n");
            printf("Izaberite jednu od ponudjenih opcija:\n");

            if (scanf("%d", &izbor) == 0 || izbor < 1 || izbor > 7) {
                printf("\nVrednost koju ste uneli nije validna!\n\n");
                continue;
            }

            switch (izbor) {
                case 1:
                    printf("\n");
                    ispisi_graf(mojGraf);
                    break;
                case 2:
                    fflush(stdin);
                    int minaID;
                    printf("\nUnesite ID mine ciju efikasnost zelite da ispitate:\n");
                    if (scanf("%d", &minaID) == 0 || !postoji_ID(mojGraf, minaID)) {
                        printf("\nMina sa zadatim ID-jem ne postoji!\n");
                        break;
                    }
                    printf("\n");
                    efikasnost_mine(mojGraf, minaID);
                    break;
                case 3:
                    printf("\n");
                    int maxID = najefikasnija_mina(mojGraf), cnt;
                    int *obilazak = obilazak_grafa(mojGraf, maxID, &cnt);
                    free(obilazak);
                    printf("Mina ciji je ID %d je najefikasnija! Njena efikasnost je %d!\n", maxID, cnt);
                    break;
                case 4:
                    fflush(stdin);
                    printf("\nUnesite koordinate pada rakete i radijus njene eksplozije:\n");
                    double x, y, r;
                    if (scanf("%lf %lf %lf", &x, &y, &r) != 3 || r < 0) {
                        printf("\nDoslo je do greske prilikom unosa!\n");
                        break;
                    }
                    printf("\n");
                    raketa(mojGraf, x, y, r);
                    break;
                case 5:
                    odredi_povrsinu(mojGraf);
                    break;
                case 6:
                    fflush(stdin);
                    printf("\nUnesite ime datoteke iz koje biste ucitali novi graf:\n");
                    fgets(buffer, 100, stdin);
                    buffer[strlen(buffer) - 1] = '\0';

                    strings = read_file(buffer);
                    if (strings == NULL) {
                        continue;
                    }

                    obrisi_graf(mojGraf);
                    mojGraf = ucitaj_podatke(strings);
                    break;
                default:
                    obrisi_graf(mojGraf);
                    printf("\nDo sledeceg vidjenja! :*");
                    return 0;
            }
        }
    }

//    graf *mojGraf = kreiraj_graf(4);
//    ukloni_cvor(mojGraf, 2);
//    dodaj_cvor(mojGraf);
//    dodaj_granu(mojGraf, 1, 3);
//    dodaj_granu(mojGraf, 3, 0);
//    ispisi_graf(mojGraf);
//    obrisi_graf(mojGraf);
}