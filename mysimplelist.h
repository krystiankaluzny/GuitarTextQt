#ifndef MYSIMPLELIST_H
#define MYSIMPLELIST_H
#include <climits> // to use max unsigned macro LLONG_MAX
#include <initializer_list>
#include <vector>
#include <cassert>
#include <iostream>

#define Max(a, b) (a > b ? a : b)
#define Min(a, b) (a > b ? b : a)

template <typename T> class MySimpleList
{
private:
    struct Obj
    {
        T data;
        Obj* previous = nullptr;
        Obj* next = nullptr;
    };
public:
    class iterator
    {
    public:
        inline iterator() : actual(nullptr) {}
        inline iterator(Obj* ob) : actual(ob) {}
        inline iterator(const iterator& iter) : actual(iter.actual) {}
        inline iterator& operator =(Obj* item) { actual = item; return *this; }
        inline iterator& operator =(const iterator& iter) { actual = iter.actual; return *this;}
        inline bool operator =(T data) { return actual->data = data; }
        inline bool operator ==(const iterator& iter) const { return actual == iter.actual; }
        inline bool operator !=(const iterator& iter) const { return actual != iter.actual; }
        inline T& operator *() const { return actual->data; }
        inline T* operator ->() const { return &actual->data; }
        inline iterator& operator ++() { //pre inkrementacja
            if(actual != nullptr)
                actual = actual->next;
            return *this;
        }
        inline iterator operator ++(int) { //post inkrementacja
            if(actual != nullptr)
            {
                Obj* t = actual;
                actual = actual->next;
                return t;
            }
            return *this;
        }
        inline iterator& operator --() { //pre dekrementacja
            if(actual != nullptr)
                actual = actual->previous;
            return *this;
        }
        inline iterator operator --(int) { //post dekrementacja
            if(actual != nullptr)
            {
                Obj* t = actual;
                actual = actual->previous;
                return t;
            }
            return *this;
        }

    private:
        Obj* actual;

        friend class MySimpleList;
    };

    friend class iterator;

public:
    MySimpleList();
    MySimpleList(std::initializer_list<T> list); //kostruktory z listami inicjalizującymi
    MySimpleList(unsigned max, std::initializer_list<T> list);
    MySimpleList(MySimpleList& simple_list); //konstruktor kopiujący
    MySimpleList(MySimpleList&& simple_list); //konstruktor przenoszący
    ~MySimpleList();

    void setMaxSize(unsigned max);
    unsigned getMaxSize() const;
    unsigned getSize() const;
    void addAtEnd(const T& item);
    void addAtFront(const T& item);
    const T& at(unsigned index) const;
    T& operator[] (unsigned index);
    T value(unsigned index) const;
    bool swap(unsigned index1, unsigned index2);
    bool moveOnTop(T& o);
    bool moveOnTop(iterator& iter);
    bool moveOnBottom(T& o);
    bool moveOnBottom(iterator& iter);
    bool remove(unsigned index);
    bool remove(iterator& iter);
    void clear();

    iterator first() { return m_first != nullptr ? iterator(m_first) : iterator(m_end); }
    iterator last() { return m_last != nullptr ? iterator(m_last) : iterator(m_start); }
    iterator start() { return iterator(m_start); }
    iterator end() { return iterator(m_end); }
    iterator find(T& o) {
        iterator iter = first();
        for(;iter != end(); iter++) if(iter.actual->data == o) return iter;
        return start();
    }

private:
    unsigned m_max_size;
    unsigned m_size;

    Obj* m_first;
    Obj* m_last;
    Obj* m_start; //element przed pierwszym, nie zawiera żadnych danych
    Obj* m_end; //element za ostatnim, nie zawiera żadnych danych
};

template <typename T> MySimpleList<T>::MySimpleList()
    : m_max_size(LLONG_MAX),
      m_size(0),
      m_first(nullptr),
      m_last(nullptr),
      m_start(new Obj),
      m_end(new Obj)
{
    m_start->next = m_end;
    m_end->previous = m_start;
}

