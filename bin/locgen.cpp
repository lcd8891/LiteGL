#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>

#define WRITE(OBJ) write(reinterpret_cast<char*>(&OBJ),sizeof(OBJ))
#define READ(OBJ) read(reinterpret_cast<char*>(&OBJ),sizeof(OBJ))

struct Locale{
    unsigned offset;
    unsigned size;
};

void create_loc(std::vector<Locale> &locales){
    std::ofstream file("locale.loc",std::ios::binary);
    unsigned size = locales.size();
    file.WRITE(size);
    for(Locale& loc : locales){
        file.WRITE(loc.offset);
        file.WRITE(loc.size);
    }
    file.close();
}
void add_ascii_characters(std::vector<Locale> &locales){
    locales.push_back({32,95});
}
void parse_from_input(){
    unsigned n;
    std::vector<Locale> locales;
    add_ascii_characters(locales);
    std::wcout << "Type locales count: "; std::wcin >> n;
    for(unsigned i = 0;i<n;i++){
        wchar_t ch1,ch2;
        std::wcout << "Type first symbol: "; std::wcin >> ch1;
        std::wcout << "Type second symbol: "; std::wcin >> ch2;
        if(ch1>ch2){
            std::wcerr << "Unknown diapason!";
            i--;
            continue;
        }
        unsigned siuz = (unsigned)ch2 - (unsigned)ch1 + 1;
        locales.push_back({(unsigned)ch1,siuz});
        std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
    }
    create_loc(locales);    
}
void check_locale(std::vector<Locale> &locales){
    for(Locale& loc : locales){
        for(unsigned i = 0;i<loc.size;i++){
            wchar_t ch = i+loc.offset;
            std::wcout << ch << " ";
        }
        std::wcout << "End...\n";
    }
}
void load_locfile(std::filesystem::path _path){
    std::vector<Locale> locales;
    std::ifstream file(_path,std::ios::binary);
    unsigned size;
    file.READ(size);
    for(unsigned i = 0;i<size;i++){
        Locale loc;
        file.READ(loc.offset);
        file.READ(loc.size);
        locales.push_back(loc);
    }
    file.close();
    check_locale(locales);
}
int main(int argc,char **argv){
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
    std::wcin.imbue(std::locale());
    if(argc==2){
        load_locfile(argv[1]);
    }else{
        parse_from_input();
    }
    return 0;
}