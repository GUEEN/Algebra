#pragma once

#include <cstdint>
#include <string>

class Certificate {
public:
    Certificate();
    explicit Certificate(size_t m);
    Certificate(const Certificate& cert);
    Certificate(Certificate&& cert);
    Certificate& operator=(const Certificate& cert);
    Certificate& operator=(Certificate&& perm);
    ~Certificate();

    uint8_t& operator[](size_t i);
    const uint8_t& operator[](size_t i) const;
    size_t size() const;
    uint8_t* data() const;

private:
    uint16_t size_;
    uint8_t* data_;
};

template<>
struct std::hash<Certificate> {
    size_t operator()(const Certificate& cert) const;
};

std::string CertToString(const Certificate& cert);
Certificate Cert(const std::string& s);

int compareCertificates(const Certificate& C, const Certificate& D);
bool operator==(const Certificate& C, const Certificate& D);