template <typename T> MySimpleList<T>::MySimpleList(std::initializer_list<T> list)
    : m_max_size(LLONG_MAX),
      m_size(0),
      m_first(nullptr),
      m_last(nullptr),
      m_start(new Obj),
      m_end(new Obj)
{
    m_start->next = m_end;
    m_end->previous = m_start;

    for(T item : list)
        addAtEnd(item);
    m_last->next = m_end;
}

template <typename T> MySimpleList<T>::MySimpleList(unsigned max, std::initializer_list<T> list)
    : m_max_size(max),
      m_size(0),
      m_first(nullptr),
      m_last(nullptr),
      m_start(new Obj),
      m_end(new Obj)
{
    m_start->next = m_end;
    m_end->previous = m_start;

    for(T item : list)
        addAtEnd(item);
    m_last->next = m_end;
}

//konstruktor kopiujący
template <typename T> MySimpleList<T>::MySimpleList(MySimpleList& simple_list)
    : m_size(0),
      m_first(nullptr),
      m_last(nullptr),
      m_start(new Obj),
      m_end(new Obj)
{
    m_max_size = simple_list.getMaxSize();

    MySimpleList<T>::iterator iter;

    for(iter = simple_list.first(); iter != simple_list.end(); iter++)
    {
        addAtEnd(*iter);
    }
}

//konstruktor przenoszący
template <typename T> MySimpleList<T>::MySimpleList(MySimpleList&& simple_list)
{
    m_max_size = simple_list.m_max_size;

    m_size = simple_list.m_size;
    simple_list.m_size = 0;

    m_start = simple_list.m_start;
    simple_list.m_start = nullptr;

    m_end = simple_list.m_last;
    simple_list.m_end = nullptr;
}

template <typename T> MySimpleList<T>::~MySimpleList()
{
    this->clear();

    delete m_end;
    delete m_start;
}

template <typename T> void MySimpleList<T>::setMaxSize(unsigned max)
{
    if(max > 0)
        m_max_size = max;
}

template <typename T> unsigned MySimpleList<T>::getMaxSize() const
{
    return m_max_size;
}

template <typename T> unsigned MySimpleList<T>::getSize() const
{
    return m_size;
}

template <typename T> void MySimpleList<T>::addAtEnd(const T &item)
{
    if(m_start == nullptr) m_start = new Obj;
    if(m_end == nullptr) m_end = new Obj;

    if(m_size < m_max_size)
    {
        Obj* tmp = new Obj;
        tmp->data = item;

        if(m_first == nullptr)
        {
            tmp->next = m_end;
            tmp->previous = m_start;

            m_start->next = tmp;
            m_end->previous = tmp;

            m_first = tmp;
            m_last = tmp;
        }
        else
        {
            m_last->next = tmp;
            tmp->previous = m_last;
            tmp->next = m_end;

            m_end->previous = tmp;

            m_last = tmp;
        }

        m_size++;
    }
    else if(m_size == m_max_size && m_size >= 1)
    {
        Obj* tmp = new Obj;
        tmp->data = item;

        m_last->next = tmp;
        tmp->previous = m_last;
        tmp->next = m_end;
        m_end->previous = tmp;
        m_last = tmp;

        //usuń pierwszy
        Obj* tmp2 = m_first;

        m_first = m_first->next;
        m_first->previous = m_start;

        m_start->next = m_first;

        delete tmp2;
    }
}

