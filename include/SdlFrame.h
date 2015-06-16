/*  Chip8++ - A CHIP-8 emulator.
    Copyright (C) 2015  Robin Chavignat

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef SDLFRAME_H
#define SDLFRAME_H

#include <wx/wx.h>

class SdlFrame : public wxFrame
{
    public:
        enum menuIds {ID_Hello};
        SdlFrame();
        virtual ~SdlFrame();

        void OnHello(wxCommandEvent& event);
        void OnExit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);

        wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(SdlFrame, wxFrame)
    EVT_MENU(ID_Hello,   SdlFrame::OnHello)
    EVT_MENU(wxID_EXIT,  SdlFrame::OnExit)
    EVT_MENU(wxID_ABOUT, SdlFrame::OnAbout)
wxEND_EVENT_TABLE()

#endif // SDLFRAME_H
