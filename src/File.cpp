#include "StdAfx.h"
#include "File.h"


void File::initialize( const Path& p )
{
    struct stat st;
    ::stat( p.string().c_str(), &st );
    m_size = st.st_size;
    m_time = st.st_mtime;
    m_name = p.filename();
    //std::cout << m_name << "(" << m_size << ", " << m_time << ")" << std::endl;
}


void File::assign( const Path& p, const Key& key  )
{
    m_name = p.filename();
    m_size = key.first;
    m_time = key.second;
}


File* File::create_file( const Path& p )
{
    File* file = new File;
    file->initialize( p );
    return file;
}
