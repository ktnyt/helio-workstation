/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

//[Headers]
#include "Serializable.h"
//[/Headers]


class ThemeSettings  : public Component,
                       public ListBoxModel,
                       private ChangeListener
{
public:

    ThemeSettings ();

    ~ThemeSettings();

    //[UserMethods]

    //===----------------------------------------------------------------------===//
    // Serializable
    //

    virtual XmlElement *serialize() const;

    virtual void deserialize(const XmlElement &xml);

    virtual void reset();

    //===----------------------------------------------------------------------===//
    // ListBoxModel
    //

    Component *refreshComponentForRow(int, bool, Component*) override;

    int getNumRows() override;

    void paintListBoxItem(int rowNumber, Graphics &g,
        int width, int height,
        bool rowIsSelected) override {}

    void listBoxItemClicked(int rowNumber, const MouseEvent &e) override {}

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]

    //===----------------------------------------------------------------------===//
    // ChangeListener
    //

    void changeListenerCallback(ChangeBroadcaster *source) override;

    //[/UserVariables]

    ScopedPointer<ListBox> themesList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThemeSettings)
};
