#ifndef TEST_H_
#define TEST_H_

class test {
public:
    test();
    virtual ~test();

    static void protocol_encode_frame();
    static void zlib();
    static void https();
};

#endif /* TEST_H_ */
