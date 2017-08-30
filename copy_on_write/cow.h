/**
 * Copyright (c) 2016, Alexandre Beaulieu
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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

