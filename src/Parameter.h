#pragma once


struct Parameter
{
    Parameter( const std::string& src, const std::vector<std::string> & dsts )
        : m_src( src ),
          m_dsts( dsts )
    {
    }

    std::string m_src;
    std::vector<std::string> m_dsts;
    std::vector<std::string> m_include_files;
    std::vector<std::string> m_exclude_files;
    std::vector<std::string> m_include_folders;
    std::vector<std::string> m_exclude_folders;
};

typedef boost::shared_ptr<Parameter> ParameterPtr;
