#pragma once

#include <string>

#using "Utilities.dll"

class Spider
{
public:
    static std::wstring GetHtmlByUrl(const std::wstring& url);
    static std::wstring GetDecodedHtml(const std::wstring& html);
    static void GenerateWordCloud(const std::wstring& csvFilePath, const std::wstring& savingFilePath);
};


std::wstring Spider::GetHtmlByUrl(const std::wstring& url)
{
    System::String ^ html = Utilities::Utilities::GetHtmlByUrl(gcnew System::String(url.c_str()));
    const wchar_t* chars = (const wchar_t *)(System::Runtime::InteropServices::Marshal::StringToHGlobalUni(html))
        .ToPointer();
    std::wstring htmlString = chars;

    System::Runtime::InteropServices::Marshal::FreeHGlobal(System::IntPtr((void *)chars));

    return htmlString;
}


std::wstring Spider::GetDecodedHtml(const std::wstring& html)
{
    System::String ^ decoded = Utilities::Utilities::GetDecodedHtml(gcnew System::String(html.c_str()));
    const wchar_t* chars = (const wchar_t *)(System::Runtime::InteropServices::Marshal::StringToHGlobalUni(decoded))
        .ToPointer();
    std::wstring decodedString = chars;

    System::Runtime::InteropServices::Marshal::FreeHGlobal(System::IntPtr((void *)chars));

    return decodedString;
}

void Spider::GenerateWordCloud(const std::wstring& csvFilePath, const std::wstring& savingFilePath)
{
    System::String ^ csvFilePathString = gcnew
    System::String(csvFilePath.c_str());
    System::String ^ savingFilePathString = gcnew
    System::String(savingFilePath.c_str());

    Utilities::Utilities::SaveWordCloud(csvFilePathString, savingFilePathString);
}