template <typename T> void MySimpleList<T>::addAtFront(const T &item)
{
    if(m_start == nullptr) m_start = new Obj;
    if(m_end == nullptr) m_end = new Obj;

    if(m_size < m_max_size)
    {
        Obj* tmp = new Obj;
        tmp->data = item;

        if(m_first == nullptr)
        {
            tmp->next = m_end;
            tmp->previous = m_start;

            m_start->next = tmp;
            m_end->previous = tmp;

            m_first = tmp;
            m_last = tmp;
        }
        else
        {
            m_first->previous = tmp;
            tmp->previous = m_start;
            tmp->next = m_first;

            m_start->next = tmp;

            m_first = tmp;
        }
        m_size++;
    }
    else if(m_size == m_max_size && m_size >= 1)
    {
        Obj* tmp = new Obj;
        tmp->data = item;

        m_first->previous = tmp;
        tmp->previous = m_start;
        tmp->next = m_first;

        m_start->next = tmp;

        m_first = tmp;

        //usuń ostatni
        Obj* tmp2 = m_last;

        m_last = m_last->previous;
        m_last->next = m_end;

        m_end->previous = m_last;

        delete tmp2;
    }
}

template <typename T> const T &MySimpleList<T>::at(unsigned index) const
{
    assert(index >= 0 && index < m_size);

    if(index < m_size/2)
    {
        Obj* tmp = m_first;

        for(int i = 0; i < index; i++)
        {
            tmp = tmp->next;
        }

        return tmp->data;
    }
    else
    {
        Obj* tmp = m_last;

        for(int i = m_size - 1; i > index; i--)
        {
            tmp = tmp->previous;
        }

        return tmp->data;
    }
}

template <typename T> T &MySimpleList<T>::operator[](unsigned index)
{
    assert(index >= 0 && index < m_size);

    if(index < m_size/2)
    {
        Obj* tmp = m_first;

        for(int i = 0; i < index; i++)
        {
            tmp = tmp->next;
        }

        return tmp->data;
    }
    else
    {
        Obj* tmp = m_last;

        for(int i = m_size - 1; i > index; i--)
        {
            tmp = tmp->previous;
        }

        return tmp->data;
    }
}

template <typename T> T MySimpleList<T>::value(unsigned index) const
{
    assert(index >= 0 && index < m_size);

    if(index < m_size/2)
    {
        Obj* tmp = m_first;

        for(int i = 0; i < index; i++)
        {
            tmp = tmp->next;
        }

        return tmp->data;
    }
    else
    {
        Obj* tmp = m_last;

        for(int i = m_size - 1; i > index; i--)
        {
            tmp = tmp->previous;
        }

        return tmp->data;
    }
}

template <typename T> bool MySimpleList<T>::swap(unsigned index1, unsigned index2)
{
    if(index1 == index2) return false;

    unsigned max = Max(index1, index2);
    unsigned min = Min(index1, index2);

    if(max >= m_size || min < 0) return false;

    Obj* ob1 = m_first;
    Obj* ob2 = m_first;
    Obj* tmp = m_first;

    for(int i = 0; i < max; i++)
    {
        if(i == min)
            ob1 = tmp;

        tmp = tmp->next;
    }
    ob2 = tmp;

    if(ob1 == m_first)
        m_first = ob2;

    if(ob2 == m_last)
        m_last = ob1;

    if(ob1->next == ob2) //dwa kolejne
    {
        tmp = ob1->previous;
        ob1->previous = ob2;
        ob1->next = ob2->next;

        ob2->previous = tmp;
        ob2->next = ob1;
    }
    else
    {
        tmp = ob1->previous;
        ob1->previous = ob2->previous;
        ob2->previous = tmp;

        tmp = ob1->next;
        ob1->next = ob2->next;
        ob2->next = tmp;
    }

    if(ob2->previous != nullptr)
        ob2->previous->next = ob2;
    if(ob2->next != nullptr)
        ob2->next->previous = ob2;

    if(ob1->next != nullptr)
        ob1->next->previous = ob1;
    if(ob1->previous != nullptr)
        ob1->previous->next = ob1;

    return true;
}

