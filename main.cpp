//POP 2021-01-20 projekt 2, Zarychta Wojciech EiT 7 184525, CodeBlocks 20.03
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

//---------------------------------------
struct Bitmap{              //struktura, w której będą zapisane niezbędne informacje dot. bitmapy
    int bfSize;             //4 bajty
    int bfOffBits;          //4
    int biWidth;            //4
    int biHeight;           //4
    uint16_t biBitCount;    //2
    int biSizeImage;        //4
};

struct RGB{
    int red;
    int green;
    int blue;
};
//------------------------------------------
void menu( Bitmap BMP, fstream& fs, RGB *colors );
void show_menu( fstream &fs, string name );
bool open_BMP ( fstream &fs, string &name );
void read_header( fstream& fs, Bitmap &BMP );
void read_picture( fstream& fs, Bitmap &BMP, RGB *colors );
void create_brightness_histogram( RGB *colors, string name );
void create_RGB_histogram( RGB *colors, string name );
void RGB_table( RGB histogram[], int intervals, int interval_length );
void RGB_graph( RGB histogram[], int intervals, int height, int utmost );
void bright_table( int histogram[], int intervals, int interval_length );
void bright_graph( int histogram[], int intervals, int height, int utmost );
void save_brightness( int histogram[], int utmost, int intervals, string name );
void save_RGB( RGB histogram[], int utmost, int intervals, string name );
//------------------------------------------

int main()
{
    Bitmap BMP;
    fstream file;
    RGB colors[256];            //tablica, w której każdy element stanowi liczebność pikseli o n-tej jasności
    RGB *colors_ptr;
    colors_ptr = colors;

    menu(BMP, file, colors_ptr);
    file.close();

    return 0;
}


void menu( Bitmap BMP, fstream& fs, RGB *colors )
{
    int choice;
    string name;

    do {
        show_menu(fs, name);
        cin >> choice;
        switch(choice){
            case 0: exit(0);
            case 1: {
                if (open_BMP( fs, name ) == true) {
                    read_header( fs, BMP );
                    read_picture( fs, BMP, colors );
                }
            }
            break;
            case 2: {
                if (fs.is_open()) create_brightness_histogram(colors, name);    //jeśli otworzono plik
                else cout << "najpierw zaladuj poprawny plik" << endl;
                system ("pause");
            }
            break;
            case 3: {
                if (fs.is_open()) create_RGB_histogram(colors, name);
                else cout << "najpierw zaladuj poprawny plik" << endl;          //jeśli nie otworzono pliku
                system ("pause");
            }
            break;
            default:{
                cout << "Niepoprawny znak" << endl;
                system("pause");
                system("cls");
            }
        }
    }
    while (choice != 0);
}


void show_menu( fstream &fs, string name )
{
    system("cls");
    cout << "-------------- MENU --------------" << endl <<
            "1. Zaladuj i przeanalizuj bitmape." << endl <<
            "2. Stworz histogram jasnosci." << endl <<
            "3. Stworz histogram RGB" << endl << endl <<
            "0. Zamknij program" << endl << endl << endl <<
            "Otwarty plik: ";
    if (fs.is_open()) cout << name;                         //wyświetla nazwę otwartego pliku
    else cout << " --- ";
    cout << endl << endl << endl << "> ";
}


bool open_BMP ( fstream &fs, string &name )
{
    system("cls");
    if (fs.is_open()) fs.close();                  //gdy był wcześniej otwarty plik, zamyka go
    cout << "Podaj nazwe pliku, ktory chcesz otworzyc: " << endl << "> ";
    cin >> name;
    if (name.size() < 5 || name.compare(name.size()-4, 4, ".bmp") != 0) {               //jeżeli plik nie jest w formacie .bmp
        cout << endl << "Niepoprawny format pliku, sprobuj ponownie" << endl << endl;
        system("pause");
        return false;
    }
    fs.open( name, ios::in | ios::binary );        //plik otwierany w trybie binarnym
    if (!fs) {                                     //kontrola otwarcia
        cerr << endl << "Nieudane otwarcie pliku '" << name << "'. Sprobuj ponownie." << endl << endl;
        system("pause");
        return false;
    }
    cout << endl << "*Plik zaladowano pomyslnie*" << endl << endl << endl;
    system("pause");

    return true;
}


