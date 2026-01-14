#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include "armv4vm_p.hpp"

namespace armv4vm {

class MemoryProtected;



struct VmProperties {

    struct MemModel {

        enum Type {
            DIRECT,
            PROTECTED,
        };

        MemModel(Type type = DIRECT) : m_type(type) { m_accessRanges.clear(); }

        Type                        m_type;
        std::vector<armv4vm::AccessRange> m_accessRanges;
    };

    VmProperties() {
        m_bin     = "";
        m_memsize = 0;
        m_debug   = 0;
        m_memModel = MemModel::DIRECT;
        m_coproModel = "assert";
    }
    ~VmProperties() { }

    VmProperties(const VmProperties &other) {

        m_bin = other.m_bin;
        m_memsize = other.m_memsize;
        m_debug = other.m_debug;
        m_memModel = other.m_memModel;
        m_coproModel = other.m_coproModel;
    }

    VmProperties operator=(const VmProperties &other) {

        m_bin      = other.m_bin;
        m_memsize  = other.m_memsize;
        m_debug    = other.m_debug;
        m_memModel = other.m_memModel;
        m_coproModel = other.m_coproModel;

        return *this;
    }

    std::string m_bin;
    uint32_t m_memsize;
    bool m_debug;
    MemModel m_memModel;
    std::string m_coproModel;

    void clear() {

        m_bin     = "";
        m_memsize = 0;
        m_debug   = 0;
    }
};

} // namespace armv4vm