template <typename T> bool MySimpleList<T>::moveOnTop(T &o)
{
    iterator iter = find(o);
    if(iter == end() || iter == first()) return false;

    Obj* ob1 = iter.actual;

    ob1->previous->next = ob1->next;
    ob1->next->previous = ob1->previous;

    ob1->previous = m_start;
    ob1->next = m_first;
    m_first->previous = ob1;

    m_first = ob1;

    return true;
}

template <typename T> bool MySimpleList<T>::moveOnTop(MySimpleList::iterator &iter)
{
    if(iter == end() || iter == first() || iter.actual == nullptr) return false;

    Obj* ob1 = iter.actual;

    ob1->previous->next = ob1->next;
    ob1->next->previous = ob1->previous;

    ob1->previous = m_start;
    ob1->next = m_first;
    m_first->previous = ob1;

    m_first = ob1;

    return true;
}

template <typename T> bool MySimpleList<T>::moveOnBottom(T &o)
{
    iterator iter = find(o);
    if(iter == end() ) return false;

    Obj* ob1 = iter.actual;

    ob1->previous->next = ob1->next;
    ob1->next->previous = ob1->previous;

    ob1->next = m_end;
    ob1->previous = m_last;
    m_last->next = ob1;

    m_last = ob1;

    return true;
}

template <typename T> bool MySimpleList<T>::moveOnBottom(MySimpleList::iterator &iter)
{
    if(iter == end() || iter.actual == nullptr) return false;

    Obj* ob1 = iter.actual;

    ob1->previous->next = ob1->next;
    ob1->next->previous = ob1->previous;

    ob1->next = m_end;
    ob1->previous = m_last;
    m_last->next = ob1;

    m_last = ob1;

    return true;
}

template <typename T> bool MySimpleList<T>::remove(unsigned index)
{
    if(index >= m_size) return false;

    Obj* tmp = m_first;

    for(int i = 0; i < index; i++)
        tmp = tmp->next;

    if(tmp == nullptr || tmp == m_start || tmp == m_end) return false;

    if(tmp == m_first)
    {
        if(m_size == 1)
        {
            m_first = m_last = nullptr;
            m_start->next = m_end;
            m_end->previous = m_start;
        }
        else
        {
            m_first = m_first->next;
            m_first->previous = m_start;
        }
	
        delete tmp;
        m_size--;
    }
    else if(tmp == m_last)
    {
        m_last = m_last->previous;
        m_last->next = m_end;

        delete tmp;
        m_size--;
    }
    else
    {
        tmp->previous->next = tmp->next;
        tmp->next->previous = tmp->previous;

        delete tmp;
        m_size--;
    }

    return true;
}

template <typename T> bool MySimpleList<T>::remove(MySimpleList::iterator &iter)
{
    if(iter == start() || iter == end()) return false;

    Obj* tmp = iter.actual;
    iter.actual = tmp->previous;

    if(tmp == nullptr || tmp == m_start || tmp == m_end) return false;

    if(tmp == m_first)
    {
        if(m_size == 1)
            m_first = m_last = nullptr;
        else
        {
            m_first = m_first->next;
            m_first->previous = m_start;
            m_start->next = m_first;
        }

        delete tmp;
        m_size--;
    }
    else if(tmp == m_last)
    {
        m_last = m_last->previous;
        m_last->next = m_end;
        m_end->previous = m_last;

        delete tmp;
        m_size--;
    }
    else
    {
        tmp->previous->next = tmp->next;
        tmp->next->previous = tmp->previous;

        delete tmp;
        m_size--;
    }

    return true;
}

template <typename T> void MySimpleList<T>::clear()
{
    while(remove(0));
}

#endif // MYSIMPLELIST_H

#ifndef LISTSORTBYWEIGHT_H
#define LISTSORTBYWEIGHT_H

