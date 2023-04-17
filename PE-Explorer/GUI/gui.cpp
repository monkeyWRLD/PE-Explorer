#include "gui.h"

void gui::render_ui()
{
    ImGui::Begin("PE Inspector", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
    {
        this->render_menubar();


        this->render_pagebar();
        {
            
         
        }
    }

    ImGui::End();
}

void gui::render_menubar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open...", "Ctrl+O")) open_file();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void gui::render_pagebar()
{


    ImGui::ShowDemoWindow();
    if (ImGui::BeginTabBar("###pagebar"))
    {
        if (ImGui::BeginTabItem("Overview"))
        {
            page = 0;
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Test"))
        {
            page = 1;



            ImGui::EndTabItem();
        }
    }
    ImGui::EndTabBar();
}

void gui::open_file()
{
    auto hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);

    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* file_open;

        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&file_open));

        if (SUCCEEDED(hr))
        {
            hr = file_open->Show(NULL);

            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = file_open->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR file_path;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &file_path);

                    if (SUCCEEDED(hr))
                    {
                        std::wstring w_p(file_path);
                        std::string s_p = std::string(w_p.begin(), w_p.end());

                        //mem->load_binary(s_p);

                        CoTaskMemFree(file_path);
                    }
                    pItem->Release();
                }
            }
            file_open->Release();
        }
        CoUninitialize();
    }
}