void read_header( fstream& fs, Bitmap &BMP )            //funkcja odczytuje niezbędne dane z nagłówków bitmapy do struktury BMP
{
    fs.seekg(2, ios::beg);
    fs.read(reinterpret_cast<char*>(&BMP.bfSize), 4);
    fs.seekg(10, ios::beg);
    fs.read(reinterpret_cast<char*>(&BMP.bfOffBits), 4);
    fs.seekg(18, ios::beg);
    fs.read(reinterpret_cast<char*>(&BMP.biWidth), sizeof(BMP.biWidth));
    fs.read(reinterpret_cast<char*>(&BMP.biHeight), sizeof(BMP.biHeight));
    fs.seekg(2, ios::cur);
    fs.read(reinterpret_cast<char*>(&BMP.biBitCount), sizeof(BMP.biBitCount));
    fs.seekg(4, ios::cur);
}


void read_picture( fstream& fs, Bitmap &BMP, RGB *colors )
{
    int offset = BMP.bfOffBits;                         //odtąd zaczyna się obraz
    fs.seekg(offset, ios::beg);

    if (BMP.biBitCount == 24){                          //dla 24-bitowych plików
        for (int i = 0; i < 256; i++) {                 //inicjalizacja
            (colors+i)->blue = 0;
            (colors+i)->green = 0;
            (colors+i)->red = 0;
        }
        uint8_t *temp;
        temp = new uint8_t;
        for (int i = 0; i < BMP.biHeight; i++)
            for (int j = 0; j < BMP.biWidth; j++){
        //poprzez wskaźnik, inkrementuje element tablicy o indeksie n, gdzie n to poziom jasności piksela i w ten sposób zlicza liczbę pikseli o danej jasności
                fs.read(reinterpret_cast<char*>(temp), sizeof(*temp));
                (colors + *temp)->blue++;
                fs.read(reinterpret_cast<char*>(temp), sizeof(*temp));
                (colors + *temp)->green++;
                fs.read(reinterpret_cast<char*>(temp), sizeof(*temp));
                (colors + *temp)->red++;
            }
        delete temp;
        return;
    }
    else{        //dla 1-, 4-, i 8-bitowych
        cout << "Program nie obsluguje bitmap tego typu" << endl;
        system("pause");
        fs.close();
    }
}


void create_brightness_histogram( RGB *colors, string name )
{
    int intervals = 64;
    int interval_length = 256/intervals;
    int histogram[intervals];            //histogram o ilości elementów równej ilości przedziałów
    int height = 20;                     //wysokość wykresu

    for (int i = 0; i < intervals; i++)
            histogram[i] = 0;
    for (int i = 0; i < intervals; i++) {
        for (int j = i*interval_length; j < (i+1)*interval_length; j++) {
        // dla każdego elementu tablicy histogram zlicza liczebnośc pikseli w tablicy colors od indeksu równemu pierwszemu pikselowi przedziału do piksela o interval_length większego
            histogram[i] += colors->blue;
            histogram[i] += colors->green;
            histogram[i] += colors->red;
            colors++;                       //przesuwa wskaźnik na kolejną komórkę tablicy colors[256]
        }
    }
    int utmost =  histogram[0];  //liczba maksymalnych wystąpień; wyznaczam ją, aby wykres nie miał wysokości kilku tysięcy znaków
    for (int i = 1; i < intervals; i++) {
        if (histogram[i] > utmost) utmost = histogram[i];
    }

    system("cls");
    cout << setw(20) << "HISTOGRAM JASNOSCI" << endl << endl;
    bright_table(histogram, intervals, interval_length);
    bright_graph(histogram, intervals, height, utmost);

    system ("pause");
    cout << "Czy chcesz zapisac histogram? (T/N)" << endl << "> ";
    char choice;
    cin >> choice;
    if (choice == 't' || choice == 'T') save_brightness( histogram, utmost, intervals, name);
}


