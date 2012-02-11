#ifndef QQTEXTCHARFORMAT_H
#define QQTEXTCHARFORMAT_H

#include <QTextFormat>

enum SpecialTextFormats
{
    TotozTextFormat = QTextFormat::UserObject + 1,
    DuckTextFormat,
    NorlogeTextFormat,
    UATextFormat,
    LoginTextFormat
};

enum SpecialProperties
{
    TotozData = 1,
    NorlogeData,
    UAData,
//zz        DuckData,
    LoginData
};


#endif // QQTEXTCHARFORMAT_H
