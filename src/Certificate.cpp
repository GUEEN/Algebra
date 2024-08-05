#include <algorithm>

#include "Certificate.h"

Certificate::Certificate(): size_(0), data_(nullptr) {
}

Certificate::Certificate(size_t m): size_(m) {
    data_ = new uint8_t[m];
}

Certificate::~Certificate() {
    delete[] data_;
}

Certificate::Certificate(const Certificate& cert) : size_(cert.size_) {
    data_ = new uint8_t[size_];
    for (size_t i = 0; i < size_; ++i) {
        data_[i] = cert.data_[i];
    }
}

Certificate::Certificate(Certificate&& cert) : size_(cert.size_), data_(cert.data_) {
    cert.size_ = 0;
    cert.data_ = nullptr;
}

Certificate& Certificate::operator=(const Certificate& cert) {
    if (cert.data_ == data_) {
        return *this;
    }
    if (size_ != cert.size_) {
        delete[] data_;
        size_ = cert.size_;
        data_ = new uint8_t[size_];
    }
    for (size_t i = 0; i < size_; ++i) {
        data_[i] = cert.data_[i];
    }
    return *this;
}

Certificate& Certificate::operator=(Certificate&& cert) {
    if (cert.data_ == data_) {
        return *this;
    }
    delete[] data_;
    size_ = cert.size_;
    data_ = cert.data_;
    cert.size_ = 0;
    cert.data_ = nullptr;
    return *this;
}

uint8_t& Certificate::operator[](size_t i) {
    return data_[i];
}

const uint8_t& Certificate::operator[](size_t i) const {
    return data_[i];
}

size_t Certificate::size() const {
    return size_;
}

uint8_t* Certificate::data() const {
    return data_;
}

size_t std::hash<Certificate>::operator()(const Certificate& cert) const {
    static const size_t p = 1000000009;
    size_t h = 0;
    for (size_t i = 0; i < cert.size(); ++i) {
        h = h * p + cert[i];
    }
    return h;
}


std::string CertToString(const Certificate& cert) {
    std::string s;
    for (int i = 0; i < cert.size(); i++) {
        s.push_back(cert[i]);
    }
    return s;
}

Certificate Cert(const std::string& s) {
    size_t n = s.length();
    Certificate cert(n);
    for (size_t i = 0; i < n; i++) {
        cert[i] = s[i];
    }
    return cert;
}

int compareCertificates(const Certificate& C, const Certificate& D) {
    if (C.size() > D.size()) {
        return -1;
    }
    if (C.size() < D.size()) {
        return 1;
    }
    size_t l = C.size();
    size_t i = 0;

    while (i < l && C[i] == D[i]) {
        i++;
    }

    if (i >= l) {
        return 0;
    }

    if (C[i] > D[i]) {
        return -1;
    } else {
        return 1;
    }
}

bool operator==(const Certificate& C, const Certificate& D) {
    return compareCertificates(C, D) == 0;
}