void bright_table( int histogram[], int intervals, int interval_length )            //tworzy tabelę o 'intervals' wierszach
{
    cout << setw(10) << "przedzial" << " |  " << "liczebnosc" << endl;
    for (int i = 0; i < intervals; i++) {
        cout << setw(6) << i*interval_length << "-" << setw(3) << (i+1)*interval_length-1 << " |  " << histogram[i] << endl;
    }
    cout << endl << endl;
}


void bright_graph( int histogram[], int intervals, int height, int utmost )         //tworzy wykres słupkowy o 'intervals' przedziałach i wysokości 'height'
{
    for (int i = height; i > 0; i--) {
        for (int j = 0; j < intervals; j++) {
            if (histogram[j]*height/utmost >= i) cout << (char)219;
            else cout << " ";
        }
        cout << endl;
    }
    for (int i = 0; i < intervals; i++)
        cout << (char)254;
    cout << endl << "ciemny" << setw(58) << "jasny" << endl << endl;
}


void create_RGB_histogram( RGB *colors, string name )
//taka sama funckja jak create_brightness_histogram, z tą różnicą, że używam tablicy struktur zamiast tablicy
{
    int intervals = 32;
    int interval_length = 256/intervals;
    RGB histogram[intervals];
    int height = 20;

    for (int i = 0; i < intervals; i++) {
        histogram[i].blue = 0;
        histogram[i].green = 0;
        histogram[i].red = 0;
    }
    for (int i = 0; i < intervals; i++)
        for (int j = i*interval_length; j < (i+1)*interval_length; j++){
            histogram[i].blue += colors->blue;
            histogram[i].green += colors->green;
            histogram[i].red += colors->red;
            colors++;
        }
    int utmost = 0;
    for (int i = 0; i < intervals; i++) {
        if (histogram[i].red > utmost) utmost = histogram[i].red;
        if (histogram[i].green > utmost) utmost = histogram[i].green;
        if (histogram[i].blue > utmost) utmost = histogram[i].blue;
    }

    system("cls");
    cout << setw(45) << "HISTOGRAM RGB" << endl << endl;
    RGB_table(histogram, intervals, interval_length);
    RGB_graph(histogram, intervals, height, utmost);

    system ("pause");
    cout << "Czy chcesz zapisac histogram? (t/n)" << endl << "> ";
    char choice;
    cin >> choice;
    if (choice == 't' || choice == 'T') save_RGB( histogram, utmost, intervals, name);
}


void RGB_table( RGB histogram[], int intervals, int interval_length )
{
    cout << setw(19) << "czerwony" << setw(26) << "zielony" << setw(28) << "niebieski" << endl;
    for (int i = 0; i <3; i++)
        cout << setw(13) << "przedzial" << " |  " << "liczebnosc";
    cout << endl;
    for (int i = 0; i < intervals; i++) {
        cout << setw(9) << i*interval_length << "-" << setw(3) << (i+1)*interval_length-1 << " |  " << setw(10) << histogram[i].red <<
                setw(9) << i*interval_length << "-" << setw(3) << (i+1)*interval_length-1 << " |  " << setw(10) << histogram[i].green <<
                setw(9) << i*interval_length << "-" << setw(3) << (i+1)*interval_length-1 << " |  " << setw(10) << histogram[i].blue << endl;
    }
    cout << endl << endl;
}


void RGB_graph( RGB histogram[], int intervals, int height, int utmost )
{
    cout << setw(20) << "czerwony" << setw(37) << "zielony" << setw(39) << "niebieski" << endl ;
    for (int i = height; i > 0; i--) {
        for (int j = 0; j < intervals; j++) {
            if (histogram[j].red*height/utmost >= i) cout << (char)219;
            else cout << " ";
        }
        cout << "      ";
        for (int j = 0; j < intervals; j++) {
            if (histogram[j].green*height/utmost >= i) cout << (char)219;
            else cout << " ";
        }
        cout << "      ";
        for (int j = 0; j < intervals; j++) {
            if (histogram[j].blue*height/utmost >= i) cout << (char)219;
            else cout << " ";
        }
        cout << endl;
    }
    for (int k = 0; k < 3; k++){
        for (int i = 0; i < intervals; i++)
            cout << (char)254;
        cout << "      ";
    }
    cout << endl << "ciemny" << setw(26) << "jasny" << setw(12) << "ciemny" << setw(26) << "jasny" << setw(12) << "ciemny" << setw(26) << "jasny" << endl << endl;
}


