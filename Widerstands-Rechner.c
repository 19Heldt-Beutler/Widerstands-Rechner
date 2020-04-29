#include <stdio.h>
#include <stdlib.h>
//sleep funktion
#include <unistd.h>
//cmd to lower & isdigit
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
//zum kompilieren -lm hinzufügen unter linux
#include <math.h>

//#include <errno.h>

//maximum für eingabelänge eines einzelnen wertes
#define MAX 20

//pointer für einzelne datenwerte
unsigned long *numbers = NULL;

//pointer für einzelne chars von einem Daten Wert
char *numberstring = NULL;

//länge, um array größe weitergeben zu können an andere funktionen und damit werte auch nach speichern vom menü aus wieder gelesen werden können
int laenge_global = 0;

//deklaration aller funktionen
int readString(int zaehler);

int stringToNum(char q[], int stringlaenge_stringToNum);

int saveNum();

bool menu(int laenge_menu);
bool looping = true;

void displayValues(unsigned long q[], int laenge_displayvalues);

unsigned long calcReihe(unsigned long q[], int laenge_row);

unsigned long calcPara(unsigned long q[], int laenge_para);

void displayHelp();

void displayData(unsigned long result);

//hauptfunktion, zeigt einmal alle optionen und steuerungen an mit displayHelp() und ruft dann menu() auf
int main(void)
{
    printf("\n--------------------------------------------------------------------------------------------\n");

    printf("\n\t\thello resistor world!\n");

    printf("\n--------------------------------------------------------------------------------------------\n");

    displayHelp();

    do
    {
        looping = menu(laenge_global);
    } while (looping);

    printf("\n--------------------------------------------------------------------------------------------\n");

    printf("\nexit program\n");
    return 0;
}

//menü funktion nimmt laenge als parameter um gespeicherte werte wieder ausgeben zu können und die für die anderen berechnungsfunktionen benötigt wird
bool menu(int laenge_menu)
{
    printf("\n--------------------------------------------------------------------------------------------\n");

    printf("\nmain menu - choose option: \t");

    unsigned long result = 0;
    char cmd = 0;
    cmd = getchar();
    cmd = tolower(cmd);

    printf("\n--------------------------------------------------------------------------------------------\n");

    if (cmd == '\n')
    {
        return looping;
    }

    //newline abfangen
    getchar();

    //eingabe entgegennehmen und dann entsprechend andere funktionen ausführen
    switch (cmd)
    {
    //h -> help
    case 104:
        displayHelp();
        break;
    //q -> quit
    //if looping was set false -> do while in main ends
    case 113:
        printf("\nbye\n");
        looping = false;
        break;
    // n -> neue werte eingeben
    //daten einlesen mit saveNum()
    case 110:
        laenge_global = saveNum();
        break;
    //a -> werte anzeigen lassen
    case 97:
        displayValues(numbers, laenge_global);
        break;
    //r -> reihe
    //daten an calcReihe()
    //result an displayData()
    case 114:
        result = calcReihe(numbers, laenge_global);
        displayData(result);
        break;
    //p -> parallel
    case 112:
        result = calcPara(numbers, laenge_global);
        displayData(result);
        break;
    //wenn etwas anderes als q,h,p,r,n,a eingegeben wird, sage falsche eingabe
    default:
        printf("\n wrong input: %d %c\n", cmd, cmd);
        sleep(1);
        return looping;
    }
    return looping;
}

//funktion um eingegebene werte anzuzeigen
void displayValues(unsigned long q[], int laenge_displayvalues)
{
    if (laenge_displayvalues != 0)
    {
        printf("\n Your input was: ");
        for (int i = 0; i <= laenge_displayvalues - 1; i++)
        {
            printf("\nResistor value %d: %lu", i + 1, q[i]);
        }
    }
    else
    {
        printf("\nno values have been entered yet\n");
    }
}

//funktion die einlese und umwandlungs funktionen aufruft und nummern speichert
int saveNum()
{
    /*
    speicher wieder freigeben, erst sinnvoll wenn neue werte eingegeben werden, weil
    wenn schon nach displayresult() free(numbers) ausgeführt würde, wären die werte nicht mehr für andere berechnung verfügbar 
    */
    free(numbers);

    //zählervariable
    int n = 0;
    //größe des arrays
    int max = 1;
    //zwischenspeicher für die eingabe
    unsigned long z;

    int stringlaenge_saveNum;

    //initial speicher belegen
    numbers = malloc(max * sizeof(unsigned long));
    //falls kein speicher verfügbar ist
    if (NULL == numbers)
    {
        printf("\nno space left in the heap\n");
        return EXIT_FAILURE;
    }
    printf("input your resistor values --- enter a 0 to finish\n");
    while (true)
    {
        //string einlesen und anschließend umwandeln zur nummer die gespeichert werden soll
        stringlaenge_saveNum = readString(n);
        z = stringToNum(numberstring, stringlaenge_saveNum);
        if (z == 0)
        {
            break;
        }
        //wenn zählervariable größer gleich größe des belegten speichers erhöhe max und realloc neuen speicher
        if (n >= max)
        {
            max++;
            numbers = realloc(numbers, max * sizeof(unsigned long));
            if (NULL == numbers)
            {
                printf("\nno space left on the heap\n");
                return EXIT_FAILURE;
            }
        }
        //speicher den eingegebenen wert im array
        numbers[n++] = z;
    }
    //zählervariable = länge des arrays -> zurückgeben
    return n;
}

