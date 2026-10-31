#include "../Source/PdfReport.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
int main(int argc,char** argv)
{
    using namespace SoundnaReport;
    ReportData d; d.project="ESEMPIO - Mix città (dati dimostrativi)";
    int64_t now=1793347200000LL; d.model.begin(now);
    for(int i=0;i<4;++i) {
        d.model.advance(3600,2700); now+=3600000; d.model.startBreak(now);
        d.model.advance(600,0); now+=600000; d.model.endBreak(now);
    }
    d.model.advance(1200,480); now+=1200000; d.generatedAt=now;
    const auto bytes=createPdf(d);
    if(bytes.substr(0,8)!="%PDF-1.4" || bytes.find("startxref")==std::string::npos)throw std::runtime_error("Invalid PDF envelope");
    if(d.model.pausesCompleted!=4 || d.model.session!=15600 || d.model.pauseTotal!=2400 || d.model.cycle!=480)
        throw std::runtime_error("Wrong report totals");
    if(winAnsi("città").size()!=5 || literal("(a)\\b")!="(\\(a\\)\\\\b)")throw std::runtime_error("PDF text encoding");
    if(argc>1){std::ofstream f(argv[1],std::ios::binary);f<<bytes;if(!f)throw std::runtime_error("Cannot save sample");}
    {
        auto english=d; english.language=SafeHearing::Language::English;
        english.project="EXAMPLE - Project workflow";
        const auto translated=createPdf(english);
        if(translated.find("TIMELINE")==std::string::npos || translated.find("CRONOLOGIA")!=std::string::npos
            || translated.find("PROJECT WORK TIME")==std::string::npos)
            throw std::runtime_error("Report localisation");
        if(argc>3) { std::ofstream f(argv[3],std::ios::binary);f<<translated;
            if(!f)throw std::runtime_error("Cannot save English sample"); }
    }
    for(int i=0;i<80;++i){d.model.startBreak(++now);d.model.advance(15,0);d.model.endBreak(now+=15000);d.model.advance(60,30);now+=60000;}
    d.project="Nome molto lungo "+std::string(200,'X')+"\\(test)";d.generatedAt=now;
    if(argc>2){std::ofstream f(argv[2],std::ios::binary);f<<createPdf(d);if(!f)throw std::runtime_error("Cannot save stress report");}
    ReportData empty;empty.model.begin(now);empty.generatedAt=now;
    if(createPdf(empty).empty())throw std::runtime_error("Empty history export");
    std::cout<<"Report totals, encoding, empty state and pagination fixtures passed\n";
}
