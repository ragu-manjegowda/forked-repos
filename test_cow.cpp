#include <iostream>
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

int main(int argc, const char* argv[]) {
    int sum = 0;
    cow_vector<int> v( std::vector<int>({ 1, 2, 3}) );

    for (int i = 0; i < 1000000; i++) {
        cow_vector_read<int> v2 = v.for_reading();
        for (const int& num : *v2) {
            sum += num;
        }
    }
    std::cout << sum << "\n";
    return 0;
}
