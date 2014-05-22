#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <vector>

#include <QtGui>
#include <QString>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QBrush>

using namespace std;

class Event {
    public:
        int id;
        string name;
        double endtime;
        Event() { id = -1; name = ""; endtime = 0; }
        Event(const int i, const string n, const double et) {
            id = i; name = n; endtime = et;
        }
};

class EventList {
    private:
        vector<int>    eventId;
        vector<string> eventName;
        vector<double> eventEndTime;
        vector<string> eventUnique;
        int uniqueNum;
        int eventNum;
    public:
        EventList() { eventNum = uniqueNum = 0; }
        ~EventList() {}

        int loadFromFile(const char* filename);
        void print();

        int getUniqueNum() { return uniqueNum; }
        int getEventNum() { return eventNum; }
        double getDuration() { return eventEndTime[eventNum-1]; }

        string getUnique(int k) {
            return (k < uniqueNum) ?
                eventUnique[k] : "";
        }

        Event operator[] (int i) {
            return (i < eventNum) ?
                Event(eventId[i], eventName[i], eventEndTime[i]) : Event();
        }
};

int EventList::loadFromFile(const char* filename) {
    if (uniqueNum)
    {
        eventName.clear();
        eventEndTime.clear();
    }

    uniqueNum = 0;
    eventNum = 0;
    ifstream ifs(filename);
    if (!ifs.good()) {
        cerr << "could not open " << filename << endl;
        return 1;
    }

    double ct = 0.;
    while (!ifs.eof())
    {
        int id;
        string str;
        double tim;
        ifs >> str >> tim;
        if (!str.size()) continue;

        ct += tim;

        eventNum++;
        vector<string>::iterator it;
        it = find(eventName.begin(), eventName.end(), str);
        if (it == eventName.end())
        {
            id = uniqueNum++;
            eventUnique.push_back(str);
        } else
        {
            id = distance(eventName.begin(), it);
        }

        eventId.push_back(id);
        eventName.push_back(str);
        eventEndTime.push_back(ct);
    }
    ifs.close();

    return 0;
}

void EventList::print() {
    int sz = eventName.size();
    cout << "different events: " << uniqueNum << endl;

    for (int i = 0; i < sz; ++i)
        cout << eventName[i] << "\t" << eventEndTime[i] << endl;

    cout << "total events: " << sz << endl;
}

static QColor colors[] = {
    Qt::black,
    Qt::red,
    Qt::darkRed,
    Qt::green,
    Qt::darkGreen,
    Qt::blue,
    Qt::darkBlue,
    Qt::cyan,
    Qt::darkCyan,
    Qt::magenta,
    Qt::darkMagenta,
    Qt::yellow,
    Qt::darkYellow,
    Qt::gray,
    Qt::darkGray,
    Qt::lightGray,

    Qt::black,
    Qt::red,
    Qt::darkRed,
    Qt::green,
    Qt::darkGreen,
    Qt::blue,
    Qt::darkBlue,
    Qt::cyan,
    Qt::darkCyan,
    Qt::magenta
};

int main(int argc, char **argv) {
    if (argc <= 2) {
        cout << "usage: " << string(argv[0])
            << " trace_file [output_pic] [precision]" << endl;
        return 2;
    }
    const char *output = (argc > 2) ? argv[2] : "output.png";
    int precision = (argc > 3) ? atoi(argv[3]) : 3;

    EventList events;
    if (events.loadFromFile(argv[1])) return 2;

    int uniqueNum = events.getUniqueNum();
    int eventNum = events.getEventNum();
    double duration = events.getDuration();

    int loffset = 150;
    int roffset = 20;
    int boffset = 40;
    int hoffset = 50;

    int fs = 32;

    int width   = 1024;
    int height  = hoffset + 32 * uniqueNum + boffset;

    int wh      = height - boffset;
    double wl   = (width - loffset - roffset)/duration;

    QApplication app(argc, argv);

    QImage img(width, height, QImage::Format_ARGB32_Premultiplied);
    QPainter painter;
    painter.begin(&img);
    painter.fillRect(0, 0, width, height, QBrush(Qt::white, Qt::SolidPattern));

    painter.setFont(QFont("Arial", 18));
    painter.drawText(img.rect(), Qt::AlignHCenter | Qt::AlignTop, QString(argv[1]));

    painter.setPen(QPen(Qt::black, 2));

    painter.drawLine(0, wh, width, wh);
    painter.drawLine(loffset, 0, loffset, height);

    for (int i = 0; i < uniqueNum; ++i)
        painter.drawText(
                QRect(
                    0,           wh - (i+1) * fs,
                    loffset - 8, fs),
                Qt::AlignVCenter | Qt::AlignRight,
                QString::fromStdString(events.getUnique(i)));

    painter.setPen(QPen(Qt::black, 3));
    double stime = 0;
    double etime = 0;
    double lastmark = 0;
    for (int i = 0; i < eventNum; ++i)
    {
        Event e = events[i];
        etime = e.endtime;

        int mh = wh - (e.id + 1) * fs;

        if ((etime - lastmark)*wl > fs*(precision-1))
        {
            if (((duration - etime)*wl > 2 )  &&
                ((duration - etime)*wl < fs*(precision-1))) continue;
            painter.drawLine(
                    loffset + etime * wl - 1, wh + 1,
                    loffset + etime * wl - 1, wh + 3);
            painter.drawText(
                    QRect(
                        loffset + lastmark * wl, wh,
                        (etime - lastmark) * wl, fs),
                    Qt::AlignVCenter | Qt::AlignRight,
                    QString::number(e.endtime, 'g', precision));
            lastmark = etime;
        }

        painter.fillRect(
                QRect(
                    loffset + stime * wl, mh,
                    (etime - stime) * wl, fs),
                QBrush(colors[tolower(e.name[0])-'a'] , Qt::SolidPattern));

        stime = etime;
    }

    painter.end();
    img.save(output, "PNG");

    return 0;
}
