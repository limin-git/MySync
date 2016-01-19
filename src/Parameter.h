#pragma once


struct Parameter
{
    Parameter( const std::string& src, const std::string& dst )
        : m_src( src ),
          m_dst( dst )
    {
    }

    std::string m_src;
    std::string m_dst;
};

typedef boost::shared_ptr<Parameter> ParameterPtr;