//liest widerstand als string ein
int readString(int zaehler)
{
    //initial speicher belegen
    //kein realloc zwingend nötig?! wegen MAX = 20 , restlicher reservierter speicher stört die rechnung nicht und wird am ende der funktion stringToNum wieder freigegeben
    numberstring = malloc(MAX * sizeof(char));
    //falls kein speicher verfügbar ist
    if (NULL == numberstring)
    {
        printf("\nno space left on the heap\n");
        return EXIT_FAILURE;
    }
    printf("\ninsert %d. resistor value: ", zaehler + 1);
    fgets(numberstring, MAX, stdin);
    //fgets newline auschneiden
    size_t p = strlen(numberstring);
    numberstring[p - 1] = '\0';
    //stringlänge ermitteln
    size_t stringlaenge_readString;
    stringlaenge_readString = strlen(numberstring);
    //prüfen ob der string überhaupt legit ist, sonst kann man sich die funktion stringToNum() erstmal sparen bis eine gültige eingabe gemacht wurde
    //strspn nimmt als argumente den numberstring und die erlaubten zeichen und gibt die position von unerlaubten zeichen im string zurück
    //wenn pos1 = letztes zeichen dann ist die eingabe erlaubt
    int pos1 = strspn(numberstring, "0123456789,.kmKM");

    if (pos1 != strlen(numberstring))
    {
        printf("\ninvalid input !!errorcode 1!!: invalid token on position %d\n", pos1 + 1);
        printf("\n...back to main menu to try again...\n");

        // einfaches aufrufen des menüs gibt beim verlassen des programms error: "free(): double free detected in tcache 2"
        // free() sanity check fail? chunk zu groß für tcache bin? -> heap lock?
        //return 0 ist der workaround, keine gute lösung, aber funktioniert, da stringlaenge 0 weiter an stringToNum gegeben wird und wiederrum 0 zurückgibt. demnach z==0 -> break -> saveNum wird beendet ohne das die fehlerhafte eingabe gespeichert wird
        //da nun menu() eh nicht mehr rekursiv aufgerufen wird, ist return 0 doch eine gute lösung
        stringlaenge_readString=0;
        free(numberstring);
        return stringlaenge_readString;
    }

    //prüfe auf groszes und kleines K via strchr
    char bigK = 'K';
    char smolK = 'k';

    char *checkstring_bigK;
    char *checkstring_smolK;

    checkstring_bigK = strchr(numberstring, bigK);
    checkstring_smolK = strchr(numberstring, smolK);

    //wenn K nicht vorkommt invertiert
    //also wenn K vorkommt
    if (!(checkstring_bigK == NULL))
    {
        //wenn K nur am Ende vorkommt invertiert
        //also wenn K vorkommt dann frag ob 'K' am Ende ist. Wenn es nicht am ende ist -> eingabe fehler
        // -> free(numberstring), sonst wird die zahl bis zum fehler gespeichert: bsp: 34k34 -> 34 wird gespeichert
        // -> stringlaenge auf 0 und return stringlaenge 
        //selbes prinzip darunter mit kleinem 'k'
        if (!(checkstring_bigK[0] == 'K' && *(checkstring_bigK + 1) == '\0'))
        {
            printf("\ninvalid input !!errorcode 2!!: einheitenvorzeichen 'K' an falscher stelle");
            printf("\n...back to main menu to try again...\n");
            stringlaenge_readString=0;
            free(numberstring);
            return stringlaenge_readString;
        }
    }   

    if (!(checkstring_smolK == NULL))
    {
        // K kommt irgendwo außer am ende vor -> nicht ok
        if (!((checkstring_smolK[0] == 'k' && *(checkstring_smolK + 1) == '\0')))
        {
            printf("\ninvalid input !!errorcode 2!!: einheitenvorzeichen 'k' an falscher stelle");
            printf("\n...back to main menu to try again...\n");
            stringlaenge_readString=0;
            free(numberstring);
            return stringlaenge_readString;
        }
    }
  
    //prüfe ob es nur ein komma bzw punkt gibt
    char komma = ',';
    char punkt = '.';

    char *checkstring_komma;
    char *checkstring_punkt;

    checkstring_komma = strchr(numberstring, komma);
    checkstring_punkt = strchr(numberstring, punkt);

    








    return stringlaenge_readString;
}

