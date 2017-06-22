#ifndef PREFIX_MAP_HPP_INCLUDED
#define PREFIX_MAP_HPP_INCLUDED


extern "C" {
#include "bgpstream_utils_pfx.h"
#include "libart/art.h"
}

namespace prefix {

#pragma pack(push, 1)
struct ipv4_key_t {
    in_addr addr;
    uint8_t mask;

    ipv4_key_t() {
        addr.s_addr = 0;
        mask = 0;
    }
    
    ipv4_key_t(const unsigned char* c) {
        memcpy((void*)&addr, c, 4);
        memcpy((void*)&mask, &c[4], 1);
    }

    ipv4_key_t(const bgpstream_pfx_storage_t& x) {
        addr = x.address.ipv4;
        mask = x.mask_len;
    }

    bool contains(const ipv4_key_t& other) {

        if (mask > other.mask)
            return false;
        else {
            in_addr tmp;
            tmp = other.addr;
            tmp.s_addr &= htonl(~(((uint64_t)1 << (32 - mask)) - 1));

            return tmp.s_addr == addr.s_addr;
        }
    }
};

inline std::ostream& operator<<(std::ostream& os, const ipv4_key_t& k) {
    char buf[INET_ADDRSTRLEN] = {0};

    inet_ntop(AF_INET, &k.addr, &buf[0], sizeof buf);

    os << buf << "/" << std::to_string(k.mask);

    return os;
}

struct ipv6_key_t {
    in6_addr addr;
    uint8_t mask;

    ipv6_key_t(const bgpstream_pfx_storage_t& x) {
        addr = x.address.ipv6;
        mask = x.mask_len;
    }
};
#pragma pack(pop)

template <typename T>
class split_map {

    // Signature for function to iterate over just the values
    typedef std::function<int(T&)> value_iter_fn;

    struct iter_arg {
        value_iter_fn fn;
        ipv4_key_t pfx;
    };
    
    art_tree ipv4;
    art_tree ipv6;

    T& access(const ipv4_key_t& k) {

        T* res = static_cast<T*>(art_search(&ipv4,
                                            reinterpret_cast<const unsigned char*>(&k),
                                            sizeof(ipv4_key_t)));
        if (!res) {

            res = new T;

            art_insert(&ipv4,
                       reinterpret_cast<const unsigned char*>(&k),
                       sizeof(ipv4_key_t),
                       static_cast<void*>(res));
        }

        return *res;
    }

    T& access(const ipv6_key_t& k) {

        T* res = static_cast<T*>(art_search(&ipv6,
                                            reinterpret_cast<const unsigned char*>(&k),
                                            sizeof(ipv6_key_t)));
        if (!res) {

            res = new T;

            art_insert(&ipv6,
                       reinterpret_cast<const unsigned char*>(&k),
                       sizeof(ipv6_key_t),
                       static_cast<void*>(res));
        }

        return *res;
    }

    std::size_t erase(const ipv4_key_t& k) {

        T* res = static_cast<T*>(art_delete(&ipv4,
                                            reinterpret_cast<const unsigned char*>(&k),
                                            sizeof(ipv4_key_t)));
        if (res)
            return 1;
        else
            return 0;
    }

    std::size_t erase(const ipv6_key_t& k) {

        T* res = static_cast<T*>(art_delete(&ipv6,
                                            reinterpret_cast<const unsigned char*>(&k),
                                            sizeof(ipv6_key_t)));
        if (res)
            return 1;
        else
            return 0;
    }

    static int delete_cb(void*, const unsigned char*, uint32_t, void* value) {
        delete static_cast<T*>(value);
        return 0;
    }

    static int value_iter_cb(void* p_ptr, const unsigned char* key, uint32_t, void* value_ptr) {

        iter_arg* p = (iter_arg*)p_ptr;

        ipv4_key_t outer(key);
        
        if (outer.contains(p->pfx)) {
            std::cout << "value_iter_cb inner: " << p->pfx << "outer: " << outer << std::endl;
            std::cout << "true" << std::endl;

            T* value = static_cast<T*>(value_ptr);

            return p->fn(*value);
        } else {
            return 0;
        }
    }

public:

    split_map() {
        art_tree_init(&ipv4);
        art_tree_init(&ipv6);
    }

    T& operator[] (const bgpstream_pfx_storage_t& k) {

        if (k.address.version == BGPSTREAM_ADDR_VERSION_IPV4) {
            ipv4_key_t in(k);
            return access(in);
        } else {
            ipv6_key_t in(k);
            return access(in);
        }
    }

    std::size_t erase(const bgpstream_pfx_storage_t& k) {

        if (k.address.version == BGPSTREAM_ADDR_VERSION_IPV4) {
            ipv4_key_t in(k);
            return erase(in);
        } else {
            ipv6_key_t in(k);
            return erase(in);
        }
    }

    T* search(const ipv4_key_t& k) {
        return static_cast<T*>(art_search(&ipv4,
                                          reinterpret_cast<const unsigned char*>(&k),
                                          sizeof(ipv4_key_t)));
    }

    int for_each(const value_iter_fn& fn) {

        int res = 0;

        void* fn_ptr = const_cast<void*>(static_cast<const void*>(&fn));

        res = art_iter(&ipv4, value_iter_cb, fn_ptr);

        if (!res)
            res = art_iter(&ipv6, value_iter_cb, fn_ptr);

        return res;
    }

    bool super_prefixes(const ipv4_key_t& k, std::function<int(const ipv4_key_t&, T&)> fn) {

        ipv4_key_t p = k;

        while (p.mask > 0) {
        // for (unsigned i = p.mask; i > 0; i--) {
            p.mask--;
            p.addr.s_addr &= htonl(~(((uint64_t)1 << (32 - p.mask)) - 1));

            if (auto ptr = search(p))
                if (fn(p, *ptr))
                    return true;
        }

        return false;
    }

    ~split_map() {

        // delete allocated values
        art_iter(&ipv4, delete_cb, nullptr);
        art_iter(&ipv6, delete_cb, nullptr);

        // destroy trees
        art_tree_destroy(&ipv4);
        art_tree_destroy(&ipv6);
    }
};
}
#endif