template <typename T> class ListSortByWeight
{
private:
    struct ObjW
    {
        T data;
        ObjW* previous = nullptr;
        ObjW* next = nullptr;
        int weight = 0;
    };

public:
    class iterator
    {
    public:
        inline iterator() : actual(nullptr) {}
        inline iterator(ObjW* ob) : actual(ob) {}
        inline iterator(const iterator& iter) : actual(iter.actual) {}
        inline int weight() { return actual->weight; }
        inline void operator =(ObjW* item) { actual = item;}
        inline void operator =(const iterator& iter) { actual = iter.actual; }
        inline bool operator =(T data) { return actual->data = data; }
        inline bool operator ==(const iterator& iter) const { return actual == iter.actual; }
        inline bool operator !=(const iterator& iter) const { return actual != iter.actual; }
        inline T& operator *() const { return actual->data; }
        inline T* operator ->() const { return &actual->data; }
        inline iterator& operator ++() { //pre inkrementacja
            if(actual != nullptr)
                actual = actual->next;
            return *this;
        }
        inline iterator operator ++(int) { //post inkrementacja
            if(actual != nullptr)
            {
                ObjW* t = actual;
                actual = actual->next;
                return t;
            }
            return *this;
        }
        inline iterator& operator --() { //pre dekrementacja
            if(actual != nullptr)
                actual = actual->previous;
            return *this;
        }
        inline iterator operator --(int) { //post dekrementacja
            if(actual != nullptr)
            {
                ObjW* t = actual;
                actual = actual->previous;
                return t;
            }
            return *this;
        }

    private:
        ObjW* actual;

        friend class ListSortByWeight;
    };

    friend class iteretor;

public:
    ListSortByWeight();
    ~ListSortByWeight();

    void add(const T& item, int weight);
    bool incrementWeight(const T& item);
    void clear();
    bool remove(iterator& iter);

    iterator first() { return m_first != nullptr ? iterator(m_first) : iterator(m_end); }
    iterator last() { return m_last != nullptr ? iterator(m_last) : iterator(m_start); }
    iterator start() { return iterator(m_start); }
    iterator end() { return iterator(m_end); }
    iterator find(T& o) {
        iterator iter = first();
        for(;iter != end(); iter++) if(iter.actual->data == o) return iter;
        return start();
    }

private:
    void sortObjW(ObjW* o);
    void swap(ObjW* o1, ObjW* o2);
    bool removeFirst();

    ObjW* m_first;
    ObjW* m_last;
    ObjW* m_start; //element przed pierwszym, nie zawiera żadnych danych
    ObjW* m_end; //element za ostatnim, nie zawiera żadnych danych
};

template <typename T> ListSortByWeight<T>::ListSortByWeight()
    : m_first(nullptr),
      m_last(nullptr),
      m_start(new ObjW),
      m_end(new ObjW)
{
    m_start->next = m_end;
    m_end->previous = m_start;
}
template <typename T> ListSortByWeight<T>::~ListSortByWeight()
{
    this->clear();

    delete m_start;
    delete m_end;
}

template <typename T> void ListSortByWeight<T>::add(const T &item, int weight)
{
    if(!incrementWeight(item)) // jeżeli nie znaleziono takiego obiektu to dodajemy go
    {
        if(m_start == nullptr) m_start = new ObjW;
        if(m_end == nullptr) m_end = new ObjW;

        ObjW* tmp = new ObjW;
        tmp->data = item;
        tmp->weight = weight;

        //dodaj na koniec
        if(m_first == nullptr)
        {
            tmp->next = m_end;
            tmp->previous = m_start;

            m_start->next = tmp;
            m_end->previous = tmp;

            m_first = tmp;
            m_last = tmp;
        }
        else
        {
            m_last->next = tmp;
            tmp->previous = m_last;
            tmp->next = m_end;

            m_end->previous = tmp;

            m_last = tmp;
        }

        sortObjW(tmp);
    }
}

