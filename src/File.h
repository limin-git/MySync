#pragma once
#include "CommonDefines.h"


class File
{
public:

    File()
        : m_size( 0 ),
          m_time( 0 )
    {
    }

    void initialize( const Path& p );

public:

    static File* create_file( const Path& p );

public:

    Path m_name;
    long m_size;
    std::time_t m_time;
};

typedef boost::shared_ptr<File> FilePtr;
