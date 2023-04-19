#pragma once
//#include"Datastream.h"
class DataStream;
class Serializable {

public:
	virtual void serialize(DataStream& stream)const = 0;
	virtual bool unserialize(DataStream& stream) = 0;
};

#define SERIALIZE(...)                            \
    void serialize(DataStream& stream)const       \
    {                                             \
        char type = DataStream::CUSTOM;           \
        stream.write(&type, sizeof(char));        \
        stream.write_args(__VA_ARGS__);           \
    }                                             \
                                                  \
    bool unserialize(DataStream& stream)          \
    {                                             \
        char type;                               \
        stream.read((char*)&type, sizeof(char));  \
        if (type != DataStream::CUSTOM)           \
        {                                         \
            return false;                        \
        }                                         \
        stream.read_args(__VA_ARGS__);             \
        return true;                              \
    }
