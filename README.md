# Bottle_Detection

## Allgemein
Dieses Projekt dient zur Erkennung, ob der Rand einer Glasflasche defekt oder in Ordnung ist.
Zusätzlich zur gut/schlecht Auswertung können weitere Parameter angezeigt werden.
Mit Hilfe von Trackbars lässt sich die Auswertung in einem bestimmten Bereich einstellen.
Eine Testsequenz kann auswerten, ob die GUI Elemente und die Funktionen für die Auswertung richtig funktionieren.

## Hardware
Es wurde ein Raspberry Pi 5 und eine Razer Kiyo verwendet.
Generell funktioniert das Programm auf jedem Linux-System.
Auch können andere Webcams gewählt werden.
Hierbei ist darauf zu achten, dass für die Detektion eine geeignete Beleuchtung vorhanden sein muss.

## Bibliotheken
Für dieses Projekt wird ein Qt GUI und OpenCV verwendet.

## Build Instructions
Qt kann mit folgendem Befehl installiert werden:

`sudo apt-get install qtbase5-dev`

Wir benötigen einen C++-Compiler zum Kompilieren von OpenCV, Git zum Klonen der Quellen aus offiziellen Repositories und CMake sowie Make zum Ausführen des Build-Systems und einiger anderer Abhängigkeiten. 
Geben Sie den folgenden Befehl ein, um alle diese zu erhalten.

`sudo apt install -y g++ cmake make git libgtk2.0-dev pkg-config`

Danach kann das Repository von OpenCV geklont werden.

`git clone https://github.com/opencv/opencv.git
git -C opencv checkout 4.x`

Erstelle ein Build Directory

`mkdir -p build && cd build`

Erstelle Build Scripts mit Support für Qt

`cmake -DWITH_QT=ON ../opencv`

Kompilieren

`make -j4`

Installieren

`sudo make install`

Weitere Hilfe und Alternativen gibt es auf der offiziellen Webseite von OpenCV.

https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html
