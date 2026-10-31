#pragma once
#include "TimerModel.h"
#include "Translations.h"
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <locale>
#include <sstream>

namespace SoundnaReport {
inline std::string duration(double seconds)
{
    const auto s = static_cast<int64_t>(TimerStats::sane(seconds));
    std::ostringstream o; o << std::setfill('0') << std::setw(2) << s/3600 << ':'
        << std::setw(2) << (s/60)%60 << ':' << std::setw(2) << s%60; return o.str();
}
inline std::string utc(int64_t millis)
{
    if (millis <= 0) return "-";
    const std::time_t t = static_cast<std::time_t>(millis / 1000);
    std::tm value{};
#ifdef _WIN32
    gmtime_s(&value, &t);
#else
    gmtime_r(&t, &value);
#endif
    char result[32]{}; std::strftime(result, sizeof(result), "%Y-%m-%d %H:%M:%S", &value); return result;
}
// PDF Base-14 Helvetica with WinAnsi encoding. Preserve Italian/Western-European
// names. Unsupported glyphs become '?' rather than corrupting the PDF stream.
inline std::string winAnsi(const std::string& utf8)
{
    std::string result;
    for (size_t i=0; i<utf8.size();) {
        uint32_t cp=static_cast<unsigned char>(utf8[i++]);
        int following=0;
        if (cp>=0xc0 && cp<0xe0) { cp &= 31; following=1; }
        else if (cp>=0xe0 && cp<0xf0) { cp &= 15; following=2; }
        else if (cp>=0xf0) { cp &= 7; following=3; }
        for (int n=0; n<following && i<utf8.size(); ++n) cp=(cp<<6)|(static_cast<unsigned char>(utf8[i++])&63);
        if ((cp>=32 && cp<=126) || (cp>=160 && cp<=255)) result += static_cast<char>(cp);
        else if (cp==0x20ac) result += static_cast<char>(128);
        else if (cp==0x2013 || cp==0x2014) result += '-';
        else if (cp==0x2018 || cp==0x2019) result += '\'';
        else if (cp==0x201c || cp==0x201d) result += '"';
        else result += '?';
    } return result;
}
inline std::string literal(const std::string& value)
{
    std::string out="(";
    for (char character: value) {
        const auto c=static_cast<unsigned char>(character);
        if(c=='(' || c==')' || c=='\\') { out+='\\'; out+=static_cast<char>(c); }
        else if(c<32 || c>126) { char octal[5]; std::snprintf(octal,sizeof(octal),"\\%03o",c); out+=octal; }
        else out+=static_cast<char>(c);
    } return out+")";
}
struct Canvas {
    std::ostringstream out;
    Canvas() { out.imbue(std::locale::classic()); out << std::fixed << std::setprecision(2); }
    void rect(double x,double top,double w,double h,const char* colour)
    { out << colour << " rg " << x << ' ' << 842-top-h << ' ' << w << ' ' << h << " re f\n"; }
    void text(double x,double top,double size,const std::string& value,bool bold=false,const char* colour="0.957 0.957 0.961")
    { out << "BT " << colour << " rg /" << (bold?"F2":"F1") << ' ' << size << " Tf 1 0 0 1 " << x << ' ' << 842-top-size << " Tm " << literal(value) << " Tj ET\n"; }
};
struct ReportData { TimerModel model; std::string project; int64_t generatedAt=0; SafeHearing::Language language=SafeHearing::Language::Italian; };
inline std::string createPdf(const ReportData& data)
{
    using SafeHearing::Text;
    const auto t=[&data](Text key) { return winAnsi(SafeHearing::tr(data.language,key)); };
    auto rows=data.model.history;
    auto current=data.model.active; current.endMs=data.generatedAt; rows.push_back(current);
    const auto& m=data.model;
    std::vector<std::string> pages;
    size_t row=0;
    do {
        Canvas c;
        const bool first=pages.empty();
        const int page=static_cast<int>(pages.size()+1);
        c.rect(0,0,595,842,"0.071 0.075 0.090");
        c.rect(0,0,595,8,"0.90 0.286 0.286");
        c.text(40,30,18,"SOUNDNA - Safe Hearing",true); c.text(500,35,10,"REPORT",true,"0.90 0.286 0.286");
        auto title=(data.project.empty()?t(Text::UntitledProject):winAnsi(data.project));
        if(title.size()>62) title=title.substr(0,59)+"...";
        c.text(40,70,std::min(15.0,515.0 / std::max(1.0,title.size()*0.85)),title,true);
        c.text(40,95,9,t(Text::Exported)+utc(data.generatedAt)+" UTC   |   v1.5.1",false,"0.69 0.71 0.75");
        double y=128;
        if(first) {
            struct Metric { std::string label,value; };
            const Metric metrics[]={{t(Text::WorkMetric),duration(m.session)},
                {t(Text::PlaybackMetric),duration(m.listening)}, {t(Text::BreakMetric),duration(m.pauseTotal)},
                {t(Text::BreakCountsMetric),std::to_string(m.pausesStarted)+" / "+std::to_string(m.pausesCompleted)},
                {t(Text::BlockMetric),duration(m.cycle)},
                {t(Text::BreakAverageMetric), m.pausesCompleted ? duration(std::max(0.0,m.pauseTotal-(m.onBreak?m.breakTime:0.0))/m.pausesCompleted):"-"}};
            for(int i=0;i<6;++i) {
                double x=40+(i%3)*175, top=128+(i/3)*72;
                c.rect(x,top,165,62,"0.12 0.13 0.16");
                c.text(x+10,top+10,7.5,metrics[i].label,true,"0.69 0.71 0.75");
                c.text(x+10,top+28,19,metrics[i].value,true);
            }
            c.text(40,282,10,t(Text::WorkflowHeading),true);
            c.text(40,304,9,t(Text::BlockAverage)+(m.pausesStarted?duration(m.completedCycleTotal/m.pausesStarted):"-")
                +t(Text::LongestBlock)+(m.pausesStarted?duration(m.longestCycle):"-"));
            c.text(40,321,9,t(Text::CurrentReminder)+std::to_string(m.intervalMinutes)+t(Text::ReminderNote));
            c.text(40,338,9,t(Text::PlaybackNote));
            c.text(40,355,9,m.legacyTotals ? t(Text::LegacyNote)
                : t(Text::MeasurementNote));
            y=392;
        }
        c.text(40,y,11,t(Text::Timeline),true); y+=23;
        c.rect(40,y,515,24,"0.21 0.23 0.26");
        const double cols[]={46,72,164,277,391,475};
        const std::string labels[]={"#",t(Text::Activity),t(Text::StartUtc),t(Text::EndUtc),t(Text::Duration),"PLAY"};
        for(int i=0;i<6;++i)c.text(cols[i],y+7,8,labels[i],true,"1 1 1");
        y+=24;
        while(row<rows.size() && y+34<=745) {
            const auto& r=rows[row];
            if(row%2==0)c.rect(40,y,515,34,"0.10 0.11 0.14");
            c.text(cols[0],y+9,8,std::to_string(row+1));
            c.text(cols[1],y+5,9,r.pause?t(Text::Break):t(Text::Work),true);
            c.text(cols[1],y+19,7,row+1==rows.size()?t(Text::InProgress):r.continuation?t(Text::Checkpoint):t(Text::Completed),false,"0.69 0.71 0.75");
            c.text(cols[2],y+10,7.5,utc(r.startMs));
            c.text(cols[3],y+10,7.5,utc(r.endMs));
            c.text(cols[4],y+9,9,duration(r.seconds)); c.text(cols[5],y+9,9,duration(r.playback));
            y+=34; ++row;
        }
        c.text(40,767,8,t(Text::TimelineNote));
        c.text(40,781,8,t(Text::DurationNote));
        c.text(40,811,8,"SOUNDNA - Safe Hearing  /  "+t(Text::ReportFooter),false,"0.69 0.71 0.75");
        c.text(502,811,8,t(Text::Page)+std::to_string(page),false,"0.69 0.71 0.75");
        pages.push_back(c.out.str());
    } while(row<rows.size());
    std::vector<std::string> objects;
    objects.push_back("<< /Type /Catalog /Pages 2 0 R >>");
    std::string kids; for(size_t i=0;i<pages.size();++i)kids+=std::to_string(5+i*2)+" 0 R ";
    objects.push_back("<< /Type /Pages /Count "+std::to_string(pages.size())+" /Kids ["+kids+"] >>");
    objects.push_back("<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica /Encoding /WinAnsiEncoding >>");
    objects.push_back("<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica-Bold /Encoding /WinAnsiEncoding >>");
    for(size_t i=0;i<pages.size();++i) {
        objects.push_back("<< /Type /Page /Parent 2 0 R /MediaBox [0 0 595 842] /Resources << /Font << /F1 3 0 R /F2 4 0 R >> >> /Contents "+std::to_string(6+i*2)+" 0 R >>");
        objects.push_back("<< /Length "+std::to_string(pages[i].size())+" >>\nstream\n"+pages[i]+"endstream");
    }
    std::string pdf="%PDF-1.4\n%\xE2\xE3\xCF\xD3\n";
    std::vector<size_t> offsets;
    for(size_t i=0;i<objects.size();++i){offsets.push_back(pdf.size());pdf+=std::to_string(i+1)+" 0 obj\n"+objects[i]+"\nendobj\n";}
    const size_t xref=pdf.size();
    pdf+="xref\n0 "+std::to_string(objects.size()+1)+"\n0000000000 65535 f \n";
    for(auto offset:offsets){char b[32];std::snprintf(b,sizeof(b),"%010llu 00000 n \n",static_cast<unsigned long long>(offset));pdf+=b;}
    pdf+="trailer\n<< /Size "+std::to_string(objects.size()+1)+" /Root 1 0 R >>\nstartxref\n"+std::to_string(xref)+"\n%%EOF\n";
    return pdf;
}
}