void save_brightness( int histogram[], int utmost, int intervals, string name )
{
    ofstream ofs;
    for (int i = 0; i < 4; i++) name.pop_back();            //usuwa 4 ostatnie znaki z nazwy (.bmp)
    name.append("_histogram_jasnosci.txt");                 //automatycznie tworzy nazwę docelowego pliku
    ofs.open( name );
    if (!ofs) {         //error
        cerr << "Nieudane otwarcie pliku " << name << endl;
        system("pause");
        return;
    }
    //drukuje to samo, co wyświetla się w konsoli przy egzekucji  bright_table  oraz  bright_graph
    ofs << setw(20) << "HISTOGRAM JASNOSCI" << endl << endl;
    ofs << setw(10) << "przedzial" << " |  " << "liczebnosc" << endl;
    for (int i = 0; i < intervals; i++) {
        ofs << setw(6) << i*16 << "-" << setw(3) << (i+1)*16-1 << " |  " << histogram[i] << endl;
    }
    ofs << endl << endl;
    for (int i = 20; i > 0; i--) {
        for (int j = 0; j < intervals; j++) {
            if (histogram[j]*20/utmost >= i) ofs << "█";
            else ofs << " ";
        }
        ofs << endl;
    }
    for (int i = 0; i < intervals; i++)
        ofs << "■";
    ofs << endl << "ciemny" << setw(58) << "jasny" << endl << endl;

    cout << endl << "*Pomyslnie zapisano plik tekstowy pod nazwa: " << name << "*" << endl << endl;
}


void save_RGB( RGB histogram[], int utmost, int intervals, string name )
//funkcja analogiczna do poprzedniej
{
    ofstream ofs;
    for (int i = 0; i < 4; i++) name.pop_back();
    name.append("_histogram_RGB.txt");
    ofs.open( name );
    if (!ofs) {
        cerr << "Nieudane otwarcie pliku " << name << endl;
        system("pause");
        return;
    }

    ofs << setw(40) << "HISTOGRAM RGB" << endl << endl;
    ofs << setw(19) << "czerwony" << setw(26) << "zielony" << setw(28) << "niebieski" << endl;
    for (int i = 0; i <3; i++)
        ofs << setw(13) << "przedzial" << " |  " << "liczebnosc";
    ofs << endl;
    for (int i = 0; i < intervals; i++) {
        ofs <<  setw(9) << i*4 << "-" << setw(3) << (i+1)*4-1 << " |  " << setw(10) << histogram[i].red <<
                setw(9) << i*4 << "-" << setw(3) << (i+1)*4-1 << " |  " << setw(10) << histogram[i].green <<
                setw(9) << i*4 << "-" << setw(3) << (i+1)*4-1 << " |  " << setw(10) << histogram[i].blue << endl;
    }
    ofs << endl << endl;
    ofs << setw(20) << "czerwony" << setw(37) << "zielony" << setw(39) << "niebieski" << endl ;
    for (int i = 20; i > 0; i--) {
        for (int j = 0; j < intervals; j++) {
            if (histogram[j].red*20/utmost >= i) ofs << "█";
            else ofs << " ";
        }
        ofs << "      ";
        for (int j = 0; j < intervals; j++) {
            if (histogram[j].green*20/utmost >= i) ofs << "█";
            else ofs << " ";
        }
        ofs << "      ";
        for (int j = 0; j < intervals; j++) {
            if (histogram[j].blue*20/utmost >= i) ofs << "█";
            else ofs << " ";
        }
        ofs << endl;
    }
    for (int k = 0; k < 3; k++){
        for (int i = 0; i < intervals; i++)
            ofs << "■";
        ofs << "      ";
    }
    ofs << endl << "ciemny" << setw(26) << "jasny" << setw(12) << "ciemny" << setw(26) << "jasny" << setw(12) << "ciemny" << setw(26) << "jasny" << endl << endl;

    cout << endl << "*Pomyslnie zapisano plik tekstowy pod nazwa: " << name << "*" << endl << endl;
}
