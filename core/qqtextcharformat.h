#ifndef QQTEXTCHARFORMAT_H
#define QQTEXTCHARFORMAT_H

#include <QTextFormat>

enum SpecialTextFormats
{
    TotozTextFormat = QTextFormat::UserObject + 1,
    DuckTextFormat,
    NorlogeTextFormat,
    UATextFormat,
    LoginTextFormat,
    BouchotBgFormat
};

enum SpecialProperties
{
    TotozData = 1,
    NorlogeData,
    UAData,
//zz        DuckData,
    LoginData,
    BouchotData
};


#endif // QQTEXTCHARFORMAT_H