//wandelt string in eine nummer um und rechnet komma und einheitenvorzeichen um
int stringToNum(char q[], int stringlaenge_stringToNum)
{
    unsigned long number = 0;
    //prüfen ob der string schon eine einfache nummer ist ohne . oder , oder einheitenvorzeichen, dann kann man sich die weiteren operationen sparen
    bool alreadydigit=false;
    for (int i = 0; i <= stringlaenge_stringToNum - 1; i++)
    {
        if (isdigit(q[i]) == 1)
        {
            alreadydigit = true;
        }
        else
        {
            alreadydigit = false;
            break;
        }
    }
    if (alreadydigit == true)
    {
        //string zu nummer convertieren
        number = strtol(q, NULL, 10);
    }
    //wenn der string keine reine nummer ist aka isdigit = 0 -> alreadydigit = false
    //dann untersuche string auf K oder M und anschließend auf , und . und wandle dementsprechend um
    else
    {

        bool kommavorhanden;
        char potenzvorhanden;

        //prüfen ob das letzte zeichen ein bekanntes Einheitenvorzeichen ist
        //auf k oder K prüfen
        if (q[stringlaenge_stringToNum - 1] == 75 || q[stringlaenge_stringToNum - 1] == 107)
        {
            potenzvorhanden = 75;
            //K abschneiden
            q[stringlaenge_stringToNum - 1] = '\0';
        }
        //auf M oder m prüfen
        else if (q[stringlaenge_stringToNum - 1] == 77 || q[stringlaenge_stringToNum - 1] == 109)
        {
            potenzvorhanden = 77;
            //M abschneiden
            q[stringlaenge_stringToNum - 1] = '\0';
        }
        //von vorne anfangend nach einem komma oder punkt suchen
        for (int k = 0; k <= stringlaenge_stringToNum - 1; k++)
        {
            if (q[k] == 44)
            {
                //falls komma mach ein punkt drauß für die umwandlung mit float
                q[k] = 46;
                kommavorhanden = true;
            }
            else if (q[k] == 46)
            {
                kommavorhanden = true;
            }
        }
        //zwischenvariablen für die umrechnungen
        float ftemp;
        unsigned long itemp;

        if (kommavorhanden == true)
        {
            //wenn komma vorhanden mache den string zu einem float und anschließend wenn vorhanden einheitenvorzeichen hinzurechnen
            ftemp = strtof(q, NULL);
            if (potenzvorhanden == 75)
            {
                ftemp = ftemp * 1000;
            }
            else if (potenzvorhanden == 77)
            {
                ftemp = ftemp * 1000000;
            }
            //fehlerfreie umwandlung von float zu int?!?!
            number = ftemp;
        }
        else if (kommavorhanden == false)
        {
            //wenn kein komma vorhanden mache den string zu einem int und anschließend wenn vorhanden einheitenvorzeichen hinzurechnen
            itemp = strtol(q, NULL, 10);
            if (potenzvorhanden == 75)
            {
                itemp = itemp * 1000;
            }
            else if (potenzvorhanden == 77)
            {
                itemp = itemp * 1000000;
            }
            number = itemp;
        }
    }
    //speicher des eingabe-strings wieder freigeben
    free(numberstring);
    //nummer zurückgeben für die funktion saveNum()
    return number;
}

void displayData(unsigned long result)
{

    printf("\n result: %lu ohm \n\n", result);

    /*    
    einheitenvorzeichen für ergebnis wieder anbauen 

    unsigned long result2;
    //ergebnis ausgeben ggf mit Einheitenvorzeichen
    if (result < 1000)
    {
        printf("\nresult: %lu ohm\n\n", result);
    }
    else if (result > 1000 && result < 1000000)
    {
        result2 = result%1000;
        result = result/1000;
        printf("\nresult: %lu.%luK ohm\n\n", result, result2);
    }
    else if (result > 1000000)
    {
        result = result/1000000;
        printf("\nresult: %luM ohm\n\n", result);
    }

*/
}

//funktionen um ergebnis zu berechnen mit den werten
unsigned long calcReihe(unsigned long q[], int laenge_row)
{
    printf("\nyou chose the calculation of the series circuit\n");
    unsigned long result = 0;

    for (int k = 0; k <= laenge_row - 1; k++)
    {
        result += q[k];
    }
    return result;
}

unsigned long calcPara(unsigned long q[], int laenge_para)
{
    printf("\nyou chose the calculation of the parallel circuit\n");
    //float benötigt um ein einigermaßen genaues ergebnis zu bekommen
    double floatresult = 0;

    for (int h = 0; h <= laenge_para - 1; h++)
    {
        double temp = (q[h]);
        floatresult += 1 / temp;

    }
    floatresult = 1 / floatresult;

    //funktion round() aus math.h rundet den float und gibt ihn zurück an result
    unsigned long result = round(floatresult);
    return result;
}

//anzeige der optionen
void displayHelp()
{
    printf("\n--------------------------------------------------------------------------------------------\n");

    printf("\n");
    printf("\tPress\n");
    printf("\tQ - to quit\n");
    printf("\n\tN - to insert new resistor values\n");
    printf("\tA - to show your inserted values\n");
    printf("\tR - to calculate for a series circuit\n");
    printf("\tP - to calculate for a parallel circuit - note that the result will be rounded\n");
    printf("\n\tH - to show these instructions again\n");
    printf("\n");

    printf("\n--------------------------------------------------------------------------------------------\n");
}