template <typename T> bool ListSortByWeight<T>::incrementWeight(const T &item)
{
    if(m_first == nullptr) return false;

    ObjW* tmp = m_first;
    while(tmp != m_end)
    {
        if(tmp->data == item)
        {
            tmp->weight++;
            sortObjW(tmp);
            return true;
        }
        tmp = tmp->next;
    }
    return false;
}

template <typename T> void ListSortByWeight<T>::clear()
{
    while(removeFirst());
}

template <typename T> bool ListSortByWeight<T>::remove(ListSortByWeight::iterator &iter)
{
    if(iter == start() || iter == end()) return false;

    ObjW* tmp = iter.actual;
    iter.actual = tmp->previous;

    if(tmp == nullptr || tmp == m_start || tmp == m_end) return false;

    if(tmp == m_first)
    {
        if(m_first->next == m_end)
            m_first = m_last = nullptr;
        else
        {
            m_first = m_first->next;
            m_first->previous = m_start;
            m_start->next = m_first;
        }

        delete tmp;
    }
    else if(tmp == m_last)
    {
        m_last = m_last->previous;
        m_last->next = m_end;
        m_end->previous = m_last;

        delete tmp;
    }
    else
    {
        tmp->previous->next = tmp->next;
        tmp->next->previous = tmp->previous;

        delete tmp;
    }

    return true;
}

template <typename T> void ListSortByWeight<T>::sortObjW(ListSortByWeight::ObjW *o)
{
    if(o == nullptr) return;
    if(o == m_start || o == m_end) return;

    //sortowanie malająco
    while((o->previous != m_start) && (o->weight > o->previous->weight))
    {
        swap(o, o->previous);
    }
    while((o->previous != m_end) && (o->weight < o->next->weight))
    {
        swap(o, o->next);
    }
}

template <typename T> void ListSortByWeight<T>::swap(ListSortByWeight::ObjW *o1, ListSortByWeight::ObjW *o2)
{
    if(o1 == o2) return;
    //sprawdzenie czy nie zamieniamy elementu listy z m_start lub m_end albo jakimś pustym obiektem
    if(o1->previous == nullptr || o1->next == nullptr || o2->previous == nullptr || o2->next == nullptr) return;

    ObjW* tmp;
    if(o1->next == o2) //dwa kolejne
    {
        if(o1 == m_first) m_first = o2;
        if(o2 == m_last) m_last = o1;

        tmp = o1->previous;
        o1->previous = o2;
        o1->next = o2->next;
        o2->previous = tmp;
        o2->next = o1;
    }
    else if(o2->next == o1)
    {
        if(o2 == m_first) m_first = o1;
        if(o1 == m_last) m_last = o2;

        tmp = o2->previous;
        o2->previous = o1;
        o2->next = o1->next;
        o1->previous = tmp;
        o1->next = o2;
    }
    else
    {
        if(o1 == m_first) m_first = o2;
        if(o1 == m_last) m_last = o2;
        if(o2 == m_first) m_first = o1;
        if(o2 == m_last) m_last = o1;

        tmp = o1->previous;
        o1->previous = o2->previous;
        o2->previous = tmp;

        tmp = o1->next;
        o1->next = o2->next;
        o2->next = tmp;
    }

    if(o2->previous != nullptr)
        o2->previous->next = o2;
    if(o2->next != nullptr)
        o2->next->previous = o2;

    if(o1->next != nullptr)
        o1->next->previous = o1;
    if(o1->previous != nullptr)
        o1->previous->next = o1;
}

template <typename T> bool ListSortByWeight<T>::removeFirst()
{
    if(m_first == nullptr) return false;
    ObjW* tmp = m_first;

    if(m_first->next == m_end)
    {
        m_first = m_last = nullptr;
        m_start->next = m_end;
        m_end->previous = m_start;
    }
    else
    {
        m_first = m_first->next;
        m_first->previous = m_start;
    }

    delete tmp;
    return true;
}
#endif // LISTSORTBYWEIGHT_H
