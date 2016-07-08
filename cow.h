
#ifndef _COW_H
#define _COW_H

#include <vector>
#include <memory>

template <class T>
class cow_vector_read {
public:
    cow_vector_read(const std::shared_ptr<std::vector<T> >& v) : m_v(v) { }

    const std::vector<T>& operator->() const {
        return *m_v;
    }

    const std::vector<T>& operator*() const {
        return *m_v;
    }

private:
    std::shared_ptr<std::vector<T> > m_v;
};

template <class T>
class cow_vector {
public:
    cow_vector(const cow_vector<T>& v) : m_v(std::make_shared<std::vector<T> >(v.m_v)) { }

    void operator=(const cow_vector<T>& v) {
        m_v = std::make_shared<std::vector<T> >(v.m_v);
    }

    cow_vector() : m_v(std::make_shared<std::vector<T> >()) { }
    explicit cow_vector(const std::vector<T>& v) : m_v(std::make_shared<std::vector<T> >(v)) { }
    void operator=(const std::vector<T>& v) {
        m_v = std::make_shared<std::vector<T> >(v);
    }

    cow_vector_read<T> for_reading() const {
        return cow_vector_read<T>(m_v);
    }

    void push_back(const T& t) {
        m_v = std::make_shared<std::vector<T> >(*m_v);
        m_v->push_back(t);
    }

    bool erase(const T& t) {
        typename std::vector<T>::iterator it =
            std::find(m_v->begin(), m_v->end(), t);
        if (it != m_v->end()) {
            m_v = std::make_shared<std::vector<T> >(*m_v);
            m_v->erase(it);
            return true;
        }
        return false;
    }

    void erase_at(std::size_t index) {
        m_v = std::make_shared<std::vector<T> >(*m_v);
        m_v->erase(m_v->begin() + index);
    }

private:
    std::shared_ptr<std::vector<T> > m_v;
};

#endif

