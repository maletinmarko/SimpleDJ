/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-11 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

extern Display* display;

//==============================================================================
class SystemTrayIconComponent::Pimpl
{
public:
    Pimpl (const Image& image_, Window windowH)
        : image (image_)
    {
        ScopedXLock xlock;

        Screen* const screen = XDefaultScreenOfDisplay (display);
        const int screenNumber = XScreenNumberOfScreen (screen);

        String screenAtom ("_NET_SYSTEM_TRAY_S");
        screenAtom << screenNumber;
        Atom selectionAtom = XInternAtom (display, screenAtom.toUTF8(), false);

        XGrabServer (display);
        Window managerWin = XGetSelectionOwner (display, selectionAtom);

        if (managerWin != None)
            XSelectInput (display, managerWin, StructureNotifyMask);

        XUngrabServer (display);
        XFlush (display);

        if (managerWin != None)
        {
            XEvent ev = { 0 };
            ev.xclient.type = ClientMessage;
            ev.xclient.window = managerWin;
            ev.xclient.message_type = XInternAtom (display, "_NET_SYSTEM_TRAY_OPCODE", False);
            ev.xclient.format = 32;
            ev.xclient.data.l[0] = CurrentTime;
            ev.xclient.data.l[1] = 0 /*SYSTEM_TRAY_REQUEST_DOCK*/;
            ev.xclient.data.l[2] = windowH;
            ev.xclient.data.l[3] = 0;
            ev.xclient.data.l[4] = 0;

            XSendEvent (display, managerWin, False, NoEventMask, &ev);
            XSync (display, False);
        }

        // For older KDE's ...
        long atomData = 1;
        Atom trayAtom = XInternAtom (display, "KWM_DOCKWINDOW", false);
        XChangeProperty (display, windowH, trayAtom, trayAtom, 32, PropModeReplace, (unsigned char*) &atomData, 1);

        // For more recent KDE's...
        trayAtom = XInternAtom (display, "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", false);
        XChangeProperty (display, windowH, trayAtom, XA_WINDOW, 32, PropModeReplace, (unsigned char*) &windowH, 1);

        // A minimum size must be specified for GNOME and Xfce, otherwise the icon is displayed with a width of 1
        XSizeHints* hints = XAllocSizeHints();
        hints->flags = PMinSize;
        hints->min_width = 22;
        hints->min_height = 22;
        XSetWMNormalHints (display, windowH, hints);
        XFree (hints);
    }

    Image image;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl);
};


//==============================================================================
void SystemTrayIconComponent::setIconImage (const Image& newImage)
{
    pimpl = nullptr;

    if (newImage.isValid())
    {
        if (! isOnDesktop())
            addToDesktop (0);

        pimpl = new Pimpl (newImage, (Window) getWindowHandle());

        setVisible (true);
        toFront (false);
    }

    repaint();
}

void SystemTrayIconComponent::paint (Graphics& g)
{
    if (pimpl != nullptr)
        g.drawImageWithin (pimpl->image, 0, 0, getWidth(), getHeight(),
                           RectanglePlacement::xLeft | RectanglePlacement::yTop | RectanglePlacement::onlyReduceInSize, false);
}

void SystemTrayIconComponent::setIconTooltip (const String& /* tooltip */)
{
    // xxx not yet implemented!
}
