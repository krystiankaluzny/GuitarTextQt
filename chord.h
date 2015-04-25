#ifndef CHORD_H
#define CHORD_H
#include <QString>
#include <QStringList>


class Chord
{
public:
    explicit Chord();
    explicit Chord(QString ch, int sh = 0);

    int ChordIndex() const;

    void SetShift(int s);
    void SetChordName(QString chord);
    int Shift() const;
    QString ChordName() const;

    QString ShiftChord() const;

private:

    void ChordsInicjalization();
    void FindChordIndex();
    QString m_chord_name;

    QStringList DurChords[12];
    bool m_is_dur;
    int m_chord_index; // jaki to akord
    int m_shift;
};

QStringList ShiftChordsList(QStringList chords, int shift);
QString ShiftChordString(QStringList chords, int shift); //akordy są rozdzielone spacjami

#endif // CHORD_H
